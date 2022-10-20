#include <queueADT.h>
#include <mm.h>
#include <stdint.h>


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
    //listADT ans = mm_alloc(sizeof (struct listCDT));
    listADT ans = malloc(sizeof (struct listCDT));
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
    //free(l);
}

//para que la funcion sea recursiva, el argumento debe ser recursivo y listADT no lo es
void freeList(listADT list){//no puede ser recursiva, la estructura listADT no es recursiva
    freeListRec(list->first);
    mm_free(list);
    //free(list);
}

int isEmpty(listADT list){
    return list->size==0;
}

//agrego un nuevo elemento al final de la lista
void add(listADT list, elemType elem){
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
}

int size(const listADT list){
    return list->size;
}

//Elimino un elemento de la lista
int remove(listADT list, elemType elem){
    if(list== NULL || list->first == NULL)
        return 1;
    TNode * current = list->first;
    TNode * prev = NULL;
    while(current != NULL){
        if(compare(elem, current->data) == 0){
            if(compare(current->data, list->first->data) == 0){
                list->first = current->next;
            }
            if(compare(current->data, list->last->data) == 0){
                list->last = prev;
                //si es el primero y el ultimo
                if(prev != NULL)
                    prev->next=NULL;
            }
            else{
                prev->next = current->next;
            }
            //free(current);
            mm_free(current);
            list->size--;
            return 0;
        }
        prev = current;
        current = current->next;
    }
    return 1;
}


TNode * extract(listADT list, elemType elem){
    if(list== NULL || list->first == NULL)
        return NULL;
    TNode * current = list->first;
    TNode * prev = NULL;
    while(current != NULL){
        if(compare(elem, current->data) == 0){
            if(compare(current->data, list->first->data) == 0){
                list->first = current->next;
            }
            if(compare(current->data, list->last->data) == 0){
                list->last = prev;
                if(prev != NULL)
                    prev->next=NULL;
            }
            else{
                prev->next = current->next;
            }
            current->next=NULL;
            list->size--;
            return current;
        }
        prev = current;
        current = current->next;
    }
    return NULL;
}

//agrego un nuevo elemento al final de la lista
void insert(listADT list, TNode * node){
    //queremos insertar el elemento al final de la cola
    if(list->last==NULL){
        //es el primer elemento que agrego
        list->first = node;
        list->last = node;
        (list->size)++;
    }else{
        list->last->next = node;
        list->last = node;
        list->size++;
    }
}






//Devuelve el nodo que sigue en la cola
//Elimina a ese elemento de la cola
elemType getNext(listADT list){
    if(list->first==NULL){
        //No hay un proximo elemento
        return NULL;
    }
    //Le damos el primer elemento
    elemType ans = list->first->data;
    TList nodeToRemove = list->first;
    //mm_free(nodeToRemove);
    free(nodeToRemove);
    (list->size)--;
    list->first = list->first->next;
    return ans;
}
