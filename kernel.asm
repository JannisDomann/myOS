;===================
; myOS Kernel v0.5.1
;===================
[BITS 16]
[global RealMode]
[extern main]

;==========
; Real Mode
;==========
RealMode:
    xor ax, ax
    mov es, ax
    mov ds, ax
    mov ss, ax
    mov sp, ax

    mov si, welcome
    call print_string

    add sp, -0x40

loop_start:
    mov si, prompt
    call print_string

    mov di, sp
    call get_string
    jcxz loop_start

    mov si, sp
    mov di, cmd_hi
    call strcmp
    je .helloworld

    mov si, sp
    mov di, cmd_help
    call strcmp
    je .help

    mov si, sp
    mov di, cmd_questionmark
    call strcmp
    je .help

    mov si, sp
    mov di, cmd_reset
    call strcmp
    je .reset

    mov si, sp
    mov di, cmd_pm
    call strcmp
    je .pm

    mov si, badcommand
    call print_string
    jmp loop_start

.helloworld:
    mov si, msg_helloworld
    call print_string
    jmp loop_start
.help:
    mov si, msg_help
    call print_string
    jmp loop_start
.reset:
    jmp 0xffff:0x0000
.pm:
    call clrscr
    mov si, msg_pm
    call print_string
    call Waitingloop
    cli
    lgdt [gdtr]
    in al, 0x92
    cmp al, 0xff
    je .no_fast_A20
    or al, 0x02
    and al, ~0x01
    out 0x92, al
    jmp .A20_done
.no_fast_A20:
    call empty_8042
    mov al, 0xd1
    out 0x64, al
    call empty_8042
    mov al, 0xdf
    out 0x60, al
    call empty_8042
.A20_done:
    mov eax, cr0
    or eax, 0x01
    mov cr0, eax
    jmp 0x8:ProtectedMode

;======
; calls
;======
empty_8042:
    call Waitingloop
    in al, 0x64
    cmp al, 0xff
    je .done
    test al, 0x01
    jz .no_output
    call Waitingloop
    in al, 0x60
    jmp empty_8042
.no_output:
    test al, 0x02
    jnz empty_8042
.done:
    ret

print_string:
    mov ah, 0x0e
.loop_start:
    lodsb
    test al, al
    jz .done
    int 0x10
    jmp .loop_start
.done:
    ret

get_string:
    xor cx, cx
.loop_start:
    xor ax, ax
    int 0x16
    cmp al, 0x08
    je .backspace
    cmp al, 0x0d
    je .done
    cmp cl, 0x1f
    je .loop_start
    mov ah, 0x0e
    int 0x10
    stosb
    inc cx
    jmp .loop_start

.backspace:
    jcxz .loop_start
    dec di
    mov byte [di], 0x00
    dec cx
    mov ah, 0x0e
    int 0x10
    mov al, ' '
    int 0x10
    mov al, 0x08
    int 0x10
    jmp .loop_start

.done:
    mov byte [di], 0x00
    mov ax, 0x0e0d
    int 0x10
    mov al, 0x0a
    int 0x10
    ret

strcmp:
.loop_start:
    mov al, [si]
    cmp al, [di]
    jne .done
    test al, al
    jz .done
    inc di
    inc si
    jmp .loop_start
.done:
    ret

clrscr:
    mov ax, 0x0600
    xor cx, cx
    mov dx, 0x174f
    mov bh, 0x07
    int 0x10
    ret

;===============
; Protected Mode
;===============
[BITS 32]

ProtectedMode:
    mov ax, 0x10
    mov es, ax
    mov ds, ax
    mov ss, ax
    xor ax, ax
    mov fs, ax
    mov gs, ax
    mov esp, 0x200000

    call clrscr_32
    mov ah, 0x01

.endless_loop:
    call Waitingloop
    inc ah
    and ah, 0x0f
    mov esi, msg_pm2
    call PutStr_32
    cmp dword [PutStr_Ptr], 0x19 * 0x50 * 0x02 + 0xb8000
    jb .endless_loop
    cmp dword [PutStr_Ptr], 0xb8000
    call main
    jmp $

Waitingloop:
    mov ebx, 0x9ffff
.loop_start:
    dec ebx
    jnz .loop_start
    ret

PutStr_32:
    mov edi, [PutStr_Ptr]
.next_char:
    lodsb
    test al, al
    jz .done
    stosw
    jmp .next_char
.done:
    mov [PutStr_Ptr], edi
    ret

clrscr_32:
    mov edi, 0xb8000
    mov [PutStr_Ptr], edi
    mov ecx, 0x28
    mov eax, 0x7200720
    rep stosd
    ret

PutStr_Ptr  dd  0xb8000

;========
; Strings
;========
welcome db 'myOS Kernel v0.5.1', 0x0d, 0x0a, 0x00
msg_helloworld db 'Hello World!', 0x0d, 0x0a, 0x00
msg_help db 'myOS commands: hi, help, ?, pm and reset', 0x0d, 0x0a, 0x00
msg_exit db 'ready for rebooting myOS..', 0x0d, 0x0a, 0x00
msg_pm db 'switching over to Protected Mode..', 0x0d, 0x0a, 0x00
msg_pm2 db 'myOS currently using Protected Mode.', 0x00
badcommand db 'Command unknown.', 0x0d, 0x0a, 0x00
prompt db '> ', 0
cmd_hi db 'hi', 0
cmd_help db 'help', 0
cmd_questionmark db '?', 0
cmd_reset db 'reset', 0
cmd_pm db 'pm', 0

;=========
; Includes
;=========
%include "gdt.inc"
