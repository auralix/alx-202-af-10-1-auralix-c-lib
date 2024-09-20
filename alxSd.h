/**
  ******************************************************************************
  * @file		alxSd.h
  * @brief		Auralix C Library - ALX SD Card Module
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
#ifndef ALX_SD_H
#define ALX_SD_H

#ifdef __cplusplus
extern "C" {
#endif


//******************************************************************************
// Includes
//******************************************************************************
#include "alxGlobal.h"
#include "alxAssert.h"
#include "alxTrace.h"
#include "alxSpi.h"
#include "alxTimSw.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB)


//******************************************************************************
// Preprocessor
//******************************************************************************
#define ALX_SD_MODULE "AlxSd.h"

// Assert //
#if defined(ALX_SD_ASSERT_BKPT_ENABLE)
	#define ALX_SD_ASSERT(expr) ALX_ASSERT_BKPT(ALX_SD_MODULE, expr)
#elif defined(ALX_SD_ASSERT_TRACE_ENABLE)
	#define ALX_SD_ASSERT(expr) ALX_ASSERT_TRACE(ALX_SD_MODULE, expr)
#elif defined(ALX_SD_ASSERT_RST_ENABLE)
	#define ALX_SD_ASSERT(expr) ALX_ASSERT_RST(ALX_SD_MODULE, expr)
#else
	#define ALX_SD_ASSERT(expr) do{} while (false)
#endif

// Trace //
#if defined(ALX_SD_TRACE_ENABLE)
	#define ALX_SD_TRACE_WRN(...) ALX_TRACE_WRN(ALX_SD_MODULE, __VA_ARGS__)
#else
	#define ALX_SD_TRACE_WRN(...) do{} while (false)
#endif


//******************************************************************************
// Types
//******************************************************************************
typedef struct
{
	// Defines
	#define ALX_SD_DATA_0xFF_LEN 520
	#define ALX_SD_BLOCK_LEN 512

	// Const
	uint8_t DATA_0xFF[ALX_SD_DATA_0xFF_LEN];
	uint8_t NUM_OF_PWR_UP_CLK_CYCLES;

	// Parameters
	AlxSpi* alxSpi;
	uint8_t spiNumOfTries;
	uint16_t spiTimeout_ms;
	uint16_t cmdRespR1Timeout_ms;
	uint16_t acmd41Timeout_ms;
	uint16_t blockReadStartTokenTimeout_ms;
	uint16_t blockWriteStartTokenTimeout_ms;
	uint16_t blockWriteStopTokenTimeout_ms;

	// Info
	bool wasCtorCalled;
	bool isInit;
} AlxSd;


//******************************************************************************
// Constructor
//******************************************************************************
void AlxSd_Ctor
(
	AlxSd* me,
	AlxSpi* alxSpi,
	uint8_t spiNumOfTries,
	uint16_t spiTimeout_ms,
	uint16_t cmdRespR1Timeout_ms,
	uint16_t acmd41Timeout_ms,
	uint16_t blockReadStartTokenTimeout_ms,
	uint16_t blockWriteStartTokenTimeout_ms,
	uint16_t blockWriteStopTokenTimeout_ms
);


//******************************************************************************
// Functions
//******************************************************************************
Alx_Status AlxSd_Init(AlxSd* me, uint8_t numOfTries, uint16_t newTryWaitTime_ms);
Alx_Status AlxSd_DeInit(AlxSd* me, uint8_t numOfTries, uint16_t newTryWaitTime_ms);
Alx_Status AlxSd_ReadBlock(AlxSd* me, uint32_t numOfBlocks, uint32_t addr, uint8_t* data, uint32_t len, uint8_t numOfTries, uint16_t newTryWaitTime_ms);
Alx_Status AlxSd_WriteBlock(AlxSd* me, uint32_t numOfBlocks, uint32_t addr, uint8_t* data, uint32_t len, uint8_t numOfTries, uint16_t newTryWaitTime_ms);


#endif	// #if defined(ALX_C_LIB)

#ifdef __cplusplus
}
#endif

#endif	// #ifndef ALX_SD_H
