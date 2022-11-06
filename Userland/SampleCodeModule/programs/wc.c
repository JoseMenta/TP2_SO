// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <stdint.h>
#include <stddef.h>
#include <libc.h>
#include <io.h>

#define ERR_SIZE    40

#define RESP_MSG    "\n-------------------------------------------------\n"
#define BYTES_RESP "Total de bytes: "
#define WORDS_RESP "Total de palabras: "
#define LINES_RESP "Total de lineas: "
#define NEW_LINE    "\n"

#define IS_BLANK(c) ((c) == '\n' || (c) == '\t' || (c) == ' ')

static unsigned int bytes_counted, words_counted, lines_counted;
static void write_results(void);

void wc_fn(char * buffer, unsigned int buff_pos){
    uint8_t in_blank = 1;

    bytes_counted += buff_pos;
    for(unsigned int i = 0; i < buff_pos; i++){
        words_counted += (IS_BLANK(buffer[i]) && !in_blank) ? 1 : 0;
        lines_counted += (buffer[i] == '\n') ? 1 : 0;
        in_blank = IS_BLANK(buffer[i]);
    }
}

void wc(uint64_t arg_c, const char ** arg_v){
    bytes_counted = words_counted = lines_counted = 0;

    if(arg_c != 0){
        throw_error("Error: el programa no recibe argumentos.");
    }

    char * err_buffer = malloc(ERR_SIZE * sizeof(char));
    if(err_buffer == NULL){
        throw_error("Error: no se pudo crear el buffer para errores.");
    }

    if(io_logic(wc_fn, &err_buffer, ERR_SIZE) == -1){
        write(STDERR, err_buffer, ERR_SIZE);
    }
    free(err_buffer);

    write_results();
}


static void write_results(void){
    unsigned int size = strlen(RESP_MSG);
    write(STDOUT, RESP_MSG, size);

    size = strlen(BYTES_RESP);
    write(STDOUT, BYTES_RESP, size);

    print_number(bytes_counted);
    write(STDOUT, NEW_LINE, 1);

    size = strlen(WORDS_RESP);
    write(STDOUT, WORDS_RESP, size);

    print_number(words_counted);
    write(STDOUT, NEW_LINE, 1);

    size = strlen(LINES_RESP);
    write(STDOUT, LINES_RESP, size);

    print_number(lines_counted);
    write(STDOUT, NEW_LINE, 1);
}


