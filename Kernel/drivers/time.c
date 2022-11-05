#include <time.h>
#include <scheduler.h>
#include <stdint.h>
#include <orderListADT.h>
#include <mm.h>
//TODO: sacar
#include "../include/orderListADT.h"
#include "../include/mm.h"
#include "../include/scheduler.h"
uint64_t ticks = 0;
extern uint64_t scheduler_ticks;

typedef struct{
    uint64_t pid;
    uint64_t final_tick; //el tick donde termina el timer
}timer_t;
//Se ordenan ascendentemente por final_tick y luego por pid
int64_t compare_timer(void* e1, void* e2){
    timer_t* t1 = e1;
    timer_t* t2 = e2;
    if(t1->final_tick==t2->final_tick){
        return t1->pid - t2->pid;
    }
    return t1->final_tick - t2->final_tick;
}
orderListADT pending_timers = NULL;

//----------------------------------------------------------------------
// timer_handler: handler para timer tick
//----------------------------------------------------------------------
// Argumentos
//  void
//----------------------------------------------------------------------
// La funcion incrementa un contador de ticks (que se utiliza para pausar
// algunos procesos) y llama al scheduler para cambiar el contexto
//----------------------------------------------------------------------
void timer_handler() {							    // Es la funcion que se ejecuta cuando ocurra la interrupcion del timer tick (Incrementa la cantidad de ticks)
	ticks++;
    scheduler_ticks++;
    if(pending_timers==NULL){
        pending_timers = new_orderListADT(compare_timer);
    }
    orderListADT_toBegin(pending_timers);
    timer_t * next = NULL;
    //mientras haya elementos y el que sigue tiene que salir ahora
    while (orderListADT_hasNext(pending_timers) && ticks>=(next = orderListADT_next(pending_timers))->final_tick){
        unblock_process(next->pid);
        //lo sacamos de la lista de pendientes
        orderListADT_delete(pending_timers,next);
        orderListADT_toBegin(pending_timers);//volvemos al primer elemento
    }
}

int32_t add_timer(uint64_t wanted_ticks){
    timer_t* aux = mm_alloc(sizeof (timer_t));
    if(aux==NULL){
        return -1;
    }
    aux->pid = get_current_pid();
    aux->final_tick = (wanted_ticks+ticks+1);//+1 para corregir la interrupcion siguiente
    orderListADT_add(pending_timers,aux);
    block_process(get_current_pid());
    return 0;
}
uint64_t ticks_elapsed() {							// Devuelve la cantidad de veces que se ejecuto la interrupcion del timer tick desde que se inicio el sistema
	return ticks;
}

uint64_t seconds_elapsed() {					    // Devuelve la cantidad de segundos que transcurrieron desde que se inicio el sistema (Se realiza un tick cada 18,2 segs)
	return ticks / 18;
}
