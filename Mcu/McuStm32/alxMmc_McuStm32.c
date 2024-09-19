/**
  ******************************************************************************
  * @file		alxMmc_McuStm32.c
  * @brief		Auralix C Library - ALX MMC MCU STM32 Module
  * @copyright	Copyright (C) Auralix d.o.o. All rights reserved.
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
#include "alxMmc_McuStm32.h"
#include "alxMmc.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB) && defined(ALX_STM32L4)


//******************************************************************************
// Private Variables
//******************************************************************************
#if defined(SDMMC1)
static AlxMmc* alxMmc_Sdmmc1_me = NULL;
#endif
#if defined(SDMMC2)
static AlxMmc* alxMmc_Sdmmc2_me = NULL;
#endif


//------------------------------------------------------------------------------
// Specific
//------------------------------------------------------------------------------
static uint32_t AlxMmc_GetBusWidth(AlxMmc* me);
static uint32_t AlxMmc_GetClkDiv(AlxMmc* me);
static Alx_Status AlxMmc_WaitForDmaReadWriteDone(AlxMmc* me, bool read);
static Alx_Status AlxMmc_WaitForTransferState_Private(AlxMmc* me);


//------------------------------------------------------------------------------
// General
//------------------------------------------------------------------------------
static Alx_Status AlxMmc_Init_Private(AlxMmc* me);
static Alx_Status AlxMmc_DeInit_Private(AlxMmc* me);
static Alx_Status AlxMmc_ReInit(AlxMmc* me);
static bool AlxMmc_IsClkOk(AlxMmc* me);
static void AlxMmc_Periph_EnableClk(AlxMmc* me);
static void AlxMmc_Periph_DisableClk(AlxMmc* me);
static void AlxMmc_Periph_ForceReset(AlxMmc* me);
static void AlxMmc_Periph_ReleaseReset(AlxMmc* me);
static void AlxMmc_Periph_EnableIrq(AlxMmc* me);
static void AlxMmc_Periph_DisableIrq(AlxMmc* me);


//******************************************************************************
// Constructor
//******************************************************************************

/**
  * @brief
  * @param[in,out]	me
  */
void AlxMmc_Ctor
(
	AlxMmc* me,
	MMC_TypeDef* mmc,
	AlxIoPin* do_nRST,
	AlxIoPin* do_CLK,
	AlxIoPin* do_CMD,
	AlxIoPin* io_DAT0,
	AlxIoPin* io_DAT1,
	AlxIoPin* io_DAT2,
	AlxIoPin* io_DAT3,
	AlxIoPin* io_DAT4,
	AlxIoPin* io_DAT5,
	AlxIoPin* io_DAT6,
	AlxIoPin* io_DAT7,
	AlxClk* clk,
	AlxMmc_Clk mmcClk,
	uint8_t* dmaReadWriteBuffAlign4,
	uint32_t dmaReadWriteBuffAlign4Len,
	uint16_t dmaReadWriteTimeout_ms,
	uint16_t waitForTransferStateTimeout_ms,
	Alx_IrqPriority irqPriority
)
{
	// Assert
	ALX_MMC_ASSERT(mmc == SDMMC1);	// Currently only SDMMC1 is supported

	// Private variables
	#if defined(SDMMC1)
	alxMmc_Sdmmc1_me = me;
	#endif
	#if defined(SDMMC2)
	alxMmc_Sdmmc2_me = me;
	#endif

	// Parameters
	me->mmc = mmc;
	me->do_nRST = do_nRST;
	me->do_CLK = do_CLK;
	me->do_CMD = do_CMD;
	me->io_DAT0 = io_DAT0;
	me->io_DAT1 = io_DAT1;
	me->io_DAT2 = io_DAT2;
	me->io_DAT3 = io_DAT3;
	me->io_DAT4 = io_DAT4;
	me->io_DAT5 = io_DAT5;
	me->io_DAT6 = io_DAT6;
	me->io_DAT7 = io_DAT7;
	me->clk = clk;
	me->mmcClk = mmcClk;
	me->dmaReadWriteBuffAlign4 = dmaReadWriteBuffAlign4;
	me->dmaReadWriteBuffAlign4Len = dmaReadWriteBuffAlign4Len;
	me->dmaReadWriteTimeout_ms = dmaReadWriteTimeout_ms;
	me->waitForTransferStateTimeout_ms = waitForTransferStateTimeout_ms;
	me->irqPriority = irqPriority;

	// Variables
	me->iclk.PeriphClockSelection = RCC_PERIPHCLK_SDMMC1;
	me->iclk.Sdmmc1ClockSelection = RCC_SDMMC1CLKSOURCE_PLLP;
	me->hmmc.Instance = mmc;
	me->hmmc.Init.ClockEdge = SDMMC_CLOCK_EDGE_RISING;
	me->hmmc.Init.ClockPowerSave = SDMMC_CLOCK_POWER_SAVE_DISABLE;
	me->hmmc.Init.BusWide = AlxMmc_GetBusWidth(me);
	me->hmmc.Init.HardwareFlowControl = SDMMC_HARDWARE_FLOW_CONTROL_ENABLE;
	me->hmmc.Init.ClockDiv = AlxMmc_GetClkDiv(me);
	me->hmmc.Init.Transceiver = SDMMC_TRANSCEIVER_DISABLE;
	me->dmaReadDone = false;
	me->dmaWriteDone = false;

	// Check clock
	ALX_MMC_ASSERT(AlxMmc_IsClkOk(me));

	// Info
	me->wasCtorCalled = true;
	me->isInit = false;
}


//******************************************************************************
// Functions
//******************************************************************************
Alx_Status AlxMmc_Init(AlxMmc* me)
{
	// Assert
	ALX_MMC_ASSERT(me->wasCtorCalled == true);
	ALX_MMC_ASSERT(me->isInit == false);

	// Return
	return AlxMmc_Init_Private(me);
}
Alx_Status AlxMmc_DeInit(AlxMmc* me)
{
	// Assert
	ALX_MMC_ASSERT(me->wasCtorCalled == true);
	ALX_MMC_ASSERT(me->isInit == true);

	// Return
	return AlxMmc_DeInit_Private(me);
}
Alx_Status AlxMmc_ReadBlock(AlxMmc* me, uint32_t numOfBlocks, uint32_t addr, uint8_t* data, uint32_t len, uint8_t numOfTries, uint16_t newTryWaitTime_ms)
{
	//------------------------------------------------------------------------------
	// Assert
	//------------------------------------------------------------------------------
	ALX_MMC_ASSERT(me->wasCtorCalled == true);
	ALX_MMC_ASSERT(me->isInit == true);
	ALX_MMC_ASSERT((numOfBlocks * ALX_MMC_BLOCK_LEN) == len);
	ALX_MMC_ASSERT(((uintptr_t)data & 0x3) == 0);	// Check data address 4-byte alignment, requred for DMA
	// TV: TODO - Implement dmaReadWriteBuffAlign4Len


	//------------------------------------------------------------------------------
	// Local Variables
	//------------------------------------------------------------------------------
	HAL_StatusTypeDef statusHal = HAL_ERROR;
	Alx_Status statusAlx = Alx_Err;


	//------------------------------------------------------------------------------
	// Try
	//------------------------------------------------------------------------------

	// Try for number of tries
	for (uint32_t _try = 1; _try <= numOfTries; _try++)
	{
		// Clear
		me->dmaReadDone = false;

		// Read
		statusHal = HAL_MMC_ReadBlocks_DMA(&me->hmmc, data, addr, numOfBlocks);
		if (statusHal != HAL_OK)
		{
			ALX_MMC_TRACE_WRN("Err: %d, try=%u, numOfBlocks=%u, addr=%u, dmaReadDone=%u", statusHal, _try, numOfBlocks, addr, me->dmaReadDone);
			statusAlx = AlxMmc_ReInit(me);
			if (statusAlx != Alx_Ok)
			{
				ALX_MMC_TRACE_WRN("Err: %d", statusAlx);
				return Alx_Err;
			}
			AlxDelay_ms(newTryWaitTime_ms);
			continue;
		}

		// Wait
		statusAlx = AlxMmc_WaitForDmaReadWriteDone(me, true);
		if (statusAlx != Alx_Ok)
		{
			ALX_MMC_TRACE_WRN("Err: %d, try=%u, numOfBlocks=%u, addr=%u, dmaReadDone=%u", statusAlx, _try, numOfBlocks, addr, me->dmaReadDone);
			statusAlx = AlxMmc_ReInit(me);
			if (statusAlx != Alx_Ok)
			{
				ALX_MMC_TRACE_WRN("Err: %d", statusAlx);
				return Alx_Err;
			}
			AlxDelay_ms(newTryWaitTime_ms);
			continue;
		}

		// Wait
		statusAlx = AlxMmc_WaitForTransferState_Private(me);
		if (statusAlx != Alx_Ok)
		{
			ALX_MMC_TRACE_WRN("Err: %d, try=%u, numOfBlocks=%u, addr=%u, dmaReadDone=%u", statusAlx, _try, numOfBlocks, addr, me->dmaReadDone);
			statusAlx = AlxMmc_ReInit(me);
			if (statusAlx != Alx_Ok)
			{
				ALX_MMC_TRACE_WRN("Err: %d", statusAlx);
				return Alx_Err;
			}
			AlxDelay_ms(newTryWaitTime_ms);
			continue;
		}

		// Return
		return Alx_Ok;
	}


	//------------------------------------------------------------------------------
	// Catch
	//------------------------------------------------------------------------------

	// Return
	return Alx_ErrNumOfTries;
}
Alx_Status AlxMmc_WriteBlock(AlxMmc* me, uint32_t numOfBlocks, uint32_t addr, uint8_t* data, uint32_t len, uint8_t numOfTries, uint16_t newTryWaitTime_ms)
{
	//------------------------------------------------------------------------------
	// Assert
	//------------------------------------------------------------------------------
	ALX_MMC_ASSERT(me->wasCtorCalled == true);
	ALX_MMC_ASSERT(me->isInit == true);
	ALX_MMC_ASSERT((numOfBlocks * ALX_MMC_BLOCK_LEN) == len);
	ALX_MMC_ASSERT(len <= me->dmaReadWriteBuffAlign4Len);
	ALX_MMC_ASSERT(((uintptr_t)me->dmaReadWriteBuffAlign4 & 0x3) == 0);	// Check data address 4-byte alignment, requred for DMA


	//------------------------------------------------------------------------------
	// Local Variables
	//------------------------------------------------------------------------------
	HAL_StatusTypeDef statusHal = HAL_ERROR;
	Alx_Status statusAlx = Alx_Err;


	//------------------------------------------------------------------------------
	// Prepare
	//------------------------------------------------------------------------------
	memset(me->dmaReadWriteBuffAlign4, 0, me->dmaReadWriteBuffAlign4Len);
	memcpy(me->dmaReadWriteBuffAlign4, data, len);


	//------------------------------------------------------------------------------
	// Try
	//------------------------------------------------------------------------------

	// Try for number of tries
	for (uint32_t _try = 1; _try <= numOfTries; _try++)
	{
		// Clear
		me->dmaWriteDone = false;

		// Read
		statusHal = HAL_MMC_WriteBlocks_DMA(&me->hmmc, me->dmaReadWriteBuffAlign4, addr, numOfBlocks);
		if (statusHal != HAL_OK)
		{
			ALX_MMC_TRACE_WRN("Err: %d, try=%u, numOfBlocks=%u, addr=%u, dmaWriteDone=%u", statusHal, _try, numOfBlocks, addr, me->dmaWriteDone);
			statusAlx = AlxMmc_ReInit(me);
			if (statusAlx != Alx_Ok)
			{
				ALX_MMC_TRACE_WRN("Err: %d", statusAlx);
				return Alx_Err;
			}
			AlxDelay_ms(newTryWaitTime_ms);
			continue;
		}

		// Wait
		statusAlx = AlxMmc_WaitForDmaReadWriteDone(me, false);
		if (statusAlx != Alx_Ok)
		{
			ALX_MMC_TRACE_WRN("Err: %d, try=%u, numOfBlocks=%u, addr=%u, dmaWriteDone=%u", statusAlx, _try, numOfBlocks, addr, me->dmaWriteDone);
			statusAlx = AlxMmc_ReInit(me);
			if (statusAlx != Alx_Ok)
			{
				ALX_MMC_TRACE_WRN("Err: %d", statusAlx);
				return Alx_Err;
			}
			AlxDelay_ms(newTryWaitTime_ms);
			continue;
		}

		// Wait
		statusAlx = AlxMmc_WaitForTransferState_Private(me);
		if (statusAlx != Alx_Ok)
		{
			ALX_MMC_TRACE_WRN("Err: %d, try=%u, numOfBlocks=%u, addr=%u, dmaWriteDone=%u", statusAlx, _try, numOfBlocks, addr, me->dmaWriteDone);
			statusAlx = AlxMmc_ReInit(me);
			if (statusAlx != Alx_Ok)
			{
				ALX_MMC_TRACE_WRN("Err: %d", statusAlx);
				return Alx_Err;
			}
			AlxDelay_ms(newTryWaitTime_ms);
			continue;
		}

		// Return
		return Alx_Ok;
	}


	//------------------------------------------------------------------------------
	// Catch
	//------------------------------------------------------------------------------

	// Return
	return Alx_ErrNumOfTries;
}
Alx_Status AlxMmc_WaitForTransferState(AlxMmc* me)
{
	// Assert
	ALX_MMC_ASSERT(me->wasCtorCalled == true);
	ALX_MMC_ASSERT(me->isInit == true);

	// Return
	return AlxMmc_WaitForTransferState_Private(me);
}
void AlxMmc_IrqHandler(AlxMmc* me)
{
	HAL_MMC_IRQHandler(&me->hmmc);
}


//******************************************************************************
// Private Functions
//******************************************************************************


//------------------------------------------------------------------------------
// Specific
//------------------------------------------------------------------------------
static uint32_t AlxMmc_GetBusWidth(AlxMmc* me)
{
	//------------------------------------------------------------------------------
	// STM32L4
	//------------------------------------------------------------------------------
	#if defined(ALX_STM32L4)
	if
	(
		(me->mmcClk == AlxMmx_Clk_McuStm32L4_1bit_MmcClk_12MHz_PllP_48MHz) ||
		(me->mmcClk == AlxMmx_Clk_McuStm32L4_1bit_MmcClk_24MHz_PllP_48MHz) ||
		(me->mmcClk == AlxMmx_Clk_McuStm32L4_1bit_MmcClk_48MHz_PllP_48MHz)
	)
	{
		return SDMMC_BUS_WIDE_1B;
	}
	if
	(
		(me->mmcClk == AlxMmx_Clk_McuStm32L4_4bit_MmcClk_12MHz_PllP_48MHz) ||
		(me->mmcClk == AlxMmx_Clk_McuStm32L4_4bit_MmcClk_24MHz_PllP_48MHz) ||
		(me->mmcClk == AlxMmx_Clk_McuStm32L4_4bit_MmcClk_48MHz_PllP_48MHz)
	)
	{
		return SDMMC_BUS_WIDE_4B;
	}
	if
	(
		(me->mmcClk == AlxMmx_Clk_McuStm32L4_8bit_MmcClk_12MHz_PllP_48MHz) ||
		(me->mmcClk == AlxMmx_Clk_McuStm32L4_8bit_MmcClk_24MHz_PllP_48MHz) ||
		(me->mmcClk == AlxMmx_Clk_McuStm32L4_8bit_MmcClk_48MHz_PllP_48MHz)
	)
	{
		return SDMMC_BUS_WIDE_8B;
	}
	#endif


	//------------------------------------------------------------------------------
	// Assert
	//------------------------------------------------------------------------------
	ALX_MMC_ASSERT(false);	// We should not get here
	return ALX_NULL;
}
static uint32_t AlxMmc_GetClkDiv(AlxMmc* me)
{
	//------------------------------------------------------------------------------
	// STM32L4
	//------------------------------------------------------------------------------
	#if defined(ALX_STM32L4)
	if
	(
		(me->mmcClk == AlxMmx_Clk_McuStm32L4_1bit_MmcClk_12MHz_PllP_48MHz) ||
		(me->mmcClk == AlxMmx_Clk_McuStm32L4_4bit_MmcClk_12MHz_PllP_48MHz) ||
		(me->mmcClk == AlxMmx_Clk_McuStm32L4_8bit_MmcClk_12MHz_PllP_48MHz)
	)
	{
		return 2;
	}
	if
	(
		(me->mmcClk == AlxMmx_Clk_McuStm32L4_1bit_MmcClk_24MHz_PllP_48MHz) ||
		(me->mmcClk == AlxMmx_Clk_McuStm32L4_4bit_MmcClk_24MHz_PllP_48MHz) ||
		(me->mmcClk == AlxMmx_Clk_McuStm32L4_8bit_MmcClk_24MHz_PllP_48MHz)
	)
	{
		return 1;
	}
	if
	(
		(me->mmcClk == AlxMmx_Clk_McuStm32L4_1bit_MmcClk_48MHz_PllP_48MHz) ||
		(me->mmcClk == AlxMmx_Clk_McuStm32L4_4bit_MmcClk_48MHz_PllP_48MHz) ||
		(me->mmcClk == AlxMmx_Clk_McuStm32L4_8bit_MmcClk_48MHz_PllP_48MHz)
	)
	{
		return 0;
	}
	#endif


	//------------------------------------------------------------------------------
	// Assert
	//------------------------------------------------------------------------------
	ALX_MMC_ASSERT(false);	// We should not get here
	return ALX_NULL;
}
static Alx_Status AlxMmc_WaitForDmaReadWriteDone(AlxMmc* me, bool read)
{
	// Local variables
	AlxTimSw alxTimSw;
	AlxTimSw_Ctor(&alxTimSw, false);

	// Start timer
	AlxTimSw_Start(&alxTimSw);

	// Loop
	while (true)
	{
		// Check state
		if (read)
		{
			if (me->dmaReadDone)
			{
				return Alx_Ok;
			}
		}
		else	// write
		{
			if (me->dmaWriteDone)
			{
				return Alx_Ok;
			}
		}

		// Check if timeout
		if (AlxTimSw_IsTimeout_ms(&alxTimSw, me->dmaReadWriteTimeout_ms))
		{
			return Alx_Err;
		}
	}
}
static Alx_Status AlxMmc_WaitForTransferState_Private(AlxMmc* me)
{
	// Local variables
	AlxTimSw alxTimSw;
	AlxTimSw_Ctor(&alxTimSw, false);

	// Start timer
	AlxTimSw_Start(&alxTimSw);

	// Loop
	while (true)
	{
		// Get state
		HAL_MMC_CardStateTypeDef state = HAL_MMC_GetCardState(&me->hmmc);

		// Check state
		if (state == HAL_MMC_CARD_TRANSFER)
		{
			return Alx_Ok;
		}

		// Check if timeout
		if (AlxTimSw_IsTimeout_ms(&alxTimSw, me->waitForTransferStateTimeout_ms))
		{
			return Alx_Err;
		}
	}
}


//------------------------------------------------------------------------------
// General
//------------------------------------------------------------------------------
static Alx_Status AlxMmc_Init_Private(AlxMmc* me)
{
	// Local variables
	HAL_StatusTypeDef statusHal = HAL_ERROR;
	Alx_Status statusAlx = Alx_Err;

	// Trace
	ALX_MMC_TRACE_INF("");
	ALX_MMC_TRACE_INF("AlxMmc - Init started");

	// Init GPIO
	AlxIoPin_Init(me->do_nRST);
	AlxIoPin_Init(me->do_CLK);
	AlxIoPin_Init(me->do_CMD);
	AlxIoPin_Init(me->io_DAT0);
	AlxIoPin_Init(me->io_DAT1);
	AlxIoPin_Init(me->io_DAT2);
	AlxIoPin_Init(me->io_DAT3);
	AlxIoPin_Init(me->io_DAT4);
	AlxIoPin_Init(me->io_DAT5);
	AlxIoPin_Init(me->io_DAT6);
	AlxIoPin_Init(me->io_DAT7);

	// Disable MMC
	AlxIoPin_Reset(me->do_nRST);

	// Wait for GPIO stable
	AlxDelay_ms(100);

	// Enable MMC
	AlxIoPin_Set(me->do_nRST);

	// Wait for MMC boot
	AlxDelay_ms(100);

	// Init clock
	statusHal = HAL_RCCEx_PeriphCLKConfig(&me->iclk);
	if (statusHal != HAL_OK)
	{
		ALX_MMC_TRACE_WRN("Err: %d", statusHal);
		return Alx_Err;
	}

	// Release MMC periphery reset
	AlxMmc_Periph_ReleaseReset(me);

	// Enable MMC periphery clock
	AlxMmc_Periph_EnableClk(me);

	// Init IRQ
	AlxMmc_Periph_EnableIrq(me);

	// Init MMC - Communication with MMC
	statusHal = HAL_MMC_Init(&me->hmmc);
	if (statusHal != HAL_OK)
	{
		ALX_MMC_TRACE_WRN("Err: %d", statusHal);
		return Alx_Err;
	}

	// Wait for Transfer State - Communication with MMC
	statusAlx = AlxMmc_WaitForTransferState_Private(me);
	if (statusAlx != Alx_Ok)
	{
		ALX_MMC_TRACE_WRN("Err: %d", statusAlx);
		return Alx_Err;
	}

	// Get CID - NO Commucination with MMC - Uses data which is aquired from HAL_MMC_Init
	statusHal = HAL_MMC_GetCardCID(&me->hmmc, &me->cid);
	if (statusHal != HAL_OK)
	{
		ALX_MMC_TRACE_WRN("Err: %d", statusHal);
		return Alx_Err;
	}

	// Get CSD - Communication with MMC
	statusHal = HAL_MMC_GetCardCSD(&me->hmmc, &me->csd);
	if (statusHal != HAL_OK)
	{
		ALX_MMC_TRACE_WRN("Err: %d", statusHal);
		return Alx_Err;
	}

	// Get Info - NO Commucination with MMC - Uses data which is aquired from HAL_MMC_GetCardCSD
	statusHal = HAL_MMC_GetCardInfo(&me->hmmc, &me->info);
	if (statusHal != HAL_OK)
	{
		ALX_MMC_TRACE_WRN("Err: %d", statusHal);
		return Alx_Err;
	}

	// Trace
	float size_B = (float)me->info.BlockNbr * (float)me->info.BlockSize;
	float size_GB = size_B / 1000000000.f;
	ALX_MMC_TRACE_INF("AlxMmc - size_GB=%.2f", size_GB);

	// Set isInit
	me->isInit = true;

	// Return
	return Alx_Ok;
}
static Alx_Status AlxMmc_DeInit_Private(AlxMmc* me)
{
	// Local variables
	HAL_StatusTypeDef statusHal = HAL_ERROR;

	// DeInit IRQ
	AlxMmc_Periph_DisableIrq(me);

	// DeInit MMC
	statusHal = HAL_MMC_DeInit(&me->hmmc);
	if (statusHal != HAL_OK)
	{
		ALX_MMC_TRACE_WRN("Err: %d", statusHal);
		return Alx_Err;
	}

	// Force MMC periphery reset
	AlxMmc_Periph_ForceReset(me);

	// Disable MMC periphery clock
	AlxMmc_Periph_DisableClk(me);

	// DeInit GPIO
	AlxIoPin_DeInit(me->do_nRST);
	AlxIoPin_DeInit(me->do_CLK);
	AlxIoPin_DeInit(me->do_CMD);
	AlxIoPin_DeInit(me->io_DAT0);
	AlxIoPin_DeInit(me->io_DAT1);
	AlxIoPin_DeInit(me->io_DAT2);
	AlxIoPin_DeInit(me->io_DAT3);
	AlxIoPin_DeInit(me->io_DAT4);
	AlxIoPin_DeInit(me->io_DAT5);
	AlxIoPin_DeInit(me->io_DAT6);
	AlxIoPin_DeInit(me->io_DAT7);

	// Clear isInit
	me->isInit = false;

	// Return
	return Alx_Ok;
}
static Alx_Status AlxMmc_ReInit(AlxMmc* me)
{
	// Local variables
	Alx_Status status = Alx_Err;

	// DeInit
	status = AlxMmc_DeInit_Private(me);
	if (status != Alx_Ok) return status;

	// Init
	status = AlxMmc_Init_Private(me);
	if (status != Alx_Ok) return status;

	// Return
	return Alx_Ok;
}
static bool AlxMmc_IsClkOk(AlxMmc* me)
{
	//------------------------------------------------------------------------------
	// STM32L4
	//------------------------------------------------------------------------------
	#if defined(ALX_STM32L4)
	if
	(
		(me->mmcClk == AlxMmx_Clk_McuStm32L4_1bit_MmcClk_12MHz_PllP_48MHz) ||
		(me->mmcClk == AlxMmx_Clk_McuStm32L4_4bit_MmcClk_12MHz_PllP_48MHz) ||
		(me->mmcClk == AlxMmx_Clk_McuStm32L4_8bit_MmcClk_12MHz_PllP_48MHz) ||
		(me->mmcClk == AlxMmx_Clk_McuStm32L4_1bit_MmcClk_24MHz_PllP_48MHz) ||
		(me->mmcClk == AlxMmx_Clk_McuStm32L4_4bit_MmcClk_24MHz_PllP_48MHz) ||
		(me->mmcClk == AlxMmx_Clk_McuStm32L4_8bit_MmcClk_24MHz_PllP_48MHz) ||
		(me->mmcClk == AlxMmx_Clk_McuStm32L4_1bit_MmcClk_48MHz_PllP_48MHz) ||
		(me->mmcClk == AlxMmx_Clk_McuStm32L4_4bit_MmcClk_48MHz_PllP_48MHz) ||
		(me->mmcClk == AlxMmx_Clk_McuStm32L4_8bit_MmcClk_48MHz_PllP_48MHz)
	)
	{
		if(48000000 == AlxClk_GetClk_Hz(me->clk, AlxClk_Clk_McuStm32_PllP_Ctor))
			return true;
		else
			return false;
	}
	#endif


	//------------------------------------------------------------------------------
	// Assert
	//------------------------------------------------------------------------------
	ALX_MMC_ASSERT(false);	// We should not get here
	return ALX_NULL;
}
static void AlxMmc_Periph_EnableClk(AlxMmc* me)
{
	#if defined(SDMMC1)
	if (me->hmmc.Instance == SDMMC1)	{ __HAL_RCC_SDMMC1_CLK_ENABLE(); return; }
	#endif
	#if defined(SDMMC2)
	if (me->hmmc.Instance == SDMMC2)	{ __HAL_RCC_SDMMC2_CLK_ENABLE(); return; }
	#endif

	ALX_MMC_ASSERT(false);	// We should not get here
}
static void AlxMmc_Periph_DisableClk(AlxMmc* me)
{
	#if defined(SDMMC1)
	if (me->hmmc.Instance == SDMMC1)	{ __HAL_RCC_SDMMC1_CLK_DISABLE(); return; }
	#endif
	#if defined(SDMMC2)
	if (me->hmmc.Instance == SDMMC2)	{ __HAL_RCC_SDMMC2_CLK_DISABLE(); return; }
	#endif

	ALX_MMC_ASSERT(false);	// We should not get here
}
static void AlxMmc_Periph_ForceReset(AlxMmc* me)
{
	#if defined(SDMMC1)
	if (me->hmmc.Instance == SDMMC1)	{ __HAL_RCC_SDMMC1_FORCE_RESET(); return; }
	#endif
	#if defined(SDMMC2)
	if (me->hmmc.Instance == SDMMC2)	{ __HAL_RCC_SDMMC2_FORCE_RESET(); return; }
	#endif

	ALX_MMC_ASSERT(false);	// We should not get here
}
static void AlxMmc_Periph_ReleaseReset(AlxMmc* me)
{
	#if defined(SDMMC1)
	if (me->hmmc.Instance == SDMMC1)	{ __HAL_RCC_SDMMC1_RELEASE_RESET(); return; }
	#endif
	#if defined(SDMMC2)
	if (me->hmmc.Instance == SDMMC2)	{ __HAL_RCC_SDMMC2_RELEASE_RESET(); return; }
	#endif

	ALX_MMC_ASSERT(false);	// We should not get here
}
static void AlxMmc_Periph_EnableIrq(AlxMmc* me)
{
	#if defined(SDMMC1)
	if (me->hmmc.Instance == SDMMC1)	{ HAL_NVIC_SetPriority(SDMMC1_IRQn, me->irqPriority, 0); HAL_NVIC_EnableIRQ(SDMMC1_IRQn); return; }
	#endif
	#if defined(SDMMC2)
	if (me->hmmc.Instance == SDMMC2)	{ HAL_NVIC_SetPriority(SDMMC2_IRQn, me->irqPriority, 0); HAL_NVIC_EnableIRQ(SDMMC2_IRQn); return; }
	#endif

	ALX_MMC_ASSERT(false);	// We should not get here
}
static void AlxMmc_Periph_DisableIrq(AlxMmc* me)
{
	#if defined(SDMMC1)
	if (me->hmmc.Instance == SDMMC1)	{ HAL_NVIC_DisableIRQ(SDMMC1_IRQn); HAL_NVIC_ClearPendingIRQ(SDMMC1_IRQn); return; }
	#endif
	#if defined(SDMMC2)
	if (me->hmmc.Instance == SDMMC2)	{ HAL_NVIC_DisableIRQ(SDMMC2_IRQn); HAL_NVIC_ClearPendingIRQ(SDMMC2_IRQn); return; }
	#endif

	ALX_MMC_ASSERT(false);	// We should not get here
}


//******************************************************************************
// Weak Functions
//******************************************************************************
void HAL_MMC_RxCpltCallback(MMC_HandleTypeDef *hmmc)
{
	(void)hmmc;

	#if defined(SDMMC1)
	alxMmc_Sdmmc1_me->dmaReadDone = true;
	#endif
	#if defined(SDMMC2)
	alxMmc_Sdmmc2_me->isDmaReadDone = true;
	#endif
}
void HAL_MMC_TxCpltCallback(MMC_HandleTypeDef *hmmc)
{
	(void)hmmc;

	#if defined(SDMMC1)
	alxMmc_Sdmmc1_me->dmaWriteDone = true;
	#endif
	#if defined(SDMMC2)
	alxMmc_Sdmmc2_me->isDmaWriteDone = true;
	#endif
}


#endif	// #if defined(ALX_C_LIB) && defined(ALX_STM32L4)
