﻿/**
  ******************************************************************************
  * @file alxPwm_McuLpc80x.h
  * @brief Auralix C Library - ALX PWM Module
  * @version $LastChangedRevision: 4270 $
  * @date $LastChangedDate: 2021-03-05 19:02:52 +0100 (Fri, 05 Mar 2021) $
  ******************************************************************************
  */

#ifndef ALX_PWM_MCU_LPC80X_H
#define ALX_PWM_MCU_LPC80X_H

#ifdef __cplusplus
extern "C" {
#endif

//******************************************************************************
// Includes
//******************************************************************************
#include "alxGlobal.h"
#include "alxTrace.h"
#include "alxAssert.h"
#include "alxIoPin.h"
#include "alxClk.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_LPC80x)


//******************************************************************************
// Types
//******************************************************************************
typedef struct
{
	// Objects - External
	AlxIoPin** ioPinArr;
	AlxClk* clk;
	
	// Parameters
	CTIMER_Type* tim;
	Alx_Ch* chArr;
	float* dutyDefaultArr_pct;
	uint8_t numOfCh;

	// Variables
	ctimer_config_t config; // Prescaler is in "ctimer_config_t"
	uint32_t srcClk_Hz; // Timer source clock, which is then divided by prescaler to get timer clock
	uint32_t periodMax;
	uint32_t period;
	
	// Info
	bool isInit;
	bool wasCtorCalled;
} AlxPwm;


//******************************************************************************
// Constructor
//******************************************************************************
void AlxPwm_Ctor
(
	AlxPwm* me,
	CTIMER_Type* tim,
	AlxIoPin** ioPinArr,
	Alx_Ch* chArr,
	float* dutyDefaultArr_pct,
	uint8_t numOfCh,
 	AlxClk* clk,
	uint32_t prescaler,
	uint32_t period
);


#endif // Module Guard

#ifdef __cplusplus
}
#endif

#endif // ALX_PWM_MCU_STM32_H
