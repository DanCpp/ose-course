extern void endless_loop();

void kernel_entry() {
    *((short*)0xB8000) = 0;
    endless_loop();
}