/**
  ******************************************************************************
  * @file alxPwm_McuStm32.h
  * @brief Auralix C Library - ALX PWM Module
  * @version $LastChangedRevision: 5484 $
  * @date $LastChangedDate: 2021-06-25 18:56:06 +0200 (Fri, 25 Jun 2021) $
  ******************************************************************************
  */

#ifndef ALX_PWM_MCU_STM32_H
#define ALX_PWM_MCU_STM32_H

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
#if defined(ALX_STM32F1) || defined(ALX_STM32F4) || defined(ALX_STM32G4) || defined(ALX_STM32L0)


//******************************************************************************
// Types
//******************************************************************************
typedef struct
{
	// Objects - External
	AlxIoPin** ioPinArr;
	AlxClk* clk;
	
	// Parameters
	Alx_Ch* chArr;
	float* dutyDefaultArr_pct;
	uint8_t numOfCh;

	// Variables
	uint32_t srcClk_Hz; // Timer source clock, which is then divided by prescaler to get timer clock
	uint32_t periodMax;
	uint32_t prescaler;
	uint32_t period;
	TIM_HandleTypeDef htim;
	TIM_OC_InitTypeDef chtim[ALX_PWM_BUFF_LEN];
	Alx_Ch ch[ALX_PWM_BUFF_LEN];
	#if !defined (ALX_PWM_OPTIMIZE_SIZE)
	float dutyDefault_pct[ALX_PWM_BUFF_LEN];
	#endif
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
	TIM_TypeDef* tim,
	AlxIoPin** ioPinArr,
	Alx_Ch* chArr,
	#if !defined (ALX_PWM_OPTIMIZE_SIZE)
	float* dutyDefaultArr_pct,
	#endif
	uint8_t numOfCh,
 	AlxClk* clk,
	uint32_t prescaler,
	uint32_t period
);

#endif

#ifdef __cplusplus
}
#endif

#endif // ALX_PWM_MCU_STM32_H
