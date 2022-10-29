
#include "../include/orderListADT.h"
#include <stdio.h>
#include <DS.h>
//No requiere una incializacion porque eso lo hago cuando inserto el primer elemento
TList new_orderListADT(){
    return NULL;
}

TList orderListADT_add(TList myListADT, elemType elem){
    //Esta vacía o el próximo es mayor
    if(myListADT == NULL || elemType_compare_to(myListADT->data, elem) < 0){
        TList newNode = mm_alloc(sizeof(*newNode));
        newNode->data=elem;
        newNode->next=myListADT;
        return newNode;
    }
    //Si ya esta en la lista
    if(elemType_compare_to(myListADT->data, elem) == 0){
        return myListADT;
    }
    myListADT->next=orderListADT_add(myListADT->next, elem);
    return myListADT;
}


TList orderListADT_delete(TList myListADT, elemType elem){
    //No esta en la lista
    if(myListADT == NULL || elemType_compare_to(myListADT->data, elem) < 0){
        return myListADT;
    }
    //Lo encontre entonces lo elimino
    if(elemType_compare_to(myListADT->data, elem) == 0){
        TList aux = myListADT->next;
        mm_free(myListADT);
        return aux;
    }
    myListADT->next=orderListADT_delete(myListADT->next, elem);
    return myListADT;
}

//int elemType_compare_to(elemType e1, elemType e2){
//    return e1->pid-e2->pid;
//}

TList orderListADT_edit(TList myListADT, elemType prevElem, elemType newElem){
    //No esta en la lista
    if(myListADT == NULL || elemType_compare_to(myListADT->data, prevElem) < 0){
        return myListADT;
    }
    //Lo encontre entonces lo elimino
    if(elemType_compare_to(myListADT->data, prevElem) == 0){
        myListADT->data = newElem;
    }
    myListADT->next=orderListADT_edit(myListADT->next, prevElem, newElem);
    return myListADT;
}

elemType orderListADT_get(TList myListADT, uint32_t pid){
    //No esta en la lista
    if(myListADT == NULL || myListADT->data->pid < pid){
        return NULL;
    }
    if(myListADT->data->pid == pid){
        return myListADT->data;
    }
    return orderListADT_get(myListADT->next, pid);
}

/*
//Para testeo
void printList(TList myListADT){
    if(myListADT == NULL)
        return;
    printf(" %d ->", myListADT->data->pid);
    return printList(myListADT->next);
}
 */

void free_orderListADT(TList myListADT){
    if(myListADT == NULL)
        return;
    free_orderListADT(myListADT->next);
    mm_free(myListADT);
}
