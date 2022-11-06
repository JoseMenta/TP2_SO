// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <tests.h>
#include <libc.h>
//Teseos utlizados en el desarrollo del TP
/*
void write_test(){
//    sys_write("Imprimiendo en blanco\n", WHITE);
//    sys_write("imprimiendo en rojo\n", RED);
//    sys_write("3 para abajo:\n\n\nfin", WHITE);

    //sys_write("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaabbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbcccccccccccccccccccccccccccccccccccccccaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaabbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbcccccccccccccccccccccccccccccccccccccccaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaabbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbcccccccccccccccccccccccccccccccccccccccaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaabbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbcccccccccccccccccccccccccccccccccccccccaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaabbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbcccccccccccccccccccccccccccccccccccccccaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaabbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbcccccccccccccccccccccccccccccccccccccccaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaabbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbcccccccccccccccccccccccccccccccccccccccaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaabbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbcccccccccccccccccccccccccccccccccccccccaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaabbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbcccccccccccccccccccccccccccccccccccccccaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaabbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbcccccccccccccccccccccccccccccccccccccccaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaabbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbcccccccccccccccccccccccccccccccccccccccaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaabbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbccccccccccccccccccccccccccccccccccccccc\n", WHITE);
    sys_write("HOLA",WHITE);
    void * aux[2] = {read_test};
    sys_exec(1, aux);
//    while(1){};
    sys_write("Volvi al primero",WHITE);
    sys_exit();
}

void read_test(){
//    char str[10] = {0};
//    uint64_t i = 0 ;
    print_string("En el otro proceso",WHITE);
    sys_exit();
//    while(1){
//        print_number(get_register(),WHITE);
//        print_string("\n",WHITE);
////        print_string("a",WHITE);
////        print_string("\n",WHITE);
//        i = (i+1)%10;
//    }
//    while(1){
//        sys_read(str);
//        sys_write(str, WHITE);
//    }

    //Si no esta esto, cuando llegue al final de la funcion va a hacer ret, pero en la direccion de retorno no va a encontrar una direccion (lo que encuentra es basura)

//    str[0] = '\0';
//    int read = 0;
//    while(read < 5){
//        sys_read(str);
//        if(str[read] != '\0'){
//            read++;
//            str++;
//        }
//    }
//    print_string("\n\n String de 5 caracteres: ", WHITE);
//    print_string(str, BLUE);
}
void other_call_full(){
    sys_write("en el segundo llamado",WHITE);
    sys_exit();
}
void multiple_full_call(){
    sys_write("En el primer llamado",WHITE);
    void * aux[2] = {other_call_full};
    sys_exec(1, aux);
    sys_write("Volvi al primer llamado",WHITE);
    sys_exit();
}
void other_call_left(){
    sys_write("A la izquierda",WHITE);
    sys_exit();
}
void other_call_right(){
    sys_write("A la derecha",WHITE);
    sys_exit();
}
void multiple_left_right_call(){
    sys_write("en el primero",WHITE);
    void * aux[] = {other_call_left,other_call_right};
    sys_exec(2,aux);
    sys_write("Volvi al primer llamado",WHITE);
    sys_exit();
}
void infinite_test(){
    uint8_t i = 0;
    while(1){
        print_number(i,WHITE);
        i = (i+1)%10;
    }
}
void print_test(){
    print_string("Imprimiendo en blanco\n", WHITE);
    print_string("imprimiendo en rojo\n", RED);
    print_string("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaabbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbcccccccccccccccccccccccccccccccccccccccaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaabbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbcccccccccccccccccccccccccccccccccccccccaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaabbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbcccccccccccccccccccccccccccccccccccccccaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaabbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbcccccccccccccccccccccccccccccccccccccccaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaabbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbcccccccccccccccccccccccccccccccccccccccaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaabbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbcccccccccccccccccccccccccccccccccccccccaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaabbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbcccccccccccccccccccccccccccccccccccccccaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaabbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbcccccccccccccccccccccccccccccccccccccccaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaabbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbcccccccccccccccccccccccccccccccccccccccaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaabbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbcccccccccccccccccccccccccccccccccccccccaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaabbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbcccccccccccccccccccccccccccccccccccccccaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaabbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbccccccccccccccccccccccccccccccccccccccc\n", WHITE);
    print_string("deberia dar 54: ", YELLOW);
    print_number(54, WHITE);
}
*/