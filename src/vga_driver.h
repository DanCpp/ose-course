#ifndef VGA_DRIVER_H
#define VGA_DRIVER_H

#include "types.h"
#include <stdarg.h>
void vga_clear_screen();
void vga_print_char(char c, size_t x, size_t y);
void vga_scroll_down();

void init_printer();
void vprintf(const char* fmt, va_list vargs);
void printf(const char* fmt, ...);
void putchar(char c);

#endif // VGA_DRIVER_H
