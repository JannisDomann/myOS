# 🚀 myOS - x86_64 Long Mode Research Kernel

A custom, from-scratch 64-bit operating system kernel developed for the x86_64 architecture. Featuring a modern memory management system, a custom bootloader chain, and a higher-half kernel design with complete abandonment of C inline assembly code.

## 🛠 Project Architecture

The project is built with **Bazel** and uses **NASM** and **GCC** for a clean, reproducible build pipeline.

### 🏗 Bootloader Chain (Custom Stage 1 & 2)
The boot process is handled by a custom two-stage bootloader before jumping into the kernel.

**Stage 1 (MBR):**
*   **Sector 0:** Fits within 512 bytes.
*   **Stage 2 Loction:** Locates and loads Stage 2 from Sector 2.
*   **Transition:** Passes execution to the 16-bit real mode entry point of Stage 2.

**Stage 2:**
*   **A20 Gate:** 3-staged enablement via BIOS, Fast A20 or Keyboard Controller (KBC).
*   **Hardware Discovery:** Detects memory map (E820) for the PMM.
*   **Paging Bootstrap:** Sets up initial 4-level paging (PML4) with 1GB identity mapping.
*   **Long Mode Switch:** Transitions the CPU from 16-bit Real Mode over 32-bit Protected Mode and finally to 64-bit Long Mode with active Paging.
*   **Kernel Loader:** Parses the file system via ATA driver with FAT32 filesystem, loads the kernel file into RAM, and jumps to its entry point.

### 🧠 The Kernel
The heart of myOS, running in the **Higher Half** of the virtual address space.

**Core Features:**
*   **Higher Half Design:** Kernel code and data are mapped to `0xFFFF800000000000` via a Direct Mapping Offset.
*   **Physical Memory Manager (PMM):** Efficient Bitmap-based allocator for tracking physical frames.
*   **Virtual Memory Manager (VMM):** 
    *   Dynamic 4-level paging (PML4 -> PDPT -> PD -> PT).
    *   Full isolation of the lower 1MB (Bootloader & BIOS area protection).
    *   NULL-pointer protection (unmapped 0x0 virtual page).
*   **Kernel Heap:** A dynamic memory allocator supporting `k_malloc` and `k_free` with splitting and coalescing logic.
*   **Interrupt Handling:** Custom IDT (Interrupt Descriptor Table) with ISRs for CPU exceptions and hardware IRQs.
*   **GDT Migration:** Self-sufficient kernel GDT, independent of the bootloader's initial tables.
*   **Formatted text output:** Custom `k_printf` function supporting various types and colors
*   **Debugging:** Basic debugging implementation with kernel panic via `k_assert`

**Drivers & File System:**
*   **ATA PIO Driver:** Basic disk I/O with IRQ synchronization.
*   **FAT32 Driver:** Read support for files and cluster chain traversal.
*   **VGA Driver:** Text-mode output mapped into the higher half.
*   **Keyboard Driver:** PS/2 keyboard support with an interrupt-driven buffer.

## 🚦 Getting Started

### Prerequisites
* **Bazel** (Build System)
* **QEMU** (Emulator)
* **GDB** (Debugger)
* **VS Code** (Recommended for the integrated debugging experience)

### ⚡ Integrated Workflow (VS Code Tasks)
The project includes a `tasks.json` to automate the build and emulation cycle. You can trigger these via `Ctrl+Shift+P`:

*   **Build myOS**: Compiles the disk image in debug mode.
    ```bash
    bazel build //:disk_image -c dbg && chmod 666 $(bazel info -c dbg bazel-bin)/disk.img
    ```
*   **Run QEMU**: Builds and launches the OS.
    ```bash
    qemu-system-x86_64 -drive format=raw,file=$(bazel info -c dbg bazel-bin)/disk.img
    ```
*   **Debug QEMU**: Starts QEMU with a GDB stub enabled (`-S -s`).
    ```bash
    qemu-system-x86_64 -drive format=raw,file=$(bazel info -c dbg bazel-bin)/disk.img -S -s
    ```
*   **Cleanup**: Purges the Bazel cache.
    ```bash
    bazel clean --expunge
    ```

### 🐞 Professional Debugging
The project features a full GDB integration. The `launch.json` is configured to map symbols for all stages:

**Key Benefits:**
* **Multi-Stage Symbol Mapping:** Automatically maps symbols for Stage 1 (at `0x7C00`), Stage 2 (at `0x7E00`), and the Kernel.
* **Seamless Workflow:** Pressing `F5` in VS Code triggers the `Debug QEMU` task and attaches GDB immediately.
* **Intel Syntax:** Disassembly is set to Intel flavor by default.

### 🧹 Maintenance
To remove all build artifacts and force a clean state:
```bash
bazel clean --expunge
```

## 🗺 Roadmap

- **[x] Higher Half Mapping & VMM Switch**
- **[x] Kernel Heap (k_malloc/k_free)**
- [ ] Multitasking (Task Switching & Scheduler)
- [ ] User-Mode & Syscalls
- [ ] AHCI / SATA Driver support**

---
***Developed with passion for low-level systems.*** ❤️ 

