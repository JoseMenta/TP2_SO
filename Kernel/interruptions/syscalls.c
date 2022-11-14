// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <syscalls.h>
#include <video_driver.h>
#include <keyboard.h>
#include <scheduler.h>
#include <time.h>
#include <stddef.h>
#include <stdint.h>
#include <interrupts.h>
#include <mm.h>
#include <pipes.h>
#include <semaphores.h>
#include <pipes.h>

#include "../include/scheduler.h"


//----------------------------------------------------------------------
// read_handler: handler para leer del pipe de consola
//----------------------------------------------------------------------
// Argumentos:
//  str: char* donde se debe guardar el caracter leido
//----------------------------------------------------------------------
// Retorno:
//  1 o 0 segun leyo o no un caracter
//----------------------------------------------------------------------
int32_t read_handler(int fd, char * buf, int count) {
    return read(fd, buf, count);
}


//----------------------------------------------------------------------
// write_handler: imprime un string en la pantalla del proceso que lo llama
//----------------------------------------------------------------------
// Argumentos:
//  str: el string que se desea imprimor
//  format: el color de la letra que se desea usar
//----------------------------------------------------------------------
int32_t write_handler(int fd, const char * str, int count){
    return write(fd, str, count);        // Imprime por pantalla
}


//----------------------------------------------------------------------
// time_handler: obtiene la unidad del tiempo que se pide
//----------------------------------------------------------------------
// Argumentos:
//  time_unit: la unidad que se desea buscar (sec, min,....)
//----------------------------------------------------------------------
// Retorno:
//  El valor pedido o -1 si no es un parametro correcto
//----------------------------------------------------------------------
uint8_t time_handler(timeType time_unit){
    if(time_unit != SEC && time_unit != MIN && time_unit != HOUR && time_unit != DAY_WEEK &&
        time_unit != DAY_MONTH && time_unit != MONTH && time_unit != YEAR){
        return -1;
    }
    return get_time(time_unit);
}


//----------------------------------------------------------------------
// mem_handler: guarda 32 bytes a partir de una direccion que se pasa en un vector
//----------------------------------------------------------------------
// Argumentos:
//  init_dir: la direccion inicial desde donde se quiere leer
//  arr: arreglo donde se escriben los valores, debe tener al menos 32 posiciones
//----------------------------------------------------------------------
// Es la funcion que utliza el programa printmem. Completa con 0's si en algun momento
// se pasa del maximo
//----------------------------------------------------------------------
// Retorno:
//  La cantidad de posiciones que se logro almacenar
//----------------------------------------------------------------------
int32_t mem_handler(uint64_t init_dir, uint8_t * arr){
    return 0;
}


//----------------------------------------------------------------------
// tick_handler: obtiene la cantidad de ticks que hizo el timer tick
//----------------------------------------------------------------------
// Argumentos:
//  void
//----------------------------------------------------------------------
// Retorno
//  La cantidad de ticks
//----------------------------------------------------------------------
uint64_t tick_handler(void){
    return ticks_elapsed();
}


//----------------------------------------------------------------------
// blink_handler: hace que el fondo del cursor de la pantalla cambie segun lo especificado en video_blink
//----------------------------------------------------------------------
// Argumentos:
//  void
//----------------------------------------------------------------------
int32_t blink_handler(void){
    video_blink(ALL);
    return 0;
}


//----------------------------------------------------------------------
// regs_handler: devuelve los valores de los arreglos cuando se utilizo la combinacion Control+s
//----------------------------------------------------------------------
// Argumentos:
//  regs_arr: un arreglo de al menos 18 uint64_t donde se almacenan los valores
//----------------------------------------------------------------------
uint8_t regs_handler(uint64_t * regs_arr){
    extern uint8_t regs_saved;
    uint64_t * inforeg_context = get_inforeg_context();
    for(uint8_t i = 0; i < REGISTERS_COUNT; i++){
        regs_arr[i] = inforeg_context[i];
    }
    return regs_saved;
}


//----------------------------------------------------------------------
// clear_handler: Limpiar la terminal de comandos
//----------------------------------------------------------------------
// Argumentos:
//  void
//----------------------------------------------------------------------
int32_t clear_handler(void){
    clear(ALL);
    return 0;
}


int32_t terminate_handler(uint64_t pid){
    int status = 0;
    if((status = terminate_process(pid))==-1){
        print("No fue posible terminar al proceso",WHITE,ALL);
        return -1;
    }
    //el stack todavia no se libero, pues scheduler utiliza mm_alloc
    //Si lo libero en terminate_process, puedo estar pisando el contexto actual con mm_alloc
    _int20();//por si me termino a mi mismo
    return status;
}


int32_t block_process_handler(uint64_t pid){
    int status = 0;
    if((status = block_process(pid))==-1){
        return -1;
    }
    _int20();//llama al scheduler para ver como sigue, es para el caso donde se bloquea a si mismo
    return status;
}


//hace _int20() adentro de wait, para que devuelva el codigo cuando ya esta disponible el que fue esperado
int32_t waitpid_handler(uint64_t pid){
    int status = 0;
    if((status = waitPid(pid))==-1){
        return -1;
    }
    return status;
}
int32_t yield_handler(){
    int status = 0;
    if((status = yield_current_process())==-1){
        return -1;
    }
    _int20();
    return status;
}
int32_t unblock_process_handler(uint64_t pid){
    int status = 0;
    if((status = unblock_process(pid))==-1){
        return -1;
    }
    return status;
}
int32_t exec_handler(executable_t* executable){
    if(executable==NULL){
        return -1;
    }
    return create_process(executable);
}
int32_t nice_handler(uint64_t pid, uint8_t priority){
    return change_process_priority(pid,priority);
}
uint64_t getpid_handler(){
    return get_current_pid();
}
int32_t exit_handler(){
    return terminate_handler(get_current_pid());
}
int32_t scheduler_info_handler(process_info_t* processInfo,  uint32_t max_count){
    return get_scheduler_info(processInfo,max_count);
}
uint64_t process_count_handler(){
    return get_process_count();
}
void* mm_alloc_handler(uint32_t wanted_size){
    void* ans =  mm_alloc(wanted_size);
    if(ans!=NULL){
        add_mm_address(ans);
    }
    return ans;
}
void mm_free_handler(void* p){
    mm_free(p);
    mm_free_address(p);
}
int pipe_handler(int fd[2]){
    return pipe(fd);
}
int open_fifo_handler(Pipe_modes mode, char * name){
    return open_fifo(mode, name);
}
int link_pipe_named_handler(Pipe_modes mode, char * name){
    return link_pipe_named(mode, name);
}
int close_fd_handler(int fd){
    return close_fd(fd,get_current_pid());
}
int write_handler_pipe(int fd, const char * buf, int count){
    return write(fd, buf, count);
}
int read_handler_pipe(int fd, char * buf, int count){
    return read(fd, buf, count);
}
int64_t get_info_handler(pipe_user_info * user_data, int64_t count){
    return get_info(user_data, count);
}
sem_t * sem_init_handler(char * name, uint64_t value){
    return sem_init(name, value);
}
sem_t * sem_open_handler(char * name, uint64_t value, open_modes mode){
    return sem_open(name, value, mode);
}
int8_t sem_wait_handler(sem_t * sem){
    return sem_wait(sem);
}
int8_t sem_post_handler(sem_t * sem){
    return sem_post(sem);
}
int8_t sem_close_handler(sem_t * sem){
    return sem_close(sem);
}
uint32_t sem_count_handler(){
    return sem_count();
}
uint32_t sem_info_handler(sem_dump_t * buffer, uint32_t length){
    return sems_dump(buffer,length);
}
void free_sem_info_handler(sem_dump_t * buffer, uint32_t length){
    sems_dump_free(buffer,length);
}
int dup2_handler(int oldfd, int newfd){
    return dup2(oldfd, newfd);
}
int dup_handler(int oldfd){
    return dup(oldfd);
}
void pause_ticks_handler(uint64_t ticks){
    add_timer(ticks);
    _int20();//veo cual sigue
}
void mm_info_handler(mm_info_t* info){
    info->total_bytes = get_total_bytes();
    info->allocated_bytes = get_allocated_bytes();
    info->free_bytes = get_free_bytes();
    info->allocated_blocks = get_allocated_blocks();
    info->algorithm = MM_NAME;
}
void sleep_handler(uint32_t seconds){
    pause_ticks_handler(seconds * 18);
}



void* syscalls[]={&read_handler,&write_handler,&exec_handler,&exit_handler,&time_handler,&mem_handler,&tick_handler,&blink_handler,&regs_handler,&clear_handler,
                  &block_process_handler, &waitpid_handler,&yield_handler, &unblock_process_handler,&terminate_handler, &nice_handler, &getpid_handler, &scheduler_info_handler, &process_count_handler, &mm_alloc_handler,&mm_free_handler,
                    &pipe_handler, &open_fifo_handler, &link_pipe_named_handler, &close_fd_handler, &write_handler_pipe, &read_handler_pipe, &get_info_handler,
                    &sem_init_handler, &sem_open_handler, &sem_wait_handler, &sem_post_handler, &sem_close_handler, &sem_info_handler, &free_sem_info_handler,
                    &dup2_handler, &dup_handler,&pause_ticks_handler, &sleep_handler, &mm_info_handler, &sem_count_handler};

void* syscall_dispatcher(uint64_t syscall_num){
    if(syscall_num>=41){
        return NULL;
    }
    return syscalls[syscall_num];
}