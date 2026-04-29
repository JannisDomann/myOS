#pragma once

#include "gdt.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

extern volatile void k_outb(uint16_t port, uint8_t value);
extern volatile uint8_t k_inb(uint16_t port);
extern volatile void k_insw(uint16_t port, void* addr, uint64_t count);
extern volatile void k_outsw(uint16_t port, void* addr, uint64_t count);
extern volatile void k_io_wait();
extern volatile uint64_t k_read_cr2();
extern volatile uint64_t k_read_cr3();
extern volatile void k_write_cr3(uint64_t value);
extern volatile void k_invlpg(uint64_t addr);
extern volatile void k_gdt_reload(gdt_ptr* gp);

#ifdef __cplusplus
}
#endif
