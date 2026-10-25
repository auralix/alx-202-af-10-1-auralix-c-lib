/**
  ******************************************************************************
  * @file		alxBootFake.c
  * @brief		Auralix C Library - ALX Boot Module - PC Unit Test Fake
  * @copyright	Copyright (C) Auralix d.o.o. All rights reserved.
  *
  * A link-time fake of the bootloader module, named after the module it fakes,
  * for an application that constructs the bootloader and polls it for an
  * update over USB. On a PC there is no USB drive and no second image, so an
  * update never starts: the constructor keeps nothing and the poll returns.
  ******************************************************************************
  **/

//******************************************************************************
// Includes
//******************************************************************************
#include "alxBoot.h"


//******************************************************************************
// The faked module's own contract
//******************************************************************************
void AlxBoot_Ctor(AlxBoot* me, AlxFs* alxFs, AlxId* alxId, AlxUsb* alxUsb, uint16_t usbConnectedTimeout_ms, uint16_t usbReadyTimeout_ms)
{
	(void)me; (void)alxFs; (void)alxId; (void)alxUsb; (void)usbConnectedTimeout_ms; (void)usbReadyTimeout_ms;
}

void AlxBoot_App_Usb_Update(AlxBoot* me)
{
	(void)me;
}
