#include <libc.h>
#include <stdint.h>
#include <stddef.h>
#include <built_in.h>


void garbage_collector(uint64_t arg_c, const char ** arg_v){
    if(arg_c != 1){
        throw_error("Error: El programa debe recibir la cantidad de bytes a alocar.\n");
    }

    uint64_t bytes = string_to_number(arg_v[0], NULL);

    void * pointer = malloc(bytes);

    if(pointer == NULL){
        p_error("No se pudo alocar memoria.\n");
    }
}