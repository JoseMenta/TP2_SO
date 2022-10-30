
#include <orderListADT.h>
#include <mm.h>
// #include <stdio.h>

typedef struct node{
    void * data;                    // Dato generico a guardar
    struct node * next;             // Proximo nodo en la lista
} node_t;

typedef struct list_t{
    node_t * first;                 // Lista
    node_t * current;               // Puntero para iterador
    compare_function cmp;           // Funcion de comparacion
    uint64_t size;                  // Cantidad de elementos
} list_t;

//----------------------------------------------------------------------
// new_orderListADT: Crea una nueva lista ordenada
//----------------------------------------------------------------------
// Argumentos:
//  cmp: la funcion de comparacion a utilizar para ordenar
//----------------------------------------------------------------------
// Retorno:
//  En caso de error o si no se indica una funcion cmp, devuelve NULL
//  En caso de exito devuelve una nueva lista
//----------------------------------------------------------------------
orderListADT new_orderListADT(compare_function cmp){
    if(cmp == NULL){
        return NULL;
    }
    orderListADT list = mm_alloc(sizeof(list_t));
    if(list == NULL){
        return NULL;
    }
    list->first = NULL;
    list->current = NULL;
    list->cmp = cmp;
    list->size = 0;
    return list;
}

// Funcion recursiva para agregar a la lista
static node_t * add_rec(node_t * node, void * elem, compare_function cmp, uint64_t * size){
    // Si es el final de la lista o el proximo es mayor, lo agrego ahora
    if(node == NULL || cmp(node->data, elem) < 0){
        node_t * new_node = mm_alloc(sizeof(node_t));
        if(new_node == NULL){
            return node;
        }
        new_node->data = elem;
        new_node->next = node;
        *size = *size + 1;
        return new_node;
    }
    // Si ya esta en la lista, retorno la lista como esta
    if(cmp(node->data, elem) == 0){
        return node;
    }
    // En otro caso, debo seguir recorriendo la lista
    node->next = add_rec(node->next, elem, cmp, size);
    return node;
}

//----------------------------------------------------------------------
// orderListADT_add: Agrega un nuevo elemento a la lista
//----------------------------------------------------------------------
// Argumentos:
//  myListADT: la lista sobre el cual agregar
//  elem: el elemento a agregar
//----------------------------------------------------------------------
// Retorno:
//  Devuelve 1 si lo pudo agregar
//  Devuelve -1 si no lo pudo agregar
//----------------------------------------------------------------------
int8_t orderListADT_add(orderListADT myListADT, void * elem){
    if(myListADT == NULL || elem == NULL){
        return -1;
    }
    myListADT->first = add_rec(myListADT->first, elem, myListADT->cmp, &myListADT->size);
    return 1;
}


static node_t * delete_rec(node_t * node, void * elem, compare_function cmp, uint64_t * size, void ** response){
    // Si llego al final de la lista o el nodo es mayor al actual, entonces el elemento no esta
    if(node == NULL || cmp(node->data, elem) < 0){
        response = NULL;
        return node;
    }
    // Si la funcion cmp da 0, entonces lo encontre
    if(cmp(node->data, elem) == 0){
        node_t * aux = node->next;
        *response = node->data;
        mm_free(node);
        *size = *size - 1;
        return aux;
    }
    // En cualquier otro caso debo seguir recorriendo la lista para ver si lo encuentro
    node->next = delete_rec(node->next, elem, cmp, size, response);
    return node;
}

//----------------------------------------------------------------------
// orderListADT_delete: Elimina un elemento de la lista
//----------------------------------------------------------------------
// Argumentos:
//  myListADT: la lista sobre el cual eliminar
//  elem: el elemento a eliminar
//----------------------------------------------------------------------
// Retorno:
//  Devuelve el elemento eliminado si lo encuentra, NULL si no estaba
//----------------------------------------------------------------------
void * orderListADT_delete(orderListADT myListADT, void * elem){
    if(myListADT == NULL || elem == NULL){
        return NULL;
    }
    void * response = NULL;
    myListADT->first = delete_rec(myListADT->first, elem, myListADT->cmp, &myListADT->size, &response);
    return response;
}

//----------------------------------------------------------------------
// orderListADT_edit: Edita un elemento de la lista
//----------------------------------------------------------------------
// Argumentos:
//  myListADT: la lista sobre el cual editar
//  prev_elem: el elemento a actualizar
//  new_elem: el nuevo elemento por el cual reemplazar
//----------------------------------------------------------------------
// Retorno:
//  Devuelve el elemento viejo si lo encuentra, NULL si no estaba
//----------------------------------------------------------------------
void * orderListADT_edit(orderListADT myListADT, void * prev_elem, void * new_elem){
    if(myListADT == NULL || prev_elem == NULL || new_elem == NULL){
        return NULL;
    }

    void * response = NULL;

    // Eliminamos el viejo
    myListADT->first = delete_rec(myListADT->first, prev_elem, myListADT->cmp, &myListADT->size, &response);
    // Si no se lo encontro, entonces no debe actualizar nada
    if(response == NULL){
        return NULL;
    }
    // Agregamos el nuevo
    myListADT->first = add_rec(myListADT->first, new_elem, myListADT->cmp, &myListADT->size);
    return response;
}

static void * get_rec(node_t * node, void * elem_id, compare_function cmp){
    // Si llegue al final de la lista o el siguiente es mayor, entonces no lo encontre
    if(node == NULL || cmp(node->data, elem_id) < 0){
        return NULL;
    }
    // Si la funcion de comparacion da 0, entonces lo encontre
    if(cmp(node->data, elem_id) == 0){
        return node->data;
    }
    // En otro caso, debo seguir recorriendo la lista
    return get_rec(node->next, elem_id, cmp);
}

//----------------------------------------------------------------------
// orderListADT_get: Devuelve un elemento de la lista
//----------------------------------------------------------------------
// Argumentos:
//  myListADT: la lista sobre el cual buscar
//  elem_id: el id del elemento a buscar
//----------------------------------------------------------------------
// OJO: Se basa en la funcion de comparacion solicitada en la creacion de la lista
//----------------------------------------------------------------------
// Retorno:
//  El elemento si lo encuentra
//  NULL si no
//----------------------------------------------------------------------
void * orderListADT_get(orderListADT myListADT, void * elem_id){
    if(myListADT == NULL || elem_id == NULL){
        return NULL;
    }
    return get_rec(myListADT->first, elem_id, myListADT->cmp);
}

//----------------------------------------------------------------------
// orderListADT_size: Devuelve la cantidad de elementos en la lista
//----------------------------------------------------------------------
// Argumentos:
//  myListADT: la lista
//----------------------------------------------------------------------
// Retorno:
//  Devuelve la cantidad de elementos en la lista
//----------------------------------------------------------------------
uint64_t orderListADT_size(orderListADT myListADT){
    return myListADT->size;
}

//----------------------------------------------------------------------
// queueADT_is_empty: Indica si la lista esta vacia
//----------------------------------------------------------------------
// Argumentos:
//  myListADT: la lista
//----------------------------------------------------------------------
// Retorno:
//  0 si no esta vacio
//  !0 si esta vacio
//----------------------------------------------------------------------
uint8_t orderListADT_is_empty(orderListADT myListADT){
    return myListADT->size == 0;
}


static void free_rec(node_t * node){
    if(node == NULL){
        return;
    }
    free_rec(node->next);
    mm_free(node);
}

//----------------------------------------------------------------------
// free_orderListADT: Destruye la lista
//----------------------------------------------------------------------
// Argumentos:
//  myListADT: la lista a eliminar
//----------------------------------------------------------------------
// Retorno:
//----------------------------------------------------------------------
void free_orderListADT(orderListADT myListADT){
    if(myListADT == NULL){
        return;
    }
    free_rec(myListADT->first);
    mm_free(myListADT);
}


//----------------------------------------------------------------------
// orderListADT_toBegin: Inicializa el iterador de la lista
//----------------------------------------------------------------------
// Argumentos:
//  myListADT: la lista
//----------------------------------------------------------------------
// Retorno:
//----------------------------------------------------------------------
void orderListADT_toBegin(orderListADT myListADT){
    myListADT->current = myListADT->first;
}

//----------------------------------------------------------------------
// orderListADT_hasNext: Indica si el iterador tiene un elemento para devolver
//----------------------------------------------------------------------
// Argumentos:
//  myListADT: la lista
//----------------------------------------------------------------------
// Retorno:
//  0 si no hay elementos por iterar
//  !0 si hay elementos
//----------------------------------------------------------------------
uint8_t orderListADT_hasNext(const orderListADT myListADT){
    return myListADT->current != NULL;
}

//----------------------------------------------------------------------
// orderListADT_next: Devuelve el proximo elemento del iterador
//----------------------------------------------------------------------
// Argumentos:
//  myListADT: la lista
//----------------------------------------------------------------------
// Retorno:
//  NULL si no quedan elementos para devolver
//  El siguiente elemento si quedan
//----------------------------------------------------------------------
void * orderListADT_next(orderListADT myListADT){
    if(myListADT->current == NULL){
        return NULL;
    }
    void * aux = myListADT->current->data;
    myListADT->current = myListADT->current->next;
    return aux;
}


//Para testeo
void printList(orderListADT myListADT){
    if(myListADT == NULL)
        return;
    node_t * aux = myListADT->first;
    while(aux != NULL){
        printf(" %d ->", *(uint32_t *)aux->data);
        aux = aux->next;
    }
}
