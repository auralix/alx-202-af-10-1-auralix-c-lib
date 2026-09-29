/**
  ******************************************************************************
  * @file		alxIoPinIrqFake.c
  * @brief		Auralix C Library - ALX IO Pin IRQ Module - PC Unit Test Fake
  * @copyright	Copyright (C) Auralix d.o.o. All rights reserved.
  *
  * A link-time fake of the pin interrupt module, named after the module it
  * fakes, for a consumer that constructs pin interrupts but does not raise
  * them on the host. It satisfies the linker for the constructor and nothing
  * else. The constructor exists on an MCU family only and is defined under
  * the module header's own guard.
  ******************************************************************************
  **/

//******************************************************************************
// Includes
//******************************************************************************
#include "alxIoPinIrq.h"


//******************************************************************************
// The faked module's own contract
//******************************************************************************
#if defined(ALX_STM32F4) || defined(ALX_STM32F7) || defined(ALX_STM32G4) || defined(ALX_STM32L0) || defined(ALX_STM32L4) || defined(ALX_STM32U5)
void AlxIoPinIrq_Ctor(AlxIoPinIrq* me, AlxIoPin** ioPinArr, uint8_t numOfIoPins, Alx_IrqPriority* irqPriorityArr)
{
	(void)me; (void)ioPinArr; (void)numOfIoPins; (void)irqPriorityArr;
}
#endif
