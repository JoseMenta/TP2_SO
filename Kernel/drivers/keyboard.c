#include <keyboard.h>
#include <scheduler.h>
#include <interrupts.h>
#include <queue.h>
#include <pipes.h>
#include "../include/pipes.h"
#include "../include/keyboard.h"
#include "../include/video_driver.h"
#define IS_ALPHA(x) ((x) >= 'a' && (x) <= 'z') ? 1 : 0
#define IS_REFERENCEABLE(x) ((x) <= KEYBOARD_REFERENCE_LENGTH && keyboard_reference[(x)]!='\0')

static void copy_curr_context_to_inforeg_context();    // Funcion auxiliar para guardar el contexto que utiliza inforeg

//El buffer guarda el ASCII del valor presionado en el teclado. Utiliza la estructura queue definida en queue.c
queue_t queue = {{0}};
static int key_case = -1;               // Estado actual del formato de la letra (mayuscula: 1, o minuscula: -1)
static int key_case_default = -1;       // Estado default del formato de la letra

//Flags de control para ver si se presionaron algunas tecas o para guardar el estado de los procesos y llamar a
//otras funciones, como las del scheduler
static int ctrl_pressed = 0;
static int alt_pressed = 0;
static int left_state = 1;
static int right_state = 1;
static int all_state = 1;
uint8_t regs_saved = 0;

uint64_t* get_inforeg_context();
//teclado Mac
static int keyboard_reference[] = {'\0','\0','1','2','3','4','5',
                                   '6','7','8','9','0','-','=',
                                   ASCII_DELETE,'\t','q','w','e','r','t',
                                   'y','u','i','o','p','[',']','\n',
                                   '\0','a','s','d','f','g','h','j',
                                   'k','l',';','\'','|','\0','\\',
                                   'z','x','c','v','b','n','m',',',
                                   '.','/','\0', '\0', ALT, ' '};
//teclado windows
/*static int keyboard_reference2[] = {'0', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0',
                                    '\'', '¡', '0', '0', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i',
                                    'o', 'p', '\'', '+', '\n', '0', 'a', 's', 'd', 'f', 'g', 'h',
                                    'j', 'k', 'l', 'ñ', '{', '0', '^', '0', 'z', 'x', 'c', 'v',
                                    'b', 'n', 'm', ',', '.', '-', '^', '0', '0', '\t'}
*/

//----------------------------------------------------------------------
// keyboard_handler: handler para la interrupcion del teclado
//----------------------------------------------------------------------
// Argumentos
//  void
//----------------------------------------------------------------------
// El handler se encarga de agregar la tecla al buffer si se puede representar en pantalla,
// o se encarga de llamar a las funciones correctas para pausar, reanudar o terminar procesos,
// o copiar el contexto del programa en un arreglo auxiliar definido en libasm.asm para inforeg
//----------------------------------------------------------------------
void keyboard_handler(){
    // Obtenemos la tecla ingresada
    uint8_t key = get_keyboard_scan_code();

    //Agrego la logica para cuando quiere volver
    uint8_t want_to_return;
    if(key == ESC){
        //TODO: hacer que mate a los foreground y vuelva a bash
    }

    // Logica para escribir en mayuscula
    else if(key == SHIFT1 || key == SHIFT2) {
        //Estamos usando que cuando se mantiene shift, no se mandan varias interrupciones
        //Si ese fuese el caso, entonces el comportamiento seria aleatorio
        key_case = key_case_default * -1;   //Invierte el default
    }
    else if (key == SHIFT1+RELEASED || key == SHIFT2+RELEASED){
        key_case = key_case_default;        // Vuelve al estado default
    }

    // Logica para el pausado y reanudado de procesos
    else if(key == CTRL){
        ctrl_pressed = 1;
    }
    else if (key == CTRL+RELEASED) {
        ctrl_pressed = 0;
    }

    // Logica para detener procesos
    else if(key == ALT){
        alt_pressed = 1;
    } else if (key == ALT + RELEASED){
        alt_pressed = 0;
    }

    // Guardamos en el buffer solo aquellas teclas que puedan ser referenciables: letras, digitos y espacios (\n, \t, etc)
    else if(IS_REFERENCEABLE(key) && key != BLOCK_MAYUSC){
        // Suspension y reanudacion de procesos
        // Primero debemos verificar si en vez de agregar una letra al buffer, se desea suspende o reanudar un proceso
//        if(keyboard_reference[key] == 'f' && ctrl_pressed){
//            //all_state: 1 si sigue corriendo, 0 si no
//            if(all_state){
//                // Suspende el proceso si esta corriendo en pantalla completo y cambia el flag de all_state al asignarse
//                // Si no esta corriendo en pantalla completa (se hizo un split), entonces no hay que cambiar el flag
////                all_state = suspend_full();
//            }else{
//                // Siempre podra restartear el proceso si all_state = 0 pues tuvo que poder suspenderlo
////                all_state = restart_full();
//            }
//        }
//        else if(keyboard_reference[key] == 'l' && ctrl_pressed){
//            //left_state: 1 si sigue corriendo, 0 si no
//            if(left_state){
//                // Suspende el proceso si esta corriendo en pantalla split y cambia el flag de left_state al asignarse
//                // Si no esta corriendo en split (esta en pantalla completa), entonces no hay que cambiar el flag
////                left_state = suspend_left();
//            }else{
//                // Siempre podra restartear el proceso si left_state = 0 pues tuvo que poder suspenderlo
////                left_state = restart_left();
//            }
//        }
//        else if(keyboard_reference[key] == 'r' && ctrl_pressed){
//            //right_state: 1 si sigue corriendo, 0 si no
//
//            if(right_state){
//                // Suspende el proceso si esta corriendo en pantalla split y cambia el flag de right_state al asignarse
//                // Si no esta corriendo en split (esta en pantalla completa), entonces no hay que cambiar el flag
////                right_state = suspend_right();
//            }else{
//                // Siempre podra restartear el proceso si right_state = 0 pues tuvo que poder suspenderlo
////                right_state = restart_right();
//            }
//        }
//
//
//        // Detenimiento de procesos
//        else if(keyboard_reference[key] == 'k' && alt_pressed){
////            all_state = kill_full();
//        }
//        else if (keyboard_reference[key] == 'l' && alt_pressed){
////            left_state = kill_left();
//        }
//        else if(keyboard_reference[key] == 'r' && alt_pressed){
////            right_state = kill_right();
//        }
//        else if(keyboard_reference[key] == 's' && ctrl_pressed){
////            copy_curr_context_to_inforeg_context();
//        }

        if(keyboard_reference[key]=='7' && key_case>0){
            //tengo que mandar un &
            char aux = '&';
            write_keyboard( &aux, 1);
        }else if(keyboard_reference[key]=='d' && ctrl_pressed){
            char aux = EOF;
            write_keyboard( &aux, 1);
        }
        else if (key_case > 0 && IS_ALPHA(keyboard_reference[key])){
            // Es una mayuscula
            //buffer[write] = keyboard_reference[key] - UPPER_OFFSET;
            //enqueue(&queue,keyboard_reference[key] - UPPER_OFFSET);
            char aux = keyboard_reference[key] - UPPER_OFFSET;
            write_keyboard( &aux, 1);
        } else {
            // Es una minuscula u otro tipo de caracter que no es letra
            //buffer[write] = keyboard_reference[key];
            //enqueue(&queue,keyboard_reference[key]);
            char aux = keyboard_reference[key];
            write_keyboard( &aux, 1);

        }
    }
}
//----------------------------------------------------------------------
// copy_curr_context_to_inforeg_context: funcion para guardar el contexto actual
//----------------------------------------------------------------------
// Argumentos
//  void
//----------------------------------------------------------------------
// La funcion guarda al contexto actual en el arreglo definido en libasm.asm, que
// se utiliza luego por el comando inforeg
//----------------------------------------------------------------------
static void copy_curr_context_to_inforeg_context(){
    regs_saved = 1;
    uint64_t* curr_context = getCurrContext(); //Guardo el contexto en la interrupcion
    uint64_t* inforeg_context = get_inforeg_context();
    for(int i = 0; i<REGISTERS_COUNT; i++){
        inforeg_context[i] = curr_context[i];//Copio el contexto para cuando lo llamen
    }
    return;
}
//----------------------------------------------------------------------
// clear_keyboard_handler: funcion que elimina el contenido del buffer
//----------------------------------------------------------------------
// Argumentos
//  void
//----------------------------------------------------------------------
// Es una funcion que se llama en otros momentos (como cuando se reanuda un
// proceso en el scheduler) para limpiar el buffer e ignorar a los caracteres
// ingresados en una pausa
//----------------------------------------------------------------------
void clear_keyboard_buffer(){
    clear_queue(&queue);
}

