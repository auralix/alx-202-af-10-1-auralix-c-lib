/**
  ******************************************************************************
  * @file		alxLp586x.c
  * @brief		Auralix C Library - ALX LED Driver LP586x Module
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
#include "alxLp586x.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB)


//******************************************************************************
// Private Functions
//******************************************************************************
static void AlxLp586x_RegStruct_SetAddr(AlxLp586x* me);
static void AlxLp586x_RegStruct_SetLen(AlxLp586x* me);
static void AlxLp586x_RegStruct_SetValToZero(AlxLp586x* me);
static void AlxLp586x_RegStruct_SetValToDefault(AlxLp586x* me);
static Alx_Status AlxLp586x_RegStruct_Write(AlxLp586x* me);

//******************************************************************************
// Weak Functions
//******************************************************************************
void AlxLp586x_RegStruct_SetVal(AlxLp586x* me);


//******************************************************************************
// Constructor
//******************************************************************************

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		i2c
  * @param[in]		i2cAddr
  * @param[in]		i2cCheckWithRead
  * @param[in]		i2cNumOfTries
  * @param[in]		i2cTimeout_ms
  */
void AlxLp586x_Ctor
(
	AlxLp586x* me,
	AlxIoPin* do_LED_DRV_EN,
	AlxIoPin* do_LED_DRV_SYNC,
	AlxI2c* i2c,
	uint8_t i2cAddr,
	bool i2cCheckWithRead,
	uint8_t i2cNumOfTries,
	uint16_t i2cTimeout_ms
)
{
	// Parameters
	me->do_LED_DRV_EN = do_LED_DRV_EN;
	me->do_LED_DRV_SYNC = do_LED_DRV_SYNC;
	me->i2c = i2c;
	me->i2cAddr = i2cAddr;
	me->i2cCheckWithRead = i2cCheckWithRead;
	me->i2cNumOfTries = i2cNumOfTries;
	me->i2cTimeout_ms = i2cTimeout_ms;

	// Variables
	AlxLp586x_RegStruct_SetAddr(me);
	AlxLp586x_RegStruct_SetLen(me);
	AlxLp586x_RegStruct_SetValToZero(me);

	// Info
	me->wasCtorCalled = true;
	me->isInitPeriph = false;
	me->isInit = false;
}


//******************************************************************************
// Functions
//******************************************************************************

/**
  * @brief
  * @param[in,out]	me
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxLp586x_InitPeriph(AlxLp586x* me)
{
	// Assert
	ALX_LP586x_ASSERT(me->wasCtorCalled == true);
	ALX_LP586x_ASSERT(me->isInitPeriph == false);
	// isInit -> Don't care

	// Local variables
	Alx_Status status = Alx_Err;

	// Init I2C
	status = AlxI2c_Init(me->i2c);
	if (status != Alx_Ok)
	{
		ALX_LP586x_TRACE_ERR("FAIL: AlxI2c_Init() status %ld", status);
		return status;
	}

	// Set isInitPeriph
	me->isInitPeriph = true;

	// Return
	return Alx_Ok;
}

/**
  * @brief
  * @param[in,out]	me
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxLp586x_DeInitPeriph(AlxLp586x* me)
{
	// Assert
	ALX_LP586x_ASSERT(me->wasCtorCalled == true);
	ALX_LP586x_ASSERT(me->isInitPeriph == true);
	// isInit -> Don't care

	// Local variables
	Alx_Status status = Alx_Err;

	// DeInit I2C
	status = AlxI2c_DeInit(me->i2c);
	if (status != Alx_Ok)
	{
		ALX_LP586x_TRACE_ERR("FAIL: AlxI2c_DeInit() status %ld", status);
		return status;
	}

	// Reset isInitPeriph
	me->isInitPeriph = false;

	// Return
	return Alx_Ok;
}

/**
  * @brief
  * @param[in,out]	me
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxLp586x_Init(AlxLp586x* me)
{
	// Assert
	ALX_LP586x_ASSERT(me->wasCtorCalled == true);
	ALX_LP586x_ASSERT(me->isInitPeriph == true);
	ALX_LP586x_ASSERT(me->isInit == false);

	// Local variables
	Alx_Status status = Alx_Err;

	// Init GPIO
	AlxIoPin_Init(me->do_LED_DRV_EN);
	AlxIoPin_Init(me->do_LED_DRV_SYNC);

	// Check if slave is ready
	status = AlxI2c_Master_IsSlaveReady(me->i2c, me->i2cAddr, me->i2cNumOfTries, me->i2cTimeout_ms);
	if (status != Alx_Ok)
	{
		ALX_LP586x_TRACE_ERR("FAIL: AlxI2c_Master_IsSlaveReady() status %ld", status);
		return status;
	}

	// Set register struct values to default
	AlxLp586x_RegStruct_SetValToDefault(me);

	// Set register struct values - WEAK
	AlxLp586x_RegStruct_SetVal(me);

	// Set GPIO do_LED_DRV_EN
	AlxIoPin_Set(me->do_LED_DRV_EN);

	// Write register struct - WEAK
	status = AlxLp586x_RegStruct_Write(me);
	if (status != Alx_Ok)
	{
		ALX_LP586x_TRACE_ERR("FAIL: AlxLp586x_RegStruct_Write() status %ld", status);
		return status;
	}

	// Set GPIO do_LED_DRV_SYNC - turn on all leds
	AlxIoPin_Set(me->do_LED_DRV_SYNC);

	// Set isInit
	me->isInit = true;

	// Return
	return Alx_Ok;
}

/**
  * @brief
  * @param[in,out]	me
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxLp586x_DeInit(AlxLp586x* me)
{
	// Assert
	ALX_LP586x_ASSERT(me->wasCtorCalled == true);
	ALX_LP586x_ASSERT(me->isInitPeriph == true);
	ALX_LP586x_ASSERT(me->isInit == true);

	// Disable device
	AlxIoPin_Reset(me->do_LED_DRV_EN);

	// DeInit GPIO
	AlxIoPin_DeInit(me->do_LED_DRV_EN);
	AlxIoPin_DeInit(me->do_LED_DRV_SYNC);

	// Reset isInit
	me->isInit = false;

	// Return
	return Alx_Ok;
}

/**
  * @brief
  * @param[in,out]	me
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxLp586x_Handle(AlxLp586x* me)
{
	// Assert
	ALX_LP586x_ASSERT(me->wasCtorCalled == true);
	ALX_LP586x_ASSERT(me->isInitPeriph == true);
	ALX_LP586x_ASSERT(me->isInit == true);

	// Return
	return Alx_Ok;
}

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		ledNum
  * @retval			false
  * @retval			true
  */

Alx_Status AlxLp586x_Reg_Write(AlxLp586x* me, void* reg)
{
	// Assert
	ALX_LP586x_ASSERT(me->wasCtorCalled == true);
	ALX_LP586x_ASSERT(me->isInitPeriph == true);
	// isInit -> Don't care

	// Local variables
	Alx_Status status = Alx_Err;
	uint8_t regAddr = *((uint8_t*)reg);
	uint8_t regLen = *((uint8_t*)reg + sizeof(regAddr));
	uint8_t* regValPtr = (uint8_t*)reg + sizeof(regAddr) + sizeof(regLen);

	// Write
	status = AlxI2c_Master_StartWriteMemStop_Multi(me->i2c, me->i2cAddr, regAddr, AlxI2c_Master_MemAddrLen_8bit, regValPtr, regLen, me->i2cCheckWithRead, me->i2cNumOfTries, me->i2cTimeout_ms);
	if (status != Alx_Ok) { ALX_LP586x_TRACE_ERR("Err"); return status; }

	// Return
	return Alx_Ok;
}

void AlxLp586x_Led_Write(AlxLp586x* me, uint8_t ledNum, bool val)
{
	// Assert
	ALX_LP586x_ASSERT(me->wasCtorCalled == true);
	// isInitPeriph -> Don't care
	// isInit -> Don't care
	ALX_LP586x_ASSERT(ledNum <= 107);
}


//******************************************************************************
// Private Functions
//******************************************************************************
static void AlxLp586x_RegStruct_SetAddr(AlxLp586x* me)
{
	// Core registers:
	me->reg._0x00_Chip_En.addr					= 0x00;
	me->reg._0x01_Dev_initial.addr				= 0x01;
	me->reg._0x02_Dev_config1.addr				= 0x02;
	me->reg._0x03_Dev_config2.addr				= 0x03;
	me->reg._0x04_Dev_config3.addr				= 0x04;
	me->reg._0x05_Global_bri.addr				= 0x05;

	// 0x06–0x08: Group brightness
	for (uint8_t i = 0; i < 3; ++i)
	{
		me->reg._group_bri[i].addr				= (uint8_t)(0x06 + i);
	}

	// 0x09…0x0B: CC-current-set
	for (uint8_t i = 0; i < 3; ++i)
	{
		me->reg._current_set_cc[i].addr			= (uint8_t)(0x09 + i);
	}

	// 0x0C–0x29: Dot group-select for L0…L5, CS0…CS17
	for (int i = 0; i < 30; ++i)
	{
		me->reg._dot_grp_sel[i].addr			= (uint8_t)(0x0C + i);
	}

	// 0x43–0x54: Dot on/off registers (L0…L5, CS0…CS7)
	for (uint8_t i = 0; i < 18; ++i)
	{
		me->reg._dot_onoff[i].addr				= 0x43 + i;
	}

	// Fault_state (0x64):
	me->reg._0x64_Fault_state.addr				= 0x64;

	// LOD (0x65–0x76):
	for (uint8_t i = 0; i < 18; ++i)
	{
		me->reg._dot_lod[i].addr				= (uint8_t)(0x65 + i);
	}

	// LSD (0x86–0x97):
	for (uint8_t i = 0; i < 18; ++i)
	{
		me->reg._dot_lsd[i].addr				= (uint8_t)(0x86 + i);
	}

	// Clears and reset registers (0xA7–0xA9):
	me->reg._0xA7_LOD_clear.addr				= 0xA7;
	me->reg._0xA8_LSD_clear.addr				= 0xA8;
	me->reg._0xA9_Reset.addr					= 0xA9;

	// Dot-current (DC0…DC107) (0x100–0x16B):
	for (uint16_t i = 0; i < 108; ++i)
	{
		me->reg._dot_curr[i].addr				= (uint16_t)(0x100 + i);
	}

	//  Dot current registers (pwm_bri0…pwm_bri215 ) (0x200–0x2D7):
	for (uint16_t i = 0; i < 216; ++i)
	{
		me->reg._pwm_bri[i].addr				= (uint16_t)(0x200 + i);
	}
}
static void AlxLp586x_RegStruct_SetLen(AlxLp586x* me)
{
	// Core registers:
	me->reg._0x00_Chip_En.len					= sizeof(me->reg._0x00_Chip_En.val);
	me->reg._0x01_Dev_initial.len				= sizeof(me->reg._0x01_Dev_initial.val);
	me->reg._0x02_Dev_config1.len				= sizeof(me->reg._0x02_Dev_config1.val);
	me->reg._0x03_Dev_config2.len				= sizeof(me->reg._0x03_Dev_config2.val);
	me->reg._0x04_Dev_config3.len				= sizeof(me->reg._0x04_Dev_config3.val);
	me->reg._0x05_Global_bri.len				= sizeof(me->reg._0x05_Global_bri.val);

	// 0x06–0x08: Group brightness
	for (uint8_t i = 0; i < 3; ++i)
	{
		me->reg._group_bri[i].len				= sizeof(me->reg._group_bri[i].val);
	}

	// 0x09…0x0B: CC-current-set
	for (uint8_t i = 0; i < 3; ++i)
	{
		me->reg._current_set_cc[i].len			= sizeof(me->reg._current_set_cc[i].val);
	}

	// 0x0C–0x29: Dot group-select for L0…L5, CS0…CS17
	for (int i = 0; i < 30; ++i)
	{
		// Each entry is exactly one byte
		me->reg._dot_grp_sel[i].len				= sizeof(me->reg._dot_grp_sel[i].val);
	}

	// 0x43–0x54: Dot on/off registers (L0…L5, CS0…CS7)
	for (uint8_t i = 0; i < 18; ++i)
	{
		me->reg._dot_onoff[i].len				= sizeof(me->reg._dot_onoff[i].val);
	}

	// Fault_state (0x64):
	me->reg._0x64_Fault_state.len				= sizeof(me->reg._0x64_Fault_state.val);

	// LOD (0x65–0x76):
	for (uint8_t i = 0; i < 18; ++i)
	{
		me->reg._dot_lod[i].len					= sizeof(me->reg._dot_lod[i].val);
	}

	// LSD (0x86–0x97):
	for (uint8_t i = 0; i < 18; ++i)
	{
		me->reg._dot_lsd[i].len					= sizeof(me->reg._dot_lsd[i].val);
	}

	// Clears and reset registers (0xA7–0xA9):
	me->reg._0xA7_LOD_clear.len					= sizeof(me->reg._0xA7_LOD_clear.val);
	me->reg._0xA8_LSD_clear.len					= sizeof(me->reg._0xA8_LSD_clear.val);
	me->reg._0xA9_Reset.len						= sizeof(me->reg._0xA9_Reset.val);

	// Dot-current (DC0…DC107) (0x100–0x16B):
	for (uint16_t i = 0; i < 108; ++i)
	{
		me->reg._dot_curr[i].len				= sizeof(me->reg._dot_curr[i].val);
	}

	//  Dot current registers (pwm_bri0…pwm_bri215 ) (0x200–0x2D7):
	for (uint16_t i = 0; i < 216; ++i)
	{
		me->reg._pwm_bri[i].len					= sizeof(me->reg._pwm_bri[i].val);
	}
}
static void AlxLp586x_RegStruct_SetValToZero(AlxLp586x* me)
{
	// Core registers:
	memset(&me->reg._0x00_Chip_En.val.raw,			0x00, sizeof(me->reg._0x00_Chip_En.val.raw));
	memset(&me->reg._0x01_Dev_initial.val.raw,		0x00, sizeof(me->reg._0x01_Dev_initial.val.raw));
	memset(&me->reg._0x02_Dev_config1.val.raw,		0x00, sizeof(me->reg._0x02_Dev_config1.val.raw));
	memset(&me->reg._0x03_Dev_config2.val.raw,		0x00, sizeof(me->reg._0x03_Dev_config2.val.raw));
	memset(&me->reg._0x04_Dev_config3.val.raw,		0x00, sizeof(me->reg._0x04_Dev_config3.val.raw));
	memset(&me->reg._0x05_Global_bri.val.raw,		0x00, sizeof(me->reg._0x05_Global_bri.val.raw));

	// 0x06–0x08: Group brightness
	for (uint8_t i = 0; i < 3; ++i)
	{
		memset(&me->reg._group_bri[i].val.raw,		0x00, sizeof(me->reg._group_bri[i].val.raw));
	}

	// 0x09…0x0B: CC-current-set
	for (uint8_t i = 0; i < 3; ++i)
	{
		memset(&me->reg._current_set_cc[i].val.raw, 0x00, sizeof(me->reg._current_set_cc[i].val.raw));
	}

	// 0x0C–0x29: Dot group-select for L0…L5, CS0…CS17
	for (int i = 0; i < 30; ++i)
	{
		memset(&me->reg._dot_grp_sel[i].val.raw,	0x00, sizeof(me->reg._dot_grp_sel[i].val.raw));
	}


	// 0x43–0x54: Dot on/off registers (L0…L5, CS0…CS7)
	for (uint8_t i = 0; i < 18; ++i)
	{
		memset(&me->reg._dot_onoff[i].val.raw,		0x00, sizeof(me->reg._dot_onoff[i].val.raw));
	}

	// Fault_state (0x64):
	memset(&me->reg._0x64_Fault_state.val.raw,		0x00, sizeof(me->reg._0x64_Fault_state.val.raw));

	// LOD (0x65–0x76):
	for (uint8_t i = 0; i < 18; ++i)
	{
		memset(&me->reg._dot_lod[i].val.raw,		0x00, sizeof(me->reg._dot_lod[i].val.raw));
	}

	// LSD (0x86–0x97):
	for (uint8_t i = 0; i < 18; ++i)
	{
		memset(&me->reg._dot_lsd[i].val.raw,		0x00, sizeof(me->reg._dot_lsd[i].val.raw));
	}

	// Clears and reset registers (0xA7–0xA9):
	memset(&me->reg._0xA7_LOD_clear.val.raw,		0x00, sizeof(me->reg._0xA7_LOD_clear.val.raw));
	memset(&me->reg._0xA8_LSD_clear.val.raw,		0x00, sizeof(me->reg._0xA8_LSD_clear.val.raw));
	memset(&me->reg._0xA9_Reset.val.raw,			0x00, sizeof(me->reg._0xA9_Reset.val.raw));

	// Dot-current (DC0…DC107) (0x100–0x16B):
	for (uint16_t i = 0; i < 108; ++i)
	{
		memset(&me->reg._dot_curr[i].val.raw,		0x00, sizeof(me->reg._dot_curr[i].val.raw));
	}

	//  Dot current registers (pwm_bri0…pwm_bri215 ) (0x200–0x2D7):
	for (uint16_t i = 0; i < 216; ++i)
	{
		memset(&me->reg._pwm_bri[i].val.raw, 0x00, sizeof(me->reg._pwm_bri[i].val.raw));
	}
}
static void AlxLp586x_RegStruct_SetValToDefault(AlxLp586x* me)
{
	// Core registers:
	memset(&me->reg._0x00_Chip_En.val.raw,			0x00, sizeof(me->reg._0x00_Chip_En.val.raw));     // Reset = 0x00
	memset(&me->reg._0x01_Dev_initial.val.raw,		0x5E, sizeof(me->reg._0x01_Dev_initial.val.raw)); // Reset = 0x5E
	memset(&me->reg._0x02_Dev_config1.val.raw,		0x00, sizeof(me->reg._0x02_Dev_config1.val.raw)); // Reset = 0x00
	memset(&me->reg._0x03_Dev_config2.val.raw,		0x00, sizeof(me->reg._0x03_Dev_config2.val.raw)); // Reset = 0x00
	memset(&me->reg._0x04_Dev_config3.val.raw,		0x47, sizeof(me->reg._0x04_Dev_config3.val.raw)); // Reset = 0x47
	memset(&me->reg._0x05_Global_bri.val.raw,		0xFF, sizeof(me->reg._0x05_Global_bri.val.raw));  // Reset = 0xFF

	// 0x06–0x08: Group brightness all 0xFF
	for (uint8_t i = 0;  i < 3;  ++i)
	{
		memset(&me->reg._group_bri[i].val.raw,		0xFF, sizeof(me->reg._group_bri[i].val.raw));
	}


	// 0x09…0x0B: CC-current-set all 0x40
	for (uint8_t i = 0; i < 3; ++i)
	{
		memset(&me->reg._current_set_cc[i].val.raw, 0x40, sizeof(me->reg._current_set_cc[i].val.raw));
	}

	// Dot group selects (0x0C–0x29): all 0x00
	for (int i = 0; i < 30; ++i)
	{
		memset(&me->reg._dot_grp_sel[i].val.raw,	0x00, sizeof(me->reg._dot_grp_sel[i].val.raw));
	}

	// Dot on/off (0x43–0x54):
	memset(&me->reg._dot_onoff[0].val.raw,			0xFF, sizeof(me->reg._dot_onoff[0].val.raw));
	memset(&me->reg._dot_onoff[1].val.raw,			0xFF, sizeof(me->reg._dot_onoff[1].val.raw));
	memset(&me->reg._dot_onoff[2].val.raw,			0x03, sizeof(me->reg._dot_onoff[2].val.raw));
	memset(&me->reg._dot_onoff[3].val.raw,			0xFF, sizeof(me->reg._dot_onoff[3].val.raw));
	memset(&me->reg._dot_onoff[4].val.raw,			0xFF, sizeof(me->reg._dot_onoff[4].val.raw));
	memset(&me->reg._dot_onoff[5].val.raw,			0x03, sizeof(me->reg._dot_onoff[5].val.raw));
	memset(&me->reg._dot_onoff[6].val.raw,			0xFF, sizeof(me->reg._dot_onoff[6].val.raw));
	memset(&me->reg._dot_onoff[7].val.raw,			0xFF, sizeof(me->reg._dot_onoff[7].val.raw));
	memset(&me->reg._dot_onoff[8].val.raw,			0x03, sizeof(me->reg._dot_onoff[8].val.raw));
	memset(&me->reg._dot_onoff[9].val.raw,			0xFF, sizeof(me->reg._dot_onoff[9].val.raw));
	memset(&me->reg._dot_onoff[10].val.raw,			0xFF, sizeof(me->reg._dot_onoff[10].val.raw));
	memset(&me->reg._dot_onoff[11].val.raw,			0x03, sizeof(me->reg._dot_onoff[11].val.raw));
	memset(&me->reg._dot_onoff[12].val.raw,			0xFF, sizeof(me->reg._dot_onoff[12].val.raw));
	memset(&me->reg._dot_onoff[13].val.raw,			0xFF, sizeof(me->reg._dot_onoff[13].val.raw));
	memset(&me->reg._dot_onoff[14].val.raw,			0x03, sizeof(me->reg._dot_onoff[14].val.raw));
	memset(&me->reg._dot_onoff[15].val.raw,			0xFF, sizeof(me->reg._dot_onoff[15].val.raw));
	memset(&me->reg._dot_onoff[16].val.raw,			0xFF, sizeof(me->reg._dot_onoff[16].val.raw));
	memset(&me->reg._dot_onoff[17].val.raw,			0x03, sizeof(me->reg._dot_onoff[17].val.raw));

	// Fault_state (0x64): default 0x00
	memset(&me->reg._0x64_Fault_state.val.raw,		0x00, sizeof(me->reg._0x64_Fault_state.val.raw));

	// LOD defaults (0x65–0x76): all 0x00
	for (uint8_t i = 0; i < 18; ++i)
	{
		memset(&me->reg._dot_lod[i].val.raw,		0x00, sizeof(me->reg._dot_lod[i].val.raw));
	}

	// LSD defaults (0x86–0x97): all 0x00
	for (uint8_t i = 0; i < 18; ++i)
	{
		memset(&me->reg._dot_lsd[i].val.raw,		0x00, sizeof(me->reg._dot_lsd[i].val.raw));
	}

	// Clears and reset registers (0xA7–0xA9): default 0x00
	memset(&me->reg._0xA7_LOD_clear.val.raw,		0x00, sizeof(me->reg._0xA7_LOD_clear.val.raw));
	memset(&me->reg._0xA8_LSD_clear.val.raw,		0x00, sizeof(me->reg._0xA8_LSD_clear.val.raw));
	memset(&me->reg._0xA9_Reset.val.raw,			0x00, sizeof(me->reg._0xA9_Reset.val.raw));

	// Dot-current (DC0…DC107) (0x100–0x16B): default all 0x80
	for (uint16_t i = 0; i < 108; ++i)
	{
		memset(&me->reg._dot_curr[i].val.raw,		0x80, sizeof(me->reg._dot_curr[i].val.raw));
	}

	//  Dot current registers (pwm_bri0…pwm_bri215 ) (0x200–0x2D7): default all 0x00
	for (uint16_t i = 0; i < 216; ++i)
	{
		memset(&me->reg._pwm_bri[i].val.raw,		0x00, sizeof(me->reg._pwm_bri[i].val.raw));
	}
}
Alx_Status AlxLp586x_RegStruct_Write(AlxLp586x* me)
{
	// Local variables
	Alx_Status status = Alx_Err;

	// Set Core registers:
	status = AlxLp586x_Reg_Write(me, &me->reg._0x00_Chip_En);
	if (status != Alx_Ok) { ALX_LP586x_TRACE_ERR("Err"); return status; }

	status = AlxLp586x_Reg_Write(me, &me->reg._0x01_Dev_initial);
	if (status != Alx_Ok) { ALX_LP586x_TRACE_ERR("Err"); return status; }

	status = AlxLp586x_Reg_Write(me, &me->reg._0x02_Dev_config1);
	if (status != Alx_Ok) { ALX_LP586x_TRACE_ERR("Err"); return status; }

	status = AlxLp586x_Reg_Write(me, &me->reg._0x03_Dev_config2);
	if (status != Alx_Ok) { ALX_LP586x_TRACE_ERR("Err"); return status; }

	status = AlxLp586x_Reg_Write(me, &me->reg._0x04_Dev_config3);
	if (status != Alx_Ok) { ALX_LP586x_TRACE_ERR("Err"); return status; }

	status = AlxLp586x_Reg_Write(me, &me->reg._0x05_Global_bri);
	if (status != Alx_Ok) { ALX_LP586x_TRACE_ERR("Err"); return status; }

	for (uint8_t i = 0; i < 3; ++i)
	{
		status = AlxLp586x_Reg_Write(me, &me->reg._group_bri[i]);
		if (status != Alx_Ok) { ALX_LP586x_TRACE_ERR("Err"); return status; }
	}

	for (uint8_t i = 0; i < 3; ++i)
	{
		status = AlxLp586x_Reg_Write(me, &me->reg._current_set_cc[i]);
		if (status != Alx_Ok) { ALX_LP586x_TRACE_ERR("Err"); return status; }
	}

	// Set Pwm Group
	for (int i = 0; i < 30; ++i)
	{
		status = AlxLp586x_Reg_Write(me, &me->reg._dot_grp_sel[i]);
		if (status != Alx_Ok) { ALX_LP586x_TRACE_ERR("Err"); return status; }
	}

	// Return
	return Alx_Ok;
}


//******************************************************************************
// Weak Functions
//******************************************************************************
ALX_WEAK void AlxLp586x_RegStruct_SetVal(AlxLp586x* me)
{
	(void)me;
	ALX_LP586x_ASSERT(false);	// Implement function in APP
}


#endif	// #if defined(ALX_C_LIB)
