// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <keyboard.h>
#include <scheduler.h>
#include <interrupts.h>
#include <pipes.h>



static int key_case = -1;               // Estado actual del formato de la letra (mayuscula: 1, o minuscula: -1)
static int key_case_default = -1;       // Estado default del formato de la letra

//Flags de control para ver si se presionaron algunas tecas
static int ctrl_pressed = 0;
static int alt_pressed = 0;
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
// o se encarga de llamar a las funciones correctas para terminar procesos,
//----------------------------------------------------------------------
void keyboard_handler(){
    // Obtenemos la tecla ingresada
    uint8_t key = get_keyboard_scan_code();

    //Agrego la logica para cuando quiere volver
    if(key == ESC){
        //  kill_foreground_process();
    }

    // Logica para escribir en mayuscula
    //Estamos usando que cuando se mantiene shift, no se mandan varias interrupciones
    //Si ese fuese el caso, entonces el comportamiento seria aleatorio
    else if(key == SHIFT1 || key == SHIFT2) {
        //Invierte el default
        key_case = key_case_default * -1;
    }
    else if (key == SHIFT1+RELEASED || key == SHIFT2+RELEASED){
        // Vuelve al estado default
        key_case = key_case_default;
    }

    // Logica para tecla CTRL
    else if(key == CTRL){
        ctrl_pressed = 1;
    }
    else if (key == CTRL+RELEASED) {
        ctrl_pressed = 0;
    }

    // Logica para tecla ALT
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

