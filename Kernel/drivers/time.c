#include <time.h>
#include <scheduler.h>
#include <stdint.h>
uint64_t ticks = 0;
extern uint64_t scheduler_ticks;
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
//    change_context();
}

uint64_t ticks_elapsed() {							// Devuelve la cantidad de veces que se ejecuto la interrupcion del timer tick desde que se inicio el sistema
	return ticks;
}

uint64_t seconds_elapsed() {					    // Devuelve la cantidad de segundos que transcurrieron desde que se inicio el sistema (Se realiza un tick cada 18,2 segs)
	return ticks / 18;
}
