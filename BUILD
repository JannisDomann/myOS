genrule(
    name = "disk_image",
    srcs = [
        "//src/bootloader:stage1",
        # "//src/bootloader:stage2_bin",
        # "//src/kernel:kernel_bin",
    ],
    outs = ["disk.img"],
    cmd = """
        # empty 32MB image
        dd if=/dev/zero of=$@ bs=1M count=32

        # stage 1 in sector 0
        dd if=$(location //src/bootloader:stage1) of=$@ conv=notrunc bs=512 count=1
        
        # stage 2 in sector 1

        # creates a partition table in mbr from sector 2048, type 'b' (FAT32), '*' (bootable)
        echo "2048,,b,*" | /usr/sbin/sfdisk $@

        /usr/sbin/mkfs.vfat -F 32 --offset 2048 $@ 60000

        # copy kernel to partition

        """,
)
#dd if=$(location //src/bootloader:stage2_bin) of=$@ seek=1 conv=notrunc bs=512 seek=1
#MTOOLS_SKIP_CHECK=1 mcopy -i $@@@2048 $(location //src/kernel:kernel_bin) ::kernel.sys
