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
static uint8_t AlxTrace_GetIoconFunc(AlxTrace* me);
static uint32_t AlxTrace_GetFlexCommClkFreq(AlxTrace* me);
static void AlxTrace_AttachClkToFlexcomm(AlxTrace* me);
static void AlxTrace_FlexcommDisableClkResetPeriph(AlxTrace* me);


//******************************************************************************
// Constructor
//******************************************************************************
void AlxTrace_Ctor
(
	AlxTrace* me,
	uint8_t port,
	uint8_t pin,
	USART_Type* usart,
	AlxGlobal_BaudRate baudRate
)
{
	// Assert
	(void)me;
	(void)port;
	(void)pin;
	(void)usart;
	(void)baudRate;

	// Parameters
	me->port = port;
	me->pin = pin;
	me->usart = usart;
	me->baudRate = (uint32_t)baudRate;

	// Variables
	#if defined(ALX_FREE_RTOS)
	me->usartRtosConfig.base = me->usart;
	me->usartRtosConfig.srcclk = 0;									// MF: This will be set in Init because "AlxClk_Init" has to be called first
	me->usartRtosConfig.baudrate = me->baudRate;
	me->usartRtosConfig.parity = kUSART_ParityDisabled;
	me->usartRtosConfig.stopbits = kUSART_OneStopBit;
	me->usartRtosConfig.buffer = me->rxDummyBuffer;					// MF: Rx won't be used and NULL ptr cannot be used for Rx buff
	me->usartRtosConfig.buffer_size = sizeof(me->rxDummyBuffer);	// MF: Rx won't be used and NULL ptr cannot be used for Rx buff
	#else
	me->usartConfig.baudRate_Bps				= (uint32_t)baudRate;
	me->usartConfig.parityMode					= kUSART_ParityDisabled;
	me->usartConfig.stopBitCount				= kUSART_OneStopBit;
	me->usartConfig.bitCountPerChar				= kUSART_8BitsPerChar;
	me->usartConfig.loopback					= false;
	me->usartConfig.enableRx					= false;
	me->usartConfig.enableTx					= true;
	me->usartConfig.enableContinuousSCLK		= false;
	me->usartConfig.enableMode32k				= false;
	me->usartConfig.enableHardwareFlowControl	= false;
	me->usartConfig.txWatermark					= kUSART_TxFifo0;	// MF: I don't understand this
	me->usartConfig.rxWatermark					= kUSART_RxFifo1;	// MF: I don't understand this
	me->usartConfig.syncMode					= kUSART_SyncModeDisabled;
	me->usartConfig.clockPolarity				= kUSART_RxSampleOnFallingEdge;
	#endif

	// Info
	me->isInit = false;
	me->wasCtorCalled = true;
}


//******************************************************************************
// Functions
//******************************************************************************
Alx_Status AlxTrace_Init(AlxTrace* me)
{
	// Assert
	(void)me;

	// #1 Set IoPin Usart Func
	IOCON_PinMuxSet(IOCON, me->port, me->pin, AlxTrace_GetIoconFunc(me));

	// #2 Attach Clk to FlexComm
	AlxTrace_AttachClkToFlexcomm(me);

	// #3 Init USART	// MF: FlexComm Init (Periph_Reset and EnableClk) happens in "USART_Init()"
	#if defined(ALX_FREE_RTOS)
	me->usartRtosConfig.srcclk = AlxTrace_GetFlexCommClkFreq(me);	// MF: Set Clk Freq

	if (USART_RTOS_Init(&me->usartRtosHandle, &me->usartHandle, &me->usartRtosConfig) != kStatus_Success) { return Alx_Err; }
	#else
	if (USART_Init(me->usart, &me->usartConfig, AlxTrace_GetFlexCommClkFreq(me)) != kStatus_Success) { return Alx_Err; }
	#endif

	// #4 Set isInit
	me->isInit = true;

	// #5 Return OK
	return Alx_Ok;
}
Alx_Status AlxTrace_DeInit(AlxTrace* me)
{
	// Assert
	(void)me;

	// #1 DeInit USART
	#if defined(ALX_FREE_RTOS)
	USART_RTOS_Deinit(&me->usartRtosHandle);	// MF: Always returns Success, so we won't hande return
	#else
	USART_Deinit(me->usart);
	#endif

	// #2 Disable FlexComm Clk and Reset FlexComm Perihpery
	AlxTrace_FlexcommDisableClkResetPeriph(me);

	// #3 Set Pin to Default Func
	IOCON_PinMuxSet(IOCON, me->port, me->pin, IOCON_FUNC0);

	// #4 Reset isInit
	me->isInit = false;

	// #5 Return OK
	return Alx_Ok;
}
Alx_Status AlxTrace_WriteStr(AlxTrace* me, const char* str)
{
	// Assert
	(void)me;
	(void)str;

	// #1 Write
	#if defined(ALX_FREE_RTOS)
	if (USART_RTOS_Send(&me->usartRtosHandle, (uint8_t*)str, strlen(str)) != kStatus_Success)
	{
		AlxTrace_ReInit(me);
		return Alx_Err;
	}
	#else
	if (USART_WriteBlocking(me->usart, (const uint8_t*)str, strlen(str)) != kStatus_Success)
	{
		AlxTrace_ReInit(me);
		return Alx_Err;
	}
	#endif

	// #2 Return OK
	return Alx_Ok;
}


//******************************************************************************
// Private Functions
//******************************************************************************
static Alx_Status AlxTrace_ReInit(AlxTrace* me)
{
	// Assert
	(void)me;

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
static uint8_t AlxTrace_GetIoconFunc(AlxTrace* me)
{
	// Assert
	(void)me;

	// #1 Prepare Info Variable
	bool isErr = true;

	// #2 Get IOCON Func (see Table 340-342 in User Manual on page 343)
	#if defined(USART0)
	if (me->usart == USART0)						{ isErr = false; return IOCON_FUNC1; }
	#endif
	#if defined(USART1)
	if (me->usart == USART1)
	{
		if		(me->port == 0 && me->pin == 10)	{ isErr = false; return IOCON_FUNC4; }
		else if (me->port == 0 && me->pin == 14)	{ isErr = false; return IOCON_FUNC6; }
		else if (me->port == 1 && me->pin == 11)	{ isErr = false; return IOCON_FUNC2; }
		else										{ isErr = true; }
	}
	#endif
	#if defined(USART2)
	if (me->usart == USART2)						{ isErr = false; return IOCON_FUNC1; }
	#endif
	#if defined(USART3)
	if (me->usart == USART3)						{ isErr = false; return IOCON_FUNC1; }
	#endif
	#if defined(USART4)
	if (me->usart == USART4)
	{
		if		(me->port == 0 && me->pin == 16)	{ isErr = false; return IOCON_FUNC1; }
		else if (me->port == 0 && me->pin == 20)	{ isErr = false; return IOCON_FUNC11; }
		else if (me->port == 1 && me->pin == 20)	{ isErr = false; return IOCON_FUNC5; }
		else										{ isErr = true; }
	}
	#endif
	#if defined(USART5)
	if (me->usart == USART5)						{ isErr = false; return IOCON_FUNC3; }
	#endif
	#if defined(USART6)
	if (me->usart == USART6)
	{
		if		(me->port == 0 && me->pin == 12)	{ isErr = false; return IOCON_FUNC7; }
		else if (me->port == 0 && me->pin == 22)	{ isErr = false; return IOCON_FUNC1; }
		else if (me->port == 1 && me->pin == 16)	{ isErr = false; return IOCON_FUNC2; }
		else										{ isErr = true; }
	}
	#endif
	#if defined(USART7)
	if (me->usart == USART7)
	{
		if		(me->port == 0 && me->pin == 19)	{ isErr = false; return IOCON_FUNC7; }
		else if (me->port == 1 && me->pin == 30)	{ isErr = false; return IOCON_FUNC1; }
		else										{ isErr = true; }
	}
	#endif

	// Assert
	if (isErr)
	{
		// We shouldn't get here
	}
	return 0xFF;
}
static uint32_t AlxTrace_GetFlexCommClkFreq(AlxTrace* me)
{
	// Assert
	(void)me;

	// #1 Prepare Info Variable
	bool isErr = true;

	// #1 Get FlexComm Clk Freq
	#if defined(USART0)
	if (me->usart == USART0)	{ isErr = false; return CLOCK_GetFlexCommClkFreq(0U); }
	#endif
	#if defined(USART1)
	if (me->usart == USART1)	{ isErr = false; return CLOCK_GetFlexCommClkFreq(1U); }
	#endif
	#if defined(USART2)
	if (me->usart == USART2)	{ isErr = false; return CLOCK_GetFlexCommClkFreq(2U); }
	#endif
	#if defined(USART3)
	if (me->usart == USART3)	{ isErr = false; return CLOCK_GetFlexCommClkFreq(3U); }
	#endif
	#if defined(USART4)
	if (me->usart == USART4)	{ isErr = false; return CLOCK_GetFlexCommClkFreq(4U); }
	#endif
	#if defined(USART5)
	if (me->usart == USART5)	{ isErr = false; return CLOCK_GetFlexCommClkFreq(5U); }
	#endif
	#if defined(USART6)
	if (me->usart == USART6)	{ isErr = false; return CLOCK_GetFlexCommClkFreq(6U); }
	#endif
	#if defined(USART7)
	if (me->usart == USART7)	{ isErr = false; return CLOCK_GetFlexCommClkFreq(7U); }
	#endif

	// Assert
	if (isErr)
	{
		// We shouldn't get here
	}
	return 0xFFFFFFFF;
}
static void AlxTrace_AttachClkToFlexcomm(AlxTrace* me)
{
	// Assert
	(void)me;

	// #1 Prepare Info Variable
	bool isErr = true;

	// #2 FlexComm Disable Clk and Reset Periphery
	#if defined(USART0)
	if (me->usart == USART0)	{ CLOCK_AttachClk(kMAIN_CLK_to_FLEXCOMM0); isErr = false; }
	#endif
	#if defined(USART1)
	if (me->usart == USART1)	{ CLOCK_AttachClk(kMAIN_CLK_to_FLEXCOMM1); isErr = false; }
	#endif
	#if defined(USART2)
	if (me->usart == USART2)	{ CLOCK_AttachClk(kMAIN_CLK_to_FLEXCOMM2); isErr = false; }
	#endif
	#if defined(USART3)
	if (me->usart == USART3)	{ CLOCK_AttachClk(kMAIN_CLK_to_FLEXCOMM3); isErr = false; }
	#endif
	#if defined(USART4)
	if (me->usart == USART4)	{ CLOCK_AttachClk(kMAIN_CLK_to_FLEXCOMM4); isErr = false; }
	#endif
	#if defined(USART5)
	if (me->usart == USART5)	{ CLOCK_AttachClk(kMAIN_CLK_to_FLEXCOMM5); isErr = false; }
	#endif
	#if defined(USART6)
	if (me->usart == USART6)	{ CLOCK_AttachClk(kMAIN_CLK_to_FLEXCOMM6); isErr = false; }
	#endif
	#if defined(USART7)
	if (me->usart == USART7)	{ CLOCK_AttachClk(kMAIN_CLK_to_FLEXCOMM7); isErr = false; }
	#endif

	// Assert
	if (isErr)
	{
		// We shouldn't get here
	}
}
static void AlxTrace_FlexcommDisableClkResetPeriph(AlxTrace* me)
{
	// Assert
	(void)me;

	// #1 Prepare Info Variable
	bool isErr = true;

	// #2 FlexComm Disable Clk and Reset Periphery
	#if defined(USART0)
	if (me->usart == USART0)	{ CLOCK_DisableClock(kCLOCK_FlexComm0); RESET_PeripheralReset(kFC0_RST_SHIFT_RSTn); isErr = false; }
	#endif
	#if defined(USART1)
	if (me->usart == USART1)	{ CLOCK_DisableClock(kCLOCK_FlexComm1); RESET_PeripheralReset(kFC1_RST_SHIFT_RSTn); isErr = false; }
	#endif
	#if defined(USART2)
	if (me->usart == USART2)	{ CLOCK_DisableClock(kCLOCK_FlexComm2); RESET_PeripheralReset(kFC2_RST_SHIFT_RSTn); isErr = false; }
	#endif
	#if defined(USART3)
	if (me->usart == USART3)	{ CLOCK_DisableClock(kCLOCK_FlexComm3); RESET_PeripheralReset(kFC3_RST_SHIFT_RSTn); isErr = false; }
	#endif
	#if defined(USART4)
	if (me->usart == USART4)	{ CLOCK_DisableClock(kCLOCK_FlexComm4); RESET_PeripheralReset(kFC4_RST_SHIFT_RSTn); isErr = false; }
	#endif
	#if defined(USART5)
	if (me->usart == USART5)	{ CLOCK_DisableClock(kCLOCK_FlexComm5); RESET_PeripheralReset(kFC5_RST_SHIFT_RSTn); isErr = false; }
	#endif
	#if defined(USART6)
	if (me->usart == USART6)	{ CLOCK_DisableClock(kCLOCK_FlexComm6); RESET_PeripheralReset(kFC6_RST_SHIFT_RSTn); isErr = false; }
	#endif
	#if defined(USART7)
	if (me->usart == USART7)	{ CLOCK_DisableClock(kCLOCK_FlexComm7); RESET_PeripheralReset(kFC7_RST_SHIFT_RSTn); isErr = false; }
	#endif

	// Assert
	if (isErr)
	{
		// We shouldn't get here
	}
}

#endif // Module Guard
