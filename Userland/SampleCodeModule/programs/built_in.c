#include <libc.h>

//TODO: sacar
#include "../include/libc.h"
//---------------------------------------------------------------------------------
// help: imprime informacion sobre los programas disponibles
//---------------------------------------------------------------------------------
// Argumentos:
//  arg_c: cantidad de argumentos del programa (0)
//  arg_v: arreglo con los strings de los argumentos
//---------------------------------------------------------------------------------
void help(uint64_t arg_c, const char ** arg_v){
    extern front_program_t programs[CANT_PROG];
    if(arg_c!=0){
        throw_error("Error: el programa no recibe argumentos");
    }
    print_string("Programas disponibles:\n");
    char* aux;
    for(int i = 0; i<CANT_PROG;i++){
        aux = programs[i].desc;
        print_string(programs[i].desc);
    }
    sys_exit();
}

void mem(uint64_t arg_c, const char ** arg_v){
    //TODO:
}

void ps(uint64_t arg_c, const char ** arg_v){
    char* status[] = {"Execute","Ready","Blocked","Finished"};
    uint64_t processes = sys_get_process_count();//OJO si son muchos!!!!
    process_info_t* aux = sys_mm_alloc(processes*sizeof (process_info_t));
    processes = sys_get_scheduler_info(aux,processes);
    print_string_with_padding("PID",8);
    print_string_with_padding("Name",20);
    print_string_with_padding("Status",10);
    print_string_with_padding("Priority",8);
    print_string_with_padding("BP",10);
    print_string_with_padding("SP",10);
    print_string_with_padding("Foreground",12);
    print_string("\n");
    char buf[20];
    for(int i = 0; i<processes; i++){
        number_to_string(aux[i].pid,buf);
        print_string_with_padding(buf,8);
        print_string_with_padding(aux[i].name,20);
        print_string_with_padding(status[aux[i].status],10);
        number_to_string(aux[i].priority,buf);
        print_string_with_padding(buf,8);
        print_string_with_padding(to_hex(buf,aux[i].bp),10);
        print_string_with_padding(to_hex(buf,aux[i].sp),10);
        print_string_with_padding(aux[i].foreground?"Foreground":"Background",12);
        print_string("\n");
    }
    sys_mm_free(aux);
}
void loop(uint64_t arg_c, const char ** arg_v){
    //TODO
}
void kill(uint64_t arg_c, const char ** arg_v){
    //TODO
}
void nice_command(uint64_t arg_c, const char ** arg_v){
    //TODO
}
void block(uint64_t arg_c, const char ** arg_v){
    //TODO
}
void unblock(uint64_t arg_c, const char ** arg_v){
    //TODO
}
void sem(uint64_t arg_c, const char ** arg_v){
    //TODO
}
void pipe_info(uint64_t arg_c, const char ** arg_v){
    //TODO
}
void phylo(uint64_t arg_c, const char ** arg_v){
    //TODO
}