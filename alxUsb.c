/**
  ******************************************************************************
  * @file        alxUsb.c
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
// Includes
//*******************************************************************************
#include "alxUsb.h"

//*******************************************************************************
// Module Guard
//*******************************************************************************
#if defined(ALX_C_LIB)

//*******************************************************************************
// Local function prototypes
//*******************************************************************************

static void AlxUsb_UserProcess(USBH_HandleTypeDef* phost, uint8_t id);
static void AlxUsb_StatusChange_Callback(AlxUsb* me, AlxUsb_State newState);

//*******************************************************************************
// Local variables
//*******************************************************************************

// NOTE: current implementation assumes a single USB host instance.
// If multiple hosts are needed, this can be extended.
static AlxUsb* s_alxUsbInstance = NULL;


//*******************************************************************************
// Constructor
//*******************************************************************************
void AlxUsb_Ctor
(
	AlxUsb*	me,
	uint8_t	hostId
)
{
	// Assert
	ALX_USB_ASSERT(me != NULL);

	// Parameters
	me->hostId = hostId;

	// Info
	me->pendingEvent  = 0U;
	me->state         = AlxUsb_State_NoDevice;
	me->wasCtorCalled = true;
	me->isInit        = false;
}


//*******************************************************************************
// Functions
//*******************************************************************************

/**
  * @brief
  * @param[in,out]	me	Context
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxUsb_Init(AlxUsb* me)
{
	// Assert
	ALX_USB_ASSERT(me != NULL);
	ALX_USB_ASSERT(me->wasCtorCalled == true);
	ALX_USB_ASSERT(me->isInit == false);

	// Store global instance pointer
	s_alxUsbInstance = me;

	// Clear internal state
	me->pendingEvent = 0U;
	me->state        = AlxUsb_State_NoDevice;

	// Init USB Host Core
	if (USBH_Init(&me->host, AlxUsb_UserProcess, me->hostId) != USBH_OK)
	{
		ALX_USB_TRACE_WRN("USBH_Init failed");
		return Alx_Err;
	}

	// Register MSC Class
	if (USBH_RegisterClass(&me->host, USBH_MSC_CLASS) != USBH_OK)
	{
		ALX_USB_TRACE_WRN("USBH_RegisterClass MSC failed");
		return Alx_Err;
	}

	// Start Host
	if (USBH_Start(&me->host) != USBH_OK)
	{
		ALX_USB_TRACE_WRN("USBH_Start failed");
		return Alx_Err;
	}

	// Info
	me->isInit = true;

	// Return
	return Alx_Ok;
}

/**
  * @brief
  * @param[in,out]	me	Context
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxUsb_DeInit(AlxUsb* me)
{
	// Assert
	ALX_USB_ASSERT(me != NULL);
	ALX_USB_ASSERT(me->wasCtorCalled == true);
	ALX_USB_ASSERT(me->isInit == true);

	// Stop + DeInit USB Host
	(void)USBH_Stop(&me->host);
	(void)USBH_DeInit(&me->host);

	// Clear instance
	if (s_alxUsbInstance == me)
	{
		s_alxUsbInstance = NULL;
	}

	me->isInit        = false;
	me->pendingEvent  = 0U;
	me->state         = AlxUsb_State_NoDevice;

	return Alx_Ok;
}

/**
  * @brief			Drive USB host state machine (polling).
  * @param[in,out]	me	Context
  */
void AlxUsb_Handle(AlxUsb* me)
{
	// Assert
	ALX_USB_ASSERT(me != NULL);
	ALX_USB_ASSERT(me->wasCtorCalled == true);
	ALX_USB_ASSERT(me->isInit == true);

	// Drive USB Host core
	USBH_Process(&me->host);

	// Consume event from callback
	uint8_t ev = me->pendingEvent;
	me->pendingEvent = 0U;

	if (ev != 0U)
	{
		AlxUsb_State oldState = me->state;

		switch (ev)
		{
			case HOST_USER_CONNECTION:
				me->state = AlxUsb_State_Connected;
				break;

			case HOST_USER_CLASS_SELECTED:
				me->state = AlxUsb_State_Enumerating;
				break;

			case HOST_USER_CLASS_ACTIVE:
				me->state = AlxUsb_State_Ready;
				break;

			case HOST_USER_DISCONNECTION:
				me->state = AlxUsb_State_Disconnected;
				break;

			case HOST_USER_UNRECOVERED_ERROR:
				me->state = AlxUsb_State_Error;
				// Optional: you could try auto-restart here
				// USBH_Stop(&me->host);
				// USBH_Start(&me->host);
				break;

			case HOST_USER_SELECT_CONFIGURATION:
			default:
				// Not used in this abstraction
				break;
		}

		if (me->state != oldState)
		{
			AlxUsb_StatusChange_Callback(me, me->state);
		}
	}

	// Extra safety: if we think we are ready but MSC is not, downgrade state
	if (me->state == AlxUsb_State_Ready)
	{
		if (USBH_MSC_IsReady(&me->host) != USBH_OK)
		{
			me->state = AlxUsb_State_Enumerating;
		}
	}
}

/**
  * @brief
  * @param[in,out]	me	Context
  * @retval			true	MSC device ready for block I/O
  * @retval			false	Not ready / no device / error
  */
bool AlxUsb_IsReady(AlxUsb* me)
{
	// Assert
	ALX_USB_ASSERT(me != NULL);
	ALX_USB_ASSERT(me->wasCtorCalled == true);
	ALX_USB_ASSERT(me->isInit == true);

	if (me->state != AlxUsb_State_Ready)
	{
		return false;
	}

	// Double-check with MSC class driver
	return (USBH_MSC_IsReady(&me->host) == USBH_OK);
}

/**
  * @brief
  * @param[in,out]	me		Context
  * @param[in]		sector	Start sector (LBA)
  * @param[in]		count	Number of sectors to read
  * @param[out]		buff	Destination buffer
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxUsb_Read
(
	AlxUsb*		me,
	uint32_t	sector,
	uint32_t	count,
	uint8_t*	buff
)
{
	// Assert
	ALX_USB_ASSERT(me != NULL);
	ALX_USB_ASSERT(me->wasCtorCalled == true);
	ALX_USB_ASSERT(me->isInit == true);
	ALX_USB_ASSERT(buff != NULL);
	ALX_USB_ASSERT(count > 0U);

	if (!AlxUsb_IsReady(me))
	{
		ALX_USB_TRACE_WRN("Read requested while not ready");
		return Alx_Err;
	}

	USBH_StatusTypeDef st = USBH_MSC_Read(&me->host, 0U, sector, buff, count);

	if (st == USBH_OK)
	{
		return Alx_Ok;
	}

	ALX_USB_TRACE_WRN("USBH_MSC_Read failed (%d)", (int)st);
	return Alx_Err;
}

/**
  * @brief
  * @param[in,out]	me		Context
  * @param[in]		sector	Start sector (LBA)
  * @param[in]		count	Number of sectors to write
  * @param[in]		buff	Source buffer
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxUsb_Write
(
	AlxUsb*			me,
	uint32_t		sector,
	uint32_t		count,
	const uint8_t*	buff
)
{
	// Assert
	ALX_USB_ASSERT(me != NULL);
	ALX_USB_ASSERT(me->wasCtorCalled == true);
	ALX_USB_ASSERT(me->isInit == true);
	ALX_USB_ASSERT(buff != NULL);
	ALX_USB_ASSERT(count > 0U);

	if (!AlxUsb_IsReady(me))
	{
		ALX_USB_TRACE_WRN("Write requested while not ready");
		return Alx_Err;
	}

	USBH_StatusTypeDef st =
		USBH_MSC_Write(&me->host, 0U, sector, (uint8_t*)buff, count);

	if (st == USBH_OK)
	{
		return Alx_Ok;
	}

	ALX_USB_TRACE_WRN("USBH_MSC_Write failed (%d)", (int)st);
	return Alx_Err;
}


//*******************************************************************************
// Local functions
//*******************************************************************************

/**
  * @brief	USB Host core user callback (bridge to AlxUsb)
  */
static void AlxUsb_UserProcess(USBH_HandleTypeDef* phost, uint8_t id)
{
	(void)phost;

	if (s_alxUsbInstance != NULL)
	{
		// Just record, actual handling is done in AlxUsb_Handle()
		s_alxUsbInstance->pendingEvent = id;
	}
}

/**
  * @brief	Status change hook, can be used for debug/LED/etc.
  * @note	Default implementation: do nothing.
  *			If you want board-specific behaviour, you can modify this
  *			function in this file as needed.
  */
static void AlxUsb_StatusChange_Callback(AlxUsb* me, AlxUsb_State newState)
{
	(void)me;
	(void)newState;
	// Example:
	// ALX_USB_TRACE_WRN("USB state = %d", (int)newState);
}


#endif	// #if defined(ALX_C_LIB)
