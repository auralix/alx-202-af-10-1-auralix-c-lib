# Auralix C Library - ALX MCU LPC80x Clock Module
---
## General
- Function "void AlxClk_Irq_Handle(AlxClk* me)" is not implemented
- This module has not been *fully* tested yet
    - The only tested configuration: __AlxClk_Config_McuLpc80x_FroOsc_30MHz_Mainclk_15MHz_CoreSysClk_15MHz = 1__

---
## Ctor Arguments
- __AlxClk* me__ - Pointer to Clk object
- __AlxClk_Config config__ - Clock configuration enum
- __AlxClk_Tick tick__ - Tick time enum
