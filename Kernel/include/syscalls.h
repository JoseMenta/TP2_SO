#ifndef TPE_SYSCALLS_H
#define TPE_SYSCALLS_H

#include <stdint.h>
#include <video_driver.h>

typedef enum {SEC = 0, MIN = 2, HOUR = 4, DAY_WEEK = 6, DAY_MONTH = 7, MONTH = 8, YEAR = 9} timeType;

#define MAX_ARRAY_SIZE 32
#define MAX_MEM_ADDRESS 0xFFFFFFFF8                             // A partir de la proxima direccion, falla (suponemos que es un seg fault)

#ifdef BUDDY
#define MM_NAME "buddy"
#endif

#ifdef HEAP
#define MM_NAME "heap"
#endif

typedef struct{
    uint64_t allocated_bytes;
    uint64_t free_bytes;
    uint64_t allocated_blocks;
    uint64_t total_bytes;
    char * algorithm;
}mm_info_t;

int32_t read_handler(int fd, char * buf, int count);            // Lee por pantalla el siguiente caracter y lo copia en str
int32_t write_handler(int fd, const char * str, int count);             // Escribe el string str por pantalla con el formato format
int32_t exec_handler(executable_t* executable);
int32_t exit_handler();                                         // Finaliza un proceso con codigo de error
uint8_t time_handler(timeType time_unit);                        // Devuelve el valor para la unidad indicada en GMT
int32_t mem_handler(uint64_t init_dir, uint8_t * arr);          // Complete un arreglo de 32 elementos con la informacion que guarda init_dir y las 31 direcciones siguientes
uint64_t tick_handler(void);                                    // Devuelve la cantidad de ticks transcurridos
int32_t blink_handler(void);                                    // Realiza un parpadeo en la pantalla en el proximo caracter a escribir
uint8_t regs_handler(uint64_t * regs_arr);
int32_t clear_handler(void);
uint8_t get_time(timeType time_unit);                           // Llama al RTC con la unidad indicada
uint8_t get_data(uint64_t address_dir);                         // Devuelve el dato de 8 bits almacenado en la direccion de memoria indicada por parametros
uint64_t* get_inforeg_context();                                // Devuelve el arreglo con el estado de los registros cuando se realizo CTRL+S por ultima vez

// Syscalls de scheduler
int32_t block_process_handler(uint64_t pid);
int32_t waitpid_handler(uint64_t pid);
int32_t yield_handler(void);
int32_t unblock_process_handler(uint64_t pid);
int32_t exec_handler(executable_t* executable);
int32_t nice_handler(uint64_t pid, uint8_t priority);
uint64_t getpid_handler();
int32_t scheduler_info_handler(process_info_t* processInfo,  uint32_t max_count);
uint64_t process_count_handler();

// Syscalls de mm
void* mm_alloc_handler(uint32_t wanted_size);
void mm_free_handler(void* p);
void pause_ticks_handler(uint64_t ticks);
void mm_info_handler(mm_info_t* info);

// Syscalls de pipes
int pipe_handler(int fd[2]);
int open_fifo_handler(Pipe_modes mode, char * name);
int link_pipe_named_handler(Pipe_modes mode, char * name);
int close_fd_handler(int fd);
int write_handler_pipe(int fd, const char * buf, int count);
int read_handler_pipe(int fd, char * buf, int count);
int64_t get_info_handler(pipe_user_info * user_data, int64_t count);



// Syscalls de semaforos
sem_t * sem_init_handler(char * name, uint64_t value);
sem_t * sem_open_handler(char * name, uint64_t value, open_modes mode);
int8_t sem_wait_handler(sem_t * sem);
int8_t sem_post_handler(sem_t * sem);
int8_t sem_close_handler(sem_t * sem);
uint32_t sem_info_handler(sem_dump_t * buffer, uint32_t length);
void free_sem_info_handler(sem_dump_t * buffer, uint32_t length);
uint32_t sem_count_handler();
int dup_handler(int oldfd);
int dup2_handler(int oldfd, int newfd);


void* syscall_dispatcher(uint64_t syscall_num);
#endif
