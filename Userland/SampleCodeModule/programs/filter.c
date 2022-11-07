#include <stdint.h>
#include <stddef.h>
#include <libc.h>
#include <io.h>

#define ERR_SIZE    40

#define IS_VOWEL(c) ((c) == 'a' || (c) == 'e' || (c) == 'i' || (c) == 'o' || (c) == 'u' || (c) == 'A' || (c) == 'E' || (c) == 'I' || (c) == 'O' || (c) == 'U')

void filter_fn(char * buffer, unsigned int buff_pos){
    for(unsigned int i = 0; i < buff_pos; i++){
        if(!IS_VOWEL(buffer[i])){
            write(STDOUT, &buffer[i], 1);
        }
    }
}

//---------------------------------------------------------------------------------
// filter: Filtra las vocales del input.
//---------------------------------------------------------------------------------
// Argumentos:
//  arg_c: cantidad de argumentos del programa
//  arg_v: arreglo con los strings de los argumentos
//---------------------------------------------------------------------------------
void filter(uint64_t arg_c, const char ** arg_v){
    if(arg_c != 0){
        throw_error("Error: el programa no recibe argumentos.");
    }

    char * err_buffer = malloc(ERR_SIZE * sizeof(char));
    if(err_buffer == NULL){
        throw_error("Error: no se pudo crear el buffer para errores.");
    }

    if(io_logic(filter_fn, &err_buffer, ERR_SIZE) == -1){
        write(STDERR, err_buffer, ERR_SIZE);
    }
    free(err_buffer);
}