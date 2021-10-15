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
#include "alxPca9430.h"

//******************************************************************************
// Private Functions
//******************************************************************************
static void AlxPca9430_RegStruct_SetAddr(AlxPca9430* me);
static void AlxPca9430_RegStruct_SetLen(AlxPca9430* me);
static void AlxPca9430_RegStruct_SetValToZero(AlxPca9430* me);
static void AlxPca9430_RegStruct_SetValToDefault(AlxPca9430* me);
static Alx_Status AlxPca9430_Reg_Write(AlxPca9430* me, void* reg);
static Alx_Status AlxPca9430_Reg_Read(AlxPca9430* me, void* reg);
static Alx_Status AlxPca9430_Reg_WriteVal(AlxPca9430* me);


//******************************************************************************
// Weak Functions
//******************************************************************************
void AlxPca9430_RegStruct_SetVal(AlxPca9430* me);

//******************************************************************************
// Constructor
//******************************************************************************
void AlxPca9430_Ctor
(
	AlxPca9430* me,
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
	AlxPca9430_RegStruct_SetAddr(me);
	AlxPca9430_RegStruct_SetLen(me);
	AlxPca9430_RegStruct_SetValToZero(me);

	// Info
	me->isInit = false;
	me->wasCtorCalled = true;
}


//******************************************************************************
// Functions
//******************************************************************************
Alx_Status AlxPca9430_Init(AlxPca9430* me)
{
	ALX_PCA9430_ASSERT(me->isInit == false);
	ALX_PCA9430_ASSERT(me->wasCtorCalled == true);

	Alx_Status status = Alx_Err;
	
	// #1 Init GPIO
	AlxIoPin_Init(me->do_SleepEn);
	AlxIoPin_Init(me->di_Interrupt);

	// #2 Init I2C
	status = AlxI2c_Init(me->i2c);
	if (status != Alx_Ok) { ALX_PCA9430_TRACE("Err_AlxI2c_Init"); return status; }

//	// #3 Check if slave ready
//	status = AlxI2c_Master_IsSlaveReady(me->i2c, me->i2cAddr, 1, 1000);
//	if (status != Alx_Ok) { ALX_PCA9430_TRACE("Err_AlxI2c_IsSlaveReady"); return status; }
	
	// #4 Set registers values to default
	 AlxPca9430_RegStruct_SetValToDefault(me);
	
	// #6 Set registers values - WEAK
	AlxPca9430_RegStruct_SetVal(me);
	
	// #7 Write registers
	status = AlxPca9430_Reg_WriteVal(me);
	if (status != Alx_Ok) { ALX_PCA9430_TRACE("Err_Reg_WriteVal"); return status;}

	// #9 Set isInit
	me->isInit = true;

	// #10 Return OK
	return Alx_Ok;
}
Alx_Status AlxPca9430_DeInit(AlxPca9430* me)
{
	ALX_PCA9430_ASSERT(me->isInit == true);
	ALX_PCA9430_ASSERT(me->wasCtorCalled == true);

	// #1 DeInit GPIO
	AlxIoPin_DeInit(me->do_SleepEn);
	AlxIoPin_DeInit(me->di_Interrupt);

	// #2 Reset isInit
	me->isInit = false;
}

Alx_Status AlxPca9430_ChargerVout_GetVoltage_V(AlxPca9430* me, float* voltage_V) // 10 bit ADC
{ 
	ALX_PCA9430_ASSERT(me->isInit == true);
	ALX_PCA9430_ASSERT(me->wasCtorCalled == true);
	uint8_t AdcHBitVoltage = 0;
	uint8_t AdcLBitVoltage = 0;
	uint16_t AdcBitVoltage = 0;
	
	Alx_Status status = AlxPca9430_Reg_Read(me, &me->reg._0x34_VOUT_ADC_H);
	if (status != Alx_Ok) { ALX_PCA9430_TRACE("Err"); return status; }
	AdcHBitVoltage = me->reg._0x34_VOUT_ADC_H.val.raw;
	AdcBitVoltage = (uint16_t) AdcHBitVoltage;
	AdcBitVoltage = AdcBitVoltage << 2;
	
	status = AlxPca9430_Reg_Read(me, &me->reg._0x35_VOUT_ADC_L);
	if (status != Alx_Ok) { ALX_PCA9430_TRACE("Err"); return status; }
	AdcLBitVoltage = me->reg._0x35_VOUT_ADC_L.val.raw;
	AdcLBitVoltage = AdcLBitVoltage >> 6;
	
	AdcBitVoltage += AdcLBitVoltage;
	*voltage_V = AdcBitVoltage*5.27/1000;

	// #10 Return OK
	return Alx_Ok;
}

Alx_Status AlxPca9430_ChargerVout_GetCurrent_A(AlxPca9430* me, float* current_A) // 10 bit ADC
{
	ALX_PCA9430_ASSERT(me->isInit == true);
	ALX_PCA9430_ASSERT(me->wasCtorCalled == true);
	uint8_t AdcHBitCurrent = 0;
	uint8_t AdcLBitCurrent = 0;
	uint16_t AdcBitCurrent = 0;
	
	Alx_Status status = AlxPca9430_Reg_Read(me, &me->reg._0x36_IOUT_ADC_H);
	if (status != Alx_Ok) { ALX_PCA9430_TRACE("Err"); return status; }
	AdcHBitCurrent = me->reg._0x36_IOUT_ADC_H.val.raw;
	AdcBitCurrent = (uint16_t) AdcHBitCurrent;
	AdcBitCurrent = AdcBitCurrent << 2;
	
	status = AlxPca9430_Reg_Read(me, &me->reg._0x37_IOUT_ADC_L);
	if (status != Alx_Ok) { ALX_PCA9430_TRACE("Err"); return status; }
	AdcLBitCurrent = me->reg._0x37_IOUT_ADC_L.val.raw;
	AdcLBitCurrent = AdcLBitCurrent >> 6;
	
	AdcBitCurrent += AdcLBitCurrent;
	*current_A = AdcBitCurrent * 421.9 / 1000000;
	
	// #10 Return OK
	return Alx_Ok;
}

Alx_Status AlxPca9430_Rect_GetVoltage_V(AlxPca9430* me, float* voltage_V) // 10 bit ADC
{
	ALX_PCA9430_ASSERT(me->isInit == true);
	ALX_PCA9430_ASSERT(me->wasCtorCalled == true);
	uint8_t AdcHBitVoltage = 0;
	uint8_t AdcLBitVoltage = 0;
	uint16_t AdcBitVoltage = 0;
	
	Alx_Status status = AlxPca9430_Reg_Read(me, &me->reg._0x30_VRECT_ADC_H);
	if (status != Alx_Ok) { ALX_PCA9430_TRACE("Err"); return status; }
	AdcHBitVoltage = me->reg._0x30_VRECT_ADC_H.val.raw;
	AdcBitVoltage = (uint16_t) AdcHBitVoltage;
	AdcBitVoltage = AdcBitVoltage << 2;
	
	status = AlxPca9430_Reg_Read(me, &me->reg._0x31_VRECT_ADC_L);
	if (status != Alx_Ok) { ALX_PCA9430_TRACE("Err"); return status; }
	AdcLBitVoltage = me->reg._0x31_VRECT_ADC_L.val.raw;
	AdcLBitVoltage = AdcLBitVoltage >> 6;

	AdcBitVoltage += AdcLBitVoltage;
	AlxTrace_WriteFormat(&alxTrace, "Test Adc_AdcBitVoltage: %lu \r\n", AdcBitVoltage);
	*voltage_V = AdcBitVoltage * 15.82 / 1000;;
	
	// #10 Return OK
	return Alx_Ok;
}

Alx_Status AlxPca9430_Rect_GetCurrent_A(AlxPca9430* me, float* current_A) // 10 bit ADC
{
	ALX_PCA9430_ASSERT(me->isInit == true);
	ALX_PCA9430_ASSERT(me->wasCtorCalled == true);
	uint8_t AdcHBitCurrent = 0;
	uint8_t AdcLBitCurrent = 0;
	uint16_t AdcBitCurrent = 0;
	
	Alx_Status status = AlxPca9430_Reg_Read(me, &me->reg._0x38_IRECT_ADC_H);
	if (status != Alx_Ok) { ALX_PCA9430_TRACE("Err"); return status; }
	AdcHBitCurrent = me->reg._0x38_IRECT_ADC_H.val.raw;
	AdcBitCurrent = (uint16_t) AdcHBitCurrent;
	AdcBitCurrent = AdcBitCurrent << 2;
	
	status = AlxPca9430_Reg_Read(me, &me->reg._0x39_IRECT_ADC_L);
	if (status != Alx_Ok) { ALX_PCA9430_TRACE("Err"); return status; }
	AdcLBitCurrent = me->reg._0x39_IRECT_ADC_L.val.raw;
	AdcLBitCurrent = AdcLBitCurrent >> 6;

	AdcBitCurrent += AdcLBitCurrent;
	*current_A = AdcBitCurrent * 421.9 / 1000000;
	
	// #10 Return OK
	return Alx_Ok;
}

Alx_Status AlxPca9430_TempSens_GetTemp_degC(AlxPca9430* me, float* temp_degC)
{
	uint8_t BitTemp= 0;
	ALX_PCA9430_ASSERT(me->isInit == true);
	ALX_PCA9430_ASSERT(me->wasCtorCalled == true);
	Alx_Status status = AlxPca9430_Reg_Read(me, &me->reg._0x3A_TDIE_ADC);
	if (status != Alx_Ok) { ALX_PCA9430_TRACE("Err"); return status; }
	BitTemp = me->reg._0x3A_TDIE_ADC.val.raw;
	
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
	default:	*temp_degC = 9999.9	; ALX_PCA9430_ASSERT(false); return Alx_Err;	break;
	}
}

Alx_Status AlxPca9430_VTune_SetVoltage_V(AlxPca9430* me, float* voltage_V) // 0-3.3V 5 bit DAC - TODO
{
	// TODO
	ALX_PCA9430_ASSERT(false);
	*voltage_V = 0;
	return Alx_Err;
}

Alx_Status AlxPca9430_VTune_GetVoltage_V(AlxPca9430* me, float* voltage_V) // 10 bit ADC - TODO
{
	ALX_PCA9430_ASSERT(me->isInit == true);
	ALX_PCA9430_ASSERT(me->wasCtorCalled == true);
	uint8_t AdcHBitVoltage = 0;
	uint8_t AdcLBitVoltage = 0;
	uint16_t AdcBitVoltage = 0;
	
	Alx_Status status = AlxPca9430_Reg_Read(me, &me->reg._0x32_VTUNE_ADC_H);
	if (status != Alx_Ok) { ALX_PCA9430_TRACE("Err"); return status; }
	AdcHBitVoltage = me->reg._0x32_VTUNE_ADC_H.val.raw;
	AdcBitVoltage = (uint16_t) AdcHBitVoltage;
	AdcBitVoltage = AdcBitVoltage << 2;
	
	status = AlxPca9430_Reg_Read(me, &me->reg._0x33_VTUNE_ADC_L);
	if (status != Alx_Ok) { ALX_PCA9430_TRACE("Err"); return status; }
	AdcLBitVoltage = me->reg._0x33_VTUNE_ADC_L.val.raw;
	AdcLBitVoltage = AdcLBitVoltage >> 6;
	
	AdcBitVoltage += AdcLBitVoltage;
		*voltage_V = AdcBitVoltage * 3.32 / 1000;
	
	// #10 Return OK
	return Alx_Ok;
}
Alx_Status AlxPca9430_Reg_ReadAndClearInterrupt(AlxPca9430* me)
{
	uint8_t SistemIntVal = 0;
	uint8_t VRectmIntVal = 0;
	uint8_t VOutChargerIntVal = 0;
	uint8_t SistemIntVal2 = 0;
	uint8_t VRectmIntVal2 = 0;
	uint8_t VOutChargerIntVal2 = 0;
	Alx_Status 
	status = AlxPca9430_Reg_Read(me, &me->reg._0x01_SYSTEM_INT);
	if (status != Alx_Ok) { ALX_PCA9430_TRACE("Reg_0x01_SystemInt_ReadAndClear	"); return status;}
	SistemIntVal = me->reg._0x01_SYSTEM_INT.val.raw;
	
	status = AlxPca9430_Reg_Read(me, &me->reg._0x01_SYSTEM_INT);
	if (status != Alx_Ok) { ALX_PCA9430_TRACE("Reg_0x01_SystemInt_ReadAndClear	"); return status;}
	SistemIntVal2 = me->reg._0x01_SYSTEM_INT.val.raw;
	AlxTrace_WriteFormat(&alxTrace, "Read 1 SistemIntVal: %lu \r\n", SistemIntVal);
	AlxTrace_WriteFormat(&alxTrace, "Read 2 SistemIntVal: %lu \r\n", SistemIntVal2);
	
	
	
	status = AlxPca9430_Reg_Read(me, &me->reg._0x03_VRECT_INT);
	if (status != Alx_Ok) { ALX_PCA9430_TRACE("Reg_0x03_VRectInt_ReadAndClear 	"); return status;}
	VRectmIntVal = me->reg._0x03_VRECT_INT.val.raw;
	
	status = AlxPca9430_Reg_Read(me, &me->reg._0x03_VRECT_INT);
	if (status != Alx_Ok) { ALX_PCA9430_TRACE("Reg_0x03_VRectInt_ReadAndClear 	"); return status;}
	VRectmIntVal2 = me->reg._0x03_VRECT_INT.val.raw;
	AlxTrace_WriteFormat(&alxTrace, "Read 1 VRectmIntVal: %lu \r\n", VRectmIntVal);
	AlxTrace_WriteFormat(&alxTrace, "Read 2 VRectmIntVal: %lu \r\n", VRectmIntVal2);
	
	
	status = AlxPca9430_Reg_Read(me, &me->reg._0x05_BATCHG_INT);
	if (status != Alx_Ok) { ALX_PCA9430_TRACE("Reg_0x05_VOutLdoInt_ReadAndClear	"); return status;}
	VOutChargerIntVal = me->reg._0x05_BATCHG_INT.val.raw;
	
	status = AlxPca9430_Reg_Read(me, &me->reg._0x05_BATCHG_INT);
	if (status != Alx_Ok) { ALX_PCA9430_TRACE("Reg_0x05_VOutLdoInt_ReadAndClear	"); return status;}
	VOutChargerIntVal2 = me->reg._0x05_BATCHG_INT.val.raw;
	AlxTrace_WriteFormat(&alxTrace, "Read 1 VOutLdoIntVal: %lu \r\n", VOutChargerIntVal);
	AlxTrace_WriteFormat(&alxTrace, "Read 2 VOutLdoIntVal: %lu \r\n", VOutChargerIntVal2);
	
	return Alx_Ok;
}
//******************************************************************************
// Private Functions
//******************************************************************************
static void AlxPca9430_RegStruct_SetAddr(AlxPca9430* me)
{
	me->reg._0x00_Device_ID						.addr =	0x00;
	me->reg._0x01_SYSTEM_INT					.addr =	0x01;
	me->reg._0x02_SYSTEM_INT_MASK				.addr =	0x02;
	me->reg._0x03_VRECT_INT						.addr =	0x03;
	me->reg._0x04_VRECT_INT_MASK				.addr =	0x04;
	me->reg._0x05_BATCHG_INT					.addr =	0x05;
	me->reg._0x06_BATCHG_INT_MASK				.addr =	0x06;
	me->reg._0x08_VRECT_THD						.addr =	0x08;
	me->reg._0x09_VRECT_OVW						.addr =	0x09;
	me->reg._0x0A_TEMP_THD						.addr =	0x0A;
	me->reg._0x0B_WD_EN_RST						.addr =	0x0B;
	me->reg._0x0C_Varactor_DAC					.addr =	0x0C;
	me->reg._0x0D_ADC_CONTROL					.addr =	0x0D;
	me->reg._0x0E_Sample_EN						.addr =	0x0E;
	me->reg._0x0F_VPWR_CONFIG					.addr =	0x0F;
	me->reg._0x10_RXIR_CONFIG					.addr =	0x10;
	me->reg._0x20_BQ_CONTROL					.addr =	0x20;
	me->reg._0x21_CC_CURRENT					.addr =	0x21;
	me->reg._0x22_TP_CURRENT					.addr =	0x22;
	me->reg._0x23_LB_CURRENT					.addr =	0x23;
	me->reg._0x24_DB_CURRENT					.addr =	0x24;
	me->reg._0x25_VBAT_REG						.addr =	0x25;
	me->reg._0x26_BQTIMER_AND_NTC				.addr =	0x26;
	me->reg._0x27_BQTHM_AND_SHRT				.addr =	0x27;
	me->reg._0x28_Batterychargerstatus1			.addr =	0x28;
	me->reg._0x29_Batterychargerstatus2			.addr =	0x29;
	me->reg._0x2A_Batterychargerstatus3			.addr =	0x2A;
	me->reg._0x2B_Batterychargerstatus4			.addr =	0x2B;
	me->reg._0x30_VRECT_ADC_H					.addr =	0x30;
	me->reg._0x31_VRECT_ADC_L					.addr =	0x31;
	me->reg._0x32_VTUNE_ADC_H					.addr =	0x32;
	me->reg._0x33_VTUNE_ADC_L					.addr =	0x33;
	me->reg._0x34_VOUT_ADC_H					.addr =	0x34;
	me->reg._0x35_VOUT_ADC_L					.addr =	0x35;
	me->reg._0x36_IOUT_ADC_H					.addr =	0x36;
	me->reg._0x37_IOUT_ADC_L					.addr =	0x37;
	me->reg._0x38_IRECT_ADC_H					.addr =	0x38;
	me->reg._0x39_IRECT_ADC_L					.addr =	0x39;
	me->reg._0x3A_TDIE_ADC						.addr =	0x3A;
	me->reg._0x3B_NTC_ADC_H						.addr =	0x3B;
	me->reg._0x3C_NTC_ADC_L						.addr =	0x3C;
}

static void AlxPca9430_RegStruct_SetLen(AlxPca9430* me)
{
	me->reg._0x00_Device_ID				.len = sizeof(me->reg._0x00_Device_ID				.val);
	me->reg._0x01_SYSTEM_INT			.len = sizeof(me->reg._0x01_SYSTEM_INT				.val);
	me->reg._0x02_SYSTEM_INT_MASK		.len = sizeof(me->reg._0x02_SYSTEM_INT_MASK			.val);
	me->reg._0x03_VRECT_INT				.len = sizeof(me->reg._0x03_VRECT_INT				.val);
	me->reg._0x04_VRECT_INT_MASK		.len = sizeof(me->reg._0x04_VRECT_INT_MASK			.val);
	me->reg._0x05_BATCHG_INT			.len = sizeof(me->reg._0x05_BATCHG_INT				.val);
	me->reg._0x06_BATCHG_INT_MASK		.len = sizeof(me->reg._0x06_BATCHG_INT_MASK			.val);
	me->reg._0x08_VRECT_THD				.len = sizeof(me->reg._0x08_VRECT_THD				.val);
	me->reg._0x09_VRECT_OVW				.len = sizeof(me->reg._0x09_VRECT_OVW				.val);
	me->reg._0x0A_TEMP_THD				.len = sizeof(me->reg._0x0A_TEMP_THD				.val);
	me->reg._0x0B_WD_EN_RST				.len = sizeof(me->reg._0x0B_WD_EN_RST				.val);
	me->reg._0x0C_Varactor_DAC			.len = sizeof(me->reg._0x0C_Varactor_DAC			.val);
	me->reg._0x0D_ADC_CONTROL			.len = sizeof(me->reg._0x0D_ADC_CONTROL				.val);
	me->reg._0x0E_Sample_EN				.len = sizeof(me->reg._0x0E_Sample_EN				.val);
	me->reg._0x0F_VPWR_CONFIG			.len = sizeof(me->reg._0x0F_VPWR_CONFIG				.val);
	me->reg._0x10_RXIR_CONFIG			.len = sizeof(me->reg._0x10_RXIR_CONFIG				.val);
	me->reg._0x20_BQ_CONTROL			.len = sizeof(me->reg._0x20_BQ_CONTROL				.val);
	me->reg._0x21_CC_CURRENT			.len = sizeof(me->reg._0x21_CC_CURRENT				.val);
	me->reg._0x22_TP_CURRENT			.len = sizeof(me->reg._0x22_TP_CURRENT				.val);
	me->reg._0x23_LB_CURRENT			.len = sizeof(me->reg._0x23_LB_CURRENT				.val);
	me->reg._0x24_DB_CURRENT			.len = sizeof(me->reg._0x24_DB_CURRENT				.val);
	me->reg._0x25_VBAT_REG				.len = sizeof(me->reg._0x25_VBAT_REG				.val);
	me->reg._0x26_BQTIMER_AND_NTC		.len = sizeof(me->reg._0x26_BQTIMER_AND_NTC			.val);
	me->reg._0x27_BQTHM_AND_SHRT		.len = sizeof(me->reg._0x27_BQTHM_AND_SHRT			.val);
	me->reg._0x28_Batterychargerstatus1	.len = sizeof(me->reg._0x28_Batterychargerstatus1	.val);
	me->reg._0x29_Batterychargerstatus2	.len = sizeof(me->reg._0x29_Batterychargerstatus2	.val);
	me->reg._0x2A_Batterychargerstatus3	.len = sizeof(me->reg._0x2A_Batterychargerstatus3	.val);
	me->reg._0x2B_Batterychargerstatus4	.len = sizeof(me->reg._0x2B_Batterychargerstatus4	.val);
	me->reg._0x30_VRECT_ADC_H			.len = sizeof(me->reg._0x30_VRECT_ADC_H				.val);
	me->reg._0x31_VRECT_ADC_L			.len = sizeof(me->reg._0x31_VRECT_ADC_L				.val);
	me->reg._0x32_VTUNE_ADC_H			.len = sizeof(me->reg._0x32_VTUNE_ADC_H				.val);
	me->reg._0x33_VTUNE_ADC_L			.len = sizeof(me->reg._0x33_VTUNE_ADC_L				.val);
	me->reg._0x34_VOUT_ADC_H			.len = sizeof(me->reg._0x34_VOUT_ADC_H				.val);
	me->reg._0x35_VOUT_ADC_L			.len = sizeof(me->reg._0x35_VOUT_ADC_L				.val);
	me->reg._0x36_IOUT_ADC_H			.len = sizeof(me->reg._0x36_IOUT_ADC_H				.val);
	me->reg._0x37_IOUT_ADC_L			.len = sizeof(me->reg._0x37_IOUT_ADC_L				.val);
	me->reg._0x38_IRECT_ADC_H			.len = sizeof(me->reg._0x38_IRECT_ADC_H				.val);
	me->reg._0x39_IRECT_ADC_L			.len = sizeof(me->reg._0x39_IRECT_ADC_L				.val);
	me->reg._0x3A_TDIE_ADC				.len = sizeof(me->reg._0x3A_TDIE_ADC				.val);
	me->reg._0x3B_NTC_ADC_H				.len = sizeof(me->reg._0x3B_NTC_ADC_H				.val);
	me->reg._0x3C_NTC_ADC_L				.len = sizeof(me->reg._0x3C_NTC_ADC_L				.val);
}

static void AlxPca9430_RegStruct_SetValToZero(AlxPca9430* me)
{
	me->reg._0x00_Device_ID					.val.raw	= 0b00000000;
	me->reg._0x01_SYSTEM_INT				.val.raw	= 0b00000000;
	me->reg._0x02_SYSTEM_INT_MASK			.val.raw	= 0b00000000;
	me->reg._0x03_VRECT_INT					.val.raw	= 0b00000000;
	me->reg._0x04_VRECT_INT_MASK			.val.raw	= 0b00000000;
	me->reg._0x05_BATCHG_INT				.val.raw	= 0b00000000;
	me->reg._0x06_BATCHG_INT_MASK			.val.raw	= 0b00000000;
	me->reg._0x08_VRECT_THD					.val.raw	= 0b00000000;
	me->reg._0x09_VRECT_OVW					.val.raw	= 0b00000000;
	me->reg._0x0A_TEMP_THD					.val.raw	= 0b00000000;
	me->reg._0x0B_WD_EN_RST					.val.raw	= 0b00000000;
	me->reg._0x0C_Varactor_DAC				.val.raw	= 0b00000000;
	me->reg._0x0D_ADC_CONTROL				.val.raw	= 0b00000000;
	me->reg._0x0E_Sample_EN					.val.raw	= 0b00000000;
	me->reg._0x0F_VPWR_CONFIG				.val.raw	= 0b00000000;
	me->reg._0x10_RXIR_CONFIG				.val.raw	= 0b00000000;
	me->reg._0x20_BQ_CONTROL				.val.raw	= 0b00000000;
	me->reg._0x21_CC_CURRENT				.val.raw	= 0b00000000;
	me->reg._0x22_TP_CURRENT				.val.raw	= 0b00000000;
	me->reg._0x23_LB_CURRENT				.val.raw	= 0b00000000;
	me->reg._0x24_DB_CURRENT				.val.raw	= 0b00000000;
	me->reg._0x25_VBAT_REG					.val.raw	= 0b00000000;
	me->reg._0x26_BQTIMER_AND_NTC			.val.raw	= 0b00000000;
	me->reg._0x27_BQTHM_AND_SHRT			.val.raw	= 0b00000000;
	me->reg._0x28_Batterychargerstatus1		.val.raw	= 0b00000000;
	me->reg._0x29_Batterychargerstatus2		.val.raw	= 0b00000000;
	me->reg._0x2A_Batterychargerstatus3		.val.raw	= 0b00000000;
	me->reg._0x2B_Batterychargerstatus4		.val.raw	= 0b00000000;
	me->reg._0x30_VRECT_ADC_H				.val.raw	= 0b00000000;
	me->reg._0x31_VRECT_ADC_L				.val.raw	= 0b00000000;
	me->reg._0x32_VTUNE_ADC_H				.val.raw	= 0b00000000;
	me->reg._0x33_VTUNE_ADC_L				.val.raw	= 0b00000000;
	me->reg._0x34_VOUT_ADC_H				.val.raw	= 0b00000000;
	me->reg._0x35_VOUT_ADC_L				.val.raw	= 0b00000000;
	me->reg._0x36_IOUT_ADC_H				.val.raw	= 0b00000000;
	me->reg._0x37_IOUT_ADC_L				.val.raw	= 0b00000000;
	me->reg._0x38_IRECT_ADC_H				.val.raw	= 0b00000000;
	me->reg._0x39_IRECT_ADC_L				.val.raw	= 0b00000000;
	me->reg._0x3A_TDIE_ADC					.val.raw	= 0b00000000;
	me->reg._0x3B_NTC_ADC_H					.val.raw	= 0b00000000;
	me->reg._0x3C_NTC_ADC_L					.val.raw	= 0b00000000;
}

static void AlxPca9430_RegStruct_SetValToDefault(AlxPca9430* me)
{
	me->reg._0x00_Device_ID					.val.raw	= 0b00001000;
	me->reg._0x01_SYSTEM_INT				.val.raw	= 0b00000000;
	me->reg._0x02_SYSTEM_INT_MASK			.val.raw	= 0b00000000;
	me->reg._0x03_VRECT_INT					.val.raw	= 0b00000000;
	me->reg._0x04_VRECT_INT_MASK			.val.raw	= 0b00000000;
	me->reg._0x05_BATCHG_INT				.val.raw	= 0b00000000;
	me->reg._0x06_BATCHG_INT_MASK			.val.raw	= 0b00000000;
	me->reg._0x08_VRECT_THD					.val.raw	= 0b00000000;
	me->reg._0x09_VRECT_OVW					.val.raw	= 0b00010000;
	me->reg._0x0A_TEMP_THD					.val.raw	= 0b00000000;
	me->reg._0x0B_WD_EN_RST					.val.raw	= 0b00000010;
	me->reg._0x0C_Varactor_DAC				.val.raw	= 0b00000000;
	me->reg._0x0D_ADC_CONTROL				.val.raw	= 0b00000110;
	me->reg._0x0E_Sample_EN					.val.raw	= 0b01111111;
	me->reg._0x0F_VPWR_CONFIG				.val.raw	= 0b10000011;
	me->reg._0x10_RXIR_CONFIG				.val.raw	= 0b00000000;
	me->reg._0x20_BQ_CONTROL				.val.raw	= 0b10101010;
	me->reg._0x21_CC_CURRENT				.val.raw	= 0b00001000;
	me->reg._0x22_TP_CURRENT				.val.raw	= 0b00000100;
	me->reg._0x23_LB_CURRENT				.val.raw	= 0b00001000;
	me->reg._0x24_DB_CURRENT				.val.raw	= 0b01000100;
	me->reg._0x25_VBAT_REG					.val.raw	= 0b00011110;
	me->reg._0x26_BQTIMER_AND_NTC			.val.raw	= 0b10010101;
	me->reg._0x27_BQTHM_AND_SHRT			.val.raw	= 0b00100100;
	me->reg._0x28_Batterychargerstatus1		.val.raw	= 0b00000000;
	me->reg._0x29_Batterychargerstatus2		.val.raw	= 0b00000000;
	me->reg._0x2A_Batterychargerstatus3		.val.raw	= 0b00000000;
	me->reg._0x2B_Batterychargerstatus4		.val.raw	= 0b00000000;
	me->reg._0x30_VRECT_ADC_H				.val.raw	= 0b00000000;
	me->reg._0x31_VRECT_ADC_L				.val.raw	= 0b00000000;
	me->reg._0x32_VTUNE_ADC_H				.val.raw	= 0b00000000;
	me->reg._0x33_VTUNE_ADC_L				.val.raw	= 0b00000000;
	me->reg._0x34_VOUT_ADC_H				.val.raw	= 0b00000000;
	me->reg._0x35_VOUT_ADC_L				.val.raw	= 0b00000000;
	me->reg._0x36_IOUT_ADC_H				.val.raw	= 0b00000000;
	me->reg._0x37_IOUT_ADC_L				.val.raw	= 0b00000000;
	me->reg._0x38_IRECT_ADC_H				.val.raw	= 0b00000000;
	me->reg._0x39_IRECT_ADC_L				.val.raw	= 0b00000000;
	me->reg._0x3A_TDIE_ADC					.val.raw	= 0b00000000;
	me->reg._0x3B_NTC_ADC_H					.val.raw	= 0b00000000;
	me->reg._0x3C_NTC_ADC_L					.val.raw	= 0b00000000;
}

static Alx_Status AlxPca9430_Reg_Write(AlxPca9430* me, void* reg)
{
	uint8_t regAddr = *((uint8_t*)reg);
	uint8_t regLen = *((uint8_t*)reg + sizeof(regAddr));
	uint8_t* regValPtr = (uint8_t*)reg + sizeof(regAddr) + sizeof(regLen);

	return AlxI2c_Master_StartWriteMemStop_Multi(me->i2c, me->i2cAddr, regAddr, AlxI2c_Master_MemAddrLen_8bit, regValPtr, regLen, me->i2cCheckWithRead, me->i2cNumOfTries, me->i2cTimeout_ms);
}

static Alx_Status AlxPca9430_Reg_Read(AlxPca9430* me, void* reg)
{
	uint8_t regAddr = *((uint8_t*)reg);
	uint8_t regLen = *((uint8_t*)reg + sizeof(regAddr));
	uint8_t* regValPtr = (uint8_t*)reg + sizeof(regAddr) + sizeof(regLen);

	return AlxI2c_Master_StartReadMemStop(me->i2c, me->i2cAddr, regAddr, AlxI2c_Master_MemAddrLen_8bit, regValPtr, regLen, me->i2cNumOfTries, me->i2cTimeout_ms);
}

static Alx_Status AlxPca9430_Reg_WriteVal(AlxPca9430* me)
{
	Alx_Status status = AlxPca9430_Reg_Write(me, &me->reg._0x02_SYSTEM_INT_MASK);
	if (status != Alx_Ok) { ALX_PCA9430_TRACE("Err_0x02_SYSTEM_INT_MASK			"); return status;}
	
	status = AlxPca9430_Reg_Write(me, &me->reg._0x04_VRECT_INT_MASK				);
	if (status != Alx_Ok) { ALX_PCA9430_TRACE("Err_0x04_VRECT_INT_MASK			"); return status;}
	
	status = AlxPca9430_Reg_Write(me, &me->reg._0x06_BATCHG_INT_MASK			);
	if (status != Alx_Ok) { ALX_PCA9430_TRACE("Err_0x06_VOUTLDO_INT_MASK		"); return status;}
	
	status = AlxPca9430_Reg_Write(me, &me->reg._0x08_VRECT_THD					);
	if (status != Alx_Ok) { ALX_PCA9430_TRACE("Err_0x08_VRECT_THD				"); return status;}
	
	status = AlxPca9430_Reg_Write(me, &me->reg._0x09_VRECT_OVW					);
	if (status != Alx_Ok) { ALX_PCA9430_TRACE("Err_0x09_VRECT_OVW				"); return status;}
	
	status = AlxPca9430_Reg_Write(me, &me->reg._0x0A_TEMP_THD					);
	if (status != Alx_Ok) { ALX_PCA9430_TRACE("Err_0x0A_TEMP_THD				"); return status;}
	
	status = AlxPca9430_Reg_Write(me, &me->reg._0x0B_WD_EN_RST					);
	if (status != Alx_Ok) { ALX_PCA9430_TRACE("Err_0x0B_WD_EN_RST				"); return status;}
	
	status = AlxPca9430_Reg_Write(me, &me->reg._0x0C_Varactor_DAC				);
	if (status != Alx_Ok) { ALX_PCA9430_TRACE("Err_0x0C_Varactor_DAC			"); return status;}
	
	status = AlxPca9430_Reg_Write(me, &me->reg._0x0D_ADC_CONTROL				);
	if (status != Alx_Ok) { ALX_PCA9430_TRACE("Err_0x0D_ADC_CONTROL				"); return status;}
	
	status = AlxPca9430_Reg_Write(me, &me->reg._0x0E_Sample_EN					);
	if (status != Alx_Ok) { ALX_PCA9430_TRACE("Err_0x0E_Sample_EN				"); return status;}
	
	status = AlxPca9430_Reg_Write(me, &me->reg._0x0F_VPWR_CONFIG				);
	if (status != Alx_Ok) { ALX_PCA9430_TRACE("Err_0x0F_VPWR_CONFIG				"); return status;}
	
	status = AlxPca9430_Reg_Write(me, &me->reg._0x10_RXIR_CONFIG				);
	if (status != Alx_Ok) { ALX_PCA9430_TRACE("Err_0x10_RXIR_CONFIG				"); return status;}
	
	status = AlxPca9430_Reg_Write(me, &me->reg._0x20_BQ_CONTROL					);
	if (status != Alx_Ok) { ALX_PCA9430_TRACE("Err_0x20_BQ_CONTROL				"); return status;}
	
	status = AlxPca9430_Reg_Write(me, &me->reg._0x21_CC_CURRENT					);
	if (status != Alx_Ok) { ALX_PCA9430_TRACE("Err_0x21_CC_CURRENT				"); return status;}
		
	status = AlxPca9430_Reg_Write(me, &me->reg._0x22_TP_CURRENT					);
	if (status != Alx_Ok) { ALX_PCA9430_TRACE("Err_0x22_TP_CURRENT				"); return status;}
		
	status = AlxPca9430_Reg_Write(me, &me->reg._0x23_LB_CURRENT					);
	if (status != Alx_Ok) { ALX_PCA9430_TRACE("Err_0x23_LB_CURRENT				"); return status;}
		
	status = AlxPca9430_Reg_Write(me, &me->reg._0x24_DB_CURRENT					);
	if (status != Alx_Ok) { ALX_PCA9430_TRACE("Err_0x24_DB_CURRENT				"); return status;}
		
	status = AlxPca9430_Reg_Write(me, &me->reg._0x25_VBAT_REG					);
	if (status != Alx_Ok) { ALX_PCA9430_TRACE("Err_0x25_VBAT_REG				"); return status;}
		
	status = AlxPca9430_Reg_Write(me, &me->reg._0x26_BQTIMER_AND_NTC			);
	if (status != Alx_Ok) { ALX_PCA9430_TRACE("Err_0x26_BQTIMER_AND_NTC			"); return status;}
			
	status = AlxPca9430_Reg_Write(me, &me->reg._0x27_BQTHM_AND_SHRT				);
	if (status != Alx_Ok) { ALX_PCA9430_TRACE("Err_0x27_BQTHM_AND_SHRT			"); return status;}
			
	status = AlxPca9430_Reg_Write(me, &me->reg._0x28_Batterychargerstatus1		);
	if (status != Alx_Ok) { ALX_PCA9430_TRACE("Err_0x28_Batterychargerstatus1	"); return status;}
				
	status = AlxPca9430_Reg_Write(me, &me->reg._0x29_Batterychargerstatus2);
	if (status != Alx_Ok) { ALX_PCA9430_TRACE("Err_0x29_Batterychargerstatus2	"); return status;}
				
	status = AlxPca9430_Reg_Write(me, &me->reg._0x2A_Batterychargerstatus3);
	if (status != Alx_Ok) { ALX_PCA9430_TRACE("Err_0x2A_Batterychargerstatus3	"); return status;}
				
	status = AlxPca9430_Reg_Write(me, &me->reg._0x2B_Batterychargerstatus4);
	if (status != Alx_Ok) { ALX_PCA9430_TRACE("Err_0x2B_Batterychargerstatus4	"); return status;}
	return Alx_Ok;
}

//******************************************************************************
// Weak Functions
//******************************************************************************

ALX_WEAK void AlxPca9430_RegStruct_SetVal(AlxPca9430* me)
{
	
	 (void)me;
	ALX_PCA9430_TRACE("Define 'AlxPca9431_RegStruct_SetVal' function in your application.");
	ALX_PCA9430_ASSERT(false);
}
