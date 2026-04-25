#include "idt.h"
#include "../include/k_string.h"

// The actual table and pointer stored in RAM
struct idt_entry idt[256];
struct idt_ptr idtp;

// Human readable messages for the first 32 exceptions
const char* exception_messages[] = {
    "Division By Zero",
    "Debug",
    "Non Maskable Interrupt",
    "Breakpoint",
    "Into Detected Overflow",
    "Out of Bounds",
    "Invalid Opcode",
    "No Coprocessor",
    "Double Fault",
    "Coprocessor Segment Overrun",
    "Bad TSS",
    "Segment Not Present",
    "Stack Fault",
    "General Protection Fault",
    "Page Fault",
    "Unknown Interrupt",
    "Coprocessor Fault",
    "Alignment Check",
    "Machine Check",
    "SIMD Floating-Point",
    "Virtualization",
    "Control Protection",
    "Reserved", "Reserved", "Reserved", "Reserved", "Reserved", "Reserved",
    "Reserved", "Reserved", "Reserved", "Security"
};

/**
 * Sets an entry in the IDT
 * @param num Vector number (0..255)
 * @param base Address of the handler function
 * @param sel Kernel code selector (0x18)
 * @param flags Access flags
 */
void idt_set_gate(uint8_t num, uint64_t base, uint16_t sel, uint8_t flags) {
    idt[num].offset_low  = (uint16_t)(base & 0xFFFF);
    idt[num].offset_mid  = (uint16_t)((base >> 16) & 0xFFFF);
    idt[num].offset_high = (uint32_t)((base >> 32) & 0xFFFFFFFF);
    
    idt[num].selector    = sel;
    idt[num].ist         = 0;
    idt[num].type_attr   = flags;
    idt[num].reserved    = 0;
}

void idt_init() {
    // Set IDT pointer
    idtp.limit = (sizeof(struct idt_entry) * 256) - 1;
    idtp.base  = (uint64_t)&idt;

    // Loop through the first 32 exceptions
    for (uint8_t i = 0; i < 32; i++) {
        idt_set_gate(i, isr_stub_table[i], 0x18, 0x8E);
    }

    // Give the table address to the CPU
    load_idt((uint64_t)&idtp);

    k_printf("IDT initialized and loaded.\n");
}

void isr_handler(interrupt_registers_t* regs) {
    if (regs->int_no < 32) {
        
        k_printf("\n" CLR_ERROR "[CPU EXCEPTION]" CLR_RESET " %s (Vector %d)", 
                exception_messages[regs->int_no], regs->int_no);
        k_printf("\tRIP: 0x%x | Error Code: %d\n", regs->rip, regs->err_code);
        
        // Halt the system
        while(1) {
            __asm__("hlt");
        }
    }
}
