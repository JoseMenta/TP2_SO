// Implementación del Memory Manager Buddy
// Algoritmo Buddy: https://www.youtube.com/watch?v=DRAHRJEAEso&ab_channel=GabrielParmer
// Basado en: https://github.com/wuwenbin/buddy2

// La implementación utiliza un arreglo el cual lo maneja como árbol binario mediante indices

#include <mm.h>
#include <mm_buddy.h>

// La dirección donde termina la seccion data del Kernel y empieza el heap
extern uint8_t endOfKernel;

// La direccion donde comienza la seccion de Userland
#define USERLAND_START_DIRECTION 0x400000

// Calcula el tamaño del heap (sin ajustar a potencia de dos)
#define MM_SIZE (((uint8_t *) USERLAND_START_DIRECTION) - ((uint8_t *) &endOfKernel))

// Define la cantidad máxima de nodos que podra manejar el buddy (depende del tamaño asignado al heap)
// Al ajustarse a una potencia de dos, buddy terminará usando una menor cantidad de nodos
#define NODES (2 * MM_SIZE - 1)

#define LEFT_LEAF(index)    ((index) * 2 + 1)               // Devuelve el indice del nodo hijo izquierdo de index
#define RIGHT_LEAF(index)   ((index) * 2 + 2)               // Devuelve el indice del nodo hijo derecho de index
#define PARENT(index)       (((index) + 1) / 2 - 1)         // Devuelve el indice del nodo padre de index

#define IS_POWER_OF_2(x)    (!((x) & ((x) - 1)))            // Indica si x es potencia de dos o no
#define MAX(a, b)           (((a) > (b)) ? (a) : (b))       // Devuelve el mayor entre a y b

static const uint8_t * start_address = (uint8_t *) &endOfKernel;
static const uint8_t * end_address = (uint8_t *) USERLAND_START_DIRECTION;
static uint64_t allocated_bytes = 0;
static uint64_t allocated_blocks = 0;

// Dado un numero, devuelve el proximo numero que sea potencia de dos (por ejemplo, si number = 20, devuelve 32)
// Explicación: https://stackoverflow.com/questions/1322510/given-an-integer-how-do-i-find-the-next-largest-power-of-two-using-bit-twiddlin/1322548#1322548
static uint32_t next_power_of_2(uint64_t number){
    if(number <= 1){
        return 2;
    }
    number--;
    number |= number >> 1;
    number |= number >> 2;
    number |= number >> 4;
    number |= number >> 8;
    number |= number >> 16;
    number |= number >> 32;
    return number + 1;
}

// Dado un numero, devuelve el numero anterior que sea potencia de dos (por ejemplo, si number = 20, devuelve 16)
// Explicación: https://stackoverflow.com/questions/2679815/previous-power-of-2
static uint32_t previous_power_of_2(uint64_t number){
    if(number == 0){
        return 0;
    }
    number--;
    number |= number >> 1;
    number |= number >> 2;
    number |= number >> 4;
    number |= number >> 8;
    number |= number >> 16;
    number |= number >> 32;
    return number - (number >> 1);
}

typedef struct
{
    uint32_t size_available;                            // Espacio para ocupar en el nodo
    uint8_t * start_address;                            // Direccion por la que comienza su espacio en el heap
} node_t;


typedef struct
{
    uint32_t size;                                      // Tamaño del heap
    node_t * node;                                      // Cantidad de nodos del arbol de buddy
} buddy_t;


// Inicializo el buddy (sin espacio)
static buddy_t buddy_manager = {-1};


// Setea el buddy para empezar a manejar el heap
void mm_init(){
    uint8_t * address = (uint8_t *) start_address;
    uint32_t node_size, buddy_size;

    // Defino el arreglo (arbol) de nodos a manejar
    node_t nodes[NODES];
    buddy_manager.node = nodes;

    // Define el tamaño del heap que va a manejar el buddy system
    buddy_size = previous_power_of_2(MM_SIZE);
    buddy_manager.size = buddy_size;

    node_size = buddy_size * 2;

    // Seteamos a cada nodo el tamaño maximo a manejar y su dirección de inicio
    for(uint32_t i = 0; i < 2 * buddy_size - 1; i++){
        // Cada vez que se llegue a una potencia de 2 es porque se paso al siguiente nivel del arbol
        // por lo que el tamaño del nodo decrece a la mitad y comienza a apuntar nuevamente al principio de la memoria
        if(IS_POWER_OF_2(i+1)){
            address = (uint8_t *) start_address;
            node_size /= 2;
        }
        buddy_manager.node[i].size_available = node_size;
        buddy_manager.node[i].start_address = address;
        address += node_size;
    }

    // Actualizamos la ultima direccion valida para asignar espacio
    end_address = address - node_size;
}

//----------------------------------------------------------------------
// mm_alloc: reserva memoria
//----------------------------------------------------------------------
// Argumentos:
//  wanted_size: la cantidad de memoria que se desea reservar
//----------------------------------------------------------------------
// Es la funcion del Memory Manager encargada de reservar memoria
//----------------------------------------------------------------------
// Retorno:
//  Si se encuentra espacio, se devuelve la direccion inicial. En caso de no encontrar
//  espacio, se devuelve NULL
//----------------------------------------------------------------------
void * mm_alloc(uint32_t wanted_size){
    uint32_t node_size, index = 0;
    void * address;

    // Si es la primera vez que se solicita memoria, se inicializa el buddy
    if(buddy_manager.size == -1){
        mm_init();
    }

    // Si el espacio solicitado no es potencia de 2, lo ajustamos a la proxima potencia de 2 para ajustarse al nodo correspondiente en el buddy
    if(wanted_size == 0){
        return NULL;
    } else if (!IS_POWER_OF_2(wanted_size)){
        wanted_size = next_power_of_2(wanted_size);
    }

    // Si no hay espacio suficiente para alojar el espacio solicitado, se retorna NULL
    if(buddy_manager.node[index].size_available < wanted_size){
        return NULL;
    }

    // Busca el indice del nodo que debe asignar para mantener el espacio solicitado, priorizando los subarboles izquierdos
    for(node_size = buddy_manager.size; node_size != wanted_size; node_size /= 2){
        if(buddy_manager.node[LEFT_LEAF(index)].size_available >= wanted_size){
            index = LEFT_LEAF(index);
        } else {
            index = RIGHT_LEAF(index);
        }
    }

    // Una vez que encuentro el nodo, actualizo su espacio disponible (0 pues se ocupo) y obtengo su dirección de inicio
    buddy_manager.node[index].size_available = 0;
    address = (void *) buddy_manager.node[index].start_address;

    // Se actualiza el espacio ocupado y los bloques ocupados
    allocated_bytes += wanted_size;
    allocated_blocks++;

    // Actualizo el espacio disponible de los ancestros del nodo que ocupé (pues se reducira)
    while(index) {
        index = PARENT(index);
        buddy_manager.node[index].size_available = MAX(buddy_manager.node[LEFT_LEAF(index)].size_available, buddy_manager.node[RIGHT_LEAF(index)].size_available);
    }

    return address;
}

//----------------------------------------------------------------------
// mm_free: libera memoria reservada por el mm
//----------------------------------------------------------------------
// Argumentos:
//  p: puntero a la posicion inicial de la memoria otorgada por el mm
//----------------------------------------------------------------------
// Es la funcion del Memory Manager encargada de liberar memoria
//----------------------------------------------------------------------
// Retorno:
//  void
//----------------------------------------------------------------------
void mm_free(void * p){
    // Verifico que el puntero no sea NULL y que sea una direccion valida (este en el rango de la memoria heap)
    if(p == NULL || ((uint8_t *)p) < start_address || ((uint8_t *)p) > end_address) {
        return;
    }

    uint32_t node_size, index = -1, left_size, right_size;

    node_size = buddy_manager.size * 2;

    // Busco el nodo cuya direccion inicial sea p
    for(uint32_t i = 0; index == -1 && i < 2 * previous_power_of_2(MM_SIZE) - 1; i++){
        if(IS_POWER_OF_2(i+1)){
            node_size /= 2;
        }
        // Si lo encuentro, me guardo su indice y restauro el tamaño inicial del nodo
        if(buddy_manager.node[i].start_address == (uint8_t *) p){
            buddy_manager.node[i].size_available = node_size;
            index = i;
        }
    }

    // Si index no cambio es porque la direccion no es valida para liberar, por lo que retorna
    if(index == -1){
        return;
    }

    // Se actualiza el espacio ocupado y los bloques ocupados
    allocated_bytes -= node_size;
    allocated_blocks--;

    // Ahora se debe actualizar el tamaño disponible de los nodos ancetros del nodo que se libero
    while(index) {
        index = PARENT(index);
        node_size *= 2;

        left_size = buddy_manager.node[LEFT_LEAF(index)].size_available;
        right_size = buddy_manager.node[RIGHT_LEAF(index)].size_available;

        // Si tanto hijo izquierdo como hijo derecho estan liberados (la suma de su espacio libre equivale al espacio esperado del nodo padre), entonces el nodo esta completamente libre
        if(left_size + right_size == node_size){
            buddy_manager.node[index].size_available = node_size;
        }
            // Si no, entonces algunos de sus subarboles esta ocupado, por lo se queda con el tamaño disponible maximo entre sus dos subarboles
        else {
            buddy_manager.node[index].size_available = MAX(left_size, right_size);
        }
    }
}

//----------------------------------------------------------------------
// get_total_bytes: devuelve la cantidad de bytes que dispone el heap
//----------------------------------------------------------------------
// Argumentos:
//
//----------------------------------------------------------------------
//
//----------------------------------------------------------------------
// Retorno:
//  Tamaño del heap en bytes
//----------------------------------------------------------------------
uint64_t get_total_bytes(){
    return buddy_manager.size;
}

//----------------------------------------------------------------------
// get_allocated_blocks: devuelve la cantidad de nodos ocupados
//----------------------------------------------------------------------
// Argumentos:
//
//----------------------------------------------------------------------
//
//----------------------------------------------------------------------
// Retorno:
//  La cantidad de nodos ocupados
//----------------------------------------------------------------------
uint64_t get_allocated_blocks(){
    return allocated_blocks;
}

//----------------------------------------------------------------------
// get_allocated_bytes: devuelve la cantidad de bytes ocupados
//----------------------------------------------------------------------
// Argumentos:
//
//----------------------------------------------------------------------
//
//----------------------------------------------------------------------
// Retorno:
//  La cantidad de bytes ocupados
//----------------------------------------------------------------------
uint64_t get_allocated_bytes(){
    return allocated_bytes;
}

//----------------------------------------------------------------------
// get_free_bytes: devuelve la cantidad de bytes libres
//----------------------------------------------------------------------
// Argumentos:
//
//----------------------------------------------------------------------
//
//----------------------------------------------------------------------
// Retorno:
//  La cantidad de bytes libres
//----------------------------------------------------------------------
uint64_t get_free_bytes(){
    return buddy_manager.size - allocated_bytes;
}


//----------------------------------------------------------------------
// mm_buddy_dump: imprime el estado de la memoria que maneja el buddy
//----------------------------------------------------------------------
// Argumentos:
//
//----------------------------------------------------------------------
// Si un espacio esta ocupado pone *, sino queda _
//----------------------------------------------------------------------
// Retorno:
//
//----------------------------------------------------------------------
void mm_buddy_dump(){
    char canvas[buddy_manager.size + 1];
    uint32_t i, j, node_size, offset;

    for(i = 0; i < buddy_manager.size + 1; i++){
        canvas[i] = '_';
    }

    node_size = buddy_manager.size * 2;

    for(i = 0; i < 2 * buddy_manager.size - 1; i++){
        if(IS_POWER_OF_2(i+1)){
            node_size /= 2;
        }

        if(buddy_manager.node[i].size_available == 0){
            if(i >= buddy_manager.size - 1){
                canvas[i - buddy_manager.size + 1] = '*';
            }
            else if(buddy_manager.node[LEFT_LEAF(i)].size_available && buddy_manager.node[RIGHT_LEAF(i)].size_available) {
                offset = (i+1) * node_size - buddy_manager.size;

                for(j = offset; j < offset + node_size; j++){
                    canvas[j] = '*';
                }
            }
        }
    }
    canvas[buddy_manager.size] = '\0';
    puts(canvas);
}