//
// Created by Jose Menta on 11/10/2022.
//

#ifndef TP2_SO_QUEUEADT_H
#define TP2_SO_QUEUEADT_H
//Se debe definir convenientemente para el tipo de dato utilizado
#define NULL ((void*)0)

//Vamos a hacer una queue utilizando una lista simplemente encadenada ascendente por PID
typedef struct node{
    elemType data;
    struct node* next;
}TNode;

typedef TNode* TList;
typedef int elemType;
typedef struct listCDT* listADT;
static int compare(elemType elem1, elemType elem2){
    return elem1-elem2;
}

void toBegin(listADT list);
int hasNext(const listADT list);
elemType next(listADT list);
listADT newList();
static void freeListRec(TList l);
void freeList(listADT list);
int isEmpty(listADT list);
void add(listADT list, elemType elem);
int size(const listADT list);
int remove(listADT list, elemType elem);
TNode * extract(listADT list, elemType elem);
void insert(listADT list, TNode * node);
elemType getNext(listADT list);


#endif //TP2_SO_QUEUEADT_H
