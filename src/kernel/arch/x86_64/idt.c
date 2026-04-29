#include "idt.h"
#include "io.h"
#include "k_string.h"

// The actual table and pointer stored in RAM
struct idt_entry idt[256];
struct idt_ptr idtp;
interrupt_handler_t interrupt_handlers[256];

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
    // remap pic
    pic_remap();

    // Set IDT pointer
    idtp.limit = (sizeof(struct idt_entry) * 256) - 1;
    idtp.base  = (uint64_t)&idt;

    // Loop through the first 32 exceptions and 16 hardware interrupts
    for (uint8_t i = 0; i < 48; i++) {
        idt_set_gate(i, isr_stub_table[i], K_CODE_SEG_SEL, INT_GATE);
    }

    // Give the table address to the CPU
    load_idt((uint64_t)&idtp);

    k_printf("IDT initialized and loaded.\n");
}

void isr_handler(interrupt_registers_t* regs) {
    if (interrupt_handlers[regs->int_no] != 0x00) {
        interrupt_handlers[regs->int_no](regs);
    }
    else if (regs->int_no < 0X20) {
        // [EXCEPTION HANDLING]
        k_printf("\n" CLR_ERROR "[CPU EXCEPTION]" CLR_RESET " %s (Vector %d)", exception_messages[regs->int_no], regs->int_no);
        
        switch (regs->int_no) {
            case EXCEPTION_PF: {
                // Special case for Page Fault: read CR2
                k_printf(" | Faulting Address: %p", k_read_cr2());
                break;
            }
            default: {}
        }
                
        k_printf("\tRIP: 0x%x | Error Code: %d\n", regs->rip, regs->err_code);
        
        // Halt the system
        halt();
    }
    
    if (regs->int_no >= 0x20 && regs->int_no < 0x30) {
        // Hardware Interrupts (IRQs) always need an EOI
        // Send EOI to PIC
        if (regs->int_no >= 40) {
            k_outb(PIC2_COMMAND, PIC_EOI); // Slave
        }
        k_outb(PIC1_COMMAND, PIC_EOI);    // Master
    }
}

void pic_remap() {
    // Save current masks to restore them later or modify them
    uint8_t mask1 = k_inb(PIC1_DATA);
    uint8_t mask2 = k_inb(PIC2_DATA);

    // ICW1: Start initialization in cascade mode
    k_outb(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);
    k_io_wait();
    k_outb(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
    k_io_wait();

    // ICW2: Set Vector Offsets
    // Master PIC vectors will start at 32 (0x20)
    k_outb(PIC1_DATA, PIC1_OFFSET);
    k_io_wait();
    // Slave PIC vectors will start at 40 (0x28)
    k_outb(PIC2_DATA, PIC2_OFFSET);
    k_io_wait();

    // ICW3: Tell Master PIC there is a slave PIC at IRQ2 (0000 0100b)
    k_outb(PIC1_DATA, PIC_MASL_IRQ2);
    k_io_wait();
    // Tell Slave PIC its cascade identity (2)
    k_outb(PIC2_DATA, PIC_SL_CA_ID);
    k_io_wait();

    // ICW4: Set mode to 8086/88 (required for x86 systems)
    k_outb(PIC1_DATA, ICW4_8086);
    k_io_wait();
    k_outb(PIC2_DATA, ICW4_8086);
    k_io_wait();

    // Restore masks (or set new ones)
    // To enable only Keyboard (IRQ1), you would use: k_outb(PIC1_DATA, 0xFD);
    k_outb(PIC1_DATA, mask1);
    k_outb(PIC2_DATA, mask2);
}

void pic_unmask(uint8_t irq) {
    uint16_t port;
    uint8_t value;

    cli();

    if (irq < 8) {
        port = PIC1_DATA;
    } else {
        port = PIC2_DATA;
        irq -= 8;
        value = k_inb(PIC1_DATA) & ~PIC_MASL_IRQ2;
        k_outb(PIC1_DATA, value);
    }

    value = k_inb(port) & ~(1 << irq);
    k_outb(port, value);

    sti();
}

void register_interrupt_handler(uint8_t n, interrupt_handler_t handler) {
    cli();
    interrupt_handlers[n] = handler;
    sti();
}
