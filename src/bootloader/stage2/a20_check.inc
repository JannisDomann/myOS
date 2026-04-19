[BITS 16]

; Checks if A20 gate is activated via boot signature mirroring
; return: ax=1 (avtivated), ax=0 (deactivated)

; --- constants ---
SIG_ADDR_LOW    equ 0x7DFE
SIG_ADDR_HIGH   equ 0x7E0E

check_a20:
    pushf
    push es
    push di
    push ds
    push si
    push bx

    cli

    ; low-segment 0x0000 -> signature at SIG_ADDR_LOW
    xor ax, ax
    mov es, ax
    mov di, SIG_ADDR_LOW

    ; high-segment 0xFFFF -> mirror at SIG_ADDR_HIGH
    ; SIG_ADDR_LOW  + 1MB       = (1)
    ; 0x7DFE        + 0x100000  = 0x107DFE
    ;
    ; (1)       - high-segment * 0x10   = SIG_ADDR_HIGH
    ; 0x107DFE  - 0xFFFF0               = 0x7E0E
    not ax
    mov ds, ax
    mov si, SIG_ADDR_HIGH

    ; load bx and invert value at high-segment
    mov bx, [ds:si]
    not bx
    mov [ds:si], bx

    ; check if inverted at low-segment
    cmp bx, [es:di]

    ; re-invert value at high-segment to original
    not bx
    mov [ds:si], bx

    je .is_disabled

    mov ax, 1
    jmp .done

.is_disabled:
    xor ax, ax

.done:
    pop bx
    pop si
    pop ds
    pop di
    pop es
    popf
    ret
