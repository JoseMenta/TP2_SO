#include <bash.h>
#include <programs.h>
#include <libc.h>

char buffer[MAX_BUFFER_SIZE];
uint8_t buffer_index = 0;
uint64_t characters_in_line = 0;
void analyze_buffer(void);
void clean_buffer(void);
void copy_token(char * token, int * start_token, int end_token);

//---------------------------------------------------------------------------------
// bash: Programa a correr al inciar el UserSpace
//---------------------------------------------------------------------------------
// Argumentos:
//   void
//---------------------------------------------------------------------------------
// Es el programa encargado de interpretar lo que entra el usuario con el teclado
// y ejecutar los otros programas si corresponde
//---------------------------------------------------------------------------------
// Retorno
//   void
//---------------------------------------------------------------------------------
//void bash(uint64_t arg_c, const char ** arg_v){
//    if(arg_c!=0){
//        throw_error("Error: el programa no recibe argumentos");
//    }
//    static uint64_t last_ticks = 0;
//    print_string("Bienvenido!\nQue modulo desea correr?\n$ ",WHITE);
//    char c[2] = {0, 0};
//    while(1){
//        c[0] = get_char();
//        if(c[0] =='\n') {
//            // analyze_buffer hace el manejo de sys_exec.
//            //si corre un programa limpia la pantalla realiza la sys_exec() y vuelve a limpairla
//            //sino entonces dejo el \n e imprimo nueva linea con el buffer en 0
//            analyze_buffer();
//            clean_buffer();
//            print_string("$ ", WHITE);
//            characters_in_line = 0;
//        }else if(c[0] == ASCII_DELETE){
//
//            if(buffer_index != 0) {
//
//                //si quiero borrar, al imprimirlo ya lo saque del video driver
//                //ahora lo saco del buffer.
//                buffer[--buffer_index] = '\0';
//            }
//            if(characters_in_line > 0){
//                print_string(c, WHITE);
//                characters_in_line--;
//            }
//        }else if(c[0] != 0 && buffer_index < MAX_BUFFER_SIZE-1) {
//            buffer[buffer_index++] = c[0];
//            buffer[buffer_index] = '\0';
//            print_string(c, WHITE);
//            characters_in_line++;
//        } else {
//            uint64_t ticks = sys_tick();
//            if(ticks - last_ticks>=10){
//                blink();
//                last_ticks = ticks;
//            }
//        }
//    }
//    sys_exit();
//}

//---------------------------------------------------------------------------------
// analyze_buffer: analiza el contenido de lo que ingreso el usuario
//---------------------------------------------------------------------------------
// Argumentos:
//  void
//---------------------------------------------------------------------------------
// Se encarga de analizar lo que ingresa el usuario con el teclado, y ejecutar
// los porgramas que corresponden o lanzar los errores
//---------------------------------------------------------------------------------
//void analyze_buffer(void) {
//    int prev_token = 0;
//    for(; buffer[prev_token] == ' ' || buffer[prev_token] == '\t'; prev_token++);
//    int new_token = str_tok(buffer + prev_token, ' ');
//    // Si no se ingreso texto, solo ENTER, no se hace nada
//    if(new_token == 0){
//        print_string("\n", WHITE);
//        return;
//    }
//    char tokens[100];
//    copy_token(tokens, &prev_token, new_token);                // Copiamos el primer string a aux
//
//    // Si se escribe "logout", bash se cierra y se apaga la compu
//    if (strcmp(tokens, LOGOUT) == 0) {
//        new_token = str_tok(buffer + prev_token + 1, ' ');
//        // Verificamos que solo se halla ingresado "logout" y nada mas
//        if (new_token == 0) {
//            print_string("\nLa computadora esta lista para apagarse.\n", WHITE);
//            sys_exit();
//        } else {
//            print_string("\nERROR: expresion invalida\n", RED);
//            return;
//        }
//    }
//
//    // Si se escribe "clear", se borran todas las lineas de comandos previas
//    if (strcmp(tokens, "clear") == 0) {
//        new_token = str_tok(buffer + prev_token + 1, ' ');
//        // Verificamos que solo se halla ingresado "clear" y nada mas
//        if (new_token == 0) {
//            clean_buffer();
//            clear();
//            return;
//        } else {
//            print_string("\nERROR: expresion invalida\n", RED);
//            return;
//        }
//    }
//
//    // Consultamos si el primer string es un programa valido
//    void *program_a = get_program(tokens);
//    // Si no se encontro programa, entonces no es un string valido
//    if (program_a == NULL) {
//        // Lanzar error: El primer string es un programa valido
//        print_string("\nERROR: programa invalido\n", RED);
//        return;
//    }
//
//    char arg_a[MAX_ARGS_SIZE][MAX_BUFFER_SIZE] = {{0}};         // Argumentos del programa A
//    int pipe_or_end_reached = 0;                                // Flag que indica si se llego a un pipe o al final del string
//    int i = 0;                                                  // Itera sobre el arreglo de argumentos
//    for(; !pipe_or_end_reached; i++){                           // Recorrera siempre y cuando quede espacio para los argumentos o hasta llegar a un pipe o \0
//        new_token = str_tok(buffer+prev_token+1, ' ');          // Obtenemos el proximo token, el cual puede ser un nuevo argumento, | o \0
//        prev_token++;
//        copy_token(tokens, &prev_token, new_token);
//        if(i < MAX_ARGS_SIZE){
//            copy_str(arg_a[i], tokens);           // Subo el argumento al arreglo de argumentos
//        }
//        // Si es un '|' o es el ultimo token, quiero que no lea mas argumentos
//        if(strcmp(tokens, "|") == 0 || strcmp(tokens, "\0") == 0) {
//            arg_a[i][0] = '\0';                                 // Si se leyo un | o \0 debemos borrar el ultimo argumento pues se copio eso
//            i--;                                                // Contabilizamos como argumento al pipe o \0, por lo que debemos decrementar
//            pipe_or_end_reached = 1;                            // Actualizamos el flag
//        }
//    }
//    i = (i > MAX_ARGS_SIZE) ? MAX_ARGS_SIZE : i;
//    char* aux_a[] = {arg_a[0],arg_a[1]};                        // Este vector es necesario, si no se pierde informacion con el casteo directo a char**
//    program_t struct_a = {program_a, i, aux_a};
//    // Si se leyo un \0, entonces se ejecuta un solo programa
//    if (new_token == 0) {
//        program_t structs[] = {struct_a};
//        sys_exec(1, structs);
//        return;
//    }
//
//    // Si llegamos aca es porque leimos un |
//    new_token = str_tok(buffer+prev_token+1, ' ');
//    if(new_token == 0){
//        // Lanzar error: Hubo un pipe pero no hubo un string despues de él
//        print_string("\nERROR: Programa de consola derecha ausente\n", RED);
//        return;
//    }
//
//    prev_token++;
//    copy_token(tokens, &prev_token, new_token);
//    // Luego, consultamos si el primer string es un programa valido
//    void * program_b = get_program(tokens);
//    // Si no lo es lanza error
//    if (program_b == NULL) {
//        // Lanzar error: Programa invalido
//        print_string("\nERROR: programa para consola derecha invalido\n", RED);
//        return;
//    }
//
//    // Si llegamos aca es porque leimos dos programas validos, falta leer los arguementos del segundo programa
//
//
//    char  arg_b[MAX_ARGS_SIZE][MAX_BUFFER_SIZE] = {{0}};        // Argumentos del programa B
//    pipe_or_end_reached = 0;                                    // Flag que indica si se llego a un pipe o al final del string
//    i=0;                                                        // Itera sobre el arreglo de argumentos
//    for(; i < MAX_ARGS_SIZE && !pipe_or_end_reached; i++){      // Recorrera siempre y cuando quede espacio para los argumentos o hasta llegar a un pipe o \0
//        new_token = str_tok(buffer+prev_token+1, ' ');          // Obtenemos el proximo token, el cual puede ser un nuevo argumento, | o \0
//        prev_token++;
//        copy_token(arg_b[i], &prev_token, new_token);           // Subo el argumento al arreglo de argumentos
//
//        // Si es el ultimo token, quiero que no lea mas argumentos
//        if(strcmp(arg_b[i], "\0") == 0) {
//            arg_b[i][0] = '\0';                                 // Si se leyo un \0 debemos borrar el ultimo argumento pues se copio eso
//            i--;                                                // Contabilizamos como argumento al pipe o \0, por lo que debemos decrementar
//            pipe_or_end_reached = 1;                            // Actualizamos el flag
//        }
//    }
//    char* aux_b[] = {arg_b[0],arg_b[1]};
//    program_t struct_b = {program_b, i, aux_b};
//    // Y se ejecutan los dos ultimos programas
//    program_t structs[] = {struct_a, struct_b};
//    sys_exec(2, structs);
//    return;
//
//}


//---------------------------------------------------------------------------------
// clean_buffer: Limpia el buffer
//---------------------------------------------------------------------------------
// Argumentos:
//   void
//---------------------------------------------------------------------------------
// Retorno
//   void
//---------------------------------------------------------------------------------
void clean_buffer(void){
    for(int i = 0; i < buffer_index; i++){
        buffer[i] = '\0';
    }
    buffer_index = 0;
}

//---------------------------------------------------------------------------------
// copy_token: Copia en un string lo que haya en el buffer entre dos punteros, start_token y end_token. Finaliza con start_token = end_token para el proximo token en el buffer
//---------------------------------------------------------------------------------
// Argumentos:
//   token: El string sobre el que se copia
//   start_token: El indice donde se comienza a copiar el buffer
//   end_token: El indice donde se termina de copiar el buffer
//---------------------------------------------------------------------------------
// Retorno
//   Actualiza el string token
//---------------------------------------------------------------------------------
void copy_token(char * token, int * start_token, int end_token){
    int i = 0;
    end_token += *start_token;
    for(; *start_token < end_token ; (*start_token)++, i++){
        token[i] = buffer[*start_token];
    }
    token[i] = '\0';
}
