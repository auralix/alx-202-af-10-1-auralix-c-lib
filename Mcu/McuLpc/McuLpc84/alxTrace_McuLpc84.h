/**
  ******************************************************************************
  * @file alxTrace_McuLpc84.h
  * @brief Auralix C Library - ALX Trace Module
  * @version $LastChangedRevision: 5455 $
  * @date $LastChangedDate: 2021-06-21 00:16:06 +0200 (Mon, 21 Jun 2021) $
  ******************************************************************************
  */

#ifndef ALX_TRACE_MCU_LPC84_H
#define ALX_TRACE_MCU_LPC84_H

#ifdef __cplusplus
extern "C" {
#endif

//******************************************************************************
// Includes
//******************************************************************************
#include "alxGlobal.h"
#include "alxTick.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_LPC84)


//******************************************************************************
// Types
//******************************************************************************
typedef struct
{
	// Parameters
	uint8_t port;
	uint8_t pin;
	USART_Type* usart;
	AlxGlobal_BaudRate baudRate;

	// Variables
	usart_config_t usartConfig;

	// Info
	bool isInit;
	bool wasCtorCalled;
} AlxTrace;


//******************************************************************************
// Constructor
//******************************************************************************
static inline void AlxTrace_Ctor
(
	AlxTrace* me,
	uint8_t port,
	uint8_t pin,
	USART_Type* usart,
	AlxGlobal_BaudRate baudRate
)
{
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


#endif

#ifdef __cplusplus
}
#endif

#endif // ALIX_TRACE_MCU_LPC84_H
