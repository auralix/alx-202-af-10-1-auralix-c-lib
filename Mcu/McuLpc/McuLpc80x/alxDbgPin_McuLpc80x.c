/**
  ******************************************************************************
  * @file alxDbgPin_McuLpc80x.c
  * @brief Auralix C Library - ALX Debug Pin Module
  * @version $LastChangedRevision: 4270 $
  * @date $LastChangedDate: 2021-03-05 19:02:52 +0100 (Fri, 05 Mar 2021) $
  ******************************************************************************
  */

//******************************************************************************
// Includes
//******************************************************************************
#include "alxDbgPin_McuLpc80x.h"
#include "alxDbgPin.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_LPC80x)


//******************************************************************************
// Variables
//******************************************************************************
AlxDbgPin alxDbgPin = { .port = 0XFF, .pin = 0xFF }; // MF: if { 0 } is set, then assert when try to init Port=0 Pin=0 pin


//******************************************************************************
// Specific Functions
//******************************************************************************
void AlxDbgPin_Ctor
(
	AlxDbgPin* me,
	uint8_t port,
	uint8_t pin,
	uint32_t mode
)
{
	// Parameters
	me->port = port;
	me->pin = pin;
	me->mode = mode;

	// Variables
	me->gpioConfig.pinDirection = kGPIO_DigitalOutput;
	me->gpioConfig.outputLogic = 0U; // Clears output bit

	// Info
	me->isInit = false;
	me->wasCtorCalled = true;
}
void AlxDbgPin_Init(AlxDbgPin* me)
{
	ALX_DBG_PIN_ASSERT(me->isInit == false);
	ALX_DBG_PIN_ASSERT(me->wasCtorCalled == true);

	// #1 Set isInit attribute
	me->isInit = true;

	// #2 Set iOCON Mode
	CLOCK_EnableClock(kCLOCK_Iocon);
	AlxPROTECTED_IoPin_SetIoconMode(me->pin, me->port, me->mode);
	CLOCK_DisableClock(kCLOCK_Iocon);

	// #3 Init GPIO
	GPIO_PinInit(GPIO, me->port, me->pin, &me->gpioConfig);

	// #4 Set Inital value to 0 (Low)
	GPIO_PinWrite(GPIO, me->port, me->pin, 1U);
}
void AlxDbgPin_DeInit(AlxDbgPin* me)
{
	ALX_DBG_PIN_ASSERT(me->isInit == true);
	ALX_DBG_PIN_ASSERT(me->wasCtorCalled == true);

	// #1 DeInit IOCON
	uint8_t ioconPortPinIndex = AlxPROTECTED_IoPin_GetIoconPortPinIndex(me->pin, me->port);

	CLOCK_EnableClock(kCLOCK_Iocon);
	IOCON->PIO[ioconPortPinIndex] &= ~(0x1 << 3U); // Reset Mode (00) Inactive (no pull-down/pull-up)
	IOCON->PIO[ioconPortPinIndex] &= ~(0x1 << 4U);
	IOCON->PIO[ioconPortPinIndex] &= ~(0x1 << 10U); // Reset Open Drain (0)
	CLOCK_DisableClock(kCLOCK_Iocon);

	// #2 DeInit GPIO
	GPIO->DIR[me->port] &= ~(1U << me->pin); // Reset Dir (0)

	// #3 Clear isInit attribute
	me->isInit = false;
}
void AlxDbgPin_Toggle(AlxDbgPin* me)
{
	ALX_DBG_PIN_ASSERT(me->isInit == true);
	ALX_DBG_PIN_ASSERT(me->wasCtorCalled == true);

	GPIO_PortToggle(GPIO, me->port, (1U << me->pin));
}


#endif // Module Guard
