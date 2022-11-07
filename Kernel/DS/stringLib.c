// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <stringLib.h>
#include <stddef.h>

// Funcion para comparacion de string
int strcmp(const char *X, const char *Y)
{
    while (*X)
    {
        if (*X != *Y) {
            break;
        }
        X++;
        Y++;
    }
    return *(const unsigned char*)X - *(const unsigned char*)Y;
}

// Function para implementar strcpy localmente
char* strcpy(char* destination, const char* source)
{
    if (destination == NULL) {
        return NULL;
    }
    char *ptr = destination;
    while (*source != '\0')
    {
        *destination = *source;
        destination++;
        source++;
    }
    *destination = '\0';
    return ptr;
}

//Funcion para strlen
unsigned int strlen(const char *s)
{
    unsigned int count = 0;
    while(*s!='\0')
    {
        count++;
        s++;
    }
    return count;
}