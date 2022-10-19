//
// Created by Jose Menta on 11/10/2022.
//

#ifndef TP2_SO_QUEUEADT_H
#define TP2_SO_QUEUEADT_H
//Se debe definir convenientemente para el tipo de dato utilizado
#define NULL ((void*)0)
typedef int elemType;
typedef struct listCDT* listADT;
static int compare(elemType elem1, elemType elem2){
    return elem1-elem2;
}
void toBegin(listADT list);
int hasNext(const listADT list);
elemType next(listADT list);
listADT newList();
void freeList(listADT list);
int isEmpty(listADT list);
int insert(listADT list, elemType elem);
int size(const listADT list);
elemType getNext(listADT list);
#endif //TP2_SO_QUEUEADT_H
