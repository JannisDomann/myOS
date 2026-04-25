[BITS 64]
extern isr_handler ; Our C function

; Macro for exceptions that do NOT push an error code
%macro ISR_NOERRCODE 1
global isr%1
isr%1:
    push qword 0      ; Push dummy error code
    push qword %1     ; Push interrupt number
    jmp isr_common_stub
%endmacro

; Macro for exceptions that DO push an error code
%macro ISR_ERRCODE 1
global isr%1
isr%1:
    push qword %1     ; Push interrupt number (error code is already there)
    jmp isr_common_stub
%endmacro

; Define the first 32 ISRs
ISR_NOERRCODE 0  ; Divide by Zero
ISR_NOERRCODE 1  ; Debug
ISR_NOERRCODE 2  ; Non Maskable Interrupt
ISR_NOERRCODE 3  ; Breakpoint
ISR_NOERRCODE 4  ; Into Detected Overflow
ISR_NOERRCODE 5  ; Out of Bounds
ISR_NOERRCODE 6  ; Invalid Opcode
ISR_NOERRCODE 7  ; No Coprocessor
ISR_ERRCODE   8  ; Double Fault (Has Error Code)
ISR_NOERRCODE 9  ; Coprocessor Segment Overrun
ISR_ERRCODE   10 ; Bad TSS (Has Error Code)
ISR_ERRCODE   11 ; Segment Not Present (Has Error Code)
ISR_ERRCODE   12 ; Stack Fault (Has Error Code)
ISR_ERRCODE   13 ; General Protection Fault (Has Error Code)
ISR_ERRCODE   14 ; Page Fault (Has Error Code)
ISR_NOERRCODE 15 ; Unknown Interrupt
ISR_NOERRCODE 16 ; Coprocessor Fault
ISR_ERRCODE   17 ; Alignment Check
ISR_NOERRCODE 18 ; Machine Check
ISR_NOERRCODE 19 ; SIMD Floating-Point Exception
ISR_NOERRCODE 20 ; Virtualization Exception
ISR_ERRCODE   21 ; Control Protection Exception
ISR_NOERRCODE 22 ; Reserved
ISR_NOERRCODE 23 ; Reserved
ISR_NOERRCODE 24 ; Reserved
ISR_NOERRCODE 25 ; Reserved
ISR_NOERRCODE 26 ; Reserved
ISR_NOERRCODE 27 ; Reserved
ISR_NOERRCODE 28 ; Reserved
ISR_NOERRCODE 29 ; Reserved
ISR_ERRCODE   30 ; VMM Communication Exception
ISR_ERRCODE   31 ; Security Exception

isr_common_stub:
    ; 1. Save CPU state (Registers)
    push rbp
    push rdi
    push rsi
    push rdx
    push rcx
    push rbx
    push rax
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15

    ; 2. Call our C handler
    mov rdi, rsp      ; Pass stack pointer as pointer to a struct
    call isr_handler

    ; 3. Restore CPU state
    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rax
    pop rbx
    pop rcx
    pop rdx
    pop rsi
    pop rdi
    pop rbp

    ; 4. Cleanup error code and ISR number
    add rsp, 16
    
    ; 5. Return from interrupt
    iretq

section .data
align 8
global isr_stub_table

isr_stub_table:
%assign i 0 
%rep    32 
    dq isr%+i     ; Creates a quad-word (64-bit) pointer for each isr
%assign i i+1 
%endrep
