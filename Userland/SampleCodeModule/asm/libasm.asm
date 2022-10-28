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

EXTERN print_string

section .text

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
    push rbp
    mov rbp, rsp            ; Armado de stack frame

;    mov rbx, rdi            ; La sys_call recibe por rbx el string a imprimir
;    mov rcx, rsi            ; La sys_call recibe por rcx el formato
    mov rax, 1
    int 80h

    mov rsp, rbp            ; Desarmado de stack frame
    pop rbp
    ret


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
    push rbp
    mov rbp, rsp

;    mov rbx, rdi            ; La sys_call recibe por rbx la posicion de memoria donde se guarda el caracter
    mov rax, 0
    int 80h

    mov rsp, rbp
    pop rbp
    ret


;-------------------------------------------------------------------------------------
; sys_exec: Ejecuta uno o dos nuevos procesos
;-------------------------------------------------------------------------------------
; Parametros:
;   rdi: la cantidad de nuevos procesos a ejecutar
;   rsi: vector con el/los nuevo/s proceso/s a ejecutar
;-------------------------------------------------------------------------------------
; Retorno: 0 si resulto exitoso, -1 si no (se agregaron mas de dos procesos)
;------------------------------------------------------------------------------------
sys_exec:
    push rbp
    mov rbp, rsp

;    mov rbx, rdi
;    mov rcx, rsi
    mov rax, 2
    int 80h

    mov rsp, rbp
    pop rbp
    ret

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
    push rbp
    mov rbp, rsp

    mov rax, 3
    int 80h

    mov rsp, rbp
    pop rbp
    ret

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
    push rbp
    mov rbp, rsp

;    mov rbx, rdi
    mov rax, 4
    int 80h

    mov rsp, rbp
    pop rbp
    ret

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
    push rbp
    mov rbp, rsp

;    mov rcx, rsi
;    mov rbx, rdi
    mov rax, 5
    int 80h

    mov rsp, rbp
    pop rbp
    ret

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
    push rbp
    mov rbp, rsp

    mov rax, 6
    int 80h

    mov rsp, rbp
    pop rbp
    ret

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
    push rbp
    mov rbp, rsp

    mov rax, 7
    int 80h

    mov rsp, rbp
    pop rbp
    ret

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
    push rbp
    mov rbp, rsp

    mov rax, 9
    int 80h

    mov rsp, rbp
    pop rbp
    ret

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
    push rbp
    mov rbp, rsp

;    mov rbx, rdi
    mov rax, 8
    int 80h

    mov rsp, rbp
    pop rbp
    ret

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

