//
// Created by Jose Menta on 22/10/2022.
//

#ifndef TP2_SO_DS_H
#define TP2_SO_DS_H
#include <scheduler.h>
#include <stddef.h>
//TODO: sacar
#include "scheduler.h"
typedef PCB* elemType;
int64_t elemType_compare_to(void *e1, void *e2);
uint64_t elemType_prehash(void * elem);
#define ELEM_NOT_FOUND NULL
#endif //TP2_SO_DS_H
