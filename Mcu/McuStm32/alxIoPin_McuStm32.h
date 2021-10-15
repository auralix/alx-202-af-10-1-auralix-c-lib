/**
  ******************************************************************************
  * @file alxIoPin_McuStm32.h
  * @brief Auralix C Library - ALX IO Pin Module
  * @version $LastChangedRevision: 5236 $
  * @date $LastChangedDate: 2021-05-26 12:47:46 +0200 (Wed, 26 May 2021) $
  ******************************************************************************
  */

#ifndef ALX_IO_PIN_MCU_STM32_H
#define ALX_IO_PIN_MCU_STM32_H

#ifdef __cplusplus
extern "C" {
#endif

//******************************************************************************
// Includes
//******************************************************************************
#include "alxGlobal.h"
#include "alxTrace.h"
#include "alxAssert.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_STM32F1) || defined(ALX_STM32F4) || defined(ALX_STM32G4) || defined(ALX_STM32L0)


//******************************************************************************
// Types
//******************************************************************************
typedef struct
{
	// Parameters
	GPIO_TypeDef* port;
	bool val;

	// Variables
	GPIO_InitTypeDef igpio;

	// Info
	bool isInit;
	bool wasCtorCalled;
	bool isAssertOn;
} AlxIoPin;


//******************************************************************************
// Constructor
//******************************************************************************
void AlxIoPin_Ctor
(
	AlxIoPin* me,
	GPIO_TypeDef* port,
	uint16_t pin,
	uint32_t mode,
	uint32_t pull,
	uint32_t speed,
#if defined(ALX_STM32F4) || defined(ALX_STM32G4) || defined(ALX_STM32L0)
	uint32_t alternate,	  
#endif // defined(ALX_STM32F4) || defined(ALX_STM32G4) || defined(ALX_STM32L0)
	bool val
);


#endif

#ifdef __cplusplus
}
#endif

#endif // ALX_IO_PIN_MCU_STM32_H
