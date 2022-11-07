// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <mm.h>
#include <stdint.h>
#include <stdlib.h>

#ifdef HEAP
//La posicion desde donde reservamos memoria
extern uint8_t endOfKernel;

static const uint8_t* start_address = (uint8_t*) 0x600000;
static const uint8_t* end_address = (uint8_t*)0x800000;
static uint64_t allocated_bytes=0;
static uint64_t allocated_blocks = 0;
static uint64_t heap_size = 0;
//Estructura para manejar la lista de bloques libres
typedef struct block{
    struct block* next_free_block;
    uint32_t block_size;
}block_t;

static uint64_t block_size = sizeof (block_t);
static void insert_block_into_free_list(block_t* new_block);
//Lo minimo que va a ocupar un bloque es el doble de lo que ocupa la estructura
//Lo tengo para ver si me conviene dividir a un bloque si es muy grande
#define MIN_BLOCK_SIZE (block_size<<1)
//El inicio de la lista, sirve solo para apuntera al primer nodo
// La idea de hacerlo asi y no con el primer bloque de memoria directamente
// es para tener la refencia siempre, ya que si guardamos al primer bloque libre
// perderemos la referencia cuando este se tome
// Ademas, sirve para no hacer casos especiales cuando se saca el primer nodo y eso
static block_t start_block;
//El fin de la lista
static block_t* end_block=NULL;
void mm_init(){
    block_t * first_block;

    start_block.next_free_block = (void*) start_address;
    start_block.block_size = 0;

    //Ubicamos al utlimo bloque en las ultimas direcciones (justo antes de la ultima direccion)
    //Le restamos block_size para poder usarlo como un block_t
    end_block = (block_t* )(end_address - block_size);
    end_block->block_size=0;
    end_block->next_free_block=NULL;

    first_block = (block_t*) start_address;
    first_block->block_size = (((uint8_t*)end_block)-((uint8_t*)start_address));
    heap_size = first_block->block_size;
    first_block->next_free_block = end_block;
}
//----------------------------------------------------------------------
// mm_alloc: reserva memoria
//----------------------------------------------------------------------
// Argumentos:
//  wanted_size: la cantidad de memoria que se desea reservar
//----------------------------------------------------------------------
// Es la funcion del Memory Manager encargada de reservar memoria
//----------------------------------------------------------------------
// Retorno:
//  Si se encuentra espacio, se devuelve la direccion inicial. En caso de no encontrar
//  espacio, se devuelve NULL
//----------------------------------------------------------------------
void* mm_alloc(uint32_t wanted_size){
    //Si no esta seteado el entorno para el mm, tengo que inicializar al mm
    if(end_block==NULL){
        mm_init();
    }
    if(wanted_size==0){
        return NULL;
    }

//    if(wanted_size>0){
    if(wanted_size+block_size>heap_size-allocated_bytes){
        return NULL;
    }
    wanted_size+=block_size;
//    }
    //Ya sabemos que deberia haber espacio (si manejamos bien la fragmentacion)
    block_t* curr = start_block.next_free_block;
    block_t* prev = &start_block;
    void* ans = NULL;
    //Usamos un algoritmo First Fit, le damos el primer bloque libre que encontramos
    //Mientras haya bloques y el de ahora no sea lo suficientemente grande
    while( (curr->block_size<wanted_size) && (curr->next_free_block!=NULL)){
        prev = curr;
        curr = curr->next_free_block;
    }
    //Si llegamos al bloque final, entonces no encontramos un bloque lo suficientemente grande
    if(curr==end_block){
        return NULL;
    }
    //Le damos la posicion siguente a cuando termina la estructura del bloque (block_t)
    //OJO con estas sumas, hay que castear a uint8_t!!!
    //TODO: revisar este cambio, me lo dijo PVS
    ans = (void *) (((uint8_t*)prev->next_free_block) + block_size);


    //Sacamos al bloque de la lista de libres
    prev->next_free_block = curr -> next_free_block;
    //Si lo que sobra ya puede ser otro bloque
    if((curr->block_size-wanted_size)>MIN_BLOCK_SIZE){
        //creo otro bloque para lo que queda
        block_t * new_block = (void*)(((uint8_t*)curr)+wanted_size);
        //Le doy lo que queda del bloque que estoy dejando
        new_block->block_size = curr->block_size-wanted_size;
        curr->block_size = wanted_size;//Hago que el que dejo tenga justo lo que piden
        insert_block_into_free_list(new_block);
    }
    allocated_bytes += curr->block_size;
    allocated_blocks++;

    curr->next_free_block = NULL;
    return ans;

}
//----------------------------------------------------------------------
// mm_free: libera memoria reservada por el mm
//----------------------------------------------------------------------
// Argumentos:
//  p: puntero a la posicion inicial de la memoria otorgada por el mm
//----------------------------------------------------------------------
// Es la funcion del Memory Manager encargada de liberar memoria
//----------------------------------------------------------------------
// Retorno:
//  void
//----------------------------------------------------------------------
void mm_free(void* p){
    if(p==NULL){
        return;
    }
    //Ojo, hay que castear a p a un (uint8_t*) por la aritmetica de punteros
    block_t* block = (void*)(((uint8_t*)p)-block_size);
    if(block->next_free_block!=NULL){
        return;
    }
    allocated_bytes -= block->block_size;
    insert_block_into_free_list(block);
    allocated_blocks--;
}

uint64_t get_total_bytes(){
    return heap_size;
}

uint64_t get_allocated_blocks(){
    return allocated_blocks;
}
uint64_t get_allocated_bytes(){
    return allocated_bytes;
}
uint64_t get_free_bytes(){
    return heap_size-allocated_bytes;
}

static void insert_block_into_free_list(block_t* new_block){
    block_t* iterator;
    //avanzamos hasta la direccion donde hay que ingresarlo (los bloques estan ordenados ascendentemente)
    //miro siempre al proximo bloque del iterador, para terminar con el iterador siendo el bloque anterior
    for(iterator = &start_block; iterator->next_free_block<new_block; iterator=(iterator->next_free_block));
    //iterator va a tener el bloque que viene antes del que estamos agregando
    //lo casteo a uint8_t* para que funcione bien la aritmetica de punteros
    uint8_t* it_address = (uint8_t*) iterator;
    //Si el que insertamos y el bloque que viene antes que el estan contiguos
    if( (it_address + iterator->block_size) == ((uint8_t*)new_block)){
        //Lo hacemos un bloque mas grande
        iterator->block_size += new_block->block_size;
        new_block = iterator;
    }
    uint8_t* new_address = (uint8_t*) new_block;
    //Si el que insertamos y el que viene despues son configuos
    if( (new_address + new_block->block_size) == ((uint8_t*)iterator->next_free_block)){
        if(iterator->next_free_block!=end_block){
            new_block->block_size += (iterator->next_free_block->block_size);
            new_block->next_free_block = iterator->next_free_block->next_free_block;
        }else{
            new_block->next_free_block = end_block;
        }
    }else{
        new_block->next_free_block = iterator->next_free_block;
    }

    //Si no se junto al iterador con el que se estaba agregando, se cambia el puntero del iterador
    if(iterator!=new_block){
        iterator->next_free_block = new_block;
    }
}
#endif