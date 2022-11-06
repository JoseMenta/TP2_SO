// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <fibonacci.h>
#include <libc.h>
//---------------------------------------------------------------------------------
// fibonacci: Imprime la sucesion de numeros de fibonacci
//---------------------------------------------------------------------------------
// Argumentos:
//  arg_c: cantidad de argumentos del programa (0)
//  arg_v: arreglo con los strings de los argumentos
//---------------------------------------------------------------------------------
void fibonacci(uint64_t arg_c, const char ** arg_v){
    if(arg_c!=0){
        throw_error("Error: El programa no recibe argumentos");
    }
    uint64_t num_1 = 0;
    uint64_t num_2 = 1;
    print_number(num_1);
    print_string("\n");
    print_number(num_2);
    print_string("\n");
//    while(1){
//        pause_ticks(5);
//        uint64_t aux = num_1 + num_2;
//        num_1 = num_2;
//        num_2 = aux;
//        print_number(aux);
//        print_string("\n");
//    }
    sys_exit();
}