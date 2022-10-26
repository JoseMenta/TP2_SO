#include <dispatcher.h>
#include "../include/mm.h"
#define STACK_SIZE 0x1000               // 4KB

// -----------------------------------------------------------------------------
// allocate_new_process_stack
// -----------------------------------------------------------------------------
// Crea el espacio de memoria necesario para el stack
// -----------------------------------------------------------------------------
// Retorno:
//      ERROR: Devuelve NULL
//      EXITO: Devuelve la dirección del BP/SP inicial
// -----------------------------------------------------------------------------
void* allocate_new_process_stack(){
    void * stack;
    if((stack = mm_alloc(STACK_SIZE)) == NULL){
        return NULL;
    }

    // BP: Inicio del stack + tamaño del stack - 1
//    return stack + STACK_SIZE - 1;
    return stack+STACK_SIZE;//nos pasamos en 1, total el primer push lo va a arreglar
}

void free_process_stack(void* bp){
    mm_free(bp-STACK_SIZE);
}