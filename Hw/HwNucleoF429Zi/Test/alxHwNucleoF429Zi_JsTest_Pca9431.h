/**
  ******************************************************************************
  * @file alxHwNucleoF429Zi_JsTest_Pca9431.h
  * @brief Auralix HW NUCLEO-F429ZI C Library - JS Test Module PCA9431
  * @version $LastChangedRevision: 4937 $
  * @date $LastChangedDate: 2021-05-02 22:05:40 +0200 (Sun, 02 May 2021) $
  ******************************************************************************
  */

#ifndef ALX_HW_NUCLEO_F429ZI_JS_TEST_PCA9431_H
#define ALX_HW_NUCLEO_F429ZI_JS_TEST_PCA9431_H

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
// G01_PCA9431
//******************************************************************************
//******************************************************************************

//******************************************************************************
// Types
//******************************************************************************
typedef struct
{
	// Objects
	AlxHwNucleoF429Zi_Main alxHwNucleoF429Zi_Main;
	AlxPca9431 Pca9431;

	float LdoVout_Voltage_V;// = 0;
	float LdoVout_Current_A;// = 0;
	float Rect_Voltage_V;	// = 0;
	float Rect_Current_A;	// = 0;
	float SensTemp;			// = 0.001;

	uint8_t pca_DevAdrSend;	// = 0b11100010; // Pca9031
	uint8_t pca_DevAdrReceive; // = 0b11100011; //Pca9031
	// Info
	bool wasCtorCalled;
	bool isInit;
} AlxHwNucleoF429Zi_JsTest_G01_Pca9431;


//******************************************************************************
// Private Functions
//******************************************************************************
static inline void AlxHwNucleoF429Zi_JsTest_G01_BringUp_T01_Led(AlxHwNucleoF429Zi_JsTest_G01_Pca9431* me)
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
	static inline void AlxHwNucleoF429Zi_JsTest_G01_BringUp_T02_Trace(AlxHwNucleoF429Zi_JsTest_G01_Pca9431* me)
{
	(void)me;

	while(1)
	{
		ALX_TRACE_FORMAT("T02_Trace\r\n");
		AlxDelay_ms(100);
	}
}

	static inline void AlxHwNucleoF429Zi_JsTest_G01_Pca9431_T01_ReadAndClearInterrupt(AlxHwNucleoF429Zi_JsTest_G01_Pca9431* me)
{
	(void)me;

	AlxPca9431_Reg_ReadAndClearInterrupt(&me->Pca9431);
}
	static inline void AlxHwNucleoF429Zi_JsTest_G01_Pca9431_T02_LdoVout_GetVoltage_V(AlxHwNucleoF429Zi_JsTest_G01_Pca9431* me)
	{
		(void)me;

		AlxPca9431_LdoVout_GetVoltage_V(&me->Pca9431, &me->LdoVout_Voltage_V); // 10 bit ADC
		AlxTrace_WriteFormat(&alxTrace, "LdoVout_Voltage_V: %f \r\n", me->LdoVout_Voltage_V);
	}
	static inline void AlxHwNucleoF429Zi_JsTest_G01_Pca9431_T03_LdoVout_GetCurrent_A(AlxHwNucleoF429Zi_JsTest_G01_Pca9431* me)
	{
		(void)me;

		AlxPca9431_LdoVout_GetCurrent_A(&me->Pca9431, &me->LdoVout_Current_A); // 10 bit ADC
		AlxTrace_WriteFormat(&alxTrace, "LdoVout_GetCurrent_A: %f \r\n", me->LdoVout_Current_A);
	}
	static inline void AlxHwNucleoF429Zi_JsTest_G01_Pca9431_T04_Rect_GetVoltage_V(AlxHwNucleoF429Zi_JsTest_G01_Pca9431* me)
	{
		(void)me;

		AlxPca9431_Rect_GetVoltage_V(&me->Pca9431, &me->Rect_Voltage_V); // 10 bit ADC
		AlxTrace_WriteFormat(&alxTrace, "Rect_Voltage_V: %f \r\n", me->Rect_Voltage_V);
	}


//******************************************************************************
// Constructor & Functions
//******************************************************************************
static inline void AlxHwNucleoF429Zi_JsTest_G01_Pca9431_Ctor(AlxHwNucleoF429Zi_JsTest_G01_Pca9431* me)
{
	// Ctor
	AlxHwNucleoF429Zi_Main_Ctor(&me->alxHwNucleoF429Zi_Main);

	me->LdoVout_Voltage_V = 0;
	me->LdoVout_Current_A = 0;
	me->Rect_Voltage_V = 0;
	me->Rect_Current_A = 0;
	me->SensTemp = 0.001;
	me->pca_DevAdrSend = 0b11100010;
	me->pca_DevAdrReceive = 0b11100011;
	AlxPca9431_Ctor(&me->Pca9431, &me->alxHwNucleoF429Zi_Main.alxI2c_I2C2_Master, me->pca_DevAdrSend, &me->alxHwNucleoF429Zi_Main.alxIoPin.do_PA9_Pca9431_sleep, &me->alxHwNucleoF429Zi_Main.alxIoPin.do_PA9_Pca9431_sleep, false, 1, 1000);

	// Info
	me->wasCtorCalled = true;
}
	static inline void AlxHwNucleoF429Zi_JsTest_G01_Pca9431_Init(AlxHwNucleoF429Zi_JsTest_G01_Pca9431* me)
{


	// Init
	AlxClk_Init(&alxClk);
	AlxTrace_Init(&alxTrace);
	AlxPca9431_Init(&me->Pca9431);
	AlxIoPinIrq_Init(&me->alxHwNucleoF429Zi_Main.ioPinIrq);

	// Info
	me->isInit = true;
}
	static inline void AlxHwNucleoF429Zi_JsTest_G01_Pca9431_Run(AlxHwNucleoF429Zi_JsTest_G01_Pca9431* me)
{
	//AlxHwNucleoF429Zi_JsTest_G01_BringUp_T01_Led(me);
	//AlxHwNucleoF429Zi_JsTest_G01_BringUp_T02_Trace(me);
	while(1)
	{
		AlxHwNucleoF429Zi_JsTest_G01_Pca9431_T01_ReadAndClearInterrupt(me);
		AlxHwNucleoF429Zi_JsTest_G01_Pca9431_T02_LdoVout_GetVoltage_V(me);
		AlxHwNucleoF429Zi_JsTest_G01_Pca9431_T03_LdoVout_GetCurrent_A(me);
		AlxHwNucleoF429Zi_JsTest_G01_Pca9431_T04_Rect_GetVoltage_V(me);
		AlxDelay_ms(1000);
	}
}


//******************************************************************************
//******************************************************************************
// G02
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
} AlxHwNucleoF429Zi_JsTest_G02;


//******************************************************************************
// Private Functions
//******************************************************************************
static inline void AlxHwNucleoF429Zi_JsTest_G02_T01(AlxHwNucleoF429Zi_JsTest_G02* me)
{
	(void)me;
}
static inline void AlxHwNucleoF429Zi_JsTest_G02_T02(AlxHwNucleoF429Zi_JsTest_G02* me)
{
	(void)me;
}


//******************************************************************************
// Constructor & Functions
//******************************************************************************
static inline void AlxHwNucleoF429Zi_JsTest_G02_Ctor(AlxHwNucleoF429Zi_JsTest_G02* me)
{
	// Ctor
	AlxHwNucleoF429Zi_Main_Ctor(&me->alxHwNucleoF429Zi_Main);

	// Info
	me->wasCtorCalled = true;
}
static inline void AlxHwNucleoF429Zi_JsTest_G02_Init(AlxHwNucleoF429Zi_JsTest_G02* me)
{
	// Init
	AlxClk_Init(&alxClk);
	AlxTrace_Init(&alxTrace);

	// Info
	me->isInit = true;
}
static inline void AlxHwNucleoF429Zi_JsTest_G02_Run(AlxHwNucleoF429Zi_JsTest_G02* me)
{
	AlxHwNucleoF429Zi_JsTest_G02_T01(me);
	//AlxHwNucleoF429Zi_TvTest_G02_T02(me);
}


#endif // #if defined(ALX_HW_NUCLEO_F429ZI_C_TEST)

#ifdef __cplusplus
}
#endif

#endif // ALX_HW_NUCLEO_F429ZI_TV_TEST_H
