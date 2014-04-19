###############################################################
#
# Purpose: Makefile for "M-JPEG Streamer"
# Author.: Tom Stoeveken (TST)
# Version: 0.3
# License: GPL
#
###############################################################

CC = g++

OTHER_HEADERS = ../../mjpg_streamer.h ../../utils.h ../output.h ../input.h

CFLAGS += -O2
CFLAGS += -DLINUX -D_GNU_SOURCE -Wall -shared -fPIC
CFLAGS += `pkg-config --cflags opencv`
# CFLAGS += -DDEBUG -g
LFLAGS += `pkg-config --libs opencv`
LFLAGS += -lpthread -ldl -ljpeg

all: output_opencv.so

clean:
	rm -f *.a *.o core *~ *.so *.lo

jpeg_routines.o: jpeg_routines.cpp
	$(CC) $(CFLAGS) -c jpeg_routines.cpp -o jpeg_routines.o

vision.o: vision.cpp
	$(CC) $(CFLAGS) -c vision.cpp -o vision.o


output_opencv.so: $(OTHER_HEADERS) output_opencv.cpp jpeg_routines.o vision.o
	$(CC) $(CFLAGS) -o $@ output_opencv.cpp jpeg_routines.o vision.o  $(LFLAGS)
