/**
  ******************************************************************************
  * @file		alxDacFake.c
  * @brief		Auralix C Library - ALX DAC Module - PC Unit Test Fake
  * @copyright	Copyright (C) Auralix d.o.o. All rights reserved.
  *
  * A link-time fake of the DAC, named after the module it fakes, for a
  * consumer that constructs a converter but does not drive it on the host. It
  * satisfies the linker for the constructor and nothing else; a consumer that
  * starts setting voltages gives this fake its model. The constructor exists
  * on an MCU family only and is defined under the module header's own guard.
  ******************************************************************************
  **/

//******************************************************************************
// Includes
//******************************************************************************
#include "alxDac.h"


//******************************************************************************
// The faked module's own contract
//******************************************************************************
#if defined(ALX_STM32F4) || defined(ALX_STM32F7) || defined(ALX_STM32G4) || defined(ALX_STM32L0) || defined(ALX_STM32L4)
void AlxDac_Ctor(AlxDac* me, DAC_TypeDef* dac, AlxIoPin** ioPinArr, Alx_Ch* chArr, float* setVoltageDefaultArr_V, uint8_t numOfCh, bool isVrefInt_V, float vrefExt_V)
{
	(void)me; (void)dac; (void)ioPinArr; (void)chArr; (void)setVoltageDefaultArr_V; (void)numOfCh;
	(void)isVrefInt_V; (void)vrefExt_V;
}
#endif
