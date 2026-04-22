[BITS 64]

section .entry
global _start
extern kernel_main

_start:
    mov rsp, stack_top
    mov rbp, rsp

    call kernel_main

.halt:
    hlt
    jmp .halt

section .bss
align 16
stack_bottom:
    resb 16384  ; 16KB stack
stack_top:
    