#include "types.h"

#include <stdarg.h>
#include <stdbool.h>

#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define VGA_MEMORY_START 0xB8000
#define VGA_MEMORY_END 0xB8FA0

typedef enum {
  VGA_COLOR_BLACK,
  VGA_COLOR_BLUE,
  VGA_COLOR_GREEN,
  VGA_COLOR_CYAN,
  VGA_COLOR_RED,
  VGA_COLOR_PURPLE,
  VGA_COLOR_BROWN,
  VGA_COLOR_LIGHT_GRAY,
  VGA_COLOR_DARK_GRAY,
  VGA_COLOR_LIGHT_BLUE,
  VGA_COLOR_LIGHT_GREEN,
  VGA_COLOR_LIGHT_CYAN,
  VGA_COLOR_LIGHT_RED,
  VGA_COLOR_LIGHT_PURPLE,
  VGA_COLOR_YELLOW,
  VGA_COLOR_WHITE,
} vga_color;

typedef struct {
  size_t x;
  size_t y;
} cursor_struct;


cursor_struct cursor = {0, 0};
vga_color letter_color = VGA_COLOR_LIGHT_GRAY;
vga_color background_color = VGA_COLOR_BLACK;
u16* mem = (u16*)VGA_MEMORY_START;

void vga_clear_screen() {
  for (u32 i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++) {
    mem[i] = 0;
  }
}

void vga_print_char(char c, size_t x, size_t y) {
  if (x >= VGA_WIDTH || y >= VGA_HEIGHT) {
    return;
  }

  u16 color_byte = (background_color << 4) | letter_color;
  u16 value = (color_byte << 8) | c;
  mem[y * VGA_WIDTH + x] = value;
}

void vga_scroll_down() {
  for (u32 y = 1; y < VGA_HEIGHT; y++) {
    for (u32 x = 0; x < VGA_WIDTH; x++) {
      mem[(y - 1) * VGA_WIDTH + x] = mem[y * VGA_WIDTH + x];
    }
  }

  for (u32 x = 0; x < VGA_WIDTH; x++) {
    mem[(VGA_HEIGHT - 1) * VGA_WIDTH + x] = 0;
  }
}

void init_printer() {
  cursor.x = 0;
  cursor.y = 0;

  vga_clear_screen();
}

void cursor_next() {
  cursor.x++;
  if (cursor.x >= VGA_WIDTH) {
    cursor.x = 0;
    cursor.y++;
    if (cursor.y >= VGA_HEIGHT) {
      vga_scroll_down();
      cursor.y = VGA_HEIGHT - 1;
    }
  }
}



void putchar(char c) {
  vga_print_char(c, cursor.x, cursor.y);
  cursor_next();
}

void print_string(const char* str) {
  if (str == NULL) {
    return;
  }

  for (const char* p = str; *p; ++p) {
    putchar(*p);
  }
}

void print_unsigned(u32 num, u8 radix) {
  if (num == 0) {
    putchar('0');
    return;
  }

  char buffer[33];
  u32 i = 0;
  for (; num > 0; i++) {
    u32 digit = num % radix;
    buffer[i] = (digit < 10) ? ('0' + digit) : ('A' + (digit - 10));
    num /= radix;
  }

  buffer[i] = '\0';

  for (i32 j = i - 1; j >= 0; j--) {
    putchar(buffer[j]);
  }
}

void print_signed(i32 num, u8 radix) {
  if (num < 0) {
    putchar('-');
    num = -num;
  }
  print_unsigned((u32)num, radix);
}

void clear_current_line() {
  for (u32 x = 0; x < VGA_WIDTH; x++) {
    mem[cursor.y * VGA_WIDTH + x] = 0;
  }
}

void apply_escape(char c) {
  switch(c) {
    case '\n':
      cursor.x = 0;
      cursor.y++;
      if (cursor.y >= VGA_HEIGHT) {
        vga_scroll_down();
        cursor.y = VGA_HEIGHT - 1;
      }
      break;
    case '\r':
      clear_current_line();
      cursor.x = 0;
      break;
    default:
      break; // will be kernel_panic
  }
}

bool is_escape_sequence(char c) {
  return c == '\n' || c == '\r';
}

void apply_format(char c, va_list vargs) {
  switch(c) {
    case 'c':
      char ch = (char)va_arg(vargs, int);
      putchar(ch);
      break;
    case 's':
      const char* str = va_arg(vargs, const char*);
      print_string(str);
      break;
    case 'd':
      i32 num = va_arg(vargs, i32);
      print_signed(num, 10);
      break;
    case 'u':
      u32 unum = va_arg(vargs, u32);
      print_unsigned(unum, 10);
      break;
    case 'x':
      u32 hexnum = va_arg(vargs, u32);
      print_unsigned(hexnum, 16);
      break;
    case 'b':
      u32 binum = va_arg(vargs, u32);
      print_unsigned(binum, 2);
      break;
    case '%':
      putchar('%');
      break;
    default:
      break; // will be kernel_panic
  }
}

void vprintf(const char* fmt, va_list vargs) {
  if (fmt == NULL) {
    return;
  }

  for (const char* p = fmt; *p; ++p) {
    char c = *p;
    if (is_escape_sequence(c)) {
      apply_escape(c);
    } else if (c == '%') {
      ++p;
      c = *p;
      apply_format(c, vargs);
    } else {
      putchar(c);
    }
  }
}


void printf(const char* fmt, ...) {
  va_list vargs;
  va_start(vargs, fmt);
  vprintf(fmt, vargs);
  va_end(vargs);
}