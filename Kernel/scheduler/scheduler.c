//Inicio scheduler SO
#include <scheduler.h>
#include <RRADT.h>
#include <mm.h>
#include <dispatcher.h>
#include <interrupts.h>
#include <video_driver.h>
#include <HashADT.h>
#include <pipes.h>
#include <stringLib.h>
#include <stringLib.h>
#include <queueADT.h>


//en el pid=0, guardamos al proceso default
static void* stack_to_free = NULL;
static RRADT rr = NULL;
static uint64_t new_pid = 1;
uint64_t scheduler_ticks = 0;
uint64_t max_ticks_for_current = 0;
static PCB* current_process=NULL;
static pipe_restrict * default_fds[3];
static int ticks_for_priority[PRIORITY_LEVELS] = {10,8,6,4,2};
static HashADT hash = NULL;

extern void idle_process();


// TODO: ver preentrega que hacer con los IFs que hicimos para los print si tenemos que manejar el error

//----------------------------------------------------------------------
// initialize_scheduler: inicializa las estructuras que va a utilizar el scheduler
//----------------------------------------------------------------------
// Argumentos:
//----------------------------------------------------------------------
// Retorno:
//  -1 en caso de error, 0 en caso de exito
//----------------------------------------------------------------------
int initialize_scheduler(){
    rr = new_RR();
    if(rr==NULL){
        return -1;
    }
    default_fds[0] = get_pipe_console_restrict();
    default_fds[1] = get_pipe_console_restrict();
    default_fds[2] = get_error_pipe_console();
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
    new_process->arg_c = 0;
    new_process->arg_v = NULL;
    new_process->pid = 0;
    //ESTO ES FUNDAMENTAL PARA QUE SIEMPRE QUE LLEGA EL TT INTENTE SACARLO
    new_process->status = READY;
    new_process->priority = BASE_PRIORITY_BACKGROUND;
    new_process->foreground = 0; //Es de background
    new_process->waiting_processes = new_queueADT(elemType_compare_to);
    new_process->waiting_count = 0;
    //No se lo carga en RR, para ejecutarlo solo cuando no hay otros
    hashADT_add(hash, new_process);
    if(hashADT_get(hash,new_process)==NULL){
        return -1;
    }
    return 0;
}

//----------------------------------------------------------------------
// create_process: creacion de proceso mediante la informacion de la estructura
//----------------------------------------------------------------------
// Argumentos:
//      executable: estructura con los datos de ejecucion del programa
//----------------------------------------------------------------------
// Retorno:
//  -1 en caso de error, 0 en caso de exito
//----------------------------------------------------------------------
int create_process(executable_t* executable){
    PCB* new_process = mm_alloc(sizeof (PCB));
    if(new_process==NULL){
        return -1;
    }
    void* bp; //para guardar las direcciones donde se ubica el stack del proceso
    void* sp;
    new_process->arg_c = executable->arg_c;
    //Para que otro no cambie los argumentos si pasa una zona de memoria local
    char** arg_v = executable->arg_v;
    if(new_process->arg_c!=0){
        arg_v = mm_alloc(executable->arg_c*sizeof (char*));
        for(int i = 0;i<executable->arg_c; i++){
            char* aux = mm_alloc((strlen(executable->arg_v[i])+1)*sizeof (char));
            strcpy(aux,executable->arg_v[i]);
            arg_v[i] = aux;
        }
    }
    new_process->arg_v = arg_v;
    if(create_new_process_context(new_process->arg_c,new_process->arg_v,executable->start,&bp,&sp)==-1){
        return -1;
    }
    new_process->bp = bp;
    new_process->sp = sp;
    new_process->pid = new_pid;
    new_process->status = READY;
    new_process->name = executable->name;
    new_process->foreground = executable->foreground;
    new_process->priority = (executable->foreground)?BASE_PRIORITY_FOREGROUND:BASE_PRIORITY_BACKGROUND;
    new_process->waiting_count = 0;
    new_process->waiting_processes = new_queueADT(elemType_compare_to); //para despertar a los que estan esperando solo si lo estan esperando
    if(new_process->waiting_processes==NULL){
        return -1;
    }
    //Sirve para no despertar siempre al padre por ejemplo, solo cuando hizo waitpid
    //Si no, podria despertar al padre de un pipe y no tiene que ser asi
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
            if(executable->fds[i]==-1){
                new_process->fd[i] = default_fds[i];
                new_process->fd[i]->count_access++;
                new_process->fd[i]->info->count_access++;
            }else {
                new_process->fd[i] = current_process->fd[executable->fds[i]];
                if (new_process->fd[i] != NULL) {
                    new_process->fd[i]->count_access++;
                    new_process->fd[i]->info->count_access++;
                }
            }
        }
        for(; i<MAXFD; i++){
            new_process->fd[i] = NULL;
        }
    }
    new_pid++;
    hashADT_add(hash, new_process);
    if( RR_add_process(rr,new_process,new_process->priority)==-1){
        return -1;
    }
    return new_process->pid;
}


//----------------------------------------------------------------------
// block_process: Cambia el estado de un proceso a bloqueado
// No cambia el proceso que va a seguir ejecutandose luego de la llamada por lo que si se bloquea al proceso que se esta ejecutando
// se debe llamar a la funcion para elegir el proceso que sigue3
//----------------------------------------------------------------------
// Argumentos:
//      executable: estructura con los datos de ejecucion del programa
//----------------------------------------------------------------------
// Retorno:
//  -1 en caso de error, 0 en caso de exito
//----------------------------------------------------------------------
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


//----------------------------------------------------------------------
// terminate_process: Cambia el estado de un proceso a terminado
//----------------------------------------------------------------------
// Argumentos:
//      pid: pid del programa a terminar
//----------------------------------------------------------------------
// Retorno:
//  -1 en caso de error, 0 en caso de exito
//----------------------------------------------------------------------
int terminate_process(uint64_t pid){
    PCB wanted;
    wanted.pid = pid;
    if(!CHECK_PID(pid,&wanted)){
        return -1;
    }
    PCB* process = hashADT_get(hash,&wanted);
    if(process->status==FINISHED){
        return -1;
    }
    process->status = FINISHED;
    for(int i=0; i<MAXFD; i++){
        if(process->fd[i]!=NULL) {
            close_fd(i, pid);
        }
    }
    RR_remove_process(rr,process->priority,process); //lo sacamos de la cola de listos (si es que esta todavia)

    PCB* curr = NULL;
    //Desbloquea a todos los que lo estan esperando
    while ((curr = queueADT_get_next( process->waiting_processes))!=ELEM_NOT_FOUND){
        unblock_process(curr->pid);//desbloqueamos al que lo esta esperando
    }
    free_queueADT(process->waiting_processes);
    for(int i = 0; i<process->arg_c; i++){
        mm_free(process->arg_v[i]);
    }
    if(process->arg_c!=0){
        mm_free(process->arg_v);
    }
    //lo hago aca, para que no haya algun mm_alloc despues
    stack_to_free = process->bp;
    //No liberamos la memoria aca, pues la funcion scheduler que se va a llamar despues para cambiar el contexto hace
    //mm_alloc para estruturas auxiliares, y eso podria estar pisando el stack donde estamos hasta el cambio de contexto
    //con la funcion free_unused_stack, liberamos la memoria en un punto donde sabemos que no vamos a hacer mas mm_alloc
    //en el contexto del proceso terminado
    return 0;
}


//----------------------------------------------------------------------
// change_process_priority: Cambia la prioridad de un proceso segun su pid
//----------------------------------------------------------------------
// Argumentos:
//      pid: pid del programa a cambiar la prioridad
//      new_prority: la nueva prioridad del proceso
//----------------------------------------------------------------------
// Retorno:
//  -1 en caso de error, 0 en caso de exito
//----------------------------------------------------------------------
int change_process_priority(uint64_t pid, uint8_t new_priority){
    PCB wanted;
    wanted.pid = pid;
    if(!CHECK_PID(pid,&wanted) || !CHECK_PRIORITY(new_priority)){
        return -1;
    }
//    PCB* process = hash[pid];

    PCB* process = hashADT_get(hash,&wanted);
    if(process->status == FINISHED){
        return -1;
    }
    //Lo tenemos que mover a la cola de la nueva prioridad si esta en el RR o deberia estar
    if(process->priority!=new_priority && process->status==READY ){
        RR_remove_process(rr,process->priority,process);
        RR_add_process(rr,process,new_priority);
    }
    process->priority=new_priority;
    return 0;
}

//----------------------------------------------------------------------
// unblock_process: Cambiar el estado de un proceso a READY
//----------------------------------------------------------------------
// Argumentos:
//      pid: pid del programa
//----------------------------------------------------------------------
// Retorno:
//  -1 en caso de error, 0 en caso de exito
//----------------------------------------------------------------------
int unblock_process(uint64_t pid){
    PCB wanted;
    wanted.pid = pid;

    if(!CHECK_PID(pid,&wanted)){
        //ACA
        //La unica manera en la que puede no estar es porque
        //se termino al proceso, si no no se saca del hash
        return -1;
    }
    PCB* process = hashADT_get(hash,&wanted);
    if(process->status!=BLOCKED){
        //Puede entrar aca si tiene timers luego de que hay
//        print("El pid no esta bloqueado",WHITE,ALL);
        if(process->status==FINISHED){
//            print("El proceso ya ha terminado",WHITE,ALL);
        }
        return -1;
    }
    process->status = READY;
    //lo agrego para que pueda seguir ejecutandose cuando sigue
    if(RR_add_process(rr,process,process->priority)==-1){
//        print("Error al agregar el proceso para ejecucion luego de desbloquearlo",WHITE,ALL);
    }
    return 0;
}


static void free_unused_stack(){
    if(stack_to_free!=NULL){
        free_process_stack(stack_to_free);
        stack_to_free = NULL;
    }
}


//----------------------------------------------------------------------
// scheduler: devuelve RSP del proceso que debe continuar en ejecucion
//----------------------------------------------------------------------
void* scheduler(void* curr_rsp){
    //si debe seguir el proceso que esta en el momento
    //Se determina la cantidad de ticks con la prioridad
    if(current_process!=NULL && current_process->status==EXECUTE && scheduler_ticks< max_ticks_for_current){
//        current_process->sp = curr_rsp;
//        return current_process->sp;
        free_unused_stack();
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
            if(RR_add_process(rr,current_process,current_process->priority)==-1){
//                print("Error al agregar el proceso para la vuelta",WHITE,ALL);
            }
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
        free_unused_stack();
        return current_process->sp;
    }
    scheduler_ticks = 0; //reiniciamos los ticks para el nuevo proceso
    //tenemos que seguir con el proximo proceso segun RR
    current_process = RR_get_next(rr);
    max_ticks_for_current = ticks_for_priority[current_process->priority];
    current_process->status = EXECUTE;
    free_unused_stack();
    return current_process->sp;
}

int waitPid(uint64_t pid){
    PCB wanted;
    wanted.pid = pid;
    if(!CHECK_PID(pid,&wanted)){
//        print("Se esta esperando a un proceso inexistente",RED,ALL);
        return -1;
    }
    PCB* waited = hashADT_get(hash,&wanted);
    //Me agrego a la lista de los que estan esperando para que me despierte cuando termine
//    if(hash[pid]->status!=FINISHED){
    if(waited->status!=FINISHED){
//        queueADT_insert(hash[pid]->waiting_processes,current_process);
        if(queueADT_insert(waited->waiting_processes,current_process)==-1){
//            print("Error al ingresar el proceso a la lista de esperando",RED,ALL);
        }
    }

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
        hashADT_delete(hash,waited);
        mm_free(waited);
    }
    return ans;
}


uint64_t get_current_pid(){
    if(current_process==NULL){
        return 0;
    }
    return current_process->pid;
}


PCB * get_current_pcb(){
    return current_process;
}


PCB* get_pcb_by_pid(uint64_t pid){
    PCB wanted;
    wanted.pid = pid;
    return hashADT_get(hash,&wanted);
}


//Perdon por usar Bubble sort, pero no vamos a tener muchos procesos simultaneamente
//Y no usa recursividad o arreglos temporales
static void sort_process_info(process_info_t* processInfo, uint32_t n){
    for(int i = 0; i<n-1; i++){
        for(int j = 0; j<n-i-1;j++){
            if(processInfo[j].pid>processInfo[j+1].pid){
                process_info_t temp = processInfo[j];
                processInfo[j] = processInfo[j+1];
                processInfo[j+1] = temp;
            }
        }
    }
}


int32_t get_scheduler_info(process_info_t* processInfo, uint32_t max_count){
    int32_t index = 0;
    hashADT_to_begin(hash);
    while (hashADT_has_next(hash)&&index<max_count){
        PCB* process = hashADT_next(hash);
        processInfo[index].name = process->name;
        processInfo[index].pid = process->pid;
        processInfo[index].status = process->status;
        processInfo[index].priority = process->priority;
        processInfo[index].foreground = process->foreground;
        processInfo[index].sp = (uint64_t) process->sp;
        processInfo[index++].bp = (uint64_t) process->bp;
    }
    sort_process_info(processInfo,index);
    return index;
}


//----------------------------------------------------------------------
// get_process_count: Cantidad de procesos
//----------------------------------------------------------------------
uint64_t get_process_count(){
    return hashADT_size(hash);
}


//----------------------------------------------------------------------
// kill_foreground_process: Termina los preocesos actuales en foreground y retorna al bash
//----------------------------------------------------------------------
// Argumentos:
//----------------------------------------------------------------------
// Retorno:
//----------------------------------------------------------------------
void kill_foreground_process(){
    hashADT_to_begin(hash);
    while(hashADT_has_next(hash)){
        PCB* process = hashADT_next(hash);
        if(process->foreground == 1 && process->pid!=1){
            terminate_process(process->pid);
        }
    }
    _int20();
}