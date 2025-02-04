/**
  ******************************************************************************
  * @file		alxAdxl355.c
  * @brief		Auralix C Library - ALX Accelerometer ADXL355 Module
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
#include "alxAdxl355.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB)


//******************************************************************************
// Private Types
//******************************************************************************
typedef union
{
	struct __attribute__((packed))
	{
		int32_t x_20bit;
		int32_t y_20bit;
		int32_t z_20bit;
	};
	uint8_t raw[12];
} AlxAdxl355_Xyz_20bit;

typedef union
{
	uint16_t val;
	uint8_t raw[2];
} AlxAdxl355_Temp_12bit;


//******************************************************************************
// Private Functions
//******************************************************************************
static void AlxAdxl355_RegStruct_SetAddr(AlxAdxl355* me);
static void AlxAdxl355_RegStruct_SetLen(AlxAdxl355* me);
static void AlxAdxl355_RegStruct_SetValToZero(AlxAdxl355* me);
static void AlxAdxl355_RegStruct_SetValToDefault(AlxAdxl355* me);
static Alx_Status AlxAdxl355_Reg_Read(AlxAdxl355* me, void* reg);
static Alx_Status AlxAdxl355_Reg_Write(AlxAdxl355* me, void* reg);
static Alx_Status AlxAdxl355_Reg_WriteAll(AlxAdxl355* me);
static Alx_Status AlxAdxl355_TraceId(AlxAdxl355* me);
static AlxAdxl355_Xyz_g AlxAdxl355_ConvertXyz(AlxAdxl355* me, AlxAdxl355_Xyz_20bit xyz_20bit);


//******************************************************************************
// Weak Functions
//******************************************************************************
void AlxAdxl355_RegStruct_SetVal(AlxAdxl355* me);


//******************************************************************************
// Constructor
//******************************************************************************

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		spi
  * @param[in]		spiNumOfTries
  * @param[in]		spiTimeout_ms
  */
void AlxAdxl355_Ctor
(
	AlxAdxl355* me,
	AlxSpi* spi,
	uint8_t spiNumOfTries,
	uint16_t spiTimeout_ms
)
{
	// Parameters
	me->spi = spi;
	me->spiNumOfTries = spiNumOfTries;
	me->spiTimeout_ms = spiTimeout_ms;

	// Variables
	AlxAdxl355_RegStruct_SetAddr(me);
	AlxAdxl355_RegStruct_SetLen(me);
	AlxAdxl355_RegStruct_SetValToZero(me);

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
Alx_Status AlxAdxl355_Init(AlxAdxl355* me)
{
	// Assert
	ALX_ADXL355_ASSERT(me->wasCtorCalled == true);
	ALX_ADXL355_ASSERT(me->isInit == false);

	// Local variables
	Alx_Status status = Alx_Err;

	// Init SPI
	status = AlxSpi_Init(me->spi);
	if (status != Alx_Ok) { ALX_ADXL355_TRACE_WRN("Err"); return status; }

	// Set register struct values to default
	AlxAdxl355_RegStruct_SetValToDefault(me);

	// Force standby mode
	me->reg._0x2D_POWER_CTL.val.Standby = Standby_StandbyMode;
	status = AlxAdxl355_Reg_Write(me, &me->reg._0x2D_POWER_CTL);
	if (status != Alx_Ok) { ALX_ADXL355_TRACE_WRN("Err"); return status; }

	// Read ID registers & Trace ID
	status = AlxAdxl355_TraceId(me);
	if (status != Alx_Ok) { ALX_ADXL355_TRACE_WRN("Err"); return status; }

	// Set registers values - WEAK
	AlxAdxl355_RegStruct_SetVal(me);

	// Write all registers
	status = AlxAdxl355_Reg_WriteAll(me);
	if (status != Alx_Ok) { ALX_ADXL355_TRACE_WRN("Err"); return status; }

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
Alx_Status AlxAdxl355_DeInit(AlxAdxl355* me)
{
	// Assert
	ALX_ADXL355_ASSERT(me->wasCtorCalled == true);
	ALX_ADXL355_ASSERT(me->isInit == true);

	// Local variables
	Alx_Status status = Alx_Err;

	// Force standby mode
	me->reg._0x2D_POWER_CTL.val.Standby = Standby_StandbyMode;
	status = AlxAdxl355_Reg_Write(me, &me->reg._0x2D_POWER_CTL);
	if (status != Alx_Ok) { ALX_ADXL355_TRACE_WRN("Err"); return status; }

	// DeInit SPI
	status = AlxSpi_DeInit(me->spi);
	if (status != Alx_Ok) { ALX_ADXL355_TRACE_WRN("Err"); return status; }

	// Clear isInit
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
Alx_Status AlxAdxl355_Enable(AlxAdxl355* me)
{
	// Assert
	ALX_ADXL355_ASSERT(me->wasCtorCalled == true);
	ALX_ADXL355_ASSERT(me->isInit == true);

	// Force measurement mode
	me->reg._0x2D_POWER_CTL.val.Standby = Standby_MeasurementMode;
	Alx_Status status = AlxAdxl355_Reg_Write(me, &me->reg._0x2D_POWER_CTL);
	if (status != Alx_Ok) { ALX_ADXL355_TRACE_WRN("Err"); return status; }

	// Return
	return Alx_Ok;
}

/**
  * @brief
  * @param[in,out]	me
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxAdxl355_Disable(AlxAdxl355* me)
{
	// Assert
	ALX_ADXL355_ASSERT(me->wasCtorCalled == true);
	ALX_ADXL355_ASSERT(me->isInit == true);

	// Force standby mode
	me->reg._0x2D_POWER_CTL.val.Standby = Standby_StandbyMode;
	Alx_Status status = AlxAdxl355_Reg_Write(me, &me->reg._0x2D_POWER_CTL);
	if (status != Alx_Ok) { ALX_ADXL355_TRACE_WRN("Err"); return status; }

	// Return
	return Alx_Ok;
}

/**
  * @brief
  * @param[in,out]	me
  * @param[out]		xyz_g
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxAdxl355_GetXyz_g(AlxAdxl355* me, AlxAdxl355_Xyz_g* xyz_g)
{
	// Assert
	ALX_ADXL355_ASSERT(me->wasCtorCalled == true);
	ALX_ADXL355_ASSERT(me->isInit == true);

	// Read
	Alx_Status status = AlxAdxl355_Reg_Read(me, &me->reg._0x08_0x10_DATA);
	if (status != Alx_Ok) { ALX_ADXL355_TRACE_WRN("Err"); return status; }

	// Prepare
	AlxAdxl355_Xyz_20bit xyz_20bit = {};
	xyz_20bit.raw[0]	= me->reg._0x08_0x10_DATA.val.XDATA1;	// dataX LSB
	xyz_20bit.raw[1]	= me->reg._0x08_0x10_DATA.val.XDATA2;
	xyz_20bit.raw[2]	= me->reg._0x08_0x10_DATA.val.XDATA3;
	xyz_20bit.raw[3]	= 0x00;
	xyz_20bit.raw[4]	= me->reg._0x08_0x10_DATA.val.YDATA1;	// dataY LSB
	xyz_20bit.raw[5]	= me->reg._0x08_0x10_DATA.val.YDATA2;
	xyz_20bit.raw[6]	= me->reg._0x08_0x10_DATA.val.YDATA3;
	xyz_20bit.raw[7]	= 0x00;
	xyz_20bit.raw[8]	= me->reg._0x08_0x10_DATA.val.ZDATA1;	// dataZ LSB
	xyz_20bit.raw[9]	= me->reg._0x08_0x10_DATA.val.ZDATA2;
	xyz_20bit.raw[10]	= me->reg._0x08_0x10_DATA.val.ZDATA3;
	xyz_20bit.raw[11]	= 0x00;

	// Convert
	AlxAdxl355_Xyz_g _xyz_g = AlxAdxl355_ConvertXyz(me, xyz_20bit);

	// Return
	*xyz_g = _xyz_g;
	return Alx_Ok;
}

/**
  * @brief
  * @param[in,out]	me
  * @param[out]		xyz_g
  * @param[in]		len
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxAdxl355_GetFifoXyz_g(AlxAdxl355* me, AlxAdxl355_Xyz_g* xyz_g, uint8_t len)
{
	// Assert
	ALX_ADXL355_ASSERT(me->wasCtorCalled == true);
	ALX_ADXL355_ASSERT(me->isInit == true);
	ALX_ADXL355_ASSERT((1 <= len) && (len <= 32));

	// Read
	me->reg._0x11_FIFO_DATA.len = len * 3 * 3;
	Alx_Status status = AlxAdxl355_Reg_Read(me, &me->reg._0x11_FIFO_DATA);
	if (status != Alx_Ok) { ALX_ADXL355_TRACE_WRN("Err"); return status; }

	// Loop
	AlxAdxl355_Xyz_g _xyz_g[32] = {};
	for (uint32_t i = 0; i < len; i++)
	{
		// Prepare
		AlxAdxl355_Xyz_20bit xyz_20bit = {};
		xyz_20bit.raw[0]	= me->reg._0x11_FIFO_DATA.val.xyz_20bit[i].XDATA1;	// dataX LSB
		xyz_20bit.raw[1]	= me->reg._0x11_FIFO_DATA.val.xyz_20bit[i].XDATA2;
		xyz_20bit.raw[2]	= me->reg._0x11_FIFO_DATA.val.xyz_20bit[i].XDATA3;
		xyz_20bit.raw[3]	= 0x00;
		xyz_20bit.raw[4]	= me->reg._0x11_FIFO_DATA.val.xyz_20bit[i].YDATA1;	// dataY LSB
		xyz_20bit.raw[5]	= me->reg._0x11_FIFO_DATA.val.xyz_20bit[i].YDATA2;
		xyz_20bit.raw[6]	= me->reg._0x11_FIFO_DATA.val.xyz_20bit[i].YDATA3;
		xyz_20bit.raw[7]	= 0x00;
		xyz_20bit.raw[8]	= me->reg._0x11_FIFO_DATA.val.xyz_20bit[i].ZDATA1;	// dataZ LSB
		xyz_20bit.raw[9]	= me->reg._0x11_FIFO_DATA.val.xyz_20bit[i].ZDATA2;
		xyz_20bit.raw[10]	= me->reg._0x11_FIFO_DATA.val.xyz_20bit[i].ZDATA3;
		xyz_20bit.raw[11]	= 0x00;

		// Convert
		_xyz_g[i] = AlxAdxl355_ConvertXyz(me, xyz_20bit);
	}

	// Return
	memcpy(xyz_g, _xyz_g, len * sizeof(AlxAdxl355_Xyz_g));
	return Alx_Ok;
}

/**
  * @brief
  * @param[in,out]	me
  * @param[out]		temp_degC
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxAdxl355_GetTemp_degC(AlxAdxl355* me, float* temp_degC)
{
	// Assert
	ALX_ADXL355_ASSERT(me->wasCtorCalled == true);
	ALX_ADXL355_ASSERT(me->isInit == true);

	// Read
	Alx_Status status = AlxAdxl355_Reg_Read(me, &me->reg._0x06_0x07_TEMP);
	if (status != Alx_Ok) { ALX_ADXL355_TRACE_WRN("Err"); return status; }

	// Convert
	AlxAdxl355_Temp_12bit temp_12bit = {};
	temp_12bit.raw[0] = me->reg._0x06_0x07_TEMP.val.TEMP1;
	temp_12bit.raw[1] = me->reg._0x06_0x07_TEMP.val.TEMP2;

	// Calculate
	float _temp_degC = temp_12bit.val * (-0.1104972376f) + 233.2872929f;	// -9.05 LSB/degC -> -0.1104972376 degC/LSB, 1885 LSB -> 25 degC, 0 LSB -> 233.2872929 degC

	// Return
	*temp_degC = _temp_degC;
	return Alx_Ok;
}

/**
  * @brief
  * @param[in,out]	me
  * @param[out]		statusReg
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxAdxl355_GetStatusReg(AlxAdxl355* me, AlxAdxl355_RegVal_0x04_Status* statusReg)
{
	// Assert
	ALX_ADXL355_ASSERT(me->wasCtorCalled == true);
	ALX_ADXL355_ASSERT(me->isInit == true);

	// Read
	Alx_Status status = AlxAdxl355_Reg_Read(me, &me->reg._0x04_Status);
	if (status != Alx_Ok) { ALX_ADXL355_TRACE_WRN("Err"); return status; }

	// Return
	*statusReg = me->reg._0x04_Status.val;
	return Alx_Ok;
}


//******************************************************************************
// Private Functions
//******************************************************************************
static void AlxAdxl355_RegStruct_SetAddr(AlxAdxl355* me)
{
	me->reg._0x00_DEVID_AD		.addr = 0x00;
	me->reg._0x01_DEVID_MST		.addr = 0x01;
	me->reg._0x02_PARTID		.addr = 0x02;
	me->reg._0x03_REVID			.addr = 0x03;
	me->reg._0x04_Status		.addr = 0x04;
	me->reg._0x28_Filter		.addr = 0x28;
	me->reg._0x29_FIFO_SAMPLES	.addr = 0x29;
	me->reg._0x2A_INT_MAP		.addr = 0x2A;
	me->reg._0x2B_Sync			.addr = 0x2B;
	me->reg._0x2C_Range			.addr = 0x2C;
	me->reg._0x2D_POWER_CTL		.addr = 0x2D;
	me->reg._0x2F_Reset			.addr = 0x2F;

	me->reg._0x06_0x07_TEMP		.addr = 0x06;
	me->reg._0x08_0x10_DATA		.addr = 0x08;
	me->reg._0x11_FIFO_DATA		.addr = 0x11;
}
static void AlxAdxl355_RegStruct_SetLen(AlxAdxl355* me)
{
	me->reg._0x00_DEVID_AD		.len = sizeof(me->reg._0x00_DEVID_AD		.val);
	me->reg._0x01_DEVID_MST		.len = sizeof(me->reg._0x01_DEVID_MST		.val);
	me->reg._0x02_PARTID		.len = sizeof(me->reg._0x02_PARTID			.val);
	me->reg._0x03_REVID			.len = sizeof(me->reg._0x03_REVID			.val);
	me->reg._0x04_Status		.len = sizeof(me->reg._0x04_Status			.val);
	me->reg._0x28_Filter		.len = sizeof(me->reg._0x28_Filter			.val);
	me->reg._0x29_FIFO_SAMPLES	.len = sizeof(me->reg._0x29_FIFO_SAMPLES	.val);
	me->reg._0x2A_INT_MAP		.len = sizeof(me->reg._0x2A_INT_MAP			.val);
	me->reg._0x2B_Sync			.len = sizeof(me->reg._0x2B_Sync			.val);
	me->reg._0x2C_Range			.len = sizeof(me->reg._0x2C_Range			.val);
	me->reg._0x2D_POWER_CTL		.len = sizeof(me->reg._0x2D_POWER_CTL		.val);
	me->reg._0x2F_Reset			.len = sizeof(me->reg._0x2F_Reset			.val);

	me->reg._0x06_0x07_TEMP		.len = sizeof(me->reg._0x06_0x07_TEMP		.val);
	me->reg._0x08_0x10_DATA		.len = sizeof(me->reg._0x08_0x10_DATA		.val);
	me->reg._0x11_FIFO_DATA		.len = sizeof(me->reg._0x11_FIFO_DATA		.val);
}
static void AlxAdxl355_RegStruct_SetValToZero(AlxAdxl355* me)
{
	me->reg._0x00_DEVID_AD		.val.raw = 0x00;
	me->reg._0x01_DEVID_MST		.val.raw = 0x00;
	me->reg._0x02_PARTID		.val.raw = 0x00;
	me->reg._0x03_REVID			.val.raw = 0x00;
	me->reg._0x04_Status		.val.raw = 0x00;
	me->reg._0x28_Filter		.val.raw = 0x00;
	me->reg._0x29_FIFO_SAMPLES	.val.raw = 0x00;
	me->reg._0x2A_INT_MAP		.val.raw = 0x00;
	me->reg._0x2B_Sync			.val.raw = 0x00;
	me->reg._0x2C_Range			.val.raw = 0x00;
	me->reg._0x2D_POWER_CTL		.val.raw = 0x00;
	me->reg._0x2F_Reset			.val.raw = 0x00;

	memset(me->reg._0x06_0x07_TEMP	.val.raw, 0x00, sizeof(me->reg._0x06_0x07_TEMP	.val.raw));
	memset(me->reg._0x08_0x10_DATA	.val.raw, 0x00, sizeof(me->reg._0x08_0x10_DATA	.val.raw));
	memset(me->reg._0x11_FIFO_DATA	.val.raw, 0x00, sizeof(me->reg._0x11_FIFO_DATA	.val.raw));
}
static void AlxAdxl355_RegStruct_SetValToDefault(AlxAdxl355* me)
{
	me->reg._0x00_DEVID_AD		.val.raw	= 0xAD;
	me->reg._0x01_DEVID_MST		.val.raw	= 0x1D;
	me->reg._0x02_PARTID		.val.raw	= 0xED;
	me->reg._0x03_REVID			.val.raw	= 0x01;
	me->reg._0x04_Status		.val.raw	= 0x00;
	me->reg._0x28_Filter		.val.raw	= 0x00;
	me->reg._0x29_FIFO_SAMPLES	.val.raw	= 0x60;
	me->reg._0x2A_INT_MAP		.val.raw	= 0x00;
	me->reg._0x2B_Sync			.val.raw	= 0x00;
	me->reg._0x2C_Range			.val.raw	= 0x81;
	me->reg._0x2D_POWER_CTL		.val.raw	= 0x01;
	me->reg._0x2F_Reset			.val.raw	= 0x00;

	memset(me->reg._0x06_0x07_TEMP	.val.raw, 0x00, sizeof(me->reg._0x06_0x07_TEMP	.val.raw));
	memset(me->reg._0x08_0x10_DATA	.val.raw, 0x00, sizeof(me->reg._0x08_0x10_DATA	.val.raw));
	memset(me->reg._0x11_FIFO_DATA	.val.raw, 0x00, sizeof(me->reg._0x11_FIFO_DATA	.val.raw));
}
static Alx_Status AlxAdxl355_Reg_Read(AlxAdxl355* me, void* reg)
{
	// Local variables
	Alx_Status status = Alx_Err;
	uint8_t regAddr = *((uint8_t*)reg);
	uint16_t regLen = *((uint16_t*)((uint8_t*)reg + sizeof(regAddr)));
	uint8_t* regValPtr = (uint8_t*)reg + sizeof(regAddr) + sizeof(regLen);

	// Assert CS
	AlxSpi_Master_AssertCs(me->spi);

	// Write address -> LSB is set to 1 for read operaton
	uint8_t regAddr_Write = (regAddr << 1) | 0x01;
	status = AlxSpi_Master_Write(me->spi, &regAddr_Write, 1, me->spiNumOfTries, me->spiTimeout_ms);
	if (status != Alx_Ok) { AlxSpi_Master_DeAssertCs(me->spi); ALX_ADXL355_TRACE_WRN("Err"); return status; }

	// Read
	status = AlxSpi_Master_Read(me->spi, regValPtr, regLen, me->spiNumOfTries, me->spiTimeout_ms);
	if (status != Alx_Ok) { AlxSpi_Master_DeAssertCs(me->spi); ALX_ADXL355_TRACE_WRN("Err"); return status; }

	// DeAssert CS
	AlxSpi_Master_DeAssertCs(me->spi);

	// Return
	return Alx_Ok;
}
static Alx_Status AlxAdxl355_Reg_Write(AlxAdxl355* me, void* reg)
{
	// Local variables
	Alx_Status status = Alx_Err;
	uint8_t regAddr = *((uint8_t*)reg);
	uint16_t regLen = *((uint16_t*)((uint8_t*)reg + sizeof(regAddr)));
	uint8_t* regValPtr = (uint8_t*)reg + sizeof(regAddr) + sizeof(regLen);

	// Assert CS
	AlxSpi_Master_AssertCs(me->spi);

	// Write address -> LSB is set to 0 for write operaton
	uint8_t regAddr_Write = regAddr << 1;
	status = AlxSpi_Master_Write(me->spi, &regAddr_Write, 1, me->spiNumOfTries, me->spiTimeout_ms);
	if (status != Alx_Ok) { AlxSpi_Master_DeAssertCs(me->spi); ALX_ADXL355_TRACE_WRN("Err"); return status; }

	// Write
	status = AlxSpi_Master_Write(me->spi, regValPtr, regLen, me->spiNumOfTries, me->spiTimeout_ms);
	if (status != Alx_Ok) { AlxSpi_Master_DeAssertCs(me->spi); ALX_ADXL355_TRACE_WRN("Err"); return status; }

	// DeAssert CS
	AlxSpi_Master_DeAssertCs(me->spi);

	// Return
	return Alx_Ok;
}
static Alx_Status AlxAdxl355_Reg_WriteAll(AlxAdxl355* me)
{
	// Local variables
	Alx_Status status = Alx_Err;

	// Write
	status = AlxAdxl355_Reg_Write(me, &me->reg._0x28_Filter);
	if (status != Alx_Ok) { ALX_ADXL355_TRACE_WRN("Err"); return status; }

	status = AlxAdxl355_Reg_Write(me, &me->reg._0x29_FIFO_SAMPLES);
	if (status != Alx_Ok) { ALX_ADXL355_TRACE_WRN("Err"); return status; }

	status = AlxAdxl355_Reg_Write(me, &me->reg._0x2A_INT_MAP);
	if (status != Alx_Ok) { ALX_ADXL355_TRACE_WRN("Err"); return status; }

	status = AlxAdxl355_Reg_Write(me, &me->reg._0x2B_Sync);
	if (status != Alx_Ok) { ALX_ADXL355_TRACE_WRN("Err"); return status; }

	status = AlxAdxl355_Reg_Write(me, &me->reg._0x2C_Range);
	if (status != Alx_Ok) { ALX_ADXL355_TRACE_WRN("Err"); return status; }

	status = AlxAdxl355_Reg_Write(me, &me->reg._0x2D_POWER_CTL);
	if (status != Alx_Ok) { ALX_ADXL355_TRACE_WRN("Err"); return status; }

	// Return
	return Alx_Ok;
}
static Alx_Status AlxAdxl355_TraceId(AlxAdxl355* me)
{
	// Local variables
	Alx_Status status = Alx_Err;

	// Read
	status = AlxAdxl355_Reg_Read(me, &me->reg._0x00_DEVID_AD);
	if (status != Alx_Ok) { ALX_ADXL355_TRACE_WRN("Err"); return status; }

	status = AlxAdxl355_Reg_Read(me, &me->reg._0x01_DEVID_MST);
	if (status != Alx_Ok) { ALX_ADXL355_TRACE_WRN("Err"); return status; }

	status = AlxAdxl355_Reg_Read(me, &me->reg._0x02_PARTID);
	if (status != Alx_Ok) { ALX_ADXL355_TRACE_WRN("Err"); return status; }

	status = AlxAdxl355_Reg_Read(me, &me->reg._0x03_REVID);
	if (status != Alx_Ok) { ALX_ADXL355_TRACE_WRN("Err"); return status; }

	// Trace
	ALX_ADXL355_TRACE_INF("");
	ALX_ADXL355_TRACE_INF("alxAdxl355.h - Trace");
	ALX_ADXL355_TRACE_INF("- DEVID_AD: 0x%02X", me->reg._0x00_DEVID_AD.val.DEVID_AD);
	ALX_ADXL355_TRACE_INF("- DEVID_MST: 0x%02X", me->reg._0x01_DEVID_MST.val.DEVID_MST);
	ALX_ADXL355_TRACE_INF("- PARTID: 0x%02X", me->reg._0x02_PARTID.val.PARTID);
	ALX_ADXL355_TRACE_INF("- REVID: 0x%02X", me->reg._0x03_REVID.val.REVID);
	ALX_ADXL355_TRACE_INF("");

	// Return
	return Alx_Ok;
}
static AlxAdxl355_Xyz_g AlxAdxl355_ConvertXyz(AlxAdxl355* me, AlxAdxl355_Xyz_20bit xyz_20bit)
{
	// Make data right-justified
	xyz_20bit.x_20bit = xyz_20bit.x_20bit >> 4;
	xyz_20bit.y_20bit = xyz_20bit.y_20bit >> 4;
	xyz_20bit.z_20bit = xyz_20bit.z_20bit >> 4;

	// Add padding 1 for negative numbers (negative numbers have their MSB set to 1)
	if((xyz_20bit.x_20bit & 0x00080000) == 0x00080000)
	{
		xyz_20bit.x_20bit = xyz_20bit.x_20bit | 0xFFF00000;
	}
	if ((xyz_20bit.y_20bit & 0x00080000) == 0x00080000)
	{
		xyz_20bit.y_20bit = xyz_20bit.y_20bit | 0xFFF00000;
	}
	if ((xyz_20bit.z_20bit & 0x00080000) == 0x00080000)
	{
		xyz_20bit.z_20bit = xyz_20bit.z_20bit | 0xFFF00000;
	}

	// Determine full scale range factor
	float rangeFactor = 0;
	switch (me->reg._0x2C_Range.val.Range)
	{
		case Range_2g048:
		{
			rangeFactor = 0.0000039f;
			break;
		}
		case Range_4g096:
		{
			rangeFactor = 0.0000078f;
			break;
		}
		case Range_8g192:
		{
			rangeFactor = 0.0000156f;
			break;
		}
		default:
		{
			ALX_ADXL355_ASSERT(false);	// We should never get here
		}
	}

	// Calculate
	AlxAdxl355_Xyz_g xyz_g = {};
	xyz_g.x_g = xyz_20bit.x_20bit * rangeFactor;
	xyz_g.y_g = xyz_20bit.y_20bit * rangeFactor;
	xyz_g.z_g = xyz_20bit.z_20bit * rangeFactor;

	// Return
	return xyz_g;
}


//******************************************************************************
// Weak Functions
//******************************************************************************
ALX_WEAK void AlxAdxl355_RegStruct_SetVal(AlxAdxl355* me)
{
	(void)me;
	ALX_ADXL355_TRACE_WRN("Err");
	ALX_ADXL355_ASSERT(false);
}


#endif	// #if defined(ALX_C_LIB)
