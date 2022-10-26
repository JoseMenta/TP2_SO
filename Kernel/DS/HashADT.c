#include "../include/HashADT.h"
#include "stdlib.h"
//#include <stdio.h>

HashADT newHashADT(){
    HashADT aux = malloc(sizeof (TList) * 10);
    for(int i=0; i<SIZE; i++){
        (*aux)[i] = new_orderListADT();
    }
    return aux;
}

void HashProcess_delete(HashADT hash, elemType myPCB){
    (*hash)[myPCB->pid % SIZE] = orderListADT_delete((*hash)[myPCB->pid % SIZE], myPCB);
}

void HashProcess_add(HashADT hash, elemType myPCB){
    (*hash)[myPCB->pid % SIZE] = orderListADT_add((*hash)[myPCB->pid % SIZE], myPCB);
}

int HashProcess_edit(HashADT hash, elemType prevPCB, elemType newPCB){
    if(elemType_compare_to(prevPCB, newPCB) != 0){
        return 1;
    }
    (*hash)[prevPCB->pid % SIZE] = orderListADT_edit((*hash)[prevPCB->pid % SIZE], prevPCB, newPCB);
    return 0;
}

//Retorna puntero al PCB o NULL si no encontro el pid
PCB * HashProcess_get(HashADT hash, int pid){
    return orderListADT_get((*hash)[pid % SIZE], pid);
}

/*
//Para testeo
void dump(HashADT hash){
    printf("\n------\n");
    for(int i=0; i<SIZE; i++){
        printf("\n%d) ", i);
        printList((*hash)[i]);
    }
    printf("\n------\n");
}
*/

void freeHashADT(HashADT hash){
    for(int i=0; i<SIZE; i++){
        free_orderListADT((*hash)[i]);
    }
    free(hash);
}
