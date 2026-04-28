load("@rules_cc//cc:defs.bzl", "cc_library")
package(default_visibility = ["//visibility:public"])

cc_library(
    name = "headers",
    hdrs = glob(["*.h"]),
    includes = ["."],
)

cc_library(
    name = "FAT32",
    hdrs = [
        "FAT32.h",
        "MBR.h",
    ],
    includes = ["."],
    visibility = ["//visibility:public"],
)
