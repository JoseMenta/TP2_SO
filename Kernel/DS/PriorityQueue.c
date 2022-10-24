//
// Created on 19/10/22.
//
#include "../include/queueADT.h"
#include <stdlib.h>
#include "../include/PriorityQueue.h"


//Agrego un proceso mediante la informacion del PCB
void add_process(PCB * process){
    add(queue_index[process->priority][READY], process->pid);
}

//Saco un proceso
int kill_process(PCB * process){
    return remove(queue_index[process->priority][READY], process->pid);
}

//Cambio la prioridad de un proceso
int change_priority_process(PCB * process, u_int8_t new_priority){
    if(new_priority < 0 || new_priority > 4)
        new_priority = DEFAULT_PRIORITY;
    TNode * aux = extract(queue_index[process->priority][READY], process->pid);
    if(aux == NULL)
        return 1;
    insert(queue_index[new_priority][READY], aux);
    return 0;
}

//Cambio el estado del proceso en RR
int change_status_process(PCB * process){
    TNode * aux = extract(queue_index[process->priority][READY], process->pid);
    if(aux == NULL)
        return 1;
    insert(queue_index[process->priority][TERMINATED], aux);
    return 0;
}

//Inicializo el ADT con la matiz de punteros que necesito
PriorityQueueADT new_priority_queue(){
    PriorityQueueADT priority_queue = malloc(sizeof(ProrityQueueCDT));
    for(int j=0; j<STATES; j++){
        for(int i=0; i<PRIORITIES; i++){
            priority_queue[i][j] = newList();
        }
    }
    return priority_queue;
}

void free_priority_queue(PriorityQueueADT queue){
    for(int j=0; j<STATES; j++){
        for(int i=0; i<PRIORITIES; i++){
            freeList(priority_queue[i][j]);
        }
    }
    free(priority_queue);
}


