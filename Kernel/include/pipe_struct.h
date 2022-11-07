
#ifndef TP2_SO_PIPE_STRUCT_H
#define TP2_SO_PIPE_STRUCT_H
#include <stdint.h>
#include <semaphores.h>

#define PIPESIZE 20
#define MAXLOCK 5

typedef enum {O_RDONLY = 0, O_WRONLY, O_RDWR, CONSOLE, CONSOLE_ERR} Pipe_modes;

typedef struct{
    sem_t * lock;
    char * name;
    char buff[PIPESIZE];
    uint64_t pid_write_lock[MAXLOCK];
    uint64_t pid_read_lock[MAXLOCK];
    uint64_t count_access;
    uint64_t index_write;
    uint64_t index_read;
}pipe_info;

typedef struct{
    pipe_info * info;
    Pipe_modes mode;
    uint64_t count_access;
}pipe_restrict;

typedef struct{
    char * name;
    uint64_t pid_write_lock[MAXLOCK];
    uint64_t pid_read_lock[MAXLOCK];
    uint64_t index_write;
    uint64_t index_read;
}pipe_user_info;

#endif //TP2_SO_PIPE_STRUCT_H
