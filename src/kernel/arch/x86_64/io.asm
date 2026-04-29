[BITS 64]

; --- ports ---
PORT_SELFTEST   equ 0x80

global k_outb
k_outb:
    mov dx, di      ; Port (1st Param) is in RDI (lower 16 Bit = DI)
    mov al, sil     ; Value (2nd Param) is in RSI (lower Byte = SIL)
    out dx, al      ; Writes value in AL onto Port in DX
    ret

global k_inb
k_inb:
    xor rax, rax
    mov dx, di
    in al, dx
    ret

global k_insw
k_insw:
    mov rcx, rdx
    mov rdx, rdi
    mov rdi, rsi
    rep insw
    ret

global k_outsw
k_outsw:
    cld
    mov rcx, rdx
    mov rdx, rdi
    rep outsw
    ret

global k_io_wait
k_io_wait:
    mov al, 0x00
    out PORT_SELFTEST, al
    ret

global k_read_cr2
k_read_cr2:
    mov rax, cr2
    ret

global k_read_cr3
k_read_cr3:
    mov rax, cr3
    ret

global k_write_cr3
k_write_cr3:
    mov cr3, rdi
    ret

global k_invlpg
k_invlpg:
    invlpg [rdi]
    ret

global k_gdt_reload
k_gdt_reload:
    lgdt [rdi]            ; Load new GDT

    ; 1. Update data segments immediately
    mov ax, 0x10          ; Data Selector
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax            ; Update Stack Segment

    ; 2. Update Code Segment (CS) via retfq
    ; Stack for retfq: [RIP] [CS]
    push 0x08             ; New Code Selector
    lea rax, [rel .done]
    push rax
    retfq                 ; Pops RAX into RIP and 0x08 into CS

.done:
    ret