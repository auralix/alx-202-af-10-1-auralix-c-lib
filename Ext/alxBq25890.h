/**
  ******************************************************************************
  * @file		alxBq25890.h
  * @brief		Auralix C Library - ALX Battery charger
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
#ifndef ALX_BQ25890_H
#define ALX_BQ25890_H

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
#define ALX_BQ25890_FILE "alxBq25890.h"

// Assert //
#if defined(ALX_BQ25890_ASSERT_BKPT_ENABLE)
	#define ALX_BQ25890_ASSERT(expr) ALX_ASSERT_BKPT(ALX_BQ25890_FILE, expr)
#elif defined(ALX_BQ25890_ASSERT_TRACE_ENABLE)
	#define ALX_BQ25890_ASSERT(expr) ALX_ASSERT_TRACE(ALX_BQ25890_FILE, expr)
#elif defined(ALX_BQ25890_ASSERT_RST_ENABLE)
	#define ALX_BQ25890_ASSERT(expr) ALX_ASSERT_RST(ALX_BQ25890_FILE, expr)
#else
	#define ALX_BQ25890_ASSERT(expr) do{} while (false)
#endif

// Trace //
#if defined(ALX_BQ25890_TRACE_ENABLE)
	#define ALX_BQ25890_TRACE(...) ALX_TRACE_WRN(ALX_BQ25890_FILE, __VA_ARGS__)
#else
	#define ALX_BQ25890_TRACE(...) do{} while (false)
#endif

//******************************************************************************
// Faults
//******************************************************************************
typedef union
{
	struct __attribute__((packed))
	{
		uint8_t NTC_FAULT	: 3;
		uint8_t CHRG_FAULT	: 2;
		bool BAT_FAULT		: 1;
		uint8_t unused		: 2;
	};
	uint8_t raw;

}AlxBq2890_faults_t;

	//******************************************************************************
// Register Values Unions
//******************************************************************************

//------------------------------------------------------------------------------
// Device registers
//------------------------------------------------------------------------------
typedef union
{
	struct __attribute__((packed))
	{
		uint8_t IINLIM	: 6;
		bool EN_ILIM	: 1;
		bool EN_HIZ		: 1;
	};
	uint8_t raw;
} AlxBq25890_Reg_00_bits;

typedef union
{
	struct __attribute__((packed))
	{
		bool PUMPX_DN		: 1;
		bool PUMPX_UP		: 1;
		bool BATFET_RST_EN	: 1;
		bool BATFET_DLY		: 1;
		bool JEITA_VSET		: 1;
		bool BATFET_DIS		: 1;
		bool TMR2X_EN		: 1;
		bool FORCE_ICO		: 1;
	};
	uint8_t raw;
} AlxBq25890_Reg_09_bits;

typedef union
{
	struct __attribute__((packed))
	{
		bool VSYS_STAT			: 1;
		bool RESERVED			: 1;
		bool PG_STAT			: 1;
		uint8_t CHRG_STAT		: 2;
		uint8_t VBUS_STAT		: 3;
	};
	uint8_t raw;
} AlxBq25890_Reg_0B_bits;

typedef union
{
	struct __attribute__((packed))
	{
		uint8_t NTC_FAULT		: 3;
		bool BAT_FAULT			: 1;
		uint8_t CHGR_FAULT		: 2;
		bool BOOST_FAULT		: 1;
		bool WATCHDOG_FAULT		: 1;
	};
	uint8_t raw;
} AlxBq25890_Reg_0C_bits;

//******************************************************************************
// Register Structures
//******************************************************************************
typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxBq25890_Reg_00_bits val;
} AlxBq25890_Reg_00;

typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxBq25890_Reg_09_bits val;
} AlxBq25890_Reg_09;

typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxBq25890_Reg_0B_bits val;
} AlxBq25890_Reg_0B;

typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxBq25890_Reg_0C_bits val;
} AlxBq25890_Reg_0C;
//******************************************************************************
// Main Register Structure
//******************************************************************************
typedef struct
{
	AlxBq25890_Reg_00 REG_00;
	AlxBq25890_Reg_09 REG_09;
	AlxBq25890_Reg_0B REG_0B;
	AlxBq25890_Reg_0C REG_0C;
} AlxBq25890_Reg;


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

	// IC Ctor specific configuration
	uint16_t IINLIM;
	bool JEITA_VSET;
	bool BATFET_DIS;

	// Variables
	AlxBq25890_Reg reg;

	// Info
	bool wasCtorCalled;
	bool isInitPeriph;
	bool isInit;
} AlxBq25890;


//******************************************************************************
// Constructor
//******************************************************************************
void AlxBq25890_Ctor
(
	AlxBq25890* me,
	AlxI2c* i2c,
	uint8_t IINLIM,
	bool JEITA_VSET,
	bool BATFET_DIS
);


//******************************************************************************
// Functions
//******************************************************************************
Alx_Status AlxBq25890_InitPeriph(AlxBq25890* me);
Alx_Status AlxBq25890_DeInitPeriph(AlxBq25890* me);
Alx_Status AlxBq25890_Init(AlxBq25890* me);
Alx_Status AlxBq25890_DeInit(AlxBq25890* me);
Alx_Status AlxBq25890_Poll(AlxBq25890* me);
Alx_Status AlxBq25890_Read(AlxBq25890* me, AlxBq25890_Reg *reg);
Alx_Status AlxBq25890_GetFaults(AlxBq25890* me, AlxBq2890_faults_t *faults);
Alx_Status AlxBq25890_GetChargingStatus(AlxBq25890* me, uint8_t *status);
Alx_Status AlxBq25890_irqHandle(AlxBq25890* me);
Alx_Status AlxBq25890_set_IINLIM(AlxBq25890* me, uint8_t value);
Alx_Status AlxBq25890_set_JEITA_VSET(AlxBq25890* me, bool value);
Alx_Status AlxBq25890_set_shipping_mode(AlxBq25890* me);


bool AlxBq25890_IoPin_Read(AlxBq25890* me, uint8_t port, uint8_t pin);
uint8_t AlxBq25890_Read_Port_Raw(AlxBq25890* me, uint8_t port);
void AlxBq25890_IoPin_Write(AlxBq25890* me, uint8_t port, uint8_t pin, bool val);
void AlxBq25890_IoPin_Set(AlxBq25890* me, uint8_t port, uint8_t pin);
void AlxBq25890_IoPin_Reset(AlxBq25890* me, uint8_t port, uint8_t pin);
void AlxBq25890_IoPin_Toggle(AlxBq25890* me, uint8_t port, uint8_t pin);
Alx_Status AlxBq25890_Reg_Write(AlxBq25890* me, void* reg);
Alx_Status AlxBq25890_Reg_Read(AlxBq25890* me, void* reg);


#endif	// #if defined(ALX_C_LIB)

#ifdef __cplusplus
}
#endif

#endif	// #ifndef ALX_BQ25890_H
