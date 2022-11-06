// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <printmem.h>
#include <libc.h>

uint64_t analyze_string(const char * str);
//---------------------------------------------------------------------------------
// printmem: imprime 32 bytes de memoria a partir de una direccion que se pasa como argumento
//---------------------------------------------------------------------------------
// Argumentos:
//  arg_c: cantidad de argumentos del programa (debe ser 1)
//  arg_v: arreglo con los strings de los argumentos (debe incluir por lo menos a la direccion inicial)
//---------------------------------------------------------------------------------
//limit = 0xFFFFFFFD9
void printmem(uint64_t arg_c, const char** arg_v){
    if(arg_c!=1){
        throw_error("ERROR: El programa debe recibir exactamente 1 argumento");
    }
    uint64_t init_dir = analyze_string(arg_v[0]);
    uint8_t mem_arr[32] = {0};
    sys_mem(init_dir, mem_arr);
    char str[21] = {0};                                                             // 2^64 tiene 20 digitos mas el "\0"
    print_string("Datos almacenados a partir de la direccion 0x");
    print_string(to_hex(str, init_dir));
    print_string(":\n");
    for(int i = 0; i < MAX_MEMORY_SIZE; i++){
        print_string("0x");
        print_string(to_hex(str, init_dir + i));
        print_string(": ");
        print_string("0x");
        print_string(to_hex(str, mem_arr[i]));
        (i%2==0)? print_string("\t"): print_string("\n");
    }
    print_string("\n");
    sys_exit();
}

//-----------------------------------------------------------------------
// analyze_string: Procesa el String de parametro
//-----------------------------------------------------------------------
// Argumentos:
//  - str: parametro
//-----------------------------------------------------------------------
// Retorna:
//  - el puntero que reprecenta el string
//-----------------------------------------------------------------------
uint64_t analyze_string(const char * str){
    uint64_t val = 0;                               // lo paso a entero para poder desreferenciar
    uint8_t len=0;
    for(; str[len] != '\0'; len++);
    if(len < 3 || str[0] != '0' || str[1] != 'x'){
        throw_error("ERROR: El string ingresado no es un formato de direccion correcta");
    }
    uint64_t mult=1;
    for(uint8_t i=len-1; i>1; i--, mult*=16){
        char num = str[i];
        if (num >= '0' && num <= '9')
            val += (num - '0') * mult;
        else if (num >= 'A' && num <='F')
            val += (num - 'A' + 10) * mult;
        else{
            throw_error("ERROR: El string ingresado no es un formato de direccion correcta");
        }
    }
    return val;
}