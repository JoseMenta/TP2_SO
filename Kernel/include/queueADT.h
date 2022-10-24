//
// Created by Jose Menta on 11/10/2022.
//

#ifndef TP2_SO_QUEUEADT_H
#define TP2_SO_QUEUEADT_H
#include <stdint.h>
#include <scheduler.h>
#include <stddef.h>
#include "../include/DS.h"


typedef struct queueCDT* queueADT;

void queueADT_toBegin(queueADT list);
int queueADT_hasNext(const queueADT list);
elemType queueADT_next(queueADT list);
queueADT new_queueADT();
void free_queueADT(queueADT list);
elemType queueADT_remove(queueADT queue, elemType elem);
elemType queueADT_find(queueADT queue, elemType elem);
int queueADT_is_empty(queueADT list);
int queueADT_insert(queueADT list, elemType elem);
uint32_t queueADT_size(const queueADT list);
elemType queueADT_get_next(queueADT list);
#endif //TP2_SO_QUEUEADT_H
