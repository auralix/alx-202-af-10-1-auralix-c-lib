/**
  ******************************************************************************
  * @file		alxPwmFake.c
  * @brief		Auralix C Library - ALX PWM Module - PC Unit Test Fake
  * @copyright	Copyright (C) Auralix d.o.o. All rights reserved.
  *
  * A channel remembers the last duty cycle written to it. That is the whole
  * model: a product's PWM outputs are how it drives its switches and bridges,
  * and the number it asks for is the thing a test wants to read back. Nothing
  * here produces a waveform.
  *
  * Channels are told apart by their address and number. On an MCU family the
  * module's own constructor exists and is defined here under the same guard
  * the module header uses.
  ******************************************************************************
  **/

//******************************************************************************
// Includes
//******************************************************************************
#include "alxPwm.h"
#include <string.h>


//******************************************************************************
// Private Variables
//******************************************************************************
#define ALX_PWM_FAKE_NUM_OF_CH 64

static const AlxPwm* alxPwmFake_pwm[ALX_PWM_FAKE_NUM_OF_CH];
static uint32_t alxPwmFake_ch[ALX_PWM_FAKE_NUM_OF_CH];
static float alxPwmFake_duty_pct[ALX_PWM_FAKE_NUM_OF_CH];
static uint32_t alxPwmFake_writeCount[ALX_PWM_FAKE_NUM_OF_CH];
static uint32_t alxPwmFake_used;


//******************************************************************************
// Prototypes - the DLL export surface (no separate header for test fakes)
//******************************************************************************
void AlxPwmFake_Reset(void);
float AlxPwmFake_Duty_pct(const AlxPwm* me, uint32_t ch);
uint32_t AlxPwmFake_WriteCount(const AlxPwm* me, uint32_t ch);


//******************************************************************************
// Private Functions
//******************************************************************************
static uint32_t AlxPwmFake_Slot(const AlxPwm* me, uint32_t ch)
{
	for (uint32_t i = 0; i < alxPwmFake_used; i++)
	{
		if (alxPwmFake_pwm[i] == me && alxPwmFake_ch[i] == ch) { return i; }
	}
	if (alxPwmFake_used >= ALX_PWM_FAKE_NUM_OF_CH) { return ALX_PWM_FAKE_NUM_OF_CH - 1; }

	alxPwmFake_pwm[alxPwmFake_used] = me;
	alxPwmFake_ch[alxPwmFake_used] = ch;
	alxPwmFake_duty_pct[alxPwmFake_used] = 0.f;
	alxPwmFake_writeCount[alxPwmFake_used] = 0;
	return alxPwmFake_used++;
}


//******************************************************************************
// The fake's own controls
//******************************************************************************
void AlxPwmFake_Reset(void)
{
	memset(alxPwmFake_pwm, 0, sizeof(alxPwmFake_pwm));
	memset(alxPwmFake_ch, 0, sizeof(alxPwmFake_ch));
	memset(alxPwmFake_duty_pct, 0, sizeof(alxPwmFake_duty_pct));
	memset(alxPwmFake_writeCount, 0, sizeof(alxPwmFake_writeCount));
	alxPwmFake_used = 0;
}

float AlxPwmFake_Duty_pct(const AlxPwm* me, uint32_t ch)
{
	return alxPwmFake_duty_pct[AlxPwmFake_Slot(me, ch)];
}

uint32_t AlxPwmFake_WriteCount(const AlxPwm* me, uint32_t ch)
{
	return alxPwmFake_writeCount[AlxPwmFake_Slot(me, ch)];
}


//******************************************************************************
// The faked module's own contract
//******************************************************************************
#if defined(ALX_STM32F1) || defined(ALX_STM32F4) || defined(ALX_STM32F7) || defined(ALX_STM32G4) || defined(ALX_STM32L0) || defined(ALX_STM32L4)
void AlxPwm_Ctor
(
	AlxPwm* me,
	TIM_TypeDef* tim,
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
	(void)me; (void)tim; (void)ioPinArr; (void)chArr; (void)numOfCh; (void)clk;
	#if defined(ALX_PWM_OPTIMIZE_SIZE)
	(void)dutyDefaultArr_permil;
	#else
	(void)dutyDefaultArr_pct;
	#endif
	(void)prescaler; (void)period;
}
#endif

Alx_Status AlxPwm_Init(AlxPwm* me)
{
	(void)me;
	return Alx_Ok;
}

Alx_Status AlxPwm_DeInit(AlxPwm* me)
{
	(void)me;
	return Alx_Ok;
}

Alx_Status AlxPwm_SetDuty_pct(AlxPwm* me, Alx_Ch ch, float duty_pct)
{
	uint32_t slot = AlxPwmFake_Slot(me, (uint32_t)ch);
	alxPwmFake_duty_pct[slot] = duty_pct;
	alxPwmFake_writeCount[slot]++;
	return Alx_Ok;
}
