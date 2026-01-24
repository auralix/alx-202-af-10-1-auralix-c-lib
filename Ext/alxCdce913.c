/**
  ******************************************************************************
  * @file		alxCdce913.c
  * @brief		Auralix C Library - ALX CDCD913 PLL Module
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
#include "alxCdce913.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB) && defined(ALX_STM32L4)


//******************************************************************************
// Private Types
//******************************************************************************
#define CDCE913_I2C_ADDR (0xCA)
#define CDCE913_BYTE_ACCESS_MASK (0x80)
#define CDCE913_BLOCK_ACCESS_MASK (0x00)
#define CDCE913_VCTRL_MIN_V (0.0f)
#define CDCE913_VCTRL_MAX_V (1.8f)

//******************************************************************************
// Private Functions
//******************************************************************************
static void Cdce913_RegStructInit(AlxCdce913_Reg *reg)
{
	memset(reg, 0, sizeof(AlxCdce913_Reg));

	reg->ID.addr = 0x00;
	reg->DEV_CTRL0.addr = 0x01;
	reg->DEV_CTRL1.addr = 0x02;
	reg->PDIV1_LSB.addr = 0x03;
	reg->CON_Y1.addr = 0x04;
	reg->XCSEL.addr = 0x05;
	reg->DEV_CTRL2.addr = 0x06;
	reg->CON_SSC0.addr = 0x10;
	reg->CON_SSC1.addr = 0x11;
	reg->CON_SSC2.addr = 0x12;
	reg->CON_FS.addr = 0x13;
	reg->DEV_CTRL3.addr = 0x14;
	reg->CON_Y2Y3.addr = 0x15;
	reg->DEV_CTRL4.addr = 0x16;
	reg->PDIV3.addr = 0x17;
	reg->PLL1_0_CFG0.addr = 0x18;
	reg->PLL1_0_CFG1.addr = 0x19;
	reg->PLL1_0_CFG2.addr = 0x1A;
	reg->PLL1_0_CFG3.addr = 0x1B;
	reg->PLL1_1_CFG0.addr = 0x1C;
	reg->PLL1_1_CFG1.addr = 0x1D;
	reg->PLL1_1_CFG2.addr = 0x1E;
	reg->PLL1_1_CFG3.addr = 0x1F;
}

static Alx_Status Cdce913_RegRead(AlxCdce913* me, void* reg)
{
	Alx_Status status = Alx_Err;

	uint8_t regAddr = *((uint8_t*)reg);
	regAddr |= CDCE913_BYTE_ACCESS_MASK;

	uint8_t* regValPtr = (uint8_t*)reg + sizeof(regAddr);

	status = AlxI2c_Master_StartReadMemStop(
		me->i2c,
		CDCE913_I2C_ADDR,
		regAddr,
		AlxI2c_Master_MemAddrLen_8bit,
		regValPtr,
		1,
		me->i2cNumOfTries,
		me->i2cTimeout_ms);

	return status;
}

static Alx_Status Cdce913_RegWrite(AlxCdce913* me, void* reg)
{
	Alx_Status status = Alx_Err;

	uint8_t regAddr = *((uint8_t*)reg);
	regAddr |= CDCE913_BYTE_ACCESS_MASK;

	uint8_t regVal = *((uint8_t*)reg + sizeof(regAddr));

	status = AlxI2c_Master_StartWriteMemStop_Single(
		me->i2c,
		CDCE913_I2C_ADDR,
		regAddr,
		1,
		regVal,
		false,
		me->i2cNumOfTries,
		me->i2cTimeout_ms);

	return status;
}

static Alx_Status Cdce913_RegReadAll(AlxCdce913* me)
{
	Alx_Status status = Alx_Ok;
	status |= Cdce913_RegRead(me, &me->reg.ID);
	status |= Cdce913_RegRead(me, &me->reg.DEV_CTRL0);
	status |= Cdce913_RegRead(me, &me->reg.DEV_CTRL1);
	status |= Cdce913_RegRead(me, &me->reg.PDIV1_LSB);
	status |= Cdce913_RegRead(me, &me->reg.CON_Y1);
	status |= Cdce913_RegRead(me, &me->reg.XCSEL);
	status |= Cdce913_RegRead(me, &me->reg.DEV_CTRL2);
	status |= Cdce913_RegRead(me, &me->reg.CON_SSC0);
	status |= Cdce913_RegRead(me, &me->reg.CON_SSC1);
	status |= Cdce913_RegRead(me, &me->reg.CON_SSC2);
	status |= Cdce913_RegRead(me, &me->reg.CON_FS);
	status |= Cdce913_RegRead(me, &me->reg.DEV_CTRL3);
	status |= Cdce913_RegRead(me, &me->reg.CON_Y2Y3);
	status |= Cdce913_RegRead(me, &me->reg.DEV_CTRL4);
	status |= Cdce913_RegRead(me, &me->reg.PDIV3);
	status |= Cdce913_RegRead(me, &me->reg.PLL1_0_CFG0);
	status |= Cdce913_RegRead(me, &me->reg.PLL1_0_CFG1);
	status |= Cdce913_RegRead(me, &me->reg.PLL1_0_CFG2);
	status |= Cdce913_RegRead(me, &me->reg.PLL1_0_CFG3);
	status |= Cdce913_RegRead(me, &me->reg.PLL1_1_CFG0);
	status |= Cdce913_RegRead(me, &me->reg.PLL1_1_CFG1);
	status |= Cdce913_RegRead(me, &me->reg.PLL1_1_CFG2);
	status |= Cdce913_RegRead(me, &me->reg.PLL1_1_CFG3);

	return (status == Alx_Ok) ? Alx_Ok : Alx_Err;
}

static Alx_Status Cdce913_RegWriteAll(AlxCdce913* me)
{
	Alx_Status status = Alx_Ok;
	status |= Cdce913_RegWrite(me, &me->reg.DEV_CTRL0);
	status |= Cdce913_RegWrite(me, &me->reg.DEV_CTRL1);
	status |= Cdce913_RegWrite(me, &me->reg.PDIV1_LSB);
	status |= Cdce913_RegWrite(me, &me->reg.CON_Y1);
	status |= Cdce913_RegWrite(me, &me->reg.XCSEL);
	status |= Cdce913_RegWrite(me, &me->reg.DEV_CTRL2);
	status |= Cdce913_RegWrite(me, &me->reg.CON_SSC0);
	status |= Cdce913_RegWrite(me, &me->reg.CON_SSC1);
	status |= Cdce913_RegWrite(me, &me->reg.CON_SSC2);
	status |= Cdce913_RegWrite(me, &me->reg.CON_FS);
	status |= Cdce913_RegWrite(me, &me->reg.DEV_CTRL3);
	status |= Cdce913_RegWrite(me, &me->reg.CON_Y2Y3);
	status |= Cdce913_RegWrite(me, &me->reg.DEV_CTRL4);
	status |= Cdce913_RegWrite(me, &me->reg.PDIV3);
	status |= Cdce913_RegWrite(me, &me->reg.PLL1_0_CFG0);
	status |= Cdce913_RegWrite(me, &me->reg.PLL1_0_CFG1);
	status |= Cdce913_RegWrite(me, &me->reg.PLL1_0_CFG2);
	status |= Cdce913_RegWrite(me, &me->reg.PLL1_0_CFG3);
	status |= Cdce913_RegWrite(me, &me->reg.PLL1_1_CFG0);
	status |= Cdce913_RegWrite(me, &me->reg.PLL1_1_CFG1);
	status |= Cdce913_RegWrite(me, &me->reg.PLL1_1_CFG2);
	status |= Cdce913_RegWrite(me, &me->reg.PLL1_1_CFG3);

	return (status == Alx_Ok) ? Alx_Ok : Alx_Err;
}

static Alx_Status Cdce913_SetPdiv1(AlxCdce913* me, uint16_t val)
{
    if (val > 1023)
	{
		return Alx_Err;
	}

	me->reg.PDIV1_LSB.val.PDIV1_0_7 = (uint8_t)((val & 0x00FF) >> 0);
	me->reg.DEV_CTRL1.val.PDIV1_8_9 = (uint8_t)((val & 0x0300) >> 8);

	return Alx_Ok;
}

static Alx_Status Cdce913_SetSsc1_0(AlxCdce913* me, AlxCdce913_RegEnum_SSC_MOD_AMOUNT val)
{
	me->reg.CON_SSC2.val.SSC1_0 = val;
	return Alx_Ok;
}

static Alx_Status Cdce913_SetSsc1_1(AlxCdce913* me, AlxCdce913_RegEnum_SSC_MOD_AMOUNT val)
{
	me->reg.CON_SSC2.val.SSC1_1 = val;
	return Alx_Ok;
}

static Alx_Status Cdce913_SetSsc1_2(AlxCdce913* me, AlxCdce913_RegEnum_SSC_MOD_AMOUNT val)
{
	me->reg.CON_SSC2.val.SSC1_2_0_1 = (uint8_t)((val & 0x03) >> 0);
	me->reg.CON_SSC1.val.SSC1_2_2 = (uint8_t)((val & 0x04) >> 2);
	return Alx_Ok;
}

static Alx_Status Cdce913_SetSsc1_3(AlxCdce913* me, AlxCdce913_RegEnum_SSC_MOD_AMOUNT val)
{
	me->reg.CON_SSC1.val.SSC1_3 = val;
	return Alx_Ok;
}

static Alx_Status Cdce913_SetSsc1_4(AlxCdce913* me, AlxCdce913_RegEnum_SSC_MOD_AMOUNT val)
{
	me->reg.CON_SSC1.val.SSC1_4 = val;
	return Alx_Ok;
}

static Alx_Status Cdce913_SetSsc1_5(AlxCdce913* me, AlxCdce913_RegEnum_SSC_MOD_AMOUNT val)
{
	me->reg.CON_SSC1.val.SSC1_5_0 = (uint8_t)((val & 0x01) >> 0);
	me->reg.CON_SSC0.val.SSC1_5_1_2 = (uint8_t)((val & 0x06) >> 1);
	return Alx_Ok;
}

static Alx_Status Cdce913_SetSsc1_6(AlxCdce913* me, AlxCdce913_RegEnum_SSC_MOD_AMOUNT val)
{
	me->reg.CON_SSC0.val.SSC1_6 = val;
	return Alx_Ok;
}

static Alx_Status Cdce913_SetSsc1_7(AlxCdce913* me, AlxCdce913_RegEnum_SSC_MOD_AMOUNT val)
{
	me->reg.CON_SSC0.val.SSC1_7 = val;
	return Alx_Ok;
}

static Alx_Status Cdce913_SetPll1_0N(AlxCdce913* me, uint16_t val)
{
	if ((val == 0) || (val > 4095))
	{
		return Alx_Err;
	}

	me->reg.PLL1_0_CFG0.val.PLL1_0N_4_11 = (uint8_t)((val & 0x0FF0) >> 4);
	me->reg.PLL1_0_CFG1.val.PLL1_0N_0_3 = (uint8_t)((val & 0x000F) >> 0);
	return Alx_Ok;
}

static Alx_Status Cdce913_SetPll1_0R(AlxCdce913* me, uint16_t val)
{
	if (val > 511)
	{
		return Alx_Err;
	}

	me->reg.PLL1_0_CFG1.val.PLL1_0R_5_8 = (uint8_t)((val & 0x01E0) >> 5);
	me->reg.PLL1_0_CFG2.val.PLL1_0R_0_4 = (uint8_t)((val & 0x001F) >> 0);
	return Alx_Ok;
}

static Alx_Status Cdce913_SetPll1_0Q(AlxCdce913* me, uint8_t val)
{
	if ((val < 16) || (val > 63))
	{
		return Alx_Err;
	}

	me->reg.PLL1_0_CFG2.val.PLL1_0Q_3_5 = (val & 0x38) >> 3;
	me->reg.PLL1_0_CFG3.val.PLL1_0Q_0_2 = (val & 0x07) >> 0;
	return Alx_Ok;
}

static Alx_Status Cdce913_SetPll1_0P(AlxCdce913* me, uint8_t val)
{
	if (val > 7)
	{
		return Alx_Err;
	}

	me->reg.PLL1_0_CFG3.val.PLL1_0P = val;
	return Alx_Ok;
}

static Alx_Status Cdce913_SetPll1_1N(AlxCdce913* me, uint16_t val)
{
	if ((val == 0) || (val > 4095))
	{
		return Alx_Err;
	}

	me->reg.PLL1_1_CFG0.val.PLL1_1N_4_11 = (uint8_t)((val & 0x0FF0) >> 4);
	me->reg.PLL1_1_CFG1.val.PLL1_1N_0_3 = (uint8_t)((val & 0x000F) >> 0);
	return Alx_Ok;
}

static Alx_Status Cdce913_SetPll1_1R(AlxCdce913* me, uint16_t val)
{
	if (val > 511)
	{
		return Alx_Err;
	}

	me->reg.PLL1_1_CFG1.val.PLL1_1R_5_8 = (uint8_t)((val & 0x01E0) >> 5);
	me->reg.PLL1_1_CFG2.val.PLL1_1R_0_4 = (uint8_t)((val & 0x001F) >> 0);
	return Alx_Ok;
}

static Alx_Status Cdce913_SetPll1_1Q(AlxCdce913* me, uint8_t val)
{
	if ((val < 16) || (val > 63))
	{
		return Alx_Err;
	}

	me->reg.PLL1_1_CFG2.val.PLL1_1Q_3_5 = (val & 0x38) >> 3;
	me->reg.PLL1_1_CFG3.val.PLL1_1Q_0_2 = (val & 0x07) >> 0;
	return Alx_Ok;
}

static Alx_Status Cdce913_SetPll1_1P(AlxCdce913* me, uint8_t val)
{
	if (val > 7)
	{
		return Alx_Err;
	}

	me->reg.PLL1_1_CFG3.val.PLL1_1P = val;
	return Alx_Ok;
}

static void Cdce913_SetInitialConfig
(
	AlxCdce913* me,
	AlxCdce913_RegEnum_INCLK xtalSource,
	uint8_t xtalCapacitance_pF
)
{
	me->reg.DEV_CTRL0.val.raw = 0;
	me->reg.DEV_CTRL0.val.EELOCK = EEPROM_LOCK_NotLocked;
	me->reg.DEV_CTRL0.val.PWDN = PWDN_DeviceActive;
	me->reg.DEV_CTRL0.val.INCLK = xtalSource;
	me->reg.DEV_CTRL0.val.TARGET_ADR = 0b01;

	me->reg.DEV_CTRL1.val.M1 = Y1_CLK_Input;
	me->reg.DEV_CTRL1.val.SPICON = SPICON_Spi;
	me->reg.DEV_CTRL1.val.Y1_ST1 = Y1_ST_Enabled;
	me->reg.DEV_CTRL1.val.Y1_ST0 = Y1_ST_DisabledHiZ;

	Cdce913_SetPdiv1(me, 0);

	// Y1 state1 (enabled) in state 1
	me->reg.CON_Y1.val.Y1_7 = Y1_X_State0;
	me->reg.CON_Y1.val.Y1_6 = Y1_X_State0;
	me->reg.CON_Y1.val.Y1_5 = Y1_X_State0;
	me->reg.CON_Y1.val.Y1_4 = Y1_X_State0;
	me->reg.CON_Y1.val.Y1_3 = Y1_X_State0;
	me->reg.CON_Y1.val.Y1_2 = Y1_X_State0;
	me->reg.CON_Y1.val.Y1_1 = Y1_X_State1;
	me->reg.CON_Y1.val.Y1_0 = Y1_X_State0;

	me->reg.XCSEL.val.XCSEL = xtalCapacitance_pF;

	me->reg.DEV_CTRL2.val.BCOUNT = 0x20;
	me->reg.DEV_CTRL2.val.EEWRITE = 0;

	// SSC off in all states
	Cdce913_SetSsc1_0(me, SSC_MOD_AMOUNT_Off);
	Cdce913_SetSsc1_1(me, SSC_MOD_AMOUNT_Off);
	Cdce913_SetSsc1_2(me, SSC_MOD_AMOUNT_Off);
	Cdce913_SetSsc1_3(me, SSC_MOD_AMOUNT_Off);
	Cdce913_SetSsc1_4(me, SSC_MOD_AMOUNT_Off);
	Cdce913_SetSsc1_5(me, SSC_MOD_AMOUNT_Off);
	Cdce913_SetSsc1_6(me, SSC_MOD_AMOUNT_Off);
	Cdce913_SetSsc1_7(me, SSC_MOD_AMOUNT_Off);

	// PLL1_0 settings in all states
	me->reg.CON_FS.val.raw = 0;

	me->reg.DEV_CTRL3.val.MUX1 = MUX1_PllBypass;
	me->reg.DEV_CTRL3.val.M2 = Y2_MUX_Pdiv2;
	me->reg.DEV_CTRL3.val.M3 = Y3_MUX_Pdiv3;
	me->reg.DEV_CTRL3.val.Y2Y3_ST1 = Y2Y3_ST_Enabled;
	me->reg.DEV_CTRL3.val.Y2Y3_ST0 = Y2Y3_ST_DisabledHiZ;

	// Y2Y3 state0 (disabled HiZ) in all states
	me->reg.CON_Y2Y3.val.raw = 0;

	me->reg.DEV_CTRL4.val.SSC1DC = PLL1_SSC_SEL_Down;
	me->reg.DEV_CTRL4.val.PDIV2 = 0;

	me->reg.PDIV3.val.PDIV3 = 0;

	Cdce913_SetPll1_0N(me, 3125);
	Cdce913_SetPll1_0R(me, 20);
	Cdce913_SetPll1_0Q(me, 26);
	Cdce913_SetPll1_0P(me, 2);
	me->reg.PLL1_0_CFG3.val.VCO1_0_RANGE = VCO1_X_RANGE_Min125Mhz;

	Cdce913_SetPll1_1N(me, 3125);
	Cdce913_SetPll1_1R(me, 20);
	Cdce913_SetPll1_1Q(me, 26);
	Cdce913_SetPll1_1P(me, 2);
	me->reg.PLL1_1_CFG3.val.VCO1_1_RANGE = VCO1_X_RANGE_Min125Mhz;
}

//******************************************************************************
// Constructor
//******************************************************************************

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		i2c
  */
void AlxCdce913_Ctor
(
	AlxCdce913* me,
	AlxI2c* i2c,
	AlxDac* dac,
	float dacReference_V,
	Alx_Ch dacChannel,
	AlxIoPin* pllEnable
)
{
	// Parameters
	me->i2c = i2c;
	me->dac = dac;
	me->dacReference_V = dacReference_V;
	me->dacChannel = dacChannel;
	me->pllEnable = pllEnable;

	// Variables
	Cdce913_RegStructInit(&me->reg);
	me->i2cNumOfTries = 1;
	me->i2cTimeout_ms = 1000;

	// Info
	me->wasCtorCalled = true;
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
Alx_Status AlxCdce913_Init
(
	AlxCdce913* me,
	float xtalFrequency,
	AlxCdce913_RegEnum_INCLK xtalSource,
	uint8_t xtalCapacitance_pF
)
{
	// Assert
	ALX_CDCE913_ASSERT(me->wasCtorCalled == true);
	ALX_CDCE913_ASSERT(me->isInit == false);

	// Hard requirements to simplify driver
	ALX_CDCE913_ASSERT(xtalFrequency == 12.288f);
	ALX_CDCE913_ASSERT(xtalSource == INCLK_Vcxo);

	ALX_CDCE913_ASSERT(xtalCapacitance_pF <= 20);

	// Local variables
	Alx_Status status = Alx_Err;

	AlxIoPin_Reset(me->pllEnable);

	if (!me->dac->isInit)
	{
		AlxDac_Init_CalibrateVref(me->dac, me->dacReference_V);
	}

	status = AlxDac_SetVoltage_V_CalibrateVref(
		me->dac,
		me->dacChannel,
		(CDCE913_VCTRL_MAX_V - CDCE913_VCTRL_MIN_V) / 2.0f,
		me->dacReference_V);

	if (!me->i2c->isInit)
	{
		AlxI2c_Init(me->i2c);
	}

	Cdce913_SetInitialConfig(me, xtalSource, xtalCapacitance_pF);
	status = Cdce913_RegWriteAll(me);
	if (status != Alx_Ok)
	{
		ALX_CDCE913_TRACE_ERR("Failed to write CDCE913 registers");
		return Alx_Err;
	}

	#if defined(ALX_CDCE913_VERBOSE_DEBUG_ENABLED)
	status = Cdce913_RegReadAll(me);
	if (status != Alx_Ok)
	{
		ALX_CDCE913_TRACE_ERR("Failed to read CDCE913 registers");
		return Alx_Err;
	}
	ALX_CDCE913_TRACE_INF("REG 0x%02X: 0x%02X", me->reg.ID.addr, me->reg.ID.val.raw);
	ALX_CDCE913_TRACE_INF("REG 0x%02X: 0x%02X", me->reg.DEV_CTRL0.addr, me->reg.DEV_CTRL0.val.raw);
	ALX_CDCE913_TRACE_INF("REG 0x%02X: 0x%02X", me->reg.DEV_CTRL1.addr, me->reg.DEV_CTRL1.val.raw);
	ALX_CDCE913_TRACE_INF("REG 0x%02X: 0x%02X", me->reg.PDIV1_LSB.addr, me->reg.PDIV1_LSB.val.raw);
	ALX_CDCE913_TRACE_INF("REG 0x%02X: 0x%02X", me->reg.CON_Y1.addr, me->reg.CON_Y1.val.raw);
	ALX_CDCE913_TRACE_INF("REG 0x%02X: 0x%02X", me->reg.XCSEL.addr, me->reg.XCSEL.val.raw);
	ALX_CDCE913_TRACE_INF("REG 0x%02X: 0x%02X", me->reg.DEV_CTRL2.addr, me->reg.DEV_CTRL2.val.raw);
	ALX_CDCE913_TRACE_INF("REG 0x%02X: 0x%02X", me->reg.CON_SSC0.addr, me->reg.CON_SSC0.val.raw);
	ALX_CDCE913_TRACE_INF("REG 0x%02X: 0x%02X", me->reg.CON_SSC1.addr, me->reg.CON_SSC1.val.raw);
	ALX_CDCE913_TRACE_INF("REG 0x%02X: 0x%02X", me->reg.CON_SSC2.addr, me->reg.CON_SSC2.val.raw);
	ALX_CDCE913_TRACE_INF("REG 0x%02X: 0x%02X", me->reg.CON_FS.addr, me->reg.CON_FS.val.raw);
	ALX_CDCE913_TRACE_INF("REG 0x%02X: 0x%02X", me->reg.DEV_CTRL3.addr, me->reg.DEV_CTRL3.val.raw);
	ALX_CDCE913_TRACE_INF("REG 0x%02X: 0x%02X", me->reg.CON_Y2Y3.addr, me->reg.CON_Y2Y3.val.raw);
	ALX_CDCE913_TRACE_INF("REG 0x%02X: 0x%02X", me->reg.DEV_CTRL4.addr, me->reg.DEV_CTRL4.val.raw);
	ALX_CDCE913_TRACE_INF("REG 0x%02X: 0x%02X", me->reg.PDIV3.addr, me->reg.PDIV3.val.raw);
	ALX_CDCE913_TRACE_INF("REG 0x%02X: 0x%02X", me->reg.PLL1_0_CFG0.addr, me->reg.PLL1_0_CFG0.val.raw);
	ALX_CDCE913_TRACE_INF("REG 0x%02X: 0x%02X", me->reg.PLL1_0_CFG1.addr, me->reg.PLL1_0_CFG1.val.raw);
	ALX_CDCE913_TRACE_INF("REG 0x%02X: 0x%02X", me->reg.PLL1_0_CFG2.addr, me->reg.PLL1_0_CFG2.val.raw);
	ALX_CDCE913_TRACE_INF("REG 0x%02X: 0x%02X", me->reg.PLL1_0_CFG3.addr, me->reg.PLL1_0_CFG3.val.raw);
	ALX_CDCE913_TRACE_INF("REG 0x%02X: 0x%02X", me->reg.PLL1_1_CFG0.addr, me->reg.PLL1_1_CFG0.val.raw);
	ALX_CDCE913_TRACE_INF("REG 0x%02X: 0x%02X", me->reg.PLL1_1_CFG1.addr, me->reg.PLL1_1_CFG1.val.raw);
	ALX_CDCE913_TRACE_INF("REG 0x%02X: 0x%02X", me->reg.PLL1_1_CFG2.addr, me->reg.PLL1_1_CFG2.val.raw);
	ALX_CDCE913_TRACE_INF("REG 0x%02X: 0x%02X", me->reg.PLL1_1_CFG3.addr, me->reg.PLL1_1_CFG3.val.raw);
	#endif  // ALX_CDCE913_VERBOSE_DEBUG_ENABLED

	me->xtalFrequency_MHz = xtalFrequency;

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
Alx_Status AlxCdce913_DeInit(AlxCdce913* me)
{
	// Assert
	ALX_CDCE913_ASSERT(me->wasCtorCalled == true);
	ALX_CDCE913_ASSERT(me->isInit == true);

	AlxIoPin_Reset(me->pllEnable);
	AlxDac_SetVoltage_V_CalibrateVref(
		me->dac,
		me->dacChannel,
		CDCE913_VCTRL_MIN_V,
		me->dacReference_V);

	// Clear isInit
	me->isInit = false;

	// Return
	return Alx_Ok;
}

Alx_Status AlxCdce913_SetY1OutputFreq(AlxCdce913* me, AlxCdce913_OutputY1 freq)
{
	switch (freq)
	{
		case ALX_CDCE913_OUTPUT_Y1_1024KHZ0:
			Cdce913_SetPdiv1(me, 12);
			break;
		case ALX_CDCE913_OUTPUT_Y1_819KHZ2:
			Cdce913_SetPdiv1(me, 15);
			break;
		default:
			return Alx_Err;
	}

	Alx_Status status = Cdce913_RegWrite(me, &me->reg.DEV_CTRL1);
	status |= Cdce913_RegWrite(me, &me->reg.PDIV1_LSB);

	return status;
}

Alx_Status AlxCdce913_SetVctrl(AlxCdce913* me, float vctrl_V)
{
	if (vctrl_V < CDCE913_VCTRL_MIN_V)
	{
		vctrl_V = CDCE913_VCTRL_MIN_V;
	}

	if (vctrl_V > CDCE913_VCTRL_MAX_V)
	{
		vctrl_V = CDCE913_VCTRL_MAX_V;
	}

	return AlxDac_SetVoltage_V_CalibrateVref(me->dac, me->dacChannel, vctrl_V, me->dacReference_V);
}

Alx_Status AlxCdce913_EnableY1(AlxCdce913* me)
{
	AlxIoPin_Set(me->pllEnable);
	return Alx_Ok;
}

Alx_Status AlxCdce913_DisableY1(AlxCdce913* me)
{
	AlxIoPin_Reset(me->pllEnable);
	return Alx_Ok;
}


#endif	// #if defined(ALX_C_LIB) && defined(ALX_STM32L4)
