#ifndef TP2_SO_DISPATCHER_H
#define TP2_SO_DISPATCHER_H



#include <stdint.h>
#define STACK_SIZE 0x1000               // 4KB

int create_new_process_context(uint64_t arg_c, char** arg_v, void* rip, void** pcb_bp, void** pcb_sp);
void free_process_stack(void* bp);
#endif //TP2_SO_DISPATCHER_H
