
#ifndef TP2_SO_PIPES_H
#define TP2_SO_PIPES_H

#include "mm.h"
#include "orderListADT.h"
#include "pipe_struct.h"
#include "scheduler.h"


int pipe(int fd[2]);
int open_fifo(Pipe_modes mode, char * name);
int link_pipe_named(Pipe_modes mode, char * name);
int close_fd(int fd);
int write(int fd, const char * buf, int count);
int read(int fd, char * buf, int count);
void get_info(pipe_user_info ** user_data, int * count);

#endif //TP2_SO_PIPES_H
