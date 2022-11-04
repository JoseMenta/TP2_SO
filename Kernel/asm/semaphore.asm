GLOBAL _xchg

; Dado dos parametros (arg1 en RDI y arg2 en RSI), asigna en [arg1] el valor de arg2 y devuelve el valor anterior de arg1
_xchg:
  mov rax, rsi
  xchg [rdi], eax
  ret