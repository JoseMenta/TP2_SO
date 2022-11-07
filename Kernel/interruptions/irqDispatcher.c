#include <time.h>
#include <stdint.h>
#include <keyboard.h>
#include <naiveConsole.h>

static void int_20();
static void int_21();
typedef void (* handler) ();

handler handlers[2] = {int_20, int_21};              // Arreglo de punteros a funcion de interrupciones

//----------------------------------------------------------------------
// irqDispatcher: funcion para llamar a los handlers de las interrupciones
//----------------------------------------------------------------------
// Argumentos:
//  irq: numero de la interrupcion
//----------------------------------------------------------------------
// Esta funcion llama a las rutinas de atencion para las interrupciones de
// teclado o timer tick. No hace algo en el caso donde se llame para otra
// interrupcion
//----------------------------------------------------------------------
void irqDispatcher(uint64_t irq) {
    //Si no tenemos definido el handler, retorna
    if(irq>1) return;
	//segun el parametro que tiro el IrqHandler cae en algun case
    //eso te lleva a la funcion posta donde defino que hace la irq
    handlers[irq]();
	return;
}

// Funcion para llamar a la rutina de atencion del timer tick
void int_20() {
	timer_handler();
}
// Funcion para llamar a la rutina de atencion del teclado
void int_21(){
    keyboard_handler();
}
