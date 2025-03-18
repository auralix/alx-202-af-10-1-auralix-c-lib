/**
  ******************************************************************************
  * @file		alxSerialPort_McuSam.h
  * @brief		Auralix C Library - ALX Serial Port MCU SAM Module
  * @copyright	Copyright (C) Auralix d.o.o. All rights reserved.
  *
  * @section License
  *
  * SPDX-License-Identifier: GPL-3.0-or-later
  *
  * This file is part of Auralix C Library.
  *
  * Auralix C Library is free software: you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation, either version 3
  * of the License, or (at your option) any later version.
  *
  * Auralix C Library is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  * GNU General Public License for more details.
  *
  * You should have received a copy of the GNU General Public License
  * along with Auralix C Library. If not, see <https://www.gnu.org/licenses/>.
  ******************************************************************************
  **/

//******************************************************************************
// Include Guard
//******************************************************************************
#ifndef ALX_SERIAL_PORT_MCU_SAM_H
#define ALX_SERIAL_PORT_MCU_SAM_H

#ifdef __cplusplus
extern "C" {
#endif


//******************************************************************************
// Includes
//******************************************************************************
#include "alxGlobal.h"
#include "alxTrace.h"
#include "alxAssert.h"
#include "alxFifo.h"
#include "alxIoPin.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB) && defined(ALX_SAM)


//******************************************************************************
// Types
//******************************************************************************
typedef enum
{
	AlxSerialPort_Lin_Disable,
	AlxSerialPort_Lin_EnableMaster,
	AlxSerialPort_Lin_EnableSlave
} AlxSerialPort_Lin;

typedef struct
{
	// Parameters
	void* hw;
	AlxIoPin* do_TX;
	AlxIoPin* di_RX;
	uint8_t* rxFifoBuff;
	uint32_t rxFifoBuffLen;
	Alx_IrqPriority rxIrqPriority;
	AlxSerialPort_Lin lin;

	// Variables
	struct usart_sync_descriptor descr;
	AlxFifo rxFifo;

	// Info
	bool wasCtorCalled;
	bool isInit;
} AlxSerialPort;


//******************************************************************************
// Constructor
//******************************************************************************
void AlxSerialPort_Ctor
(
	AlxSerialPort* me,
	void* hw,
	AlxIoPin* do_TX,
	AlxIoPin* di_RX,
	uint8_t* rxFifoBuff,
	uint32_t rxFifoBuffLen,
	Alx_IrqPriority rxIrqPriority,
	AlxSerialPort_Lin lin
);


#endif	// #if defined(ALX_C_LIB) && defined(ALX_SAM)

#ifdef __cplusplus
}
#endif

#endif	// #ifndef ALX_SERIAL_PORT_MCU_SAM_H
