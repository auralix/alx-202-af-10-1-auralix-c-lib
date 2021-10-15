/**
  ******************************************************************************
  * @file alxRtc_Global.h
  * @brief Auralix C Library - ALX RTC Module - Global
  * @version $LastChangedRevision: 5745 $
  * @date $LastChangedDate: 2021-08-08 19:52:52 +0200 (Sun, 08 Aug 2021) $
  ******************************************************************************
  */

#ifndef ALX_RTC_GLOBAL_H
#define ALX_RTC_GLOBAL_H

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
// Preprocessor
//******************************************************************************
#define ALX_RTC_GLOBAL_FILE "alxRtcGlobal"

// Assert //
#if defined(_ALX_RTC_GLOBAL_ASSERT_BKPT) || defined(_ALX_ASSERT_BKPT_ALL)
	#define ALX_RTC_GLOBAL_ASSERT(expr) ALX_ASSERT_BKPT(ALX_RTC_GLOBAL_FILE, expr)
#elif defined(_ALX_RTC_GLOBAL_ASSERT_TRACE) || defined(_ALX_ASSERT_TRACE_ALL)
	#define ALX_RTC_GLOBAL_ASSERT(expr) ALX_ASSERT_TRACE(ALX_RTC_GLOBAL_FILE, expr)
#elif defined(_ALX_RTC_GLOBAL_ASSERT_RST) || defined(_ALX_ASSERT_RST_ALL)
	#define ALX_RTC_GLOBAL_ASSERT(expr) ALX_ASSERT_RST(ALX_RTC_GLOBAL_FILE, expr)
#else
	#define ALX_RTC_GLOBAL_ASSERT(expr) do{} while (false)
#endif

// Trace //
#if defined(_ALX_RTC_GLOBAL_TRACE) || defined(_ALX_TRACE_ALL)
	#define ALX_RTC_GLOBAL_TRACE(...) ALX_TRACE_STD(ALX_RTC_GLOBAL_FILE, __VA_ARGS__)
#else
	#define ALX_RTC_GLOBAL_TRACE(...) do{} while (false)
#endif

// DbgPin //
#if defined(_ALX_RTC_GLOBAL_DBG_PIN) || defined(_ALX_DBG_PIN_ALL)
	#define ALX_RTC_GLOBAL_DBG_PIN(...) ALX_DBG_PIN_TOGGLE()
#else
	#define ALX_RTC_GLOBAL_DBG_PIN(...) do{} while (false)
#endif


//******************************************************************************
// Types
//******************************************************************************
typedef struct
{
	uint8_t yr;
	uint8_t mo;
	uint8_t day;
	uint8_t weekDay;
	uint8_t hr;
	uint8_t min;
	uint8_t sec;
	uint16_t ms;
	uint16_t us;
	uint16_t ns;
} AlxRtc_DateTime;


//******************************************************************************
// Functions
//******************************************************************************
uint64_t AlxRtc_MsUsNsToNs(uint64_t rtcTick_ns, uint16_t ms, uint16_t us, uint16_t ns);
uint64_t AlxRtc_MsUsToNs(uint64_t rtcTick_ns, uint16_t ms, uint16_t us);
uint64_t AlxRtc_MsToNs(uint64_t rtcTick_ns, uint16_t ms);
float AlxRtc_MsUsNsToSecFract(uint16_t ms, uint16_t us, uint16_t ns);
void AlxRtc_NsToMsUsNs(uint64_t rtcTick_ns, uint64_t in_ns, uint16_t* ms, uint16_t* us, uint16_t* ns);
void AlxRtc_NsToMsUs(uint64_t rtcTick_ns, uint64_t in_ns, uint16_t* ms, uint16_t* us);
void AlxRtc_NsToMs(uint64_t rtcTick_ns, uint64_t in_ns, uint16_t* ms);
void AlxRtc_SecFractToMsUsNs(float secFract, uint16_t* ms, uint16_t* us, uint16_t* ns);
AlxRtc_DateTime AlxRtc_UnixTimeNsToDateTime(uint64_t unixTime_ns);
AlxRtc_DateTime AlxRtc_UnixTimeUsToDateTime(uint64_t unixTime_us);
AlxRtc_DateTime AlxRtc_UnixTimeMsToDateTime(uint64_t unixTime_ms);
AlxRtc_DateTime AlxRtc_UnixTimeSecToDateTime(uint64_t unixTime_sec);
uint64_t AlxRtc_DateTimeToUnixTimeNs(AlxRtc_DateTime dateTime);
uint64_t AlxRtc_DateTimeToUnixTimeUs(AlxRtc_DateTime dateTime);
uint64_t AlxRtc_DateTimeToUnixTimeMs(AlxRtc_DateTime dateTime);
uint64_t AlxRtc_DateTimeToUnixTimeSec(AlxRtc_DateTime dateTime);

#ifdef __cplusplus
}
#endif

#endif // ALX_RTC_GLOBAL_H
