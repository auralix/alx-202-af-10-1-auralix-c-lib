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
/*static bool AlxSpi_Ctor_IsClkOk(AlxSpi* me);
static void AlxSpi_Ctor_ParseMode(AlxSpi* me);
static Alx_Status AlxSpi_Reset(AlxSpi* me);
static void AlxSpi_Periph_EnableClk(AlxSpi* me);
static void AlxSpi_Periph_DisableClk(AlxSpi* me);
static void AlxSpi_Periph_ForceReset(AlxSpi* me);
static void AlxSpi_Periph_ReleaseReset(AlxSpi* me);*/

//mf
static void AlxSpi_Ctor_ParseMode(AlxSpi* me);
static uint32_t AlxSpi_GetFlexCommClkFreq(AlxSpi* me);
static void AlxSpi_Periph_ReleaseReset(AlxSpi* me);
static void AlxSpi_Periph_AttachClk(AlxSpi* me);

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
	me->spiClk = spiClk;

	// Variables									// MF: Everything is set to default (see "void SPI_MasterGetDefaultConfig()" function) except "___"
	me->spiMasterConfig.enableLoopback				= false;
	me->spiMasterConfig.enableMaster				= true;
	me->spiMasterConfig.polarity					= kSPI_ClockPolarityActiveHigh; //todo
	me->spiMasterConfig.phase						= kSPI_ClockPhaseFirstEdge;//todo
	me->spiMasterConfig.direction					= kSPI_MsbFirst;
	me->spiMasterConfig.baudRate_Bps				= 500000U;
	me->spiMasterConfig.dataWidth					= kSPI_Data8Bits;
	me->spiMasterConfig.sselNum						= kSPI_Ssel1;	// MF: Default is kSPI_Ssel0 but it doesnt work
	me->spiMasterConfig.sselPol						= kSPI_SpolActiveAllLow;
	me->spiMasterConfig.txWatermark					= (uint8_t)kSPI_TxFifo0;
	me->spiMasterConfig.rxWatermark					= (uint8_t)kSPI_RxFifo1;
	me->spiMasterConfig.delayConfig.preDelay		= 0U;
	me->spiMasterConfig.delayConfig.postDelay		= 0U;
	me->spiMasterConfig.delayConfig.frameDelay		= 0U;
	me->spiMasterConfig.delayConfig.transferDelay	= 0U;

	AlxSpi_Ctor_ParseMode(me);

	// Info
	me->isInit = false;
	me->wasCtorCalled = true;
}


//******************************************************************************
// Specific Functions
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
	AlxSpi_Periph_ReleaseReset(me);

	// #3 Enable SPI Periphery Clock
	AlxSpi_Periph_AttachClk(me);

	// #4 Init SPI
	if (SPI_MasterInit(me->spi, &me->spiMasterConfig, AlxSpi_GetFlexCommClkFreq(me)) != kStatus_Success)	// MF: FlexComm Periph reset happens here
	{
		ALX_SPI_TRACE("ErrInit");
		return Alx_Err;
	};

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

	// #2 Force SPI Periphery Reset
	//AlxSpi_Periph_ForceReset(me);

	// #3 Disable SPI Periphery Clock
	//AlxSpi_Periph_DisableClk(me);

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
	// Assert
	ALX_SPI_ASSERT(me->isInit == true);
	ALX_SPI_ASSERT(me->wasCtorCalled == true);
	(void)me;
	(void)writeData;
	ALX_SPI_ASSERT(0 < len);
	ALX_SPI_ASSERT(0 < numOfTries);
	ALX_SPI_ASSERT(0 < timeout_ms);


	uint8_t destBuff[64];

	me->spiTransfer.txData = destBuff;
	me->spiTransfer.txData = writeData;
	me->spiTransfer.dataSize = len;//sizeof(destBuff);
	me->spiTransfer.configFlags = kSPI_FrameAssert;
	if (SPI_MasterTransferBlocking(me->spi, &me->spiTransfer) != kStatus_Success)	{ return Alx_Err; }

	// #1 Prepare variables
	/*HAL_StatusTypeDef status = HAL_ERROR;

	// #2 Try SPI write/read
	for (uint32_t _try = 1; _try <= numOfTries; _try++)
	{

		status = HAL_SPI_Transmit(&me->hspi, writeData, len, timeout_ms);
		if (status == HAL_OK)
		{
			break; // SPI write/read OK
		}
		else
		{
			ALX_SPI_TRACE("ErrHal");
			if(AlxSpi_Reset(me) != Alx_Ok) { ALX_SPI_TRACE("ErrReset"); return Alx_Err;};
			continue;
		}
	}

	// #3 If we are here, SPI write/read was OK or number of tries error occured
	if (status == HAL_OK)
	{
		return Alx_Ok;
	}
	else
	{
		ALX_SPI_TRACE("ErrNumOfTries");
		return Alx_ErrNumOfTries;
	}*/
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
	ALX_SPI_ASSERT(0 < timeout_ms);

	// #1 Prepare variables
	/*HAL_StatusTypeDef status = HAL_ERROR;

	// #2 Try SPI write/read
	for (uint32_t _try = 1; _try <= numOfTries; _try++)
	{

		status = HAL_SPI_Receive(&me->hspi, readData, len, timeout_ms);
		if (status == HAL_OK)
		{
			break; // SPI write/read OK
		}
		else
		{
			ALX_SPI_TRACE("ErrHal");
			if(AlxSpi_Reset(me) != Alx_Ok) { ALX_SPI_TRACE("ErrReset"); return Alx_Err;};
			continue;
		}
	}

	// #3 If we are here, SPI write/read was OK or number of tries error occured
	if (status == HAL_OK)
	{
		return Alx_Ok;
	}
	else
	{
		ALX_SPI_TRACE("ErrNumOfTries");
		return Alx_ErrNumOfTries;
	}*/
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
	ALX_SPI_ASSERT(0 < timeout_ms);

	// #1 Prepare variables
	/*HAL_StatusTypeDef status = HAL_ERROR;

	// #2 Try SPI write/read
	for (uint32_t _try = 1; _try <= numOfTries; _try++)
	{

		status = HAL_SPI_TransmitReceive(&me->hspi, writeData, readData, len, timeout_ms);
		if (status == HAL_OK)
		{
			break; // SPI write/read OK
		}
		else
		{
			ALX_SPI_TRACE("ErrHal");
			if(AlxSpi_Reset(me) != Alx_Ok) { ALX_SPI_TRACE("ErrReset"); return Alx_Err;};
			continue;
		}
	}

	// #3 If we are here, SPI write/read was OK or number of tries error occured
	if (status == HAL_OK)
	{
		return Alx_Ok;
	}
	else
	{
		ALX_SPI_TRACE("ErrNumOfTries");
		return Alx_ErrNumOfTries;
	}*/
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
static bool AlxSpi_Ctor_IsClkOk(AlxSpi* me)
{
	#if defined(Lpc55S6xF4)
	if((me->hspi.Instance == SPI1) || (me->hspi.Instance == SPI4))
	{
		if
		(
			(me->spiClk == AlxSpi_Clk_McuLpc55S6xF4_Spi1_Spi4_SpiClk_1MHz4_Pclk2Apb2_90MHz) ||
			(me->spiClk == AlxSpi_Clk_McuLpc55S6xF4_Spi1_Spi4_SpiClk_5MHz625_Pclk2Apb2_90MHz) ||
			(me->spiClk == AlxSpi_Clk_McuLpc55S6xF4_Spi1_Spi4_SpiClk_11MHz25_Pclk2Apb2_90MHz)
		)
		{
			if(90000000UL == AlxClk_GetClk_Hz(me->clk, AlxClk_Clk_McuLpc55S6x_Pclk2Apb2_Ctor))
				return true;
			else
				return false;
		}
	}
	if((me->hspi.Instance == SPI2) || (me->hspi.Instance == SPI3))
	{
		if
		(
			(me->spiClk == AlxSpi_Clk_McuLpc55S6xF4_Spi2_Spi3_SpiClk_1MHz4_Pclk1Apb1_45MHz) ||
			(me->spiClk == AlxSpi_Clk_McuLpc55S6xF4_Spi2_Spi3_SpiClk_5MHz625_Pclk1Apb1_45MHz) ||
			(me->spiClk == AlxSpi_Clk_McuLpc55S6xF4_Spi2_Spi3_SpiClk_11MHz25_Pclk1Apb1_45MHz)
		)
		{
			if(45000000UL == AlxClk_GetClk_Hz(me->clk, AlxClk_Clk_McuLpc55S6x_Pclk1Apb1_Ctor))
				return true;
			else
				return false;
		}
	}
	#endif
	#if defined(Lpc55S6xG4)
	if(me->hspi.Instance == SPI1)
	{
		if(me->spiClk == AlxSpi_Clk_McuLpc55S6xG4_Spi1_Spi4_SpiClk_1MHz33_Pclk2Apb2_170MHz)
		{
			if(170000000UL == AlxClk_GetClk_Hz(me->clk, AlxClk_Clk_McuLpc55S6x_Pclk2Apb2_Ctor))
				return true;
			else
				return false;
		}
	}
	#if !defined(Lpc55S6xG431xx)
	if(me->hspi.Instance == SPI4)
	{
		if(me->spiClk == AlxSpi_Clk_McuLpc55S6xG4_Spi1_Spi4_SpiClk_1MHz33_Pclk2Apb2_170MHz)
		{
			if(170000000UL == AlxClk_GetClk_Hz(me->clk, AlxClk_Clk_McuLpc55S6x_Pclk2Apb2_Ctor))
				return true;
			else
				return false;
		}
	}
	#endif
	if((me->hspi.Instance == SPI2) || (me->hspi.Instance == SPI3))
	{
		if(me->spiClk == AlxSpi_Clk_McuLpc55S6xG4_Spi2_Spi3_SpiClk_1MHz33_Pclk1Apb1_170MHz)
		{
			if(170000000UL == AlxClk_GetClk_Hz(me->clk, AlxClk_Clk_McuLpc55S6x_Pclk1Apb1_Ctor))
				return true;
			else
				return false;
		}
	}
	#endif
	#if defined(Lpc55S6xL0)
	if(me->hspi.Instance == SPI1)
	{
		if(me->spiClk == AlxSpi_Clk_McuLpc55S6xL0_Spi1_SpiClk_1MHz_Pclk2Apb2_32MHz)
		{
			if(32000000UL == AlxClk_GetClk_Hz(me->clk, AlxClk_Clk_McuLpc55S6x_Pclk2Apb2_Ctor))
				return true;
			else
				return false;
		}
	}
	if(me->hspi.Instance == SPI2)
	{
		if(me->spiClk == AlxSpi_Clk_McuLpc55S6xL0_Spi2_SpiClk_1MHz_Pclk1Apb1_32MHz)
		{
			if(32000000UL == AlxClk_GetClk_Hz(me->clk, AlxClk_Clk_McuLpc55S6x_Pclk1Apb1_Ctor))
				return true;
			else
				return false;
		}
	}
	#endif

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
static void AlxSpi_Periph_ReleaseReset(AlxSpi* me)
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
	if (me->spi == SPI0)	{ CLOCK_AttachClk(kFRO12M_to_FLEXCOMM0); return; }	// MF: In example there was Clk from FRO12MHz I do not understand which clk is right
	#endif
	#if defined(SPI1)
	if (me->spi == SPI1)	{ CLOCK_AttachClk(kFRO12M_to_FLEXCOMM1); return; }
	#endif
	#if defined(SPI2)
	if (me->spi == SPI2)	{ CLOCK_AttachClk(kFRO12M_to_FLEXCOMM2); return; }
	#endif
	#if defined(SPI3)
	if (me->spi == SPI3)	{ CLOCK_AttachClk(kFRO12M_to_FLEXCOMM3); return; }
	#endif
	#if defined(SPI4)
	if (me->spi == SPI4)	{ CLOCK_AttachClk(kFRO12M_to_FLEXCOMM4); return; }
	#endif
	#if defined(SPI5)
	if (me->spi == SPI5)	{ CLOCK_AttachClk(kFRO12M_to_FLEXCOMM5); return; }
	#endif
	#if defined(SPI6)
	if (me->spi == SPI6)	{ CLOCK_AttachClk(kFRO12M_to_FLEXCOMM6); return; }
	#endif
	#if defined(SPI7)
	if (me->spi == SPI7)	{ CLOCK_AttachClk(kFRO12M_to_FLEXCOMM7); return; }
	#endif

	//Assert
	ALX_SPI_ASSERT(false); // We shouldn't get here
	return;
}


/*static Alx_Status AlxSpi_Reset(AlxSpi* me)
{
	// #1 DeInit SPI
	if (HAL_SPI_DeInit(&me->hspi) != HAL_OK) { ALX_SPI_TRACE("ErrDeInit"); return Alx_Err; }
	
	// #2 Force SPI Periphery Reset
	AlxSpi_Periph_ForceReset(me);
	
	// #3 Reset isInit
	me->isInit = false;
	
	// #4 Release SPI Periphery Reset
	AlxSpi_Periph_ReleaseReset(me);
	
	// #5 Init SPI
	if (HAL_SPI_Init(&me->hspi) != HAL_OK) { ALX_SPI_TRACE("ErrInit"); return Alx_Err; }
	
	// #6 Set isInit
	me->isInit = true;
	
	// #7 Return OK
	return Alx_Ok;
}
static void AlxSpi_Periph_EnableClk(AlxSpi* me)
{
	bool isErr = true;

	#if defined(SPI1)
	if (me->hspi.Instance == SPI1)	{ __HAL_RCC_SPI1_CLK_ENABLE(); isErr = false; }
	#endif
	#if defined(SPI2)
	if (me->hspi.Instance == SPI2)	{ __HAL_RCC_SPI2_CLK_ENABLE(); isErr = false; }
	#endif
	#if defined(SPI3)
	if (me->hspi.Instance == SPI3)	{ __HAL_RCC_SPI3_CLK_ENABLE(); isErr = false; }
	#endif
	#if defined(SPI4)
	if (me->hspi.Instance == SPI4)	{ __HAL_RCC_SPI4_CLK_ENABLE(); isErr = false; }
	#endif
	#if defined(SPI5)
	if (me->hspi.Instance == SPI5)	{ __HAL_RCC_SPI5_CLK_ENABLE(); isErr = false; }
	#endif

	if(isErr)						{ ALX_SPI_ASSERT(false); } // We shouldn't get here
}
static void AlxSpi_Periph_DisableClk(AlxSpi* me)
{
	bool isErr = true;

	#if defined(SPI1)
	if (me->hspi.Instance == SPI1)	{ __HAL_RCC_SPI1_CLK_DISABLE(); isErr = false; }
	#endif
	#if defined(SPI2)
	if (me->hspi.Instance == SPI2)	{ __HAL_RCC_SPI2_CLK_DISABLE(); isErr = false; }
	#endif
	#if defined(SPI3)
	if (me->hspi.Instance == SPI3)	{ __HAL_RCC_SPI3_CLK_DISABLE(); isErr = false; }
	#endif
	#if defined(SPI4)
	if (me->hspi.Instance == SPI4)	{ __HAL_RCC_SPI4_CLK_DISABLE(); isErr = false; }
	#endif
	#if defined(SPI5)
	if (me->hspi.Instance == SPI5)	{ __HAL_RCC_SPI5_CLK_DISABLE(); isErr = false; }
	#endif

	if(isErr)						{ ALX_SPI_ASSERT(false); } // We shouldn't get here
}
static void AlxSpi_Periph_ForceReset(AlxSpi* me)
{
	bool isErr = true;

	#if defined(SPI1)
	if (me->hspi.Instance == SPI1)	{ __HAL_RCC_SPI1_FORCE_RESET(); isErr = false; }
	#endif
	#if defined(SPI2)
	if (me->hspi.Instance == SPI2)	{ __HAL_RCC_SPI2_FORCE_RESET(); isErr = false; }
	#endif
	#if defined(SPI3)
	if (me->hspi.Instance == SPI3)	{ __HAL_RCC_SPI3_FORCE_RESET(); isErr = false; }
	#endif
	#if defined(SPI4)
	if (me->hspi.Instance == SPI4)	{ __HAL_RCC_SPI4_FORCE_RESET(); isErr = false; }
	#endif
	#if defined(SPI5)
	if (me->hspi.Instance == SPI5)	{ __HAL_RCC_SPI5_FORCE_RESET(); isErr = false; }
	#endif

	if(isErr)						{ ALX_SPI_ASSERT(false); } // We shouldn't get here
}
static void AlxSpi_Periph_ReleaseReset(AlxSpi* me)
{
	bool isErr = true;

	#if defined(SPI1)
	if (me->hspi.Instance == SPI1)	{ __HAL_RCC_SPI1_RELEASE_RESET(); isErr = false; }
	#endif
	#if defined(SPI2)
	if (me->hspi.Instance == SPI2)	{ __HAL_RCC_SPI2_RELEASE_RESET(); isErr = false; }
	#endif
	#if defined(SPI3)
	if (me->hspi.Instance == SPI3)	{ __HAL_RCC_SPI3_RELEASE_RESET(); isErr = false; }
	#endif
	#if defined(SPI4)
	if (me->hspi.Instance == SPI4)	{ __HAL_RCC_SPI4_RELEASE_RESET(); isErr = false; }
	#endif
	#if defined(SPI5)
	if (me->hspi.Instance == SPI5)	{ __HAL_RCC_SPI5_RELEASE_RESET(); isErr = false; }
	#endif

	if(isErr)						{ ALX_SPI_ASSERT(false); } // We shouldn't get here
}*/




#endif
