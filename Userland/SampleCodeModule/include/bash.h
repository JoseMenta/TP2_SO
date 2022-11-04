#ifndef TPE_BASH_H
#define TPE_BASH_H


#include <libc.h>


#define LOGOUT ("logout")             // El string con el cual se debe finalizar bash
#define ASCII_DELETE (127)            // El valor ASCII de la tecla borrar

void bash(uint64_t arg_c, char ** arg_v);

#endif