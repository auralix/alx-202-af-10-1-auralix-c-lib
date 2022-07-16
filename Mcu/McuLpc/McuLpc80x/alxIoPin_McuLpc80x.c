/**
  ******************************************************************************
  * @file		alxIoPin_McuLpc80x.c
  * @brief		Auralix C Library - ALX IO Pin MCU LPC80X Module
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
#include "alxIoPin_McuLpc80x.h"
#include "alxIoPin.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_LPC80X)


//******************************************************************************
// Private Functions
//******************************************************************************
static swm_port_pin_type_t AlxIoPin_GetSwmPortPinIndex(AlxIoPin* me);
static bool AlxIoPin_CheckIfSwmUsed(AlxIoPin* me);
static swm_select_movable_t AlxIoPin_GetSwmMoveFunc(AlxIoPin* me);
static swm_select_fixed_pin_t AlxIoPin_GetSwmFixFunc(AlxIoPin* me);


//******************************************************************************
// Specific Functions
//******************************************************************************
void AlxIoPin_Ctor
(
	AlxIoPin* me,
	uint8_t port,
	uint8_t pin,
	AlxIoPin_Func func,
	uint32_t mode,
	bool isOpenDrain,
	bool dir,
	bool val
)
{
	// Assert
	(void)me;
	(void)port;
	(void)pin;
	(void)func;
	(void)mode;
	(void)isOpenDrain;
	(void)dir;
	(void)val;

	// Parameters
	me->port = port;
	me->pin = pin;
	me->func = func;
	me->mode = mode;
	me->isOpenDrain = isOpenDrain;
	me->dir = dir;
	me->val = val;

	// Variables
	me->swmFunc_isMovable = false;

	// Info
	me->isInit = false;
	me->wasCtorCalled = true;
}
void AlxIoPin_Init(AlxIoPin* me)
{
	// Assert
	ALX_IO_PIN_ASSERT(me->isInit == false);
	ALX_IO_PIN_ASSERT(me->wasCtorCalled == true);

	// #1 Set isInit attribute
	me->isInit = true;

	// #2 Set initial output value, before config
	AlxIoPin_Write(me, me->val);

	// #3.1 Get IoconPortPinIndex
	uint8_t ioconPortPinIndex = AlxPROTECTED_IoPin_GetIoconPortPinIndex(me->pin, me->port);

	// #3.2 Enable IOCON Clk
	CLOCK_EnableClock(kCLOCK_Iocon);

	// #3.3 Set IOCON Mode
	AlxPROTECTED_IoPin_SetIoconMode(me->pin, me->port, me->mode);

	// #3.4 Set Open Drain
	if (me->isOpenDrain)	{ IOCON->PIO[ioconPortPinIndex] |= (0x1 << 10U); }
	else					{ IOCON->PIO[ioconPortPinIndex] &= ~(0x1 << 10U); }

	// #3.5 Disable IOCON Clk
	CLOCK_DisableClock(kCLOCK_Iocon);

	// #4.1 Init if SWM
	if (AlxIoPin_CheckIfSwmUsed(me))
	{
		CLOCK_EnableClock(kCLOCK_Swm);
		if (me->swmFunc_isMovable)
		{
			// #4.1.1 Init if Movable
			swm_select_movable_t swmMoveFunc = AlxIoPin_GetSwmMoveFunc(me);
			swm_port_pin_type_t swmPortPinIndex = AlxIoPin_GetSwmPortPinIndex(me);
			SWM_SetMovablePinSelect(SWM0, swmMoveFunc, swmPortPinIndex);
		}
		else
		{
			// #4.1.2 Init if Fixed
			swm_select_fixed_pin_t swmFixFunc = AlxIoPin_GetSwmFixFunc(me);
			SWM_SetFixedPinSelect(SWM0, swmFixFunc, true);
		}
		CLOCK_DisableClock(kCLOCK_Swm);
	}

	// #4.2 Init if GPIO
	if (me->func == AlxIoPin_Func_GPIO)
	{
		gpio_pin_config_t gpioConfig;

		if (me->dir)	gpioConfig.pinDirection = kGPIO_DigitalOutput;
		else			gpioConfig.pinDirection = kGPIO_DigitalInput;

		gpioConfig.outputLogic = 0U; // MF: Clears output bit

		GPIO_PinInit(GPIO, me->port, me->pin, &gpioConfig);
	}

	// #5 Set initial output value, after config
	AlxIoPin_Write(me, me->val);
}
void AlxIoPin_DeInit(AlxIoPin* me)
{
	// Assert
	ALX_IO_PIN_ASSERT(me->isInit == true);
	ALX_IO_PIN_ASSERT(me->wasCtorCalled == true);

	// #1 DeInit IOCON
	uint8_t ioconPortPinIndex = AlxPROTECTED_IoPin_GetIoconPortPinIndex(me->pin, me->port);

	CLOCK_EnableClock(kCLOCK_Iocon);
	IOCON->PIO[ioconPortPinIndex] &= ~(0x1 << 3U); // MF: Reset Mode (00) Inactive (no pull-down/pull-up)
	IOCON->PIO[ioconPortPinIndex] &= ~(0x1 << 4U);
	IOCON->PIO[ioconPortPinIndex] &= ~(0x1 << 10U); // MF: Reset Open Drain (0)
	CLOCK_DisableClock(kCLOCK_Iocon);

	// #2.1 DeInit if SWM used
	if (AlxIoPin_CheckIfSwmUsed(me))
	{
		CLOCK_EnableClock(kCLOCK_Swm);
		if (me->swmFunc_isMovable)
		{
			swm_select_movable_t swmMoveFunc = AlxIoPin_GetSwmMoveFunc(me);
			SWM_SetMovablePinSelect(SWM0, swmMoveFunc, kSWM_PortPin_Reset);
		}
		else
		{
			swm_select_fixed_pin_t swmFixFunc = AlxIoPin_GetSwmFixFunc(me);
			SWM_SetFixedPinSelect(SWM0, swmFixFunc, false);
		}
		CLOCK_DisableClock(kCLOCK_Swm);
	}

	// #2.2 DeInit if GPIO
	if (me->func == AlxIoPin_Func_GPIO)
	{
		GPIO->DIR[me->port] &= ~(1U << me->pin); // Reset Dir (0)
	}

	// #3 Clear isInit attribute
	me->isInit = false;
}
bool AlxIoPin_Read(AlxIoPin* me)
{
	// Assert
	ALX_IO_PIN_ASSERT(me->isInit == true);
	ALX_IO_PIN_ASSERT(me->wasCtorCalled == true);

	// #1 Read Pin
	if (GPIO_PinRead(GPIO, me->port, me->pin) == 1) { return true; }
	else { return false; }
}
void AlxIoPin_Write(AlxIoPin* me, bool val)
{
	// Assert
	(void)val;
	ALX_IO_PIN_ASSERT(me->isInit == true);
	ALX_IO_PIN_ASSERT(me->wasCtorCalled == true);

	// #1 Write Pin
	if (val) { GPIO_PinWrite(GPIO, me->port, me->pin, 1U); }
	else { GPIO_PinWrite(GPIO, me->port, me->pin, 0U); }
}
void AlxIoPin_Set(AlxIoPin* me)
{
	// Assert
	ALX_IO_PIN_ASSERT(me->isInit == true);
	ALX_IO_PIN_ASSERT(me->wasCtorCalled == true);

	// #1 Set Pin
	GPIO_PortSet(GPIO, me->port, (1U << me->pin));
}
void AlxIoPin_Reset(AlxIoPin* me)
{
	// Assert
	ALX_IO_PIN_ASSERT(me->isInit == true);
	ALX_IO_PIN_ASSERT(me->wasCtorCalled == true);

	// #1 Reset Pin
	GPIO_PortClear(GPIO, me->port, (1U << me->pin));
}
void AlxIoPin_Toggle(AlxIoPin* me)
{
	// Assert
	ALX_IO_PIN_ASSERT(me->isInit == true);
	ALX_IO_PIN_ASSERT(me->wasCtorCalled == true);

	// #1 Toggle Pin
	GPIO_PortToggle(GPIO, me->port, (1U << me->pin));
}
void AlxIoPin_Lock(AlxIoPin* me)
{
	// TODO
	ALX_IO_PIN_ASSERT(false);
}
void AlxIoPin_IrqHandler(AlxIoPin* me)
{
	// TODO
	ALX_IO_PIN_ASSERT(false);
}
void AlxIoPin_Config_PullNone(AlxIoPin* me)
{
	// TODO
	ALX_IO_PIN_ASSERT(false);
}
void AlxIoPin_Config_PullUp(AlxIoPin* me)
{
	// TODO
	ALX_IO_PIN_ASSERT(false);
}
void AlxIoPin_Config_PullDown(AlxIoPin* me)
{
	// TODO
	ALX_IO_PIN_ASSERT(false);
}
void AlxIoPin_Config_AssertOn(AlxIoPin* me)
{
	// TODO
	ALX_IO_PIN_ASSERT(false);
}
void AlxIoPin_Config_AssertOff(AlxIoPin* me)
{
	// TODO
	ALX_IO_PIN_ASSERT(false);
}


//******************************************************************************
// Private Functions
//******************************************************************************
static swm_port_pin_type_t AlxIoPin_GetSwmPortPinIndex(AlxIoPin* me)
{
	if (me->port == 0)		return me->pin;
	else					return (me->pin + 32U);
}
static bool AlxIoPin_CheckIfSwmUsed(AlxIoPin* me)
{
	#if defined(ALX_LPC80X)
	if (me->func <= 38)
	{
		me->swmFunc_isMovable = true;
		return true;
	}
	if ((me->func > 38) && (me->func <= 62))
	{
		me->swmFunc_isMovable = false;
		return true;
	}
	#endif

	#if defined(ALX_LPC84X)
	if (me->func <= 59)
	{
		me->func_isMovable = true;
		return true;
	}
	if ((me->func > 59) && (me->func <= 98))
	{
		me->func_isMovable = false;
		return true;
	}
	#endif

	else { return false; }
}
static swm_select_movable_t AlxIoPin_GetSwmMoveFunc(AlxIoPin* me)
{
	ALX_IO_PIN_ASSERT(me->swmFunc_isMovable == true);

	#if defined(ALX_LPC80X)
	if (me->func == AlxIoPin_Func_Swm_USART0_TXD)		return kSWM_USART0_TXD;
	if (me->func == AlxIoPin_Func_Swm_USART0_RXD)		return kSWM_USART0_RXD;
	if (me->func == AlxIoPin_Func_Swm_USART0_RTS)		return kSWM_USART0_RTS;
	if (me->func == AlxIoPin_Func_Swm_USART0_CTS)		return kSWM_USART0_CTS;
	if (me->func == AlxIoPin_Func_Swm_USART0_SCLK)		return kSWM_USART0_SCLK;

	if (me->func == AlxIoPin_Func_Swm_USART1_TXD)		return kSWM_USART1_TXD;
	if (me->func == AlxIoPin_Func_Swm_USART1_RXD)		return kSWM_USART1_RXD;
	if (me->func == AlxIoPin_Func_Swm_USART1_SCLK)		return kSWM_USART1_SCLK;

	if (me->func == AlxIoPin_Func_Swm_SPI0_SCK)			return kSWM_SPI0_SCK;
	if (me->func == AlxIoPin_Func_Swm_SPI0_MOSI)		return kSWM_SPI0_MOSI;
	if (me->func == AlxIoPin_Func_Swm_SPI0_MISO)		return kSWM_SPI0_MISO;
	if (me->func == AlxIoPin_Func_Swm_SPI0_SSEL0)		return kSWM_SPI0_SSEL0;
	if (me->func == AlxIoPin_Func_Swm_SPI0_SSEL1)		return kSWM_SPI0_SSEL1;

	if (me->func == AlxIoPin_Func_Swm_T0_CAP_CHN0)		return kSWM_T0_CAP_CHN0;
	if (me->func == AlxIoPin_Func_Swm_T0_CAP_CHN1)		return kSWM_T0_CAP_CHN1;
	if (me->func == AlxIoPin_Func_Swm_T0_CAP_CHN2)		return kSWM_T0_CAP_CHN2;

	if (me->func == AlxIoPin_Func_Swm_T0_MAT_CHN0)		return kSWM_T0_MAT_CHN0;
	if (me->func == AlxIoPin_Func_Swm_T0_MAT_CHN1)		return kSWM_T0_MAT_CHN1;
	if (me->func == AlxIoPin_Func_Swm_T0_MAT_CHN2)		return kSWM_T0_MAT_CHN2;
	if (me->func == AlxIoPin_Func_Swm_T0_MAT_CHN3)		return kSWM_T0_MAT_CHN3;

	if (me->func == AlxIoPin_Func_Swm_I2C0_SDA)			return kSWM_I2C0_SDA;
	if (me->func == AlxIoPin_Func_Swm_I2C0_SCL)			return kSWM_I2C0_SCL;

	if (me->func == AlxIoPin_Func_Swm_ACMP_OUT)			return kSWM_ACMP_OUT;
	if (me->func == AlxIoPin_Func_Swm_CLKOUT)			return kSWM_CLKOUT;
	if (me->func == AlxIoPin_Func_Swm_GPIO_INT_BMAT)	return kSWM_GPIO_INT_BMAT;

	if (me->func == AlxIoPin_Func_Swm_LVLSHFT_IN0)		return kSWM_LVLSHFT_IN0;
	if (me->func == AlxIoPin_Func_Swm_LVLSHFT_IN1)		return kSWM_LVLSHFT_IN1;
	if (me->func == AlxIoPin_Func_Swm_LVLSHFT_OUT0)		return kSWM_LVLSHFT_OUT0;
	if (me->func == AlxIoPin_Func_Swm_LVLSHFT_OUT1)		return kSWM_LVLSHFT_OUT1;

	if (me->func == AlxIoPin_Func_Swm_I2C1_SDA)			return kSWM_I2C1_SDA;
	if (me->func == AlxIoPin_Func_Swm_I2C1_SCL)			return kSWM_I2C1_SCL;

	if (me->func == AlxIoPin_Func_Swm_PLU_CLKIN_IN)		return kSWM_PLU_CLKIN_IN;

	if (me->func == AlxIoPin_Func_Swm_CAPT_X0)			return kSWM_CAPT_X0;
	if (me->func == AlxIoPin_Func_Swm_CAPT_X1)			return kSWM_CAPT_X1;
	if (me->func == AlxIoPin_Func_Swm_CAPT_X2)			return kSWM_CAPT_X2;
	if (me->func == AlxIoPin_Func_Swm_CAPT_X3)			return kSWM_CAPT_X3;
	if (me->func == AlxIoPin_Func_Swm_CAPT_X4)			return kSWM_CAPT_X4;
	if (me->func == AlxIoPin_Func_Swm_CAPT_YL)			return kSWM_CAPT_YL;
	if (me->func == AlxIoPin_Func_Swm_CAPT_YH)			return kSWM_CAPT_YH;
	#endif

	#if defined(ALX_LPC84X)
	if (me->func == AlxIoPin_Func_Swm_USART0_TXD)		return kSWM_USART0_TXD;
	if (me->func == AlxIoPin_Func_Swm_USART0_RXD)		return kSWM_USART0_RXD;
	if (me->func == AlxIoPin_Func_Swm_USART0_RTS)		return kSWM_USART0_RTS;
	if (me->func == AlxIoPin_Func_Swm_USART0_CTS)		return kSWM_USART0_CTS;
	if (me->func == AlxIoPin_Func_Swm_USART0_SCLK)		return kSWM_USART0_SCLK;

	if (me->func == AlxIoPin_Func_Swm_USART1_TXD)		return kSWM_USART1_TXD;
	if (me->func == AlxIoPin_Func_Swm_USART1_RXD)		return kSWM_USART1_RXD;
	if (me->func == AlxIoPin_Func_Swm_USART1_RTS)		return kSWM_USART1_RTS;
	if (me->func == AlxIoPin_Func_Swm_USART1_CTS)		return kSWM_USART1_CTS;
	if (me->func == AlxIoPin_Func_Swm_USART1_SCLK)		return kSWM_USART1_SCLK;

	if (me->func == AlxIoPin_Func_Swm_USART2_TXD)		return kSWM_USART2_TXD;
	if (me->func == AlxIoPin_Func_Swm_USART2_RXD)		return kSWM_USART2_RXD;
	if (me->func == AlxIoPin_Func_Swm_USART2_RTS)		return kSWM_USART2_RTS;
	if (me->func == AlxIoPin_Func_Swm_USART2_CTS)		return kSWM_USART2_CTS;
	if (me->func == AlxIoPin_Func_Swm_USART2_SCLK)		return kSWM_USART2_SCLK;

	if (me->func == AlxIoPin_Func_Swm_SPI0_SCK)			return kSWM_SPI0_SCK;
	if (me->func == AlxIoPin_Func_Swm_SPI0_MOSI)		return kSWM_SPI0_MOSI;
	if (me->func == AlxIoPin_Func_Swm_SPI0_MISO)		return kSWM_SPI0_MISO;
	if (me->func == AlxIoPin_Func_Swm_SPI0_SSEL0)		return kSWM_SPI0_SSEL0;
	if (me->func == AlxIoPin_Func_Swm_SPI0_SSEL1)		return kSWM_SPI0_SSEL1;
	if (me->func == AlxIoPin_Func_Swm_SPI0_SSEL2)		return kSWM_SPI0_SSEL2;
	if (me->func == AlxIoPin_Func_Swm_SPI0_SSEL3)		return kSWM_SPI0_SSEL3;

	if (me->func == AlxIoPin_Func_Swm_SPI1_SCK)			return kSWM_SPI1_SCK;
	if (me->func == AlxIoPin_Func_Swm_SPI1_MOSI)		return kSWM_SPI1_MOSI;
	if (me->func == AlxIoPin_Func_Swm_SPI1_MISO)		return kSWM_SPI1_MISO;
	if (me->func == AlxIoPin_Func_Swm_SPI1_SSEL0)		return kSWM_SPI1_SSEL0;
	if (me->func == AlxIoPin_Func_Swm_SPI1_SSEL1)		return kSWM_SPI1_SSEL1;

	if (me->func == AlxIoPin_Func_Swm_SCT_PIN0)			return kSWM_SCT_PIN0;
	if (me->func == AlxIoPin_Func_Swm_SCT_PIN1)			return kSWM_SCT_PIN1;
	if (me->func == AlxIoPin_Func_Swm_SCT_PIN2)			return kSWM_SCT_PIN2;
	if (me->func == AlxIoPin_Func_Swm_SCT_PIN3)			return kSWM_SCT_PIN3;
	if (me->func == AlxIoPin_Func_Swm_SCT_OUT0)			return kSWM_SCT_OUT0;
	if (me->func == AlxIoPin_Func_Swm_SCT_OUT1)			return kSWM_SCT_OUT1;
	if (me->func == AlxIoPin_Func_Swm_SCT_OUT2)			return kSWM_SCT_OUT2;
	if (me->func == AlxIoPin_Func_Swm_SCT_OUT3)			return kSWM_SCT_OUT3;
	if (me->func == AlxIoPin_Func_Swm_SCT_OUT4)			return kSWM_SCT_OUT4;
	if (me->func == AlxIoPin_Func_Swm_SCT_OUT5)			return kSWM_SCT_OUT5;
	if (me->func == AlxIoPin_Func_Swm_SCT_OUT6)			return kSWM_SCT_OUT6;

	if (me->func == AlxIoPin_Func_Swm_I2C1_SDA)			return kSWM_I2C1_SDA;
	if (me->func == AlxIoPin_Func_Swm_I2C1_SCL)			return kSWM_I2C1_SCL;
	if (me->func == AlxIoPin_Func_Swm_I2C2_SDA)			return kSWM_I2C2_SDA;
	if (me->func == AlxIoPin_Func_Swm_I2C2_SCL)			return kSWM_I2C2_SCL;
	if (me->func == AlxIoPin_Func_Swm_I2C3_SDA)			return kSWM_I2C3_SDA;
	if (me->func == AlxIoPin_Func_Swm_I2C3_SCL)			return kSWM_I2C3_SCL;

	if (me->func == AlxIoPin_Func_Swm_ACMP_OUT)			return kSWM_ACMP_OUT;
	if (me->func == AlxIoPin_Func_Swm_CLKOUT)			return kSWM_CLKOUT;
	if (me->func == AlxIoPin_Func_Swm_GPIO_INT_BMAT)	return kSWM_GPIO_INT_BMAT;

	if (me->func == AlxIoPin_Func_Swm_USART3_TXD)		return kSWM_USART3_TXD;
	if (me->func == AlxIoPin_Func_Swm_USART3_RXD)		return kSWM_USART3_RXD;
	if (me->func == AlxIoPin_Func_Swm_USART3_SCLK)		return kSWM_USART3_SCLK;

	if (me->func == AlxIoPin_Func_Swm_USART4_TXD)		return kSWM_USART4_TXD;
	if (me->func == AlxIoPin_Func_Swm_USART4_RXD)		return kSWM_USART4_RXD;
	if (me->func == AlxIoPin_Func_Swm_USART4_SCLK)		return kSWM_USART4_SCLK;

	if (me->func == AlxIoPin_Func_Swm_T0_MAT_CHN0)		return kSWM_T0_MAT_CHN0;
	if (me->func == AlxIoPin_Func_Swm_T0_MAT_CHN1)		return kSWM_T0_MAT_CHN1;
	if (me->func == AlxIoPin_Func_Swm_T0_MAT_CHN2)		return kSWM_T0_MAT_CHN2;
	if (me->func == AlxIoPin_Func_Swm_T0_MAT_CHN3)		return kSWM_T0_MAT_CHN3;
	if (me->func == AlxIoPin_Func_Swm_T0_CAP_CHN0)		return kSWM_T0_CAP_CHN0;
	if (me->func == AlxIoPin_Func_Swm_T0_CAP_CHN1)		return kSWM_T0_CAP_CHN1;
	if (me->func == AlxIoPin_Func_Swm_T0_CAP_CHN2)		return kSWM_T0_CAP_CHN2;
	#endif

	ALX_IO_PIN_ASSERT(false); // We shouldn't get here
	return ALX_NULL;
}
static swm_select_fixed_pin_t AlxIoPin_GetSwmFixFunc(AlxIoPin* me)
{
	ALX_IO_PIN_ASSERT(me->swmFunc_isMovable == false);

	#if defined(ALX_LPC80X)
	if (me->func == AlxIoPin_Func_Swm_ACMP_INPUT1)		return kSWM_ACMP_INPUT1;
	if (me->func == AlxIoPin_Func_Swm_ACMP_INPUT2)		return kSWM_ACMP_INPUT2;
	if (me->func == AlxIoPin_Func_Swm_ACMP_INPUT3)		return kSWM_ACMP_INPUT3;
	if (me->func == AlxIoPin_Func_Swm_ACMP_INPUT4)		return kSWM_ACMP_INPUT4;

	if (me->func == AlxIoPin_Func_Swm_SWCLK)			return kSWM_SWCLK;
	if (me->func == AlxIoPin_Func_Swm_SWDIO)			return kSWM_SWDIO;
	if (me->func == AlxIoPin_Func_Swm_RESETN)			return kSWM_RESETN;
	if (me->func == AlxIoPin_Func_Swm_CLKIN)			return kSWM_CLKIN;
	if (me->func == AlxIoPin_Func_Swm_WKCLKIN)			return kSWM_WKCLKIN;
	if (me->func == AlxIoPin_Func_Swm_VDDCMP)			return kSWM_VDDCMP;

	if (me->func == AlxIoPin_Func_Swm_ADC_CHN0)			return kSWM_ADC_CHN0;
	if (me->func == AlxIoPin_Func_Swm_ADC_CHN1)			return kSWM_ADC_CHN1;
	if (me->func == AlxIoPin_Func_Swm_ADC_CHN2)			return kSWM_ADC_CHN2;
	if (me->func == AlxIoPin_Func_Swm_ADC_CHN3)			return kSWM_ADC_CHN3;
	if (me->func == AlxIoPin_Func_Swm_ADC_CHN4)			return kSWM_ADC_CHN4;
	if (me->func == AlxIoPin_Func_Swm_ADC_CHN5)			return kSWM_ADC_CHN5;
	if (me->func == AlxIoPin_Func_Swm_ADC_CHN6)			return kSWM_ADC_CHN6;
	if (me->func == AlxIoPin_Func_Swm_ADC_CHN7)			return kSWM_ADC_CHN7;
	if (me->func == AlxIoPin_Func_Swm_ADC_CHN8)			return kSWM_ADC_CHN8;
	if (me->func == AlxIoPin_Func_Swm_ADC_CHN9)			return kSWM_ADC_CHN9;
	if (me->func == AlxIoPin_Func_Swm_ADC_CHN10)		return kSWM_ADC_CHN10;
	if (me->func == AlxIoPin_Func_Swm_ADC_CHN11)		return kSWM_ADC_CHN11;

	if (me->func == AlxIoPin_Func_Swm_ACMP_INPUT5)		return kSWM_ACMP_INPUT5;
	if (me->func == AlxIoPin_Func_Swm_DAC_OUT0)		return kSWM_DAC_OUT0;
	#endif

	#if defined(ALX_LPC84X)
	if (me->func == AlxIoPin_Func_Swm_ACMP_INPUT1)		return kSWM_ACMP_INPUT1;
	if (me->func == AlxIoPin_Func_Swm_ACMP_INPUT2)		return kSWM_ACMP_INPUT2;
	if (me->func == AlxIoPin_Func_Swm_ACMP_INPUT3)		return kSWM_ACMP_INPUT3;
	if (me->func == AlxIoPin_Func_Swm_ACMP_INPUT4)		return kSWM_ACMP_INPUT4;
	if (me->func == AlxIoPin_Func_Swm_ACMP_INPUT5)		return kSWM_ACMP_INPUT5;

	if (me->func == AlxIoPin_Func_Swm_SWCLK)			return kSWM_SWCLK;
	if (me->func == AlxIoPin_Func_Swm_SWDIO)			return kSWM_SWDIO;
	if (me->func == AlxIoPin_Func_Swm_XTALIN)			return kSWM_XTALIN;
	if (me->func == AlxIoPin_Func_Swm_XTALOUT)			return kSWM_XTALOUT;
	if (me->func == AlxIoPin_Func_Swm_RESETN)			return kSWM_RESETN;
	if (me->func == AlxIoPin_Func_Swm_CLKIN)			return kSWM_CLKIN;
	if (me->func == AlxIoPin_Func_Swm_VDDCMP)			return kSWM_VDDCMP;
	if (me->func == AlxIoPin_Func_Swm_I2C0_SDA)			return kSWM_I2C0_SDA;
	if (me->func == AlxIoPin_Func_Swm_I2C0_SCL)			return kSWM_I2C0_SCL;

	if (me->func == AlxIoPin_Func_Swm_ADC_CHN0)			return kSWM_ADC_CHN0;
	if (me->func == AlxIoPin_Func_Swm_ADC_CHN1)			return kSWM_ADC_CHN1;
	if (me->func == AlxIoPin_Func_Swm_ADC_CHN2)			return kSWM_ADC_CHN2;
	if (me->func == AlxIoPin_Func_Swm_ADC_CHN3)			return kSWM_ADC_CHN3;
	if (me->func == AlxIoPin_Func_Swm_ADC_CHN4)			return kSWM_ADC_CHN4;
	if (me->func == AlxIoPin_Func_Swm_ADC_CHN5)			return kSWM_ADC_CHN5;
	if (me->func == AlxIoPin_Func_Swm_ADC_CHN6)			return kSWM_ADC_CHN6;
	if (me->func == AlxIoPin_Func_Swm_ADC_CHN7)			return kSWM_ADC_CHN7;
	if (me->func == AlxIoPin_Func_Swm_ADC_CHN8)			return kSWM_ADC_CHN8;
	if (me->func == AlxIoPin_Func_Swm_ADC_CHN9)			return kSWM_ADC_CHN9;
	if (me->func == AlxIoPin_Func_Swm_ADC_CHN10)		return kSWM_ADC_CHN10;
	if (me->func == AlxIoPin_Func_Swm_ADC_CHN11)		return kSWM_ADC_CHN11;

	if (me->func == AlxIoPin_Func_Swm_DAC_OUT0)			return kSWM_DAC_OUT0;
	if (me->func == AlxIoPin_Func_Swm_DAC_OUT1)			return kSWM_DAC_OUT1;
	if (me->func == AlxIoPin_Func_Swm_CAPT_X0)			return kSWM_CAPT_X0;
	if (me->func == AlxIoPin_Func_Swm_CAPT_X1)			return kSWM_CAPT_X1;
	if (me->func == AlxIoPin_Func_Swm_CAPT_X2)			return kSWM_CAPT_X2;
	if (me->func == AlxIoPin_Func_Swm_CAPT_X3)			return kSWM_CAPT_X3;
	if (me->func == AlxIoPin_Func_Swm_CAPT_X4)			return kSWM_CAPT_X4;
	if (me->func == AlxIoPin_Func_Swm_CAPT_X5)			return kSWM_CAPT_X5;
	if (me->func == AlxIoPin_Func_Swm_CAPT_X6)			return kSWM_CAPT_X6;
	if (me->func == AlxIoPin_Func_Swm_CAPT_X7)			return kSWM_CAPT_X7;
	if (me->func == AlxIoPin_Func_Swm_CAPT_X8)			return kSWM_CAPT_X8;
	if (me->func == AlxIoPin_Func_Swm_CAPT_YL)			return kSWM_CAPT_YL;
	if (me->func == AlxIoPin_Func_Swm_CAPT_YH)			return kSWM_CAPT_YH;
	#endif

	ALX_IO_PIN_ASSERT(false); // We shouldn't get here
	return ALX_NULL;
}


#endif // Module Guard
