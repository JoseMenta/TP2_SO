//
// Created by Jose Menta on 22/10/2022.
//
#include <DS.H>
#include "../include/RRADT.h"
#include "../include/RRLevelADT.h"
#include "../include/mm.h"

struct RRCDT{
    RRLevelADT levels[PRIORITY_LEVELS];
    uint32_t current_level;
    uint64_t process_count;
};

RRADT new_RR(){
    RRADT ans = mm_alloc(sizeof (struct RRCDT));
    if(ans==NULL){
        return ans;
    }
    for(int i = 0; i<PRIORITY_LEVELS; i++){
        (ans->levels)[i] = new_RRLevel(i, GET_QUANTUM_INTERVAL(i));
        if((ans->levels)[i]==NULL){
            for(int j = 0; j<i; j++){
                free_RRLevel((ans->levels)[i]);
                return NULL;
            }
        }
    };
    ans->current_level = 0;
    ans->process_count = 0;
    return ans;
}
void free_RR(RRADT rr){
    for(int i = 0; i<PRIORITY_LEVELS; i++){
        free_RRLevel((rr->levels)[i]);
    }
    mm_free(rr);
}
//Agrega un proceso con la prioridad indicada (en la cola de expirados)
int RR_add_process(RRADT rr, elemType elem, uint64_t priority){
    if(!CHECK_PRIORITY(priority)){
        return -1;
    }
    int ans =  RRLevel_insert((rr->levels)[priority],elem);
    if(ans!=-1){
        (rr->process_count)++;
    }
    return ans;
}
//Saca a un proceso de las colas de listos del RR
elemType RR_remove_process(RRADT rr, uint64_t priority,elemType elem){
    if(!CHECK_PRIORITY(priority)){
        return ELEM_NOT_FOUND;
    }
    elemType ans = RRLevel_delete((rr->levels)[priority],elem);
    if(ans!=ELEM_NOT_FOUND){
        (rr->process_count)--;
    }
    return ans;
}

//Obtiene el siguiente proceso a ejecutar, retorna ELEM_NOT_FOUND si no hay mas procesos esperando
elemType RR_get_next(RRADT rr){
    if(rr->process_count==0){
        return ELEM_NOT_FOUND;
    }
    elemType ans = ELEM_NOT_FOUND;
    while ( rr->current_level<PRIORITY_LEVELS &&(ans = RRLevel_get_next((rr->levels)[rr->current_level]))==ELEM_NOT_FOUND ){
        (rr->current_level)++;
    }
    if(rr->current_level==PRIORITY_LEVELS){
        //llegue al final de las listas, tengo que volver a empezar (cambiar las listas)
        for(int i = 0; i<PRIORITY_LEVELS; i++){
            RRLevel_switch_queues((rr->levels)[i]);
        }
        rr->current_level = 0; //vuelvo a empezar por la prioridad menor
        //Esto no va a ser infinito, pues ya se que hay procesos por ejecutar
        return RR_get_next(rr);
    }else{
//        ans!=ELEM_NOT_FOUND, se encontro el siguiente proceso
        (rr->process_count)--;
        return ans;
    }
}

uint64_t RR_process_count(RRADT r){
    return r->process_count;
}
