[BITS 64]

global outb_asm

outb_asm:
    push rbp
    mov rbp, rsp

    mov dx, di      ; Der Port (1. Parameter) liegt in RDI (untere 16 Bit = DI)
    mov al, sil     ; Der Wert (2. Parameter) liegt in RSI (unterstes Byte = SIL)
    out dx, al      ; Schreibt den Wert in AL an den Port in DX

    pop rbp
    ret
    