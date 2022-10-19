#include <queueADT.h>
#include "../include/queueADT.h"
#include <mm.h>
#include "../include/mm.h"
#include <stdint.h>
//Vamos a hacer una queue utilizando una lista simplemente encadenada
typedef struct node{
    elemType data;
    struct node* next;
}TNode;
typedef TNode* TList;
struct listCDT{
    uint64_t size;
    TList first;
    TList current; //para iteradores, apunta al siguiente que tiene que devolver
    TList last; //Para agregar un elemento al final
};
void toBegin(listADT list){
    list->current=list->first;
}
int hasNext(const listADT list){
    return list->current!= NULL;
}
elemType next(listADT list){
    if(list->current==NULL){
        return NULL;
    }
    elemType aux=list->current->head;
    list->current=list->current->tail;
    return aux;
}
listADT newList(){
    listADT ans = mm_alloc(sizeof (struct listCDT));
    ans->first = NULL;
    ans->last = NULL;
    ans->current = NULL;
    ans->size = 0;
}
static void freeListRec(TList l){
    if(l==NULL){
        return;
    }
    freeListRec(l->tail);
    mm_free(l);
}
//para que la funcion sea recursiva, el argumento debe ser recursivo y listADT no lo es
void freeList(listADT list){//no puede ser recursiva, la estructura listADT no es recursiva
    freeListRec(list->first);
    mm_free(list);
}
int isEmpty(listADT list){
    return list->size==0;
}
int insert(listADT list, elemType elem){
    TList newNode = mm_alloc(sizeof (struct node));
    newNode->data = elem;
    newNode->next = NULL;
    //queremos insertar el elemento al final de la cola
    if(list->last==NULL){
        //es el primer elemento que agrego
        list->first = newNode;
        list->last = newNode;
        (list->size)++;
    }else{
        list->last->next = newNode;
        list->last = newNode;
        list->size++;
    }
    return 1;
}

int size(const listADT list){
    return list->size;
}
//devuelve el nodo que sigue en la cola
//Elimina a ese elemento de la cola
elemType getNext(listADT list){
    if(list->first==NULL){
        //No hay un proximo elemento
        return NULL;
    }
    //Le damos el primer elemento
    elemType ans = list->first->data;
    TList nodeToRemove = list->first;
    mm_free(nodeToRemove);
    (list->size)--;
    list->first = list->first->next;
    return ans;
}

