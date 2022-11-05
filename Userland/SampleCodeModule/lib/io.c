#include <io.h>
#include <stddef.h>
#include <libc.h>

#define INITIAL_SIZE    10
#define CHUNKS          10
#define DELETE_ASCII    127
#define MAX_SIZE        4294967290

#define FULL_ERR        "\nNo se pueden almacenar mas teclas, por favor ingrese ENTER.\n"

static int8_t resize_buffer(char ** buffer, unsigned int * buff_size);
static void write_err_msg(char ** buffer, unsigned int buffer_size, char * err_msg);
static void clear_buffer(char ** buffer, unsigned int buff_size);

// ------------------------------------------------------------------------------
// io_logic: Recibe las teclas, las imprime por pantalla y ejecuta una funcion al recibir un \n
// ------------------------------------------------------------------------------
// Argumentos:
//  fn: La funcion a ejecutar al recibir un \n
//  err_msg: Arreglo para enviar el mensaje de error, en caso de haber uno
//  msg_size: Tamaño del arreglo err_msg
// ------------------------------------------------------------------------------
// Devuelve -1 en caso de error, 0 en caso de exito
int8_t io_logic(newline_read_fn fn, char ** err_msg, unsigned int msg_size){
    unsigned int size = 0, buff_size = INITIAL_SIZE, buff_pos = 0;
    char * buffer = malloc(INITIAL_SIZE * sizeof(char));
    if(buffer == NULL){
        write_err_msg(err_msg, msg_size, "Error alojando espacio en el buffer.");
        return -1;
    }

    char letter;
    uint8_t want_to_exit = 0;

    while(!want_to_exit && (letter = get_char()) != 0){

        write(STDOUT, &letter, 1);

        // TODO: VER SI SE IMPLEMENTA EL BORRAR TECLAS
        if (letter == DELETE_ASCII){
            if(buff_pos != 0){
                //write(STDOUT, "\b", 1);
                buffer[--buff_pos] = '\0';
            }
        }
        else {
            if(buff_pos == buff_size && resize_buffer(&buffer, &buff_size) == -1){
                size = strlen(FULL_ERR);
                write(STDOUT, FULL_ERR, size);
            } else {
                buffer[buff_pos++] = letter;
                if(letter == '\n'){
                    // Vemos si la persona desea terminar el programa escribiendo el mensaje de salida
                    if(strcmp(buffer, EXIT_MSG) == 0){
                        want_to_exit = 1;
                    } else {
                        // Ejecutamos la funcion que se debe realizar al recibir un \n
                        fn(buffer, buff_pos);
                        clear_buffer(&buffer, buff_size);
                        buff_pos = 0;
                    }
                }
            }
        }
    }

    free(buffer);
    write_err_msg(err_msg, msg_size, "Finalizo correctamente.");
    return 0;
}


// ------------------------------------------------------------------------------
// resize_buffer: Expande el buffer, actualizando la variable buff_size
// ------------------------------------------------------------------------------
// Argumentos:
//  buffer: Puntero al buffer
//  buff_size: Puntero al tamaño actual del buffer
// ------------------------------------------------------------------------------
// Al finalizar se habra expandido el tamaño del buffer y actualizado buff_size a su nuevo tamaño
// En caso de error, deja todo como estaba
// Devuelve -1 en caso de error, 0 en caso de exito
static int8_t resize_buffer(char ** buffer, unsigned int * buff_size){
    if(*buff_size + CHUNKS > MAX_SIZE){
        return -1;
    }

    unsigned int i = 0;

    // Creamos un arreglo auxiliar que almacene el texto
    char * aux = malloc(*buff_size * sizeof(char));
    if(aux == NULL){
        return -1;
    }

    for(i = 0; i < *buff_size; i++){
        aux[i] = (*buffer)[i];
    }

    // Expandimos el buffer, liberando su tamaño actual y solicitando uno mas grande
    free(*buffer);
    *buffer = malloc((*buff_size + CHUNKS) * sizeof(char));
    if(*buffer == NULL){
        *buffer = aux;
        return -1;
    }

    // Copiamos lo salvado (aux) en el buffer
    for(i = 0; i < *buff_size; i++){
        (*buffer)[i] = aux[i];
    }
    free(aux);
    *buff_size += CHUNKS;
    return 0;
}


static void write_err_msg(char ** buffer, unsigned int buffer_size, char * err_msg){
    unsigned int i = 0;
    for(; i < buffer_size-1 && err_msg[i] != '\0'; i++){
        (*buffer)[i] = err_msg[i];
    }
    (*buffer)[i] = '\0';
}


static void clear_buffer(char ** buffer, unsigned int buff_size){
    for(unsigned int i = 0; i < buff_size; i++){
        (*buffer)[i] = '\0';
    }
}