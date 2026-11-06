#include "types.h"
#include <stdbool.h>


#define MASTER_COMMAND_PORT 0x20
#define SLAVE_COMMAND_PORT 0xA0
#define MASTER_DATA_PORT 0x21
#define SLAVE_DATA_PORT 0xA1
#define MASTER_PIC_VECTORS_OFFSET 0x20u
#define SLAVE_PIC_VECTORS_OFFSET 0x28u

extern void delay();
extern void write_to_port(u16 port, u8 value);
extern u8 read_from_port(u16 port);


void pic8259_setup(bool auto_eoi) {
  u8 eoi_mode = auto_eoi ? 0x03 : 0x01;
  u8 icw1 = 0b10001;

  write_to_port(MASTER_COMMAND_PORT, icw1);
  write_to_port(SLAVE_COMMAND_PORT, icw1);

  delay();
  write_to_port(MASTER_DATA_PORT, MASTER_PIC_VECTORS_OFFSET); // Master PIC vector offset
  write_to_port(SLAVE_DATA_PORT, SLAVE_PIC_VECTORS_OFFSET); // Slave PIC vector offset

  delay();
  write_to_port(MASTER_DATA_PORT, 0b00000100); // Tell Master PIC that there is a slave PIC at IRQ2 (0000 0100)
  write_to_port(SLAVE_DATA_PORT, 0b00000010); // Tell Slave PIC its cascade identity (0000 0010 = 2)

  delay();
  write_to_port(MASTER_DATA_PORT, eoi_mode); // Set EOI mode for Master PIC
  write_to_port(SLAVE_DATA_PORT, eoi_mode); // Set EOI mode for Slave PIC

  delay();
  write_to_port(MASTER_DATA_PORT, 0xFF); // Mask all interrupts off
  write_to_port(SLAVE_DATA_PORT, 0xFF); // Mask all interrupts off
}

void pic8259_enable_device(u8 irq) {
  u16 port;
  if (irq < 8) {
    port = MASTER_DATA_PORT; // Master PIC
  } else {
    port = SLAVE_DATA_PORT; // Slave PIC
    irq -= 8;
  }

  u8 mask = read_from_port(port);
  mask &= ~(1 << irq);
  write_to_port(port, mask);
} 

void pic8259_disable_device(u8 irq) {
  u16 port;
  if (irq < 8) {
    port = MASTER_DATA_PORT; // Master PIC
  } else {
    port = SLAVE_DATA_PORT; // Slave PIC
    irq -= 8;
  }

  u8 mask = read_from_port(port);
  mask |= (1 << irq);
  write_to_port(port, mask);
}

void eoi(u8 irq) {
  if (irq >= 8) {
    write_to_port(SLAVE_COMMAND_PORT, 0x20); // Send EOI to Slave PIC
  }
  write_to_port(MASTER_COMMAND_PORT, 0x20); // Send EOI to Master PIC
}