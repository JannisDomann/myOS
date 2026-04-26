[BITS 64]
extern isr_handler  ; Our C function

global load_idt
load_idt:
    lidt [rdi]      ; RDI holds the pointer to idtp (System V ABI)
    ret

global halt
halt:
    hlt
    jmp halt

global cli
cli:
    cli
    ret

global sti
sti:
    sti
    ret

; Macro for exceptions that do NOT push an error code
%macro ISR_NOERRCODE 1
global isr%1
isr%1:
    push qword 0    ; Push dummy error code
    push qword %1   ; Push interrupt number
    jmp isr_common_stub
%endmacro

; Macro for exceptions that DO push an error code
%macro ISR_ERRCODE 1
global isr%1
isr%1:
    push qword %1     ; Push interrupt number (error code is already there)
    jmp isr_common_stub
%endmacro

; Macro for hardware interrupts (IRQs)
%macro IRQ 2
global irq%1
irq%1:
    push qword 0    ; Dummy error code
    push qword %2   ; Vector number (32 + IRQ number)
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

; Hardware Interrupts (IRQs) mapped to 32-47
ISR_NOERRCODE 32 ; IRQ 0 - Timer
ISR_NOERRCODE 33 ; IRQ 1 - Keyboard
ISR_NOERRCODE 34 ; IRQ 2 - Cascade
ISR_NOERRCODE 35 ; IRQ 3 - COM2
ISR_NOERRCODE 36 ; IRQ 4 - COM1
ISR_NOERRCODE 37 ; IRQ 5 - LPT2
ISR_NOERRCODE 38 ; IRQ 6 - Floppy
ISR_NOERRCODE 39 ; IRQ 7 - LPT1
ISR_NOERRCODE 40 ; IRQ 8 - CMOS Real Time Clock
ISR_NOERRCODE 41 ; IRQ 9 - Free
ISR_NOERRCODE 42 ; IRQ 10 - Free
ISR_NOERRCODE 43 ; IRQ 11 - Free
ISR_NOERRCODE 44 ; IRQ 12 - PS2 Mouse
ISR_NOERRCODE 45 ; IRQ 13 - FPU
ISR_NOERRCODE 46 ; IRQ 14 - Primary ATA (Disk)
ISR_NOERRCODE 47 ; IRQ 15 - Secondary ATA

; Define IRQs (mapped to 32-47 via pic_remap)
IRQ 0, 32    ; Timer
IRQ 1, 33    ; Keyboard
IRQ 2, 34    ; Cascade
; ... add more as needed

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
    mov rdi, rsp    ; Pass stack pointer as pointer to a struct
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
%rep    48 
    dq isr%+i     ; Creates a quad-word (64-bit) pointer for each isr
%assign i i+1 
%endrep
