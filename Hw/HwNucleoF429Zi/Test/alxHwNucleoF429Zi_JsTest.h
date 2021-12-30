/**
  ******************************************************************************
  * @file alxHwNucleoF429Zi_JsTest.h
  * @brief Auralix HW NUCLEO-F429ZI C Library - JS Test Module
  * @version $LastChangedRevision: 4937 $
  * @date $LastChangedDate: 2021-05-02 22:05:40 +0200 (Sun, 02 May 2021) $
  ******************************************************************************
  */

#ifndef ALX_HW_NUCLEO_F429ZI_JS_TEST_H
#define ALX_HW_NUCLEO_F429ZI_JS_TEST_H

#ifdef __cplusplus
extern "C" {
#endif

//******************************************************************************
// Includes
//******************************************************************************
#include <alxHwNucleoF429Zi_Main.h>


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_HW_NUCLEO_F429ZI_C_TEST)


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
	AlxHwNucleoF429Zi_Main alxHwNucleoF429Zi_Main;

	// Info
	bool wasCtorCalled;
	bool isInit;
} AlxHwNucleoF429Zi_JsTest_G01_BringUp;


//******************************************************************************
// Private Functions
//******************************************************************************
static inline void AlxHwNucleoF429Zi_JsTest_G01_BringUp_T01_Led(AlxHwNucleoF429Zi_JsTest_G01_BringUp* me)
{
	AlxIoPin_Init(&me->alxHwNucleoF429Zi_Main.alxIoPin.do_PB0_LED1_GR);
	AlxIoPin_Init(&me->alxHwNucleoF429Zi_Main.alxIoPin.do_PB7_LED2_BL);
	AlxIoPin_Init(&me->alxHwNucleoF429Zi_Main.alxIoPin.do_PB14_LED3_RD);

	while(1)
	{
		AlxIoPin_Toggle(&me->alxHwNucleoF429Zi_Main.alxIoPin.do_PB0_LED1_GR);
		AlxIoPin_Toggle(&me->alxHwNucleoF429Zi_Main.alxIoPin.do_PB7_LED2_BL);
		AlxIoPin_Toggle(&me->alxHwNucleoF429Zi_Main.alxIoPin.do_PB14_LED3_RD);
		AlxDelay_ms(100);
	}
}
static inline void AlxHwNucleoF429Zi_JsTest_G01_BringUp_T02_Trace(AlxHwNucleoF429Zi_JsTest_G01_BringUp* me)
{
	(void)me;

	while(1)
	{
		ALX_TRACE_FORMAT("T02_Trace\r\n");
		AlxDelay_ms(100);
	}
}


//******************************************************************************
// Constructor & Functions
//******************************************************************************
static inline void AlxHwNucleoF429Zi_JsTest_G01_BringUp_Ctor(AlxHwNucleoF429Zi_JsTest_G01_BringUp* me)
{
	// Ctor
	AlxHwNucleoF429Zi_Main_Ctor(&me->alxHwNucleoF429Zi_Main);

	// Info
	me->wasCtorCalled = true;
}
static inline void AlxHwNucleoF429Zi_JsTest_G01_BringUp_Init(AlxHwNucleoF429Zi_JsTest_G01_BringUp* me)
{
	// Init
	AlxClk_Init(&alxClk);
	AlxTrace_Init(&alxTrace);

	// Info
	me->isInit = true;
}
static inline void AlxHwNucleoF429Zi_JsTest_G01_BringUp_Run(AlxHwNucleoF429Zi_JsTest_G01_BringUp* me)
{
	//AlxHwNucleoF429Zi_JsTest_G01_BringUp_T01_Led(me);
	AlxHwNucleoF429Zi_JsTest_G01_BringUp_T02_Trace(me);
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
	AlxHwNucleoF429Zi_Main alxHwNucleoF429Zi_Main;

	// Live Watch - variabla
	float LdoVout_Voltage_V;
	float LdoVout_Current_A;
	float Rect_Voltage_V;
	float Rect_Current_A;
	float SensTemp_C;

	// Info
	bool wasCtorCalled;
	bool isInit;
} AlxHwNucleoF429Zi_JsTest_G02_Pca9431;


//******************************************************************************
// Private Functions
//******************************************************************************
static inline void AlxHwNucleoF429Zi_JsTest_G02_Pca9431_T01_ReadAndClearInterrupt(AlxHwNucleoF429Zi_JsTest_G02_Pca9431* me)
{
	while (1)
	{
		AlxPca9431_Reg_ReadAndClearInterrupt(&me->alxHwNucleoF429Zi_Main.alxPca9431);
		AlxDelay_ms(1000);
	}
}
static inline void AlxHwNucleoF429Zi_JsTest_G02_Pca9431_T02_LdoVout_GetVoltage_V(AlxHwNucleoF429Zi_JsTest_G02_Pca9431* me)
{
	while (1)
	{
		AlxPca9431_LdoVout_GetVoltage_V(&me->alxHwNucleoF429Zi_Main.alxPca9431, &me->LdoVout_Voltage_V); // 10 bit ADC
		AlxDelay_ms(1000);
	}
}
static inline void AlxHwNucleoF429Zi_JsTest_G02_Pca9431_T03_LdoVout_GetCurrent_A(AlxHwNucleoF429Zi_JsTest_G02_Pca9431* me)
{
	while (1)
	{
		AlxPca9431_LdoVout_GetCurrent_A(&me->alxHwNucleoF429Zi_Main.alxPca9431, &me->LdoVout_Current_A); // 10 bit ADC
		AlxDelay_ms(1000);
	}
}
static inline void AlxHwNucleoF429Zi_JsTest_G02_Pca9431_T04_Rect_GetVoltage_V(AlxHwNucleoF429Zi_JsTest_G02_Pca9431* me)
{
	while (1)
	{
		AlxPca9431_Rect_GetVoltage_V(&me->alxHwNucleoF429Zi_Main.alxPca9431, &me->Rect_Voltage_V); // 10 bit ADC
		AlxDelay_ms(1000);
	}
}
static inline void AlxHwNucleoF429Zi_JsTest_G02_Pca9431_T05_Rect_Current_A(AlxHwNucleoF429Zi_JsTest_G02_Pca9431* me)
{
	while (1)
	{
		AlxPca9431_Rect_GetCurrent_A(&me->alxHwNucleoF429Zi_Main.alxPca9431, &me->Rect_Current_A); // 10 bit ADC
		AlxDelay_ms(1000);
	}
}
static inline void AlxHwNucleoF429Zi_JsTest_G02_Pca9431_T06_SensTemp_C(AlxHwNucleoF429Zi_JsTest_G02_Pca9431* me)
{
	while (1)
	{
		AlxPca9431_TempSens_GetTemp_degC(&me->alxHwNucleoF429Zi_Main.alxPca9431, &me->SensTemp_C); // 10 bit ADC
		AlxDelay_ms(1000);
	}
}
static inline void AlxHwNucleoF429Zi_JsTest_G02_Pca9431_T99_TestAll(AlxHwNucleoF429Zi_JsTest_G02_Pca9431* me)
{
	while (1)
	{
		AlxPca9431_Reg_ReadAndClearInterrupt(&me->alxHwNucleoF429Zi_Main.alxPca9431);
		AlxPca9431_LdoVout_GetVoltage_V(&me->alxHwNucleoF429Zi_Main.alxPca9431, &me->LdoVout_Voltage_V); // 10 bit ADC
		AlxPca9431_LdoVout_GetCurrent_A(&me->alxHwNucleoF429Zi_Main.alxPca9431, &me->LdoVout_Current_A); // 10 bit ADC
		AlxPca9431_Rect_GetVoltage_V(&me->alxHwNucleoF429Zi_Main.alxPca9431, &me->Rect_Voltage_V); // 10 bit ADC
		AlxPca9431_Rect_GetCurrent_A(&me->alxHwNucleoF429Zi_Main.alxPca9431, &me->Rect_Current_A); // 10 bit ADC
		AlxPca9431_TempSens_GetTemp_degC(&me->alxHwNucleoF429Zi_Main.alxPca9431, &me->SensTemp_C); // 10 bit ADC
		AlxDelay_ms(1000);
	}
}


//******************************************************************************
// Constructor & Functions
//******************************************************************************
static inline void AlxHwNucleoF429Zi_JsTest_G02_Pca9431_Ctor(AlxHwNucleoF429Zi_JsTest_G02_Pca9431* me)
{
	// Ctor
	AlxHwNucleoF429Zi_Main_Ctor(&me->alxHwNucleoF429Zi_Main);

	// Live Watch - set default value
	me->LdoVout_Voltage_V = 0;
	me->LdoVout_Current_A = 0;
	me->Rect_Voltage_V = 0;
	me->Rect_Current_A = 0;
	me->SensTemp_C = 0.001;

	// Info
	me->wasCtorCalled = true;
}
static inline void AlxHwNucleoF429Zi_JsTest_G02_Pca9431_Init(AlxHwNucleoF429Zi_JsTest_G02_Pca9431* me)
{
	// Init
	AlxClk_Init(&alxClk);
	AlxTrace_Init(&alxTrace);
	AlxIoPinIrq_Init(&me->alxHwNucleoF429Zi_Main.alxIoPinIrq);
	AlxPca9431_Init(&me->alxHwNucleoF429Zi_Main.alxPca9431);

	// Info
	me->isInit = true;
}
static inline void AlxHwNucleoF429Zi_JsTest_G02_Pca9431_Run(AlxHwNucleoF429Zi_JsTest_G02_Pca9431* me)
{
//	AlxHwNucleoF429Zi_JsTest_G02_Pca9431_T01_ReadAndClearInterrupt(me);
//	AlxHwNucleoF429Zi_JsTest_G02_Pca9431_T02_LdoVout_GetVoltage_V(me);
//	AlxHwNucleoF429Zi_JsTest_G02_Pca9431_T03_LdoVout_GetCurrent_A(me);
//	AlxHwNucleoF429Zi_JsTest_G02_Pca9431_T04_Rect_GetVoltage_V(me);
//	AlxHwNucleoF429Zi_JsTest_G02_Pca9431_T05_Rect_Current_A(me);
//	AlxHwNucleoF429Zi_JsTest_G02_Pca9431_T06_SensTemp_C(me);
	AlxHwNucleoF429Zi_JsTest_G02_Pca9431_T99_TestAll(me);
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
	AlxHwNucleoF429Zi_Main alxHwNucleoF429Zi_Main;

	// Info
	bool wasCtorCalled;
	bool isInit;
} AlxHwNucleoF429Zi_JsTest_G03_Crn120;


//******************************************************************************
// Private Functions
//******************************************************************************
static inline void AlxHwNucleoF429Zi_JsTest_G03_Crn120_T01_regasession0(AlxHwNucleoF429Zi_JsTest_G03_Crn120* me)
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

	uint8_t crn_DevAdrSend = 0b10101010; // Crn120

	while(1)
	{
		/*		Read		*/
		// 1. Start condition (with write), address +ACK, mema +ACK, rega +ACK and stop.
		AlxI2c_Master_StartWriteMemStop_Multi(&me->alxHwNucleoF429Zi_Main.alxI2c_I2C2_Master, crn_DevAdrSend, crn120_mema_session_registers, AlxI2c_Master_MemAddrLen_8bit, regasession0, 1, false, 1, 1000);

		// 2. Start condition (with read)+ACK, read reg data +NAK and stop.
		AlxI2c_Master_StartReadStop(&me->alxHwNucleoF429Zi_Main.alxI2c_I2C2_Master, crn_DevAdrSend, data, 1, 1, 1000);
		AlxDelay_ms(10);

		/*		Write		*/
		// 2. Start condition (with write), address +ACK, mema +ACK, rega +ACK, mask +ACK, data +ACK. stop
		AlxI2c_Master_StartWriteStop(&me->alxHwNucleoF429Zi_Main.alxI2c_I2C2_Master, crn_DevAdrSend, data2, 4, 1, 1000);
		if (data2[3])
		{
			data2[3] = 0x00; // zadnji bit na 0
		}
		else
		{
			data2[3] = 0x01; // zadnji bit na 0
		}
		AlxDelay_ms(1000);
	}
}
static inline void AlxHwNucleoF429Zi_JsTest_G03_Crn120_T02_regasession1(AlxHwNucleoF429Zi_JsTest_G03_Crn120* me)
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
		AlxI2c_Master_StartWriteMemStop_Multi(&me->alxHwNucleoF429Zi_Main.alxI2c_I2C2_Master, crn_DevAdrSend, crn120_mema_session_registers, AlxI2c_Master_MemAddrLen_8bit, regasession1, 1, false, 1, 1000);
		AlxI2c_Master_StartReadStop(&me->alxHwNucleoF429Zi_Main.alxI2c_I2C2_Master, crn_DevAdrSend, data, 1, 1, 1000);
		AlxDelay_ms(1000);
	}
}
static inline void AlxHwNucleoF429Zi_JsTest_G03_Crn120_T03_regasession2(AlxHwNucleoF429Zi_JsTest_G03_Crn120* me)
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
		AlxI2c_Master_StartWriteMemStop_Multi(&me->alxHwNucleoF429Zi_Main.alxI2c_I2C2_Master, crn_DevAdrSend, crn120_mema_session_registers, AlxI2c_Master_MemAddrLen_8bit, regasession2, 1, false, 1, 1000);
		AlxI2c_Master_StartReadStop(&me->alxHwNucleoF429Zi_Main.alxI2c_I2C2_Master, crn_DevAdrSend, data, 1, 1, 1000);
		AlxDelay_ms(1000);
	}
}
static inline void AlxHwNucleoF429Zi_JsTest_G03_Crn120_T04_regasession3(AlxHwNucleoF429Zi_JsTest_G03_Crn120* me)
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
		AlxI2c_Master_StartWriteMemStop_Multi(&me->alxHwNucleoF429Zi_Main.alxI2c_I2C2_Master, crn_DevAdrSend, crn120_mema_session_registers, AlxI2c_Master_MemAddrLen_8bit, regasession3, 1, false, 1, 1000);
		AlxI2c_Master_StartReadStop(&me->alxHwNucleoF429Zi_Main.alxI2c_I2C2_Master, crn_DevAdrSend, data, 1, 1, 1000);
		AlxDelay_ms(1000);
	}
}
static inline void AlxHwNucleoF429Zi_JsTest_G03_Crn120_T05_regasession4(AlxHwNucleoF429Zi_JsTest_G03_Crn120* me)
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
		AlxI2c_Master_StartWriteMemStop_Multi(&me->alxHwNucleoF429Zi_Main.alxI2c_I2C2_Master, crn_DevAdrSend, crn120_mema_session_registers, AlxI2c_Master_MemAddrLen_8bit, regasession4, 1, false, 1, 1000);
		AlxI2c_Master_StartReadStop(&me->alxHwNucleoF429Zi_Main.alxI2c_I2C2_Master, crn_DevAdrSend, data, 1, 1, 1000);
		AlxDelay_ms(1000);
	}
}
static inline void AlxHwNucleoF429Zi_JsTest_G03_Crn120_T06_regasession5(AlxHwNucleoF429Zi_JsTest_G03_Crn120* me)
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
		AlxI2c_Master_StartWriteMemStop_Multi(&me->alxHwNucleoF429Zi_Main.alxI2c_I2C2_Master, crn_DevAdrSend, crn120_mema_session_registers, AlxI2c_Master_MemAddrLen_8bit, regasession5, 1, false, 1, 1000);
		AlxI2c_Master_StartReadStop(&me->alxHwNucleoF429Zi_Main.alxI2c_I2C2_Master, crn_DevAdrSend, data, 1, 1, 1000);
		AlxDelay_ms(1000);
	}
}
static inline void AlxHwNucleoF429Zi_JsTest_G03_Crn120_T99_TestAllregasession(AlxHwNucleoF429Zi_JsTest_G03_Crn120* me)
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
		AlxI2c_Master_StartWriteMemStop_Multi(&me->alxHwNucleoF429Zi_Main.alxI2c_I2C2_Master, crn_DevAdrSend, crn120_mema_session_registers, AlxI2c_Master_MemAddrLen_8bit, regasession0, 1, false, 1, 1000);

		// 2. Start condition (with read)+ACK, read reg data +NAK and stop.
		AlxI2c_Master_StartReadStop(&me->alxHwNucleoF429Zi_Main.alxI2c_I2C2_Master, crn_DevAdrSend, data, 1, 1, 1000);
		AlxDelay_ms(10);

		/*		Write		*/
		// 2. Start condition (with write), address +ACK, mema +ACK, rega +ACK, mask +ACK, data +ACK. stop
		AlxI2c_Master_StartWriteStop(&me->alxHwNucleoF429Zi_Main.alxI2c_I2C2_Master, crn_DevAdrSend, data2, 4, 1, 1000);
		if (data2[3])
		{
			data2[3] = 0x00; // zadnji bit na 0
		}
		else
		{
			data2[3] = 0x01; // zadnji bit na 0
		}
		AlxDelay_ms(10);

		AlxI2c_Master_StartWriteMemStop_Multi(&me->alxHwNucleoF429Zi_Main.alxI2c_I2C2_Master, crn_DevAdrSend, crn120_mema_session_registers, AlxI2c_Master_MemAddrLen_8bit, regasession1, 1, false, 1, 1000);
		AlxI2c_Master_StartReadStop(&me->alxHwNucleoF429Zi_Main.alxI2c_I2C2_Master, crn_DevAdrSend, data, 1, 1, 1000);
		AlxDelay_ms(10);

		AlxI2c_Master_StartWriteMemStop_Multi(&me->alxHwNucleoF429Zi_Main.alxI2c_I2C2_Master, crn_DevAdrSend, crn120_mema_session_registers, AlxI2c_Master_MemAddrLen_8bit, regasession2, 1, false, 1, 1000);
		AlxI2c_Master_StartReadStop(&me->alxHwNucleoF429Zi_Main.alxI2c_I2C2_Master, crn_DevAdrSend, data, 1, 1, 1000);
		AlxDelay_ms(10);

		AlxI2c_Master_StartWriteMemStop_Multi(&me->alxHwNucleoF429Zi_Main.alxI2c_I2C2_Master, crn_DevAdrSend, crn120_mema_session_registers, AlxI2c_Master_MemAddrLen_8bit, regasession3, 1, false, 1, 1000);
		AlxI2c_Master_StartReadStop(&me->alxHwNucleoF429Zi_Main.alxI2c_I2C2_Master, crn_DevAdrSend, data, 1, 1, 1000);
		AlxDelay_ms(10);

		AlxI2c_Master_StartWriteMemStop_Multi(&me->alxHwNucleoF429Zi_Main.alxI2c_I2C2_Master, crn_DevAdrSend, crn120_mema_session_registers, AlxI2c_Master_MemAddrLen_8bit, regasession4, 1, false, 1, 1000);
		AlxI2c_Master_StartReadStop(&me->alxHwNucleoF429Zi_Main.alxI2c_I2C2_Master, crn_DevAdrSend, data, 1, 1, 1000);
		AlxDelay_ms(10);

		AlxI2c_Master_StartWriteMemStop_Multi(&me->alxHwNucleoF429Zi_Main.alxI2c_I2C2_Master, crn_DevAdrSend, crn120_mema_session_registers, AlxI2c_Master_MemAddrLen_8bit, regasession5, 1, false, 1, 1000);
		AlxI2c_Master_StartReadStop(&me->alxHwNucleoF429Zi_Main.alxI2c_I2C2_Master, crn_DevAdrSend, data, 1, 1, 1000);
		AlxDelay_ms(1000);
	}
}


//******************************************************************************
// Constructor & Functions
//******************************************************************************
static inline void AlxHwNucleoF429Zi_JsTest_G03_Crn120_Ctor(AlxHwNucleoF429Zi_JsTest_G03_Crn120* me)
{
	// Ctor
	AlxHwNucleoF429Zi_Main_Ctor(&me->alxHwNucleoF429Zi_Main);

	// Info
	me->wasCtorCalled = true;
}
static inline void AlxHwNucleoF429Zi_JsTest_G03_Crn120_Init(AlxHwNucleoF429Zi_JsTest_G03_Crn120* me)
{
	// Init
	AlxClk_Init(&alxClk);
	AlxTrace_Init(&alxTrace);
	AlxI2c_Init(&me->alxHwNucleoF429Zi_Main.alxI2c_I2C2_Master);	// JS: Init Crn120

	// Info
	me->isInit = true;
}
static inline void AlxHwNucleoF429Zi_JsTest_G03_Crn120_Run(AlxHwNucleoF429Zi_JsTest_G03_Crn120* me)
{
	//AlxHwNucleoF429Zi_JsTest_G03_Crn120_T01_regasession0(me);
	//AlxHwNucleoF429Zi_JsTest_G03_Crn120_T02_regasession1(me);
	//AlxHwNucleoF429Zi_JsTest_G03_Crn120_T03_regasession2(me);
	//AlxHwNucleoF429Zi_JsTest_G03_Crn120_T04_regasession3(me);
	//AlxHwNucleoF429Zi_JsTest_G03_Crn120_T05_regasession4(me);
	//AlxHwNucleoF429Zi_JsTest_G03_Crn120_T06_regasession5(me);
	AlxHwNucleoF429Zi_JsTest_G03_Crn120_T99_TestAllregasession(me);
}


#endif // #if defined(ALX_HW_NUCLEO_F429ZI_C_TEST)

#ifdef __cplusplus
}
#endif

#endif // ALX_HW_NUCLEO_F429ZI_JS_TEST_H
