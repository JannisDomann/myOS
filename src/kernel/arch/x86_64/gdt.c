#include "gdt.h"
#include "io.h"
#include "k_string.h"

// These will live in your Kernel Data Segment
static gdt_entry gdt[3] __attribute__((aligned(16)));
static gdt_ptr gp;

static void gdt_set_gate(int num, uint64_t base, uint32_t limit, uint8_t access, uint8_t gran) {
    gdt[num].base_low    = (base & 0xFFFF);
    gdt[num].base_middle = (base >> 16) & 0xFF;
    gdt[num].base_high   = (base >> 24) & 0xFF;

    gdt[num].limit_low   = (limit & 0xFFFF);
    gdt[num].granularity = (limit >> 16) & 0x0F;
    gdt[num].granularity |= gran & 0xF0;
    gdt[num].access      = access;
}

void gdt_init() {
    gp.limit = (sizeof(gdt_entry) * 0x03) - 0x01;
    gp.base  = (uint64_t)&gdt;

    gdt_set_gate(0x0, 0x0, 0x0, 0x0, 0x0);      // Null segment
    gdt_set_gate(0x1, 0x0, 0x00, 0x9A, 0x20);   // Code segment (64-bit)
    gdt_set_gate(0x2, 0x0, 0x00, 0x92, 0xCF);   // Data segment

    k_gdt_reload(&gp);

    k_printf("GDT reloaded in kernel mode.\n");
}
