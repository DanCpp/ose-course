#ifndef KERNEL_PANIC_H
#define KERNEL_PANIC_H

#include <stdarg.h>
void vkernel_panic(const char* fmt, va_list args);
void kernel_panic(const char* fmt, ...);
#endif // KERNEL_PANIC_H
