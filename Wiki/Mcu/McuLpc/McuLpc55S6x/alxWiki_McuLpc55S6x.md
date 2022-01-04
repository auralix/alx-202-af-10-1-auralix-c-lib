# Auralix C Library - ALX MCU LPC55S6x
---
- C Library for MCU LPC55S6x
- __FLS problems__:
    - FLS has defined 15 IOCON_FUNC, in User Manual, there are only 11 (page 344)
    - FLS's INPUTMUX base address is 0x40006000u in User Manual it is 0x50006000u (page 368)
    - If functions "CLOCK_EnableClock(kCLOCK_Iocon);" and "CLOCK_DisableClock(kCLOCK_Iocon);" are called one after another, nothing works anymore, thats why there is only "CLOCK_EnableClock(kCLOCK_Iocon);" in alxClk module, and there is no disabling of kCLOCK_Iocon anywhere else 
- This Library has OPTIMIZE SIZE option on the following modules:
    - Adc
    - Pwm