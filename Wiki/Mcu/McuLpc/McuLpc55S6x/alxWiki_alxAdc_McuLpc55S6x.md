# Auralix C Library - ALX MCU LPC55S6x ADC Module
---
## General
- This module has OPTIMIZE SIZE option
    - When optimization enabled:
        - Ctor - VRef is in millivolts unsigned
        - Function "float AlxAdc_GetVoltage_V(AlxAdc* me, Alx_Ch ch);" triggers Assert and must not be used
    - When optimization disabled:
        - Ctor - VRef is in volts float
        - Function "uint32_t AlxAdc_GetVoltage_mV(AlxAdc* me, Alx_Ch ch);" triggers Assert and must not be used
- The module is implemented for **Single ended A** mode conversiony only
    - If B mode channel is used, assert is triggered
- Only one of the TCTRLa registers is actively controlling ADC conversions (User Manual page 776), that's why only TCTRL0 and CMD1 are used regardless of ACH channels used, otherwise it doesn't work.
    - Note that there is no CMD0, there are only CMD1-15, although in Fls indexes are from 0-14

---
## Ctor Arguments
- __AlxAdc* me__ - Pointer to Adc object
- __AlxIoPin** ioPinArr__ - Pointer to array of IoPin pointers
- __Alx_Ch* chArr__ - Pointer to array of Channels
- __uint8_t numOfIoPinsAndCh__ - Number of IoPins/Channels used for Adc
- __AlxClk* clk__ - Pointer to Clk object
- __uint32_t vRef_mV__ - Reference voltage in millivolts when OPTIMIZE SIZE __enabled__
- __float vRef_V__  - Reference voltage in volts when OPTIMIZE SIZE __disabled__
