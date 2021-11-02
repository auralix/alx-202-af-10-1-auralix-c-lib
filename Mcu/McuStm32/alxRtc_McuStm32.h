/**
  ******************************************************************************
  * @file alxRtc_McuStm32.h
  * @brief Auralix C Library - ALX RTC Module
  * @version $LastChangedRevision: 5745 $
  * @date $LastChangedDate: 2021-08-08 19:52:52 +0200 (Sun, 08 Aug 2021) $
  ******************************************************************************
  */

#ifndef ALX_RTC_MCU_STM32_H
#define ALX_RTC_MCU_STM32_H

#ifdef __cplusplus
extern "C" {
#endif

//******************************************************************************
// Includes
//******************************************************************************
#include "alxGlobal.h"
#include "alxRtc_Global.h"
#include "alxTrace.h"
#include "alxAssert.h"
#include "alxIoPin.h"
#include "alxClk.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_STM32F4)


//******************************************************************************
// Types
//******************************************************************************
typedef enum
{
	AlxRtc_Clk_McuStm32_SyncClk_1Hz_AsynClk_256Hz_RtcClk_32kHz768_LseCrystal_32kHz768_Default = 0,
	AlxRtc_Clk_McuStm32_SyncClk_1Hz_AsynClk_32kHz768_RtcClk_32kHz768_LseCrystal_32kHz768 = 1,
	AlxRtc_Clk_McuStm32_SyncClk_1Hz_AsynClk_32kHz768_RtcClk_32kHz768_LseOscillator_32kHz768 = 2,
} AlxRtc_Clk;

typedef struct
{
	// Objects - External

	// Parameters
	AlxRtc_Clk rtcClk;
	uint64_t rtcTick_ns;

	// Variables
	RCC_OscInitTypeDef iosc;
	RCC_PeriphCLKInitTypeDef iclk;
	RTC_HandleTypeDef hrtc;
	uint32_t PRER_Expected;

	AlxRtc_DateTime lastSetDateTime;
	uint32_t lastSetSubSec;
	uint64_t lastSetUnixTime_ns;

	bool isDateTimeConfigured;

	// Info
	bool isErr;
	bool isInit;
	bool wasCtorCalled;
} AlxRtc;


//******************************************************************************
// Constructor
//******************************************************************************
void AlxRtc_Ctor
(
	AlxRtc* me,
	AlxRtc_Clk rtcClk
);


#endif

#ifdef __cplusplus
}
#endif

#endif // ALX_RTC_MCU_STM32_H
