
#ifndef TPE_PROGRAMS_H
#define TPE_PROGRAMS_H

#include <stdint.h>


#define COUNT_REGS (18)

void bash(uint64_t arg_c, char ** arg_v);

typedef uint8_t (* time_func) ();

void inforeg(uint64_t arg_c, const char** arg_v);

void printmem(uint64_t arg_c, const char** arg_v);

void tiempo(uint64_t arg_c, const char** arg_v);

void primos(uint64_t arg_c, const char ** arg_v);

void fibonacci(uint64_t arg_c, const char** arg_v);

void help(uint64_t arg_c, const char** arg_v);

void zero_division_exc(uint64_t arg_c, const char** arg_v);

void invalid_opcode_exc(uint64_t arc_c, const char** arg_v);

#endif //TPE_PROGRAMS_H
