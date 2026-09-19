extern void endless_loop(void);

void kernel_entry(void) {
    *((short*)0xB8000) = 0;
    endless_loop();
}