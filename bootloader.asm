; setup start address
org 0x7c00

;=========================
; setup stack and seg regs
;=========================
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, ax

;=============================
; read kernel from floppy disk
;=============================
    mov [bootdrive], dl

load_kernel:
    xor ax, ax
    int 0x13
    jc load_kernel

    mov bx, 0x8000

    mov dl, [bootdrive]
    mov ax, 0x020a
    mov cx, 0x0002
    mov dh, 0x00
    int 0x13
    jc load_kernel

    mov si, load_msg
    call print_string

;===============
; jump to kernel
;===============
    jmp bx

;====================
; call "print_string"
;====================
print_string:
    mov ah, 0x0e
.loop:
    lodsb
    test al, al
    jz .done
    int 0x10
    jmp .loop
.done:
    ret
    
;=====
; data
;=====
    bootdrive db 0x00
    load_msg db "bootloader: loading kernel..", 0x0d, 0x0a, 0x00

    times 0x1fe-($-$$) hlt
    db 0x55
    db 0xaa