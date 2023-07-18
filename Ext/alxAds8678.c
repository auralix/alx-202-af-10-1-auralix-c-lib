/**
  ******************************************************************************
  * @file		alxAds8678.c
  * @brief		Auralix C Library - ALX Current Monitor ADS8678 Module
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
#include "alxAds8678.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB)


//******************************************************************************
// Private Types
//******************************************************************************
typedef struct
{
	uint16_t chData_RightJustified;
	uint8_t chNum;
	uint8_t devAddr;
	uint8_t chVoltageRangeLsb3;
} AlxAds8678_ChDataFrame;


//******************************************************************************
// Private Functions
//******************************************************************************
static void AlxAds8678_RegStruct_SetAddr(AlxAds8678* me);
static void AlxAds8678_RegStruct_SetLen(AlxAds8678* me);
static void AlxAds8678_RegStruct_SetValToZero(AlxAds8678* me);
static void AlxAds8678_RegStruct_SetToDefault(AlxAds8678* me);
static Alx_Status AlxAds8678_Reg_Read(AlxAds8678* me, void* reg);
static Alx_Status AlxAds8678_Reg_Write(AlxAds8678* me, void* reg);
static Alx_Status AlxAds8678_Reg_WriteAll(AlxAds8678* me);
static Alx_Status AlxAds8678_Cmd_Write_NoOp(AlxAds8678* me, AlxAds8678_ChDataFrame* chDataFrame);
static Alx_Status AlxAds8678_Cmd_Write_AutoRst(AlxAds8678* me);
static bool AlxAds8678_IsChEnabled(AlxAds8678* me, uint8_t chNum);
static AlxAds8678_RegEnum_0x05_0x0C_Range_CHn AlxAds8678_GetChRegEnumChVoltageRange(AlxAds8678* me, uint8_t chNum);
static void AlxAds8678_RegEnumChVoltageRangeToChVoltageRangeParam(AlxAds8678_RegEnum_0x05_0x0C_Range_CHn regEnumChVoltageRange, float* chVoltagePerBit_V, float* chVoltageOffset_V);


//******************************************************************************
// Weak Functions
//******************************************************************************
void AlxAds8678_RegStruct_SetVal(AlxAds8678* me);


//******************************************************************************
// Constructor
//******************************************************************************
void AlxAds8678_Ctor
(
	AlxAds8678* me,
	AlxSpi* spi,
	uint8_t spiNumOfTries,
	uint16_t spiTimeout_ms,
	AlxIoPin* do_nRST
)
{
	// Parameters
	me->spi = spi;
	me->spiNumOfTries = spiNumOfTries;
	me->spiTimeout_ms = spiTimeout_ms;
	me->do_nRST = do_nRST;

	// Variables
	AlxAds8678_RegStruct_SetAddr(me);
	AlxAds8678_RegStruct_SetLen(me);
	AlxAds8678_RegStruct_SetValToZero(me);

	// Info
	me->wasCtorCalled = true;
	me->isInitPeriph = false;
	me->isInit = false;
}


//******************************************************************************
// Functions
//******************************************************************************
Alx_Status AlxAds8678_InitPeriph(AlxAds8678* me)
{
	// Assert
	ALX_ADS8678_ASSERT(me->wasCtorCalled == true);
	ALX_ADS8678_ASSERT(me->isInitPeriph == false);
	// isInit -> Don't care

	// Local variables
	Alx_Status status = Alx_Err;

	// Init GPIO
	AlxIoPin_Init(me->do_nRST);

	// Init SPI
	status = AlxSpi_Init(me->spi);
	if (status != Alx_Ok) { ALX_ADS8678_TRACE("Err"); return status; }

	// Set isInitPeriph
	me->isInitPeriph = true;

	// Return
	return Alx_Ok;
}
Alx_Status AlxAds8678_DeInitPeriph(AlxAds8678* me)
{
	// Assert
	ALX_ADS8678_ASSERT(me->wasCtorCalled == true);
	ALX_ADS8678_ASSERT(me->isInitPeriph == true);
	// isInit -> Don't care

	// Local variables
	Alx_Status status = Alx_Err;

	// DeInit SPI
	status = AlxSpi_DeInit(me->spi);
	if (status != Alx_Ok) { ALX_ADS8678_TRACE("Err"); return status; }

	// DeInit GPIO
	AlxIoPin_DeInit(me->do_nRST);

	// Clear isInitPeriph
	me->isInitPeriph = false;

	// Return
	return Alx_Ok;
}
Alx_Status AlxAds8678_Init(AlxAds8678* me)
{
	// Assert
	ALX_ADS8678_ASSERT(me->wasCtorCalled == true);
	ALX_ADS8678_ASSERT(me->isInitPeriph == true);
	ALX_ADS8678_ASSERT(me->isInit == false);

	// Local variables
	Alx_Status status = Alx_Err;

	// Set register struct values to default
	AlxAds8678_RegStruct_SetToDefault(me);

	// Set register values - WEAK
	AlxAds8678_RegStruct_SetVal(me);

	// Wake-up device from power down mode
	AlxIoPin_Set(me->do_nRST);

	// Wait for device to wake-up
	AlxDelay_ms(100);

	// Write registers
	status = AlxAds8678_Reg_WriteAll(me);
	if (status != Alx_Ok) { ALX_ADS8678_TRACE("Err"); return status; }

	// Set isInit
	me->isInit = true;

	// Return
	return Alx_Ok;
}
Alx_Status AlxAds8678_DeInit(AlxAds8678* me)
{
	// Assert
	ALX_ADS8678_ASSERT(me->wasCtorCalled == true);
	ALX_ADS8678_ASSERT(me->isInitPeriph == true);
	ALX_ADS8678_ASSERT(me->isInit == true);

	// Local variables
	Alx_Status status = Alx_Err;

	// Set register struct values to default
	AlxAds8678_RegStruct_SetToDefault(me);

	// Write registers
	status = AlxAds8678_Reg_WriteAll(me);
	if (status != Alx_Ok) { ALX_ADS8678_TRACE("Err"); return status; }

	// Place device in power down mode (all registers are reset to default values)
	AlxIoPin_Reset(me->do_nRST);

	// Clear isInit
	me->isInit = false;

	// Return
	return Alx_Ok;
}
Alx_Status AlxAds8678_GetChVoltageAll_V(AlxAds8678* me, AlxAds8678_ChVoltageAll_V* chVoltageAll_V)
{
	// Assert
	ALX_ADS8678_ASSERT(me->wasCtorCalled == true);
	ALX_ADS8678_ASSERT(me->isInitPeriph == true);
	ALX_ADS8678_ASSERT(me->isInit == true);
	ALX_ADS8678_ASSERT(me->reg._0x03_FeatureSelect.val.SDO == SDO_Conv_ChAddr_DevAddr_InRange);	// We want full channel output format for data checking

	// Local variables
	Alx_Status status = Alx_Err;

	// Write AUTO_RST command
	status = AlxAds8678_Cmd_Write_AutoRst(me);
	if (status != Alx_Ok) { ALX_ADS8678_TRACE("Err"); return status; }

	// Loop through all channels
	for (uint32_t chNum = 0; chNum < ALX_ADS8678_NUM_OF_CH; chNum++)
	{
		// If channel enabled, get & set channel voltage, else set channel voltage to zero
		if (AlxAds8678_IsChEnabled(me, chNum))
		{
			// Write NO_OP command
			AlxAds8678_ChDataFrame chDataFrame = {};
			status = AlxAds8678_Cmd_Write_NoOp(me, &chDataFrame);
			if (status != Alx_Ok) { ALX_ADS8678_TRACE("Err"); return status; }

			// Check channel number
			if(chDataFrame.chNum != chNum) { ALX_ADS8678_TRACE("Err"); return Alx_Err; }

			// Check device address
			if(chDataFrame.devAddr != me->reg._0x03_FeatureSelect.val.DEV) { ALX_ADS8678_TRACE("Err"); return Alx_Err; }

			// Check channel voltage range
			AlxAds8678_RegEnum_0x05_0x0C_Range_CHn regEnumChVoltageRange = AlxAds8678_GetChRegEnumChVoltageRange(me, chNum);
			uint8_t chVoltageRangeLsb3 = regEnumChVoltageRange & 0x07;
			if(chDataFrame.chVoltageRangeLsb3 != chVoltageRangeLsb3) { ALX_ADS8678_TRACE("Err"); return Alx_Err; }

			// Prepare
			float chDataFloat = (float)(chDataFrame.chData_RightJustified);
			float chVoltagePerBit_V = 0;
			float chVoltageOffset_V = 0;
			AlxAds8678_RegEnumChVoltageRangeToChVoltageRangeParam(regEnumChVoltageRange, &chVoltagePerBit_V, &chVoltageOffset_V);

			// Set
			chVoltageAll_V->chVoltage_V[chNum] = (chDataFloat * chVoltagePerBit_V) + chVoltageOffset_V;
			chVoltageAll_V->isEnabled[chNum] = true;
		}
		else
		{
			// Set
			chVoltageAll_V->chVoltage_V[chNum] = 0;
			chVoltageAll_V->isEnabled[chNum] = false;
		}
	}

	// Return
	return Alx_Ok;
}


//******************************************************************************
// Private Functions
//******************************************************************************
static void AlxAds8678_RegStruct_SetAddr(AlxAds8678* me)
{
	me->reg._0x01_AutoScanSequenceEnable	.addr = 0x01;
	me->reg._0x02_ChannelPowerDown			.addr = 0x02;
	me->reg._0x03_FeatureSelect				.addr = 0x03;
	me->reg._0x05_Channel0InputRange		.addr = 0x05;
	me->reg._0x06_Channel1InputRange		.addr = 0x06;
	me->reg._0x07_Channel2InputRange		.addr = 0x07;
	me->reg._0x08_Channel3InputRange		.addr = 0x08;
	me->reg._0x09_Channel4InputRange		.addr = 0x09;
	me->reg._0x0A_Channel5InputRange		.addr = 0x0A;
	me->reg._0x0B_Channel6InputRange		.addr = 0x0B;
	me->reg._0x0C_Channel7InputRange		.addr = 0x0C;
}
static void AlxAds8678_RegStruct_SetLen(AlxAds8678* me)
{
	me->reg._0x01_AutoScanSequenceEnable	.len = sizeof(me->reg._0x01_AutoScanSequenceEnable	.val);
	me->reg._0x02_ChannelPowerDown			.len = sizeof(me->reg._0x02_ChannelPowerDown		.val);
	me->reg._0x03_FeatureSelect				.len = sizeof(me->reg._0x03_FeatureSelect			.val);
	me->reg._0x05_Channel0InputRange		.len = sizeof(me->reg._0x05_Channel0InputRange		.val);
	me->reg._0x06_Channel1InputRange		.len = sizeof(me->reg._0x06_Channel1InputRange		.val);
	me->reg._0x07_Channel2InputRange		.len = sizeof(me->reg._0x07_Channel2InputRange		.val);
	me->reg._0x08_Channel3InputRange		.len = sizeof(me->reg._0x08_Channel3InputRange		.val);
	me->reg._0x09_Channel4InputRange		.len = sizeof(me->reg._0x09_Channel4InputRange		.val);
	me->reg._0x0A_Channel5InputRange		.len = sizeof(me->reg._0x0A_Channel5InputRange		.val);
	me->reg._0x0B_Channel6InputRange		.len = sizeof(me->reg._0x0B_Channel6InputRange		.val);
	me->reg._0x0C_Channel7InputRange		.len = sizeof(me->reg._0x0C_Channel7InputRange		.val);
}
static void AlxAds8678_RegStruct_SetValToZero(AlxAds8678* me)
{
	me->reg._0x01_AutoScanSequenceEnable	.val.raw =	0x00;
	me->reg._0x02_ChannelPowerDown			.val.raw =	0x00;
	me->reg._0x03_FeatureSelect				.val.raw =	0x00;
	me->reg._0x05_Channel0InputRange		.val.raw =	0x00;
	me->reg._0x06_Channel1InputRange		.val.raw =	0x00;
	me->reg._0x07_Channel2InputRange		.val.raw =	0x00;
	me->reg._0x08_Channel3InputRange		.val.raw =	0x00;
	me->reg._0x09_Channel4InputRange		.val.raw =	0x00;
	me->reg._0x0A_Channel5InputRange		.val.raw =	0x00;
	me->reg._0x0B_Channel6InputRange		.val.raw =	0x00;
	me->reg._0x0C_Channel7InputRange		.val.raw =	0x00;
}
static void AlxAds8678_RegStruct_SetToDefault(AlxAds8678* me)
{
	me->reg._0x01_AutoScanSequenceEnable	.val.raw =	0xFF;
	me->reg._0x02_ChannelPowerDown			.val.raw =	0x00;
	me->reg._0x03_FeatureSelect				.val.raw =	0x00;
	me->reg._0x05_Channel0InputRange		.val.raw =	0x00;
	me->reg._0x06_Channel1InputRange		.val.raw =	0x00;
	me->reg._0x07_Channel2InputRange		.val.raw =	0x00;
	me->reg._0x08_Channel3InputRange		.val.raw =	0x00;
	me->reg._0x09_Channel4InputRange		.val.raw =	0x00;
	me->reg._0x0A_Channel5InputRange		.val.raw =	0x00;
	me->reg._0x0B_Channel6InputRange		.val.raw =	0x00;
	me->reg._0x0C_Channel7InputRange		.val.raw =	0x00;
}
static Alx_Status AlxAds8678_Reg_Read(AlxAds8678* me, void* reg)
{
	// Local variables
	Alx_Status status = Alx_Err;
	uint8_t regAddr = *((uint8_t*)reg);
	uint8_t regLen = *((uint8_t*)reg + sizeof(regAddr));
	uint8_t* regValPtr = (uint8_t*)reg + sizeof(regAddr) + sizeof(regLen);
	uint8_t dataWriteArr[3] = {};
	uint8_t dataReadArr[3] = {};

	// Assert CS
	AlxSpi_Master_AssertCs(me->spi);

	// Prepare
	dataWriteArr[0] = (regAddr << 1) | 0x00;	// Register address and read/write info - LSB must be set to 0 for read operation
	dataWriteArr[1] = 0x00;						// Dummy byte
	dataWriteArr[2] = 0x00;						// Dummy byte

	// Read
	status = AlxSpi_Master_WriteRead(me->spi, dataWriteArr, dataReadArr, sizeof(dataWriteArr), me->spiNumOfTries, me->spiTimeout_ms);
	if (status != Alx_Ok) { AlxSpi_Master_DeAssertCs(me->spi); ALX_ADS8678_TRACE("Err"); return status; }

	// DeAssert CS
	AlxSpi_Master_DeAssertCs(me->spi);

	// Set
	*regValPtr = dataReadArr[2];

	// Return
	return Alx_Ok;
}
static Alx_Status AlxAds8678_Reg_Write(AlxAds8678* me, void* reg)
{
	// Local variables
	Alx_Status status = Alx_Err;
	uint8_t regAddr = *((uint8_t*)reg);
	uint8_t regLen = *((uint8_t*)reg + sizeof(regAddr));
	uint8_t* regValPtr = (uint8_t*)reg + sizeof(regAddr) + sizeof(regLen);
	uint8_t dataWriteArr[3] = {};
	uint8_t dataReadArr[3] = {};

	// Assert CS
	AlxSpi_Master_AssertCs(me->spi);

	// Prepare
	dataWriteArr[0] = (regAddr << 1) | 0x01;	// Register address and read/write info - LSB must be set to 1 for write operation
	dataWriteArr[1] = *regValPtr;				// Data to write to register
	dataWriteArr[2] = 0x00;						// Dummy byte

	// Write
	status = AlxSpi_Master_WriteRead(me->spi, dataWriteArr, dataReadArr, sizeof(dataWriteArr), me->spiNumOfTries, me->spiTimeout_ms);
	if (status != Alx_Ok) { AlxSpi_Master_DeAssertCs(me->spi); ALX_ADS8678_TRACE("Err"); return status; }

	// DeAssert CS
	AlxSpi_Master_DeAssertCs(me->spi);

	// Check if data was written ok
	if (dataWriteArr[1] != dataReadArr[2]) { ALX_ADS8678_TRACE("Err"); return Alx_Err; }

	// Return
	return Alx_Ok;
}
static Alx_Status AlxAds8678_Reg_WriteAll(AlxAds8678* me)
{
	// Local variables
	Alx_Status status = Alx_Err;

	// Write
	status = AlxAds8678_Reg_Write(me, &me->reg._0x01_AutoScanSequenceEnable);
	if (status != Alx_Ok) { ALX_ADS8678_TRACE("Err"); return status; }

	status = AlxAds8678_Reg_Write(me, &me->reg._0x02_ChannelPowerDown);
	if (status != Alx_Ok) { ALX_ADS8678_TRACE("Err"); return status; }

	status = AlxAds8678_Reg_Write(me, &me->reg._0x03_FeatureSelect);
	if (status != Alx_Ok) { ALX_ADS8678_TRACE("Err"); return status; }

	status = AlxAds8678_Reg_Write(me, &me->reg._0x05_Channel0InputRange);
	if (status != Alx_Ok) { ALX_ADS8678_TRACE("Err"); return status; }

	status = AlxAds8678_Reg_Write(me, &me->reg._0x06_Channel1InputRange);
	if (status != Alx_Ok) { ALX_ADS8678_TRACE("Err"); return status; }

	status = AlxAds8678_Reg_Write(me, &me->reg._0x07_Channel2InputRange);
	if (status != Alx_Ok) { ALX_ADS8678_TRACE("Err"); return status; }

	status = AlxAds8678_Reg_Write(me, &me->reg._0x08_Channel3InputRange);
	if (status != Alx_Ok) { ALX_ADS8678_TRACE("Err"); return status; }

	status = AlxAds8678_Reg_Write(me, &me->reg._0x09_Channel4InputRange);
	if (status != Alx_Ok) { ALX_ADS8678_TRACE("Err"); return status; }

	status = AlxAds8678_Reg_Write(me, &me->reg._0x0A_Channel5InputRange);
	if (status != Alx_Ok) { ALX_ADS8678_TRACE("Err"); return status; }

	status = AlxAds8678_Reg_Write(me, &me->reg._0x0B_Channel6InputRange);
	if (status != Alx_Ok) { ALX_ADS8678_TRACE("Err"); return status; }

	status = AlxAds8678_Reg_Write(me, &me->reg._0x0C_Channel7InputRange);
	if (status != Alx_Ok) { ALX_ADS8678_TRACE("Err"); return status; }

	// Return
	return Alx_Ok;
}
static Alx_Status AlxAds8678_Cmd_Write_NoOp(AlxAds8678* me, AlxAds8678_ChDataFrame* chDataFrame)
{
	// Local variables
	Alx_Status status = Alx_Err;
	uint8_t dataWriteArr[6] = {};
	uint8_t dataReadArr[6] = {};

	// Assert CS
	AlxSpi_Master_AssertCs(me->spi);

	// Prepare NO_OP command
	dataWriteArr[0] = 0x00;
	dataWriteArr[1] = 0x00;
	dataWriteArr[2] = 0x00;
	dataWriteArr[3] = 0x00;
	dataWriteArr[4] = 0x00;
	dataWriteArr[5] = 0x00;

	// Write NO_OP command
	status = AlxSpi_Master_WriteRead(me->spi, dataWriteArr, dataReadArr, sizeof(dataWriteArr), me->spiNumOfTries, me->spiTimeout_ms);
	if (status != Alx_Ok) { AlxSpi_Master_DeAssertCs(me->spi); ALX_ADS8678_TRACE("Err"); return status; }

	// DeAssert CS
	AlxSpi_Master_DeAssertCs(me->spi);

	// Set chData
	uint16_t chData_LeftJustified = (dataReadArr[2] << 8) | dataReadArr[3];
	chDataFrame->chData_RightJustified = chData_LeftJustified >> 2;

	// Set chNum
	chDataFrame->chNum = (dataReadArr[4] & 0xF0) >> 4;

	// Set devAddr
	chDataFrame->devAddr = (dataReadArr[4] & 0x0C) >> 2;

	// Set chVoltageRangeLsb3
	uint8_t chVoltageRangeLsb3_MSB = dataReadArr[4] & 0x03;
	uint8_t chVoltageRangeLsb3_LSB = dataReadArr[5] & 0x80;
	chDataFrame->chVoltageRangeLsb3 = (chVoltageRangeLsb3_MSB << 1) | (chVoltageRangeLsb3_LSB >> 7);

	// Return
	return Alx_Ok;
}
static Alx_Status AlxAds8678_Cmd_Write_AutoRst(AlxAds8678* me)
{
	// Local variables
	Alx_Status status = Alx_Err;
	uint8_t dataWriteArr[4] = {};
	uint8_t dataReadArr[4] = {};

	// Assert CS
	AlxSpi_Master_AssertCs(me->spi);

	// Prepare AUTO_RST command
	dataWriteArr[0] = 0xA0;
	dataWriteArr[1] = 0x00;
	dataWriteArr[2] = 0x00;
	dataWriteArr[3] = 0x00;

	// Write AUTO_RST command
	status = AlxSpi_Master_WriteRead(me->spi, dataWriteArr, dataReadArr, sizeof(dataWriteArr), me->spiNumOfTries, me->spiTimeout_ms);
	if (status != Alx_Ok) { AlxSpi_Master_DeAssertCs(me->spi); ALX_ADS8678_TRACE("Err"); return status; }

	// DeAssert CS
	AlxSpi_Master_DeAssertCs(me->spi);

	// Return
	return Alx_Ok;
}
static bool AlxAds8678_IsChEnabled(AlxAds8678* me, uint8_t chNum)
{
	if (me->reg._0x01_AutoScanSequenceEnable.val.raw & (1 << chNum))
		return true;
	else
		return false;
}
static AlxAds8678_RegEnum_0x05_0x0C_Range_CHn AlxAds8678_GetChRegEnumChVoltageRange(AlxAds8678* me, uint8_t chNum)
{
	if		(chNum == 0)	return me->reg._0x05_Channel0InputRange.val.Range_CHn;
	else if	(chNum == 1)	return me->reg._0x06_Channel1InputRange.val.Range_CHn;
	else if	(chNum == 2)	return me->reg._0x07_Channel2InputRange.val.Range_CHn;
	else if	(chNum == 3)	return me->reg._0x08_Channel3InputRange.val.Range_CHn;
	else if	(chNum == 4)	return me->reg._0x09_Channel4InputRange.val.Range_CHn;
	else if	(chNum == 5)	return me->reg._0x0A_Channel5InputRange.val.Range_CHn;
	else if	(chNum == 6)	return me->reg._0x0B_Channel6InputRange.val.Range_CHn;
	else if	(chNum == 7)	return me->reg._0x0C_Channel7InputRange.val.Range_CHn;

	ALX_ADS8678_ASSERT(false);	// We shouldn't get here
	return ALX_NULL;
}
static void AlxAds8678_RegEnumChVoltageRangeToChVoltageRangeParam(AlxAds8678_RegEnum_0x05_0x0C_Range_CHn regEnumChVoltageRange, float* chVoltagePerBit_V, float* chVoltageOffset_V)
{
	if (regEnumChVoltageRange == Range_CHn_PM_2V5xVref)
	{
		*chVoltagePerBit_V = 1250 * 0.000001;
		*chVoltageOffset_V = -10.24;
	}
	else if (regEnumChVoltageRange == Range_CHn_PM_1V25xVref)
	{
		*chVoltagePerBit_V = 625 * 0.000001;
		*chVoltageOffset_V = -5.12;
	}
	else if (regEnumChVoltageRange == Range_CHn_PM_0V625xVref)
	{
		*chVoltagePerBit_V = 312.5 * 0.000001;
		*chVoltageOffset_V = -2.56;
	}
	else if (regEnumChVoltageRange == Range_CHn_PM_0V3125xVref)
	{
		*chVoltagePerBit_V = 156.25 * 0.000001;
		*chVoltageOffset_V = -1.28;
	}
	else if (regEnumChVoltageRange == Range_CHn_PM_0V15625xVref)
	{
		*chVoltagePerBit_V = 78.125 * 0.000001;
		*chVoltageOffset_V = -0.64;
	}
	else if (regEnumChVoltageRange == Range_CHn_0V_2V5xVref)
	{
		*chVoltagePerBit_V = 625 * 0.000001;
		*chVoltageOffset_V = 0;
	}
	else if (regEnumChVoltageRange == Range_CHn_0V_1V25xVref)
	{
		*chVoltagePerBit_V = 312.5 * 0.000001;
		*chVoltageOffset_V = 0;
	}
	else if (regEnumChVoltageRange == Range_CHn_0V_0V625xVref)
	{
		*chVoltagePerBit_V = 156.25 * 0.000001;
		*chVoltageOffset_V = 0;
	}
	else if (regEnumChVoltageRange == Range_CHn_0V_0V3125xVref)
	{
		*chVoltagePerBit_V = 78.125 * 0.000001;
		*chVoltageOffset_V = 0;
	}
	else
	{
		ALX_ADS8678_ASSERT(false);	// We shouldn't get here
	}
}


//******************************************************************************
// Weak Functions
//******************************************************************************
ALX_WEAK void AlxAds8678_RegStruct_SetVal(AlxAds8678* me)
{
	// Local variables
	(void)me;

	// Assert
	ALX_ADS8678_TRACE("Err");
	ALX_ADS8678_ASSERT(false);
}


#endif	// #if defined(ALX_C_LIB)
