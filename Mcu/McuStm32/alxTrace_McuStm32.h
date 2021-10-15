/**
  ******************************************************************************
  * @file alxTrace_McuStm32.h
  * @brief Auralix C Library - ALX Trace Module
  * @version $LastChangedRevision: 5455 $
  * @date $LastChangedDate: 2021-06-21 00:16:06 +0200 (Mon, 21 Jun 2021) $
  ******************************************************************************
  */

#ifndef ALX_TRACE_MCU_STM32_H
#define ALX_TRACE_MCU_STM32_H

#ifdef __cplusplus
extern "C" {
#endif

//******************************************************************************
// Includes
//******************************************************************************
#include "alxGlobal.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if (defined(ALX_STM32F1) || defined(ALX_STM32F4) || defined(ALX_STM32G4) || defined(ALX_STM32L0)) && (!defined(ALX_MBED))


//******************************************************************************
// Types
//******************************************************************************
typedef struct
{
	// Parameters
	GPIO_TypeDef* port;

	// Variables
	GPIO_InitTypeDef igpio;
	UART_HandleTypeDef huart;

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
	GPIO_TypeDef* port,
	uint16_t pin,
#if defined(ALX_STM32F4) || defined(ALX_STM32G4) || defined(ALX_STM32L0)
	uint32_t alternate,
#endif // defined(ALX_STM32F4) || defined(ALX_STM32G4) || defined(ALX_STM32L0)
	USART_TypeDef* uart,
	AlxGlobal_BaudRate baudRate
);


#endif

#ifdef __cplusplus
}
#endif

#endif // ALX_TRACE_MCU_STM32_H
