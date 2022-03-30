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
AlxHwLpcXpresso55S69_MainTest alxHwLpcXpresso55S69_MainTest = {0};


//******************************************************************************
// IRQ Handlers
//******************************************************************************
#if defined(ALX_FREE_RTOS)
void vApplicationTickHook(void)
{
	AlxTick_IncRange_ms(&alxTick, (uint64_t)portTICK_PERIOD_MS);
}
#else
void SysTick_Handler(void)
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
	#if defined(ALX_FREE_RTOS)
	// Toggle Pin
	//GPIO->NOT[1] = (1U << 9);

	// Increment Global Variable
	UBaseType_t returnVal = taskENTER_CRITICAL_FROM_ISR();	// MF: This prevents nested IRQs to occur but it might add delay or miss other IRQs if they’re triggered while in this critical section.
	G02_counter++;
	taskEXIT_CRITICAL_FROM_ISR(returnVal);

	// Trace - Using Semaphore
	BaseType_t taskWokenSem = pdFALSE;
	xSemaphoreGiveFromISR(IsrTraceSem, &taskWokenSem);
	portYIELD_FROM_ISR(taskWokenSem);

	// Trace - Using Notifications
	BaseType_t taskWokenNotify = pdFALSE;
	vTaskNotifyGiveFromISR(T11_TraceIsrHandle, &taskWokenNotify);
	portYIELD_FROM_ISR(taskWokenNotify);
	#else
	AlxTrace_WriteStr(&alxTrace, "RiseEdge\r\n");
	#endif
}
void AlxIoPinIrq_Foreground_Callback_Pin1()
{
	AlxTrace_WriteStr(&alxTrace, "FallEdge\r\n");
}

// G03_IoExpander
void AlxPcal6416a_RegStruct_SetVal(AlxPcal6416a* me)
{
	(void)me;
	me->reg._06h_Configuration_0.val.P0_0 = false;	// MF: P0_0 set as Output
	me->reg._06h_Configuration_0.val.P0_1 = false;	// MF: P0_1 set as Output
	me->reg._06h_Configuration_0.val.P0_2 = true;	// MF: P0_2 set as Input
}
#endif

///-------
/// Tv
///-------
#if defined(ALX_TEST_TV)
#endif


#endif // #if defined(ALX_HW_LPC_XPRESSO_55S69_C_TEST)
