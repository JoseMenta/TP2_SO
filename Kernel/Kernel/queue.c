// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <queue.h>

//-----------------------------------------------------------------------
// enqueue: Agrega el nuevo caracter presionada a la cola si queda espacio
//-----------------------------------------------------------------------
// Argumentos:
// - queue: Puntero a la cola
// - elem: El nuevo caracter a agregar
//-----------------------------------------------------------------------
// Retorno:
//  void
//-----------------------------------------------------------------------
void enqueue(queue_t* queue, int elem){
    if(queue->len == MAX)
        return;                                 //No lo puede agregar si no hay espacio

    // Si queda espacio, lo agrego a la proxima posicion, muevo el puntero de agregar
    // e incremento la cantidad de caracteres en el buffer
    queue->elems[queue->last] = elem;
    queue->last = (queue->last+1) % MAX;
    (queue->len)++;
}

//-----------------------------------------------------------------------
// is_empty: Indica si hay caracteres o no en la cola
//-----------------------------------------------------------------------
// Argumentos:
// - queue: Puntero a la cola
//-----------------------------------------------------------------------
// Retorno:
//  1 si no hay elementos, 0 si hay
//-----------------------------------------------------------------------
uint8_t is_empty(queue_t * queue){
   return queue->len==0;
}

//-----------------------------------------------------------------------
// dequeue: Toma el proximo caracter de la cola
//-----------------------------------------------------------------------
// Argumentos:
// - queue: Puntero a la cola
//-----------------------------------------------------------------------
// Retorno:
//  El valor ASCII del proximo caracter, o \0 si no hay caracteres disponibles
//-----------------------------------------------------------------------
uint8_t dequeue(queue_t* queue){
    if(queue->len==0)
        return '\0';                            // Si no hay caracteres para leer, guarda 0
    int ans = queue->elems[queue->first];
    (queue->len)--;
    queue->first = (queue->first + 1) % MAX;
    return ans;
}
//----------------------------------------------------------------------
// clear_queue: Elimina todos los elementos de la cola
//----------------------------------------------------------------------
// Argumentos:
//  -queue: Puntero a la cola
//----------------------------------------------------------------------
// Retorno
//  void
//----------------------------------------------------------------------
void clear_queue(queue_t* queue){
    queue->len = 0;
    queue->first = 0;
    queue->last = 0;
}
