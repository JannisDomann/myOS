;==============================
; Global Descriptor Table (GDT)
;==============================

section .data
align 8

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

CODE64_Desc:            ; 0x18
    dw  0x0000          ; Limit ignored
    dw  0x0000          ; Base ignored
    db  0x00            ; Base ignored
    db  10011010b       ; P=1, DPL=0, Type=Code/Execute/Read
    db  00100000b       ; L=1 (Long Mode), D=0 (must be 0 if L=1)
    db  0x00            ; Base ignored

gdt_end:

global gdtr
gdtr:
    dw  gdt_end - NULL_Desc - 1
    dd  NULL_Desc
