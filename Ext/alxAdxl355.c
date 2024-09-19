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
// Private Functions
//******************************************************************************
static void AlxAdxl355_RegStruct_SetAddr(AlxAdxl355* me);
static void AlxAdxl355_RegStruct_SetLen(AlxAdxl355* me);
static void AlxAdxl355_RegStruct_SetValToZero(AlxAdxl355* me);
static void AlxAdxl355_RegStruct_SetValToDefault(AlxAdxl355* me);

static Alx_Status AlxAdxl355_Reg_Write(AlxAdxl355* me, void* reg);
static Alx_Status AlxAdxl355_Reg_Read(AlxAdxl355* me, void* reg);
static Alx_Status AlxAdxl355_Reg_WriteAll(AlxAdxl355* me);

static Alx_Status AlxAdxl355_TraceId(AlxAdxl355* me);
static Alx_Status AlxAdxl355_ReadXyz_g(AlxAdxl355* me);
static Alx_Status AlxAdxl355_ReadTemp_degC(AlxAdxl355* me);


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
  * @param[in]		fifoBuff
  * @param[in]		fifoBuffLen
  */
void AlxAdxl355_Ctor
(
	AlxAdxl355* me,
	AlxSpi* spi,
	uint8_t spiNumOfTries,
	uint16_t spiTimeout_ms,
	uint8_t* fifoBuff,
	uint32_t fifoBuffLen
)
{
	// Ctor
	AlxFifo_Ctor(&me->fifo, fifoBuff, fifoBuffLen);

	// Objects - External
	me->spi = spi;

	// Parameters
	me->spiNumOfTries = spiNumOfTries;
	me->spiTimeout_ms = spiTimeout_ms;
	me->fifoBuff = fifoBuff;
	me->fifoBuffLen = fifoBuffLen;

	// Variables
	me->rangeFactor = 0.f;
	me->xyz_g.x_g = 0.f;
	me->xyz_g.y_g = 0.f;
	me->xyz_g.z_g = 0.f;
	me->temp_degC = 0.f;
	AlxAdxl355_RegStruct_SetAddr(me);
	AlxAdxl355_RegStruct_SetLen(me);
	AlxAdxl355_RegStruct_SetValToZero(me);

	// Info
	me->isInit = false;
	me->wasCtorCalled = true;
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
	ALX_ADXL355_ASSERT(me->isInit == false);
	ALX_ADXL355_ASSERT(me->wasCtorCalled == true);

	Alx_Status status = Alx_Err;

	// #1 Flush FIFO
	AlxFifo_Flush(&me->fifo);

	// #2 Init SPI
	status = AlxSpi_Init(me->spi);
	if (status != Alx_Ok) { ALX_ADXL355_TRACE_WRN("Err_AlxSpi_Init"); return status; }

	// #3 Set register struct values to default
	AlxAdxl355_RegStruct_SetValToDefault(me);

	// #4 Force stanby mode
	me->reg._0x2D_POWER_CTL.val.Standby = Standby_StandbyMode;
	status = AlxAdxl355_Reg_Write(me, &me->reg._0x2D_POWER_CTL	);
	if (status != Alx_Ok) { ALX_ADXL355_TRACE_WRN("Err_0x2D_POWER_CTL	"); return status;}

	// #5 Read ID registers & Trace ID
	status = AlxAdxl355_TraceId(me);
	if (status != Alx_Ok) { ALX_ADXL355_TRACE_WRN("Err_TraceId"); return status; }

	// #6 Set registers values - WEAK
	AlxAdxl355_RegStruct_SetVal(me);

	// #7 Write all registers
	status = AlxAdxl355_Reg_WriteAll(me);
	if (status != Alx_Ok) { ALX_ADXL355_TRACE_WRN("Err_Reg_WriteAll"); return status;}

	// #8 Set isInit
	me->isInit = true;

	// #9 Return OK
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
	ALX_ADXL355_ASSERT(me->isInit == true);
	ALX_ADXL355_ASSERT(me->wasCtorCalled == true);

	Alx_Status status = Alx_Err;

	// #1 Force stanby mode
	me->reg._0x2D_POWER_CTL.val.Standby = Standby_StandbyMode;
	status = AlxAdxl355_Reg_Write(me, &me->reg._0x2D_POWER_CTL	);
	if (status != Alx_Ok) { ALX_ADXL355_TRACE_WRN("Err_0x2D_POWER_CTL	"); return status;}

	// #2 DeInit SPI
	status = AlxSpi_DeInit(me->spi);
	if (status != Alx_Ok) { ALX_ADXL355_TRACE_WRN("Err_AlxSpi_DeInit"); return status; }

	// #3 Flush FIFO
	AlxFifo_Flush(&me->fifo);

	// #4 Reset isInit
	me->isInit = false;

	// #5 Return OK
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
	ALX_ADXL355_ASSERT(me->isInit == true);
	ALX_ADXL355_ASSERT(me->wasCtorCalled == true);

	Alx_Status status = Alx_Err;

	// #1 Flush FIFO
	AlxFifo_Flush(&me->fifo);

	// #2 Force measurement mode
	me->reg._0x2D_POWER_CTL.val.Standby = Standby_MeasurementMode;
	status = AlxAdxl355_Reg_Write(me, &me->reg._0x2D_POWER_CTL	);
	if (status != Alx_Ok) { ALX_ADXL355_TRACE_WRN("Err_0x2D_POWER_CTL	"); return status;}

	// #3 Return OK
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
	ALX_ADXL355_ASSERT(me->isInit == true);
	ALX_ADXL355_ASSERT(me->wasCtorCalled == true);

	Alx_Status status = Alx_Err;

	// #1 Flush FIFO
	AlxFifo_Flush(&me->fifo);

	// #2 Force stanby mode
	me->reg._0x2D_POWER_CTL.val.Standby = Standby_StandbyMode;
	status = AlxAdxl355_Reg_Write(me, &me->reg._0x2D_POWER_CTL	);
	if (status != Alx_Ok) { ALX_ADXL355_TRACE_WRN("Err_0x2D_POWER_CTL	"); return status;}

	// #3 Return OK
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
	return AlxAdxl355_GetXyzMulti_g(me, xyz_g, 1);
}

/**
  * @brief
  * @param[in,out]	me
  * @param[out]		xyz_g
  * @param[in]		len
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxAdxl355_GetXyzMulti_g(AlxAdxl355* me, AlxAdxl355_Xyz_g* xyz_g, uint32_t len)
{
	ALX_ADXL355_ASSERT(me->isInit == true);
	ALX_ADXL355_ASSERT(me->wasCtorCalled == true);
	ALX_ADXL355_ASSERT((0 < len) && (len <= me->fifoBuffLen));

	// #1 Prepare variables
	Alx_Status status = Alx_Err;

	// #2 Read acceleration from FIFO
	for (uint32_t i = 0; i < len; i++)
	{
		uint32_t xyzLen = sizeof(AlxAdxl355_Xyz_g);
		uint8_t* ptr = (uint8_t*)&xyz_g[i * xyzLen];

		AlxGlobal_DisableIrq();
		status = AlxFifo_Read(&me->fifo, ptr, xyzLen);
		AlxGlobal_EnableIrq();

		if(status == AlxFifo_ErrEmpty)
		{
			break;	// We break if there are no more messages in ALX RX FIFO
		}
		else if(status != Alx_Ok)
		{
			ALX_ADXL355_ASSERT(false);	// We should never get here
			break;
		}
	}

	// #3 Return status
	return status;
}

/**
  * @brief
  * @param[in,out]	me
  * @return
  */
float AlxAdxl355_GetTemp_degC(AlxAdxl355* me)
{
	ALX_ADXL355_ASSERT(me->isInit == true);
	ALX_ADXL355_ASSERT(me->wasCtorCalled == true);

	return me->temp_degC;
}

/**
  * @brief
  * @param[in,out]	me
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxAdxl355_Foreground_Handle(AlxAdxl355* me)
{
	Alx_Status status = Alx_Err;

	// #1 Read status register, clears interrupt flags
	status = AlxAdxl355_Reg_Read(me, &me->reg._0x04_Status);
	if (status != Alx_Ok) { ALX_ADXL355_TRACE_WRN("Err_ReadStatus"); return status;}

	// #2 Read acceleration data
	status = AlxAdxl355_ReadXyz_g(me);
	if (status != Alx_Ok) { ALX_ADXL355_TRACE_WRN("Err_ReadXyz_g"); return status;}

	// #3 Read temperature data
	status = AlxAdxl355_ReadTemp_degC(me);
	if (status != Alx_Ok) { ALX_ADXL355_TRACE_WRN("Err_ReadTemp_degC"); return status;}

	// #4 Write acceleration data to FIFO
	status = AlxFifo_WriteMulti(&me->fifo, (uint8_t*)&me->xyz_g, sizeof(AlxAdxl355_Xyz_g));
	if (status == AlxFifo_ErrFull)
	{
		// TV: TODO, decide if we will handle FIFO overflow as error, or we will discard overflow data..
		// For now we will discard additional overflow data..
		return status;
	}
	else if (status != Alx_Ok)
	{
		ALX_ADXL355_ASSERT(false);	// We should never get here
		return status;
	}

	// #5 Return OK
	return Alx_Ok;
}


//******************************************************************************
// Private Functions
//******************************************************************************
static void AlxAdxl355_RegStruct_SetAddr(AlxAdxl355* me)
{
	me->reg._0x00_DEVID_AD	.addr = 0x00;
	me->reg._0x01_DEVID_MST	.addr = 0x01;
	me->reg._0x02_PARTID	.addr = 0x02;
	me->reg._0x03_REVID		.addr = 0x03;
	me->reg._0x04_Status	.addr = 0x04;
	me->reg._0x28_Filter	.addr = 0x28;
	me->reg._0x2A_INT_MAP	.addr = 0x2A;
	me->reg._0x2B_Sync		.addr = 0x2B;
	me->reg._0x2C_Range		.addr = 0x2C;
	me->reg._0x2D_POWER_CTL	.addr = 0x2D;
	me->reg._0x2F_Reset		.addr = 0x2F;

	me->reg._0x06_0x07_TEMP	.addr = 0x06;
	me->reg._0x08_0x10_DATA	.addr = 0x08;
}
static void AlxAdxl355_RegStruct_SetLen(AlxAdxl355* me)
{
	me->reg._0x00_DEVID_AD	.len = sizeof(me->reg._0x00_DEVID_AD	.val);
	me->reg._0x01_DEVID_MST	.len = sizeof(me->reg._0x01_DEVID_MST	.val);
	me->reg._0x02_PARTID	.len = sizeof(me->reg._0x02_PARTID		.val);
	me->reg._0x03_REVID		.len = sizeof(me->reg._0x03_REVID		.val);
	me->reg._0x04_Status	.len = sizeof(me->reg._0x04_Status		.val);
	me->reg._0x28_Filter	.len = sizeof(me->reg._0x28_Filter		.val);
	me->reg._0x2A_INT_MAP	.len = sizeof(me->reg._0x2A_INT_MAP		.val);
	me->reg._0x2B_Sync		.len = sizeof(me->reg._0x2B_Sync		.val);
	me->reg._0x2C_Range		.len = sizeof(me->reg._0x2C_Range		.val);
	me->reg._0x2D_POWER_CTL	.len = sizeof(me->reg._0x2D_POWER_CTL	.val);
	me->reg._0x2F_Reset		.len = sizeof(me->reg._0x2F_Reset		.val);

	me->reg._0x06_0x07_TEMP	.len = sizeof(me->reg._0x06_0x07_TEMP	.val);
	me->reg._0x08_0x10_DATA	.len = sizeof(me->reg._0x08_0x10_DATA	.val);
}
static void AlxAdxl355_RegStruct_SetValToZero(AlxAdxl355* me)
{
	me->reg._0x00_DEVID_AD	.val.raw	= 0x00;
	me->reg._0x01_DEVID_MST	.val.raw	= 0x00;
	me->reg._0x02_PARTID	.val.raw	= 0x00;
	me->reg._0x03_REVID		.val.raw	= 0x00;
	me->reg._0x04_Status	.val.raw	= 0x00;
	me->reg._0x28_Filter	.val.raw	= 0x00;
	me->reg._0x2A_INT_MAP	.val.raw	= 0x00;
	me->reg._0x2B_Sync		.val.raw	= 0x00;
	me->reg._0x2C_Range		.val.raw	= 0x00;
	me->reg._0x2D_POWER_CTL	.val.raw	= 0x00;
	me->reg._0x2F_Reset		.val.raw	= 0x00;

	for (uint32_t i = 0; i < sizeof(me->reg._0x06_0x07_TEMP	.val); i++)	{ me->reg._0x06_0x07_TEMP	.val.raw[i] = 0x00; }
	for (uint32_t i = 0; i < sizeof(me->reg._0x08_0x10_DATA	.val); i++)	{ me->reg._0x08_0x10_DATA	.val.raw[i] = 0x00; }
}
static void AlxAdxl355_RegStruct_SetValToDefault(AlxAdxl355* me)
{
	me->reg._0x00_DEVID_AD	.val.raw	= 0xAD;
	me->reg._0x01_DEVID_MST	.val.raw	= 0x1D;
	me->reg._0x02_PARTID	.val.raw	= 0xED;
	me->reg._0x03_REVID		.val.raw	= 0x01;
	me->reg._0x04_Status	.val.raw	= 0x00;
	me->reg._0x28_Filter	.val.raw	= 0x00;
	me->reg._0x2A_INT_MAP	.val.raw	= 0x00;
	me->reg._0x2B_Sync		.val.raw	= 0x00;
	me->reg._0x2C_Range		.val.raw	= 0x81;
	me->reg._0x2D_POWER_CTL	.val.raw	= 0x01;
	me->reg._0x2F_Reset		.val.raw	= 0x00;

	for (uint32_t i = 0; i < sizeof(me->reg._0x06_0x07_TEMP	.val); i++)	{ me->reg._0x06_0x07_TEMP	.val.raw[i] = 0x00; }
	for (uint32_t i = 0; i < sizeof(me->reg._0x08_0x10_DATA	.val); i++)	{ me->reg._0x08_0x10_DATA	.val.raw[i] = 0x00; }
}
static Alx_Status AlxAdxl355_Reg_Write(AlxAdxl355* me, void* reg)
{
	// #1 Prepare variables
	Alx_Status status = Alx_Err;

	uint8_t regAddr = *((uint8_t*)reg);
	uint8_t regLen = *((uint8_t*)reg + sizeof(regAddr));
	uint8_t* regValPtr = (uint8_t*)reg + sizeof(regAddr) + sizeof(regLen);

	// #2 Assert CS
	AlxSpi_Master_AssertCs(me->spi);

	// #3 Write address -> LSB is set to 0 for write operaton
	uint8_t regAddr_Write = regAddr << 1;
	status = AlxSpi_Master_Write(me->spi, &regAddr_Write , 1, me->spiNumOfTries, me->spiTimeout_ms);
	if (status != Alx_Ok) { AlxSpi_Master_DeAssertCs(me->spi); ALX_ADXL355_TRACE_WRN("Err_WriteAddress"); return status;}

	// #4 Write data
	status = AlxSpi_Master_Write(me->spi, regValPtr, regLen, me->spiNumOfTries, me->spiTimeout_ms);
	if (status != Alx_Ok) { AlxSpi_Master_DeAssertCs(me->spi); ALX_ADXL355_TRACE_WRN("Err_WriteData"); return status;}

	// #5 DeAssert CS
	AlxSpi_Master_DeAssertCs(me->spi);

	// #6 Return OK
	return Alx_Ok;
}
static Alx_Status AlxAdxl355_Reg_Read(AlxAdxl355* me, void* reg)
{
	// #1 Prepare variables
	Alx_Status status = Alx_Err;

	uint8_t regAddr = *((uint8_t*)reg);
	uint8_t regLen = *((uint8_t*)reg + sizeof(regAddr));
	uint8_t* regValPtr = (uint8_t*)reg + sizeof(regAddr) + sizeof(regLen);

	// #2 Assert CS
	AlxSpi_Master_AssertCs(me->spi);

	// #3 Write address -> LSB is set to 1 for read operaton
	uint8_t regAddr_Write = (regAddr << 1) | 0x01;
	status = AlxSpi_Master_Write(me->spi, &regAddr_Write , 1, me->spiNumOfTries, me->spiTimeout_ms);
	if (status != Alx_Ok) { AlxSpi_Master_DeAssertCs(me->spi); ALX_ADXL355_TRACE_WRN("Err_WriteAddress"); return status;}

	// #4 Read data
	status = AlxSpi_Master_Read(me->spi, regValPtr, regLen, me->spiNumOfTries, me->spiTimeout_ms);
	if (status != Alx_Ok) { AlxSpi_Master_DeAssertCs(me->spi); ALX_ADXL355_TRACE_WRN("Err_ReadData"); return status;}

	// #5 DeAssert CS
	AlxSpi_Master_DeAssertCs(me->spi);

	// #6 Return OK
	return Alx_Ok;
}
static Alx_Status AlxAdxl355_Reg_WriteAll(AlxAdxl355* me)
{
	Alx_Status status = Alx_Err;

	status = AlxAdxl355_Reg_Write(me, &me->reg._0x28_Filter		);
	if (status != Alx_Ok) { ALX_ADXL355_TRACE_WRN("Err_0x28_Filter		"); return status;}

	status = AlxAdxl355_Reg_Write(me, &me->reg._0x2A_INT_MAP	);
	if (status != Alx_Ok) { ALX_ADXL355_TRACE_WRN("Err_0x2A_INT_MAP		"); return status;}

	status = AlxAdxl355_Reg_Write(me, &me->reg._0x2B_Sync		);
	if (status != Alx_Ok) { ALX_ADXL355_TRACE_WRN("Err_0x2B_Sync		"); return status;}

	status = AlxAdxl355_Reg_Write(me, &me->reg._0x2C_Range		);
	if (status != Alx_Ok) { ALX_ADXL355_TRACE_WRN("Err_0x2C_Range		"); return status;}

	status = AlxAdxl355_Reg_Write(me, &me->reg._0x2D_POWER_CTL	);
	if (status != Alx_Ok) { ALX_ADXL355_TRACE_WRN("Err_0x2D_POWER_CTL	"); return status;}

	return Alx_Ok;
}
static Alx_Status AlxAdxl355_TraceId(AlxAdxl355* me)
{
	Alx_Status status = Alx_Err;

	// #1 Read ID registers
	status = AlxAdxl355_Reg_Read(me, &me->reg._0x00_DEVID_AD	);
	if (status != Alx_Ok) { ALX_ADXL355_TRACE_WRN("Err_0x00_DEVID_AD	"); return status;}

	status = AlxAdxl355_Reg_Read(me, &me->reg._0x01_DEVID_MST	);
	if (status != Alx_Ok) { ALX_ADXL355_TRACE_WRN("Err_0x01_DEVID_MST	"); return status;}

	status = AlxAdxl355_Reg_Read(me, &me->reg._0x02_PARTID		);
	if (status != Alx_Ok) { ALX_ADXL355_TRACE_WRN("Err_0x02_PARTID		"); return status;}

	status = AlxAdxl355_Reg_Read(me, &me->reg._0x03_REVID		);
	if (status != Alx_Ok) { ALX_ADXL355_TRACE_WRN("Err_0x03_REVID		"); return status;}

	// #2 Trace
	ALX_ADXL355_TRACE_INF("");
	ALX_ADXL355_TRACE_INF("AlxAdxl355 - Trace");
	ALX_ADXL355_TRACE_INF("- DEVID_AD: 0x%02X", me->reg._0x00_DEVID_AD.val.DEVID_AD);
	ALX_ADXL355_TRACE_INF("- DEVID_MST: 0x%02X", me->reg._0x01_DEVID_MST.val.DEVID_MST);
	ALX_ADXL355_TRACE_INF("- PARTID: 0x%02X", me->reg._0x02_PARTID.val.PARTID);
	ALX_ADXL355_TRACE_INF("- REVID: 0x%02X", me->reg._0x03_REVID.val.REVID);
	ALX_ADXL355_TRACE_INF("");

	// #3 Return OK
	return Alx_Ok;
}
static Alx_Status AlxAdxl355_ReadXyz_g(AlxAdxl355* me)
{
	Alx_Status status = Alx_Err;

	// #1 Read acceleration data
	status = AlxAdxl355_Reg_Read(me, &me->reg._0x08_0x10_DATA);
	if (status != Alx_Ok) { ALX_ADXL355_TRACE_WRN("Err_0x08_0x10_DATA"); return status;}

	// #2 Re-organize raw data
	me->xyz_20bit.raw[0]	= me->reg._0x08_0x10_DATA.val.XDATA1;	// dataX LSB
	me->xyz_20bit.raw[1]	= me->reg._0x08_0x10_DATA.val.XDATA2;
	me->xyz_20bit.raw[2]	= me->reg._0x08_0x10_DATA.val.XDATA3;
	me->xyz_20bit.raw[3]	= 0x00;
	me->xyz_20bit.raw[4]	= me->reg._0x08_0x10_DATA.val.YDATA1;	// dataY LSB
	me->xyz_20bit.raw[5]	= me->reg._0x08_0x10_DATA.val.YDATA2;
	me->xyz_20bit.raw[6]	= me->reg._0x08_0x10_DATA.val.YDATA3;
	me->xyz_20bit.raw[7]	= 0x00;
	me->xyz_20bit.raw[8]	= me->reg._0x08_0x10_DATA.val.ZDATA1;	// dataZ LSB
	me->xyz_20bit.raw[9]	= me->reg._0x08_0x10_DATA.val.ZDATA2;
	me->xyz_20bit.raw[10]	= me->reg._0x08_0x10_DATA.val.ZDATA3;
	me->xyz_20bit.raw[11]	= 0x00;

	// #3 Make data right-justified
	me->xyz_20bit.x_20bit = me->xyz_20bit.x_20bit >> 4;
	me->xyz_20bit.y_20bit = me->xyz_20bit.y_20bit >> 4;
	me->xyz_20bit.z_20bit = me->xyz_20bit.z_20bit >> 4;

	// #4 Add padding 1 for negative numbers (negative numbers have their MSB set to 1)
	if((me->xyz_20bit.x_20bit & 0x00080000) == 0x00080000)
	{
		me->xyz_20bit.x_20bit = me->xyz_20bit.x_20bit | 0xFFF00000;
	}
	if ((me->xyz_20bit.y_20bit & 0x00080000) == 0x00080000)
	{
		me->xyz_20bit.y_20bit = me->xyz_20bit.y_20bit | 0xFFF00000;
	}
	if ((me->xyz_20bit.z_20bit & 0x00080000) == 0x00080000)
	{
		me->xyz_20bit.z_20bit = me->xyz_20bit.z_20bit | 0xFFF00000;
	}

	// #5 Determine full scale range factor
	switch (me->reg._0x2C_Range.val.Range)
	{
		case Range_2g048:
		{
			me->rangeFactor = 0.0000039f;
			break;
		}
		case Range_4g096:
		{
			me->rangeFactor = 0.0000078f;
			break;
		}
		case Range_8g192:
		{
			me->rangeFactor = 0.0000156f;
			break;
		}
		default:
		{
			ALX_ADXL355_ASSERT(false);	// We should never get here
			return status;
		}
	}

	// #6 Calculate & set
	me->xyz_g.x_g = me->xyz_20bit.x_20bit * me->rangeFactor;
	me->xyz_g.y_g = me->xyz_20bit.y_20bit * me->rangeFactor;
	me->xyz_g.z_g = me->xyz_20bit.z_20bit * me->rangeFactor;

	// #7 Return OK
	return Alx_Ok;
}
static Alx_Status AlxAdxl355_ReadTemp_degC(AlxAdxl355* me)
{
	Alx_Status status = Alx_Err;

	// #1 Read temperature data
	status = AlxAdxl355_Reg_Read(me, &me->reg._0x06_0x07_TEMP);
	if (status != Alx_Ok) { ALX_ADXL355_TRACE_WRN("Err_0x06_0x07_TEMP"); return status;}

	// #2 Merge raw data to 16bit variable
	me->temp_12bit.raw[0] = me->reg._0x06_0x07_TEMP.val.TEMP1;
	me->temp_12bit.raw[1] = me->reg._0x06_0x07_TEMP.val.TEMP2;

	// #3 Calculate
	me->temp_degC = me->temp_12bit.val * (-0.1104972376f) + 233.2872929f;	// -9.05 LSB/°C -> -0.1104972376 °C/LSB
																			// 1885 LSB -> 25 °C
																			// 0 LSB -> 233.2872929 °C

	// #4 Return OK
	return Alx_Ok;
}


//******************************************************************************
// Weak Functions
//******************************************************************************
ALX_WEAK void AlxAdxl355_RegStruct_SetVal(AlxAdxl355* me)
{
	(void)me;
	ALX_ADXL355_TRACE_WRN("Define 'AlxAdxl355_RegStruct_SetVal' function in your application.");
	ALX_ADXL355_ASSERT(false);
}


#endif	// #if defined(ALX_C_LIB)
