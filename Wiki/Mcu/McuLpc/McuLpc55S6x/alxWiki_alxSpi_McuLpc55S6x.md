# Auralix C Library - ALX MCU LPC55S6x Spi Module
---
## General
- MainClk is used for FlexComm
- Spi SCK is divided FlexComm Clk, so it can be the same as FlexComm or divided by integer (User Manual Page 707 subtitle __35.7.4 Clocking and date rates__)
- Func "AlxSpi_Ctor_IsClkOk()" only checks that SCK is not too high when MainClk is 12MHz because Init handles the input Freq to Spi

---
## Ctor Arguments
__AlxSpi* me__ - Pointer to Spi object
__SPI_Type* spi__ - Spi peripheral base pointer
__AlxIoPin* do_SCK__ - Pointer to SCK IoPin
__AlxIoPin* do_MOSI__ - Pointer to MOSI IoPin
__AlxIoPin* di_MISO__ - Pointer to MISO IoPin
__AlxIoPin* do_nCS__ - Pointer to nCS IoPin
__AlxSpi_Mode mode__ - 
__spi_ssel_t nCSSel__ - Spi nCS select enum
__AlxClk* clk__ - Pointer to Clk object
__AlxSpi_Clk spiClk__
