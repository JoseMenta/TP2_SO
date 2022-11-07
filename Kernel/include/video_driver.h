#ifndef TPE_VIDEO_DRIVER_H
#define TPE_VIDEO_DRIVER_H

#include <stdint.h>
#include <scheduler.h>

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

typedef struct{
    uint32_t row_start;             // La fila inicial
    uint32_t col_start;             // La columna inicial
    uint32_t row_current;           // La fila actual
    uint32_t col_current;           // La columna actual
    uint32_t row_end;               // La fila final
    uint32_t col_end;               // La columna final
} coordinatesType;

// enum para indicar los colores disponibles, se pasan como parametro en las funciones del driver
typedef enum {BLACK=0x00, BLUE, GREEN, CYAN, RED, MAGENTA, BROWN, LIGHT_GRAY, DARK_GREY, LIGHT_BLUE, LIGHT_GREEN, LIGHT_CYAN, LIGHT_RED, PINK, YELLOW, WHITE, BACKGROUND_BLACK = BLACK << 4, BACKGROUND_WHITE = WHITE << 4} formatType;

void print_tab(positionType position, formatType letterFormat);
void print(const char * str, formatType letterFormat, positionType position);
void new_line(positionType position);
void println(const char * str, formatType letterFormat, positionType position);
void scroll_up(positionType position);
void print_char(char c, formatType letterFormat, positionType position);
void print_dec_format(uint64_t value, positionType position);
void print_hex_format(uint64_t value, positionType position);
void print_bin_format(uint64_t value, positionType position);
void clear(positionType position);
void print_lines();
void delete_last_char(positionType position);
void video_blink(positionType position);

#endif
