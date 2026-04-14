load("//tools/nasm:nasm.bzl", "nasm_binary")

nasm_binary(
    name = "stage1",
    src = "stage1/stage1.asm",
    out = "stage1.bin",
    visibility = ["//visibility:public"],
)

nasm_binary(
    name = "stage2",
    src = "stage2/stage2.asm",
    out = "stage2.bin",
    visibility = ["//visibility:public"],
)
