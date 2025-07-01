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

	// Write register struct - WEAK
	status = AlxLp586x_RegStruct_Write(me);
	if (status != Alx_Ok)
	{
		ALX_LP586x_TRACE_ERR("FAIL: AlxLp586x_RegStruct_Write() status %ld", status);
		return status;
	}

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
	me->reg._0x00_Chip_En.addr					= 0x00;
	me->reg._0x01_Dev_initial.addr				= 0x01;
	me->reg._0x02_Dev_config1.addr				= 0x02;
	me->reg._0x03_Dev_config2.addr				= 0x03;
	me->reg._0x04_Dev_config3.addr				= 0x04;
	me->reg._0x05_Global_bri.addr				= 0x05;
	me->reg._0x06_Group0_bri.addr				= 0x06;
	me->reg._0x07_Group1_bri.addr				= 0x07;
	me->reg._0x08_Group2_bri.addr				= 0x08;
	me->reg._0x09_R_current_set_CC_Group1.addr	= 0x09;
	me->reg._0x0A_G_current_set_CC_Group2.addr	= 0x0A;
	me->reg._0x0B_B_current_set_CC_Group3.addr	= 0x0B;
	me->reg._0x0C_Dot_grp_sel0.addr				= 0x0C;
	me->reg._0x0D_Dot_grp_sel1.addr				= 0x0D;
	me->reg._0x0E_Dot_grp_sel2.addr				= 0x0E;
	me->reg._0x0F_Dot_grp_sel3.addr				= 0x0F;
	me->reg._0x10_Dot_grp_sel4.addr				= 0x10;
	me->reg._0x11_Dot_grp_sel5.addr				= 0x11;
	me->reg._0x12_Dot_grp_sel6.addr				= 0x12;
	me->reg._0x13_Dot_grp_sel7.addr				= 0x13;
	me->reg._0x14_Dot_grp_sel8.addr				= 0x14;
	me->reg._0x15_Dot_grp_sel9.addr				= 0x15;
	me->reg._0x16_Dot_grp_sel10.addr			= 0x16;
	me->reg._0x17_Dot_grp_sel11.addr			= 0x17;
	me->reg._0x0C_Dot_grp_sel0.addr				= 0x0C;
	me->reg._0x0D_Dot_grp_sel1.addr				= 0x0D;
	me->reg._0x0E_Dot_grp_sel2.addr				= 0x0E;
	me->reg._0x0F_Dot_grp_sel3.addr				= 0x0F;
	me->reg._0x10_Dot_grp_sel4.addr				= 0x10;
	me->reg._0x11_Dot_grp_sel5.addr				= 0x11;
	me->reg._0x12_Dot_grp_sel6.addr				= 0x12;
	me->reg._0x13_Dot_grp_sel7.addr				= 0x13;
	me->reg._0x14_Dot_grp_sel8.addr				= 0x14;
	me->reg._0x15_Dot_grp_sel9.addr				= 0x15;
	me->reg._0x16_Dot_grp_sel10.addr			= 0x16;
	me->reg._0x17_Dot_grp_sel11.addr			= 0x17;
	me->reg._0x18_Dot_grp_sel12.addr			= 0x18;
	me->reg._0x19_Dot_grp_sel13.addr			= 0x19;
	me->reg._0x1A_Dot_grp_sel14.addr			= 0x1A;
	me->reg._0x1B_Dot_grp_sel15.addr			= 0x1B;
	me->reg._0x1C_Dot_grp_sel16.addr			= 0x1C;
	me->reg._0x1D_Dot_grp_sel17.addr			= 0x1D;
	me->reg._0x1E_Dot_grp_sel18.addr			= 0x1E;
	me->reg._0x1F_Dot_grp_sel19.addr			= 0x1F;
	me->reg._0x20_Dot_grp_sel20.addr			= 0x20;
	me->reg._0x21_Dot_grp_sel21.addr			= 0x21;
	me->reg._0x22_Dot_grp_sel22.addr			= 0x22;
	me->reg._0x23_Dot_grp_sel23.addr			= 0x23;
	me->reg._0x24_Dot_grp_sel24.addr			= 0x24;
	me->reg._0x25_Dot_grp_sel25.addr			= 0x25;
	me->reg._0x26_Dot_grp_sel26.addr			= 0x26;
	me->reg._0x27_Dot_grp_sel27.addr			= 0x27;
	me->reg._0x28_Dot_grp_sel28.addr			= 0x28;
	me->reg._0x29_Dot_grp_sel29.addr			= 0x29;
	me->reg._0x43_Dot_onoff0.addr				= 0x43;
	me->reg._0x44_Dot_onoff1.addr				= 0x44;
	me->reg._0x45_Dot_onoff2.addr				= 0x45;
	me->reg._0x46_Dot_onoff3.addr				= 0x46;
	me->reg._0x47_Dot_onoff4.addr				= 0x47;
	me->reg._0x48_Dot_onoff5.addr				= 0x48;
	me->reg._0x49_Dot_onoff6.addr				= 0x49;
	me->reg._0x4A_Dot_onoff7.addr				= 0x4A;
	me->reg._0x4B_Dot_onoff8.addr				= 0x4B;
	me->reg._0x4C_Dot_onoff9.addr				= 0x4C;
	me->reg._0x4D_Dot_onoff10.addr				= 0x4D;
	me->reg._0x4E_Dot_onoff11.addr				= 0x4E;
	me->reg._0x4F_Dot_onoff12.addr				= 0x4F;
	me->reg._0x50_Dot_onoff13.addr				= 0x50;
	me->reg._0x51_Dot_onoff14.addr				= 0x51;
	me->reg._0x52_Dot_onoff15.addr				= 0x52;
	me->reg._0x53_Dot_onoff16.addr				= 0x53;
	me->reg._0x54_Dot_onoff17.addr				= 0x54;
	me->reg._0x64_Fault_state.addr				= 0x64;
	me->reg._0x65_Dot_lod0.addr					= 0x65;
	me->reg._0x66_Dot_lod1.addr					= 0x66;
	me->reg._0x67_Dot_lod2.addr					= 0x67;
	me->reg._0x68_Dot_lod3.addr					= 0x68;
	me->reg._0x69_Dot_lod4.addr					= 0x69;
	me->reg._0x6A_Dot_lod5.addr					= 0x6A;
	me->reg._0x6B_Dot_lod6.addr					= 0x6B;
	me->reg._0x6C_Dot_lod7.addr					= 0x6C;
	me->reg._0x6D_Dot_lod8.addr					= 0x6D;
	me->reg._0x6E_Dot_lod9.addr					= 0x6E;
	me->reg._0x6F_Dot_lod10.addr				= 0x6F;
	me->reg._0x70_Dot_lod11.addr				= 0x70;
	me->reg._0x71_Dot_lod12.addr				= 0x71;
	me->reg._0x72_Dot_lod13.addr				= 0x72;
	me->reg._0x73_Dot_lod14.addr				= 0x73;
	me->reg._0x74_Dot_lod15.addr				= 0x74;
	me->reg._0x75_Dot_lod16.addr				= 0x75;
	me->reg._0x76_Dot_lod17.addr				= 0x76;
	me->reg._0x86_Dot_lsd0.addr					= 0x86;
	me->reg._0x87_Dot_lsd1.addr					= 0x87;
	me->reg._0x88_Dot_lsd2.addr					= 0x88;
	me->reg._0x89_Dot_lsd3.addr					= 0x89;
	me->reg._0x8A_Dot_lsd4.addr					= 0x8A;
	me->reg._0x8B_Dot_lsd5.addr					= 0x8B;
	me->reg._0x8C_Dot_lsd6.addr					= 0x8C;
	me->reg._0x8D_Dot_lsd7.addr					= 0x8D;
	me->reg._0x8E_Dot_lsd8.addr					= 0x8E;
	me->reg._0x8F_Dot_lsd9.addr					= 0x8F;
	me->reg._0x90_Dot_lsd10.addr				= 0x90;
	me->reg._0x91_Dot_lsd11.addr				= 0x91;
	me->reg._0x92_Dot_lsd12.addr				= 0x92;
	me->reg._0x93_Dot_lsd13.addr				= 0x93;
	me->reg._0x94_Dot_lsd14.addr				= 0x94;
	me->reg._0x95_Dot_lsd15.addr				= 0x95;
	me->reg._0x96_Dot_lsd16.addr				= 0x96;
	me->reg._0x97_Dot_lsd17.addr				= 0x97;
	me->reg._0xA7_LOD_clear.addr				= 0xA7;
	me->reg._0xA8_LSD_clear.addr				= 0xA8;
	me->reg._0xA9_Reset.addr					= 0xA9;

}
static void AlxLp586x_RegStruct_SetLen(AlxLp586x* me)
{
	me->reg._0x00_Chip_En.len					= sizeof(me->reg._0x00_Chip_En.val);
	me->reg._0x01_Dev_initial.len				= sizeof(me->reg._0x01_Dev_initial.val);
	me->reg._0x02_Dev_config1.len				= sizeof(me->reg._0x02_Dev_config1.val);
	me->reg._0x03_Dev_config2.len				= sizeof(me->reg._0x03_Dev_config2.val);
	me->reg._0x04_Dev_config3.len				= sizeof(me->reg._0x04_Dev_config3.val);
	me->reg._0x05_Global_bri.len				= sizeof(me->reg._0x05_Global_bri.val);
	me->reg._0x06_Group0_bri.len				= sizeof(me->reg._0x06_Group0_bri.val);
	me->reg._0x07_Group1_bri.len				= sizeof(me->reg._0x07_Group1_bri.val);
	me->reg._0x08_Group2_bri.len				= sizeof(me->reg._0x08_Group2_bri.val);
	me->reg._0x09_R_current_set_CC_Group1.len	= sizeof(me->reg._0x09_R_current_set_CC_Group1.val);
	me->reg._0x0A_G_current_set_CC_Group2.len	= sizeof(me->reg._0x0A_G_current_set_CC_Group2.val);
	me->reg._0x0B_B_current_set_CC_Group3.len	= sizeof(me->reg._0x0B_B_current_set_CC_Group3.val);
	me->reg._0x0C_Dot_grp_sel0.len				= sizeof(me->reg._0x0C_Dot_grp_sel0.val);
	me->reg._0x0D_Dot_grp_sel1.len				= sizeof(me->reg._0x0D_Dot_grp_sel1.val);
	me->reg._0x0E_Dot_grp_sel2.len				= sizeof(me->reg._0x0E_Dot_grp_sel2.val);
	me->reg._0x0F_Dot_grp_sel3.len				= sizeof(me->reg._0x0F_Dot_grp_sel3.val);
	me->reg._0x10_Dot_grp_sel4.len				= sizeof(me->reg._0x10_Dot_grp_sel4.val);
	me->reg._0x11_Dot_grp_sel5.len				= sizeof(me->reg._0x11_Dot_grp_sel5.val);
	me->reg._0x12_Dot_grp_sel6.len				= sizeof(me->reg._0x12_Dot_grp_sel6.val);
	me->reg._0x13_Dot_grp_sel7.len				= sizeof(me->reg._0x13_Dot_grp_sel7.val);
	me->reg._0x14_Dot_grp_sel8.len				= sizeof(me->reg._0x14_Dot_grp_sel8.val);
	me->reg._0x15_Dot_grp_sel9.len				= sizeof(me->reg._0x15_Dot_grp_sel9.val);
	me->reg._0x16_Dot_grp_sel10.len				= sizeof(me->reg._0x16_Dot_grp_sel10.val);
	me->reg._0x17_Dot_grp_sel11.len				= sizeof(me->reg._0x17_Dot_grp_sel11.val);
	me->reg._0x18_Dot_grp_sel12.len				= sizeof(me->reg._0x18_Dot_grp_sel12.val);
	me->reg._0x19_Dot_grp_sel13.len				= sizeof(me->reg._0x19_Dot_grp_sel13.val);
	me->reg._0x1A_Dot_grp_sel14.len				= sizeof(me->reg._0x1A_Dot_grp_sel14.val);
	me->reg._0x1B_Dot_grp_sel15.len				= sizeof(me->reg._0x1B_Dot_grp_sel15.val);
	me->reg._0x1C_Dot_grp_sel16.len				= sizeof(me->reg._0x1C_Dot_grp_sel16.val);
	me->reg._0x1D_Dot_grp_sel17.len				= sizeof(me->reg._0x1D_Dot_grp_sel17.val);
	me->reg._0x1E_Dot_grp_sel18.len				= sizeof(me->reg._0x1E_Dot_grp_sel18.val);
	me->reg._0x1F_Dot_grp_sel19.len				= sizeof(me->reg._0x1F_Dot_grp_sel19.val);
	me->reg._0x20_Dot_grp_sel20.len				= sizeof(me->reg._0x20_Dot_grp_sel20.val);
	me->reg._0x21_Dot_grp_sel21.len				= sizeof(me->reg._0x21_Dot_grp_sel21.val);
	me->reg._0x22_Dot_grp_sel22.len				= sizeof(me->reg._0x22_Dot_grp_sel22.val);
	me->reg._0x23_Dot_grp_sel23.len				= sizeof(me->reg._0x23_Dot_grp_sel23.val);
	me->reg._0x24_Dot_grp_sel24.len				= sizeof(me->reg._0x24_Dot_grp_sel24.val);
	me->reg._0x25_Dot_grp_sel25.len				= sizeof(me->reg._0x25_Dot_grp_sel25.val);
	me->reg._0x26_Dot_grp_sel26.len				= sizeof(me->reg._0x26_Dot_grp_sel26.val);
	me->reg._0x27_Dot_grp_sel27.len				= sizeof(me->reg._0x27_Dot_grp_sel27.val);
	me->reg._0x28_Dot_grp_sel28.len				= sizeof(me->reg._0x28_Dot_grp_sel28.val);
	me->reg._0x29_Dot_grp_sel29.len				= sizeof(me->reg._0x29_Dot_grp_sel29.val);
	me->reg._0x43_Dot_onoff0.len				= sizeof(me->reg._0x43_Dot_onoff0.val);
	me->reg._0x44_Dot_onoff1.len				= sizeof(me->reg._0x44_Dot_onoff1.val);
	me->reg._0x45_Dot_onoff2.len				= sizeof(me->reg._0x45_Dot_onoff2.val);
	me->reg._0x46_Dot_onoff3.len				= sizeof(me->reg._0x46_Dot_onoff3.val);
	me->reg._0x47_Dot_onoff4.len				= sizeof(me->reg._0x47_Dot_onoff4.val);
	me->reg._0x48_Dot_onoff5.len				= sizeof(me->reg._0x48_Dot_onoff5.val);
	me->reg._0x49_Dot_onoff6.len				= sizeof(me->reg._0x49_Dot_onoff6.val);
	me->reg._0x4A_Dot_onoff7.len				= sizeof(me->reg._0x4A_Dot_onoff7.val);
	me->reg._0x4B_Dot_onoff8.len				= sizeof(me->reg._0x4B_Dot_onoff8.val);
	me->reg._0x4C_Dot_onoff9.len				= sizeof(me->reg._0x4C_Dot_onoff9.val);
	me->reg._0x4D_Dot_onoff10.len				= sizeof(me->reg._0x4D_Dot_onoff10.val);
	me->reg._0x4E_Dot_onoff11.len				= sizeof(me->reg._0x4E_Dot_onoff11.val);
	me->reg._0x4F_Dot_onoff12.len				= sizeof(me->reg._0x4F_Dot_onoff12.val);
	me->reg._0x50_Dot_onoff13.len				= sizeof(me->reg._0x50_Dot_onoff13.val);
	me->reg._0x51_Dot_onoff14.len				= sizeof(me->reg._0x51_Dot_onoff14.val);
	me->reg._0x52_Dot_onoff15.len				= sizeof(me->reg._0x52_Dot_onoff15.val);
	me->reg._0x53_Dot_onoff16.len				= sizeof(me->reg._0x53_Dot_onoff16.val);
	me->reg._0x54_Dot_onoff17.len				= sizeof(me->reg._0x54_Dot_onoff17.val);
	me->reg._0x64_Fault_state.len				= sizeof(me->reg._0x64_Fault_state.val);
	me->reg._0x65_Dot_lod0.len					= sizeof(me->reg._0x65_Dot_lod0.val);
	me->reg._0x66_Dot_lod1.len					= sizeof(me->reg._0x66_Dot_lod1.val);
	me->reg._0x67_Dot_lod2.len					= sizeof(me->reg._0x67_Dot_lod2.val);
	me->reg._0x68_Dot_lod3.len					= sizeof(me->reg._0x68_Dot_lod3.val);
	me->reg._0x69_Dot_lod4.len					= sizeof(me->reg._0x69_Dot_lod4.val);
	me->reg._0x6A_Dot_lod5.len					= sizeof(me->reg._0x6A_Dot_lod5.val);
	me->reg._0x6B_Dot_lod6.len					= sizeof(me->reg._0x6B_Dot_lod6.val);
	me->reg._0x6C_Dot_lod7.len					= sizeof(me->reg._0x6C_Dot_lod7.val);
	me->reg._0x6D_Dot_lod8.len					= sizeof(me->reg._0x6D_Dot_lod8.val);
	me->reg._0x6E_Dot_lod9.len					= sizeof(me->reg._0x6E_Dot_lod9.val);
	me->reg._0x6F_Dot_lod10.len					= sizeof(me->reg._0x6F_Dot_lod10.val);
	me->reg._0x70_Dot_lod11.len					= sizeof(me->reg._0x70_Dot_lod11.val);
	me->reg._0x71_Dot_lod12.len					= sizeof(me->reg._0x71_Dot_lod12.val);
	me->reg._0x72_Dot_lod13.len					= sizeof(me->reg._0x72_Dot_lod13.val);
	me->reg._0x73_Dot_lod14.len					= sizeof(me->reg._0x73_Dot_lod14.val);
	me->reg._0x74_Dot_lod15.len					= sizeof(me->reg._0x74_Dot_lod15.val);
	me->reg._0x75_Dot_lod16.len					= sizeof(me->reg._0x75_Dot_lod16.val);
	me->reg._0x76_Dot_lod17.len					= sizeof(me->reg._0x76_Dot_lod17.val);
	me->reg._0x86_Dot_lsd0.len					= sizeof(me->reg._0x86_Dot_lsd0.val);
	me->reg._0x87_Dot_lsd1.len					= sizeof(me->reg._0x87_Dot_lsd1.val);
	me->reg._0x88_Dot_lsd2.len					= sizeof(me->reg._0x88_Dot_lsd2.val);
	me->reg._0x89_Dot_lsd3.len					= sizeof(me->reg._0x89_Dot_lsd3.val);
	me->reg._0x8A_Dot_lsd4.len					= sizeof(me->reg._0x8A_Dot_lsd4.val);
	me->reg._0x8B_Dot_lsd5.len					= sizeof(me->reg._0x8B_Dot_lsd5.val);
	me->reg._0x8C_Dot_lsd6.len					= sizeof(me->reg._0x8C_Dot_lsd6.val);
	me->reg._0x8D_Dot_lsd7.len					= sizeof(me->reg._0x8D_Dot_lsd7.val);
	me->reg._0x8E_Dot_lsd8.len					= sizeof(me->reg._0x8E_Dot_lsd8.val);
	me->reg._0x8F_Dot_lsd9.len					= sizeof(me->reg._0x8F_Dot_lsd9.val);
	me->reg._0x90_Dot_lsd10.len					= sizeof(me->reg._0x90_Dot_lsd10.val);
	me->reg._0x91_Dot_lsd11.len					= sizeof(me->reg._0x91_Dot_lsd11.val);
	me->reg._0x92_Dot_lsd12.len					= sizeof(me->reg._0x92_Dot_lsd12.val);
	me->reg._0x93_Dot_lsd13.len					= sizeof(me->reg._0x93_Dot_lsd13.val);
	me->reg._0x94_Dot_lsd14.len					= sizeof(me->reg._0x94_Dot_lsd14.val);
	me->reg._0x95_Dot_lsd15.len					= sizeof(me->reg._0x95_Dot_lsd15.val);
	me->reg._0x96_Dot_lsd16.len					= sizeof(me->reg._0x96_Dot_lsd16.val);
	me->reg._0x97_Dot_lsd17.len					= sizeof(me->reg._0x97_Dot_lsd17.val);
	me->reg._0xA7_LOD_clear.len					= sizeof(me->reg._0xA7_LOD_clear.val);
	me->reg._0xA8_LSD_clear.len					= sizeof(me->reg._0xA8_LSD_clear.val);
	me->reg._0xA9_Reset.len						= sizeof(me->reg._0xA9_Reset.val);
}
static void AlxLp586x_RegStruct_SetValToZero(AlxLp586x* me)
{
	memset(&me->reg._0x00_Chip_En.val.raw,					0x00, sizeof(me->reg._0x00_Chip_En.val.raw));
	memset(&me->reg._0x01_Dev_initial.val.raw,				0x00, sizeof(me->reg._0x01_Dev_initial.val.raw));
	memset(&me->reg._0x02_Dev_config1.val.raw,				0x00, sizeof(me->reg._0x02_Dev_config1.val.raw));
	memset(&me->reg._0x03_Dev_config2.val.raw,				0x00, sizeof(me->reg._0x03_Dev_config2.val.raw));
	memset(&me->reg._0x04_Dev_config3.val.raw,				0x00, sizeof(me->reg._0x04_Dev_config3.val.raw));
	memset(&me->reg._0x05_Global_bri.val.raw,				0x00, sizeof(me->reg._0x05_Global_bri.val.raw));
	memset(&me->reg._0x06_Group0_bri.val.raw,				0x00, sizeof(me->reg._0x06_Group0_bri.val.raw));
	memset(&me->reg._0x07_Group1_bri.val.raw,				0x00, sizeof(me->reg._0x07_Group1_bri.val.raw));
	memset(&me->reg._0x08_Group2_bri.val.raw,				0x00, sizeof(me->reg._0x08_Group2_bri.val.raw));
	memset(&me->reg._0x09_R_current_set_CC_Group1.val.raw,	0x00, sizeof(me->reg._0x09_R_current_set_CC_Group1.val.raw));
	memset(&me->reg._0x0A_G_current_set_CC_Group2.val.raw,	0x00, sizeof(me->reg._0x0A_G_current_set_CC_Group2.val.raw));
	memset(&me->reg._0x0B_B_current_set_CC_Group3.val.raw,	0x00, sizeof(me->reg._0x0B_B_current_set_CC_Group3.val.raw));
	memset(&me->reg._0x0C_Dot_grp_sel0.val.raw,				0x00, sizeof(me->reg._0x0C_Dot_grp_sel0.val.raw));
	memset(&me->reg._0x0D_Dot_grp_sel1.val.raw,				0x00, sizeof(me->reg._0x0D_Dot_grp_sel1.val.raw));
	memset(&me->reg._0x0E_Dot_grp_sel2.val.raw,				0x00, sizeof(me->reg._0x0E_Dot_grp_sel2.val.raw));
	memset(&me->reg._0x0F_Dot_grp_sel3.val.raw,				0x00, sizeof(me->reg._0x0F_Dot_grp_sel3.val.raw));
	memset(&me->reg._0x10_Dot_grp_sel4.val.raw,				0x00, sizeof(me->reg._0x10_Dot_grp_sel4.val.raw));
	memset(&me->reg._0x11_Dot_grp_sel5.val.raw,				0x00, sizeof(me->reg._0x11_Dot_grp_sel5.val.raw));
	memset(&me->reg._0x12_Dot_grp_sel6.val.raw,				0x00, sizeof(me->reg._0x12_Dot_grp_sel6.val.raw));
	memset(&me->reg._0x13_Dot_grp_sel7.val.raw,				0x00, sizeof(me->reg._0x13_Dot_grp_sel7.val.raw));
	memset(&me->reg._0x14_Dot_grp_sel8.val.raw,				0x00, sizeof(me->reg._0x14_Dot_grp_sel8.val.raw));
	memset(&me->reg._0x15_Dot_grp_sel9.val.raw,				0x00, sizeof(me->reg._0x15_Dot_grp_sel9.val.raw));
	memset(&me->reg._0x16_Dot_grp_sel10.val.raw,			0x00, sizeof(me->reg._0x16_Dot_grp_sel10.val.raw));
	memset(&me->reg._0x17_Dot_grp_sel11.val.raw,			0x00, sizeof(me->reg._0x17_Dot_grp_sel11.val.raw));
	memset(&me->reg._0x18_Dot_grp_sel12.val.raw,			0x00, sizeof(me->reg._0x18_Dot_grp_sel12.val.raw));
	memset(&me->reg._0x19_Dot_grp_sel13.val.raw,			0x00, sizeof(me->reg._0x19_Dot_grp_sel13.val.raw));
	memset(&me->reg._0x1A_Dot_grp_sel14.val.raw,			0x00, sizeof(me->reg._0x1A_Dot_grp_sel14.val.raw));
	memset(&me->reg._0x1B_Dot_grp_sel15.val.raw,			0x00, sizeof(me->reg._0x1B_Dot_grp_sel15.val.raw));
	memset(&me->reg._0x1C_Dot_grp_sel16.val.raw,			0x00, sizeof(me->reg._0x1C_Dot_grp_sel16.val.raw));
	memset(&me->reg._0x1D_Dot_grp_sel17.val.raw,			0x00, sizeof(me->reg._0x1D_Dot_grp_sel17.val.raw));
	memset(&me->reg._0x1E_Dot_grp_sel18.val.raw,			0x00, sizeof(me->reg._0x1E_Dot_grp_sel18.val.raw));
	memset(&me->reg._0x1F_Dot_grp_sel19.val.raw,			0x00, sizeof(me->reg._0x1F_Dot_grp_sel19.val.raw));
	memset(&me->reg._0x20_Dot_grp_sel20.val.raw,			0x00, sizeof(me->reg._0x20_Dot_grp_sel20.val.raw));
	memset(&me->reg._0x21_Dot_grp_sel21.val.raw,			0x00, sizeof(me->reg._0x21_Dot_grp_sel21.val.raw));
	memset(&me->reg._0x22_Dot_grp_sel22.val.raw,			0x00, sizeof(me->reg._0x22_Dot_grp_sel22.val.raw));
	memset(&me->reg._0x23_Dot_grp_sel23.val.raw,			0x00, sizeof(me->reg._0x23_Dot_grp_sel23.val.raw));
	memset(&me->reg._0x24_Dot_grp_sel24.val.raw,			0x00, sizeof(me->reg._0x24_Dot_grp_sel24.val.raw));
	memset(&me->reg._0x25_Dot_grp_sel25.val.raw,			0x00, sizeof(me->reg._0x25_Dot_grp_sel25.val.raw));
	memset(&me->reg._0x26_Dot_grp_sel26.val.raw,			0x00, sizeof(me->reg._0x26_Dot_grp_sel26.val.raw));
	memset(&me->reg._0x27_Dot_grp_sel27.val.raw,			0x00, sizeof(me->reg._0x27_Dot_grp_sel27.val.raw));
	memset(&me->reg._0x28_Dot_grp_sel28.val.raw,			0x00, sizeof(me->reg._0x28_Dot_grp_sel28.val.raw));
	memset(&me->reg._0x29_Dot_grp_sel29.val.raw,			0x00, sizeof(me->reg._0x29_Dot_grp_sel29.val.raw));
	memset(&me->reg._0x43_Dot_onoff0.val.raw,				0x00, sizeof(me->reg._0x43_Dot_onoff0.val.raw));
	memset(&me->reg._0x44_Dot_onoff1.val.raw,				0x00, sizeof(me->reg._0x44_Dot_onoff1.val.raw));
	memset(&me->reg._0x45_Dot_onoff2.val.raw,				0x00, sizeof(me->reg._0x45_Dot_onoff2.val.raw));
	memset(&me->reg._0x46_Dot_onoff3.val.raw,				0x00, sizeof(me->reg._0x46_Dot_onoff3.val.raw));
	memset(&me->reg._0x47_Dot_onoff4.val.raw,				0x00, sizeof(me->reg._0x47_Dot_onoff4.val.raw));
	memset(&me->reg._0x48_Dot_onoff5.val.raw,				0x00, sizeof(me->reg._0x48_Dot_onoff5.val.raw));
	memset(&me->reg._0x49_Dot_onoff6.val.raw,				0x00, sizeof(me->reg._0x49_Dot_onoff6.val.raw));
	memset(&me->reg._0x4A_Dot_onoff7.val.raw,				0x00, sizeof(me->reg._0x4A_Dot_onoff7.val.raw));
	memset(&me->reg._0x4B_Dot_onoff8.val.raw,				0x00, sizeof(me->reg._0x4B_Dot_onoff8.val.raw));
	memset(&me->reg._0x4C_Dot_onoff9.val.raw,				0x00, sizeof(me->reg._0x4C_Dot_onoff9.val.raw));
	memset(&me->reg._0x4D_Dot_onoff10.val.raw,				0x00, sizeof(me->reg._0x4D_Dot_onoff10.val.raw));
	memset(&me->reg._0x4E_Dot_onoff11.val.raw,				0x00, sizeof(me->reg._0x4E_Dot_onoff11.val.raw));
	memset(&me->reg._0x4F_Dot_onoff12.val.raw,				0x00, sizeof(me->reg._0x4F_Dot_onoff12.val.raw));
	memset(&me->reg._0x50_Dot_onoff13.val.raw,				0x00, sizeof(me->reg._0x50_Dot_onoff13.val.raw));
	memset(&me->reg._0x51_Dot_onoff14.val.raw,				0x00, sizeof(me->reg._0x51_Dot_onoff14.val.raw));
	memset(&me->reg._0x52_Dot_onoff15.val.raw,				0x00, sizeof(me->reg._0x52_Dot_onoff15.val.raw));
	memset(&me->reg._0x53_Dot_onoff16.val.raw,				0x00, sizeof(me->reg._0x53_Dot_onoff16.val.raw));
	memset(&me->reg._0x54_Dot_onoff17.val.raw,				0x00, sizeof(me->reg._0x54_Dot_onoff17.val.raw));
	memset(&me->reg._0x64_Fault_state.val.raw,				0x00, sizeof(me->reg._0x64_Fault_state.val.raw));
	memset(&me->reg._0x65_Dot_lod0.val.raw,					0x00, sizeof(me->reg._0x65_Dot_lod0.val.raw));
	memset(&me->reg._0x66_Dot_lod1.val.raw,					0x00, sizeof(me->reg._0x66_Dot_lod1.val.raw));
	memset(&me->reg._0x67_Dot_lod2.val.raw,					0x00, sizeof(me->reg._0x67_Dot_lod2.val.raw));
	memset(&me->reg._0x68_Dot_lod3.val.raw,					0x00, sizeof(me->reg._0x68_Dot_lod3.val.raw));
	memset(&me->reg._0x69_Dot_lod4.val.raw,					0x00, sizeof(me->reg._0x69_Dot_lod4.val.raw));
	memset(&me->reg._0x6A_Dot_lod5.val.raw,					0x00, sizeof(me->reg._0x6A_Dot_lod5.val.raw));
	memset(&me->reg._0x6B_Dot_lod6.val.raw,					0x00, sizeof(me->reg._0x6B_Dot_lod6.val.raw));
	memset(&me->reg._0x6C_Dot_lod7.val.raw,					0x00, sizeof(me->reg._0x6C_Dot_lod7.val.raw));
	memset(&me->reg._0x6D_Dot_lod8.val.raw,					0x00, sizeof(me->reg._0x6D_Dot_lod8.val.raw));
	memset(&me->reg._0x6E_Dot_lod9.val.raw,					0x00, sizeof(me->reg._0x6E_Dot_lod9.val.raw));
	memset(&me->reg._0x6F_Dot_lod10.val.raw,				0x00, sizeof(me->reg._0x6F_Dot_lod10.val.raw));
	memset(&me->reg._0x70_Dot_lod11.val.raw,				0x00, sizeof(me->reg._0x70_Dot_lod11.val.raw));
	memset(&me->reg._0x71_Dot_lod12.val.raw,				0x00, sizeof(me->reg._0x71_Dot_lod12.val.raw));
	memset(&me->reg._0x72_Dot_lod13.val.raw,				0x00, sizeof(me->reg._0x72_Dot_lod13.val.raw));
	memset(&me->reg._0x73_Dot_lod14.val.raw,				0x00, sizeof(me->reg._0x73_Dot_lod14.val.raw));
	memset(&me->reg._0x74_Dot_lod15.val.raw,				0x00, sizeof(me->reg._0x74_Dot_lod15.val.raw));
	memset(&me->reg._0x75_Dot_lod16.val.raw,				0x00, sizeof(me->reg._0x75_Dot_lod16.val.raw));
	memset(&me->reg._0x76_Dot_lod17.val.raw,				0x00, sizeof(me->reg._0x76_Dot_lod17.val.raw));
	memset(&me->reg._0x86_Dot_lsd0.val.raw,					0x00, sizeof(me->reg._0x86_Dot_lsd0.val.raw));
	memset(&me->reg._0x87_Dot_lsd1.val.raw,					0x00, sizeof(me->reg._0x87_Dot_lsd1.val.raw));
	memset(&me->reg._0x88_Dot_lsd2.val.raw,					0x00, sizeof(me->reg._0x88_Dot_lsd2.val.raw));
	memset(&me->reg._0x89_Dot_lsd3.val.raw,					0x00, sizeof(me->reg._0x89_Dot_lsd3.val.raw));
	memset(&me->reg._0x8A_Dot_lsd4.val.raw,					0x00, sizeof(me->reg._0x8A_Dot_lsd4.val.raw));
	memset(&me->reg._0x8B_Dot_lsd5.val.raw,					0x00, sizeof(me->reg._0x8B_Dot_lsd5.val.raw));
	memset(&me->reg._0x8C_Dot_lsd6.val.raw,					0x00, sizeof(me->reg._0x8C_Dot_lsd6.val.raw));
	memset(&me->reg._0x8D_Dot_lsd7.val.raw,					0x00, sizeof(me->reg._0x8D_Dot_lsd7.val.raw));
	memset(&me->reg._0x8E_Dot_lsd8.val.raw,					0x00, sizeof(me->reg._0x8E_Dot_lsd8.val.raw));
	memset(&me->reg._0x8F_Dot_lsd9.val.raw,					0x00, sizeof(me->reg._0x8F_Dot_lsd9.val.raw));
	memset(&me->reg._0x90_Dot_lsd10.val.raw,				0x00, sizeof(me->reg._0x90_Dot_lsd10.val.raw));
	memset(&me->reg._0x91_Dot_lsd11.val.raw,				0x00, sizeof(me->reg._0x91_Dot_lsd11.val.raw));
	memset(&me->reg._0x92_Dot_lsd12.val.raw,				0x00, sizeof(me->reg._0x92_Dot_lsd12.val.raw));
	memset(&me->reg._0x93_Dot_lsd13.val.raw,				0x00, sizeof(me->reg._0x93_Dot_lsd13.val.raw));
	memset(&me->reg._0x94_Dot_lsd14.val.raw,				0x00, sizeof(me->reg._0x94_Dot_lsd14.val.raw));
	memset(&me->reg._0x95_Dot_lsd15.val.raw,				0x00, sizeof(me->reg._0x95_Dot_lsd15.val.raw));
	memset(&me->reg._0x96_Dot_lsd16.val.raw,				0x00, sizeof(me->reg._0x96_Dot_lsd16.val.raw));
	memset(&me->reg._0x97_Dot_lsd17.val.raw,				0x00, sizeof(me->reg._0x97_Dot_lsd17.val.raw));
	memset(&me->reg._0xA7_LOD_clear.val.raw,				0x00, sizeof(me->reg._0xA7_LOD_clear.val.raw));
	memset(&me->reg._0xA8_LSD_clear.val.raw,				0x00, sizeof(me->reg._0xA8_LSD_clear.val.raw));
	memset(&me->reg._0xA9_Reset.val.raw,					0x00, sizeof(me->reg._0xA9_Reset.val.raw));
}
static void AlxLp586x_RegStruct_SetValToDefault(AlxLp586x* me)
{
	// Core registers:
	memset(&me->reg._0x00_Chip_En.val.raw,						0x00, sizeof(me->reg._0x00_Chip_En.val.raw));     // Reset = 0x00
	memset(&me->reg._0x01_Dev_initial.val.raw,					0x5E, sizeof(me->reg._0x01_Dev_initial.val.raw)); // Reset = 0x5E
	memset(&me->reg._0x02_Dev_config1.val.raw,					0x00, sizeof(me->reg._0x02_Dev_config1.val.raw)); // Reset = 0x00
	memset(&me->reg._0x03_Dev_config2.val.raw,					0x00, sizeof(me->reg._0x03_Dev_config2.val.raw)); // Reset = 0x00
	memset(&me->reg._0x04_Dev_config3.val.raw,					0x47, sizeof(me->reg._0x04_Dev_config3.val.raw)); // Reset = 0x47
	memset(&me->reg._0x05_Global_bri.val.raw,					0xFF, sizeof(me->reg._0x05_Global_bri.val.raw));  // Reset = 0xFF
	memset(&me->reg._0x06_Group0_bri.val.raw,					0xFF, sizeof(me->reg._0x06_Group0_bri.val.raw));  // Reset = 0xFF
	memset(&me->reg._0x07_Group1_bri.val.raw,					0xFF, sizeof(me->reg._0x07_Group1_bri.val.raw));  // Reset = 0xFF
	memset(&me->reg._0x08_Group2_bri.val.raw,					0xFF, sizeof(me->reg._0x08_Group2_bri.val.raw));  // Reset = 0xFF
	memset(&me->reg._0x09_R_current_set_CC_Group1.val.raw,		0x40, sizeof(me->reg._0x09_R_current_set_CC_Group1.val.raw)); // Reset = 0x40
	memset(&me->reg._0x0A_G_current_set_CC_Group2.val.raw,		0x40, sizeof(me->reg._0x0A_G_current_set_CC_Group2.val.raw)); // Reset = 0x40
	memset(&me->reg._0x0B_B_current_set_CC_Group3.val.raw,		0x40, sizeof(me->reg._0x0B_B_current_set_CC_Group3.val.raw)); // Reset = 0x40

	// Dot group selects (0x0C–0x29): all 0x00
	memset(&me->reg._0x0C_Dot_grp_sel0.val.raw,					0x00, sizeof(me->reg._0x0C_Dot_grp_sel0.val.raw));
	memset(&me->reg._0x0D_Dot_grp_sel1.val.raw,					0x00, sizeof(me->reg._0x0D_Dot_grp_sel1.val.raw));
	memset(&me->reg._0x0E_Dot_grp_sel2.val.raw,					0x00, sizeof(me->reg._0x0E_Dot_grp_sel2.val.raw));
	memset(&me->reg._0x0F_Dot_grp_sel3.val.raw,					0x00, sizeof(me->reg._0x0F_Dot_grp_sel3.val.raw));
	memset(&me->reg._0x10_Dot_grp_sel4.val.raw,					0x00, sizeof(me->reg._0x10_Dot_grp_sel4.val.raw));
	memset(&me->reg._0x11_Dot_grp_sel5.val.raw,					0x00, sizeof(me->reg._0x11_Dot_grp_sel5.val.raw));
	memset(&me->reg._0x12_Dot_grp_sel6.val.raw,					0x00, sizeof(me->reg._0x12_Dot_grp_sel6.val.raw));
	memset(&me->reg._0x13_Dot_grp_sel7.val.raw,					0x00, sizeof(me->reg._0x13_Dot_grp_sel7.val.raw));
	memset(&me->reg._0x14_Dot_grp_sel8.val.raw,					0x00, sizeof(me->reg._0x14_Dot_grp_sel8.val.raw));
	memset(&me->reg._0x15_Dot_grp_sel9.val.raw,					0x00, sizeof(me->reg._0x15_Dot_grp_sel9.val.raw));
	memset(&me->reg._0x16_Dot_grp_sel10.val.raw,				0x00, sizeof(me->reg._0x16_Dot_grp_sel10.val.raw));
	memset(&me->reg._0x17_Dot_grp_sel11.val.raw,				0x00, sizeof(me->reg._0x17_Dot_grp_sel11.val.raw));
	memset(&me->reg._0x18_Dot_grp_sel12.val.raw,				0x00, sizeof(me->reg._0x18_Dot_grp_sel12.val.raw));
	memset(&me->reg._0x19_Dot_grp_sel13.val.raw,				0x00, sizeof(me->reg._0x19_Dot_grp_sel13.val.raw));
	memset(&me->reg._0x1A_Dot_grp_sel14.val.raw,				0x00, sizeof(me->reg._0x1A_Dot_grp_sel14.val.raw));
	memset(&me->reg._0x1B_Dot_grp_sel15.val.raw,				0x00, sizeof(me->reg._0x1B_Dot_grp_sel15.val.raw));
	memset(&me->reg._0x1C_Dot_grp_sel16.val.raw,				0x00, sizeof(me->reg._0x1C_Dot_grp_sel16.val.raw));
	memset(&me->reg._0x1D_Dot_grp_sel17.val.raw,				0x00, sizeof(me->reg._0x1D_Dot_grp_sel17.val.raw));
	memset(&me->reg._0x1E_Dot_grp_sel18.val.raw,				0x00, sizeof(me->reg._0x1E_Dot_grp_sel18.val.raw));
	memset(&me->reg._0x1F_Dot_grp_sel19.val.raw,				0x00, sizeof(me->reg._0x1F_Dot_grp_sel19.val.raw));
	memset(&me->reg._0x20_Dot_grp_sel20.val.raw,				0x00, sizeof(me->reg._0x20_Dot_grp_sel20.val.raw));
	memset(&me->reg._0x21_Dot_grp_sel21.val.raw,				0x00, sizeof(me->reg._0x21_Dot_grp_sel21.val.raw));
	memset(&me->reg._0x22_Dot_grp_sel22.val.raw,				0x00, sizeof(me->reg._0x22_Dot_grp_sel22.val.raw));
	memset(&me->reg._0x23_Dot_grp_sel23.val.raw,				0x00, sizeof(me->reg._0x23_Dot_grp_sel23.val.raw));
	memset(&me->reg._0x24_Dot_grp_sel24.val.raw,				0x00, sizeof(me->reg._0x24_Dot_grp_sel24.val.raw));
	memset(&me->reg._0x25_Dot_grp_sel25.val.raw,				0x00, sizeof(me->reg._0x25_Dot_grp_sel25.val.raw));
	memset(&me->reg._0x26_Dot_grp_sel26.val.raw,				0x00, sizeof(me->reg._0x26_Dot_grp_sel26.val.raw));
	memset(&me->reg._0x27_Dot_grp_sel27.val.raw,				0x00, sizeof(me->reg._0x27_Dot_grp_sel27.val.raw));
	memset(&me->reg._0x28_Dot_grp_sel28.val.raw,				0x00, sizeof(me->reg._0x28_Dot_grp_sel28.val.raw));
	memset(&me->reg._0x29_Dot_grp_sel29.val.raw,				0x00, sizeof(me->reg._0x29_Dot_grp_sel29.val.raw));

	// Dot on/off (0x43–0x54):
	memset(&me->reg._0x43_Dot_onoff0.val.raw,					0xFF, sizeof(me->reg._0x43_Dot_onoff0.val.raw));
	memset(&me->reg._0x44_Dot_onoff1.val.raw,					0xFF, sizeof(me->reg._0x44_Dot_onoff1.val.raw));
	memset(&me->reg._0x45_Dot_onoff2.val.raw,					0x03, sizeof(me->reg._0x45_Dot_onoff2.val.raw));
	memset(&me->reg._0x46_Dot_onoff3.val.raw,					0xFF, sizeof(me->reg._0x46_Dot_onoff3.val.raw));
	memset(&me->reg._0x47_Dot_onoff4.val.raw,					0xFF, sizeof(me->reg._0x47_Dot_onoff4.val.raw));
	memset(&me->reg._0x48_Dot_onoff5.val.raw,					0x03, sizeof(me->reg._0x48_Dot_onoff5.val.raw));
	memset(&me->reg._0x49_Dot_onoff6.val.raw,					0xFF, sizeof(me->reg._0x49_Dot_onoff6.val.raw));
	memset(&me->reg._0x4A_Dot_onoff7.val.raw,					0xFF, sizeof(me->reg._0x4A_Dot_onoff7.val.raw));
	memset(&me->reg._0x4B_Dot_onoff8.val.raw,					0x03, sizeof(me->reg._0x4B_Dot_onoff8.val.raw));
	memset(&me->reg._0x4C_Dot_onoff9.val.raw,					0xFF, sizeof(me->reg._0x4C_Dot_onoff9.val.raw));
	memset(&me->reg._0x4D_Dot_onoff10.val.raw,					0xFF, sizeof(me->reg._0x4D_Dot_onoff10.val.raw));
	memset(&me->reg._0x4E_Dot_onoff11.val.raw,					0x03, sizeof(me->reg._0x4E_Dot_onoff11.val.raw));
	memset(&me->reg._0x4F_Dot_onoff12.val.raw,					0xFF, sizeof(me->reg._0x4F_Dot_onoff12.val.raw));
	memset(&me->reg._0x50_Dot_onoff13.val.raw,					0xFF, sizeof(me->reg._0x50_Dot_onoff13.val.raw));
	memset(&me->reg._0x51_Dot_onoff14.val.raw,					0x03, sizeof(me->reg._0x51_Dot_onoff14.val.raw));
	memset(&me->reg._0x52_Dot_onoff15.val.raw,					0xFF, sizeof(me->reg._0x52_Dot_onoff15.val.raw));
	memset(&me->reg._0x53_Dot_onoff16.val.raw,					0xFF, sizeof(me->reg._0x53_Dot_onoff16.val.raw));
	memset(&me->reg._0x54_Dot_onoff17.val.raw,					0x03, sizeof(me->reg._0x54_Dot_onoff17.val.raw));

	// Fault_state (0x64): default 0x00
	memset(&me->reg._0x64_Fault_state.val.raw,					0x00, sizeof(me->reg._0x64_Fault_state.val.raw));

	// LOD status (0x65–0x76): default 0x00
	memset(&me->reg._0x65_Dot_lod0.val.raw,						0x00, sizeof(me->reg._0x65_Dot_lod0.val.raw));
	memset(&me->reg._0x66_Dot_lod1.val.raw,						0x00, sizeof(me->reg._0x66_Dot_lod1.val.raw));
	memset(&me->reg._0x67_Dot_lod2.val.raw,						0x00, sizeof(me->reg._0x67_Dot_lod2.val.raw));
	memset(&me->reg._0x68_Dot_lod3.val.raw,						0x00, sizeof(me->reg._0x68_Dot_lod3.val.raw));
	memset(&me->reg._0x69_Dot_lod4.val.raw,						0x00, sizeof(me->reg._0x69_Dot_lod4.val.raw));
	memset(&me->reg._0x6A_Dot_lod5.val.raw,						0x00, sizeof(me->reg._0x6A_Dot_lod5.val.raw));
	memset(&me->reg._0x6B_Dot_lod6.val.raw,						0x00, sizeof(me->reg._0x6B_Dot_lod6.val.raw));
	memset(&me->reg._0x6C_Dot_lod7.val.raw,						0x00, sizeof(me->reg._0x6C_Dot_lod7.val.raw));
	memset(&me->reg._0x6D_Dot_lod8.val.raw,						0x00, sizeof(me->reg._0x6D_Dot_lod8.val.raw));
	memset(&me->reg._0x6E_Dot_lod9.val.raw,						0x00, sizeof(me->reg._0x6E_Dot_lod9.val.raw));
	memset(&me->reg._0x6F_Dot_lod10.val.raw,					0x00, sizeof(me->reg._0x6F_Dot_lod10.val.raw));
	memset(&me->reg._0x70_Dot_lod11.val.raw,					0x00, sizeof(me->reg._0x70_Dot_lod11.val.raw));
	memset(&me->reg._0x71_Dot_lod12.val.raw,					0x00, sizeof(me->reg._0x71_Dot_lod12.val.raw));
	memset(&me->reg._0x72_Dot_lod13.val.raw,					0x00, sizeof(me->reg._0x72_Dot_lod13.val.raw));
	memset(&me->reg._0x73_Dot_lod14.val.raw,					0x00, sizeof(me->reg._0x73_Dot_lod14.val.raw));
	memset(&me->reg._0x74_Dot_lod15.val.raw,					0x00, sizeof(me->reg._0x74_Dot_lod15.val.raw));
	memset(&me->reg._0x75_Dot_lod16.val.raw,					0x00, sizeof(me->reg._0x75_Dot_lod16.val.raw));
	memset(&me->reg._0x76_Dot_lod17.val.raw,					0x00, sizeof(me->reg._0x76_Dot_lod17.val.raw));

	// LSD status (0x86–0x97): default 0x00
	memset(&me->reg._0x86_Dot_lsd0.val.raw,						0x00, sizeof(me->reg._0x86_Dot_lsd0.val.raw));
	memset(&me->reg._0x87_Dot_lsd1.val.raw,						0x00, sizeof(me->reg._0x87_Dot_lsd1.val.raw));
	memset(&me->reg._0x88_Dot_lsd2.val.raw,						0x00, sizeof(me->reg._0x88_Dot_lsd2.val.raw));
	memset(&me->reg._0x89_Dot_lsd3.val.raw,						0x00, sizeof(me->reg._0x89_Dot_lsd3.val.raw));
	memset(&me->reg._0x8A_Dot_lsd4.val.raw,						0x00, sizeof(me->reg._0x8A_Dot_lsd4.val.raw));
	memset(&me->reg._0x8B_Dot_lsd5.val.raw,						0x00, sizeof(me->reg._0x8B_Dot_lsd5.val.raw));
	memset(&me->reg._0x8C_Dot_lsd6.val.raw,						0x00, sizeof(me->reg._0x8C_Dot_lsd6.val.raw));
	memset(&me->reg._0x8D_Dot_lsd7.val.raw,						0x00, sizeof(me->reg._0x8D_Dot_lsd7.val.raw));
	memset(&me->reg._0x8E_Dot_lsd8.val.raw,						0x00, sizeof(me->reg._0x8E_Dot_lsd8.val.raw));
	memset(&me->reg._0x8F_Dot_lsd9.val.raw,						0x00, sizeof(me->reg._0x8F_Dot_lsd9.val.raw));
	memset(&me->reg._0x90_Dot_lsd10.val.raw,					0x00, sizeof(me->reg._0x90_Dot_lsd10.val.raw));
	memset(&me->reg._0x91_Dot_lsd11.val.raw,					0x00, sizeof(me->reg._0x91_Dot_lsd11.val.raw));
	memset(&me->reg._0x92_Dot_lsd12.val.raw,					0x00, sizeof(me->reg._0x92_Dot_lsd12.val.raw));
	memset(&me->reg._0x93_Dot_lsd13.val.raw,					0x00, sizeof(me->reg._0x93_Dot_lsd13.val.raw));
	memset(&me->reg._0x94_Dot_lsd14.val.raw,					0x00, sizeof(me->reg._0x94_Dot_lsd14.val.raw));
	memset(&me->reg._0x95_Dot_lsd15.val.raw,					0x00, sizeof(me->reg._0x95_Dot_lsd15.val.raw));
	memset(&me->reg._0x96_Dot_lsd16.val.raw,					0x00, sizeof(me->reg._0x96_Dot_lsd16.val.raw));
	memset(&me->reg._0x97_Dot_lsd17.val.raw,					0x00, sizeof(me->reg._0x97_Dot_lsd17.val.raw));

	// Clears and reset registers (0xA7–0xA9): default 0x00
	memset(&me->reg._0xA7_LOD_clear.val.raw,					0x00, sizeof(me->reg._0xA7_LOD_clear.val.raw));
	memset(&me->reg._0xA8_LSD_clear.val.raw,					0x00, sizeof(me->reg._0xA8_LSD_clear.val.raw));
	memset(&me->reg._0xA9_Reset.val.raw,						0x00, sizeof(me->reg._0xA9_Reset.val.raw));
}
Alx_Status AlxLp586x_RegStruct_Write(AlxLp586x* me)
{
	// Local variables
	Alx_Status status = Alx_Err;

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
