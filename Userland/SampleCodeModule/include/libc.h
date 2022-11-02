
#ifndef TPE_LIBC_H
#define TPE_LIBC_H

#include <stdint.h>
#include <stddef.h>
#define DEFAULTFD 3
#define MAXLOCK 5
typedef enum {O_RDONLY = 0, O_WRONLY, O_RDWR} Pipe_modes;
typedef enum {BLACK=0x00, BLUE, GREEN, CYAN, RED, MAGENTA, BROWN, LIGHT_GRAY, DARK_GREY, LIGHT_BLUE, LIGHT_GREEN, LIGHT_CYAN, LIGHT_RED, PINK, YELLOW, WHITE} formatType;

typedef enum {SEC = 0, MIN = 2, HOUR = 4, DAY_WEEK = 6, DAY_MONTH = 7, MONTH = 8, YEAR = 9} timeType;

typedef enum {EXECUTE = 0, READY, BLOCKED, FINISHED} process_status;

typedef struct{
    char * name;
    uint64_t pid_write_lock[MAXLOCK];
    uint64_t pid_read_lock[MAXLOCK];
    uint8_t index_write;
    uint8_t index_read;
}pipe_user_info;

typedef struct{
    void * start;                   // Direccion de la funcion que ejecuta el programa
    uint64_t cant_arg;              // Cantidad de argumentos ingresados al programa
    char ** args;                   // Este es un vector que se tiene que definir aparte, antes de inicializar la estructura
} program_t;

typedef struct{
    char* name;
    void* start;            // Direccion de la funcion que ejecuta el programa
    uint64_t arg_c;      // Cantidad de argumentos ingresados al programa
    char** arg_v;            // Vector de strings con los argumentos del programa
    uint8_t foreground;
    int * fds[3];
} executable_t;

typedef struct {
    char* name;
    char* desc;
    void* start;
}front_program_t;

typedef struct{
    char* name;
    uint64_t pid;
    uint8_t priority;
    uint64_t sp;
    uint64_t bp;
    process_status status;
    uint8_t foreground;
}process_info_t;


// SEMAFOROS -----------------------------------------------------------------------------------------------------------------

// Codigo de errores del semaforo
// OK: La operacion se realizo exitosamente
// ERR_MM: Error en la alocacion de memoria
// ERR_NAME: El nombre ya existe
// ERR_NOT_FOUND: El semaforo buscado no existe
// ERR_NOT_INIT: No se inicializo el manejador de semaforos
// ERR_SEM_NOT_INIT: No se inicializo el semaforo
// ERR_SCHEDULER: No se pudo bloquear el proceso
typedef enum {OK = 0, ERR_MM = -1, ERR_NAME = -2, ERR_NOT_FOUND = -3, ERR_NOT_INIT = -4, ERR_SEM_NOT_INIT = -5, ERR_SCHEDULER = -6} error_code;

// Acciones a realizar para la funcion sem_open si no se encuentra el semaforo
// O_CREATE: Crear un nuevo semaforo
// O_NULL: Devolver NULL
// Si se indica cualquier otro valor, se asume O_NULL
typedef enum {O_CREATE = 0, O_NULL} open_modes;

// Manejo de semaforos
typedef void* sem_t;

// Para pasar la informacion al usuario
typedef struct {
    uint64_t value;                     // Valor del semaforo
    uint64_t * blocked_processes;       // Lista de procesos detenidos por el semaforo
    uint64_t * connected_processes;     // Procesos que utilizan el semaforo
    uint32_t blocked_size;              // Cantidad de procesos bloqueados
    uint32_t connected_size;            // Cantidad de procesos conectados
    uint64_t id;                        // id del semaforo
    char * name;                        // Nombre del semaforo
} sem_dump_t;
// SEMAFOROS -----------------------------------------------------------------------------------------------------------------

//#define NULL ((void*)0)
#define CANT_PROG (8)
#define STDOUT WHITE
#define STDERR RED

int32_t sys_write(const char * string, formatType format);
int32_t sys_read(char * c);
int32_t sys_exec(executable_t* executable);
uint8_t sys_exit(void);
uint8_t sys_time(timeType time_unit);
int32_t sys_mem(uint64_t init_dir, uint8_t * arr);
uint64_t sys_tick(void);
int32_t sys_blink(void);
int32_t sys_clear(void);
uint8_t sys_regs(uint64_t * regs_arr);
int32_t sys_block(uint64_t pid);
int32_t sys_unblock(uint64_t pid);
int32_t sys_waitpid(uint64_t pid);
int32_t sys_yield(void);
int32_t sys_terminate(uint64_t pid);
uint64_t sys_getpid();
int32_t sys_nice(uint64_t pid, uint8_t priority);
int32_t sys_get_scheduler_info(process_info_t* processInfo, uint32_t max_count);
uint64_t sys_get_process_count();
void* sys_mm_alloc(uint32_t wanted_size);
void sys_mm_free(void* p);
int sys_dup2(int oldfd, int newfd);
int sys_dup(int oldfd);

void * get_program(const char * str);
uint8_t get_char(void);
uint8_t print_string(const char * s1, formatType format);
uint8_t print_number(uint64_t number, formatType format);
//uint64_t strcmp(const char * s1, const char * s2);
char * to_hex(char * str, uint64_t val);
uint64_t uintToBase(uint64_t value, char * buffer, uint64_t base);
void copy_str(char * dest, char * source);
int32_t block_process(uint64_t pid);
uint32_t unblock_process(uint64_t pid);
int32_t waitpid(uint64_t pid);
int32_t yield();
int32_t terminate_process(uint64_t pid);
uint64_t getpid();
uint64_t nice(uint64_t pid, uint8_t priority);
//void exit();
uint8_t get_year(void);
uint8_t get_month(void);
uint8_t get_day_week(void);
uint8_t get_day(void);
uint8_t get_hour(void);
uint8_t get_min(void);
uint8_t get_secs(void);

uint64_t* get_registers(void);
uint64_t str_tok(char * buffer, char sep);
void throw_error(char * str);
void pause_ticks(uint64_t ticks);
uint8_t blink(void);
uint8_t clear(void);

int sys_pipe(int fd[2]);
int sys_open_fifo(Pipe_modes mode, char * name);
int sys_link_pipe_named(Pipe_modes mode, char * name);
int sys_close_fd(int fd);
int sys_write_pipe(int fd, const char * buf, int count);
int sys_read_pipe(int fd, char * buf, int count);
void sys_get_info(pipe_user_info * user_data, int * count);

sem_t sys_sem_init(char * name, uint64_t value);
sem_t sys_sem_open(char * name, uint64_t value, open_modes mode);
int8_t sys_sem_wait(sem_t  sem);
int8_t sys_sem_post(sem_t  sem);
int8_t sys_sem_close(sem_t  sem);
uint32_t sys_sems_dump(sem_dump_t * buffer, uint32_t length);
void sys_sems_dump_free(sem_dump_t * buffer, uint32_t length);

int pipe(int fd[2]);
int open_fifo(Pipe_modes mode, char * name);
int link_pipe_named(Pipe_modes mode, char * name);
int close_fd(int fd);
void get_info(pipe_user_info * user_data, int * count);
int write_pipe(int fd, const char * buf, int count);
int read_pipe(int fd, char * buf, int count);

sem_t sem_init(char * name, uint64_t value);
sem_t sem_open(char * name, uint64_t value, open_modes mode);
int8_t sem_wait(sem_t sem);
int8_t sem_post(sem_t  sem);
int8_t sem_close(sem_t  sem);
uint32_t sems_dump(sem_dump_t * buffer, uint32_t length);
void sems_dump_free(sem_dump_t * buffer, uint32_t length);
int dup_handler(int oldfd);
int dup2(int oldfd, int newfd);

#endif //TPE_LIBC_H
