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
- AlxAdc* me - Adc object
- AlxIoPin** ioPinArr - Pointer to array of IoPins
- Alx_Ch* chArr - Array of Channels
- uint8_t numOfIoPinsAndCh - Number of channels
- AlxClk* clk - Clk
- uint32_t vRef_mV
- float vRef_V
