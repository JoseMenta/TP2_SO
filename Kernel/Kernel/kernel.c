// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <stdint.h>
#include <string.h>
#include <lib.h>
#include <moduleLoader.h>
#include <naiveConsole.h>
#include <idtLoader.h>
#include <video_driver.h>
#include <mm.h>
#include <scheduler.h>
#include <pipes.h>
#include "../include/scheduler.h"
#include "../include/pipes.h"
#include "../include/video_driver.h"
extern uint8_t text;
extern uint8_t rodata;
extern uint8_t data;
extern uint8_t bss;
extern uint8_t endOfKernelBinary;
extern uint8_t endOfKernel;

static const uint64_t PageSize = 0x1000;						// El tamaño de una pagina (en este caso, se definio que sea de 4KB)
int curr_process = 0;
static void * const sampleCodeModuleAddress = (void*)0x400000;	// Userland comienza en la direccion 0x400000
static void * const sampleDataModuleAddress = (void*)0x500000;

extern void TesterWrite();
extern void TesterRead();
extern void zero_division_exc();
typedef int (*EntryPoint)();									// Entrypoint es un alias para punteros a funcion que devuelven un entero, sin parametros

void print_defined();
#ifdef BUDDY
void print_defined(){
    print("buddy",WHITE,ALL);
}
#endif
#ifdef HEAP
void print_defined(){
    print("heap",WHITE,ALL);
}
#endif

void clearBSS(void * bssAddress, uint64_t bssSize)
{
	memset(bssAddress, 0, bssSize);
}

//TODO: preguntar si lo deberiamos hacer con el MM ahora, porque si no hay que considerarlo para la direccion inicial del MM
void * getStackBase()											// Devuelve el valor del registro RSP (direccion de memoria donde apunta el registro)
{
	return (void*)(
		(uint64_t)&endOfKernel
		+ PageSize * 8				//The size of the stack itself, 32KiB
		- sizeof(uint64_t)			//Begin at the top of the stack
	);
}
//Esto se llama en loader.asm
void * initializeKernelBinary()
{
	char buffer[10];

	ncPrint("[x64BareBones]");
	ncNewline();

	ncPrint("CPU Vendor:");
	ncPrint(cpuVendor(buffer));
	ncNewline();

	ncPrint("[Loading modules]");
	ncNewline();
	void * moduleAddresses[] = {
		sampleCodeModuleAddress,
		sampleDataModuleAddress
	};

	loadModules(&endOfKernelBinary, moduleAddresses);
	ncPrint("[Done]");
	ncNewline();
	ncNewline();

	ncPrint("[Initializing kernel's binary]");
	ncNewline();

	clearBSS(&bss, &endOfKernel - &bss);

	ncPrint("  text: 0x");
	ncPrintHex((uint64_t)&text);
	ncNewline();
	ncPrint("  rodata: 0x");
	ncPrintHex((uint64_t)&rodata);
	ncNewline();
	ncPrint("  data: 0x");
	ncPrintHex((uint64_t)&data);
	ncNewline();
	ncPrint("  bss: 0x");
	ncPrintHex((uint64_t)&bss);
	ncNewline();

	ncPrint("[Done]");
	ncNewline();
	ncNewline();
	return getStackBase();
}


int main()													// Es la primera funcion que se ejecutará una vez se halla cargado el SO en el sistema
{
//    load_idt();
	ncPrint("[Kernel Main]");
	ncNewline();
	ncPrint("  Sample code module at 0x");
	ncPrintHex((uint64_t)sampleCodeModuleAddress);
	ncNewline();
	ncPrint("  Calling the sample code module returned: ");
    //Aca enttra al sampleCodeModule.c
   // ncPrintHex(((EntryPoint)sampleCodeModuleAddress)());
	ncNewline();
	ncNewline();

	ncPrint("Sample data module at 0x");
	ncPrintHex((uint64_t)sampleDataModuleAddress);
	ncNewline();
	ncPrint("  Sample data module contents: ");
	ncPrint((char*)sampleDataModuleAddress);
	ncNewline();

	ncPrint("[Finished]");

    print_lines();
    ncClear();
//    print("Pasa el testeo del mm\n",WHITE,ALL);
    pipe_initialize();
    initialize_scheduler();
//    print_defined();
    executable_t exec;
    char* aux[]={NULL};
    exec.arg_c=0;
    exec.arg_v=aux;
    exec.name = "Shell";
    exec.foreground = 1;
    int aux2[3] = {-1,-1,-1};
    exec.fds = aux2;
    exec.start=sampleCodeModuleAddress;
    create_process(&exec);
    //    aca habilitamos las interrupciones, para que el scheduler ya tenga a donde ir en la primera
    load_idt();
    while(1);//para que espere hasta el tt y se vaya al bash
    //pipe_terminated();
    //((EntryPoint)sampleCodeModuleAddress)();
    print("Computadora apagada",WHITE,ALL);
    clear(ALL);

	return 0;
}
