
#ifndef TP2_SO_HASHADT_H
#define TP2_SO_HASHADT_H

#include <stdint.h>
#include <orderListADT.h>

#define SIZE 10

typedef uint64_t (* prehash_function) (void * elem);

typedef struct HashCDT * HashADT;

HashADT new_hashADT(prehash_function pre_hash, compare_function cmp);
void * hashADT_delete(HashADT table, void * elem);
int8_t hashADT_add(HashADT table, void * elem);
void * hashADT_edit(HashADT table, void * prev_elem, void * new_elem, int8_t * add_response);
void * hashADT_get(HashADT table, void * elem_id);
uint8_t hashADT_is_empty(HashADT table);
uint32_t hashADT_size(HashADT table);
void free_hashADT(HashADT table);
void hashADT_to_begin(HashADT table);
int hashADT_has_next(HashADT table);
void* hashADT_next(HashADT table);

void dump(HashADT table);

#endif //TP2_SO_HASHADT_H
