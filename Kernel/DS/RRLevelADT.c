// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <RRLevelADT.h>
#include <queueADT.h>
#include <mm.h>
#include <stddef.h>


struct RRLevelCDT {
    queueADT to_execute;
    queueADT executed;
};

//----------------------------------------------------------------------
// new_RRLevel: Inicializo el manejador de niveles del RR
//----------------------------------------------------------------------
RRLevelADT new_RRLevel(){
    RRLevelADT ans = mm_alloc(sizeof (struct RRLevelCDT));
    if(ans==NULL){
        return NULL;
    }
    ans->to_execute = new_queueADT(elemType_compare_to);
    ans->executed = new_queueADT(elemType_compare_to);
    return ans;
}

//----------------------------------------------------------------------
// new_RRLevel: liberar la memoria de niveles del RR
//----------------------------------------------------------------------
void free_RRLevel(RRLevelADT RRLevel){
    free_queueADT(RRLevel->to_execute);
    free_queueADT(RRLevel->executed);
    mm_free(RRLevel);
}

//----------------------------------------------------------------------
// RRLevel_insert: agrega un proceso a la cola de ejecutados para que se ejecute despues
//----------------------------------------------------------------------
int RRLevel_insert(RRLevelADT RRLevel,elemType process){
    return queueADT_insert(RRLevel->executed,process);
}

//----------------------------------------------------------------------
// RRLevel_insert: agrega un proceso a la cola de ejecutados para que se ejecute despues
//----------------------------------------------------------------------
void RRLevel_switch_queues(RRLevelADT RRLevel){
    queueADT temp = RRLevel->to_execute;
    RRLevel->to_execute = RRLevel->executed;
    RRLevel->executed = temp;
}

//----------------------------------------------------------------------
// RRLevel_delete: saca a un elemType de alguna de las 2 colas (supone que no se insertan repetidos)
//----------------------------------------------------------------------
elemType RRLevel_delete(RRLevelADT RRLevel, elemType elem){
    elemType ans = queueADT_remove(RRLevel->to_execute, elem);
    if(ans==ELEM_NOT_FOUND){
        ans = queueADT_remove(RRLevel->executed,elem);
    }
    return ans;
}

//----------------------------------------------------------------------
// RRLevel_to_execute_size: retorna la cantidad de elementos en el nivel
//----------------------------------------------------------------------
uint32_t RRLevel_to_execute_size(RRLevelADT RRLevel){
    return queueADT_size(RRLevel->to_execute);
}

//----------------------------------------------------------------------
// RRLevel_get_next: Iterados del nivel
//----------------------------------------------------------------------
elemType RRLevel_get_next(RRLevelADT RRLevel){
    return queueADT_get_next(RRLevel->to_execute);
}