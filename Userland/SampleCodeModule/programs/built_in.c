// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <libc.h>
#include <test_util.h>
//TODO: sacar
#include "../include/libc.h"
#include "../include/test_util.h"
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
    for(int i = 0; i<CANT_PROG;i++){
        print_string(programs[i].desc);
    }
    sys_exit();
}

void mem(uint64_t arg_c, const char ** arg_v){
    mm_info_t aux;
    sys_mm_info(&aux);
    print_string_with_padding("Algoritmo:", 40);
    print_string(aux.algorithm);
    print_string("\n");
    print_string_with_padding("Espacio total:",40);
    print_number(aux.total_bytes);
    print_string("\n");
    print_string_with_padding("Bytes ocupados:",40);
    print_number(aux.allocated_bytes);
    print_string("\n");print_string_with_padding("Bloques ocupados:",40);
    print_number(aux.allocated_blocks);
    print_string("\n");print_string_with_padding("Bytes libres:",40);
    print_number(aux.free_bytes);
    print_string("\n");
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
        print_string_with_padding(aux[i].foreground?"True":"False",12);
        print_string("\n");
    }
    sys_mm_free(aux);
}

// TODO: IMPLEMENTAR ESC PARA PODER TERMINAR ESTE PROGRAMA
void loop(uint64_t arg_c, const char ** arg_v){
    if(arg_c != 1){
        throw_error("Error: el programa debe recibir la cantidad de segundos que debe esperar antes de repetir el loop");
        return;
    }
    uint32_t seconds = string_to_number(arg_v[0], NULL);
    while(1){
        print_string_with_padding("Hola, soy el proceso con pid: ", 30);
        print_number(getpid());
        print_string("\n");
        sleep(seconds);
    }
}
void kill(uint64_t arg_c, const char ** arg_v){
    if(arg_c!=1){
        throw_error("Error: el programa debe recibir el pid del proceso que se desea matar");
        return;
    }
    uint64_t pid;
    if((pid = satoi(arg_v[0])) <= 0){
        throw_error("Error: el argumento ingresado para el pid no es valido");
        return;
    }
    terminate_process(pid);
}
void nice_command(uint64_t arg_c, const char ** arg_v){
    if(arg_c!=2){
        throw_error("Error: el programa debe recibir el pid del proceso cuya prioridad se desea cambiar y su nueva prioridad entre 0 y 4");
        return;
    }
    uint64_t pid;
    int16_t prio;
    if((pid = satoi(arg_v[0])) <= 0){
        throw_error("Error: el argumento ingresado para el pid no es valido");
        return;
    }
    if((prio = satoi(arg_v[1])) < 0 || prio>=5){
        throw_error("Error: el argumento ingresado para la prioridad no es valido");
        return;
    }
    if(sys_nice(pid,prio)==-1){
        print_string("Ocurrio un error al intentar cambiar la prioridad del proceso.\n Asegurese de que el proceso no haya terminado\n");
    }
}
void block(uint64_t arg_c, const char ** arg_v){
    if(arg_c!=1){
        throw_error("Error: el programa debe recibir el pid del proceso que se desea bloquear");
        return;
    }
    uint64_t pid;
    if((pid = satoi(arg_v[0])) <= 0){
        throw_error("Error: el argumento ingresado para el pid no es valido");
        return;
    }
    if(block_process(pid)==-1){
        print_string("Ocurrio un error al intentar bloquear el proceso.\n Asegurese de que el proceso no este bloqueado o terminado\n");
    }
}
void unblock(uint64_t arg_c, const char ** arg_v){
    if(arg_c!=1){
        throw_error("Error: el programa debe recibir el pid del proceso que se desea desbloquear");
        return;
    }
    uint64_t pid;
    if((pid = satoi(arg_v[0])) <= 0){
        throw_error("Error: el argumento ingresado para el pid no es valido");
        return;
    }
    if(unblock_process(pid)==-1){
        print_string("Ocurrio un error al intentar desbloquear el proceso.\n Asegurese de que el proceso este bloqueado\n");
    }
}
void wait_pid_command(uint64_t arg_c, const char ** arg_v){
    if(arg_c!=1){
        throw_error("Error: el programa debe recibir el pid del proceso que se desea esperar");
        return;
    }
    uint64_t pid;
    if((pid = satoi(arg_v[0])) <= 0){
        throw_error("Error: el argumento ingresado para el pid no es valido");
        return;
    }
    waitpid(pid);
}

void sem(uint64_t arg_c, const char ** arg_v){
    sem_dump_t* aux_sem_dump;
    int32_t count = sys_sem_count();
    aux_sem_dump = sys_mm_alloc(count*sizeof (sem_dump_t));
    count = sem_info(aux_sem_dump,count);
    for(int i = 0; i<count; i++){
        print_string("-------------------------------------------------------------------------------\n");
        print_string("Semaforo: ");
        print_string(aux_sem_dump[i].name!=NULL?aux_sem_dump[i].name:"sin nombre");
        print_string(", Valor: ");
        print_number(aux_sem_dump[i].value);
        print_string("\n");
        print_string("Procesos bloqueados: ");
        print_numbers(aux_sem_dump[i].blocked_processes,aux_sem_dump[i].blocked_size);
        print_string("\n");
        print_string("Procesos conectados: ");
        print_numbers(aux_sem_dump[i].connected_processes,aux_sem_dump[i].connected_size);
        print_string("\n");
    }
    free_sem_info(aux_sem_dump,count);
    free(aux_sem_dump);
}

void pipe_info(uint64_t arg_c, const char ** arg_v){
    if(arg_c != 1){
        throw_error("Error: el programa debe recibir un argumento");
    }
    uint64_t count;
    string_to_number(arg_v[0], &count);

    //pipe_user_info aux[count];
    pipe_user_info * aux = sys_mm_alloc(count * sizeof(pipe_user_info));
    if(aux == NULL){
        throw_error("Error: En la reserva de memoria dinamica");
    }

    int size = get_info(aux, count);
    if(size==0){
        print_string("No se crearon pipes aun");
    }
    for(int i=0; i<size; i++){
        print_string("-------------------------------------------------------------------------------\n");
        print_string("NOMBRE: ");
        if(aux[i].name == NULL){
            print_string("Sin Nombre");
        }else{
            print_string(aux[i].name);
        }
        print_string("\nWrite Index: ");
        print_number(aux[i].index_write);
        print_string("\nRead Index: ");
        print_number(aux[i].index_read);
        print_string("\nProcesos bloqueados en escritura: ");
        for(int j=0; i<MAXLOCK && aux[i].pid_write_lock[j] != 0; j++){
            print_number(aux[i].pid_write_lock[j]);
        }
        print_string("\nProcesos bloqueados en lectura: ");
        for(int j=0; i<MAXLOCK && aux[i].pid_read_lock[j] != 0; j++){
            print_number(aux[i].pid_read_lock[j]);
        }
        print_string("\n");
    }
    sys_mm_free(aux);
    return;
}