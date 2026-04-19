def _nasm_object_impl(ctx):
    out = ctx.outputs.out
    src = ctx.file.src
    srcs = ctx.files.srcs

    ctx.actions.run(
        inputs = [src] + srcs, # Hier werden die Dateien für die Sandbox registriert
        outputs = [out],
        arguments = [
            "-f", "elf32",
            "-I" + src.dirname + "/", # Füge das Verzeichnis der Quelle als Include-Pfad hinzu
            src.path,
            "-o", out.path,
        ],
        executable = ctx.executable.nasm,
    )

    return DefaultInfo(files = depset([out]))

nasm_object = rule(
    implementation = _nasm_object_impl,
    attrs = {
        "src": attr.label(allow_single_file = True, mandatory = True),
        "srcs": attr.label_list(allow_files = True), # Das fehlende Attribut
        "nasm": attr.label(
            allow_single_file = True,
            default = Label("//tools/nasm:nasm_wrapper"),
            executable = True,
            cfg = "exec",
        ),
        "out": attr.output(mandatory = True),
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
