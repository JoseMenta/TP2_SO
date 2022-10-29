/* sampleCodeModule.c */
#include <libc.h>
#include <tests.h>
#include <programs.h>
#include <bash.h>
#include <libc.h>
#include <primos.h>
#include "./include/primos.h"
#include "./include/libc.h"

void aa(){
    for(int i = 0; i<1000; i++){
        print_string("a ",WHITE);
        pause_ticks(1);
    }
    exit();
}
int main(uint64_t arg_c, char** arg_v) {
    print_number(arg_c,WHITE);
    print_string(arg_v[0],WHITE);
    executable_t exec;
    exec.start = &primos;
    exec.name = "primos";
    exec.arg_c = 0;
    exec.arg_v = NULL;
    uint64_t  pid = sys_exec(&exec);
//    nice(pid,4);
    executable_t exec2;
    exec2.name = "a";
    exec2.start = &aa;
    exec2.arg_c = 0;
    exec2.arg_v = NULL;
    uint64_t pid2 = sys_exec(&exec2);
//    nice(pid2,0);
//    pause_ticks(3);
//    pause_ticks(3);
//    pause_ticks(3);
//    block_process(pid);
//    print_number(getpid(),WHITE);
//    pause_ticks(3);
//    pause_ticks(3);
//    pause_ticks(3);
//    pause_ticks(3);
//    pause_ticks(3);
//    pause_ticks(3);
//    unblock_process(pid);
    waitpid(pid);
    waitpid(pid2);
    while (1){
        print_number(getpid(),WHITE);
        pause_ticks(1);
    }

//    uint64_t a = 0;
//    while (1){
//        print_number(a++,WHITE);
//        print_string(" ",WHITE);
//    }
    return 0;

}