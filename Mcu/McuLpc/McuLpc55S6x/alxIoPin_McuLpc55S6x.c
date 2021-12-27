/**
  ******************************************************************************
  * @file alxIoPin_McuLpc55S6x.c
  * @brief Auralix C Library - ALX IO Pin Module
  * @version $LastChangedRevision: 4270 $
  * @date $LastChangedDate: 2021-03-05 19:02:52 +0100 (Fri, 05 Mar 2021) $
  ******************************************************************************
  */

//******************************************************************************
// Includes
//******************************************************************************
#include "alxIoPin_McuLpc55S6x.h"
#include "alxIoPin.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_LPC55S6X)


//******************************************************************************
// Private Functions
//******************************************************************************


//******************************************************************************
// Specific Functions
//******************************************************************************
void AlxIoPin_Ctor
(
	AlxIoPin* me,
	uint8_t port,
	uint8_t pin,
	AlxIoPin_Iocon_Func func,
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
	//me->swmFunc_isMovable = false;

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

	// #3.1 Enable IOCON Clk
	CLOCK_EnableClock(kCLOCK_Iocon);

	// #3.3 Set IOCON Func
	IOCON_PinMuxSet(IOCON, me->port, me->pin, me->func);

	// #3.4 Set Open Drain
	if (me->isOpenDrain)	{ IOCON_PinMuxSet(IOCON, me->port, me->pin, IOCON_OPENDRAIN_EN); }
	//else					{ IOCON->PIO[ioconPortPinIndex] &= ~(0x1 << 10U); }

	// #3.5 Disable IOCON Clk
	CLOCK_DisableClock(kCLOCK_Iocon);

	// #4.1 Init if SWM
	/*if (AlxIoPin_CheckIfSwmUsed(me))
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
	}*/

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

	/*CLOCK_EnableClock(kCLOCK_Iocon);
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
	}*/

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


#endif // Module Guard
