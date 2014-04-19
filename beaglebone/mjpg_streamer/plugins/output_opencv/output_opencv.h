#ifndef OUTPUT_OPENCV_H
#define OUTPUT_OPENCV_H

#define OUT_FILE_CMD_TAKE           1
#define OUT_FILE_CMD_FILENAME       2

void help(void);
void worker_cleanup(void *arg);
void *worker_thread(void *arg);
int output_init(output_parameter *param, int id);
int output_stop(int id);
int output_run(int id);
int output_cmd(int plugin_id, unsigned int control_id, unsigned int group, int value, char *valueStr);
#endif
