
#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <stdint.h>

void zero_division();
void invalid_opcode();
void print_registers();
typedef void (* exception) ();

#endif
