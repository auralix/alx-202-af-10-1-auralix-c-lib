/**
  ******************************************************************************
  * @file alxIoPinIrq_McuLpc80x.c
  * @brief Auralix C Library - ALX IO Pin Module
  * @version $LastChangedRevision: 4270 $
  * @date $LastChangedDate: 2021-03-05 19:02:52 +0100 (Fri, 05 Mar 2021) $
  ******************************************************************************
  */

//******************************************************************************
// Includes
//******************************************************************************
#include "alxIoPinIrq_McuLpc80x.h"
#include "alxIoPinIrq.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_LPC80X)


//******************************************************************************
// Private Functions
//******************************************************************************
static syscon_connection_t AlxIoPin_GetIrqPortPinSel(AlxIoPinIrq* me);
static IRQn_Type AlxIoPin_GetIrqType(AlxIoPinIrq* me);
static void AlxIoPin_PINT_Init(AlxIoPinIrq* me, PINT_Type* base);	// MF: I had to rewrite FSL func without "Periph_Reset" in it so we can init more pins


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
	ALX_IO_PIN_IRQ_ASSERT(me->isInit == false);
	ALX_IO_PIN_IRQ_ASSERT(me->wasCtorCalled == true);

	// #1 Set isInit attribute
	me->isInit = true;

	// #2 Init IoPin
	AlxIoPin_Init(me->ioPin);

	// #3 Prepare IRQ Pin
	syscon_connection_t irqPortPinSel = AlxIoPin_GetIrqPortPinSel(me);
	IRQn_Type irqType = AlxIoPin_GetIrqType(me);

	// #4 Attach IRQ to right Pin
	SYSCON_AttachSignal(SYSCON, me->irqPin, irqPortPinSel);

	// #5 Init PINT Periphery
	AlxIoPin_PINT_Init(me, PINT); // "EnableClk" happens here.

	// #6 Enable IRQ
	PINT_PinInterruptConfig(PINT, me->irqPin, me->irqType, ALX_NULL_PTR);	// MF: "ALX_NULL_PTR" because we'll use "PIN_INTX_IRQHandler" from startup.s
	NVIC_SetPriority(irqType, (uint32_t)me->irqPriority);					// MF: Set IRQ Priority
	PINT_EnableCallbackByIndex(PINT, me->irqPin);							// MF: Enable IRQ
}
void AlxIoPinIrq_DeInit(AlxIoPinIrq* me)
{
	ALX_IO_PIN_IRQ_ASSERT(me->isInit == true);
	ALX_IO_PIN_IRQ_ASSERT(me->wasCtorCalled == true);

	// #1 DeInit if IRQ
	PINT_DisableCallbackByIndex(PINT, me->irqPin);	// MF: Disable IRQ

	// #2 Clear isInit attribute
	me->isInit = false;
}


//******************************************************************************
// Private Functions
//******************************************************************************
static syscon_connection_t AlxIoPin_GetIrqPortPinSel(AlxIoPinIrq* me)
{
	if (me->ioPin->port == 0 && me->ioPin->pin == 0)	return kSYSCON_GpioPort0Pin0ToPintsel;
	if (me->ioPin->port == 0 && me->ioPin->pin == 1)	return kSYSCON_GpioPort0Pin1ToPintsel;
	if (me->ioPin->port == 0 && me->ioPin->pin == 2)	return kSYSCON_GpioPort0Pin2ToPintsel;
	if (me->ioPin->port == 0 && me->ioPin->pin == 3)	return kSYSCON_GpioPort0Pin3ToPintsel;
	if (me->ioPin->port == 0 && me->ioPin->pin == 4)	return kSYSCON_GpioPort0Pin4ToPintsel;
	if (me->ioPin->port == 0 && me->ioPin->pin == 5)	return kSYSCON_GpioPort0Pin5ToPintsel;

	if (me->ioPin->port == 0 && me->ioPin->pin == 7)	return kSYSCON_GpioPort0Pin7ToPintsel;
	if (me->ioPin->port == 0 && me->ioPin->pin == 8)	return kSYSCON_GpioPort0Pin8ToPintsel;
	if (me->ioPin->port == 0 && me->ioPin->pin == 9)	return kSYSCON_GpioPort0Pin9ToPintsel;
	if (me->ioPin->port == 0 && me->ioPin->pin == 10)	return kSYSCON_GpioPort0Pin10ToPintsel;
	if (me->ioPin->port == 0 && me->ioPin->pin == 11)	return kSYSCON_GpioPort0Pin11ToPintsel;
	if (me->ioPin->port == 0 && me->ioPin->pin == 12)	return kSYSCON_GpioPort0Pin12ToPintsel;
	if (me->ioPin->port == 0 && me->ioPin->pin == 13)	return kSYSCON_GpioPort0Pin13ToPintsel;
	if (me->ioPin->port == 0 && me->ioPin->pin == 14)	return kSYSCON_GpioPort0Pin14ToPintsel;
	if (me->ioPin->port == 0 && me->ioPin->pin == 15)	return kSYSCON_GpioPort0Pin15ToPintsel;
	if (me->ioPin->port == 0 && me->ioPin->pin == 16)	return kSYSCON_GpioPort0Pin16ToPintsel;
	if (me->ioPin->port == 0 && me->ioPin->pin == 17)	return kSYSCON_GpioPort0Pin17ToPintsel;
	if (me->ioPin->port == 0 && me->ioPin->pin == 18)	return kSYSCON_GpioPort0Pin18ToPintsel;
	if (me->ioPin->port == 0 && me->ioPin->pin == 19)	return kSYSCON_GpioPort0Pin19ToPintsel;
	if (me->ioPin->port == 0 && me->ioPin->pin == 20)	return kSYSCON_GpioPort0Pin20ToPintsel;
	if (me->ioPin->port == 0 && me->ioPin->pin == 21)	return kSYSCON_GpioPort0Pin21ToPintsel;
	if (me->ioPin->port == 0 && me->ioPin->pin == 22)	return kSYSCON_GpioPort0Pin22ToPintsel;
	if (me->ioPin->port == 0 && me->ioPin->pin == 23)	return kSYSCON_GpioPort0Pin23ToPintsel;
	if (me->ioPin->port == 0 && me->ioPin->pin == 24)	return kSYSCON_GpioPort0Pin24ToPintsel;
	if (me->ioPin->port == 0 && me->ioPin->pin == 25)	return kSYSCON_GpioPort0Pin25ToPintsel;
	if (me->ioPin->port == 0 && me->ioPin->pin == 26)	return kSYSCON_GpioPort0Pin26ToPintsel;
	if (me->ioPin->port == 0 && me->ioPin->pin == 27)	return kSYSCON_GpioPort0Pin27ToPintsel;
	if (me->ioPin->port == 0 && me->ioPin->pin == 28)	return kSYSCON_GpioPort0Pin28ToPintsel;
	if (me->ioPin->port == 0 && me->ioPin->pin == 29)	return kSYSCON_GpioPort0Pin29ToPintsel;
	if (me->ioPin->port == 0 && me->ioPin->pin == 30)	return kSYSCON_GpioPort0Pin30ToPintsel;

	ALX_IO_PIN_ASSERT(false); // We shouldn't get here
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

	ALX_IO_PIN_ASSERT(false); // We shouldn't get here
	return ALX_NULL;
}
static void AlxIoPin_PINT_Init(AlxIoPinIrq* me, PINT_Type* base)
{
	ALX_IO_PIN_ASSERT(base != NULL);

	// #1 Prepare Parameters
	uint32_t pmcfg = 0;
	uint8_t pintcount = 0;

	// #2 Set pintcount
	pintcount = FSL_FEATURE_PINT_NUMBER_OF_CONNECTED_OUTPUTS;

	// #3 Disable all bit slices for pint
	for (uint32_t i = 0; i < pintcount; i++)
	{
		pmcfg = pmcfg | ((uint32_t)kPINT_PatternMatchNever << (PININT_BITSLICE_CFG_START + (i * 3U)));
	}

	// #4 Enable Irq Clk
	CLOCK_EnableClock(kCLOCK_GpioInt);

	// #5 Disable all pattern match bit slices
	base->PMCFG = pmcfg;
}

//******************************************************************************
// Weak Functions
//******************************************************************************
ALX_WEAK void AlxIoPinIrq_Foreground_Callback_Pin0()
{
	ALX_IO_PIN_TRACE("Define AlxIoPinIrq_Foreground_Callback_Pin0");
	ALX_IO_PIN_ASSERT(false);
}
ALX_WEAK void AlxIoPinIrq_Foreground_Callback_Pin1()
{
	ALX_IO_PIN_TRACE("Define AlxIoPinIrq_Foreground_Callback_Pin1");
	ALX_IO_PIN_ASSERT(false);
}
ALX_WEAK void AlxIoPinIrq_Foreground_Callback_Pin2()
{
	ALX_IO_PIN_TRACE("Define AlxIoPinIrq_Foreground_Callback_Pin2");
	ALX_IO_PIN_ASSERT(false);
}
ALX_WEAK void AlxIoPinIrq_Foreground_Callback_Pin3()
{
	ALX_IO_PIN_TRACE("Define AlxIoPinIrq_Foreground_Callback_Pin3");
	ALX_IO_PIN_ASSERT(false);
}
ALX_WEAK void AlxIoPinIrq_Foreground_Callback_Pin4()
{
	ALX_IO_PIN_TRACE("Define AlxIoPinIrq_Foreground_Callback_Pin4");
	ALX_IO_PIN_ASSERT(false);
}
ALX_WEAK void AlxIoPinIrq_Foreground_Callback_Pin5()
{
	ALX_IO_PIN_TRACE("Define AlxIoPinIrq_Foreground_Callback_Pin5");
	ALX_IO_PIN_ASSERT(false);
}
ALX_WEAK void AlxIoPinIrq_Foreground_Callback_Pin6()
{
	ALX_IO_PIN_TRACE("Define AlxIoPinIrq_Foreground_Callback_Pin6");
	ALX_IO_PIN_ASSERT(false);
}
ALX_WEAK void AlxIoPinIrq_Foreground_Callback_Pin7()
{
	ALX_IO_PIN_TRACE("Define AlxIoPinIrq_Foreground_Callback_Pin7");
	ALX_IO_PIN_ASSERT(false);
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
