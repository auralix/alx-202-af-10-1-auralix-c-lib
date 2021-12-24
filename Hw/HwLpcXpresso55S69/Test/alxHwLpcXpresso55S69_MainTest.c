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
#include <alxHwLpcXpresso55S69_MainTest.h>


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_HW_LPC_XPRESSO_55S69_C_TEST)


//******************************************************************************
// Variables
//******************************************************************************
AlxHwLpcXpresso55S69_MainTest alxHwLpcXpresso55S69_MainTest = { 0 };


//******************************************************************************
// IRQ Handlers
//******************************************************************************
void SysTick_Handler(void)
{
	AlxTick_Inc_ms(&alxTick);
}


//******************************************************************************
// Auralix C Library - ALX IoPinIrq Module - Weak Functions
//******************************************************************************
/*void AlxIoPinIrq_Foreground_Callback_Pin0()
{
	AlxTrace_WriteStr(&alxTrace, "RiseEdge\r\n");
}
void AlxIoPinIrq_Foreground_Callback_Pin1()
{
	AlxTrace_WriteStr(&alxTrace, "FallEdge\r\n");
}*/


#endif // #if defined(ALX_HW_LPC_XPRESSO_55S69_C_TEST)
