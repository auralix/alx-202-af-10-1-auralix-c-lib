/**
  ******************************************************************************
  * @file		alxUsb.h
  * @brief		Auralix C Library - ALX USB Module
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

//*******************************************************************************
// Include Guard
//*******************************************************************************
#ifndef ALX_USB_H
#define ALX_USB_H

#ifdef __cplusplus
extern "C" {
#endif


//*******************************************************************************
// Includes
//*******************************************************************************
#include "alxGlobal.h"
#include "alxTrace.h"
#include "alxAssert.h"


//*******************************************************************************
// Module Guard
//*******************************************************************************
#if defined(ALX_C_LIB)


//*******************************************************************************
// Preprocessor
//*******************************************************************************
#define ALX_USB_FILE "alxUsb.h"

// Assert //
#if defined(ALX_USB_ASSERT_BKPT_ENABLE)
	#define ALX_USB_ASSERT(expr) ALX_ASSERT_BKPT(ALX_USB_FILE, expr)
#elif defined(ALX_USB_ASSERT_TRACE_ENABLE)
	#define ALX_USB_ASSERT(expr) ALX_ASSERT_TRACE(ALX_USB_FILE, expr)
#elif defined(ALX_USB_ASSERT_RST_ENABLE)
	#define ALX_USB_ASSERT(expr) ALX_ASSERT_RST(ALX_USB_FILE, expr)
#else
	#define ALX_USB_ASSERT(expr) do{} while (false)
#endif

// Trace //
#if defined(ALX_USB_TRACE_ENABLE)
	#define ALX_USB_TRACE_ERR(...) ALX_TRACE_ERR(ALX_USB_FILE, __VA_ARGS__)
	#define ALX_USB_TRACE_WRN(...) ALX_TRACE_WRN(ALX_USB_FILE, __VA_ARGS__)
	#define ALX_USB_TRACE_INF(...) ALX_TRACE_INF(ALX_USB_FILE, __VA_ARGS__)
#else
	#define ALX_USB_TRACE_ERR(...) do {} while (false)
	#define ALX_USB_TRACE_WRN(...) do {} while (false)
	#define ALX_USB_TRACE_INF(...) do {} while (false)
#endif


//*******************************************************************************
// Types
//*******************************************************************************
typedef struct
{
	// Variables
	USBH_HandleTypeDef usbh;
	uint8_t	usbh_event;
	bool usbhMsc_isReady;
	bool isReady;

	// Info
	bool wasCtorCalled;
	bool isInit;
} AlxUsb;


//*******************************************************************************
// Constructor
//*******************************************************************************
void AlxUsb_Ctor
(
	AlxUsb*	me
);


//*******************************************************************************
// Functions
//*******************************************************************************
Alx_Status AlxUsb_Init(AlxUsb* me);
Alx_Status AlxUsb_DeInit(AlxUsb* me);
Alx_Status AlxUsb_Handle(AlxUsb* me);
bool AlxUsb_IsReady(AlxUsb* me);
Alx_Status AlxUsb_Read(AlxUsb* me, uint32_t addr, uint8_t* data, uint32_t len);
Alx_Status AlxUsb_Write(AlxUsb* me, uint32_t addr, uint8_t* data, uint32_t len);


#endif	// #if defined(ALX_C_LIB)

#ifdef __cplusplus
}
#endif

#endif	// #ifndef ALX_USB_H
