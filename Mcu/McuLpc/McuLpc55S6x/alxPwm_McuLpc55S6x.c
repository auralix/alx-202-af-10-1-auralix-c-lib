/**
  ******************************************************************************
  * @file alxPwm_McuLpc55S6x.c
  * @brief Auralix C Library - ALX PWM Module
  * @version $LastChangedRevision: 4690 $
  * @date $LastChangedDate: 2021-04-12 13:34:52 +0200 (Mon, 12 Apr 2021) $
  ******************************************************************************
  */

//******************************************************************************
// Includes
//******************************************************************************
#include "alxPwm_McuLpc55S6x.h"
#include "alxPwm.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_LPC55S6X)


//******************************************************************************
// Private Functions
//******************************************************************************
static bool AlxPwm_CheckIoPins(CTIMER_Type* tim, AlxIoPin** ioPinArr, uint8_t numOfCh);
static void AlxPwm_SetSrcClk_SetPrescalerMax(AlxPwm* me);
static uint32_t AlxPwm_GetCh(Alx_Ch ch);
static void AlxPwm_EnableCtimerClk(AlxPwm* me);
#if defined(ALX_PWM_OPTIMIZE_SIZE) || defined(ALX_OPTIMIZE_SIZE_ALL)
static void AlxPwm_UpdatePwmDutyPermil(AlxPwm* me, Alx_Ch ch, uint16_t duty_permil);
#else
static void AlxPwm_UpdatePwmDutyPct(AlxPwm* me, Alx_Ch ch, float duty_pct);
#endif


//******************************************************************************
// Constructor
//******************************************************************************
void AlxPwm_Ctor
(
	AlxPwm* me,
	CTIMER_Type* tim,
	AlxIoPin** ioPinArr,
	Alx_Ch* chArr,
	uint8_t numOfCh,
	AlxClk* clk,
	#if defined(ALX_PWM_OPTIMIZE_SIZE) || defined(ALX_OPTIMIZE_SIZE_ALL)
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
	#if defined(ALX_PWM_OPTIMIZE_SIZE) || defined(ALX_OPTIMIZE_SIZE_ALL)
	(void)dutyDefaultArr_permil;
	#else
	(void)dutyDefaultArr_pct;
	#endif
	ALX_PWM_ASSERT(prescaler == 0);	// MF: Do not understand how prescaler affect the PWM behaviour
	(void)period;
	for (uint8_t i = 0; i < numOfCh - 1; i++) ALX_PWM_ASSERT(chArr[i] < chArr[i + 1]);	// MF: Channel sequence must be from low to high number
	ALX_PWM_ASSERT(AlxPwm_CheckIoPins(tim, ioPinArr, numOfCh));	// MF: Check if right pins are used

	// Objects - External
	me->ioPinArr = ioPinArr;
	me->clk = clk;

	// Parameters
	me->tim = tim;
	me->chArr = chArr;
	#if defined(ALX_PWM_OPTIMIZE_SIZE) || defined(ALX_OPTIMIZE_SIZE_ALL)
	me->dutyDefaultArr_permil = dutyDefaultArr_permil;
	#else
	me->dutyDefaultArr_pct = dutyDefaultArr_pct;
	#endif
	me->numOfCh = numOfCh;

	// Variables
	me->config.mode			= kCTIMER_TimerMode;
	me->config.input		= kCTIMER_Capture_0;	// MF: This field is ignored when mode is "timer"
	me->config.prescale		= prescaler;
	me->period				= period;

	AlxPwm_SetSrcClk_SetPrescalerMax(me);

	// Info
	me->isInit = false;
	me->wasCtorCalled = true;
}


//******************************************************************************
// Functions
//******************************************************************************
Alx_Status AlxPwm_Init(AlxPwm* me)
{
	// Assert
	ALX_PWM_ASSERT(me->isInit == false);
	ALX_PWM_ASSERT(me->wasCtorCalled == true);
	(void)me;

	// #1 Init IoPin
	for (uint32_t i = 0; i < me->numOfCh; i++) AlxIoPin_Init((*(me->ioPinArr + i)));

	// #2 Enable Clk to CTimer
	AlxPwm_EnableCtimerClk(me);

	// #2 Init CTIMER
	CTIMER_Init(me->tim, &me->config);	// MF: "Periph_Reset" and "EnableClk" happens here

	// #3 Init CTIMER Channels
	for (uint32_t i = 0; i < me->numOfCh; i++)
	{
		#if defined(ALX_PWM_OPTIMIZE_SIZE) || defined(ALX_OPTIMIZE_SIZE_ALL)
		if (CTIMER_SetupPwm(me->tim, kCTIMER_Match_3, AlxPwm_GetCh(me->chArr[i]), me->dutyDefaultArr_permil[i] / 10, me->period, me->srcClk_Hz, false) != kStatus_Success) { ALX_PWM_TRACE("ErrChInit"); return Alx_Err; }	// MF: Match register 3 ("kCTIMER_Match_3") is for cycle lenght(freq or period). Divide duty by 10 to get pct from permil
		AlxPwm_UpdatePwmDutyPermil(me, me->chArr[i], me->dutyDefaultArr_permil[i]);	// MF: To setup duty in permil because FLS can not handle it
		#else
		if (CTIMER_SetupPwm(me->tim, kCTIMER_Match_3, AlxPwm_GetCh(me->chArr[i]), (uint8_t)me->dutyDefaultArr_pct[i], me->period, me->srcClk_Hz, false) != kStatus_Success) { ALX_PWM_TRACE("ErrChInit"); return Alx_Err; }	// MF: Match register 3 ("kCTIMER_Match_3") is for cycle lenght(freq or period)
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
Alx_Status AlxPwm_DeInit(AlxPwm* me)
{
	// Assert
	ALX_PWM_ASSERT(me->isInit == true);
	ALX_PWM_ASSERT(me->wasCtorCalled == true);
	(void)me;

	// #1 DeInit CTIMER
	CTIMER_Deinit(me->tim);		// MF: "Stop CTIMER" and "DisableClk" happens here

	// #2 DeInit GPIO
	for (uint32_t i = 0; i < me->numOfCh; i++) AlxIoPin_DeInit((*(me->ioPinArr + i)));

	// #3 Reset isInit
	me->isInit = false;

	// #4 Return OK
	return Alx_Ok;
}
Alx_Status AlxPwm_SetDuty_pct(AlxPwm* me, Alx_Ch ch, float duty_pct)
{
	// Assert
	ALX_PWM_ASSERT(me->isInit == true);
	ALX_PWM_ASSERT(me->wasCtorCalled == true);
	(void)me;
	(void)ch;
	(void)duty_pct;
	ALX_PWM_ASSERT((0.f <= duty_pct) && (duty_pct <= 100.f));

	// Optimize Guard
	#if defined(ALX_PWM_OPTIMIZE_SIZE) || defined(ALX_OPTIMIZE_SIZE_ALL)
	ALX_PWM_ASSERT(false);
	return ALX_NULL;
	#else

	// #1 Update Duty Cycle
	for (uint32_t i = 0; i < me->numOfCh; i++)
	{
		if (me->chArr[i] == ch)
		{
			AlxPwm_UpdatePwmDutyPct(me, ch, duty_pct);
			return Alx_Ok;
		}
	}

	// Assert
	ALX_PWM_ASSERT(false); // We shouldn't get here
	return Alx_Err;
	#endif
}
Alx_Status AlxPwm_SetDuty_permil(AlxPwm* me, Alx_Ch ch, uint16_t duty_permil)
{
	// Assert
	ALX_PWM_ASSERT(me->isInit == true);
	ALX_PWM_ASSERT(me->wasCtorCalled == true);
	(void)me;
	(void)ch;
	(void)duty_permil;
	ALX_PWM_ASSERT((0U <= duty_permil) && (duty_permil <= 1000U));

	// Optimize Guard
	#if !(defined (ALX_PWM_OPTIMIZE_SIZE) || defined(ALX_OPTIMIZE_SIZE_ALL))
	ALX_PWM_ASSERT(false);
	return ALX_NULL;
	#else

	// #1 Update Duty Cycle
	for (uint32_t i = 0; i < me->numOfCh; i++)
	{
		if (me->chArr[i] == ch)
		{
			AlxPwm_UpdatePwmDutyPermil(me, ch, duty_permil);
			return Alx_Ok;
		}
	}

	// Assert
	ALX_PWM_ASSERT(false); // We shouldn't get here
	return Alx_Err;
	#endif
}


//******************************************************************************
// Private Functions
//******************************************************************************
static bool AlxPwm_CheckIoPins(CTIMER_Type* tim, AlxIoPin** ioPinArr, uint8_t numOfCh)
{
	// Assert
	(void)tim;
	(void)ioPinArr;
	(void)numOfCh;

	// #1 Check IoPins
	#if defined(CTIMER0)
	if (tim == CTIMER0)
		for (uint32_t i = 0; i < numOfCh; i++)
		{
			if (!(	(ioPinArr[i]->port == 0 && ioPinArr[i]->pin == 0)  ||
					(ioPinArr[i]->port == 0 && ioPinArr[i]->pin == 3)  ||
					(ioPinArr[i]->port == 0 && ioPinArr[i]->pin == 19) ||
					(ioPinArr[i]->port == 0 && ioPinArr[i]->pin == 30) ||
					(ioPinArr[i]->port == 0 && ioPinArr[i]->pin == 31) ||
					(ioPinArr[i]->port == 1 && ioPinArr[i]->pin == 31) ))	{ return false; }

			if (i == (uint32_t)(numOfCh - 1))								{ return true;  }	// MF: If all pins are checked, return from function
		}
	#endif
	#if defined(CTIMER1)
	if (tim == CTIMER1)
		for (uint32_t i = 0; i < numOfCh; i++)
		{
			if (!(	(ioPinArr[i]->port == 0 && ioPinArr[i]->pin == 18) ||
					(ioPinArr[i]->port == 0 && ioPinArr[i]->pin == 20) ||
					(ioPinArr[i]->port == 0 && ioPinArr[i]->pin == 23) ||
					(ioPinArr[i]->port == 1 && ioPinArr[i]->pin == 10) ||
					(ioPinArr[i]->port == 1 && ioPinArr[i]->pin == 12) ||
					(ioPinArr[i]->port == 1 && ioPinArr[i]->pin == 14) ))	{ return false; }

			if (i == (uint32_t)(numOfCh - 1))								{ return true;  }	// MF: If all pins are checked, return from function
		}
	#endif
	#if defined(CTIMER2)
	if (tim == CTIMER2)
		for (uint32_t i = 0; i < numOfCh; i++)
		{
			if (!(	(ioPinArr[i]->port == 0 && ioPinArr[i]->pin == 10) ||
					(ioPinArr[i]->port == 0 && ioPinArr[i]->pin == 11) ||
					(ioPinArr[i]->port == 1 && ioPinArr[i]->pin == 4)  ||
					(ioPinArr[i]->port == 1 && ioPinArr[i]->pin == 5)  ||
					(ioPinArr[i]->port == 1 && ioPinArr[i]->pin == 6)  ||
					(ioPinArr[i]->port == 1 && ioPinArr[i]->pin == 7)  ))	{ return false; }

			if (i == (uint32_t)(numOfCh - 1))								{ return true;  }	// MF: If all pins are checked, return from function
		}
	#endif
	#if defined(CTIMER3)
	if (tim == CTIMER3)
		for (uint32_t i = 0; i < numOfCh; i++)
		{
			if (!(	(ioPinArr[i]->port == 0 && ioPinArr[i]->pin == 5)  ||
					(ioPinArr[i]->port == 0 && ioPinArr[i]->pin == 27) ||
					(ioPinArr[i]->port == 1 && ioPinArr[i]->pin == 19) ||
					(ioPinArr[i]->port == 1 && ioPinArr[i]->pin == 21) ))	{ return false; }

			if (i == (uint32_t)(numOfCh - 1))								{ return true;  }	// MF: If all pins are checked, return from function
		}
	#endif
	#if defined(CTIMER4)
	if (tim == CTIMER4)
		for (uint32_t i = 0; i < numOfCh; i++)
		{
			if (!(	(ioPinArr[i]->port == 0 && ioPinArr[i]->pin == 6)  ))	{ return false; }	// MF: CTIMER4 can have only 1 channel, will we ever use id?

			if (i == (uint32_t)(numOfCh - 1))								{ return true;  }	// MF: If all pins are checked, return from function
		}
		#endif

	// Return
	return false; // We shouldn't get here
}
static void AlxPwm_SetSrcClk_SetPrescalerMax(AlxPwm* me)
{
	// Assert
	(void)me;

	// #1 Set Clk
	me->periodMax = 0x0000FFFF;
	me->srcClk_Hz = AlxClk_GetClk_Hz(me->clk, AlxClk_Clk_McuLpc55s6x_MainClk_Ctor);
}
static uint32_t AlxPwm_GetCh(Alx_Ch ch)
{
	// Assert
	(void)ch;

	// #1 Return Ch
	if (ch == Alx_Ch_0) return kCTIMER_Match_0;
	if (ch == Alx_Ch_1) return kCTIMER_Match_1;
	if (ch == Alx_Ch_2) return kCTIMER_Match_2;
	//if (ch == Alx_Ch_3) return kCTIMER_Match_3;	// MF: Match register 3 is for cycle lenght (freq or period)

	// Assert
	ALX_PWM_ASSERT(false); // We shouldn't get here
	return ALX_NULL;
}
static void AlxPwm_EnableCtimerClk(AlxPwm* me)
{
	// Assert
	(void)me;

	// #1 Attach MainClk to CTIMER
	#if defined(CTIMER0)
	if (me->tim == CTIMER0)		{ CLOCK_AttachClk(kMAIN_CLK_to_CTIMER0); return; }
	#endif
	#if defined(CTIMER1)
	if (me->tim == CTIMER1)		{ CLOCK_AttachClk(kMAIN_CLK_to_CTIMER1); return; }
	#endif
	#if defined(CTIMER2)
	if (me->tim == CTIMER2)		{ CLOCK_AttachClk(kMAIN_CLK_to_CTIMER2); return; }
	#endif
	#if defined(CTIMER3)
	if (me->tim == CTIMER3)		{ CLOCK_AttachClk(kMAIN_CLK_to_CTIMER3); return; }
	#endif
	#if defined(CTIMER4)
	if (me->tim == CTIMER4)		{ CLOCK_AttachClk(kMAIN_CLK_to_CTIMER4); return; }
	#endif

	// Assert
	ALX_PWM_ASSERT(false); // We shouldn't get here
	return;
}
#if defined(ALX_PWM_OPTIMIZE_SIZE) || defined(ALX_OPTIMIZE_SIZE_ALL)
static void AlxPwm_UpdatePwmDutyPermil(AlxPwm* me, Alx_Ch ch, uint16_t duty_permil)
{
	// Assert
	(void)me;
	(void)ch;
	(void)duty_permil;

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
	// Assert
	(void)me;
	(void)ch;
	(void)duty_pct;

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


#endif // Module Guard
