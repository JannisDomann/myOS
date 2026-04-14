def _nasm_object_impl(ctx):
    out = ctx.outputs.out
    src = ctx.file.src

    ctx.actions.run(
        inputs = [src],
        outputs = [out],
        arguments = [
            "-f", "elf32",
            src.path,
            "-o", out.path,
        ],
        executable = ctx.executable.nasm,
    )

    return DefaultInfo(files = depset([out]))


nasm_object = rule(
    implementation = _nasm_object_impl,
    attrs = {
        "src": attr.label(allow_single_file = True),
        "nasm": attr.label(
            allow_single_file = True,
            default = Label("//tools/nasm:nasm_wrapper"),
            executable = True,
            cfg = "exec",
        ),
        "out": attr.output(),
    },
)


def _nasm_binary_impl(ctx):
    out = ctx.outputs.out
    src = ctx.file.src

    ctx.actions.run(
        inputs = [src],
        outputs = [out],
        arguments = [
            "-f", "bin",
            src.path,
            "-o", out.path,
        ],
        executable = ctx.executable.nasm,
    )

    return DefaultInfo(files = depset([out]))


nasm_binary = rule(
    implementation = _nasm_binary_impl,
    attrs = {
        "src": attr.label(allow_single_file = True),
        "nasm": attr.label(
            allow_single_file = True,
            default = Label("//tools/nasm:nasm_wrapper"),
            executable = True,
            cfg = "exec",
        ),
        "out": attr.output(),
    },
)
