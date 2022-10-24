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
struct queueCDT{
    uint64_t size;
    TList first;
    TList current; //para iteradores, apunta al siguiente que tiene que devolver
    TList last; //Para agregar un elemento al final
};
void queueADT_toBegin(queueADT list){
    list->current=list->first;
}
int queueADT_hasNext(const queueADT list){
    return list->current!= NULL;
}
elemType queueADT_next(queueADT list){
    if(list->current==NULL){
        return ELEM_NOT_FOUND;
    }
    elemType aux=list->current->data;
    list->current=list->current->next;
    return aux;
}
queueADT new_queueADT(){
    queueADT ans = mm_alloc(sizeof (struct queueCDT));
    if(ans==NULL){
        return NULL;
    }
    ans->first = NULL;
    ans->last = NULL;
    ans->current = NULL;
    ans->size = 0;
    return ans;
}
static void freeListRec(TList l){
    if(l==NULL){
        return;
    }
    freeListRec(l->next);
    mm_free(l);
}
//para que la funcion sea recursiva, el argumento debe ser recursivo y listADT no lo es
void free_queueADT(queueADT list){//no puede ser recursiva, la estructura listADT no es recursiva
    freeListRec(list->first);
    mm_free(list);
}
int queueADT_is_empty(queueADT list){
    return list->size==0;
}
elemType queueADT_find(queueADT queue, elemType elem){
    TList curr = queue->first;
    while(curr!=NULL && elemType_compare_to(curr->data, elem)!=0){
        curr = curr->next;
    }
    if(curr==NULL){
        return ELEM_NOT_FOUND;
    }
    return curr->data;
}
//para el kill
elemType queueADT_remove(queueADT queue, elemType elem){
    if(queue->size==0){
        return ELEM_NOT_FOUND;
    }
    TList prev = NULL;
    TList curr = queue->first;
    if(elemType_compare_to(curr->data,elem)==0){
        //si es el primero en la lista
        elemType ans = curr->data;
        if(queue->last==queue->first){
            queue->last = NULL;
        }
        queue->first = curr->next;
        (queue->size)--;
        mm_free(curr);
        return ans;
    }
    while(curr!=NULL && elemType_compare_to(curr->data, elem)!=0){
        prev = curr;
        curr = curr->next;
    }
    if(curr==NULL){
        return ELEM_NOT_FOUND;
    }
    (queue->size)--;
    if(curr==queue->last){
        queue->last = prev;
    }
    elemType ans = curr->data;
    prev->next = curr->next;
    mm_free(curr);
    return ans;
}
//Agrega un elemento al final de la cola
int queueADT_insert(queueADT list, elemType elem){
    TList newNode = mm_alloc(sizeof (struct node));
    if(newNode==NULL){
        return -1;
    }
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

uint32_t queueADT_size(const queueADT list){
    return list->size;
}
//devuelve el nodo que sigue en la cola
//Elimina a ese elemento de la cola
elemType queueADT_get_next(queueADT list){
    if(list->first==NULL){
        //No hay un proximo elemento
        return ELEM_NOT_FOUND;
    }
    //Le damos el primer elemento
    elemType ans = list->first->data;
    TList nodeToRemove = list->first;
    if(list->last == list->first){
        list->last = NULL;
    }
    (list->size)--;
    list->first = list->first->next;
    mm_free(nodeToRemove);
    return ans;
}

