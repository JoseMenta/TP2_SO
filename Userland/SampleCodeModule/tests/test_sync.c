#include <stdint.h>
#include <stdio.h>
#include <libc.h>
#include <test_util.h>

//TODO: sacar
#include "../include/libc.h"
#include "../include/test_util.h"

#define SEM_ID "/sem"
#define TOTAL_PAIR_PROCESSES 5

int64_t global;  //shared memory

void slowInc(int64_t *p, int64_t inc){
    uint64_t aux = *p;
    for(int i = 0; i<40000; i++);
//    yield();
//    my_yield(); //This makes the race condition highly probable
    aux += inc;
    *p = aux;
}

uint64_t my_process_inc(uint64_t argc, char *argv[]){
    uint64_t n;
    int8_t inc;
    int8_t use_sem;

    if (argc != 3) return -1;

    if ((n = satoi(argv[0])) <= 0) return -1;
    if ((inc = satoi(argv[1])) == 0) return -1;
    if ((use_sem = satoi(argv[2])) < 0) return -1;
    sem_t sem;
    if (use_sem)
//        if (!my_sem_open(SEM_ID, 1)){
        if((sem=sem_open(SEM_ID,1, O_NULL))==NULL){
            print_string("test_sync: ERROR opening semaphore\n");
//            printf("test_sync: ERROR opening semaphore\n");
            return -1;
        }

    uint64_t i;
    for (i = 0; i < n; i++){
        if (use_sem) {
            sem_wait(sem);
//            my_sem_wait(SEM_ID);
        }
        slowInc(&global, inc);
        if (use_sem){
            sem_post(sem);
//            my_sem_post(SEM_ID);
        }
    }

    if (use_sem){
        sem_close(sem);
//        my_sem_close(SEM_ID);
    }

    return 0;
}

uint64_t test_sync(uint64_t argc, char *argv[]){ //{n, use_sem, 0}
    uint64_t pids[2 * TOTAL_PAIR_PROCESSES];

    if (argc != 2) return -1;

    char * argvDec[] = {argv[0], "-1", argv[1], NULL};
    char * argvInc[] = {argv[0], "1", argv[1], NULL};

    sem_t sem;
    if((sem = sem_init(SEM_ID, 1)) == NULL){
        print_string("test_sync: ERROR opening semaphore\n");
        return -1;
    }

    global = 1398480;

    uint64_t i;
    for(i = 0; i < TOTAL_PAIR_PROCESSES; i++){
        executable_t aux={"my_process_inc",&my_process_inc,3,argvDec,0};
        pids[i] = sys_exec(&aux);
//        pids[i] = my_create_process("my_process_inc", 3, argvDec);
        aux.arg_v = argvInc;
        pids[i+TOTAL_PAIR_PROCESSES] = sys_exec(&aux);
//        pids[i + TOTAL_PAIR_PROCESSES] = my_create_process("my_process_inc", 3, argvInc);
    }

    for(i = 0; i < TOTAL_PAIR_PROCESSES; i++){
        waitpid(pids[i]);
//        my_wait(pids[i]);
        waitpid(pids[i+TOTAL_PAIR_PROCESSES]);
//        my_wait(pids[i + TOTAL_PAIR_PROCESSES]);
    }

    sem_close(sem);

    print_string("Final value: ");
//    if(global<0){
//        print_string("Negativo!",WHITE);
//        global = -global;
//    }
    print_number(global);
    print_string("\n");
//    printf("Final value: %d\n", global);

    return 0;
}