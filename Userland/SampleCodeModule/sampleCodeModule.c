/* sampleCodeModule.c */
#include <libc.h>
#include <bash.h>
#include <libc.h>
#include <os_tests.h>
#include <test_util.h>


void aa(){
    while(1){
        print_string("a");
        pause_ticks(1);
    }
}
void bb(uint64_t pid){
    waitpid(pid);
    for(int i = 0; i<10; i++){
        print_number( getpid());
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
    print_string("\n");//aca se rompe con new_pid = 71
    char buf[20];
    for(int i = 0; i<processes; i++){
        print_number(aux[i].pid);
        print_string(" ");
        print_string(aux[i].name);
        print_string(" ");
        print_string(status[aux[i].status]);
        print_string(" ");
        print_number(aux[i].priority);
        print_string(" ");
        print_string(to_hex(buf,aux[i].bp));
        print_string(" ");
        print_string(to_hex(buf,aux[i].sp));
        print_string(" ");
        print_number(aux[i].foreground);
        print_string("\n");
    }
}
int main(uint64_t arg_c, char** arg_v) {
    bash(arg_c,arg_v);
}