#include <queueADT.h>
#include <mm.h>
#include <stddef.h>

//Vamos a hacer una queue utilizando una lista simplemente encadenada
typedef struct node{
    void * data;                                // Info del nodo
    struct node* next;                          // Proximo en la cola
}TNode;

typedef TNode* TList;


struct queueCDT{
    uint64_t size;                              // Cantidad de elementos en la lista
    compare_function cmp;                       // Funcion de comparacion
    TList first;                                // Puntero al primer elemento
    TList current;                              // Para iteradores, apunta al siguiente que tiene que devolver
    TList last;                                 // Puntero al ultimo elemento
};

//----------------------------------------------------------------------
// queueADT_toBegin: Inicializa el iterador de la cola
//----------------------------------------------------------------------
// Argumentos:
//  queue: la cola
//----------------------------------------------------------------------
// Retorno:
//----------------------------------------------------------------------
void queueADT_toBegin(queueADT queue){
    queue->current=queue->first;
}

//----------------------------------------------------------------------
// queueADT_hasNext: Indica si el iterador tiene un elemento para devolver
//----------------------------------------------------------------------
// Argumentos:
//  queue: la cola
//----------------------------------------------------------------------
// Retorno:
//  0 si no hay elementos por iterar
//  !0 si hay elementos
//----------------------------------------------------------------------
uint8_t queueADT_hasNext(const queueADT queue){
    return queue->current!= NULL;
}

//----------------------------------------------------------------------
// queueADT_next: Devuelve el proximo elemento del iterador
//----------------------------------------------------------------------
// Argumentos:
//  queue: la cola
//----------------------------------------------------------------------
// Retorno:
//  ELEM_NOT_FOUND si no quedan elementos para devolver
//  El siguiente elemento si quedan
//----------------------------------------------------------------------
void * queueADT_next(queueADT queue){
    if(queue->current==NULL){
        return ELEM_NOT_FOUND;
    }
    void * aux = queue->current->data;
    queue->current = queue->current->next;
    return aux;
}

//----------------------------------------------------------------------
// new_queueADT: Crea una nueva cola
//----------------------------------------------------------------------
// Argumentos:
//  cmp: la funcion de comparacion a utilizar para ordenar
//----------------------------------------------------------------------
// Retorno:
//  En caso de error o si no se indica una funcion cmp, devuelve NULL
//  En caso de exito devuelve una nueva lista
//----------------------------------------------------------------------
queueADT new_queueADT(compare_function cmp){
    if(cmp == NULL){
        return NULL;
    }
    queueADT ans = mm_alloc(sizeof (struct queueCDT));
    if(ans==NULL){
        return NULL;
    }
    ans->cmp = cmp;
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

//----------------------------------------------------------------------
// free_queueADT: Destruye una cola
//----------------------------------------------------------------------
// Argumentos:
//  queue: la cola a destruir
//----------------------------------------------------------------------
// Retorno:
//----------------------------------------------------------------------
void free_queueADT(queueADT queue){
    freeListRec(queue->first);
    mm_free(queue);
}

//----------------------------------------------------------------------
// queueADT_is_empty: Indica si la cola esta vacia
//----------------------------------------------------------------------
// Argumentos:
//  queue: la cola
//----------------------------------------------------------------------
// Retorno:
//  0 si no esta vacio
//  !0 si esta vacio
//----------------------------------------------------------------------
uint8_t queueADT_is_empty(queueADT queue){
    return queue->size==0;
}

//----------------------------------------------------------------------
// queueADT_find: Devuelve un elemento de la cola
//----------------------------------------------------------------------
// Argumentos:
//  queue: la cola sobre la cual buscar
//  elem: el elemento a buscar
//----------------------------------------------------------------------
// Retorno:
//  Devuelve el elemento si esta
//  Devuelve ELEM_NOT_FOUND si no esta
//----------------------------------------------------------------------
void * queueADT_find(queueADT queue, void * elem){
    TList curr = queue->first;
    while(curr!=NULL && queue->cmp(curr->data, elem) != 0){
        curr = curr->next;
    }
    if(curr==NULL){
        return ELEM_NOT_FOUND;
    }
    return curr->data;
}

//----------------------------------------------------------------------
// queueADT_remove: Elimina un elemento de la cola
//----------------------------------------------------------------------
// Argumentos:
//  queue: la cola sobre la cual eliminar
//  elem: el elemento a eliminar
//----------------------------------------------------------------------
// Retorno:
//  Devuelve el elemento si esta
//  Devuelve ELEM_NOT_FOUND si no esta
//----------------------------------------------------------------------
void * queueADT_remove(queueADT queue, void * elem){
    if(queue->size==0){
        return ELEM_NOT_FOUND;
    }
    TList prev = NULL;
    TList curr = queue->first;
    if(queue->cmp(curr->data,elem)==0){
        //si es el primero en la lista
        void * ans = curr->data;
        if(queue->last==queue->first){
            queue->last = NULL;
        }
        queue->first = curr->next;
        (queue->size)--;
        mm_free(curr);
        return ans;
    }
    while(curr!=NULL && queue->cmp(curr->data, elem)!=0){
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
    void * ans = curr->data;
    prev->next = curr->next;
    mm_free(curr);
    return ans;
}

//----------------------------------------------------------------------
// queueADT_insert: Agrega un elemento en la cola
//----------------------------------------------------------------------
// Argumentos:
//  queue: la cola sobre la cual agregar
//  elem: el elemento a agregar
//----------------------------------------------------------------------
// Retorno:
//  Devuelve 1 si lo pudo agregar
//  Devuelve -1 si no lo pudo agregar
//----------------------------------------------------------------------
int8_t queueADT_insert(queueADT queue, void * elem){
    TList newNode = mm_alloc(sizeof (struct node));
    if(newNode==NULL){
        return -1;
    }
    newNode->data = elem;
    newNode->next = NULL;
    //queremos insertar el elemento al final de la cola
    if(queue->last==NULL){
        //es el primer elemento que agrego
        queue->first = newNode;
        queue->last = newNode;
        (queue->size)++;
    }else{
        queue->last->next = newNode;
        queue->last = newNode;
        queue->size++;
    }
    return 1;
}

//----------------------------------------------------------------------
// queueADT_size: Devuelve la cantidad de elementos en la cola
//----------------------------------------------------------------------
// Argumentos:
//  queue: la cola
//----------------------------------------------------------------------
// Retorno:
//  Devuelve la cantidad de elementos en la cola
//----------------------------------------------------------------------
uint32_t queueADT_size(const queueADT queue){
    return queue->size;
}

//----------------------------------------------------------------------
// queueADT_get_next: Devuelve el proximo elemento en la cola y lo elimina de la cola
//----------------------------------------------------------------------
// Argumentos:
//  queue: la cola
//----------------------------------------------------------------------
// Retorno:
//  Devuelve el elemento si la cola no esta vacia
//  Devuelve ELEM_NOT_FOUND si la cola esta vacia
//----------------------------------------------------------------------
//devuelve el nodo que sigue en la cola
//Elimina a ese elemento de la cola
void * queueADT_get_next(queueADT queue){
    if(queue->first==NULL){
        //No hay un proximo elemento
        return ELEM_NOT_FOUND;
    }
    //Le damos el primer elemento
    void * ans = queue->first->data;
    TList nodeToRemove = queue->first;
    if(queue->last == queue->first){
        queue->last = NULL;
    }
    (queue->size)--;
    queue->first = queue->first->next;
    mm_free(nodeToRemove);
    return ans;
}

