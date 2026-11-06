#ifndef PIC_H
#define PIC_H

#include "types.h"
#include <stdbool.h>

void pic8259_setup(bool auto_eoi);

void pic8259_enable_device(u8 irq);

void pic8259_disable_device(u8 irq);

void eoi(u8 irq);

#endif // PIC_H