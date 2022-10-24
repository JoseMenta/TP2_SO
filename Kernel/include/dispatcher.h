#ifndef TP2_SO_DISPATCHER_H
#define TP2_SO_DISPATCHER_H

#include <mm.h>
#include <stdlib.h>

//No es necesario ponerlo si no se va a usar desde otro programa C
//void * allocate_new_process_stack();

void create_new_process_context(uint64_t arg_c, char** arg_v, void* rip, uint64_t* pcb_bp, uint64_t* pcb_sp);
void
#endif //TP2_SO_DISPATCHER_H
