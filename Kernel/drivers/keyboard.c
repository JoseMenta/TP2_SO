// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
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


//El buffer guarda el ASCII del valor presionado en el teclado. Utiliza la estructura queue definida en queue.c
queue_t queue = {{0}};
static int key_case = -1;               // Estado actual del formato de la letra (mayuscula: 1, o minuscula: -1)
static int key_case_default = -1;       // Estado default del formato de la letra

//Flags de control para ver si se presionaron algunas tecas o para guardar el estado de los procesos y llamar a
//otras funciones, como las del scheduler
static int ctrl_pressed = 0;
static int alt_pressed = 0;
//static int left_state = 1;
//static int right_state = 1;
//static int all_state = 1;
uint8_t regs_saved = 0;

//teclado Mac
static int keyboard_reference[] = {'\0','\0','1','2','3','4','5',
                                   '6','7','8','9','0','-','=',
                                   ASCII_DELETE,'\t','q','w','e','r','t',
                                   'y','u','i','o','p','[',']','\n',
                                   '\0','a','s','d','f','g','h','j',
                                   'k','l',';','\'','|','\0','\\',
                                   'z','x','c','v','b','n','m',',',
                                   '.','/','\0', '\0', ALT, ' ',' '};
//teclado windows
//static int keyboard_reference2[] = {'0', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0',
//                                    '\'', '¡', '0', '0', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i',
//                                    'o', 'p', '\'', '+', '\n', '0', 'a', 's', 'd', 'f', 'g', 'h',
//                                    'j', 'k', 'l', 'ñ', '{', '0', '^', '0', 'z', 'x', 'c', 'v',
//                                    'b', 'n', 'm', ',', '.', '-', '^', '0', '0', '\t'}


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
            char aux = keyboard_reference[key] - UPPER_OFFSET;
            write_keyboard( &aux, 1);
        } else {
            // Es una minuscula u otro tipo de caracter que no es letra
            char aux = keyboard_reference[key];
            write_keyboard( &aux, 1);

        }
    }
}
