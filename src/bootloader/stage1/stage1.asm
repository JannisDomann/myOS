[BITS 16]

section .entry
global stage1_start

; --- constants ---
STAGE2_STARTOFFSET  equ 0x7E00  ; stage 2 start offset
STAGE2_STARTSECTOR  equ 0x02    ; sector 2 on drive
STAGE2_SECTORS      equ 0x10    ; sector 16 sectors to be loaded
CR                  equ 0x0D    ; Carriage Return
NL                  equ 0x0A    ; New Line


stage1_start:
    jmp 0x0000:init             ; CS normalization (far jump)

init:
    ; init segments and stack
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00              ; stack grows downwards from ORG

    ; safe boot drive
    mov [BOOT_DRIVE], dl

    ; booting message
    mov si, MSG_BOOTING
    call print_string

    ; load stage 2 via bios int 13h func 0x02 (read sectors)
    mov di, 0x03
.retry:
    mov ah, 0x02                ; call function 2
    mov al, STAGE2_SECTORS
    mov ch, 0x00                ; cylinder 0 
    mov dh, 0x00                ; head 0
    mov cl, STAGE2_STARTSECTOR
    mov dl, [BOOT_DRIVE]
    mov bx, STAGE2_STARTOFFSET
    clc
    int 0x13
    jnc .success

    xor ax, ax                  ; up to 3x disk reset on error
    int 0x13
    dec di
    jnz .retry

    jmp disk_error

.success:
    ; success and jump (far jump)
    mov si, MSG_LOADED
    call print_string
    
    jmp 0x0000:STAGE2_STARTOFFSET


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

disk_error:
    push ax
    mov si, MSG_ERROR
    call print_string

    ; print error code in hex
    ; print prefix "0x"
    mov ah, 0x0E
    mov al, '0'
    int 0x10
    mov al, 'x'
    int 0x10

    pop ax
    call print_hex

    mov si, MSG_NEWLINE
    call print_string

    mov si, MSG_REBOOT
    call print_string

    xor ax, ax                  ; wait for key press
    int 16h
    jmp 0xFFFF:0000             ; jump (far jump) to reboot address


print_hex:
    push cx
    mov cx, 0x02
    mov bl, ah                  ; store error code in bl
.hex_loop:
    rol bl, 0x04                ; roll nibble
    mov al, bl
    and al, 0x0F
    add al, '0'
    cmp al, '9'
    jbe .out
    add al, 0x07                ; nibble > 9 add 7 to 'A'
.out:
    mov ah, 0x0E
    int 0x10
    loop .hex_loop
    pop cx
    ret

; --- data ---
BOOT_DRIVE  db 0
MSG_BOOTING db "Booting...", CR, NL, 0x00
MSG_LOADED  db "Stage 2 ready", CR, NL, 0x00
MSG_ERROR   db "Disk error: ", 0x00
MSG_REBOOT  db "Press key to reboot..", CR, NL, 0x00
MSG_NEWLINE db  CR, NL, 0x00

; --- MBR structure ---
%if ($-$$) > 446
    %fatal "Code too big for mbr area, max 446 bytes!"
%endif

times 446-($-$$) db 0x00        ; padding to mbr partition table

; placeholder for 4 mbr partition entrys (64 bytes)
times 64 db 0x00

dw 0xAA55                       ; bootloader signature
