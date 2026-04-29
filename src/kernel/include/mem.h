#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define HEAP_START      0xFFFF900000000000
#define HEAP_MIN_SPLIT  0x20

typedef struct free_block {
    uint64_t size;             // Size of the usable memory in this block
    struct free_block* next;   // Pointer to the next free block
} __attribute__((packed)) free_block_t;

void k_memcpy(const void* dest, const void* src, uint64_t count);
void k_memset(const void* dest, uint8_t value, uint64_t count);
void k_memcpy_u16(const void* dest, const void* src, uint64_t count);
void k_memset_u16(const void* dest, uint16_t value, uint64_t count);
void k_memcpy_u64(const void* dest, const void* src, uint64_t count);
void k_memset_u64(const void* dest, uint64_t value, uint64_t count);
uint32_t k_memcmp(const void* s1, const void* s2, uint32_t n);
void* k_malloc(uint64_t size);
void k_free(void* ptr);

#ifdef __cplusplus
}
#endif
