/**
  ******************************************************************************
  * @file		alxA352.c
  * @brief		Auralix C Library - ALX Accelerometer Epson A352 Module
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
#include "alxA352.h"

#include "alxOsDelay.h"

//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB)


//******************************************************************************
// Private Types
//******************************************************************************
static TIM_HandleTypeDef htim = {};

//******************************************************************************
// Private Functions
//******************************************************************************
static void AlxA352_RegStruct_Init(AlxA352* me);
static Alx_Status AlxA352_Reg_Read(AlxA352* me, void* reg);
static Alx_Status AlxA352_Reg_Write(AlxA352* me, void* reg);
static Alx_Status AlxA352_SetWindow(AlxA352* me, uint16_t window);
static Alx_Status AlxA352_GetProdId(AlxA352* me);
static Alx_Status AlxA352_GetFwVersion(AlxA352* me);
static Alx_Status AlxA352_GetSerialNum(AlxA352* me);
static Alx_Status AlxA352_ReadBurst(AlxA352* me, uint16_t *burstData);
static Alx_Status AlxA352_Configure(AlxA352* me);
static Alx_Status AlxA352_FlashBackup(AlxA352* me);
static Alx_Status AlxA352_SwReset(AlxA352* me);
static void AlxA352_Wait_us(uint32_t waitTime);


//******************************************************************************
// Weak Functions
//******************************************************************************



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
void AlxA352_Ctor
(
	AlxA352* me,
	AlxSpi* spi,
	uint8_t spiNumOfTries,
	uint16_t spiTimeout_ms)
{
	// Parameters
	me->spi = spi;
	me->spiNumOfTries = spiNumOfTries;
	me->spiTimeout_ms = spiTimeout_ms;

	// Variables
	AlxA352_RegStruct_Init(me);
	me->burstSize = 0;

	// Info
	me->wasCtorCalled = true;
	me->isInit = false;
	me->isConfig = false;
	
	// Timer
	htim.Instance = TIM6;
	htim.Init.Prescaler = 120 - 1;
	htim.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim.Init.Period = 0xFFFF;
	htim.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;	// We will wait for update event to change period value
	htim.Init.RepetitionCounter = 0;
	__HAL_RCC_TIM6_RELEASE_RESET();
	__HAL_RCC_TIM6_CLK_ENABLE();
	HAL_TIM_Base_Init(&htim);
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
Alx_Status AlxA352_Init(AlxA352* me)
{
	// Assert
	ALX_A352_ASSERT(me->wasCtorCalled == true);
	ALX_A352_ASSERT(me->isInit == false);

	// Local variables
	Alx_Status status = Alx_Err;

	// Init SPI
	status = AlxSpi_Reconfigure(me->spi,
		AlxSpi_Mode_3,
		AlxSpi_DataSize_16bit,
		AlxSpi_Clk_McuStm32L4_Spi2_Spi3_SpiClk_1MHz875_Pclk1Apb1_120MHz,
		true);
	if (!me->spi->isInit)
	{
		status = AlxSpi_Init(me->spi);
		if (status != Alx_Ok) { ALX_A352_TRACE_WRN("Err SpiInit"); return status; }
	}
	
	status = AlxA352_Disable(me);
	if (status != Alx_Ok) { ALX_A352_TRACE_WRN("Err Disable"); return status; }

	// Read ID registers & Trace ID
	status = AlxA352_GetProdId(me);
	if (status != Alx_Ok) { ALX_A352_TRACE_WRN("Err GetId"); return status; }
	
	status = AlxA352_GetFwVersion(me);
	if (status != Alx_Ok) { ALX_A352_TRACE_WRN("Err GetFwVersion"); return status; }
	
	status = AlxA352_GetSerialNum(me);
	if (status != Alx_Ok) { ALX_A352_TRACE_WRN("Err GetSerialNum"); return status; }
	
	status = AlxA352_Configure(me);
	if (status != Alx_Ok) { ALX_A352_TRACE_WRN("Err Configure"); return status; }

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
Alx_Status AlxA352_DeInit(AlxA352* me)
{
	// Assert
	ALX_A352_ASSERT(me->wasCtorCalled == true);
	ALX_A352_ASSERT(me->isInit == true);

	// Local variables
	Alx_Status status = Alx_Err;

	// Force standby mode

	// DeInit SPI
	status = AlxSpi_DeInit(me->spi);
	if (status != Alx_Ok) { ALX_A352_TRACE_WRN("Err SpiDeinit"); return status; }
	
	// Force window refresh in next init
	me->reg.WIN_CTRL.val.WINDOW_ID = 0xFFFF;

	// Clear isInit
	me->isInit = false;
	me->isConfig = false;

	// Return
	return Alx_Ok;
}

Alx_Status AlxA352_Enable(AlxA352* me)
{
	me->reg.MODE_CTRL.val.MODE_CMD = 1;
	Alx_Status status = AlxA352_Reg_Write(me, &me->reg.MODE_CTRL);
	
	uint32_t i = 0;
	while (1)
	{
		i++;
		status = AlxA352_Reg_Read(me, &me->reg.MODE_CTRL);
		if (me->reg.MODE_CTRL.val.MODE_CMD == 0)
		{
			status = Alx_Ok;
			break;
		}
	}
	
	ALX_A352_TRACE_INF("Sampling mode entered 0x%04X (%u) (%s)", me->reg.MODE_CTRL.val.raw, i, status == Alx_Ok ? "OK" : "ERR");
	
	return status;
}

Alx_Status AlxA352_Disable(AlxA352* me)
{
	// Go to command mode
	me->reg.MODE_CTRL.val.MODE_CMD = 2;
	Alx_Status status = AlxA352_Reg_Write(me, &me->reg.MODE_CTRL);
	
	uint32_t i = 0;
	while (1)
	{
		i++;
		status = AlxA352_Reg_Read(me, &me->reg.MODE_CTRL);
		if (me->reg.MODE_CTRL.val.MODE_CMD == 0)
		{
			status = Alx_Ok;
			break;
		}
	}
	ALX_A352_TRACE_INF("Command mode entered 0x%04X (%u) (%s)", me->reg.MODE_CTRL.val.raw, i, status == Alx_Ok ? "OK" : "ERR");
	
	return status;
}

Alx_Status AlxA352_GetData(AlxA352* me, AccDataPoint* data, uint8_t len)
{	
	uint16_t burst[8] = {0};
	Alx_Status status = AlxA352_ReadBurst(me, burst);
	
	int32_t x_raw = ((uint32_t)(burst[2]) << 16) | (uint32_t)burst[3];
	int32_t y_raw = ((uint32_t)(burst[4]) << 16) | (uint32_t)burst[5];
	int32_t z_raw = ((uint32_t)(burst[6]) << 16) | (uint32_t)burst[7];

	float x = ((float)x_raw / (float)(1 << 24));
	float y = ((float)y_raw / (float)(1 << 24));
	float z = ((float)z_raw / (float)(1 << 24));
		
	float temp = -0.0037918 * (((uint32_t)(burst[0]) << 16) | (uint32_t)burst[1]) + 34.987;

	if (status == Alx_Ok)
	{
		//ALX_A352_TRACE_INF("Get ACCL: %.2f %.2f %.2f %.2f", x, y, z, temp);
		
		data[0].x = x;
		data[0].y = y;
		data[0].z = z;
		data[0].temp = temp;
	}
	else
	{
		//ALX_A352_TRACE_WRN("AlxA352_ReadBurst returned %u", status);
	}

	return status;
}

uint8_t AlxA352_GetFifoLen(AlxA352* me)
{
	return 1;
}

//******************************************************************************
// Private Functions
//******************************************************************************
static void AlxA352_RegStruct_Init(AlxA352* me)
{
	me->reg.BURST.meta.addr = 0x00;
	me->reg.BURST.meta.len = 1;
	me->reg.BURST.meta.win = 0;
	me->reg.BURST.meta.w.high = 0;
	me->reg.BURST.meta.w.low = 1;
	me->reg.BURST.val.raw = 0;
	
	me->reg.MODE_CTRL.meta.addr = 0x02;
	me->reg.MODE_CTRL.meta.len = 1;
	me->reg.MODE_CTRL.meta.win = 0;
	me->reg.MODE_CTRL.meta.w.high = 1;
	me->reg.MODE_CTRL.meta.w.low = 0;
	me->reg.MODE_CTRL.val.raw = 0;
	
	me->reg.DIAG_STAT.meta.addr = 0x04;
	me->reg.DIAG_STAT.meta.len = 1;
	me->reg.DIAG_STAT.meta.win = 0;
	me->reg.DIAG_STAT.meta.w.high = 0;
	me->reg.DIAG_STAT.meta.w.low = 0;
	me->reg.DIAG_STAT.val.raw = 0;
	
	me->reg.FLAG.meta.addr = 0x06;
	me->reg.FLAG.meta.len = 1;
	me->reg.FLAG.meta.win = 0;
	me->reg.FLAG.meta.w.high = 0;
	me->reg.FLAG.meta.w.low = 0;
	me->reg.FLAG.val.raw = 0;
	
	me->reg.COUNT.meta.addr = 0x0A;
	me->reg.COUNT.meta.len = 1;
	me->reg.COUNT.meta.win = 0;
	me->reg.COUNT.meta.w.high = 0;
	me->reg.COUNT.meta.w.low = 0;
	me->reg.COUNT.val.raw = 0;
	
	me->reg.TEMP.meta.addr = 0x0E;
	me->reg.TEMP.meta.len = 2;
	me->reg.TEMP.meta.win = 0;
	me->reg.TEMP.meta.w.high = 0;
	me->reg.TEMP.meta.w.low = 0;
	memset(me->reg.TEMP.val.raw, 0, sizeof(me->reg.TEMP.val.raw));
	
	me->reg.ACCL.meta.addr = 0x30;
	me->reg.ACCL.meta.len = 6;
	me->reg.ACCL.meta.win = 0;
	me->reg.ACCL.meta.w.high = 0;
	me->reg.ACCL.meta.w.low = 0;
	memset(me->reg.ACCL.val.raw, 0, sizeof(me->reg.ACCL.val.raw));
	
	me->reg.SIG_CTRL.meta.addr = 0x00;
	me->reg.SIG_CTRL.meta.len = 1;
	me->reg.SIG_CTRL.meta.win = 1;
	me->reg.SIG_CTRL.meta.w.high = 1;
	me->reg.SIG_CTRL.meta.w.low = 1;
	me->reg.SIG_CTRL.val.raw = 0;
	
	me->reg.MSC_CTRL.meta.addr = 0x02;
	me->reg.MSC_CTRL.meta.len = 1;
	me->reg.MSC_CTRL.meta.win = 1;
	me->reg.MSC_CTRL.meta.w.high = 1;
	me->reg.MSC_CTRL.meta.w.low = 1;
	me->reg.MSC_CTRL.val.raw = 0;
	
	me->reg.SMPL_CTRL.meta.addr = 0x04;
	me->reg.SMPL_CTRL.meta.len = 1;
	me->reg.SMPL_CTRL.meta.win = 1;
	me->reg.SMPL_CTRL.meta.w.high = 1;
	me->reg.SMPL_CTRL.meta.w.low = 0;
	me->reg.SMPL_CTRL.val.raw = 0;
	
	me->reg.FILTER_CTRL.meta.addr = 0x06;
	me->reg.FILTER_CTRL.meta.len = 1;
	me->reg.FILTER_CTRL.meta.win = 1;
	me->reg.FILTER_CTRL.meta.w.high = 0;
	me->reg.FILTER_CTRL.meta.w.low = 1;
	me->reg.FILTER_CTRL.val.raw = 0;
	
	me->reg.GLOB_CMD.meta.addr = 0x0A;
	me->reg.GLOB_CMD.meta.len = 1;
	me->reg.GLOB_CMD.meta.win = 1;
	me->reg.GLOB_CMD.meta.w.high = 0;
	me->reg.GLOB_CMD.meta.w.low = 1;
	me->reg.GLOB_CMD.val.raw = 0;
	
	me->reg.BURST_CTRL.meta.addr = 0x0C;
	me->reg.BURST_CTRL.meta.len = 1;
	me->reg.BURST_CTRL.meta.win = 1;
	me->reg.BURST_CTRL.meta.w.high = 1;
	me->reg.BURST_CTRL.meta.w.low = 1;
	me->reg.BURST_CTRL.val.raw = 0;
	
	me->reg.LONGFILT_CTRL.meta.addr = 0x1C;
	me->reg.LONGFILT_CTRL.meta.len = 1;
	me->reg.LONGFILT_CTRL.meta.win = 1;
	me->reg.LONGFILT_CTRL.meta.w.high = 0;
	me->reg.LONGFILT_CTRL.meta.w.low = 1;
	me->reg.LONGFILT_CTRL.val.raw = 0;
	
	me->reg.LONGFILT_TAP.meta.addr = 0x1E;
	me->reg.LONGFILT_TAP.meta.len = 1;
	me->reg.LONGFILT_TAP.meta.win = 1;
	me->reg.LONGFILT_TAP.meta.w.high = 0;
	me->reg.LONGFILT_TAP.meta.w.low = 1;
	me->reg.LONGFILT_TAP.val.raw = 0;
	
	me->reg.XOFFSET_HIGH.meta.addr = 0x2C;
	me->reg.XOFFSET_HIGH.meta.len = 1;
	me->reg.XOFFSET_HIGH.meta.win = 1;
	me->reg.XOFFSET_HIGH.meta.w.high = 1;
	me->reg.XOFFSET_HIGH.meta.w.low = 1;
	me->reg.XOFFSET_HIGH.val.raw = 0;
	
	me->reg.XOFFSET_LOW.meta.addr = 0x2E;
	me->reg.XOFFSET_LOW.meta.len = 1;
	me->reg.XOFFSET_LOW.meta.win = 1;
	me->reg.XOFFSET_LOW.meta.w.high = 1;
	me->reg.XOFFSET_LOW.meta.w.low = 1;
	me->reg.XOFFSET_LOW.val.raw = 0;
	
	me->reg.YOFFSET_HIGH.meta.addr = 0x30;
	me->reg.YOFFSET_HIGH.meta.len = 1;
	me->reg.YOFFSET_HIGH.meta.win = 1;
	me->reg.YOFFSET_HIGH.meta.w.high = 1;
	me->reg.YOFFSET_HIGH.meta.w.low = 1;
	me->reg.YOFFSET_HIGH.val.raw = 0;
	
	me->reg.YOFFSET_LOW.meta.addr = 0x32;
	me->reg.YOFFSET_LOW.meta.len = 1;
	me->reg.YOFFSET_LOW.meta.win = 1;
	me->reg.YOFFSET_LOW.meta.w.high = 1;
	me->reg.YOFFSET_LOW.meta.w.low = 1;
	me->reg.YOFFSET_LOW.val.raw = 0;
	
	me->reg.ZOFFSET_HIGH.meta.addr = 0x34;
	me->reg.ZOFFSET_HIGH.meta.len = 1;
	me->reg.ZOFFSET_HIGH.meta.win = 1;
	me->reg.ZOFFSET_HIGH.meta.w.high = 1;
	me->reg.ZOFFSET_HIGH.meta.w.low = 1;
	me->reg.ZOFFSET_HIGH.val.raw = 0;
	
	me->reg.ZOFFSET_LOW.meta.addr = 0x36;
	me->reg.ZOFFSET_LOW.meta.len = 1;
	me->reg.ZOFFSET_LOW.meta.win = 1;
	me->reg.ZOFFSET_LOW.meta.w.high = 1;
	me->reg.ZOFFSET_LOW.meta.w.low = 1;
	me->reg.ZOFFSET_LOW.val.raw = 0;
	
	me->reg.PROD_ID.meta.addr = 0x6A;
	me->reg.PROD_ID.meta.len = 4;
	me->reg.PROD_ID.meta.win = 1;
	me->reg.PROD_ID.meta.w.high = 0;
	me->reg.PROD_ID.meta.w.low = 0;
	memset(me->reg.PROD_ID.val.raw, 0, sizeof(me->reg.PROD_ID.val.raw));

	me->reg.VERSION.meta.addr = 0x72;
	me->reg.VERSION.meta.len = 1;
	me->reg.VERSION.meta.win = 1;
	me->reg.VERSION.meta.w.high = 0;
	me->reg.VERSION.meta.w.low = 0;
	me->reg.VERSION.val.raw = 0;

	me->reg.SERIAL_NUM.meta.addr = 0x74;
	me->reg.SERIAL_NUM.meta.len = 4;
	me->reg.SERIAL_NUM.meta.win = 1;
	me->reg.SERIAL_NUM.meta.w.high = 0;
	me->reg.SERIAL_NUM.meta.w.low = 0;
	memset(me->reg.SERIAL_NUM.val.raw, 0, sizeof(me->reg.SERIAL_NUM.val.raw));

	me->reg.WIN_CTRL.meta.addr = 0x7E;
	me->reg.WIN_CTRL.meta.len = 1;
	me->reg.WIN_CTRL.meta.win = 0;
	me->reg.WIN_CTRL.meta.w.high = 0;
	me->reg.WIN_CTRL.meta.w.low = 1;
	me->reg.WIN_CTRL.val.raw = 0xFFFF;
}

static Alx_Status AlxA352_Reg_Read(AlxA352* me, void* reg)
{
	// Local variables
	Alx_Status status = Alx_Err;
	AlxA352_RegMeta regMeta = *(AlxA352_RegMeta*)reg;
	uint16_t *regValPtr = (uint16_t *)((uint8_t *)reg + sizeof(regMeta));

	if (regMeta.addr != me->reg.WIN_CTRL.meta.addr)
	{
		status = AlxA352_SetWindow(me, regMeta.win);
		if (status != Alx_Ok) { ALX_A352_TRACE_WRN("Err SpiReadSetWindow"); return status; }
	}

	// Assert CS
	AlxSpi_Master_AssertCs(me->spi);

	// Write address -> MSB is set to 0 for read operaton
	uint16_t regAddr_Read = 0x0000 | ((regMeta.addr & 0x7F) << 8);
	status = AlxSpi_Master_Write(me->spi, (uint8_t*)&regAddr_Read, 1, me->spiNumOfTries, me->spiTimeout_ms);
	if (status != Alx_Ok) { AlxSpi_Master_DeAssertCs(me->spi); ALX_A352_TRACE_WRN("Err SpiReadSetAddr"); return status; }

	// Stall time (A352 requires 20 us with 40 us readrate)
	AlxA352_Wait_us(30);

	for (uint8_t i = 1; i < regMeta.len; i++)
	{
		regAddr_Read = 0x0000 | (((regMeta.addr + (2 * i)) & 0x7F) << 8);
		status = AlxSpi_Master_WriteRead(me->spi, (uint8_t*)&regAddr_Read, (uint8_t*)&regValPtr[i - 1], 1, me->spiNumOfTries, me->spiTimeout_ms);
		if (status != Alx_Ok) { AlxSpi_Master_DeAssertCs(me->spi); ALX_A352_TRACE_WRN("Err SpiReadGetData1"); return status; }
		
		AlxA352_Wait_us(30);
	}

	// Read final data
	status = AlxSpi_Master_Read(me->spi, (uint8_t*)&regValPtr[regMeta.len - 1], 1, me->spiNumOfTries, me->spiTimeout_ms);
	if (status != Alx_Ok) { AlxSpi_Master_DeAssertCs(me->spi); ALX_A352_TRACE_WRN("Err SpiReadGetData2"); return status; }

	// DeAssert CS
	AlxSpi_Master_DeAssertCs(me->spi);

	// Return
	return Alx_Ok;
}

static Alx_Status AlxA352_Reg_Write(AlxA352* me, void* reg)
{
	// Local variables
	Alx_Status status = Alx_Err;
	AlxA352_RegMeta regMeta = *(AlxA352_RegMeta*)reg;
	uint16_t regVal = *(uint16_t *)((uint8_t *)reg + sizeof(regMeta));

	if (regMeta.addr != me->reg.WIN_CTRL.meta.addr)
	{
		status = AlxA352_SetWindow(me, regMeta.win);
		if (status != Alx_Ok) { ALX_A352_TRACE_WRN("Err SpiWriteSetWindow"); return status; }
	}

	// Assert CS
	AlxSpi_Master_AssertCs(me->spi);

	// Write low address -> MSB is set to 1 for write operaton
	if (regMeta.w.low)
	{
		uint16_t regCmd_Write = 0x8000 | ((regMeta.addr & 0x7F) << 8) | (regVal & 0x00FF);
		status = AlxSpi_Master_Write(me->spi, (uint8_t*)&regCmd_Write, 1, me->spiNumOfTries, me->spiTimeout_ms);
		if (status != Alx_Ok) { AlxSpi_Master_DeAssertCs(me->spi); ALX_A352_TRACE_WRN("Err SpiWrite"); return status; }
		AlxA352_Wait_us(30);
	}
	
	if (regMeta.w.high)
	{
		uint16_t regCmd_Write = 0x8000 | (((regMeta.addr + 1) & 0x7F) << 8) | ((regVal & 0xFF00) >> 8);
		status = AlxSpi_Master_Write(me->spi, (uint8_t*)&regCmd_Write, 1, me->spiNumOfTries, me->spiTimeout_ms);
		if (status != Alx_Ok) { AlxSpi_Master_DeAssertCs(me->spi); ALX_A352_TRACE_WRN("Err SpiWrite"); return status; }
		AlxA352_Wait_us(30);
	}

	// DeAssert CS
	AlxSpi_Master_DeAssertCs(me->spi);

	// Return
	return Alx_Ok;
}

static Alx_Status AlxA352_SetWindow(AlxA352* me, uint16_t window)
{
	Alx_Status status = Alx_Ok;
	uint16_t window_current = me->reg.WIN_CTRL.val.WINDOW_ID;
	if (window_current != window)
	{
		me->reg.WIN_CTRL.val.WINDOW_ID = window;
		status = AlxA352_Reg_Write(me, &me->reg.WIN_CTRL);
		if (status != Alx_Ok)
		{
			me->reg.WIN_CTRL.val.WINDOW_ID = window_current;
		}
	}

	return status;
}

static Alx_Status AlxA352_GetProdId(AlxA352* me)
{
	char prod_id[9] = {0};
	const char *prod_id_expected = "A352AD10";
	Alx_Status status = AlxA352_Reg_Read(me, &me->reg.PROD_ID);
	
	memcpy(prod_id, (uint8_t *)me->reg.PROD_ID.val.raw, sizeof(me->reg.PROD_ID.val.raw));
	if (!strcmp(prod_id, prod_id_expected))
	{
		status = Alx_Ok;
	}
	else
	{
		status = Alx_Err;
	}

	ALX_A352_TRACE_INF("Get PROD_ID: %s (%s)", prod_id, status == Alx_Ok ? "OK" : "ERR");

	return status;
}


static Alx_Status AlxA352_GetFwVersion(AlxA352* me)
{
	Alx_Status status = AlxA352_Reg_Read(me, &me->reg.VERSION);

	ALX_A352_TRACE_INF("Get VERSION: %u (%s)", me->reg.VERSION.val.VERSION, status == Alx_Ok ? "OK" : "ERR");

	return status;
}

static Alx_Status AlxA352_GetSerialNum(AlxA352* me)
{
	char serial[9] = {0};
	Alx_Status status = AlxA352_Reg_Read(me, &me->reg.SERIAL_NUM);
	
	memcpy(serial, (uint8_t *)me->reg.SERIAL_NUM.val.raw, sizeof(me->reg.SERIAL_NUM.val.raw));

	ALX_A352_TRACE_INF("Get SERIAL_NUM: %s (%s)", serial, status == Alx_Ok ? "OK" : "ERR");

	return status;
}

static Alx_Status AlxA352_Configure(AlxA352* me)
{
	Alx_Status status = AlxA352_Reg_Read(me, &me->reg.GLOB_CMD);
	ALX_A352_TRACE_INF("Get GLOB_CMD: 0x%04X (%s)", me->reg.GLOB_CMD.val.raw, status == Alx_Ok ? "OK" : "ERR");
	if (me->reg.GLOB_CMD.val.MESMOD_STAT != 1)
	{
		me->reg.SIG_CTRL.val.raw = 0x8E14;
		status = AlxA352_Reg_Write(me, &me->reg.SIG_CTRL);
		status = AlxA352_FlashBackup(me);
		status = AlxA352_SwReset(me);
	}
	
	status = AlxA352_Reg_Read(me, &me->reg.MODE_CTRL);
	ALX_A352_TRACE_INF("Get MODE_CTRL: 0x%04X (%s)", me->reg.MODE_CTRL.val.raw, status == Alx_Ok ? "OK" : "ERR");
	
	status = AlxA352_Reg_Read(me, &me->reg.DIAG_STAT);
	ALX_A352_TRACE_INF("Get DIAG_STAT: 0x%04X (%s)", me->reg.DIAG_STAT.val.raw, status == Alx_Ok ? "OK" : "ERR");
	
	status = AlxA352_Reg_Read(me, &me->reg.FLAG);
	ALX_A352_TRACE_INF("Get FLAG: 0x%04X (%s)", me->reg.FLAG.val.raw, status == Alx_Ok ? "OK" : "ERR");
	
	status = AlxA352_Reg_Read(me, &me->reg.COUNT);
	ALX_A352_TRACE_INF("Get COUNT: 0x%04X (%s)", me->reg.COUNT.val.raw, status == Alx_Ok ? "OK" : "ERR");
	
	me->reg.SIG_CTRL.val.raw = 0x0E14;  // ND_EN_TEMP disabled
	status = AlxA352_Reg_Write(me, &me->reg.SIG_CTRL);
	status = AlxA352_Reg_Read(me, &me->reg.SIG_CTRL);
	ALX_A352_TRACE_INF("Get SIG_CTRL: 0x%04X (%s)", me->reg.SIG_CTRL.val.raw, status == Alx_Ok ? "OK" : "ERR");
	
	//me->reg.MSC_CTRL.val.raw = 0x0004;  // Set for internal trigger, DRDY active low
	me->reg.MSC_CTRL.val.raw = 0x0044;  // Set for external trigger, rising edge, DRDY active low
	status = AlxA352_Reg_Write(me, &me->reg.MSC_CTRL);
	status = AlxA352_Reg_Read(me, &me->reg.MSC_CTRL);
	ALX_A352_TRACE_INF("Get MSC_CTRL: 0x%04X (%s)", me->reg.MSC_CTRL.val.raw, status == Alx_Ok ? "OK" : "ERR");
	
	me->reg.SMPL_CTRL.val.DOUT_RATE = 2;  // 1000 sps
	status = AlxA352_Reg_Write(me, &me->reg.SMPL_CTRL);
	status = AlxA352_Reg_Read(me, &me->reg.SMPL_CTRL);
	ALX_A352_TRACE_INF("Get SMPL_CTRL: 0x%04X (%s)", me->reg.SMPL_CTRL.val.raw, status == Alx_Ok ? "OK" : "ERR");
	
	me->reg.FILTER_CTRL.val.FILTER_SEL = 10;  // TAP=512, Fc=460
	status = AlxA352_Reg_Write(me, &me->reg.FILTER_CTRL);
	
	uint32_t i = 0;
	while (1)
	{
		i++;
		status = AlxA352_Reg_Read(me, &me->reg.FILTER_CTRL);
		if (me->reg.FILTER_CTRL.val.FILTER_STAT == 0)
		{
			break;
		}
	}
	ALX_A352_TRACE_INF("Get FILTER_CTRL: 0x%04X (%u) (%s)", me->reg.FILTER_CTRL.val.raw, i, status == Alx_Ok ? "OK" : "ERR");
	
	//me->reg.BURST_CTRL.val.raw = 0xC703;  // Everything enabled, burst size 11
	me->reg.BURST_CTRL.val.raw = 0x4700;  // Temp and ACC enabled, burst size 8
	status = AlxA352_Reg_Write(me, &me->reg.BURST_CTRL);
	status = AlxA352_Reg_Read(me, &me->reg.BURST_CTRL);
	me->burstSize = 8;
	ALX_A352_TRACE_INF("Get BURST_CTRL: 0x%04X (%s)", me->reg.BURST_CTRL.val.raw, status == Alx_Ok ? "OK" : "ERR");
	
	me->reg.LONGFILT_CTRL.val.raw = 0x0000;  // Disabled
	status = AlxA352_Reg_Write(me, &me->reg.LONGFILT_CTRL);
	status = AlxA352_Reg_Read(me, &me->reg.LONGFILT_CTRL);
	ALX_A352_TRACE_INF("Get LONGFILT_CTRL: 0x%04X (%s)", me->reg.LONGFILT_CTRL.val.raw, status == Alx_Ok ? "OK" : "ERR");
	
	status = AlxA352_Reg_Read(me, &me->reg.LONGFILT_TAP);
	ALX_A352_TRACE_INF("Get LONGFILT_TAP: 0x%04X (%s)", me->reg.LONGFILT_TAP.val.raw, status == Alx_Ok ? "OK" : "ERR");
	
	me->isConfig = true;
	
	return status;
}

static Alx_Status AlxA352_ReadBurst(AlxA352* me, uint16_t *burstData)
{
	// Local variables
	Alx_Status status = Alx_Err;
	AlxA352_RegMeta regMeta = me->reg.BURST.meta;
	uint16_t regVal = me->reg.BURST.val.raw;

	status = AlxA352_SetWindow(me, regMeta.win);
	if (status != Alx_Ok) { ALX_A352_TRACE_WRN("Err SpiReadSetWindow"); return status; }

	// Assert CS
	AlxSpi_Master_AssertCs(me->spi);

	// Write low address -> MSB is set to 1 for write operaton
	if (regMeta.w.low)
	{
		uint16_t regCmd_Write = 0x8000 | ((regMeta.addr & 0x7F) << 8) | (regVal & 0x00FF);
		status = AlxSpi_Master_Write(me->spi, (uint8_t*)&regCmd_Write, 1, me->spiNumOfTries, me->spiTimeout_ms);
		if (status != Alx_Ok) { AlxSpi_Master_DeAssertCs(me->spi); ALX_A352_TRACE_WRN("Err SpiWrite"); return status; }
	}

	AlxA352_Wait_us(45);

	status = AlxSpi_Master_Read(me->spi, (uint8_t*)burstData, me->burstSize, me->spiNumOfTries, me->spiTimeout_ms);
	if (status != Alx_Ok) { AlxSpi_Master_DeAssertCs(me->spi); ALX_A352_TRACE_WRN("Err SpiReadGetData1"); return status; }

	// DeAssert CS
	AlxSpi_Master_DeAssertCs(me->spi);

	// Return
	return Alx_Ok;
}

static Alx_Status AlxA352_FlashBackup(AlxA352* me)
{
	Alx_Status status = Alx_Ok;
	
	me->reg.GLOB_CMD.val.raw = 0;
	me->reg.GLOB_CMD.val.FLASH_BACKUP = 1;
	status = AlxA352_Reg_Write(me, &me->reg.GLOB_CMD);
	
	uint32_t i = 0;
	while (1)
	{
		i++;
		status = AlxA352_Reg_Read(me, &me->reg.GLOB_CMD);
		if (me->reg.GLOB_CMD.val.FLASH_BACKUP == 0)
		{
			break;
		}
	}
	status = AlxA352_Reg_Read(me, &me->reg.DIAG_STAT);
	
	if ((status == Alx_Ok) && (me->reg.DIAG_STAT.val.FLASH_BU_ERR == 0))
	{
		ALX_A352_TRACE_INF("Flash backup complete (%u)", i);
	}
	else
	{
		ALX_A352_TRACE_INF("Flash backup failed (%u)", i);
		status = Alx_Err;
	}
	
	return status;
}

static Alx_Status AlxA352_SwReset(AlxA352* me)
{
	Alx_Status status = Alx_Ok;
	
	me->reg.GLOB_CMD.val.raw = 0;
	me->reg.GLOB_CMD.val.SOFT_RST = 1;
	status = AlxA352_Reg_Write(me, &me->reg.GLOB_CMD);
	
	AlxOsDelay_ms(&alxOsDelay, 1000);
	
	me->reg.WIN_CTRL.val.WINDOW_ID = 0xFFFF;
	uint32_t i = 0;
	while (1)
	{
		i++;
		status = AlxA352_Reg_Read(me, &me->reg.GLOB_CMD);
		if (me->reg.GLOB_CMD.val.SOFT_RST == 0)
		{
			break;
		}
	}
	
	ALX_A352_TRACE_INF("Software reset complete (%u) (%s)", i, (status == Alx_Ok) ? "OK" : "ERR");
	return status;
}

static void AlxA352_Wait_us(uint32_t waitTime)
{
	__HAL_TIM_SET_COUNTER(&htim, 0);
	HAL_TIM_Base_Start(&htim);
	while (__HAL_TIM_GET_COUNTER(&htim) < waitTime) {}
	HAL_TIM_Base_Stop(&htim);
}
#endif	// #if defined(ALX_C_LIB)
