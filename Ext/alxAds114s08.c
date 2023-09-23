/**
  ******************************************************************************
  * @file		alxAds114s08.c
  * @brief		Auralix C Library - ALX Current Monitor ADS114S08 Module
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
#include "alxAds114s08.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB)


//******************************************************************************
// Private Types
//******************************************************************************


//******************************************************************************
// Private Variables
//******************************************************************************
float transferFun_thermocoupleS_PointArr_mV[] = { -0.236, 0.000, 0.173, 0.646, 1.441, 2.323, 3.259, 4.233, 5.239, 6.275, 7.345, 8.449, 9.587, 10.757, 11.951, 13.159, 15.582, 17.947 };
float transferFun_thermocoupleS_PointArr_degC[] = { -50, 0, 30, 100, 200, 300, 400, 500, 600, 700, 800, 900, 1000, 1100, 1200, 1300, 1500, 1700 };
#define NUM_OF_PONTS_TRANSFER_FUN_THERMCOUPLE_S (sizeof(transferFun_thermocoupleS_PointArr_mV) / sizeof(transferFun_thermocoupleS_PointArr_mV[0]))

//******************************************************************************
// Private Functions
//******************************************************************************
static void AlxAds114s08_RegStruct_SetAddr(AlxAds114s08* me);
static void AlxAds114s08_RegStruct_SetLen(AlxAds114s08* me);
static void AlxAds114s08_RegStruct_SetValToZero(AlxAds114s08* me);
static void AlxAds114s08_RegStruct_SetToDefault(AlxAds114s08* me);
static Alx_Status AlxAds114s08_Cmd_RREG(AlxAds114s08* me, uint8_t regAddr, uint8_t len, uint8_t* data);

static Alx_Status AlxAds114s08_Reg_Read(AlxAds114s08* me, void* reg);
static Alx_Status AlxAds114s08_Reg_Write(AlxAds114s08* me, void* reg);
static Alx_Status AlxAds114s08_Reg_WriteAndRead(AlxAds114s08* me, void* reg);
static Alx_Status AlxAds114s08_Reg_WriteAll(AlxAds114s08* me);
Alx_Status AlxAds114s08_PerformAdcConversion(AlxAds114s08* me, int16_t* chVoltage_raw, uint32_t timeout_ms);
Alx_Status AlxAds114s08_Convert_raw_mV(AlxAds114s08* me, int16_t chVoltage_raw, float* chVoltage_mV, bool* isOutOfRange);

//******************************************************************************
// Weak Functions
//******************************************************************************
void AlxAds114s08_RegStruct_SetVal(AlxAds114s08* me);


//******************************************************************************
// Constructor
//******************************************************************************
void AlxAds114s08_Ctor
(
AlxAds114s08* me,
	AlxSpi* spi,
	uint8_t spiNumOfTries,
	uint16_t spiTimeout_ms,
	AlxIoPin* di_nDRDY,
	AlxIoPin* do_START,
	AlxIoPin* do_nRST)
{
	// Parameters
	me->spi = spi;
	me->spiNumOfTries = spiNumOfTries;
	me->spiTimeout_ms = spiTimeout_ms;
	me->di_nDRDY = di_nDRDY;
	me->do_START = do_START;
	me->do_nRST = do_nRST;
	me->CONVERSION_TIMEOUT_ms = 750;	// action takes up to 250ms, but might get delayed due RTOS load

	// Variables
	AlxAds114s08_RegStruct_SetAddr(me);
	AlxAds114s08_RegStruct_SetLen(me);
	AlxAds114s08_RegStruct_SetValToZero(me);
	AlxTimSw_Ctor(&me->timeoutTimer, false);

	// Prepare transfer function for thermocouple S conversion from mV to degC
	AlxInterpLin_Ctor
	(
		&me->transferFun_thermocoupleS_mV_degC,
		transferFun_thermocoupleS_PointArr_mV,
		transferFun_thermocoupleS_PointArr_degC,
		NUM_OF_PONTS_TRANSFER_FUN_THERMCOUPLE_S,
		true);

	// Prepare transfer function for thermocouple S conversion from degC to mV
	AlxInterpLin_Ctor
	(
		&me->transferFun_thermocoupleS_degC_mV,
		transferFun_thermocoupleS_PointArr_degC,
		transferFun_thermocoupleS_PointArr_mV,
		NUM_OF_PONTS_TRANSFER_FUN_THERMCOUPLE_S,
		true);

	// Info
	me->wasCtorCalled = true;
	me->isInitPeriph = false;
	me->isInit = false;
}


//******************************************************************************
// Functions
//******************************************************************************
Alx_Status AlxAds114s08_InitPeriph(AlxAds114s08* me)
{
	// Assert
	ALX_ADS114S08_ASSERT(me->wasCtorCalled == true);
	ALX_ADS114S08_ASSERT(me->isInitPeriph == false);
	// isInit -> Don't care

	// Local variables
	Alx_Status status = Alx_Err;

	// Init GPIO
	AlxIoPin_Init(me->di_nDRDY);
	AlxIoPin_Init(me->do_START);
	AlxIoPin_Init(me->do_nRST);

	// Init SPI
	status = AlxSpi_Init(me->spi);
	if (status != Alx_Ok) { ALX_ADS114S08_TRACE("Err"); return status; }

	// Set isInitPeriph
	me->isInitPeriph = true;

	// Return
	return Alx_Ok;
}
Alx_Status AlxAds114s08_DeInitPeriph(AlxAds114s08* me)
{
	// Assert
	ALX_ADS114S08_ASSERT(me->wasCtorCalled == true);
	ALX_ADS114S08_ASSERT(me->isInitPeriph == true);
	// isInit -> Don't care

	// Local variables
	Alx_Status status = Alx_Err;

	// DeInit SPI
	status = AlxSpi_DeInit(me->spi);
	if (status != Alx_Ok) { ALX_ADS114S08_TRACE("Err"); return status; }

	// DeInit GPIO
	AlxIoPin_DeInit(me->di_nDRDY);
	AlxIoPin_DeInit(me->do_START);
	AlxIoPin_DeInit(me->do_nRST);

	// Clear isInitPeriph
	me->isInitPeriph = false;

	// Return
	return Alx_Ok;
}
Alx_Status AlxAds114s08_Init(AlxAds114s08* me)
{
	// Assert
	ALX_ADS114S08_ASSERT(me->wasCtorCalled == true);
	ALX_ADS114S08_ASSERT(me->isInitPeriph == true);
	ALX_ADS114S08_ASSERT(me->isInit == false);

	// Local variables
	Alx_Status status = Alx_Err;

	// Set register struct values to default
	AlxAds114s08_RegStruct_SetToDefault(me);

	// Reset device
	AlxIoPin_Reset(me->do_nRST);
	AlxDelay_ms(10);

	// Wake-up device from power down mode
	AlxIoPin_Set(me->do_nRST);

	// Wait for device to wake-up
	AlxDelay_ms(100);

	// Write registers
	status = AlxAds114s08_Reg_WriteAll(me);
	if(status != Alx_Ok) { ALX_ADS114S08_TRACE("Err"); return Alx_Err; }

	// Set isInit
	me->isInit = true;

	// Return
	return Alx_Ok;
}
Alx_Status AlxAds114s08_DeInit(AlxAds114s08* me)
{
	// Assert
	ALX_ADS114S08_ASSERT(me->wasCtorCalled == true);
	ALX_ADS114S08_ASSERT(me->isInitPeriph == true);
	ALX_ADS114S08_ASSERT(me->isInit == true);

	// Local variables
	Alx_Status status = Alx_Err;

	// Set register struct values to default
	AlxAds114s08_RegStruct_SetToDefault(me);

	// Write registers
	status = AlxAds114s08_Reg_WriteAll(me);
	if (status != Alx_Ok) { ALX_ADS114S08_TRACE("Err"); return status; }

	// Place device in power down mode (all registers are reset to default values)
	AlxIoPin_Reset(me->di_nDRDY);
	AlxIoPin_Reset(me->do_START);
	AlxIoPin_Reset(me->do_nRST);

	// Clear isInit
	me->isInit = false;

	// Return
	return Alx_Ok;
}
Alx_Status AlxAds114s08_GetDevId(AlxAds114s08* me, Ads114s08_RegEnum_0x00_DEV_ID* DEV_ID)
{
	// Assert
	ALX_ADS114S08_ASSERT(me->wasCtorCalled == true);
	ALX_ADS114S08_ASSERT(me->isInitPeriph == true);
	ALX_ADS114S08_ASSERT(me->isInit == true);

	// Local variables
	Alx_Status status = Alx_Err;

	// Read all rgisters
	status = AlxAds114s08_Reg_Read(me, (void*)&me->reg.reg_0x00_ID);
	if (status != Alx_Ok) { ALX_ADS114S08_TRACE("Err"); return Alx_Err; }

	// Return ID register
	*DEV_ID = me->reg.reg_0x00_ID.val.DEV_ID;
	return Alx_Ok;
}
Alx_Status AlxAds114s08_GetChVoltage_mV(AlxAds114s08* me, uint8_t chNum, float* chVoltage_mV, bool* isOutOfRange)
{
	// Assert
	ALX_ADS114S08_ASSERT(me->wasCtorCalled == true);
	ALX_ADS114S08_ASSERT(me->isInitPeriph == true);
	ALX_ADS114S08_ASSERT(me->isInit == true);
	ALX_ADS114S08_ASSERT(chNum < 6);

	// Local variables
	Alx_Status status = Alx_Err;

	// Configure registers
	AlxAds114s08_RegStruct_SetToDefault(me);
	switch (chNum)
	{
	case 0: {
			me->reg.reg_0x02_INPMUX.val.MUXP = MUXP_AIN0;
			me->reg.reg_0x02_INPMUX.val.MUXN = MUXN_AIN1;
		break;
		}
	case 1: {
			me->reg.reg_0x02_INPMUX.val.MUXP = MUXP_AIN2;
			me->reg.reg_0x02_INPMUX.val.MUXN = MUXN_AIN3;
		break;
		}
	case 2: {
			me->reg.reg_0x02_INPMUX.val.MUXP = MUXP_AIN4;
			me->reg.reg_0x02_INPMUX.val.MUXN = MUXN_AIN5;
		break;
		}
	case 3: {
			me->reg.reg_0x02_INPMUX.val.MUXP = MUXP_AIN6;
			me->reg.reg_0x02_INPMUX.val.MUXN = MUXN_AIN7;
		break;
		}
	case 4: {
			me->reg.reg_0x02_INPMUX.val.MUXP = MUXP_AIN8;
			me->reg.reg_0x02_INPMUX.val.MUXN = MUXN_AIN9;
		break;
		}
	case 5: {
			me->reg.reg_0x02_INPMUX.val.MUXP = MUXP_AIN10;
			me->reg.reg_0x02_INPMUX.val.MUXN = MUXN_AIN11;
			break;
		}
	default: ALX_ADS114S08_ASSERT(false);
	}

	me->reg.reg_0x03_PGA.val.GAIN = PGA_GAIN_128;
	me->reg.reg_0x03_PGA.val.PGA_EN = PGA_EN_PgaIsEnabled;
	me->reg.reg_0x04_DATARATE.val.G_CHOP = G_CHOP_Enabled;
	me->reg.reg_0x04_DATARATE.val.DR = DR_10sps;
	me->reg.reg_0x04_DATARATE.val.MODE = MODE_SingleConversion;
	me->reg.reg_0x05_REF.val.REFSEL = REFSEL_Internal2_5Vref;
	me->reg.reg_0x05_REF.val.REFCON = REFCON_InternalRefAlwaysOn;
	me->reg.reg_0x09_SYS.val.SYS_MON = SYS_MON_BurnOutCurrentSrcEn_0_2uA;	// GK: Should be dissabled if G_CHOP is enabled, but we need it and it doesn't seem to pose issues at low currents
		;
	status = AlxAds114s08_Reg_WriteAll(me);
	if (status != Alx_Ok) { ALX_ADS114S08_TRACE("Err"); return status; }

	//------------------------------------------------------------------------------
	// Perform ADC Conversion, Create Sample in blocking mode
	//------------------------------------------------------------------------------
	int16_t chVoltage_raw = 0;
	status = AlxAds114s08_PerformAdcConversion(me, &chVoltage_raw, me->CONVERSION_TIMEOUT_ms);
	if (status != Alx_Ok) { ALX_ADS114S08_TRACE("Err"); return status; }

	//------------------------------------------------------------------------------
	// Convert to mV
	//------------------------------------------------------------------------------
	status = AlxAds114s08_Convert_raw_mV(me, chVoltage_raw, chVoltage_mV, isOutOfRange);

	// Return
	return Alx_Ok;
}

Alx_Status AlxAds114s08_GetInternalTemp_degC(AlxAds114s08* me, float* internalTemp_degC)
{
	// Assert
	ALX_ADS114S08_ASSERT(me->wasCtorCalled == true);
	ALX_ADS114S08_ASSERT(me->isInitPeriph == true);
	ALX_ADS114S08_ASSERT(me->isInit == true);

	// Local variables
	Alx_Status status = Alx_Err;

	//------------------------------------------------------------------------------
	// Configure registers
	//------------------------------------------------------------------------------
	AlxAds114s08_RegStruct_SetToDefault(me);
	me->reg.reg_0x02_INPMUX.val.MUXN = MUXN_AIN0;
	me->reg.reg_0x02_INPMUX.val.MUXP = MUXP_AIN1;
	me->reg.reg_0x03_PGA.val.GAIN = PGA_GAIN_4;
	me->reg.reg_0x03_PGA.val.PGA_EN = PGA_EN_PgaIsEnabled;
	me->reg.reg_0x04_DATARATE.val.G_CHOP = G_CHOP_Dissabled;
	me->reg.reg_0x04_DATARATE.val.DR = DR_20sps;
	me->reg.reg_0x04_DATARATE.val.MODE = MODE_SingleConversion;
	me->reg.reg_0x05_REF.val.REFSEL = REFSEL_Internal2_5Vref;
	me->reg.reg_0x05_REF.val.REFCON = REFCON_InternalRefAlwaysOn;
	me->reg.reg_0x09_SYS.val.SYS_MON = SYS_MON_InternalTempSense;
	status = AlxAds114s08_Reg_WriteAll(me);
	if (status != Alx_Ok) { ALX_ADS114S08_TRACE("Err"); return status; }

	//------------------------------------------------------------------------------
	// Perform ADC Conversion, Create Sample in blocking mode
	//------------------------------------------------------------------------------
	int16_t adcVoltage_raw = 0;
	status = AlxAds114s08_PerformAdcConversion(me, &adcVoltage_raw, me->CONVERSION_TIMEOUT_ms);
	if (status != Alx_Ok) { ALX_ADS114S08_TRACE("Err"); return status; }

	//------------------------------------------------------------------------------
	// Convert to mV
	//------------------------------------------------------------------------------
	bool isOutOfRange = true;
	float adcVoltage_mV = -99999999;	// set out of range value
	status = AlxAds114s08_Convert_raw_mV(me, adcVoltage_raw, &adcVoltage_mV, &isOutOfRange);
	if (status != Alx_Ok) { ALX_ADS114S08_TRACE("Err"); return status; }

	//------------------------------------------------------------------------------
	// Convert to degC
	//------------------------------------------------------------------------------
	*internalTemp_degC = 25.f + (adcVoltage_mV - 129.f) / 0.403f;

	// Return
	return Alx_Ok;
}
Alx_Status AlxAds114s08_PerformAdcConversion(AlxAds114s08* me, int16_t* chVoltage_raw, uint32_t timeout_ms)
{
	// Assert
	ALX_ADS114S08_ASSERT(me->wasCtorCalled == true);
	ALX_ADS114S08_ASSERT(me->isInitPeriph == true);
	ALX_ADS114S08_ASSERT(me->isInit == true);

	// Local variables
	Alx_Status status = Alx_Err;

	//------------------------------------------------------------------------------
	// Start Conversion (START pin to high)
	//------------------------------------------------------------------------------
	// Set START pin
	AlxIoPin_Set(me->do_START);

	// Wait for Conversion to Complete (nDRDY goes low)
	AlxTimSw_Start(&me->timeoutTimer);
	while (AlxIoPin_Read(me->di_nDRDY))
	{
		if (AlxTimSw_IsTimeout_ms(&me->timeoutTimer, timeout_ms))
		{
			// DeAssert CS
			AlxSpi_Master_DeAssertCs(me->spi);

			// Reset START (START pin to low)
			AlxIoPin_Reset(me->do_START);

			// Return
			ALX_ADS114S08_TRACE("Err");
			return Alx_Err;
		}
	};

	//------------------------------------------------------------------------------
	// Read data
	//------------------------------------------------------------------------------
	// Assert CS
	AlxSpi_Master_AssertCs(me->spi);

	// Prepare
	uint8_t dataWriteArr[3] = {};
	uint8_t dataReadArr[3] = {};
	dataWriteArr[0] = 0x12; // RDATA cmd
	dataWriteArr[1] = 0x00; // dummy byte, here IC will return data
	dataWriteArr[2] = 0x00; // dummy byte, here IC will return data

	// Read
	status = AlxSpi_Master_WriteRead(me->spi, dataWriteArr, dataReadArr, sizeof(dataWriteArr), 1, 100);
	if (status != Alx_Ok) { ALX_ADS114S08_TRACE("Err"); return status; }

	// DeAssert CS
	AlxSpi_Master_DeAssertCs(me->spi);

	// Reset START (START pin to low)
	AlxIoPin_Reset(me->do_START);

	//------------------------------------------------------------------------------
	// Assemble bytes
	//------------------------------------------------------------------------------
	*chVoltage_raw = (dataReadArr[1] << 8) | dataReadArr[2];

	// Return
	return Alx_Ok;
}

Alx_Status AlxAds114s08_Convert_raw_mV(AlxAds114s08* me, int16_t chVoltage_raw, float* chVoltage_mV, bool* isOutOfRange)
{
	// Assert
	ALX_ADS114S08_ASSERT(me->reg.reg_0x05_REF.val.REFSEL == REFSEL_Internal2_5Vref);
	ALX_ADS114S08_ASSERT(me->reg.reg_0x05_REF.val.REFCON == REFCON_InternalRefAlwaysOn);

	//------------------------------------------------------------------------------
	// Check if value is close to Out Of Range
	//------------------------------------------------------------------------------
	int16_t positive_limit = (int16_t)(32767 * 0.95); // approximately 31128
	int16_t negative_limit = (int16_t)(-32768 * 0.95); // approximately -31129
	if (chVoltage_raw >= positive_limit || chVoltage_raw <= negative_limit)
	{
		*isOutOfRange = true;
	}
	else
	{
		*isOutOfRange = false;
	}

	//------------------------------------------------------------------------------
	// Define multiplier
	//------------------------------------------------------------------------------
	float vref_mV = 2500.f;
	float pgaMultiplier = 0;
	if (me->reg.reg_0x03_PGA.val.PGA_EN == PGA_EN_PgaIsEnabled)
	{
		switch (me->reg.reg_0x03_PGA.val.GAIN)
		{
		case PGA_GAIN_1:	pgaMultiplier = 1; break;
		case PGA_GAIN_2:	pgaMultiplier = 2; break;
		case PGA_GAIN_4:	pgaMultiplier = 4; break;
		case PGA_GAIN_8:	pgaMultiplier = 8; break;
		case PGA_GAIN_16:	pgaMultiplier = 16; break;
		case PGA_GAIN_32:	pgaMultiplier = 32; break;
		case PGA_GAIN_64:	pgaMultiplier = 64; break;
		case PGA_GAIN_128:	pgaMultiplier = 128; break;
		default: ALX_ADS114S08_ASSERT(false);
		}
	}
	else
	{
		pgaMultiplier = 1;
	}
	float multiplier = (vref_mV / 32767.f) / pgaMultiplier;

	//------------------------------------------------------------------------------
	// Prform Multiplication
	//------------------------------------------------------------------------------
	*chVoltage_mV = (float)chVoltage_raw * multiplier;

	// Return
	return Alx_Ok;
}


//******************************************************************************
// Private Functions
//******************************************************************************
static void AlxAds114s08_RegStruct_SetAddr(AlxAds114s08* me)
{
	me->reg.reg_0x00_ID.addr		=	0x00;
	me->reg.reg_0x01_STATUS.addr	=	0x01;
	me->reg.reg_0x02_INPMUX.addr	=	0x02;
	me->reg.reg_0x03_PGA.addr		=	0x03;
	me->reg.reg_0x04_DATARATE.addr	=	0x04;
	me->reg.reg_0x05_REF.addr		=	0x05;
	me->reg.reg_0x06_IDACMAG.addr	=	0x06;
	me->reg.reg_0x07_IDACMUX.addr	=	0x07;
	me->reg.reg_0x08_VBIAS.addr		=	0x08;
	me->reg.reg_0x09_SYS.addr		=	0x09;
	me->reg.reg_0x0A_RESERVED.addr	=	0x0A;
	me->reg.reg_0x0B_OFCAL0.addr	=	0x0B;
	me->reg.reg_0x0C_OFCAL1.addr	=	0x0C;
	me->reg.reg_0x0D_RESERVED.addr	=	0x0D;
	me->reg.reg_0x0E_FSCAL0.addr	=	0x0E;
	me->reg.reg_0x0F_FSCAL1.addr	=	0x0F;
	me->reg.reg_0x10_GPIODAT.addr	=	0x10;
	me->reg.reg_0x11_GPIOCON.addr	=	0x11;
}
static void AlxAds114s08_RegStruct_SetLen(AlxAds114s08* me)
{
	me->reg.reg_0x00_ID.len			= sizeof(me->reg.reg_0x00_ID.val);
	me->reg.reg_0x01_STATUS.len		= sizeof(me->reg.reg_0x01_STATUS.val);
	me->reg.reg_0x02_INPMUX.len		= sizeof(me->reg.reg_0x02_INPMUX.val);
	me->reg.reg_0x03_PGA.len		= sizeof(me->reg.reg_0x03_PGA.val);
	me->reg.reg_0x04_DATARATE.len	= sizeof(me->reg.reg_0x04_DATARATE.val);
	me->reg.reg_0x05_REF.len		= sizeof(me->reg.reg_0x05_REF.val);
	me->reg.reg_0x06_IDACMAG.len	= sizeof(me->reg.reg_0x06_IDACMAG.val);
	me->reg.reg_0x07_IDACMUX.len	= sizeof(me->reg.reg_0x07_IDACMUX.val);
	me->reg.reg_0x08_VBIAS.len		= sizeof(me->reg.reg_0x08_VBIAS.val);
	me->reg.reg_0x09_SYS.len		= sizeof(me->reg.reg_0x09_SYS.val);
	me->reg.reg_0x0A_RESERVED.len	= sizeof(me->reg.reg_0x0A_RESERVED.val);
	me->reg.reg_0x0B_OFCAL0.len		= sizeof(me->reg.reg_0x0B_OFCAL0.val);
	me->reg.reg_0x0C_OFCAL1.len		= sizeof(me->reg.reg_0x0C_OFCAL1.val);
	me->reg.reg_0x0D_RESERVED.len	= sizeof(me->reg.reg_0x0D_RESERVED.val);
	me->reg.reg_0x0E_FSCAL0.len		= sizeof(me->reg.reg_0x0E_FSCAL0.val);
	me->reg.reg_0x0F_FSCAL1.len		= sizeof(me->reg.reg_0x0F_FSCAL1.val);
	me->reg.reg_0x10_GPIODAT.len	= sizeof(me->reg.reg_0x10_GPIODAT.val);
	me->reg.reg_0x11_GPIOCON.len	= sizeof(me->reg.reg_0x11_GPIOCON.val);
}
static void AlxAds114s08_RegStruct_SetValToZero(AlxAds114s08* me)
{
	me->reg.reg_0x00_ID.val.raw =			0x00;
	me->reg.reg_0x01_STATUS.val.raw =		0x00;
	me->reg.reg_0x02_INPMUX.val.raw =		0x00;
	me->reg.reg_0x03_PGA.val.raw =			0x00;
	me->reg.reg_0x04_DATARATE.val.raw =		0x00;
	me->reg.reg_0x05_REF.val.raw =			0x00;
	me->reg.reg_0x06_IDACMAG.val.raw =		0x00;
	me->reg.reg_0x07_IDACMUX.val.raw =		0x00;
	me->reg.reg_0x08_VBIAS.val.raw =		0x00;
	me->reg.reg_0x09_SYS.val.raw =			0x00;
	me->reg.reg_0x0A_RESERVED.val.raw =		0x00;
	me->reg.reg_0x0B_OFCAL0.val.raw =		0x00;
	me->reg.reg_0x0C_OFCAL1.val.raw =		0x00;
	me->reg.reg_0x0D_RESERVED.val.raw =		0x00;
	me->reg.reg_0x0E_FSCAL0.val.raw =		0x00;
	me->reg.reg_0x0F_FSCAL1.val.raw =		0x00;
	me->reg.reg_0x10_GPIODAT.val.raw =		0x00;
	me->reg.reg_0x11_GPIOCON.val.raw =		0x00;
}
static void AlxAds114s08_RegStruct_SetToDefault(AlxAds114s08* me)
{
	me->reg.reg_0x00_ID.val.raw =			0x00;
	me->reg.reg_0x01_STATUS.val.raw =		0x80;
	me->reg.reg_0x02_INPMUX.val.raw =		0x01;
	me->reg.reg_0x03_PGA.val.raw =			0x00;
	me->reg.reg_0x04_DATARATE.val.raw =		0x14;
	me->reg.reg_0x05_REF.val.raw =			0x10;
	me->reg.reg_0x06_IDACMAG.val.raw =		0x00;
	me->reg.reg_0x07_IDACMUX.val.raw =		0xFF;
	me->reg.reg_0x08_VBIAS.val.raw =		0x00;
	me->reg.reg_0x09_SYS.val.raw =			0x10;
	me->reg.reg_0x0A_RESERVED.val.raw =		0x00;
	me->reg.reg_0x0B_OFCAL0.val.raw =		0x00;
	me->reg.reg_0x0C_OFCAL1.val.raw =		0x00;
	me->reg.reg_0x0D_RESERVED.val.raw =		0x00;
	me->reg.reg_0x0E_FSCAL0.val.raw =		0x00;
	me->reg.reg_0x0F_FSCAL1.val.raw =		0x40;
	me->reg.reg_0x10_GPIODAT.val.raw =		0x00;
	me->reg.reg_0x11_GPIOCON.val.raw =		0x00;
}

static Alx_Status AlxAds114s08_Cmd_RREG(AlxAds114s08* me, uint8_t regAddr, uint8_t len, uint8_t* data)
{
	// Assert
	ALX_ADS114S08_ASSERT(me->wasCtorCalled == true);
	ALX_ADS114S08_ASSERT(me->isInitPeriph == true);
	ALX_ADS114S08_ASSERT(regAddr <= 0x1F);
	ALX_ADS114S08_ASSERT(len >  0x00);
	ALX_ADS114S08_ASSERT(len <= 0x1F);
	ALX_ADS114S08_ASSERT(len <= 0x02);	// GK: not implemeted for more

	// Local variables
	Alx_Status status = Alx_Err;

	// Assert CS
	AlxSpi_Master_AssertCs(me->spi);

	// Prepare
	uint8_t dataWriteArr[3] = { };
	dataWriteArr[0] = 0x20 | regAddr; // Register address and read/write info - LSB must be set to 0 for read operation
	dataWriteArr[1] = len - 1;
	dataWriteArr[2] = 0;

	uint8_t dataReadArr[3] = { };

	// Read
	status = AlxSpi_Master_WriteRead(me->spi, dataWriteArr, dataReadArr, len + 2, me->spiNumOfTries, me->spiTimeout_ms);
	if (status != Alx_Ok) { AlxSpi_Master_DeAssertCs(me->spi); ALX_ADS114S08_TRACE("Err"); return status; }

	*data = dataReadArr[2];

	// DeAssert CS
	AlxSpi_Master_DeAssertCs(me->spi);
	return Alx_Ok;
}

static Alx_Status AlxAds114s08_Reg_Read(AlxAds114s08* me, void* reg)
{
	// Assert
	ALX_ADS114S08_ASSERT(me->wasCtorCalled == true);
	ALX_ADS114S08_ASSERT(me->isInitPeriph == true);

	// Local variables
	Alx_Status status = Alx_Err;
	uint8_t regAddr = *((uint8_t*)reg);
	uint8_t regLen = *((uint8_t*)reg + sizeof(regAddr));
	uint8_t* regValPtr = (uint8_t*)reg + sizeof(regAddr) + sizeof(regLen);

	// Read register with RREG command
	status = AlxAds114s08_Cmd_RREG(me, regAddr, regLen, regValPtr);
	if (status != Alx_Ok) { ALX_ADS114S08_TRACE("Err"); return status; }

	// Return
	return Alx_Ok;
}



static Alx_Status AlxAds114s08_Reg_Write(AlxAds114s08* me, void* reg)
{
	// Assert
	ALX_ADS114S08_ASSERT(me->wasCtorCalled == true);
	ALX_ADS114S08_ASSERT(me->isInitPeriph == true);

	// Local variables
	Alx_Status status = Alx_Err;
	uint8_t regAddr = *((uint8_t*)reg);
	uint8_t regLen = *((uint8_t*)reg + sizeof(regAddr));
	uint8_t* regValPtr = (uint8_t*)reg + sizeof(regAddr) + sizeof(regLen);
	uint8_t dataWriteArr[3] = { };
	uint8_t dataReadArr[3] = { };

	// Assert CS
	AlxSpi_Master_AssertCs(me->spi);

	// Prepare
	dataWriteArr[0] = 0x40 | regAddr; // WREG command + rgister address
	dataWriteArr[1] = 0x00; // num_of_reg_to_read = 0x00 (we want to write 1 register, so we need to put 0x00)
	dataWriteArr[2] = *regValPtr; // Data to write to register

	// Write
	status = AlxSpi_Master_WriteRead(me->spi, dataWriteArr, dataReadArr, sizeof(dataWriteArr), me->spiNumOfTries, me->spiTimeout_ms);
	if (status != Alx_Ok) { AlxSpi_Master_DeAssertCs(me->spi); ALX_ADS114S08_TRACE("Err"); return status; }

	// DeAssert CS
	AlxSpi_Master_DeAssertCs(me->spi);

	// Return
	return Alx_Ok;
}
static Alx_Status AlxAds114s08_Reg_WriteAndRead(AlxAds114s08* me, void* reg)
{
	// Assert
	ALX_ADS114S08_ASSERT(me->wasCtorCalled == true);
	ALX_ADS114S08_ASSERT(me->isInitPeriph == true);

	Alx_Status status = Alx_Err;

	// Prepare variables
	uint8_t regAddr = *((uint8_t*)reg);
	uint8_t regLen = *((uint8_t*)reg + sizeof(regAddr));
	uint8_t* regValPtr = (uint8_t*)reg + sizeof(regAddr) + sizeof(regLen);

	// Write
	status = AlxAds114s08_Reg_Write(me, reg);
	if (status != Alx_Ok) { ALX_ADS114S08_TRACE("Err"); return status; }

	// Read
	uint8_t regData[3] = { };
	regData[0] = regAddr;
	regData[1] = regLen;
	regData[2] = *regValPtr;
	status = AlxAds114s08_Reg_Read(me, regData);
	if (status != Alx_Ok) { ALX_ADS114S08_TRACE("Err"); return status; }

	// Compare
	if (regData[2] != *regValPtr)
	{
		ALX_ADS114S08_TRACE("Err");
		return Alx_Err;
	}

	// Return
	return Alx_Ok;
}

static Alx_Status AlxAds114s08_Reg_WriteAll(AlxAds114s08* me)
{
	// Write
	// me->reg.reg_0x00_ID // is read only
	if( AlxAds114s08_Reg_WriteAndRead(me, &me->reg.reg_0x01_STATUS	) != Alx_Ok) { ALX_ADS114S08_TRACE("Err"); return Alx_Err; }
	if( AlxAds114s08_Reg_WriteAndRead(me, &me->reg.reg_0x02_INPMUX	) != Alx_Ok) { ALX_ADS114S08_TRACE("Err"); return Alx_Err; }
	if( AlxAds114s08_Reg_WriteAndRead(me, &me->reg.reg_0x03_PGA		) != Alx_Ok) { ALX_ADS114S08_TRACE("Err"); return Alx_Err; }
	if( AlxAds114s08_Reg_WriteAndRead(me, &me->reg.reg_0x04_DATARATE) != Alx_Ok) { ALX_ADS114S08_TRACE("Err"); return Alx_Err; }
	if( AlxAds114s08_Reg_WriteAndRead(me, &me->reg.reg_0x05_REF		) != Alx_Ok) { ALX_ADS114S08_TRACE("Err"); return Alx_Err; }
	if( AlxAds114s08_Reg_WriteAndRead(me, &me->reg.reg_0x06_IDACMAG	) != Alx_Ok) { ALX_ADS114S08_TRACE("Err"); return Alx_Err; }
	if( AlxAds114s08_Reg_WriteAndRead(me, &me->reg.reg_0x07_IDACMUX	) != Alx_Ok) { ALX_ADS114S08_TRACE("Err"); return Alx_Err; }
	if( AlxAds114s08_Reg_WriteAndRead(me, &me->reg.reg_0x08_VBIAS	) != Alx_Ok) { ALX_ADS114S08_TRACE("Err"); return Alx_Err; }
	if( AlxAds114s08_Reg_WriteAndRead(me, &me->reg.reg_0x09_SYS		) != Alx_Ok) { ALX_ADS114S08_TRACE("Err"); return Alx_Err; }
	if( AlxAds114s08_Reg_WriteAndRead(me, &me->reg.reg_0x0A_RESERVED) != Alx_Ok) { ALX_ADS114S08_TRACE("Err"); return Alx_Err; }
	if( AlxAds114s08_Reg_WriteAndRead(me, &me->reg.reg_0x0B_OFCAL0	) != Alx_Ok) { ALX_ADS114S08_TRACE("Err"); return Alx_Err; }
	if( AlxAds114s08_Reg_WriteAndRead(me, &me->reg.reg_0x0C_OFCAL1	) != Alx_Ok) { ALX_ADS114S08_TRACE("Err"); return Alx_Err; }
	if( AlxAds114s08_Reg_WriteAndRead(me, &me->reg.reg_0x0D_RESERVED) != Alx_Ok) { ALX_ADS114S08_TRACE("Err"); return Alx_Err; }
	if( AlxAds114s08_Reg_WriteAndRead(me, &me->reg.reg_0x0E_FSCAL0	) != Alx_Ok) { ALX_ADS114S08_TRACE("Err"); return Alx_Err; }
	if( AlxAds114s08_Reg_WriteAndRead(me, &me->reg.reg_0x0F_FSCAL1	) != Alx_Ok) { ALX_ADS114S08_TRACE("Err"); return Alx_Err; }
	if( AlxAds114s08_Reg_WriteAndRead(me, &me->reg.reg_0x10_GPIODAT	) != Alx_Ok) { ALX_ADS114S08_TRACE("Err"); return Alx_Err; }
	if( AlxAds114s08_Reg_WriteAndRead(me, &me->reg.reg_0x11_GPIOCON	) != Alx_Ok) { ALX_ADS114S08_TRACE("Err"); return Alx_Err; }

	// Return
	return Alx_Ok;
}
Alx_Status AlxAds114s08_GetPgaShorted_uV(AlxAds114s08* me, float* pgaShortedVoltage_uV)
{
	// Assert
	ALX_ADS114S08_ASSERT(me->wasCtorCalled == true);
	ALX_ADS114S08_ASSERT(me->isInitPeriph == true);
	ALX_ADS114S08_ASSERT(me->isInit == true);

	// Local variables
	Alx_Status status = Alx_Err;

	//------------------------------------------------------------------------------
	// Configure registers
	//------------------------------------------------------------------------------
	AlxAds114s08_RegStruct_SetToDefault(me);
	me->reg.reg_0x02_INPMUX.val.MUXN = MUXN_AIN0;
	me->reg.reg_0x02_INPMUX.val.MUXP = MUXP_AIN1;
	me->reg.reg_0x03_PGA.val.GAIN = PGA_GAIN_128;
	me->reg.reg_0x03_PGA.val.PGA_EN = PGA_EN_PgaIsEnabled;
	me->reg.reg_0x04_DATARATE.val.G_CHOP = G_CHOP_Dissabled;
	me->reg.reg_0x04_DATARATE.val.DR = DR_20sps;
	me->reg.reg_0x04_DATARATE.val.MODE = MODE_SingleConversion;
	me->reg.reg_0x05_REF.val.REFSEL = REFSEL_Internal2_5Vref;
	me->reg.reg_0x05_REF.val.REFCON = REFCON_InternalRefAlwaysOn;
	me->reg.reg_0x09_SYS.val.SYS_MON = SYS_MON_Pga_AvddAvss_Div2;
	status = AlxAds114s08_Reg_WriteAll(me);
	if (status != Alx_Ok) { ALX_ADS114S08_TRACE("Err"); return status; }

	//------------------------------------------------------------------------------
	// Perform ADC Conversion, Create Sample in blocking mode
	//------------------------------------------------------------------------------
	int16_t adcVoltage_raw = 0;
	status = AlxAds114s08_PerformAdcConversion(me, &adcVoltage_raw, me->CONVERSION_TIMEOUT_ms);
	if (status != Alx_Ok) { ALX_ADS114S08_TRACE("Err"); return status; }

	//------------------------------------------------------------------------------
	// Convert to mV
	//------------------------------------------------------------------------------
	bool isOutOfRange = true;
	float adcVoltage_mV = -99999999; // set out of range value
	status = AlxAds114s08_Convert_raw_mV(me, adcVoltage_raw, &adcVoltage_mV, &isOutOfRange);
	if (status != Alx_Ok) { ALX_ADS114S08_TRACE("Err"); return status; }

	//------------------------------------------------------------------------------
	// Convert to degC
	//------------------------------------------------------------------------------
	*pgaShortedVoltage_uV = adcVoltage_mV * 2000.f;

	// Return
	return Alx_Ok;
}
Alx_Status AlxAds114s08_GetAvddAvss_mV(AlxAds114s08* me, float* avddAvss_mV)
{
	// Assert
	ALX_ADS114S08_ASSERT(me->wasCtorCalled == true);
	ALX_ADS114S08_ASSERT(me->isInitPeriph == true);
	ALX_ADS114S08_ASSERT(me->isInit == true);

	// Local variables
	Alx_Status status = Alx_Err;

	//------------------------------------------------------------------------------
	// Configure registers
	//------------------------------------------------------------------------------
	AlxAds114s08_RegStruct_SetToDefault(me);
	me->reg.reg_0x02_INPMUX.val.MUXN = MUXN_AIN0;
	me->reg.reg_0x02_INPMUX.val.MUXP = MUXP_AIN1;
	me->reg.reg_0x03_PGA.val.GAIN = PGA_GAIN_1;
	me->reg.reg_0x03_PGA.val.PGA_EN = PGA_EN_PgaIsEnabled;
	me->reg.reg_0x04_DATARATE.val.G_CHOP = G_CHOP_Dissabled;
	me->reg.reg_0x04_DATARATE.val.DR = DR_20sps;
	me->reg.reg_0x04_DATARATE.val.MODE = MODE_SingleConversion;
	me->reg.reg_0x05_REF.val.REFSEL = REFSEL_Internal2_5Vref;
	me->reg.reg_0x05_REF.val.REFCON = REFCON_InternalRefAlwaysOn;
	me->reg.reg_0x09_SYS.val.SYS_MON = SYS_MON_AvddAvss_Div4;
	status = AlxAds114s08_Reg_WriteAll(me);
	if (status != Alx_Ok) { ALX_ADS114S08_TRACE("Err"); return status; }

	//------------------------------------------------------------------------------
	// Perform ADC Conversion, Create Sample in blocking mode
	//------------------------------------------------------------------------------
	int16_t adcVoltage_raw = 0;
	status = AlxAds114s08_PerformAdcConversion(me, &adcVoltage_raw, me->CONVERSION_TIMEOUT_ms);
	if (status != Alx_Ok) { ALX_ADS114S08_TRACE("Err"); return status; }

	//------------------------------------------------------------------------------
	// Convert to mV
	//------------------------------------------------------------------------------
	bool isOutOfRange = true;
	float adcVoltage_mV = -99999999; // set out of range value
	status = AlxAds114s08_Convert_raw_mV(me, adcVoltage_raw, &adcVoltage_mV, &isOutOfRange);
	if (status != Alx_Ok) { ALX_ADS114S08_TRACE("Err"); return status; }

	//------------------------------------------------------------------------------
	// Convert to degC
	//------------------------------------------------------------------------------
	*avddAvss_mV = adcVoltage_mV * 4.0f;

	// Return
	return Alx_Ok;
}
Alx_Status AlxAds114s08_GetDvdd_mV(AlxAds114s08* me, float* dvdd_mV)
{
	// Assert
	ALX_ADS114S08_ASSERT(me->wasCtorCalled == true);
	ALX_ADS114S08_ASSERT(me->isInitPeriph == true);
	ALX_ADS114S08_ASSERT(me->isInit == true);

	// Local variables
	Alx_Status status = Alx_Err;

	//------------------------------------------------------------------------------
	// Configure registers
	//------------------------------------------------------------------------------
	AlxAds114s08_RegStruct_SetToDefault(me);
	me->reg.reg_0x02_INPMUX.val.MUXN = MUXN_AIN0;
	me->reg.reg_0x02_INPMUX.val.MUXP = MUXP_AIN1;
	me->reg.reg_0x03_PGA.val.GAIN = PGA_GAIN_1;
	me->reg.reg_0x03_PGA.val.PGA_EN = PGA_EN_PgaIsEnabled;
	me->reg.reg_0x04_DATARATE.val.G_CHOP = G_CHOP_Dissabled;
	me->reg.reg_0x04_DATARATE.val.DR = DR_20sps;
	me->reg.reg_0x04_DATARATE.val.MODE = MODE_SingleConversion;
	me->reg.reg_0x05_REF.val.REFSEL = REFSEL_Internal2_5Vref;
	me->reg.reg_0x05_REF.val.REFCON = REFCON_InternalRefAlwaysOn;
	me->reg.reg_0x09_SYS.val.SYS_MON = SYS_MON_Dvdd_Div4;
	status = AlxAds114s08_Reg_WriteAll(me);
	if (status != Alx_Ok) { ALX_ADS114S08_TRACE("Err"); return status; }

	//------------------------------------------------------------------------------
	// Perform ADC Conversion, Create Sample in blocking mode
	//------------------------------------------------------------------------------
	int16_t adcVoltage_raw = 0;
	status = AlxAds114s08_PerformAdcConversion(me, &adcVoltage_raw, me->CONVERSION_TIMEOUT_ms);
	if (status != Alx_Ok) { ALX_ADS114S08_TRACE("Err"); return status; }

	//------------------------------------------------------------------------------
	// Convert to mV
	//------------------------------------------------------------------------------
	bool isOutOfRange = true;
	float adcVoltage_mV = -99999999; // set out of range value
	status = AlxAds114s08_Convert_raw_mV(me, adcVoltage_raw, &adcVoltage_mV, &isOutOfRange);
	if (status != Alx_Ok) { ALX_ADS114S08_TRACE("Err"); return status; }

	//------------------------------------------------------------------------------
	// Convert to degC
	//------------------------------------------------------------------------------
	*dvdd_mV = adcVoltage_mV * 4.0f;

	// Return
	return Alx_Ok;
}
Alx_Status AlxAds114s08_ApplyColdJunctionCompensation_degC(AlxAds114s08* me,
	Ads114s08_ThermocoupleType thermocoupleType,
	float thermocoupleVoltage_mV,
	float coldJunctionTemp_degC,
	float* thermocoupleTemperatureCompensated_degC)
{
	// Assert
	ALX_ADS114S08_ASSERT(me->wasCtorCalled == true);
	ALX_ADS114S08_ASSERT(thermocoupleType == thermocoupleType_S); // only supported type at the moment

	float coldJunctionCompensationVoltage_mV;
	AlxInterpLin_GetY_WithStatus(&me->transferFun_thermocoupleS_degC_mV, coldJunctionTemp_degC, &coldJunctionCompensationVoltage_mV);
	float thermocoupleVotlageCompensated_mV = thermocoupleVoltage_mV + coldJunctionCompensationVoltage_mV;
	AlxInterpLin_GetY_WithStatus(&me->transferFun_thermocoupleS_mV_degC, thermocoupleVotlageCompensated_mV, thermocoupleTemperatureCompensated_degC);

	return Alx_Ok;
}

#endif	// #if defined(ALX_C_LIB)
