/* sampleCodeModule.c */
#include <libc.h>
#include <tests.h>
#include <programs.h>
#include <bash.h>
#include <libc.h>
#include <primos.h>
#include <os_tests.h>

//TODO: sacar
#include "./include/os_tests.h"
#include "./include/primos.h"
#include "./include/test_util.h"
#include "./include/libc.h"

void aa(){
    while(1){
        print_string("a",WHITE);
        pause_ticks(1);
    }
}
void bb(uint64_t pid){
    waitpid(pid);
    for(int i = 0; i<10; i++){
        print_number( getpid(),WHITE);
        pause_ticks(1);
    }
    return;
}
/*
 * Anecdota de un bug bastante feo
 * Cuando estuvimos testeando al scheduler con test_processes, decidimos
 * hacer que el proceso que lo llama imprima mientras tanto al estado de los procesos
 * con esta funcion. Haciendo esto, con un stack de 4KB para cada proceso, llegabamos
 * a una excepcion de GP cuando se imprimia el estado luego de llegar al proceso 70.
 * Las hipotesis para el probelma fueron varias:
 * 1- el espacio de stack no es suficiente
 * 2- el scheduler esta cambiando mal a los procesos
 * 3- falla el driver de video
 * La hipotesis 1 se descarto cuando se aumento tamto el tamaño del banco de memoria del mm como
 * el del stack de cada proceso (esto ultimo solo logro postergar el problema).
 * La hipotesis 2 y 3 fueron consideradas. El problema ocurria porque se cambiaban datos del PCB
 * (estructura cuya memoria era otorgada por el mm). Utilizando un watch de gdb, se determino
 * que los datos se cambiaban en el driver de video, particularmente en la funcion scroll_up.
 * Esto causo que nos centraramos en ese driver, pero lo que nos llamaba la atencion era que
 * el dato se cambiaba (aunque parezca increible) en la inicializacion de un ciclo for.
 * Despues de mucho tiempo, creemos haber encontrado el error:
 * Para cubirse del peor de los casos, cuando se usaba sys_get_scheduler_info, se creaba un
 * arreglo de process_info_t con la cantidad de elementos igual a la cantidad de procesos creados, contando
 * a los ya eliminados (una aproximacion mala pero segura)
 * Esto causaba que el stack del proceso main, que llamaba a esta funcion, se agrande mucho y se pasara de
 * los 4K disponibles. Sin embargo, el problema viene despues. Dado que el SP estaba muy arriba (en el stack)
 * por lo que su valor bajaba, cuando llegaba a los handlers de las syscalls (entre ellos, el de imprimir en pantalla)
 * todos los valores que se guardaban en el stack estaban pisando memoria que no era de el (entre esa memoria, se
 * encontraba el PCB para main, guardado en hash[1], ya que era reservado justo antes de guardar el stack del proceso main)
 * Esto hacia que justo cuando el driver de video intentaba imprimir, al hacer scroll_up e inicializar a las variables locales
 * que necesitaba para mover el contenido de la pantalla hacia arriba, este pise al PCB de ese proceso (por eso se rompia en
 * scroll_up).
 * Tomando un mejor caso para la longitud del arreglo, creemos (y esperamos) que no vuelva el problema
 */
void print_scheduler_info(){
    char* status[] = {"Execute","Ready","Blocked","Finished"};
    uint64_t processes = sys_get_process_count();//OJO si son muchos!!!!
    process_info_t aux[8];
    processes = sys_get_scheduler_info(aux,processes);
    print_string("\n",WHITE);//aca se rompe con new_pid = 71
    char buf[20];
    for(int i = 0; i<processes; i++){
        print_number(aux[i].pid,WHITE);
        print_string(" ",WHITE);
        print_string(aux[i].name,WHITE);
        print_string(" ",WHITE);
        print_string(status[aux[i].status],WHITE);
        print_string(" ",WHITE);
        print_number(aux[i].priority,WHITE);
        print_string(" ",WHITE);
        print_string(to_hex(buf,aux[i].bp),WHITE);
        print_string(" ",WHITE);
        print_string(to_hex(buf,aux[i].sp),WHITE);
        print_string(" ",WHITE);
        print_number(aux[i].foreground,WHITE);
        print_string("\n",WHITE);
    }
}
int main(uint64_t arg_c, char** arg_v) {
//    bash(arg_c,arg_v);
//    char* aux[] = {"4","1",NULL};
//    executable_t prio_test = {"test_sync",&test_processes,1,aux,0};
//    uint64_t pid = sys_exec(&prio_test);
//    char aux2[20];
//    snprintf(aux2,10,"%d",1);
//    print_string(aux2,WHITE);
//    while (1){
//        write_pipe(1,"a",2);
//        pause_ticks(1);
//    }
    executable_t pipes_test = {"test_pipes",&test_pipes,0,NULL,0, NULL};
    uint64_t pid = sys_exec(&pipes_test);

/*
    char* aux[] = {"100","1",NULL};
    executable_t prio_test = {"test_sync",&test_sync,2,aux,0};
    uint64_t pid = sys_exec(&prio_test);
//    while (1){
//        print_scheduler_info();
////        print_number(sys_get_process_count(),WHITE);
//        pause_ticks(1);
//    }
*/

    waitpid(pid);
    while (1){
        print_string("a",WHITE);
        pause_ticks(1);
    }
//    for(int i = 0; i<100; i++){
//        executable_t exec = {"test", &endless_loop_print,100000,NULL,0};
//        uint64_t  pid = sys_exec(&exec);
//        pause_ticks(4);
//        terminate_process(pid);
//        waitpid(pid);
//    }
//    print_string("Termino el test\n",WHITE);
//    print_number(arg_c,WHITE);
//    print_string(arg_v[0],WHITE);
//    executable_t exec;
//    exec.start = &aa;
//    exec.name = "proceso 1";
//    exec.arg_c = 0;
//    exec.arg_v = NULL;
//    uint64_t  pid = sys_exec(&exec);
//    executable_t exec2;
//    exec2.name = "proceso 2";
//    exec2.start = &bb;
//    exec2.arg_c = pid;
//    exec2.arg_v = NULL;
//    uint64_t pid2 = sys_exec(&exec2);
//    print_number(sys_get_process_count(),WHITE);
//    waitpid(pid);
//    while(1){
//        print_string("main ",WHITE);
//        pause_ticks(1);
//    }
//    while (1){
//        print_scheduler_info();
//        unblock_process(pid);
//        pause_ticks(1);
//    }

//    nice(pid2,0);
//    pause_ticks(3);
//    pause_ticks(3);
//    pause_ticks(3);
//    block_process(pid);
//    print_number(getpid(),WHITE);
//    pause_ticks(3);
//    pause_ticks(3);
//    pause_ticks(3);
//    pause_ticks(3);
//    pause_ticks(3);
//    pause_ticks(3);
//    unblock_process(pid);
//    waitpid(pid);
//    waitpid(pid2);
//    while (1){
//        print_number(getpid(),WHITE);
//        pause_ticks(1);
//    }

//    uint64_t a = 0;
//    while (1){
//        print_number(a++,WHITE);
//        print_string(" ",WHITE);
//    }
    return 0;

}