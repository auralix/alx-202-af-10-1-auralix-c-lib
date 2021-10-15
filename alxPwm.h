/**
  ******************************************************************************
  * @file alxPwm.h
  * @brief Auralix C Library - ALX PWM Module
  * @version $LastChangedRevision: 5253 $
  * @date $LastChangedDate: 2021-05-28 00:13:18 +0200 (Fri, 28 May 2021) $
  ******************************************************************************
  */

#ifndef ALX_PWM_H
#define ALX_PWM_H

#ifdef __cplusplus
extern "C" {
#endif

//******************************************************************************
// Includes
//******************************************************************************
#include "alxGlobal.h"
#include "alxTrace.h"
#include "alxAssert.h"

// AlxMcu //
#if defined(ALX_STM32F1) || defined(ALX_STM32F4) || defined(ALX_STM32G4) || defined(ALX_STM32L0)
#include "alxPwm_McuStm32.h"

#elif defined(ALX_LPC80x)
#include "alxPwm_McuLpc80x.h"

#else
typedef struct { bool dummy; } AlxPwm;
#endif


//******************************************************************************
// Preprocessor
//******************************************************************************
#define ALX_PWM_FILE "alxPwm"

// Assert //
#if defined(_ALX_PWM_ASSERT_BKPT) || defined(_ALX_ASSERT_BKPT_ALL)
	#define ALX_PWM_ASSERT(expr) ALX_ASSERT_BKPT(ALX_PWM_FILE, expr)
#elif defined(_ALX_PWM_ASSERT_TRACE) || defined(_ALX_ASSERT_TRACE_ALL)
	#define ALX_PWM_ASSERT(expr) ALX_ASSERT_TRACE(ALX_PWM_FILE, expr)
#elif defined(_ALX_PWM_ASSERT_RST) || defined(_ALX_ASSERT_RST_ALL)
	#define ALX_PWM_ASSERT(expr) ALX_ASSERT_RST(ALX_PWM_FILE, expr)
#else
	#define ALX_PWM_ASSERT(expr) do{} while (false)
#endif

// Trace //
#if defined(_ALX_PWM_TRACE) || defined(_ALX_TRACE_ALL)
	#define ALX_PWM_TRACE(...) ALX_TRACE_STD(ALX_PWM_FILE, __VA_ARGS__)
#else
	#define ALX_PWM_TRACE(...) do{} while (false)
#endif

// DbgPin //
#if defined(_ALX_PWM_DBG_PIN) || defined(_ALX_DBG_PIN_ALL)
	#define ALX_PWM_DBG_PIN(...) ALX_DBG_PIN_TOGGLE()
#else
	#define ALX_PWM_DBG_PIN(...) do{} while (false)
#endif


//******************************************************************************
// Specific Functions
//******************************************************************************
Alx_Status AlxPwm_Init(AlxPwm* me);
Alx_Status AlxPwm_DeInit(AlxPwm* me);
Alx_Status AlxPwm_SetDuty_pct(AlxPwm* me, Alx_Ch ch, float duty_pct);
Alx_Status AlxPwm_SetDuty_permil(AlxPwm* me, Alx_Ch ch, uint16_t duty_permil);


#ifdef __cplusplus
}
#endif

#endif // ALX_PWM_H
