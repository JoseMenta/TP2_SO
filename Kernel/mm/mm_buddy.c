// Implementación del Memory Manager Buddy
// Algoritmo Buddy: https://www.youtube.com/watch?v=DRAHRJEAEso&ab_channel=GabrielParmer
// Basado en: https://github.com/wuwenbin/buddy2

// La implementación utiliza un arreglo el cual lo maneja como árbol binario mediante indices

#include <mm.h>
#include <mm_buddy.h>

/* ----------------------------------------------------------------------------------------------------------------
 * Buddy
 *
 * Tamaño total de heap: 128 MB
 * Tamaño minimo de bloque: 8 B
 *
 * Cant. de nodos: 1 (128 MB) + 2 (64 MB) + 4 (32 MB) + 8 (16 MB) + 16 (8 MB) + 32 (4 MB) + 64 (2 MB) + 128 (1 MB) +
		    256 (512 KB) + 512 (256 KB) + 1024 (128 KB) + 2048 (64 KB) + 4096 (32 KB) + 8192 (16 KB). +
                16384 (8 KB) + 32768 (4 KB) + 65536 (2 KB) + 131072 (1 KB) + 262144 (512 B) + 524288 (256 B) +
   	  	    1048576 (128 B) + 2097152 (64 B) + 4194304 (32 B) + 8388608 (16 B) + 16777216 (8 B) = 33554431 nodos

 * Tamaño de un nodo: 4 B
 * Tamaño total de nodos: 134217724 B
 *
 * Tamaño del buddy: 16 B
 *
 * Tamaño total: 134217740 B = 134.21774 MB < 135 MB
 *
 * Inicio del buddy:    0x  600000 (6 MB)
 * Fin del buddy: 	    0x 8D00000 (141 MB)
 *
 * Inicio del heap: 	0x 8E00000 (142 MB)
 * Fin del heap:	    0x10E00000 (270 MB)
// ---------------------------------------------------------------------------------------------------------------- */


#define BUDDY_MANAGER_START 0x600000
#define BUDDY_MANAGER_END   0x8D00000

#define HEAP_START          0x8E00000
#define HEAP_END            0x10E00000

#define NODES               33554431
#define MIN_SIZE            8
#define MM_SIZE             (((uint8_t *)HEAP_END) - ((uint8_t *)HEAP_START))

#define LEFT_LEAF(index)    ((index) * 2 + 1)               // Devuelve el indice del nodo hijo izquierdo de index
#define RIGHT_LEAF(index)   ((index) * 2 + 2)               // Devuelve el indice del nodo hijo derecho de index
#define PARENT(index)       (((index) + 1) / 2 - 1)         // Devuelve el indice del nodo padre de index

#define IS_POWER_OF_2(x)    (!((x) & ((x) - 1)))            // Indica si x es potencia de dos o no
#define MAX(a, b)           (((a) > (b)) ? (a) : (b))       // Devuelve el mayor entre a y b

static const uint8_t * start_address = (uint8_t *) HEAP_START;
static const uint8_t * end_address = (uint8_t *) HEAP_END;
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
} node_t;                                               // Tamaño: 4B


typedef struct
{
    uint32_t size;                                      // Tamaño del heap
    node_t * node;                                      // Cantidad de nodos del arbol de buddy
} buddy_t;                                              // Tamaño: 16B


// Inicializo el buddy
static buddy_t buddy_manager = {-1, (node_t *) BUDDY_MANAGER_START};


// Setea el buddy para empezar a manejar el heap
void mm_init(){
    uint8_t * address = (uint8_t *) start_address;
    uint32_t node_size, buddy_size;

    // Define el tamaño del heap que va a manejar el buddy system
    buddy_size = previous_power_of_2(MM_SIZE);
    buddy_manager.size = buddy_size;

    node_size = buddy_size * 2;

    // Seteamos a cada nodo el tamaño maximo a manejar
    for(uint32_t i = 0; i < NODES; i++){
        // Cada vez que se llegue a una potencia de 2 es porque se paso al siguiente nivel del arbol
        // por lo que el tamaño del nodo decrece a la mitad y comienza a apuntar nuevamente al principio de la memoria
        if(IS_POWER_OF_2(i+1)){
            address = (uint8_t *) start_address;
            node_size /= 2;
        }
        buddy_manager.node[i].size_available = node_size;
        address += node_size;
    }
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
    uint8_t * address = (uint8_t *) start_address;

    // Si es la primera vez que se solicita memoria, se inicializa el buddy
    if(buddy_manager.size == -1){
        mm_init();
    }

    // Si el espacio solicitado no es potencia de 2, lo ajustamos a la proxima potencia de 2 para ajustarse al nodo correspondiente en el buddy
    // Otro caso es que el tamaño solicitado sea menor al minimo nodo, en ese caso se debe ajustar el tamaño solicitado al minimo
    if (wanted_size == 0){
        return NULL;
    } else if (wanted_size < MIN_SIZE){
        wanted_size = MIN_SIZE;
    } else if (!IS_POWER_OF_2(wanted_size)){
        wanted_size = next_power_of_2(wanted_size);
    }

    // Si no hay espacio suficiente para alojar el espacio solicitado, se retorna NULL
    if(buddy_manager.node[index].size_available < wanted_size){
        return NULL;
    }

    // Busca el índice del nodo que debe asignar para mantener el espacio solicitado, priorizando los subarboles izquierdos
    for(node_size = buddy_manager.size; node_size != wanted_size; node_size /= 2){
        if(buddy_manager.node[LEFT_LEAF(index)].size_available >= wanted_size){
            index = LEFT_LEAF(index);
        } else {
            // Si nos movemos a la derecha, debemos ajustar address a la direccion de memoria que le corresponde al nodo derecho
            address += node_size / 2;
            index = RIGHT_LEAF(index);
        }
    }

    // Una vez que encuentro el nodo, actualizo su espacio disponible (0 pues se ocupo)
    buddy_manager.node[index].size_available = 0;

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
    uint8_t * address = (uint8_t *) start_address;

    node_size = buddy_manager.size * 2;

    // Busco el nodo cuya direccion inicial sea p
    for(uint32_t i = 0; index == -1 && i < NODES; i++){
        if(IS_POWER_OF_2(i+1)){
            // Cada vez que alcanzo una potencia de 2, bajo un nivel mas en el arbol, empezando por el nodo más a la izquierda
            address = (uint8_t *) start_address;
            node_size /= 2;
        }
        // Si lo encuentro, me guardo su índice y restauro el tamaño inicial del nodo
        if(address == (uint8_t *) p && buddy_manager.node[i].size_available == 0 &&
           (node_size == MIN_SIZE || (buddy_manager.node[LEFT_LEAF(i)].size_available != 0 && buddy_manager.node[RIGHT_LEAF(i)].size_available != 0))){
            buddy_manager.node[i].size_available = node_size;
            index = i;
        }
        // Si no lo encuentro, entonces me muevo a la dirección del proximo nodo a la derecha (si es que hay)
        else {
            address += node_size;
        }
    }

    // Si index no cambio es porque la direccion no es válida para liberar, por lo que retorna
    if(index == -1){
        return;
    }

    // Se actualiza el espacio ocupado y los bloques ocupados
    allocated_bytes -= node_size;
    allocated_blocks--;

    // Ahora se debe actualizar el tamaño disponible de los nodos ancetros del nodo que se liberó
    while(index) {
        index = PARENT(index);
        node_size *= 2;

        left_size = buddy_manager.node[LEFT_LEAF(index)].size_available;
        right_size = buddy_manager.node[RIGHT_LEAF(index)].size_available;

        // Si tanto hijo izquierdo como hijo derecho están liberados (la suma de su espacio libre equivale al espacio esperado del nodo padre), entonces el nodo esta completamente libre
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
    char * canvas = (char *) BUDDY_MANAGER_END - (buddy_manager.size / MIN_SIZE + 1) - 1;
    uint32_t i, j, node_size, offset;

    for(i = 0; i < buddy_manager.size / MIN_SIZE + 1; i++){
        canvas[i] = '_';
    }

    node_size = buddy_manager.size * 2;

    for(i = 0; i < NODES; i++){
        if(IS_POWER_OF_2(i+1)){
            node_size /= 2;
        }

        if(buddy_manager.node[i].size_available == 0){
            if(node_size == MIN_SIZE){
                print("\nTamaño del nodo: ", WHITE, ALL);
                print_dec_format(node_size, ALL);
                print("\nCantidad: 1\n", WHITE, ALL);
                print("Posicion: ", WHITE, ALL);
                print_dec_format(i - buddy_manager.size / MIN_SIZE + 1, ALL);
                new_line(ALL);
                canvas[i - buddy_manager.size / MIN_SIZE + 1] = '*';
            }
            else if(buddy_manager.node[LEFT_LEAF(i)].size_available && buddy_manager.node[RIGHT_LEAF(i)].size_available) {
                offset = ((i + 1) * node_size - buddy_manager.size) / MIN_SIZE;

                print("\nTamaño del nodo: ", WHITE, ALL);
                print_dec_format(node_size, ALL);
                print("\nCantidad: ", WHITE, ALL);
                print_dec_format(node_size / MIN_SIZE, ALL);

                for(j = offset; j < offset + node_size / MIN_SIZE; j++){
                    print("\nPosicion: ", WHITE, ALL);
                    print_dec_format(j, ALL);
                    canvas[j] = '*';
                }
            }
        }
    }
    canvas[buddy_manager.size / MIN_SIZE] = '\0';
    print("\n\nEstado de la memoria\n*: Byte ocupado\n_: Byte libre\n", WHITE, ALL);
    print(canvas, WHITE, ALL);
    print("\n\n", WHITE, ALL);
}