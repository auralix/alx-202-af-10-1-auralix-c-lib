/**
  ******************************************************************************
  * @file		alxIoPinIrq_McuStm32.c
  * @brief		Auralix C Library - ALX IO Pin IRQ MCU STM32 Module
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
#include "alxIoPinIrq_McuStm32.h"
#include "alxIoPinIrq.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB) && (defined(ALX_STM32F4) || defined(ALX_STM32G4) || defined(ALX_STM32L0) || defined(ALX_STM32L4) || defined(ALX_STM32U5))


//******************************************************************************
// Private Functions
//******************************************************************************


//------------------------------------------------------------------------------
// General
//------------------------------------------------------------------------------
static void AlxIoPinIrq_Periph_EnableIrq(AlxIoPinIrq* me);
static void AlxIoPinIrq_Periph_DisableIrq(AlxIoPinIrq* me);


//******************************************************************************
// Weak Functions
//******************************************************************************
void AlxIoPinIrq_IrqCallback_Pin0(void);
void AlxIoPinIrq_IrqCallback_Pin1(void);
void AlxIoPinIrq_IrqCallback_Pin2(void);
void AlxIoPinIrq_IrqCallback_Pin3(void);
void AlxIoPinIrq_IrqCallback_Pin4(void);
void AlxIoPinIrq_IrqCallback_Pin5(void);
void AlxIoPinIrq_IrqCallback_Pin6(void);
void AlxIoPinIrq_IrqCallback_Pin7(void);
void AlxIoPinIrq_IrqCallback_Pin8(void);
void AlxIoPinIrq_IrqCallback_Pin9(void);
void AlxIoPinIrq_IrqCallback_Pin10(void);
void AlxIoPinIrq_IrqCallback_Pin11(void);
void AlxIoPinIrq_IrqCallback_Pin12(void);
void AlxIoPinIrq_IrqCallback_Pin13(void);
void AlxIoPinIrq_IrqCallback_Pin14(void);
void AlxIoPinIrq_IrqCallback_Pin15(void);


//******************************************************************************
// Constructor
//******************************************************************************

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		ioPinArr
  * @param[in]		numOfIoPins
  * @param[in]		irqPriorityArr
  */
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

	// Parameters
	me->ioPinArr = ioPinArr;
	me->numOfIoPins = numOfIoPins;
	me->irqPriorityArr = irqPriorityArr;

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
void AlxIoPinIrq_Init(AlxIoPinIrq* me)
{
	// Assert
	ALX_IO_PIN_IRQ_ASSERT(me->wasCtorCalled == true);
	ALX_IO_PIN_IRQ_ASSERT(me->isInit == false);

	// Init GPIO
	for (uint32_t i = 0; i < me->numOfIoPins; i++)
	{
		AlxIoPin_Init((*(me->ioPinArr + i)));
	}

	// Enable IRQ
	AlxIoPinIrq_Periph_EnableIrq(me);

	// Set isInit
	me->isInit = true;
}

/**
  * @brief
  * @param[in,out] me
  */
void AlxIoPinIrq_DeInit(AlxIoPinIrq* me)
{
	// Assert
	ALX_IO_PIN_IRQ_ASSERT(me->wasCtorCalled == true);
	ALX_IO_PIN_IRQ_ASSERT(me->isInit == true);

	// Disable IRQ
	AlxIoPinIrq_Periph_DisableIrq(me);

	// DeInit GPIO
	for (uint32_t i = 0; i < me->numOfIoPins; i++)
	{
		AlxIoPin_DeInit((*(me->ioPinArr + i)));
	}

	// Clear isInit
	me->isInit = false;
}


//******************************************************************************
// Private Functions
//******************************************************************************


//------------------------------------------------------------------------------
// General
//------------------------------------------------------------------------------
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
		#if defined(ALX_STM32F4) || defined(ALX_STM32G4) || defined(ALX_STM32L0) || defined(ALX_STM32L4)
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
		#endif
		#if defined(ALX_STM32U5)
		else if (ioPin->igpio.Pin == GPIO_PIN_5)
		{
			HAL_NVIC_SetPriority(EXTI5_IRQn, me->irqPriorityArr[i], 0);
			HAL_NVIC_EnableIRQ(EXTI5_IRQn);
		}
		else if (ioPin->igpio.Pin == GPIO_PIN_6)
		{
			HAL_NVIC_SetPriority(EXTI6_IRQn, me->irqPriorityArr[i], 0);
			HAL_NVIC_EnableIRQ(EXTI6_IRQn);
		}
		else if (ioPin->igpio.Pin == GPIO_PIN_7)
		{
			HAL_NVIC_SetPriority(EXTI7_IRQn, me->irqPriorityArr[i], 0);
			HAL_NVIC_EnableIRQ(EXTI7_IRQn);
		}
		else if (ioPin->igpio.Pin == GPIO_PIN_8)
		{
			HAL_NVIC_SetPriority(EXTI8_IRQn, me->irqPriorityArr[i], 0);
			HAL_NVIC_EnableIRQ(EXTI8_IRQn);
		}
		else if (ioPin->igpio.Pin == GPIO_PIN_9)
		{
			HAL_NVIC_SetPriority(EXTI9_IRQn, me->irqPriorityArr[i], 0);
			HAL_NVIC_EnableIRQ(EXTI9_IRQn);
		}
		else if (ioPin->igpio.Pin == GPIO_PIN_10)
		{
			HAL_NVIC_SetPriority(EXTI10_IRQn, me->irqPriorityArr[i], 0);
			HAL_NVIC_EnableIRQ(EXTI10_IRQn);
		}
		else if (ioPin->igpio.Pin == GPIO_PIN_11)
		{
			HAL_NVIC_SetPriority(EXTI11_IRQn, me->irqPriorityArr[i], 0);
			HAL_NVIC_EnableIRQ(EXTI11_IRQn);
		}
		else if (ioPin->igpio.Pin == GPIO_PIN_12)
		{
			HAL_NVIC_SetPriority(EXTI12_IRQn, me->irqPriorityArr[i], 0);
			HAL_NVIC_EnableIRQ(EXTI12_IRQn);
		}
		else if (ioPin->igpio.Pin == GPIO_PIN_13)
		{
			HAL_NVIC_SetPriority(EXTI13_IRQn, me->irqPriorityArr[i], 0);
			HAL_NVIC_EnableIRQ(EXTI13_IRQn);
		}
		else if (ioPin->igpio.Pin == GPIO_PIN_14)
		{
			HAL_NVIC_SetPriority(EXTI14_IRQn, me->irqPriorityArr[i], 0);
			HAL_NVIC_EnableIRQ(EXTI14_IRQn);
		}
		else if (ioPin->igpio.Pin == GPIO_PIN_15)
		{
			HAL_NVIC_SetPriority(EXTI15_IRQn, me->irqPriorityArr[i], 0);
			HAL_NVIC_EnableIRQ(EXTI15_IRQn);
		}
		#endif
		else
		{
			ALX_IO_PIN_IRQ_ASSERT(false);	// We should not get here
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
		#if defined(ALX_STM32F4) || defined(ALX_STM32G4) || defined(ALX_STM32L0) || defined(ALX_STM32L4)
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
		#endif
		#if defined(ALX_STM32U5)
		else if (ioPin->igpio.Pin == GPIO_PIN_5)
		{
			HAL_NVIC_DisableIRQ(EXTI5_IRQn);
			HAL_NVIC_ClearPendingIRQ(EXTI5_IRQn);
		}
		else if (ioPin->igpio.Pin == GPIO_PIN_6)
		{
			HAL_NVIC_DisableIRQ(EXTI6_IRQn);
			HAL_NVIC_ClearPendingIRQ(EXTI6_IRQn);
		}
		else if (ioPin->igpio.Pin == GPIO_PIN_7)
		{
			HAL_NVIC_DisableIRQ(EXTI7_IRQn);
			HAL_NVIC_ClearPendingIRQ(EXTI7_IRQn);
		}
		else if (ioPin->igpio.Pin == GPIO_PIN_8)
		{
			HAL_NVIC_DisableIRQ(EXTI8_IRQn);
			HAL_NVIC_ClearPendingIRQ(EXTI8_IRQn);
		}
		else if (ioPin->igpio.Pin == GPIO_PIN_9)
		{
			HAL_NVIC_DisableIRQ(EXTI9_IRQn);
			HAL_NVIC_ClearPendingIRQ(EXTI9_IRQn);
		}
		else if (ioPin->igpio.Pin == GPIO_PIN_10)
		{
			HAL_NVIC_DisableIRQ(EXTI10_IRQn);
			HAL_NVIC_ClearPendingIRQ(EXTI10_IRQn);
		}
		else if (ioPin->igpio.Pin == GPIO_PIN_11)
		{
			HAL_NVIC_DisableIRQ(EXTI11_IRQn);
			HAL_NVIC_ClearPendingIRQ(EXTI11_IRQn);
		}
		else if (ioPin->igpio.Pin == GPIO_PIN_12)
		{
			HAL_NVIC_DisableIRQ(EXTI12_IRQn);
			HAL_NVIC_ClearPendingIRQ(EXTI12_IRQn);
		}
		else if (ioPin->igpio.Pin == GPIO_PIN_13)
		{
			HAL_NVIC_DisableIRQ(EXTI13_IRQn);
			HAL_NVIC_ClearPendingIRQ(EXTI13_IRQn);
		}
		else if (ioPin->igpio.Pin == GPIO_PIN_14)
		{
			HAL_NVIC_DisableIRQ(EXTI14_IRQn);
			HAL_NVIC_ClearPendingIRQ(EXTI14_IRQn);
		}
		else if (ioPin->igpio.Pin == GPIO_PIN_15)
		{
			HAL_NVIC_DisableIRQ(EXTI15_IRQn);
			HAL_NVIC_ClearPendingIRQ(EXTI15_IRQn);
		}
		#endif
		else
		{
			ALX_IO_PIN_IRQ_ASSERT(false);	// We should not get here
		}

		__HAL_GPIO_EXTI_CLEAR_IT(ioPin->igpio.Pin);
	}
}


//******************************************************************************
// Weak Functions
//******************************************************************************
#if defined(ALX_STM32F4) || defined(ALX_STM32G4) || defined(ALX_STM32L0) || defined(ALX_STM32L4)
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if		(GPIO_Pin == GPIO_PIN_0 ) AlxIoPinIrq_IrqCallback_Pin0();
	else if	(GPIO_Pin == GPIO_PIN_1 ) AlxIoPinIrq_IrqCallback_Pin1();
	else if	(GPIO_Pin == GPIO_PIN_2 ) AlxIoPinIrq_IrqCallback_Pin2();
	else if	(GPIO_Pin == GPIO_PIN_3 ) AlxIoPinIrq_IrqCallback_Pin3();
	else if	(GPIO_Pin == GPIO_PIN_4 ) AlxIoPinIrq_IrqCallback_Pin4();
	else if	(GPIO_Pin == GPIO_PIN_5 ) AlxIoPinIrq_IrqCallback_Pin5();
	else if	(GPIO_Pin == GPIO_PIN_6 ) AlxIoPinIrq_IrqCallback_Pin6();
	else if	(GPIO_Pin == GPIO_PIN_7 ) AlxIoPinIrq_IrqCallback_Pin7();
	else if	(GPIO_Pin == GPIO_PIN_8 ) AlxIoPinIrq_IrqCallback_Pin8();
	else if	(GPIO_Pin == GPIO_PIN_9 ) AlxIoPinIrq_IrqCallback_Pin9();
	else if	(GPIO_Pin == GPIO_PIN_10) AlxIoPinIrq_IrqCallback_Pin10();
	else if	(GPIO_Pin == GPIO_PIN_11) AlxIoPinIrq_IrqCallback_Pin11();
	else if	(GPIO_Pin == GPIO_PIN_12) AlxIoPinIrq_IrqCallback_Pin12();
	else if	(GPIO_Pin == GPIO_PIN_13) AlxIoPinIrq_IrqCallback_Pin13();
	else if	(GPIO_Pin == GPIO_PIN_14) AlxIoPinIrq_IrqCallback_Pin14();
	else if	(GPIO_Pin == GPIO_PIN_15) AlxIoPinIrq_IrqCallback_Pin15();
	else	ALX_IO_PIN_IRQ_ASSERT(false);	// We should not get here
}
#endif
#if defined(ALX_STM32U5)
void HAL_GPIO_EXTI_Rising_Callback(uint16_t GPIO_Pin)
{
	if		(GPIO_Pin == GPIO_PIN_0 ) AlxIoPinIrq_IrqCallback_Pin0();
	else if	(GPIO_Pin == GPIO_PIN_1 ) AlxIoPinIrq_IrqCallback_Pin1();
	else if	(GPIO_Pin == GPIO_PIN_2 ) AlxIoPinIrq_IrqCallback_Pin2();
	else if	(GPIO_Pin == GPIO_PIN_3 ) AlxIoPinIrq_IrqCallback_Pin3();
	else if	(GPIO_Pin == GPIO_PIN_4 ) AlxIoPinIrq_IrqCallback_Pin4();
	else if	(GPIO_Pin == GPIO_PIN_5 ) AlxIoPinIrq_IrqCallback_Pin5();
	else if	(GPIO_Pin == GPIO_PIN_6 ) AlxIoPinIrq_IrqCallback_Pin6();
	else if	(GPIO_Pin == GPIO_PIN_7 ) AlxIoPinIrq_IrqCallback_Pin7();
	else if	(GPIO_Pin == GPIO_PIN_8 ) AlxIoPinIrq_IrqCallback_Pin8();
	else if	(GPIO_Pin == GPIO_PIN_9 ) AlxIoPinIrq_IrqCallback_Pin9();
	else if	(GPIO_Pin == GPIO_PIN_10) AlxIoPinIrq_IrqCallback_Pin10();
	else if	(GPIO_Pin == GPIO_PIN_11) AlxIoPinIrq_IrqCallback_Pin11();
	else if	(GPIO_Pin == GPIO_PIN_12) AlxIoPinIrq_IrqCallback_Pin12();
	else if	(GPIO_Pin == GPIO_PIN_13) AlxIoPinIrq_IrqCallback_Pin13();
	else if	(GPIO_Pin == GPIO_PIN_14) AlxIoPinIrq_IrqCallback_Pin14();
	else if	(GPIO_Pin == GPIO_PIN_15) AlxIoPinIrq_IrqCallback_Pin15();
	else	ALX_IO_PIN_IRQ_ASSERT(false);	// We should not get here
}
void HAL_GPIO_EXTI_Falling_Callback(uint16_t GPIO_Pin)
{
	if		(GPIO_Pin == GPIO_PIN_0 ) AlxIoPinIrq_IrqCallback_Pin0();
	else if	(GPIO_Pin == GPIO_PIN_1 ) AlxIoPinIrq_IrqCallback_Pin1();
	else if	(GPIO_Pin == GPIO_PIN_2 ) AlxIoPinIrq_IrqCallback_Pin2();
	else if	(GPIO_Pin == GPIO_PIN_3 ) AlxIoPinIrq_IrqCallback_Pin3();
	else if	(GPIO_Pin == GPIO_PIN_4 ) AlxIoPinIrq_IrqCallback_Pin4();
	else if	(GPIO_Pin == GPIO_PIN_5 ) AlxIoPinIrq_IrqCallback_Pin5();
	else if	(GPIO_Pin == GPIO_PIN_6 ) AlxIoPinIrq_IrqCallback_Pin6();
	else if	(GPIO_Pin == GPIO_PIN_7 ) AlxIoPinIrq_IrqCallback_Pin7();
	else if	(GPIO_Pin == GPIO_PIN_8 ) AlxIoPinIrq_IrqCallback_Pin8();
	else if	(GPIO_Pin == GPIO_PIN_9 ) AlxIoPinIrq_IrqCallback_Pin9();
	else if	(GPIO_Pin == GPIO_PIN_10) AlxIoPinIrq_IrqCallback_Pin10();
	else if	(GPIO_Pin == GPIO_PIN_11) AlxIoPinIrq_IrqCallback_Pin11();
	else if	(GPIO_Pin == GPIO_PIN_12) AlxIoPinIrq_IrqCallback_Pin12();
	else if	(GPIO_Pin == GPIO_PIN_13) AlxIoPinIrq_IrqCallback_Pin13();
	else if	(GPIO_Pin == GPIO_PIN_14) AlxIoPinIrq_IrqCallback_Pin14();
	else if	(GPIO_Pin == GPIO_PIN_15) AlxIoPinIrq_IrqCallback_Pin15();
	else	ALX_IO_PIN_IRQ_ASSERT(false);	// We should not get here
}
#endif
ALX_WEAK void AlxIoPinIrq_IrqCallback_Pin0(void)
{
	ALX_IO_PIN_IRQ_TRACE("Err");
	ALX_IO_PIN_IRQ_ASSERT(false);
}
ALX_WEAK void AlxIoPinIrq_IrqCallback_Pin1(void)
{
	ALX_IO_PIN_IRQ_TRACE("Err");
	ALX_IO_PIN_IRQ_ASSERT(false);
}
ALX_WEAK void AlxIoPinIrq_IrqCallback_Pin2(void)
{
	ALX_IO_PIN_IRQ_TRACE("Err");
	ALX_IO_PIN_IRQ_ASSERT(false);
}
ALX_WEAK void AlxIoPinIrq_IrqCallback_Pin3(void)
{
	ALX_IO_PIN_IRQ_TRACE("Err");
	ALX_IO_PIN_IRQ_ASSERT(false);
}
ALX_WEAK void AlxIoPinIrq_IrqCallback_Pin4(void)
{
	ALX_IO_PIN_IRQ_TRACE("Err");
	ALX_IO_PIN_IRQ_ASSERT(false);
}
ALX_WEAK void AlxIoPinIrq_IrqCallback_Pin5(void)
{
	ALX_IO_PIN_IRQ_TRACE("Err");
	ALX_IO_PIN_IRQ_ASSERT(false);
}
ALX_WEAK void AlxIoPinIrq_IrqCallback_Pin6(void)
{
	ALX_IO_PIN_IRQ_TRACE("Err");
	ALX_IO_PIN_IRQ_ASSERT(false);
}
ALX_WEAK void AlxIoPinIrq_IrqCallback_Pin7(void)
{
	ALX_IO_PIN_IRQ_TRACE("Err");
	ALX_IO_PIN_IRQ_ASSERT(false);
}
ALX_WEAK void AlxIoPinIrq_IrqCallback_Pin8(void)
{
	ALX_IO_PIN_IRQ_TRACE("Err");
	ALX_IO_PIN_IRQ_ASSERT(false);
}
ALX_WEAK void AlxIoPinIrq_IrqCallback_Pin9(void)
{
	ALX_IO_PIN_IRQ_TRACE("Err");
	ALX_IO_PIN_IRQ_ASSERT(false);
}
ALX_WEAK void AlxIoPinIrq_IrqCallback_Pin10(void)
{
	ALX_IO_PIN_IRQ_TRACE("Err");
	ALX_IO_PIN_IRQ_ASSERT(false);
}
ALX_WEAK void AlxIoPinIrq_IrqCallback_Pin11(void)
{
	ALX_IO_PIN_IRQ_TRACE("Err");
	ALX_IO_PIN_IRQ_ASSERT(false);
}
ALX_WEAK void AlxIoPinIrq_IrqCallback_Pin12(void)
{
	ALX_IO_PIN_IRQ_TRACE("Err");
	ALX_IO_PIN_IRQ_ASSERT(false);
}
ALX_WEAK void AlxIoPinIrq_IrqCallback_Pin13(void)
{
	ALX_IO_PIN_IRQ_TRACE("Err");
	ALX_IO_PIN_IRQ_ASSERT(false);
}
ALX_WEAK void AlxIoPinIrq_IrqCallback_Pin14(void)
{
	ALX_IO_PIN_IRQ_TRACE("Err");
	ALX_IO_PIN_IRQ_ASSERT(false);
}
ALX_WEAK void AlxIoPinIrq_IrqCallback_Pin15(void)
{
	ALX_IO_PIN_IRQ_TRACE("Err");
	ALX_IO_PIN_IRQ_ASSERT(false);
}


//******************************************************************************
// IRQ Handlers
//******************************************************************************
#if !defined(ALX_IO_PIN_IRQ_HANDLERS_OFF)
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
#if defined(ALX_STM32F4) || defined(ALX_STM32G4) || defined(ALX_STM32L0) || defined(ALX_STM32L4)
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
#endif
#if defined(ALX_STM32U5)
void EXTI5_IRQHandler(void)
{
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_5);
}
void EXTI6_IRQHandler(void)
{
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_6);
}
void EXTI7_IRQHandler(void)
{
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_7);
}
void EXTI8_IRQHandler(void)
{
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_8);
}
void EXTI9_IRQHandler(void)
{
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_9);
}
void EXTI10_IRQHandler(void)
{
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_10);
}
void EXTI11_IRQHandler(void)
{
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_11);
}
void EXTI12_IRQHandler(void)
{
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_12);
}
void EXTI13_IRQHandler(void)
{
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_13);
}
void EXTI14_IRQHandler(void)
{
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_14);
}
void EXTI15_IRQHandler(void)
{
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_15);
}
#endif
#endif


#endif	// #if defined(ALX_C_LIB) && (defined(ALX_STM32F4) || defined(ALX_STM32G4) || defined(ALX_STM32L0) || defined(ALX_STM32L4) || defined(ALX_STM32U5))
