#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

extern void k_outb(uint16_t port, uint8_t val);
extern uint8_t k_inb(uint16_t port);
extern void io_wait();

#ifdef __cplusplus
}
#endif
