/**
  ******************************************************************************
  * @file alxTrace_McuLpc55S6x.c
  * @brief Auralix C Library - ALX Trace Module
  * @version $LastChangedRevision: 4270 $
  * @date $LastChangedDate: 2021-03-05 19:02:52 +0100 (Fri, 05 Mar 2021) $
  ******************************************************************************
  */

//******************************************************************************
// Includes
//******************************************************************************
#include "alxTrace_McuLpc55S6x.h"
#include "alxTrace.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_LPC55S6X)


//******************************************************************************
// Private Functions
//******************************************************************************
static Alx_Status AlxTrace_ReInit(AlxTrace* me);
static void AlxTrace_Periph_EnableFlexcommSelectClk(AlxTrace* me);
static void AlxTrace_Periph_SelectClk(AlxTrace* me);


//******************************************************************************
// Specific Functions
//******************************************************************************
void AlxTrace_Ctor
(
	AlxTrace* me,
	uint8_t port,
	uint8_t pin,
	FLEXCOMM_Type* flexcomm,
	USART_Type* usart,
	AlxGlobal_BaudRate baudRate
)
{
	// Assert
	(void)me;
	(void)port;
	(void)pin;
	(void)flexcomm;
	(void)usart;
	(void)baudRate;

	// Parameters
	me->port = port;
	me->pin = pin;
	me->usart = usart;
	me->baudRate = (uint32_t)baudRate;

	// Variables
	me->usartConfig.baudRate_Bps = (uint32_t)baudRate;
	me->usartConfig.enableRx = false;
	me->usartConfig.enableTx = true;
	me->usartConfig.loopback = false;
	me->usartConfig.enableContinuousSCLK = false;
	me->usartConfig.parityMode = kUSART_ParityDisabled;
	me->usartConfig.stopBitCount = kUSART_OneStopBit;
	me->usartConfig.bitCountPerChar = kUSART_8BitsPerChar;
	me->usartConfig.syncMode = kUSART_SyncModeDisabled;
	me->usartConfig.clockPolarity = kUSART_RxSampleOnFallingEdge;

	// Info
	me->isInit = false;
	me->wasCtorCalled = true;
}
Alx_Status AlxTrace_Init(AlxTrace* me)
{
	// #1 Enable Flexcomm and select Clk source
	//AlxTrace_Periph_EnableFlexcommSelectClk(me);

	// #2 Init Flexcomm Interface
	//FLEXCOMM_Init(me->usart, FLEXCOMM_PERIPH_USART);	// MF: "Periph_Reset" and "EnableClk" happens here

	// #3 Init UART
	if (USART_Init(me->usart, &me->usartConfig, CLOCK_GetCoreSysClkFreq()) != kStatus_Success) { return Alx_Err; }		// MF: "Periph_Reset" and "EnableClk" happens here

	// #5 Set isInit
	me->isInit = true;

	// #6 Return OK
	return Alx_Ok;
}
Alx_Status AlxTrace_DeInit(AlxTrace* me)
{

	// #3 Reset isInit
	me->isInit = false;

	// #4 Return OK
	return Alx_Ok;
}
Alx_Status AlxTrace_WriteStr(AlxTrace* me, const char* str)
{
	// #1 Write
	if (USART_WriteBlocking(me->usart, (const uint8_t*)str, strlen(str)) != kStatus_Success)
	{
		AlxTrace_ReInit(me);
		return Alx_Err;
	}

	// #2 Return OK
	return Alx_Ok;
}


//******************************************************************************
// Private Functions
//******************************************************************************
static Alx_Status AlxTrace_ReInit(AlxTrace* me)
{
	// #1 DeInit Trace
	if (AlxTrace_DeInit(me) != Alx_Ok) { return Alx_Err; }

	// #2 Reset isInit
	me->isInit = false;

	// #3 Init Trace
	if (AlxTrace_Init(me) != Alx_Ok) { return Alx_Err; }

	// #4 Set isInit
	me->isInit = true;

	// #5 Return OK
	return Alx_Ok;
}
static void AlxTrace_Periph_EnableFlexcommSelectClk(AlxTrace* me)
{
	bool isErr = true;

	#if defined(FLEXCOMM0)
	if (me->flexcomm == FLEXCOMM0)	{ CLOCK_EnableClock(kCLOCK_FlexComm0); CLOCK_AttachClk(kMAIN_CLK_to_FLEXCOMM0); isErr = false; }
	#endif
	#if defined(FLEXCOMM1)
	if (me->flexcomm == FLEXCOMM1)	{ CLOCK_EnableClock(kCLOCK_FlexComm1); CLOCK_AttachClk(kMAIN_CLK_to_FLEXCOMM1); isErr = false; }
	#endif
	#if defined(FLEXCOMM2)
	if (me->flexcomm == FLEXCOMM2)	{ CLOCK_EnableClock(kCLOCK_FlexComm2); CLOCK_AttachClk(kMAIN_CLK_to_FLEXCOMM2); isErr = false; }
	#endif
	#if defined(FLEXCOMM3)
	if (me->flexcomm == FLEXCOMM3)	{ CLOCK_EnableClock(kCLOCK_FlexComm3); CLOCK_AttachClk(kMAIN_CLK_to_FLEXCOMM3); isErr = false; }
	#endif
	#if defined(FLEXCOMM4)
	if (me->flexcomm == FLEXCOMM4)	{ CLOCK_EnableClock(kCLOCK_FlexComm4); CLOCK_AttachClk(kMAIN_CLK_to_FLEXCOMM4); isErr = false; }
	#endif
	#if defined(FLEXCOMM5)
	if (me->flexcomm == FLEXCOMM5)	{ CLOCK_EnableClock(kCLOCK_FlexComm5); CLOCK_AttachClk(kMAIN_CLK_to_FLEXCOMM5); isErr = false; }
	#endif
	#if defined(FLEXCOMM6)
	if (me->flexcomm == FLEXCOMM6)	{ CLOCK_EnableClock(kCLOCK_FlexComm6); CLOCK_AttachClk(kMAIN_CLK_to_FLEXCOMM6); isErr = false; }
	#endif
	#if defined(FLEXCOMM7)
	if (me->flexcomm == FLEXCOMM7)	{ CLOCK_EnableClock(kCLOCK_FlexComm7); CLOCK_AttachClk(kMAIN_CLK_to_FLEXCOMM7); isErr = false; }
	#endif
	#if defined(FLEXCOMM8)
	if (me->flexcomm == FLEXCOMM8)	{ isErr = true; }	// MF: There is no "kCLOCK_FlexComm8" TODO
	#endif

	if (isErr)
	{
		// We shouldn't get here
	}
}

/*static void AlxTrace_Periph_SelectClk(AlxTrace* me)
{
	bool isErr = true;

	#if defined(USART0)
	if (me->usart == USART0)	{ CLOCK_Select(kUART0_Clk_From_MainClk); isErr = false; }
	#endif
	#if defined(USART1)
	if (me->usart == USART1)	{ CLOCK_Select(kUART1_Clk_From_MainClk); isErr = false; }
	#endif
	#if defined(USART2)
	if (me->usart == USART2)	{ CLOCK_Select(kUART2_Clk_From_MainClk); isErr = false; }
	#endif
	#if defined(USART3)
	if (me->usart == USART3)	{ CLOCK_Select(kUART3_Clk_From_MainClk); isErr = false; }
	#endif
	#if defined(USART4)
	if (me->usart == USART4)	{ CLOCK_Select(kUART4_Clk_From_MainClk); isErr = false; }
	#endif

	if (isErr)
	{
		// We shouldn't get here
	}
}*/


#endif // Module Guard
