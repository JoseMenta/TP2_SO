#include <naiveConsole.h>

static uint32_t uintToBase(uint64_t value, char * buffer, uint32_t base);

static char buffer[64] = { '0' };						// Se cuenta con un buffer de 64 caracteres
static uint8_t * const video = (uint8_t*)0xB8000;			// La placa de video comienza en la direccion 0xB8000
static uint8_t * currentVideo = (uint8_t*)0xB8000;			// Indica la posicion actual donde se esta imprimiendo por pantalla
static const uint32_t width = 80;							// La pantalla es una matriz de 80x25 caracteres (largoXalto)
static const uint32_t height = 25;

void ncPrint(const char * string)							// Imprime un string pasado por parametros
{
	int i;

	for (i = 0; string[i] != 0; i++)
		ncPrintChar(string[i]);
}

void ncPrintChar(char character)							// Imprime un char pasado por parametros
{
	*currentVideo = character;
	currentVideo += 2;										// Se suma 2 pues la posicion siguiente a la de un caracter se define el formato con la que se imprime (color de fondo y letra) 
}

void ncNewline()											// Realiza un salto de linea
{
	do
	{
		ncPrintChar(' ');									// Basicamente es imprimir un espacio en los caracteres que falten para completar la linea
	}
	while((uint64_t)(currentVideo - video) % (width * 2) != 0);
}

void ncPrintDec(uint64_t value)								// Imprime un numero en formato decimal
{
	ncPrintBase(value, 10);
}

void ncPrintHex(uint64_t value)								// Imprime un numero en formato hexadecimal
{
	ncPrintBase(value, 16);
}

void ncPrintBin(uint64_t value)								// Imprime un numero en formato binario
{
	ncPrintBase(value, 2);
}

void ncPrintBase(uint64_t value, uint32_t base)				// Imprime un numero en base pasado por parametros
{
    uintToBase(value, buffer, base);						// Guarda el string del numero en el buffer
    ncPrint(buffer);
}

void ncClear()												// Borra todo lo que aparezca en pantalla
{
	int i;

	for (i = 0; i < height * width; i++)
		video[i * 2] = ' ';									// Basicamente es imprimir un espacio en todos los caracteres de la pantalla
	currentVideo = video;
}

static uint32_t uintToBase(uint64_t value, char * buffer, uint32_t base)
{
	char *p = buffer;
	char *p1, *p2;
	uint32_t digits = 0;

	//Calculate characters for each digit
	do
	{
		uint32_t remainder = value % base;
		*p++ = (remainder < 10) ? remainder + '0' : remainder + 'A' - 10;
		digits++;
	}
	while (value /= base);

	// Terminate string in buffer.	(El \0 del string)
	*p = 0;

	//Reverse string in buffer. (Notar que al hacer el pasaje de int a char, se tiene el numero al reves pues se analiza de derecha a izquierda)
	p1 = buffer;
	p2 = p - 1;
	while (p1 < p2)
	{
		char tmp = *p1;
		*p1 = *p2;
		*p2 = tmp;
		p1++;
		p2--;
	}

	return digits;
}
