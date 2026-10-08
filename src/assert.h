#ifndef ASSERT_H
#define ASSERT_H

#ifdef DEBUG

#include "kernel_panic.h"

#define assert(cond) \
    do { \
        if (!(cond)) { \
            kernel_panic("assertion failed: %s, file %s, line %d\n", #cond, __FILE__, __LINE__); \
        } \
    } while (0)

#define assert(cond, msg) \
    do { \
        if (!(cond)) { \
            kernel_panic("assertion failed: %s, file %s, line %d: %s\n", #cond, __FILE__, __LINE__, msg); \
        } \
    } while (0)

#else

#define assert(cond) do { } while (0)
#define assert(cond, msg) do { } while (0)

#endif

#endif // ASSERT_H
