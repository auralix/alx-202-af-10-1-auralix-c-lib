/**
  ******************************************************************************
  * @file		alxUsbFake.c
  * @brief		Auralix C Library - ALX USB Module - PC Unit Test Fake
  * @copyright	Copyright (C) Auralix d.o.o. All rights reserved.
  *
  * A link-time fake of the USB host, named after the module it fakes. On a PC
  * no drive is ever attached, so the fake constructs nothing and its interrupt
  * handler has nothing to handle. The constructor exists on an MCU family only
  * and is defined under the module header's own guard.
  ******************************************************************************
  **/

//******************************************************************************
// Includes
//******************************************************************************
#include "alxUsb.h"


//******************************************************************************
// The faked module's own contract
//******************************************************************************
#if defined(ALX_STM32F7)
void AlxUsb_Ctor(AlxUsb* me, HCD_TypeDef* usb, AlxIoPin* io_USB_D_P, AlxIoPin* io_USB_D_N, Alx_IrqPriority irqPriority)
{
	(void)me; (void)usb; (void)io_USB_D_P; (void)io_USB_D_N; (void)irqPriority;
}
#endif

void AlxUsb_Irq_Handle(AlxUsb* me)
{
	(void)me;
}
