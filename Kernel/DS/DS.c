//
// Created by Jose Menta on 24/10/2022.
//
#include <DS.h>
#include "../include/DS.h"
//buenas
int64_t elemType_compare_to(void *e1, void *e2){
    return ((elemType)e1)->pid-((elemType)e2)->pid;
}
uint64_t elemType_prehash(void * elem){
    return ((elemType)elem)->pid;
}