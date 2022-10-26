//
// Created by Jose Menta on 19/10/2022.
//
#include <RRLevelADT.h>
#include "../include/RRLevelADT.h"
#include <queueADT.h>
#include "../include/queueADT.h"
#include <mm.h>
#include "../include/mm.h"

struct RRLevelCDT {
    queueADT to_execute;
    queueADT executed;
    uint8_t priority;
    uint8_t tt_interval; //la cantidad de timer tick que determinan al quantum
};

RRLevelADT new_RRLevel(uint8_t priority, uint8_t tt_interval){
    RRLevelADT ans = mm_alloc(sizeof (struct RRLevelCDT));
    if(ans==NULL){
        return NULL;
    }
    ans->to_execute = new_queueADT();
    ans->executed = new_queueADT();
    ans->priority = priority;
    ans->tt_interval = tt_interval;
    return ans;
}

void free_RRLevel(RRLevelADT RRLevel){
    free_queueADT(RRLevel->to_execute);
    free_queueADT(RRLevel->executed);
    mm_free(RRLevel);
}

//agrega un proceso a la cola de ejecutados para que se ejecute despues
int RRLevel_insert(RRLevelADT RRLevel,elemType process){
    //lo inserto en la cola de los expirados
    return queueADT_insert(RRLevel->executed,process);
}

void RRLevel_switch_queues(RRLevelADT RRLevel){
    queueADT temp = RRLevel->to_execute;
    RRLevel->to_execute = RRLevel->executed;
    RRLevel->executed = temp;
}

//saca a un elemType de alguna de las 2 colas (supone que no se insertan repetidos)
elemType RRLevel_delete(RRLevelADT RRLevel, elemType elem){
    elemType ans = queueADT_remove(RRLevel->to_execute, elem);
    if(ans==ELEM_NOT_FOUND){
        ans = queueADT_remove(RRLevel->executed,elem);
    }
    return ans;
}

uint32_t RRLevel_to_execute_size(RRLevelADT RRLevel){
    return queueADT_size(RRLevel->to_execute);
}

elemType RRLevel_get_next(RRLevelADT RRLevel){
    return queueADT_get_next(RRLevel->to_execute);
}