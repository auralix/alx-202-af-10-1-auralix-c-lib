//****************************************************************************
// @file alxHwNfcWlcListenerV3_5b_MfTest.c
// @brief Auralix HW NFC WLC Listener V3.5 - MF Test Module
// @copyright Copyright (C) 2022 Auralix d.o.o. All rights reserved.
//****************************************************************************


//******************************************************************************
// Includes
//******************************************************************************
#include <alxHwNfcWlcListenerV3_5b_MfTest.h>


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_HW_NFC_WLC_LISTENER_V3_5B_C_TEST) && defined(ALX_TEST_MF)


//************************************************************************************************************************************************************
//************************************************************************************************************************************************************
// G01_BringUp
//************************************************************************************************************************************************************
//************************************************************************************************************************************************************


//******************************************************************************
// Private Functions
//******************************************************************************
static void AlxHwNfcWlcListenerV3_5b_Main_MfTest_G01_BringUp_T01_Led(AlxHwNfcWlcListenerV3_5b_Main_MfTest_G01_BringUp* me);
static void AlxHwNfcWlcListenerV3_5b_Main_MfTest_G01_BringUp_T02_Trace(AlxHwNfcWlcListenerV3_5b_Main_MfTest_G01_BringUp* me);
static void AlxHwNfcWlcListenerV3_5b_Main_MfTest_G01_BringUp_T03_Adc(AlxHwNfcWlcListenerV3_5b_Main_MfTest_G01_BringUp* me);
static void AlxHwNfcWlcListenerV3_5b_Main_MfTest_G01_BringUp_T04_Pwm(AlxHwNfcWlcListenerV3_5b_Main_MfTest_G01_BringUp* me);
static void AlxHwNfcWlcListenerV3_5b_Main_MfTest_G01_BringUp_T05_I2c(AlxHwNfcWlcListenerV3_5b_Main_MfTest_G01_BringUp* me);


//******************************************************************************
// Constructor & Functions
//******************************************************************************
void AlxHwNfcWlcListenerV3_5b_Main_MfTest_G01_BringUp_Ctor(AlxHwNfcWlcListenerV3_5b_Main_MfTest_G01_BringUp* me)
{
	// Ctor
	AlxHwNfcWlcListenerV3_5b_Main_Ctor(&me->alxHwNfcWlcListenerV3_5b_Main);

	// Info
	me->wasCtorCalled = true;
}
void AlxHwNfcWlcListenerV3_5b_Main_MfTest_G01_BringUp_Init(AlxHwNfcWlcListenerV3_5b_Main_MfTest_G01_BringUp* me)
{
	// Init
	AlxClk_Init(&alxClk);
	AlxTrace_Init(&alxTrace);

	// IoPinIrq
	//AlxIoPinIrq_Init(&me->alxHwNfcWlcListenerV3_5b_Main.alxIoIrqPin_di_P0_9_PCA943X_nINT_IRQ1);	// MF: IoPinIrq is always Inited

	// Info
	me->isInit = true;
}
void AlxHwNfcWlcListenerV3_5b_Main_MfTest_G01_BringUp_Run(AlxHwNfcWlcListenerV3_5b_Main_MfTest_G01_BringUp* me)
{
	//AlxHwNfcWlcListenerV3_5b_Main_MfTest_G01_BringUp_T01_Led(me);
	AlxHwNfcWlcListenerV3_5b_Main_MfTest_G01_BringUp_T02_Trace(me);
	//AlxHwNfcWlcListenerV3_5b_Main_MfTest_G01_BringUp_T03_Adc(me);
	//AlxHwNfcWlcListenerV3_5b_Main_MfTest_G01_BringUp_T04_Pwm(me);
	//AlxHwNfcWlcListenerV3_5b_Main_MfTest_G01_BringUp_T05_I2c(me);
}


//******************************************************************************
// Private Functions
//******************************************************************************
static inline void AlxHwNfcWlcListenerV3_5b_Main_MfTest_G01_BringUp_T01_Led(AlxHwNfcWlcListenerV3_5b_Main_MfTest_G01_BringUp* me)
{
	// Init
	AlxIoPin_Init(&me->alxHwNfcWlcListenerV3_5b_Main.alxIoPin.do_P0_18_LED_RD);
	AlxIoPin_Init(&me->alxHwNfcWlcListenerV3_5b_Main.alxIoPin.do_P0_19_LED_GR);
	//AlxIoPin_Init(&me->alxHwNfcWlcListenerV3_5b_Main.alxIoPin.do_P0_22_LED_GR);
	//AlxIoPin_Init(&me->alxHwNfcWlcListenerV3_5b_Main.alxIoPin.do_P0_23_LED_GR);

	while (1)
	{
		AlxIoPin_Toggle(&me->alxHwNfcWlcListenerV3_5b_Main.alxIoPin.do_P0_18_LED_RD);
		AlxIoPin_Toggle(&me->alxHwNfcWlcListenerV3_5b_Main.alxIoPin.do_P0_19_LED_GR);
		//AlxIoPin_Toggle(&me->alxHwNfcWlcListenerV3_5b_Main.alxIoPin.do_P0_22_LED_GR);
		//AlxIoPin_Toggle(&me->alxHwNfcWlcListenerV3_5b_Main.alxIoPin.do_P0_23_LED_GR);

		AlxDelay_ms(500);
	}
}
static inline void AlxHwNfcWlcListenerV3_5b_Main_MfTest_G01_BringUp_T02_Trace(AlxHwNfcWlcListenerV3_5b_Main_MfTest_G01_BringUp* me)
{
	while (1)
	{
		ALX_TRACE_FORMAT("T02_Trace\r\n");

		AlxDelay_ms(1000);
	}
}
static inline void AlxHwNfcWlcListenerV3_5b_Main_MfTest_G01_BringUp_T03_Adc(AlxHwNfcWlcListenerV3_5b_Main_MfTest_G01_BringUp* me)
{
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
static inline void AlxHwNfcWlcListenerV3_5b_Main_MfTest_G01_BringUp_T04_Pwm(AlxHwNfcWlcListenerV3_5b_Main_MfTest_G01_BringUp* me)
{
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
static inline void AlxHwNfcWlcListenerV3_5b_Main_MfTest_G01_BringUp_T05_I2c(AlxHwNfcWlcListenerV3_5b_Main_MfTest_G01_BringUp* me)
{
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


//************************************************************************************************************************************************************
//************************************************************************************************************************************************************
// G02_I2cCrn120
//************************************************************************************************************************************************************
//************************************************************************************************************************************************************


//******************************************************************************
// Private Functions
//******************************************************************************
static void AlxHwNfcWlcListenerV3_5b_Main_MfTest_G02_I2cCrn120_T01_Led(AlxHwNfcWlcListenerV3_5b_Main_MfTest_G02_I2cCrn120* me);
static void AlxHwNfcWlcListenerV3_5b_Main_MfTest_G02_I2cCrn120_T02_IsSlaveReady(AlxHwNfcWlcListenerV3_5b_Main_MfTest_G02_I2cCrn120* me);
static void AlxHwNfcWlcListenerV3_5b_Main_MfTest_G02_I2cCrn120_T03_ReadRegOperation(AlxHwNfcWlcListenerV3_5b_Main_MfTest_G02_I2cCrn120* me);
static void AlxHwNfcWlcListenerV3_5b_Main_MfTest_G02_I2cCrn120_T04_ReadOperation(AlxHwNfcWlcListenerV3_5b_Main_MfTest_G02_I2cCrn120* me);
static void AlxHwNfcWlcListenerV3_5b_Main_MfTest_G02_I2cCrn120_T05_WriteRegOperation(AlxHwNfcWlcListenerV3_5b_Main_MfTest_G02_I2cCrn120* me);
static void AlxHwNfcWlcListenerV3_5b_Main_MfTest_G02_I2cCrn120_T06_WriteOperation(AlxHwNfcWlcListenerV3_5b_Main_MfTest_G02_I2cCrn120* me);


//******************************************************************************
// Constructor & Functions
//******************************************************************************
void AlxHwNfcWlcListenerV3_5b_Main_MfTest_G02_I2cCrn120_Ctor(AlxHwNfcWlcListenerV3_5b_Main_MfTest_G02_I2cCrn120* me)
{
	// Ctor
	AlxHwNfcWlcListenerV3_5b_Main_Ctor(&me->alxHwNfcWlcListenerV3_5b_Main);

	// Info
	me->wasCtorCalled = true;
}
void AlxHwNfcWlcListenerV3_5b_Main_MfTest_G02_I2cCrn120_Init(AlxHwNfcWlcListenerV3_5b_Main_MfTest_G02_I2cCrn120* me)
{
	// Init
	AlxClk_Init(&alxClk);
	AlxTrace_Init(&alxTrace);

	// Info
	me->isInit = true;
}
void AlxHwNfcWlcListenerV3_5b_Main_MfTest_G02_I2cCrn120_Run(AlxHwNfcWlcListenerV3_5b_Main_MfTest_G02_I2cCrn120* me)
{
	//AlxHwNfcWlcListenerV3_5b_Main_MfTest_G02_I2cCrn120_T01_Led(me);
	AlxHwNfcWlcListenerV3_5b_Main_MfTest_G02_I2cCrn120_T02_IsSlaveReady(me);
	//AlxHwNfcWlcListenerV3_5b_Main_MfTest_G02_I2cCrn120_T03_ReadRegOperation(me);
	//AlxHwNfcWlcListenerV3_5b_Main_MfTest_G02_I2cCrn120_T04_ReadOperation(me);
	//AlxHwNfcWlcListenerV3_5b_Main_MfTest_G02_I2cCrn120_T05_WriteRegOperation(me);
	//AlxHwNfcWlcListenerV3_5b_Main_MfTest_G02_I2cCrn120_T06_WriteOperation(me);
}


//******************************************************************************
// Private Functions
//******************************************************************************
static inline void AlxHwNfcWlcListenerV3_5b_Main_MfTest_G02_I2cCrn120_T01_Led(AlxHwNfcWlcListenerV3_5b_Main_MfTest_G02_I2cCrn120* me)
{
	// Init
	AlxIoPin_Init(&me->alxHwNfcWlcListenerV3_5b_Main.alxIoPin.do_P0_18_LED_RD);
	AlxIoPin_Init(&me->alxHwNfcWlcListenerV3_5b_Main.alxIoPin.do_P0_19_LED_GR);

	while (1)
	{
		AlxIoPin_Toggle(&me->alxHwNfcWlcListenerV3_5b_Main.alxIoPin.do_P0_18_LED_RD);
		AlxIoPin_Toggle(&me->alxHwNfcWlcListenerV3_5b_Main.alxIoPin.do_P0_19_LED_GR);

		AlxDelay_ms(200);
	}
}
static inline void AlxHwNfcWlcListenerV3_5b_Main_MfTest_G02_I2cCrn120_T02_IsSlaveReady(AlxHwNfcWlcListenerV3_5b_Main_MfTest_G02_I2cCrn120* me)
{
	// Variables
	uint8_t devAddr = 0b10101010;

	// Init I2c
	AlxI2c_Init(&me->alxHwNfcWlcListenerV3_5b_Main.alxI2c_I2C0);

	while (1)
	{
		AlxI2c_Master_IsSlaveReady(&me->alxHwNfcWlcListenerV3_5b_Main.alxI2c_I2C0, devAddr, 3, 1000);

		AlxDelay_ms(1000);
	}
}
static inline void AlxHwNfcWlcListenerV3_5b_Main_MfTest_G02_I2cCrn120_T03_ReadRegOperation(AlxHwNfcWlcListenerV3_5b_Main_MfTest_G02_I2cCrn120* me)
{
	// Variables
	uint8_t devAddr		= 0b10101010;
	uint8_t MEMA		= 0xFE;		// MF: Session Register
	uint8_t REGA[7]		= { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06 };
	uint8_t REGDAT[7]	= { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

	// Init I2c
	AlxI2c_Init(&me->alxHwNfcWlcListenerV3_5b_Main.alxI2c_I2C0);

	while (1)
	{
		// REGA 00h - Should return 0x01
		AlxI2c_Master_StartWriteMemStop_Multi(&me->alxHwNfcWlcListenerV3_5b_Main.alxI2c_I2C0, devAddr, MEMA, AlxI2c_Master_MemAddrLen_8bit, &REGA[0], 1, false, 1, 1000);
		AlxI2c_Master_StartReadStop(&me->alxHwNfcWlcListenerV3_5b_Main.alxI2c_I2C0, devAddr, &REGDAT[0], 1, 1, 1000);
		AlxDelay_ms(10);

		// REGA 01h - Should return 0x00
		AlxI2c_Master_StartWriteMemStop_Multi(&me->alxHwNfcWlcListenerV3_5b_Main.alxI2c_I2C0, devAddr, MEMA, AlxI2c_Master_MemAddrLen_8bit, &REGA[1], 1, false, 1, 1000);
		AlxI2c_Master_StartReadStop(&me->alxHwNfcWlcListenerV3_5b_Main.alxI2c_I2C0, devAddr, &REGDAT[1], 1, 1, 1000);
		AlxDelay_ms(10);

		// REGA 02h - Should return 0xF8
		AlxI2c_Master_StartWriteMemStop_Multi(&me->alxHwNfcWlcListenerV3_5b_Main.alxI2c_I2C0, devAddr, MEMA, AlxI2c_Master_MemAddrLen_8bit, &REGA[2], 1, false, 1, 1000);
		AlxI2c_Master_StartReadStop(&me->alxHwNfcWlcListenerV3_5b_Main.alxI2c_I2C0, devAddr, &REGDAT[2], 1, 1, 1000);
		AlxDelay_ms(10);

		// REGA 03h - Should return 0x48
		AlxI2c_Master_StartWriteMemStop_Multi(&me->alxHwNfcWlcListenerV3_5b_Main.alxI2c_I2C0, devAddr, MEMA, AlxI2c_Master_MemAddrLen_8bit, &REGA[3], 1, false, 1, 1000);
		AlxI2c_Master_StartReadStop(&me->alxHwNfcWlcListenerV3_5b_Main.alxI2c_I2C0, devAddr, &REGDAT[3], 1, 1, 1000);
		AlxDelay_ms(10);

		// REGA 04h - Should return 0x08
		AlxI2c_Master_StartWriteMemStop_Multi(&me->alxHwNfcWlcListenerV3_5b_Main.alxI2c_I2C0, devAddr, MEMA, AlxI2c_Master_MemAddrLen_8bit, &REGA[4], 1, false, 1, 1000);
		AlxI2c_Master_StartReadStop(&me->alxHwNfcWlcListenerV3_5b_Main.alxI2c_I2C0, devAddr, &REGDAT[4], 1, 1, 1000);
		AlxDelay_ms(10);

		// REGA 05h - Should return 0x01
		AlxI2c_Master_StartWriteMemStop_Multi(&me->alxHwNfcWlcListenerV3_5b_Main.alxI2c_I2C0, devAddr, MEMA, AlxI2c_Master_MemAddrLen_8bit, &REGA[5], 1, false, 1, 1000);
		AlxI2c_Master_StartReadStop(&me->alxHwNfcWlcListenerV3_5b_Main.alxI2c_I2C0, devAddr, &REGDAT[5], 1, 1, 1000);
		AlxDelay_ms(10);

		// REGA 06h - Should return 0x00
		AlxI2c_Master_StartWriteMemStop_Multi(&me->alxHwNfcWlcListenerV3_5b_Main.alxI2c_I2C0, devAddr, MEMA, AlxI2c_Master_MemAddrLen_8bit, &REGA[6], 1, false, 1, 1000);
		AlxI2c_Master_StartReadStop(&me->alxHwNfcWlcListenerV3_5b_Main.alxI2c_I2C0, devAddr, &REGDAT[6], 1, 1, 1000);
		AlxDelay_ms(10);

		AlxDelay_ms(500);
	}
}
static inline void AlxHwNfcWlcListenerV3_5b_Main_MfTest_G02_I2cCrn120_T04_ReadOperation(AlxHwNfcWlcListenerV3_5b_Main_MfTest_G02_I2cCrn120* me)
{
	// Variables
	uint8_t devAddr		= 0b10101010;
	uint8_t MEMA		= 0x00;		// MF: Addr. + Serial number (UID) Register
	uint8_t data[16]	= { 0x00 };

	// Init I2c
	AlxI2c_Init(&me->alxHwNfcWlcListenerV3_5b_Main.alxI2c_I2C0);

	while (1)
	{
		AlxI2c_Master_StartWriteStop(&me->alxHwNfcWlcListenerV3_5b_Main.alxI2c_I2C0, devAddr, &MEMA, 1, 5, 1000);
		AlxI2c_Master_StartReadStop(&me->alxHwNfcWlcListenerV3_5b_Main.alxI2c_I2C0, devAddr, data, sizeof(data), 5, 1000);

		AlxDelay_ms(500);
	}
}
static inline void AlxHwNfcWlcListenerV3_5b_Main_MfTest_G02_I2cCrn120_T05_WriteRegOperation(AlxHwNfcWlcListenerV3_5b_Main_MfTest_G02_I2cCrn120* me)
{
	// Variables
	uint8_t devAddr		= 0b10101010;
	uint8_t MEMA		= 0xFE;			// MF: Session Register
	uint8_t REGA[7]		= { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06 };
	uint8_t MASK		= 0b11111111;	// MF: Write 0xFF to WDT_MS Byte
	uint8_t REGDAT		= 0b11111111;	// MF: Write 0xFF to WDT_MS Byte
	uint8_t data[3]		= { REGA[4], MASK, REGDAT };
	uint8_t dummy;

	// Init I2c
	AlxI2c_Init(&me->alxHwNfcWlcListenerV3_5b_Main.alxI2c_I2C0);

	// Read REGA 04h - Should return 0x08
	AlxI2c_Master_StartWriteMemStop_Multi(&me->alxHwNfcWlcListenerV3_5b_Main.alxI2c_I2C0, devAddr, MEMA, AlxI2c_Master_MemAddrLen_8bit, &REGA[4], 1, false, 1, 1000);
	AlxI2c_Master_StartReadStop(&me->alxHwNfcWlcListenerV3_5b_Main.alxI2c_I2C0, devAddr, &dummy, 1, 1, 1000);
	AlxDelay_ms(10);

	while (1)
	{
		// Write REGA 04h - WDT_MS
		AlxI2c_Master_StartWriteMemStop_Multi(&me->alxHwNfcWlcListenerV3_5b_Main.alxI2c_I2C0, devAddr, MEMA, AlxI2c_Master_MemAddrLen_8bit, data, sizeof(data), false, 1, 1000);
		AlxDelay_ms(10);

		// Read REGA 04h - Should return 0xFF
		AlxI2c_Master_StartWriteMemStop_Multi(&me->alxHwNfcWlcListenerV3_5b_Main.alxI2c_I2C0, devAddr, MEMA, AlxI2c_Master_MemAddrLen_8bit, &REGA[4], 1, false, 1, 1000);
		AlxI2c_Master_StartReadStop(&me->alxHwNfcWlcListenerV3_5b_Main.alxI2c_I2C0, devAddr, &dummy, 1, 1, 1000);
		AlxDelay_ms(10);

		AlxDelay_ms(500);
	}
}
static inline void AlxHwNfcWlcListenerV3_5b_Main_MfTest_G02_I2cCrn120_T06_WriteOperation(AlxHwNfcWlcListenerV3_5b_Main_MfTest_G02_I2cCrn120* me)
{
	// Variables
	uint8_t devAddr		= 0b10101010;
	uint8_t MEMA		= 0xF8;		// MF: SRAM Register
	uint8_t data[16]	= { 0x00 };

	// Init I2c
	AlxI2c_Init(&me->alxHwNfcWlcListenerV3_5b_Main.alxI2c_I2C0);

	// Read First 16 Bytes of SRAM. All should be 0x00
	AlxI2c_Master_StartWriteStop(&me->alxHwNfcWlcListenerV3_5b_Main.alxI2c_I2C0, devAddr, &MEMA, 1, 5, 1000);
	AlxI2c_Master_StartReadStop(&me->alxHwNfcWlcListenerV3_5b_Main.alxI2c_I2C0, devAddr, data, sizeof(data), 5, 1000);

	// Set 16 Bytes
	data[0]  = 0xFF;
	data[1]  = 0xFF;
	data[2]  = 0xFF;
	data[3]  = 0xFF;
	data[4]  = 0xAA;
	data[5]  = 0xAA;
	data[6]  = 0xAA;
	data[7]  = 0xAA;
	data[8]  = 0xCC;
	data[9]  = 0xCC;
	data[10] = 0xCC;
	data[11] = 0xCC;
	data[12] = 0x33;
	data[13] = 0x33;
	data[14] = 0x33;
	data[15] = 0x33;

	// Delay
	AlxDelay_ms(50);

	while (1)
	{
		// Write
		AlxI2c_Master_StartWriteMemStop_Multi(&me->alxHwNfcWlcListenerV3_5b_Main.alxI2c_I2C0, devAddr, MEMA, AlxI2c_Master_MemAddrLen_8bit, data, sizeof(data), false, 5, 1000);

		// At least ~4ms delay after Write
		AlxDelay_ms(10);

		// Read First 16 Bytes of SRAM
		AlxI2c_Master_StartWriteStop(&me->alxHwNfcWlcListenerV3_5b_Main.alxI2c_I2C0, devAddr, &MEMA, 1, 5, 1000);
		AlxI2c_Master_StartReadStop(&me->alxHwNfcWlcListenerV3_5b_Main.alxI2c_I2C0, devAddr, data, sizeof(data), 5, 1000);

		AlxDelay_ms(500);
	}
}


//************************************************************************************************************************************************************
//************************************************************************************************************************************************************
// G03_AlxCrn120
//************************************************************************************************************************************************************
//************************************************************************************************************************************************************


//******************************************************************************
// Private Functions
//******************************************************************************
//static void AlxHwNfcWlcListenerV3_5b_Main_MfTest_G03_AlxCrn120_T01_Led(AlxHwNfcWlcListenerV3_5b_Main_MfTest_G03_AlxCrn120* me);	// MF: Already tested in previous groups
static void AlxHwNfcWlcListenerV3_5b_Main_MfTest_G03_AlxCrn120_T02_IsSlaveReady(AlxHwNfcWlcListenerV3_5b_Main_MfTest_G03_AlxCrn120* me);
static void AlxHwNfcWlcListenerV3_5b_Main_MfTest_G03_AlxCrn120_T03_ReadUsrMem(AlxHwNfcWlcListenerV3_5b_Main_MfTest_G03_AlxCrn120* me);
//static void AlxHwNfcWlcListenerV3_5b_Main_MfTest_G03_AlxCrn120_T04_ModuleReadReg(AlxHwNfcWlcListenerV3_5b_Main_MfTest_G03_AlxCrn120* me);	// MF: S starimi funkcijami ki so narobne
//static void AlxHwNfcWlcListenerV3_5b_Main_MfTest_G03_AlxCrn120_T05_ModuleWriteReg(AlxHwNfcWlcListenerV3_5b_Main_MfTest_G03_AlxCrn120* me);
//static void AlxHwNfcWlcListenerV3_5b_Main_MfTest_G03_AlxCrn120_T06_ModuleRead(AlxHwNfcWlcListenerV3_5b_Main_MfTest_G03_AlxCrn120* me);
//static void AlxHwNfcWlcListenerV3_5b_Main_MfTest_G03_AlxCrn120_T07_ModuleWrite(AlxHwNfcWlcListenerV3_5b_Main_MfTest_G03_AlxCrn120* me);
static void AlxHwNfcWlcListenerV3_5b_Main_MfTest_G03_AlxCrn120_T08_ModuleSramRead(AlxHwNfcWlcListenerV3_5b_Main_MfTest_G03_AlxCrn120* me);
static void AlxHwNfcWlcListenerV3_5b_Main_MfTest_G03_AlxCrn120_T09_ModuleSramWrite(AlxHwNfcWlcListenerV3_5b_Main_MfTest_G03_AlxCrn120* me);
static void AlxHwNfcWlcListenerV3_5b_Main_MfTest_G03_AlxCrn120_T10_ModuleEepromRead(AlxHwNfcWlcListenerV3_5b_Main_MfTest_G03_AlxCrn120* me);
static void AlxHwNfcWlcListenerV3_5b_Main_MfTest_G03_AlxCrn120_T11_ModuleEepromWrite(AlxHwNfcWlcListenerV3_5b_Main_MfTest_G03_AlxCrn120* me);


//******************************************************************************
// Constructor & Functions
//******************************************************************************
void AlxHwNfcWlcListenerV3_5b_Main_MfTest_G03_AlxCrn120_Ctor(AlxHwNfcWlcListenerV3_5b_Main_MfTest_G03_AlxCrn120* me)
{
	//------------------------------------------------------------------------------
	// ALX - IoPin
	//------------------------------------------------------------------------------
	AlxIoPin_Ctor(&me->ao_P0_11_CRN_VCC,		0,	11,	AlxIoPin_Func_GPIO,				IOCON_MODE_INACT,	false,	true,		true		);	// TV: Enable CRN120 in Init
	AlxIoPin_Ctor(&me->io_P0_16_I2C0_SDA,		0,	16,	AlxIoPin_Func_Swm_I2C0_SDA,		IOCON_MODE_INACT,	true,	ALX_NULL,	ALX_NULL	);
	AlxIoPin_Ctor(&me->do_P0_17_I2C0_SCL,		0,	17,	AlxIoPin_Func_Swm_I2C0_SCL,		IOCON_MODE_INACT,	true,	ALX_NULL,	ALX_NULL	);

	//------------------------------------------------------------------------------
	// ALX - Clock
	//------------------------------------------------------------------------------
	AlxClk_Ctor
	(
		&alxClk,
		AlxClk_Config_McuLpc80x_FroOsc_30MHz_Mainclk_15MHz_CoreSysClk_15MHz,
		AlxClk_Tick_1ms
	);

	//------------------------------------------------------------------------------
	// ALX - Trace
	//------------------------------------------------------------------------------
	AlxTrace_Ctor
	(
		&alxTrace,
		0,	// port
		4,	// pin
		USART1,
		AlxGlobal_BaudRate_115200
	);

	//------------------------------------------------------------------------------
	// ALX - I2C
	//------------------------------------------------------------------------------
	AlxI2c_Ctor
	(
		&me->alxI2c_I2C0,
		I2C0,
		&me->do_P0_17_I2C0_SCL,
		&me->io_P0_16_I2C0_SDA,
		AlxI2c_Clk_McuLpc80x_BitRate_400kHz
	);

	//------------------------------------------------------------------------------
	// ALX - CRN120
	//------------------------------------------------------------------------------
	AlxCrn120_Ctor
	(
		&me->alxCrn120,
		&me->alxI2c_I2C0,
		0xAA,		// i2cAddr
		true,		// i2cCheckWithRead
		3,			// i2cNumOfTries
		1000		// i2cTimeout_ms
	);

	// Info
	me->wasCtorCalled = true;
}
void AlxHwNfcWlcListenerV3_5b_Main_MfTest_G03_AlxCrn120_Init(AlxHwNfcWlcListenerV3_5b_Main_MfTest_G03_AlxCrn120* me)
{
	// Init
	AlxClk_Init(&alxClk);
	AlxTrace_Init(&alxTrace);

	// Info
	me->isInit = true;
}
void AlxHwNfcWlcListenerV3_5b_Main_MfTest_G03_AlxCrn120_Run(AlxHwNfcWlcListenerV3_5b_Main_MfTest_G03_AlxCrn120* me)
{
	AlxHwNfcWlcListenerV3_5b_Main_MfTest_G03_AlxCrn120_T02_IsSlaveReady(me);
	//AlxHwNfcWlcListenerV3_5b_Main_MfTest_G03_AlxCrn120_T03_ReadUsrMem(me);
	//AlxHwNfcWlcListenerV3_5b_Main_MfTest_G03_AlxCrn120_T08_ModuleSramRead(me);
	//AlxHwNfcWlcListenerV3_5b_Main_MfTest_G03_AlxCrn120_T09_ModuleSramWrite(me);
	//AlxHwNfcWlcListenerV3_5b_Main_MfTest_G03_AlxCrn120_T10_ModuleEepromRead(me);
	//AlxHwNfcWlcListenerV3_5b_Main_MfTest_G03_AlxCrn120_T11_ModuleEepromWrite(me);
}

//******************************************************************************
// Private Functions
//******************************************************************************
static inline void AlxHwNfcWlcListenerV3_5b_Main_MfTest_G03_AlxCrn120_T02_IsSlaveReady(AlxHwNfcWlcListenerV3_5b_Main_MfTest_G03_AlxCrn120* me)
{
	// Variables
	Alx_Status status = Alx_Err;
	uint8_t devAddr = 0xAA;

	// Init I2c
	status = AlxI2c_Init(&me->alxI2c_I2C0);
	if (status != Alx_Ok) { ALX_BKPT; }

	while (1)
	{
		status = AlxI2c_Master_IsSlaveReady(&me->alxI2c_I2C0, devAddr, 3, 1000);	// MF: Fuka, popravi linijo 552 (treba dpoèakat da se flagi nastavijo drugace skos ACK vrne
		if (status != Alx_Ok) { ALX_BKPT; }

		AlxDelay_ms(1000);
	}
}
static inline void AlxHwNfcWlcListenerV3_5b_Main_MfTest_G03_AlxCrn120_T03_ReadUsrMem(AlxHwNfcWlcListenerV3_5b_Main_MfTest_G03_AlxCrn120* me)
{
	// Variables
	uint8_t devAddr  = 0xAA;
	uint8_t data[16] = { 0x00 };

	// Init I2c
	AlxI2c_Init(&me->alxI2c_I2C0);

	while (1)
	{
		for (uint8_t i = 1; i <= 55; i++)	// MF: From addr 0x01-0x37
		{
			AlxI2c_Master_StartWriteStop(&me->alxI2c_I2C0, devAddr, &i, 1, 5, 1000);
			AlxI2c_Master_StartReadStop(&me->alxI2c_I2C0, devAddr, data, sizeof(data), 5, 1000);
			AlxDelay_ms(5);
		}

		AlxDelay_ms(500);
	}
}
static inline void AlxHwNfcWlcListenerV3_5b_Main_MfTest_G03_AlxCrn120_T08_ModuleSramRead(AlxHwNfcWlcListenerV3_5b_Main_MfTest_G03_AlxCrn120* me)
{
	// Variables
	Alx_Status status = Alx_Err;
	uint8_t data[60] = { 0x00 };

	// Init CRN120
	status = AlxCrn120_Init(&me->alxCrn120);
	if (status != Alx_Ok) { ALX_BKPT; }

	// Delay
	AlxDelay_ms(50);

	while (1)
	{
		// #1 Read data
		status = AlxCrn120_ReadSram(&me->alxCrn120, 0xF8, data, sizeof(data));
		if (status != Alx_Ok) { ALX_BKPT; }

		// #2 Check data
		for (uint32_t i = 0; i < sizeof(data); i++)
		{
			if (data[i] != 0) { ALX_BKPT; }
		}

		AlxDelay_ms(1000);
	}
}
static inline void AlxHwNfcWlcListenerV3_5b_Main_MfTest_G03_AlxCrn120_T09_ModuleSramWrite(AlxHwNfcWlcListenerV3_5b_Main_MfTest_G03_AlxCrn120* me)
{
	// Variables
	Alx_Status status = Alx_Err;
	uint8_t dataWrite[62] = { 0x00 };
	uint8_t dataRead[62] = { 0x00 };

	// Init CRN120
	status = AlxCrn120_Init(&me->alxCrn120);
	if (status != Alx_Ok) { ALX_BKPT; }

	// Set Bytes
	for (uint32_t i = 0; i < sizeof(dataWrite); i++) { dataWrite[i] = i; }

	// Delay
	AlxDelay_ms(50);

	while (1)
	{
		// #1 Write data
		status = AlxCrn120_WriteSram(&me->alxCrn120, 0xF8, dataWrite, sizeof(dataWrite));
		if (status != Alx_Ok) { ALX_BKPT; }

		// #2 Read data
		status = AlxCrn120_ReadSram(&me->alxCrn120, 0xF8, dataRead, sizeof(dataRead));
		if (status != Alx_Ok) { ALX_BKPT; }

		// #3 Check data
		for (uint32_t i = 0; i < sizeof(dataWrite); i++)
		{
			if (dataWrite[i] != dataRead[i]) { ALX_BKPT; }
		}

		AlxDelay_ms(1000);
	}
}
static inline void AlxHwNfcWlcListenerV3_5b_Main_MfTest_G03_AlxCrn120_T10_ModuleEepromRead(AlxHwNfcWlcListenerV3_5b_Main_MfTest_G03_AlxCrn120* me)
{
	// Variables
	Alx_Status status = Alx_Err;
	uint8_t data[60] = { 0x00 };

	// Init CRN120
	status = AlxCrn120_Init(&me->alxCrn120);
	if (status != Alx_Ok) { ALX_BKPT; }

	// Delay
	AlxDelay_ms(50);

	while (1)
	{
		// #1 Read data
		status = AlxCrn120_ReadEeprom(&me->alxCrn120, 0x01, data, sizeof(data));
		if (status != Alx_Ok) { ALX_BKPT; }

		AlxDelay_ms(1000);
	}
}
static inline void AlxHwNfcWlcListenerV3_5b_Main_MfTest_G03_AlxCrn120_T11_ModuleEepromWrite(AlxHwNfcWlcListenerV3_5b_Main_MfTest_G03_AlxCrn120* me)
{
	// Variables
	Alx_Status status = Alx_Err;
	uint8_t dataWrite[62] = { 0x00 };
	uint8_t dataRead[62] = { 0x00 };

	// Init CRN120
	status = AlxCrn120_Init(&me->alxCrn120);
	if (status != Alx_Ok) { ALX_BKPT; }

	// Set Bytes
	for (uint32_t i = 0; i < sizeof(dataWrite); i++) { dataWrite[i] = i; }

	// Delay
	AlxDelay_ms(50);

	while (1)
	{
		// #1 Write data
		status = AlxCrn120_WriteEeprom(&me->alxCrn120, 0x01, dataWrite, sizeof(dataWrite));
		if (status != Alx_Ok) { ALX_BKPT; }

		// #2 Read data
		status = AlxCrn120_ReadEeprom(&me->alxCrn120, 0x01, dataRead, sizeof(dataRead));
		if (status != Alx_Ok) { ALX_BKPT; }

		// #3 Check data
		for (uint32_t i = 0; i < sizeof(dataWrite); i++)
		{
			if (dataWrite[i] != dataRead[i]) { ALX_BKPT; }
		}

		AlxDelay_ms(1000);
	}
}


//************************************************************************************************************************************************************
//************************************************************************************************************************************************************
// G04_AlxWlcl
//************************************************************************************************************************************************************
//************************************************************************************************************************************************************


//******************************************************************************
// Private Functions
//******************************************************************************
static void AlxHwNfcWlcListenerV3_5b_Main_MfTest_G04_AlxWlcl_T01_WriteCcAndNdef(AlxHwNfcWlcListenerV3_5b_Main_MfTest_G04_AlxWlcl* me);
static void AlxHwNfcWlcListenerV3_5b_Main_MfTest_G04_AlxWlcl_T02_SetBat(AlxHwNfcWlcListenerV3_5b_Main_MfTest_G04_AlxWlcl* me);
static void AlxHwNfcWlcListenerV3_5b_Main_MfTest_G04_AlxWlcl_T03_CiliExample(AlxHwNfcWlcListenerV3_5b_Main_MfTest_G04_AlxWlcl* me);


//******************************************************************************
// Constructor & Functions
//******************************************************************************
void AlxHwNfcWlcListenerV3_5b_Main_MfTest_G04_AlxWlcl_Ctor(AlxHwNfcWlcListenerV3_5b_Main_MfTest_G04_AlxWlcl* me)
{
	//------------------------------------------------------------------------------
	// ALX - IoPin
	//------------------------------------------------------------------------------
	AlxIoPin_Ctor(&me->ao_P0_11_CRN_VCC,		0,	11,	AlxIoPin_Func_GPIO,				IOCON_MODE_INACT,	false,	true,		true		);	// TV: Enable CRN120 in Init
	AlxIoPin_Ctor(&me->io_P0_16_I2C0_SDA,		0,	16,	AlxIoPin_Func_Swm_I2C0_SDA,		IOCON_MODE_INACT,	true,	ALX_NULL,	ALX_NULL	);
	AlxIoPin_Ctor(&me->do_P0_17_I2C0_SCL,		0,	17,	AlxIoPin_Func_Swm_I2C0_SCL,		IOCON_MODE_INACT,	true,	ALX_NULL,	ALX_NULL	);
	AlxIoPin_Ctor(&me->di_P0_18_SET_BAT,		0,	18,	AlxIoPin_Func_GPIO,				IOCON_MODE_INACT,	false,	false,		true		);

	//------------------------------------------------------------------------------
	// ALX - Clock
	//------------------------------------------------------------------------------
	AlxClk_Ctor
	(
		&alxClk,
		AlxClk_Config_McuLpc80x_FroOsc_30MHz_Mainclk_15MHz_CoreSysClk_15MHz,
		AlxClk_Tick_1ms
	);

	//------------------------------------------------------------------------------
	// ALX - Trace
	//------------------------------------------------------------------------------
	AlxTrace_Ctor
	(
		&alxTrace,
		0,	// port
		4,	// pin
		USART1,
		AlxGlobal_BaudRate_115200
	);

	//------------------------------------------------------------------------------
	// ALX - I2C
	//------------------------------------------------------------------------------
	AlxI2c_Ctor
	(
		&me->alxI2c_I2C0,
		I2C0,
		&me->do_P0_17_I2C0_SCL,
		&me->io_P0_16_I2C0_SDA,
		AlxI2c_Clk_McuLpc80x_BitRate_400kHz
	);

	//------------------------------------------------------------------------------
	// ALX - CRN120
	//------------------------------------------------------------------------------
	AlxCrn120_Ctor
	(
		&me->alxCrn120,
		&me->alxI2c_I2C0,
		0xAA,	// i2cAddr
		true,		// i2cCheckWithRead
		3,			// i2cNumOfTries
		1000		// i2cTimeout_ms
	);

	//------------------------------------------------------------------------------
	// WLCL - MAIN
	//------------------------------------------------------------------------------
	AlxWlclMain_Ctor
	(
		&me->alxWlclMain,
		&me->alxWlclNfc,
		&me->alxWlclPwr
	);

	//------------------------------------------------------------------------------
	// WLCL - NFC
	//------------------------------------------------------------------------------
	AlxWlclNfc_Ctor
	(
		&me->alxWlclNfc,
		&me->alxCrn120
	);

	//------------------------------------------------------------------------------
	// WLCL - PWR
	//------------------------------------------------------------------------------
	AlxWlclPwr_Ctor
	(
		&me->alxWlclPwr
	);

	// Info
	me->wasCtorCalled = true;
}
void AlxHwNfcWlcListenerV3_5b_Main_MfTest_G04_AlxWlcl_Init(AlxHwNfcWlcListenerV3_5b_Main_MfTest_G04_AlxWlcl* me)
{
	// Init
	AlxClk_Init(&alxClk);
	AlxTrace_Init(&alxTrace);

	// Info
	me->isInit = true;
}
void AlxHwNfcWlcListenerV3_5b_Main_MfTest_G04_AlxWlcl_Run(AlxHwNfcWlcListenerV3_5b_Main_MfTest_G04_AlxWlcl* me)
{
	//AlxHwNfcWlcListenerV3_5b_Main_MfTest_G04_AlxWlcl_T01_WriteCcAndNdef(me);
	//AlxHwNfcWlcListenerV3_5b_Main_MfTest_G04_AlxWlcl_T02_SetBat(me);
	AlxHwNfcWlcListenerV3_5b_Main_MfTest_G04_AlxWlcl_T03_CiliExample(me);
}

//******************************************************************************
// Private Functions
//******************************************************************************
static inline void AlxHwNfcWlcListenerV3_5b_Main_MfTest_G04_AlxWlcl_T01_WriteCcAndNdef(AlxHwNfcWlcListenerV3_5b_Main_MfTest_G04_AlxWlcl* me)
{
	// Variables
	Alx_Status status = Alx_Err;
	uint8_t data[20] = { 0x00 };

	// Init I2c
	status = AlxCrn120_Init(&me->alxCrn120);
	if (status != Alx_Ok) { ALX_BKPT; }

	while (1)
	{
		// #1 Read 00h
		status = AlxCrn120_ReadEeprom(&me->alxCrn120, 0x00, data, 16);
		if (status != Alx_Ok) { ALX_BKPT; }

		// #2 Set 00h Data
		data[0]  = 0xAA;
		data[12] = 0xE1;
		data[13] = 0x10;
		data[14] = 0x6D;
		data[15] = 0x00;

		// #3 Write 00h - CC
		if (AlxCrn120_IsCheckWithReadEnabled(&me->alxCrn120))
		{
			AlxCrn120_CheckWithReadDisable(&me->alxCrn120);

			status = AlxCrn120_WriteEeprom(&me->alxCrn120, 0x00, data, 16);
			if (status != Alx_Ok) { ALX_BKPT; }

			AlxCrn120_CheckWithReadEnable(&me->alxCrn120);
		}

		// #4 Set 01h Data
		data[0]  = 0x03;
		data[1]  = 0x0F;
		data[2]  = 0xD1;
		data[3]  = 0x06;
		data[4]  = 0x06;
		data[5]  = 0x57;
		data[6]  = 0x4C;
		data[7]  = 0x43;
		data[8]  = 0x43;
		data[9]  = 0x41;
		data[10] = 0x50;
		data[11] = 0x10;
		data[12] = 0x3E;
		data[13] = 0x06;
		data[14] = 0x02;
		data[15] = 0x02;
		data[16] = 0x0A;
		data[17] = 0xFE;
		data[18] = 0x00;
		data[19] = 0x00;

		// #5 Write 01h - Eeprom
		status = AlxCrn120_WriteEeprom(&me->alxCrn120, 0x01, data, 20);
		if (status != Alx_Ok) { ALX_BKPT; }

		AlxDelay_ms(1000);
	}
}
static inline void AlxHwNfcWlcListenerV3_5b_Main_MfTest_G04_AlxWlcl_T02_SetBat(AlxHwNfcWlcListenerV3_5b_Main_MfTest_G04_AlxWlcl* me)
{
	// Variables
	Alx_Status status = Alx_Err;
	uint8_t data[20] = { 0x00 };
	me->isBatFul = false;

	// Init I2c
	status = AlxCrn120_Init(&me->alxCrn120);
	if (status != Alx_Ok) { ALX_BKPT; }

	while (1)
	{
		// #1 Read 01h
		status = AlxCrn120_ReadEeprom(&me->alxCrn120, 0x01, data, 20);
		if (status != Alx_Ok) { ALX_BKPT; }

		// #2 Set 01h Data
		if (me->isBatFul == true)	{ data[12] = 0xBE; }
		else						{ data[12] = 0x3E; }

		// #3 Write 00h - CC
		status = AlxCrn120_WriteEeprom(&me->alxCrn120, 0x01, data, 20);	// MF: checkWithRead happens here
		if (status != Alx_Ok) { ALX_BKPT; }

		AlxDelay_ms(1000);
	}
}
static inline void AlxHwNfcWlcListenerV3_5b_Main_MfTest_G04_AlxWlcl_T03_CiliExample(AlxHwNfcWlcListenerV3_5b_Main_MfTest_G04_AlxWlcl* me)
{
	// Variables
	Alx_Status status = Alx_Err;

	// Init WlclMain
	status = AlxWlclMain_Init(&me->alxWlclMain);
	if (status != Alx_Ok) { ALX_BKPT; }

	// Init IoPin
	AlxIoPin_Init(&me->di_P0_18_SET_BAT);

	while (1)
	{
		AlxWlclMain_Handle(&me->alxWlclMain);

		if (AlxIoPin_Read(&me->di_P0_18_SET_BAT))	{ me->alxWlclMain.isBatFul = false; }
		else										{ me->alxWlclMain.isBatFul = true; }

		AlxDelay_ms(1000);
	}
}


#endif // #if defined(ALX_HW_NFC_WLC_LISTENER_V3_5B_C_TEST) && defined(ALX_TEST_MF)
