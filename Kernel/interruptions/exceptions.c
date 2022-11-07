// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <exceptions.h>
#include <scheduler.h>
#include <pipes.h>
#include <syscalls.h>
#include <video_driver.h>
#include <interrupts.h>
#include <lib.h>


exception exceptions[] = {zero_division, 0,0,0,0,0, invalid_opcode};              // Arreglo de punteros a funcion de excepciones


//----------------------------------------------------------------------
// excepitonDispatcher: funcion auxiliar que llama a los handlers de las excecpiones
//----------------------------------------------------------------------
// Argumentos:
//  exception: el numero de la excepcion
//----------------------------------------------------------------------
void exceptionDispatcher(int exception) {
	exceptions[exception]();
    exit_handler();
	return;
}

//-----------------------------------------------------------------------
// zero_division: Excepcion ejecutada al realizarse una division por cero (exceptionID = 0)
//-----------------------------------------------------------------------
void zero_division() {
	write(STDERR, "EXCEPCION generada: Division por cero\n", 39);

}


//-----------------------------------------------------------------------
// zero_division: Excepcion ejecutada al utilizar un operador invalido (exceptionID = 6)
//-----------------------------------------------------------------------
void invalid_opcode() {
	write(STDERR, "EXCEPCION generada: Invalid opcode\n", 36);
}


