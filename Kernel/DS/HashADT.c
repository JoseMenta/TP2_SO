#include <HashADT.h>
#include <mm.h>
#include <stddef.h>

typedef struct HashCDT{
    orderListADT lists[SIZE];       // Listas del hash
    prehash_function pre_hash;      // Funcion de prehasheo
    uint32_t iterator_index;
    uint8_t curr_initialized;
} HashCDT;


//----------------------------------------------------------------------
// hash: Funcion de Hasheo
//----------------------------------------------------------------------
static uint64_t hash(uint64_t value){
    return value % SIZE;
}

//----------------------------------------------------------------------
// new_hashADT: Crea una nueva tabla de hash
//----------------------------------------------------------------------
// Argumentos:
//  pre_hash: la funcion de prehasheo
//  cmp: la funcion de comparacion para las listas
//----------------------------------------------------------------------
// Retorno:
//  En caso de error o si no se indica una funcion cmp o prehash, devuelve NULL
//  En caso de exito devuelve una nueva tabla
//----------------------------------------------------------------------
HashADT new_hashADT(prehash_function pre_hash, compare_function cmp){
    if(pre_hash == NULL || cmp == NULL){
        return NULL;
    }
    HashADT table = mm_alloc(sizeof(HashCDT));
    if(table == NULL){
        return NULL;
    }
    table->pre_hash = pre_hash;

    orderListADT aux;
    for(uint8_t i = 0; i < SIZE; i++){
        aux = new_orderListADT(cmp);
        if(aux == NULL){
            for(uint8_t j = 0; j < i; j++){
                free_orderListADT(table->lists[j]);
            }
            free_hashADT(table);
            return NULL;
        }
        table->lists[i] = aux;
    }
    table->iterator_index = 0;
    table->curr_initialized = 0;
    return table;
}

//----------------------------------------------------------------------
// hashADT_delete: Elimina un elemento de la tabla
//----------------------------------------------------------------------
// Argumentos:
//  table: la tabla sobre la cual eliminar
//  elem: el elemento a eliminar
//----------------------------------------------------------------------
// Retorno:
//  Devuelve el elemento si lo encuentra, NULL si no estaba
//----------------------------------------------------------------------
void * hashADT_delete(HashADT table, void * elem){
    if(table == NULL || elem == NULL){
        return NULL;
    }

    uint64_t value = table->pre_hash(elem);
    uint8_t list = hash(value);

    return orderListADT_delete(table->lists[list], elem);

}

//----------------------------------------------------------------------
// hashADT_add: Agrega un elemento de la tabla
//----------------------------------------------------------------------
// Argumentos:
//  table: la tabla sobre la cual agregar
//  elem: el elemento a agregar
//----------------------------------------------------------------------
// Retorno:
//  Devuelve 1 si lo pudo agregar
//  Devuelve -1 si no lo pudo agregar
//----------------------------------------------------------------------
int8_t hashADT_add(HashADT table, void * elem){
    if(table == NULL || elem == NULL){
        return -1;
    }

    uint64_t value = table->pre_hash(elem);
    uint8_t list = hash(value);

    return orderListADT_add(table->lists[list], elem);
}

//----------------------------------------------------------------------
// hashADT_add: Edita un elemento de la tabla
//----------------------------------------------------------------------
// Argumentos:
//  table: la tabla sobre la cual editar
//  prev_elem: el elemento a actualizar
//  new_elem: el nuevo elemento por el cual reemplazar
//  add_response: indica si se pudo agregar el nuevo elemento (1) o no (-1)
//----------------------------------------------------------------------
// Retorno:
//  Devuelve el elemento si lo encuentra, NULL si no estaba
//----------------------------------------------------------------------
void * hashADT_edit(HashADT table, void * prev_elem, void * new_elem, int8_t * add_response){
    *add_response = -1;

    if(table == NULL || prev_elem == NULL || new_elem == NULL){
        return NULL;
    }

    uint64_t prev_value = table->pre_hash(prev_elem);
    uint8_t prev_list = hash(prev_value);
    void * response = NULL;

    response = orderListADT_delete(table->lists[prev_list], prev_elem);

    // Si el elemento no existia, no deberia agregar el nuevo
    if(response != NULL){
        uint64_t new_value = table->pre_hash(new_elem);
        uint8_t new_list = hash(new_value);

        *add_response = orderListADT_add(table->lists[new_list], new_elem);
    }

    return response;
}

//----------------------------------------------------------------------
// hashADT_get: Devuelve un elemento de la tabla
//----------------------------------------------------------------------
// Argumentos:
//  table: la tabla sobre el cual buscar
//  elemId: el id del elemento a buscar
//----------------------------------------------------------------------
// OJO: Se basa en la funcion de prehasheo y comparacion solicitada en la creacion de la tabla
//----------------------------------------------------------------------
// Retorno:
//  El elemento si lo encuentra
//  NULL si no
//----------------------------------------------------------------------
void * hashADT_get(HashADT table, void * elem_id){
    if(table == NULL || elem_id == NULL){
        return NULL;
    }

    uint64_t value = table->pre_hash(elem_id);
    uint8_t list = hash(value);

    return orderListADT_get(table->lists[list], elem_id);
}

//----------------------------------------------------------------------
// hashADT_is_empty: Indica si la tabla esta vacia
//----------------------------------------------------------------------
// Argumentos:
//  table: la tabla
//----------------------------------------------------------------------
// Retorno:
//  0 si no esta vacio
//  !0 si esta vacio
//----------------------------------------------------------------------
uint8_t hashADT_is_empty(HashADT table){
    return hashADT_size(table) == 0;
}

//----------------------------------------------------------------------
// hashADT_size: Devuelve la cantidad de elementos en la tabla
//----------------------------------------------------------------------
// Argumentos:
//  table: la tabla
//----------------------------------------------------------------------
// Retorno:
//  Devuelve la cantidad de elementos en la tabla
//----------------------------------------------------------------------
uint32_t hashADT_size(HashADT table){
    uint32_t total = 0;
    for(int i = 0; i < SIZE; i++){
        total += orderListADT_size(table->lists[i]);
    }
    return total;
}


//----------------------------------------------------------------------
// Funciones para Iterador del Hash
//----------------------------------------------------------------------
void hashADT_to_begin(HashADT table){
    table->iterator_index = 0;
    table->curr_initialized = 0;
}
int hashADT_has_next(HashADT table){
    while (table->iterator_index<SIZE){
        if(!table->curr_initialized){
            orderListADT_toBegin(table->lists[table->iterator_index]);
            table->curr_initialized = 1;
        }
        if(orderListADT_hasNext(table->lists[table->iterator_index])==1){
            return 1;
        }
        (table->iterator_index)++;
        table->curr_initialized = 0;
    }
    return 0;
}
void* hashADT_next(HashADT table){
    return orderListADT_next(table->lists[table->iterator_index]);
}


//----------------------------------------------------------------------
// free_hashADT: Destruye la tabla
//----------------------------------------------------------------------
// Argumentos:
//  table: la tabla a eliminar
//----------------------------------------------------------------------
// Retorno:
//----------------------------------------------------------------------
void free_hashADT(HashADT table){
    for(uint8_t i=0; i < SIZE; i++){
        free_orderListADT(table->lists[i]);
    }
    mm_free(table);
}


