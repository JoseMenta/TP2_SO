
#ifndef TP2_SO_QUEUEADT_H
#define TP2_SO_QUEUEADT_H

#include <stdint.h>
#include <stddef.h>

#define ELEM_NOT_FOUND NULL

// compare_function es la funcion que se debe pasar para comparar los elementos que conformen a la estructura
typedef int64_t (* compare_function) (void * e1, void * e2);

typedef struct queueCDT* queueADT;

void queueADT_toBegin(queueADT queue);
uint8_t queueADT_hasNext(const queueADT queue);
void * queueADT_next(queueADT queue);

queueADT new_queueADT(compare_function cmp);
void free_queueADT(queueADT queue);
void * queueADT_remove(queueADT queue, void * elem);
void * queueADT_find(queueADT queue, void * elem);
uint8_t queueADT_is_empty(queueADT queue);
int8_t queueADT_insert(queueADT queue, void * elem);
uint32_t queueADT_size(const queueADT queue);
void * queueADT_get_next(queueADT queue);

#endif //TP2_SO_QUEUEADT_H
