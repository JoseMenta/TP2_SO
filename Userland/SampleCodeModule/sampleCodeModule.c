/* sampleCodeModule.c */
#include <libc.h>
#include <tests.h>
#include <programs.h>
#include <bash.h>
#include <libc.h>


int main(uint64_t arg_c, char** arg_v) {
    print_number(arg_c,WHITE);
    print_string(arg_v[0],WHITE);
    while (1);
//    uint64_t a = 0;
//    while (1){
//        print_number(a++,WHITE);
//        print_string(" ",WHITE);
//    }
    return 0;

}