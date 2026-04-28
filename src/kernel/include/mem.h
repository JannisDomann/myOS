#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void k_memcpy(const void* dest, const void* src, uint64_t count);
void k_memset(const void* dest, uint8_t value, uint64_t count);
void k_memcpy_u16(const void* dest, const void* src, uint64_t count);
void k_memset_u16(const void* dest, uint16_t value, uint64_t count);
void k_memcpy_u64(const void* dest, const void* src, uint64_t count);
void k_memset_u64(const void* dest, uint64_t value, uint64_t count);
uint32_t k_memcmp(const void* s1, const void* s2, uint32_t n);

#ifdef __cplusplus
}
#endif
