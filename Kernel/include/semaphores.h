
#ifndef TP2_SO_SEMAPHORE_H
#define TP2_SO_SEMAPHORE_H

#include <stdint.h>
#include <queueADT.h>
#include <orderListADT.h>

// Codigo de errores del semaforo
// OK: La operacion se realizo exitosamente
// ERR_MM: Error en la alocacion de memoria
// ERR_NAME: El nombre ya existe
// ERR_NOT_FOUND: El semaforo buscado no existe
// ERR_NOT_INIT: No se inicializo el manejador de semaforos
// ERR_SEM_NOT_INIT: No se inicializo el semaforo
// ERR_SCHEDULER: No se pudo bloquear el proceso
typedef enum {OK = 0, ERR_MM = -1, ERR_NAME = -2, ERR_NOT_FOUND = -3, ERR_NOT_INIT = -4, ERR_SEM_NOT_INIT = -5, ERR_SCHEDULER = -6} error_code;

// Acciones a realizar para la funcion sem_open si no se encuentra el semaforo
// O_CREATE: Crear un nuevo semaforo
// O_NULL: Devolver NULL
// Si se indica cualquier otro valor, se asume O_NULL
typedef enum {O_CREATE = 0, O_NULL} open_modes;

// Manejo de semaforos
typedef struct {
    uint64_t value;                     // Valor del semaforo
    queueADT blocked_processes;         // Lista de procesos detenidos por el semaforo
    orderListADT connected_processes;   // Procesos que utilizan el semaforo
    uint64_t id;                        // id del semaforo
    char * name;                        // Nombre del semaforo
} sem_t;

// Para pasar la informacion al usuario
typedef struct {
    uint64_t value;                     // Valor del semaforo
    uint64_t * blocked_processes;       // Lista de procesos detenidos por el semaforo
    uint64_t * connected_processes;     // Procesos que utilizan el semaforo
    uint64_t blocked_size;              // Cantidad de procesos bloqueados
    uint64_t connected_size;            // Cantidad de procesos conectados
    uint64_t id;                        // id del semaforo
    char * name;                        // Nombre del semaforo
} sem_dump_t;

sem_t * sem_init(char * name, uint64_t value);
sem_t * sem_open(char * name, uint64_t value, open_modes mode);
int8_t sem_wait(sem_t * sem);
int8_t sem_post(sem_t * sem);
int8_t sem_close(sem_t * sem);

uint32_t sems_dump(sem_dump_t * buffer, uint32_t length);
void sems_dump_free(sem_dump_t * buffer, uint32_t length);
uint32_t sem_count();

#endif // TP2_SO_SEMAPHORE_H