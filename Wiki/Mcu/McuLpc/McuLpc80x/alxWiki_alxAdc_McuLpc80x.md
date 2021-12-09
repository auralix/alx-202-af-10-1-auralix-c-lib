# Auralix C Library - ALX MCU LPC80x ADC Module
---
## General
- This module has OPTIMIZE SIZE option
    - When optimization enabled:
        - Ctor - VRef is in millivolts unsigned
        - Function "float AlxAdc_GetVoltage_V(AlxAdc* me, Alx_Ch ch);" triggers Assert and must not be used
    - When optimization disabled:
        - Ctor - VRef is in volts float
        - Function "uint32_t AlxAdc_GetVoltage_mV(AlxAdc* me, Alx_Ch ch);" triggers Assert and must not be used
- TempSens function is not supported on this MCU
---
## Ctor Arguments
- __AlxAdc* me__ - Adc object
- __AlxIoPin** ioPinArr__ - Pointer to array of IoPin pointers
- __Alx_Ch* chArr__ - Pointer to array of Channels
- __uint8_t numOfIoPinsAndCh__ - Number of channels used for Adc
- __AlxClk* clk__ - Clk obejct
- __uint32_t vRef_mV__ - Reference voltage in millivolts when OPTIMIZE SIZE __enabled__
- __float vRef_V__  - Reference voltage in volts when OPTIMIZE SIZE __disabled__
