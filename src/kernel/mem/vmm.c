#include "vmm.h"
#include "pmm.h"
#include "mem.h"
#include "io.h"
#include "k_string.h"

static pt_entry* get_next_table(pt_entry* parent_table, uint64_t index, bool allocate) {
    if (parent_table[index] & PTE_PRESENT) {
        // Extract physical address (masking flags) and convert to virtual
        uint64_t phys = parent_table[index] & PAGE_FRAME_MASK;
        return (pt_entry*)(phys + PHYS_OFFSET);
    }

    if (!allocate) {
        return NULL;
    }

    // No table present, allocate a new one via PMM
    pt_entry new_table_phys = (pt_entry)pmm_alloc_frame();
    if (!new_table_phys) {
        return NULL;
    }

    // Zero out the new table (crucial!)
    pt_entry* new_table_virt = (pt_entry*)(new_table_phys + PHYS_OFFSET);
    k_memset_u64(new_table_virt, 0x00, PAGE_SIZE / 0x08);

    // Link new table into parent with standard flags (usually Present | Writable | User)
    // Note: Effective permissions are restricted by the final leaf entry
    parent_table[index] = new_table_phys | PTE_PRESENT | PTE_WRITABLE | PTE_USER;

    return new_table_virt;
}

void vmm_init() {
    // 1. Allocate a frame for the new kernel PML4
    pt_entry new_pml4_phys = (pt_entry)pmm_alloc_frame();
    
    // 2. Access the new PML4 via PHYS_OFFSET
    // This works because Stage 2 already mapped PHYS_OFFSET to the first 1GB
    pt_entry* new_pml4_virt = (pt_entry*)(new_pml4_phys + PHYS_OFFSET);
    k_memset_u64(new_pml4_virt, 0x00, PAGE_SIZE / 0x08);

    // 3. Setup Identity Mapping for the Kernel (Transition phase)
    // Map the first 16MB (or more) to stay alive during CR3 switch
    for (uint64_t addr = 0; addr < 0x1000000; addr += PAGE_SIZE) {
        vmm_map(addr, addr, new_pml4_virt, PTE_PRESENT | PTE_WRITABLE);
    }

    // 4. Setup Physical Memory Direct Mapping (Higher Half)
    // Calculate the number of pages needed to cover all bytes
    uint64_t total_pages = (total_mem_bytes + PAGE_SIZE - 1) / PAGE_SIZE;
    // Map the entire available RAM starting at PHYS_OFFSET
    for (uint64_t i=0; i<total_pages; i++) {
        uint64_t addr = i * PAGE_SIZE;
        vmm_map(addr + PHYS_OFFSET, addr, new_pml4_virt, PTE_PRESENT | PTE_WRITABLE);
    }

    // 5. Setup Kernel Higher Half Mapping (Optional but recommended)
    // If your kernel is linked to 0xFFFFFFFF80000000, map it here.
    // For now, we rely on the Direct Mapping access.

    // 6. Switch to the new PML4
    k_write_cr3(new_pml4_phys);

    // 7. Cleanup: From here on, we could unmap the Identity Mapping (0x0...)
    // to catch NULL pointer dereferences in kernel space.
    // unmap bootloader section in first 1MB
    for (uint64_t addr=0x00; addr<0x100000; addr+=PAGE_SIZE) {
        vmm_unmap(addr, NULL);
    }

    // 8. NOW: Unmap the NULL page to activate the safety net
    // We pass NULL as the second argument so vmm_unmap uses the current CR3
    vmm_unmap(0x00, NULL);

    k_printf("VMM initialized. NULL-pointer protection active.\n");
}

bool vmm_map(uint64_t virt, uint64_t phys, pt_entry* pml4, uint64_t flags) {
    // Start at the current PML4 (or stored in CR3)
    if (!pml4) {
        uint64_t cr3 = k_read_cr3();
        // Convert the CR3 physical address to virtual via our offset
        pml4 = (pt_entry*)((cr3 & PAGE_FRAME_MASK) + PHYS_OFFSET);
    }

    // Traverse the hierarchy
    pt_entry* pdpt = get_next_table(pml4, PML4_IDX(virt), true);
    if (!pdpt) {
        return false;
    }

    pt_entry* pd = get_next_table(pdpt, PDPT_IDX(virt), true);
    if (!pd) {
        return false;
    }

    pt_entry* pt = get_next_table(pd, PD_IDX(virt), true);
    if (!pt) {
        return false;
    }

    // Set the final PT entry
    uint64_t idx = PT_IDX(virt);
    if (pt[idx] & PTE_PRESENT) {
        // Warning: Page already mapped. Decide if you want to overwrite or fail.
        return false; 
    }

    pt[idx] = (phys & PAGE_FRAME_MASK) | flags | PTE_PRESENT;

    // Invalidate TLB for this address
    k_invlpg(virt);

    return true;
}

bool vmm_unmap(uint64_t virt, pt_entry* pml4) {
    // If no PML4 provided, use the current one from CR3
    if (!pml4) {
        uint64_t cr3 = k_read_cr3();
        pml4 = (pt_entry*)((cr3 & PAGE_FRAME_MASK) + PHYS_OFFSET);
    }

    // Traverse the hierarchy, but DO NOT allocate new tables
    // We only want to find existing mappings to remove them
    pt_entry* pdpt = get_next_table(pml4, PML4_IDX(virt), false);
    if (!pdpt) {
        return false;
    }

    pt_entry* pd = get_next_table(pdpt, PDPT_IDX(virt), false);
    if (!pd) {
        return false;
    }

    pt_entry* pt = get_next_table(pd, PD_IDX(virt), false);
    if (!pt) {
        return false;
    }

    uint64_t idx = PT_IDX(virt);
    
    // Clear the entry (set to 0)
    pt[idx] = 0x00;

    // Flush the TLB for this specific address
    k_invlpg(virt);

    return true;
}

void* vmm_allocate_kernel_pages(uint64_t count, uint64_t start_vaddr) {
    // 1. Get a physical frames from PMM
    void* phys_base;
    if (!(phys_base = pmm_alloc_frames(count))) {
        return NULL;
    }

    for (uint64_t i=0; i<count; ++i) {
        // 2. Map it to the virtual address
        uint64_t current_virt = start_vaddr + (i * PAGE_SIZE);
        uint64_t current_phys = (uint64_t)phys_base + (i * PAGE_SIZE);

        if (!vmm_map(current_virt, current_phys, NULL, PTE_PRESENT | PTE_WRITABLE)) {
            // ERROR: We must free the physical frames we allocated
            // Note: We use phys_base (physical), not start_vaddr (virtual)!
            // Assuming your PMM has a function like pmm_free_range(phys, count)
            pmm_free_range(phys_base, count * PAGE_SIZE);
            
            // Optional: Unmap the pages that WERE successful before the error
            // to keep the VMM clean
            for (uint64_t j=0; j<i; ++j) {
                vmm_unmap(start_vaddr + (j * PAGE_SIZE), NULL);
            }
            
            return NULL;
        }
    }

    return (void*)start_vaddr;
}
