
#ifndef TP2_SO_TEST_UTIL_H
#define TP2_SO_TEST_UTIL_H
#include <stdint.h>

uint32_t GetUint();
uint32_t GetUniform(uint32_t max);
uint8_t memcheck(void *start, uint8_t value, uint32_t size);
int64_t satoi(const char* str);
void bussy_wait(uint64_t n);
void endless_loop();
void endless_loop_print(uint64_t argc, char *argv[]);
void * memset(void * destiation, int32_t c, uint64_t length); //se encuentra definida en _loader.c
#endif //TP2_SO_TEST_UTIL_H
