genrule(
    name = "disk_image",
    srcs = [
        "//src/bootloader:stage1_bin",
        "//src/bootloader:stage2_bin",
        "//src/kernel:kernel_bin",
    ],
    outs = ["disk.img"],
    cmd = """
        # empty 32MB image
        dd if=/dev/zero of=$@ bs=1M count=32

        # stages 1&2 in first 2 sectors
        dd if=$(location //src/bootloader:stage1_bin) of=$@ conv=notrunc bs=512 count=1
        dd if=$(location //src/bootloader:stage2_bin) of=$@ conv=notrunc bs=512 seek=1

        # creates a partition table in mbr from sector 2048, type 'b' (FAT32), '*' (bootable)
        echo "2048,,b,*" | /usr/sbin/sfdisk $@

        # format with offset
        /usr/sbin/mkfs.vfat -F 32 --offset 2048 $@ 60000

        # copy kernel to partition
        MTOOLS_SKIP_CHECK=1 mcopy -i $@@@1048576 $(location //src/kernel:kernel_bin) ::/kernel.sys
        """,
)
        