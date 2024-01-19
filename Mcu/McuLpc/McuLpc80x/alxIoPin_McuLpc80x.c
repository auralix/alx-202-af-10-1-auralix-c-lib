/**
  ******************************************************************************
  * @file		alxIoPin_McuLpc80x.c
  * @brief		Auralix C Library - ALX IO Pin MCU LPC80X Module
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
#include "alxIoPin_McuLpc80x.h"
#include "alxIoPin.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB) && (defined(ALX_LPC80X) || defined(ALX_LPC84X))


//******************************************************************************
// Private Functions
//******************************************************************************
static swm_port_pin_type_t AlxIoPin_GetSwmPortPinIndex(AlxIoPin* me);
static uint8_t AlxIoPin_GetIoconPortPinIndex(AlxIoPin* me);
static bool AlxIoPin_CheckIfSwmUsed(AlxIoPin* me);
static swm_select_movable_t AlxIoPin_GetSwmMoveFunc(AlxIoPin* me);
static swm_select_fixed_pin_t AlxIoPin_GetSwmFixFunc(AlxIoPin* me);
static void AlxIoPin_SetIoconMode(AlxIoPin* me);


//******************************************************************************
// Constructor
//******************************************************************************

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		port
  * @param[in]		pin
  * @param[in]		func
  * @param[in]		mode
  * @param[in]		isOpenDrain
  * @param[in]		dir
  * @param[in]		val
  */
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


//******************************************************************************
// Functions
//******************************************************************************

/**
  * @brief
  * @param[in,out] me
  */
void AlxIoPin_Init(AlxIoPin* me)
{
	// Assert
	ALX_IO_PIN_ASSERT(me->isInit == false);
	ALX_IO_PIN_ASSERT(me->wasCtorCalled == true);

	// Set isInit
	me->isInit = true;

	// Set initial output value, before config
	AlxIoPin_Write(me, me->val);

	// Get IoconPortPinIndex
	uint8_t ioconPortPinIndex = AlxIoPin_GetIoconPortPinIndex(me);

	// Enable IOCON clock
	CLOCK_EnableClock(kCLOCK_Iocon);

	// Set IOCON mode
	AlxIoPin_SetIoconMode(me);

	// Set open-drain
	if (me->isOpenDrain)
	{
		IOCON->PIO[ioconPortPinIndex] |= (0x1 << 10U);
	}
	else
	{
		IOCON->PIO[ioconPortPinIndex] &= ~(0x1 << 10U);
	}

	// Disable IOCON clock
	CLOCK_DisableClock(kCLOCK_Iocon);

	// Init SWM if used
	if (AlxIoPin_CheckIfSwmUsed(me))
	{
		// Enable SWM clock
		CLOCK_EnableClock(kCLOCK_Swm);

		// Init SWM
		if (me->swmFunc_isMovable)
		{
			// If Movable
			swm_select_movable_t swmMoveFunc = AlxIoPin_GetSwmMoveFunc(me);
			swm_port_pin_type_t swmPortPinIndex = AlxIoPin_GetSwmPortPinIndex(me);
			SWM_SetMovablePinSelect(SWM0, swmMoveFunc, swmPortPinIndex);
		}
		else
		{
			// If Fixed
			swm_select_fixed_pin_t swmFixFunc = AlxIoPin_GetSwmFixFunc(me);
			SWM_SetFixedPinSelect(SWM0, swmFixFunc, true);
		}

		// Disable SWM clock
		CLOCK_DisableClock(kCLOCK_Swm);
	}

	// Init GPIO if used
	if (me->func == AlxIoPin_Func_GPIO)
	{
		// Prepare
		gpio_pin_config_t gpioConfig = {};
		if (me->dir)
		{
			gpioConfig.pinDirection = kGPIO_DigitalOutput;
		}
		else
		{
			gpioConfig.pinDirection = kGPIO_DigitalInput;
		}
		gpioConfig.outputLogic = 0U;	// MF: Clears output bit

		// Init
		GPIO_PinInit(GPIO, me->port, me->pin, &gpioConfig);
	}

	// Set initial output value, after config
	AlxIoPin_Write(me, me->val);
}

/**
  * @brief
  * @param[in,out] me
  */
void AlxIoPin_DeInit(AlxIoPin* me)
{
	// Assert
	ALX_IO_PIN_ASSERT(me->isInit == true);
	ALX_IO_PIN_ASSERT(me->wasCtorCalled == true);

	// DeInit IOCON
	uint8_t ioconPortPinIndex = AlxIoPin_GetIoconPortPinIndex(me);

	// Enable IOCON clock
	CLOCK_EnableClock(kCLOCK_Iocon);

	// Reset
	IOCON->PIO[ioconPortPinIndex] &= ~(0x1 << 3U);		// MF: Reset Mode (00) Inactive (no pull-down/pull-up)
	IOCON->PIO[ioconPortPinIndex] &= ~(0x1 << 4U);
	IOCON->PIO[ioconPortPinIndex] &= ~(0x1 << 10U);		// MF: Reset Open Drain (0)

	// Disable IOCON clock
	CLOCK_DisableClock(kCLOCK_Iocon);

	// DeInit SWM if used
	if (AlxIoPin_CheckIfSwmUsed(me))
	{
		// Enable SWM clock
		CLOCK_EnableClock(kCLOCK_Swm);

		// DeInit SWM
		if (me->swmFunc_isMovable)
		{
			// If Movable
			swm_select_movable_t swmMoveFunc = AlxIoPin_GetSwmMoveFunc(me);
			SWM_SetMovablePinSelect(SWM0, swmMoveFunc, kSWM_PortPin_Reset);
		}
		else
		{
			// If Fixed
			swm_select_fixed_pin_t swmFixFunc = AlxIoPin_GetSwmFixFunc(me);
			SWM_SetFixedPinSelect(SWM0, swmFixFunc, false);
		}

		// Disable SWM clock
		CLOCK_DisableClock(kCLOCK_Swm);
	}

	// DeInit GPIO if used
	if (me->func == AlxIoPin_Func_GPIO)
	{
		GPIO->DIR[me->port] &= ~(1U << me->pin);		// MF: Reset Dir (0)
	}

	// Clear isInit
	me->isInit = false;
}

/**
  * @brief
  * @param[in,out] me
  * @retval		false
  * @retval		true
  */
bool AlxIoPin_Read(AlxIoPin* me)
{
	// Assert
	ALX_IO_PIN_ASSERT(me->isInit == true);
	ALX_IO_PIN_ASSERT(me->wasCtorCalled == true);

	// Read
	if (GPIO_PinRead(GPIO, me->port, me->pin) == 1)
	{
		return true;
	}
	else
	{
		return false;
	}
}

/**
  * @brief
  * @param[in,out] me
  * @param[in]	val
  */
void AlxIoPin_Write(AlxIoPin* me, bool val)
{
	// Assert
	ALX_IO_PIN_ASSERT(me->isInit == true);
	ALX_IO_PIN_ASSERT(me->wasCtorCalled == true);

	// Write
	if (val)
	{
		GPIO_PinWrite(GPIO, me->port, me->pin, 1U);
	}
	else
	{
		GPIO_PinWrite(GPIO, me->port, me->pin, 0U);
	}
}

/**
  * @brief
  * @param[in,out] me
  */
void AlxIoPin_Set(AlxIoPin* me)
{
	// Assert
	ALX_IO_PIN_ASSERT(me->isInit == true);
	ALX_IO_PIN_ASSERT(me->wasCtorCalled == true);

	// Set
	GPIO_PortSet(GPIO, me->port, (1U << me->pin));
}

/**
  * @brief
  * @param[in,out] me
  */
void AlxIoPin_Reset(AlxIoPin* me)
{
	// Assert
	ALX_IO_PIN_ASSERT(me->isInit == true);
	ALX_IO_PIN_ASSERT(me->wasCtorCalled == true);

	// Reset
	GPIO_PortClear(GPIO, me->port, (1U << me->pin));
}

/**
  * @brief
  * @param[in,out] me
  */
void AlxIoPin_Toggle(AlxIoPin* me)
{
	// Assert
	ALX_IO_PIN_ASSERT(me->isInit == true);
	ALX_IO_PIN_ASSERT(me->wasCtorCalled == true);

	// Toggle
	GPIO_PortToggle(GPIO, me->port, (1U << me->pin));
}

/**
  * @brief
  * @param[in,out] me
  * @retval			AlxIoPin_TriState_HiZ
  * @retval			AlxIoPin_TriState_Hi
  * @retval			AlxIoPin_TriState_Lo
  * @retval			AlxIoPin_TriState_Undefined
  */
AlxIoPin_TriState AlxIoPin_Read_TriState(AlxIoPin* me)
{
	//------------------------------------------------------------------------------
	// Assert
	//------------------------------------------------------------------------------
	ALX_IO_PIN_ASSERT(me->wasCtorCalled == true);
	ALX_IO_PIN_ASSERT(me->isInit == true);


	//------------------------------------------------------------------------------
	// Get IoconPortPinIndex
	//------------------------------------------------------------------------------
	uint8_t ioconPortPinIndex = AlxIoPin_GetIoconPortPinIndex(me);


	//------------------------------------------------------------------------------
	// Read @ PullUp
	//------------------------------------------------------------------------------

	// Config PullUp
	IOCON->PIO[ioconPortPinIndex] &= ~(0x1 << 3U);
	IOCON->PIO[ioconPortPinIndex] |= (0x1 << 4U);

	// Wait
	AlxDelay_ms(1);

	// Read
	bool valPullUp = AlxIoPin_Read(me);


	//------------------------------------------------------------------------------
	// Read @ PullDown
	//------------------------------------------------------------------------------

	// Config PullDown
	IOCON->PIO[ioconPortPinIndex] |= (0x1 << 3U);
	IOCON->PIO[ioconPortPinIndex] &= ~(0x1 << 4U);

	// Wait
	AlxDelay_ms(1);

	// Read
	bool valPullDown = AlxIoPin_Read(me);


	//------------------------------------------------------------------------------
	// Return
	//------------------------------------------------------------------------------
	if ((valPullUp == true) && (valPullDown == false))
	{
		return AlxIoPin_TriState_HiZ;
	}
	else if ((valPullUp == true) && (valPullDown == true))
	{
		return AlxIoPin_TriState_Hi;
	}
	else if ((valPullUp == false) && (valPullDown == false))
	{
		return AlxIoPin_TriState_Lo;
	}
	else
	{
		return AlxIoPin_TriState_Undefined;
	}
}


//******************************************************************************
// Private Functions
//******************************************************************************
static swm_port_pin_type_t AlxIoPin_GetSwmPortPinIndex(AlxIoPin* me)
{
	if (me->port == 0)
	{
		return me->pin;
	}
	else
	{
		return (me->pin + 32U);
	}
}
static uint8_t AlxIoPin_GetIoconPortPinIndex(AlxIoPin* me)
{
	#if defined(ALX_LPC80X)
	if (me->pin == 17)		return 0;
	if (me->pin == 13)		return 1;
	if (me->pin == 12)		return 2;
	if (me->pin == 5)		return 3;
	if (me->pin == 4)		return 4;
	if (me->pin == 3)		return 5;
	if (me->pin == 2)		return 6;
	if (me->pin == 11)		return 7;
	if (me->pin == 10)		return 8;
	if (me->pin == 16)		return 9;
	if (me->pin == 15)		return 10;
	if (me->pin == 1)		return 11;
	if (me->pin == 21)		return 12;
	if (me->pin == 9)		return 13;
	if (me->pin == 8)		return 14;
	if (me->pin == 7)		return 15;
	if (me->pin == 29)		return 16;
	if (me->pin == 0)		return 17;
	if (me->pin == 14)		return 18;
	if (me->pin == 28)		return 19;
	if (me->pin == 27)		return 20;
	if (me->pin == 26)		return 21;
	if (me->pin == 20)		return 22;
	if (me->pin == 30)		return 23;
	if (me->pin == 19)		return 24;
	if (me->pin == 25)		return 25;
	if (me->pin == 24)		return 26;
	if (me->pin == 23)		return 27;
	if (me->pin == 22)		return 28;
	if (me->pin == 18)		return 29;
	#endif

	#if defined(ALX_LPC84X)
	if ((me->port == 0) && (me->pin == 17))		return 0;
	if ((me->port == 0) && (me->pin == 13))		return 1;
	if ((me->port == 0) && (me->pin == 12))		return 2;
	if ((me->port == 0) && (me->pin == 5))		return 3;
	if ((me->port == 0) && (me->pin == 4))		return 4;
	if ((me->port == 0) && (me->pin == 3))		return 5;
	if ((me->port == 0) && (me->pin == 2))		return 6;
	if ((me->port == 0) && (me->pin == 11))		return 7;
	if ((me->port == 0) && (me->pin == 10))		return 8;
	if ((me->port == 0) && (me->pin == 16))		return 9;
	if ((me->port == 0) && (me->pin == 15))		return 10;
	if ((me->port == 0) && (me->pin == 1))		return 11;
	if ((me->port == 0) && (me->pin == 9))		return 13;
	if ((me->port == 0) && (me->pin == 8))		return 14;
	if ((me->port == 0) && (me->pin == 7))		return 15;
	if ((me->port == 0) && (me->pin == 6))		return 16;
	if ((me->port == 0) && (me->pin == 0))		return 17;
	if ((me->port == 0) && (me->pin == 14))		return 18;
	if ((me->port == 0) && (me->pin == 28))		return 20;
	if ((me->port == 0) && (me->pin == 27))		return 21;
	if ((me->port == 0) && (me->pin == 26))		return 22;
	if ((me->port == 0) && (me->pin == 25))		return 23;
	if ((me->port == 0) && (me->pin == 24))		return 24;
	if ((me->port == 0) && (me->pin == 23))		return 25;
	if ((me->port == 0) && (me->pin == 22))		return 26;
	if ((me->port == 0) && (me->pin == 21))		return 27;
	if ((me->port == 0) && (me->pin == 20))		return 28;
	if ((me->port == 0) && (me->pin == 19))		return 29;
	if ((me->port == 0) && (me->pin == 18))		return 30;
	if ((me->port == 1) && (me->pin == 8))		return 31;
	if ((me->port == 1) && (me->pin == 9))		return 32;
	if ((me->port == 1) && (me->pin == 12))		return 33;
	if ((me->port == 1) && (me->pin == 13))		return 34;
	if ((me->port == 0) && (me->pin == 31))		return 35;
	if ((me->port == 1) && (me->pin == 0))		return 36;
	if ((me->port == 1) && (me->pin == 1))		return 37;
	if ((me->port == 1) && (me->pin == 2))		return 38;
	if ((me->port == 1) && (me->pin == 14))		return 39;
	if ((me->port == 1) && (me->pin == 15))		return 40;
	if ((me->port == 1) && (me->pin == 3))		return 41;
	if ((me->port == 1) && (me->pin == 4))		return 42;
	if ((me->port == 1) && (me->pin == 5))		return 43;
	if ((me->port == 1) && (me->pin == 16))		return 44;
	if ((me->port == 1) && (me->pin == 17))		return 45;
	if ((me->port == 1) && (me->pin == 6))		return 46;
	if ((me->port == 1) && (me->pin == 18))		return 47;
	if ((me->port == 1) && (me->pin == 19))		return 48;
	if ((me->port == 1) && (me->pin == 7))		return 49;
	if ((me->port == 0) && (me->pin == 29))		return 50;
	if ((me->port == 0) && (me->pin == 30))		return 51;
	if ((me->port == 1) && (me->pin == 20))		return 52;
	if ((me->port == 1) && (me->pin == 21))		return 53;
	if ((me->port == 1) && (me->pin == 11))		return 54;
	if ((me->port == 1) && (me->pin == 10))		return 55;
	#endif

	// MF: Assert cannot be used
	return 255;
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
		me->swmFunc_isMovable = true;
		return true;
	}
	if ((me->func > 59) && (me->func <= 98))
	{
		me->swmFunc_isMovable = false;
		return true;
	}
	#endif

	return false;
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

	ALX_IO_PIN_ASSERT(false);	// We shouldn't get here
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
	if (me->func == AlxIoPin_Func_Swm_DAC_OUT0)			return kSWM_DAC_OUT0;
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

	ALX_IO_PIN_ASSERT(false);	// We shouldn't get here
	return ALX_NULL;
}
static void AlxIoPin_SetIoconMode(AlxIoPin* me)
{
	uint8_t ioconPortPinIndex = AlxIoPin_GetIoconPortPinIndex(me);

	if (me->mode == IOCON_MODE_INACT)
	{
		IOCON->PIO[ioconPortPinIndex] &= ~(0x1 << 3U);
		IOCON->PIO[ioconPortPinIndex] &= ~(0x1 << 4U);
		return;
	}
	if (me->mode == IOCON_MODE_PULLDOWN)
	{
		IOCON->PIO[ioconPortPinIndex] |= (0x1 << 3U);
		IOCON->PIO[ioconPortPinIndex] &= ~(0x1 << 4U);
		return;
	}
	if (me->mode == IOCON_MODE_PULLUP)
	{
		IOCON->PIO[ioconPortPinIndex] &= ~(0x1 << 3U);
		IOCON->PIO[ioconPortPinIndex] |= (0x1 << 4U);
		return;
	}
	if (me->mode == IOCON_MODE_REPEATER)
	{
		IOCON->PIO[ioconPortPinIndex] |= (0x1 << 3U);
		IOCON->PIO[ioconPortPinIndex] |= (0x1 << 4U);
		return;
	}

	ALX_IO_PIN_ASSERT(false);	// We shouldn't get here
}


#endif	// #if defined(ALX_C_LIB) && (defined(ALX_LPC80X) || defined(ALX_LPC84X))
