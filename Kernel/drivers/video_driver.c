// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <video_driver.h>
#include <keyboard.h>
//TODO: sacar
#include "../include/video_driver.h"
#include "../include/keyboard.h"
#define WIDTH 160
#define HEIGHT 25
#define NEXT_COL 2
#define MID_WIDTH (WIDTH/2)

#define L_ROW_START 0
#define L_COL_START 0

#define L_ROW_END (HEIGHT - 1)
#define L_COL_END (MID_WIDTH - 2 * NEXT_COL)

#define R_ROW_START 0
#define R_COL_START (MID_WIDTH + NEXT_COL)

#define R_ROW_END (HEIGHT - 1)
#define R_COL_END (WIDTH - NEXT_COL)

#define A_ROW_START 0
#define A_COL_START 0

#define A_ROW_END (HEIGHT - 1)
#define A_COL_END (WIDTH - NEXT_COL)

#define SCREEN_ENDED(pos) ((pos).row_current == (pos).row_end && (pos).col_current == (pos).col_end)
#define VIDEO_OFFSET(pos) (WIDTH*((pos).row_current) + (pos).col_current)

//TODO: sacar magic numbers
typedef struct{
    uint32_t row_start;             // La fila inicial
    uint32_t col_start;             // La columna inicial
    uint32_t row_current;           // La fila actual
    uint32_t col_current;           // La columna actual
    uint32_t row_end;               // La fila final
    uint32_t col_end;               // La columna final
} coordinatesType;

void next(coordinatesType * position);
void print_aux(uint8_t * curr, char c, formatType letterFormat, positionType position);

coordinatesType coordinates[3] = {  {A_ROW_START, A_COL_START, A_ROW_START, A_COL_START, A_ROW_END, A_COL_END},
                                    {L_ROW_START, L_COL_START, L_ROW_START, L_COL_START, L_ROW_END, L_COL_END},
                                    {R_ROW_START, R_COL_START, R_ROW_START, R_COL_START, R_ROW_END, R_COL_END} };

// Con estas variables podemos calcular la posicion donde se debe imprimir un caracter con la formula:
//                              video_start + WIDTH * row + column

//uint8_t para que cuando hacemos ++, vaya avanzando de a 1 byte
static uint8_t * const video_start = (uint8_t *) 0xB8000;


//-----------------------------------------------------------------------
// print_char: imprime un caracter en pantalla
//-----------------------------------------------------------------------
// Argumentos:
// -c: el caracter que se quiere imprimir
// -letterFormat: el color de la letra, una constante de color definida en .h
// -position: Indica en que posicion se debe imprimir el caracter
//-----------------------------------------------------------------------
// Si no hay mas lugar en la pantalla, llama a video_scroll_up
//-----------------------------------------------------------------------
void print_char(char c, formatType letterFormat, positionType position){
    uint8_t * curr = video_start;                   // La primera direccion de video
    // A esto se le suma el offset correspondiente
    curr += VIDEO_OFFSET(coordinates[position]);
    if(*(curr + 1) == BACKGROUND_WHITE){            // Si esta parpadeando lo resetea
        *(curr + 1) = BACKGROUND_BLACK;
    }
    // Luego, imprimimos el caracter (si es \n hara un salto de linea)
    print_aux(curr, c, letterFormat, position);
    if(SCREEN_ENDED(coordinates[position])){
        // Si llego al final de la pantalla, tengo que hacer scroll up
        scroll_up(position);
    }
    // Obtenemos la proxima posicion sobre la cual imprimir en la porcion correspondiente
    if(c!='\n' && c != '\t') {
        next(&coordinates[position]);
    }
}

//-----------------------------------------------------------------------
// print_aux: Logica de impresion segun el caracter
//-----------------------------------------------------------------------
// Argumentos:
// - curr: La posicion sobre la cual imprimir en pantalla
// - letterFormat: el color de la letra, una constante de color definida en .h
// - position: La posicion de la pantalla donde se desea ir a la proxima linea
//-----------------------------------------------------------------------
void print_aux(uint8_t * curr, char c, formatType letterFormat, positionType position){
    if(c == '\n'){                      // Si recibe un \n, se imprime un salto de linea
        new_line(position);
    }else if(c == '\t'){                // Si recibe un \t, se imprime un tabulador (4 espacios)
        print_tab(position, letterFormat);
    }else if(c == ASCII_DELETE){        // Si recibe un BACKSPACE, borra el ultimo caracter ingresado
        delete_last_char(position);
    }else{
        // Si no es ninguno de los anteriores, imprime el caracter en la posicion indicada
        *curr = c;                      // Imprimo el ASCII
        *(curr + 1) = letterFormat;     // Indico el color que va a tener
    }
}

//-----------------------------------------------------------------------
// print_tab: Imprime un tabulador
//-----------------------------------------------------------------------
// Argumentos:
// - position: La posicion de la pantalla donde se desea ir a la proxima linea
//-----------------------------------------------------------------------
void print_tab(positionType position, formatType letterFormat){
    int can_print = 1;
    for(int i=0; i < 4 && can_print; i++){
        if(coordinates[position].col_current == coordinates[position].col_end){
            can_print = 0;
        }
        print_char(' ', WHITE, position);
    }
}

//-----------------------------------------------------------------------
// print: imprime un string en pantalla
//-----------------------------------------------------------------------
// Argumentos:
// -str: el string que se quiere imprimir
// -letterFormat: el color de la letra, una constante de color definida en .h
// -position: la posicion de la pantalla donde se desea imprimir
//-----------------------------------------------------------------------
// Si no hay mas lugar en la pantalla, llama a video_scroll_up
//-----------------------------------------------------------------------
void print(const char * str, formatType letterFormat, positionType position){
    for(;*str!='\0';str++){
        print_char(*str,letterFormat,position);
    }
}

//-----------------------------------------------------------------------
// println: imprime un string en pantalla y salta a la proxima linea
//-----------------------------------------------------------------------
// Argumentos:
// -str: el caracter que se quiere imprimir
// -letterFormat: el color de la letra, una constante de color definida en .h
// -position: posicion de la pantalla donde se quiere imprimir
//-----------------------------------------------------------------------
// Si no hay mas lugar en la pantalla, llama a scroll_up
//-----------------------------------------------------------------------
void println(const char * str, formatType letterFormat, positionType position){
    print(str,letterFormat,position);
    new_line(position);
}

//-----------------------------------------------------------------------
// new_line: ubica al cursor en la proxima linea
//-----------------------------------------------------------------------
// Argumentos:
// -position: La posicion de la pantalla donde se desea ir a la proxima linea
//-----------------------------------------------------------------------
void new_line(positionType position){
    if(coordinates[position].row_current==coordinates[position].row_end){
        scroll_up(position);
    }
    // Posicionamos la fila de la porcion correspondiente en la proxima
    coordinates[position].row_current += 1;
    // Posicionamos la columna de la porcion correspondiente al principio
    coordinates[position].col_current = coordinates[position].col_start;
}

//-----------------------------------------------------------------------
// Funcion auxiliar para obtener la siguiente posicion donde tiene que ir el offset según la porcion indicado por parametro
//-----------------------------------------------------------------------

void next(coordinatesType * position){
    // Si se llego al final de una fila, paso a la proxima fila y al principio de ella
    if(position->col_current == position->col_end){
        position->col_current = position->col_start;
        (position->row_current)++;
    }
        // Si no llegue al final, paso a la siguiente columna en la fila
    else {
        position->col_current += NEXT_COL;
    }
}

void prev(coordinatesType * position){
    // Si el proximo caracter es al principio de una fila (distinta de la primera) entonces debo borrar el ultimo caracter
    // ingresado de la fila anterior
    if(position->col_current == position->col_start){
        // Me muevo a la fila anterior
        position->row_current--;
        // Para la columna es mas dificil, porque no esta asegurado que el ultimo caracter se imprimio en la ultima posicion
        // Por lo que debo encontrar la primera posicion en la que hay un caracter distinto de ' ' (vacio) empezando por el final
        // para hallar el ultimo caracter ingresado
        // Si llego al principio de la fila, entonces puede ser que se ingreso un caracter y se presiono ENTER o directamente
        // se presiono ENTER
        position->col_current = position->col_end;
        while (*(video_start + 160 * position->row_current + position->col_current) == ' ' &&
               position->col_current > position->col_start){
            position->col_current -= NEXT_COL;
        }
    }
    // En cambio, si estoy en la mitad o al final de una fila, entonces debo borrar el caracter en la posicion anterior
    else {
        position->col_current -= NEXT_COL;
    }
}

//-----------------------------------------------------------------------
// scroll_up: Mueve los caracteres una linea arriba (la primera se pierde)
//-----------------------------------------------------------------------
// Argumentos:
// - position: la posicion de la pantalla donde se desea imprimir
//-----------------------------------------------------------------------
void scroll_up(positionType position){
    for(coordinatesType c = {coordinates[position].row_start, coordinates[position].col_start, coordinates[position].row_start, coordinates[position].col_start, coordinates[position].row_end, coordinates[position].col_end};
        c.row_current < coordinates[position].row_end;
        next(&c)){
        *(video_start + c.row_current * WIDTH + c.col_current) = *(video_start + (c.row_current+1) * WIDTH + c.col_current);
        *(video_start + c.row_current * WIDTH + c.col_current + 1) = *(video_start + (c.row_current+1) * WIDTH + c.col_current + 1);
    }
    // Nos paramos en la fila anterior pues todo se subio una fila arriba
    if(coordinates[position].row_current >= 1){
        // Si habia llegado a la ultima fila, debo borrar los caracteres en esa fila
        if(coordinates[position].row_current == coordinates[position].row_end){//Aca se esta rompiendo hash[70]
            for(coordinatesType c = {coordinates[position].row_start, coordinates[position].col_start, coordinates[position].row_end, coordinates[position].col_start, coordinates[position].row_end, coordinates[position].col_end};
                c.row_current <= coordinates[position].row_end && c.col_current <= coordinates[position].col_end;
                next(&c)){
                *(video_start + c.row_current * WIDTH + c.col_current) = ' ';
                *(video_start + c.row_current * WIDTH + c.col_current + 1) = 0;
            }
        }
        coordinates[position].row_current--;
    }
        // En cambio, si estoy en la primera fila, se borro todo, por lo que se debe empezar desde el principio
    else {
        coordinates[position].col_current = coordinates[position].col_start;
    }
}

// ----------------------------------------------------------------------------
// print_lines: Imprime las lineas de separacion para cuando el usuario haga la función |
// ----------------------------------------------------------------------------
// Argumentos:
//  void
// ----------------------------------------------------------------------------
// Retorno:
//  void
// ----------------------------------------------------------------------------
void print_lines(){
    uint32_t col1 = 80;                         // La posicion 78 y 80 son la mitad de la pantalla en el arreglo del driver
    uint32_t col2 = 78;
    for(uint32_t i = 0; i <= A_ROW_END; i++){
        *(video_start + WIDTH * i + col1) = '|';
        *(video_start + WIDTH * i + col1 + 1) = WHITE;
        *(video_start + WIDTH * i + col2) = '|';
        *(video_start + WIDTH * i + col2 + 1) = WHITE;
    }
}

//-----------------------------------------------------------------------
// clear: limpia la pantalla, pone ' ' en todos los lugares disponibles
//-----------------------------------------------------------------------
// Argumentos:
// - position: La posicion de la pantalla que se desea limpiar
//-----------------------------------------------------------------------
void clear(positionType position){
    // Si borramos toda la pantalla debemos tambien reiniciar todos las porciones
    if(position == ALL){
        for(coordinatesType c = {A_ROW_START, A_COL_START, A_ROW_START, A_COL_START, A_ROW_END, A_COL_END};
            c.row_current != A_ROW_END || c.col_current != A_COL_END;
            next(&c)){
            uint8_t * aux = video_start + VIDEO_OFFSET(c);
            *(aux) = ' ';       // "Borrar" = Vacio
            *(aux + 1) = 0;     // Formato default
        }
        // Actualizamos los punteros de las distintas porciones
        coordinates[ALL].row_current = coordinates[ALL].row_start;
        coordinates[ALL].col_current = coordinates[ALL].col_start;
        coordinates[LEFT].row_current = coordinates[LEFT].row_start;
        coordinates[LEFT].col_current = coordinates[LEFT].col_start;
        coordinates[RIGHT].row_current = coordinates[RIGHT].row_start;
        coordinates[RIGHT].col_current = coordinates[RIGHT].col_start;
        return;
    }
    // Borramos todos los caracteres impresos en la porcion (LEFT o RIGHT) pasada por parametros
    for(coordinatesType c = {coordinates[position].row_start, coordinates[position].col_start, coordinates[position].row_start, coordinates[position].col_start, coordinates[position].row_end, coordinates[position].col_end};
        c.row_current != coordinates[position].row_current || c.col_current != coordinates[position].col_current;
        next(&c)){
        uint8_t * aux = video_start + VIDEO_OFFSET(c);
        *(aux) = ' ';       // "Borrar" = Vacio
        *(aux + 1) = 0;     // Formato default
    }
    // Reiniciamos la escritura en dicha porcion
    coordinates[position].row_current = coordinates[position].row_start;
    coordinates[position].col_current = coordinates[position].col_start;
}

//-----------------------------------------------------------------------
// delete_last_char: borra el ultimo caracter ingresado
//-----------------------------------------------------------------------
// Argumentos:
//  position: La posicion de la pantalla donde se desea borrar el ultimo caracter
//-----------------------------------------------------------------------
void delete_last_char(positionType position){
    // Solo debo borrar si no estoy en la posicion inicial de la porcion pasada por parametros (pues sino no habria nada para borrar)
    if(!(coordinates[position].row_current == coordinates[position].row_start &&
         coordinates[position].col_current == coordinates[position].col_start)){
        prev(&coordinates[position]);
        // Borro el ultimo caracter ingresado
        *(video_start + VIDEO_OFFSET(coordinates[position])) = ' ';
        *(video_start + VIDEO_OFFSET(coordinates[position]) + 1) = 0;
        // Me paro uno mas antes para poder escribir la proxima letra
        if(!(coordinates[position].row_current == coordinates[position].row_start &&
             coordinates[position].col_current == coordinates[position].col_start)) {
            prev(&coordinates[position]);
        }
    }
}
//----------------------------------------------------------------------
// video_blink: cambia el fondo del cursor en la posicion
//----------------------------------------------------------------------
// Argumentos:
//  position: posicion donde se quiere cambiar el fondo
//----------------------------------------------------------------------
// El color va cambiando entre blanco y negro cuando se llama
//----------------------------------------------------------------------
void video_blink(positionType position){
    static uint16_t toggle_background = BACKGROUND_BLACK;
    if(toggle_background == BACKGROUND_BLACK){
        *(video_start + VIDEO_OFFSET(coordinates[position]) + 1) = toggle_background = BACKGROUND_WHITE;
    } else {
        *(video_start + VIDEO_OFFSET(coordinates[position]) + 1) = toggle_background = BACKGROUND_BLACK;
    }
}
