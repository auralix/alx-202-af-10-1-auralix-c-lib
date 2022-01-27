# Auralix C Library - ALX MCU LPC55S6x Clock Module
---
## General
- Func "void AlxClk_Irq_Handle(AlxClk* me)" is not implemented
- Svinjarijo
    - For AHB clock div register ((AHBCLKDIV, offset = 0x380) User Manual page 78 subtitle __4.5.50 AHB clock divider register__) it is said that reset valuje for DIV = 0x00, but it is 0x01 (meaning SysClk is divided by 2 by default not 1)
    - There are problems when Clk to IOCON is enabled and disabled, that's why there is only enable in "Alx_Status AlxClk_Init(AlxClk* me)" and no disable anywhere else
        - If Clk to IOCON is enabled and disabled it might happen that SEGGER won't recognize the MCU any more and __J-Flash Lite__ has to be used to erase the chip
    - Func "void POWER_SetVoltageForFreq(uint32_t system_freq_hz)" doesnt have the definition. You get build error if you call this function
- Posible Configurations:
    - __AlxClk_Config_McuLpc55S6x_MainClk_12MHz_SysClk_6MHz_FroOsc_12MHz_Default__
        - Internal FRO12M is used for MainClk
    - __AlxClk_Config_McuLpc55S6x_MainClk_96MHz_SysClk_96MHz_FroOsc_96MHz__
        - Internal FROHF is used for MainClk
    - __AlxClk_Config_McuLpc55S6x_MainClk_150MHz_SysClk_150MHz_FroOsc_12MHz_Pll0__
        - Internal FRO12M is used for PLL and then PLL is used for MainClk
    - __AlxClk_Config_McuLpc55S6x_MainClk_150MHz_SysClk_150MHz_ExtOsc_16MHz__
        - This example uses 16MHz crystal that is on LPC55S69-EVK board
        - External 16MHz crystal is used for PLL and then PLL is used for MainClk
- Private Func "void AlxClk_SetupPll()" is written based on information in User Manual page 112 subtitle __4.6.6.3.2 Selecting the bandwidth__
- When using frequencies to flash that are above around 100MHz, "void CLOCK_SetFLASHAccessCyclesForFreq(uint32_t iFreq)" func has to be called when Init clk
- In cases where PLL is used N, P and M are hardcoded to get the desired Clk

---
## Ctor Arguments
- __AlxClk* me__ - Pointer to Clk object
- __AlxClk_Config config__ - Clock configuration enum
- __AlxClk_Tick tick__ - Tick time enum
