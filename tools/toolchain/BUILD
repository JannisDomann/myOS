toolchain(
    name = "i686_elf_toolchain",
    toolchain_type = "@bazel_tools//tools/cpp:toolchain_type",
    toolchain = ":i686_elf_cc",
)

filegroup(
    name = "i686_elf_cc",
    srcs = [
        "i686-elf-gcc",
        "i686-elf-ld",
        "i686-elf-as",
        "i686-elf-objcopy",
    ],
)
