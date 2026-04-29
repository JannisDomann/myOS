#include "mem.h"
#include "pmm.h"
#include "vmm.h"

static free_block_t* free_list_head = NULL;

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

uint32_t k_memcmp(const void* s1, const void* s2, uint32_t n) {
    const uint8_t* p1 = (uint8_t*)s1;
    const uint8_t* p2 = (uint8_t*)s2;
    for (uint32_t i = 0; i < n; i++) {
        if (p1[i] != p2[i]) return p1[i] - p2[i];
    }
    return 0;
}



static void heap_expand(uint64_t size) {
    // Align size to full pages
    uint64_t pages = (size + sizeof(free_block_t) + PAGE_SIZE - 0x01) / PAGE_SIZE;
    
    // Allocate and map via VMM (using our new vmm_map)
    // For now, we'll use a simple static pointer for the next virtual heap address
    static uint64_t current_vheap_addr = HEAP_START;
    
    void* new_mem;
	if (!(new_mem = vmm_allocate_kernel_pages(pages, current_vheap_addr))) {
		return;
	}
    
    // Create a new block header at the start of this memory
    free_block_t* new_block = (free_block_t*)current_vheap_addr;
    new_block->size = (pages * PAGE_SIZE) - sizeof(free_block_t);
    new_block->next = free_list_head;
    free_list_head = new_block;

    current_vheap_addr += (pages * PAGE_SIZE);
}

void* k_malloc(uint64_t size) {
	if (!size) {
		return NULL;
	}

	// 8-byte alignment for performance
	size = (size + 0x07) & ~0x07;

	free_block_t* prev = NULL;
	free_block_t* curr = free_list_head;

	while (curr) {
		if (curr->size >= size) {          
			// Check if we can split the block
            // We need enough space for: requested size + new header + some data
            if (curr->size >= (size + sizeof(free_block_t) + HEAP_MIN_SPLIT)) {
                
                // Calculate position for the new header (behind the allocated data)
                free_block_t* next_block = (free_block_t*)((uint64_t)curr + sizeof(free_block_t) + size);
                
                // Set up the new remaining block
                next_block->size = curr->size - size - sizeof(free_block_t);
                next_block->next = curr->next;
                
                // Update the list: Replace current with the new remainder
                if (prev) {
					prev->next = next_block;
				}
                else {
					free_list_head = next_block;
				}
                
                // Update current block to exact requested size
                curr->size = size;
			}
			else {
				if (prev) {
					prev->next = curr->next;
				}
				else {
					free_list_head = curr->next;
				}
			}

			return (void*)((uint64_t)curr + sizeof(free_block_t));
		}
		prev = curr;
		curr = curr->next;
	}

	// If we reach this point, the list was walked but NO block was big enough
	heap_expand(size);

	// After expanding, the new memory is at the HEAD of the list.
    // We try again. This time it MUST succeed (unless PMM is out of memory).
	k_malloc(size); //retry
}

void k_free(void* ptr) {
    if (!ptr) {
		return;
	}

    free_block_t* block_to_free = (free_block_t*)((uint64_t)ptr - sizeof(free_block_t));

    // 1. Insert into list sorted by address
    if (!free_list_head || block_to_free < free_list_head) {
        block_to_free->next = free_list_head;
        free_list_head = block_to_free;
    }
	else {
        free_block_t* curr = free_list_head;
        while (curr->next && curr->next < block_to_free) {
            curr = curr->next;
        }
        block_to_free->next = curr->next;
        curr->next = block_to_free;
    }

    // 2. Coalesce (Merge neighbors)
    free_block_t* curr = free_list_head;
    while (curr && curr->next) {
        uint64_t end_of_curr = (uint64_t)curr + sizeof(free_block_t) + curr->size;
        
        if (end_of_curr == (uint64_t)curr->next) {
            // They are neighbors! Merge them.
            curr->size += sizeof(free_block_t) + curr->next->size;
            curr->next = curr->next->next;
            // Don't move curr yet, it might merge with the NEW next one too!
        } else {
            curr = curr->next;
        }
    }
}
