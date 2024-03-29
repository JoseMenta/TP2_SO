// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <stdint.h>
#include <stdio.h>
#include <libc.h>
#include <test_util.h>

#define MINOR_WAIT 4000000 // TODO: Change this value to prevent a process from flooding the screen
#define WAIT      240000000 // TODO: Change this value to make the wait long enough to see theese processes beeing run at least twice

#define TOTAL_PROCESSES 3
#define LOWEST 4 //TODO: Change as required
#define MEDIUM 2 //TODO: Change as required
#define HIGHEST 0 //TODO: Change as required

int64_t prio[TOTAL_PROCESSES] = {LOWEST, MEDIUM, HIGHEST};

void test_prio(){
    int64_t pids[TOTAL_PROCESSES];
    char *argv[] = {0};
    uint64_t i;

    for(i = 0; i < TOTAL_PROCESSES; i++) {
        executable_t exec = {"endless_loop_print",&endless_loop_print,0,argv,1, NULL};
        pids[i] = sys_exec(&exec);
    }
    bussy_wait(WAIT);
    print_string("\nCHANGING PRIORITIES...\n");

    for(i = 0; i < TOTAL_PROCESSES; i++) {
        nice(pids[i],prio[i]);

    }
    bussy_wait(WAIT);
    bussy_wait(MINOR_WAIT);
    bussy_wait(MINOR_WAIT);
    bussy_wait(MINOR_WAIT);
    print_string("\nBLOCKING...\n");


    for(i = 0; i < TOTAL_PROCESSES; i++) {
        block_process(pids[i]);

    }
    print_string("CHANGING PRIORITIES WHILE BLOCKED...\n");


    for(i = 0; i < TOTAL_PROCESSES; i++) {
        nice(pids[i],MEDIUM);

    }
    print_string("UNBLOCKING...\n");


    for(i = 0; i < TOTAL_PROCESSES; i++) {
        unblock_process(pids[i]);

    }

    bussy_wait(WAIT);
    print_string("\nKILLING...\n");


    for(i = 0; i < TOTAL_PROCESSES; i++) {
        terminate_process(pids[i]);
        waitpid(pids[i]);

    }
}