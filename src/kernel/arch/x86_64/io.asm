[BITS 64]

global k_outb, k_inb, k_insw, k_io_wait

; --- ports ---
PORT_SELFTEST   equ 0x80

k_outb:
    mov dx, di      ; Port (1st Param) is in RDI (lower 16 Bit = DI)
    mov al, sil     ; Value (2nd Param) is in RSI (lower Byte = SIL)
    out dx, al      ; Writes value in AL onto Port in DX
    ret

k_inb:
    xor rax, rax
    mov dx, di
    in al, dx
    ret

k_insw:
    mov rcx, rdx
    mov rdx, rdi
    mov rdi, rsi
    rep insw
    ret

k_io_wait:
    mov al, 0x00
    out PORT_SELFTEST, al
    ret
