/**
  ******************************************************************************
  * @file alxHwLpcXpresso55S69_MainTest.c
  * @brief Auralix HW LPC Xpresso 55S69 C Library - Main Test Module
  * @version $LastChangedRevision: 4937 $
  * @date $LastChangedDate: 2021-05-02 22:05:40 +0200 (Sun, 02 May 2021) $
  ******************************************************************************
  */


//******************************************************************************
// Includes
//******************************************************************************
#include <alxConfig.h>
#include <alxHwLpcXpresso55S69_MainTest.h>


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_HW_LPC_XPRESSO_55S69_C_TEST)


//******************************************************************************
// IRQ Handlers
//******************************************************************************
#if defined(ALX_FREE_RTOS)
void vApplicationTickHook(void)
{
	AlxTick_Inc_ms(&alxTick);
}
#elsevoid SysTick_Handler(void)
{
	AlxTick_Inc_ms(&alxTick);
}
#endif



//******************************************************************************
// Auralix C Library - Weak Functions
//******************************************************************************

///-------
/// Gh
///-------
#if defined(ALX_TEST_GH)
#endif

///-------
/// Gk
///-------
#if defined(ALX_TEST_GK)
#endif

///-------
/// Jk
///-------
#if defined(ALX_TEST_JK)
#endif

///-------
/// Mf
///-------
#if defined(ALX_TEST_MF)
void AlxIoPinIrq_Foreground_Callback_Pin0()
{
	AlxTrace_WriteStr(&alxTrace, "RiseEdge\r\n");
}
void AlxIoPinIrq_Foreground_Callback_Pin1()
{
	AlxTrace_WriteStr(&alxTrace, "FallEdge\r\n");
}
#endif

///-------
/// Tv
///-------
#if defined(ALX_TEST_TV)
#endif

#endif // #if defined(ALX_HW_LPC_XPRESSO_55S69_C_TEST)
