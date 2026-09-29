/**
  ******************************************************************************
  * @file		alxSpiFake.c
  * @brief		Auralix C Library - ALX SPI Module - PC Unit Test Fake
  * @copyright	Copyright (C) Auralix d.o.o. All rights reserved.
  *
  * A link-time fake of the SPI master, named after the module it fakes, for a
  * consumer that constructs a bus but does not transfer on it on the host. It
  * satisfies the linker for the constructor and nothing else; a consumer that
  * starts transferring gives this fake its model. The constructor exists on
  * an MCU family only and is defined under the module header's own guard.
  ******************************************************************************
  **/

//******************************************************************************
// Includes
//******************************************************************************
#include "alxSpi.h"


//******************************************************************************
// The faked module's own contract
//******************************************************************************
#if defined(ALX_STM32F4) || defined(ALX_STM32F7) || defined(ALX_STM32G4) || defined(ALX_STM32L0) || defined(ALX_STM32L4)
void AlxSpi_Ctor(AlxSpi* me, SPI_TypeDef* spi, AlxIoPin* do_SCK, AlxIoPin* do_MOSI, AlxIoPin* di_MISO, AlxIoPin* do_nCS, AlxSpi_Mode mode, AlxClk* clk, AlxSpi_Clk spiClk, bool isWriteReadLowLevel)
{
	(void)me; (void)spi; (void)do_SCK; (void)do_MOSI; (void)di_MISO; (void)do_nCS; (void)mode;
	(void)clk; (void)spiClk; (void)isWriteReadLowLevel;
}
#endif
