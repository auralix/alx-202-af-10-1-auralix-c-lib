/**
  ******************************************************************************
  * @file alxHwNfcWlcListenerV3_5b_JsTest.h
  * @brief Auralix HW NFC WLC Listener V3_5 C Library - JS Test Module
  * @version $LastChangedRevision: 4937 $
  * @date $LastChangedDate: 2021-05-02 22:05:40 +0200 (Sun, 02 May 2021) $
  ******************************************************************************
  */

#ifndef ALX_HW_NFC_WLC_LISTENER_V3_5B_JS_TEST_H
#define ALX_HW_NFC_WLC_LISTENER_V3_5B_JS_TEST_H

#ifdef __cplusplus
extern "C" {
#endif

//******************************************************************************
// Includes
//******************************************************************************
#include <alxHwNfcWlcListenerV3_5b_Main.h>


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_HW_NFC_WLC_LISTENER_V3_5B_C_TEST)


//******************************************************************************
//******************************************************************************
// G01_BringUp
//******************************************************************************
//******************************************************************************


//******************************************************************************
// Types
//******************************************************************************
typedef struct
{
	// Objects
	AlxHwNfcWlcListenerV3_5b_Main alxHwNfcWlcListenerV3_5b_Main;

	// Info
	bool wasCtorCalled;
	bool isInit;
} AlxHwNfcWlcListenerV3_5b_JsTest_G01_BringUp;


//******************************************************************************
// Private Functions
//******************************************************************************
static inline void AlxHwNfcWlcListenerV3_5b_JsTest_G01_BringUp_T01_Led(AlxHwNfcWlcListenerV3_5b_JsTest_G01_BringUp* me)
{
	// Assert
	(void)me;

	// Init
	AlxIoPin_Init(&me->alxHwNfcWlcListenerV3_5b_Main.alxIoPin.do_P0_18_LED205_RD);
	AlxIoPin_Init(&me->alxHwNfcWlcListenerV3_5b_Main.alxIoPin.do_P0_19_LED200_GR);
	//AlxIoPin_Init(&me->alxHwNfcWlcListenerV3_5b_Main.alxIoPin.do_P0_22_LED204_GR);
	//AlxIoPin_Init(&me->alxHwNfcWlcListenerV3_5b_Main.alxIoPin.do_P0_23_LED203_GR);
	//AlxIoPin_Init(&me->alxHwNfcWlcListenerV3_5b_Main.alxIoPin.do_P0_24_LED202_GR);
	//AlxIoPin_Init(&me->alxHwNfcWlcListenerV3_5b_Main.alxIoPin.do_P0_25_LED201_GR);

	while (1)
	{
		AlxIoPin_Toggle(&me->alxHwNfcWlcListenerV3_5b_Main.alxIoPin.do_P0_18_LED205_RD);
		AlxIoPin_Toggle(&me->alxHwNfcWlcListenerV3_5b_Main.alxIoPin.do_P0_19_LED200_GR);
		//AlxIoPin_Toggle(&me->alxHwNfcWlcListenerV3_5b_Main.alxIoPin.do_P0_22_LED204_GR);
		//AlxIoPin_Toggle(&me->alxHwNfcWlcListenerV3_5b_Main.alxIoPin.do_P0_23_LED203_GR);
		//AlxIoPin_Toggle(&me->alxHwNfcWlcListenerV3_5b_Main.alxIoPin.do_P0_24_LED202_GR);
		//AlxIoPin_Toggle(&me->alxHwNfcWlcListenerV3_5b_Main.alxIoPin.do_P0_25_LED201_GR);

		AlxDelay_ms(500);
	}
}
static inline void AlxHwNfcWlcListenerV3_5b_JsTest_G01_BringUp_T02_Trace(AlxHwNfcWlcListenerV3_5b_JsTest_G01_BringUp* me)
{
	// Assert
	(void)me;

	while (1)
	{
		ALX_TRACE_FORMAT("T02_Trace\r\n");

		AlxDelay_ms(500);
	}
}
static inline void AlxHwNfcWlcListenerV3_5b_JsTest_G01_BringUp_T03_Adc(AlxHwNfcWlcListenerV3_5b_JsTest_G01_BringUp* me)
{
	// Assert
	(void)me;

	// Init
	AlxAdc_Init(&me->alxHwNfcWlcListenerV3_5b_Main.alxAdc);

	while (1)
	{
		#if defined ALX_OPTIMIZE_SIZE_ALL
		AlxTrace_WriteFormat(&alxTrace, "Ch0, P1 = %d\r\n", AlxAdc_GetVoltage_mV(&me->alxHwNfcWlcListenerV3_5b_Main.alxAdc, me->alxHwNfcWlcListenerV3_5b_Main.adcChArr[0]));
		#else
		AlxTrace_WriteFormat(&alxTrace, "Ch0, P1 = %d\r\n", AlxAdc_GetVoltage_V(&me->alxHwNfcWlcListenerV3_5b_Main.alxAdc, me->alxHwNfcWlcListenerV3_5b_Main.adcChArr[0]));
		#endif

		AlxDelay_ms(1000);
	}
}
static inline void AlxHwNfcWlcListenerV3_5b_JsTest_G01_BringUp_T04_Pwm(AlxHwNfcWlcListenerV3_5b_JsTest_G01_BringUp* me)
{
	// Assert
	(void)me;

	// Init
	AlxPwm_Init(&me->alxHwNfcWlcListenerV3_5b_Main.alxPwm);

	// Setup Delay	// MF: To see how duty changes from 50% (set in Ctor) to what is set below
	AlxDelay_ms(500);

	while (1)
	{
		#if defined ALX_OPTIMIZE_SIZE_ALL
		AlxPwm_SetDuty_permil(&me->alxHwNfcWlcListenerV3_5b_Main.alxPwm, me->alxHwNfcWlcListenerV3_5b_Main.pwmChArr[0], 111);
		AlxPwm_SetDuty_permil(&me->alxHwNfcWlcListenerV3_5b_Main.alxPwm, me->alxHwNfcWlcListenerV3_5b_Main.pwmChArr[1], 999);
		#else
		AlxPwm_SetDuty_pct(&me->alxHwNfcWlcListenerV3_5b_Main.alxPwm, me->alxHwNfcWlcListenerV3_5b_Main.pwmChArr[0], 10.34f);
		AlxPwm_SetDuty_pct(&me->alxHwNfcWlcListenerV3_5b_Main.alxPwm, me->alxHwNfcWlcListenerV3_5b_Main.pwmChArr[1], 90);
		#endif

		AlxDelay_ms(500);
	}
}
static inline void AlxHwNfcWlcListenerV3_5b_JsTest_G01_BringUp_T05_I2c(AlxHwNfcWlcListenerV3_5b_JsTest_G01_BringUp* me)
{
	// Assert
	(void)me;

	// Variables
	uint8_t devAdrReceive = 0b1010101;
	uint16_t memAddr = 0x00U;	// MF: Sth is not right here
	uint8_t i2cData[6] = { 0x0E, 0x00, 0x00, 0x00, 0x00, 0x00 };

	// Init
	AlxIoPin_Init(&me->alxHwNfcWlcListenerV3_5b_Main.alxIoPin.ao_P0_11_CRN_VCC);
	AlxI2c_Init(&me->alxHwNfcWlcListenerV3_5b_Main.alxI2c_I2C0);

	while (1)
	{
		AlxI2c_Master_StartReadMemStop(&me->alxHwNfcWlcListenerV3_5b_Main.alxI2c_I2C0, devAdrReceive, memAddr, AlxI2c_Master_MemAddrLen_16bit, i2cData, 1, 20, 100);
		//AlxI2c_Master_StartWriteMemStop_Single(&i2c, slaveAddr, memAddr, AlxI2c_Master_MemAddrLen_16bit, i2cData[0], true, 5, 100);

		AlxDelay_ms(500);
	}
}


//******************************************************************************
// Constructor & Functions
//******************************************************************************
static inline void AlxHwNfcWlcListenerV3_5b_JsTest_G01_BringUp_Ctor(AlxHwNfcWlcListenerV3_5b_JsTest_G01_BringUp* me)
{
	// Ctor
	AlxHwNfcWlcListenerV3_5b_Main_Ctor(&me->alxHwNfcWlcListenerV3_5b_Main);

	// Info
	me->wasCtorCalled = true;
}
static inline void AlxHwNfcWlcListenerV3_5b_JsTest_G01_BringUp_Init(AlxHwNfcWlcListenerV3_5b_JsTest_G01_BringUp* me)
{
	// Init
	AlxClk_Init(&alxClk);
	AlxTrace_Init(&alxTrace);

	// IoPinIrq
	AlxIoPinIrq_Init(&me->alxHwNfcWlcListenerV3_5b_Main.alxIrqPin_IRQ1);	// MF: IoPinIrq is always Inited

	// Info
	me->isInit = true;
}
static inline void AlxHwNfcWlcListenerV3_5b_JsTest_G01_BringUp_Run(AlxHwNfcWlcListenerV3_5b_JsTest_G01_BringUp* me)
{
	AlxHwNfcWlcListenerV3_5b_JsTest_G01_BringUp_T01_Led(me);
	//AlxHwNfcWlcListenerV3_5b_JsTest_G01_BringUp_T02_Trace(me);
	//AlxHwNfcWlcListenerV3_5b_JsTest_G01_BringUp_T03_Adc(me);
	//AlxHwNfcWlcListenerV3_5b_JsTest_G01_BringUp_T04_Pwm(me);
	//AlxHwNfcWlcListenerV3_5b_JsTest_G01_BringUp_T05_I2c(me);
}

//******************************************************************************
//******************************************************************************
// G02_Pca9431
//******************************************************************************
//******************************************************************************


//******************************************************************************
// Types
//******************************************************************************
typedef struct
{
	// Objects
	AlxHwNfcWlcListenerV3_5b_Main alxHwNfcWlcListenerV3_5b_Main;

	// Live Watch - variabla
	float LdoVout_Voltage_V;
	float LdoVout_Current_A;
	float Rect_Voltage_V;
	float Rect_Current_A;
	float SensTemp_C;

	// Info
	bool wasCtorCalled;
	bool isInit;
} AlxHwNfcWlcListenerV3_5b_JsTest_G02_Pca9431;


//******************************************************************************
// Private Functions
//******************************************************************************

static inline void AlxHwNfcWlcListenerV3_5b_JsTest_G02_Pca9431_T01_ReadAndClearInterrupt(AlxHwNfcWlcListenerV3_5b_JsTest_G02_Pca9431* me)
{
	while (1)
	{
		AlxPca9431_Reg_ReadAndClearInterrupt(&me->alxHwNfcWlcListenerV3_5b_Main.alxPca9431);
		AlxDelay_ms(1000);
	}
}
static inline void AlxHwNfcWlcListenerV3_5b_JsTest_G02_Pca9431_T02_LdoVout_GetVoltage_V(AlxHwNfcWlcListenerV3_5b_JsTest_G02_Pca9431* me)
{
	while (1)
	{
		AlxPca9431_LdoVout_GetVoltage_V(&me->alxHwNfcWlcListenerV3_5b_Main.alxPca9431, &me->LdoVout_Voltage_V); // 10 bit ADC
		AlxDelay_ms(1000);
	}
}
	static inline void AlxHwNfcWlcListenerV3_5b_JsTest_G02_Pca9431_T03_LdoVout_GetCurrent_A(AlxHwNfcWlcListenerV3_5b_JsTest_G02_Pca9431* me)
{
	while (1)
	{
		AlxPca9431_LdoVout_GetCurrent_A(&me->alxHwNfcWlcListenerV3_5b_Main.alxPca9431, &me->LdoVout_Current_A); // 10 bit ADC
		AlxDelay_ms(1000);
	}
}
	static inline void AlxHwNfcWlcListenerV3_5b_JsTest_G02_Pca9431_T04_Rect_GetVoltage_V(AlxHwNfcWlcListenerV3_5b_JsTest_G02_Pca9431* me)
{
	while (1)
	{
		AlxPca9431_Rect_GetVoltage_V(&me->alxHwNfcWlcListenerV3_5b_Main.alxPca9431, &me->Rect_Voltage_V); // 10 bit ADC
		AlxDelay_ms(1000);
	}
}
	static inline void AlxHwNfcWlcListenerV3_5b_JsTest_G02_Pca9431_T05_Rect_Current_A(AlxHwNfcWlcListenerV3_5b_JsTest_G02_Pca9431* me)
{
	while (1)
	{
		AlxPca9431_Rect_GetCurrent_A(&me->alxHwNfcWlcListenerV3_5b_Main.alxPca9431, &me->Rect_Current_A); // 10 bit ADC
		AlxDelay_ms(1000);
	}
}
	static inline void AlxHwNfcWlcListenerV3_5b_JsTest_G02_Pca9431_T06_SensTemp_C(AlxHwNfcWlcListenerV3_5b_JsTest_G02_Pca9431* me)
{
	while (1)
	{
		AlxPca9431_TempSens_GetTemp_degC(&me->alxHwNfcWlcListenerV3_5b_Main.alxPca9431, &me->SensTemp_C); // 10 bit ADC
		AlxDelay_ms(1000);
	}
}
	static inline void AlxHwNfcWlcListenerV3_5b_JsTest_G02_Pca9431_T99_TestAll(AlxHwNfcWlcListenerV3_5b_JsTest_G02_Pca9431* me)
{
	while (1)
	{
		AlxPca9431_Reg_ReadAndClearInterrupt(&me->alxHwNfcWlcListenerV3_5b_Main.alxPca9431);
		AlxPca9431_LdoVout_GetVoltage_V(&me->alxHwNfcWlcListenerV3_5b_Main.alxPca9431, &me->LdoVout_Voltage_V); // 10 bit ADC
		AlxPca9431_LdoVout_GetCurrent_A(&me->alxHwNfcWlcListenerV3_5b_Main.alxPca9431, &me->LdoVout_Current_A); // 10 bit ADC
		AlxPca9431_Rect_GetVoltage_V(&me->alxHwNfcWlcListenerV3_5b_Main.alxPca9431, &me->Rect_Voltage_V); // 10 bit ADC
		AlxPca9431_Rect_GetCurrent_A(&me->alxHwNfcWlcListenerV3_5b_Main.alxPca9431, &me->Rect_Current_A); // 10 bit ADC
		AlxPca9431_TempSens_GetTemp_degC(&me->alxHwNfcWlcListenerV3_5b_Main.alxPca9431, &me->SensTemp_C); // 10 bit ADC
		AlxDelay_ms(1000);
	}
}


//******************************************************************************
// Constructor & Functions
//******************************************************************************

static inline void AlxHwNfcWlcListenerV3_5b_JsTest_G02_Pca9431_Ctor(AlxHwNfcWlcListenerV3_5b_JsTest_G02_Pca9431* me)
{
	// Ctor
	AlxHwNfcWlcListenerV3_5b_Main_Ctor(&me->alxHwNfcWlcListenerV3_5b_Main);

	// Live Watch - set default value
	me->LdoVout_Voltage_V = 0;
	me->LdoVout_Current_A = 0;
	me->Rect_Voltage_V = 0;
	me->Rect_Current_A = 0;
	me->SensTemp_C = 0.001;

	// Info
	me->wasCtorCalled = true;
}
	static inline void AlxHwNfcWlcListenerV3_5b_JsTest_G02_Pca9431_Init(AlxHwNfcWlcListenerV3_5b_JsTest_G02_Pca9431* me)
{
	// Init
	AlxClk_Init(&alxClk);
	AlxTrace_Init(&alxTrace);
	//AlxIoPinIrq_Init(&me->alxHwNfcWlcListenerV3_5b_Main.ioPinIrq); // JS: not imlemented yet
	AlxPca9431_Init(&me->alxHwNfcWlcListenerV3_5b_Main.alxPca9431);


	// Info
	me->isInit = true;
}
	static inline void AlxHwNfcWlcListenerV3_5b_JsTest_G02_Pca9431_Run(AlxHwNfcWlcListenerV3_5b_JsTest_G02_Pca9431* me)
{
//	//AlxHwNfcWlcListenerV3_5b_JsTest_G02_Pca9431_T01_ReadAndClearInterrupt(me);
//	AlxHwNfcWlcListenerV3_5b_JsTest_G02_Pca9431_T02_LdoVout_GetVoltage_V(me);
//	AlxHwNfcWlcListenerV3_5b_JsTest_G02_Pca9431_T03_LdoVout_GetCurrent_A(me);
//	AlxHwNfcWlcListenerV3_5b_JsTest_G02_Pca9431_T04_Rect_GetVoltage_V(me);
//	AlxHwNfcWlcListenerV3_5b_JsTest_G02_Pca9431_T05_Rect_Current_A(me);
//	AlxHwNfcWlcListenerV3_5b_JsTest_G02_Pca9431_T06_SensTemp_C(me);
	AlxHwNfcWlcListenerV3_5b_JsTest_G02_Pca9431_T99_TestAll(me);
}

//******************************************************************************
//******************************************************************************
// G03 - Crn120
//******************************************************************************
//******************************************************************************


//******************************************************************************
// Types
//******************************************************************************
typedef struct
{
	// Objects
	AlxHwNfcWlcListenerV3_5b_Main alxHwNfcWlcListenerV3_5b_Main;

	// Info
	bool wasCtorCalled;
	bool isInit;
} AlxHwNfcWlcListenerV3_5b_JsTest_G03_Crn120;


//******************************************************************************
// Private Functions
//******************************************************************************
static inline void AlxHwNfcWlcListenerV3_5b_JsTest_G03_Crn120_T01_regasession0(AlxHwNfcWlcListenerV3_5b_JsTest_G03_Crn120* me)
{
	uint8_t regasession0[1];
	regasession0[0] = 0x00;

	uint8_t data[7];
	data[0] = 0x00;
	data[1] = 0x00;
	data[2] = 0x00;
	data[3] = 0x00;
	data[4] = 0x00;
	data[5] = 0x00;
	data[6] = 0x00;

	uint8_t crn120_mema_session_registers = 0xFE;
	uint8_t data2[7];
	data2[0] = crn120_mema_session_registers;//mema
	data2[1] = regasession0[0]; //rega
	data2[2] = 0x01; // mask last zero bit
	data2[3] = 0x00; // last bit to 0
	data2[4] = 0x00;
	data2[5] = 0x00;
	data2[6] = 0x00;

	uint8_t crn_DevAdrSend = 0b10101010; // Crn120 // 0xAA // 170 dec

	while(1)
	{
		/*		Read		*/
		// 1. Start condition (with write), address +ACK, mema +ACK, rega +ACK and stop.
		AlxI2c_Master_StartWriteMemStop_Multi(&me->alxHwNfcWlcListenerV3_5b_Main.alxI2c_I2C0, crn_DevAdrSend, crn120_mema_session_registers, AlxI2c_Master_MemAddrLen_8bit, regasession0, 1, false, 1, 1000);

		// 2. Start condition (with read)+ACK, read reg data +NAK and stop.
		AlxI2c_Master_StartReadStop(&me->alxHwNfcWlcListenerV3_5b_Main.alxI2c_I2C0, crn_DevAdrSend, data, 1, 1, 1000);
//		AlxDelay_ms(10);
//
//		/*		Write		*/
//		// 2. Start condition (with write), address +ACK, mema +ACK, rega +ACK, mask +ACK, data +ACK. stop
//		//AlxI2c_Master_StartWriteStop(&me->alxHwNfcWlcListenerV3_5b_Main.alxI2c_I2C0, crn_DevAdrSend, data2, 4, 1, 1000);
//		if (data2[3])
//		{
//			data2[3] = 0x00; // zadnji bit na 0
//		}
//		else
//		{
//			data2[3] = 0x01; // zadnji bit na 0
//		}
		AlxDelay_ms(1000);
	}
}
static inline void AlxHwNfcWlcListenerV3_5b_JsTest_G03_Crn120_T02_regasession1(AlxHwNfcWlcListenerV3_5b_JsTest_G03_Crn120* me)
{
	uint8_t regasession1[1];
	regasession1[0] = 0x01;

	uint8_t data[7];
	data[0] = 0x00;
	data[1] = 0x00;
	data[2] = 0x00;
	data[3] = 0x00;
	data[4] = 0x00;
	data[5] = 0x00;
	data[6] = 0x00;

	uint8_t crn120_mema_session_registers = 0xFE;
	uint8_t crn_DevAdrSend = 0b10101010; // Crn120

	while (1)
	{
		AlxI2c_Master_StartWriteMemStop_Multi(&me->alxHwNfcWlcListenerV3_5b_Main.alxI2c_I2C0, crn_DevAdrSend, crn120_mema_session_registers, AlxI2c_Master_MemAddrLen_8bit, regasession1, 1, false, 1, 1000);
		AlxI2c_Master_StartReadStop(&me->alxHwNfcWlcListenerV3_5b_Main.alxI2c_I2C0, crn_DevAdrSend, data, 1, 1, 1000);
		AlxDelay_ms(1000);
	}
}
static inline void AlxHwNfcWlcListenerV3_5b_JsTest_G03_Crn120_T03_regasession2(AlxHwNfcWlcListenerV3_5b_JsTest_G03_Crn120* me)
{
	uint8_t regasession2[1];
	regasession2[0] = 0x02;

	uint8_t data[7];
	data[0] = 0x00;
	data[1] = 0x00;
	data[2] = 0x00;
	data[3] = 0x00;
	data[4] = 0x00;
	data[5] = 0x00;
	data[6] = 0x00;

	uint8_t crn120_mema_session_registers = 0xFE;
	uint8_t crn_DevAdrSend = 0b10101010; // Crn120

	while (1)
	{
		AlxI2c_Master_StartWriteMemStop_Multi(&me->alxHwNfcWlcListenerV3_5b_Main.alxI2c_I2C0, crn_DevAdrSend, crn120_mema_session_registers, AlxI2c_Master_MemAddrLen_8bit, regasession2, 1, false, 1, 1000);
		AlxI2c_Master_StartReadStop(&me->alxHwNfcWlcListenerV3_5b_Main.alxI2c_I2C0, crn_DevAdrSend, data, 1, 1, 1000);
		AlxDelay_ms(1000);
	}
}
static inline void AlxHwNfcWlcListenerV3_5b_JsTest_G03_Crn120_T04_regasession3(AlxHwNfcWlcListenerV3_5b_JsTest_G03_Crn120* me)
{
	uint8_t regasession3[1];
	regasession3[0] = 0x03;

	uint8_t data[7];
	data[0] = 0x00;
	data[1] = 0x00;
	data[2] = 0x00;
	data[3] = 0x00;
	data[4] = 0x00;
	data[5] = 0x00;
	data[6] = 0x00;

	uint8_t crn120_mema_session_registers = 0xFE;
	uint8_t crn_DevAdrSend = 0b10101010; // Crn120

	while (1)
	{
		AlxI2c_Master_StartWriteMemStop_Multi(&me->alxHwNfcWlcListenerV3_5b_Main.alxI2c_I2C0, crn_DevAdrSend, crn120_mema_session_registers, AlxI2c_Master_MemAddrLen_8bit, regasession3, 1, false, 1, 1000);
		AlxI2c_Master_StartReadStop(&me->alxHwNfcWlcListenerV3_5b_Main.alxI2c_I2C0, crn_DevAdrSend, data, 1, 1, 1000);
		AlxDelay_ms(1000);
	}
}
static inline void AlxHwNfcWlcListenerV3_5b_JsTest_G03_Crn120_T05_regasession4(AlxHwNfcWlcListenerV3_5b_JsTest_G03_Crn120* me)
{
	uint8_t regasession4[1];
	regasession4[0] = 0x04;

	uint8_t data[7];
	data[0] = 0x00;
	data[1] = 0x00;
	data[2] = 0x00;
	data[3] = 0x00;
	data[4] = 0x00;
	data[5] = 0x00;
	data[6] = 0x00;

	uint8_t crn120_mema_session_registers = 0xFE;
	uint8_t crn_DevAdrSend = 0b10101010; // Crn120

	while (1)
	{
		AlxI2c_Master_StartWriteMemStop_Multi(&me->alxHwNfcWlcListenerV3_5b_Main.alxI2c_I2C0, crn_DevAdrSend, crn120_mema_session_registers, AlxI2c_Master_MemAddrLen_8bit, regasession4, 1, false, 1, 1000);
		AlxI2c_Master_StartReadStop(&me->alxHwNfcWlcListenerV3_5b_Main.alxI2c_I2C0, crn_DevAdrSend, data, 1, 1, 1000);
		AlxDelay_ms(1000);
	}
}
static inline void AlxHwNfcWlcListenerV3_5b_JsTest_G03_Crn120_T06_regasession5(AlxHwNfcWlcListenerV3_5b_JsTest_G03_Crn120* me)
{
	uint8_t regasession5[1];
	regasession5[0] = 0x05;

	uint8_t data[7];
	data[0] = 0x00;
	data[1] = 0x00;
	data[2] = 0x00;
	data[3] = 0x00;
	data[4] = 0x00;
	data[5] = 0x00;
	data[6] = 0x00;

	uint8_t crn120_mema_session_registers = 0xFE;
	uint8_t crn_DevAdrSend = 0b10101010; // Crn120

	while (1)
	{
		AlxI2c_Master_StartWriteMemStop_Multi(&me->alxHwNfcWlcListenerV3_5b_Main.alxI2c_I2C0, crn_DevAdrSend, crn120_mema_session_registers, AlxI2c_Master_MemAddrLen_8bit, regasession5, 1, false, 1, 1000);
		AlxI2c_Master_StartReadStop(&me->alxHwNfcWlcListenerV3_5b_Main.alxI2c_I2C0, crn_DevAdrSend, data, 1, 1, 1000);
		AlxDelay_ms(1000);
	}
}
static inline void AlxHwNfcWlcListenerV3_5b_JsTest_G03_Crn120_T99_TestAllregasession(AlxHwNfcWlcListenerV3_5b_JsTest_G03_Crn120* me)
{
	uint8_t regasession0[1];
	regasession0[0] = 0x00;

	uint8_t regasession1[1];
	regasession1[0] = 0x01;

	uint8_t regasession2[1];
	regasession2[0] = 0x02;

	uint8_t regasession3[1];
	regasession3[0] = 0x03;

	uint8_t regasession4[1];
	regasession4[0] = 0x04;

	uint8_t regasession5[1];
	regasession5[0] = 0x05;

	uint8_t data[7];
	data[0] = 0x00;
	data[1] = 0x00;
	data[2] = 0x00;
	data[3] = 0x00;
	data[4] = 0x00;
	data[5] = 0x00;
	data[6] = 0x00;

	uint8_t crn120_mema_session_registers = 0xFE;
	uint8_t data2[7];
	data2[0] = crn120_mema_session_registers;//mema
	data2[1] = regasession0[0]; //rega
	data2[2] = 0x01; // mask last zero bit
	data2[3] = 0x00; // last bit to 0
	data2[4] = 0x00;
	data2[5] = 0x00;
	data2[6] = 0x00;

	uint8_t crn_DevAdrSend = 0b10101010; // Crn120

	while (1)
	{
		/*		Read		*/
		// 1. Start condition (with write), address +ACK, mema +ACK, rega +ACK and stop.
		AlxI2c_Master_StartWriteMemStop_Multi(&me->alxHwNfcWlcListenerV3_5b_Main.alxI2c_I2C0, crn_DevAdrSend, crn120_mema_session_registers, AlxI2c_Master_MemAddrLen_8bit, regasession0, 1, false, 1, 1000);

		// 2. Start condition (with read)+ACK, read reg data +NAK and stop.
		AlxI2c_Master_StartReadStop(&me->alxHwNfcWlcListenerV3_5b_Main.alxI2c_I2C0, crn_DevAdrSend, data, 1, 1, 1000);
		AlxDelay_ms(10);

		/*		Write		*/
		// 2. Start condition (with write), address +ACK, mema +ACK, rega +ACK, mask +ACK, data +ACK. stop
		AlxI2c_Master_StartWriteStop(&me->alxHwNfcWlcListenerV3_5b_Main.alxI2c_I2C0, crn_DevAdrSend, data2, 4, 1, 1000);
		if (data2[3])
		{
			data2[3] = 0x00; // zadnji bit na 0
		}
		else
		{
			data2[3] = 0x01; // zadnji bit na 0
		}
		AlxDelay_ms(10);

		AlxI2c_Master_StartWriteMemStop_Multi(&me->alxHwNfcWlcListenerV3_5b_Main.alxI2c_I2C0, crn_DevAdrSend, crn120_mema_session_registers, AlxI2c_Master_MemAddrLen_8bit, regasession1, 1, false, 1, 1000);
		AlxI2c_Master_StartReadStop(&me->alxHwNfcWlcListenerV3_5b_Main.alxI2c_I2C0, crn_DevAdrSend, data, 1, 1, 1000);
		AlxDelay_ms(10);

		AlxI2c_Master_StartWriteMemStop_Multi(&me->alxHwNfcWlcListenerV3_5b_Main.alxI2c_I2C0, crn_DevAdrSend, crn120_mema_session_registers, AlxI2c_Master_MemAddrLen_8bit, regasession2, 1, false, 1, 1000);
		AlxI2c_Master_StartReadStop(&me->alxHwNfcWlcListenerV3_5b_Main.alxI2c_I2C0, crn_DevAdrSend, data, 1, 1, 1000);
		AlxDelay_ms(10);

		AlxI2c_Master_StartWriteMemStop_Multi(&me->alxHwNfcWlcListenerV3_5b_Main.alxI2c_I2C0, crn_DevAdrSend, crn120_mema_session_registers, AlxI2c_Master_MemAddrLen_8bit, regasession3, 1, false, 1, 1000);
		AlxI2c_Master_StartReadStop(&me->alxHwNfcWlcListenerV3_5b_Main.alxI2c_I2C0, crn_DevAdrSend, data, 1, 1, 1000);
		AlxDelay_ms(10);

		AlxI2c_Master_StartWriteMemStop_Multi(&me->alxHwNfcWlcListenerV3_5b_Main.alxI2c_I2C0, crn_DevAdrSend, crn120_mema_session_registers, AlxI2c_Master_MemAddrLen_8bit, regasession4, 1, false, 1, 1000);
		AlxI2c_Master_StartReadStop(&me->alxHwNfcWlcListenerV3_5b_Main.alxI2c_I2C0, crn_DevAdrSend, data, 1, 1, 1000);
		AlxDelay_ms(10);

		AlxI2c_Master_StartWriteMemStop_Multi(&me->alxHwNfcWlcListenerV3_5b_Main.alxI2c_I2C0, crn_DevAdrSend, crn120_mema_session_registers, AlxI2c_Master_MemAddrLen_8bit, regasession5, 1, false, 1, 1000);
		AlxI2c_Master_StartReadStop(&me->alxHwNfcWlcListenerV3_5b_Main.alxI2c_I2C0, crn_DevAdrSend, data, 1, 1, 1000);
		AlxDelay_ms(1000);
	}
}

//******************************************************************************
// Constructor & Functions
//******************************************************************************
static inline void AlxHwNfcWlcListenerV3_5b_JsTest_G03_Crn120_Ctor(AlxHwNfcWlcListenerV3_5b_JsTest_G03_Crn120* me)
{
	// Ctor
	AlxHwNfcWlcListenerV3_5b_Main_Ctor(&me->alxHwNfcWlcListenerV3_5b_Main);

	// Info
	me->wasCtorCalled = true;
}
	static inline void AlxHwNfcWlcListenerV3_5b_JsTest_G03_Crn120_Init(AlxHwNfcWlcListenerV3_5b_JsTest_G03_Crn120* me)
{
	// Init
	AlxClk_Init(&alxClk);
	AlxTrace_Init(&alxTrace);
	AlxI2c_Init(&me->alxHwNfcWlcListenerV3_5b_Main.alxI2c_I2C0);		// JS: Init Crn120

	// Info
	me->isInit = true;
}
	static inline void AlxHwNfcWlcListenerV3_5b_JsTest_G03_Crn120_Run(AlxHwNfcWlcListenerV3_5b_JsTest_G03_Crn120* me)
{
	AlxHwNfcWlcListenerV3_5b_JsTest_G03_Crn120_T01_regasession0(me);			// JS: not worked yet
	//AlxHwNfcWlcListenerV3_5b_JsTest_G03_Crn120_T02_regasession1(me);			// JS: not worked yet
	//AlxHwNfcWlcListenerV3_5b_JsTest_G03_Crn120_T03_regasession2(me);			// JS: not worked yet
	//AlxHwNfcWlcListenerV3_5b_JsTest_G03_Crn120_T04_regasession3(me);			// JS: not worked yet
	//AlxHwNfcWlcListenerV3_5b_JsTest_G03_Crn120_T05_regasession4(me);			// JS: not worked yet
	//AlxHwNfcWlcListenerV3_5b_JsTest_G03_Crn120_T06_regasession5(me);			// JS: not worked yet
	//AlxHwNfcWlcListenerV3_5b_JsTest_G03_Crn120_T99_TestAllregasession(me);	// JS: not worked yet
}

#endif // #if defined(ALX_HW_NFC_WLC_LISTENER_V3_5B_C_TEST)

#ifdef __cplusplus
}
#endif

#endif // ALX_HW_NFC_WLC_LISTENER_V3_5B_JS_TEST_H
