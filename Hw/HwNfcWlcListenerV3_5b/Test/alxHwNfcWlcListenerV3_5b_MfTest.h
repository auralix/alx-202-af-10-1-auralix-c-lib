/**
  ******************************************************************************
  * @file alxHwNucleoF429Zi_TvTest.h
  * @brief Auralix HW NUCLEO-F429ZI C Library - TV Test Module
  * @version $LastChangedRevision: 4937 $
  * @date $LastChangedDate: 2021-05-02 22:05:40 +0200 (Sun, 02 May 2021) $
  ******************************************************************************
  */

#ifndef ALX_HW_NFCWLC_LISTENER_V3_5B_MF_TEST_H
#define ALX_HW_NFCWLC_LISTENER_V3_5B_MF_TEST_H

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
#if defined(ALX_HW_NFCWLC_LISTENER_V3_5B_C_TEST)


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
	AlxIoPin_Init(&me->alxHwNfcWlcListenerV3_5b_Main.alxIoPin.do_P0_18_LED205_RD);
	AlxIoPin_Init(&me->alxHwNfcWlcListenerV3_5b_Main.alxIoPin.do_P0_19_LED200_GR);
	AlxIoPin_Init(&me->alxHwNfcWlcListenerV3_5b_Main.alxIoPin.do_P0_22_LED204_GR);
	AlxIoPin_Init(&me->alxHwNfcWlcListenerV3_5b_Main.alxIoPin.do_P0_23_LED203_GR);
	AlxIoPin_Init(&me->alxHwNfcWlcListenerV3_5b_Main.alxIoPin.do_P0_24_LED202_GR);
	AlxIoPin_Init(&me->alxHwNfcWlcListenerV3_5b_Main.alxIoPin.do_P0_25_LED201_GR);

	while (1)
	{
		AlxIoPin_Toggle(&me->alxHwNfcWlcListenerV3_5b_Main.alxIoPin.do_P0_18_LED205_RD);
		AlxIoPin_Toggle(&me->alxHwNfcWlcListenerV3_5b_Main.alxIoPin.do_P0_19_LED200_GR);
		AlxIoPin_Toggle(&me->alxHwNfcWlcListenerV3_5b_Main.alxIoPin.do_P0_22_LED204_GR);
		AlxIoPin_Toggle(&me->alxHwNfcWlcListenerV3_5b_Main.alxIoPin.do_P0_23_LED203_GR);
		AlxIoPin_Toggle(&me->alxHwNfcWlcListenerV3_5b_Main.alxIoPin.do_P0_24_LED202_GR);
		AlxIoPin_Toggle(&me->alxHwNfcWlcListenerV3_5b_Main.alxIoPin.do_P0_25_LED201_GR);
		AlxDelay_ms(500);
	}
}
static inline void AlxHwNfcWlcListenerV3_5b_Main_MfTest_G01_BringUp_T02_Trace(AlxHwNfcWlcListenerV3_5b_Main_MfTest_G01_BringUp* me)
{
	(void)me;

	while (1)
	{
		ALX_TRACE_FORMAT("T02_Trace\r\n");
		AlxDelay_ms(1000);
	}
}
static inline void AlxHwNfcWlcListenerV3_5b_Main_MfTest_G01_BringUp_T03_Adc(AlxHwNfcWlcListenerV3_5b_Main_MfTest_G01_BringUp* me)
{
	(void)me;

	while (1)
	{
		ALX_TRACE_FORMAT("T02_Trace\r\n");
		AlxDelay_ms(1000);
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

	// Info
	me->isInit = true;
}
	static inline void AlxHwNfcWlcListenerV3_5b_Main_MfTest_G01_BringUp_Run(AlxHwNfcWlcListenerV3_5b_Main_MfTest_G01_BringUp* me)
{
	AlxHwNfcWlcListenerV3_5b_Main_MfTest_G01_BringUp_T01_Led(me);
	//AlxHwNfcWlcListenerV3_5b_Main_MfTest_G01_BringUp_T02_Trace(me);
}


#endif // #if defined(ALX_HW_NFCWLC_LISTENER_V3_5B_C_TEST)

#ifdef __cplusplus
}
#endif

#endif // ALX_HW_NFCWLC_LISTENER_V3_5B_MF_TEST_H
