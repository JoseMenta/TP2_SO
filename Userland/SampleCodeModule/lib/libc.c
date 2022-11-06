#include <libc.h>
#include <programs.h>
#include <os_tests.h>
#include <built_in.h>
#include <bash.h>
//TODO: sacar
#include "../include/libc.h"
#include "../include/bash.h"
#include "../include/os_tests.h"
#include "../include/test_pipes.h"
#include "../include/built_in.h"

#define MAX_NUMBER_LENGTH 21


front_program_t programs[CANT_PROG] = {
        {"bash","\tbash: Crea un bash\n",&bash},
        {"help","\thelp: Despliega los distintos comandos disponibles\n",&help},
        {"div0","\tdiv0: Genera una excepcion por division por cero\n",&zero_division_exc},
        {"opcode","\topcode: Genera una excepcion por instruccion invalida\n",&invalid_opcode_exc},
        {"inforeg","\tinforeg: en un momento con la combinacion Control+s\n",&inforeg},
        {"printmem","\tprintmem: Dada una direccion de memoria como argumento, devuelve el vuelco de memoria de las 32 direcciones de memoria a partir de la indicada\n",&printmem},
        {"tiempo","\ttiempo: Fecha y hora actuales (GMT -3)\n",&tiempo},
        {"primos","\tprimos: Despliega los numeros primos a partir del 2\n",&primos},
        {"fibonacci","\tfibonacci: Despliega los numeros de la serie de Fibonacci\n",&fibonacci},
        {"test-processes","\ttest_processes: Testeo del scheduler creando y matando procesos\n",&test_processes},
        {"test-prio","\ttest_prio: Testeo del manejo de prioridades en el scheduler\n",&test_prio},
        {"test-sync","\ttest_sync: Testeo del funcionamiento de los semaforos\n",&test_sync},
        {"test-mm","\ttest_mm: Testeo del funcionamiento del mm \n",&test_mm},
        {"mem","\tmem: Imprime el estado de la memoria\n",&mem},
        {"ps","\tps: Imprime la lista de todos los procesos y sus propiedades\n",&ps},
        {"loop","\tloop: Imprime su ID cada 2 segundos\n",&loop},
        {"kill","\tkill: Mata a un proceso dado su ID\n",&kill},
        {"waitpid","\twaitpid: Espera a un proceso dado su ID (o eliminar zombies)\n",&wait_pid_command},
        {"nice","\tnice: Cambia la prioridad de un proceso dado su ID, donde 0 es la prioridad maxima y 4 la minima\n",&nice_command},
        {"block","\tblock: Bloquea a un proceso dado su ID\n",&block},
        {"unblock","\tunblock: Desbloquea a un proceso dado su ID\n",&unblock},
        {"sem","\tsem: Imprime la lista de los semaforos con sus propiedades\n",&sem},
        {"cat","\tcat: Imprime la entrada estandar\n",&cat},
        {"wc","\twc: Cuenta la cantidad de lineas del input por entrada estandar\n",&wc},
        {"filter","\tfilter: Filtra las vocales del input por entrada estandar\n",&filter},
        {"pipe","\tpipe: Imprime una lista de los pipes con sus propiedades\n",&pipe_info},
        {"phylo","\tphylo: Implementacion del problema de los filosofos comensales con una cantidad dinamica de los mismos\n",&phylo},
        {"write.name", "\twrite_pipe\n", &write_pipe_name},
        {"read.name", "\tread_pipe\n", &read_pipe_name},
        {"write.common", "\twrite_pipe_c\n", &write_pipe_common},
        {"read.common", "\tread_pipe_c\n", &read_pipe_common},
};



void number_to_string(uint64_t number, char * str);



//---------------------------------------------------------------------------------
// strcpy: implementacion local de strcpy
//---------------------------------------------------------------------------------
char* strcpy(char* destination, const char* source){
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

//---------------------------------------------------------------------------------
// strlen: implementacion local de strlen
//---------------------------------------------------------------------------------
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

//---------------------------------------------------------------------------------
// strcmp: implementacion local de strcmp
//---------------------------------------------------------------------------------
uint64_t strcmp(const char *X, const char *Y)
{
    while (*X){
        if (*X != *Y) {
            break;
        }
        X++;
        Y++;
    }
    return *(const unsigned char*)X - *(const unsigned char*)Y;
}

//---------------------------------------------------------------------------------
// getChar: lectura de un caracter con sys_call de lectura
//---------------------------------------------------------------------------------
// Argumentos:
//      void
//---------------------------------------------------------------------------------
// Retorno:
//      caracter leido o -1 si se obtuvo EOF
//---------------------------------------------------------------------------------
uint8_t get_char(void){
    char c[2];
    int ret = read(STDIN, c, 1);
    if(ret == -1){
        return -1;
    } //Si no leyo caracteres
    return c[0];
}


//---------------------------------------------------------------------------------
// getChar_fd: lectura de un caracter del fd con sys_call de lectura
//---------------------------------------------------------------------------------
// Argumentos:
//      fd: fd del que se quiere leer
//---------------------------------------------------------------------------------
// Retorno:
//      caracter leido o 0 si no se leyo un caracter
//---------------------------------------------------------------------------------
uint8_t get_char_fd(int fd){
    char c;
    int ret = read(fd, &c, 1);
    if(ret == 0){
        return 0;
    } //Si no leyo caracteres
    return c;
}


//---------------------------------------------------------------------------------
// getLine: lectura de una linea (hasta \n) de STDIN e imprime en pantalla a medida que lee
//---------------------------------------------------------------------------------
// Argumentos:
//      buf: el buffer donde se guarda el resultado
//      max_len: la longitud maxima de caracteres que se puede guardar (incluyendo el \0)
//---------------------------------------------------------------------------------
// Retorno:
//      La cantidad de caracteres leidos (incluyendo a \n), sin incluir \0
//      Devuelve a la linea o max_len caracteres, lo que ocurra antes
//      Por lo tanto, si el valor devuelto es max_len-1, se debe chequear si el ultimo caracter es \n (buf[max_len-2] o buf[ret-1])
//---------------------------------------------------------------------------------
//TODO: solucionar el tema de eliminar, a veces funciona y a veces no
uint32_t get_line(char* buf, uint32_t max_len){
    int read = 0;
    char c[2];
    c[1]='\0';
    for(; (c[0] = get_char()) != '\n' && read<max_len-1;){
        if(c[0]!=ASCII_DELETE){
            buf[read]=c[0];
            read++;
            write(STDOUT, c, 1);
        }else if(read>0){
            read--;
            write(STDOUT, c, 1);
        }

    }
    buf[read] = '\n';
    buf[read+1] = '\0';
    return read;
}

uint32_t get_string(char* buf, uint32_t max_len){
    int read = 0;
    char c[2];
    c[1]='\0';
    for(; (c[0] = get_char()) != '\0' && read<max_len-1;){
        if(c[0]!=ASCII_DELETE){
            buf[read]=c[0];
            read++;
            write(STDOUT, c, 1);
        } else if(read>0){
            read--;
            write(STDOUT, c, 1);
        }
    }
    buf[read] = '\n';
    return read;
}

//---------------------------------------------------------------------------------
// get_line_fd: lectura de una linea (hasta \n) de fd e imprime en pantalla a medida que lee
//---------------------------------------------------------------------------------
// Argumentos:
//      fd: de donde se quiere leer
//      buf: el buffer donde se guarda el resultado
//      max_len: la longitud maxima de caracteres que se puede guardar (incluyendo el \0)
//---------------------------------------------------------------------------------
// Retorno:
//      La cantidad de caracteres leidos (incluyendo a \n), sin incluir \0
//      Devuelve a la linea o max_len caracteres, lo que ocurra antes
//      Por lo tanto, si el valor devuelto es max_len-1, se debe chequear si el ultimo caracter es \n (buf[max_len-2] o buf[ret-1])
//---------------------------------------------------------------------------------

uint32_t get_line_fd(char* buf, uint32_t max_len, int fd){
    int read = 0;
    char c;
    for(; (c = get_char_fd(fd)) != '\n' && read<max_len; read++){
        buf[read]=c;
    }
    buf[read] = '\n';
    return read;
}

/*
uint32_t get_line(char* buf, uint32_t max_len){
    uint32_t curr = 0;
    int finished = 0;
//    max_len--; //para poder hacer buf[i+1] sin problema en el caso donde entra justo
    do{
        long aux = 0;
        //si le paso maximo 10, lee 9 caracteres y el \0 y devuelve 9
        aux = read(STDIN,buf,(max_len-curr));
        print_number(aux);
        write(STDOUT,buf,aux);
        for(uint32_t i = curr; i<curr+aux && !finished; i++){
            print_string("entre");
            print_string(&buf[i]);
            print_string("dsp");
            if(buf[i]=='\n'){
                print_string("hola");
                buf[i+1]='\0';
                curr = i+1;//le devuelvo la cantidad de caracteres incluyendo el \n
                finished = 1;
            }
        }
        if(!finished){
            curr+=aux;
            if(curr+1==max_len){//aux<=max_len-curr => aux+curr<=max_len
                finished=1;
            }
        }
    } while (!finished);
    return curr;
}
*/

//---------------------------------------------------------------------------------
// print_string_with_padding: imprime un String completando con espacios el tamaño libre
//---------------------------------------------------------------------------------
// Argumentos:
//      s1: el string que se desea imprimir
//      len: el tamaño que se desea que tenga el string (completando con espacios a derecha)
//---------------------------------------------------------------------------------
// Retorno:
//      cantidad de caracteres escritos
//---------------------------------------------------------------------------------
uint8_t print_string_with_padding(const char * s1, uint8_t len){
    char aux[len+1];
    int i = 0;
    for(;s1[i]!='\0' && i<len;i++){
        aux[i]=s1[i];
    }
    for(;i<len;i++){
        aux[i] = ' ';
    }
    aux[i]='\0';
    return write(STDOUT, aux, len);
}

void print_numbers(const uint64_t* nums, uint32_t len){
    if(nums==NULL){
        return;
    }
    for(int i = 0; i<len; i++){
        print_number(nums[i]);
        print_string(" ");
    }
}
//---------------------------------------------------------------------------------
// printString: imprime un String
//---------------------------------------------------------------------------------
// Argumentos:
//      void printString(char * s1)
//---------------------------------------------------------------------------------
// Retorno:
//      cantidad de caracteres escritos
//---------------------------------------------------------------------------------
uint8_t print_string(const char * s1){
    return write(STDOUT, s1, strlen(s1));
}

//---------------------------------------------------------------------------------
// printString_fd: imprime un String en fd
//---------------------------------------------------------------------------------
// Argumentos:
//      void printString(char * s1)
//      fd: a donde se quiere imprimir
//---------------------------------------------------------------------------------
// Retorno:
//      cantidad de caracteres escritos
//---------------------------------------------------------------------------------
uint8_t print_string_fd(const char * s1, int fd){
    return write(fd, s1, strlen(s1));
}

//---------------------------------------------------------------------------------
// printNumber: imprime un Numero
//---------------------------------------------------------------------------------
// Argumentos:
//      void printString(int number, int format)
//---------------------------------------------------------------------------------
// Retorno:
//      cantidad de caracteres escritos
//---------------------------------------------------------------------------------
uint8_t print_number(uint64_t number){
    char str[MAX_NUMBER_LENGTH];
    number_to_string(number, str);
    return print_string(str);
}

//---------------------------------------------------------------------------------
// printNumber_fd: imprime un Numero en fd
//---------------------------------------------------------------------------------
// Argumentos:
//      void printString(int number, int format)
//      fd: donde se quiere imprimir
//---------------------------------------------------------------------------------
// Retorno:
//      cantidad de caracteres escritos
//---------------------------------------------------------------------------------
uint8_t print_number_fd(uint64_t number, int fd){
    char str[MAX_NUMBER_LENGTH];
    number_to_string(number, str);
    return print_string_fd(str, fd);
}

//---------------------------------------------------------------------------------
// number_to_string: Devuelve un string del numero pasado por parametros
//---------------------------------------------------------------------------------
// Argumentos:
//   number: El numero a convertir
//   str: El string sobre el cual copiar
//---------------------------------------------------------------------------------
// Retorno:
//      cantidad de caracteres escritos
//---------------------------------------------------------------------------------
void number_to_string(uint64_t num, char * str){
    // Si el numero es 0, devuelvo el string 0 y ya esta
    if(num == 0){
        str[0] = '0';
        str[1] = '\0';
        return;
    }
    int i = 0, sign = 1;
    // Si el numero es negativo, tengo que al final agregarle el simbolo - al string
    if(num < 0){
        sign = -1;
        // Lo pongo positivo para facilitar la operacion
        num *= -1;
    }
    // Recorro hasta que num valga 0
    while(num > 0){
        int j = i;
        // Como leo del mas pequeño al mas grande, debo ir corriendo los digitos hacia la derecha: 1 2 3 -> _ 1 2 3
        while(j > 0){
            str[j] = str[j-1];
            j--;
        }
        // Obtengo el proximo digito y le sumo 30h para obtener su ascii
        str[0] = (num % 10) + 0x30;
        num /= 10;
        i++;
    }
    // Si el numero era negativo
    if(sign == -1){
        int j = i;
        // Vuelvo a correr el numero para poder insertar el -
        while(j > 0){
            str[j] = str[j-1];
            j--;
        }
        str[0] = '-';
        i++;
    }
    // Y agrego el \0 al final del string
    str[i] = '\0';
}


// Dado el string de un numero, devuelve su entero correspondiente
// Tambien se puede recibir el resultado por puntero
uint64_t string_to_number(const char * str, uint64_t * resp){
    uint64_t i = 0;
    uint64_t res = 0;

    if (!str) return 0;

    for ( ; str[i] != '\0'; ++i){
        if(str[i] < '0' || str[i] > '9') return 0;
        res = res * 10 + str[i] - '0';
    }

    if(resp != NULL){
        *resp = res;
    }

    return res;
}


//---------------------------------------------------------------------------------
// get_program: Devuelve el programa asociado al string str
//---------------------------------------------------------------------------------
// Argumentos:
//   - el nombre del programa a ejecutar
//---------------------------------------------------------------------------------
// Retorno
//   - el puntero al programa a ejecutar
//---------------------------------------------------------------------------------
void* get_program(const char * str){
    for(int i = 0; i<CANT_PROG;i++){
        if(strcmp(str,programs[i].name)==0){
                return programs[i].start;
        }
    }
    // Si el string no es el de un programa, se devuelve NULL
    return NULL;
}

char* get_program_name(void* program){
    for(int i = 0; i<CANT_PROG; i++){
        if(programs[i].start == program){
            return programs[i].name;
        }
    }
    return NULL;
}
//-----------------------------------------------------------------------
// uintToBase: Convierte un entero en la base indica por parametro en un string
//-----------------------------------------------------------------------
// Argumentos:
//  value: el valor del entero
//  buffer: el string sobre cual copiar
//  base: la base a convertir del entero
//-----------------------------------------------------------------------
uint64_t uintToBase(uint64_t value, char * buffer, uint64_t base)
{
    char *p = buffer;
    char *p1, *p2;
    uint64_t digits = 0;

    //Calculate characters for each digit
    do
    {
        uint64_t remainder = value % base;
        *p++ = (remainder < 10) ? remainder + '0' : remainder + 'A' - 10;
        digits++;
    }
    while (value /= base);

    // Terminate string in buffer.	(El \0 del string)
    *p = 0;

    //Reverse string in buffer. (Notar que al hacer el pasaje de int a char, se tiene el numero al reves pues se analiza de derecha a izquierda)
    p1 = buffer;
    p2 = p - 1;
    while (p1 < p2)
    {
        char tmp = *p1;
        *p1 = *p2;
        *p2 = tmp;
        p1++;
        p2--;
    }

    return digits;
}

//-----------------------------------------------------------------------
// to_hex: Devuelve un entero hexadecimal en un string
//-----------------------------------------------------------------------
// Argumentos:
//  str: el string sobre cual copiar
//  val: el valor del entero
//-----------------------------------------------------------------------
char * to_hex(char * str, uint64_t val){
    uintToBase(val,str,16);
    return str;
}


//-------------------------------------------------------------------------------------
// FUNCIONES PARA MANEJO DE FECHA Y HORA
//-------------------------------------------------------------------------------------
// Parametros:
//   void
//-------------------------------------------------------------------------------------
// Retorno:
//   Entero con el tipo de unidad temporal deseada del momento en que se consulto
//-------------------------------------------------------------------------------------
uint8_t get_day_week(void){
    return sys_time(DAY_WEEK);
}

uint8_t get_day(void){
    return sys_time(DAY_MONTH);
}

uint8_t get_month(void){
    return sys_time(MONTH);
}

uint8_t get_year(void){
    return sys_time(YEAR);
}

uint8_t get_hour(void){
    return sys_time(HOUR);
}

uint8_t get_min(void){
    return sys_time(MIN);
}

uint8_t get_secs(void){
    return sys_time(SEC);
}

//---------------------------------------------------------------------------------
// str_tok: Retorna el indice de la primera aparicion del separador (un caracter) en el string
//---------------------------------------------------------------------------------
// Argumentos:
//   - buffer: El texto a analizar
//   - sep: El caracter a encontrar
//   - is_end: Indica si termino de leer el string (llego al \0) = 1, 0 si no
//---------------------------------------------------------------------------------
// Retorno
//   - El indice de la primera aparicion de sep en buffer o de \0
//---------------------------------------------------------------------------------
uint64_t str_tok(char * buffer, char sep){
    uint64_t i=0;
    for(; buffer[i]!=sep && buffer[i]!='\0' && buffer[i]!='\n'; i++);
    return i;
}


//---------------------------------------------------------------------------------
// throw_error: Imprime un mensaje de error y corta el programa
//---------------------------------------------------------------------------------
// Argumentos:
//   - str: mensaje de error a imprimir
//---------------------------------------------------------------------------------
// Retorno
//   - void
//---------------------------------------------------------------------------------
void throw_error(char * str){
    p_error(str);
    sys_exit();
}

//---------------------------------------------------------------------------------
// throw_error: Imprime un mensaje de error y corta el programa
//---------------------------------------------------------------------------------
// Argumentos:
//   - str: mensaje de error a imprimir
//---------------------------------------------------------------------------------
// Retorno
//   - void
//---------------------------------------------------------------------------------
void p_error(char * str){
    uint64_t size = strlen(str);
    write(STDERR, "\n", 1);
    write(STDERR, str, size);
    write(STDERR, "\n", 2);
    // print_string("\n", WHITE);
    // print_string(str, STDERR);
    // print_string("\n\n", WHITE);
//    exit();
//TODO: revisar de hacer un error expulsivo y otro no quizas
}

//---------------------------------------------------------------------------------
// pause_ticks: Pausa el proceso de la computadora por la cantidad de ticks indicados
//---------------------------------------------------------------------------------
// Argumentos:
//   - ticks: Cantidad de ticks que se desea detener
//---------------------------------------------------------------------------------
// Retorno
//   - void
//---------------------------------------------------------------------------------
void pause_ticks(uint64_t ticks){
    sys_pause_ticks(ticks);
}


//---------------------------------------------------------------------------------
// sleep: Pausa el proceso de la computadora por la cantidad de segundos indicados
//---------------------------------------------------------------------------------
// Argumentos:
//   - seconds: Cantidad de segundos que se desea detener
//---------------------------------------------------------------------------------
// Retorno
//   - void
//---------------------------------------------------------------------------------
void sleep(uint32_t seconds){
    sys_sleep(seconds);
}

//---------------------------------------------------------------------------------
// blink: Wrapper de la syscall blink
//---------------------------------------------------------------------------------
// Argumentos:
//   void
//---------------------------------------------------------------------------------
// Retorno
//  0 si resulto exitoso, 1 si no
//---------------------------------------------------------------------------------
uint8_t blink(void){
    return sys_blink();
}

//---------------------------------------------------------------------------------
// clear: Wrapper de la syscall clear
//---------------------------------------------------------------------------------
// Argumentos:
//   void
//---------------------------------------------------------------------------------
// Retorno
//  0 si resulto exitoso, 1 si no
//---------------------------------------------------------------------------------
uint8_t clear(void){
    return sys_clear();
}


//---------------------------------------------------------------------------------
// copy_str:
//---------------------------------------------------------------------------------
// Argumentos:
//   dest: El string sobre el que se copia
//   source: El string del que se copia
//---------------------------------------------------------------------------------
// Retorno
//   Actualiza el string dest
//---------------------------------------------------------------------------------
void copy_str(char * dest, char * source){
    uint8_t i = 0;
    for(; source[i] !='\0'; i++){
        dest[i]=source[i];
    }
    dest[i] = '\0';
}

int32_t block_process(uint64_t pid){
    return sys_block(pid);
}
uint32_t unblock_process(uint64_t pid){
    return sys_unblock(pid);
}
int32_t waitpid(uint64_t pid){
    return sys_waitpid(pid);
}

int32_t yield(){
    return sys_yield();
}
int32_t terminate_process(uint64_t pid){
    return sys_terminate(pid);
}

uint64_t getpid(){
    return sys_getpid();
}

uint64_t nice(uint64_t pid, uint8_t priority){
    return sys_nice(pid,priority);
}

void* malloc(uint32_t wanted_size){
    return sys_mm_alloc(wanted_size);
}
void free(void* p){
    sys_mm_free(p);
}

int pipe(int fd[2]){
    return sys_pipe(fd);
}
int open_fifo(Pipe_modes mode, char * name){
    return sys_open_fifo(mode, name);
}
int link_pipe_named(Pipe_modes mode, char * name){
    return sys_link_pipe_named(mode, name);
}
int close_fd(int fd){
    return sys_close_fd(fd);
}
int write(int fd, const char * buf, int count){
    return sys_write(fd, buf, count);
}
int read(int fd, char * buf, int count){
    return sys_read(fd, buf, count);
}
void get_info(pipe_user_info * user_data, int * count){
    sys_get_info(user_data, count);
}

sem_t sem_init(char * name, uint64_t value){
    return sys_sem_init(name, value);
}

sem_t sem_open(char * name, uint64_t value, open_modes mode){
    return sys_sem_open(name, value, mode);
}

int8_t sem_wait(sem_t sem){
    return sys_sem_wait(sem);
}

int8_t sem_post(sem_t sem){
    return sys_sem_post(sem);
}

int8_t sem_close(sem_t sem){
    return sys_sem_close(sem);
}

uint32_t sem_info(sem_dump_t * buffer, uint32_t length){
    return sys_sem_info(buffer, length);
}

void free_sem_info(sem_dump_t * buffer, uint32_t length){
    sys_free_sem_info(buffer, length);
}

int dup2(int oldfd, int newfd){
    return sys_dup2(oldfd, newfd);
}

int dup_handler(int oldfd){
    return sys_dup(oldfd);
}
