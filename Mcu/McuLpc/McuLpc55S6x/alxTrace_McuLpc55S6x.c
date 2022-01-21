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
static uint32_t AlxTrace_GetFlexcommId(AlxTrace* me);
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

	// Info
	me->isInit = false;
	me->wasCtorCalled = true;
}


//******************************************************************************
// Functions
//******************************************************************************
Alx_Status AlxTrace_Init(AlxTrace* me)
{
	// #1 Set IoPin Usart Func
	IOCON_PinMuxSet(IOCON, me->port, me->pin, AlxTrace_GetIoconFunc(me));

	// #2 Attach Clk to FlexComm
	AlxTrace_AttachClkToFlexcomm(me);

	// #3 Init USART	// MF: FlexComm Init (Periph_Reset and EnableClk) happens in "USART_Init()". Also, USART0 has the same addres and ID as FLEXCOMM0, USART1 has the same addres and ID as FLEXCOMM1,... that's why I used me->usart in "CLOCK_GetFlexCommInputClock()"
	if (USART_Init(me->usart, &me->usartConfig, CLOCK_GetFlexCommInputClock(AlxTrace_GetFlexcommId(me))) != kStatus_Success) { return Alx_Err; }

	// #4 Set isInit
	me->isInit = true;

	// #5 Return OK
	return Alx_Ok;
}
Alx_Status AlxTrace_DeInit(AlxTrace* me)
{
	// #1 DeInit USART
	USART_Deinit(me->usart);

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
static uint8_t AlxTrace_GetIoconFunc(AlxTrace* me)
{
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
	if (isErr) { assert(true); }
	return 0xFF;
}

static uint32_t AlxTrace_GetFlexcommId(AlxTrace* me)
{
	// #1 Prepare Info Variable
	bool isErr = true;

	// #2 Get FlexComm ID
	#if defined(USART0)
	if (me->usart == USART0)	{ isErr = false; return 0; }
	#endif
	#if defined(USART1)
	if (me->usart == USART1)	{ isErr = false; return 1; }
	#endif
	#if defined(USART2)
	if (me->usart == USART2)	{ isErr = false; return 2; }
	#endif
	#if defined(USART3)
	if (me->usart == USART3)	{ isErr = false; return 3; }
	#endif
	#if defined(USART4)
	if (me->usart == USART4)	{ isErr = false; return 4; }
	#endif
	#if defined(USART5)
	if (me->usart == USART5)	{ isErr = false; return 5; }
	#endif
	#if defined(USART6)
	if (me->usart == USART6)	{ isErr = false; return 6; }
	#endif
	#if defined(USART7)
	if (me->usart == USART7)	{ isErr = false; return 7; }
	#endif

	// Assert
	if (isErr) { assert(true); }
	return 0xFFFFFFFF;
}
static void AlxTrace_AttachClkToFlexcomm(AlxTrace* me)
{
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
	if (isErr) { assert(true); }
}
static void AlxTrace_FlexcommDisableClkResetPeriph(AlxTrace* me)
{
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
	if (isErr) { assert(true); }
}

#endif // Module Guard
