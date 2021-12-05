/**
  ******************************************************************************
  * @file alxAdau1961.h
  * @brief Auralix C Library - ALX Audio Codec ADAU1961 Module
  * @version $LastChangedRevision: 5108 $
  * @date $LastChangedDate: 2021-05-14 16:19:47 +0200 (Fri, 14 May 2021) $
  ******************************************************************************
  */

//******************************************************************************
// Includes
//******************************************************************************
#include "alxPca9431.h"


//******************************************************************************
// Private Functions
//******************************************************************************
static void AlxPca9431_RegStruct_SetAddr(AlxPca9431* me);
static void AlxPca9431_RegStruct_SetLen(AlxPca9431* me);
static void AlxPca9431_RegStruct_SetValToZero(AlxPca9431* me);
static void AlxPca9431_RegStruct_SetValToDefault(AlxPca9431* me);
static Alx_Status AlxPca9431_Reg_Write(AlxPca9431* me, void* reg);
static Alx_Status AlxPca9431_Reg_Read(AlxPca9431* me, void* reg);
static Alx_Status AlxPca9431_Reg_WriteVal(AlxPca9431* me);


/************************************************************************************************************************************************************************************************************** SVINJARJENJE */
//static Alx_Status AlxPca9431_Reg_ReadMultiAdc(AlxPca9431* me, void* reg);
/************************************************************************************************************************************************************************************************************** SVINJARJENJE */


//******************************************************************************
// Weak Functions
//******************************************************************************
void AlxPca9431_RegStruct_SetVal(AlxPca9431* me);


//******************************************************************************
// Constructor
//******************************************************************************
void AlxPca9431_Ctor
(
	AlxPca9431* me,
	AlxI2c* i2c,
	uint8_t i2cAddr,
	AlxIoPin* do_SleepEn,
	AlxIoPin* di_Interrupt,
	bool i2cCheckWithRead,
	uint8_t i2cNumOfTries,
	uint16_t i2cTimeout_ms
)
{
	// Parameters - Const

	// Objects - External
	me->i2c = i2c;
	me->do_SleepEn = do_SleepEn;
	me->di_Interrupt = di_Interrupt;

	// Parameters
	me->i2cAddr = i2cAddr;
	me->i2cCheckWithRead = i2cCheckWithRead;
	me->i2cNumOfTries = i2cNumOfTries;
	me->i2cTimeout_ms = i2cTimeout_ms;

	// Variables
	AlxPca9431_RegStruct_SetAddr(me);
	AlxPca9431_RegStruct_SetLen(me);
	AlxPca9431_RegStruct_SetValToZero(me);

	// Info
	me->isInit = false;
	me->wasCtorCalled = true;
}


//******************************************************************************
// Functions
//******************************************************************************
Alx_Status AlxPca9431_Init(AlxPca9431* me)
{
	ALX_PCA9431_ASSERT(me->isInit == false);
	ALX_PCA9431_ASSERT(me->wasCtorCalled == true);

	Alx_Status status = Alx_Err;
	
	// #1 Init GPIO
	AlxIoPin_Init(me->do_SleepEn);
	//AlxIoPin_Init(me->di_Interrupt);

	// #2 Init I2C
	status = AlxI2c_Init(me->i2c);
	if (status != Alx_Ok) { ALX_PCA9431_TRACE("Err_AlxI2c_Init"); return status; }

//	// #3 Check if slave ready
//	status = AlxI2c_Master_IsSlaveReady(me->i2c, me->i2cAddr, 1, 1000);
//	if (status != Alx_Ok) { ALX_PCA9431_TRACE("Err_AlxI2c_IsSlaveReady"); return status; }
	
	// #4 Set registers values to default
	 AlxPca9431_RegStruct_SetValToDefault(me);
	
	// #5 Set registers values - WEAK
	AlxPca9431_RegStruct_SetVal(me);
	
	// #6 Write registers
	status = AlxPca9431_Reg_WriteVal(me);
	if (status != Alx_Ok) { ALX_PCA9431_TRACE("Err_Reg_WriteVal"); return status;}

	// #7 Set isInit
	me->isInit = true;

	// #8 Return OK
	return Alx_Ok;
}
Alx_Status AlxPca9431_DeInit(AlxPca9431* me)
{
	ALX_PCA9431_ASSERT(me->isInit == true);
	ALX_PCA9431_ASSERT(me->wasCtorCalled == true);

	// #1 DeInit GPIO
	AlxIoPin_DeInit(me->do_SleepEn);
	AlxIoPin_DeInit(me->di_Interrupt);

	// #2 Reset isInit
	me->isInit = false;

	// #3 Return OK
	return Alx_Ok;
}
Alx_Status AlxPca9431_LdoVout_GetVoltage_V(AlxPca9431* me, float* voltage_V) // 10 bit ADC
{
	ALX_PCA9431_ASSERT(me->isInit == true);
	ALX_PCA9431_ASSERT(me->wasCtorCalled == true);

	// #1 Prepare Variables
	uint8_t AdcHBitVoltage = 0;
	uint8_t AdcLBitVoltage = 0;
	uint16_t AdcBitVoltage = 0;

	// #2 
	Alx_Status status = AlxPca9431_Reg_Read(me, &me->reg._34h_VOUT_ADC_H);
	if (status != Alx_Ok) { ALX_PCA9431_TRACE("Err"); return status; }
	AdcHBitVoltage = me->reg._34h_VOUT_ADC_H.val.raw;
	AdcBitVoltage = (uint16_t) AdcHBitVoltage;
	AdcBitVoltage = AdcBitVoltage << 2;

	// #3 
	status = AlxPca9431_Reg_Read(me, &me->reg._35h_VOUT_ADC_L);
	if (status != Alx_Ok) { ALX_PCA9431_TRACE("Err"); return status; }
	AdcLBitVoltage = me->reg._35h_VOUT_ADC_L.val.raw;
	AdcLBitVoltage = AdcLBitVoltage >> 6;

	// #4 
	AdcBitVoltage += AdcLBitVoltage;
	*voltage_V = AdcBitVoltage*5.27/1000;

	// #5 Return OK
	return Alx_Ok;
}
Alx_Status AlxPca9431_LdoVout_GetCurrent_A(AlxPca9431* me, float* current_A) // 10 bit ADC
{
	ALX_PCA9431_ASSERT(me->isInit == true);
	ALX_PCA9431_ASSERT(me->wasCtorCalled == true);

	// #1 Prepare Variables
	uint8_t AdcHBitCurrent = 0;
	uint8_t AdcLBitCurrent = 0;
	uint16_t AdcBitCurrent = 0;

	// #2 
	Alx_Status status = AlxPca9431_Reg_Read(me, &me->reg._36h_IOUT_ADC_H);
	if (status != Alx_Ok) { ALX_PCA9431_TRACE("Err"); return status; }
	AdcHBitCurrent = me->reg._36h_IOUT_ADC_H.val.raw;
	AdcBitCurrent = (uint16_t) AdcHBitCurrent;
	AdcBitCurrent = AdcBitCurrent << 2;

	// #3 
	status = AlxPca9431_Reg_Read(me, &me->reg._37h_IOUT_ADC_L);
	if (status != Alx_Ok) { ALX_PCA9431_TRACE("Err"); return status; }
	AdcLBitCurrent = me->reg._37h_IOUT_ADC_L.val.raw;
	AdcLBitCurrent = AdcLBitCurrent >> 6;

	// #4 
	AdcBitCurrent += AdcLBitCurrent;
	*current_A = AdcBitCurrent * 421.8 / 1000000;

	// #5 Return OK
	return Alx_Ok;
}
Alx_Status AlxPca9431_Rect_GetVoltage_V(AlxPca9431* me, float* voltage_V) // 10 bit ADC
{
	ALX_PCA9431_ASSERT(me->isInit == true);
	ALX_PCA9431_ASSERT(me->wasCtorCalled == true);

	// #1 Prepare Variables
	uint8_t AdcHBitVoltage = 0;
	uint8_t AdcLBitVoltage = 0;
	uint16_t AdcBitVoltage = 0;

	// #2 
	Alx_Status status = AlxPca9431_Reg_Read(me, &me->reg._30h_VRECT_ADC_H);
	//Alx_Status status = AlxPca9431_Reg_ReadMultiAdc(me, &me->reg._30_VRECT_ADC_H);
	if (status != Alx_Ok) { ALX_PCA9431_TRACE("Err"); return status; }
	AdcHBitVoltage = me->reg._30h_VRECT_ADC_H.val.raw;
	AdcBitVoltage = (uint16_t) AdcHBitVoltage;
	AdcBitVoltage = AdcBitVoltage << 2;

	// #3 
	status = AlxPca9431_Reg_Read(me, &me->reg._31h_VRECT_ADC_L);
	if (status != Alx_Ok) { ALX_PCA9431_TRACE("Err"); return status; }
	AdcLBitVoltage = me->reg._31h_VRECT_ADC_L.val.raw;
	AdcLBitVoltage = AdcLBitVoltage >> 6;

	// #4
	AdcBitVoltage += AdcLBitVoltage;
	AlxTrace_WriteFormat(&alxTrace, "Test Adc_AdcBitVoltage: %lu \r\n", AdcBitVoltage);
	*voltage_V = AdcBitVoltage * 15.82 / 1000;;

	// #5 Return OK
	return Alx_Ok;
}
Alx_Status AlxPca9431_Rect_GetCurrent_A(AlxPca9431* me, float* current_A) // 10 bit ADC
{
	ALX_PCA9431_ASSERT(me->isInit == true);
	ALX_PCA9431_ASSERT(me->wasCtorCalled == true);

	// #1 Prepare Variables
	uint8_t AdcHBitCurrent = 0;
	uint8_t AdcLBitCurrent = 0;
	uint16_t AdcBitCurrent = 0;

	// #2 
	Alx_Status status = AlxPca9431_Reg_Read(me, &me->reg._38h_IRECT_ADC_H);
	if (status != Alx_Ok) { ALX_PCA9431_TRACE("Err"); return status; }
	AdcHBitCurrent = me->reg._38h_IRECT_ADC_H.val.raw;
	AdcBitCurrent = (uint16_t) AdcHBitCurrent;
	AdcBitCurrent = AdcBitCurrent << 2;

	// #3
	status = AlxPca9431_Reg_Read(me, &me->reg._39h_IRECT_ADC_L);
	if (status != Alx_Ok) { ALX_PCA9431_TRACE("Err"); return status; }
	AdcLBitCurrent = me->reg._39h_IRECT_ADC_L.val.raw;
	AdcLBitCurrent = AdcLBitCurrent >> 6;

	// #4 
	AdcBitCurrent += AdcLBitCurrent;
	*current_A = AdcBitCurrent * 421.8 / 1000000;

	// #5 Return OK
	return Alx_Ok;
}
Alx_Status AlxPca9431_TempSens_GetTemp_degC(AlxPca9431* me, float* temp_degC)
{
	ALX_PCA9431_ASSERT(me->isInit == true);
	ALX_PCA9431_ASSERT(me->wasCtorCalled == true);

	// #1 Prepare Variable
	uint8_t BitTemp = 0;

	// #2 
	Alx_Status status = AlxPca9431_Reg_Read(me, &me->reg._3Ah_TDIE_ADC);
	if (status != Alx_Ok) { ALX_PCA9431_TRACE("Err"); return status; }
	BitTemp = me->reg._3Ah_TDIE_ADC.val.raw;

	// #3 
	switch (BitTemp) 
	{
	case 0:		*temp_degC = -43	;	break;
	case 1:		*temp_degC = -39	;	break;	
	case 2:		*temp_degC = -35	;	break;
	case 3:		*temp_degC = -30	;	break;
	case 4:		*temp_degC = -26	;	break;
	case 5:		*temp_degC = -21	;	break;	
	case 6:		*temp_degC = -17	;	break;
	case 7:		*temp_degC = -12.5	;	break;
	case 8:		*temp_degC = -8		;	break;	
	case 9:		*temp_degC = -4		;	break;
	case 10:	*temp_degC = 0		;	break;
	case 11:	*temp_degC = 5		;	break;
	case 12:	*temp_degC = 9		;	break;
	case 13:	*temp_degC = 13.5	;	break;
	case 14:	*temp_degC = 18		;	break;
	case 15:	*temp_degC = 22.5	;	break;
	case 16:	*temp_degC = 27		;	break;
	case 17:	*temp_degC = 31		;	break;
	case 18:	*temp_degC = 35		;	break;
	case 19:	*temp_degC = 40		;	break;
	case 20:	*temp_degC = 44		;	break;
	case 21:	*temp_degC = 48		;	break;
	case 22:	*temp_degC = 53		;	break;
	case 23:	*temp_degC = 57		;	break;
	case 24:	*temp_degC = 62		;	break;
	case 25:	*temp_degC = 65.5	;	break;
	case 26:	*temp_degC = 71		;	break;
	case 27:	*temp_degC = 75		;	break;
	case 28:	*temp_degC = 79.5	;	break;
	case 29:	*temp_degC = 84		;	break;
	case 30:	*temp_degC = 88		;	break;
	case 31:	*temp_degC = 92		;	break;
	case 32:	*temp_degC = 96		;	break;
	case 33:	*temp_degC = 100.5	;	break;
	case 34:	*temp_degC = 105	;	break;
	case 35:	*temp_degC = 109	;	break;
	case 36:	*temp_degC = 114	;	break;
	case 37:	*temp_degC = 118	;	break;
	case 38:	*temp_degC = 122	;	break;
	case 39:	*temp_degC = 126	;	break;
	case 40:	*temp_degC = 130	;	break;
	case 41:	*temp_degC = 134.5	;	break;
	case 42:	*temp_degC = 138	;	break;
	case 43:	*temp_degC = 141	;	break;
	case 44:	*temp_degC = 145	;	break;
	case 45:	*temp_degC = 148	;	break;
	case 46:	*temp_degC = 152	;	break;
	case 47:	*temp_degC = 156	;	break;
	default:	*temp_degC = 9999.9	; ALX_PCA9431_ASSERT(false); return Alx_Err;	break;
	}
}
Alx_Status AlxPca9431_VTune_SetVoltage_V(AlxPca9431* me, float* voltage_V) // 0-3.3V 5 bit DAC - TODO
{
	// TODO
	ALX_PCA9431_ASSERT(false);
	*voltage_V = 0;
	return Alx_Err;
}
Alx_Status AlxPca9431_VTune_GetVoltage_V(AlxPca9431* me, float* voltage_V) // 10 bit ADC - TODO
{
	ALX_PCA9431_ASSERT(me->isInit == true);
	ALX_PCA9431_ASSERT(me->wasCtorCalled == true);

	// #1 Prepare Vareables
	uint8_t AdcHBitVoltage = 0;
	uint8_t AdcLBitVoltage = 0;
	uint16_t AdcBitVoltage = 0;

	// #2 
	Alx_Status status = AlxPca9431_Reg_Read(me, &me->reg._32h_VTUNE_ADC_H);
	if (status != Alx_Ok) { ALX_PCA9431_TRACE("Err"); return status; }
	AdcHBitVoltage = me->reg._32h_VTUNE_ADC_H.val.raw;
	AdcBitVoltage = (uint16_t) AdcHBitVoltage;
	AdcBitVoltage = AdcBitVoltage << 2;

	// #3 
	status = AlxPca9431_Reg_Read(me, &me->reg._33h_VTUNE_ADC_L);
	if (status != Alx_Ok) { ALX_PCA9431_TRACE("Err"); return status; }
	AdcLBitVoltage = me->reg._33h_VTUNE_ADC_L.val.raw;
	AdcLBitVoltage = AdcLBitVoltage >> 6;

	// #4 
	AdcBitVoltage += AdcLBitVoltage;
	*voltage_V = AdcBitVoltage * 3.32 / 1000;

	// #5 Return OK
	return Alx_Ok;
}
Alx_Status AlxPca9431_Exit_EcoMode(AlxPca9431* me) 
{
	ALX_PCA9431_ASSERT(me->isInit == true);
	ALX_PCA9431_ASSERT(me->wasCtorCalled == true);

	me->reg._0Bh_WD_EN_RST.val.ECO_EXIT = EcoExt_Exit;

	Alx_Status status = AlxPca9431_Reg_Write(me, &me->reg._0Bh_WD_EN_RST);

	if (status != Alx_Ok) { ALX_PCA9431_TRACE("Reg__0B_WD_EN_RST -> EcoModo not exit	"); return status;}

	ALX_PCA9431_TRACE("EcoModo exit	");

	// #5 Return OK
	return Alx_Ok;
}
Alx_Status AlxPca9431_Reg_ReadAndClearInterrupt(AlxPca9431* me)
{
	ALX_PCA9431_ASSERT(me->isInit == true);
	ALX_PCA9431_ASSERT(me->wasCtorCalled == true);

	// #1 Prepare Variables
	uint8_t SistemIntVal = 0;
	uint8_t VRectmIntVal = 0;
	uint8_t VOutLdoIntVal = 0;
	uint8_t SistemIntVal2 = 0;
	uint8_t VRectmIntVal2 = 0;
	uint8_t VOutLdoIntVal2 = 0;

	// #2 
	Alx_Status status = AlxPca9431_Reg_Read(me, &me->reg._01h_SYSTEM_INT);
	if (status != Alx_Ok) { ALX_PCA9431_TRACE("Reg_01_SystemInt_ReadAndClear	"); return status;}
	SistemIntVal = me->reg._01h_SYSTEM_INT.val.raw;

	// #3 
	status = AlxPca9431_Reg_Read(me, &me->reg._01h_SYSTEM_INT);
	if (status != Alx_Ok) { ALX_PCA9431_TRACE("Reg_01_SystemInt_ReadAndClear	"); return status;}
	SistemIntVal2 = me->reg._01h_SYSTEM_INT.val.raw;
	AlxTrace_WriteFormat(&alxTrace, "Read 1 SistemIntVal: %lu \r\n", SistemIntVal);
	AlxTrace_WriteFormat(&alxTrace, "Read 2 SistemIntVal: %lu \r\n", SistemIntVal2);

	// #4 
	status = AlxPca9431_Reg_Read(me, &me->reg._03h_VRECT_INT);
	if (status != Alx_Ok) { ALX_PCA9431_TRACE("Reg_03_VRectInt_ReadAndClear 	"); return status;}
	VRectmIntVal = me->reg._03h_VRECT_INT.val.raw;

	// #5 
	status = AlxPca9431_Reg_Read(me, &me->reg._03h_VRECT_INT);
	if (status != Alx_Ok) { ALX_PCA9431_TRACE("Reg_03_VRectInt_ReadAndClear 	"); return status;}
	VRectmIntVal2 = me->reg._03h_VRECT_INT.val.raw;
	AlxTrace_WriteFormat(&alxTrace, "Read 1 VRectmIntVal: %lu \r\n", VRectmIntVal);
	AlxTrace_WriteFormat(&alxTrace, "Read 2 VRectmIntVal: %lu \r\n", VRectmIntVal2);

	// #6 
	status = AlxPca9431_Reg_Read(me, &me->reg._05h_VOUTLDO_INT);
	if (status != Alx_Ok) { ALX_PCA9431_TRACE("Reg_05_VOutLdoInt_ReadAndClear	"); return status;}
	VOutLdoIntVal = me->reg._05h_VOUTLDO_INT.val.raw;

	// #7 
	status = AlxPca9431_Reg_Read(me, &me->reg._05h_VOUTLDO_INT);
	if (status != Alx_Ok) { ALX_PCA9431_TRACE("Reg_05_VOutLdoInt_ReadAndClear	"); return status;}
	VOutLdoIntVal2 = me->reg._05h_VOUTLDO_INT.val.raw;
	AlxTrace_WriteFormat(&alxTrace, "Read 1 VOutLdoIntVal: %lu \r\n", VOutLdoIntVal);
	AlxTrace_WriteFormat(&alxTrace, "Read 2 VOutLdoIntVal: %lu \r\n", VOutLdoIntVal2);

	// #8 Return OK
	return Alx_Ok;
}


//******************************************************************************
// Private Functions
//******************************************************************************
static void AlxPca9431_RegStruct_SetAddr(AlxPca9431* me)
{
	me->reg._00h_Device_ID			.addr = 0x00;
	me->reg._01h_SYSTEM_INT			.addr = 0x01;
	me->reg._02h_SYSTEM_INT_MASK	.addr = 0x02;
	me->reg._03h_VRECT_INT			.addr = 0x03;
	me->reg._04h_VRECT_INT_MASK		.addr = 0x04;
	me->reg._05h_VOUTLDO_INT		.addr = 0x05;
	me->reg._06h_VOUTLDO_INT_MASK	.addr = 0x06;
	me->reg._07h_VOUT_LDO			.addr = 0x07;
	me->reg._08h_VRECT_THD			.addr = 0x08;
	me->reg._09h_VRECT_OVW			.addr = 0x09;
	me->reg._0Ah_TEMP_THD			.addr = 0x0A;
	me->reg._0Bh_WD_EN_RST			.addr = 0x0B;
	me->reg._0Ch_Varactor_DAC		.addr = 0x0C;
	me->reg._0Dh_ADC_CONTROL		.addr = 0x0D;
	me->reg._0Eh_Sample_EN			.addr = 0x0E;
	me->reg._0Fh_VPWR_CONFIG		.addr = 0x0F;
	me->reg._10h_RXIR_CONFIG		.addr = 0x10;
	me->reg._20h_OCPSET_LOCK		.addr = 0x20;
	me->reg._21h_VOUTLDO_OCP		.addr = 0x21;
	me->reg._30h_VRECT_ADC_H		.addr = 0x30;
	me->reg._31h_VRECT_ADC_L		.addr = 0x31;
	me->reg._32h_VTUNE_ADC_H		.addr = 0x32;
	me->reg._33h_VTUNE_ADC_L		.addr = 0x33;
	me->reg._34h_VOUT_ADC_H			.addr = 0x34;
	me->reg._35h_VOUT_ADC_L			.addr = 0x35;
	me->reg._36h_IOUT_ADC_H			.addr = 0x36;
	me->reg._37h_IOUT_ADC_L			.addr = 0x37;
	me->reg._38h_IRECT_ADC_H		.addr = 0x38;
	me->reg._39h_IRECT_ADC_L		.addr = 0x39;
	me->reg._3Ah_TDIE_ADC			.addr = 0x3A;
	me->reg._3Bh_NTC_ADC_H			.addr = 0x3B;
	me->reg._3Ch_NTC_ADC_L			.addr = 0x3C;
}
static void AlxPca9431_RegStruct_SetLen(AlxPca9431* me)
{
	me->reg._00h_Device_ID			.len = sizeof(me->reg._00h_Device_ID			.val);
	me->reg._01h_SYSTEM_INT			.len = sizeof(me->reg._01h_SYSTEM_INT			.val);
	me->reg._02h_SYSTEM_INT_MASK	.len = sizeof(me->reg._02h_SYSTEM_INT_MASK		.val);
	me->reg._03h_VRECT_INT			.len = sizeof(me->reg._03h_VRECT_INT			.val);
	me->reg._04h_VRECT_INT_MASK		.len = sizeof(me->reg._04h_VRECT_INT_MASK		.val);
	me->reg._05h_VOUTLDO_INT		.len = sizeof(me->reg._05h_VOUTLDO_INT			.val);
	me->reg._06h_VOUTLDO_INT_MASK	.len = sizeof(me->reg._06h_VOUTLDO_INT_MASK		.val);
	me->reg._07h_VOUT_LDO			.len = sizeof(me->reg._07h_VOUT_LDO				.val);
	me->reg._08h_VRECT_THD			.len = sizeof(me->reg._08h_VRECT_THD			.val);
	me->reg._09h_VRECT_OVW			.len = sizeof(me->reg._09h_VRECT_OVW			.val);
	me->reg._0Ah_TEMP_THD			.len = sizeof(me->reg._0Ah_TEMP_THD				.val);
	me->reg._0Bh_WD_EN_RST			.len = sizeof(me->reg._0Bh_WD_EN_RST			.val);
	me->reg._0Ch_Varactor_DAC		.len = sizeof(me->reg._0Ch_Varactor_DAC			.val);
	me->reg._0Dh_ADC_CONTROL		.len = sizeof(me->reg._0Dh_ADC_CONTROL			.val);
	me->reg._0Eh_Sample_EN			.len = sizeof(me->reg._0Eh_Sample_EN			.val);
	me->reg._0Fh_VPWR_CONFIG		.len = sizeof(me->reg._0Fh_VPWR_CONFIG			.val);
	me->reg._10h_RXIR_CONFIG		.len = sizeof(me->reg._10h_RXIR_CONFIG			.val);
	me->reg._20h_OCPSET_LOCK		.len = sizeof(me->reg._20h_OCPSET_LOCK			.val);
	me->reg._21h_VOUTLDO_OCP		.len = sizeof(me->reg._21h_VOUTLDO_OCP			.val);
	me->reg._30h_VRECT_ADC_H		.len = sizeof(me->reg._30h_VRECT_ADC_H			.val);
	me->reg._31h_VRECT_ADC_L		.len = sizeof(me->reg._31h_VRECT_ADC_L			.val);
	me->reg._32h_VTUNE_ADC_H		.len = sizeof(me->reg._32h_VTUNE_ADC_H			.val);
	me->reg._33h_VTUNE_ADC_L		.len = sizeof(me->reg._33h_VTUNE_ADC_L			.val);
	me->reg._34h_VOUT_ADC_H			.len = sizeof(me->reg._34h_VOUT_ADC_H			.val);
	me->reg._35h_VOUT_ADC_L			.len = sizeof(me->reg._35h_VOUT_ADC_L			.val);
	me->reg._36h_IOUT_ADC_H			.len = sizeof(me->reg._36h_IOUT_ADC_H			.val);
	me->reg._37h_IOUT_ADC_L			.len = sizeof(me->reg._37h_IOUT_ADC_L			.val);
	me->reg._38h_IRECT_ADC_H		.len = sizeof(me->reg._38h_IRECT_ADC_H			.val);
	me->reg._39h_IRECT_ADC_L		.len = sizeof(me->reg._39h_IRECT_ADC_L			.val);
	me->reg._3Ah_TDIE_ADC			.len = sizeof(me->reg._3Ah_TDIE_ADC				.val);
	me->reg._3Bh_NTC_ADC_H			.len = sizeof(me->reg._3Bh_NTC_ADC_H			.val);
	me->reg._3Ch_NTC_ADC_L			.len = sizeof(me->reg._3Ch_NTC_ADC_L			.val);
}
static void AlxPca9431_RegStruct_SetValToZero(AlxPca9431* me)
{
	me->reg._00h_Device_ID			.val.raw = 0b00000000;
	me->reg._01h_SYSTEM_INT			.val.raw = 0b00000000;
	me->reg._02h_SYSTEM_INT_MASK	.val.raw = 0b00000000;
	me->reg._03h_VRECT_INT			.val.raw = 0b00000000;
	me->reg._04h_VRECT_INT_MASK		.val.raw = 0b00000000;
	me->reg._05h_VOUTLDO_INT		.val.raw = 0b00000000;
	me->reg._06h_VOUTLDO_INT_MASK	.val.raw = 0b00000000;
	me->reg._07h_VOUT_LDO			.val.raw = 0b00000000;
	me->reg._08h_VRECT_THD			.val.raw = 0b00000000;
	me->reg._09h_VRECT_OVW			.val.raw = 0b00000000;
	me->reg._0Ah_TEMP_THD			.val.raw = 0b00000000;
	me->reg._0Bh_WD_EN_RST			.val.raw = 0b00000000;
	me->reg._0Ch_Varactor_DAC		.val.raw = 0b00000000;
	me->reg._0Dh_ADC_CONTROL		.val.raw = 0b00000000;
	me->reg._0Eh_Sample_EN			.val.raw = 0b00000000;
	me->reg._0Fh_VPWR_CONFIG		.val.raw = 0b00000000;
	me->reg._10h_RXIR_CONFIG		.val.raw = 0b00000000;
	me->reg._20h_OCPSET_LOCK		.val.raw = 0b00000000;
	me->reg._21h_VOUTLDO_OCP		.val.raw = 0b00000000;
	me->reg._30h_VRECT_ADC_H		.val.raw = 0b00000000;
	me->reg._31h_VRECT_ADC_L		.val.raw = 0b00000000;
	me->reg._32h_VTUNE_ADC_H		.val.raw = 0b00000000;
	me->reg._33h_VTUNE_ADC_L		.val.raw = 0b00000000;
	me->reg._34h_VOUT_ADC_H			.val.raw = 0b00000000;
	me->reg._35h_VOUT_ADC_L			.val.raw = 0b00000000;
	me->reg._36h_IOUT_ADC_H			.val.raw = 0b00000000;
	me->reg._37h_IOUT_ADC_L			.val.raw = 0b00000000;
	me->reg._38h_IRECT_ADC_H		.val.raw = 0b00000000;
	me->reg._39h_IRECT_ADC_L		.val.raw = 0b00000000;
	me->reg._3Ah_TDIE_ADC			.val.raw = 0b00000000;
	me->reg._3Bh_NTC_ADC_H			.val.raw = 0b00000000;
	me->reg._3Ch_NTC_ADC_L			.val.raw = 0b00000000;
}
static void AlxPca9431_RegStruct_SetValToDefault(AlxPca9431* me)
{
	me->reg._00h_Device_ID			.val.raw = 0b00011000;
	me->reg._01h_SYSTEM_INT			.val.raw = 0b00000000;
	me->reg._02h_SYSTEM_INT_MASK	.val.raw = 0b00000000;
	me->reg._03h_VRECT_INT			.val.raw = 0b00000000;
	me->reg._04h_VRECT_INT_MASK		.val.raw = 0b00000000;
	me->reg._05h_VOUTLDO_INT		.val.raw = 0b00000000;
	me->reg._06h_VOUTLDO_INT_MASK	.val.raw = 0b00000000;
	me->reg._07h_VOUT_LDO			.val.raw = 0b10000000;
	me->reg._08h_VRECT_THD			.val.raw = 0b00000000;
	me->reg._09h_VRECT_OVW			.val.raw = 0b00010000;
	me->reg._0Ah_TEMP_THD			.val.raw = 0b00000000;
	me->reg._0Bh_WD_EN_RST			.val.raw = 0b00000010;
	me->reg._0Ch_Varactor_DAC		.val.raw = 0b00000000;
	me->reg._0Dh_ADC_CONTROL		.val.raw = 0b00000110;
	me->reg._0Eh_Sample_EN			.val.raw = 0b01111111;
	me->reg._0Fh_VPWR_CONFIG		.val.raw = 0b10000011;
	me->reg._10h_RXIR_CONFIG		.val.raw = 0b00000000;
	me->reg._20h_OCPSET_LOCK		.val.raw = 0b00001000;
	me->reg._21h_VOUTLDO_OCP		.val.raw = 0b00001000;
	me->reg._30h_VRECT_ADC_H		.val.raw = 0b00000000;
	me->reg._31h_VRECT_ADC_L		.val.raw = 0b00000000;
	me->reg._32h_VTUNE_ADC_H		.val.raw = 0b00000000;
	me->reg._33h_VTUNE_ADC_L		.val.raw = 0b00000000;
	me->reg._34h_VOUT_ADC_H			.val.raw = 0b00000000;
	me->reg._35h_VOUT_ADC_L			.val.raw = 0b00000000;
	me->reg._36h_IOUT_ADC_H			.val.raw = 0b00000000;
	me->reg._37h_IOUT_ADC_L			.val.raw = 0b00000000;
	me->reg._38h_IRECT_ADC_H		.val.raw = 0b00000000;
	me->reg._39h_IRECT_ADC_L		.val.raw = 0b00000000;
	me->reg._3Ah_TDIE_ADC			.val.raw = 0b00000000;
	me->reg._3Bh_NTC_ADC_H			.val.raw = 0b00000000;
	me->reg._3Ch_NTC_ADC_L			.val.raw = 0b00000000;
}
static Alx_Status AlxPca9431_Reg_Write(AlxPca9431* me, void* reg)
{
	uint8_t regAddr = *((uint8_t*)reg);
	uint8_t regLen = *((uint8_t*)reg + sizeof(regAddr));
	uint8_t* regValPtr = (uint8_t*)reg + sizeof(regAddr) + sizeof(regLen);

	return AlxI2c_Master_StartWriteMemStop_Multi(me->i2c, me->i2cAddr, regAddr, AlxI2c_Master_MemAddrLen_8bit, regValPtr, regLen, me->i2cCheckWithRead, me->i2cNumOfTries, me->i2cTimeout_ms);
}
static Alx_Status AlxPca9431_Reg_Read(AlxPca9431* me, void* reg)
{
	uint8_t regAddr = *((uint8_t*)reg);
	uint8_t regLen = *((uint8_t*)reg + sizeof(regAddr));
	uint8_t* regValPtr = (uint8_t*)reg + sizeof(regAddr) + sizeof(regLen);

	return AlxI2c_Master_StartReadMemStop(me->i2c, me->i2cAddr, regAddr, AlxI2c_Master_MemAddrLen_8bit, regValPtr, regLen, me->i2cNumOfTries, me->i2cTimeout_ms);
}
///************************************************************************************************************************************************************************************************************** SVINJARJENJE */
//static Alx_Status AlxPca9431_Reg_ReadMultiAdc(AlxPca9431* me, void* reg)
//{
//	uint16_t regAddr = *((uint8_t*)reg);
//	uint8_t regLen = *((uint8_t*)reg + sizeof(regAddr));
//	uint8_t* regValPtr = (uint8_t*)reg + sizeof(regAddr) + sizeof(regLen);
//
//	return AlxI2c_Master_StartReadMemStop(me->i2c, me->i2cAddr, regAddr, AlxI2c_Master_MemAddrLen_8bit, regValPtr, regLen, me->i2cNumOfTries, me->i2cTimeout_ms);
//}
///************************************************************************************************************************************************************************************************************** SVINJARJENJE */
static Alx_Status AlxPca9431_Reg_WriteVal(AlxPca9431* me)
{
	Alx_Status status = AlxPca9431_Reg_Write(me, &me->reg._02h_SYSTEM_INT_MASK);
	if (status != Alx_Ok) { ALX_PCA9431_TRACE("Err_02_SYSTEM_INT_MASK		"); return status;}
	
	status = AlxPca9431_Reg_Write(me, &me->reg._04h_VRECT_INT_MASK		);
	if (status != Alx_Ok) { ALX_PCA9431_TRACE("Err_04_VRECT_INT_MASK		"); return status;}
	
	status = AlxPca9431_Reg_Write(me, &me->reg._06h_VOUTLDO_INT_MASK	);
	if (status != Alx_Ok) { ALX_PCA9431_TRACE("Err_06_VOUTLDO_INT_MASK		"); return status;}
	
	status = AlxPca9431_Reg_Write(me, &me->reg._07h_VOUT_LDO			);
	if (status != Alx_Ok) { ALX_PCA9431_TRACE("Err_07_VOUT_LDO				"); return status;}
	
	status = AlxPca9431_Reg_Write(me, &me->reg._08h_VRECT_THD			);
	if (status != Alx_Ok) { ALX_PCA9431_TRACE("Err_08_VRECT_THD				"); return status;}
	
	status = AlxPca9431_Reg_Write(me, &me->reg._09h_VRECT_OVW			);
	if (status != Alx_Ok) { ALX_PCA9431_TRACE("Err_09_VRECT_OVW				"); return status;}
	
	status = AlxPca9431_Reg_Write(me, &me->reg._0Ah_TEMP_THD			);
	if (status != Alx_Ok) { ALX_PCA9431_TRACE("Err_0A_TEMP_THD				"); return status;}
	
	status = AlxPca9431_Reg_Write(me, &me->reg._0Bh_WD_EN_RST			);
	if (status != Alx_Ok) { ALX_PCA9431_TRACE("Err_0B_WD_EN_RST				"); return status;}
	
	status = AlxPca9431_Reg_Write(me, &me->reg._0Ch_Varactor_DAC		);
	if (status != Alx_Ok) { ALX_PCA9431_TRACE("Err_0C_Varactor_DAC			"); return status;}
	
	status = AlxPca9431_Reg_Write(me, &me->reg._0Dh_ADC_CONTROL			);
	if (status != Alx_Ok) { ALX_PCA9431_TRACE("Err_0D_ADC_CONTROL			"); return status;}
	
	status = AlxPca9431_Reg_Write(me, &me->reg._0Eh_Sample_EN			);
	if (status != Alx_Ok) { ALX_PCA9431_TRACE("Err_0E_Sample_EN				"); return status;}
	
	status = AlxPca9431_Reg_Write(me, &me->reg._0Fh_VPWR_CONFIG			);
	if (status != Alx_Ok) { ALX_PCA9431_TRACE("Err_0F_VPWR_CONFIG			"); return status;}
	
	status = AlxPca9431_Reg_Write(me, &me->reg._10h_RXIR_CONFIG			);
	if (status != Alx_Ok) { ALX_PCA9431_TRACE("Err_10_RXIR_CONFIG			"); return status;}
	
	status = AlxPca9431_Reg_Write(me, &me->reg._20h_OCPSET_LOCK			);
	if (status != Alx_Ok) { ALX_PCA9431_TRACE("Err_20_OCPSET_LOCK			"); return status;}
	
	status = AlxPca9431_Reg_Write(me, &me->reg._21h_VOUTLDO_OCP			);
	if (status != Alx_Ok) { ALX_PCA9431_TRACE("Err_21_VOUTLDO_OCP			"); return status;}

	return Alx_Ok;
}


//******************************************************************************
// Weak Functions
//******************************************************************************
ALX_WEAK void AlxPca9431_RegStruct_SetVal(AlxPca9431* me)
{
	(void)me;
	ALX_PCA9431_TRACE("Define 'AlxPca9431_RegStruct_SetVal' function in your application.");
	ALX_PCA9431_ASSERT(false);
}
