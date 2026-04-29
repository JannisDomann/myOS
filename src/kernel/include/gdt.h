#pragma once


#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

// Typedefs
typedef struct {
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t  base_middle;
    uint8_t  access;
    uint8_t  granularity;
    uint8_t  base_high;
} __attribute__((packed)) gdt_entry;

typedef struct {
    uint16_t limit;
    uint64_t base;
} __attribute__((packed)) gdt_ptr;

// Functions
void gdt_init();

#ifdef __cplusplus
}
#endif
