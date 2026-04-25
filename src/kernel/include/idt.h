#pragma once
#include <stdint.h>

// IDT entry structure (16 bytes in 64-bit mode)
struct idt_entry {
    uint16_t offset_low;    // Offset bits 0..15
    uint16_t selector;      // Kernel code segment selector (0x18)
    uint8_t  ist;           // Interrupt Stack Table (usually 0)
    uint8_t  type_attr;     // Type and attributes (e.g., 0x8E for Interrupt Gate)
    uint16_t offset_mid;    // Offset bits 16..31
    uint32_t offset_high;   // Offset bits 32..63
    uint32_t reserved;      // Must be 0
} __attribute__((packed));

// IDT pointer for the 'lidt' instruction
struct idt_ptr {
    uint16_t limit;         // Size of IDT - 1
    uint64_t base;          // Base address of IDT
} __attribute__((packed));

// Function declarations
void idt_set_gate(uint8_t num, uint64_t base, uint16_t sel, uint8_t flags);
void idt_init();

// Assembly helper to load the IDTR
extern void load_idt(uint64_t idtp_addr);
extern void isr0(void); 
extern uint64_t isr_stub_table[];

// CPU interrupt registers
typedef struct {
    uint64_t r15, r14, r13, r12, r11, r10, r9, r8;
    uint64_t rax, rbx, rcx, rdx, rsi, rdi, rbp;
    uint64_t int_no, err_code;          // Pushed by our stub/CPU
    uint64_t rip, cs, rflags, rsp, ss;  // Pushed by CPU automatically
} interrupt_registers_t;

// Declaration of the C-Handler
void isr_handler(interrupt_registers_t* regs);
