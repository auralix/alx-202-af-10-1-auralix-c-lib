/**
  ******************************************************************************
  * @file		alxMmc.h
  * @brief		Auralix C Library - ALX MMC Module
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
#ifndef ALX_MMC_H
#define ALX_MMC_H

#ifdef __cplusplus
extern "C" {
#endif


//******************************************************************************
// Includes
//******************************************************************************
#include "alxGlobal.h"
#include "alxTrace.h"
#include "alxAssert.h"

#if defined(ALX_STM32L4)
#include "alxMmc_McuStm32.h"

#else
typedef struct { bool dummy; } AlxMmc;
#endif


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB)


//******************************************************************************
// Preprocessor
//******************************************************************************
#define ALX_MMC_FILE "alxMmc.h"

// Assert //
#if defined(ALX_MMC_ASSERT_BKPT_ENABLE)
	#define ALX_MMC_ASSERT(expr) ALX_ASSERT_BKPT(ALX_MMC_FILE, expr)
#elif defined(ALX_MMC_ASSERT_TRACE_ENABLE)
	#define ALX_MMC_ASSERT(expr) ALX_ASSERT_TRACE(ALX_MMC_FILE, expr)
#elif defined(ALX_MMC_ASSERT_RST_ENABLE)
	#define ALX_MMC_ASSERT(expr) ALX_ASSERT_RST(ALX_MMC_FILE, expr)
#else
	#define ALX_MMC_ASSERT(expr) do{} while (false)
#endif

// Trace //
#if defined(ALX_MMC_TRACE_ENABLE)
	#define ALX_MMC_TRACE(...) ALX_TRACE_STD(ALX_MMC_FILE, __VA_ARGS__)
#else
	#define ALX_MMC_TRACE(...) do{} while (false)
#endif


//******************************************************************************
// Functions
//******************************************************************************
Alx_Status AlxMmc_Init(AlxMmc* me);
Alx_Status AlxMmc_DeInit(AlxMmc* me);
Alx_Status AlxMmc_ReadBlock(AlxMmc* me, uint32_t numOfBlocks, uint32_t addr, uint8_t* data, uint32_t len, uint8_t numOfTries, uint16_t newTryWaitTime_ms);
Alx_Status AlxMmc_WriteBlock(AlxMmc* me, uint32_t numOfBlocks, uint32_t addr, uint8_t* data, uint32_t len, uint8_t numOfTries, uint16_t newTryWaitTime_ms);
void AlxMmc_IrqHandler(AlxMmc* me);


#endif	// #if defined(ALX_C_LIB)

#ifdef __cplusplus
}
#endif

#endif	// #ifndef ALX_MMC_H
