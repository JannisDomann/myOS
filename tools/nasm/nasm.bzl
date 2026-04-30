def _nasm_object_impl(ctx):
    out = ctx.outputs.out
    src = ctx.file.src
    srcs = ctx.files.srcs
    
    # Sammle alle Verzeichnisse der zusätzlichen Sourcen (wie deine .inc Datei)
    # So finden wir Pfade in 'bazel-out' automatisch.
    include_paths = []
    for f in srcs:
        if f.dirname not in include_paths:
            include_paths.append(f.dirname)

    # Initialisiere die Argumente
    args = [
        "-g",
        "-F", "dwarf",
        "-f", "elf32",
    ]
    
    # Füge jedes Verzeichnis als -I Flag hinzu
    for path in include_paths:
        args.append("-I" + path + "/")
    
    # Wichtig: Auch das Verzeichnis der .asm Quelldatei selbst hinzufügen
    args.append("-I" + src.dirname + "/")
    
    # Das aktuelle Arbeitsverzeichnis (Workspace-Root in der Sandbox)
    args.append("-I./")

    # Quelldatei und Output
    args.extend([
        src.path,
        "-o", out.path,
    ])

    ctx.actions.run(
        inputs = [src] + srcs,
        outputs = [out],
        arguments = args,
        executable = ctx.executable.nasm,
        mnemonic = "NasmCompile",
    )

    return DefaultInfo(files = depset([out]))


def _nasm_object_impl_64(ctx):
    out = ctx.outputs.out
    src = ctx.file.src
    srcs = ctx.files.srcs

    ctx.actions.run(
        inputs = [src] + srcs, # Hier werden die Dateien für die Sandbox registriert
        outputs = [out],
        arguments = [
            "-g",
            "-F", "dwarf",
            "-f", "elf64",
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

nasm_object_64 = rule(
    implementation = _nasm_object_impl_64,
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
