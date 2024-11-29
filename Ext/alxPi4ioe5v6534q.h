/**
  ******************************************************************************
  * @file		alxPi4ioe5v6534q.h
  * @brief		Auralix C Library - ALX IO Expander PI4IOE5V6534Q Module
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
#ifndef ALX_PI4IOE5V6534Q_H
#define ALX_PI4IOE5V6534Q_H

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
#include "alxI2c.h"
#include "alxDelay.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB)


//******************************************************************************
// Preprocessor
//******************************************************************************
#define ALX_PI4IOE5V6534Q_FILE "alxPi4ioe5v6534q.h"

// Assert //
#if defined(ALX_PI4IOE5V6534Q_ASSERT_BKPT_ENABLE)
	#define ALX_PI4IOE5V6534Q_ASSERT(expr) ALX_ASSERT_BKPT(ALX_PI4IOE5V6534Q_FILE, expr)
#elif defined(ALX_PI4IOE5V6534Q_ASSERT_TRACE_ENABLE)
	#define ALX_PI4IOE5V6534Q_ASSERT(expr) ALX_ASSERT_TRACE(ALX_PI4IOE5V6534Q_FILE, expr)
#elif defined(ALX_PI4IOE5V6534Q_ASSERT_RST_ENABLE)
	#define ALX_PI4IOE5V6534Q_ASSERT(expr) ALX_ASSERT_RST(ALX_PI4IOE5V6534Q_FILE, expr)
#else
	#define ALX_PI4IOE5V6534Q_ASSERT(expr) do{} while (false)
#endif

// Trace //
#if defined(ALX_PI4IOE5V6534Q_TRACE_ENABLE)
	#define ALX_PI4IOE5V6534Q_TRACE_WRN(...) ALX_TRACE_WRN(ALX_PI4IOE5V6534Q_FILE, __VA_ARGS__)
	#define ALX_PI4IOE5V6534Q_TRACE_ERR(...) ALX_TRACE_ERR(ALX_PI4IOE5V6534Q_FILE, __VA_ARGS__)
#else
	#define ALX_PI4IOE5V6534Q_TRACE_WRN(...) do{} while (false)
	#define ALX_PI4IOE5V6534Q_TRACE_ERR(...) do{} while (false)
#endif


//******************************************************************************
// Register Values Unions
//******************************************************************************


//------------------------------------------------------------------------------
// Types
//------------------------------------------------------------------------------
typedef union
{
	struct __attribute__((packed))
	{
		bool _0 : 1;
		bool _1 : 1;
		bool _2 : 1;
		bool _3 : 1;
		bool _4 : 1;
		bool _5 : 1;
		bool _6 : 1;
		bool _7 : 1;
	};
	uint8_t raw;
} AlxPi4ioe5v6534q_RegVal_Port;


//------------------------------------------------------------------------------
// Input Port Register Group
//------------------------------------------------------------------------------
typedef union
{
	struct __attribute__((packed))
	{
		AlxPi4ioe5v6534q_RegVal_Port P[5];
	};
	uint8_t raw[5];
} AlxPi4ioe5v6534q_RegGroupVal_0x00_InputPort;


//------------------------------------------------------------------------------
// Output Port Register Group
//------------------------------------------------------------------------------
typedef union
{
	struct __attribute__((packed))
	{
		AlxPi4ioe5v6534q_RegVal_Port P[5];
	};
	uint8_t raw[5];
} AlxPi4ioe5v6534q_RegGroupVal_0x05_OutputPort;


//------------------------------------------------------------------------------
// Configuration Port Register Group
//------------------------------------------------------------------------------
typedef union
{
	struct __attribute__((packed))
	{
		AlxPi4ioe5v6534q_RegVal_Port P[5];
	};
	uint8_t raw[5];
} AlxPi4ioe5v6534q_RegGroupVal_0x0F_ConfigurationPort;


//******************************************************************************
// Register Structures
//******************************************************************************
typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxPi4ioe5v6534q_RegGroupVal_0x00_InputPort val;
} AlxPi4ioe5v6534q_RegGroup_0x00_InputPort;

typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxPi4ioe5v6534q_RegGroupVal_0x05_OutputPort val;
} AlxPi4ioe5v6534q_RegGroup_0x05_OutputPort;

typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxPi4ioe5v6534q_RegGroupVal_0x0F_ConfigurationPort val;
} AlxPi4ioe5v6534q_RegGroup_0x0F_ConfigurationPort;


//******************************************************************************
// Main Register Structure
//******************************************************************************
typedef struct
{
	AlxPi4ioe5v6534q_RegGroup_0x00_InputPort _0x00_InputPort;
	AlxPi4ioe5v6534q_RegGroup_0x05_OutputPort _0x05_OutputPort;
	AlxPi4ioe5v6534q_RegGroup_0x0F_ConfigurationPort _0x0F_ConfigurationPort;
} AlxPi4ioe5v6534q_Reg;


//******************************************************************************
// Types
//******************************************************************************
typedef struct
{
	// Parameters
	AlxIoPin* do_nRESET;
	AlxI2c* i2c;
	uint8_t i2cAddr;
	bool i2cCheckWithRead;
	uint8_t i2cNumOfTries;
	uint16_t i2cTimeout_ms;

	// Variables
	AlxPi4ioe5v6534q_Reg reg;

	// Info
	bool wasCtorCalled;
	bool isInitPeriph;
	bool isInit;
} AlxPi4ioe5v6534q;


//******************************************************************************
// Constructor
//******************************************************************************
void AlxPi4ioe5v6534q_Ctor
(
	AlxPi4ioe5v6534q* me,
	AlxIoPin* do_nRESET,
	AlxI2c* i2c,
	uint8_t i2cAddr,
	bool i2cCheckWithRead,
	uint8_t i2cNumOfTries,
	uint16_t i2cTimeout_ms
);


//******************************************************************************
// Functions
//******************************************************************************
Alx_Status AlxPi4ioe5v6534q_InitPeriph(AlxPi4ioe5v6534q* me);
Alx_Status AlxPi4ioe5v6534q_DeInitPeriph(AlxPi4ioe5v6534q* me);
Alx_Status AlxPi4ioe5v6534q_Init(AlxPi4ioe5v6534q* me);
Alx_Status AlxPi4ioe5v6534q_DeInit(AlxPi4ioe5v6534q* me);
Alx_Status AlxPi4ioe5v6534q_Handle(AlxPi4ioe5v6534q* me, uint8_t inPortNum, uint8_t outPortNum);
bool AlxPi4ioe5v6534q_IoPin_Read(AlxPi4ioe5v6534q* me, uint8_t port, uint8_t pin);
void AlxPi4ioe5v6534q_IoPin_Write(AlxPi4ioe5v6534q* me, uint8_t port, uint8_t pin, bool val);
void AlxPi4ioe5v6534q_IoPin_Set(AlxPi4ioe5v6534q* me, uint8_t port, uint8_t pin);
void AlxPi4ioe5v6534q_IoPin_Reset(AlxPi4ioe5v6534q* me, uint8_t port, uint8_t pin);
void AlxPi4ioe5v6534q_IoPin_Toggle(AlxPi4ioe5v6534q* me, uint8_t port, uint8_t pin);


#endif	// #if defined(ALX_C_LIB)

#ifdef __cplusplus
}
#endif

#endif	// #ifndef ALX_PI4IOE5V6534Q_H
