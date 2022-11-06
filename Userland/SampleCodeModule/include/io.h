
#ifndef TPE_IO_H
#define TPE_IO_H

#include <stdint.h>

// Funcion a ejecutar cuando se recibe el \n (con la informacion en el buffer, escrito hasta la posicion buff_pos)
typedef void (* newline_read_fn) (char * buffer, unsigned int buff_pos);

int8_t io_logic(newline_read_fn fn, char ** err_msg, unsigned int msg_size);

#endif