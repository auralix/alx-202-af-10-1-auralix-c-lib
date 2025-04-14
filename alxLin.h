/**
  ******************************************************************************
  * @file		alxLin.h
  * @brief		Auralix C Library - ALX LIN Module
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
#ifndef ALX_LIN_H
#define ALX_LIN_H

#ifdef __cplusplus
extern "C" {
#endif


//******************************************************************************
// Includes
//******************************************************************************
#include "alxGlobal.h"
#include "alxAssert.h"
#include "alxTrace.h"
#include "alxSerialPort.h"
#include "alxTimSw.h"
#include "alxOsDelay.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB)


//******************************************************************************
// Preprocessor
//******************************************************************************
#define ALX_LIN_FILE "alxLin.h"

// Assert //
#if defined(ALX_LIN_ASSERT_BKPT_ENABLE)
	#define ALX_LIN_ASSERT(expr) ALX_ASSERT_BKPT(ALX_LIN_FILE, expr)
#elif defined(ALX_LIN_ASSERT_TRACE_ENABLE)
	#define ALX_LIN_ASSERT(expr) ALX_ASSERT_TRACE(ALX_LIN_FILE, expr)
#elif defined(ALX_LIN_ASSERT_RST_ENABLE)
	#define ALX_LIN_ASSERT(expr) ALX_ASSERT_RST(ALX_LIN_FILE, expr)
#else
	#define ALX_LIN_ASSERT(expr) do{} while (false)
#endif

// Trace //
#if defined(ALX_LIN_TRACE_ENABLE)
	#define ALX_LIN_TRACE_FTL(...) ALX_TRACE_FTL(ALX_LIN_FILE, __VA_ARGS__)
	#define ALX_LIN_TRACE_ERR(...) ALX_TRACE_ERR(ALX_LIN_FILE, __VA_ARGS__)
	#define ALX_LIN_TRACE_WRN(...) ALX_TRACE_WRN(ALX_LIN_FILE, __VA_ARGS__)
	#define ALX_LIN_TRACE_INF(...) ALX_TRACE_INF(ALX_LIN_FILE, __VA_ARGS__)
	#define ALX_LIN_TRACE_DBG(...) ALX_TRACE_DBG(ALX_LIN_FILE, __VA_ARGS__)
	#define ALX_LIN_TRACE_VRB(...) ALX_TRACE_VRB(ALX_LIN_FILE, __VA_ARGS__)
#else
	#define ALX_LIN_TRACE_FTL(...) do{} while (false)
	#define ALX_LIN_TRACE_ERR(...) do{} while (false)
	#define ALX_LIN_TRACE_WRN(...) do{} while (false)
	#define ALX_LIN_TRACE_INF(...) do{} while (false)
	#define ALX_LIN_TRACE_DBG(...) do{} while (false)
	#define ALX_LIN_TRACE_VRB(...) do{} while (false)
#endif

// Defines
#ifndef ALX_LIN_BUFF_LEN
	#define ALX_LIN_BUFF_LEN 16
#endif


//******************************************************************************
// Types
//******************************************************************************
typedef struct
{
	uint8_t id;
	uint8_t protectedId;
	uint8_t dataLen;
	uint8_t data[ALX_LIN_BUFF_LEN];
	uint8_t enhancedChecksum;
} AlxLin_Frame;

typedef struct
{
	uint8_t id;
	uint8_t dataLen;
	bool publish;
} AlxLin_FrameConfig;

typedef struct
{
	uint8_t buff[ALX_LIN_BUFF_LEN];
	uint8_t i;
	AlxLin_Frame frame;
	AlxLin_FrameConfig frameConfig;
	bool active;
} AlxLin_RxBuffStruct;

typedef struct
{
	// Parameters
	AlxSerialPort* alxSerialPort;
	bool masterReadSwHandleBreak;
	uint8_t breakSyncOffset;
	AlxLin_FrameConfig* frameConfigArr;
	uint8_t frameConfigArrLen;

	// Variables
	AlxLin_RxBuffStruct rxb;

	// Info
	bool wasCtorCalled;
	bool isInitMaster;
	bool isInitSlave;
} AlxLin;


//******************************************************************************
// Constructor
//******************************************************************************
void AlxLin_Ctor
(
	AlxLin* me,
	AlxSerialPort* alxSerialPort,
	bool masterReadSwHandleBreak,
	uint8_t breakSyncOffset,
	AlxLin_FrameConfig* frameConfigArr,
	uint8_t frameConfigArrLen
);


//******************************************************************************
// Functions
//******************************************************************************


//------------------------------------------------------------------------------
// Master
//------------------------------------------------------------------------------
Alx_Status AlxLin_Master_Init(AlxLin* me);
Alx_Status AlxLin_Master_DeInit(AlxLin* me);
bool AlxLin_Master_IsInit(AlxLin* me);
Alx_Status AlxLin_Master_Read(AlxLin* me, uint8_t id, uint8_t* data, uint32_t len, uint16_t slaveResponseWaitTime_ms, uint8_t numOfTries, bool variableLen, uint32_t variableLen_maxLen, uint32_t* variableLen_actualLen);
Alx_Status AlxLin_Master_Publish(AlxLin* me, AlxLin_Frame frame);


//------------------------------------------------------------------------------
// Slave
//------------------------------------------------------------------------------
Alx_Status AlxLin_Slave_Init(AlxLin* me);
Alx_Status AlxLin_Slave_DeInit(AlxLin* me);
bool AlxLin_Slave_IsInit(AlxLin* me);
Alx_Status AlxLin_Slave_Subscribe(AlxLin* me, AlxLin_Frame* frame, uint16_t timeout_ms, uint16_t rxFifoNumOfEntriesNewCheckWaitTime_ms);


//------------------------------------------------------------------------------
// RX Buffer
//------------------------------------------------------------------------------
void AlxLin_RxBuff_Flush(AlxLin* me);
void AlxLin_RxBuff_Handle(AlxLin* me, uint8_t data);


//------------------------------------------------------------------------------
// IRQ
//------------------------------------------------------------------------------
void AlxLin_Irq_Handle(AlxLin* me);


#endif	// #if defined(ALX_C_LIB)

#ifdef __cplusplus
}
#endif

#endif	// #ifndef ALX_LIN_H
