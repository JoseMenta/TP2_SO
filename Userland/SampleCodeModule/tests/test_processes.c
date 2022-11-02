#include <stdio.h>
#include <test_util.h>
#include <libc.h>

//TODO: sacar
#include "../include/test_util.h"
#include "../include/libc.h"

enum State {RUNNING, BLOCKED_1, KILLED};

typedef struct P_rq{
    int32_t pid;
    enum State state;
}p_rq;

int64_t test_processes(uint64_t argc, char *argv[]){
    uint8_t rq;
    uint8_t alive = 0;
    uint8_t action;
    uint64_t max_processes;
    char * argvAux[] = {0};

    if (argc != 1) return -1;

    if ((max_processes = satoi(argv[0])) <= 0) return -1;
    print_string(argv[0]);
    p_rq p_rqs[max_processes];

    while (1){

        // Create max_processes processes
        for(rq = 0; rq < max_processes; rq++){
            executable_t exec = {"endless_loop",&endless_loop,0,argvAux,0};
            p_rqs[rq].pid = sys_exec(&exec);
//            p_rqs[rq].pid = my_create_process("endless_loop", 0, argvAux);

            if (p_rqs[rq].pid == -1){
                print_string("test_processes: ERROR creating process\n");
//                printf("test_processes: ERROR creating process\n");
                return -1;
            }else{
                p_rqs[rq].state = RUNNING;
                alive++;
            }
        }

        // Randomly kills, blocks or unblocks processes until every one has been killed
        while (alive > 0){

            for(rq = 0; rq < max_processes; rq++){
                action = GetUniform(100) % 2;

                switch(action){
                    case 0:
                        if (p_rqs[rq].state == RUNNING || p_rqs[rq].state == BLOCKED_1){
//                            if (my_kill(p_rqs[rq].pid) == -1){
                            if (terminate_process(p_rqs[rq].pid) == -1){
                                print_string("test_processes: ERROR killing process\n");
//                                printf("test_processes: ERROR killing process\n");
                                return -1;
                            }
                            waitpid(p_rqs[rq].pid);//Lo sacamos para que no quede como zombie
                            p_rqs[rq].state = KILLED;
                            alive--;
                        }
                        break;

                    case 1:
                        if (p_rqs[rq].state == RUNNING){
//                            if(my_block(p_rqs[rq].pid) == -1){
                            if(block_process(p_rqs[rq].pid) == -1){
                                print_string("test_processes: ERROR blocking process: ");
                                print_number(p_rqs[rq].pid);
                                print_string("\n");
//                                printf("test_processes: ERROR blocking process\n");
                                return -1;
                            }
                            p_rqs[rq].state = BLOCKED_1;
                        }
                        break;
                }
            }

            // Randomly unblocks processes
            for(rq = 0; rq < max_processes; rq++)
                if (p_rqs[rq].state == BLOCKED_1 && GetUniform(100) % 2){
//                    if(my_unblock(p_rqs[rq].pid) == -1){
                    if(unblock_process(p_rqs[rq].pid)== -1){
                        print_string("test_processes: ERROR unblocking process\n");
//                        printf("test_processes: ERROR unblocking process\n");
                        return -1;
                    }
                    p_rqs[rq].state = RUNNING;
                }
        }
    }
}