
#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <stdint.h>

void zero_division();
void invalid_opcode();

//uint64_t* get_registers(void);
void print_registers();

typedef void (* exception) ();

#endif
