/**
  ******************************************************************************
  * @file alxHwNfcWlcListenerV3_5b_MfTest.h
  * @brief Auralix HW NFC WLC Listener V3_5 C Library - MF Test Module
  * @version $LastChangedRevision: 4937 $
  * @date $LastChangedDate: 2021-05-02 22:05:40 +0200 (Sun, 02 May 2021) $
  ******************************************************************************
  */

#ifndef ALX_HW_NFC_WLC_LISTENER_V3_5B_MF_TEST_H
#define ALX_HW_NFC_WLC_LISTENER_V3_5B_MF_TEST_H

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
#if defined(ALX_HW_NFC_WLC_LISTENER_V3_5B_C_TEST) && defined(ALX_TEST_MF)


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
} AlxHwNfcWlcListenerV3_5b_Main_MfTest_G01_BringUp;


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


//******************************************************************************
// Constructor & Functions
//******************************************************************************
static inline void AlxHwNfcWlcListenerV3_5b_Main_MfTest_G01_BringUp_Ctor(AlxHwNfcWlcListenerV3_5b_Main_MfTest_G01_BringUp* me)
{
	// Ctor
	AlxHwNfcWlcListenerV3_5b_Main_Ctor(&me->alxHwNfcWlcListenerV3_5b_Main);

	// Info
	me->wasCtorCalled = true;
}
static inline void AlxHwNfcWlcListenerV3_5b_Main_MfTest_G01_BringUp_Init(AlxHwNfcWlcListenerV3_5b_Main_MfTest_G01_BringUp* me)
{
	// Init
	AlxClk_Init(&alxClk);
	AlxTrace_Init(&alxTrace);

	// IoPinIrq
	//AlxIoPinIrq_Init(&me->alxHwNfcWlcListenerV3_5b_Main.alxIoIrqPin_di_P0_9_PCA943X_nINT_IRQ1);	// MF: IoPinIrq is always Inited

	// Info
	me->isInit = true;
}
static inline void AlxHwNfcWlcListenerV3_5b_Main_MfTest_G01_BringUp_Run(AlxHwNfcWlcListenerV3_5b_Main_MfTest_G01_BringUp* me)
{
	//AlxHwNfcWlcListenerV3_5b_Main_MfTest_G01_BringUp_T01_Led(me);
	AlxHwNfcWlcListenerV3_5b_Main_MfTest_G01_BringUp_T02_Trace(me);
	//AlxHwNfcWlcListenerV3_5b_Main_MfTest_G01_BringUp_T03_Adc(me);
	//AlxHwNfcWlcListenerV3_5b_Main_MfTest_G01_BringUp_T04_Pwm(me);
	//AlxHwNfcWlcListenerV3_5b_Main_MfTest_G01_BringUp_T05_I2c(me);
}


//******************************************************************************
//******************************************************************************
// G02_I2cCrn120
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
} AlxHwNfcWlcListenerV3_5b_Main_MfTest_G02_I2cCrn120;


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


//******************************************************************************
// Constructor & Functions
//******************************************************************************
static inline void AlxHwNfcWlcListenerV3_5b_Main_MfTest_G02_I2cCrn120_Ctor(AlxHwNfcWlcListenerV3_5b_Main_MfTest_G02_I2cCrn120* me)
{
	// Ctor
	AlxHwNfcWlcListenerV3_5b_Main_Ctor(&me->alxHwNfcWlcListenerV3_5b_Main);

	// Info
	me->wasCtorCalled = true;
}
static inline void AlxHwNfcWlcListenerV3_5b_Main_MfTest_G02_I2cCrn120_Init(AlxHwNfcWlcListenerV3_5b_Main_MfTest_G02_I2cCrn120* me)
{
	// Init
	AlxClk_Init(&alxClk);
	AlxTrace_Init(&alxTrace);

	// Info
	me->isInit = true;
}
static inline void AlxHwNfcWlcListenerV3_5b_Main_MfTest_G02_I2cCrn120_Run(AlxHwNfcWlcListenerV3_5b_Main_MfTest_G02_I2cCrn120* me)
{
	//AlxHwNfcWlcListenerV3_5b_Main_MfTest_G02_I2cCrn120_T01_Led(me);
	AlxHwNfcWlcListenerV3_5b_Main_MfTest_G02_I2cCrn120_T02_IsSlaveReady(me);
	//AlxHwNfcWlcListenerV3_5b_Main_MfTest_G02_I2cCrn120_T03_ReadRegOperation(me);
	//AlxHwNfcWlcListenerV3_5b_Main_MfTest_G02_I2cCrn120_T04_ReadOperation(me);
	//AlxHwNfcWlcListenerV3_5b_Main_MfTest_G02_I2cCrn120_T05_WriteRegOperation(me);
	//AlxHwNfcWlcListenerV3_5b_Main_MfTest_G02_I2cCrn120_T06_WriteOperation(me);
}


//******************************************************************************
//******************************************************************************
// G03_AlxCrn120
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
} AlxHwNfcWlcListenerV3_5b_Main_MfTest_G03_AlxCrn120;


//******************************************************************************
// Private Functions
//******************************************************************************
static inline void AlxHwNfcWlcListenerV3_5b_Main_MfTest_G03_AlxCrn120_T01_Led(AlxHwNfcWlcListenerV3_5b_Main_MfTest_G03_AlxCrn120* me)
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
static inline void AlxHwNfcWlcListenerV3_5b_Main_MfTest_G03_AlxCrn120_T02_IsSlaveReady(AlxHwNfcWlcListenerV3_5b_Main_MfTest_G03_AlxCrn120* me)
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
static inline void AlxHwNfcWlcListenerV3_5b_Main_MfTest_G03_AlxCrn120_T03_ReadUsrMem(AlxHwNfcWlcListenerV3_5b_Main_MfTest_G03_AlxCrn120* me)
{
	// Variables
	uint8_t devAddr  = 0b10101010;
	uint8_t data[16] = { 0x00 };

	// Init I2c
	AlxI2c_Init(&me->alxHwNfcWlcListenerV3_5b_Main.alxI2c_I2C0);

	while (1)
	{
		for (uint8_t i = 1; i <= 55; i++)	// MF: From addr 0x01-0x37
		{
			AlxI2c_Master_StartWriteStop(&me->alxHwNfcWlcListenerV3_5b_Main.alxI2c_I2C0, devAddr, &i, 1, 5, 1000);
			AlxI2c_Master_StartReadStop(&me->alxHwNfcWlcListenerV3_5b_Main.alxI2c_I2C0, devAddr, data, sizeof(data), 5, 1000);
			AlxDelay_ms(5);
		}

		AlxDelay_ms(500);
	}
}
static inline void AlxHwNfcWlcListenerV3_5b_Main_MfTest_G03_AlxCrn120_T04_ModuleReadReg(AlxHwNfcWlcListenerV3_5b_Main_MfTest_G03_AlxCrn120* me)
{
	// Variables
	uint8_t data[7] = { 0x00 };

	// Init CRN120
	AlxCrn120_Init(&me->alxHwNfcWlcListenerV3_5b_Main.alxCrn120);

	while (1)
	{
		// REGA 00h - Should return 0x01
		AlxCrn120_Reg_ReadReg(&me->alxHwNfcWlcListenerV3_5b_Main.alxCrn120, &me->alxHwNfcWlcListenerV3_5b_Main.alxCrn120.reg._FEh_SessionReg, &data[0], 0);
		AlxDelay_ms(10);

		// REGA 01h - Should return 0x00
		AlxCrn120_Reg_ReadReg(&me->alxHwNfcWlcListenerV3_5b_Main.alxCrn120, &me->alxHwNfcWlcListenerV3_5b_Main.alxCrn120.reg._FEh_SessionReg, &data[1], 1);
		AlxDelay_ms(10);

		// REGA 02h - Should return 0xF8
		AlxCrn120_Reg_ReadReg(&me->alxHwNfcWlcListenerV3_5b_Main.alxCrn120, &me->alxHwNfcWlcListenerV3_5b_Main.alxCrn120.reg._FEh_SessionReg, &data[2], 2);
		AlxDelay_ms(10);

		// REGA 03h - Should return 0x48
		AlxCrn120_Reg_ReadReg(&me->alxHwNfcWlcListenerV3_5b_Main.alxCrn120, &me->alxHwNfcWlcListenerV3_5b_Main.alxCrn120.reg._FEh_SessionReg, &data[3], 3);
		AlxDelay_ms(10);

		// REGA 04h - Should return 0x08
		AlxCrn120_Reg_ReadReg(&me->alxHwNfcWlcListenerV3_5b_Main.alxCrn120, &me->alxHwNfcWlcListenerV3_5b_Main.alxCrn120.reg._FEh_SessionReg, &data[4], 4);
		AlxDelay_ms(10);

		// REGA 05h - Should return 0x01
		AlxCrn120_Reg_ReadReg(&me->alxHwNfcWlcListenerV3_5b_Main.alxCrn120, &me->alxHwNfcWlcListenerV3_5b_Main.alxCrn120.reg._FEh_SessionReg, &data[5], 5);
		AlxDelay_ms(10);

		// REGA 06h - Should return 0x00
		AlxCrn120_Reg_ReadReg(&me->alxHwNfcWlcListenerV3_5b_Main.alxCrn120, &me->alxHwNfcWlcListenerV3_5b_Main.alxCrn120.reg._FEh_SessionReg, &data[6], 6);
		AlxDelay_ms(10);

		AlxDelay_ms(1000);
	}
}
static inline void AlxHwNfcWlcListenerV3_5b_Main_MfTest_G03_AlxCrn120_T05_ModuleWriteReg(AlxHwNfcWlcListenerV3_5b_Main_MfTest_G03_AlxCrn120* me)
{
	// Variables
	uint8_t dataRead = 0;
	uint8_t dataWrite = 0b11111111;

	// Init CRN120
	AlxCrn120_Init(&me->alxHwNfcWlcListenerV3_5b_Main.alxCrn120);

	// Read REGA 04h - Should return 0x08
	AlxCrn120_Reg_ReadReg(&me->alxHwNfcWlcListenerV3_5b_Main.alxCrn120, &me->alxHwNfcWlcListenerV3_5b_Main.alxCrn120.reg._FEh_SessionReg, &dataRead, 4);
	AlxDelay_ms(10);

	while (1)
	{
		// Write REGA 04h - WDT_MS
		AlxCrn120_Reg_WriteReg(&me->alxHwNfcWlcListenerV3_5b_Main.alxCrn120, &me->alxHwNfcWlcListenerV3_5b_Main.alxCrn120.reg._FEh_SessionReg, &dataWrite, 4);
		AlxDelay_ms(10);

		// Read REGA 04h - Should return 0xFF
		AlxCrn120_Reg_ReadReg(&me->alxHwNfcWlcListenerV3_5b_Main.alxCrn120, &me->alxHwNfcWlcListenerV3_5b_Main.alxCrn120.reg._FEh_SessionReg, &dataRead, 4);
		AlxDelay_ms(10);

		AlxDelay_ms(1000);
	}
}
static inline void AlxHwNfcWlcListenerV3_5b_Main_MfTest_G03_AlxCrn120_T06_ModuleRead(AlxHwNfcWlcListenerV3_5b_Main_MfTest_G03_AlxCrn120* me)
{
	// Variables
	uint8_t data[16] = { 0x00 };

	// Init CRN120
	AlxCrn120_Init(&me->alxHwNfcWlcListenerV3_5b_Main.alxCrn120);

	while (1)
	{
		// Read register 00h
		AlxCrn120_Reg_Read(&me->alxHwNfcWlcListenerV3_5b_Main.alxCrn120, &me->alxHwNfcWlcListenerV3_5b_Main.alxCrn120.reg._00h, data);
		AlxDelay_ms(10);

		// Read register 38h
		AlxCrn120_Reg_Read(&me->alxHwNfcWlcListenerV3_5b_Main.alxCrn120, &me->alxHwNfcWlcListenerV3_5b_Main.alxCrn120.reg._38h, data);
		AlxDelay_ms(10);

		// Read register 39h
		AlxCrn120_Reg_Read(&me->alxHwNfcWlcListenerV3_5b_Main.alxCrn120, &me->alxHwNfcWlcListenerV3_5b_Main.alxCrn120.reg._39h, data);
		AlxDelay_ms(10);

		AlxDelay_ms(1000);
	}
}
static inline void AlxHwNfcWlcListenerV3_5b_Main_MfTest_G03_AlxCrn120_T07_ModuleWrite(AlxHwNfcWlcListenerV3_5b_Main_MfTest_G03_AlxCrn120* me)
{
	// Variables
	uint8_t data[16] = { 0x00 };

	// Init CRN120
	AlxCrn120_Init(&me->alxHwNfcWlcListenerV3_5b_Main.alxCrn120);

	// Read register 38h
	AlxCrn120_Reg_Read(&me->alxHwNfcWlcListenerV3_5b_Main.alxCrn120, &me->alxHwNfcWlcListenerV3_5b_Main.alxCrn120.reg._38h, data);

	// Set 16 Bytes
	data[0]  = 0xFF;	// MF: Protected user memory
	data[1]  = 0xFF;	// MF: Protected user memory
	data[2]  = 0xAA;	// MF: Protected user memory
	data[3]  = 0xAA;	// MF: Protected user memory
	data[4]  = 0xCC;	// MF: Protected user memory
	data[5]  = 0xCC;	// MF: Protected user memory
	data[6]  = 0x33;	// MF: Protected user memory
	data[7]  = 0x33;	// MF: Protected user memory
	data[8]  = 0x00;
	data[9]  = 0x00;
	data[10] = 0x00;
	data[11] = 0x00;
	data[12] = 0x00;
	data[13] = 0x00;
	data[14] = 0x00;
	data[15] = 0xFF;	// MF: AUTH0 sfould be 0xFF (Default)

	// Delay
	AlxDelay_ms(50);

	while (1)
	{
		// Write register 38h
		AlxCrn120_Reg_Write(&me->alxHwNfcWlcListenerV3_5b_Main.alxCrn120, &me->alxHwNfcWlcListenerV3_5b_Main.alxCrn120.reg._38h, data);

		// At least ~4ms delay after Write
		AlxDelay_ms(10);

		// Read register 38h
		AlxCrn120_Reg_Read(&me->alxHwNfcWlcListenerV3_5b_Main.alxCrn120, &me->alxHwNfcWlcListenerV3_5b_Main.alxCrn120.reg._38h, data);

		AlxDelay_ms(100000);
	}
}
static inline void AlxHwNfcWlcListenerV3_5b_Main_MfTest_G03_AlxCrn120_T08_ModuleSramRead(AlxHwNfcWlcListenerV3_5b_Main_MfTest_G03_AlxCrn120* me)
{
	// Variables
	uint8_t data[60] = { 0x00 };

	// Init CRN120
	AlxCrn120_Init(&me->alxHwNfcWlcListenerV3_5b_Main.alxCrn120);

	// Delay
	AlxDelay_ms(50);

	while (1)
	{
		AlxCrn120_ReadSram(&me->alxHwNfcWlcListenerV3_5b_Main.alxCrn120, 0xF8, data, sizeof(data));

		AlxDelay_ms(1000);
	}
}
static inline void AlxHwNfcWlcListenerV3_5b_Main_MfTest_G03_AlxCrn120_T09_ModuleSramWrite(AlxHwNfcWlcListenerV3_5b_Main_MfTest_G03_AlxCrn120* me)
{
	// Variables
	uint8_t data[62] = { 0x00 };

	// Init CRN120
	AlxCrn120_Init(&me->alxHwNfcWlcListenerV3_5b_Main.alxCrn120);

	// Set Bytes
	for (uint32_t i = 0; i < sizeof(data); i++) { data[i] = i; }

	// Delay
	AlxDelay_ms(50);

	while (1)
	{
		AlxCrn120_WriteSram(&me->alxHwNfcWlcListenerV3_5b_Main.alxCrn120, 0xF8, data, sizeof(data));

		AlxDelay_ms(1000);
	}
}
static inline void AlxHwNfcWlcListenerV3_5b_Main_MfTest_G03_AlxCrn120_T10_ModuleEepromRead(AlxHwNfcWlcListenerV3_5b_Main_MfTest_G03_AlxCrn120* me)
{
	// Variables
	uint8_t data[7] = { 0x00 };

	// Init CRN120
	AlxCrn120_Init(&me->alxHwNfcWlcListenerV3_5b_Main.alxCrn120);

	// Delay
	AlxDelay_ms(50);

	while (1)
	{
		// Write
		AlxCrn120_ReadEeprom(&me->alxHwNfcWlcListenerV3_5b_Main.alxCrn120, 0x01, data, sizeof(data));

		AlxDelay_ms(1000);
	}
}
static inline void AlxHwNfcWlcListenerV3_5b_Main_MfTest_G03_AlxCrn120_T11_ModuleEepromWrite(AlxHwNfcWlcListenerV3_5b_Main_MfTest_G03_AlxCrn120* me)
{
	// Variables
	uint8_t data[62] = { 0x00 };

	// Init CRN120
	AlxCrn120_Init(&me->alxHwNfcWlcListenerV3_5b_Main.alxCrn120);

	// Set Bytes
	for (uint32_t i = 0; i < sizeof(data); i++) { data[i] = i; }

	// Delay
	AlxDelay_ms(50);

	while (1)
	{
		AlxCrn120_WriteEeprom(&me->alxHwNfcWlcListenerV3_5b_Main.alxCrn120, 0x01, data, sizeof(data));

		AlxDelay_ms(1000);
	}
}


//******************************************************************************
// Constructor & Functions
//******************************************************************************
static inline void AlxHwNfcWlcListenerV3_5b_Main_MfTest_G03_AlxCrn120_Ctor(AlxHwNfcWlcListenerV3_5b_Main_MfTest_G03_AlxCrn120* me)
{
	// Ctor
	AlxHwNfcWlcListenerV3_5b_Main_Ctor(&me->alxHwNfcWlcListenerV3_5b_Main);

	// Info
	me->wasCtorCalled = true;
}
static inline void AlxHwNfcWlcListenerV3_5b_Main_MfTest_G03_AlxCrn120_Init(AlxHwNfcWlcListenerV3_5b_Main_MfTest_G03_AlxCrn120* me)
{
	// Init
	AlxClk_Init(&alxClk);
	AlxTrace_Init(&alxTrace);

	// Info
	me->isInit = true;
}
static inline void AlxHwNfcWlcListenerV3_5b_Main_MfTest_G03_AlxCrn120_Run(AlxHwNfcWlcListenerV3_5b_Main_MfTest_G03_AlxCrn120* me)
{
	//AlxHwNfcWlcListenerV3_5b_Main_MfTest_G03_AlxCrn120_T01_Led(me);
	//AlxHwNfcWlcListenerV3_5b_Main_MfTest_G03_AlxCrn120_T02_IsSlaveReady(me);
	//AlxHwNfcWlcListenerV3_5b_Main_MfTest_G03_AlxCrn120_T03_ReadUsrMem(me);
	//AlxHwNfcWlcListenerV3_5b_Main_MfTest_G03_AlxCrn120_T04_ModuleReadReg(me);
	//AlxHwNfcWlcListenerV3_5b_Main_MfTest_G03_AlxCrn120_T05_ModuleWriteReg(me);
	//AlxHwNfcWlcListenerV3_5b_Main_MfTest_G03_AlxCrn120_T06_ModuleRead(me);
	//AlxHwNfcWlcListenerV3_5b_Main_MfTest_G03_AlxCrn120_T07_ModuleWrite(me);
	//AlxHwNfcWlcListenerV3_5b_Main_MfTest_G03_AlxCrn120_T08_ModuleSramRead(me);
	//AlxHwNfcWlcListenerV3_5b_Main_MfTest_G03_AlxCrn120_T09_ModuleSramWrite(me);
	//AlxHwNfcWlcListenerV3_5b_Main_MfTest_G03_AlxCrn120_T10_ModuleEepromRead(me);
	AlxHwNfcWlcListenerV3_5b_Main_MfTest_G03_AlxCrn120_T11_ModuleEepromWrite(me);
}



#endif // #if defined(ALX_HW_NFC_WLC_LISTENER_V3_5B_C_TEST) && defined(ALX_TEST_MF)

#ifdef __cplusplus
}
#endif

#endif // ALX_HW_NFC_WLC_LISTENER_V3_5B_MF_TEST_H
