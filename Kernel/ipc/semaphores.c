#include <semaphores.h>
#include <stringLib.h>
#include <syscalls.h>
#include <mm.h>
#include <scheduler.h>

typedef struct {
    uint64_t next_sem_id;                   // Guarda el id del proximo semaforo que se cree
    orderListADT semaphores;                // Lista con los semaforos activos
    uint8_t lock;                           // Lock para manejar las solicitudes de los distintos semaforos
} manager_t;


static manager_t sem_manager = {0, NULL, FREE};


// Realiza la operacion _xchg de ASM
extern uint8_t _xchg(uint8_t * lock, uint8_t value);

//----------------------------------------------------------------------
// cmp_process: Funcion para comparar dos procesos por su pid
//----------------------------------------------------------------------
// Argumentos:
//  p1: pid del primer proceso
//  p2: pid del segundo proceso
//----------------------------------------------------------------------
// Retorno:
//  Devuelve 0 si son el mismo proceso, > 0 si el pid del primer proceso
//  es mayor al segundo y < 0 si el pid del primer proceso es menor al segundo
//----------------------------------------------------------------------
int64_t cmp_process(uint64_t * p1, uint64_t * p2){
    return *p1 - *p2;
}

//----------------------------------------------------------------------
// cmp_semaphores: Funcion para comparar dos semaforos
//----------------------------------------------------------------------
// Argumentos:
//  s1: semaforo 1
//  s2: semaforo 2
//----------------------------------------------------------------------
// Retorno:
//  Devuelve 0 si son el mismo semaforo, > 0 si el id del primer semaforo
//  es mayor al segundo y < 0 si el id del primer semaforo es menor al segundo
//----------------------------------------------------------------------
int64_t cmp_semaphores(sem_t * s1, sem_t * s2){
    return s1->id - s2->id;
}

//----------------------------------------------------------------------
// acquire: Intenta tomar el semaforo
//----------------------------------------------------------------------
// Argumentos:
//  lock: el semaforo que se desea obtener
//----------------------------------------------------------------------
// Retorno:
//----------------------------------------------------------------------
static void acquire(uint8_t * lock){
    while(_xchg(lock, OCCUPIED) != FREE);
}

//----------------------------------------------------------------------
// release: Libera el semaforo
//----------------------------------------------------------------------
// Argumentos:
//  lock: el semaforo que se desea liberar
//----------------------------------------------------------------------
// Retorno:
//----------------------------------------------------------------------
static void release(uint8_t * lock){
    _xchg(lock, FREE);
}


//----------------------------------------------------------------------
// semaphore_manager_init: Inicializa el manejador de semaforos
//----------------------------------------------------------------------
// Argumentos:
//
//----------------------------------------------------------------------
// Retorno:
//  Si hubo algun error, devuelve -1
//  Si hubo exito en la inicializacion, devuelve 0
//----------------------------------------------------------------------
static int8_t semaphore_manager_init(){
    sem_manager.semaphores = new_orderListADT((compare_function)cmp_semaphores);
    if(sem_manager.semaphores == NULL){
        return -1;
    }
    sem_manager.next_sem_id = 0;
    sem_manager.lock = FREE;
    return 0;
}

//----------------------------------------------------------------------
// get_sem_id: Devuelve el proximo id a otorgar
//----------------------------------------------------------------------
// Argumentos:
//
//----------------------------------------------------------------------
// Retorno:
//  El nuevo id
//----------------------------------------------------------------------
static uint64_t get_sem_id(){
    return sem_manager.next_sem_id++;
}

//----------------------------------------------------------------------
// find_sem_by_name: Busca el semaforo por nombre
//----------------------------------------------------------------------
// Argumentos:
//  name: nombre buscado
//----------------------------------------------------------------------
// Retorno:
//  Devuelve el semaforo con dicho nombre
//  Devuelve NULL si no existe
//----------------------------------------------------------------------
static sem_t * find_sem_by_name(char * name){
    sem_t * curr_sem = NULL;

    orderListADT_toBegin(sem_manager.semaphores);

    while(orderListADT_hasNext(sem_manager.semaphores)){
        curr_sem = (sem_t *)orderListADT_next(sem_manager.semaphores);
        if(strcmp(name, curr_sem->name) == 0){
            return curr_sem;
        }
    }

    return NULL;
}


//----------------------------------------------------------------------
// create_semaphore: Crea un nuevo semaforo
//----------------------------------------------------------------------
// Argumentos:
//  name: nombre del semaforo (puede ser NULL)
//  value: valor inicial del semaforo
//----------------------------------------------------------------------
// Retorno:
//  Devuelve el semaforo con dicho nombre
//  Devuelve NULL en caso de error
//----------------------------------------------------------------------
static sem_t * create_semaphore(char * name, uint64_t value){

    uint64_t pid = 0, * pid_p = NULL, name_size = 0;

    // Creamos el semaforo
    sem_t * sem = mm_alloc(sizeof(sem_t));
    if(sem == NULL){
        return NULL;
    }

    // Se inicializa el semaforo
    sem->value = value;
    sem->id = get_sem_id();

    if(name != NULL){
        // Se verifica que no exista un semaforo con ese nombre
        if(find_sem_by_name(name) != NULL){
            mm_free(sem);
            return NULL;
        }

        name_size = strlen(name);
        sem->name = mm_alloc((name_size + 1) * sizeof(char));
        if(sem->name == NULL){
            mm_free(sem);
            return NULL;
        }
        strcpy(sem->name, name);
    } else {
        sem->name = NULL;
    }
    sem->blocked_processes = new_queueADT((compare_function)cmp_process);
    sem->connected_processes = new_orderListADT((compare_function)cmp_process);

    // Verificamos que se hayan creado correctamente ambas estructuras
    if(sem->blocked_processes == NULL){
        if(sem->connected_processes != NULL){
            free_orderListADT(sem->connected_processes);
        }
        mm_free(sem->name);
        mm_free(sem);
        return NULL;
    } else if(sem->connected_processes == NULL){
        free_queueADT(sem->blocked_processes);
        mm_free(sem->name);
        mm_free(sem);
        return NULL;
    }

    // Agregamos el proceso que creo el semaforo a la lista de procesos conectados
    pid = get_current_pid();
    if(pid==0){
        pid = 1;
        //esto es para la inicializacion del pipe de teclado
        //como todavia no hay un procesos corriendo, pero sabemos que el que va a usar ese pipe en un
        //principio va a ser bash, entonces lo ponemos con 1
    }

    pid_p = mm_alloc(sizeof(uint64_t));
    if(pid_p == NULL){
        free_queueADT(sem->blocked_processes);
        free_orderListADT(sem->connected_processes);
        mm_free(sem->name);
        mm_free(sem);
        return NULL;
    }

    *pid_p = pid;
    if(orderListADT_add(sem->connected_processes, pid_p) == -1){
        free_queueADT(sem->blocked_processes);
        free_orderListADT(sem->connected_processes);
        mm_free(pid_p);
        mm_free(sem->name);
        mm_free(sem);
        return NULL;
    }

    // Se crea el semaforo y se guarda en la lista de semaforos
    if(orderListADT_add(sem_manager.semaphores, sem) == -1){
        free_queueADT(sem->blocked_processes);
        free_orderListADT(sem->connected_processes);
        mm_free(pid_p);
        mm_free(sem->name);
        mm_free(sem);
        return NULL;
    }

    return sem;
}


//----------------------------------------------------------------------
// sem_init: Crea un nuevo semaforo
//----------------------------------------------------------------------
// Argumentos:
//  value: valor inicial del semaforo
//  name: nombre del semaforo (named semaphores)
//----------------------------------------------------------------------
// Retorno:
//  Devuelve la estructura del semaforo creado
//  Devuelve NULL si hubo algun error
//----------------------------------------------------------------------
sem_t * sem_init(char * name, uint64_t value){

    sem_t * sem;

    // Tomamos el lock para acceder a la seccion critica
    acquire(&sem_manager.lock);

    // Verificamos que se haya inicializado el manejador de semaforos
    if(sem_manager.semaphores == NULL){
        if(semaphore_manager_init() == -1){
            release(&sem_manager.lock);
            return NULL;
        }
    }

    sem = create_semaphore(name, value);

    // Se libera el lock para que otro proceso pueda acceder a la seccion critica
    release(&sem_manager.lock);
    return sem;
}


//----------------------------------------------------------------------
// sem_open: Devuelve el semaforo con nombre name
//----------------------------------------------------------------------
// Argumentos:
//  name: nombre del semaforo (named semaphores)
//  value: el valor inicial del semaforo en caso de crearlo
//  mode: indica que accion realizar si no se encontro el semaforo
//----------------------------------------------------------------------
// Retorno:
//  Devuelve la estructura del semaforo si existe
//  Si mode = O_CREATE, devuelve un nuevo semaforo
//  Si mode = O_NULL, devuelve NULL
//  Devuelve NULL si ocurrio algun error
//----------------------------------------------------------------------
sem_t * sem_open(char * name, uint64_t value, open_modes mode){

    sem_t * sem = NULL;
    uint64_t pid = 0, * pid_p = NULL;

    // Tomamos el lock para acceder a la seccion critica
    acquire(&sem_manager.lock);

    // Verificamos que se haya inicializado el manejador de semaforos
    if(sem_manager.semaphores == NULL){
        if(semaphore_manager_init() == -1){
            release(&sem_manager.lock);
            return NULL;
        }
    }

    // Buscamos el semaforo con nombre name
    if(name != NULL){
        sem = find_sem_by_name(name);
    }

    // Si se encontro el semaforo, agregamos el proceso a la lista de procesos conectados si no lo estaba aun
    if(sem != NULL){
        pid = get_current_pid();
        pid_p = mm_alloc(sizeof(uint64_t));
        if(pid_p == NULL){
            release(&sem_manager.lock);
            return NULL;
        }

        *pid_p = pid;
        if(orderListADT_get(sem->connected_processes, pid_p) == NULL){
            if(orderListADT_add(sem->connected_processes, pid_p) == -1){
                mm_free(pid_p);
                release(&sem_manager.lock);
                return NULL;
            }
        }
    }
    // Si no se encontro el semaforo, se crea uno nuevo con ese nombre si mode = O_CREATE
    else if (mode == O_CREATE) {
        sem = create_semaphore(name, value);
    }

    // Se libera el lock para que otro proceso pueda acceder a la seccion critica
    release(&sem_manager.lock);

    // Devolvemos el resultado
    return sem;
}

//----------------------------------------------------------------------
// sem_wait: Realiza la operacion wait sobre el semaforo sem
//----------------------------------------------------------------------
// Argumentos:
//  sem: el semaforo a esperar
//----------------------------------------------------------------------
// Retorno:
//  Devuelve un error_code (ver semaphores.h)
//----------------------------------------------------------------------
int8_t sem_wait(sem_t * sem){
    uint64_t pid = 0, * pid_p = NULL;

    // Verificamos que se nos haya pasado un semaforo inicializado
    if(sem == NULL){
        return ERR_SEM_NOT_INIT;
    }

    // Tomamos el lock para acceder a la seccion critica
    acquire(&sem_manager.lock);

    // Verificamos que se haya inicializado el manejador de semaforos
    if(sem_manager.semaphores == NULL){
        if(semaphore_manager_init() == -1){
            release(&sem_manager.lock);
            return ERR_MM;
        }
        release(&sem_manager.lock);
        return ERR_NOT_INIT;
    }

    // Verificamos que exista el semaforo buscado
    if(orderListADT_get(sem_manager.semaphores, sem) == NULL){
        release(&sem_manager.lock);
        return ERR_NOT_FOUND;
    }

    // Si el semaforo ya vale 0, entonces debemos detener el proceso hasta que se realice un post y sea el proximo en la cola
    // PERO OJO: SE DEBE USAR UN WHILE Y NO UN IF PORQUE AL HACER UN POST NO ESTA GARANTIZADO QUE EL PROCESO DESBLOQUEADO PUEDE CONTINUAR
    // PORQUE LE PUEDE GANAR LA MANO EL PROCESO QUE LO LIBERO (SI ESTA DENTRO DE UN WHILE) O UN TERCER PROCESO QUE REALIZO EL WAIT ANTES DE QUE SE
    // VUELVA A EJECUTAR EL DESBLOQUEADO
    while(sem->value == 0){
        pid = get_current_pid();
        pid_p = mm_alloc(sizeof(uint64_t));
        if(pid_p == NULL){
            release(&sem_manager.lock);
            return ERR_MM;
        }

        *pid_p = pid;
        if(queueADT_insert(sem->blocked_processes, pid_p) == -1){
            mm_free(pid_p);
            release(&sem_manager.lock);
            return ERR_MM;
        }
        release(&sem_manager.lock);
        if(block_process_handler(pid) == -1){
            return ERR_SCHEDULER;
        }
        acquire(&sem_manager.lock);
    }

    // Se decrementa el semaforo para indicar que pudo pasar
    // Si se detuvo, cuando se despierte tambien debe decrementar para tomar el post realizado por otro proceso
    sem->value--;

    // Se libera el lock para que otro proceso pueda acceder a la seccion critica
    release(&sem_manager.lock);
    return OK;
}


//----------------------------------------------------------------------
// sem_post: Realiza la operacion post sobre el semaforo sem
//----------------------------------------------------------------------
// Argumentos:
//  sem: el semaforo a publicar
//----------------------------------------------------------------------
// Retorno:
//  Devuelve un error_code (ver semaphores.h)
//----------------------------------------------------------------------
int8_t sem_post(sem_t * sem){
    uint64_t pid = 0, * pid_p = NULL;

    // Verificamos que se nos haya pasado un semaforo inicializado
    if(sem == NULL){
        return ERR_SEM_NOT_INIT;
    }

    // Tomamos el lock para acceder a la seccion critica
    acquire(&sem_manager.lock);//ACA se queda

    // Verificamos que se haya inicializado el manejador de semaforos
    if(sem_manager.semaphores == NULL){
        if(semaphore_manager_init() == -1){
            release(&sem_manager.lock);
            return ERR_MM;
        }
        release(&sem_manager.lock);
        return ERR_NOT_INIT;
    }

    sem->value++;

    // Liberamos el proximo proceso que este esperando para ejecutarse
    // OJO: No esta garantizado que sea el primero, hay que seguir recorriendo hasta que se libere un proceso bloqueado
    // o hasta que ya no queden procesos en la cola de bloqueados
    if(!queueADT_is_empty(sem->blocked_processes)){
        do {
            pid_p = (uint64_t *)queueADT_get_next(sem->blocked_processes);
            pid = *pid_p;
            mm_free(pid_p);
        } while(unblock_process_handler(pid) == -1 && !queueADT_is_empty(sem->blocked_processes));
    }

    // Se libera el lock para que otro proceso pueda acceder a la seccion critica
    release(&sem_manager.lock);
    return OK;
}


//----------------------------------------------------------------------
// sem_close: Desconecta el proceso del semaforo (y destruye el semaforo si era el ultimo proceso conectado)
//----------------------------------------------------------------------
// Argumentos:
//  sem: el semaforo sobre el cual desconectarse
//----------------------------------------------------------------------
// Retorno:
//  Devuelve un error_code (ver semaphores.h)
//----------------------------------------------------------------------
int8_t sem_close(sem_t * sem){
    uint64_t pid = 0, * pid_p = NULL;

    // Verificamos que se nos haya pasado un semaforo inicializado
    if(sem == NULL){
        return ERR_SEM_NOT_INIT;
    }

    // Tomamos el lock para acceder a la seccion critica
    acquire(&sem_manager.lock);

    // Verificamos que se haya inicializado el manejador de semaforos
    if(sem_manager.semaphores == NULL){
        if(semaphore_manager_init() == -1){
            release(&sem_manager.lock);
            return ERR_MM;
        }
        release(&sem_manager.lock);
        return ERR_NOT_INIT;
    }

    pid = get_current_pid();

    // Quitamos el proceso de la lista de procesos conectados al semaforo
    pid_p = (uint64_t *)orderListADT_get(sem->connected_processes, &pid);
    if(pid_p != NULL){
        pid_p = orderListADT_delete(sem->connected_processes, pid_p);
        mm_free(pid_p);
    }

    // Si era el ultimo proceso conectado, se liberan los recursos del semaforo
    if(orderListADT_is_empty(sem->connected_processes)){
        sem = orderListADT_delete(sem_manager.semaphores, sem);
        free_queueADT(sem->blocked_processes);
        free_orderListADT(sem->connected_processes);
        mm_free(sem->name);
        mm_free(sem);
    }

    // Se libera el lock para que otro proceso pueda acceder a la seccion critica
    release(&sem_manager.lock);
    return OK;
}


//----------------------------------------------------------------------
// sem_count: retorna la cantidad de semaforos actuales
//----------------------------------------------------------------------
// Argumentos:
//----------------------------------------------------------------------
// Retorno:
//  Cantidad de semaforos actuales
//----------------------------------------------------------------------
uint32_t sem_count(){
    uint32_t ans = 0;
    acquire(&sem_manager.lock);
    // Verificamos que se haya inicializado el manejador de semaforos
    //TODO: verificar si deberia irse aca, creo que si porque no hay semaforos que mostrar
    if(sem_manager.semaphores == NULL){
        semaphore_manager_init();
        release(&sem_manager.lock);
        return 0;
    }
    ans = orderListADT_size(sem_manager.semaphores);
    release(&sem_manager.lock);
    return ans;
}

//----------------------------------------------------------------------
// sems_dump: Dado un buffer pasado por el usuario, se devuelve la informacion de los semaforos a traves del buffer
//----------------------------------------------------------------------
// Argumentos:
//  buffer: arreglo con espacio ya asignado para almacenar la informacion
//  length: tamaño del buffer
//----------------------------------------------------------------------
// Luego se debera llamar a sems_dump_free para liberar los recursos
//----------------------------------------------------------------------
// Retorno:
//  Devuelve la cantidad de elementos que tiene el buffer (en caso de que haya menos semaforos que length)
//----------------------------------------------------------------------
uint32_t sems_dump(sem_dump_t * buffer, uint32_t length){
    uint32_t buff_size = 0;
    sem_t * curr_sem = NULL;

    // Verificamos que se nos haya pasado un buffer inicializado
    if(buffer == NULL){
        return 0;
    }

    // Tomamos el lock para acceder a la seccion critica
    acquire(&sem_manager.lock);

    // Verificamos que se haya inicializado el manejador de semaforos
    if(sem_manager.semaphores == NULL){
        semaphore_manager_init();
        release(&sem_manager.lock);
        return 0;
    }

    // Comprobamos cual es menor entre el tamaño del buffer y la cantidad de semaforos
    buff_size = orderListADT_size(sem_manager.semaphores);
    buff_size = (length < buff_size) ? length : buff_size;

    orderListADT_toBegin(sem_manager.semaphores);

    // Vamos iterando por cada semaforo
    for(uint32_t i = 0; i < buff_size; i++){
        uint64_t size = 0;
        uint64_t pid = 0, * pid_p = NULL;

        curr_sem = orderListADT_next(sem_manager.semaphores);

        buffer[i].value = curr_sem->value;
        buffer[i].id = curr_sem->id;
        buffer[i].name = NULL;

        buffer[i].blocked_size = queueADT_size(curr_sem->blocked_processes);
        if(buffer[i].blocked_size != 0){
            buffer[i].blocked_processes = mm_alloc(buffer[i].blocked_size * sizeof(uint64_t));
        }else{
            buffer[i].blocked_processes = NULL;
        }

        buffer[i].connected_size = orderListADT_size(curr_sem->connected_processes);
        if(buffer[i].connected_size != 0){
            buffer[i].connected_processes = mm_alloc(buffer[i].connected_size * sizeof(uint64_t));
        }else{
            buffer[i].connected_processes = NULL;
        }


        // Verificamos que se hayan podido crear ambos buffers (bloqueados y conectados)
        if((buffer[i].blocked_processes == NULL && buffer[i].blocked_size != 0) ||
            (buffer[i].connected_processes == NULL && buffer[i].connected_size != 0)){
            sems_dump_free(buffer, i+1);
            release(&sem_manager.lock);
            return 0;
        }

        // Guardamos el nombre del semaforo (si es que tiene uno)
        if(curr_sem->name != NULL){
            size = strlen(curr_sem->name);
            buffer[i].name = mm_alloc((size+1) * sizeof(char));
            if(buffer[i].name == NULL){
                sems_dump_free(buffer, i+1);
                release(&sem_manager.lock);
                return 0;
            }
            strcpy(buffer[i].name, curr_sem->name);
        }

        queueADT_toBegin(curr_sem->blocked_processes);
        for(uint32_t j = 0; j < buffer[i].blocked_size && queueADT_hasNext(curr_sem->blocked_processes); j++){
            pid_p = (uint64_t *)queueADT_next(curr_sem->blocked_processes);
            pid = *pid_p;
            (buffer[i].blocked_processes)[j] = pid;
        }

        orderListADT_toBegin(curr_sem->connected_processes);
        for(uint32_t j = 0; j < buffer[i].connected_size && orderListADT_hasNext(curr_sem->connected_processes); j++){
            pid_p = (uint64_t *)orderListADT_next(curr_sem->connected_processes);
            pid = *pid_p;
            (buffer[i].connected_processes)[j] = pid;
        }
    }

    for(uint32_t i = buff_size; i < length; i++){
        buffer[i].name = NULL;
        buffer[i].blocked_processes = NULL;
        buffer[i].connected_processes = NULL;
        buffer[i].connected_size = 0;
        buffer[i].blocked_size = 0;
    }

    // Se libera el lock para que otro proceso pueda acceder a la seccion critica
    release(&sem_manager.lock);
    return buff_size;
}


//----------------------------------------------------------------------
// sems_dump_free: Libera los recursos generados en sems_dump
//----------------------------------------------------------------------
// Argumentos:
//  buffer: arreglo a liberar
//  length: tamaño del buffer
//----------------------------------------------------------------------
// Retorno:
//----------------------------------------------------------------------
void sems_dump_free(sem_dump_t * buffer, uint32_t length){
    if(buffer == NULL || sem_manager.semaphores == NULL){
        return;
    }
    for(uint32_t j = 0; j < length; j++){
        buffer[j].blocked_size = 0;
        buffer[j].connected_size = 0;
        if(buffer[j].name != NULL){
            mm_free(buffer[j].name);
        }
        if(buffer[j].blocked_processes != NULL){
            mm_free(buffer[j].blocked_processes);
        }
        if(buffer[j].connected_processes != NULL){
            mm_free(buffer[j].connected_processes);
        }
    }
}