[BITS 16]

section .entry
global stage2_start
extern stage2_main

; --- constants ---
SMAP            equ 0x534D4150  ; SMAP - signature
BSIZE           equ 0x18        ; SMAP buffer size
CR              equ 0x0D        ; Carriage Return
NL              equ 0x0A        ; New Line

; --- System Control Ports ---
SCA_PORT            equ 0x92

; --- System Control Register Bits ---
CR0_PE              equ 0x01
SCA_A20             equ 0x02

stage2_start:
    jmp 0x0000:init             ; CS normalization (far jump)

; --- includes ---
%include "a20_check.inc"
%include "a20_kbc.inc"
%include "gdt.inc"
%include "memory_layout.inc"

init:
    ; init segments and stack
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7E00              ; stack grows downwards from ORG

    sti
    
    call check_a20
    test ax, ax
    jnz .a20_success

    ; -- activate A20 via BIOS int --
    mov ax, 0x2401
    int 0x15
    call check_a20
    test ax, ax
    jnz .a20_success

    ; -- fast A20 activation --
    ; setting bit 1 in port 0x92
    in al, SCA_PORT
    or al, SCA_A20
    out SCA_PORT, al
    call check_a20
    test ax, ax
    jnz .a20_success

    ; -- activate A20 via KBC --
    call kbc_a20_activate
    call check_a20
    test ax, ax
    jnz .a20_success

    ; on error
    mov si, MSG_ERROR_A20
    call print_string

    mov si, MSG_NEWLINE
    call print_string

    mov si, MSG_REBOOT
    call print_string

    xor ax, ax                  ; wait for key press
    int 16h
    jmp 0xFFFF:0000             ; jump (far jump) to reboot address


.a20_success:
    mov si, MSG_SUCCESS_A20
    call print_string

    mov si, MSG_NEWLINE
    call print_string

    mov si, MSG_GDT_PM
    call print_string

    mov si, MSG_NEWLINE
    call print_string

    cli

    ; -- load GDT --
    lgdt [gdtr]

    ; detect RAM
    call detect_memory

    ; --- activate Protected Mode ---
    mov eax, cr0
    or eax, CR0_PE
    mov cr0, eax

    ; far jump: load CODE_Desc (0x08) in CS
    jmp 0x08:init_pm32


[BITS 32]
init_pm32:
    ; load DATA_Desc (0x10)
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    ; setup stack
    mov esp, 0x7E00
    mov ebp, esp

    ; call C code
    call stage2_main

.halt:
    hlt
    jmp .halt


[BITS 16]

; --- data ---
MSG_SUCCESS_A20 db "Gate A20 active.", 0x00
MSG_GDT_PM      db "Load GDT and activate Protected Mode.", 0x00
MSG_ERROR_A20   db "Could not activate gate A20.", 0x00
MSG_ERROR_SRAM  db "Could not detect RAM size.", 0x00
MSG_REBOOT      db "Press key to reboot..", CR, NL, 0x00
MSG_NEWLINE     db  CR, NL, 0x00

; --- utility functions ---
print_string:
    mov ah, 0x0E
.loop:
    lodsb
    or al, al
    jz .done
    int 0x10
    jmp .loop
.done:
    ret

detect_memory:
    push bp

    mov ax, (MEM_MAP_ADDR>>0x04)
    mov es, ax
    mov di, 0x0004

    xor ebx, ebx
    mov edx, SMAP
    xor bp, bp

.next_entry:
    mov eax, 0xE820
    mov ecx, BSIZE
    int 0x15

    jc .done
    cmp eax, SMAP
    jne .error

    test ecx, ecx
    jz .skip_entry

    inc bp
    add di, BSIZE

.skip_entry:
    test ebx, ebx
    jz .done

    cmp di, 0xF000
    ja .done

    jmp .next_entry

.done:
    mov [es:0x0000], bp
    pop bp
    ret

.error:
    mov si, MSG_ERROR_SRAM
    call print_string
    pop bp
    hlt
