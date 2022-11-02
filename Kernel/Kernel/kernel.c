#include <stdint.h>
#include <string.h>
#include <lib.h>
#include <moduleLoader.h>
#include <naiveConsole.h>
#include <idtLoader.h>
#include <video_driver.h>
#include <mm.h>
#include <scheduler.h>
#include "../include/scheduler.h"
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
void testMM(){
    uint8_t * dir = mm_alloc(10);
    if(dir<=&endOfKernel){
        ncPrint("Error1: la direccion de salida es menor a la inicial");
        ncNewline();
    }
    *dir='1';
    uint8_t* dir2 = mm_alloc(20);
    if(dir>=dir2){
        ncPrint("Error2: la segunda direccion que se dio fue menor o igual a la primera");
        ncNewline();
    }
    *dir2 = '2';
//    ncPrint("Dir: ");
//    ncPrintHex((uint64_t)dir);
//    ncPrint("Dir2: ");
//    ncPrintHex((uint64_t)dir2);
    if(*dir!='1'){
        ncPrint("Error: No se preseva el valor escrito en la direccion dir");
        ncNewline();
    }
    if(*dir2!='2'){
        ncPrint("Error: No se preseva el valor escrito en la direccion dir2");
        ncNewline();
    }
    ncNewline();
    mm_free(dir);
    uint8_t* dir3 = mm_alloc(8);
    if(dir3!=dir){
        ncPrint("Error3: el MM no esta reutilizando los bloques liberados");
        ncNewline();
        ncPrint("Dir: ");
        ncPrintHex((uint64_t)dir);
        ncPrint("Dir3: ");
        ncPrintHex((uint64_t)dir3);
        ncNewline();
    }
    uint8_t* dir4 = mm_alloc(30);
    uint8_t* dir5 = mm_alloc(40);
    mm_free(dir4);
    mm_free(dir5);
    uint8_t * dir6 = mm_alloc(70);
    if(dir4!=dir6){
        ncPrint("Error4: el MM no esta manejando bien la fragmentacion (union de derecha)");
        ncNewline();
        ncPrint("Dir4: ");
        ncPrintHex((uint64_t)dir4);
        ncPrint("Dir5: ");
        ncPrintHex((uint64_t)dir5);
        ncPrint("Dir6: ");
        ncPrintHex((uint64_t)dir6);
        ncNewline();
    }
    uint8_t* dir7 = mm_alloc(100);
    //Libero todo para ver si junta los bloques liberados de adelante y atras
    if(*dir2!='2'){
        ncPrint("Error: No se preseva el valor escrito en la direccion dir2");
        ncNewline();
    }
    mm_free(dir2);
    mm_free(dir7);
    mm_free(dir3);
    mm_free(dir6);//espero que esto se junte con los bloques 3 y 7
    uint8_t* dir8 = mm_alloc(200);
    if(dir8!=dir3){
        ncPrint("Error5: el MM no esta manejando bien la fragmentacion (ambas uniones)");
        ncNewline();
        ncPrint("Dir3: ");
        ncPrintHex((uint64_t)dir3);
        ncPrint("Dir8: ");
        ncPrintHex((uint64_t)dir8);
    }
    mm_free(dir8);
    uint8_t* dir9 = mm_alloc(10);
    uint8_t* dir10 = mm_alloc(20);
    mm_free(dir9);
    mm_free(dir10); //espero que este se junte con el bloque 9
    uint8_t* dir11 = mm_alloc(30);
    if(dir9!=dir11){
        ncPrint("Error6: el MM no esta manejando bien la fragmentacion (union de izquierda)");
        ncNewline();
        ncPrint("Dir9: ");
        ncPrintHex((uint64_t)dir9);
        ncPrint("Dir11: ");
        ncPrintHex((uint64_t)dir11);
    }
    if(dir!=dir11){
        ncPrint("Error7: el MM no esta reutilizando bloques liberados");
        ncNewline();
        ncPrint("Dir: ");
        ncPrintHex((uint64_t)dir);
        ncPrint("Dir11: ");
        ncPrintHex((uint64_t)dir11);
    }
    if(dir2<&endOfKernel ||dir3<&endOfKernel ||dir4<&endOfKernel || dir5<&endOfKernel ||dir6<&endOfKernel ||dir7<&endOfKernel ||dir8<&endOfKernel ||dir9<&endOfKernel ||dir10<&endOfKernel ||dir11<&endOfKernel){
        ncPrint("Error8: el MM esta yendo antes de la direccion inicial");
        ncNewline();
    }
    uint8_t * dir12 = mm_alloc(0x400000);
    if(dir12!=NULL){
        ncPrint("Error9: el MM esta dando memoria que no tiene");
        ncNewline();
    }
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
//    mm_init();
    testMM();
//    print("Pasa el testeo del mm\n",WHITE,ALL);

    initialize_scheduler();
    executable_t exec;
    char* aux[]={NULL};
    exec.arg_c=0;
    exec.arg_v=aux;
    exec.name = "Shell";
    exec.foreground = 1;
    exec.start=sampleCodeModuleAddress;
    create_process(&exec);
//    aca habilitamos las interrupciones, para que el scheduler ya tenga a donde ir en la primera
    load_idt();

//    ((EntryPoint)sampleCodeModuleAddress)();
    while(1);//para que espere hasta el tt y se vaya al bash
    print("Computadora apagada",WHITE,ALL);
    clear(ALL);

	return 0;
}
