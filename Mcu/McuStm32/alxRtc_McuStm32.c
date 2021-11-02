/**
  ******************************************************************************
  * @file alxRtc_McuStm32.c
  * @brief Auralix C Library - ALX SPI Module
  * @version $LastChangedRevision: 5807 $
  * @date $LastChangedDate: 2021-08-14 05:44:39 +0200 (Sat, 14 Aug 2021) $
  ******************************************************************************
  */

//******************************************************************************
// Includes
//******************************************************************************
#include "alxRtc_McuStm32.h"
#include "alxRtc.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_STM32F4)


//******************************************************************************
// Private Variables
//******************************************************************************
static AlxRtc* alxRtc_me = NULL;


//******************************************************************************
// Constructor
//******************************************************************************
void AlxRtc_Ctor
(
	AlxRtc* me,
	AlxRtc_Clk rtcClk
)
{
	// Objects - External

	// Parameters
	me->rtcClk = rtcClk;

	// Private Variables
	alxRtc_me = me;

	// Variables

	// RTC - Oscillator
	me->iosc.OscillatorType = RCC_OSCILLATORTYPE_LSI | RCC_OSCILLATORTYPE_LSE;
	me->iosc.HSEState = ALX_NULL;
	if
	(
		me->rtcClk == AlxRtc_Clk_McuStm32_SyncClk_1Hz_AsynClk_256Hz_RtcClk_32kHz768_LseCrystal_32kHz768_Default ||
		me->rtcClk == AlxRtc_Clk_McuStm32_SyncClk_1Hz_AsynClk_32kHz768_RtcClk_32kHz768_LseCrystal_32kHz768
	)
	{
		me->iosc.LSEState = RCC_LSE_ON;
	}
	else if
	(
		me->rtcClk == AlxRtc_Clk_McuStm32_SyncClk_1Hz_AsynClk_32kHz768_RtcClk_32kHz768_LseOscillator_32kHz768
	)
	{
		me->iosc.LSEState = RCC_LSE_BYPASS;
	}
	else
	{
		ALX_RTC_ASSERT(false);	// We shouldn't get here
		return;
	}
	me->iosc.HSIState = ALX_NULL;
	me->iosc.HSICalibrationValue = ALX_NULL;
	me->iosc.LSIState = RCC_LSI_OFF;
	me->iosc.PLL.PLLState = ALX_NULL;
	me->iosc.PLL.PLLSource = ALX_NULL;
	me->iosc.PLL.PLLM = ALX_NULL;
	me->iosc.PLL.PLLN = ALX_NULL;
	me->iosc.PLL.PLLP = ALX_NULL;
	me->iosc.PLL.PLLQ = ALX_NULL;
	#if defined(STM32F410Tx) || defined(STM32F410Cx) || defined(STM32F410Rx) || defined(STM32F446xx) || defined(STM32F469xx) ||\
		defined(STM32F479xx) || defined(STM32F412Zx) || defined(STM32F412Vx) || defined(STM32F412Rx) || defined(STM32F412Cx) ||\
		defined(STM32F413xx) || defined(STM32F423xx)
	me->iosc.PLL.PLLR = ALX_NULL;
	#endif

	// RTC - Clock Source
	me->iclk.PeriphClockSelection = RCC_PERIPHCLK_RTC;
	me->iclk.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;

	// RTC - Init
	me->hrtc.Instance = RTC;
	me->hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
	me->hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
	me->hrtc.Init.OutPutPolarity = ALX_NULL;
	me->hrtc.Init.OutPutType = ALX_NULL;
	if (me->rtcClk == AlxRtc_Clk_McuStm32_SyncClk_1Hz_AsynClk_256Hz_RtcClk_32kHz768_LseCrystal_32kHz768_Default)
	{
		me->hrtc.Init.AsynchPrediv = 128 - 1;
		me->hrtc.Init.SynchPrediv = 256 - 1;
		me->rtcTick_ns = 3906250;	// 1000000000 / 256 = 3906250ns

		ALX_RTC_ASSERT(false);	// TODO - Handle if SS Larger than PREDIV_S
	}
	else if
	(
		me->rtcClk == AlxRtc_Clk_McuStm32_SyncClk_1Hz_AsynClk_32kHz768_RtcClk_32kHz768_LseCrystal_32kHz768 ||
		me->rtcClk == AlxRtc_Clk_McuStm32_SyncClk_1Hz_AsynClk_32kHz768_RtcClk_32kHz768_LseOscillator_32kHz768
	)
	{
		me->hrtc.Init.AsynchPrediv = 1 - 1;
		me->hrtc.Init.SynchPrediv = 32768 - 1;
		me->rtcTick_ns = 30518;	// 1000000000 / 32768 = 30517.57813 = ~30518ns
		me->PRER_Expected = 0x00007FFF;
	}
	else
	{
		ALX_RTC_ASSERT(false);	// We shouldn't get here
		return;
	}
	__HAL_RTC_RESET_HANDLE_STATE(&me->hrtc);

	// Internal
	me->lastSetDateTime.yr = 0;
	me->lastSetDateTime.mo = 0;
	me->lastSetDateTime.day = 0;
	me->lastSetDateTime.weekDay = 0;
	me->lastSetDateTime.hr = 0;
	me->lastSetDateTime.min = 0;
	me->lastSetDateTime.sec = 0;
	me->lastSetDateTime.ms = 0;
	me->lastSetDateTime.us = 0;
	me->lastSetDateTime.ns = 0;
	me->lastSetSubSec = 0;

	me->isDateTimeConfigured = false;

	// Info
	me->isErr = false;
	me->isInit = false;
	me->wasCtorCalled = true;
}


//******************************************************************************
// Functions
//******************************************************************************
Alx_Status AlxRtc_Init(AlxRtc* me)
{
	ALX_RTC_ASSERT(me->isInit == false);
	ALX_RTC_ASSERT(me->wasCtorCalled == true);

	// #1 Reset isErr
	me->isErr = false;

	// #2 Set isInit -> We need to do this here because IsDateTimeConfigured need this
	me->isInit = true;

	// #3 Init RTC
	if
	(
		(AlxRtc_IsDateTimeConfigured(me) == false) ||
	 	(me->hrtc.Instance->PRER != me->PRER_Expected)	// Check if register PRER value is NOK
	)
	{
		if
		(
			(HAL_RTC_Init(&me->hrtc) != HAL_OK) ||
			(me->isErr == true)
		)
		{
			ALX_RTC_TRACE("ErrInit");
			me->isErr = true;
			me->isInit = false;
			return Alx_Err;
		}
	}
	else
	{
		HAL_RTC_MspInit(&me->hrtc);
	}

//	if (HAL_RTCEx_SetCalibrationOutPut(&me->hrtc, RTC_CALIBOUTPUT_512HZ) != HAL_OK) { ALX_RTC_TRACE("ErrCalibOutInit"); me->isErr = true; return Alx_Err; }

	// #4 Return OK
	return Alx_Ok;
}
Alx_Status AlxRtc_DeInit(AlxRtc* me)
{
	ALX_RTC_ASSERT(me->isInit == true);
	ALX_RTC_ASSERT(me->wasCtorCalled == true);

	// #1 DeInit RTC
	if (HAL_RTC_DeInit(&me->hrtc) != HAL_OK) { ALX_RTC_TRACE("ErrDeInit"); me->isErr = true; return Alx_Err; }

	// #2 Reset isDateTimeConfigured
	HAL_RTCEx_BKUPWrite(&me->hrtc, RTC_BKP_DR0, 0);
	me->isDateTimeConfigured = false;

	// #3 Reset isInit
	me->isInit = false;

	// #4 Return OK
	return Alx_Ok;
}
Alx_Status AlxRtc_SetDateTime(AlxRtc* me, AlxRtc_DateTime dateTime)
{
	ALX_RTC_ASSERT(me->isInit == true);
	ALX_RTC_ASSERT(me->wasCtorCalled == true);

	ALX_RTC_ASSERT(dateTime.yr <= 99)
	ALX_RTC_ASSERT((1 <= dateTime.mo) && (dateTime.mo <= 12));
	ALX_RTC_ASSERT((1 <= dateTime.day) && (dateTime.day <= 31));
	ALX_RTC_ASSERT((1 <= dateTime.weekDay) && (dateTime.weekDay <= 7));
	ALX_RTC_ASSERT(dateTime.hr <= 23);
	ALX_RTC_ASSERT(dateTime.min <= 59);
	ALX_RTC_ASSERT(dateTime.sec <= 59);
	ALX_RTC_ASSERT(dateTime.ms == 0);
	ALX_RTC_ASSERT(dateTime.us == 0);
	ALX_RTC_ASSERT(dateTime.ns == 0);

	// #1 Set Time to Zero
	RTC_TimeTypeDef sTimeZero;
	sTimeZero.Hours = 0;
	sTimeZero.Minutes = 0;
	sTimeZero.Seconds = 0;
	sTimeZero.TimeFormat = ALX_NULL;
	sTimeZero.SubSeconds = ALX_NULL;
	sTimeZero.SecondFraction = ALX_NULL;
	sTimeZero.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
	sTimeZero.StoreOperation = RTC_STOREOPERATION_RESET;
	if(HAL_RTC_SetTime(&me->hrtc, &sTimeZero, RTC_FORMAT_BIN) != HAL_OK) { ALX_RTC_TRACE("ErrSetTimeZero"); me->isErr = true; return Alx_Err; };

	// #2 Set Date
	RTC_DateTypeDef sDate;
	sDate.WeekDay = dateTime.weekDay;
	sDate.Month = dateTime.mo;
	sDate.Date = dateTime.day;
	sDate.Year = dateTime.yr;
	if(HAL_RTC_SetDate(&me->hrtc, &sDate, RTC_FORMAT_BIN) != HAL_OK) { ALX_RTC_TRACE("ErrSetDate"); me->isErr = true; return Alx_Err; };

	me->lastSetDateTime.yr = dateTime.yr;
	me->lastSetDateTime.mo = dateTime.mo;
	me->lastSetDateTime.day = dateTime.day;
	me->lastSetDateTime.weekDay = dateTime.weekDay;

	// #3 Set Time
	RTC_TimeTypeDef sTime;
	sTime.Hours = dateTime.hr;
	sTime.Minutes = dateTime.min;
	sTime.Seconds = dateTime.sec;
	sTime.TimeFormat = ALX_NULL;
	sTime.SubSeconds = ALX_NULL;
	sTime.SecondFraction = ALX_NULL;
	sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
	sTime.StoreOperation = RTC_STOREOPERATION_RESET;
	if(HAL_RTC_SetTime(&me->hrtc, &sTime, RTC_FORMAT_BIN) != HAL_OK) { ALX_RTC_TRACE("ErrSetTime"); me->isErr = true; return Alx_Err; };

	me->lastSetDateTime.hr = dateTime.hr;
	me->lastSetDateTime.min = dateTime.min;
	me->lastSetDateTime.sec = dateTime.sec;
	me->lastSetDateTime.ms = dateTime.ms;
	me->lastSetDateTime.us = dateTime.us;
	me->lastSetSubSec = 0;

	// #4 Write backup register to mark that DateTime is configured
	HAL_RTCEx_BKUPWrite(&me->hrtc, RTC_BKP_DR0, 0x000032F2);
	me->isDateTimeConfigured = true;

	// #5 Return OK
	return Alx_Ok;
}
Alx_Status AlxRtc_GetDateTimeWithStatus(AlxRtc* me, AlxRtc_DateTime* dateTime)
{
	ALX_RTC_ASSERT(me->isInit == true);
	ALX_RTC_ASSERT(me->wasCtorCalled == true);

	// #1 Get Time
	RTC_TimeTypeDef sTime;
	if(HAL_RTC_GetTime(&me->hrtc, &sTime, RTC_FORMAT_BIN) != HAL_OK) { ALX_RTC_TRACE("ErrGetTime"); me->isErr = true; return Alx_Err; };

	// #2 Set hr, min, sec
	dateTime->hr = sTime.Hours;
	dateTime->min = sTime.Minutes;
	dateTime->sec = sTime.Seconds;

	// #3 Calculate secFract
	float secFract = ((float)sTime.SecondFraction - (float)sTime.SubSeconds) / ((float)sTime.SecondFraction + 1.f);

	// #4 Handle if SS Larger than PREDIV_S, then secFract will be negative
	if(secFract < 0.f)
	{
		// #3.1 Make secFract positive
		secFract = secFract * (-1.f);	// Make positive

		// #3.2 Handle if hr, min, sec are 0
		if (dateTime->sec == 0)								// If sec are 0, we must substract min
		{
			if (dateTime->min == 0)							// If min are 0, we must substract hr
			{
				if (dateTime->hr == 0)						// If hr are 0, we must substract days
				{
					// We will not handle this, this can only happen once per day
				}
				else
				{
					dateTime->hr = dateTime->hr - 1;		// Substract 1hr
					dateTime->min = 59;						// Set min
					dateTime->sec = 59;						// Set sec
				}
			}
			else
			{
				dateTime->min = dateTime->min - 1;			// Substract 1min
				dateTime->sec = 59;							// Set sec
			}
		}
		else
		{
			dateTime->sec = dateTime->sec - 1;				// Substract 1s
		}
	}

	// #5 Set ms, us, ns
	AlxRtc_SecFractToMsUsNs(secFract, &dateTime->ms, &dateTime->us, &dateTime->ns);

	// #6 Get Date
	RTC_DateTypeDef sDate;
	if(HAL_RTC_GetDate(&me->hrtc, &sDate, RTC_FORMAT_BIN) != HAL_OK) { ALX_RTC_TRACE("ErrGetDate"); me->isErr = true; return Alx_Err; };

	// #7 Set yr, mo, day, weekDay
	dateTime->yr = sDate.Year;
	dateTime->mo = sDate.Month;
	dateTime->day = sDate.Date;
	dateTime->weekDay = sDate.WeekDay;

	// #8 Return OK
	return Alx_Ok;
}
AlxRtc_DateTime AlxRtc_GetDateTime(AlxRtc* me)
{
	AlxRtc_DateTime temp = {};
	if(AlxRtc_GetDateTimeWithStatus(me, &temp) != Alx_Ok) { ALX_RTC_ASSERT(false) ; return temp ; }
	return temp;
}
bool AlxRtc_IsDateTimeConfigured(AlxRtc* me)
{
	ALX_RTC_ASSERT(me->isInit == true);
	ALX_RTC_ASSERT(me->wasCtorCalled == true);

	if (HAL_RTCEx_BKUPRead(&me->hrtc, RTC_BKP_DR0) == 0x000032F2)	// Read backup register to check if DateTime is configured
		return true;
	else
		return false;
}
Alx_Status AlxRtc_SetUnixTime_ns(AlxRtc* me, uint64_t unixTime_ns)
{
	ALX_RTC_ASSERT(me->isInit == true);
	ALX_RTC_ASSERT(me->wasCtorCalled == true);

	// #1 Set Time
	AlxRtc_DateTime dateTime = AlxRtc_UnixTimeNsToDateTime(unixTime_ns);
	if(AlxRtc_SetDateTime(me, dateTime) != Alx_Ok) { ALX_RTC_TRACE("ErrSetTime"); me->isErr = true; return Alx_Err; };
	me->lastSetUnixTime_ns = unixTime_ns;

	// #2 Return OK
	return Alx_Ok;
}
Alx_Status AlxRtc_SetUnixTime_us(AlxRtc* me, uint64_t unixTime_us)
{
	return AlxRtc_SetUnixTime_ns(me, unixTime_us * 1000ull);
}
Alx_Status AlxRtc_SetUnixTime_ms(AlxRtc* me, uint64_t unixTime_ms)
{
	return AlxRtc_SetUnixTime_ns(me, unixTime_ms * 1000000ull);
}
Alx_Status AlxRtc_SetUnixTime_sec(AlxRtc* me, uint64_t unixTime_sec)
{
	return AlxRtc_SetUnixTime_ns(me, unixTime_sec * 1000000000ull);
}
Alx_Status AlxRtc_GetUnixTimeWithStatus_ns(AlxRtc* me, uint64_t* unixTime_ns)
{
	ALX_RTC_ASSERT(me->isInit == true);
	ALX_RTC_ASSERT(me->wasCtorCalled == true);

	// #1 Get Time
	AlxRtc_DateTime dateTime;
	if(AlxRtc_GetDateTimeWithStatus(me, &dateTime) != Alx_Ok) { ALX_RTC_TRACE("ErrGetTime"); me->isErr = true; return Alx_Err; };
	*unixTime_ns = AlxRtc_DateTimeToUnixTimeNs(dateTime);

	// #2 Return OK
	return Alx_Ok;
}
Alx_Status AlxRtc_GetUnixTimeWithStatus_us(AlxRtc* me, uint64_t* unixTime_us)
{
	uint64_t ns = 0;
	if(AlxRtc_GetUnixTimeWithStatus_ns(me, &ns) != Alx_Ok) return Alx_Err;
	*unixTime_us = ns / 1000ull;

	return Alx_Ok;
}
Alx_Status AlxRtc_GetUnixTimeWithStatus_ms(AlxRtc* me, uint64_t* unixTime_ms)
{
	uint64_t ns = 0;
	if(AlxRtc_GetUnixTimeWithStatus_ns(me, &ns) != Alx_Ok) return Alx_Err;
	*unixTime_ms = ns / 1000000ull;

	return Alx_Ok;
}
Alx_Status AlxRtc_GetUnixTimeWithStatus_sec(AlxRtc* me, uint64_t* unixTime_sec)
{
	uint64_t ns = 0;
	if(AlxRtc_GetUnixTimeWithStatus_ns(me, &ns) != Alx_Ok) return Alx_Err;
	*unixTime_sec = ns / 1000000000ull;

	return Alx_Ok;
}
uint64_t AlxRtc_GetUnixTime_ns(AlxRtc* me)
{
	uint64_t temp = 0;
	if(AlxRtc_GetUnixTimeWithStatus_ns(me, &temp) != Alx_Ok) { ALX_RTC_ASSERT(false) ; return temp ; }
	return temp;
}
uint64_t AlxRtc_GetUnixTime_us(AlxRtc* me)
{
	uint64_t temp = 0;
	if(AlxRtc_GetUnixTimeWithStatus_us(me, &temp) != Alx_Ok) { ALX_RTC_ASSERT(false) ; return temp ; }
	return temp;
}
uint64_t AlxRtc_GetUnixTime_ms(AlxRtc* me)
{
	uint64_t temp = 0;
	if(AlxRtc_GetUnixTimeWithStatus_ms(me, &temp) != Alx_Ok) { ALX_RTC_ASSERT(false) ; return temp ; }
	return temp;
}
uint64_t AlxRtc_GetUnixTime_sec(AlxRtc* me)
{
	uint64_t temp = 0;
	if(AlxRtc_GetUnixTimeWithStatus_sec(me, &temp) != Alx_Ok) { ALX_RTC_ASSERT(false) ; return temp ; }
	return temp;
}
Alx_Status AlxRtc_TuneTime_ns(AlxRtc* me, int64_t tuneTime_ns)
{
	ALX_RTC_ASSERT(me->isInit == true);
	ALX_RTC_ASSERT(me->wasCtorCalled == true);
	if (tuneTime_ns < -990000000) tuneTime_ns = -990000000;	// Max tunning is +990000000ns = +990ms
	if (tuneTime_ns > +990000000) tuneTime_ns = +990000000;	// Max tunning is -990000000ns = -990ms

	if(tuneTime_ns == 0) return Alx_Ok;	// No tune needed

	// #1 Calculate number of tick to add to sub seconds register
	uint16_t numOfTickToAdd = 0;
	uint32_t ShiftAdd1S = 0;
	if (tuneTime_ns > 0)
	{
		//float numOfTickToAddFloat = (float)tuneTime_ns / (float)me->rtcTick_ns;
		float numOfTickToAddFloat = (1.f - ((float)tuneTime_ns / 1000000000.f)) * ((float)me->hrtc.Init.SynchPrediv + 1.f);		// According to reference manual: Advance (seconds) = ( 1 - ( SUBFS / ( PREDIV_S + 1 ) ) )
		numOfTickToAdd = (uint16_t)numOfTickToAddFloat;
		ShiftAdd1S = RTC_SHIFTADD1S_SET;
	}
	else if (tuneTime_ns < 0)
	{
		//float numOfTickToAddFloat = ((float)tuneTime_ns * (-1.f)) / (float)me->rtcTick_ns;
		float numOfTickToAddFloat = (((float)tuneTime_ns / 1000000000.f)* (-1.f) ) * ((float)me->hrtc.Init.SynchPrediv + 1.f);	// According to reference manual: Delay (seconds) = SUBFS / ( PREDIV_S + 1 )
		numOfTickToAdd = (uint16_t)numOfTickToAddFloat;
		ShiftAdd1S = RTC_SHIFTADD1S_RESET;
	}
	else
	{
		ALX_RTC_ASSERT(false);	// We should not get here
	}

	if(numOfTickToAdd == 0) return Alx_Ok;	// No tune needed

	// #2 Read sub seconds register and calculate ssrAfterAdd
	uint32_t ssr = me->hrtc.Instance->SSR;

	// #3 Check if overflow occurs
	if (ssr > 0x00007FFF) return Alx_Err;

	// #4 Tune
	if(HAL_RTCEx_SetSynchroShift(&me->hrtc, ShiftAdd1S, numOfTickToAdd) != HAL_OK) { ALX_RTC_TRACE("Err"); me->isErr = true; return Alx_Err; };

	// #5 Return
	return Alx_Ok;
}
Alx_Status AlxRtc_TuneTime_us(AlxRtc* me, int64_t tuneTime_us)
{
	return AlxRtc_TuneTime_ns(me, tuneTime_us * 1000ull);
}
Alx_Status AlxRtc_TuneTime_ms(AlxRtc* me, int64_t tuneTime_ms)
{
	return AlxRtc_TuneTime_ns(me, tuneTime_ms * 1000000ull);
}


//******************************************************************************
// Weak Functions
//******************************************************************************
void HAL_RTC_MspInit(RTC_HandleTypeDef *hrtc)
{
	(void)hrtc;

	if(HAL_RCC_OscConfig(&alxRtc_me->iosc) != HAL_OK)			{ ALX_RTC_TRACE("ErrOsc"); alxRtc_me->isErr = true; };
	if(HAL_RCCEx_PeriphCLKConfig(&alxRtc_me->iclk) != HAL_OK)	{ ALX_RTC_TRACE("ErrClk"); alxRtc_me->isErr = true; };
	__HAL_RCC_RTC_ENABLE();
}

void HAL_RTC_MspDeInit(RTC_HandleTypeDef *hrtc)
{
	(void)hrtc;

	__HAL_RCC_RTC_DISABLE();
}


#endif
