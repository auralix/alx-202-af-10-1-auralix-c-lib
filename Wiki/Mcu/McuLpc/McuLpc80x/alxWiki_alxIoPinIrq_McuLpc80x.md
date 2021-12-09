# Auralix C Library - ALX MCU LPC80x IO Pin Irq Module
---
## General
- Weak finctions have to be implemented to handle Irq's otherwise assert is triggered
- TODO
---
## Ctor Arguments
- __AlxIoPinIrq* me__ - Pointer to IoPinIrq objest
- __AlxIoPin* ioPin__ - Pointer to IoPin objest
- __pint_pin_int_t irqPin__ - Which of 8 Irq's is choosen
- __pint_pin_enable_t irqType__ - Which signal type triggers Irq
- __Alx_IrqPriority irqPriority__ - Irq priority
