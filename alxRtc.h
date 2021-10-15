/**
  ******************************************************************************
  * @file alxRtc.h
  * @brief Auralix C Library - ALX RTC Module
  * @version $LastChangedRevision: 5745 $
  * @date $LastChangedDate: 2021-08-08 19:52:52 +0200 (Sun, 08 Aug 2021) $
  ******************************************************************************
  */

#ifndef ALX_RTC_H
#define ALX_RTC_H

#ifdef __cplusplus
extern "C" {
#endif

//******************************************************************************
// Includes
//******************************************************************************
#include "alxGlobal.h"
#include "alxTrace.h"
#include "alxAssert.h"
#include "alxRtc_Global.h"

// AlxMcu //
#if defined(ALX_STM32F4)
#include "alxRtc_McuStm32.h"

#else
typedef struct { bool dummy; } AlxRtc;
#endif


//******************************************************************************
// Preprocessor
//******************************************************************************
#define ALX_RTC_FILE "alxRtc"

// Assert //
#if defined(_ALX_RTC_ASSERT_BKPT) || defined(_ALX_ASSERT_BKPT_ALL)
	#define ALX_RTC_ASSERT(expr) ALX_ASSERT_BKPT(ALX_RTC_FILE, expr)
#elif defined(_ALX_RTC_ASSERT_TRACE) || defined(_ALX_ASSERT_TRACE_ALL)
	#define ALX_RTC_ASSERT(expr) ALX_ASSERT_TRACE(ALX_RTC_FILE, expr)
#elif defined(_ALX_RTC_ASSERT_RST) || defined(_ALX_ASSERT_RST_ALL)
	#define ALX_RTC_ASSERT(expr) ALX_ASSERT_RST(ALX_RTC_FILE, expr)
#else
	#define ALX_RTC_ASSERT(expr) do{} while (false)
#endif

// Trace //
#if defined(_ALX_RTC_TRACE) || defined(_ALX_TRACE_ALL)
	#define ALX_RTC_TRACE(...) ALX_TRACE_STD(ALX_RTC_FILE, __VA_ARGS__)
#else
	#define ALX_RTC_TRACE(...) do{} while (false)
#endif

// DbgPin //
#if defined(_ALX_RTC_DBG_PIN) || defined(_ALX_DBG_PIN_ALL)
	#define ALX_RTC_DBG_PIN(...) ALX_DBG_PIN_TOGGLE()
#else
	#define ALX_RTC_DBG_PIN(...) do{} while (false)
#endif


//******************************************************************************
// Functions
//******************************************************************************
Alx_Status AlxRtc_Init(AlxRtc* me);
Alx_Status AlxRtc_DeInit(AlxRtc* me);
Alx_Status AlxRtc_SetDateTime(AlxRtc* me, AlxRtc_DateTime dateTime);
Alx_Status AlxRtc_GetDateTimeWithStatus(AlxRtc* me, AlxRtc_DateTime* dateTime);
AlxRtc_DateTime AlxRtc_GetDateTime(AlxRtc* me);
bool AlxRtc_IsDateTimeConfigured(AlxRtc* me);
Alx_Status AlxRtc_SetUnixTime_ns(AlxRtc* me, uint64_t unixTime_ns);
Alx_Status AlxRtc_SetUnixTime_us(AlxRtc* me, uint64_t unixTime_us);
Alx_Status AlxRtc_SetUnixTime_ms(AlxRtc* me, uint64_t unixTime_ms);
Alx_Status AlxRtc_SetUnixTime_sec(AlxRtc* me, uint64_t unixTime_sec);
Alx_Status AlxRtc_GetUnixTimeWithStatus_ns(AlxRtc* me, uint64_t* unixTime_ns);
Alx_Status AlxRtc_GetUnixTimeWithStatus_us(AlxRtc* me, uint64_t* unixTime_us);
Alx_Status AlxRtc_GetUnixTimeWithStatus_ms(AlxRtc* me, uint64_t* unixTime_ms);
Alx_Status AlxRtc_GetUnixTimeWithStatus_sec(AlxRtc* me, uint64_t* unixTime_sec);
uint64_t AlxRtc_GetUnixTime_ns(AlxRtc* me);
uint64_t AlxRtc_GetUnixTime_us(AlxRtc* me);
uint64_t AlxRtc_GetUnixTime_ms(AlxRtc* me);
uint64_t AlxRtc_GetUnixTime_sec(AlxRtc* me);
Alx_Status AlxRtc_TuneTime_ns(AlxRtc* me, int64_t tuneTime_ns);
Alx_Status AlxRtc_TuneTime_us(AlxRtc* me, int64_t tuneTime_us);
Alx_Status AlxRtc_TuneTime_ms(AlxRtc* me, int64_t tuneTime_ms);

#ifdef __cplusplus
}
#endif

#endif // ALX_RTC_H
