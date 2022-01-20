# Auralix C Library - ALX MCU LPC55S6x
---
- C Library for MCU LPC55S6x
- __FLS problems__:
    - FLS has defined 15 IOCON_FUNC, in User Manual, there are only 11 (User Manual page 343 subtitle __15.5.4 IOCON pin functions in relation to FUNC values__)
    - FLS's INPUTMUX base address is 0x40006000u in User Manual it is 0x50006000u (User Manual page 368 table __371__)
    - If functions "CLOCK_EnableClock(kCLOCK_Iocon);" and "CLOCK_DisableClock(kCLOCK_Iocon);" are called one after another with some code in between, nothing works anymore, thats why there is only "CLOCK_EnableClock(kCLOCK_Iocon);" in alxClk module, and there is no disabling of kCLOCK_Iocon anywhere else 
- This Library has OPTIMIZE SIZE option on the following modules:
    - Adc
    - Pwm