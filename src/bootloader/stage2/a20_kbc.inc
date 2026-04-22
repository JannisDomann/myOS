[BITS 16]

; Activates A20 gate via KBC (keyboard controller)

; --- KBC Ports ---
KBC_DATA_PORT       equ 0x60
KBC_COMMAND_PORT    equ 0x64

; --- KBC Commands ---
KBC_CMD_DEACTIVATE  equ 0xAD
KBC_CMD_ACTIVATE    equ 0xAE
KBC_CMD_READ        equ 0xD0
KBC_CMD_WRITE       equ 0xD1

; --- KBC Register Bits ---
KBC_STATUS_OBF      equ 0x01
KBC_STATUS_IBF      equ 0x02
KBC_DATA_OUT_A20    equ 0x02

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

    call kbc_wait_output

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
    or al, KBC_DATA_OUT_A20
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
    test al, KBC_STATUS_IBF
    jnz kbc_wait

kbc_wait_output:
    in al, KBC_COMMAND_PORT
    test al, KBC_STATUS_OBF
    jz kbc_wait_output          ; wait until data ready
    ret
