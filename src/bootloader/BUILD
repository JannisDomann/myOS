load("@rules_cc//cc:defs.bzl", "cc_binary")
load("@rules_cc//cc:defs.bzl", "cc_import")
load("//tools/nasm:nasm.bzl", "nasm_binary")
load("//tools/nasm:nasm.bzl", "nasm_object")

nasm_binary(
    name = "stage1",
    src = "stage1/stage1.asm",
    out = "stage1.bin",
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

cc_import(
    name = "stage2_startup",
    objects = [
        ":stage2_startup_obj",
    ],
)
cc_binary(
    name = "stage2_elf",
    srcs = [
        "stage2/stage2.c",
    ],
    deps = [
        ":stage2_startup",
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
    srcs = [":stage2_elf"],
    outs = ["stage2.bin"],
    cmd = "objcopy -O binary $(SRCS) $@",
    visibility = ["//visibility:public"],
)
