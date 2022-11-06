// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <libc.h>
#include <stdint.h>
#include <stddef.h>

#define MIN_PHYLO       2
#define MAX_PHYLO       15
#define INITIAL_NUMBER  5
#define CHUNKS          5
#define IS_A(c)         ((c) == 'a' || (c) == 'A')
#define IS_R(c)         ((c) == 'r' || (c) == 'R')
#define IS_Q(c)         ((c) == 'q' || (c) == 'Q')
#define IS_EVEN(x)      ((x) % 2 == 0)

typedef enum { EATING = 0, THINKING, HUNGRY } phylo_states;

typedef struct phylo {
    sem_t chopstick;            // Palito que pertenece al filosofo
    phylo_states phylo_state;   // Estado del filosofo
    uint64_t id;                // Id del filosofo
    uint64_t pid;               // PID del proceso que ejecuta al filosofo
} phylo_t;

static sem_t aux_sem;           // Necesario para el caso de eliminar un filosofo teniendo una cantidad par de filosofos
static sem_t print_mutex;       // Mutex para impedir que se imprima cuando se esta en un estado inconsistente
static phylo_t * phylos;        // Los filosofos

static uint64_t amount = INITIAL_NUMBER;
static uint64_t size = INITIAL_NUMBER;


#define FIRST_MSG   "Estado de los filosofos:\n(C: comiendo, P: pensando, H: hambriento)\n-------------------------------------------------------------------------------\n"
#define LAST_MSG    "\n-------------------------------------------------------------------------------\nPulse la tecla A si desea agregar un filosofo, o R si desea eliminar uno.\nPulse la tecla Q si desea finalizar el programa.\n\n"
#define RESIZE_ERR  "No se pudo agregar un nuevo filosofo.\n"
#define REMOVE_ERR  "Ya no se pueden eliminar mas filosofos.\n"
#define ADD_ERR     "Ya no se pueden agregar mas filosofos.\n"

#define ADDING_MSG  "\nAgregando filosofo...\n"
#define ADDED_MSG   "Agregado!\n\n\n"
#define REMOVING_MSG "\nEliminando filosofo...\n"
#define REMOVED_MSG "Eliminado!\n\n\n"

static void print_phylo_state(uint64_t arg_c, const char ** arg_v);
static void philosophers(uint64_t arg_c, const char ** arg_v);
static int resize(void);
static void close_resources(uint64_t print_process_pid);



void phylo(uint64_t arg_c, const char ** arg_v) {

    if(arg_c != 0){
        throw_error("Error: el programa no recibe argumentos.");
    }

    uint64_t print_process_pid;
    uint64_t i = 0;
    unsigned int str_size;

    amount = INITIAL_NUMBER;
    size = INITIAL_NUMBER;

    // Creamos los filosofos
    phylos = malloc(size * sizeof(phylo_t));
    if (phylos == NULL) {
        p_error("Error - Creando el arreglo de filosofos.\n");
        return;
    }

    // Inicializamos los semaforos y los ids
    for (i = 0; i < amount; i++) {
        phylos[i].id = i;
        if ((phylos[i].chopstick = sem_init(NULL, 1)) == NULL) {
            p_error("Error - Creando un palito.\n");
            for (uint64_t j = 0; j < i; j++) {
                sem_close(phylos[j].chopstick);
            }
            free(phylos);
            return;
        }
    }

    // Creamos el mutex de impresion
    if ((print_mutex = sem_init(NULL, 1)) == NULL) {
        p_error("Error - Creando el mutex de impresion.\n");
        for (uint64_t j = 0; j < amount; j++) {
            sem_close(phylos[j].chopstick);
        }
        free(phylos);
        return;
    }

    // Creamos el semaforo auxiliar (inicializado en 0)
    if ((aux_sem = sem_init(NULL, 0)) == NULL) {
        p_error("Error - Creando el semaforo auxiliar.\n");
        for (uint64_t j = 0; j < amount; j++) {
            sem_close(phylos[j].chopstick);
        }
        free(phylos);
        sem_close(print_mutex);
        return;
    }


    // Creamos los filosofos
    for (i = 0; i < amount; i++) {
        char arg[21];
        char * args[] = {arg};
        //print_number(i);
        number_to_string(i, arg);
        //print_string(arg);
        executable_t aux = {"philosopher", &philosophers, 1, args, 0, NULL};
        if ((phylos[i].pid = sys_exec(&aux)) == -1) {
            p_error("Error - Creando un filosofo.\n");
            for (uint64_t j = 0; j < i; j++) {
                terminate_process(phylos[j].pid);
            }
            for (uint64_t j = 0; j < i; j++) {
                sem_close(phylos[j].chopstick);
            }
            sem_close(aux_sem);
            sem_close(print_mutex);
            free(phylos);
            return;
        }
    }

    // Ejecutamos el proceso de impresion
    executable_t print_exec = {"print_philosophers", &print_phylo_state, 0, NULL, 0, NULL};
    if ((print_process_pid = sys_exec(&print_exec)) == -1) {
        p_error("Error - Creando el proceso de impresion.\n");
        for (uint64_t j = 0; j < amount; j++) {
            terminate_process(phylos[j].pid);
        }
        for (uint64_t j = 0; j < amount; j++) {
            sem_close(phylos[j].chopstick);
        }
        sem_close(aux_sem);
        sem_close(print_mutex);
        free(phylos);
        return;
    }


    // El proceso padre sera el encargado de ir agregando o quitando filosofos
    char letter;

    while ((letter = get_char()) != 0) {

        if (IS_Q(letter)) {
            close_resources(print_process_pid);
            return;
        }


        // Agrega filosofo
        else if (IS_A(letter)) {
            // Verificamos que no se supere el limite maximo de filosofos
            if(amount == MAX_PHYLO){
                str_size = strlen(ADD_ERR);
                write(STDOUT, ADD_ERR, str_size);
            }

            // Si es necesario, agrandamos el arreglo de filosofos
            else if (amount == size && resize() == -1) {
                str_size = strlen(RESIZE_ERR);
                write(STDOUT, RESIZE_ERR, str_size);
            } else {

                // Caso 1: Cantidad impar de filosofos
                if (!IS_EVEN(amount)) {

                    // Tomamos el ultimo palito
                    sem_wait(phylos[amount - 1].chopstick);

                    sem_wait(print_mutex);

                    str_size = strlen(ADDING_MSG);
                    write(STDOUT, ADDING_MSG, str_size);

                    phylos[amount].id = amount;

                    // Agregamos el nuevo filosofo
                    if ((phylos[amount].chopstick = sem_init(NULL, 1)) == NULL) {
                        p_error("Error - Agregando un filosofo.\n");
                        sem_post(print_mutex);
                        sem_post(phylos[amount - 1].chopstick);
                    } else {

                        char arg[21];
                        char * args[] = {arg};
                        number_to_string(amount, arg);
                        executable_t aux = {"philosopher", &philosophers, 1, args, 0, NULL};
                        if ((phylos[amount].pid = sys_exec(&aux)) == -1) {
                            p_error("Error - Agregando un filosofo.\n");
                            sem_close(phylos[amount].chopstick);
                            sem_post(print_mutex);
                            sem_post(phylos[amount - 1].chopstick);
                        } else {

                            str_size = strlen(ADDED_MSG);
                            write(STDOUT, ADDED_MSG, str_size);

                            amount++;

                            sem_post(print_mutex);

                            // Liberamos el ultimo (ahora anteultimo) palito
                            sem_post(phylos[amount - 2].chopstick);
                        }
                    }
                }

                // Caso 2: Cantidad par de filosofos
                else {

                    // Esperamos al ultimo proceso (que es impar)
                    sem_wait(aux_sem);

                    sem_wait(print_mutex);

                    str_size = strlen(ADDING_MSG);
                    write(STDOUT, ADDING_MSG, str_size);

                    // Agregamos el nuevo filosofo
                    if ((phylos[amount].chopstick = sem_init(NULL, 0)) == NULL) {
                        p_error("Error - Agregando un filosofo.\n");
                        sem_post(print_mutex);
                        sem_post(aux_sem);
                        sem_post(phylos[0].chopstick);
                    } else {

                        phylos[amount].id = amount;

                        // Creamos el nuevo filosofo
                        char arg[21];
                        char * args[] = {arg};
                        number_to_string(amount, arg);
                        executable_t aux = {"philosopher", &philosophers, 1, args, 0, NULL};
                        if ((phylos[amount].pid = sys_exec(&aux)) == -1) {
                            p_error("Error - Agregando un filosofo.\n");
                            sem_close(phylos[amount].chopstick);
                            sem_post(print_mutex);
                            sem_post(aux_sem);
                            sem_post(phylos[0].chopstick);
                        } else {

                            str_size = strlen(ADDED_MSG);
                            write(STDOUT, ADDED_MSG, str_size);

                            amount++;

                            // Liberamos el primer palito y el auxiliar
                            sem_post(print_mutex);
                            sem_post(aux_sem);
                            sem_post(phylos[0].chopstick);
                        }
                    }
                }

            }

        }

        // Eliminar un filosofo
        else if (IS_R(letter)) {
            // Verificamos que no supere el limite inferior de filosofos
            if (amount == MIN_PHYLO) {
                str_size = strlen(REMOVE_ERR);
                write(STDOUT, REMOVE_ERR, str_size);
            } else {

                // Caso 1: Cantidad impar de filosofos
                if (!IS_EVEN(amount)) {
                    // Esperamos a que el anteultimo proceso quiera tomar el ultimo palito
                    sem_wait(aux_sem);

                    // Tomamos el ultimo palito (el que se va a eliminar)
                    sem_wait(phylos[amount - 1].chopstick);

                    sem_wait(print_mutex);

                    str_size = strlen(REMOVING_MSG);
                    write(STDOUT, REMOVING_MSG, str_size);

                    // Eliminamos el ultimo filosofo
                    terminate_process(phylos[amount - 1].pid);

                    // Eliminamos el ultimo palito
                    sem_close(phylos[amount - 1].chopstick);

                    amount--;

                    str_size = strlen(REMOVED_MSG);
                    write(STDOUT, REMOVED_MSG, str_size);

                    // Liberamos el anteultimo (ahora ultimo) proceso
                    sem_post(print_mutex);
                    sem_post(aux_sem);
                }

                // Caso 2: Cantidad par de filosofos
                else {

                    // Esperamos al anteultimo palito
                    sem_wait(phylos[amount-2].chopstick);

                    sem_wait(aux_sem);

                    sem_wait(print_mutex);

                    str_size = strlen(REMOVING_MSG);
                    write(STDOUT, REMOVING_MSG, str_size);

                    // Eliminamos el ultimo filosofo
                    terminate_process(phylos[amount-1].pid);

                    // Eliminamos el ultimo palito
                    sem_close(phylos[amount-1].chopstick);

                    amount--;

                    str_size = strlen(REMOVED_MSG);
                    write(STDOUT, REMOVED_MSG, str_size);

                    sem_post(print_mutex);

                    // Liberamos el primer palito que lo ocupo el filosofo eliminado
                    sem_post(phylos[0].chopstick);

                    // Liberamos el anteultimo (ahora) palito
                    sem_post(phylos[amount-1].chopstick);

                }

            }

        }

        sleep(1);
    }

    close_resources(print_process_pid);
}



static void philosophers(uint64_t arg_c, const char ** arg_v){

    uint64_t id = string_to_number(arg_v[0], NULL);

    while(1){
        phylos[id].phylo_state = HUNGRY;

        // Los filosofos pares...
        if(IS_EVEN(id)){

            // Toman el palito de la derecha
            sem_wait(phylos[id].chopstick);

            // Toman el palito de la izquierda
            sem_wait(phylos[(id + 1) % amount].chopstick);

        }
            // Mientras que, los filosofos impares...
        else {

            // Para que funcione el borrado de filosofos cuando hay una cantidad impar de filosofos, debemos usar un semaforo auxiliar
            if(id == amount-2){
                sem_post(aux_sem);
                sleep(1);
                sem_wait(aux_sem);
            }

            // Toma el palito de la izquierda
            sem_wait(phylos[(id + 1) % amount].chopstick);

            // Para que funcione el borrado de filosofos cuando hay una cantidad par de filosofos, debemos usar un semaforo auxiliar
            if(id == amount-1){
                sem_post(aux_sem);
                sleep(1);
                sem_wait(aux_sem);
            }

            // Toma el palito de la derecha
            sem_wait(phylos[id].chopstick);
        }

        // Tomamos el mutex para que no pueda imprimir cuando se cambia el estado
        sem_wait(print_mutex);

        phylos[id].phylo_state = EATING;

        sem_post(print_mutex);

        // Comiendo
        sleep(1);

        // Tomamos el mutex para que no pueda imprimir cuando se cambia el estado
        sem_wait(print_mutex);

        phylos[id].phylo_state = THINKING;

        sem_post(print_mutex);

        // Se liberan los palitos que se usaron para comer
        sem_post(phylos[id].chopstick);
        sem_post(phylos[(id + 1) % amount].chopstick);


        // Pensando
        sleep(2);
    }
}



static void print_phylo_state(uint64_t arg_c, const char ** arg_v){
    unsigned int size = 0;

    while(1){

        sem_wait(print_mutex);

        size = strlen(FIRST_MSG);
        write(STDOUT, FIRST_MSG, size);

        for(uint64_t i = 0; i < amount; i++){
            switch (phylos[i].phylo_state) {
                case EATING:
                    write(STDOUT, "C\t", 2);
                    break;
                case THINKING:
                    write(STDOUT, "P\t", 2);
                    break;
                default:
                    write(STDOUT, "H\t", 2);
                    break;
            }
        }

        size = strlen(LAST_MSG);
        write(STDOUT, LAST_MSG, size);

        sem_post(print_mutex);

        sleep(3);
    }
}


// Agranda el arreglo de filosofos
// Si hubo algun error, devuelve -1 y deja todo como estaba
static int resize(void){
    uint64_t i = 0;

    phylo_t * aux_phylos = malloc(size * sizeof(phylo_t));
    if(aux_phylos == NULL){
        return -1;
    }

    for(i = 0; i < amount; i++){
        aux_phylos[i] = phylos[i];
    }

    free(phylos);
    phylos = malloc((size + CHUNKS) * sizeof(phylo_t));
    if(phylos == NULL){
        phylos = aux_phylos;
        return -1;
    }

    for (i = 0; i < amount; i++) {
        phylos[i] = aux_phylos[i];
    }
    free(aux_phylos);

    size += CHUNKS;
    return 0;
}



// Cierra todos los recursos utilizados (procesos, semaforos, arreglos, etc)
static void close_resources(uint64_t print_process_pid){

    uint64_t i = 0;

    // Cerramos los filosofos
    for(i = 0; i < amount; i++){
        if(terminate_process(phylos[i].pid) == -1){
            p_error("Error - Cerrando uno de los filosofos.\n");
        }
    }

    // Cerramos los semaforos
    for(i = 0; i < amount; i++){
        if(sem_close(phylos[i].chopstick) == -1){
            p_error("Error - Cerrando un semaforo palito.\n");
        }
    }

    free(phylos);

    // Cerramos el proceso de impresion
    if (terminate_process(print_process_pid) == -1){
        p_error("Error - Cerrando el proceso de impresion.\n");
    }

    // Cerramos los semaforos auxiliares
    if(sem_close(print_mutex) == -1){
        p_error("Error - Cerrando el semaforo de impresion.\n");
    }

    if(sem_close(aux_sem) == -1){
        p_error("Error - Cerrando el semaforo auxiliar.\n");
    }
}