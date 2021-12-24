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
/*static swm_port_pin_type_t AlxTrace_GetSwmPortPinIndex(AlxTrace* me);
static swm_select_movable_t AlxTrace_GetUartFunc(AlxTrace* me);
static void AlxTrace_Periph_SelectClk(AlxTrace* me);*/


//******************************************************************************
// Specific Functions
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

#endif // Module Guard
