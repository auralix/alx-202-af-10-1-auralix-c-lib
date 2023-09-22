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
// Private Functions
//******************************************************************************
static void AlxAds114s08_RegStruct_SetAddr(AlxAds114s08* me);
static void AlxAds114s08_RegStruct_SetLen(AlxAds114s08* me);
static void AlxAds114s08_RegStruct_SetValToZero(AlxAds114s08* me);
static void AlxAds114s08_RegStruct_SetToDefault(AlxAds114s08* me);
static Alx_Status AlxAds114s08_Reg_Read(AlxAds114s08* me, void* reg);
static Alx_Status AlxAds114s08_Reg_Write(AlxAds114s08* me, void* reg);
static Alx_Status AlxAds114s08_Reg_WriteAll(AlxAds114s08* me);

//******************************************************************************
// Weak Functions
//******************************************************************************
void AlxAds114s08_RegStruct_SetVal(AlxAds114s08* me);


//******************************************************************************
// Constructor
//******************************************************************************
void Ads114s08_Ctor
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

	// Variables
	AlxAds114s08_RegStruct_SetAddr(me);
	AlxAds114s08_RegStruct_SetLen(me);
	AlxAds114s08_RegStruct_SetValToZero(me);

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

	// Set register values - WEAK
	AlxAds114s08_RegStruct_SetVal(me);

	// Wake-up device from power down mode
	AlxIoPin_Set(me->do_START);
	AlxIoPin_Set(me->do_nRST);

	// Wait for device to wake-up
	AlxDelay_ms(100);

	// Write registers
	status = AlxAds114s08_Reg_WriteAll(me);
	if (status != Alx_Ok) { ALX_ADS114S08_TRACE("Err"); return status; }

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
Alx_Status Ads114s08_GetChVoltage_mV(AlxAds114s08* me, float chVoltage_mV[])
{
	// Assert
	ALX_ADS114S08_ASSERT(me->wasCtorCalled == true);
	ALX_ADS114S08_ASSERT(me->isInitPeriph == true);
	ALX_ADS114S08_ASSERT(me->isInit == true);

	// Local variables
	Alx_Status status = Alx_Err;
//
//	// Write AUTO_RST command
//	status = AlxAds114s08_Cmd_Write_AutoRst(me);
//	if (status != Alx_Ok) { ALX_ADS114S08_TRACE("Err"); return status; }
//
//	// Loop through all channels
//	for (uint32_t chNum = 0; chNum < ALX_ADS114S08_NUM_OF_CH; chNum++)
//	{
//		// If channel enabled, get & set channel voltage, else set channel voltage to zero
//		if (AlxAds114s08_IsChEnabled(me, chNum))
//		{
//			// Local variables
//			AlxAds114s08_ChDataFrame chDataFrame = {};
//
//			// Write NO_OP command
//			uint8_t dataReadArr[6] = {};
//			status = AlxAds114s08_Cmd_Write_NoOp(me, dataReadArr);
//			if (status != Alx_Ok) { ALX_ADS114S08_TRACE("Err"); return status; }
//
//			// Check channel number
//			if
//			(
//				(me->reg._0x03_FeatureSelect.val.SDO == SDO_Conv_ChAddr) ||
//				(me->reg._0x03_FeatureSelect.val.SDO == SDO_Conv_ChAddr_DevAddr) ||
//				(me->reg._0x03_FeatureSelect.val.SDO == SDO_Conv_ChAddr_DevAddr_InRange)
//			)
//			{
//				// Parse
//				chDataFrame.chNum = (dataReadArr[4] & 0xF0) >> 4;
//
//				// Check
//				if(chDataFrame.chNum != chNum) { ALX_ADS114S08_TRACE("Err"); return Alx_Err; }
//			}
//
//			// Check device address
//			if
//			(
//				(me->reg._0x03_FeatureSelect.val.SDO == SDO_Conv_ChAddr_DevAddr) ||
//				(me->reg._0x03_FeatureSelect.val.SDO == SDO_Conv_ChAddr_DevAddr_InRange)
//			)
//			{
//				// Parse
//				chDataFrame.devAddr = (dataReadArr[4] & 0x0C) >> 2;
//
//				// Check
//				if(chDataFrame.devAddr != me->reg._0x03_FeatureSelect.val.DEV) { ALX_ADS114S08_TRACE("Err"); return Alx_Err; }
//			}
//
//			// Check channel voltage range
//			AlxAds114s08_RegEnum_0x05_0x0C_Range_CHn regEnumChVoltageRange = 0;
//			if (me->reg._0x03_FeatureSelect.val.SDO == SDO_Conv_ChAddr_DevAddr_InRange)
//			{
//				// Parse
//				uint8_t chVoltageRangeLsb3_MSB = dataReadArr[4] & 0x03;
//				uint8_t chVoltageRangeLsb3_LSB = dataReadArr[5] & 0x80;
//				chDataFrame.chVoltageRangeLsb3 = (chVoltageRangeLsb3_MSB << 1) | (chVoltageRangeLsb3_LSB >> 7);
//
//				// Check
//				regEnumChVoltageRange = AlxAds114s08_GetChRegEnumChVoltageRange(me, chNum);
//				uint8_t chVoltageRangeLsb3 = regEnumChVoltageRange & 0x07;
//				if(chDataFrame.chVoltageRangeLsb3 != chVoltageRangeLsb3) { ALX_ADS114S08_TRACE("Err"); return Alx_Err; }
//			}
//
//			// Prepare
//			uint16_t chData_LeftJustified = (dataReadArr[2] << 8) | dataReadArr[3];
//			chDataFrame.chData_RightJustified = chData_LeftJustified >> 2;
//			float chDataFloat = (float)(chDataFrame.chData_RightJustified);
//			float chVoltagePerBit_V = 0;
//			float chVoltageOffset_V = 0;
//			AlxAds114s08_RegEnumChVoltageRangeToChVoltageRangeParam(regEnumChVoltageRange, &chVoltagePerBit_V, &chVoltageOffset_V);
//
//			// Set
//			chVoltageAll_V->chVoltage_V[chNum] = (chDataFloat * chVoltagePerBit_V) + chVoltageOffset_V;
//			chVoltageAll_V->isEnabled[chNum] = true;
//		}
//		else
//		{
//			// Set
//			chVoltageAll_V->chVoltage_V[chNum] = 0;
//			chVoltageAll_V->isEnabled[chNum] = false;
//		}
//	}

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
static Alx_Status AlxAds114s08_Reg_Read(AlxAds114s08* me, void* reg)
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
	if (status != Alx_Ok) { AlxSpi_Master_DeAssertCs(me->spi); ALX_ADS114S08_TRACE("Err"); return status; }

	// DeAssert CS
	AlxSpi_Master_DeAssertCs(me->spi);

	// Set
	*regValPtr = dataReadArr[2];

	// Return
	return Alx_Ok;
}
static Alx_Status AlxAds114s08_Reg_Write(AlxAds114s08* me, void* reg)
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
	if (status != Alx_Ok) { AlxSpi_Master_DeAssertCs(me->spi); ALX_ADS114S08_TRACE("Err"); return status; }

	// DeAssert CS
	AlxSpi_Master_DeAssertCs(me->spi);

	// Check if data was written ok
	if (dataWriteArr[1] != dataReadArr[2]) { ALX_ADS114S08_TRACE("Err"); return Alx_Err; }

	// Return
	return Alx_Ok;
}
static Alx_Status AlxAds114s08_Reg_WriteAll(AlxAds114s08* me)
{
	// Write
	if( AlxAds114s08_Reg_Write(me, &me->reg.reg_0x00_ID			) != Alx_Ok) { ALX_ADS114S08_TRACE("Err"); return Alx_Err; }
	if( AlxAds114s08_Reg_Write(me, &me->reg.reg_0x01_STATUS		) != Alx_Ok) { ALX_ADS114S08_TRACE("Err"); return Alx_Err; }
	if( AlxAds114s08_Reg_Write(me, &me->reg.reg_0x02_INPMUX		) != Alx_Ok) { ALX_ADS114S08_TRACE("Err"); return Alx_Err; }
	if( AlxAds114s08_Reg_Write(me, &me->reg.reg_0x03_PGA		) != Alx_Ok) { ALX_ADS114S08_TRACE("Err"); return Alx_Err; }
	if( AlxAds114s08_Reg_Write(me, &me->reg.reg_0x04_DATARATE	) != Alx_Ok) { ALX_ADS114S08_TRACE("Err"); return Alx_Err; }
	if( AlxAds114s08_Reg_Write(me, &me->reg.reg_0x05_REF		) != Alx_Ok) { ALX_ADS114S08_TRACE("Err"); return Alx_Err; }
	if( AlxAds114s08_Reg_Write(me, &me->reg.reg_0x06_IDACMAG	) != Alx_Ok) { ALX_ADS114S08_TRACE("Err"); return Alx_Err; }
	if( AlxAds114s08_Reg_Write(me, &me->reg.reg_0x07_IDACMUX	) != Alx_Ok) { ALX_ADS114S08_TRACE("Err"); return Alx_Err; }
	if( AlxAds114s08_Reg_Write(me, &me->reg.reg_0x08_VBIAS		) != Alx_Ok) { ALX_ADS114S08_TRACE("Err"); return Alx_Err; }
	if( AlxAds114s08_Reg_Write(me, &me->reg.reg_0x09_SYS		) != Alx_Ok) { ALX_ADS114S08_TRACE("Err"); return Alx_Err; }
	if( AlxAds114s08_Reg_Write(me, &me->reg.reg_0x0A_RESERVED	) != Alx_Ok) { ALX_ADS114S08_TRACE("Err"); return Alx_Err; }
	if( AlxAds114s08_Reg_Write(me, &me->reg.reg_0x0B_OFCAL0		) != Alx_Ok) { ALX_ADS114S08_TRACE("Err"); return Alx_Err; }
	if( AlxAds114s08_Reg_Write(me, &me->reg.reg_0x0C_OFCAL1		) != Alx_Ok) { ALX_ADS114S08_TRACE("Err"); return Alx_Err; }
	if( AlxAds114s08_Reg_Write(me, &me->reg.reg_0x0D_RESERVED	) != Alx_Ok) { ALX_ADS114S08_TRACE("Err"); return Alx_Err; }
	if( AlxAds114s08_Reg_Write(me, &me->reg.reg_0x0E_FSCAL0		) != Alx_Ok) { ALX_ADS114S08_TRACE("Err"); return Alx_Err; }
	if( AlxAds114s08_Reg_Write(me, &me->reg.reg_0x0F_FSCAL1		) != Alx_Ok) { ALX_ADS114S08_TRACE("Err"); return Alx_Err; }
	if( AlxAds114s08_Reg_Write(me, &me->reg.reg_0x10_GPIODAT	) != Alx_Ok) { ALX_ADS114S08_TRACE("Err"); return Alx_Err; }
	if( AlxAds114s08_Reg_Write(me, &me->reg.reg_0x11_GPIOCON	) != Alx_Ok) { ALX_ADS114S08_TRACE("Err"); return Alx_Err; }

	// Return
	return Alx_Ok;
}

#endif	// #if defined(ALX_C_LIB)
