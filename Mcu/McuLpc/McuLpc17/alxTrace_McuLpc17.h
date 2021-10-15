/**
  ******************************************************************************
  * @file alxTrace_McuLpc17.h
  * @brief Auralix C Library - ALX Trace Module
  * @version $LastChangedRevision: 5455 $
  * @date $LastChangedDate: 2021-06-21 00:16:06 +0200 (Mon, 21 Jun 2021) $
  ******************************************************************************
  */

#ifndef ALX_TRACE_MCU_LPC17_H
#define ALX_TRACE_MCU_LPC17_H

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
#if defined(ALX_LPC17)


//******************************************************************************
// Types
//******************************************************************************
typedef struct
{
	// Parameters
	uint8_t port;
	uint8_t pin;
	uint8_t func;
	LPC_USART_T* uart;
	uint32_t baudRate;

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
	uint8_t func,
	LPC_USART_T* uart,
	AlxGlobal_BaudRate baudRate
)
{
	// Parameters
	me->port = port;
	me->pin = pin;
	me->func = func;
	me->uart = uart;
	me->baudRate = (uint32_t)baudRate;

	// Info
	me->isInit = false;
	me->wasCtorCalled = true;
}


#endif

#ifdef __cplusplus
}
#endif

#endif // ALX_TRACE_MCU_LPC17_H
