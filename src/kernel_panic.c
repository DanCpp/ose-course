#include "types.h"
#include "vga_driver.h"

extern void go_cli();
extern void endless_loop();

void vkernel_panic(const char* fmt, va_list args) {
  go_cli();
  printf("Kernel panic: ");
  vprintf(fmt, args);
  endless_loop();
}


void kernel_panic(const char* fmt, ...) {
  go_cli();
  va_list args;
  va_start(args, fmt);
  vkernel_panic(fmt, args);
  va_end(args);
}