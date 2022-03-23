/**
  ******************************************************************************
  * @file alxTrace_McuLpc55S6x.h
  * @brief Auralix C Library - ALX Trace Module
  * @version $LastChangedRevision: 4270 $
  * @date $LastChangedDate: 2021-03-05 19:02:52 +0100 (Fri, 05 Mar 2021) $
  ******************************************************************************
  */

#ifndef ALX_TRACE_MCU_LPC55S6X_H
#define ALX_TRACE_MCU_LPC55S6X_H

#ifdef __cplusplus
extern "C" {
#endif

//******************************************************************************
// Includes
//******************************************************************************
#include "alxGlobal.h"
#include "alxTick.h"
#if defined(ALX_OS)
#include "alxOsMutex.h"
#endif


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_LPC55S6X)


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
	#if defined(ALX_OS)
	AlxOsMutex mutex;
	#endif

	// Info
	bool isInit;
	bool wasCtorCalled;
} AlxTrace;


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
);

#endif // Module Guard

#ifdef __cplusplus
}
#endif

#endif // ALX_TRACE_MCU_LPC55S6X_H
