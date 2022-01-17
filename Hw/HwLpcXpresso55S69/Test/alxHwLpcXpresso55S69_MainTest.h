/**
  ******************************************************************************
  * @file alxHwLpcXpresso55S69_MainTest.h
  * @brief Auralix HW LPC Xpresso 55S69 C Library - Main Test Module
  * @version $LastChangedRevision: 4937 $
  * @date $LastChangedDate: 2021-05-02 22:05:40 +0200 (Sun, 02 May 2021) $
  ******************************************************************************
  */

#ifndef ALX_HW_LPC_XPRESSO_55S69_MAIN_TEST_H
#define ALX_HW_LPC_XPRESSO_55S69_MAIN_TEST_H

#ifdef __cplusplus
extern "C" {
#endif

//******************************************************************************
// Includes
//******************************************************************************
#include <alxHwLpcXpresso55S69_Main.h>
//#include <alxHwLpcXpresso55S69_GhTest.h>
//#include <alxHwLpcXpresso55S69_GkTest.h>
//#include <alxHwLpcXpresso55S69_JkTest.h>
#include <alxHwLpcXpresso55S69_MfTest.h>
//#include <alxHwLpcXpresso55S69_TvTest.h>


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_HW_LPC_XPRESSO_55S69_C_TEST)


//******************************************************************************
// Types
//******************************************************************************
typedef struct
{
	// Gh

	// Gk

	// Jk

	// Mf
	AlxHwLpcXpresso55S69_MfTest_G01_BringUp alxHwLpcXpresso55S69_MfTest_G01_BringUp;

	// Tv

} AlxHwLpcXpresso55S69_MainTest;


//******************************************************************************
// Variables
//******************************************************************************
extern AlxHwLpcXpresso55S69_MainTest alxHwLpcXpresso55S69_MainTest;


//******************************************************************************
// Constructor & Functions
//******************************************************************************
static inline void AlxHwLpcXpresso55S69_MainTest_Ctor(AlxHwLpcXpresso55S69_MainTest* me)
{
	// Gh

	// Gk

	// Jk

	// Mf
	AlxHwLpcXpresso55S69_MfTest_G01_BringUp_Ctor(&me->alxHwLpcXpresso55S69_MfTest_G01_BringUp);

	// Tv
}
static inline void AlxHwLpcXpresso55S69_MainTest_Init(AlxHwLpcXpresso55S69_MainTest* me)
{
	// Gh

	// Gk

	// Jk

	// Mf
	AlxHwLpcXpresso55S69_MfTest_G01_BringUp_Init(&me->alxHwLpcXpresso55S69_MfTest_G01_BringUp);

	// Tv
}
static inline void AlxHwLpcXpresso55S69_MainTest_Run(AlxHwLpcXpresso55S69_MainTest* me)
{
	// Gh

	// Gk

	// Jk

	// Mf
	AlxHwLpcXpresso55S69_MfTest_G01_BringUp_Run(&me->alxHwLpcXpresso55S69_MfTest_G01_BringUp);

	// Tv
}


#endif // #if defined(ALX_HW_LPC_XPRESSO_55S69_C_TEST)

#ifdef __cplusplus
}
#endif

#endif // ALX_HW_LPC_XPRESSO_55S69_MAIN_TEST_H
