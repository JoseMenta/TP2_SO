

#ifndef TPE_SCHEDULER_H
#define TPE_SCHEDULER_H
#include <stdint.h>
typedef enum {ALL = 0, LEFT, RIGHT} positionType;
typedef enum {EXECUTE = 0, READY, BLOCKED, FINISHED} process_status;
typedef struct{
    char* name;
    uint32_t pid;
    uint8_t priority;
    process_status status;
    void* bp;
    void* sp;
}PCB;
typedef struct{
    char* name;
    void* start;            // Direccion de la funcion que ejecuta el programa
    uint64_t arg_c;      // Cantidad de argumentos ingresados al programa
    char** arg_v;            // Vector de strings con los argumentos del programa
} executable_t;
int initialize_scheduler();
int create_process(executable_t* executable);
int block_process(int pid);
int terminate_process(int pid);
int change_process_priority(int pid, uint8_t new_priority);
int unblock_process(int pid);
void* scheduler(void* curr_rsp);
int waitPid(int pid);
//Inicio scheduler arqui
//#include <interrupts.h>
//#include <stdint.h>
//#include <stddef.h>
//// Estado en el que se puede encontrar un programa/proceso
//// RUNNING: El proceso debe correrse
//// WAITING: El proceso esta esperando a que terminen procesos que el ejecuto
//// SUSPENDED: El proceso fue suspendido (con una de las teclas indicadas). En su lugar se ejecuta el proceso default (while 1)
//// FINISHED: El proceso fue terminado, ya se puede eliminar cuando se reinicie el proceso que lo ejecuto
//typedef enum {RUNNING = 0, WAITING, SUSPENDED, TERMINATED} statusType;
//
//typedef enum {ALL = 0, LEFT, RIGHT} positionType;       // Posicion del programa en pantalla
//
typedef enum {R8 = 0, R9, R10, R11, R12, R13, R14, R15, RAX, RBX, RCX, RDX, RSI, RDI, RBP, RSP, RIP, RFLAGS} registers; // El orden en el que llegan los registros en el arreglo
//
//
//
//
////#define PCB_NOT_FOUND NULL
//#define OFFSET (1024)                                     // Espacio que se le va a dejar de stack a cada proceso
////Estructura de un proceso que utiliza el scheduler para manejar el cambio de contexto
//typedef struct {
//    uint64_t registers[REGISTERS_COUNT];                // Estado de los registros cuando se interrumpe el programa
//    statusType status;                                  // Estado del proceso
//    positionType position;                              // Posicion en pantalla del proceso
//} process_t;
//
////Estructura de programa que pasan desde Userland
//typedef struct{
//    void* start;            // Direccion de la funcion que ejecuta el programa
//    uint64_t cant_arg;      // Cantidad de argumentos ingresados al programa
//    char** args;            // Vector de strings con los argumentos del programa
//} program_t;
//
//uint8_t terminate_process(void);                                    // Finaliza un proceso (luego sera borrado)
//positionType get_current_position(void);                            // Devuelve la posicion del programa corriendo en el momento
//int process_array_is_empty();                                       // Devuelve 1 si no hay procesos cargados, 0 si no
//uint8_t suspend_left();                                             // Funciones para la logica de pausar, reanudar o terminar un proceso
//uint8_t suspend_right();
//uint8_t suspend_full();
//uint8_t restart_left();
//uint8_t restart_right();
//uint8_t restart_full();
//uint8_t kill_left();
//uint8_t kill_right();
//uint8_t kill_full();
//
//void change_context();                                          // Cambia de contexto si es posible, si no mantiene el actual
//void add_full_process(program_t process);                       // Agrega un nuevo proceso al arreglo de procesos
//void add_two_processes(program_t left, program_t right);        // Agrega dos nuevos procesos (left y right) al arreglo de procesos
#endif
