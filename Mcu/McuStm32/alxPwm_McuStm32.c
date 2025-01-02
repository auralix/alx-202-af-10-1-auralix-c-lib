/**
  ******************************************************************************
  * @file		alxPwm_McuStm32.c
  * @brief		Auralix C Library - ALX PWM MCU STM32 Module
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
#include "alxPwm_McuStm32.h"
#include "alxPwm.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB) && (defined(ALX_STM32F1) || defined(ALX_STM32F4) || defined(ALX_STM32F7) || defined(ALX_STM32G4) || defined(ALX_STM32L0) || defined(ALX_STM32L4))


//******************************************************************************
// Private Functions
//******************************************************************************


//------------------------------------------------------------------------------
// Specific
//------------------------------------------------------------------------------
static void AlxPwm_SetSrcClk_SetPeriodMax(AlxPwm* me);
static Alx_Status AlxPwm_RemapIoPin(AlxPwm* me);
static uint32_t AlxPwm_GetCh(Alx_Ch ch);


//------------------------------------------------------------------------------
// General
//------------------------------------------------------------------------------
static void AlxPwm_Periph_EnableClk(AlxPwm* me);
static void AlxPwm_Periph_DisableClk(AlxPwm* me);
static void AlxPwm_Periph_ForceReset(AlxPwm* me);
static void AlxPwm_Periph_ReleaseReset(AlxPwm* me);


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
	//------------------------------------------------------------------------------
	// Assert
	//------------------------------------------------------------------------------
	(void)me;
	(void)tim;
	(void)ioPinArr;
	(void)chArr;
	ALX_PWM_ASSERT(numOfCh <= ALX_PWM_BUFF_LEN);
	(void)clk;
	#if defined(ALX_PWM_OPTIMIZE_SIZE)
	(void)dutyDefaultArr_permil;
	#else
	(void)dutyDefaultArr_pct;
	#endif
	(void)prescaler;
	(void)period;


	//------------------------------------------------------------------------------
	// Parameters
	//------------------------------------------------------------------------------
	me->tim = tim;
	me->ioPinArr = ioPinArr;
	me->chArr = chArr;
	me->numOfCh = numOfCh;
	me->clk = clk;
	#if defined(ALX_PWM_OPTIMIZE_SIZE)
	me->dutyDefaultArr_permil = dutyDefaultArr_permil;
	#else
	me->dutyDefaultArr_pct = dutyDefaultArr_pct;
	#endif
	me->prescaler = prescaler;
	me->period = period;

	// Check channel sequence
	for (uint8_t i = 0; i < numOfCh - 1; i++)
	{
		ALX_PWM_ASSERT(chArr[i] < chArr[i + 1]);	// Channel sequence must be from low to high number
	}


	//------------------------------------------------------------------------------
	// Variables
	//------------------------------------------------------------------------------

	// TIM Common
	me->htim.Instance = tim;
	me->htim.Init.Prescaler = me->prescaler;
	me->htim.Init.CounterMode = TIM_COUNTERMODE_UP;
	me->htim.Init.Period = me->period;
	me->htim.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;	// Not relevant for PWM, used by dead-time generators & digital filters
	me->htim.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
	#if defined(ALX_STM32F1) || defined(ALX_STM32F4) || defined(ALX_STM32F7) || defined(ALX_STM32G4) || defined(ALX_STM32L4)
	me->htim.Init.RepetitionCounter = 0;
	#endif

	// TIM Channel
	for (uint32_t buffPos = 0 ; buffPos < me->numOfCh ; buffPos++)
	{
		me->chtim[buffPos].OCMode = TIM_OCMODE_PWM1;
		me->chtim[buffPos].Pulse = 0;
		me->chtim[buffPos].OCPolarity = TIM_OCPOLARITY_HIGH;	// Will will use active high, so duty = 100% output is high, duty = 0% output is low
		me->chtim[buffPos].OCFastMode = TIM_OCFAST_DISABLE;
		#if defined(ALX_STM32F1) || defined(ALX_STM32F4) || defined(ALX_STM32F7) || defined(ALX_STM32G4) || defined(ALX_STM32L4)
		me->chtim[buffPos].OCNPolarity = TIM_OCNPOLARITY_HIGH;
		me->chtim[buffPos].OCIdleState = TIM_OCIDLESTATE_RESET;	// Only relevant if OCN is used with dead time
		me->chtim[buffPos].OCNIdleState = TIM_OCNIDLESTATE_RESET;
		#endif
		me->ch[buffPos] = *(me->chArr + buffPos);
		#if defined(ALX_PWM_OPTIMIZE_SIZE)
		me->dutyDefaultArr_permil[buffPos] = *(me->dutyDefaultArr_permil + buffPos);
		#else
		me->dutyDefault_pct[buffPos] = *(me->dutyDefaultArr_pct + buffPos);
		#endif
	}

	// Set srcClk_Hz & periodMax
	AlxPwm_SetSrcClk_SetPeriodMax(me);


	//------------------------------------------------------------------------------
	// Info
	//------------------------------------------------------------------------------
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

	// Init GPIO
	for (uint32_t i = 0; i < me->numOfCh; i++)
	{
		AlxIoPin_Init((*(me->ioPinArr + i)));
	}

	// Release TIM periphery reset
	AlxPwm_Periph_ReleaseReset(me);

	// Enable TIM periphery clock
	AlxPwm_Periph_EnableClk(me);

	// Remap TIM IoPins
	if(AlxPwm_RemapIoPin(me) != Alx_Ok) { ALX_PWM_TRACE("Err"); return Alx_Err; };

	// Init TIM_PWM
	if(HAL_TIM_PWM_Init(&me->htim) != HAL_OK) { ALX_PWM_TRACE("Err"); return Alx_Err; }

	// Init TIM_PWM channels
	for (uint32_t i = 0; i < me->numOfCh; i++)
	{
		if (HAL_TIM_PWM_ConfigChannel(&me->htim, &me->chtim[i], AlxPwm_GetCh(me->ch[i])) != HAL_OK) { ALX_PWM_TRACE("Err"); return Alx_Err; }
	}

	// Set isInit
	me->isInit = true;

	// Set TIM_PWM channels default duty
	#if defined(ALX_PWM_OPTIMIZE_SIZE)
	for (uint32_t i = 0; i < me->numOfCh; i++)
	{
		if (AlxPwm_SetDuty_permil(me, me->ch[i], me->dutyDefault_permil[i]) != Alx_Ok) { ALX_PWM_TRACE("Err"); return Alx_Err; }
	}
	#else
	for (uint32_t i = 0; i < me->numOfCh; i++)
	{
		if (AlxPwm_SetDuty_pct(me, me->ch[i], me->dutyDefault_pct[i]) != Alx_Ok) { ALX_PWM_TRACE("Err"); return Alx_Err; }
	}
	#endif

	// Start TIM_PWM channels
	for (uint32_t i = 0; i < me->numOfCh; i++)
	{
		if (HAL_TIM_PWM_Start(&me->htim, AlxPwm_GetCh(me->ch[i])) != HAL_OK) { ALX_PWM_TRACE("Err"); return Alx_Err; }
	}

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

	// DeInit TIM_PWM
	if(HAL_TIM_PWM_DeInit(&me->htim) != HAL_OK) { ALX_PWM_TRACE("Err"); return Alx_Err; }

	// Force TIM periphery reset
	AlxPwm_Periph_ForceReset(me);

	// Disable TIM periphery clock
	AlxPwm_Periph_DisableClk(me);

	// DeInit GPIO
	for (uint32_t i = 0; i < me->numOfCh; i++)
	{
		AlxIoPin_DeInit((*(me->ioPinArr + i)));
	}

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
	// Assert
	ALX_PWM_ASSERT(me->wasCtorCalled == true);
	ALX_PWM_ASSERT(me->isInit == true);
	(void)ch;
	ALX_PWM_ASSERT((0.f <= duty_pct) && (duty_pct <= 100.f));

	// Optimize guard
	#if defined(ALX_PWM_OPTIMIZE_SIZE)
	ALX_PWM_ASSERT(false);
	return Alx_Err;
	#else

	// Set duty
	for (uint32_t i = 0; i < me->numOfCh; i++)
	{
		if (me->ch[i] == ch)
		{
			// Local variables
			uint64_t pulse = (duty_pct * ((float)me->period + 1.f)) / 100.f;		// We must add 1 to period because transition from, example: 0xFFFF to 0 also count..
			if (pulse > (uint64_t)me->periodMax) pulse = me->periodMax;				// Limit uper pulse value

			// Set
			__HAL_TIM_SET_COMPARE(&me->htim, AlxPwm_GetCh(ch), (uint32_t)pulse);	// Here max value can be only 0xFFFF, so we must design system with period that is maximum set 0xFFFE by user, so that we can get clean high level when duty = 100%

			// Return
			return Alx_Ok;
		}
	}

	// Assert
	ALX_PWM_ASSERT(false);	// We should not get here
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
	// Assert
	ALX_PWM_ASSERT(me->wasCtorCalled == true);
	ALX_PWM_ASSERT(me->isInit == true);
	(void)ch;
	ALX_PWM_ASSERT((0 <= duty_permil) && (duty_permil <= 1000));

	// Optimize guard
	#if !(defined (ALX_PWM_OPTIMIZE_SIZE))
	ALX_PWM_ASSERT(false);
	return ALX_NULL;
	#else

	// Set duty
	for (uint32_t i = 0; i < me->numOfCh; i++)
	{
		if (me->ch[i] == ch)
		{
			// Local variables
			uint64_t pulse = (duty_permil * (me->period + 1ul)) / 1000ul;			// We must add 1 to period because transition from, example: 0xFFFF to 0 also count..
			if (pulse > (uint64_t)me->periodMax) pulse = me->periodMax;				// Limit uper pulse value

			// Set
			__HAL_TIM_SET_COMPARE(&me->htim, AlxPwm_GetCh(ch), (uint32_t)pulse);	// Here max value can be only 0xFFFF, so we must design system with period that is maximum set 0xFFFE by user, so that we can get clean high level when duty = 100%

			// Return
			return Alx_Ok;
		}
	}

	// Assert
	ALX_PWM_ASSERT(false);	// We should not get here
	return Alx_Err;
	#endif
}


//******************************************************************************
// Private Functions
//******************************************************************************


//------------------------------------------------------------------------------
// Specific
//------------------------------------------------------------------------------
static void AlxPwm_SetSrcClk_SetPeriodMax(AlxPwm* me)
{
	bool isErr = true;

	#if defined(TIM1)
	if (me->htim.Instance == TIM1)	{ me->periodMax = 0x0000FFFF; me->srcClk_Hz = AlxClk_GetClk_Hz(me->clk, AlxClk_Clk_McuStm32_Pclk2Apb2Tim_Ctor); isErr = false; }
	#endif
	#if defined(TIM2)
	if (me->htim.Instance == TIM2)
	{
		#if defined(ALX_STM32F4) || defined(ALX_STM32F7) || defined(ALX_STM32G4) || defined(ALX_STM32L4)
		me->periodMax = 0xFFFFFFFF;
		#elif defined(ALX_STM32F1) || defined(ALX_STM32L0)
		me->periodMax = 0x0000FFFF;
		#endif

		me->srcClk_Hz = AlxClk_GetClk_Hz(me->clk, AlxClk_Clk_McuStm32_Pclk1Apb1Tim_Ctor);
		isErr = false;
	}
	#endif
	#if defined(TIM3)
	if (me->htim.Instance == TIM3)	{ me->periodMax = 0x0000FFFF; me->srcClk_Hz = AlxClk_GetClk_Hz(me->clk, AlxClk_Clk_McuStm32_Pclk1Apb1Tim_Ctor); isErr = false; }
	#endif
	#if defined(TIM4)
	if (me->htim.Instance == TIM4)	{ me->periodMax = 0x0000FFFF; me->srcClk_Hz = AlxClk_GetClk_Hz(me->clk, AlxClk_Clk_McuStm32_Pclk1Apb1Tim_Ctor); isErr = false; }
	#endif
	#if defined(TIM5)
	if (me->htim.Instance == TIM5)
	{
		#if defined(ALX_STM32F4) || defined(ALX_STM32F7) || defined(ALX_STM32G4) || defined(ALX_STM32L4)
		me->periodMax = 0xFFFFFFFF;
		#endif

		me->srcClk_Hz = AlxClk_GetClk_Hz(me->clk, AlxClk_Clk_McuStm32_Pclk1Apb1Tim_Ctor);
		isErr = false;
	}
	#endif
	#if defined(TIM6)
	if (me->htim.Instance == TIM6)	{ me->periodMax = 0x0000FFFF; me->srcClk_Hz = AlxClk_GetClk_Hz(me->clk, AlxClk_Clk_McuStm32_Pclk1Apb1Tim_Ctor); isErr = false; }
	#endif
	#if defined(TIM7)
	if (me->htim.Instance == TIM7)	{ me->periodMax = 0x0000FFFF; me->srcClk_Hz = AlxClk_GetClk_Hz(me->clk, AlxClk_Clk_McuStm32_Pclk1Apb1Tim_Ctor); isErr = false; }
	#endif
	#if defined(TIM8)
	if (me->htim.Instance == TIM8)	{ me->periodMax = 0x0000FFFF; me->srcClk_Hz = AlxClk_GetClk_Hz(me->clk, AlxClk_Clk_McuStm32_Pclk2Apb2Tim_Ctor); isErr = false; }
	#endif
	#if defined(TIM9)
	if (me->htim.Instance == TIM9)	{ me->periodMax = 0x0000FFFF; me->srcClk_Hz = AlxClk_GetClk_Hz(me->clk, AlxClk_Clk_McuStm32_Pclk2Apb2Tim_Ctor); isErr = false; }
	#endif
	#if defined(TIM10)
	if (me->htim.Instance == TIM10)	{ me->periodMax = 0x0000FFFF; me->srcClk_Hz = AlxClk_GetClk_Hz(me->clk, AlxClk_Clk_McuStm32_Pclk2Apb2Tim_Ctor); isErr = false; }
	#endif
	#if defined(TIM11)
	if (me->htim.Instance == TIM11)	{ me->periodMax = 0x0000FFFF; me->srcClk_Hz = AlxClk_GetClk_Hz(me->clk, AlxClk_Clk_McuStm32_Pclk2Apb2Tim_Ctor); isErr = false; }
	#endif
	#if defined(TIM12)
	if (me->htim.Instance == TIM12)	{ me->periodMax = 0x0000FFFF; me->srcClk_Hz = AlxClk_GetClk_Hz(me->clk, AlxClk_Clk_McuStm32_Pclk1Apb1Tim_Ctor); isErr = false; }
	#endif
	#if defined(TIM13)
	if (me->htim.Instance == TIM13)	{ me->periodMax = 0x0000FFFF; me->srcClk_Hz = AlxClk_GetClk_Hz(me->clk, AlxClk_Clk_McuStm32_Pclk1Apb1Tim_Ctor); isErr = false; }
	#endif
	#if defined(TIM14)
	if (me->htim.Instance == TIM14)	{ me->periodMax = 0x0000FFFF; me->srcClk_Hz = AlxClk_GetClk_Hz(me->clk, AlxClk_Clk_McuStm32_Pclk1Apb1Tim_Ctor); isErr = false; }
	#endif
	#if defined(TIM15)
	if (me->htim.Instance == TIM15)	{ me->periodMax = 0x0000FFFF; me->srcClk_Hz = AlxClk_GetClk_Hz(me->clk, AlxClk_Clk_McuStm32_Pclk2Apb2Tim_Ctor); isErr = false; }
	#endif
	#if defined(TIM16)
	if (me->htim.Instance == TIM16)	{ me->periodMax = 0x0000FFFF; me->srcClk_Hz = AlxClk_GetClk_Hz(me->clk, AlxClk_Clk_McuStm32_Pclk2Apb2Tim_Ctor); isErr = false; }
	#endif
	#if defined(TIM17)
	if (me->htim.Instance == TIM17)	{ me->periodMax = 0x0000FFFF; me->srcClk_Hz = AlxClk_GetClk_Hz(me->clk, AlxClk_Clk_McuStm32_Pclk2Apb2Tim_Ctor); isErr = false; }
	#endif
	#if defined(TIM18)
	if (me->htim.Instance == TIM18)	{ me->periodMax = 0x0000FFFF; me->srcClk_Hz = AlxClk_GetClk_Hz(me->clk, AlxClk_Clk_McuStm32_Pclk1Apb1Tim_Ctor); isErr = false; }
	#endif
	#if defined(TIM19)
	if (me->htim.Instance == TIM19)	{ me->periodMax = 0x0000FFFF; me->srcClk_Hz = AlxClk_GetClk_Hz(me->clk, AlxClk_Clk_McuStm32_Pclk1Apb1Tim_Ctor); isErr = false; }
	#endif
	#if defined(TIM20)
	if (me->htim.Instance == TIM20)	{ me->periodMax = 0x0000FFFF; me->srcClk_Hz = AlxClk_GetClk_Hz(me->clk, AlxClk_Clk_McuStm32_Pclk2Apb2Tim_Ctor); isErr = false; }
	#endif
	#if defined(TIM21)
	if (me->htim.Instance == TIM21)	{ me->periodMax = 0x0000FFFF; me->srcClk_Hz = AlxClk_GetClk_Hz(me->clk, AlxClk_Clk_McuStm32_Pclk2Apb2Tim_Ctor); isErr = false; }
	#endif
	#if defined(TIM22)
	if (me->htim.Instance == TIM22)	{ me->periodMax = 0x0000FFFF; me->srcClk_Hz = AlxClk_GetClk_Hz(me->clk, AlxClk_Clk_McuStm32_Pclk2Apb2Tim_Ctor); isErr = false; }
	#endif

	if(isErr)						{ ALX_PWM_ASSERT(false); }	// We should not get here
}
static Alx_Status AlxPwm_RemapIoPin(AlxPwm* me)
{
	for (uint32_t i = 0; i < me->numOfCh; i++)
	{
		AlxIoPin* ioPin = *(me->ioPinArr + i);

		#if defined(STM32L073xx)
		if
		(
			(me->htim.Instance == TIM3) &&
			(me->ch[i] == Alx_Ch_2) &&
			(ioPin->port == GPIOB) &&
			(ioPin->igpio.Pin == GPIO_PIN_5) &&
			(ioPin->igpio.Alternate == GPIO_AF4_TIM3)
		)
		{
			if(HAL_TIMEx_RemapConfig(&me->htim, TIM3_TI2_GPIOB5_AF4) != HAL_OK) { ALX_PWM_TRACE("Err"); return Alx_Err; };
		}
		#endif
	}

	return Alx_Ok;
}
static uint32_t AlxPwm_GetCh(Alx_Ch ch)
{
	if(ch == Alx_Ch_1 ) return TIM_CHANNEL_1;
	if(ch == Alx_Ch_2 ) return TIM_CHANNEL_2;
	if(ch == Alx_Ch_3 ) return TIM_CHANNEL_3;
	if(ch == Alx_Ch_4 ) return TIM_CHANNEL_4;
	#if defined(ALX_STM32G4)
	if(ch == Alx_Ch_5 ) return TIM_CHANNEL_5;
	if(ch == Alx_Ch_6 ) return TIM_CHANNEL_6;
	#endif

	ALX_PWM_ASSERT(false);	// We should not get here
	return ALX_NULL;
}


//------------------------------------------------------------------------------
// General
//------------------------------------------------------------------------------
static void AlxPwm_Periph_EnableClk(AlxPwm* me)
{
	#if defined(TIM1)
	if (me->htim.Instance == TIM1)	{ __HAL_RCC_TIM1_CLK_ENABLE(); return; }
	#endif
	#if defined(TIM2)
	if (me->htim.Instance == TIM2)	{ __HAL_RCC_TIM2_CLK_ENABLE(); return; }
	#endif
	#if defined(TIM3)
	if (me->htim.Instance == TIM3)	{ __HAL_RCC_TIM3_CLK_ENABLE(); return; }
	#endif
	#if defined(TIM4)
	if (me->htim.Instance == TIM4)	{ __HAL_RCC_TIM4_CLK_ENABLE(); return; }
	#endif
	#if defined(TIM5)
	if (me->htim.Instance == TIM5)	{ __HAL_RCC_TIM5_CLK_ENABLE(); return; }
	#endif
	#if defined(TIM6)
	if (me->htim.Instance == TIM6)	{ __HAL_RCC_TIM6_CLK_ENABLE(); return; }
	#endif
	#if defined(TIM7)
	if (me->htim.Instance == TIM7)	{ __HAL_RCC_TIM7_CLK_ENABLE(); return; }
	#endif
	#if defined(TIM8)
	if (me->htim.Instance == TIM8)	{ __HAL_RCC_TIM8_CLK_ENABLE(); return; }
	#endif
	#if defined(TIM9)
	if (me->htim.Instance == TIM9)	{ __HAL_RCC_TIM9_CLK_ENABLE(); return; }
	#endif
	#if defined(TIM10)
	if (me->htim.Instance == TIM10)	{ __HAL_RCC_TIM10_CLK_ENABLE(); return; }
	#endif
	#if defined(TIM11)
	if (me->htim.Instance == TIM11)	{ __HAL_RCC_TIM11_CLK_ENABLE(); return; }
	#endif
	#if defined(TIM12)
	if (me->htim.Instance == TIM12)	{ __HAL_RCC_TIM12_CLK_ENABLE(); return; }
	#endif
	#if defined(TIM13)
	if (me->htim.Instance == TIM13)	{ __HAL_RCC_TIM13_CLK_ENABLE(); return; }
	#endif
	#if defined(TIM14)
	if (me->htim.Instance == TIM14)	{ __HAL_RCC_TIM14_CLK_ENABLE(); return; }
	#endif
	#if defined(TIM15)
	if (me->htim.Instance == TIM15)	{ __HAL_RCC_TIM15_CLK_ENABLE(); return; }
	#endif
	#if defined(TIM16)
	if (me->htim.Instance == TIM16)	{ __HAL_RCC_TIM16_CLK_ENABLE(); return; }
	#endif
	#if defined(TIM17)
	if (me->htim.Instance == TIM17)	{ __HAL_RCC_TIM17_CLK_ENABLE(); return; }
	#endif
	#if defined(TIM18)
	if (me->htim.Instance == TIM18)	{ __HAL_RCC_TIM18_CLK_ENABLE(); return; }
	#endif
	#if defined(TIM19)
	if (me->htim.Instance == TIM19)	{ __HAL_RCC_TIM19_CLK_ENABLE(); return; }
	#endif
	#if defined(TIM20)
	if (me->htim.Instance == TIM20)	{ __HAL_RCC_TIM20_CLK_ENABLE(); return; }
	#endif
	#if defined(TIM21)
	if (me->htim.Instance == TIM21)	{ __HAL_RCC_TIM21_CLK_ENABLE(); return; }
	#endif
	#if defined(TIM22)
	if (me->htim.Instance == TIM22)	{ __HAL_RCC_TIM22_CLK_ENABLE(); return; }
	#endif

	ALX_PWM_ASSERT(false);	// We should not get here
}
static void AlxPwm_Periph_DisableClk(AlxPwm* me)
{
	#if defined(TIM1)
	if (me->htim.Instance == TIM1)	{ __HAL_RCC_TIM1_CLK_DISABLE(); return; }
	#endif
	#if defined(TIM2)
	if (me->htim.Instance == TIM2)	{ __HAL_RCC_TIM2_CLK_DISABLE(); return; }
	#endif
	#if defined(TIM3)
	if (me->htim.Instance == TIM3)	{ __HAL_RCC_TIM3_CLK_DISABLE(); return; }
	#endif
	#if defined(TIM4)
	if (me->htim.Instance == TIM4)	{ __HAL_RCC_TIM4_CLK_DISABLE(); return; }
	#endif
	#if defined(TIM5)
	if (me->htim.Instance == TIM5)	{ __HAL_RCC_TIM5_CLK_DISABLE(); return; }
	#endif
	#if defined(TIM6)
	if (me->htim.Instance == TIM6)	{ __HAL_RCC_TIM6_CLK_DISABLE(); return; }
	#endif
	#if defined(TIM7)
	if (me->htim.Instance == TIM7)	{ __HAL_RCC_TIM7_CLK_DISABLE(); return; }
	#endif
	#if defined(TIM8)
	if (me->htim.Instance == TIM8)	{ __HAL_RCC_TIM8_CLK_DISABLE(); return; }
	#endif
	#if defined(TIM9)
	if (me->htim.Instance == TIM9)	{ __HAL_RCC_TIM9_CLK_DISABLE(); return; }
	#endif
	#if defined(TIM10)
	if (me->htim.Instance == TIM10)	{ __HAL_RCC_TIM10_CLK_DISABLE(); return; }
	#endif
	#if defined(TIM11)
	if (me->htim.Instance == TIM11)	{ __HAL_RCC_TIM11_CLK_DISABLE(); return; }
	#endif
	#if defined(TIM12)
	if (me->htim.Instance == TIM12)	{ __HAL_RCC_TIM12_CLK_DISABLE(); return; }
	#endif
	#if defined(TIM13)
	if (me->htim.Instance == TIM13)	{ __HAL_RCC_TIM13_CLK_DISABLE(); return; }
	#endif
	#if defined(TIM14)
	if (me->htim.Instance == TIM14)	{ __HAL_RCC_TIM14_CLK_DISABLE(); return; }
	#endif
	#if defined(TIM15)
	if (me->htim.Instance == TIM15)	{ __HAL_RCC_TIM15_CLK_DISABLE(); return; }
	#endif
	#if defined(TIM16)
	if (me->htim.Instance == TIM16)	{ __HAL_RCC_TIM16_CLK_DISABLE(); return; }
	#endif
	#if defined(TIM17)
	if (me->htim.Instance == TIM17)	{ __HAL_RCC_TIM17_CLK_DISABLE(); return; }
	#endif
	#if defined(TIM18)
	if (me->htim.Instance == TIM18)	{ __HAL_RCC_TIM18_CLK_DISABLE(); return; }
	#endif
	#if defined(TIM19)
	if (me->htim.Instance == TIM19)	{ __HAL_RCC_TIM19_CLK_DISABLE(); return; }
	#endif
	#if defined(TIM20)
	if (me->htim.Instance == TIM20)	{ __HAL_RCC_TIM20_CLK_DISABLE(); return; }
	#endif
	#if defined(TIM21)
	if (me->htim.Instance == TIM21)	{ __HAL_RCC_TIM21_CLK_DISABLE(); return; }
	#endif
	#if defined(TIM22)
	if (me->htim.Instance == TIM22)	{ __HAL_RCC_TIM22_CLK_DISABLE(); return; }
	#endif

	ALX_PWM_ASSERT(false);	// We should not get here
}
static void AlxPwm_Periph_ForceReset(AlxPwm* me)
{
	#if defined(TIM1)
	if (me->htim.Instance == TIM1)	{ __HAL_RCC_TIM1_FORCE_RESET(); return; }
	#endif
	#if defined(TIM2)
	if (me->htim.Instance == TIM2)	{ __HAL_RCC_TIM2_FORCE_RESET(); return; }
	#endif
	#if defined(TIM3)
	if (me->htim.Instance == TIM3)	{ __HAL_RCC_TIM3_FORCE_RESET(); return; }
	#endif
	#if defined(TIM4)
	if (me->htim.Instance == TIM4)	{ __HAL_RCC_TIM4_FORCE_RESET(); return; }
	#endif
	#if defined(TIM5)
	if (me->htim.Instance == TIM5)	{ __HAL_RCC_TIM5_FORCE_RESET(); return; }
	#endif
	#if defined(TIM6)
	if (me->htim.Instance == TIM6)	{ __HAL_RCC_TIM6_FORCE_RESET(); return; }
	#endif
	#if defined(TIM7)
	if (me->htim.Instance == TIM7)	{ __HAL_RCC_TIM7_FORCE_RESET(); return; }
	#endif
	#if defined(TIM8)
	if (me->htim.Instance == TIM8)	{ __HAL_RCC_TIM8_FORCE_RESET(); return; }
	#endif
	#if defined(TIM9)
	if (me->htim.Instance == TIM9)	{ __HAL_RCC_TIM9_FORCE_RESET(); return; }
	#endif
	#if defined(TIM10)
	if (me->htim.Instance == TIM10)	{ __HAL_RCC_TIM10_FORCE_RESET(); return; }
	#endif
	#if defined(TIM11)
	if (me->htim.Instance == TIM11)	{ __HAL_RCC_TIM11_FORCE_RESET(); return; }
	#endif
	#if defined(TIM12)
	if (me->htim.Instance == TIM12)	{ __HAL_RCC_TIM12_FORCE_RESET(); return; }
	#endif
	#if defined(TIM13)
	if (me->htim.Instance == TIM13)	{ __HAL_RCC_TIM13_FORCE_RESET(); return; }
	#endif
	#if defined(TIM14)
	if (me->htim.Instance == TIM14)	{ __HAL_RCC_TIM14_FORCE_RESET(); return; }
	#endif
	#if defined(TIM15)
	if (me->htim.Instance == TIM15)	{ __HAL_RCC_TIM15_FORCE_RESET(); return; }
	#endif
	#if defined(TIM16)
	if (me->htim.Instance == TIM16)	{ __HAL_RCC_TIM16_FORCE_RESET(); return; }
	#endif
	#if defined(TIM17)
	if (me->htim.Instance == TIM17)	{ __HAL_RCC_TIM17_FORCE_RESET(); return; }
	#endif
	#if defined(TIM18)
	if (me->htim.Instance == TIM18)	{ __HAL_RCC_TIM18_FORCE_RESET(); return; }
	#endif
	#if defined(TIM19)
	if (me->htim.Instance == TIM19)	{ __HAL_RCC_TIM19_FORCE_RESET(); return; }
	#endif
	#if defined(TIM20)
	if (me->htim.Instance == TIM20)	{ __HAL_RCC_TIM20_FORCE_RESET(); return; }
	#endif
	#if defined(TIM21)
	if (me->htim.Instance == TIM21)	{ __HAL_RCC_TIM21_FORCE_RESET(); return; }
	#endif
	#if defined(TIM22)
	if (me->htim.Instance == TIM22)	{ __HAL_RCC_TIM22_FORCE_RESET(); return; }
	#endif

	ALX_PWM_ASSERT(false);	// We should not get here
}
static void AlxPwm_Periph_ReleaseReset(AlxPwm* me)
{
	#if defined(TIM1)
	if (me->htim.Instance == TIM1)	{ __HAL_RCC_TIM1_RELEASE_RESET(); return; }
	#endif
	#if defined(TIM2)
	if (me->htim.Instance == TIM2)	{ __HAL_RCC_TIM2_RELEASE_RESET(); return; }
	#endif
	#if defined(TIM3)
	if (me->htim.Instance == TIM3)	{ __HAL_RCC_TIM3_RELEASE_RESET(); return; }
	#endif
	#if defined(TIM4)
	if (me->htim.Instance == TIM4)	{ __HAL_RCC_TIM4_RELEASE_RESET(); return; }
	#endif
	#if defined(TIM5)
	if (me->htim.Instance == TIM5)	{ __HAL_RCC_TIM5_RELEASE_RESET(); return; }
	#endif
	#if defined(TIM6)
	if (me->htim.Instance == TIM6)	{ __HAL_RCC_TIM6_RELEASE_RESET(); return; }
	#endif
	#if defined(TIM7)
	if (me->htim.Instance == TIM7)	{ __HAL_RCC_TIM7_RELEASE_RESET(); return; }
	#endif
	#if defined(TIM8)
	if (me->htim.Instance == TIM8)	{ __HAL_RCC_TIM8_RELEASE_RESET(); return; }
	#endif
	#if defined(TIM9)
	if (me->htim.Instance == TIM9)	{ __HAL_RCC_TIM9_RELEASE_RESET(); return; }
	#endif
	#if defined(TIM10)
	if (me->htim.Instance == TIM10)	{ __HAL_RCC_TIM10_RELEASE_RESET(); return; }
	#endif
	#if defined(TIM11)
	if (me->htim.Instance == TIM11)	{ __HAL_RCC_TIM11_RELEASE_RESET(); return; }
	#endif
	#if defined(TIM12)
	if (me->htim.Instance == TIM12)	{ __HAL_RCC_TIM12_RELEASE_RESET(); return; }
	#endif
	#if defined(TIM13)
	if (me->htim.Instance == TIM13)	{ __HAL_RCC_TIM13_RELEASE_RESET(); return; }
	#endif
	#if defined(TIM14)
	if (me->htim.Instance == TIM14)	{ __HAL_RCC_TIM14_RELEASE_RESET(); return; }
	#endif
	#if defined(TIM15)
	if (me->htim.Instance == TIM15)	{ __HAL_RCC_TIM15_RELEASE_RESET(); return; }
	#endif
	#if defined(TIM16)
	if (me->htim.Instance == TIM16)	{ __HAL_RCC_TIM16_RELEASE_RESET(); return; }
	#endif
	#if defined(TIM17)
	if (me->htim.Instance == TIM17)	{ __HAL_RCC_TIM17_RELEASE_RESET(); return; }
	#endif
	#if defined(TIM18)
	if (me->htim.Instance == TIM18)	{ __HAL_RCC_TIM18_RELEASE_RESET(); return; }
	#endif
	#if defined(TIM19)
	if (me->htim.Instance == TIM19)	{ __HAL_RCC_TIM19_RELEASE_RESET(); return; }
	#endif
	#if defined(TIM20)
	if (me->htim.Instance == TIM20)	{ __HAL_RCC_TIM20_RELEASE_RESET(); return; }
	#endif
	#if defined(TIM21)
	if (me->htim.Instance == TIM21)	{ __HAL_RCC_TIM21_RELEASE_RESET(); return; }
	#endif
	#if defined(TIM22)
	if (me->htim.Instance == TIM22)	{ __HAL_RCC_TIM22_RELEASE_RESET(); return; }
	#endif

	ALX_PWM_ASSERT(false);	// We should not get here
}


#endif	// #if defined(ALX_C_LIB) && (defined(ALX_STM32F1) || defined(ALX_STM32F4) || defined(ALX_STM32F7) || defined(ALX_STM32G4) || defined(ALX_STM32L0) || defined(ALX_STM32L4))
