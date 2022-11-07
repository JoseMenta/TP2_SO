#include <dispatcher.h>
#include <mm.h>
#include <stddef.h>



// -----------------------------------------------------------------------------
// allocate_new_process_stack
// -----------------------------------------------------------------------------
// Crea el espacio de memoria necesario para el stack
// -----------------------------------------------------------------------------
// Retorno:
//      ERROR: Devuelve NULL
//      EXITO: Devuelve la dirección del BP/SP inicial
// -----------------------------------------------------------------------------
void* allocate_new_process_stack() {
    void *stack;
    if ((stack = mm_alloc(STACK_SIZE)) == NULL) {
        return NULL;
    }

    //nos pasamos en 1, total el primer push lo va a arreglar
    return (uint8_t *)stack+STACK_SIZE;
}

void free_process_stack(void* bp){
    mm_free((uint8_t*)bp-STACK_SIZE);
}