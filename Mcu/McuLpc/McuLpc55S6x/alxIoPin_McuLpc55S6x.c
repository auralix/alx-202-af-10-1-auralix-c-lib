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
static void AlxIoPin_ResetIocon(AlxIoPin* me);


//******************************************************************************
// Constructor
//******************************************************************************
void AlxIoPin_Ctor
(
	AlxIoPin* me,
	uint8_t port,
	uint8_t pin,
	AlxIoPin_Iocon_Func func,
	uint32_t mode,
	bool digiMode,
	bool isOpenDrain,
	bool dir,
	bool val
)
{
	// Assert
	(void)me;
	ALX_IO_PIN_ASSERT(port == 0 || port == 1);
	ALX_IO_PIN_ASSERT(pin >= 0 && pin <= 31);
	ALX_IO_PIN_ASSERT(!(port == 0 && pin == 13));	// MF: PIO0_13 is for i2c and will be Inited in alxI2c module TODO
	ALX_IO_PIN_ASSERT(!(port == 0 && pin == 14));	// MF: PIO0_13 is for i2c and will be Inited in alxI2c module TODO
	(void)func;
	(void)mode;
	(void)digiMode;
	(void)isOpenDrain;
	(void)dir;
	(void)val;

	// Parameters
	me->port = port;
	me->pin = pin;
	me->func = func;
	me->mode = mode;
	me->digiMode = digiMode;
	me->isOpenDrain = isOpenDrain;
	me->dir = dir;
	me->val = val;

	// Info
	me->isInit = false;
	me->wasCtorCalled = true;
}


//******************************************************************************
// Functions
//******************************************************************************
void AlxIoPin_Init(AlxIoPin* me)
{
	// Assert
	ALX_IO_PIN_ASSERT(me->isInit == false);
	ALX_IO_PIN_ASSERT(me->wasCtorCalled == true);

	// #1 Set isInit attribute
	me->isInit = true;

	// #2 Set initial output value, before config
	AlxIoPin_Write(me, me->val);

	// #3 Set IOCON
	// #3.1 Set IOCON Func
	IOCON_PinMuxSet(IOCON, me->port, me->pin, me->func);

	// #3.2 Set IOCON Mode
	AlxPROTECTED_IoPin_SetIoconMode(me->port, me->pin, me->mode);	// MF: Maybe it doesn't have to be PROTECTED and can be IoPin private func, we will see

	// #3.3 Set IOCON DigiMode
	if (me->digiMode)		{ IOCON->PIO[me->port][me->pin] |=  (0x1 << 8U); }
	else					{ IOCON->PIO[me->port][me->pin] &= ~(0x1 << 8U); }

	// #3.4 Set IOCON Open Drain
	if (me->isOpenDrain)	{ IOCON->PIO[me->port][me->pin] |=  (0x1 << 9U); }
	else					{ IOCON->PIO[me->port][me->pin] &= ~(0x1 << 9U); }

	// #4 Init if GPIO
	if (me->func == AlxIoPin_Func_0_GPIO)
	{
		gpio_pin_config_t gpioConfig;

		if (me->dir)	gpioConfig.pinDirection = kGPIO_DigitalOutput;
		else			gpioConfig.pinDirection = kGPIO_DigitalInput;

		gpioConfig.outputLogic = 0U;	// MF: Clears output bit

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

	// #1 Reset IOCON
	AlxIoPin_ResetIocon(me);

	// #2.2 DeInit if GPIO
	if (me->func == AlxIoPin_Func_0_GPIO)
	{
		GPIO->DIR[me->port] &= ~(1U << me->pin);	// MF: Reset Dir (0)
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
static void AlxIoPin_ResetIocon(AlxIoPin* me)
{
	// #1 Set IOCON to Default Value (User Manual page 338)
	if		(me->port == 0 && me->pin == 2)		{ IOCON_PinMuxSet(IOCON, me->port, me->pin, 0x0110); }
	else if (me->port == 0 && me->pin == 5)		{ IOCON_PinMuxSet(IOCON, me->port, me->pin, 0x0120); }
	else if (me->port == 0 && me->pin == 11)	{ IOCON_PinMuxSet(IOCON, me->port, me->pin, 0x0116); }
	else if (me->port == 0 && me->pin == 12)	{ IOCON_PinMuxSet(IOCON, me->port, me->pin, 0x0126); }
	else if (me->port == 0 && me->pin == 13)	{ IOCON_PinMuxSet(IOCON, me->port, me->pin, 0x5000); }
	else if (me->port == 0 && me->pin == 14)	{ IOCON_PinMuxSet(IOCON, me->port, me->pin, 0x5000); }
	else										{ IOCON_PinMuxSet(IOCON, me->port, me->pin, 0x0000); }
}


#endif // Module Guard
