# Auralix C Library - ALX MCU LPC80x Clock Module
---
## General
- Function "void AlxClk_Irq_Handle(AlxClk* me)" is not implemented
- This module has not been *fully* tested yet
    - The only tested configuration: __AlxClk_Config_McuLpc80x_FroOsc_24MHz_Mainclk_12MHz_CoreSysClk_12MHz_Default = 0__

---
## Ctor Arguments
- __AlxClk* me__ - Pointer to Clk object
- __AlxClk_Config config__ - Clock configuration enum
- __AlxClk_Tick tick__ - Tick time enum
