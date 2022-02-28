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
	#if defined(ALX_FREE_RTOS)
	struct rtos_usart_config usartRtosConfig;
	usart_rtos_handle_t usartRtosHandle;
	struct _usart_handle usartHandle;
	uint8_t rxDummyBuffer[1];	// MF: Rx won't be used and NULL ptr cannot be used for Rx buff
	#else
	usart_config_t usartConfig;
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
