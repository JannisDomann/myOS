[BITS 64]

section .entry
global _start
extern kernel_main

KERNEL_MAGIC    equ 0xBEBAFECA

_start:
    dd KERNEL_MAGIC 
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
    