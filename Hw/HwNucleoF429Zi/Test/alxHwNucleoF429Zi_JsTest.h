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
// G02_PCA9431
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
} AlxHwNucleoF429Zi_JsTest_G02_Pca9431;


//******************************************************************************
// Private Functions
//******************************************************************************

	static inline void AlxHwNucleoF429Zi_JsTest_G02_Pca9431_T01_ReadAndClearInterrupt(AlxHwNucleoF429Zi_JsTest_G02_Pca9431* me)
{
	(void)me;

	while (1)
	{
		AlxPca9431_Reg_ReadAndClearInterrupt(&me->alxHwNucleoF429Zi_Main.Pca9431);
		AlxDelay_ms(1000);
	}
}
static inline void AlxHwNucleoF429Zi_JsTest_G02_Pca9431_T02_LdoVout_GetVoltage_V(AlxHwNucleoF429Zi_JsTest_G02_Pca9431* me)
{
	float LdoVout_Voltage_V = 0;

	while (1)
	{
		AlxPca9431_LdoVout_GetVoltage_V(&me->alxHwNucleoF429Zi_Main.Pca9431, &LdoVout_Voltage_V); // 10 bit ADC
		AlxTrace_WriteFormat(&alxTrace, "LdoVout_Voltage_V: %f \r\n", LdoVout_Voltage_V);
		AlxDelay_ms(1000);
	}
}
static inline void AlxHwNucleoF429Zi_JsTest_G02_Pca9431_T03_LdoVout_GetCurrent_A(AlxHwNucleoF429Zi_JsTest_G02_Pca9431* me)
{
	float LdoVout_Current_A = 0;

	while (1)
	{
		AlxPca9431_LdoVout_GetCurrent_A(&me->alxHwNucleoF429Zi_Main.Pca9431, &LdoVout_Current_A); // 10 bit ADC
		AlxTrace_WriteFormat(&alxTrace, "LdoVout_GetCurrent_A: %f \r\n", LdoVout_Current_A);
		AlxDelay_ms(1000);
	}
}
static inline void AlxHwNucleoF429Zi_JsTest_G02_Pca9431_T04_Rect_GetVoltage_V(AlxHwNucleoF429Zi_JsTest_G02_Pca9431* me)
{
	float Rect_Voltage_V = 0;

	while (1)
	{
		AlxPca9431_Rect_GetVoltage_V(&me->alxHwNucleoF429Zi_Main.Pca9431, &Rect_Voltage_V); // 10 bit ADC
		AlxTrace_WriteFormat(&alxTrace, "Rect_Voltage_V: %f \r\n", Rect_Voltage_V);
		AlxDelay_ms(1000);
	}
}
static inline void AlxHwNucleoF429Zi_JsTest_G02_Pca9431_T05_Rect_Current_A(AlxHwNucleoF429Zi_JsTest_G02_Pca9431* me)
{
	float Rect_Current_A = 0;

	while (1)
	{
		AlxPca9431_Rect_GetCurrent_A(&me->alxHwNucleoF429Zi_Main.Pca9431, &Rect_Current_A); // 10 bit ADC
		AlxTrace_WriteFormat(&alxTrace, "Rect_Current_A: %f \r\n", Rect_Current_A);
		AlxDelay_ms(1000);
	}
}
static inline void AlxHwNucleoF429Zi_JsTest_G02_Pca9431_T06_SensTemp_C(AlxHwNucleoF429Zi_JsTest_G02_Pca9431* me)
{
	float SensTemp_C = 0;

	while (1)
	{
		AlxPca9431_TempSens_GetTemp_degC(&me->alxHwNucleoF429Zi_Main.Pca9431, &SensTemp_C); // 10 bit ADC
		AlxTrace_WriteFormat(&alxTrace, "SensTemp_C: %f \r\n", SensTemp_C);
		AlxDelay_ms(1000);
	}
}
static inline void AlxHwNucleoF429Zi_JsTest_G02_Pca9431_T99_TestAll(AlxHwNucleoF429Zi_JsTest_G02_Pca9431* me)
{
	float LdoVout_Voltage_V = 0;
	float LdoVout_Current_A = 0;
	float Rect_Voltage_V = 0;
	float Rect_Current_A = 0;
	float SensTemp_C = 0.001;

	while (1)
	{
		AlxPca9431_Reg_ReadAndClearInterrupt(&me->alxHwNucleoF429Zi_Main.Pca9431);

		AlxPca9431_LdoVout_GetVoltage_V(&me->alxHwNucleoF429Zi_Main.Pca9431, &LdoVout_Voltage_V); // 10 bit ADC
		AlxTrace_WriteFormat(&alxTrace, "LdoVout_Voltage_V: %f \r\n", LdoVout_Voltage_V);

		AlxPca9431_LdoVout_GetCurrent_A(&me->alxHwNucleoF429Zi_Main.Pca9431, &LdoVout_Current_A); // 10 bit ADC
		AlxTrace_WriteFormat(&alxTrace, "LdoVout_GetCurrent_A: %f \r\n", LdoVout_Current_A);

		AlxPca9431_Rect_GetVoltage_V(&me->alxHwNucleoF429Zi_Main.Pca9431, &Rect_Voltage_V); // 10 bit ADC
		AlxTrace_WriteFormat(&alxTrace, "Rect_Voltage_V: %f \r\n", Rect_Voltage_V);

		AlxPca9431_Rect_GetCurrent_A(&me->alxHwNucleoF429Zi_Main.Pca9431, &Rect_Current_A); // 10 bit ADC
		AlxTrace_WriteFormat(&alxTrace, "Rect_Current_A: %f \r\n", Rect_Current_A);

		AlxPca9431_TempSens_GetTemp_degC(&me->alxHwNucleoF429Zi_Main.Pca9431, &SensTemp_C); // 10 bit ADC
		AlxTrace_WriteFormat(&alxTrace, "SensTemp_C: %f \r\n", SensTemp_C);
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

	uint8_t pca_DevAdrSend = 0b11100010; // Pca9031
	//uint8_t pca_DevAdrReceive = 0b11100011; //Pca9031

	AlxPca9431_Ctor(&me->alxHwNucleoF429Zi_Main.Pca9431, &me->alxHwNucleoF429Zi_Main.alxI2c_I2C2_Master, pca_DevAdrSend, &me->alxHwNucleoF429Zi_Main.alxIoPin.do_PA9_Pca9431_sleep, &me->alxHwNucleoF429Zi_Main.alxIoPin.do_PA9_Pca9431_sleep, false, 1, 1000);

	// Info
	me->wasCtorCalled = true;
}
static inline void AlxHwNucleoF429Zi_JsTest_G02_Pca9431_Init(AlxHwNucleoF429Zi_JsTest_G02_Pca9431* me)
{
	// Init
	AlxClk_Init(&alxClk);
	AlxTrace_Init(&alxTrace);
	AlxPca9431_Init(&me->alxHwNucleoF429Zi_Main.Pca9431);
	AlxIoPinIrq_Init(&me->alxHwNucleoF429Zi_Main.ioPinIrq);

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
#define PCA9431_DEVICE_ID       0x18

//#define CRN120_ADDRESS              (uint8_t) 0x55
#define CRN120_BLOCK_LENGTH           (uint8_t) 16
#define CRN120_MEMA_SESSION_REGISTERS (uint8_t) 0xFE
/* Block Addresses Defines (MEMA) */
#define CRN120_MEMA_SERIAL_NUMBER           (uint8_t) 0x00
#define CRN120_MEMA_CONFIGURATION_REGISTERS (uint8_t) 0x7A
#define CRN120_MEMA_DYNAMIC_LOCK_BYTES      (uint8_t) 0x78
/* End Block Addresses Defines (MEMA) */

/* Session Register Addresses Defines (REGA) */
#define CRN120_REGA_NC_REG            (uint8_t) 0x00
#define CRN120_REGA_LAST_NDEF_BLOCK   (uint8_t) 0x01
#define CRN120_REGA_SRAM_MIRROR_BLOCK (uint8_t) 0x02
#define CRN120_REGA_WDT_LS            (uint8_t) 0x03
#define CRN120_REGA_WDT_MS            (uint8_t) 0x04
#define CRN120_REGA_I2C_CLOCK_STR     (uint8_t) 0x05
#define CRN120_REGA_NS_REG            (uint8_t) 0x06

/* Get Session Registers */
uint8_t CRN120_Get_nc_reg(uint8_t *p_regdat);
uint8_t CRN120_Get_last_ndef_block(uint8_t *p_regdat);
uint8_t CRN120_Get_sram_mirror_block(uint8_t *p_regdat);
uint8_t CRN120_Get_wtd_ls(uint8_t *p_regdat);
uint8_t CRN120_Get_wdt_ms(uint8_t *p_regdat);
uint8_t CRN120_Get_i2c_clock_str(uint8_t *p_regdat);
uint8_t CRN120_Get_ns_reg(uint8_t *p_regdat);
/* End Get Session Registers */

/* Set Session Registers */
uint8_t CRN120_Set_nc_reg(const uint8_t mask, const uint8_t regdat);
uint8_t CRN120_Set_last_ndef_block(const uint8_t mask, const uint8_t regdat);
uint8_t CRN120_Set_sram_mirror_block(const uint8_t mask, const uint8_t regdat);
uint8_t CRN120_Set_wtd_ls(const uint8_t mask, const uint8_t regdat);
uint8_t CRN120_Set_wdt_ms(const uint8_t mask, const uint8_t regdat);
uint8_t CRN120_Set_i2c_clock_str(const uint8_t mask, const uint8_t regdat);
uint8_t CRN120_Set_ns_reg(const uint8_t mask, const uint8_t regdat);
/* End Set Session Registers */

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

	uint8_t data2[7];
	data2[0] = CRN120_MEMA_SESSION_REGISTERS;//mema
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
		AlxI2c_Master_StartWriteMemStop_Multi(&me->alxHwNucleoF429Zi_Main.alxI2c_I2C2_Master, crn_DevAdrSend, CRN120_MEMA_SESSION_REGISTERS, AlxI2c_Master_MemAddrLen_8bit, regasession0, 1, false, 1, 1000);

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

	uint8_t crn_DevAdrSend = 0b10101010; // Crn120

	while (1)
	{
		AlxI2c_Master_StartWriteMemStop_Multi(&me->alxHwNucleoF429Zi_Main.alxI2c_I2C2_Master, crn_DevAdrSend, CRN120_MEMA_SESSION_REGISTERS, AlxI2c_Master_MemAddrLen_8bit, regasession1, 1, false, 1, 1000);
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

	uint8_t crn_DevAdrSend = 0b10101010; // Crn120

	while (1)
	{
		AlxI2c_Master_StartWriteMemStop_Multi(&me->alxHwNucleoF429Zi_Main.alxI2c_I2C2_Master, crn_DevAdrSend, CRN120_MEMA_SESSION_REGISTERS, AlxI2c_Master_MemAddrLen_8bit, regasession2, 1, false, 1, 1000);
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

	uint8_t crn_DevAdrSend = 0b10101010; // Crn120

	while (1)
	{
		AlxI2c_Master_StartWriteMemStop_Multi(&me->alxHwNucleoF429Zi_Main.alxI2c_I2C2_Master, crn_DevAdrSend, CRN120_MEMA_SESSION_REGISTERS, AlxI2c_Master_MemAddrLen_8bit, regasession3, 1, false, 1, 1000);
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

	uint8_t crn_DevAdrSend = 0b10101010; // Crn120

	while (1)
	{
		AlxI2c_Master_StartWriteMemStop_Multi(&me->alxHwNucleoF429Zi_Main.alxI2c_I2C2_Master, crn_DevAdrSend, CRN120_MEMA_SESSION_REGISTERS, AlxI2c_Master_MemAddrLen_8bit, regasession4, 1, false, 1, 1000);
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

	uint8_t crn_DevAdrSend = 0b10101010; // Crn120

	while (1)
	{
		AlxI2c_Master_StartWriteMemStop_Multi(&me->alxHwNucleoF429Zi_Main.alxI2c_I2C2_Master, crn_DevAdrSend, CRN120_MEMA_SESSION_REGISTERS, AlxI2c_Master_MemAddrLen_8bit, regasession5, 1, false, 1, 1000);
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

	uint8_t data2[7];
	data2[0] = CRN120_MEMA_SESSION_REGISTERS;//mema
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
		AlxI2c_Master_StartWriteMemStop_Multi(&me->alxHwNucleoF429Zi_Main.alxI2c_I2C2_Master, crn_DevAdrSend, CRN120_MEMA_SESSION_REGISTERS, AlxI2c_Master_MemAddrLen_8bit, regasession0, 1, false, 1, 1000);

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

		AlxI2c_Master_StartWriteMemStop_Multi(&me->alxHwNucleoF429Zi_Main.alxI2c_I2C2_Master, crn_DevAdrSend, CRN120_MEMA_SESSION_REGISTERS, AlxI2c_Master_MemAddrLen_8bit, regasession1, 1, false, 1, 1000);
		AlxI2c_Master_StartReadStop(&me->alxHwNucleoF429Zi_Main.alxI2c_I2C2_Master, crn_DevAdrSend, data, 1, 1, 1000);
		AlxDelay_ms(10);

		AlxI2c_Master_StartWriteMemStop_Multi(&me->alxHwNucleoF429Zi_Main.alxI2c_I2C2_Master, crn_DevAdrSend, CRN120_MEMA_SESSION_REGISTERS, AlxI2c_Master_MemAddrLen_8bit, regasession2, 1, false, 1, 1000);
		AlxI2c_Master_StartReadStop(&me->alxHwNucleoF429Zi_Main.alxI2c_I2C2_Master, crn_DevAdrSend, data, 1, 1, 1000);
		AlxDelay_ms(10);

		AlxI2c_Master_StartWriteMemStop_Multi(&me->alxHwNucleoF429Zi_Main.alxI2c_I2C2_Master, crn_DevAdrSend, CRN120_MEMA_SESSION_REGISTERS, AlxI2c_Master_MemAddrLen_8bit, regasession3, 1, false, 1, 1000);
		AlxI2c_Master_StartReadStop(&me->alxHwNucleoF429Zi_Main.alxI2c_I2C2_Master, crn_DevAdrSend, data, 1, 1, 1000);
		AlxDelay_ms(10);

		AlxI2c_Master_StartWriteMemStop_Multi(&me->alxHwNucleoF429Zi_Main.alxI2c_I2C2_Master, crn_DevAdrSend, CRN120_MEMA_SESSION_REGISTERS, AlxI2c_Master_MemAddrLen_8bit, regasession4, 1, false, 1, 1000);
		AlxI2c_Master_StartReadStop(&me->alxHwNucleoF429Zi_Main.alxI2c_I2C2_Master, crn_DevAdrSend, data, 1, 1, 1000);
		AlxDelay_ms(10);

		AlxI2c_Master_StartWriteMemStop_Multi(&me->alxHwNucleoF429Zi_Main.alxI2c_I2C2_Master, crn_DevAdrSend, CRN120_MEMA_SESSION_REGISTERS, AlxI2c_Master_MemAddrLen_8bit, regasession5, 1, false, 1, 1000);
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
	AlxI2c_Init(&me->alxHwNucleoF429Zi_Main.alxI2c_I2C2_Master);		// Init Crn120

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
