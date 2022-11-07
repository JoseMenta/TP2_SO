
#ifndef TPE_IO_H
#define TPE_IO_H

#include <stdint.h>

#define INITIAL_SIZE    10
#define CHUNKS          10
#define DELETE_ASCII    127
#define MAX_SIZE        4294967290
#define TAB_SPACE       4
#define FULL_ERR        "\nNo se pueden almacenar mas teclas, por favor ingrese ENTER.\n"

// Funcion a ejecutar cuando se recibe el \n (con la informacion en el buffer, escrito hasta la posicion buff_pos)
typedef void (* newline_read_fn) (char * buffer, unsigned int buff_pos);

int8_t io_logic(newline_read_fn fn, char ** err_msg, unsigned int msg_size);

#endif