/**
  ******************************************************************************
  * @file		alxCan.h
  * @brief		Auralix C Library - ALX CAN Module
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

// AlxMcu //
#if (defined(ALX_STM32F4) && defined(HAL_CAN_MODULE_ENABLED)) || (defined(ALX_STM32G4) && defined(HAL_FDCAN_MODULE_ENABLED))
#include "alxCan_McuStm32.h"
#else
typedef struct { bool dummy; } AlxCan;
#endif


//******************************************************************************
// Preprocessor
//******************************************************************************
#define ALX_CAN_FILE "alxCan"

// Assert //
#if defined(_ALX_CAN_ASSERT_BKPT) || defined(_ALX_ASSERT_BKPT_ALL)
	#define ALX_CAN_ASSERT(expr) ALX_ASSERT_BKPT(ALX_CAN_FILE, expr)
#elif defined(_ALX_CAN_ASSERT_TRACE) || defined(_ALX_ASSERT_TRACE_ALL)
	#define ALX_CAN_ASSERT(expr) ALX_ASSERT_TRACE(ALX_CAN_FILE, expr)
#elif defined(_ALX_CAN_ASSERT_RST) || defined(_ALX_ASSERT_RST_ALL)
	#define ALX_CAN_ASSERT(expr) ALX_ASSERT_RST(ALX_CAN_FILE, expr)
#else
	#define ALX_CAN_ASSERT(expr) do{} while (false)
#endif

// Trace //
#if defined(_ALX_CAN_TRACE) || defined(_ALX_TRACE_ALL)
	#define ALX_CAN_TRACE(...) ALX_TRACE_STD(ALX_CAN_FILE, __VA_ARGS__)
#else
	#define ALX_CAN_TRACE(...) do{} while (false)
#endif

// DbgPin //
#if defined(_ALX_CAN_DBG_PIN) || defined(_ALX_DBG_PIN_ALL)
	#define ALX_CAN_DBG_PIN(...) ALX_DBG_PIN_TOGGLE()
#else
	#define ALX_CAN_DBG_PIN(...) do{} while (false)
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
void AlxCan_Foreground_Handle(AlxCan* me);


#ifdef __cplusplus
}
#endif

#endif // ALX_CAN_H
