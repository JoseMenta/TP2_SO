
#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <stdint.h>

void zero_division();
void invalid_opcode();


typedef void (* exception) ();

#endif
