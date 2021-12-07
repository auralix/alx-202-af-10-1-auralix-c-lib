/**
  ******************************************************************************
  * @file AlxHwNfcWlcListenerV3_5b_MainTest.h
  * @brief Auralix HW NUCLEO-F429ZI C Library - Main Test Module
  * @version $LastChangedRevision: 4937 $
  * @date $LastChangedDate: 2021-05-02 22:05:40 +0200 (Sun, 02 May 2021) $
  ******************************************************************************
  */

#ifndef ALX_HW_NFCWLC_LISTENER_V3_5B_MAIN_TEST_H
#define ALX_HW_NFCWLC_LISTENER_V3_5B_MAIN_TEST_H

#ifdef __cplusplus
extern "C" {
#endif

//******************************************************************************
// Includes
//******************************************************************************
#include <alxHwNfcWlcListenerV3_5b_Main.h>
//#include <alxHwNfcWlcListenerV3_5b_GhTest.h>
//#include <alxHwNfcWlcListenerV3_5b_GkTest.h>
//#include <alxHwNfcWlcListenerV3_5b_JkTest.h>
#include <alxHwNfcWlcListenerV3_5b_MfTest.h>
//#include <alxHwNfcWlcListenerV3_5b_TvTest.h>


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_HW_NFCWLC_LISTENER_V3_5B_C_TEST)


//******************************************************************************
// Types
//******************************************************************************
typedef struct
{
	// Gh

	// Gk

	// Jk

	// Mf
	AlxHwNfcWlcListenerV3_5b_Main_MfTest_G01_BringUp alxHwNfcWlcListenerV3_5b_Main_MfTest_G01_BringUp;

	// Tv
	//AlxHwNucleoF429Zi_TvTest_G01_BringUp alxHwNucleoF429Zi_TvTest_G01_BringUp;

} AlxHwNfcWlcListenerV3_5b_MainTest;


//******************************************************************************
// Variables
//******************************************************************************
extern AlxHwNfcWlcListenerV3_5b_MainTest alxHwNfcWlcListenerV3_5b_MainTest;


//******************************************************************************
// Constructor & Functions
//******************************************************************************
static inline void AlxHwNfcWlcListenerV3_5b_MainTest_Ctor(AlxHwNfcWlcListenerV3_5b_MainTest* me)
{
	// Gh

	// Gk

	// Jk

	// Mf
	AlxHwNfcWlcListenerV3_5b_Main_MfTest_G01_BringUp_Ctor(&me->alxHwNfcWlcListenerV3_5b_Main_MfTest_G01_BringUp);

	// Tv
	//AlxHwNucleoF429Zi_TvTest_G01_BringUp_Ctor(&me->alxHwNucleoF429Zi_TvTest_G01_BringUp);
}
static inline void AlxHwNfcWlcListenerV3_5b_MainTest_Init(AlxHwNfcWlcListenerV3_5b_MainTest* me)
{
	// Gh

	// Gk

	// Jk

	// Mf
	AlxHwNfcWlcListenerV3_5b_Main_MfTest_G01_BringUp_Init(&me->alxHwNfcWlcListenerV3_5b_Main_MfTest_G01_BringUp);

	// Tv
	//AlxHwNucleoF429Zi_TvTest_G01_BringUp_Init(&me->alxHwNucleoF429Zi_TvTest_G01_BringUp);
}
static inline void AlxHwNfcWlcListenerV3_5b_MainTest_Run(AlxHwNfcWlcListenerV3_5b_MainTest* me)
{
	// Gh

	// Gk

	// Jk

	// Mf
	AlxHwNfcWlcListenerV3_5b_Main_MfTest_G01_BringUp_Run(&me->alxHwNfcWlcListenerV3_5b_Main_MfTest_G01_BringUp);

	// Tv
	//AlxHwNucleoF429Zi_TvTest_G01_BringUp_Run(&me->alxHwNucleoF429Zi_TvTest_G01_BringUp);
}


#endif // #if defined(ALX_HW_NFCWLC_LISTENER_V3_5B_C_TEST)

#ifdef __cplusplus
}
#endif

#endif // ALX_HW_NFCWLC_LISTENER_V3_5B_MAIN_TEST_H
