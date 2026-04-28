#include "mem.h"

void k_memcpy(const void* dest, const void* src, uint64_t count) {
	uint8_t* d = (uint8_t*)dest;
	uint8_t* s = (uint8_t*)src;
	for (uint64_t i=0; i<count; ++i) {
		*d++ = *s++;
	}
}

void k_memcpy_u16(const void* dest, const void* src, uint64_t count) {
	uint16_t* d = (uint16_t*)dest;
	uint16_t* s = (uint16_t*)src;
	for (uint64_t i=0; i<count; ++i) {
		*d++ = *s++;
	}
}

void k_memcpy_u64(const void* dest, const void* src, uint64_t count) {
	uint64_t* d = (uint64_t*)dest;
	uint64_t* s = (uint64_t*)src;
	for (uint64_t i=0; i<count; ++i) {
		*d++ = *s++;
	}
}

void k_memset(const void* dest, uint8_t value, uint64_t count) {
	uint8_t* d = (uint8_t*)dest;
	for (uint64_t i=0; i<count; ++i) {
		*d++ = value;
	}
}

void k_memset_u16(const void* dest, uint16_t value, uint64_t count) {
	uint16_t* d = (uint16_t*)dest;
	for (uint64_t i=0; i<count; ++i) {
		*d++ = value;
	}
}

void k_memset_u64(const void* dest, uint64_t value, uint64_t count) {
	uint64_t* d = (uint64_t*)dest;
	for (uint64_t i=0; i<count; ++i) {
		*d++ = value;
	}
}

