/**
  ******************************************************************************
  * @file alxHwNucleoF429Zi_MainTest.h
  * @brief Auralix HW NUCLEO-F429ZI C Library - Main Test Module
  * @version $LastChangedRevision: 4937 $
  * @date $LastChangedDate: 2021-05-02 22:05:40 +0200 (Sun, 02 May 2021) $
  ******************************************************************************
  */

#ifndef ALX_HW_NUCLEO_F429ZI_MAIN_TEST_H
#define ALX_HW_NUCLEO_F429ZI_MAIN_TEST_H

#ifdef __cplusplus
extern "C" {
#endif

//******************************************************************************
// Includes
//******************************************************************************
#include <alxHwNucleoF429Zi_Main.h>
//#include <alxHwNucleoF429Zi_GhTest.h>
//#include <alxHwNucleoF429Zi_GkTest.h>
//#include <alxHwNucleoF429Zi_JkTest.h>
//#include <alxHwNucleoF429Zi_MfTest.h>
#include <alxHwNucleoF429Zi_TvTest.h>


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_HW_NUCLEO_F429ZI_C_TEST)


//******************************************************************************
// Types
//******************************************************************************
typedef struct
{
	// Gh

	// Gk

	// Jk

	// Mf

	// Tv
	AlxHwNucleoF429Zi_TvTest_G01_BringUp alxHwNucleoF429Zi_TvTest_G01_BringUp;

} AlxHwNucleoF429Zi_MainTest;


//******************************************************************************
// Variables
//******************************************************************************
extern AlxHwNucleoF429Zi_MainTest alxHwNucleoF429Zi_MainTest;


//******************************************************************************
// Constructor & Functions
//******************************************************************************
static inline void AlxHwNucleoF429Zi_MainTest_Ctor(AlxHwNucleoF429Zi_MainTest* me)
{
	// Gh

	// Gk

	// Jk

	// Mf

	// Tv
	AlxHwNucleoF429Zi_TvTest_G01_BringUp_Ctor(&me->alxHwNucleoF429Zi_TvTest_G01_BringUp);
}
static inline void AlxHwNucleoF429Zi_MainTest_Init(AlxHwNucleoF429Zi_MainTest* me)
{
	// Gh

	// Gk

	// Jk

	// Mf

	// Tv
	AlxHwNucleoF429Zi_TvTest_G01_BringUp_Init(&me->alxHwNucleoF429Zi_TvTest_G01_BringUp);
}
static inline void AlxHwNucleoF429Zi_MainTest_Run(AlxHwNucleoF429Zi_MainTest* me)
{
	// Gh

	// Gk

	// Jk

	// Mf

	// Tv
	AlxHwNucleoF429Zi_TvTest_G01_BringUp_Run(&me->alxHwNucleoF429Zi_TvTest_G01_BringUp);
}


#endif // #if defined(ALX_HW_NUCLEO_F429ZI_C_TEST)

#ifdef __cplusplus
}
#endif

#endif // ALX_HW_NUCLEO_F429ZI_MAIN_TEST_H
