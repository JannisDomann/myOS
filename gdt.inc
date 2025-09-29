;==============================
; Global Descriptor Table (GDT)
;==============================

NULL_Desc:
    dd  0x00
    dd  0x00

CODE_Desc:
    dw  0xffff
    dw  0x00
    db  0x00
    db  10011010b
    db  11001111b
    db  0x00

DATA_Desc:
    dw  0xffff
    dw  0x00
    db  0x00
    db  10010010b
    db  11001111b
    db  0x00

gdtr:
Limit   dw  0x18
Base    dd  NULL_Desc