
#include "../include/libc.h"
#include "../include/test_pipes.h"

void test_pipes(){

    error_test();
    first_write_name();
    first_read_name();
    first_write_common();
    first_read_common();
    stdin_test();
    full_pipes();

}

//-----------------------------------------------------------------------------------------
//full_pipes:
//      Uso 11 fd => el proximo da error
//      10 es la cantidad de FD que tiene el PCB
//      lo dejo como Magic Number porque no deberias saberlo desde user
//-----------------------------------------------------------------------------------------
void full_pipes(){
    int fd[2];
    for (int i = 3; i < 11; i += 2) {
        pipe(fd);
    }
    pipe(fd);
    if (fd[0] == -1 && fd[1] == -1){
        write_pipe(1, "OK\n", 3);
    }
    return;
}

//-----------------------------------------------------------------------------------------
//error_test:
//      probar la impresion diferente de errores
//-----------------------------------------------------------------------------------------
void error_test(){
    write_pipe(2, "Error\n", 10);
    return;
}


//-----------------------------------------------------------------------------------------
//first_write_name:
//      creo un pipe con nombre y escribo
//      conecto otro pipe y leo lo que escribí
//-----------------------------------------------------------------------------------------
void first_write_name(){
    executable_t write = {"write",&write_pipe_name,0,NULL,0, NULL};
    uint64_t pidWrite = sys_exec(&write);
    executable_t read = {"read",&read_pipe_name,0,NULL,0, NULL};
    uint64_t pidRead = sys_exec(&read);
    waitpid(pidWrite);
    waitpid(pidRead);
    return;
}

void write_pipe_name(){
    int fd = open_fifo(O_RDWR, "PorFavor");
    write_pipe(fd, "con Nombre\n", 20);
    close_fd(fd);
    return;
}

void read_pipe_name(){
    int fd = link_pipe_named(O_RDWR, "PorFavor");
    char buf[20];
    read_pipe(fd, buf, 20);
    write_pipe(1, buf, 20);
    close_fd(fd);
    return;
}

//-----------------------------------------------------------------------------------------
//first_read_name:
//      creo un pipe con nombre, trato de leer, se bloquea
//      conecto a otro pipe y escribo, desbloquea el  anterior y lee
//-----------------------------------------------------------------------------------------
void first_read_name(){
    executable_t read = {"read2",&read_pipe_name2,0,NULL,0, NULL};
    uint64_t pidRead = sys_exec(&read);
    executable_t write = {"write2",&write_pipe_name2,0,NULL,0, NULL};
    uint64_t pidWrite = sys_exec(&write);
    waitpid(pidRead);
    waitpid(pidWrite);
    return;
}

void write_pipe_name2(){
    int fd = link_pipe_named(O_RDWR, "PorFavor2");
    write_pipe(fd, "con Nombre\n", 20);
    write_pipe(fd, "con Nombre2\n", 20);
    close_fd(fd);
    return;
}

void read_pipe_name2(){
    int fd = open_fifo(O_RDWR, "PorFavor2");
    char buf[30];
    read_pipe(fd, buf, 30);
    write_pipe(1, buf, 30);
    close_fd(fd);
    return;
}

//-----------------------------------------------------------------------------------------
//stdin_test:
//      leer de stdin e imprimir en stdout
//-----------------------------------------------------------------------------------------
void stdin_test(){
    char buff[20];
    char aux[2];
    write_pipe(1, "Ingresar texto: ", 20);
    for(int i=0; aux[0]!='\n'; i++){
        read_pipe(0, aux, 1);
        buff[i]=aux[0];
    }
    write_pipe(1, buff, 10);

    aux[0]='\0';

    write_pipe(1, "\nIngresar texto: ", 20);
    for(int i=0; aux[0]!='\n'; i++){
        read_pipe(0, aux, 1);
        buff[i]=aux[0];
    }
    write_pipe(1, buff, 10);
    return;
}

//-----------------------------------------------------------------------------------------
//first_write_common:
//      Creo un pipe, cambio los fd a los procesos "hijos"
//      imprimo en uno y leeo del otro
//-----------------------------------------------------------------------------------------
void first_write_common(){
    int fd[2];
    pipe(fd);
    int fdwrite[3] = {0, fd[1], 2};
    int fdread[3] = {fd[0], 1, 2};
    executable_t write = {"write",&write_pipe_common,0,NULL,0, fdwrite};
    uint64_t pidWrite = sys_exec(&write);
    executable_t read = {"read",&read_pipe_common,0,NULL,0, fdread};
    uint64_t pidRead = sys_exec(&read);
    waitpid(pidWrite);
    waitpid(pidRead);
    return;
}


//-----------------------------------------------------------------------------------------
//first_write_common:
//      Creo un pipe, intento leer y bloquea
//      Escribo en el pipe, desbloquea y lee
//-----------------------------------------------------------------------------------------
void first_read_common(){
    int fd[2];
    pipe(fd);
    int fdwrite[3] = {0, fd[1], 2};
    int fdread[3] = {fd[0], 1, 2};
    executable_t read = {"read",&read_pipe_common,0,NULL,0, fdread};
    uint64_t pidRead = sys_exec(&read);
    executable_t write = {"write",&write_pipe_common,0,NULL,0, fdwrite};
    uint64_t pidWrite = sys_exec(&write);
    waitpid(pidWrite);
    waitpid(pidRead);
    return;
}


void write_pipe_common(){
    write_pipe(1, "Comun\n", 20);
    return;
}

void read_pipe_common(){
    char buf[20];
    read_pipe(0, buf, 20);
    write_pipe(1, buf, 20);
    return;
}
