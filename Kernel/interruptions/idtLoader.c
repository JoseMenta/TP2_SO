// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <stdint.h>
#include <idtLoader.h>
#include <defs.h>
#include <interrupts.h>

//#pragma == una instruccion para el compilador
#pragma pack(push)		/* Push de la alineación actual */
#pragma pack (1) 		/* Alinear las siguiente estructuras a 1 byte */

/* Descriptor de interrupcion */
typedef struct {
  uint16_t offset_l, selector;
  uint8_t cero, access;
  uint16_t offset_m;
  uint32_t offset_h, other_cero;
} DESCR_INT;

#pragma pack(pop)		/* Reestablece la alinceación actual */



DESCR_INT * idt = (DESCR_INT *) 0;	// IDT de 255 entradas: Indica la direccion de memoria donde comienza la tabla de interrupciones (en este caso, en la direccion 0x00)
//La IDT esta al principio del mapa de memoria virtual
static void setup_IDT_entry (int index, uint64_t offset);
//----------------------------------------------------------------------
// load_idt: carga las rutinas de atencion de las interrupciones y excepciones en la IDT
//----------------------------------------------------------------------
// Argumentos:
//  void
//----------------------------------------------------------------------
// En esta funcion se deben cargar las interrupciones y las excepciones que se desean utilizar
// Para ello, se debe asociar el codigo de interrupcion/excepcion con la rutina de atencion que se desea ejecutar si se lanza
//----------------------------------------------------------------------
void load_idt() {
  _cli();
  //colocar todas las entradas a la IDT que queremos
  //excepciones o interupciones, segun la tabla de la catedra
  setup_IDT_entry (0x20, (uint64_t)&_irq00Handler);         // La interrupcion para el timer tick es la 0x20
  setup_IDT_entry (0x21, (uint64_t)&_irq01Handler);         // La interrupcion para el teclado es la 0x21
  setup_IDT_entry (0x00, (uint64_t)&_exception0Handler);    // La excepcion para la division por 0 es la 0x00
  setup_IDT_entry (0x06, (uint64_t)&_exception6Handler);    // La excepcion para operador invalido es la 0x06
  setup_IDT_entry (0x80, (uint64_t)&_syscallHandler);       // Manejo de syscalls 0x80


	// Habilitamos el timer tick (IRQ0) y el teclado (IRQ1)
	picMasterMask(0xFC);
	picSlaveMask(0xFF);
        
	_sti();                                                   // Habilita las interrupciones enmascarables
}
//----------------------------------------------------------------------
// setup_IDT_entry: carga la rutuina de atencion a la IDT
//----------------------------------------------------------------------
// Argumentos:
//  index: la posicion en la IDT donde se desea cargar la rutina
//  offset: la direccion de la rutina de atencion
//----------------------------------------------------------------------
// En esta funcion se carga la interrupcion/excepcion (IRQ code / ID code) asociado con su respectiva rutina de atencion (puntero a funcion)
//----------------------------------------------------------------------
static void setup_IDT_entry (int index, uint64_t offset) {
  idt[index].selector = 0x08;
  idt[index].offset_l = offset & 0xFFFF;
  idt[index].offset_m = (offset >> 16) & 0xFFFF;
  idt[index].offset_h = (offset >> 32) & 0xFFFFFFFF;
  idt[index].access = ACS_INT;
  idt[index].cero = 0;
  idt[index].other_cero = (uint64_t) 0;
}
