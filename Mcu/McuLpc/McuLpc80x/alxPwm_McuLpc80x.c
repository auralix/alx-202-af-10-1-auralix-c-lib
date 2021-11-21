/**
  ******************************************************************************
  * @file alxPwm_McuLpc80x.c
  * @brief Auralix C Library - ALX PWM Module
  * @version $LastChangedRevision: 4690 $
  * @date $LastChangedDate: 2021-04-12 13:34:52 +0200 (Mon, 12 Apr 2021) $
  ******************************************************************************
  */

//******************************************************************************
// Includes
//******************************************************************************
#include "alxPwm_McuLpc80x.h"
#include "alxPwm.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_LPC80x_UNDEFINED)


//******************************************************************************
// Private Functions
//******************************************************************************
static uint32_t AlxPwm_GetCh(Alx_Ch ch);
static void AlxPwm_SetSrcClk_SetPrescalerMax(AlxPwm* me);


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
)
{
	// Assert
	(void)me;
	(void)tim;
	(void)ioPinArr;
	(void)chArr;
	(void)dutyDefaultArr_pct;
	ALX_PWM_ASSERT(numOfCh <= 3); // MF: Only match registers 0, 1, 2 can be used for PRM Output. Match register 3 is for cycle lenght(freq or period)
	(void)clk;
	(void)prescaler;
	(void)period;
	for (uint8_t i = 0; i < numOfCh - 1; i++) ALX_PWM_ASSERT(chArr[i] < chArr[i + 1]); // Channel sequence must be from low to high number

	// Objects - External
	me->ioPinArr = ioPinArr;
	me->clk = clk;

	// Parameters
	me->tim = tim;
	me->chArr = chArr;
	me->dutyDefaultArr_pct = dutyDefaultArr_pct;
	me->numOfCh = numOfCh;

	// Variables
	me->config.input = kCTIMER_TimerMode;
	me->config.mode = kCTIMER_Capture_0; // This field is ignored when mode is timer
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
Alx_Status AlxPwm_Init(AlxPwm* me)
{
	ALX_PWM_ASSERT(me->isInit == false);
	ALX_PWM_ASSERT(me->wasCtorCalled == true);

	// #1 Init GPIO
	for (uint32_t i = 0; i < me->numOfCh; i++)
		AlxIoPin_Init((*(me->ioPinArr + i)));

	// #2 Init CTIMER
	CTIMER_Init(me->tim, &me->config); // "Periph_Reset" and "EnableClk" happens here

	// #3 Init CTIMER Channels
	for(uint32_t i = 0; i < me->numOfCh; i++)
		if(CTIMER_SetupPwm(me->tim, AlxPwm_GetCh(me->chArr[i]), (uint32_t)me->dutyDefaultArr_pct[i], me->period, me->srcClk_Hz, false) != kStatus_Success) { ALX_PWM_TRACE("ErrChInit"); return Alx_Err; }

	// #4 Set isInit
	me->isInit = true;

	// #5 Start CTIMER
	CTIMER_StartTimer(me->tim);

	// #6 Return OK
	return Alx_Ok;
}
Alx_Status AlxPwm_DeInit(AlxPwm* me)
{
	ALX_PWM_ASSERT(me->isInit == true);
	ALX_PWM_ASSERT(me->wasCtorCalled == true);

	// #1 DeInit CTIMER
	CTIMER_Deinit(me->tim); // "Stop CTIMER" and "DisableClk" happens here

	// #2 DeInit GPIO
	for (uint32_t i = 0; i < me->numOfCh; i++)
		AlxIoPin_DeInit((*(me->ioPinArr + i)));

	// #3 Reset isInit
	me->isInit = false;

	// #4 Return OK
	return Alx_Ok;
}
Alx_Status AlxPwm_SetDuty_pct(AlxPwm* me, Alx_Ch ch, float duty_pct)
{
	ALX_PWM_ASSERT(me->isInit == true);
	ALX_PWM_ASSERT(me->wasCtorCalled == true);
	(void)ch;
	ALX_PWM_ASSERT((0.f <= duty_pct) && (duty_pct <= 100.f));

	for (uint32_t i = 0; i < me->numOfCh; i++)
	{
		if (me->chArr[i] == ch)
		{
			CTIMER_UpdatePwmDutycycle(me->tim, ch, (uint32_t)duty_pct);
			return Alx_Ok;
		}
	}

	ALX_PWM_ASSERT(false); // We shouldn't get here
	return Alx_Err;
}


//******************************************************************************
// Private Functions
//******************************************************************************
static uint32_t AlxPwm_GetCh(Alx_Ch ch)
{
	if (ch == Alx_Ch_0) return kCTIMER_Match_0;
	if (ch == Alx_Ch_1) return kCTIMER_Match_1;
	if (ch == Alx_Ch_2) return kCTIMER_Match_2;
	//if (ch == Alx_Ch_3) return kCTIMER_Match_3; // MF: Match register 3 is for cycle lenght (freq or period)

	ALX_PWM_ASSERT(false); // We shouldn't get here
	return 0;
}
static void AlxPwm_SetSrcClk_SetPrescalerMax(AlxPwm* me)
{
	me->periodMax = 0x0000FFFF;
	me->srcClk_Hz = AlxClk_GetClk_Hz(me->clk, AlxClk_Clk_McuLpc8xx_CoreSysClk_Ctor);
}


#endif // Module Guard
