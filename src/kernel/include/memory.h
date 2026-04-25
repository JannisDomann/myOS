#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void k_memcpy(const void* dest, const void* src, uint64_t count);
void k_memset(const void* dest, uint64_t value, uint64_t count);

#ifdef __cplusplus
}
#endif
