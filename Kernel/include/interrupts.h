 /*
 *   interrupts.h
 *
 *  Created on: Apr 18, 2010
 *      Author: anizzomc
 */

#ifndef INTERRUPS_H_
#define INTERRUPS_H_
#define REGISTERS_COUNT (18)
#include <idtLoader.h>
#include <stdint.h>
void _irq00Handler(void);                   // Las siguientes son funciones que ejecutan la interrupcion cuyo codigo IRQ está indicado en el nombre
void _irq01Handler(void);
void _irq02Handler(void);
void _irq03Handler(void);
void _irq04Handler(void);
void _irq05Handler(void);

void _exception0Handler(void);              // Es una funcion que ejecuta la excepcion de id 0 (division por cero)
void _exception6Handler(void);              // Es una funcion que ejecuta la excepcion de id 6 (operador invalido)

void _syscallHandler(void);                 // Ejecuta las interrupciones de software
 void _int20(void);                         // Ejecuta la interrupcion 20 (Timer tick)
void _cli(void);                            // Ejecuta la instruccion cli, la cual inhabilita las interrupciones

void _sti(void);                            // Ejecuta la instruccion sti, la cual vuelve a habilitar las instrucciones

void _hlt(void);                            // Es una funcion que contiene la instruccion HLT (halt) la cual provoca que el procesador deje de ejecutar instrucciones hasta la proxima interrupcion externa (https://www.eeeguide.com/8086-external-hardware-synchronization-instructions/)

void picMasterMask(uint8_t mask);           // Permite modificar las interrupciones que se van a poder habilitar (0 = on, 1 = off), recibiendo por parametros un numero de 16 bits (aunque solo se utilicen los primeros 8), del pic maestro

void picSlaveMask(uint8_t mask);            // Permite modificar las interrupciones que se van a poder habilitar (0 = on, 1 = off), recibiendo por parametros un numero de 16 bits (aunque solo se utilicen los primeros 8), del pic esclavo

//Termina la ejecución de la cpu.
void haltcpu(void);
uint64_t * getCurrContext(void);            // Funcion para obtener el arreglo curr_context declarado en interrupts.asm
#endif /* INTERRUPS_H_ */
