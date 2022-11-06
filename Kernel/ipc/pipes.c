#include "../include/pipes.h"
#include <mm.h>
#include <orderListADT.h>
#include <scheduler.h>
#include <syscalls.h>
#include <video_driver.h>

//TODO: sacar
#include "../include/syscalls.h"
#include "../include/semaphores.h"
#include "../include/mm.h"
#include "../include/video_driver.h"
#include "../include/keyboard.h"
orderListADT pipes_list = NULL;
static pipe_info * console_pipe = NULL;
static pipe_restrict * console_pipe_restrict = NULL;
static pipe_restrict * error_pipe_restrict = NULL;

int64_t pipe_compare(pipe_info * elem1, pipe_info * elem2);
pipe_restrict * create_restrict_pipe(Pipe_modes mode, pipe_info * info);
pipe_info * create_info_pipe(char * name);
int get_next_fd();
int index_lock_pid(const uint64_t array[MAXLOCK]);


//-----------------------------------------------------------------------------------------
//pipe_initialize: Inicializar el manejo de los pipes
//-----------------------------------------------------------------------------------------
//Argumentos:
//-----------------------------------------------------------------------------------------
//Retorno:
//  -1 en caso de error, 0 en caso de exito
//-----------------------------------------------------------------------------------------
int pipe_initialize(){
    pipes_list = new_orderListADT((compare_function)pipe_compare);
    if(pipes_list == NULL){
        return -1;
    }
    console_pipe = create_info_pipe("CONSOLE\0");
    console_pipe_restrict = create_restrict_pipe(CONSOLE, console_pipe);
    error_pipe_restrict = create_restrict_pipe(CONSOLE_ERR, console_pipe);
    return 0;
    /*
    get_current_pcb()->fd[0] = console_pipe_restrict;
    get_current_pcb()->fd[1] = console_pipe_restrict;
    get_current_pcb()->fd[2] = error_pipe_restrict;
    for(int i=3; i<MAXFD; i++){
        get_current_pcb()->fd[i] = NULL;
    }
     */
}

//-----------------------------------------------------------------------------------------
//pipe_end: Finalizar el manejo de los pipes
//-----------------------------------------------------------------------------------------
//Argumentos:
//-----------------------------------------------------------------------------------------
//Retorno:
//-----------------------------------------------------------------------------------------
void pipe_terminated(){
    free_orderListADT(pipes_list);
    mm_free(console_pipe);
}

//-----------------------------------------------------------------------------------------
//Obtener las variables globales, para que sean static
//-----------------------------------------------------------------------------------------
pipe_info * get_pipe_console(){
    return console_pipe;
}
pipe_restrict * get_error_pipe_console(){
    return error_pipe_restrict;
}
pipe_restrict * get_pipe_console_restrict(){
    return console_pipe_restrict;
}

//-----------------------------------------------------------------------------------------
//pipe: Crea de un pipe
//-----------------------------------------------------------------------------------------
//Argumentos:
//  fd[2] = puntero donde almaceno los fd del pipe
//-----------------------------------------------------------------------------------------
//Retorno:
//  -1 en caso de error, 0 en caso de exito
//-----------------------------------------------------------------------------------------
int pipe(int fd[2]){
    pipe_info * info = create_info_pipe(NULL);
    PCB * curren = get_current_pcb();
    if(info == NULL){
        return -1;
    }

    //Modo lectura en el primer fd
    fd[0] = get_next_fd();
    if(fd[0] == -1){
        fd[1] = -1;
        mm_free(info);
        return -1;
    }
    curren->fd[fd[0]] = create_restrict_pipe(O_RDONLY, info);


    //Modo escritura en el segundo fd
    fd[1] = get_next_fd();
    if(fd[1] == -1){
        fd[0]=-1;
        mm_free(curren->fd[fd[0]]);
        mm_free(info);
        return -1;
    }
    curren->fd[fd[1]] = create_restrict_pipe(O_WRONLY, info);
    //Agrego la estructura con la informacion a la lista
    orderListADT_add(pipes_list, info);
    return 0;
}


//-----------------------------------------------------------------------------------------
//open_fifo: Crea un fifo
//-----------------------------------------------------------------------------------------
//Argumentos:
//  mode = modo del fd
//  name = identificador del fifo
//-----------------------------------------------------------------------------------------
//Retorno:
//  fd de donde se referencia el fifo, -1 en caso de error
//-----------------------------------------------------------------------------------------
int open_fifo(Pipe_modes mode, char * name){

    pipe_info * info = create_info_pipe(name);
    if(info == NULL){
        print("fallo info", WHITE, ALL);
        return -1;
    }
    //Un fifo admite cualquiera de los 3 modos
    int fd = get_next_fd();
    if(fd==-1){
        print("fallo fd", WHITE, ALL);
        mm_free(info);
        return -1;
    }
    get_current_pcb()->fd[fd] = create_restrict_pipe(mode, info);
    info->count_access++;
    orderListADT_add(pipes_list, info);
    return fd;
}


//-----------------------------------------------------------------------------------------
//link_pipe_named: Crear un fd para conectarse a un fifo
//-----------------------------------------------------------------------------------------
//Argumentos:
//  mode = modo del fd
//  name = identificador del fifo que busco
//-----------------------------------------------------------------------------------------
//Retorno:
//  fd de donde se referencia el fifo, -1 en caso de error
//-----------------------------------------------------------------------------------------
int link_pipe_named(Pipe_modes mode, char * name){
    int fd = get_next_fd();
    if(fd==-1){
        print("fallo fd del link", WHITE, ALL);
        return -1;
    }
    //Creo una structura igual para la comparacion
    pipe_info aux;
    aux.name=name;
    pipe_info * info = orderListADT_get(pipes_list, &aux);
    if(info == NULL){
        print("fallo  creacion de info", WHITE, ALL);
        return -1;
    }

    get_current_pcb()->fd[fd] = create_restrict_pipe(mode, info);
    info->count_access++;
    return fd;
}


//-----------------------------------------------------------------------------------------
//close_fd: cerrar un fd y liberar recursos si es necesario
//-----------------------------------------------------------------------------------------
//Argumentos:
//  info_pipe = modo del fd
//-----------------------------------------------------------------------------------------
//Retorno:
//  fd de donde se referencia el fifo, -1 en caso de error
//-----------------------------------------------------------------------------------------
int close_fd(int fd, uint64_t pid){
    pipe_restrict * pipe_mode = get_pcb_by_pid(pid)->fd[fd];
    pipe_mode->count_access--;
    pipe_mode->info->count_access--;
    //Libero si no tengo a nadie apuntandolo y si no queda nada por leer
    if(pipe_mode->info->count_access == 0 && pipe_mode->info->index_write == pipe_mode->info->index_read){
        orderListADT_delete(pipes_list, pipe_mode->info);
        sem_close(pipe_mode->info->lock);
        mm_free(pipe_mode->info);
    }
    if(pipe_mode->count_access == 0){
        mm_free(pipe_mode);
    }
    get_pcb_by_pid(pid)->fd[fd] = NULL;
    return 0;
}


//-----------------------------------------------------------------------------------------
//write: Escribir a un fd
//-----------------------------------------------------------------------------------------
//Argumentos:
//  fd: fd donde se va a escribir
//  msj: char* a imprimir
//  count: cantidad de caracteres a escribir
//-----------------------------------------------------------------------------------------
//Retorno:
//  fd de donde se referencia el fifo, -1 en caso de error
//-----------------------------------------------------------------------------------------
int write(int fd, const char * buf, int count){
    pipe_restrict * pipe_mode = get_current_pcb()->fd[fd];
    if(pipe_mode->mode==O_RDONLY){
        return -1;
    }

    int write;
    //lock del mutex del pipe
    sem_wait(pipe_mode->info->lock);

    for(write=0; write<count && buf[write] != '\0'; write++){
        if(pipe_mode->mode==CONSOLE){
            print_char(buf[write], WHITE, ALL);
        }
        else if(pipe_mode->mode==CONSOLE_ERR){
            print_char(buf[write], RED, ALL);
        }
        else{
            //Si la distancia es el tamaño del buffer => en el proximo pisaria informacion
            while(pipe_mode->info->index_write == pipe_mode->info->index_read + PIPESIZE){
                //Despierta a todos los lectores que se habian bloqueado
                for(int i=0; i<MAXLOCK && pipe_mode->info->pid_read_lock[i] != 0; i++){
                    unblock_process_handler(pipe_mode->info->pid_read_lock[i]);
                    pipe_mode->info->pid_read_lock[i] = 0;
                }

                //Bloqueo al proceso actual
                int index_block = index_lock_pid(pipe_mode->info->pid_write_lock);
                pipe_mode->info->pid_write_lock[index_block] = get_current_pcb()->pid;
                pipe_mode->info->pid_write_lock[index_block+1] = 0;
                sem_post(pipe_mode->info->lock);
                //print("block", WHITE, ALL);
                if(block_process_handler(get_current_pcb()->pid) == -1){
                    return -1;
                }
                sem_wait(pipe_mode->info->lock);
            }
            pipe_mode->info->buff[pipe_mode->info->index_write++ % PIPESIZE] = buf[write];
        }
    }
    if(pipe_mode->mode!=CONSOLE){
    //No chequea nunca que haya espacio
    //TODO: revisar, lo saque poruqe nunca se verifica que haya espacio
    //hacer testeos
        pipe_mode->info->buff[pipe_mode->info->index_write % PIPESIZE] = '\0';
    }

    //Desperta a todos los lectores que se habian bloqueado
    for(int i=0; i<MAXLOCK && pipe_mode->info->pid_read_lock[i] != 0; i++){
        unblock_process_handler(pipe_mode->info->pid_read_lock[i]);
        pipe_mode->info->pid_read_lock[i] = 0;
    }
    sem_post(pipe_mode->info->lock);
    return write;
}

int write_keyboard(const char * buf, int count){
    int write;

    sem_wait(console_pipe->lock);
    for(write=0; write<count && buf[write] != '\0'; write++){
        //Si la distancia es el tamaño del buffer => en el proximo pisaria informacion
        if(console_pipe->index_write != console_pipe->index_read + PIPESIZE){
            console_pipe->buff[console_pipe->index_write++ % PIPESIZE] = buf[write];
        }
    }

    //Desperta a todos los lectores que se habian bloqueado
    for(int i=0; i<MAXLOCK && console_pipe->pid_read_lock[i] != 0; i++){
        unblock_process_handler(console_pipe->pid_read_lock[i]);
        console_pipe->pid_read_lock[i] = 0;
    }

    sem_post(console_pipe->lock);
    return write;
}


//-----------------------------------------------------------------------------------------
//read: Leer de un fd
//-----------------------------------------------------------------------------------------
//Argumentos:
//  fd: fd de donde se quiere leer
//  msj: char* donde recibir
//  count: cantidad de caracteres a leer
//-----------------------------------------------------------------------------------------
//Retorno:
//  fd de donde se referencia el fifo, -1 en caso de error
//-----------------------------------------------------------------------------------------
int read(int fd, char * buf, int count){
    pipe_restrict * pipe_mode = get_current_pcb()->fd[fd];
    if(pipe_mode->mode==O_WRONLY){
        return -1;
    }
    int read;
    //lock del mutex del pipe
    sem_wait(pipe_mode->info->lock);
    //No tengo nada para leer
    while(pipe_mode->info->index_read == pipe_mode->info->index_write){
        //char aux[2] = {pipe_mode->info->count_access +'0', '\0'};
        //print("--pipe", WHITE, ALL);
        //print(aux, WHITE, ALL);
        //char aux2[2] = {pipe_mode->count_access +'0', '\0'};
        //print("\nrestrict", WHITE, ALL);
        //print(aux2, WHITE, ALL);
        if(pipe_mode->info->count_access == pipe_mode->count_access){
            //print("aca", WHITE, ALL);
            sem_post(pipe_mode->info->lock);
            return -1;
        }
        //Bloqueo al proceso actual
        int index_block = index_lock_pid(pipe_mode->info->pid_read_lock);
        pipe_mode->info->pid_read_lock[index_block] = get_current_pcb()->pid;
        pipe_mode->info->pid_read_lock[index_block+1] = 0;
        //TODO revisar concurrencia
        sem_post(pipe_mode->info->lock);
        if(block_process_handler(get_current_pcb()->pid) == -1){
            return -1;
        }
        sem_wait(pipe_mode->info->lock);
    }
    //para que devuelva -1 en el caso de eof
    if(pipe_mode->info->buff[pipe_mode->info->index_read % PIPESIZE]==EOF && pipe_mode->mode==CONSOLE){
        sem_post(pipe_mode->info->lock);
        pipe_mode->info->index_read++;
        return -1;
    }
    //Si tengo algo para leer entonces no lo tengo que bloquear cuando termina
    for(read = 0; read < count && pipe_mode->info->index_read != pipe_mode->info->index_write && !(pipe_mode->info->buff[pipe_mode->info->index_read % PIPESIZE]==EOF && pipe_mode->mode==CONSOLE); read++){
        buf[read] = pipe_mode->info->buff[pipe_mode->info->index_read++ % PIPESIZE];
        if(pipe_mode->mode==CONSOLE){
            //imprimimos el caracter por consola
            print_char(pipe_mode->info->buff[(pipe_mode->info->index_read-1) % PIPESIZE],WHITE,ALL);
        }
    }
    //TODO: revisar, nunca chequea que haya espacio, hay que testearlo
    buf[read] = '\0';

    //Desperta a todos los escritores que se habian bloqueado
    for(int i=0; i<MAXLOCK && pipe_mode->info->pid_write_lock[i] != 0; i++){
        unblock_process_handler(pipe_mode->info->pid_write_lock[i]);
        pipe_mode->info->pid_write_lock[i] = 0;
    }
//    if(pipe_mode->mode==CONSOLE){
//        print(buf, WHITE, ALL);
//    }
    sem_post(pipe_mode->info->lock);
    return read;
}


//-----------------------------------------------------------------------------------------
//get_info: Pasar informacion de los pipes al usuario
//-----------------------------------------------------------------------------------------
//Argumentos:
//  user_data: Puntero desde el user donde cargar la informacion
//  count: puntero con la cantidad de estructuras del user_data
//-----------------------------------------------------------------------------------------
//Retorno:
//-----------------------------------------------------------------------------------------
void get_info(pipe_user_info * user_data, int * count){
    orderListADT_toBegin(pipes_list);
    int i=0;
    pipe_info * info_next;
    while(orderListADT_hasNext(pipes_list)){
        info_next = orderListADT_next(pipes_list);
        user_data[i].index_read=info_next->index_read;
        user_data[i].index_write=info_next->index_write;
        user_data[i].name=info_next->name;
        for(int j=0; j<MAXLOCK; j++){
            user_data[i].pid_read_lock[j]=info_next->pid_read_lock[j];
            user_data[i].pid_write_lock[j]=info_next->pid_write_lock[j];
        }
        i++;
    }
    (*count) = i;
}


//-----------------------------------------------------------------------------------------
//dup2: Duplicar la referencia de un fd en otro
//-----------------------------------------------------------------------------------------
//Argumentos:
//  oldfd: fd que se quiere duplicar
//  newfd: nuevo fd a referenciar
//-----------------------------------------------------------------------------------------
//Retorno:
//  retorna newfd
//-----------------------------------------------------------------------------------------
int dup2(int oldfd, int newfd){
    pipe_restrict * pipe_mode = get_current_pcb()->fd[oldfd];
    get_current_pcb()->fd[newfd] = pipe_mode;
    pipe_mode->count_access++;
    pipe_mode->info->count_access++;
    return newfd;
}


//-----------------------------------------------------------------------------------------
//dup: Duplicar la referencia de un fd en el proximo fd libre
//-----------------------------------------------------------------------------------------
//Argumentos:
//  oldfd: fd que se quiere duplicar
//-----------------------------------------------------------------------------------------
//Retorno:
//  retorna newfd o (-1 en caso de error)
//-----------------------------------------------------------------------------------------
int dup(int oldfd){
    pipe_restrict * pipe_mode = get_current_pcb()->fd[oldfd];
    int newfd = get_next_fd();
    if(newfd == -1){
        return -1;
    }
    get_current_pcb()->fd[newfd] = pipe_mode;
    pipe_mode->count_access++;
    pipe_mode->info->count_access++;
    return newfd;
}


//-----------------------------------------------------------------------------------------
//Funcion de Comparacion para la lista de Pipes
//  Se intenta que los pipes con nombres esten primero para mejorar la performance en busqueda
//-----------------------------------------------------------------------------------------
int64_t pipe_compare(pipe_info * elem1, pipe_info * elem2){
    if(elem1->name == NULL && elem2->name==NULL)
        return elem1 - elem2;
    if(elem1->name == NULL)
        return -1;
    if(elem2->name==NULL)
        return 1;
    return *elem1->name - *elem1->name;
}


//-----------------------------------------------------------------------------------------
//create_info_pipe: crea la estructura con los datos de un pipe
//-----------------------------------------------------------------------------------------
pipe_info * create_info_pipe(char * name){
    pipe_info * aux = mm_alloc(sizeof(pipe_info));
//    aux->lock = mm_alloc(sizeof (sem_t));
    if((aux->lock=sem_init(NULL, 1)) == NULL){
        mm_free(aux->lock);
        mm_free(aux);
        return NULL;
    }
    aux->name=name;
    aux->count_access=0;
    aux->index_write=0;
    aux->index_read=0;
    aux->pid_write_lock[0]=0;
    aux->pid_read_lock[0]=0;
    return aux;
}


//-----------------------------------------------------------------------------------------
//create_restrict_pipe: crea la estructura a la que apunta un fd
//-----------------------------------------------------------------------------------------
pipe_restrict * create_restrict_pipe(Pipe_modes mode, pipe_info * info){
    pipe_restrict * aux = mm_alloc(sizeof(pipe_restrict));
    if(aux == NULL){
        return NULL;
    }
    aux->info=info;
    aux->mode=mode;
    aux->count_access=1;
    aux->info->count_access++;
    return aux;
}


//-----------------------------------------------------------------------------------------
//get_next_fd: Devuelve el proximo fd libre del PCB actual
//-----------------------------------------------------------------------------------------
int get_next_fd(){
    for(int i=0; i<MAXFD; i++){
        if(get_current_pcb()->fd[i] == NULL){
            return i;
        }
    }
    return -1;
}


//-----------------------------------------------------------------------------------------
//index_lock_pid: Devuelve ultimo indice usado en la lista de bloqueados
//-----------------------------------------------------------------------------------------
int index_lock_pid(const uint64_t array[MAXLOCK]){
    for(int i=0; i<MAXLOCK; i++){
        if(array[i] == 0){
            return i;
        }
    }
    return -1;
}

