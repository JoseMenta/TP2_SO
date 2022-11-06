#include <stdint.h>
#include <stddef.h>
#include <libc.h>
#include <io.h>

#include "../include/libc.h"
#define ERR_SIZE    40

void cat_fn(char * buffer, unsigned int buff_pos){
    write(STDOUT, buffer, buff_pos);
}

void cat(uint64_t arg_c, const char ** arg_v){
    if(arg_c != 0){
        throw_error("Error: el programa no recibe argumentos.");
    }

    char * err_buffer = malloc(ERR_SIZE * sizeof(char));
    if(err_buffer == NULL){
        throw_error("Error: no se pudo crear el buffer para errores.");
    }

    if(io_logic(cat_fn, &err_buffer, ERR_SIZE) == -1){
        write(STDERR, err_buffer, ERR_SIZE);
    }
    free(err_buffer);
}



