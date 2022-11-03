#include <primos.h>
#include <libc.h>
#include "../include/libc.h"
//---------------------------------------------------------------------------------
// primos: imprime a los numeros primos
//---------------------------------------------------------------------------------
// Argumentos:
//  arg_c: cantidad de argumentos del programa (0)
//  arg_v: arreglo con los strings de los argumentos
//---------------------------------------------------------------------------------
static uint8_t is_prime(uint64_t number){
    for(int i=FIRST_PRIME; i*i <= number; i++){
        if(number%i == 0)
            return 0;
    }
    return 1;
}

void primos(uint64_t arg_c, const char ** arg_v){
    if(arg_c!=0){
        throw_error("Error: primos el programa no recibe argumentos");
    }
    char c[2];
    c[1]='\0';
    for(int i=0; (c[0] = get_char()) != '\n'; i++){
        print_string(c);
    }
   for(uint64_t i=FIRST_PRIME; i<=10 ; i++){
        if(is_prime(i)){
            //TODO: revisar cuando se pasan valores mayores a 4, pues se reinician los ticks en el scheduler
           pause_ticks(1);
//            yield();
           print_number(i);
           print_string(" ");
        }
   }
//   exit();
    return;
}


