global get_keyboard_scan_code
section .text
;----------------------------------------------------------------------
; get_keyboard_scan_code: obtiene el codigo de la tecla presionada del teclado
;----------------------------------------------------------------------
; Argumentos:
;  void
;----------------------------------------------------------------------
; La funcion busca en el mapa de entrada y salida, en la poscicion donde el teclado
; deja al codigo de la tecla presionada, y lo devuelve en rax
;----------------------------------------------------------------------
; Retorno
;  El codigo de la tecla presionada en el teclado
;----------------------------------------------------------------------
get_keyboard_scan_code:
    push rbp
	mov rbp, rsp
;   ya se sabe que se puede leer, pues el teclado mando la interrupcion, no es necesario revisar si el ultimo bit en el puerto 64 es 1
; 	mov rax, 0
; 	in al, 64h
; 	and rax, 1 ;tengo que ver solo el ultimo bit
; 	cmp rax, 1
	mov rax, 0
	in al, 60h
	mov rsp, rbp
	pop rbp
	ret
