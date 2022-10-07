
#ifndef TP2_SO_MM_H
#define TP2_SO_MM_H
#include <stdint.h>
void* mm_alloc(uint32_t wanted_size);
void mm_free(void* p);
uint64_t get_allocated_blocks();
uint64_t get_allocated_bytes();
uint64_t get_free_bytes();
#endif //TP2_SO_MM_H
