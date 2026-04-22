load("@rules_cc//cc:defs.bzl", "cc_binary")
load("@rules_cc//cc:defs.bzl", "cc_library")
load("@rules_cc//cc:defs.bzl", "cc_import")
load("//tools/nasm:nasm.bzl", "nasm_object")
load("//tools/nasm:nasm.bzl", "nasm_object_64")

nasm_object_64(
    name = "kernel_startup_obj",
    src = "kernel.asm",
    out = "kernel.o",
)

nasm_object_64(
    name = "util_obj",
    src = "util64.asm",
    out = "util64.o",
)

cc_import(
    name = "kernel_startup",
    objects = [
        ":kernel_startup_obj",
        ":util_obj",
    ],
)

cc_library(
    name = "kernel_hdrs",
    hdrs = [
        "kernel.h",
    ],
)

cc_binary(
    name = "kernel_elf",
    srcs = [
        "kernel.c",
    ],
    deps = [
        ":kernel_startup",
        ":kernel_hdrs",
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
    ],
    linkopts = [
        "-m64",
        "-T", "$(location //src/linker:linker.ld)",
        "-nostdlib",
        "-static",
        "-no-pie",
        "-Wl,--build-id=none",
    ],
    additional_linker_inputs = ["//src/linker:linker.ld"],
    visibility = ["//visibility:public"],
)

genrule(
    name = "kernel_bin",
    srcs = [":kernel_elf"],
    outs = ["kernel.bin"],
    cmd = "objcopy -O binary $(SRCS) $@",
    visibility = ["//visibility:public"],
)
