/**
  ******************************************************************************
  * @file		alxSpi_McuLpc55S6x.c
  * @brief		Auralix C Library - ALX SPI MCU LPC55S6X Module
  * @copyright	Copyright (C) 2020-2022 Auralix d.o.o. All rights reserved.
  *
  * @section License
  *
  * SPDX-License-Identifier: GPL-3.0-or-later
  *
  * This file is part of Auralix C Library.
  *
  * Auralix C Library is free software: you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation, either version 3
  * of the License, or (at your option) any later version.
  *
  * Auralix C Library is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  * GNU General Public License for more details.
  *
  * You should have received a copy of the GNU General Public License
  * along with Auralix C Library. If not, see <https://www.gnu.org/licenses/>.
  ******************************************************************************
  **/

//******************************************************************************
// Includes
//******************************************************************************
#include "alxSpi_McuLpc55S6x.h"
#include "alxSpi.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB) && defined(ALX_LPC55S6X)


//******************************************************************************
// Private Functions
//******************************************************************************
static Alx_Status AlxSpi_Reset(AlxSpi* me);
static Alx_Status AlxSpi_MasterTransferBlocking(AlxSpi* me, uint8_t numOfTries);
static void AlxSpi_Ctor_ParseMode(AlxSpi* me);
static uint32_t AlxSpi_GetFlexCommClkFreq(AlxSpi* me);
static void AlxSpi_Periph_Reset(AlxSpi* me);
static void AlxSpi_Periph_AttachClk(AlxSpi* me);
static void AlxSpi_Periph_DisableClk(AlxSpi* me);


//******************************************************************************
// Constructor
//******************************************************************************
void AlxSpi_Ctor
(
	AlxSpi* me,
	SPI_Type* spi,
	AlxIoPin* do_SCK,
	AlxIoPin* do_MOSI,
	AlxIoPin* di_MISO,
	AlxIoPin* do_nCS,
	AlxSpi_Mode mode,
	AlxSpi_Clk spiClk
)
{
	//Assert
	(void)me;
	ALX_SPI_ASSERT(!(spi == SPI8));	// SPI8 should not be used
	(void)do_SCK;
	(void)do_MOSI;
	(void)di_MISO;
	(void)do_nCS;
	(void)mode;
	(void)spiClk;

	// Objects - External
	me->spi = spi;
	me->do_SCK = do_SCK;
	me->do_MOSI = do_MOSI;
	me->di_MISO = di_MISO;
	me->do_nCS = do_nCS;

	// Parameters
	me->mode = mode;
	me->spiClk = spiClk;

	// Variables																	// MF: Everything is set to default (see "void SPI_MasterGetDefaultConfig()" function) except "me->spiMasterConfig.baudRate_Bps"
	me->spiMasterConfig.enableLoopback				= false;
	me->spiMasterConfig.enableMaster				= true;
	me->spiMasterConfig.polarity					= kSPI_ClockPolarityActiveHigh;	// MF: Is set in "AlxSpi_Ctor_ParseMode()"
	me->spiMasterConfig.phase						= kSPI_ClockPhaseFirstEdge;		// MF: Is set in "AlxSpi_Ctor_ParseMode()"
	me->spiMasterConfig.direction					= kSPI_MsbFirst;
	me->spiMasterConfig.baudRate_Bps				= (uint32_t)spiClk;
	me->spiMasterConfig.dataWidth					= kSPI_Data8Bits;
	me->spiMasterConfig.sselNum						= kSPI_Ssel0;
	me->spiMasterConfig.sselPol						= kSPI_SpolActiveAllLow;
	me->spiMasterConfig.txWatermark					= (uint8_t)kSPI_TxFifo0;
	me->spiMasterConfig.rxWatermark					= (uint8_t)kSPI_RxFifo1;
	me->spiMasterConfig.delayConfig.preDelay		= 0U;
	me->spiMasterConfig.delayConfig.postDelay		= 0U;
	me->spiMasterConfig.delayConfig.frameDelay		= 0U;
	me->spiMasterConfig.delayConfig.transferDelay	= 0U;

	AlxSpi_Ctor_ParseMode(me);

	me->spiTransfer.txData			= ALX_NULL_PTR;
	me->spiTransfer.rxData			= ALX_NULL_PTR;
	me->spiTransfer.configFlags		= kSPI_FrameAssert;
	me->spiTransfer.dataSize		= 0;

	// Info
	me->isInit = false;
	me->wasCtorCalled = true;
}


//******************************************************************************
// Functions
//******************************************************************************
Alx_Status AlxSpi_Init(AlxSpi* me)
{
	// #1 Assert
	ALX_SPI_ASSERT(me->isInit == false);
	ALX_SPI_ASSERT(me->wasCtorCalled == true);
	(void)me;

	// #2 Init GPIO
	AlxIoPin_Init(me->do_SCK);
	AlxIoPin_Init(me->do_MOSI);
	AlxIoPin_Init(me->di_MISO);
	AlxIoPin_Init(me->do_nCS);

	// #3 Enable SPI Periphery Clock
	AlxSpi_Periph_AttachClk(me);

	// #4 Init SPI
	if (SPI_MasterInit(me->spi, &me->spiMasterConfig, AlxSpi_GetFlexCommClkFreq(me)) != kStatus_Success)	{ ALX_SPI_TRACE("ErrInit"); return Alx_Err; }	// MF: FlexComm "EnableClk" and "Periph reset" happens here

	// #5 Set isInit
	me->isInit = true;

	// #6 Return OK
	return Alx_Ok;
}
Alx_Status AlxSpi_DeInit(AlxSpi* me)
{
	// #1 Assert
	ALX_SPI_ASSERT(me->isInit == true);
	ALX_SPI_ASSERT(me->wasCtorCalled == true);
	(void)me;

	// #2 DeInit SPI
	SPI_Deinit(me->spi);

	// #3 Disable SPI Periphery Clock
	AlxSpi_Periph_DisableClk(me);

	// #4 DeInit GPIO
	AlxIoPin_DeInit(me->do_SCK);
	AlxIoPin_DeInit(me->do_MOSI);
	AlxIoPin_DeInit(me->di_MISO);
	AlxIoPin_DeInit(me->do_nCS);

	// #5 Reset isInit
	me->isInit = false;

	// #6 Return OK
	return Alx_Ok;
}
Alx_Status AlxSpi_Master_Write(AlxSpi* me, uint8_t* writeData, uint16_t len, uint8_t numOfTries, uint16_t timeout_ms)
{
	// #1 Assert
	ALX_SPI_ASSERT(me->isInit == true);
	ALX_SPI_ASSERT(me->wasCtorCalled == true);
	(void)me;
	(void)writeData;
	ALX_SPI_ASSERT(0 < len);
	ALX_SPI_ASSERT(0 < numOfTries);
	ALX_SPI_ASSERT(0 == timeout_ms);	// MF: Timeout won't be used

	// #2 Prepare variables
	uint8_t* dummy = NULL;
	me->spiTransfer.txData = writeData;
	me->spiTransfer.rxData = dummy;		// MF: We dont need received data
	me->spiTransfer.dataSize = len;
	Alx_Status status = Alx_Err;

	// #3 Try SPI write/read
	status = AlxSpi_MasterTransferBlocking(me, numOfTries);

	// #4 If we are here, SPI write/read was OK or number of tries error occured
	if (status == Alx_Ok)	{ return Alx_Ok; }
	else					{ ALX_SPI_TRACE("ErrNumOfTries"); return Alx_ErrNumOfTries; }
}
Alx_Status AlxSpi_Master_Read(AlxSpi* me, uint8_t* readData, uint16_t len, uint8_t numOfTries, uint16_t timeout_ms)
{
	// #1 Assert
	ALX_SPI_ASSERT(me->isInit == true);
	ALX_SPI_ASSERT(me->wasCtorCalled == true);
	(void)me;
	(void)readData;
	ALX_SPI_ASSERT(0 < len);
	ALX_SPI_ASSERT(0 < numOfTries);
	ALX_SPI_ASSERT(0 == timeout_ms);	// MF: Timeout won't be used

	// #2 Prepare variables
	uint8_t* dummy = NULL;
	me->spiTransfer.txData = dummy;		// MF: We dont need transmited data
	me->spiTransfer.rxData = readData;
	me->spiTransfer.dataSize = len;
	Alx_Status status = Alx_Err;

	// #3 Try SPI write/read
	status = AlxSpi_MasterTransferBlocking(me, numOfTries);

	// #4 If we are here, SPI write/read was OK or number of tries error occured
	if (status == Alx_Ok)	{ return Alx_Ok; }
	else					{ ALX_SPI_TRACE("ErrNumOfTries"); return Alx_ErrNumOfTries; }
}
Alx_Status AlxSpi_Master_WriteRead(AlxSpi* me, uint8_t* writeData, uint8_t* readData, uint16_t len, uint8_t numOfTries, uint16_t timeout_ms)
{
	// #1 Assert
	ALX_SPI_ASSERT(me->isInit == true);
	ALX_SPI_ASSERT(me->wasCtorCalled == true);
	(void)me;
	(void)writeData;
	(void)readData;
	ALX_SPI_ASSERT(0 < len);
	ALX_SPI_ASSERT(0 < numOfTries);
	ALX_SPI_ASSERT(0 == timeout_ms);	// MF: Timeout won't be used

	// #2 Prepare variables
	me->spiTransfer.txData = writeData;
	me->spiTransfer.rxData = readData;
	me->spiTransfer.dataSize = len;
	Alx_Status status = Alx_Err;

	// #3 Try SPI write/read
	status = AlxSpi_MasterTransferBlocking(me, numOfTries);

	// #4 If we are here, SPI write/read was OK or number of tries error occured
	if (status == Alx_Ok)	{ return Alx_Ok; }
	else					{ ALX_SPI_TRACE("ErrNumOfTries"); return Alx_ErrNumOfTries; }
}
void AlxSpi_Master_AssertCs(AlxSpi* me)
{
	// #1 Assert
	ALX_SPI_ASSERT(me->isInit == true);
	ALX_SPI_ASSERT(me->wasCtorCalled == true);
	(void)me;

	// #2 Assert nCS
	AlxIoPin_Reset(me->do_nCS);
}
void AlxSpi_Master_DeAssertCs(AlxSpi* me)
{
	// #1 Assert
	ALX_SPI_ASSERT(me->isInit == true);
	ALX_SPI_ASSERT(me->wasCtorCalled == true);
	(void)me;

	// #2 DeAssert nCS
	AlxIoPin_Set(me->do_nCS);
}


//******************************************************************************
// Private Functions
//******************************************************************************
static Alx_Status AlxSpi_Reset(AlxSpi* me)
{
	// #1 DeInit
	// #1.1 DeInit SPI
	SPI_Deinit(me->spi);

	// #1.2 Disable SPI Periphery Clock
	AlxSpi_Periph_DisableClk(me);

	// #1.3 DeInit GPIO
	AlxIoPin_DeInit(me->do_SCK);
	AlxIoPin_DeInit(me->do_MOSI);
	AlxIoPin_DeInit(me->di_MISO);
	AlxIoPin_DeInit(me->do_nCS);

	// #2 Reset isInit
	me->isInit = false;

	// #3 Init
	// #3.1 Init GPIO
	AlxIoPin_Init(me->do_SCK);
	AlxIoPin_Init(me->do_MOSI);
	AlxIoPin_Init(me->di_MISO);
	AlxIoPin_Init(me->do_nCS);

	// #3.2 Enable SPI Periphery Clock
	AlxSpi_Periph_AttachClk(me);

	// #3.3 Init SPI
	if (SPI_MasterInit(me->spi, &me->spiMasterConfig, AlxSpi_GetFlexCommClkFreq(me)) != kStatus_Success)	{ ALX_SPI_TRACE("ErrInit"); return Alx_Err; }	// MF: FlexComm "EnableClk" and "Periph reset" happens here

	// #4 Set isInit
	me->isInit = true;

	// #5 Return OK
	return Alx_Ok;
}
static Alx_Status AlxSpi_MasterTransferBlocking(AlxSpi* me, uint8_t numOfTries)
{
	// Assert
	(void)me;
	(void)numOfTries;

	// #1 Prepare variables
	status_t status = kStatus_Fail;

	// #2 Try SPI write/read
	for (uint32_t _try = 1; _try <= numOfTries; _try++)
	{
		status = SPI_MasterTransferBlocking(me->spi, &me->spiTransfer);
		if (status == kStatus_Success)
		{
			break; // SPI write OK
		}
		else
		{
			ALX_SPI_TRACE("ErrFls");
			if (AlxSpi_Reset(me) != Alx_Ok) { ALX_SPI_TRACE("ErrReset"); return Alx_Err;}
			continue;
		}
	}

	// #3 If we are here, SPI write/read was OK or number of tries error occured
	if (status == kStatus_Success)	{ return Alx_Ok; }
	else							{ ALX_SPI_TRACE("ErrNumOfTries"); return Alx_ErrNumOfTries; }
}
static void AlxSpi_Ctor_ParseMode(AlxSpi* me)
{
	// Assert
	(void)me;

	// #1 Parse Mode
	if (me->mode == AlxSpi_Mode_0)	{ me->spiMasterConfig.polarity = kSPI_ClockPolarityActiveHigh; me->spiMasterConfig.phase = kSPI_ClockPhaseFirstEdge; return;}
	if (me->mode == AlxSpi_Mode_1)	{ me->spiMasterConfig.polarity = kSPI_ClockPolarityActiveHigh; me->spiMasterConfig.phase = kSPI_ClockPhaseSecondEdge; return;}
	if (me->mode == AlxSpi_Mode_2)	{ me->spiMasterConfig.polarity = kSPI_ClockPolarityActiveLow; me->spiMasterConfig.phase = kSPI_ClockPhaseFirstEdge; return;}
	if (me->mode == AlxSpi_Mode_3)	{ me->spiMasterConfig.polarity = kSPI_ClockPolarityActiveLow; me->spiMasterConfig.phase = kSPI_ClockPhaseSecondEdge; return;}

	//Assert
	ALX_SPI_ASSERT(false); // We shouldn't get here
	return;
}
static uint32_t AlxSpi_GetFlexCommClkFreq(AlxSpi* me)
{
	// Assert
	(void)me;

	// #1 Get FlexComm Clk Freq
	#if defined(SPI0)
	if (me->spi == SPI0)	{ return CLOCK_GetFlexCommClkFreq(0U); }
	#endif
	#if defined(SPI1)
	if (me->spi == SPI1)	{ return CLOCK_GetFlexCommClkFreq(1U); }
	#endif
	#if defined(SPI2)
	if (me->spi == SPI2)	{ return CLOCK_GetFlexCommClkFreq(2U); }
	#endif
	#if defined(SPI3)
	if (me->spi == SPI3)	{ return CLOCK_GetFlexCommClkFreq(3U); }
	#endif
	#if defined(SPI4)
	if (me->spi == SPI4)	{ return CLOCK_GetFlexCommClkFreq(4U); }
	#endif
	#if defined(SPI5)
	if (me->spi == SPI5)	{ return CLOCK_GetFlexCommClkFreq(5U); }
	#endif
	#if defined(SPI6)
	if (me->spi == SPI6)	{ return CLOCK_GetFlexCommClkFreq(6U); }
	#endif
	#if defined(SPI7)
	if (me->spi == SPI7)	{ return CLOCK_GetFlexCommClkFreq(7U); }
	#endif

	//Assert
	ALX_SPI_ASSERT(false); // We shouldn't get here
	return 0xFFFFFFFF;
}
static void AlxSpi_Periph_Reset(AlxSpi* me)
{
	// Assert
	(void)me;

	// #1 Reset FlexComm
	#if defined(SPI0)
	if (me->spi == SPI0)	{ RESET_PeripheralReset(kFC0_RST_SHIFT_RSTn); return; }
	#endif
	#if defined(SPI1)
	if (me->spi == SPI1)	{ RESET_PeripheralReset(kFC1_RST_SHIFT_RSTn); return; }
	#endif
	#if defined(SPI2)
	if (me->spi == SPI2)	{ RESET_PeripheralReset(kFC2_RST_SHIFT_RSTn); return; }
	#endif
	#if defined(SPI3)
	if (me->spi == SPI3)	{ RESET_PeripheralReset(kFC3_RST_SHIFT_RSTn); return; }
	#endif
	#if defined(SPI4)
	if (me->spi == SPI4)	{ RESET_PeripheralReset(kFC4_RST_SHIFT_RSTn); return; }
	#endif
	#if defined(SPI5)
	if (me->spi == SPI5)	{ RESET_PeripheralReset(kFC5_RST_SHIFT_RSTn); return; }
	#endif
	#if defined(SPI6)
	if (me->spi == SPI6)	{ RESET_PeripheralReset(kFC6_RST_SHIFT_RSTn); return; }
	#endif
	#if defined(SPI7)
	if (me->spi == SPI7)	{ RESET_PeripheralReset(kFC7_RST_SHIFT_RSTn); return; }
	#endif

	//Assert
	ALX_SPI_ASSERT(false); // We shouldn't get here
	return;
}
static void AlxSpi_Periph_AttachClk(AlxSpi* me)
{
	// Assert
	(void)me;

	// #1 Attach Clk to FlexComm
	#if defined(SPI0)
	if (me->spi == SPI0)	{ CLOCK_AttachClk(kMAIN_CLK_to_FLEXCOMM0); return; }	// MF: In example there was Clk from FRO12MHz I do not understand which clk is right
	#endif
	#if defined(SPI1)
	if (me->spi == SPI1)	{ CLOCK_AttachClk(kMAIN_CLK_to_FLEXCOMM1); return; }
	#endif
	#if defined(SPI2)
	if (me->spi == SPI2)	{ CLOCK_AttachClk(kMAIN_CLK_to_FLEXCOMM2); return; }
	#endif
	#if defined(SPI3)
	if (me->spi == SPI3)	{ CLOCK_AttachClk(kMAIN_CLK_to_FLEXCOMM3); return; }
	#endif
	#if defined(SPI4)
	if (me->spi == SPI4)	{ CLOCK_AttachClk(kMAIN_CLK_to_FLEXCOMM4); return; }
	#endif
	#if defined(SPI5)
	if (me->spi == SPI5)	{ CLOCK_AttachClk(kMAIN_CLK_to_FLEXCOMM5); return; }
	#endif
	#if defined(SPI6)
	if (me->spi == SPI6)	{ CLOCK_AttachClk(kMAIN_CLK_to_FLEXCOMM6); return; }
	#endif
	#if defined(SPI7)
	if (me->spi == SPI7)	{ CLOCK_AttachClk(kMAIN_CLK_to_FLEXCOMM7); return; }
	#endif

	//Assert
	ALX_SPI_ASSERT(false); // We shouldn't get here
	return;
}
static void AlxSpi_Periph_DisableClk(AlxSpi* me)
{
	// Assert
	(void)me;

	// #1 Disable FlexComm Clk
	#if defined(SPI0)
	if (me->spi == SPI0)	{ CLOCK_DisableClock(kCLOCK_FlexComm0); return; }
	#endif
	#if defined(SPI1)
	if (me->spi == SPI1)	{ CLOCK_DisableClock(kCLOCK_FlexComm1); return; }
	#endif
	#if defined(SPI2)
	if (me->spi == SPI2)	{ CLOCK_DisableClock(kCLOCK_FlexComm2); return; }
	#endif
	#if defined(SPI3)
	if (me->spi == SPI3)	{ CLOCK_DisableClock(kCLOCK_FlexComm3); return; }
	#endif
	#if defined(SPI4)
	if (me->spi == SPI4)	{ CLOCK_DisableClock(kCLOCK_FlexComm4); return; }
	#endif
	#if defined(SPI5)
	if (me->spi == SPI5)	{ CLOCK_DisableClock(kCLOCK_FlexComm5); return; }
	#endif
	#if defined(SPI6)
	if (me->spi == SPI6)	{ CLOCK_DisableClock(kCLOCK_FlexComm6); return; }
	#endif
	#if defined(SPI7)
	if (me->spi == SPI7)	{ CLOCK_DisableClock(kCLOCK_FlexComm7); return; }
	#endif

	//Assert
	ALX_SPI_ASSERT(false); // We shouldn't get here
	return;
}


#endif	// #if defined(ALX_C_LIB) && defined(ALX_LPC55S6X)
