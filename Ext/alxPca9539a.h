/**
  ******************************************************************************
  * @file		alxPca9539a.h
  * @brief		Auralix C Library - ALX IO Expander PCA9539A Module
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
#ifndef ALX_PCA9539A_H
#define ALX_PCA9539A_H

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
#define ALX_PCA9539A_FILE "alxPca9539a.h"

// Assert //
#if defined(ALX_PCA9539A_ASSERT_BKPT_ENABLE)
	#define ALX_PCA9539A_ASSERT(expr) ALX_ASSERT_BKPT(ALX_PCA9539A_FILE, expr)
#elif defined(ALX_PCA9539A_ASSERT_TRACE_ENABLE)
	#define ALX_PCA9539A_ASSERT(expr) ALX_ASSERT_TRACE(ALX_PCA9539A_FILE, expr)
#elif defined(ALX_PCA9539A_ASSERT_RST_ENABLE)
	#define ALX_PCA9539A_ASSERT(expr) ALX_ASSERT_RST(ALX_PCA9539A_FILE, expr)
#else
	#define ALX_PCA9539A_ASSERT(expr) do{} while (false)
#endif

// Trace //
#if defined(ALX_PCA9539A_TRACE_ENABLE)
	#define ALX_PCA9539A_TRACE_ERR(...) ALX_TRACE_ERR(ALX_PCA9539A_FILE, __VA_ARGS__)
#else
	#define ALX_PCA9539A_TRACE_ERR(...) do{} while (false)
#endif


//******************************************************************************
// Register Values Unions
//******************************************************************************

//------------------------------------------------------------------------------
// Input port register pair (00h, 01h)
//------------------------------------------------------------------------------
typedef union
{
	struct __attribute__((packed))
	{
		bool P0_0 : 1;
		bool P0_1 : 1;
		bool P0_2 : 1;
		bool P0_3 : 1;
		bool P0_4 : 1;
		bool P0_5 : 1;
		bool P0_6 : 1;
		bool P0_7 : 1;
	};
	uint8_t raw;
} AlxPca9539a_RegVal_00h_InputPort_0;
typedef union
{
	struct __attribute__((packed))
	{
		bool P1_0 : 1;
		bool P1_1 : 1;
		bool P1_2 : 1;
		bool P1_3 : 1;
		bool P1_4 : 1;
		bool P1_5 : 1;
		bool P1_6 : 1;
		bool P1_7 : 1;
	};
	uint8_t raw;
} AlxPca9539a_RegVal_01h_InputPort_1;

//------------------------------------------------------------------------------
// Output port register pair (02h, 03h)
//------------------------------------------------------------------------------
typedef union
{
	struct __attribute__((packed))
	{
		bool P0_0 : 1;
		bool P0_1 : 1;
		bool P0_2 : 1;
		bool P0_3 : 1;
		bool P0_4 : 1;
		bool P0_5 : 1;
		bool P0_6 : 1;
		bool P0_7 : 1;
	};
	uint8_t raw;
} AlxPca9539a_RegVal_02h_OutputPort_0;
typedef union
{
	struct __attribute__((packed))
	{
		bool P1_0 : 1;
		bool P1_1 : 1;
		bool P1_2 : 1;
		bool P1_3 : 1;
		bool P1_4 : 1;
		bool P1_5 : 1;
		bool P1_6 : 1;
		bool P1_7 : 1;
	};
	uint8_t raw;
} AlxPca9539a_RegVal_03h_OutputPort_1;

//------------------------------------------------------------------------------
// Polarity inversion register pair (04h, 05h)
//------------------------------------------------------------------------------
typedef union
{
	struct __attribute__((packed))
	{
		bool P0_0 : 1;
		bool P0_1 : 1;
		bool P0_2 : 1;
		bool P0_3 : 1;
		bool P0_4 : 1;
		bool P0_5 : 1;
		bool P0_6 : 1;
		bool P0_7 : 1;
	};
	uint8_t raw;
} AlxPca9539a_RegVal_04h_PolarityInversion_0;
typedef union
{
	struct __attribute__((packed))
	{
		bool P1_0 : 1;
		bool P1_1 : 1;
		bool P1_2 : 1;
		bool P1_3 : 1;
		bool P1_4 : 1;
		bool P1_5 : 1;
		bool P1_6 : 1;
		bool P1_7 : 1;
	};
	uint8_t raw;
} AlxPca9539a_RegVal_05h_PolarityInversion_1;

//------------------------------------------------------------------------------
// Configuration register pair (06h, 07h)
//------------------------------------------------------------------------------
typedef union
{
	struct __attribute__((packed))
	{
		bool P0_0 : 1;
		bool P0_1 : 1;
		bool P0_2 : 1;
		bool P0_3 : 1;
		bool P0_4 : 1;
		bool P0_5 : 1;
		bool P0_6 : 1;
		bool P0_7 : 1;
	};
	uint8_t raw;
} AlxPca9539a_RegVal_06h_Configuration_0;
typedef union
{
	struct __attribute__((packed))
	{
		bool P1_0 : 1;
		bool P1_1 : 1;
		bool P1_2 : 1;
		bool P1_3 : 1;
		bool P1_4 : 1;
		bool P1_5 : 1;
		bool P1_6 : 1;
		bool P1_7 : 1;
	};
	uint8_t raw;
} AlxPca9539a_RegVal_07h_Configuration_1;
typedef union
{
	struct __attribute__((packed))
	{
		bool P1_0 : 1;
		bool P1_1 : 1;
		bool P1_2 : 1;
		bool P1_3 : 1;
		bool P1_4 : 1;
		bool P1_5 : 1;
		bool P1_6 : 1;
		bool P1_7 : 1;
	};
	uint8_t raw;
} AlxPca9539a_RegVal_OpenDrain_0;
typedef union
{
	struct __attribute__((packed))
	{
		bool P1_0 : 1;
		bool P1_1 : 1;
		bool P1_2 : 1;
		bool P1_3 : 1;
		bool P1_4 : 1;
		bool P1_5 : 1;
		bool P1_6 : 1;
		bool P1_7 : 1;
	};
	uint8_t raw;
} AlxPca9539a_RegVal_OpenDrain_1;
//******************************************************************************
// Register Structures
//******************************************************************************
typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxPca9539a_RegVal_00h_InputPort_0 val;
} AlxPca9539a_Reg_00h_InputPort_0;
typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxPca9539a_RegVal_01h_InputPort_1 val;
} AlxPca9539a_Reg_01h_InputPort_1;
typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxPca9539a_RegVal_02h_OutputPort_0 val;
} AlxPca9539a_Reg_02h_OutputPort_0;
typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxPca9539a_RegVal_03h_OutputPort_1 val;
} AlxPca9539a_Reg_03h_OutputPort_1;
typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxPca9539a_RegVal_04h_PolarityInversion_0 val;
} AlxPca9539a_Reg_04h_PolarityInversion_0;
typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxPca9539a_RegVal_05h_PolarityInversion_1 val;
} AlxPca9539a_Reg_05h_PolarityInversion_1;
typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxPca9539a_RegVal_06h_Configuration_0 val;
} AlxPca9539a_Reg_06h_Configuration_0;
typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxPca9539a_RegVal_07h_Configuration_1 val;
} AlxPca9539a_Reg_07h_Configuration_1;

//******************************************************************************
// Main Register Structure
//******************************************************************************
typedef struct
{
	AlxPca9539a_Reg_00h_InputPort_0 _00h_InputPort_0;
	AlxPca9539a_Reg_01h_InputPort_1 _01h_InputPort_1;
	AlxPca9539a_Reg_02h_OutputPort_0 _02h_OutputPort_0;
	AlxPca9539a_Reg_03h_OutputPort_1 _03h_OutputPort_1;
	AlxPca9539a_Reg_04h_PolarityInversion_0 _04h_PolarityInversion_0;
	AlxPca9539a_Reg_05h_PolarityInversion_1 _05h_PolarityInversion_1;
	AlxPca9539a_Reg_06h_Configuration_0 _06h_Configuration_0;
	AlxPca9539a_Reg_07h_Configuration_1 _07h_Configuration_1;
} AlxPca9539a_Reg;


//******************************************************************************
// Types
//******************************************************************************
typedef struct
{
	// Parameters
	AlxI2c* i2c;
	uint8_t i2cAddr;
	bool i2cCheckWithRead;
	uint8_t i2cNumOfTries;
	uint16_t i2cTimeout_ms;

	// Variables
	AlxPca9539a_Reg reg;
	AlxPca9539a_RegVal_OpenDrain_0 _OpenDrain_0;
	AlxPca9539a_RegVal_OpenDrain_1 _OpenDrain_1;

	// Info
	bool wasCtorCalled;
	bool isInitPeriph;
	bool isInit;
} AlxPca9539a;


//******************************************************************************
// Constructor
//******************************************************************************
void AlxPca9539a_Ctor
(
	AlxPca9539a* me,
	AlxI2c* i2c,
	uint8_t i2cAddr,
	bool i2cCheckWithRead,
	uint8_t i2cNumOfTries,
	uint16_t i2cTimeout_ms
);


//******************************************************************************
// Functions
//******************************************************************************
Alx_Status AlxPca9539a_InitPeriph(AlxPca9539a* me);
Alx_Status AlxPca9539a_DeInitPeriph(AlxPca9539a* me);
Alx_Status AlxPca9539a_Init(AlxPca9539a* me);
Alx_Status AlxPca9539a_DeInit(AlxPca9539a* me);
Alx_Status AlxPca9539a_Handle(AlxPca9539a* me);
Alx_Status AlxPca9539a_irqHandle(AlxPca9539a* me);

bool AlxPca9539a_IoPin_Read(AlxPca9539a* me, uint8_t port, uint8_t pin);
uint8_t AlxPca9539a_Read_Port_Raw(AlxPca9539a* me, uint8_t port);
void AlxPca9539a_IoPin_Write(AlxPca9539a* me, uint8_t port, uint8_t pin, bool val);
void AlxPca9539a_IoPin_Set(AlxPca9539a* me, uint8_t port, uint8_t pin);
void AlxPca9539a_IoPin_Reset(AlxPca9539a* me, uint8_t port, uint8_t pin);
void AlxPca9539a_IoPin_Toggle(AlxPca9539a* me, uint8_t port, uint8_t pin);
Alx_Status AlxPca9539a_Reg_Write(AlxPca9539a* me, void* reg);
Alx_Status AlxPca9539a_Reg_Read(AlxPca9539a* me, void* reg);


#endif	// #if defined(ALX_C_LIB)

#ifdef __cplusplus
}
#endif

#endif	// #ifndef ALX_PCA9539A_H
