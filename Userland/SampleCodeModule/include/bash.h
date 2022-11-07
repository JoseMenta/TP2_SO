#ifndef TPE_BASH_H
#define TPE_BASH_H


#include <libc.h>


#define ASCII_DELETE (127)            // El valor ASCII de la tecla borrar
#define MAX_BUFFER_SIZE 128         // Tamaño maximo de caracteres que puede almacenar el buffer
#define MAX_ARGS_SIZE 3            // La cantidad máxima de argumentos que vamos a aceptar en un programa

void bash(uint64_t arg_c, char ** arg_v);

#endif