
#ifndef TP2_SO_ORDERLISTADT_H
#define TP2_SO_ORDERLISTADT_H

#include <stdint.h>


// compare_function es la funcion que se debe pasar para comparar los elementos que conformen a la estructura
typedef int64_t (* compare_function) (void * e1, void * e2);

typedef struct list_t * orderListADT;

orderListADT new_orderListADT(compare_function cmp);
int8_t orderListADT_add(orderListADT myListADT, void * elem);
void * orderListADT_delete(orderListADT myListADT, void * elem);
void * orderListADT_edit(orderListADT myListADT, void * prev_elem, void * new_elem);
void * orderListADT_get(orderListADT myListADT, void * elem_id);
uint64_t orderListADT_size(orderListADT myListADT);
uint8_t orderListADT_is_empty(orderListADT myListADT);
void free_orderListADT(orderListADT myListADT);

void orderListADT_toBegin(orderListADT myListADT);
uint8_t orderListADT_hasNext(const orderListADT myListADT);
void * orderListADT_next(orderListADT myListADT);

void printList(orderListADT myListADT);

#endif //TP2_SO_ORDERLISTADT_H
