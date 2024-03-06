/**
  ******************************************************************************
  * @file		alxMmc_McuStm32.h
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
// Include Guard
//******************************************************************************
#ifndef ALX_MMC_MCU_STM32_H
#define ALX_MMC_MCU_STM32_H

#ifdef __cplusplus
extern "C" {
#endif


//******************************************************************************
// Includes
//******************************************************************************
#include "alxGlobal.h"
#include "alxTrace.h"
#include "alxAssert.h"
#include "alxIoPin.h"
#include "alxTimSw.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB) && defined(ALX_STM32L4)


//******************************************************************************
// Types
//******************************************************************************
typedef struct
{
	// Defines
	#define ALX_MMC_BLOCK_LEN 512

	// Parameters
	MMC_TypeDef* mmc;
	AlxIoPin* do_nRST;
	AlxIoPin* do_CLK;
	AlxIoPin* do_CMD;
	AlxIoPin* io_DAT0;
	AlxIoPin* io_DAT1;
	AlxIoPin* io_DAT2;
	AlxIoPin* io_DAT3;
	AlxIoPin* io_DAT4;
	AlxIoPin* io_DAT5;
	AlxIoPin* io_DAT6;
	AlxIoPin* io_DAT7;
	uint16_t blockReadWriteTimeout_ms;
	uint16_t waitForTransferStateTimeout_ms;

	// Variables
	RCC_PeriphCLKInitTypeDef iclk;
	MMC_HandleTypeDef hmmc;
	HAL_MMC_CardCIDTypeDef cid;
	HAL_MMC_CardCSDTypeDef csd;
	HAL_MMC_CardInfoTypeDef info;

	// Info
	bool wasCtorCalled;
	bool isInit;
} AlxMmc;


//******************************************************************************
// Constructor
//******************************************************************************
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
);


#endif	// #if defined(ALX_C_LIB) && defined(ALX_STM32L4)

#ifdef __cplusplus
}
#endif

#endif	// #ifndef ALX_MMC_MCU_STM32_H
