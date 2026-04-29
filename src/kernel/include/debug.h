#pragma once

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

void kernel_panic(const char* file, int line, const char* msg);

#define k_assert(condition, msg) \
    if (!(condition)) { \
        kernel_panic(__FILE__, __LINE__, msg); \
    }

#ifdef __cplusplus
}
#endif
