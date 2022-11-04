
#ifndef TP2_SO_PIPES_H
#define TP2_SO_PIPES_H

#include <pipe_struct.h>


int pipe(int fd[2]);
int open_fifo(Pipe_modes mode, char * name);
int link_pipe_named(Pipe_modes mode, char * name);
int close_fd(int fd, uint64_t pid);
int write(int fd, const char * buf, int count);
int read(int fd, char * buf, int count);
void get_info(pipe_user_info * user_data, int * count);
int dup2(int oldfd, int newfd);
int dup(int oldfd);
int pipe_initialize();
int pipe_terminated();
pipe_info * get_pipe_console();
pipe_restrict * get_pipe_console_restrict();
int write_keyboard(const char * buf, int count);
pipe_restrict * get_error_pipe_console();
#endif //TP2_SO_PIPES_H
