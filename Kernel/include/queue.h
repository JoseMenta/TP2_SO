

#ifndef TPE_QUEUE_H
#define TPE_QUEUE_H

#include <stdint.h>
//TODO: cambiar por pipes
#define MAX 100
//NOTA: Si no tiene mas espacio en la cola para agregar, no agrega al elemento y se queda con los que ya estaban
//Esto hace que si se escribe mucho y se supera al buffer, algunos caracteres no se guarden y se vuelven a guardar recien
//cuando se sacan algunos leyendo
//Estructura utilizada para representar una cola circular
typedef struct{
    int elems[MAX];
    int first;
    int last;
    int len;
}queue_t;

void enqueue(queue_t* queue, int elem); //agrega un elemento a la cola
uint8_t dequeue(queue_t* queue);        //elimina un elemento de la cola, en el orden FIFO: first in, first out
uint8_t is_empty(queue_t* queue);       //devuelve true si la cola esta vacia
void clear_queue(queue_t* queue);       //elimina todos los elementos de la cola
#endif //TPE_QUEUE_H
