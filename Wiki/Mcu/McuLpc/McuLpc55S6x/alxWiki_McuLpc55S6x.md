# Auralix C Library - ALX MCU LPC55S6x
---
- C Library for MCU LPC55S6x
- __FLS problems__:
    - FLS has defined 15 IOCON_FUNC, in User Manual, there are only 11 (page 344)
    - FLS's INPUTMUX base address is 0x40006000u in User Manual it is 0x50006000u (page 368)
- This Library has OPTIMIZE SIZE option on the following modules:
    - Adc
    - Pwm
