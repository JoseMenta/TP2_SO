
#ifndef TPE_LIBC_H
#define TPE_LIBC_H

#include <stdint.h>
#include <stddef.h>

#define DEFAULTFD 3
#define MAXLOCK 5
#define CANT_PROG (23)

typedef enum {O_RDONLY = 0, O_WRONLY, O_RDWR} Pipe_modes;
typedef enum {BLACK=0x00, BLUE, GREEN, CYAN, RED, MAGENTA, BROWN, LIGHT_GRAY, DARK_GREY, LIGHT_BLUE, LIGHT_GREEN, LIGHT_CYAN, LIGHT_RED, PINK, YELLOW, WHITE} formatType;
typedef enum {SEC = 0, MIN = 2, HOUR = 4, DAY_WEEK = 6, DAY_MONTH = 7, MONTH = 8, YEAR = 9} timeType;
typedef enum {EXECUTE = 0, READY, BLOCKED, FINISHED} process_status;
enum {STDIN=0, STDOUT, STDERR};

typedef struct{
    char * name;
    uint64_t pid_write_lock[MAXLOCK];
    uint64_t pid_read_lock[MAXLOCK];
    uint64_t index_write;
    uint64_t index_read;
}pipe_user_info;

typedef struct{
    void * start;                   // Direccion de la funcion que ejecuta el programa
    uint64_t cant_arg;              // Cantidad de argumentos ingresados al programa
    char ** args;                   // Este es un vector que se tiene que definir aparte, antes de inicializar la estructura
} program_t;

typedef struct{
    char* name;
    void* start;            // Direccion de la funcion que ejecuta el programa
    uint64_t arg_c;         // Cantidad de argumentos ingresados al programa
    char** arg_v;           // Vector de strings con los argumentos del programa
    uint8_t foreground;
    int * fds;
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

typedef struct{
    uint64_t allocated_bytes;
    uint64_t free_bytes;
    uint64_t allocated_blocks;
    uint64_t total_bytes;
    char * algorithm;
}mm_info_t;
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
    uint64_t blocked_size;              // Cantidad de procesos bloqueados
    uint64_t connected_size;            // Cantidad de procesos conectados
    uint64_t id;                        // id del semaforo
    char * name;                        // Nombre del semaforo
} sem_dump_t;
// SEMAFOROS -----------------------------------------------------------------------------------------------------------------



uint64_t strcmp(const char *X, const char *Y);
char* strcpy(char* destination, const char* source);
unsigned int strlen(const char *s);

int32_t sys_write(int fd, const char * buf, int count);
int32_t sys_read(int fd, char * buf, int count);
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
void sys_pause_ticks(uint64_t ticks);
void sys_sleep(uint32_t seconds);
void* sys_mm_alloc(uint32_t wanted_size);
void sys_mm_free(void* p);
void sys_mm_info(mm_info_t* info);

int sys_pipe(int fd[2]);
int sys_open_fifo(Pipe_modes mode, char * name);
int sys_link_pipe_named(Pipe_modes mode, char * name);
int sys_close_fd(int fd);
int sys_write_pipe(int fd, const char * buf, int count);
int sys_read_pipe(int fd, char * buf, int count);
int64_t sys_get_info(pipe_user_info * user_data, int64_t count);

sem_t sys_sem_init(char * name, uint64_t value);
sem_t sys_sem_open(char * name, uint64_t value, open_modes mode);
int8_t sys_sem_wait(sem_t  sem);
int8_t sys_sem_post(sem_t  sem);
int8_t sys_sem_close(sem_t  sem);
uint32_t sys_sem_info(sem_dump_t * buffer, uint32_t length);
void sys_free_sem_info(sem_dump_t * buffer, uint32_t length);
uint32_t sys_sem_count();


void free(void* p);
void* malloc(uint32_t wanted_size);
int sys_dup2(int oldfd, int newfd);
int sys_dup(int oldfd);
void * get_program(const char * str);
char* get_program_name(void* program);
char get_char(void);
uint8_t print_string(const char * s1);
uint8_t print_string_with_padding(const char * s1, uint8_t len);
void number_to_string(int64_t num, char * str);
uint64_t string_to_number(const char * str, uint64_t * resp);
uint8_t print_number(uint64_t number);
char * to_hex(char * str, uint64_t val);
void print_numbers(const uint64_t* nums, uint32_t len);
uint64_t uintToBase(uint64_t value, char * buffer, uint64_t base);
void copy_str(char * dest, char * source);
int32_t block_process(uint64_t pid);
uint32_t unblock_process(uint64_t pid);
int32_t waitpid(uint64_t pid);
int32_t yield();
int32_t terminate_process(uint64_t pid);
uint64_t getpid();
uint64_t nice(uint64_t pid, uint8_t priority);
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
void sleep(uint32_t seconds);
uint32_t get_line(char* buf, uint32_t max_len);
uint8_t blink(void);
uint8_t clear(void);
int pipe(int fd[2]);
int open_fifo(Pipe_modes mode, char * name);
int link_pipe_named(Pipe_modes mode, char * name);
int close_fd(int fd);
int64_t get_info(pipe_user_info * user_data, int64_t count);
int write(int fd, const char * buf, int count);
int read(int fd, char * buf, int count);
uint8_t print_string_fd(const char * s1, int fd);
sem_t sem_init(char * name, uint64_t value);
sem_t sem_open(char * name, uint64_t value, open_modes mode);
int8_t sem_wait(sem_t sem);
int8_t sem_post(sem_t  sem);
int8_t sem_close(sem_t  sem);
uint32_t sem_info(sem_dump_t * buffer, uint32_t length);
void free_sem_info(sem_dump_t * buffer, uint32_t length);
int dup_handler(int oldfd);
int dup2(int oldfd, int newfd);
void p_error(char * str);


#endif //TPE_LIBC_H
