# Auralix C Library - ALX MCU LPC55S6x IO Pin Irq Module
---
## General
- Weak functions have to be implemented to handle Irq's otherwise assert is triggered
- Only Digital mode has to be enabled in IOCON for IRQ to work, everything else is default

---
## Ctor Arguments
- __AlxIoPinIrq* me__ - Pointer to IoPinIrq object
- __AlxIoPin* ioPin__ - Pointer to IoPin object
- __pint_pin_int_t irqPin__ - Which of 8 Irq's is choosen
- __pint_pin_enable_t irqType__ - Which signal type triggers Irq
- __Alx_IrqPriority irqPriority__ - Irq priority
