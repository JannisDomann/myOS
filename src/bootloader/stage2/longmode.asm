[BITS 32]

global load_paging_and_jump

; --- Register Bits ---
CR0_PG                  equ 1<<31
CR4_PAE                 equ 1<<5
EFER_LME                equ 1<<8

load_paging_and_jump:
    ; safe base pointer and kernel_entry
    push ebp
    mov ebp, esp

    ; Address of PML4
    mov eax, [ebp + 8]
    mov cr3, eax

    ; 1. CR4: set PAE
    mov eax, cr4
    or eax, CR4_PAE
    mov cr4, eax

    ; 2. EFER: set LME
    mov ecx, [ebp + 12]
    rdmsr
    or eax, EFER_LME
    wrmsr

    ; 3. CR0: Activate paging
    mov eax, cr0
    or eax, CR0_PG
    mov cr0, eax
    
    ; 4. jump (far jump) to 64 bit long mode 
    jmp 0x18:long_mode_64

[BITS 64]

long_mode_64:
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    ; call kernel_main @ [rbp + 16]
    mov rax, [rbp + 16]
    jmp rax

    hlt
