load("@rules_cc//cc:defs.bzl", "cc_binary")
load("//tools/nasm:nasm.bzl", "nasm_binary")
load("//tools/nasm:nasm.bzl", "nasm_object")

nasm_binary(
    name = "stage1",
    src = "stage1/stage1.asm",
    out = "stage1.bin",
    visibility = ["//visibility:public"],
)

nasm_object(
    name = "stage2_startup",
    src = "stage2/stage2.asm",
    out = "stage2_startup.o",
)

cc_binary(
    name = "stage2_elf",
    srcs = [
        "stage2.c",
        ":stage2_startup",
    ],
    copts = [
        "-m32",
        "-ffreestanding",
        "-fno-pie",
        "-fno-stack-protector",
        "-nostdlib",
    ],
    linkopts = [
        "-melf_i386",
        "-nostdlib",
        "-T", "//src/linker:stage2.ld",
    ],
    linkstatic = True,
    additional_linker_inputs = ["//src/linker:stage2.ld"],
    visibility = ["//visibility:public"],
)

genrule(
    name = "stage2_bin",
    srcs = [":stage2_elf"],
    outs = ["stage2.bin"],
    cmd = "objcopy -O binary $(SRCS) $@",
    visibility = ["//visibility:public"],
)
