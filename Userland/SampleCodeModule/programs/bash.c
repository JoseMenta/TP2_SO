// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <bash.h>


char buffer[MAX_BUFFER_SIZE];       // lo hacemos global para evitar posibles problemas de stack
char arg_a[MAX_ARGS_SIZE][MAX_BUFFER_SIZE] = {{0}};         // Argumentos del programa A
char arg_b[MAX_ARGS_SIZE][MAX_BUFFER_SIZE] = {{0}};        // Argumentos del programa B
uint64_t characters_in_line = 0;

void analyze_buffer(void);
void clean_buffer(void);
void copy_token(char * token, int * start_token, int end_token);

//---------------------------------------------------------------------------------
// bash:
//      Es el programa encargado de interpretar lo que entra el usuario con el teclado
//      y ejecutar los otros programas si corresponde
//---------------------------------------------------------------------------------
// Argumentos:
//   void
//---------------------------------------------------------------------------------
// Retorno
//   void
//---------------------------------------------------------------------------------
void bash(uint64_t arg_c, char ** arg_v){
    if(arg_c!=0){
        throw_error("Error: el programa no recibe argumentos");
    }
    print_string("Bienvenido!\nQue modulo desea correr?");
    while(1){
        print_string("\n$> ");
        int len = get_line(buffer,MAX_BUFFER_SIZE);
        if(len==MAX_BUFFER_SIZE-1 && buffer[len-1]=='\n'){
            print_string("No fue posible leer lo ingresado, por favor intente nuevamente\n$");
        }else{
            analyze_buffer();
        }
    }
}


//---------------------------------------------------------------------------------
// analyze_buffer: analiza el contenido de lo que ingreso el usuario
//---------------------------------------------------------------------------------
// Argumentos:
//  void
//---------------------------------------------------------------------------------
// Se encarga de analizar lo que ingresa el usuario con el teclado, y ejecutar
// los porgramas que corresponden o lanzar los errores
//---------------------------------------------------------------------------------
void analyze_buffer(void) {
    int prev_token = 0;
    //ignoramos espacios iniciales
    for(; buffer[prev_token] == ' ' || buffer[prev_token] == '\t'; prev_token++);
    int new_token = str_tok(buffer + prev_token, ' ');
    // Si no se ingreso texto, solo ENTER se genera nueva linea de consola
    if(new_token == 0){
        print_string("\n");
        return;
    }


    char tokens[MAX_BUFFER_SIZE];
    // Copiamos el primer string en tokens
    copy_token(tokens, &prev_token, new_token);
    // Si se escribe "clear", se borran todas las lineas de comandos previas
    if (strcmp(tokens, "clear") == 0) {
        new_token = str_tok(buffer + prev_token + 1, ' ');
        // Verificamos que solo se halla ingresado "clear" y nada mas
        if (new_token == 0) {
            clean_buffer();
            clear();
            return;
        } else {
            p_error("ERROR: expresion invalida");
            clean_buffer();
            return;
        }
    }

    // Consultamos si el primer string es un programa valido
    void *program_a = get_program(tokens);
    // Si no se encontro programa, entonces no es un string valido
    if (program_a == NULL) {
        // Lanzar error: El primer string es un programa valido
        p_error("ERROR: programa invalido");
        return;
    }


    int background = 0;
    int end = 0;
    int pipe_found = 0;
    int i = 0;                                                  // Itera sobre el arreglo de argumentos
    for(; !pipe_found && !end  && !background; i++){                           // Recorrera siempre y cuando quede espacio para los argumentos o hasta llegar a un pipe o \0
        new_token = str_tok(buffer+prev_token+1, ' ');          // Obtenemos el proximo token, el cual puede ser un nuevo argumento, | o \0
        prev_token++;
        copy_token(tokens, &prev_token, new_token);
        if(i < MAX_ARGS_SIZE){
            copy_str(arg_a[i], tokens);           // Subo el argumento al arreglo de argumentos
        }
        // Si es un '|' o es el ultimo token, quiero que no lea mas argumentos
        if(strcmp(tokens, "|") == 0 || strcmp(tokens,"&")==0 || strcmp(tokens, "\0") == 0 || strcmp(tokens, "\n") == 0) {
            if(i<MAX_ARGS_SIZE){
                // Si se leyo un | o & o \0 debemos borrar el ultimo argumento pues se copio eso
                arg_a[i][0] = '\0';
                i--;
            }
            // Contabilizamos como argumento al pipe o \0, por lo que debemos decrementar
            if(strcmp(tokens,"|")==0){
                pipe_found= 1;                            // Actualizamos el flag
            }else if(strcmp(tokens, "&") == 0){
                background = 1;
            }else{
                end = 1;
            }
        }
    }

    i = (i > MAX_ARGS_SIZE) ? MAX_ARGS_SIZE : i;
    char* aux_a[] = {arg_a[0],arg_a[1],arg_a[2]};
    int arg_c_a = i;
    if (!pipe_found){
        new_token = str_tok(buffer+prev_token+1, ' ');
        //si no hay un pipe, tengo que ejecutar uno solo
        if(new_token!=0){
            p_error("ERROR: Programa ausente");
            clean_buffer();
            return;
        }
        print_string("\n");
        executable_t exec_a = {get_program_name(program_a),program_a,i,aux_a,!background,NULL};
        //print_string("\n");
        int pid = sys_exec(&exec_a);
        if(!background){
            waitpid(pid);
        }
        clean_buffer();
        return;
    }
    // Si se leyo un \0, entonces se ejecuta un solo programa


    // Si llegamos aca es porque leimos un | (pipe=1)
    new_token = str_tok(buffer+prev_token+1, ' ');
    if(new_token == 0){
        // Lanzar error: Hubo un pipe pero no hubo un string despues de él
        p_error("ERROR: Programa lector del pipe ausente.");
        clean_buffer();
        return;
    }

    prev_token++;
    copy_token(tokens, &prev_token, new_token);
    // Luego, consultamos si el primer string es un programa valido
    void * program_b = get_program(tokens);
    // Si no lo es lanza error
    if (program_b == NULL) {
        // Lanzar error: Programa invalido
        p_error("ERROR: programa lector del pipe invalido");
        clean_buffer();
        return;
    }

    // Si llegamos aca es porque leimos dos programas validos, falta leer los arguementos del segundo programa
    end = 0;
    background = 0;
    i=0;
    // Itera sobre el arreglo de argumentos
    for(; !end && !background; i++){      // Recorrera siempre y cuando quede espacio para los argumentos o hasta llegar a un pipe o \0
        new_token = str_tok(buffer+prev_token+1, ' ');          // Obtenemos el proximo token, el cual puede ser un nuevo argumento, | o \0
        prev_token++;
        copy_token(tokens, &prev_token, new_token);           // Subo el argumento al arreglo de argumentos
        if(i < MAX_ARGS_SIZE){
            copy_str(arg_b[i], tokens);           // Subo el argumento al arreglo de argumentos
        }
        // Si es el ultimo token, quiero que no lea mas argumentos
        if(strcmp(arg_b[i], "\0") == 0 || strcmp(arg_b[i], "\n")==0 || strcmp(arg_b[i], "&")==0){
            if(i<MAX_ARGS_SIZE) {
                arg_b[i][0] = '\0';                                 // Si se leyo un \0 debemos borrar el ultimo argumento pues se copio eso
                i--;                                                // Contabilizamos como argumento al pipe o \0, por lo que debemos decrementar
            }                                             // Contabilizamos como argumento al pipe o \0, por lo que debemos decrementar
            if(strcmp(tokens, "&")==0){
                background = 1;
            }else{
                end = 1;
            }
        }
    }
    char* aux_b[] = {arg_b[0],arg_b[1], arg_b[2]};

    //tengo que crear el pipe
    int fd[2];
    pipe(fd);
    int newFdLeft[DEFAULTFD] =  {0, fd[1], 2};
    int newFdRight[DEFAULTFD] =  {fd[0], 1, 2};
    executable_t exec_a = {get_program_name(program_a),program_a,arg_c_a,aux_a,!background, newFdLeft};
    uint64_t pidLeft = sys_exec(&exec_a);
    close_fd(fd[1]);
    executable_t exec_b = {get_program_name(program_b),program_b,i,aux_b,!background, newFdRight};
    uint64_t pidRight = sys_exec(&exec_b);
    close_fd(fd[0]);
    if(!background) {
        waitpid(pidLeft);
        waitpid(pidRight);
    }
    clean_buffer();
    return;
}


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
    for(int i = 0; i < MAX_BUFFER_SIZE; i++){
        buffer[i] = '\0';
    }
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
