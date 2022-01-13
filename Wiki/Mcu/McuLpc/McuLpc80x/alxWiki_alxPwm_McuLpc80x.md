# Auralix C Library - ALX MCU LPC80x PWM Module
---
## General
- This module has OPTIMIZE SIZE option
    - When optimization enabled:
        - Ctor - Default duties are in permil unsigned
        - Function "Alx_Status AlxPwm_SetDuty_pct(AlxPwm* me, Alx_Ch ch, float duty_pct)" triggers Assert and must not be used
    - When optimization disabled:
        - Ctor - Default duties are in percent float
        - Function "Alx_Status AlxPwm_SetDuty_permil(AlxPwm* me, Alx_Ch ch, uint16_t duty_permil)" triggers Assert and must not be used

---
## Ctor Arguments
- __AlxPwm* me__ - Pointer to Pwm object
- __CTIMER_Type* tim__ - Ctimer peripheral base pointer
- __AlxIoPin** ioPinArr__ - Pointer to array of IoPin pointers
- __Alx_Ch* chArr__ - Pointer to array of Pwm Channels
- __uint8_t numOfCh__ - Number of channels used for Pwm
- __AlxClk* clk__ - Pointer to Clk object
- __uint16_t* dutyDefaultArr_permil__ - Default duty in permils when OPTIMIZE SIZE __enabled__
- __float* dutyDefaultArr_pct__ - Default duty in percent when OPTIMIZE SIZE __disabled__
- __uint32_t prescaler__ - Must be __0__ otherwise Assert is triggered
- __uint32_t period__ - Frequency in Hz of Pwm
