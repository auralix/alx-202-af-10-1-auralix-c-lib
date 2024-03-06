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
// Private Functions
//******************************************************************************


//------------------------------------------------------------------------------
// Specific
//------------------------------------------------------------------------------
static Alx_Status AlxMmc_WaitForTransferState(AlxMmc* me);


//------------------------------------------------------------------------------
// General
//------------------------------------------------------------------------------
static Alx_Status AlxMmc_Reset(AlxMmc* me);
static bool AlxMmc_IsClkOk(AlxMmc* me);
static void AlxMmc_Periph_EnableClk(AlxMmc* me);
static void AlxMmc_Periph_DisableClk(AlxMmc* me);
static void AlxMmc_Periph_ForceReset(AlxMmc* me);
static void AlxMmc_Periph_ReleaseReset(AlxMmc* me);


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
	uint16_t blockReadWriteTimeout_ms,
	uint16_t waitForTransferStateTimeout_ms
)
{
	// Assert
	ALX_MMC_ASSERT(mmc == SDMMC1);	// Currently only SDMMC1 is supported

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
	me->blockReadWriteTimeout_ms = blockReadWriteTimeout_ms;
	me->waitForTransferStateTimeout_ms = waitForTransferStateTimeout_ms;

	// Variables
	me->iclk.PeriphClockSelection = RCC_PERIPHCLK_SDMMC1;
	me->iclk.Sdmmc1ClockSelection = RCC_SDMMC1CLKSOURCE_PLLP;
	me->hmmc.Instance = mmc;
	me->hmmc.Init.ClockEdge = SDMMC_CLOCK_EDGE_RISING;
	me->hmmc.Init.ClockPowerSave = SDMMC_CLOCK_POWER_SAVE_DISABLE;
	me->hmmc.Init.BusWide = SDMMC_BUS_WIDE_8B;
	me->hmmc.Init.HardwareFlowControl = SDMMC_HARDWARE_FLOW_CONTROL_ENABLE;
	me->hmmc.Init.ClockDiv = 0;
	me->hmmc.Init.Transceiver = SDMMC_TRANSCEIVER_DISABLE;

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
	if (HAL_RCCEx_PeriphCLKConfig(&me->iclk) != HAL_OK) { ALX_MMC_TRACE("Err"); return Alx_Err; }

	// Release SDMMC periphery reset
	AlxMmc_Periph_ReleaseReset(me);

	// Enable SDMMC periphery clock
	AlxMmc_Periph_EnableClk(me);

	// Init SDMMC
	if (HAL_MMC_Init(&me->hmmc) != HAL_OK) { ALX_MMC_TRACE("Err"); return Alx_Err; }					// TV: Communication with MMC

	// Wait for Transfer State
	if (AlxMmc_WaitForTransferState(me) != Alx_Ok) { ALX_MMC_TRACE("Err"); return Alx_Err; }			// TV: Communication with MMC

	// Get CID
	if (HAL_MMC_GetCardCID(&me->hmmc, &me->cid) != HAL_OK) { ALX_MMC_TRACE("Err"); return Alx_Err; }	// TV: NO Commucination with MMC - Uses data which is aquired from HAL_MMC_Init

	// Get CSD
	if (HAL_MMC_GetCardCSD(&me->hmmc, &me->csd) != HAL_OK) { ALX_MMC_TRACE("Err"); return Alx_Err; }	// TV: Communication with MMC

	// Get Info
	if (HAL_MMC_GetCardInfo(&me->hmmc, &me->info) != HAL_OK) { ALX_MMC_TRACE("Err"); return Alx_Err; }	// TV: NO Commucination with MMC - Uses data which is aquired from HAL_MMC_GetCardCSD

	// Set isInit
	me->isInit = true;

	// Return
	return Alx_Ok;
}
Alx_Status AlxMmc_DeInit(AlxMmc* me)
{
	// Assert
	ALX_MMC_ASSERT(me->wasCtorCalled == true);
	ALX_MMC_ASSERT(me->isInit == true);

	// DeInit MMC
	if (HAL_MMC_DeInit(&me->hmmc) != HAL_OK) { ALX_MMC_TRACE("Err"); return Alx_Err; }

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
Alx_Status AlxMmc_ReadBlock(AlxMmc* me, uint32_t numOfBlocks, uint32_t addr, uint8_t* data, uint32_t len, uint8_t numOfTries, uint16_t newTryWaitTime_ms)
{
	//------------------------------------------------------------------------------
	// Assert
	//------------------------------------------------------------------------------
	ALX_MMC_ASSERT(me->wasCtorCalled == true);
	ALX_MMC_ASSERT(me->isInit == true);
	ALX_MMC_ASSERT((numOfBlocks * ALX_MMC_BLOCK_LEN) == len);


	//------------------------------------------------------------------------------
	// Try
	//------------------------------------------------------------------------------

	// Try for number of tries
	for (uint32_t _try = 1; _try <= numOfTries; _try++)
	{
		// Read
		HAL_StatusTypeDef statusHal = HAL_MMC_ReadBlocks(&me->hmmc, data, addr, numOfBlocks, me->blockReadWriteTimeout_ms);
		if (statusHal != HAL_OK)
		{
			ALX_MMC_TRACE("Err");
			if (AlxMmc_Reset(me) != Alx_Ok) { ALX_MMC_TRACE("Err"); return Alx_Err; }
			AlxDelay_ms(newTryWaitTime_ms);
			continue;
		}

		// Wait
		Alx_Status statusAlx = AlxMmc_WaitForTransferState(me);
		if (statusAlx != Alx_Ok)
		{
			ALX_MMC_TRACE("Err");
			if (AlxMmc_Reset(me) != Alx_Ok) { ALX_MMC_TRACE("Err"); return Alx_Err; }
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


	//------------------------------------------------------------------------------
	// Try
	//------------------------------------------------------------------------------

	// Try for number of tries
	for (uint32_t _try = 1; _try <= numOfTries; _try++)
	{
		// Read
		HAL_StatusTypeDef statusHal = HAL_MMC_WriteBlocks(&me->hmmc, data, addr, numOfBlocks, me->blockReadWriteTimeout_ms);
		if (statusHal != HAL_OK)
		{
			ALX_MMC_TRACE("Err");
			if (AlxMmc_Reset(me) != Alx_Ok) { ALX_MMC_TRACE("Err"); return Alx_Err; }
			AlxDelay_ms(newTryWaitTime_ms);
			continue;
		}

		// Wait
		Alx_Status statusAlx = AlxMmc_WaitForTransferState(me);
		if (statusAlx != Alx_Ok)
		{
			ALX_MMC_TRACE("Err");
			if (AlxMmc_Reset(me) != Alx_Ok) { ALX_MMC_TRACE("Err"); return Alx_Err; }
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


//******************************************************************************
// Private Functions
//******************************************************************************


//------------------------------------------------------------------------------
// Specific
//------------------------------------------------------------------------------
static Alx_Status AlxMmc_WaitForTransferState(AlxMmc* me)
{
	// Local variables
	AlxTimSw alxTimSw;
	AlxTimSw_Ctor(&alxTimSw, false);

	// Start timer
	AlxTimSw_Start(&alxTimSw);

	// Loop
	while (1)
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
static Alx_Status AlxMmc_Reset(AlxMmc* me)
{
	// DeInit I2C
	if (HAL_MMC_DeInit(&me->hmmc) != HAL_OK) { ALX_MMC_TRACE("Err"); return Alx_Err; }

	// Force MMC periphery reset
	AlxMmc_Periph_ForceReset(me);

	// Clear isInit
	me->isInit = false;

	// Disable MMC
	AlxIoPin_Reset(me->do_nRST);

	// Wait for GPIO stable
	AlxDelay_ms(100);

	// Enable MMC
	AlxIoPin_Set(me->do_nRST);

	// Wait for MMC boot
	AlxDelay_ms(100);

	// Release SDMMC periphery reset
	AlxMmc_Periph_ReleaseReset(me);

	// Init SDMMC
	if (HAL_MMC_Init(&me->hmmc) != HAL_OK) { ALX_MMC_TRACE("Err"); return Alx_Err; }

	// Wait for Transfer State
	if (AlxMmc_WaitForTransferState(me) != Alx_Ok) { ALX_MMC_TRACE("Err"); return Alx_Err; }

	// Set isInit
	me->isInit = true;

	// Return
	return Alx_Ok;
}
static bool AlxMmc_IsClkOk(AlxMmc* me)
{
	// TODO
	(void)me;
	return true;
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


#endif	// #if defined(ALX_C_LIB) && defined(ALX_STM32L4)
