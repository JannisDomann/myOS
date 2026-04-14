exports_files(["nasm"])

genrule(
    name = "nasm_wrapper",
    srcs = ["nasm"],
    outs = ["nasm_exec.sh"],
    cmd = "cp $(location nasm) $@ && chmod +x $@",
    executable = True,
    visibility = ["//visibility:public"],
)
