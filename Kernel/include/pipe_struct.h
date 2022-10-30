
#ifndef TP2_SO_PIPE_STRUCT_H
#define TP2_SO_PIPE_STRUCT_H

typedef enum {O_RDONLY = 0, O_WRONLY, O_RDWR} Pipe_modes;
#define PIPESIZE 30
#define MAXLOCK 5

typedef struct{
    //sem lock;
    int lock;
    char * name;
    char buff[PIPESIZE];
    uint64_t pid_write_lock[MAXLOCK];
    uint64_t pid_read_lock[MAXLOCK];
    uint8_t count_access;
    uint8_t index_write;
    uint8_t index_read;
}pipe_info;

typedef struct{
    pipe_info * info;
    Pipe_modes mode;
    uint8_t count_access;
}pipe_restrict;

typedef struct{
    char * name;
    uint64_t pid_write_lock[MAXLOCK];
    uint64_t pid_read_lock[MAXLOCK];
    uint8_t index_write;
    uint8_t index_read;
}pipe_user_info;


#endif //TP2_SO_PIPE_STRUCT_H
