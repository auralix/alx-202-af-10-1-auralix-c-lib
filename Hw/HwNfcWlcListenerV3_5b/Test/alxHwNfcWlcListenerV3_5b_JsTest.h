//****************************************************************************
// @file alxHwNfcWlcListenerV3_5b_JsTest.h
// @brief Auralix HW NFC WLC Listener V3.5 - JS Test Module
// @copyright Copyright (C) 2022 Auralix d.o.o. All rights reserved.
//****************************************************************************

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
#if defined(ALX_HW_NFC_WLC_LISTENER_V3_5B_C_TEST) && defined(ALX_TEST_JS)


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
// Constructor & Functions
//******************************************************************************
void AlxHwNfcWlcListenerV3_5b_JsTest_G01_BringUp_Ctor(AlxHwNfcWlcListenerV3_5b_JsTest_G01_BringUp* me);
void AlxHwNfcWlcListenerV3_5b_JsTest_G01_BringUp_Init(AlxHwNfcWlcListenerV3_5b_JsTest_G01_BringUp* me);
void AlxHwNfcWlcListenerV3_5b_JsTest_G01_BringUp_Run(AlxHwNfcWlcListenerV3_5b_JsTest_G01_BringUp* me);


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
	float SensTemp_degC;

	// Info
	bool wasCtorCalled;
	bool isInit;
} AlxHwNfcWlcListenerV3_5b_JsTest_G02_Pca9431;


//******************************************************************************
// Constructor & Functions
//******************************************************************************
void AlxHwNfcWlcListenerV3_5b_JsTest_G02_Pca9431_Ctor(AlxHwNfcWlcListenerV3_5b_JsTest_G02_Pca9431* me);
void AlxHwNfcWlcListenerV3_5b_JsTest_G02_Pca9431_Init(AlxHwNfcWlcListenerV3_5b_JsTest_G02_Pca9431* me);
void AlxHwNfcWlcListenerV3_5b_JsTest_G02_Pca9431_Run(AlxHwNfcWlcListenerV3_5b_JsTest_G02_Pca9431* me);


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
// Constructor & Functions
//******************************************************************************
void AlxHwNfcWlcListenerV3_5b_JsTest_G03_Crn120_Ctor(AlxHwNfcWlcListenerV3_5b_JsTest_G03_Crn120* me);
void AlxHwNfcWlcListenerV3_5b_JsTest_G03_Crn120_Init(AlxHwNfcWlcListenerV3_5b_JsTest_G03_Crn120* me);
void AlxHwNfcWlcListenerV3_5b_JsTest_G03_Crn120_Run(AlxHwNfcWlcListenerV3_5b_JsTest_G03_Crn120* me);


//******************************************************************************
//******************************************************************************
// G04_Pca9431_Optimize
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
	uint32_t LdoVout_Voltage_mV;
	uint32_t LdoVout_Current_uA;
	uint32_t Rect_Voltage_mV;
	uint32_t Rect_Current_uA;
	int32_t SensTemp_mDegC;

	// Info
	bool wasCtorCalled;
	bool isInit;
} AlxHwNfcWlcListenerV3_5b_JsTest_G04_Pca9431_Optimize;


//******************************************************************************
// Constructor & Functions
//******************************************************************************
void AlxHwNfcWlcListenerV3_5b_JsTest_G04_Pca9431_Optimize_Ctor(AlxHwNfcWlcListenerV3_5b_JsTest_G04_Pca9431_Optimize* me);
void AlxHwNfcWlcListenerV3_5b_JsTest_G04_Pca9431_Optimize_Init(AlxHwNfcWlcListenerV3_5b_JsTest_G04_Pca9431_Optimize* me);
void AlxHwNfcWlcListenerV3_5b_JsTest_G04_Pca9431_Optimize_Run(AlxHwNfcWlcListenerV3_5b_JsTest_G04_Pca9431_Optimize* me);


#endif // #if defined(ALX_HW_NFC_WLC_LISTENER_V3_5B_C_TEST)

#ifdef __cplusplus
}
#endif

#endif // ALX_HW_NFC_WLC_LISTENER_V3_5B_JS_TEST_H
