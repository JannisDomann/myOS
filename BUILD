genrule(
    name = "disk_image",
    srcs = [
        "//src/bootloader:stage1",
        # "//src/bootloader:stage2",
        # "//src/kernel:kernel_bin",
    ],
    outs = ["disk.img"],
    cmd = """
        dd if=/dev/zero of=$@ bs=1M count=10
        dd if=$(location //src/bootloader:stage1) of=$@ conv=notrunc
        """,
    #     dd if=$(location //src/bootloader:stage2) of=$@ seek=1 conv=notrunc
    #     dd if=$(location //src/kernel:kernel_bin) of=$@ seek=10 conv=notrunc
    # """,
)
