

#ifndef TPE_SCHEDULER_H
#define TPE_SCHEDULER_H

#include <stdint.h>
#include <pipe_struct.h>
#include <queueADT.h>

#define MAXFD 11
#define DEFAULTFD 3

#define BASE_PRIORITY_FOREGROUND 1
#define BASE_PRIORITY_BACKGROUND 3
#define CHECK_PID(pid,elem) (pid>=1 && pid<new_pid && hashADT_get(hash,elem)!=NULL)

typedef enum {ALL = 0, LEFT, RIGHT} positionType;
typedef enum {EXECUTE = 0, READY, BLOCKED, FINISHED} process_status;
typedef enum {R8 = 0, R9, R10, R11, R12, R13, R14, R15, RAX, RBX, RCX, RDX, RSI, RDI, RBP, RSP, RIP, RFLAGS} registers; // El orden en el que llegan los registros en el arreglo
enum {STDIN=0, STDOUT, STDERR};

typedef struct{
    char* name;
    uint64_t pid;
    uint8_t priority;
    process_status status;
    pipe_restrict * fd[MAXFD];
    void* bp;
    void* sp;
    queueADT waiting_processes;
    uint64_t waiting_count;
    uint8_t foreground;
    uint64_t arg_c;
    char** arg_v;
}PCB;
typedef struct{
    char* name;
    void* start;            // Direccion de la funcion que ejecuta el programa
    uint64_t arg_c;         // Cantidad de argumentos ingresados al programa
    char** arg_v;           // Vector de strings con los argumentos del programa
    uint8_t foreground;
    int * fds;
} executable_t;

typedef struct{
    char* name;
    uint64_t pid;
    uint8_t priority;
    uint64_t sp;
    uint64_t bp;
    process_status status;
    uint8_t foreground;
}process_info_t;

int initialize_scheduler();
int create_process(executable_t* executable);
int yield_current_process();
int block_process(uint64_t pid);
int terminate_process(uint64_t pid);
int change_process_priority(uint64_t pid, uint8_t new_priority);
int unblock_process(uint64_t pid);
uint64_t get_current_pid();
PCB * get_current_pcb();
void* scheduler(void* curr_rsp);
int waitPid(uint64_t pid);
int32_t get_scheduler_info(process_info_t* processInfo, uint32_t max_count);
uint64_t get_process_count();
PCB* get_pcb_by_pid(uint64_t pid);
void kill_foreground_process();

#endif
