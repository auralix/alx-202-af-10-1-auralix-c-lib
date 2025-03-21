/**
  ******************************************************************************
  * @file		alxPwm_McuSam.c
  * @brief		Auralix C Library - ALX PWM MCU SAM Module
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
#include "alxPwm_McuSam.h"
#include "alxPwm.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB) && defined(ALX_SAM)


//******************************************************************************
// Private Functions
//******************************************************************************
static void AlxPwm_Periph_EnableClk(AlxPwm* me);
static void AlxPwm_Periph_DisableClk(AlxPwm* me);


//******************************************************************************
// Constructor
//******************************************************************************

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		hw
  * @param[in]		do_PWM
  * @param[in]		ch
  * @param[in]		dutyDefault_permil
  * @param[in]		period
  */
void AlxPwm_Ctor
(
	AlxPwm* me,
	void* hw,
	AlxIoPin* do_PWM,
	Alx_Ch ch,
	uint16_t dutyDefault_permil,
	uint16_t period
)
{
	// Parameters
	me->hw = hw;
	me->do_PWM = do_PWM;
	me->ch = ch;
	me->dutyDefault_permil = dutyDefault_permil;
	me->period = period;

	// Variables
	memset(&me->descr, 0, sizeof(me->descr));

	// Info
	me->wasCtorCalled = true;
	me->isInit = false;
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
	ALX_PWM_ASSERT(me->wasCtorCalled == true);
	ALX_PWM_ASSERT(me->isInit == false);
	ALX_PWM_ASSERT((0x0001 <= me->period) && (me->period <= 0xFFFE));

	// Init GPIO
	AlxIoPin_Init(me->do_PWM);

	// Enable TCC clock
	_pm_enable_bus_clock(PM_BUS_APBC, me->hw);
	AlxPwm_Periph_EnableClk(me);

	// Init TCC_PWM
	pwm_init(&me->descr, me->hw, _tcc_get_pwm());	// TV: Always returns OK, resets periphery

	// Set TCC_PWM default duty
	uint32_t pulse = (me->dutyDefault_permil * (me->period + 1)) / 1000;
	pwm_set_parameters(&me->descr, me->period, pulse);	// TV: Always returns OK

	// Enable TCC_PWM
	pwm_enable(&me->descr);	// TV: We will NOT handle return

	// Set isInit
	me->isInit = true;

	// Return
	return Alx_Ok;
}

/**
  * @brief
  * @param[in,out] me
  */
Alx_Status AlxPwm_DeInit(AlxPwm* me)
{
	// Assert
	ALX_PWM_ASSERT(me->wasCtorCalled == true);
	ALX_PWM_ASSERT(me->isInit == true);

	// DeInit TCC_PWM
	pwm_deinit(&me->descr);	// TV: Always returns OK, disables & resets periphery

	// Disable TCC clock
	_pm_disable_bus_clock(PM_BUS_APBC, me->hw);
	AlxPwm_Periph_DisableClk(me);

	// DeInit GPIO
	AlxIoPin_DeInit(me->do_PWM);

	// Clear isInit
	me->isInit = false;

	// Return
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
	// Unsupported
	ALX_PWM_ASSERT(false);
	return ALX_NULL;
}

/**
  * @brief
  * @param[in] me
  * @param[in] ch
  * @param[in] duty_permil
  */
Alx_Status AlxPwm_SetDuty_permil(AlxPwm* me, Alx_Ch ch, uint16_t duty_permil)
{
	// Assert
	ALX_PWM_ASSERT(me->wasCtorCalled == true);
	ALX_PWM_ASSERT(me->isInit == true);
	ALX_PWM_ASSERT(ch == me->ch);
	ALX_PWM_ASSERT((0 <= duty_permil) && (duty_permil <= 1000));

	// Set duty
	uint32_t pulse = (duty_permil * (me->period + 1)) / 1000;
	pwm_set_parameters(&me->descr, me->period, pulse);

	// Return
	return Alx_Ok;
}


//******************************************************************************
// Private Functions
//******************************************************************************
static void AlxPwm_Periph_EnableClk(AlxPwm* me)
{
	#ifdef TCC0
	if (me->hw == TCC0)	{ _gclk_enable_channel(TCC0_GCLK_ID, CONF_GCLK_TCC0_SRC); return; }
	#endif
	#ifdef TCC1
	if (me->hw == TCC1)	{ _gclk_enable_channel(TCC1_GCLK_ID, CONF_GCLK_TCC1_SRC); return; }
	#endif
	#ifdef TCC2
	if (me->hw == TCC2)	{ _gclk_enable_channel(TCC2_GCLK_ID, CONF_GCLK_TCC2_SRC); return; }
	#endif

	ALX_PWM_ASSERT(false);	// We should not get here
}
static void AlxPwm_Periph_DisableClk(AlxPwm* me)
{
	// TV: TODO
}


#endif	// #if defined(ALX_C_LIB) && defined(ALX_SAM)
