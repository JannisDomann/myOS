load("@rules_cc//cc:defs.bzl", "cc_binary")
load("//tools/nasm:nasm.bzl", "nasm_object")

nasm_object(
    name = "startup",
    src = "kernel.asm",
    out = "startup.o",
)

cc_binary(
    name = "kernel_elf",
    srcs = [
        "kernel.c",
        ":startup",
    ],
    copts = [
        "-m32",
        "-ffreestanding",
        "-fno-pie",
        "-fno-stack-protector",
        "-nostdlib",
    ],
    linkopts = [
        "-m32",
        "-nostdlib",
        "-T", "//src/linker:linker_ld",
    ],
    linkstatic = True,
    visibility = ["//visibility:public"],
)

genrule(
    name = "kernel_bin",
    srcs = [":kernel_elf"],
    outs = ["kernel.bin"],
    cmd = "objcopy -O binary $(SRCS) $@",
    visibility = ["//visibility:public"],
)
