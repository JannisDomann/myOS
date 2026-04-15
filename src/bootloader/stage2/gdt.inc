;==============================
; Global Descriptor Table (GDT)
;==============================

section .data
align 4

NULL_Desc:
    dq  0x00

CODE_Desc:              ; 0x08
    dw  0xffff
    dw  0x0000
    db  0x00
    db  10011010b
    db  11001111b
    db  0x00

DATA_Desc:              ; 0x10
    dw  0xffff
    dw  0x0000
    db  0x00
    db  10010010b
    db  11001111b
    db  0x00

gdt_end:

global gdtr
gdtr:
    dw  gdt_end - NULL_Desc - 1
    dd  NULL_Desc
