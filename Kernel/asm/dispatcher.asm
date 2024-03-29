; push_state_no_rax: Pushea el estado de los registros generales (GPR), salvo RAX, en el stack
%macro push_state_no_rax 0
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
%macro pop_state_no_rax 0
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
%macro push_state 0
    push rax
    push_state_no_rax
%endmacro

; pop_state: Popea el estado de los registros generales (GPR) del stack
%macro pop_state 0
    pop_state_no_rax
    pop rax
%endmacro

; ----------------------------------------------------------------------------------------------------

EXTERN allocate_new_process_stack
EXTERN scheduler
EXTERN exit_handler
GLOBAL context_switch_handler
GLOBAL idle_process
GLOBAL create_new_process_context

SECTION .text


;wrapper para los procesos para que no tengan que ejecutar exit (pueden hacer return)
process_wrapper:
    call rdx   ;el puntero del proceso se deja en rdx cuando se crea el stack del proceso
    call exit_handler

; ---------------------------------------------------------------------------------
; create_new_process_context
; ---------------------------------------------------------------------------------
; Argumentos:
;   RDI: arg_c del programa
;   RSI: arg_v del programa
;   RDX: Dirección de la primera función a ejecutar
;   RCX: Puntero a la variable que almacena el base pointer del nuevo proceso (PCB)
;   R8: Puntero a la variable que almacena el stack pointer del nuevo proceso (PCB)
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

    push rdi
    push rsi
    push rdx
    push rcx
    push r8
    call allocate_new_process_stack
    pop r8
    pop rcx
    pop rdx
    pop rsi
    pop rdi

    cmp rax, 0
    jnz create_stack                    ; Si la funcion devuelve NULL (0), se devuelve -1 (ERROR)
    mov rax, -1
    mov rsp, rbp
    pop rbp
    ret

create_stack:                           ; Sino, se crea el stack con la direccion obtenida (rax)

    mov rsp, rax
                                        ; Ver la diapositiva 69 del PDF Context Switching
    ;and rsp, -32                        ; ALIGN
    mov[rcx], rax
    and rsp, -16
    ;mov [r8], rsp                       ;Guarda el SP del nuevo proceso (no la base)
    push 0                              ; SS
    push rax                            ; RSP (Stack alineado para el proceso)
    push 0x202                          ; RFLAGS
    push 0x8                            ; CS
    push process_wrapper                            ; RIP
    push_state                          ; GPRs

    ;mov [rcx], rax                      ; Almaceno el valor del BP (rax)
    ;mov [rdx], rcx
    mov [r8],rsp ;dejo a sp en el tope del stack

    mov rsp, rbp
    pop rbp
    ret
