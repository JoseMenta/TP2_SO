/* _loader.c */
#include <stdint.h>

// Las variables sin inicializar o inicializadas con 0, se definen en el segmento Better Save Space (BSS)
// Estas dos variables, al no estar inicializadas, se crearan en dicho segmento
// El propósito de estas variables es indicar el comienzo y el fin del segmento
// Es decir, todas las variables que se deseen que esten en bss, deben estar definidas entre estas dos variables
// De esta manera, se tiene un delimitador del segmento
extern char bss;					// Inicio del segmento BSS
extern char endOfBinary;			// Fin del segmento BSS

int main(uint64_t arg_c, char** arg_v);

void * memset(void * destiny, int32_t c, uint64_t length);

//Agregamos los parametros para pasarlos a main
int _start(uint64_t arg_c, char** arg_v) {
    //ESTO CAMBIA A LOS PARAMETROS QUE RECIBE MAIN
    //NO VAMOS A PODER PASAR PARAMETROS A MAIN DIRECTO DESDE KERNEL, HAY QUE PASAR POR START
	//Clean BSS: Al setear en 0, desde el inicio hasta el final del segmento BSS, este se limpia
	memset(&bss, 0, &endOfBinary - &bss);

	return main(arg_c,arg_v);

}

// Dada una direccion de memoria d, un entero z y una longitud n, setea el valor z desde [d] hasta [d+n-1]
void * memset(void * destiation, int32_t c, uint64_t length) {
	uint8_t chr = (uint8_t)c;
	char * dst = (char*)destiation;

	while(length--)
		dst[length] = chr;

	return destiation;
}
