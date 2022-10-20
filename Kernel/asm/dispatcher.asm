; push_state_no_rax: Pushea el estado de los registros generales (GPR), salvo RAX, en el stack
%macro push_state_no_rax
    push rbx
    push rcx
    push rdx
    push rbp
    push rsp
    push rsi
    push rdi
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15
%endmacro

; pop_state_no_rax: Popea el estado de los registros generales (GPR), salvo RAX, del stack
%macro pop_state_no_rax
    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rdi
    pop rsi
    pop rsp
    pop rbp
    pop rdx
    pop rcx
    pop rbx
%endmacro

; push_state: Pushea el estado de los registros generales (GPR) en el stack
%macro push_state
    push rax
    push_state_no_rax
%endmacro

; pop_state: Popea el estado de los registros generales (GPR) del stack
%macro pop_state
    pop_state_no_rax
    pop rax
%endmacro

; ----------------------------------------------------------------------------------------------------

EXTERN allocate_new_process_stack
EXTERN scheduler

GLOBAL context_switch_handler
GLOBAL create_new_process_context

SECTION .text

; ---------------------------------------------------------------------------------
; context_switch_handler
; ---------------------------------------------------------------------------------
; Se encarga de realizar el cambio de contexto entre dos procesos: El que se está ejecutando se detiene,
; y alguno de los procesos que está en la cola de listos comienza su ejecucion
; ---------------------------------------------------------------------------------
context_switch_handler:

    push_state

    mov rdi, rsp
    call scheduler                           ; scheduler es la funcion encargada de recibir el SP del proceso ejecutandose y otorgar el SP del proceso a ejecutar
    mov rsp, rax

    mov al, 20h                              ; Aviso al PIC que termino la interrupcion
    out 20h, al

    pop_state

    iretq

; ---------------------------------------------------------------------------------
; create_new_process_context
; ---------------------------------------------------------------------------------
; Argumentos:
;   RDI: Dirección de la primera función a ejecutar
;   RSI: Puntero a la variable que almacena el base pointer del nuevo proceso (PCB)
;   RDX: Puntero a la variable que almacena el stack pointer del nuevo proceso (PCB)
; ---------------------------------------------------------------------------------
; Se encarga de crear el contexto de un nuevo proceso, solicitando espacio al Memory Manager
; ---------------------------------------------------------------------------------
; Retorno:
;   ERROR: Devuelve -1
;   EXITO: Devuelve la dirección del registro RSP
; ---------------------------------------------------------------------------------
create_new_process_context:
    push rbp
    mov rbp, rsp

    call allocate_new_process_stack
    cmp rax, 0
    jnz create_stack                    ; Si la funcion devuelve NULL (0), se devuelve -1 (ERROR)
    mov rax, -1
    mov rsp, rbp
    pop rbp
    ret

create_stack:                           ; Sino, se crea el stack con la direccion obtenida (rax)

    mov rsp, rax
                                        ; Ver la diapositiva 69 del PDF Context Switching
    and rsp, -32                        ; ALIGN
    mov rcx, rsp
    push 0                              ; SS
    push rcx                            ; RSP (Stack alineado para el proceso)
    push 0x202                          ; RFLAGS
    push 0x8                            ; CS
    push rdi                            ; RIP
    push_state                          ; GPRs

    mov [rsi], rax                      ; Almaceno el valor del BP (rax) y del SP (rcx) del nuevo proceso
    mov [rdx], rcx                      ; en los punteros otorgados

    mov rsp, rbp
    pop rbp
    ret
