load("@rules_cc//cc:defs.bzl", "cc_binary")
load("@rules_cc//cc:defs.bzl", "cc_library")
load("@rules_cc//cc:defs.bzl", "cc_import")
load("//tools/nasm:nasm.bzl", "nasm_binary")
load("//tools/nasm:nasm.bzl", "nasm_object")

nasm_object(
    name = "stage1_obj",
    src = "stage1/stage1.asm",
    out = "stage1.o",
    visibility = ["//visibility:public"],
)

cc_binary(
    name = "stage1.elf",
    srcs = [":stage1_obj"],
    features = [
        "-default_link_libs",
    ],
    linkstatic = True,
    copts = [
        "-m32",
        "-ffreestanding",
        "-fno-pie",
        "-fno-stack-protector",
        "-O0",
        "-g",
    ],
    linkopts = [
        "-m32",
        "-T", "$(location //src/linker:stage1.ld)",
        "-nostdlib",
        "-static",
        "-no-pie",
        "-Wl,--build-id=none",
    ],
    additional_linker_inputs = ["//src/linker:stage1.ld"],
    visibility = ["//visibility:public"],
)

genrule(
    name = "stage1_bin",
    srcs = [":stage1.elf"],
    outs = ["stage1.bin"],
    cmd = "objcopy -O binary $(SRCS) $@",
    visibility = ["//visibility:public"],
)



nasm_object(
    name = "stage2_startup_obj",
    src = "stage2/stage2.asm",
    srcs = [
        "stage2/a20_check.inc",
        "stage2/a20_kbc.inc",
        "stage2/gdt.inc",
    ],
    out = "stage2_startup.o",
)

nasm_object(
    name = "longmode_obj",
    src = "stage2/longmode.asm",
    out = "longmode.o",
)

nasm_object(
    name = "util_obj",
    src = "stage2/util.asm",
    out = "util.o",
)

cc_import(
    name = "stage2_startup",
    objects = [
        ":stage2_startup_obj",
        ":longmode_obj",
        ":util_obj",
    ],
)

cc_library(
    name = "stage2_hdrs",
    hdrs = [
        "stage2/stage2.h",
        "stage2/MBR.h",
        "stage2/FAT32.h",
        "stage2/ATA_driver.h",
    ],
)

cc_binary(
    name = "stage2.elf",
    srcs = [
        "stage2/stage2.c",
    ],
    deps = [
        ":stage2_startup",
        ":stage2_hdrs",
    ],
    features = [
        "-default_link_libs",
    ],
    linkstatic = True,
    copts = [
        "-m32",
        "-ffreestanding",
        "-fno-pie",
        "-fno-stack-protector",
        "-O0",
        "-g",
    ],
    linkopts = [
        "-m32",
        "-T", "$(location //src/linker:stage2.ld)",
        "-nostdlib",
        "-static",
        "-no-pie",
        "-Wl,--build-id=none",
    ],
    additional_linker_inputs = ["//src/linker:stage2.ld"],
    visibility = ["//visibility:public"],
)

genrule(
    name = "stage2_bin",
    srcs = [":stage2.elf"],
    outs = ["stage2.bin"],
    cmd = "objcopy -O binary $(SRCS) $@",
    visibility = ["//visibility:public"],
)
