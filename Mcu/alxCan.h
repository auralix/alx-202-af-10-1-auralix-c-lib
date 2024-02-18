/**
  ******************************************************************************
  * @file		alxCan.h
  * @brief		Auralix C Library - ALX CAN Module
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
#ifndef ALX_CAN_H
#define ALX_CAN_H

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

#if ((defined(ALX_STM32F4) || defined(ALX_STM32L4)) && defined(HAL_CAN_MODULE_ENABLED)) || (defined(ALX_STM32G4) && defined(HAL_FDCAN_MODULE_ENABLED))
#include "alxCan_McuStm32.h"

#else
typedef struct { bool dummy; } AlxCan;
#endif


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB)


//******************************************************************************
// Preprocessor
//******************************************************************************
#define ALX_CAN_FILE "alxCan.h"

// Assert //
#if defined(ALX_CAN_ASSERT_BKPT_ENABLE)
	#define ALX_CAN_ASSERT(expr) ALX_ASSERT_BKPT(ALX_CAN_FILE, expr)
#elif defined(ALX_CAN_ASSERT_TRACE_ENABLE)
	#define ALX_CAN_ASSERT(expr) ALX_ASSERT_TRACE(ALX_CAN_FILE, expr)
#elif defined(ALX_CAN_ASSERT_RST_ENABLE)
	#define ALX_CAN_ASSERT(expr) ALX_ASSERT_RST(ALX_CAN_FILE, expr)
#else
	#define ALX_CAN_ASSERT(expr) do{} while (false)
#endif

// Trace //
#if defined(ALX_CAN_TRACE_ENABLE)
	#define ALX_CAN_TRACE(...) ALX_TRACE_STD(ALX_CAN_FILE, __VA_ARGS__)
#else
	#define ALX_CAN_TRACE(...) do{} while (false)
#endif


//******************************************************************************
// Types
//******************************************************************************
typedef struct
{
	uint32_t id;
	bool isExtendedId;	// false = Standard
	bool isDataFrame;	// false = Remote
	uint8_t dataLen;
	uint8_t data[8];
} AlxCan_Msg;


//******************************************************************************
// Functions
//******************************************************************************
Alx_Status AlxCan_Init(AlxCan* me);
Alx_Status AlxCan_DeInit(AlxCan* me);
Alx_Status AlxCan_ReInit(AlxCan* me);
Alx_Status AlxCan_TxMsg(AlxCan* me, AlxCan_Msg msg);
Alx_Status AlxCan_TxMsgMulti(AlxCan* me, AlxCan_Msg* msg, uint32_t numOfMsg);
Alx_Status AlxCan_RxMsg(AlxCan* me, AlxCan_Msg* msg);
Alx_Status AlxCan_RxMsgMulti(AlxCan* me, AlxCan_Msg* msg, uint32_t numOfMsg);
bool AlxCan_IsErr(AlxCan* me);
void AlxCan_IrqHandler(AlxCan* me);


#endif	// #if defined(ALX_C_LIB)

#ifdef __cplusplus
}
#endif

#endif	// #ifndef ALX_CAN_H
