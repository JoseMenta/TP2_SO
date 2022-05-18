#include <time.h>
#include <stdint.h>

static void int_20();

// Es la funcion que recibira el codigo de la interrupcion y, con ello, llamar a su rutina de atencion
void irqDispatcher(uint64_t irq) {
	//segun el parametro que tiro el IrqHandler cae en algun case
	//eso te lleva a la funcion posta donde defino que hace la irq
	switch (irq) {
		case 0:
			int_20();
			break;
	}
	return;
}

// La interrupcion 0x20 es la del timer tick -> Llamamos a la rutina de atencion del mismo
void int_20() {
	timer_handler();
}
