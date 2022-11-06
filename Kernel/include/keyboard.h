
#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdint.h>
//Constantes para los scan_code
#define BUFF_LENGTH 100
#define BLOCK_MAYUSC 58
#define SHIFT1 42
#define SHIFT2 54
#define CTRL 29
#define ALT 56
#define ESC 1
#define RELEASED 128
#define ASCII_DELETE 127
#define EOF (-1)
#define UPPER_OFFSET ('a'-'A')  //0x20
#define KEYBOARD_REFERENCE_LENGTH 58

void keyboard_handler();
uint8_t get_keyboard_scan_code();               // Funcion hecha en assembler para interactuar con el teclado

#endif
