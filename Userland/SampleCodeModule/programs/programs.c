#include <programs.h>
#include <libc.h>
#include "../include/libc.h"
static const char * Names[COUNT_REGS] = { "R8: ", "R9: ", "R10: ", "R11: ", "R12: ", "R13: ", "R14: ", "R15: ", "RAX: ", "RBX: ", "RCX: ", "RDX: ", "RSI: ", "RDI: ", "RBP: ", "RSP: ", "RIP: ", "FLAGS: "};

const time_func time_arr[] = {get_secs, 0, get_min, 0, get_hour, 0, get_day_week, get_day, get_month, get_year};
//---------------------------------------------------------------------------------
// help: imprime informacion sobre los programas disponibles
//---------------------------------------------------------------------------------
// Argumentos:
//  arg_c: cantidad de argumentos del programa (0)
//  arg_v: arreglo con los strings de los argumentos
//---------------------------------------------------------------------------------
//void help(uint64_t arg_c, const char ** arg_v){
//    extern front_program_t programs[CANT_PROG];
//    if(arg_c!=0){
//        throw_error("Error: el programa no recibe argumentos");
//    }
//    print_string("Programas disponibles:\n");
//    for(int i = 0; i<CANT_PROG;i++){
//        print_string(programs[i].desc);
//    }
//    sys_exit();
//}

//---------------------------------------------------------------------------------
// inforeg: imprime los registros guardados con la combinacion de teclas Control+s
//---------------------------------------------------------------------------------
// Argumentos:
//  arg_c: cantidad de argumentos del programa (0)
//  arg_v: arreglo con los strings de los argumentos
//---------------------------------------------------------------------------------
void inforeg(uint64_t arg_c, const char ** arg_v){
    if(arg_c!=0){
        throw_error("Error: El programa no recibe argumentos");
        sys_exit();
    }
    print_string("Registros: \n");
    // Pongo en reg los valores de los registros
    uint64_t aux[COUNT_REGS];
    sys_regs(aux);
    char reg_str[20];
    for(int i=0; i<COUNT_REGS; i++){
        print_string(Names[i]);
        to_hex(reg_str, aux[i]);
        print_string(reg_str);
        print_string("\n");
    }
    sys_exit();
}


//---------------------------------------------------------------------------------
// tiempo: imprime fecha y hora local en tiempo GMT-3
//---------------------------------------------------------------------------------
// Argumentos:
//  arg_c: cantidad de argumentos del programa (0)
//  arg_v: arreglo con los strings de los argumentos
//---------------------------------------------------------------------------------
// Imprime:
//      Fecha y hora local (GMT-3):
//          Miercoles 25/05/2022 15:35:20hs
//---------------------------------------------------------------------------------
void tiempo(uint64_t arg_c, const char ** arg_v) {
    if(arg_c!=0){
        throw_error("Error: El programa no recibe argumentos");
    }
    print_string("Fecha y hora local (GMT-3): \n");
    char * week[] = {" ", "Domingo", "Lunes", "Martes", "Miercoles", "Jueves", "Viernes", "Sabado"};
    int day_months[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    print_string("\t");
    int hs = time_arr[HOUR]();
    int day = time_arr[DAY_WEEK]();
    int day_month = time_arr[DAY_MONTH]();
    int month = time_arr[MONTH]();
    int year = time_arr[YEAR]();
    if(hs < 3){                                 // Caso particular: Cuando en Greenwich es el dia siguiente al de Argentina
        year = (day == 1 && month==1)? year-1: year;
        if(((year % 4 == 0)&&( year % 100 != 0)) || (year % 400== 0))
            day_months[2] = 29;
        month = (day == 1)? ((month == 1)? 12 : month-1) : month;
        day_month = (day_month == 1)? day_months[month] : day_month-1;
        day = (day == 1)? 7 : day-1;
        hs = 24 + hs;
    }
    print_string(week[day]);
    print_string(" ");
    print_number(day_month);
    print_string("/");
    print_number(month);
    print_string("/");
    print_number(year);
    print_string(" ");
    print_number(hs - 3);
    print_string(":");
    print_number(time_arr[MIN]());
    print_string(":");
    print_number(time_arr[SEC]());
    print_string("hs\n");
    sys_exit();
}





