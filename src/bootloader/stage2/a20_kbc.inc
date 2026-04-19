[BITS 16]

; Activates A20 gate via KBC (keyboard controller)

; --- constants ---
KBC_DATA_PORT       equ 0x60
KBC_COMMAND_PORT    equ 0x64
KBC_CMD_DEACTIVATE  equ 0xAD
KBC_CMD_ACTIVATE    equ 0xAE
KBC_CMD_READ        equ 0xD0
KBC_CMD_WRITE       equ 0xD1

kbc_a20_activate:
    cli 

    ; 0xAD - deactivate kbc
    call kbc_wait
    mov al, KBC_CMD_DEACTIVATE
    out KBC_COMMAND_PORT, al

    ; 0xD0 - read status byte from output port
    call kbc_wait
    mov al, KBC_CMD_READ
    out KBC_COMMAND_PORT, al

    ; safe status
    call kbc_wait
    in al, KBC_DATA_PORT
    push ax

    ; 0xD1 - write status byte to output port
    mov al, KBC_CMD_WRITE
    out KBC_COMMAND_PORT, al

    ; set bit 1 (gate A20) in status 
    call kbc_wait
    pop ax
    or al, 00000010b
    out KBC_DATA_PORT, al

    ; 0xAE - re-activate kbc
    call kbc_wait
    mov al, KBC_CMD_ACTIVATE
    out KBC_COMMAND_PORT, al

    call kbc_wait
    sti
    ret

; --- utility functions ---
kbc_wait:
    in al, KBC_COMMAND_PORT
    test al, 00000010b
    jnz kbc_wait
