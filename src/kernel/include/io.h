#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

extern volatile inline void k_outb(uint16_t port, uint8_t val);
extern volatile inline uint8_t k_inb(uint16_t port);
extern volatile inline void k_insw(uint16_t port, void* addr, uint64_t count);
extern volatile inline void k_outsw(uint16_t port, void* addr, uint64_t count);
extern volatile inline void k_io_wait();

#ifdef __cplusplus
}
#endif
