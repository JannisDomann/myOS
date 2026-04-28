#pragma once

#include <stdint.h>

// PICs
#define PIC1_COMMAND    0x20
#define PIC1_DATA       0x21
#define PIC2_COMMAND    0xA0
#define PIC2_DATA       0xA1

#define PIC_MASL_IRQ2   0x04    // Slave Pic at Masters IRQ2 (Bitwise)
#define PIC_SL_CA_ID    0x02    // Slaves ID at Master

#define ICW1_INIT       0x10    // Start initialization sequence
#define ICW1_ICW4       0x01    // ICW4 will be present
#define ICW4_8086       0x01    // 8086/88 (MCS-80/85) mode

#define PIC1_OFFSET     0x20    // Vector offset for Master (32-39)
#define PIC2_OFFSET     0x28    // Vector offset for Slave (40-47)

#define PIC_EOI         0x20    // End of Interrupt command

#define INT_GATE        0x8E    // Define type of interrupt gate
#define K_CODE_SEG_SEL  0x18    // Kernel Code Segment Selector


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

// CPU interrupt registers
typedef struct {
    uint64_t r15, r14, r13, r12, r11, r10, r9, r8;
    uint64_t rax, rbx, rcx, rdx, rsi, rdi, rbp;
    uint64_t int_no, err_code;          // Pushed by our stub/CPU
    uint64_t rip, cs, rflags, rsp, ss;  // Pushed by CPU automatically
} interrupt_registers_t;

// typedef interrupt handler callback function
typedef void (*interrupt_handler_t)(interrupt_registers_t*);

// stub table created in asm
extern volatile uint64_t isr_stub_table[];

// Assembly helper to load the IDTR
void idt_set_gate(uint8_t num, uint64_t base, uint16_t sel, uint8_t flags);
void idt_init();
extern volatile void load_idt(uint64_t idtp_addr);

// Common function declarations
extern volatile void halt();
extern volatile void cli();
extern volatile void sti();

// Declaration of the C-Handler
void isr_handler(interrupt_registers_t* regs);

// remap pic for processing hardware interrupts
void pic_remap();
void pic_unmask(uint8_t irq) ;

// rergister int handler
void register_interrupt_handler(uint8_t n, interrupt_handler_t handler);
