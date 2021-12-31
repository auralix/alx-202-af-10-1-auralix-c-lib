﻿/**
  ******************************************************************************
  * @file alxIoPinIrq_McuLpc55S6x.c
  * @brief Auralix C Library - ALX IO Pin IRQ Module
  * @version $LastChangedRevision: 4270 $
  * @date $LastChangedDate: 2021-03-05 19:02:52 +0100 (Fri, 05 Mar 2021) $
  ******************************************************************************
  */

//******************************************************************************
// Includes
//******************************************************************************
#include "alxIoPinIrq_McuLpc55S6x.h"
#include "alxIoPinIrq.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_LPC55S6X)


//******************************************************************************
// Private Functions
//******************************************************************************
static inputmux_connection_t AlxIoPin_GetIrqPortPinSel(AlxIoPinIrq* me);
static IRQn_Type AlxIoPin_GetIrqType(AlxIoPinIrq* me);


//******************************************************************************
// Weak Functions
//******************************************************************************
void AlxIoPinIrq_Foreground_Callback_Pin0();
void AlxIoPinIrq_Foreground_Callback_Pin1();
void AlxIoPinIrq_Foreground_Callback_Pin2();
void AlxIoPinIrq_Foreground_Callback_Pin3();
void AlxIoPinIrq_Foreground_Callback_Pin4();
void AlxIoPinIrq_Foreground_Callback_Pin5();
void AlxIoPinIrq_Foreground_Callback_Pin6();
void AlxIoPinIrq_Foreground_Callback_Pin7();


//******************************************************************************
// Specific Functions
//******************************************************************************
void AlxIoPinIrq_Ctor
(
	AlxIoPinIrq* me,
	AlxIoPin* ioPin,
	pint_pin_int_t irqPin,
	pint_pin_enable_t irqType,
	Alx_IrqPriority irqPriority
)
{
	// Assert
	(void)me;
	(void)ioPin;
	(void)irqPin;
	(void)irqType;
	(void)irqPriority;

	// Parameters
	me->ioPin = ioPin;
	me->irqPin = irqPin;
	me->irqType = irqType;
	me->irqPriority = irqPriority;

	// Info
	me->isInit = false;
	me->wasCtorCalled = true;
}
void AlxIoPinIrq_Init(AlxIoPinIrq* me)
{
	// Assert
	ALX_IO_PIN_IRQ_ASSERT(me->isInit == false);
	ALX_IO_PIN_IRQ_ASSERT(me->wasCtorCalled == true);

	// #1 Set isInit attribute
	me->isInit = true;

	// #2 Init IoPin
	AlxIoPin_Init(me->ioPin);	// MF: This Init func doesn't have to be called for IRQ to work

	// #3 Enable Digital Function
	CLOCK_EnableClock(kCLOCK_Iocon);
	IOCON_PinMuxSet(IOCON, me->ioPin->port, me->ioPin->pin, IOCON_DIGITAL_EN);
	CLOCK_DisableClock(kCLOCK_Iocon);

	// #4 Prepare IRQ Pin
	inputmux_connection_t irqPortPinSel = AlxIoPin_GetIrqPortPinSel(me);
	IRQn_Type irqType = AlxIoPin_GetIrqType(me);

	// #5 Attach IRQ to right Pin
	INPUTMUX_Init(INPUTMUX);	// MF: Enable Inputmux Clk
	INPUTMUX_AttachSignal(INPUTMUX, me->irqPin, irqPortPinSel);
	INPUTMUX_Deinit(INPUTMUX);	// MF: Disable Inputmux Clk

	// #6 Init PINT Periphery
	PINT_Init(PINT);

	// #7 Enable IRQ
	PINT_PinInterruptConfig(PINT, me->irqPin, me->irqType, ALX_NULL_PTR);	// MF: "ALX_NULL_PTR" because we'll use "PIN_INTX_IRQHandler" from startup.s
	NVIC_SetPriority(irqType, (uint32_t)me->irqPriority);					// MF: Set IRQ Priority
	PINT_EnableCallbackByIndex(PINT, me->irqPin);							// MF: Enable IRQ
}
void AlxIoPinIrq_DeInit(AlxIoPinIrq* me)
{
	// Assert
	ALX_IO_PIN_IRQ_ASSERT(me->isInit == true);
	ALX_IO_PIN_IRQ_ASSERT(me->wasCtorCalled == true);

	// #1 DeInit if IRQ
	PINT_DisableCallbackByIndex(PINT, me->irqPin);	// MF: Disable IRQ

	// #2 Disable Clk
	CLOCK_DisableClock(kCLOCK_Pint);

	// #3 Reser Periph
	RESET_PeripheralReset(kPINT_RST_SHIFT_RSTn);

	// #4 Clear isInit attribute
	me->isInit = false;
}


//******************************************************************************
// Private Functions
//******************************************************************************
static inputmux_connection_t AlxIoPin_GetIrqPortPinSel(AlxIoPinIrq* me)
{
	if (me->ioPin->port == 0 && me->ioPin->pin == 0)	return kINPUTMUX_GpioPort0Pin0ToPintsel;
	if (me->ioPin->port == 0 && me->ioPin->pin == 1)	return kINPUTMUX_GpioPort0Pin1ToPintsel;
	if (me->ioPin->port == 0 && me->ioPin->pin == 2)	return kINPUTMUX_GpioPort0Pin2ToPintsel;
	if (me->ioPin->port == 0 && me->ioPin->pin == 3)	return kINPUTMUX_GpioPort0Pin3ToPintsel;
	if (me->ioPin->port == 0 && me->ioPin->pin == 4)	return kINPUTMUX_GpioPort0Pin4ToPintsel;
	if (me->ioPin->port == 0 && me->ioPin->pin == 5)	return kINPUTMUX_GpioPort0Pin5ToPintsel;
	if (me->ioPin->port == 0 && me->ioPin->pin == 6)	return kINPUTMUX_GpioPort0Pin6ToPintsel;
	if (me->ioPin->port == 0 && me->ioPin->pin == 7)	return kINPUTMUX_GpioPort0Pin7ToPintsel;
	if (me->ioPin->port == 0 && me->ioPin->pin == 8)	return kINPUTMUX_GpioPort0Pin8ToPintsel;
	if (me->ioPin->port == 0 && me->ioPin->pin == 9)	return kINPUTMUX_GpioPort0Pin9ToPintsel;
	if (me->ioPin->port == 0 && me->ioPin->pin == 10)	return kINPUTMUX_GpioPort0Pin10ToPintsel;
	if (me->ioPin->port == 0 && me->ioPin->pin == 11)	return kINPUTMUX_GpioPort0Pin11ToPintsel;
	if (me->ioPin->port == 0 && me->ioPin->pin == 12)	return kINPUTMUX_GpioPort0Pin12ToPintsel;
	if (me->ioPin->port == 0 && me->ioPin->pin == 13)	return kINPUTMUX_GpioPort0Pin13ToPintsel;
	if (me->ioPin->port == 0 && me->ioPin->pin == 14)	return kINPUTMUX_GpioPort0Pin14ToPintsel;
	if (me->ioPin->port == 0 && me->ioPin->pin == 15)	return kINPUTMUX_GpioPort0Pin15ToPintsel;
	if (me->ioPin->port == 0 && me->ioPin->pin == 16)	return kINPUTMUX_GpioPort0Pin16ToPintsel;
	if (me->ioPin->port == 0 && me->ioPin->pin == 17)	return kINPUTMUX_GpioPort0Pin17ToPintsel;
	if (me->ioPin->port == 0 && me->ioPin->pin == 18)	return kINPUTMUX_GpioPort0Pin18ToPintsel;
	if (me->ioPin->port == 0 && me->ioPin->pin == 19)	return kINPUTMUX_GpioPort0Pin19ToPintsel;
	if (me->ioPin->port == 0 && me->ioPin->pin == 20)	return kINPUTMUX_GpioPort0Pin20ToPintsel;
	if (me->ioPin->port == 0 && me->ioPin->pin == 21)	return kINPUTMUX_GpioPort0Pin21ToPintsel;
	if (me->ioPin->port == 0 && me->ioPin->pin == 22)	return kINPUTMUX_GpioPort0Pin22ToPintsel;
	if (me->ioPin->port == 0 && me->ioPin->pin == 23)	return kINPUTMUX_GpioPort0Pin23ToPintsel;
	if (me->ioPin->port == 0 && me->ioPin->pin == 24)	return kINPUTMUX_GpioPort0Pin24ToPintsel;
	if (me->ioPin->port == 0 && me->ioPin->pin == 25)	return kINPUTMUX_GpioPort0Pin25ToPintsel;
	if (me->ioPin->port == 0 && me->ioPin->pin == 26)	return kINPUTMUX_GpioPort0Pin26ToPintsel;
	if (me->ioPin->port == 0 && me->ioPin->pin == 27)	return kINPUTMUX_GpioPort0Pin27ToPintsel;
	if (me->ioPin->port == 0 && me->ioPin->pin == 28)	return kINPUTMUX_GpioPort0Pin28ToPintsel;
	if (me->ioPin->port == 0 && me->ioPin->pin == 29)	return kINPUTMUX_GpioPort0Pin29ToPintsel;
	if (me->ioPin->port == 0 && me->ioPin->pin == 30)	return kINPUTMUX_GpioPort0Pin30ToPintsel;
	if (me->ioPin->port == 0 && me->ioPin->pin == 31)	return kINPUTMUX_GpioPort0Pin31ToPintsel;

	if (me->ioPin->port == 1 && me->ioPin->pin == 0)	return kINPUTMUX_GpioPort1Pin0ToPintsel;
	if (me->ioPin->port == 1 && me->ioPin->pin == 1)	return kINPUTMUX_GpioPort1Pin1ToPintsel;
	if (me->ioPin->port == 1 && me->ioPin->pin == 2)	return kINPUTMUX_GpioPort1Pin2ToPintsel;
	if (me->ioPin->port == 1 && me->ioPin->pin == 3)	return kINPUTMUX_GpioPort1Pin3ToPintsel;
	if (me->ioPin->port == 1 && me->ioPin->pin == 4)	return kINPUTMUX_GpioPort1Pin4ToPintsel;
	if (me->ioPin->port == 1 && me->ioPin->pin == 5)	return kINPUTMUX_GpioPort1Pin5ToPintsel;
	if (me->ioPin->port == 1 && me->ioPin->pin == 6)	return kINPUTMUX_GpioPort1Pin6ToPintsel;
	if (me->ioPin->port == 1 && me->ioPin->pin == 7)	return kINPUTMUX_GpioPort1Pin7ToPintsel;
	if (me->ioPin->port == 1 && me->ioPin->pin == 8)	return kINPUTMUX_GpioPort1Pin8ToPintsel;
	if (me->ioPin->port == 1 && me->ioPin->pin == 9)	return kINPUTMUX_GpioPort1Pin9ToPintsel;
	if (me->ioPin->port == 1 && me->ioPin->pin == 10)	return kINPUTMUX_GpioPort1Pin10ToPintsel;
	if (me->ioPin->port == 1 && me->ioPin->pin == 11)	return kINPUTMUX_GpioPort1Pin11ToPintsel;
	if (me->ioPin->port == 1 && me->ioPin->pin == 12)	return kINPUTMUX_GpioPort1Pin12ToPintsel;
	if (me->ioPin->port == 1 && me->ioPin->pin == 13)	return kINPUTMUX_GpioPort1Pin13ToPintsel;
	if (me->ioPin->port == 1 && me->ioPin->pin == 14)	return kINPUTMUX_GpioPort1Pin14ToPintsel;
	if (me->ioPin->port == 1 && me->ioPin->pin == 15)	return kINPUTMUX_GpioPort1Pin15ToPintsel;
	if (me->ioPin->port == 1 && me->ioPin->pin == 16)	return kINPUTMUX_GpioPort1Pin16ToPintsel;
	if (me->ioPin->port == 1 && me->ioPin->pin == 17)	return kINPUTMUX_GpioPort1Pin17ToPintsel;
	if (me->ioPin->port == 1 && me->ioPin->pin == 18)	return kINPUTMUX_GpioPort1Pin18ToPintsel;
	if (me->ioPin->port == 1 && me->ioPin->pin == 19)	return kINPUTMUX_GpioPort1Pin19ToPintsel;
	if (me->ioPin->port == 1 && me->ioPin->pin == 20)	return kINPUTMUX_GpioPort1Pin20ToPintsel;
	if (me->ioPin->port == 1 && me->ioPin->pin == 21)	return kINPUTMUX_GpioPort1Pin21ToPintsel;
	if (me->ioPin->port == 1 && me->ioPin->pin == 22)	return kINPUTMUX_GpioPort1Pin22ToPintsel;
	if (me->ioPin->port == 1 && me->ioPin->pin == 23)	return kINPUTMUX_GpioPort1Pin23ToPintsel;
	if (me->ioPin->port == 1 && me->ioPin->pin == 24)	return kINPUTMUX_GpioPort1Pin24ToPintsel;
	if (me->ioPin->port == 1 && me->ioPin->pin == 25)	return kINPUTMUX_GpioPort1Pin25ToPintsel;
	if (me->ioPin->port == 1 && me->ioPin->pin == 26)	return kINPUTMUX_GpioPort1Pin26ToPintsel;
	if (me->ioPin->port == 1 && me->ioPin->pin == 27)	return kINPUTMUX_GpioPort1Pin27ToPintsel;
	if (me->ioPin->port == 1 && me->ioPin->pin == 28)	return kINPUTMUX_GpioPort1Pin28ToPintsel;
	if (me->ioPin->port == 1 && me->ioPin->pin == 29)	return kINPUTMUX_GpioPort1Pin29ToPintsel;
	if (me->ioPin->port == 1 && me->ioPin->pin == 30)	return kINPUTMUX_GpioPort1Pin30ToPintsel;
	if (me->ioPin->port == 1 && me->ioPin->pin == 31)	return kINPUTMUX_GpioPort1Pin31ToPintsel;

	ALX_IO_PIN_IRQ_ASSERT(false); // We shouldn't get here
	return ALX_NULL;
}
static IRQn_Type AlxIoPin_GetIrqType(AlxIoPinIrq* me)
{
	if (me->irqPin == kPINT_PinInt0)	return PIN_INT0_IRQn;
	if (me->irqPin == kPINT_PinInt1)	return PIN_INT1_IRQn;
	if (me->irqPin == kPINT_PinInt2)	return PIN_INT2_IRQn;
	if (me->irqPin == kPINT_PinInt3)	return PIN_INT3_IRQn;
	if (me->irqPin == kPINT_PinInt4)	return PIN_INT4_IRQn;
	if (me->irqPin == kPINT_PinInt5)	return PIN_INT5_IRQn;
	if (me->irqPin == kPINT_PinInt6)	return PIN_INT6_IRQn;
	if (me->irqPin == kPINT_PinInt7)	return PIN_INT7_IRQn;

	ALX_IO_PIN_IRQ_ASSERT(false); // We shouldn't get here
	return ALX_NULL;
}


//******************************************************************************
// Weak Functions
//******************************************************************************
ALX_WEAK void AlxIoPinIrq_Foreground_Callback_Pin0()
{
	ALX_IO_PIN_IRQ_TRACE("Define AlxIoPinIrq_Foreground_Callback_Pin0");
	ALX_IO_PIN_IRQ_ASSERT(false);
}
ALX_WEAK void AlxIoPinIrq_Foreground_Callback_Pin1()
{
	ALX_TRACE_FORMAT("Define AlxIoPinIrq_Foreground_Callback_Pin1");
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


//******************************************************************************
// IRQ Handlers
//******************************************************************************
void PIN_INT0_IRQHandler(void)
{
	AlxIoPinIrq_Foreground_Callback_Pin0();
	PINT_PinInterruptClrStatus(PINT, kPINT_PinInt0);	// MF: Clear IRQ Flag
}
void PIN_INT1_IRQHandler(void)
{
	AlxIoPinIrq_Foreground_Callback_Pin1();
	PINT_PinInterruptClrStatus(PINT, kPINT_PinInt1);	// MF: Clear IRQ Flag
}
void PIN_INT2_IRQHandler(void)
{
	AlxIoPinIrq_Foreground_Callback_Pin2();
	PINT_PinInterruptClrStatus(PINT, kPINT_PinInt2);	// MF: Clear IRQ Flag
}
void PIN_INT3_IRQHandler(void)
{
	AlxIoPinIrq_Foreground_Callback_Pin3();
	PINT_PinInterruptClrStatus(PINT, kPINT_PinInt3);	// MF: Clear IRQ Flag
}
void PIN_INT4_IRQHandler(void)
{
	AlxIoPinIrq_Foreground_Callback_Pin4();
	PINT_PinInterruptClrStatus(PINT, kPINT_PinInt4);	// MF: Clear IRQ Flag
}
void PIN_INT5_IRQHandler(void)
{
	AlxIoPinIrq_Foreground_Callback_Pin5();
	PINT_PinInterruptClrStatus(PINT, kPINT_PinInt5);	// MF: Clear IRQ Flag
}
void PIN_INT6_IRQHandler(void)
{
	AlxIoPinIrq_Foreground_Callback_Pin6();
	PINT_PinInterruptClrStatus(PINT, kPINT_PinInt6);	// MF: Clear IRQ Flag
}
void PIN_INT7_IRQHandler(void)
{
	AlxIoPinIrq_Foreground_Callback_Pin7();
	PINT_PinInterruptClrStatus(PINT, kPINT_PinInt7);	// MF: Clear IRQ Flag
}


#endif // Module Guard
