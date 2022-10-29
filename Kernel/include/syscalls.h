#ifndef TPE_SYSCALLS_H
#define TPE_SYSCALLS_H

#include <stdint.h>
#include <video_driver.h>

typedef enum {SEC = 0, MIN = 2, HOUR = 4, DAY_WEEK = 6, DAY_MONTH = 7, MONTH = 8, YEAR = 9} timeType;

#define MAX_ARRAY_SIZE 32
#define MAX_MEM_ADDRESS 0xFFFFFFFF8                             // A partir de la proxima direccion, falla (suponemos que es un seg fault)
//#define NULL 0

int32_t read_handler(char * str);                               // Lee por pantalla el siguiente caracter y lo copia en str
int32_t write_handler(const char* str, formatType format);      // Escribe el string str por pantalla con el formato format
//int32_t exec_handler(uint8_t cant, const executable_t* programas);                    // Agrega un nuevo proceso al arreglo de procesos
int32_t exec_handler(executable_t* executable);
int32_t exit_handler();                                         // Finaliza un proceso con codigo de error
uint8_t time_handler(timeType time_unit);                        // Devuelve el valor para la unidad indicada en GMT
int32_t mem_handler(uint64_t init_dir, uint8_t * arr);          // Complete un arreglo de 32 elementos con la informacion que guarda init_dir y las 31 direcciones siguientes
uint64_t tick_handler(void);                                    // Devuelve la cantidad de ticks transcurridos
int32_t blink_handler(void);                                    // Realiza un parpadeo en la pantalla en el proximo caracter a escribir
uint8_t regs_handler(uint64_t * regs_arr);
int32_t clear_handler(void);
uint8_t get_time(timeType time_unit);                           // Llama al RTC con la unidad indicada
uint8_t get_data(uint64_t address_dir);                         // Devuelve el dato de 8 bits almacenado en la direccion de memoria indicada por parametros
uint64_t* get_inforeg_context();                                // Devuelve el arreglo con el estado de los registros cuando se realizo CTRL+S por ultima vez
int32_t block_process_handler(uint64_t pid);
int32_t waitpid_handler(uint64_t pid);
int32_t yield_handler(void);
int32_t unblock_process_handler(uint64_t pid);
int32_t exec_handler(executable_t* executable);
int32_t nice_handler(uint64_t pid, uint8_t priority);
uint64_t getpid_handler();
void* syscall_dispatcher(uint64_t syscall_num);
#endif
