//Inicio scheduler SO
#include <scheduler.h>
#include <RRADT.h>
#include "../include/RRADT.h"
#include <mm.h>
#include "../include/mm.h"
#include <dispatcher.h>
#include <interrupts.h>
#include <video_driver.h>
#include <HashADT.h>
#include <pipes.h>
//TODO: sacar
#include "../include/queueADT.h"
#include "../include/video_driver.h"
#include "../include/interrupts.h"
#include "../include/dispatcher.h"
#include "../include/HashADT.h"
#include "../include/pipes.h"

static RRADT rr = NULL;
static uint64_t new_pid = 1;
uint64_t scheduler_ticks = 0;
static PCB* current_process=NULL;
#define BASE_PRIORITY 2
#define CHECK_PID(pid,elem) (pid>=1 && pid<new_pid && hashADT_get(hash,elem)!=NULL)
//TODO: en el pid=0, guardamos al proceso default
extern void idle_process();

//hasta que no este listo el hash, uso un arreglo para guardar a los procesos
//static PCB* hash[6000]={0};
static HashADT hash = NULL;

//TODO: crear con malloc los strings para los argumentos, para no tener problemas de referenciar memoria local
//Liberarlos cuando se termina el proceso

//inicializa las estructuras que va a utilizar el scheduler
int initialize_scheduler(){
    rr = new_RR();
    if(rr==NULL){
        return -1;
    }
    hash = new_hashADT(elemType_prehash,elemType_compare_to);
    if(hash==NULL){
        return -1;
    }
    PCB* new_process = mm_alloc(sizeof (PCB));
    if(new_process==NULL){
        return -1;
    }
    void* bp; //para guardar las direcciones donde se ubica el stack del proceso
    void* sp;
    if(create_new_process_context(0,NULL,&idle_process,&bp,&sp)==-1){
        return -1;
    }
    new_process->bp = bp;
    new_process->name = "Idle process";
    new_process->sp = sp;
    new_process->pid = 0;
    new_process->status = READY; //ESTO ES FUNDAMENTAL PARA QUE SIEMPRE QUE LLEGA EL TT INTENTE SACARLO
    new_process->priority = BASE_PRIORITY;
    new_process->waiting_processes = NULL;
    new_process->foreground = 0; //Es de background
    new_process->waiting_processes = new_queueADT(elemType_compare_to);
    new_process->waiting_count = 0;
    //No se lo carga en RR, para ejecutarlo solo cuando no hay otros
//    hash[0] = new_process;
    hashADT_add(hash, new_process);
    return 0;
}
//devuelve el pid del proceso si no hubo error, -1 si hubo error
int create_process(executable_t* executable){
    PCB* new_process = mm_alloc(sizeof (PCB));
    if(new_process==NULL){
        return -1;
    }
    void* bp; //para guardar las direcciones donde se ubica el stack del proceso
    void* sp;
    if(create_new_process_context(executable->arg_c,executable->arg_v,executable->start,&bp,&sp)==-1){
        return -1;
    }
    new_process->bp = bp;
    new_process->sp = sp;
    new_process->pid = new_pid;
    new_process->status = READY;
    new_process->name = executable->name;
    new_process->foreground = executable->foreground;
    new_process->priority = BASE_PRIORITY;
    new_process->waiting_count = 0;
    new_process->waiting_processes = new_queueADT(elemType_compare_to); //para despertar a los que estan esperando solo si lo estan esperando
    //Sirve para no despertar siempre al padre por ejemplo, solo cuando hizo waitpid
    //Si no, podria despertar al padre de un pipe y no tiene que ser asi
//    hash[new_pid++]=new_process;
    if(executable->fds == NULL){
        for(int i=0; i<MAXFD; i++){
            new_process->fd[i] = current_process->fd[i];
            if(new_process->fd[i] != NULL){
                new_process->fd[i]->count_access++;
                new_process->fd[i]->info->count_access++;
            }
        }
    } else{
        int i;
        for(i=0; i<DEFAULTFD; i++){
            new_process->fd[i] = current_process->fd[executable->fds[i]];
            if(new_process->fd[i] != NULL){
                new_process->fd[i]->count_access++;
                new_process->fd[i]->info->count_access++;
            }
        }
        for(; i<MAXFD; i++){
            new_process->fd[i] = NULL;
        }
    }
    new_pid++;
    hashADT_add(hash, new_process);
    if( RR_add_process(rr,new_process,BASE_PRIORITY)==-1){
        return -1;
    }
    return new_process->pid;
}
//Cambia el estado de un proceso a bloqueado
//No cambia el proceso que va a seguir ejecutandose luego de la llamada
//por lo que si se bloquea al proceso que se esta ejecutando, se debe llamar a la funcion para elegir el proceso que sigue3
int block_process(uint64_t pid){
    PCB wanted;
    wanted.pid = pid;
    if(!CHECK_PID(pid,&wanted)){
        return -1;
    }

    PCB* process = hashADT_get(hash,&wanted);
    if(process->status==FINISHED || process->status == BLOCKED){
        return -1;
    }
    //PCB* process = HashProcess_get(myHash, pid);
    //lo saco de la cola de listos (solo me sirve si no es el que esta corriendo)
    RR_remove_process(rr,process->priority,process);
    process->status=BLOCKED;
    //si es una syscall bloqueante, se debe llamar en ella al cambio de contexto si se desea bloquear al proceso que esta corriendo
    return 0;
}
int yield_current_process(){
    if(current_process==NULL){
        return -1;
    }
    current_process->status = READY;
    return 0;
}
//TODO: ver si tenemos que guardar el valor de retorno en rax, aunque creo que el wait no lo espera
int terminate_process(uint64_t pid){
    PCB wanted;
    wanted.pid = pid;
    if(!CHECK_PID(pid,&wanted)){
        return -1;
    }
//    PCB* process = hash[pid];
    PCB* process = hashADT_get(hash,&wanted);
    process->status = FINISHED;
    for(int i=0; i<DEFAULTFD; i++){
        close_fd(i);
    }
    RR_remove_process(rr,process->priority,process); //lo sacamos de la cola de listos (si es que esta todavia)
    //Liberamos ahora a la memoria, pero en el caso donde esta terminando el proceso que se estaba ejecutando, hay que
    //notar que si esta terminando el proceso que estaba en ejecucion, parece peligroso liberar la memoria ahora
    //sin embargo, es responsabilidad de la syscall llamar al cambio de contexto luego de terminar a un proceso
    //Y si se cambia el contexto inmediatamente, entonces no hay problemas de seguridad con liberar la memoria ahora
    //porque nadie va a pedir mas memoria hasta que termine la syscall (interrupt gate)
    free_process_stack(process->bp);
    PCB* curr = NULL;
    //Desbloquea a todos los que lo estan esperando
    while ((curr = queueADT_get_next( process->waiting_processes))!=ELEM_NOT_FOUND){
        unblock_process(curr->pid);//desbloqueamos al que lo esta esperando
    }
    free_queueADT(process->waiting_processes);
    return 0;
}
//cambia la prioridad de un proceso dado su pid
int change_process_priority(uint64_t pid, uint8_t new_priority){
    PCB wanted;
    wanted.pid = pid;
    if(!CHECK_PID(pid,&wanted) || !CHECK_PRIORITY(new_priority)){
        return -1;
    }
//    PCB* process = hash[pid];

    PCB* process = hashADT_get(hash,&wanted);
    //PCB* process = HashProcess_get(myHash, pid);
    //Lo tenemos que mover a la cola de la nueva prioridad si esta en el RR o deberia estar
//    if(process->priority!=new_priority && (process->status==READY || process->status == EXECUTE)){
    if(process->priority!=new_priority && process->status==READY ){
        RR_remove_process(rr,process->priority,process);
        RR_add_process(rr,process,new_priority);
    }
    process->priority=new_priority;
    return 0;
}
int unblock_process(uint64_t pid){
    PCB wanted;
    wanted.pid = pid;

    if(!CHECK_PID(pid,&wanted)){
        return -1;
    }
//    PCB* process = hash[pid];
    PCB* process = hashADT_get(hash,&wanted);
    if(process->status!=BLOCKED){
        return -1;
    }
    //PCB* process = HashProcess_get(myHash, pid);
    process->status = READY;
    //lo agrego para que pueda seguir ejecutandose cuando sigue
    RR_add_process(rr,process,process->priority);
    return 0;
}
//devuelve RSP del proceso que debe continuar en ejecucion
void* scheduler(void* curr_rsp){
    //si debe seguir el proceso que esta en el momento
    //Se determina la cantidad de ticks con la prioridad
    if(current_process!=NULL && current_process->status==EXECUTE && scheduler_ticks< GET_QUANTUM_INTERVAL(current_process->priority)){
//        current_process->sp = curr_rsp;
//        return current_process->sp;
        return curr_rsp;
    }
    //Si corresponde, guardamos al proceso que se estaba ejecutando para que siga luego
    if(current_process!=NULL){
        PCB aux;
        aux.pid = 0;
        //si tenemos que dejarlo para mas adelante
//        if((current_process->status==EXECUTE || current_process->status==READY)&&current_process!=hash[0]){
        if((current_process->status==EXECUTE || current_process->status==READY)&&current_process!= hashADT_get(hash,&aux)){
            current_process->status = READY;
            RR_add_process(rr,current_process,current_process->priority);
        }
        //si no, fue bloqueado o terminado (en todos los casos guardamos a sp)
        current_process->sp = curr_rsp;
    }
    //Debemos buscar el siguiente proceso a ejecutar
    if(RR_process_count(rr)==0){
        //si no hay procesos listos, debemos ejecutar al proceso default
        //Notar que no le ponemos el estado en EXECUTE para que intente salir en el proximo tt
//        current_process = hash[0];
        PCB aux;
        aux.pid = 0;
        current_process = hashADT_get(hash,&aux);
        return current_process->sp;
    }
    scheduler_ticks = 0; //reiniciamos los ticks para el nuevo proceso
    //tenemos que seguir con el proximo proceso segun RR
    current_process = RR_get_next(rr);
    current_process->status = EXECUTE;
    return current_process->sp;
}

int waitPid(uint64_t pid){
    PCB wanted;
    wanted.pid = pid;
    if(!CHECK_PID(pid,&wanted)){
        return -1;
    }
    PCB* waited = hashADT_get(hash,&wanted);
    //Me agrego a la lista de los que estan esperando para que me despierte cuando termine
//    if(hash[pid]->status!=FINISHED){
    if(waited->status!=FINISHED){
//        queueADT_insert(hash[pid]->waiting_processes,current_process);
        queueADT_insert(waited->waiting_processes,current_process);
    }
    //Es decir, cuando ya no queda alguno por hacer wait
    //Lo hice con la variable waiting_count

    //veo si el proceso por el que espera ya termino
    //if(HashProcess_get(myHash, pid)->status!=FINISHED);
//    while(hash[pid]->status!=FINISHED){
//        (hash[pid]->waiting_count)++;
//        //Si el proceso no termino
//        //bloqueo al proceso que llama a wait
//        block_process(current_process->pid);
//        _int20();
//        (hash[pid]->waiting_count)--;
//    }
//    int ans = hash[pid]->status;
////    (hash[pid]->waiting_count)--;
//    if(hash[pid]->waiting_count==0){
//        //si soy el ultimo en hacer wait, lo saco del hash
//        //TODO: sacar del hash
//        mm_free(hash[pid]);
//        hash[pid]=NULL;
//    }

    while(waited->status!=FINISHED){
        (waited->waiting_count)++;
        //Si el proceso no termino
        //bloqueo al proceso que llama a wait
        block_process(current_process->pid);
        _int20();
        (waited->waiting_count)--;
    }
    int ans = waited->status;
//    (hash[pid]->waiting_count)--;
    if(waited->waiting_count==0){
        //si soy el ultimo en hacer wait, lo saco del hash
        //TODO: sacar del hash
        hashADT_delete(hash,waited);
        mm_free(waited);
    }
    //TODO: ver si llego a devolver el valor de retorno
    return ans;
}
uint64_t get_current_pid(){
    if(current_process==NULL){
        return -1;
    }
    return current_process->pid;
}
PCB * get_current_pcb(){
    return current_process;
}
int32_t get_scheduler_info(process_info_t* processInfo, uint32_t max_count){
    //TODO: cambiar por el iterador del hash map
    PCB wanted;
    int32_t index = 0;
    for(int i = 0; i<max_count && i<new_pid; i++){
//        PCB* process = hash[i];
        wanted.pid = i;
        PCB* process = hashADT_get(hash,&wanted);
        if(process!=NULL){
            processInfo[index].name = process->name;
            processInfo[index].pid = process->pid;
            processInfo[index].status = process->status;
            processInfo[index].priority = process->priority;
            processInfo[index].foreground = process->foreground;
            processInfo[index].sp = (uint64_t) process->sp;
            processInfo[index++].bp = (uint64_t) process->bp;
        }
    }
    return index;
}
uint64_t get_process_count(){
    //TODO: cambiar con el hash
    return new_pid;
}
