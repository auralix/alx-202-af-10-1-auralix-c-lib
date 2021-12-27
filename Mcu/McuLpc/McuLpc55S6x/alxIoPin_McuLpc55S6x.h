﻿/**
  ******************************************************************************
  * @file alxIoPin_McuLpc55S6x.h
  * @brief Auralix C Library - ALX IO Pin Module
  * @version $LastChangedRevision: 4270 $
  * @date $LastChangedDate: 2021-03-05 19:02:52 +0100 (Fri, 05 Mar 2021) $
  ******************************************************************************
  */

#ifndef ALX_IO_PIN_MCU_LPC55S6X_H
#define ALX_IO_PIN_MCU_LPC55S6X_H

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
#if defined(ALX_LPC55S6X)


//******************************************************************************
// Types
//******************************************************************************
typedef enum
{
	// SWM Movable
	AlxIoPin_Func_GPIO	= IOCON_FUNC0,
	AlxIoPin_Func_1		= IOCON_FUNC1,
	AlxIoPin_Func_2		= IOCON_FUNC2,
	AlxIoPin_Func_3		= IOCON_FUNC3,
	AlxIoPin_Func_4		= IOCON_FUNC4,
	AlxIoPin_Func_5		= IOCON_FUNC5,
	AlxIoPin_Func_6		= IOCON_FUNC6,
	AlxIoPin_Func_7		= IOCON_FUNC7,
	AlxIoPin_Func_8		= IOCON_FUNC8,
	AlxIoPin_Func_9		= IOCON_FUNC9,
	AlxIoPin_Func_10	= IOCON_FUNC10,
	AlxIoPin_Func_11	= IOCON_FUNC11,
} AlxIoPin_Iocon_Func;


typedef struct
{
	// Parameters
	uint8_t port;
	uint8_t pin;
	AlxIoPin_Iocon_Func func;
	uint32_t mode;	// MF: PullUp or PulDown
	bool isOpenDrain;
	bool dir;	// MF: True = digital output, False = digital input
	bool val;

	// Variables
	//bool swmFunc_isMovable;

	// Info
	bool isInit;
	bool wasCtorCalled;
} AlxIoPin;


//******************************************************************************
// Constructor
//******************************************************************************
void AlxIoPin_Ctor
(
	AlxIoPin* me,
	uint8_t port,
	uint8_t pin,
	AlxIoPin_Iocon_Func func,
	uint32_t mode,
	bool isOpenDrain,
	bool dir,
	bool val
);


#endif // Module Guard

#ifdef __cplusplus
}
#endif

#endif // ALX_IO_PIN_MCU_LPC55S6X_H
