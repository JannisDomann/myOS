[BITS 32]

global outb_asm
outb_asm:
    push ebp
    mov ebp, esp
    push edx
    push eax

    mov edx, [ebp + 8]
    mov eax, [ebp + 12]
    out dx, al

    pop eax
    pop edx
    pop ebp
    ret

global outw_asm
outw_asm:
    push ebp
    mov ebp, esp
    push edx
    push eax

    mov edx, [ebp + 8]
    mov eax, [ebp + 12]
    out dx, ax

    pop eax
    pop edx
    pop ebp
    ret

global inb_asm
inb_asm:
    push ebp
    mov ebp, esp
    push edx

    mov edx, [ebp + 8]
    xor eax, eax
    in al, dx

    pop edx
    pop ebp
    ret

global inw_asm
inw_asm:
    push ebp
    mov ebp, esp
    push edx

    mov edx, [ebp + 8]
    xor eax, eax
    in ax, dx

    pop edx
    pop ebp
    ret
    
global halt
halt:
    hlt
    jmp halt
    