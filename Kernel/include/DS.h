//
// Created by Jose Menta on 22/10/2022.
//

#ifndef TP2_SO_DS_H
#define TP2_SO_DS_H
#include "scheduler.h"
#include <stddef.h>
typedef PCB* elemType;
int elemType_compare_to(elemType e1, elemType e2){
    return e1->pid-e2->pid;
}
#define ELEM_NOT_FOUND NULL
#endif //TP2_SO_DS_H
