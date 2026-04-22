[BITS 32]

global outb_asm, outw_asm, inb_asm, inw_asm

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