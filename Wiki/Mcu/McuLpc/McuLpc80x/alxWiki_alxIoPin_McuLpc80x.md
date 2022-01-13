# Auralix C Library - ALX MCU LPC80x IO Pin Module
---
## General
- These functiones are not implemented. Assert is triggered when call any function below.
    - __void AlxIoPin_Lock(AlxIoPin* me)__
    - __void AlxIoPin_IrqHandler(AlxIoPin* me)__
    - __void AlxIoPin_Config_PullNone(AlxIoPin* me)__
    - __void AlxIoPin_Config_PullUp(AlxIoPin* me)__
    - __void AlxIoPin_Config_PullDown(AlxIoPin* me)__
    - __void AlxIoPin_Config_AssertOn(AlxIoPin* me)__
    - __void AlxIoPin_Config_AssertOff(AlxIoPin* me)__

---
## Ctor Arguments
- __AlxIoPin* me__ - Pointer to IoPin object
- __uint8_t port__ - Port
- __uint8_t pin__ - Pin
- __AlxIoPin_Func func__ - SWM  pin function
- __uint32_t mode__ - IOCON mode
- __bool isOpenDrain__ - Open drain flag
- __bool dir__ - Direction when set as GPIO. True is set as output.
- __bool val__ - Initial output value
