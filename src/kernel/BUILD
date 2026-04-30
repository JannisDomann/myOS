load("@rules_cc//cc:defs.bzl", "cc_binary")
load("@rules_cc//cc:defs.bzl", "cc_library")
load("@rules_cc//cc:defs.bzl", "cc_import")
load("//tools/nasm:nasm.bzl", "nasm_object_64")

nasm_object_64(
    name = "kernel_startup_obj",
    src = "arch/x86_64/kernel.asm",
    out = "kernel_startup.o",
)

nasm_object_64(
    name = "io_obj",
    src = "arch/x86_64/io.asm",
    out = "io.o",
)

nasm_object_64(
    name = "interrupts_obj",
    src = "arch/x86_64/interrupts.asm",
    out = "interrupts.o",
)

cc_import(
    name = "kernel_startup",
    objects = [
        ":kernel_startup_obj",
        ":io_obj",
        ":interrupts_obj",
    ],
)

cc_library(
    name = "kernel_lib",
    srcs = glob([
        "core/*.c",
        "drivers/*.c",
        "fs/*.c",
        "lib/*.c",
        "mem/*.c",
        "arch/x86_64/*.c",
    ]),
    deps = [
        ":kernel_startup",
        "//src/kernel/include:headers",
        "//src/include/shared:memory_layout",

    ],
    copts = [
        "-m64",
        "-ffreestanding",
        "-fno-stack-protector",
        "-mno-red-zone", 
    ],
)

genrule(
    name = "processed_kernel_ld",
    srcs = [
        "//src/linker:kernel.ld.S",
        "//src/include/shared:memory_layout.h",
    ],
    outs = ["kernel.ld"],
    cmd = "gcc -E -x c -P $(location //src/linker:kernel.ld.S) -I$$(dirname $(location //src/include/shared:memory_layout.h)) -o $@",
)

cc_binary(
    name = "kernel.elf",
    srcs = [
        ":kernel_startup_obj",
    ],
    deps = [
        ":kernel_lib",
        "//src/include/shared:memory_layout",
    ],
    features = [
        "-default_link_libs",
    ],
    linkstatic = True,
    copts = [
        "-m64",
        "-ffreestanding",
        "-fno-pic",
        "-fno-pie",
        "-fno-stack-protector",
        "-mno-sse",
        "-mno-mmx",
        "-mno-red-zone", 
        "-O0",
        "-g",
    ],
    linkopts = [
        "-m64",
        "-T", "$(location :processed_kernel_ld)",
        "-nostdlib",
        "-static",
        "-no-pie",
        "-Wl,--build-id=none",
    ],
    additional_linker_inputs = [":processed_kernel_ld"],
    visibility = ["//visibility:public"],
)
