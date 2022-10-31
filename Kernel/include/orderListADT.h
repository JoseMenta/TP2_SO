
#ifndef TP2_SO_ORDERLISTADT_H
#define TP2_SO_ORDERLISTADT_H
#include <mm.h>
#include <scheduler.h>


typedef PCB * elemType;

typedef struct node{
    elemType data;
    struct node* next;
}TNode;

typedef TNode* TList;
//TODO: cambiar el pid a uint64_t
int elemType_compare_to(elemType e1, elemType e2);
TList new_orderListADT();
TList orderListADT_add(TList myListADT, elemType elem);
TList orderListADT_delete(TList myListADT, elemType elem);
TList orderListADT_edit(TList myListADT, elemType prevElem, elemType newElem);
elemType orderListADT_get(TList myListADT, uint32_t pid);
void printList(TList myListADT);
void free_orderListADT(TList myListADT);

#endif //TP2_SO_ORDERLISTADT_H
