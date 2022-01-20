/**
  ******************************************************************************
  * @file alxSpi_McuLpc55S6x.c
  * @brief Auralix C Library - ALX SPI Module
  * @version $LastChangedRevision: 5584 $
  * @date $LastChangedDate: 2021-07-13 17:22:12 +0200 (Tue, 13 Jul 2021) $
  ******************************************************************************
  */

//******************************************************************************
// Includes
//******************************************************************************
#include "alxSpi_McuLpc55S6x.h"
#include "alxSpi.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_LPC55S6X)


//******************************************************************************
// Private Functions
//******************************************************************************
static Alx_Status AlxSpi_MasterTransferBlocking(AlxSpi* me, uint8_t numOfTries);
static bool AlxSpi_Ctor_IsClkOk(AlxSpi* me);
static void AlxSpi_Ctor_ParseMode(AlxSpi* me);
static uint32_t AlxSpi_GetFlexCommClkFreq(AlxSpi* me);
static Alx_Status AlxSpi_Reset(AlxSpi* me);
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
	spi_ssel_t nCSSel,
	AlxClk* clk,
	AlxSpi_Clk spiClk
)
{
	//Assert
	(void)me;
	ALX_SPI_ASSERT((spi == SPI0) || (spi == SPI1) || (spi == SPI2) || (spi == SPI3) || (spi == SPI4) || (spi == SPI5) || (spi == SPI6) || (spi == SPI7));
	(void)do_SCK;
	(void)do_MOSI;
	(void)di_MISO;
	(void)do_nCS;
	ALX_SPI_ASSERT((mode == AlxSpi_Mode_0) || (mode == AlxSpi_Mode_1) || (mode == AlxSpi_Mode_2) || (mode == AlxSpi_Mode_3));
	ALX_SPI_ASSERT((nCSSel == kSPI_Ssel0) || (nCSSel == kSPI_Ssel1) || (nCSSel == kSPI_Ssel2) || (nCSSel == kSPI_Ssel3));
	(void)clk;
	//todo

	// Objects - External
	me->spi = spi;
	me->do_SCK = do_SCK;
	me->do_MOSI = do_MOSI;
	me->di_MISO = di_MISO;
	me->do_nCS = do_nCS;
	me->clk = clk;

	// Parameters
	me->mode = mode;
	me->nCSSel = nCSSel;
	me->spiClk = spiClk;

	// Check if Clk is OK
	ALX_SPI_ASSERT(AlxSpi_Ctor_IsClkOk(me));

	// Variables																	// MF: Everything is set to default (see "void SPI_MasterGetDefaultConfig()" function) except "me->spiMasterConfig.sselNum" and "me->spiMasterConfig.baudRate_Bps"
	me->spiMasterConfig.enableLoopback				= false;
	me->spiMasterConfig.enableMaster				= true;
	me->spiMasterConfig.polarity					= kSPI_ClockPolarityActiveHigh;	// MF: Is set in "AlxSpi_Ctor_ParseMode()"
	me->spiMasterConfig.phase						= kSPI_ClockPhaseFirstEdge;		// MF: Is set in "AlxSpi_Ctor_ParseMode()"
	me->spiMasterConfig.direction					= kSPI_MsbFirst;
	me->spiMasterConfig.baudRate_Bps				= (uint32_t)spiClk;
	me->spiMasterConfig.dataWidth					= kSPI_Data8Bits;
	me->spiMasterConfig.sselNum						= nCSSel;
	me->spiMasterConfig.sselPol						= kSPI_SpolActiveAllLow;
	me->spiMasterConfig.txWatermark					= (uint8_t)kSPI_TxFifo0;
	me->spiMasterConfig.rxWatermark					= (uint8_t)kSPI_RxFifo1;
	me->spiMasterConfig.delayConfig.preDelay		= 0U;
	me->spiMasterConfig.delayConfig.postDelay		= 0U;
	me->spiMasterConfig.delayConfig.frameDelay		= 0U;
	me->spiMasterConfig.delayConfig.transferDelay	= 0U;

	AlxSpi_Ctor_ParseMode(me);

	me->spiTransfer.txData = ALX_NULL_PTR;
	me->spiTransfer.rxData = ALX_NULL_PTR;
	me->spiTransfer.configFlags = kSPI_FrameAssert;
	me->spiTransfer.dataSize = 0;

	// Info
	me->isInit = false;
	me->wasCtorCalled = true;
}


//******************************************************************************
// Functions
//******************************************************************************
Alx_Status AlxSpi_Init(AlxSpi* me)
{
	// Assert
	ALX_SPI_ASSERT(me->isInit == false);
	ALX_SPI_ASSERT(me->wasCtorCalled == true);

	// #1 Init GPIO
	AlxIoPin_Init(me->do_SCK);
	AlxIoPin_Init(me->do_MOSI);
	AlxIoPin_Init(me->di_MISO);
	AlxIoPin_Init(me->do_nCS);

	// #2 Release SPI Periphery Reset
	AlxSpi_Periph_Reset(me);

	// #3 Enable SPI Periphery Clock
	AlxSpi_Periph_AttachClk(me);

	// #4 Init SPI
	if (SPI_MasterInit(me->spi, &me->spiMasterConfig, AlxSpi_GetFlexCommClkFreq(me)) != kStatus_Success)	// MF: FlexComm "EnableClk" and "Periph reset" happens here
	{
		ALX_SPI_TRACE("ErrInit");
		return Alx_Err;
	}

	// #5 Set isInit
	me->isInit = true;

	// #6 Return OK
	return Alx_Ok;
}
Alx_Status AlxSpi_DeInit(AlxSpi* me)
{
	// Assert
	ALX_SPI_ASSERT(me->isInit == true);
	ALX_SPI_ASSERT(me->wasCtorCalled == true);

	// #1 DeInit SPI
	SPI_Deinit(me->spi);

	// #2 Disable SPI Periphery Clock
	AlxSpi_Periph_DisableClk(me);

	// #3 DeInit GPIO
	AlxIoPin_DeInit(me->do_SCK);
	AlxIoPin_DeInit(me->do_MOSI);
	AlxIoPin_DeInit(me->di_MISO);
	AlxIoPin_DeInit(me->do_nCS);

	// #4 Reset isInit
	me->isInit = false;

	// #5 Return OK
	return Alx_Ok;
}
Alx_Status AlxSpi_Master_Write(AlxSpi* me, uint8_t* writeData, uint16_t len, uint8_t numOfTries, uint16_t timeout_ms)
{
	// Assert
	ALX_SPI_ASSERT(me->isInit == true);
	ALX_SPI_ASSERT(me->wasCtorCalled == true);
	(void)me;
	(void)writeData;
	ALX_SPI_ASSERT(0 < len);
	ALX_SPI_ASSERT(0 < numOfTries);
	ALX_SPI_ASSERT(0 == timeout_ms);	// MF: Timeout won't be used

	// #1 Prepare variables
	uint8_t dummy[len];
	me->spiTransfer.txData = writeData;
	me->spiTransfer.rxData = dummy;		// MF: We dont need received data
	me->spiTransfer.dataSize = len;
	Alx_Status status = Alx_Err;

	// #2 Try SPI write/read
	status = AlxSpi_MasterTransferBlocking(me, numOfTries);

	// #3 If we are here, SPI write/read was OK or number of tries error occured
	if (status == kStatus_Success)
	{
		return Alx_Ok;
	}
	else
	{
		ALX_SPI_TRACE("ErrNumOfTries");
		return Alx_ErrNumOfTries;
	}
}
Alx_Status AlxSpi_Master_Read(AlxSpi* me, uint8_t* readData, uint16_t len, uint8_t numOfTries, uint16_t timeout_ms)
{
	// Assert
	ALX_SPI_ASSERT(me->isInit == true);
	ALX_SPI_ASSERT(me->wasCtorCalled == true);
	(void)me;
	(void)readData;
	ALX_SPI_ASSERT(0 < len);
	ALX_SPI_ASSERT(0 < numOfTries);
	ALX_SPI_ASSERT(0 == timeout_ms);	// MF: Timeout won't be used

	// #1 Prepare variables
	uint8_t dummy[len];
	me->spiTransfer.txData = dummy;		// MF: We dont need transmited data
	me->spiTransfer.rxData = readData;
	me->spiTransfer.dataSize = len;
	Alx_Status status = Alx_Err;

	// #2 Try SPI write/read
	status = AlxSpi_MasterTransferBlocking(me, numOfTries);

	// #3 If we are here, SPI write/read was OK or number of tries error occured
	if (status == kStatus_Success)
	{
		return Alx_Ok;
	}
	else
	{
		ALX_SPI_TRACE("ErrNumOfTries");
		return Alx_ErrNumOfTries;
	}
}
Alx_Status AlxSpi_Master_WriteRead(AlxSpi* me, uint8_t* writeData, uint8_t* readData, uint16_t len, uint8_t numOfTries, uint16_t timeout_ms)
{
	// Assert
	ALX_SPI_ASSERT(me->isInit == true);
	ALX_SPI_ASSERT(me->wasCtorCalled == true);
	(void)me;
	(void)writeData;
	(void)readData;
	ALX_SPI_ASSERT(0 < len);
	ALX_SPI_ASSERT(0 < numOfTries);
	ALX_SPI_ASSERT(0 == timeout_ms);	// MF: Timeout won't be used

	// #1 Prepare variables
	me->spiTransfer.txData = writeData;
	me->spiTransfer.rxData = readData;
	me->spiTransfer.dataSize = len;
	Alx_Status status = Alx_Err;

	// #2 Try SPI write/read
	status = AlxSpi_MasterTransferBlocking(me, numOfTries);

	// #3 If we are here, SPI write/read was OK or number of tries error occured
	if (status == kStatus_Success)
	{
		return Alx_Ok;
	}
	else
	{
		ALX_SPI_TRACE("ErrNumOfTries");
		return Alx_ErrNumOfTries;
	}
}
void AlxSpi_Master_AssertCs(AlxSpi* me)
{
	AlxIoPin_Reset(me->do_nCS);
}
void AlxSpi_Master_DeAssertCs(AlxSpi* me)
{
	AlxIoPin_Set(me->do_nCS);
}


//******************************************************************************
// Private Functions
//******************************************************************************
static Alx_Status AlxSpi_MasterTransferBlocking(AlxSpi* me, uint8_t numOfTries)
{
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
	if (status == kStatus_Success)
	{
		return Alx_Ok;
	}
	else
	{
		ALX_SPI_TRACE("ErrNumOfTries");
		return Alx_ErrNumOfTries;
	}
}
static bool AlxSpi_Ctor_IsClkOk(AlxSpi* me)
{
	// #1 Check that right Spi SCK is used
	if (me->clk->config == AlxClk_Config_McuLpc55S6x_MainClk_12MHz_SysClk_6MHz_FroOsc_12MHz_Default &&	// MF: When MainClk Freq is 12MHz, max Spi SCK is 6MHz
		me->spiClk > AlxSpi_Clk_McuLpc55S6x_SpiClk_6MHz)	{ return false; }
	else													{ return true; }

	ALX_SPI_ASSERT(false); // We shouldn't get here
	return ALX_NULL;
}
static void AlxSpi_Ctor_ParseMode(AlxSpi* me)
{
	// #1 Parse Mode
	if (me->mode == AlxSpi_Mode_0)	{ me->spiMasterConfig.polarity = kSPI_ClockPolarityActiveLow;  me->spiMasterConfig.phase = kSPI_ClockPhaseFirstEdge;  return;}
	if (me->mode == AlxSpi_Mode_1)	{ me->spiMasterConfig.polarity = kSPI_ClockPolarityActiveLow;  me->spiMasterConfig.phase = kSPI_ClockPhaseSecondEdge; return;}
	if (me->mode == AlxSpi_Mode_2)	{ me->spiMasterConfig.polarity = kSPI_ClockPolarityActiveHigh; me->spiMasterConfig.phase = kSPI_ClockPhaseFirstEdge;  return;}
	if (me->mode == AlxSpi_Mode_3)	{ me->spiMasterConfig.polarity = kSPI_ClockPolarityActiveHigh; me->spiMasterConfig.phase = kSPI_ClockPhaseSecondEdge; return;}

	//Assert
	ALX_SPI_ASSERT(false); // We shouldn't get here
	return;
}
static uint32_t AlxSpi_GetFlexCommClkFreq(AlxSpi* me)
{
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
static Alx_Status AlxSpi_Reset(AlxSpi* me)
{
	// #1 DeInit SPI
	SPI_Deinit(me->spi);

	// #2 Reset isInit
	me->isInit = false;

	// #3 Release SPI Periphery Reset
	AlxSpi_Periph_Reset(me);

	// #4 Init SPI
	if (SPI_MasterInit(me->spi, &me->spiMasterConfig, AlxSpi_GetFlexCommClkFreq(me)) != kStatus_Success)
	{
		ALX_SPI_TRACE("ErrInit");
		return Alx_Err;
	}

	// #5 Set isInit
	me->isInit = true;

	// #6 Return OK
	return Alx_Ok;
}
static void AlxSpi_Periph_Reset(AlxSpi* me)
{
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


#endif
