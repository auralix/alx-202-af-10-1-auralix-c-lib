/**
  ******************************************************************************
  * @file alxHwNfcWlcListenerV3_5b_MainTest.c
  * @brief Auralix HW NFC WLC Listener V3_5 C Library - Main Test Module
  * @version $LastChangedRevision: 4937 $
  * @date $LastChangedDate: 2021-05-02 22:05:40 +0200 (Sun, 02 May 2021) $
  ******************************************************************************
  */


//******************************************************************************
// Includes
//******************************************************************************
#include <alxHwNfcWlcListenerV3_5b_MainTest.h>


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_HW_NFCWLC_LISTENER_V3_5B_C_TEST)


//******************************************************************************
// Variables
//******************************************************************************
AlxHwNfcWlcListenerV3_5b_MainTest alxHwNfcWlcListenerV3_5b_MainTest = { 0 };


//******************************************************************************
// IRQ Handlers
//******************************************************************************
void SysTick_Handler(void)
{
	AlxTick_Inc_ms(&alxTick);
}


//******************************************************************************
// Auralix C Library - ALX 01 Module - Weak Functions
//******************************************************************************
void AlxIoPinIrq_Foreground_Callback_Pin0()
{
	AlxTrace_WriteStr(&alxTrace, "RiseEdge\r\n");
}
void AlxIoPinIrq_Foreground_Callback_Pin1()
{
	AlxTrace_WriteStr(&alxTrace, "FallEdge\r\n");
}

//******************************************************************************
// Auralix C Library - ALX 02 Module - Weak Functions
//******************************************************************************


#endif // #if defined(ALX_HW_NFCWLC_LISTENER_V3_5B_C_TEST)
