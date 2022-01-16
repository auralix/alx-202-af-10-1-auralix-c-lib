# Auralix C Library - ALX MCU LPC55S6x Clock Module
---
## General
- Function "void AlxClk_GetClk_Hz(AlxClk* me, AlxClk_Clk clk)" is not implemented
- Function "void AlxClk_Irq_Handle(AlxClk* me)" is not implemented
- This module *has not been* finished yet
- Svinjarijo
    - za AHB clock divider register (AHBCLKDIV, offset = 0x380) page 79 pravijo da je reset valuje za DIV = 0X00, je pa 0x01

---
## Ctor Arguments
- __AlxClk* me__ - Pointer to Clk object
- __AlxClk_Config config__ - Clock configuration enum
- __AlxClk_Tick tick__ - Tick time enum
