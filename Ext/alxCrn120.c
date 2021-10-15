/**
  ******************************************************************************
  * @file alxAdau1961.h
  * @brief Auralix C Library - ALX Audio Codec ADAU1961 Module
  * @version $LastChangedRevision: 5305 $
  * @date $LastChangedDate: 2021-06-01 19:13:59 +0200 (Tue, 01 Jun 2021) $
  ******************************************************************************
  */

//******************************************************************************
// Includes
//******************************************************************************
#include "alxCrn120.h"

//******************************************************************************
// Private Functions
//******************************************************************************
static void AlxCrn120_RegBlockStruct_SetAddr(AlxCrn120* me);
static void AlxCrn120_RegBlockStruct_SetLen(AlxCrn120* me);
static void AlxCrn120_RegBlockStruct_SetValToZero(AlxCrn120* me);
static void AlxCrn120_RegBlockStruct_SetValToDefault(AlxCrn120* me);
//static Alx_Status AlxPca9430_Reg_Write(AlxPca9430* me, void* reg);
//static Alx_Status AlxPca9430_Reg_Read(AlxPca9430* me, void* reg);
//static Alx_Status AlxPca9430_Reg_WriteVal(AlxPca9430* me);
//
//
////******************************************************************************
//// Weak Functions
////******************************************************************************
//void AlxPca9430_RegStruct_SetVal(AlxPca9430* me);
//
////******************************************************************************
//// Constructor
////******************************************************************************
void AlxCrn120_Ctor
(
	AlxCrn120* me,
	AlxI2c* i2c,
	uint8_t i2cAddr,
	AlxIoPin* di_EventDetect,	
	AlxIoPin* di_NfcFieldDetect,
	uint8_t i2cNumOfTries,
	uint16_t i2cTimeout_ms
)
{
	// Parameters - Const

	// Objects - External
	me->i2c = i2c;
	me->di_EventDetect = di_EventDetect;
	me->di_NfcFieldDetect = di_NfcFieldDetect;

	// Parameters
	me->i2cAddr = i2cAddr;
	me->i2cNumOfTries = i2cNumOfTries;
	me->i2cTimeout_ms = i2cTimeout_ms;

	// Variables
//	AlxPca9430_RegBlockStruct_SetAddr(me);
//	AlxPca9430_RegBlockStruct_SetLen(me);
//	AlxPca9430_RegBlockStruct_SetValToZero(me);

	// Info
	me->isInit = false;
	me->wasCtorCalled = true;
}


////******************************************************************************
//// Functions
////******************************************************************************
//Alx_Status AlxPca9430_Init(AlxPca9430* me)
//{
//	ALX_PCA9430_ASSERT(me->isInit == false);
//	ALX_PCA9430_ASSERT(me->wasCtorCalled == true);
//
//	Alx_Status status = Alx_Err;
//	
//	// #1 Init GPIO
//	AlxIoPin_Init(me->do_SleepEn);
//	AlxIoPin_Init(me->di_Interrupt);
//
//	// #2 Init I2C
//	status = AlxI2c_Init(me->i2c);
//	if (status != Alx_Ok) { ALX_PCA9430_TRACE("Err_AlxI2c_Init"); return status; }
//
////	// #3 Check if slave ready
////	status = AlxI2c_Master_IsSlaveReady(me->i2c, me->i2cAddr, 1, 1000);
////	if (status != Alx_Ok) { ALX_PCA9430_TRACE("Err_AlxI2c_IsSlaveReady"); return status; }
//	
//	// #4 Set registers values to default
//	 AlxPca9430_RegStruct_SetValToDefault(me);
//	
//	// #6 Set registers values - WEAK
//	AlxPca9430_RegStruct_SetVal(me);
//	
//	// #7 Write registers
//	status = AlxPca9430_Reg_WriteVal(me);
//	if (status != Alx_Ok) { ALX_PCA9430_TRACE("Err_Reg_WriteVal"); return status;}
//
//	// #9 Set isInit
//	me->isInit = true;
//
//	// #10 Return OK
//	return Alx_Ok;
//}
//Alx_Status AlxPca9430_DeInit(AlxPca9430* me)
//{
//	ALX_PCA9430_ASSERT(me->isInit == true);
//	ALX_PCA9430_ASSERT(me->wasCtorCalled == true);
//
//	// #1 DeInit GPIO
//	AlxIoPin_DeInit(me->do_SleepEn);
//	AlxIoPin_DeInit(me->di_Interrupt);
//
//	// #2 Reset isInit
//	me->isInit = false;
//}
//

////******************************************************************************
//// Private Functions
////******************************************************************************
static void AlxCrn120_RegBlockStruct_SetAddr(AlxCrn120* me)
{
	me->regblock._0x00						.addr =	0x00;
	me->regblock._0x38						.addr =	0x38;
	me->regblock._0x39						.addr =	0x39;
	me->regblock._0x3A_ConfigurationReg		.addr =	0x3A;
	me->regblock._0xFE_SessionReg			.addr =	0xFE;
}

static void AlxCrn120_RegBlockStruct_SetLen(AlxCrn120* me)
{
	me->regblock._0x00						.len = sizeof(me->regblock._0x00						.val);
	me->regblock._0x38						.len = sizeof(me->regblock._0x38						.val);
	me->regblock._0x39						.len = sizeof(me->regblock._0x39						.val);
	me->regblock._0x3A_ConfigurationReg		.len = sizeof(me->regblock._0x3A_ConfigurationReg		.val);
	me->regblock._0xFE_SessionReg			.len = sizeof(me->regblock._0xFE_SessionReg				.val);
}

static void AlxCrn120_RegBlockStruct_SetValToZero(AlxCrn120* me)
{
	me->regblock._0x00						.val.raw[0x0]	= 0b00000000;
	me->regblock._0x00						.val.raw[0x1]	= 0b00000000;
	me->regblock._0x00						.val.raw[0x2]	= 0b00000000;
	me->regblock._0x00						.val.raw[0x3]	= 0b00000000;
	me->regblock._0x00						.val.raw[0x4]	= 0b00000000;
	me->regblock._0x00						.val.raw[0x5]	= 0b00000000;
	me->regblock._0x00						.val.raw[0x6]	= 0b00000000;
	me->regblock._0x00						.val.raw[0x7]	= 0b00000000;
	me->regblock._0x00						.val.raw[0x8]	= 0b00000000;
	me->regblock._0x00						.val.raw[0x9]	= 0b00000000;
	me->regblock._0x00						.val.raw[0xA]	= 0b00000000;
	me->regblock._0x00						.val.raw[0xB]	= 0b00000000;
	me->regblock._0x00						.val.raw[0xC]	= 0b00000000;
	me->regblock._0x00						.val.raw[0xD]	= 0b00000000;
	me->regblock._0x00						.val.raw[0xE]	= 0b00000000;
	me->regblock._0x00						.val.raw[0xF]	= 0b00000000;
	
	me->regblock._0x38						.val.raw[0x0]	= 0b00000000;
	me->regblock._0x38						.val.raw[0x1]	= 0b00000000;
	me->regblock._0x38						.val.raw[0x2]	= 0b00000000;
	me->regblock._0x38						.val.raw[0x3]	= 0b00000000;
	me->regblock._0x38						.val.raw[0x4]	= 0b00000000;
	me->regblock._0x38						.val.raw[0x5]	= 0b00000000;
	me->regblock._0x38						.val.raw[0x6]	= 0b00000000;
	me->regblock._0x38						.val.raw[0x7]	= 0b00000000;
	me->regblock._0x38						.val.raw[0x8]	= 0b00000000;
	me->regblock._0x38						.val.raw[0x9]	= 0b00000000;
	me->regblock._0x38						.val.raw[0xA]	= 0b00000000;
	me->regblock._0x38						.val.raw[0xB]	= 0b00000000;
	me->regblock._0x38						.val.raw[0xC]	= 0b00000000;
	me->regblock._0x38						.val.raw[0xD]	= 0b00000000;
	me->regblock._0x38						.val.raw[0xE]	= 0b00000000;
	me->regblock._0x38						.val.raw[0xF]	= 0b00000000;
	
	me->regblock._0x39						.val.raw[0x0]	= 0b00000000;
	me->regblock._0x39						.val.raw[0x1]	= 0b00000000;
	me->regblock._0x39						.val.raw[0x2]	= 0b00000000;
	me->regblock._0x39						.val.raw[0x3]	= 0b00000000;
	me->regblock._0x39						.val.raw[0x4]	= 0b00000000;
	me->regblock._0x39						.val.raw[0x5]	= 0b00000000;
	me->regblock._0x39						.val.raw[0x6]	= 0b00000000;
	me->regblock._0x39						.val.raw[0x7]	= 0b00000000;
	me->regblock._0x39						.val.raw[0x8]	= 0b00000000;
	me->regblock._0x39						.val.raw[0x9]	= 0b00000000;
	me->regblock._0x39						.val.raw[0xA]	= 0b00000000;
	me->regblock._0x39						.val.raw[0xB]	= 0b00000000;
	me->regblock._0x39						.val.raw[0xC]	= 0b00000000;
	me->regblock._0x39						.val.raw[0xD]	= 0b00000000;
	me->regblock._0x39						.val.raw[0xE]	= 0b00000000;
	me->regblock._0x39						.val.raw[0xF]	= 0b00000000;
	
	me->regblock._0x3A_ConfigurationReg		.val.raw[0x0]	= 0b00000000;
	me->regblock._0x3A_ConfigurationReg		.val.raw[0x1]	= 0b00000000;
	me->regblock._0x3A_ConfigurationReg		.val.raw[0x2]	= 0b00000000;
	me->regblock._0x3A_ConfigurationReg		.val.raw[0x3]	= 0b00000000;
	me->regblock._0x3A_ConfigurationReg		.val.raw[0x4]	= 0b00000000;
	me->regblock._0x3A_ConfigurationReg		.val.raw[0x5]	= 0b00000000;
	me->regblock._0x3A_ConfigurationReg		.val.raw[0x6]	= 0b00000000;
	me->regblock._0x3A_ConfigurationReg		.val.raw[0x7]	= 0b00000000;
	me->regblock._0x3A_ConfigurationReg		.val.raw[0x8]	= 0b00000000;
	me->regblock._0x3A_ConfigurationReg		.val.raw[0x9]	= 0b00000000;
	me->regblock._0x3A_ConfigurationReg		.val.raw[0xA]	= 0b00000000;
	me->regblock._0x3A_ConfigurationReg		.val.raw[0xB]	= 0b00000000;
	me->regblock._0x3A_ConfigurationReg		.val.raw[0xC]	= 0b00000000;
	me->regblock._0x3A_ConfigurationReg		.val.raw[0xD]	= 0b00000000;
	me->regblock._0x3A_ConfigurationReg		.val.raw[0xE]	= 0b00000000;
	me->regblock._0x3A_ConfigurationReg		.val.raw[0xF]	= 0b00000000;
	
	me->regblock._0xFE_SessionReg			.val.raw[0x0]	= 0b00000000;
	me->regblock._0xFE_SessionReg			.val.raw[0x1]	= 0b00000000;
	me->regblock._0xFE_SessionReg			.val.raw[0x2]	= 0b00000000;
	me->regblock._0xFE_SessionReg			.val.raw[0x3]	= 0b00000000;
	me->regblock._0xFE_SessionReg			.val.raw[0x4]	= 0b00000000;
	me->regblock._0xFE_SessionReg			.val.raw[0x5]	= 0b00000000;
	me->regblock._0xFE_SessionReg			.val.raw[0x6]	= 0b00000000;
	me->regblock._0xFE_SessionReg			.val.raw[0x7]	= 0b00000000;
	me->regblock._0xFE_SessionReg			.val.raw[0x8]	= 0b00000000;
	me->regblock._0xFE_SessionReg			.val.raw[0x9]	= 0b00000000;
	me->regblock._0xFE_SessionReg			.val.raw[0xA]	= 0b00000000;
	me->regblock._0xFE_SessionReg			.val.raw[0xB]	= 0b00000000;
	me->regblock._0xFE_SessionReg			.val.raw[0xC]	= 0b00000000;
	me->regblock._0xFE_SessionReg			.val.raw[0xD]	= 0b00000000;
	me->regblock._0xFE_SessionReg			.val.raw[0xE]	= 0b00000000;
	me->regblock._0xFE_SessionReg			.val.raw[0xF]	= 0b00000000;
}

static void AlxCrn120_RegBlockStruct_SetValToDefault(AlxCrn120* me)
{
	me->regblock._0x00						.val.raw[0x0]	= 0b00000000;	/*NISO PRAVILNE VREDNOSTI*/
	me->regblock._0x00						.val.raw[0x1]	= 0b00000000;	/*NISO PRAVILNE VREDNOSTI*/
	me->regblock._0x00						.val.raw[0x2]	= 0b00000000;	/*NISO PRAVILNE VREDNOSTI*/
	me->regblock._0x00						.val.raw[0x3]	= 0b00000000;	/*NISO PRAVILNE VREDNOSTI*/
	me->regblock._0x00						.val.raw[0x4]	= 0b00000000;	/*NISO PRAVILNE VREDNOSTI*/
	me->regblock._0x00						.val.raw[0x5]	= 0b00000000;	/*NISO PRAVILNE VREDNOSTI*/
	me->regblock._0x00						.val.raw[0x6]	= 0b00000000;	/*NISO PRAVILNE VREDNOSTI*/
	me->regblock._0x00						.val.raw[0x7]	= 0b00000000;	/*NISO PRAVILNE VREDNOSTI*/
	me->regblock._0x00						.val.raw[0x8]	= 0b00000000;	/*NISO PRAVILNE VREDNOSTI*/
	me->regblock._0x00						.val.raw[0x9]	= 0b00000000;	/*NISO PRAVILNE VREDNOSTI*/
	me->regblock._0x00						.val.raw[0xA]	= 0b00000000;	/*NISO PRAVILNE VREDNOSTI*/
	me->regblock._0x00						.val.raw[0xB]	= 0b00000000;	/*NISO PRAVILNE VREDNOSTI*/
	me->regblock._0x00						.val.raw[0xC]	= 0b00000000;	/*NISO PRAVILNE VREDNOSTI*/
	me->regblock._0x00						.val.raw[0xD]	= 0b00000000;	/*NISO PRAVILNE VREDNOSTI*/
	me->regblock._0x00						.val.raw[0xE]	= 0b00000000;	/*NISO PRAVILNE VREDNOSTI*/
	me->regblock._0x00						.val.raw[0xF]	= 0b00000000;	/*NISO PRAVILNE VREDNOSTI*/
																			/*NISO PRAVILNE VREDNOSTI*/
	me->regblock._0x38						.val.raw[0x0]	= 0b00000000;	/*NISO PRAVILNE VREDNOSTI*/
	me->regblock._0x38						.val.raw[0x1]	= 0b00000000;	/*NISO PRAVILNE VREDNOSTI*/
	me->regblock._0x38						.val.raw[0x2]	= 0b00000000;	/*NISO PRAVILNE VREDNOSTI*/
	me->regblock._0x38						.val.raw[0x3]	= 0b00000000;	/*NISO PRAVILNE VREDNOSTI*/
	me->regblock._0x38						.val.raw[0x4]	= 0b00000000;	/*NISO PRAVILNE VREDNOSTI*/
	me->regblock._0x38						.val.raw[0x5]	= 0b00000000;	/*NISO PRAVILNE VREDNOSTI*/
	me->regblock._0x38						.val.raw[0x6]	= 0b00000000;	/*NISO PRAVILNE VREDNOSTI*/
	me->regblock._0x38						.val.raw[0x7]	= 0b00000000;	/*NISO PRAVILNE VREDNOSTI*/
	me->regblock._0x38						.val.raw[0x8]	= 0b00000000;	/*NISO PRAVILNE VREDNOSTI*/
	me->regblock._0x38						.val.raw[0x9]	= 0b00000000;	/*NISO PRAVILNE VREDNOSTI*/
	me->regblock._0x38						.val.raw[0xA]	= 0b00000000;	/*NISO PRAVILNE VREDNOSTI*/
	me->regblock._0x38						.val.raw[0xB]	= 0b00000000;	/*NISO PRAVILNE VREDNOSTI*/
	me->regblock._0x38						.val.raw[0xC]	= 0b00000000;	/*NISO PRAVILNE VREDNOSTI*/
	me->regblock._0x38						.val.raw[0xD]	= 0b00000000;	/*NISO PRAVILNE VREDNOSTI*/
	me->regblock._0x38						.val.raw[0xE]	= 0b00000000;	/*NISO PRAVILNE VREDNOSTI*/
	me->regblock._0x38						.val.raw[0xF]	= 0b00000000;	/*NISO PRAVILNE VREDNOSTI*/
																			/*NISO PRAVILNE VREDNOSTI*/
	me->regblock._0x39						.val.raw[0x0]	= 0b00000000;	/*NISO PRAVILNE VREDNOSTI*/
	me->regblock._0x39						.val.raw[0x1]	= 0b00000000;	/*NISO PRAVILNE VREDNOSTI*/
	me->regblock._0x39						.val.raw[0x2]	= 0b00000000;	/*NISO PRAVILNE VREDNOSTI*/
	me->regblock._0x39						.val.raw[0x3]	= 0b00000000;	/*NISO PRAVILNE VREDNOSTI*/
	me->regblock._0x39						.val.raw[0x4]	= 0b00000000;	/*NISO PRAVILNE VREDNOSTI*/
	me->regblock._0x39						.val.raw[0x5]	= 0b00000000;	/*NISO PRAVILNE VREDNOSTI*/
	me->regblock._0x39						.val.raw[0x6]	= 0b00000000;	/*NISO PRAVILNE VREDNOSTI*/
	me->regblock._0x39						.val.raw[0x7]	= 0b00000000;	/*NISO PRAVILNE VREDNOSTI*/
	me->regblock._0x39						.val.raw[0x8]	= 0b00000000;	/*NISO PRAVILNE VREDNOSTI*/
	me->regblock._0x39						.val.raw[0x9]	= 0b00000000;	/*NISO PRAVILNE VREDNOSTI*/
	me->regblock._0x39						.val.raw[0xA]	= 0b00000000;	/*NISO PRAVILNE VREDNOSTI*/
	me->regblock._0x39						.val.raw[0xB]	= 0b00000000;	/*NISO PRAVILNE VREDNOSTI*/
	me->regblock._0x39						.val.raw[0xC]	= 0b00000000;	/*NISO PRAVILNE VREDNOSTI*/
	me->regblock._0x39						.val.raw[0xD]	= 0b00000000;	/*NISO PRAVILNE VREDNOSTI*/
	me->regblock._0x39						.val.raw[0xE]	= 0b00000000;	/*NISO PRAVILNE VREDNOSTI*/
	me->regblock._0x39						.val.raw[0xF]	= 0b00000000;	/*NISO PRAVILNE VREDNOSTI*/
																			/*NISO PRAVILNE VREDNOSTI*/
	me->regblock._0x3A_ConfigurationReg		.val.raw[0x0]	= 0b00000000;	/*NISO PRAVILNE VREDNOSTI*/
	me->regblock._0x3A_ConfigurationReg		.val.raw[0x1]	= 0b00000000;	/*NISO PRAVILNE VREDNOSTI*/
	me->regblock._0x3A_ConfigurationReg		.val.raw[0x2]	= 0b00000000;	/*NISO PRAVILNE VREDNOSTI*/
	me->regblock._0x3A_ConfigurationReg		.val.raw[0x3]	= 0b00000000;	/*NISO PRAVILNE VREDNOSTI*/
	me->regblock._0x3A_ConfigurationReg		.val.raw[0x4]	= 0b00000000;	/*NISO PRAVILNE VREDNOSTI*/
	me->regblock._0x3A_ConfigurationReg		.val.raw[0x5]	= 0b00000000;	/*NISO PRAVILNE VREDNOSTI*/
	me->regblock._0x3A_ConfigurationReg		.val.raw[0x6]	= 0b00000000;	/*NISO PRAVILNE VREDNOSTI*/
	me->regblock._0x3A_ConfigurationReg		.val.raw[0x7]	= 0b00000000;	/*NISO PRAVILNE VREDNOSTI*/
	me->regblock._0x3A_ConfigurationReg		.val.raw[0x8]	= 0b00000000;	/*NISO PRAVILNE VREDNOSTI*/
	me->regblock._0x3A_ConfigurationReg		.val.raw[0x9]	= 0b00000000;	/*NISO PRAVILNE VREDNOSTI*/
	me->regblock._0x3A_ConfigurationReg		.val.raw[0xA]	= 0b00000000;	/*NISO PRAVILNE VREDNOSTI*/
	me->regblock._0x3A_ConfigurationReg		.val.raw[0xB]	= 0b00000000;	/*NISO PRAVILNE VREDNOSTI*/
	me->regblock._0x3A_ConfigurationReg		.val.raw[0xC]	= 0b00000000;	/*NISO PRAVILNE VREDNOSTI*/
	me->regblock._0x3A_ConfigurationReg		.val.raw[0xD]	= 0b00000000;	/*NISO PRAVILNE VREDNOSTI*/
	me->regblock._0x3A_ConfigurationReg		.val.raw[0xE]	= 0b00000000;	/*NISO PRAVILNE VREDNOSTI*/
	me->regblock._0x3A_ConfigurationReg		.val.raw[0xF]	= 0b00000000;	/*NISO PRAVILNE VREDNOSTI*/
																			/*NISO PRAVILNE VREDNOSTI*/
	me->regblock._0xFE_SessionReg			.val.raw[0x0]	= 0b00000000;	/*NISO PRAVILNE VREDNOSTI*/
	me->regblock._0xFE_SessionReg			.val.raw[0x1]	= 0b00000000;	/*NISO PRAVILNE VREDNOSTI*/
	me->regblock._0xFE_SessionReg			.val.raw[0x2]	= 0b00000000;	/*NISO PRAVILNE VREDNOSTI*/
	me->regblock._0xFE_SessionReg			.val.raw[0x3]	= 0b00000000;	/*NISO PRAVILNE VREDNOSTI*/
	me->regblock._0xFE_SessionReg			.val.raw[0x4]	= 0b00000000;	/*NISO PRAVILNE VREDNOSTI*/
	me->regblock._0xFE_SessionReg			.val.raw[0x5]	= 0b00000000;	/*NISO PRAVILNE VREDNOSTI*/
	me->regblock._0xFE_SessionReg			.val.raw[0x6]	= 0b00000000;	/*NISO PRAVILNE VREDNOSTI*/
	me->regblock._0xFE_SessionReg			.val.raw[0x7]	= 0b00000000;	/*NISO PRAVILNE VREDNOSTI*/
	me->regblock._0xFE_SessionReg			.val.raw[0x8]	= 0b00000000;	/*NISO PRAVILNE VREDNOSTI*/
	me->regblock._0xFE_SessionReg			.val.raw[0x9]	= 0b00000000;	/*NISO PRAVILNE VREDNOSTI*/
	me->regblock._0xFE_SessionReg			.val.raw[0xA]	= 0b00000000;	/*NISO PRAVILNE VREDNOSTI*/
	me->regblock._0xFE_SessionReg			.val.raw[0xB]	= 0b00000000;	/*NISO PRAVILNE VREDNOSTI*/
	me->regblock._0xFE_SessionReg			.val.raw[0xC]	= 0b00000000;	/*NISO PRAVILNE VREDNOSTI*/
	me->regblock._0xFE_SessionReg			.val.raw[0xD]	= 0b00000000;	/*NISO PRAVILNE VREDNOSTI*/
	me->regblock._0xFE_SessionReg			.val.raw[0xE]	= 0b00000000;	/*NISO PRAVILNE VREDNOSTI*/
	me->regblock._0xFE_SessionReg			.val.raw[0xF]	= 0b00000000;	/*NISO PRAVILNE VREDNOSTI*/
}

//static Alx_Status AlxPca9430_Reg_Write(AlxPca9430* me, void* reg)
//{
//	uint8_t regAddr = *((uint8_t*)reg);
//	uint8_t regLen = *((uint8_t*)reg + sizeof(regAddr));
//	uint8_t* regValPtr = (uint8_t*)reg + sizeof(regAddr) + sizeof(regLen);
//
//	return AlxI2c_Master_StartWriteMemStop_Multi(me->i2c, me->i2cAddr, regAddr, AlxI2c_Master_MemAddrLen_8bit, regValPtr, regLen, me->i2cCheckWithRead, me->i2cNumOfTries, me->i2cTimeout_ms);
//}
//
//static Alx_Status AlxPca9430_Reg_Read(AlxPca9430* me, void* reg)
//{
//	uint8_t regAddr = *((uint8_t*)reg);
//	uint8_t regLen = *((uint8_t*)reg + sizeof(regAddr));
//	uint8_t* regValPtr = (uint8_t*)reg + sizeof(regAddr) + sizeof(regLen);
//
//	return AlxI2c_Master_StartReadMemStop(me->i2c, me->i2cAddr, regAddr, AlxI2c_Master_MemAddrLen_8bit, regValPtr, regLen, me->i2cNumOfTries, me->i2cTimeout_ms);
//}
//
//static Alx_Status AlxPca9430_Reg_WriteVal(AlxPca9430* me)
//{
//	Alx_Status status = AlxPca9430_Reg_Write(me, &me->reg._0x02_SYSTEM_INT_MASK);
//	if (status != Alx_Ok) { ALX_PCA9430_TRACE("Err_0x02_SYSTEM_INT_MASK			"); return status;}
//	
//	status = AlxPca9430_Reg_Write(me, &me->reg._0x04_VRECT_INT_MASK				);
//	if (status != Alx_Ok) { ALX_PCA9430_TRACE("Err_0x04_VRECT_INT_MASK			"); return status;}
//	
//	status = AlxPca9430_Reg_Write(me, &me->reg._0x06_BATCHG_INT_MASK			);
//	if (status != Alx_Ok) { ALX_PCA9430_TRACE("Err_0x06_VOUTLDO_INT_MASK		"); return status;}
//	
//	status = AlxPca9430_Reg_Write(me, &me->reg._0x08_VRECT_THD					);
//	if (status != Alx_Ok) { ALX_PCA9430_TRACE("Err_0x08_VRECT_THD				"); return status;}
//	
//	return Alx_Ok;
//}
//
////******************************************************************************
//// Weak Functions
////******************************************************************************
//
//ALX_WEAK void AlxPca9430_RegStruct_SetVal(AlxPca9430* me)
//{
//	
//	 (void)me;
//	ALX_PCA9430_TRACE("Define 'AlxPca9431_RegStruct_SetVal' function in your application.");
//	ALX_PCA9430_ASSERT(false);
//}
