
#ifndef TPE_LIBC_H
#define TPE_LIBC_H

#include <stdint.h>



typedef enum {BLACK=0x00, BLUE, GREEN, CYAN, RED, MAGENTA, BROWN, LIGHT_GRAY, DARK_GREY, LIGHT_BLUE, LIGHT_GREEN, LIGHT_CYAN, LIGHT_RED, PINK, YELLOW, WHITE} formatType;

typedef enum {SEC = 0, MIN = 2, HOUR = 4, DAY_WEEK = 6, DAY_MONTH = 7, MONTH = 8, YEAR = 9} timeType;

typedef struct{
    void * start;                   // Direccion de la funcion que ejecuta el programa
    uint64_t cant_arg;              // Cantidad de argumentos ingresados al programa
    char ** args;                   // Este es un vector que se tiene que definir aparte, antes de inicializar la estructura
} program_t;
typedef struct{
    char* name;
    void* start;            // Direccion de la funcion que ejecuta el programa
    uint64_t arg_c;      // Cantidad de argumentos ingresados al programa
    char** arg_v;            // Vector de strings con los argumentos del programa
} executable_t;
typedef struct {
    char* name;
    char* desc;
    void* start;
}front_program_t;

#define NULL ((void*)0)
#define CANT_PROG (8)
#define STDOUT WHITE
#define STDERR RED

uint8_t sys_write(const char * string, formatType format);
uint8_t sys_read(char * c);
uint8_t sys_exec(uint8_t process_c, const program_t* process_v);
uint8_t sys_exit(void);
uint8_t sys_time(timeType time_unit);
uint8_t sys_mem(uint64_t init_dir, uint8_t * arr);
uint64_t sys_tick(void);
uint8_t sys_blink(void);
uint8_t sys_clear(void);
uint8_t sys_regs(uint64_t * regs_arr);

void * get_program(const char * str);
uint8_t get_char(void);
uint8_t print_string(const char * s1, formatType format);
uint8_t print_number(uint64_t number, formatType format);
uint64_t strcmp(const char * s1, const char * s2);
char * to_hex(char * str, uint64_t val);
uint64_t uintToBase(uint64_t value, char * buffer, uint64_t base);
void copy_str(char * dest, char * source);

uint8_t get_year(void);
uint8_t get_month(void);
uint8_t get_day_week(void);
uint8_t get_day(void);
uint8_t get_hour(void);
uint8_t get_min(void);
uint8_t get_secs(void);

uint64_t* get_registers(void);
uint64_t str_tok(char * buffer, char sep);
void throw_error(char * str);
void pause_ticks(uint64_t ticks);
uint8_t blink(void);
uint8_t clear(void);

#endif //TPE_LIBC_H
