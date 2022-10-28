#include <exceptions.h>
#include <scheduler.h>
#include <video_driver.h>
#include <syscalls.h>
#include <video_driver.h>
#include <interrupts.h>
#include <syscalls.h>
#include <lib.h>
exception exceptions[] = {zero_division, 0,0,0,0,0, invalid_opcode};              // Arreglo de punteros a funcion de excepciones

static const char * Names[] = { "R8: ", "R9: ", "R10: ", "R11: ", "R12: ", "R13: ", "R14: ", "R15: ", "RAX: ", "RBX: ", "RCX: ", "RDX: ", "RSI: ", "RDI: ", "RBP: ", "RSP: ", "RIP: ", "FLAGS: "};
//----------------------------------------------------------------------
// excepitonDispatcher: funcion auxiliar que llama a los handlers de las excecpiones
//----------------------------------------------------------------------
// Argumentos:
//  exception: el numero de la excepcion
//----------------------------------------------------------------------
void exceptionDispatcher(int exception) {
	exceptions[exception]();
    terminate_process(1);//Matamos al programa que lanzo la excepcion
	return;
}

//-----------------------------------------------------------------------
// zero_division: Excepcion ejecutada al realizarse una division por cero (exceptionID = 0)
//-----------------------------------------------------------------------
void zero_division() {
	write_handler("EXCEPCION generada: Division por cero\n", RED);
	print_registers();

}



//-----------------------------------------------------------------------
// zero_division: Excepcion ejecutada al utilizar un operador invalido (exceptionID = 6)
//-----------------------------------------------------------------------
void invalid_opcode() {
	write_handler("EXCEPCION generada: Invalid opcode\n", RED);
	print_registers();
    //write_handler("\n\nPulse ESC para volver a consola", WHITE);
}



//-----------------------------------------------------------------------
// print_registers: Imprime el estado de los registros
//-----------------------------------------------------------------------
// Argumentos:
//  void
//-----------------------------------------------------------------------
void print_registers(){
	write_handler("Registros al momento de la excepcion: \n", RED);
	// Pongo en reg los valores de los registros
//	uint64_t * reg = get_registers();
    uint64_t * reg = getCurrContext();
    char reg_str[20];
    // Al llamar a write_handler, se consulta la posicion del proceso que se encuentra corriendo actualmente
    // y lo imprime en dicha posicion
	for(int i=0; i<=RFLAGS; i++){
		write_handler(Names[i], RED);
		to_hex(reg_str, reg[i]);
        write_handler(reg_str,RED);
        write_handler("\n", RED);
	}
}