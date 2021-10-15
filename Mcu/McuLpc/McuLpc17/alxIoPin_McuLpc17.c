/**
  ******************************************************************************
  * @file alxIoPin_McuLpc17.c
  * @brief Auralix C Library - ALX IO Pin Module
  * @version $LastChangedRevision: 5455 $
  * @date $LastChangedDate: 2021-06-21 00:16:06 +0200 (Mon, 21 Jun 2021) $
  ******************************************************************************
  */

//******************************************************************************
// Includes
//******************************************************************************
#include "alxIoPin_McuLpc17.h"
#include "alxIoPin.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_LPC17)


//******************************************************************************
// Specific Functions
//******************************************************************************
void AlxIoPin_Init(AlxIoPin* me)
{
	ALX_IO_PIN_ASSERT(me->isInit == false);
	ALX_IO_PIN_ASSERT(me->wasCtorCalled == true);

	// #1 Set isInit attribute
	me->isInit = true;

	// #2 Init GPIO
	AlxIoPin_Write(me, me->val);  // Set initial output value, before config

	Chip_IOCON_PinMux(LPC_IOCON, me->port, me->pin, me->mode, me->func);
	if (me->isOpenDrain)	{ Chip_IOCON_EnableOD(LPC_IOCON, me->port, me->pin); }
	else					{ Chip_IOCON_DisableOD(LPC_IOCON, me->port, me->pin); }
	Chip_GPIO_SetPinDIR(LPC_GPIO, me->port, me->pin, me->dir);

	AlxIoPin_Write(me, me->val);  // Set initial output value, after config
}
void AlxIoPin_DeInit(AlxIoPin* me)
{
	ALX_IO_PIN_ASSERT(me->isInit == true);
	ALX_IO_PIN_ASSERT(me->wasCtorCalled == true);

	// #1 DeInit GPIO
	Chip_IOCON_PinMux(LPC_IOCON, me->port, me->pin, IOCON_MODE_PULLUP, IOCON_FUNC0);
	Chip_IOCON_DisableOD(LPC_IOCON, me->port, me->pin);
	Chip_GPIO_SetPinDIR(LPC_GPIO, me->port, me->pin, false);

	// #2 Clear isInit attribute
	me->isInit = false;
}
void AlxIoPin_ReInit(AlxIoPin* me)
{
	ALX_IO_PIN_ASSERT(me->isInit == true);
	ALX_IO_PIN_ASSERT(me->wasCtorCalled == true);

	AlxIoPin_DeInit(me);
	AlxIoPin_Init(me);
}
bool AlxIoPin_Read(AlxIoPin* me)
{
	ALX_IO_PIN_ASSERT(me->isInit == true);
	ALX_IO_PIN_ASSERT(me->wasCtorCalled == true);

	bool val = Chip_GPIO_GetPinState(LPC_GPIO, me->port, me->pin);
	return val;
}
void AlxIoPin_Write(AlxIoPin* me, bool val)
{
	(void)val;
	ALX_IO_PIN_ASSERT(me->isInit == true);
	ALX_IO_PIN_ASSERT(me->wasCtorCalled == true);

	Chip_GPIO_SetPinState(LPC_GPIO, me->port, me->pin, val);
}
void AlxIoPin_Set(AlxIoPin* me)
{
	ALX_IO_PIN_ASSERT(me->isInit == true);
	ALX_IO_PIN_ASSERT(me->wasCtorCalled == true);

	Chip_GPIO_SetPinState(LPC_GPIO, me->port, me->pin, true);
}
void AlxIoPin_Reset(AlxIoPin* me)
{
	ALX_IO_PIN_ASSERT(me->isInit == true);
	ALX_IO_PIN_ASSERT(me->wasCtorCalled == true);

	Chip_GPIO_SetPinState(LPC_GPIO, me->port, me->pin, false);
}
void AlxIoPin_Toggle(AlxIoPin* me)
{
	ALX_IO_PIN_ASSERT(me->isInit == true);
	ALX_IO_PIN_ASSERT(me->wasCtorCalled == true);

	Chip_GPIO_SetPinToggle(LPC_GPIO, me->port, me->pin);
}
void AlxIoPin_Lock(AlxIoPin* me)
{
	// TODO
}
void AlxIoPin_IrqHandler(AlxIoPin* me)
{
	// TODO
}
void AlxIoPin_Config_PullNone(AlxIoPin* me)
{
	// TODO
}
void AlxIoPin_Config_PullUp(AlxIoPin* me)
{
	// TODO
}
void AlxIoPin_Config_PullDown(AlxIoPin* me)
{
	// TODO
}


#endif
