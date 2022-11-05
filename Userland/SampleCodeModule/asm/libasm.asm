GLOBAL sys_write
GLOBAL sys_read
GLOBAL sys_exec
GLOBAL sys_exit
GLOBAL sys_time
GLOBAL sys_mem
GLOBAL sys_tick
GLOBAL sys_blink
GLOBAL sys_regs
GLOBAL zero_division_exc
GLOBAL invalid_opcode_exc
GLOBAL get_registers
GLOBAL get_register
GLOBAL sys_clear
GLOBAL sys_block
GLOBAL sys_unblock
GLOBAL sys_waitpid
GLOBAL sys_yield
GLOBAL sys_terminate
GLOBAL sys_getpid
GLOBAL sys_nice
GLOBAL sys_get_scheduler_info
GLOBAL sys_get_process_count
GLOBAL sys_mm_alloc
GLOBAL sys_mm_free
GLOBAL sys_pipe
GLOBAL sys_open_fifo
GLOBAL sys_open_fifo
GLOBAL sys_link_pipe_named
GLOBAL sys_close_fd
GLOBAL sys_write_pipe
GLOBAL sys_read_pipe
GLOBAL sys_get_info
GLOBAL sys_sem_init
GLOBAL sys_sem_open
GLOBAL sys_sem_wait
GLOBAL sys_sem_post
GLOBAL sys_sem_close
GLOBAL sys_sem_info
GLOBAL sys_free_sem_info
GLOBAL sys_sem_count
GLOBAL sys_dup2
GLOBAL sys_dup
GLOBAL sys_pause_ticks
GLOBAL sys_mm_info

EXTERN print_string

section .text

%macro syscall_macro 1
    push rbp
    mov rbp, rsp

    mov rax, %1
    int 80h

    mov rsp, rbp
    pop rbp
    ret
%endmacro
;-------------------------------------------------------------------------------------
; sys_write: Llama a la sys_call de escritura
;-------------------------------------------------------------------------------------
; Parametros:
;   rdi: el string a imprimir
;   rsi: el formato con el que se imprime el texto
;-------------------------------------------------------------------------------------
; Retorno:
;   rax: cantidad de caracteres escritos
;------------------------------------------------------------------------------------
sys_write:
    syscall_macro 1
;    push rbp
;    mov rbp, rsp            ; Armado de stack frame
;
;;    mov rbx, rdi            ; La sys_call recibe por rbx el string a imprimir
;;    mov rcx, rsi            ; La sys_call recibe por rcx el formato
;    mov rax, 1
;    int 80h
;
;    mov rsp, rbp            ; Desarmado de stack frame
;    pop rbp
;    ret


;-------------------------------------------------------------------------------------
; sys_read: Llama a la sys_call de lectura
;-------------------------------------------------------------------------------------
; Parametros:
;   rdi: El puntero al string sobre el cual copiar el caracter leido del teclado
;-------------------------------------------------------------------------------------
; Retorno:
;   rax: cantidad de caracteres leidos
;------------------------------------------------------------------------------------
sys_read:
    syscall_macro 0
;    push rbp
;    mov rbp, rsp
;
;;    mov rbx, rdi            ; La sys_call recibe por rbx la posicion de memoria donde se guarda el caracter
;    mov rax, 0
;    int 80h
;
;    mov rsp, rbp
;    pop rbp
;    ret


;-------------------------------------------------------------------------------------
; sys_exec: Ejecuta un programa
;-------------------------------------------------------------------------------------
; Parametros:
;   rdi: estructura executable_t con la informacion del programa que se desea ejecutar
;-------------------------------------------------------------------------------------
; Retorno: el pid del nuevo proceso si no hubo error, -1 si no
;------------------------------------------------------------------------------------
sys_exec:
    syscall_macro 2
;    push rbp
;    mov rbp, rsp
;
;;    mov rbx, rdi
;;    mov rcx, rsi
;    mov rax, 2
;    int 80h
;
;    mov rsp, rbp
;    pop rbp
;    ret

;-------------------------------------------------------------------------------------
; sys_exit: Finaliza el proceso ejecutandose en el momento
;-------------------------------------------------------------------------------------
; Parametros:
;   null
;-------------------------------------------------------------------------------------
; Retorno:
;   null
;------------------------------------------------------------------------------------
sys_exit:
    syscall_macro 3
;    push rbp
;    mov rbp, rsp
;
;    mov rax, 3
;    int 80h
;
;    mov rsp, rbp
;    pop rbp
;    ret

;-------------------------------------------------------------------------------------
; sys_time: Devuelve el valor para la unidad de tiempo pasado
;-------------------------------------------------------------------------------------
; Parametros:
;   rdi: el tipo de unidad (0 = secs, 2 = mins, 4 = hour, 6 = day of week, 7 = day, 8 = month, 9 = year)
;-------------------------------------------------------------------------------------
; Retorno:
;   El valor para esa unidad de tiempo
;------------------------------------------------------------------------------------
sys_time:
    syscall_macro 4
;    push rbp
;    mov rbp, rsp
;
;;    mov rbx, rdi
;    mov rax, 4
;    int 80h
;
;    mov rsp, rbp
;    pop rbp
;    ret

;-------------------------------------------------------------------------------------
; sys_mem:  Dado un arreglo de 32 bytes y una direccion de memoria, completa el arreglo con la informacion
;           guardada en dicha direccion y las 31 siguientes
;-------------------------------------------------------------------------------------
; Parametros:
;   rdi: direccion de memoria inicial
;   rsi: direccion de memoria del arreglo
;-------------------------------------------------------------------------------------
; Retorno:
;   Cantidad de datos que realmente se almacenaron (los restantes son NULL)
;------------------------------------------------------------------------------------
sys_mem:
    syscall_macro 5
;    push rbp
;    mov rbp, rsp
;
;;    mov rcx, rsi
;;    mov rbx, rdi
;    mov rax, 5
;    int 80h
;
;    mov rsp, rbp
;    pop rbp
;    ret

;-------------------------------------------------------------------------------------
; sys_tick: Devuelve los ticks transcurridos desde que inicio la computadora
;-------------------------------------------------------------------------------------
; Parametros:
;   void
;-------------------------------------------------------------------------------------
; Retorno:
;   Cantidad de ticks
;------------------------------------------------------------------------------------
sys_tick:
    syscall_macro 6
;    push rbp
;    mov rbp, rsp
;
;    mov rax, 6
;    int 80h
;
;    mov rsp, rbp
;    pop rbp
;    ret

;-------------------------------------------------------------------------------------
; sys_blink: Realiza un parpadeo en la pantalla
;-------------------------------------------------------------------------------------
; Parametros:
;   void
;-------------------------------------------------------------------------------------
; Retorno:
;   0 si resulto exitoso, 1 si no
;------------------------------------------------------------------------------------
sys_blink:
    syscall_macro 7
;    push rbp
;    mov rbp, rsp
;
;    mov rax, 7
;    int 80h
;
;    mov rsp, rbp
;    pop rbp
;    ret

;-------------------------------------------------------------------------------------
; sys_clear: Limpia la terminal
;-------------------------------------------------------------------------------------
; Parametros:
;   void
;-------------------------------------------------------------------------------------
; Retorno:
;   void
;------------------------------------------------------------------------------------
sys_clear:
    syscall_macro 9
;    push rbp
;    mov rbp, rsp
;
;    mov rax, 9
;    int 80h
;
;    mov rsp, rbp
;    pop rbp
;    ret

;-------------------------------------------------------------------------------------
; sys_regs: Devuelve el estado de los registros de la ultima vez que se realizo CTRL+S
;-------------------------------------------------------------------------------------
; Parametros:
;   rbx: Puntero a un arreglo de 18 enteros de 64 bits
;-------------------------------------------------------------------------------------
; Retorno:
;   0 si nunca se hizo CTRL+S, 1 si se hizo al menos una vez
;------------------------------------------------------------------------------------
sys_regs:
    syscall_macro 8
;    push rbp
;    mov rbp, rsp
;
;;    mov rbx, rdi
;    mov rax, 8
;    int 80h
;
;    mov rsp, rbp
;    pop rbp
;    ret

;-------------------------------------------------------------------------------------
; sys_block: Bloquea al proceso con el pid indicado (debe usarse solo para testeos)
;-------------------------------------------------------------------------------------
; Parametros:
;   rdi: pid del proceso que se desea bloquear
;-------------------------------------------------------------------------------------
; Retorno:
;   -1 si hubo error, 0 si no
;------------------------------------------------------------------------------------
; Bloquear a un proceso no garantiza que se vaya a desbloquear con la syscall de
; desbloqueo, puede desbloquearse por otra razon.
;------------------------------------------------------------------------------------
sys_block:
    syscall_macro 10
;    push rbp
;    mov rbp, rsp
;
;    mov rax, 10
;    int 80h
;
;    mov rsp, rbp
;    pop rbp
;    ret

;-------------------------------------------------------------------------------------
; sys_unblock: Desbloquea al proceso con el pid indicado (debe usarse solo para testeos)
;-------------------------------------------------------------------------------------
; Parametros:
;   rdi: pid del proceso que se desea desbloquear
;-------------------------------------------------------------------------------------
; Retorno:
;   -1 si hubo error, 0 si no
;------------------------------------------------------------------------------------
; Desbloquar a un proceso es peligroso, ya que puede romper con la logica como
; los bloqueos para leer de un pipe o esperar para entrar a una zona critica
; Por lo tanto, debe usarse solo para testeo
;------------------------------------------------------------------------------------
sys_unblock:
    syscall_macro 13
;    push rbp
;    mov rbp, rsp
;
;    mov rax, 13
;    int 80h
;
;    mov rsp, rbp
;    pop rbp
;    ret

;-------------------------------------------------------------------------------------
; sys_waitpid: Espera a la terminacion de un proceso, posiblemente bloqueando al que
;               realiza la llamada si el proceso indicado con el pid no finalizo
;-------------------------------------------------------------------------------------
; Parametros:
;   rdi: pid del proceso que se desea esperar
;-------------------------------------------------------------------------------------
; Retorno:
;   -1 si hubo error, 3 si no (FINISHED)
;------------------------------------------------------------------------------------
sys_waitpid:
    syscall_macro 11
;    push rbp
;    mov rbp, rsp
;
;    mov rax, 11
;    int 80h
;
;    mov rsp, rbp
;    pop rbp
;    ret

;-------------------------------------------------------------------------------------
; sys_yield: Libera al procesador
;-------------------------------------------------------------------------------------
; Parametros:
;
;-------------------------------------------------------------------------------------
; Retorno:
;   -1 si hubo error, 0 si no
;------------------------------------------------------------------------------------
sys_yield:
    syscall_macro 12
;    push rbp
;    mov rbp, rsp
;
;    mov rax, 12
;    int 80h
;
;    mov rsp, rbp
;    pop rbp
;    ret

;-------------------------------------------------------------------------------------
; sys_terminate: Termina al proceso con el pid indicado
;-------------------------------------------------------------------------------------
; Parametros:
;   rdi: pid del proceso que se desea terminar
;-------------------------------------------------------------------------------------
; Retorno:
;   -1 si hubo error, 0 si no
;------------------------------------------------------------------------------------
sys_terminate:
    syscall_macro 14
;    push rbp
;    mov rbp, rsp
;
;    mov rax, 14
;    int 80h
;
;    mov rsp, rbp
;    pop rbp
;    ret

;-------------------------------------------------------------------------------------
; sys_getpid: Devuelve el pid del proceso actual
;-------------------------------------------------------------------------------------
; Parametros:
;
;-------------------------------------------------------------------------------------
; Retorno:
;   -1 si hubo error, el pid si no hubo error (uint64_t)
;------------------------------------------------------------------------------------
sys_getpid:
    syscall_macro 16
;    push rbp
;    mov rbp, rsp
;
;    mov rax, 16
;    int 80h
;
;    mov rsp, rbp
;    pop rbp
;    ret
;-------------------------------------------------------------------------------------
; sys_nice: Cambia la prioridad del proceso con el pid indicado
;-------------------------------------------------------------------------------------
; Parametros:
;   rdi: pid del proceso cuya prioridad se desea cambiar
;   rsi: prioridad que se le desea asignar (entre 0, la mayor, y 4)
;-------------------------------------------------------------------------------------
; Retorno:
;   -1 si hubo error, el pid si no hubo error (uint64_t)
;------------------------------------------------------------------------------------
sys_nice:
    syscall_macro 15
;    push rbp
;    mov rbp, rsp
;
;    mov rax, 15
;    int 80h
;
;    mov rsp, rbp
;    pop rbp
;    ret

;-------------------------------------------------------------------------------------
; sys_get_scheduler_info: Obtinene la informacion del scheduler
;-------------------------------------------------------------------------------------
; Parametros:
;   rdi: vector de process_info_t donde se debe almacenar la informacion (reservado por el usuario)
;   rsi: cantidad maxima de procesos cuya informacion se puede almacenar
;-------------------------------------------------------------------------------------
; Retorno:
;   cantidad de procesos almacenados en el vector
;------------------------------------------------------------------------------------
sys_get_scheduler_info:
    syscall_macro 17
;    push rbp
;    mov rbp, rsp
;
;    mov rax, 17
;    int 80h
;
;    mov rsp, rbp
;    pop rbp
;    ret

;-------------------------------------------------------------------------------------
; sys_get_process_count: Obtiene la cantidad maxima de procesos cuya informacion
;                       se logra obtener con sys_get_scheduler_info
;-------------------------------------------------------------------------------------
; Parametros:
;-------------------------------------------------------------------------------------
; Retorno:
;   Cantidad maxima de procesos que maneja el scheduler (todos los creados - los que
;    fueron esperados por sus padres (es decir, los que dejaron de ser zombies)
;------------------------------------------------------------------------------------
sys_get_process_count:
    syscall_macro 18
;    push rbp
;    mov rbp, rsp
;
;    mov rax, 18
;    int 80h
;
;    mov rsp, rbp
;    pop rbp
;    ret


;-------------------------------------------------------------------------------------
; sys_mm_alloc: Reserva memoria en el heap
;-------------------------------------------------------------------------------------
; Parametros:
;   rdi: cantidad de memoria (en bytes) que se desea reservar
;-------------------------------------------------------------------------------------
; Retorno:
;   NULL si hubo error, el inicio de la memoria reservada si no
;------------------------------------------------------------------------------------
sys_mm_alloc:
    syscall_macro 19
;    push rbp
;    mov rbp, rsp
;
;    mov rax, 19
;    int 80h
;
;    mov rsp, rbp
;    pop rbp
;    ret

;-------------------------------------------------------------------------------------
; sys_mm_free: Libera memoria en el heap
;-------------------------------------------------------------------------------------
; Parametros:
;   rdi: el puntero al inicio de la memoria reservada
;-------------------------------------------------------------------------------------
; Retorno:
;
;------------------------------------------------------------------------------------
sys_mm_free:
    syscall_macro 20
;    push rbp
;    mov rbp, rsp
;
;    mov rax, 20
;    int 80h
;
;    mov rsp, rbp
;    pop rbp
;    ret

;-------------------------------------------------------------------------------------
; sys_pipe: Crear un pipe
;-------------------------------------------------------------------------------------
; Parametros:
;   rdi: puntero a vector de 2 enteros
;-------------------------------------------------------------------------------------
; Retorno:
;   -1 en caso de error, 0 en caso de exito
;------------------------------------------------------------------------------------
sys_pipe:
    syscall_macro 21

;-------------------------------------------------------------------------------------
; sys_open_fifo: Crear un pipe con identificador/fifo
;-------------------------------------------------------------------------------------
; Parametros:
;   rdi: modo del fifo
;   rsi: char * del identificador
;-------------------------------------------------------------------------------------
; Retorno:
;   -1 en caso de error, 0 en caso de exito
;rdi, rsi, rdx
;------------------------------------------------------------------------------------
sys_open_fifo:
    syscall_macro 22

;-------------------------------------------------------------------------------------
; sys_link_pipe_named: conectarse a un fifo/pipe con identificador
;-------------------------------------------------------------------------------------
; Parametros:
;   rdi: modo del fifo
;   rsi: char * del identificador
;-------------------------------------------------------------------------------------
; Retorno:
;   -1 en caso de error, 0 en caso de exito
;------------------------------------------------------------------------------------
sys_link_pipe_named:
    syscall_macro 23

;-------------------------------------------------------------------------------------
; sys_close_fd: cerrar el fd
;-------------------------------------------------------------------------------------
; Parametros:
;   rdi: modo del fifo
;-------------------------------------------------------------------------------------
; Retorno:
;   -1 en caso de error, 0 en caso de exito
;------------------------------------------------------------------------------------
sys_close_fd:
    syscall_macro 24

;-------------------------------------------------------------------------------------
; sys_write: escribir en un fd
;-------------------------------------------------------------------------------------
; Parametros:
;   rdi: fd a escribir
;   rsi: buffer a escribir
;   rdx: cantidad de caracteres a escribir
;-------------------------------------------------------------------------------------
; Retorno:
;   cantidad de caracteres escritos
;------------------------------------------------------------------------------------
sys_write_pipe:
    syscall_macro 25

;-------------------------------------------------------------------------------------
; sys_read: leer de un fd
;-------------------------------------------------------------------------------------
; Parametros:
;   rdi: fd a escribir
;   rsi: buffer donde leer
;   rdx: cantidad de caracteres a leer
;-------------------------------------------------------------------------------------
; Retorno:
;   cantidad de caracteres leidos
;------------------------------------------------------------------------------------
sys_read_pipe:
    syscall_macro 26

;-------------------------------------------------------------------------------------
; sys_get_info: obtener la informacion de los pipes del sistema
;-------------------------------------------------------------------------------------
; Parametros:
;   rdi: puntero a vector de estructuras donde recibir la informacion
;   rsi: cantidad de pipes leidos
;-------------------------------------------------------------------------------------
; Retorno:
;------------------------------------------------------------------------------------
sys_get_info:
    syscall_macro 27

;----------------------------------------------------------------------
; sem_init: Crea un nuevo semaforo
;----------------------------------------------------------------------
; Argumentos:
;  rdi: nombre del semaforo (named semaphores)
;  rsi: valor inicial del semaforo
;----------------------------------------------------------------------
; Retorno:
;  Devuelve la estructura del semaforo creado
;  Devuelve NULL si hubo algun error
;----------------------------------------------------------------------
sys_sem_init:
    syscall_macro 28

;-------------------------------------------------------------------------------------
; sys_sem_open: Se conecta a un semaforo ya creado
;-------------------------------------------------------------------------------------
; Parametros:
;   rdi: nombre del semaforo (named semaphores)
;   rsi: valor inicial del semaforo
;   rdx: indica que accion realizar si no se encontro el semaforo (ver libc.h)
;-------------------------------------------------------------------------------------
; Retorno:
;   Devuelve la estructura del semaforo si existe
;   Si mode = O_CREATE, devuelve un nuevo semaforo
;   Si mode = O_NULL, devuelve NULL
;   Devuelve NULL si ocurrio algun error
;------------------------------------------------------------------------------------
sys_sem_open:
    syscall_macro 29

;-------------------------------------------------------------------------------------
; sys_sem_wait: Realiza la operacion wait en el semaforo
;-------------------------------------------------------------------------------------
; Parametros:
;   rdi: estructura del semaforo
;-------------------------------------------------------------------------------------
; Retorno:
;   Devuelve un error_code (ver libc.h)
;------------------------------------------------------------------------------------
sys_sem_wait:
    syscall_macro 30

;-------------------------------------------------------------------------------------
; sys_sem_post: Realiza la operacion post en el semaforo
;-------------------------------------------------------------------------------------
; Parametros:
;   rdi: estructura del semaforo
;-------------------------------------------------------------------------------------
; Retorno:
;   Devuelve un error_code (ver libc.h)
;------------------------------------------------------------------------------------
sys_sem_post:
    syscall_macro 31

;-------------------------------------------------------------------------------------
; sys_sem_close: Se desconecta del semaforo
;-------------------------------------------------------------------------------------
; Parametros:
;   rdi: estructura del semaforo
;-------------------------------------------------------------------------------------
; Retorno:
;   Devuelve un error_code (ver libc.h)
;------------------------------------------------------------------------------------
sys_sem_close:
    syscall_macro 32

;-------------------------------------------------------------------------------------
; sys_sem_dump: Devuelve la informacion de los semaforos activos
;-------------------------------------------------------------------------------------
; Parametros:
;   rdi: arreglo de estructuras sem_dump_t
;   rsi: cantidad de elementos del arreglo
;-------------------------------------------------------------------------------------
; OJO: Luego se debe llamar a sys_sems_dump_free para liberar todos los recursos
;-------------------------------------------------------------------------------------
; Retorno:
;   Devuelve la cantidad de semaforos (elementos completados del arreglo)
;------------------------------------------------------------------------------------
sys_sem_info:
    syscall_macro 33

;-------------------------------------------------------------------------------------
; sys_sem_dump_free: Libera los recursos creados en sys_sems_dump
;-------------------------------------------------------------------------------------
; Parametros:
;   rdi: arreglo de estructuras sem_dump_t
;   rsi: cantidad de elementos a liberar del arreglo
;-------------------------------------------------------------------------------------
; Retorno:
;------------------------------------------------------------------------------------
sys_free_sem_info:
    syscall_macro 34

;-------------------------------------------------------------------------------------
; sys_dup2: Duplicar la referencia de un fd en otro
;-------------------------------------------------------------------------------------
; Parametros:
;   rdi: fd que se quiere duplicar
;   rsi: nuevo fd a referenciar
;-------------------------------------------------------------------------------------
; Retorno:
;   el nuevo fd a referenciar
;------------------------------------------------------------------------------------
sys_dup2:
    syscall_macro 35

;-------------------------------------------------------------------------------------
; sys_dup: Duplicar la referencia de un fd en el proximo fd libre
;-------------------------------------------------------------------------------------
; Parametros:
;   rdi: fd que se quiere duplicar
;-------------------------------------------------------------------------------------
; Retorno:
;   el nuevo fd donde se compio la referencia
;------------------------------------------------------------------------------------
sys_dup:
    syscall_macro 36

sys_pause_ticks:
    syscall_macro 37

sys_mm_info:
    syscall_macro 38

sys_sem_count:
    syscall_macro 39

;-------------------------------------------------------------------------------------
; zero_division_exc: Programa para generar un excepcion de division por cero
;-------------------------------------------------------------------------------------
; Parametros:
;   void
;-------------------------------------------------------------------------------------
zero_division_exc:
	push rbp
	mov rbp, rsp

    mov rcx, 0
    mov rax, 1
	 div rcx                 ; Hacemos la division 1 / 0

;    int 0h

	mov rsp, rbp
	pop rbp
	ret

;-------------------------------------------------------------------------------------
; invalid_opcode_exc: Programa para generar un excepcion de operador invalido
;-------------------------------------------------------------------------------------
; Parametros:
;   void
;-------------------------------------------------------------------------------------
invalid_opcode_exc:
	push rbp
	mov rbp, rsp

;    mov rcx, 0
;    mov rax, 1
;	div rcx
    mov cr6, rax
;	ud2                     ; Genera un invalid opcode

	mov rsp, rbp
	pop rbp
	ret



;-------------------------------------------------------------------------------------
; get_registers: Obtener el valor de los registros
;-------------------------------------------------------------------------------------
; Parametros:
;-------------------------------------------------------------------------------------
; Retorno:
;   el vector con el valor de los registros
;------------------------------------------------------------------------------------
get_registers:
    push rbp
    mov rbp, rsp

    pushfq                              ; Pushea los flags
    pop qword [reg + 136]                ; Los guarda en la estructura auxiliar
    mov [reg], r8
	mov [reg+8], r9
	mov [reg+16], r10
	mov [reg+24], r11
	mov [reg+32], r12
	mov [reg+40], r13
	mov [reg+48], r14
	mov [reg+56], r15
	mov [reg+64], rax
	mov [reg+72], rbx
	mov [reg+80], rcx
	mov [reg+88], rdx
	mov [reg+96], rsi
	mov [reg+104], rdi
	mov [reg+112], rbp
    mov [reg+120], rbp                      ; Deberia ir rsp pero por stack frame dejo este

    push rax
    call get_rip                            ; Logica para poder obtener el registro RIP
                                            ; TODO: Colocar el get_rip arriba de todo para tener el rip mas cerca al pedido

get_rip_return:
    mov qword [reg+128], rax                ; Guardo en el arreglo, el valor de RIP

    pop rax

    mov rax, reg

    mov rbp, rsp
    pop rbp
    ret

get_rip:
    pop rax
    jmp get_rip_return


SECTION .bss
	reg resb 144		    ; Guarda 8*18 lugares de memoria (para los 18 registros)

;