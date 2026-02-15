/**
  ******************************************************************************
  * @file		alxPwm_McuLpc80x.c
  * @brief		Auralix C Library - ALX PWM MCU LPC80X Module
  * @copyright	Copyright (C) Auralix d.o.o. All rights reserved.
  *
  * @section License
  *
  * SPDX-License-Identifier: GPL-3.0-or-later
  *
  * This file is part of Auralix C Library.
  *
  * Auralix C Library is free software: you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation, either version 3
  * of the License, or (at your option) any later version.
  *
  * Auralix C Library is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  * GNU General Public License for more details.
  *
  * You should have received a copy of the GNU General Public License
  * along with Auralix C Library. If not, see <https://www.gnu.org/licenses/>.
  ******************************************************************************
  **/

//******************************************************************************
// Includes
//******************************************************************************
#include "alxPwm_McuLpc80x.h"
#include "alxPwm.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB) && defined(ALX_LPC80X_TODO)


//******************************************************************************
// Private Functions
//******************************************************************************
static uint32_t AlxPwm_GetCh(Alx_Ch ch);
static void AlxPwm_SetSrcClk_SetPrescalerMax(AlxPwm* me);
#if defined(ALX_PWM_OPTIMIZE_SIZE)
static void AlxPwm_UpdatePwmDutyPermil(AlxPwm* me, Alx_Ch ch, uint16_t duty_permil);
#else
static void AlxPwm_UpdatePwmDutyPct(AlxPwm* me, Alx_Ch ch, float duty_pct);
#endif


//******************************************************************************
// Constructor
//******************************************************************************

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		tim
  * @param[in]		ioPinArr
  * @param[in]		chArr
  * @param[in]		numOfCh
  * @param[in]		clk
  * @param[in]		dutyDefaultArr_permil
  * @param[in]		dutyDefaultArr_pct
  * @param[in]		prescaler
  * @param[in]		period
  */
void AlxPwm_Ctor
(
	AlxPwm* me,
	CTIMER_Type* tim,
	AlxIoPin** ioPinArr,
	Alx_Ch* chArr,
	uint8_t numOfCh,
	AlxClk* clk,
	#if defined(ALX_PWM_OPTIMIZE_SIZE)
	uint16_t* dutyDefaultArr_permil,
	#else
	float* dutyDefaultArr_pct,
	#endif
	uint32_t prescaler,
	uint32_t period
)
{
	// Assert
	(void)me;
	(void)tim;
	(void)ioPinArr;
	(void)chArr;
	ALX_PWM_ASSERT(numOfCh <= 3);	// MF: Only match registers 0, 1, 2 can be used for PWM Output. Match register 3 is for cycle lenght (freq or period)
	(void)clk;
	#if defined(ALX_PWM_OPTIMIZE_SIZE)
	(void)dutyDefaultArr_permil;
	#else
	(void)dutyDefaultArr_pct;
	#endif
	ALX_PWM_ASSERT(prescaler == 0);	// MF: Do not understand how prescaler affect the PWM behaviour
	(void)period;
	for (uint8_t i = 0; i < numOfCh - 1; i++) ALX_PWM_ASSERT(chArr[i] < chArr[i + 1]);	// MF: Channel sequence must be from low to high number

	// Objects - External
	me->ioPinArr = ioPinArr;
	me->clk = clk;

	// Parameters
	me->tim = tim;
	me->chArr = chArr;
	#if defined(ALX_PWM_OPTIMIZE_SIZE)
	me->dutyDefaultArr_permil = dutyDefaultArr_permil;
	#else
	me->dutyDefaultArr_pct = dutyDefaultArr_pct;
	#endif
	me->numOfCh = numOfCh;

	// Variables
	me->config.input = kCTIMER_Capture_0;	// MF: This field is ignored when mode is "timer"
	me->config.mode =  kCTIMER_TimerMode;
	me->config.prescale = prescaler;
	me->period = period;

	AlxPwm_SetSrcClk_SetPrescalerMax(me);

	// Info
	me->isInit = false;
	me->wasCtorCalled = true;
}


//******************************************************************************
// Functions
//******************************************************************************

/**
  * @brief
  * @param[in,out] me
  */
Alx_Status AlxPwm_Init(AlxPwm* me)
{
	// Assert
	ALX_PWM_ASSERT(me->isInit == false);
	ALX_PWM_ASSERT(me->wasCtorCalled == true);

	// #1 Init GPIO
	for (uint32_t i = 0; i < me->numOfCh; i++)
		AlxIoPin_Init((*(me->ioPinArr + i)));

	// #2 Init CTIMER
	CTIMER_Init(me->tim, &me->config);	// MF: "Periph_Reset" and "EnableClk" happens here

	// #3 Init CTIMER Channels
	for (uint32_t i = 0; i < me->numOfCh; i++)
	{
		#if defined(ALX_PWM_OPTIMIZE_SIZE)
		// MF: Match register 3 ("kCTIMER_Match_3") is for cycle lenght(freq or period). Divide duty by 10 to get pct from permil
		if (CTIMER_SetupPwm(me->tim, kCTIMER_Match_3, AlxPwm_GetCh(me->chArr[i]), me->dutyDefaultArr_permil[i] / 10, me->period, me->srcClk_Hz, false) != kStatus_Success)
		{
			ALX_PWM_TRACE_ERR("ErrChInit");
			return Alx_Err;
		}
		AlxPwm_UpdatePwmDutyPermil(me, me->chArr[i], me->dutyDefaultArr_permil[i]);	// MF: To setup duty in permil because FLS can not handle it
		#else
		// MF: Match register 3 ("kCTIMER_Match_3") is for cycle lenght(freq or period)
		if (CTIMER_SetupPwm(me->tim, kCTIMER_Match_3, AlxPwm_GetCh(me->chArr[i]), (uint8_t)me->dutyDefaultArr_pct[i], me->period, me->srcClk_Hz, false) != kStatus_Success)
		{
			ALX_PWM_TRACE_ERR("ErrChInit");
			return Alx_Err;
		}
		AlxPwm_UpdatePwmDutyPct(me, me->chArr[i], me->dutyDefaultArr_pct[i]);		// MF: To setup duty in float because FLS can not handle it
		#endif
	}

	// #4 Set isInit
	me->isInit = true;

	// #5 Start CTIMER
	CTIMER_StartTimer(me->tim);

	// #6 Return OK
	return Alx_Ok;
}

/**
  * @brief
  * @param[in,out] me
  */
Alx_Status AlxPwm_DeInit(AlxPwm* me)
{
	// Assert
	ALX_PWM_ASSERT(me->isInit == true);
	ALX_PWM_ASSERT(me->wasCtorCalled == true);

	// #1 DeInit CTIMER
	CTIMER_Deinit(me->tim);		// MF: "Stop CTIMER" and "DisableClk" happens here

	// #2 DeInit GPIO
	for (uint32_t i = 0; i < me->numOfCh; i++)
		AlxIoPin_DeInit((*(me->ioPinArr + i)));

	// #3 Reset isInit
	me->isInit = false;

	// #4 Return OK
	return Alx_Ok;
}

/**
  * @brief
  * @param[in] me
  * @param[in] ch
  * @param[in] duty_pct
  */
Alx_Status AlxPwm_SetDuty_pct(AlxPwm* me, Alx_Ch ch, float duty_pct)
{
	// Optimize Guard
	#if defined(ALX_PWM_OPTIMIZE_SIZE)
	(void)me;
	(void)ch;
	(void)duty_pct;
	ALX_PWM_ASSERT(false);
	return ALX_NULL;
	#else

	// Assert
	ALX_PWM_ASSERT(me->isInit == true);
	ALX_PWM_ASSERT(me->wasCtorCalled == true);
	(void)ch;
	ALX_PWM_ASSERT((0.f <= duty_pct) && (duty_pct <= 100.f));

	// #1 Update Duty Cycle
	for (uint32_t i = 0; i < me->numOfCh; i++)
	{
		if (me->chArr[i] == ch)
		{
			AlxPwm_UpdatePwmDutyPct(me, ch, duty_pct);
			//CTIMER_UpdatePwmDutycycle(me->tim, kCTIMER_Match_3, AlxPwm_GetCh(ch), (uint32_t)duty_pct);	// MF: Match register 3 ("kCTIMER_Match_3") is for cycle lenght(freq or period)
			return Alx_Ok;
		}
	}

	ALX_PWM_ASSERT(false); // We shouldn't get here
	return Alx_Err;
	#endif
}

/**
  * @brief
  * @param[in] me
  * @param[in] ch
  * @param[in] duty_permil
  */
Alx_Status AlxPwm_SetDuty_permil(AlxPwm* me, Alx_Ch ch, uint16_t duty_permil)
{
	// Optimize Guard
	#if !(defined (ALX_PWM_OPTIMIZE_SIZE))
	(void)me;
	(void)ch;
	(void)duty_permil;
	ALX_PWM_ASSERT(false);
	return ALX_NULL;
	#else

	// Assert
	ALX_PWM_ASSERT(me->isInit == true);
	ALX_PWM_ASSERT(me->wasCtorCalled == true);
	(void)ch;
	ALX_PWM_ASSERT((0 <= duty_permil) && (duty_permil <= 1000));

	// #1 Update Duty Cycle
	for (uint32_t i = 0; i < me->numOfCh; i++)
	{
		if (me->chArr[i] == ch)
		{
			AlxPwm_UpdatePwmDutyPermil(me, ch, duty_permil);
			return Alx_Ok;
		}
	}

	ALX_PWM_ASSERT(false); // We shouldn't get here
	return Alx_Err;
	#endif
}


//******************************************************************************
// Private Functions
//******************************************************************************
static uint32_t AlxPwm_GetCh(Alx_Ch ch)
{
	if (ch == Alx_Ch_0) return kCTIMER_Match_0;
	if (ch == Alx_Ch_1) return kCTIMER_Match_1;
	if (ch == Alx_Ch_2) return kCTIMER_Match_2;
	//if (ch == Alx_Ch_3) return kCTIMER_Match_3; //	MF: Match register 3 is for cycle lenght (freq or period)

	ALX_PWM_ASSERT(false); // We shouldn't get here
	return ALX_NULL;
}
static void AlxPwm_SetSrcClk_SetPrescalerMax(AlxPwm* me)
{
	me->periodMax = 0x0000FFFF;
	me->srcClk_Hz = AlxClk_GetClk_Hz(me->clk, AlxClk_Clk_McuLpc8xx_CoreSysClk_Ctor);
}
#if defined(ALX_PWM_OPTIMIZE_SIZE)
static void AlxPwm_UpdatePwmDutyPermil(AlxPwm* me, Alx_Ch ch, uint16_t duty_permil)
{
	// #1 Prepare variable
	uint32_t pulsePeriod = 0;

	// #2 Get Period
	uint32_t period = me->tim->MR[kCTIMER_Match_3];	// MF: Match register 3 ("kCTIMER_Match_3") is for cycle lenght(freq or period)

	// #3 Calculate Pulse
	if (duty_permil == 0U)	{ pulsePeriod = period + 1U; }
	else					{ pulsePeriod = (period * (1000U - (uint32_t)duty_permil)) / 1000U; }

	// #4 Update dutycycle
	me->tim->MR[AlxPwm_GetCh(ch)] = pulsePeriod;
}
#else
static void AlxPwm_UpdatePwmDutyPct(AlxPwm* me, Alx_Ch ch, float duty_pct)
{
	// #1 Prepare variable
	uint32_t pulsePeriod = 0;

	// #2 Get Period
	uint32_t period = me->tim->MR[kCTIMER_Match_3];	// MF: Match register 3 ("kCTIMER_Match_3") is for cycle lenght(freq or period)

	// #3 Calculate Pulse
	if (duty_pct == 0.f)	{ pulsePeriod = period + 1U; }
	else					{ pulsePeriod = (period * (100000U - (duty_pct * 1000.f))) / 100000U; }		// MF: Multiply "duty_pct" with 1000 to handle 3 decimals (the max resolution "MR" can handle)

	// #4 Update dutycycle
	me->tim->MR[AlxPwm_GetCh(ch)] = pulsePeriod;
}
#endif


#endif	// #if defined(ALX_C_LIB) && defined(ALX_LPC80X)
