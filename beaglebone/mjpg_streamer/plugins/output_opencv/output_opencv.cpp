/*******************************************************************************
#                                                                              #
#      MJPG-streamer allows to stream JPG frames from an input-plugin          #
#      to several output plugins                                               #
#                                                                              #
#      Copyright (C) 2007 Tom Stöveken                                         #
#                                                                              #
# This program is free software; you can redistribute it and/or modify         #
# it under the terms of the GNU General Public License as published by         #
# the Free Software Foundation; version 2 of the License.                      #
#                                                                              #
# This program is distributed in the hope that it will be useful,              #
# but WITHOUT ANY WARRANTY; without even the implied warranty of               #
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                #
# GNU General Public License for more details.                                 #
#                                                                              #
# You should have received a copy of the GNU General Public License            #
# along with this program; if not, write to the Free Software                  #
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA    #
#                                                                              #
*******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <signal.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <getopt.h>
#include <pthread.h>
#include <fcntl.h>
#include <time.h>
#include <syslog.h>
#include <dirent.h>
#include <cv.h>
#include <highgui.h>

#include <linux/types.h>          /* for videodev2.h */
#include <linux/videodev2.h>

#include "jpeg_routines.h"
#include "vision.h"

extern "C" {
#include "../../utils.h"
#include "../../mjpg_streamer.h"
#include "output_opencv.h"
}

#define OUTPUT_PLUGIN_NAME "OPENCV output plugin"

static pthread_t worker;
static globals *pglobal;
static int fd, delay, max_decompressed_size;
static unsigned char *decompressed = NULL;
static int input_number = 0;

/******************************************************************************
Description.: print a help message
Input Value.: -
Return Value: -
******************************************************************************/
void help(void) {
    fprintf(stderr, " ---------------------------------------------------------------\n" \
            " There is no help for ...: "OUTPUT_PLUGIN_NAME"\n");
}

/******************************************************************************
Description.: clean up allocated ressources
Input Value.: unused argument
Return Value: -
******************************************************************************/
void worker_cleanup(void *arg) {
    static unsigned char first_run = 1;

    if(!first_run) {
        DBG("already cleaned up ressources\n");
        return;
    }

    first_run = 0;
    OPRINT("cleaning up ressources allocated by worker thread\n");

    if (decompressed != NULL) {
        free(decompressed);
    }
    close(fd);
}


/******************************************************************************
Description.: this is the main worker thread
              it loops forever, grabs a fresh frame and stores it to file
Input Value.:
Return Value:
******************************************************************************/
void *worker_thread(void *arg)
{
    int ok = 1, frame_size = 0, decompressed_size = 0;
    unsigned char *tmp_decompressed = NULL;

    int width = 320;
    int height = 240;
    clock_t start, mid, end;

    Vision cv_runner;
    /* set cleanup handler to cleanup allocated ressources */
    pthread_cleanup_push(worker_cleanup, NULL);

    while(ok >= 0 && !pglobal->stop) {
        DBG("waiting for fresh frame\n");

        pthread_mutex_lock(&pglobal->in[input_number].db);
        pthread_cond_wait(&pglobal->in[input_number].db_update, &pglobal->in[input_number].db);
        start = clock();
        DBG("getting sizes\n");

        /* Get sizes */
        int fmt_idx = pglobal->in[input_number].currentFormat;
        if(fmt_idx > (pglobal->in[input_number].formatCount - 1)) {
            DBG("Format index out of bounds");
            return NULL;
        }
        int res_idx = pglobal->in[input_number].in_formats[fmt_idx].currentResolution;
        if(res_idx > (pglobal->in[input_number].in_formats[fmt_idx].resolutionCount - 1)) {
            DBG("Res index out of bounds");
            return NULL;
        }

        // width = pglobal->in[input_number].in_formats[fmt_idx].supportedResolutions[res_idx].width;
        // height = pglobal->in[input_number].in_formats[fmt_idx].supportedResolutions[res_idx].height;
        DBG("Width: %d,Height:%d\n", width, height);
        decompressed_size = width * height * sizeof(char) * 3;

        /* read buffer */
        frame_size = pglobal->in[input_number].size;

        // TODO: resize decompressed buffer
        if (decompressed_size > max_decompressed_size) {
            DBG("increasing decompresser size to %d\n", decompressed_size);

            max_decompressed_size = decompressed_size + (1 << 16);
            if((tmp_decompressed = (unsigned char *) realloc(decompressed, max_decompressed_size)) == NULL) {
                pthread_mutex_unlock(&pglobal->in[input_number].db);
                LOG("not enough memory\n");
                return NULL;
            }

            decompressed = tmp_decompressed;
        }


        /* allow others to access the global buffer again */
        DBG("About to make the mat\n");
        vision::process_jpeg(decompressed,pglobal->in[input_number].buf,frame_size);
        pthread_mutex_unlock(&pglobal->in[input_number].db);
        DBG("Width:%d, Height:%d\n", width, height);
        cv::Mat img(height, width, CV_8UC3, decompressed, width * 3 * sizeof(char));
        mid = clock();
        DBG("Made the mat!\n");
        ok = cv_runner.loop(img);
        end = clock();
        DBG("Wrote the image!\n");
        DBG("Decompression time: %f\n",((float)mid-start)/CLOCKS_PER_SEC);
        DBG("OpenCV time: %f\n",((float)end-mid)/CLOCKS_PER_SEC);

        // Wait 100ms so there's less overwhelming of data
        //usleep(100000);
    }

    /* cleanup now */
    pthread_cleanup_pop(1);

    return NULL;
}

/*** plugin interface functions ***/
/******************************************************************************
Description.: this function is called first, in order to initialize
              this plugin and pass a parameter string
Input Value.: parameters
Return Value: 0 if everything is OK, non-zero otherwise
******************************************************************************/
int output_init(output_parameter *param, int id)
{
	int i;
    delay = 0;
    pglobal = param->global;
    pglobal->out[id].name = (char *) malloc((1+strlen(OUTPUT_PLUGIN_NAME))*sizeof(char));
    sprintf(pglobal->out[id].name, "%s", OUTPUT_PLUGIN_NAME);
    DBG("OUT plugin %d name: %s\n", id, pglobal->out[id].name);

    param->argv[0] = OUTPUT_PLUGIN_NAME;

    /* show all parameters for DBG purposes */
    for(i = 0; i < param->argc; i++) {
        DBG("argv[%d]=%s\n", i, param->argv[i]);
    }

    reset_getopt();
    while(1) {
        int option_index = 0, c = 0;
        static struct option long_options[] = {
            {"h", no_argument, 0, 0
            },
            {"help", no_argument, 0, 0},
            {0, 0, 0, 0}
        };

        c = getopt_long_only(param->argc, param->argv, "", long_options, &option_index);

        /* no more options to parse */
        if(c == -1) break;

        /* unrecognized option */
        if(c == '?') {
            help();
            return 1;
        }

        switch(option_index) {
            /* h, help */
        case 0:
        case 1:
            DBG("case 0,1\n");
            help();
            return 1;
            break;
        }
    }

    if(!(input_number < pglobal->incnt)) {
        OPRINT("ERROR: the %d input_plugin number is too much only %d plugins loaded\n", input_number, param->global->incnt);
        return 1;
    }

    OPRINT("Starting OpenCV.....\n");
    OPRINT("input plugin.....: %d: %s\n", input_number, pglobal->in[input_number].plugin);

    param->global->out[id].parametercount = 0;
    return 0;
}

/******************************************************************************
Description.: calling this function stops the worker thread
Input Value.: -
Return Value: always 0
******************************************************************************/
int output_stop(int id)
{
    DBG("will cancel worker thread\n");
    pthread_cancel(worker);
    return 0;
}

/******************************************************************************
Description.: calling this function creates and starts the worker thread
Input Value.: -
Return Value: always 0
******************************************************************************/
int output_run(int id)
{
    DBG("launching worker thread\n");
    pthread_create(&worker, 0, worker_thread, NULL);
    pthread_detach(worker);
    return 0;
}

int output_cmd(int plugin_id, unsigned int control_id, unsigned int group, int value, char *valueStr)
{
    DBG("command (%d, value: %d) for group %d triggered for plugin instance #%02d\n", control_id, value, group, plugin_id);
    return 0;
}
