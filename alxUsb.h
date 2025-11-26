/**
  ******************************************************************************
  * @file        alxUsb.h
  * @brief       Auralix C Library - ALX USB Host MSC Module
  * @copyright   Copyright (C) Auralix d.o.o. All rights reserved.
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

#include "usbh_core.h"
#include "usbh_msc.h"

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
	#define ALX_USB_ASSERT(expr)	ALX_ASSERT_BKPT(ALX_USB_FILE, expr)
#elif defined(ALX_USB_ASSERT_TRACE_ENABLE)
	#define ALX_USB_ASSERT(expr)	ALX_ASSERT_TRACE(ALX_USB_FILE, expr)
#elif defined(ALX_USB_ASSERT_RST_ENABLE)
	#define ALX_USB_ASSERT(expr)	ALX_ASSERT_RST(ALX_USB_FILE, expr)
#else
	#define ALX_USB_ASSERT(expr)	do {} while (false)
#endif

// Trace //
#if defined(ALX_USB_TRACE_WRN_ENABLE)
	#define ALX_USB_TRACE_WRN(...)	ALX_TRACE_WRN(ALX_USB_FILE, __VA_ARGS__)
#else
	#define ALX_USB_TRACE_WRN(...)	do {} while (false)
#endif

//*******************************************************************************
// Types
//*******************************************************************************

/**
 * @brief High-level USB MSC host state
 *
 * This is internal to the module; external users normally only call
 * AlxUsb_IsReady().
 */
typedef enum
{
	AlxUsb_State_NoDevice = 0,
	AlxUsb_State_Connected,
	AlxUsb_State_Enumerating,
	AlxUsb_State_Ready,
	AlxUsb_State_Disconnected,
	AlxUsb_State_Error,
} AlxUsb_State;

/**
 * @brief ALX USB MSC context
 */
typedef struct
{
	// Parameters
	uint8_t				hostId;			///< USB host ID passed to USBH_Init (e.g. HOST_FS)

	// Low-level USB Host handle (owned by this module)
	USBH_HandleTypeDef	host;

	// Internal state
	volatile uint8_t	pendingEvent;	///< Last HOST_USER_* event (0 = none)
	AlxUsb_State		state;			///< Current high-level state

	// Info
	bool				wasCtorCalled;
	bool				isInit;
} AlxUsb;


//*******************************************************************************
// Constructor
//*******************************************************************************

/**
  * @brief		Constructor
  * @param[in]	me		Context
  * @param[in]	hostId	Host ID passed to USBH_Init (e.g. HOST_FS)
  */
void AlxUsb_Ctor
(
	AlxUsb*	me,
	uint8_t	hostId
);


//*******************************************************************************
// Functions
//*******************************************************************************

/**
  * @brief	Initialize USB Host + MSC class and start host.
  * @param	me		Context
  * @retval	Alx_Ok
  * @retval	Alx_Err
  */
Alx_Status AlxUsb_Init(AlxUsb* me);

/**
  * @brief	DeInitialize USB Host.
  * @param	me		Context
  * @retval	Alx_Ok
  * @retval	Alx_Err
  */
Alx_Status AlxUsb_DeInit(AlxUsb* me);

/**
  * @brief	Drive USB Host state machine (polling).
  *			Must be called regularly (e.g. in main loop).
  * @param	me		Context
  */
void AlxUsb_Handle(AlxUsb* me);

/**
  * @brief	Check if MSC device is ready for block I/O.
  * @param	me		Context
  * @retval	true	Device is ready (enumerated + MSC ready)
  * @retval	false	Not ready / no device / error
  */
bool AlxUsb_IsReady(AlxUsb* me);

/**
  * @brief	Read sectors from USB MSC device.
  * @param	me		Context
  * @param	sector	Start sector (LBA)
  * @param	count	Number of sectors to read
  * @param	buff	Destination buffer
  * @retval	Alx_Ok
  * @retval	Alx_Err
  */
Alx_Status AlxUsb_Read
(
	AlxUsb*		me,
	uint32_t	sector,
	uint32_t	count,
	uint8_t*	buff
);

/**
  * @brief	Write sectors to USB MSC device.
  * @param	me		Context
  * @param	sector	Start sector (LBA)
  * @param	count	Number of sectors to write
  * @param	buff	Source buffer
  * @retval	Alx_Ok
  * @retval	Alx_Err
  */
Alx_Status AlxUsb_Write
(
	AlxUsb*			me,
	uint32_t		sector,
	uint32_t		count,
	const uint8_t*	buff
);


#endif	// #if defined(ALX_C_LIB)

#ifdef __cplusplus
}
#endif

#endif	// #ifndef ALX_USB_H
