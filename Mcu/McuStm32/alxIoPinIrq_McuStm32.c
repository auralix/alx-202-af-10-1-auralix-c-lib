/**
  ******************************************************************************
  * @file		alxIoPinIrq_McuStm32.c
  * @brief		Auralix C Library - ALX IO Pin IRQ MCU STM32 Module
  * @copyright	Copyright (C) 2020-2022 Auralix d.o.o. All rights reserved.
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
#include "alxIoPinIrq_McuStm32.h"
#include "alxIoPinIrq.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB) && (defined(ALX_STM32F4) || defined(ALX_STM32G4) || defined(ALX_STM32L0))


//******************************************************************************
// Private Functions
//******************************************************************************
static void AlxIoPinIrq_Periph_EnableIrq(AlxIoPinIrq* me);
static void AlxIoPinIrq_Periph_DisableIrq(AlxIoPinIrq* me);


//******************************************************************************
// Weak Functions
//******************************************************************************
void AlxIoPinIrq_Foreground_Callback_Pin0 (void);
void AlxIoPinIrq_Foreground_Callback_Pin1 (void);
void AlxIoPinIrq_Foreground_Callback_Pin2 (void);
void AlxIoPinIrq_Foreground_Callback_Pin3 (void);
void AlxIoPinIrq_Foreground_Callback_Pin4 (void);
void AlxIoPinIrq_Foreground_Callback_Pin5 (void);
void AlxIoPinIrq_Foreground_Callback_Pin6 (void);
void AlxIoPinIrq_Foreground_Callback_Pin7 (void);
void AlxIoPinIrq_Foreground_Callback_Pin8 (void);
void AlxIoPinIrq_Foreground_Callback_Pin9 (void);
void AlxIoPinIrq_Foreground_Callback_Pin10(void);
void AlxIoPinIrq_Foreground_Callback_Pin11(void);
void AlxIoPinIrq_Foreground_Callback_Pin12(void);
void AlxIoPinIrq_Foreground_Callback_Pin13(void);
void AlxIoPinIrq_Foreground_Callback_Pin14(void);
void AlxIoPinIrq_Foreground_Callback_Pin15(void);


//******************************************************************************
// Specific Functions
//******************************************************************************
void AlxIoPinIrq_Ctor
(
	AlxIoPinIrq* me,
	AlxIoPin** ioPinArr,
	uint8_t numOfIoPins,
	Alx_IrqPriority* irqPriorityArr
)
{
	// Assert
	(void)me;
	(void)ioPinArr;
	(void)irqPriorityArr;
	ALX_IO_PIN_IRQ_ASSERT((1 <= numOfIoPins) && (numOfIoPins <= 16));
	for (uint8_t i = 0; i < numOfIoPins - 1; i++) ALX_IO_PIN_IRQ_ASSERT((*(ioPinArr + i))->igpio.Pin != (*(ioPinArr + i + 1))->igpio.Pin);	// All GPIO pin numbers must be different

	// Objects - External
	me->ioPinArr = ioPinArr;

	// Parameters
	me->numOfIoPins = numOfIoPins;
	me->irqPriorityArr = irqPriorityArr;

	// Info
	me->isInit = false;
	me->wasCtorCalled = true;
}
void AlxIoPinIrq_Init(AlxIoPinIrq* me)
{
	ALX_IO_PIN_IRQ_ASSERT(me->isInit == false);
	ALX_IO_PIN_IRQ_ASSERT(me->wasCtorCalled == true);

	// #1 Init GPIO
	for (uint32_t i = 0; i < me->numOfIoPins; i++)
	AlxIoPin_Init((*(me->ioPinArr + i)));

	// #2 Enable IRQ
	AlxIoPinIrq_Periph_EnableIrq(me);

	// #3 Set isInit
	me->isInit = true;
}
void AlxIoPinIrq_DeInit(AlxIoPinIrq* me)
{
	ALX_IO_PIN_IRQ_ASSERT(me->isInit == true);
	ALX_IO_PIN_IRQ_ASSERT(me->wasCtorCalled == true);

	// #1 Disable IRQ
	AlxIoPinIrq_Periph_DisableIrq(me);

	// #2 DeInit GPIO
	for (uint32_t i = 0; i < me->numOfIoPins; i++)
	AlxIoPin_DeInit((*(me->ioPinArr + i)));

	// #3 Reset isInit
	me->isInit = false;
}


//******************************************************************************
// Private Functions
//******************************************************************************
static void AlxIoPinIrq_Periph_EnableIrq(AlxIoPinIrq* me)
{
	for (uint8_t i = 0; i < me->numOfIoPins; i++)
	{
		AlxIoPin* ioPin = *(me->ioPinArr + i);

		if (ioPin->igpio.Pin == GPIO_PIN_0)
		{
			HAL_NVIC_SetPriority(EXTI0_IRQn, me->irqPriorityArr[i], 0);
			HAL_NVIC_EnableIRQ(EXTI0_IRQn);
		}
		else if (ioPin->igpio.Pin == GPIO_PIN_1)
		{
			HAL_NVIC_SetPriority(EXTI1_IRQn, me->irqPriorityArr[i], 0);
			HAL_NVIC_EnableIRQ(EXTI1_IRQn);
		}
		else if (ioPin->igpio.Pin == GPIO_PIN_2)
		{
			HAL_NVIC_SetPriority(EXTI2_IRQn, me->irqPriorityArr[i], 0);
			HAL_NVIC_EnableIRQ(EXTI2_IRQn);
		}
		else if (ioPin->igpio.Pin == GPIO_PIN_3)
		{
			HAL_NVIC_SetPriority(EXTI3_IRQn, me->irqPriorityArr[i], 0);
			HAL_NVIC_EnableIRQ(EXTI3_IRQn);
		}
		else if (ioPin->igpio.Pin == GPIO_PIN_4)
		{
			HAL_NVIC_SetPriority(EXTI4_IRQn, me->irqPriorityArr[i], 0);
			HAL_NVIC_EnableIRQ(EXTI4_IRQn);
		}
		else if
		(
			(ioPin->igpio.Pin == GPIO_PIN_5) ||
			(ioPin->igpio.Pin == GPIO_PIN_6) ||
			(ioPin->igpio.Pin == GPIO_PIN_7) ||
			(ioPin->igpio.Pin == GPIO_PIN_8) ||
			(ioPin->igpio.Pin == GPIO_PIN_9)
		)
		{
			HAL_NVIC_SetPriority(EXTI9_5_IRQn, me->irqPriorityArr[i], 0);
			HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
		}
		else if
		(
			(ioPin->igpio.Pin == GPIO_PIN_10) ||
			(ioPin->igpio.Pin == GPIO_PIN_11) ||
			(ioPin->igpio.Pin == GPIO_PIN_12) ||
			(ioPin->igpio.Pin == GPIO_PIN_13) ||
			(ioPin->igpio.Pin == GPIO_PIN_14) ||
			(ioPin->igpio.Pin == GPIO_PIN_15)
		)
		{
			HAL_NVIC_SetPriority(EXTI15_10_IRQn, me->irqPriorityArr[i], 0);
			HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
		}
		else
		{
			ALX_IO_PIN_IRQ_ASSERT(false); // We shouldn't get here
		}
	}
}
static void AlxIoPinIrq_Periph_DisableIrq(AlxIoPinIrq* me)
{
	for (uint8_t i = 0; i < me->numOfIoPins; i++)
	{
		AlxIoPin* ioPin = *(me->ioPinArr + i);

		if (ioPin->igpio.Pin == GPIO_PIN_0)
		{
			HAL_NVIC_DisableIRQ(EXTI0_IRQn);
			HAL_NVIC_ClearPendingIRQ(EXTI0_IRQn);
		}
		else if (ioPin->igpio.Pin == GPIO_PIN_1)
		{
			HAL_NVIC_DisableIRQ(EXTI1_IRQn);
			HAL_NVIC_ClearPendingIRQ(EXTI1_IRQn);
		}
		else if (ioPin->igpio.Pin == GPIO_PIN_2)
		{
			HAL_NVIC_DisableIRQ(EXTI2_IRQn);
			HAL_NVIC_ClearPendingIRQ(EXTI2_IRQn);
		}
		else if (ioPin->igpio.Pin == GPIO_PIN_3)
		{
			HAL_NVIC_DisableIRQ(EXTI3_IRQn);
			HAL_NVIC_ClearPendingIRQ(EXTI3_IRQn);
		}
		else if (ioPin->igpio.Pin == GPIO_PIN_4)
		{
			HAL_NVIC_DisableIRQ(EXTI4_IRQn);
			HAL_NVIC_ClearPendingIRQ(EXTI4_IRQn);
		}
		else if
		(
			(ioPin->igpio.Pin == GPIO_PIN_5) ||
			(ioPin->igpio.Pin == GPIO_PIN_6) ||
			(ioPin->igpio.Pin == GPIO_PIN_7) ||
			(ioPin->igpio.Pin == GPIO_PIN_8) ||
			(ioPin->igpio.Pin == GPIO_PIN_9)
		)
		{
			HAL_NVIC_DisableIRQ(EXTI9_5_IRQn);
			HAL_NVIC_ClearPendingIRQ(EXTI9_5_IRQn);
		}
		else if
		(
			(ioPin->igpio.Pin == GPIO_PIN_10) ||
			(ioPin->igpio.Pin == GPIO_PIN_11) ||
			(ioPin->igpio.Pin == GPIO_PIN_12) ||
			(ioPin->igpio.Pin == GPIO_PIN_13) ||
			(ioPin->igpio.Pin == GPIO_PIN_14) ||
			(ioPin->igpio.Pin == GPIO_PIN_15)
		)
		{
			HAL_NVIC_DisableIRQ(EXTI15_10_IRQn);
			HAL_NVIC_ClearPendingIRQ(EXTI15_10_IRQn);
		}
		else
		{
			ALX_IO_PIN_IRQ_ASSERT(false); // We shouldn't get here
		}

		__HAL_GPIO_EXTI_CLEAR_IT(ioPin->igpio.Pin);
	}
}


//******************************************************************************
// Weak Functions
//******************************************************************************
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if		(GPIO_Pin == GPIO_PIN_0 ) AlxIoPinIrq_Foreground_Callback_Pin0 ();
	else if	(GPIO_Pin == GPIO_PIN_1 ) AlxIoPinIrq_Foreground_Callback_Pin1 ();
	else if	(GPIO_Pin == GPIO_PIN_2 ) AlxIoPinIrq_Foreground_Callback_Pin2 ();
	else if	(GPIO_Pin == GPIO_PIN_3 ) AlxIoPinIrq_Foreground_Callback_Pin3 ();
	else if	(GPIO_Pin == GPIO_PIN_4 ) AlxIoPinIrq_Foreground_Callback_Pin4 ();
	else if	(GPIO_Pin == GPIO_PIN_5 ) AlxIoPinIrq_Foreground_Callback_Pin5 ();
	else if	(GPIO_Pin == GPIO_PIN_6 ) AlxIoPinIrq_Foreground_Callback_Pin6 ();
	else if	(GPIO_Pin == GPIO_PIN_7 ) AlxIoPinIrq_Foreground_Callback_Pin7 ();
	else if	(GPIO_Pin == GPIO_PIN_8 ) AlxIoPinIrq_Foreground_Callback_Pin8 ();
	else if	(GPIO_Pin == GPIO_PIN_9 ) AlxIoPinIrq_Foreground_Callback_Pin9 ();
	else if	(GPIO_Pin == GPIO_PIN_10) AlxIoPinIrq_Foreground_Callback_Pin10();
	else if	(GPIO_Pin == GPIO_PIN_11) AlxIoPinIrq_Foreground_Callback_Pin11();
	else if	(GPIO_Pin == GPIO_PIN_12) AlxIoPinIrq_Foreground_Callback_Pin12();
	else if	(GPIO_Pin == GPIO_PIN_13) AlxIoPinIrq_Foreground_Callback_Pin13();
	else if	(GPIO_Pin == GPIO_PIN_14) AlxIoPinIrq_Foreground_Callback_Pin14();
	else if	(GPIO_Pin == GPIO_PIN_15) AlxIoPinIrq_Foreground_Callback_Pin15();
	else	ALX_IO_PIN_IRQ_ASSERT(false);	// We shouldn't get here
}
ALX_WEAK void AlxIoPinIrq_Foreground_Callback_Pin0()
{
	ALX_IO_PIN_IRQ_TRACE("Define AlxIoPinIrq_Foreground_Callback_Pin0");
	ALX_IO_PIN_IRQ_ASSERT(false);
}
ALX_WEAK void AlxIoPinIrq_Foreground_Callback_Pin1()
{
	ALX_IO_PIN_IRQ_TRACE("Define AlxIoPinIrq_Foreground_Callback_Pin1");
	ALX_IO_PIN_IRQ_ASSERT(false);
}
ALX_WEAK void AlxIoPinIrq_Foreground_Callback_Pin2()
{
	ALX_IO_PIN_IRQ_TRACE("Define AlxIoPinIrq_Foreground_Callback_Pin2");
	ALX_IO_PIN_IRQ_ASSERT(false);
}
ALX_WEAK void AlxIoPinIrq_Foreground_Callback_Pin3()
{
	ALX_IO_PIN_IRQ_TRACE("Define AlxIoPinIrq_Foreground_Callback_Pin3");
	ALX_IO_PIN_IRQ_ASSERT(false);
}
ALX_WEAK void AlxIoPinIrq_Foreground_Callback_Pin4()
{
	ALX_IO_PIN_IRQ_TRACE("Define AlxIoPinIrq_Foreground_Callback_Pin4");
	ALX_IO_PIN_IRQ_ASSERT(false);
}
ALX_WEAK void AlxIoPinIrq_Foreground_Callback_Pin5()
{
	ALX_IO_PIN_IRQ_TRACE("Define AlxIoPinIrq_Foreground_Callback_Pin5");
	ALX_IO_PIN_IRQ_ASSERT(false);
}
ALX_WEAK void AlxIoPinIrq_Foreground_Callback_Pin6()
{
	ALX_IO_PIN_IRQ_TRACE("Define AlxIoPinIrq_Foreground_Callback_Pin6");
	ALX_IO_PIN_IRQ_ASSERT(false);
}
ALX_WEAK void AlxIoPinIrq_Foreground_Callback_Pin7()
{
	ALX_IO_PIN_IRQ_TRACE("Define AlxIoPinIrq_Foreground_Callback_Pin7");
	ALX_IO_PIN_IRQ_ASSERT(false);
}
ALX_WEAK void AlxIoPinIrq_Foreground_Callback_Pin8()
{
	ALX_IO_PIN_IRQ_TRACE("Define AlxIoPinIrq_Foreground_Callback_Pin8");
	ALX_IO_PIN_IRQ_ASSERT(false);
}
ALX_WEAK void AlxIoPinIrq_Foreground_Callback_Pin9()
{
	ALX_IO_PIN_IRQ_TRACE("Define AlxIoPinIrq_Foreground_Callback_Pin9");
	ALX_IO_PIN_IRQ_ASSERT(false);
}
ALX_WEAK void AlxIoPinIrq_Foreground_Callback_Pin10()
{
	ALX_IO_PIN_IRQ_TRACE("Define AlxIoPinIrq_Foreground_Callback_Pin10");
	ALX_IO_PIN_IRQ_ASSERT(false);
}
ALX_WEAK void AlxIoPinIrq_Foreground_Callback_Pin11()
{
	ALX_IO_PIN_IRQ_TRACE("Define AlxIoPinIrq_Foreground_Callback_Pin11");
	ALX_IO_PIN_IRQ_ASSERT(false);
}
ALX_WEAK void AlxIoPinIrq_Foreground_Callback_Pin12()
{
	ALX_IO_PIN_IRQ_TRACE("Define AlxIoPinIrq_Foreground_Callback_Pin12");
	ALX_IO_PIN_IRQ_ASSERT(false);
}
ALX_WEAK void AlxIoPinIrq_Foreground_Callback_Pin13()
{
	ALX_IO_PIN_IRQ_TRACE("Define AlxIoPinIrq_Foreground_Callback_Pin13");
	ALX_IO_PIN_IRQ_ASSERT(false);
}
ALX_WEAK void AlxIoPinIrq_Foreground_Callback_Pin14()
{
	ALX_IO_PIN_IRQ_TRACE("Define AlxIoPinIrq_Foreground_Callback_Pin14");
	ALX_IO_PIN_IRQ_ASSERT(false);
}
ALX_WEAK void AlxIoPinIrq_Foreground_Callback_Pin15()
{
	ALX_IO_PIN_IRQ_TRACE("Define AlxIoPinIrq_Foreground_Callback_Pin15");
	ALX_IO_PIN_IRQ_ASSERT(false);
}


//******************************************************************************
// IRQ Handlers
//******************************************************************************
void EXTI0_IRQHandler(void)
{
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0);
}
void EXTI1_IRQHandler(void)
{
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_1);
}
void EXTI2_IRQHandler(void)
{
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_2);
}
void EXTI3_IRQHandler(void)
{
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_3);
}
void EXTI4_IRQHandler(void)
{
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_4);
}
void EXTI9_5_IRQHandler(void)
{
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_5);
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_6);
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_7);
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_8);
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_9);
}
void EXTI15_10_IRQHandler(void)
{
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_10);
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_11);
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_12);
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_13);
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_14);
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_15);
}


#endif // #if defined(ALX_C_LIB) && (defined(ALX_STM32F4) || defined(ALX_STM32G4) || defined(ALX_STM32L0))
