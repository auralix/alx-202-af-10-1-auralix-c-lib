/**
  ******************************************************************************
  * @file		alxUsb.c
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
// Includes
//*******************************************************************************
#include "alxUsb.h"


//*******************************************************************************
// Module Guard
//*******************************************************************************
#if defined(ALX_C_LIB)


//*******************************************************************************
// Private Variables
//*******************************************************************************
static AlxUsb* alxUsb_me = NULL;


//*******************************************************************************
// Private Functions
//*******************************************************************************
Alx_Status AlxUsb_Init_Private(AlxUsb* me);
Alx_Status AlxUsb_DeInit_Private(AlxUsb* me);
static void AlxUsb_Event_Callback(USBH_HandleTypeDef* phost, uint8_t id);


//*******************************************************************************
// Constructor
//*******************************************************************************
void AlxUsb_Ctor
(
	AlxUsb*	me
)
{
	// Private Variables
	alxUsb_me = me;

	// Variables
	memset(&me->usbh, 0, sizeof(me->usbh));
	me->usbh_event = 0;
	me->usbhMsc_isReady = false;
	me->isReady = false;

	// Info
	me->wasCtorCalled = true;
	me->isInit = false;
}


//*******************************************************************************
// Functions
//*******************************************************************************

/**
  * @brief
  * @param[in,out]	me
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxUsb_Init(AlxUsb* me)
{
	// Assert
	ALX_USB_ASSERT(me->wasCtorCalled == true);
	ALX_USB_ASSERT(me->isInit == false);

	// Return
	return AlxUsb_Init_Private(me);
}

/**
  * @brief
  * @param[in,out]	me
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxUsb_DeInit(AlxUsb* me)
{
	// Assert
	ALX_USB_ASSERT(me->wasCtorCalled == true);
	ALX_USB_ASSERT(me->isInit == true);

	// Return
	return AlxUsb_DeInit_Private(me);
}

/**
  * @brief
  * @param[in,out]	me
  */
Alx_Status AlxUsb_Handle(AlxUsb* me)
{
	//------------------------------------------------------------------------------
	// Assert
	//------------------------------------------------------------------------------
	ALX_USB_ASSERT(me->wasCtorCalled == true);
	ALX_USB_ASSERT(me->isInit == true);


	//------------------------------------------------------------------------------
	// Handle
	//------------------------------------------------------------------------------

	// Process
	USBH_Process(&me->usbh);	// Always returns OK
	me->usbhMsc_isReady = USBH_MSC_IsReady(&me->usbh);

	// Handle
	if (me->usbh_event == HOST_USER_UNRECOVERED_ERROR)
	{
		// Trace
		ALX_USB_TRACE_ERR("FAIL: AlxUsb_Handle() HOST_USER_UNRECOVERED_ERROR");

		// Local variables
		Alx_Status status = Alx_Err;

		// DeInit
		status = AlxUsb_DeInit_Private(me);
		if (status != Alx_Ok)
		{
			ALX_USB_TRACE_ERR("FAIL: AlxUsb_DeInit_Private() status %ld", status);
			return Alx_Err;
		}

		// Init
		status = AlxUsb_Init_Private(me);
		if (status != Alx_Ok)
		{
			ALX_USB_TRACE_ERR("FAIL: AlxUsb_Init_Private() status %ld", status);
			return Alx_Err;
		}
	}
	else if (me->usbh_event == HOST_USER_CLASS_ACTIVE && me->usbhMsc_isReady)
	{
		me->isReady = true;
	}
	else
	{
		me->isReady = false;
	}


	//------------------------------------------------------------------------------
	// Return
	//------------------------------------------------------------------------------
	return Alx_Ok;
}

/**
  * @brief
  * @param[in,out]	me
  * @retval			true
  * @retval			false
  */
bool AlxUsb_IsReady(AlxUsb* me)
{
	// Assert
	ALX_USB_ASSERT(me->wasCtorCalled == true);
	ALX_USB_ASSERT(me->isInit == true);

	// Return
	return me->isReady;
}

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		addr
  * @param[in]		data
  * @param[out]		len
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxUsb_Read(AlxUsb* me, uint32_t addr, uint8_t* data, uint32_t len)
{
	// Assert
	ALX_USB_ASSERT(me->wasCtorCalled == true);
	ALX_USB_ASSERT(me->isInit == true);
	ALX_USB_ASSERT(0 < len);

	// Check
	if (!me->isReady)
	{
		ALX_USB_TRACE_ERR("FAIL: isReady() addr %lu len %lu", addr, len);
		return Alx_Err;
	}

	// Read
	USBH_StatusTypeDef status = USBH_MSC_Read(&me->usbh, 0, addr, data, len);
	if (status != USBH_OK)
	{
		ALX_USB_TRACE_ERR("FAIL: USBH_MSC_Read() status %ld addr %lu len %lu", status, addr, len);
		return Alx_Err;
	}

	// Return
	return Alx_Ok;
}

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		addr
  * @param[in]		data
  * @param[in]		len
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxUsb_Write(AlxUsb* me, uint32_t addr, uint8_t* data, uint32_t len)
{
	// Assert
	ALX_USB_ASSERT(me->wasCtorCalled == true);
	ALX_USB_ASSERT(me->isInit == true);
	ALX_USB_ASSERT(0 < len);

	// Check
	if (!me->isReady)
	{
		ALX_USB_TRACE_ERR("FAIL: isReady() addr %lu len %lu", addr, len);
		return Alx_Err;
	}

	// Read
	USBH_StatusTypeDef status = USBH_MSC_Write(&me->usbh, 0, addr, data, len);
	if (status != USBH_OK)
	{
		ALX_USB_TRACE_ERR("FAIL: USBH_MSC_Write() status %ld addr %lu len %lu", status, addr, len);
		return Alx_Err;
	}

	// Return
	return Alx_Ok;
}


//*******************************************************************************
// Private Functions
//*******************************************************************************
Alx_Status AlxUsb_Init_Private(AlxUsb* me)
{
	// Local variables
	USBH_StatusTypeDef status = USBH_FAIL;

	// Init
	status = USBH_Init(&me->usbh, AlxUsb_Event_Callback, 0);
	if (status != USBH_OK)
	{
		ALX_USB_TRACE_ERR("FAIL: USBH_Init() status %ld", status);
		return Alx_Err;
	}

	// Register class
	status = USBH_RegisterClass(&me->usbh, USBH_MSC_CLASS);
	if (status != USBH_OK)
	{
		ALX_USB_TRACE_ERR("FAIL: USBH_RegisterClass() status %ld", status);
		return Alx_Err;
	}

	// Start
	status = USBH_Start(&me->usbh);
	if (status != USBH_OK)
	{
		ALX_USB_TRACE_ERR("FAIL: USBH_Start() status %ld", status);
		return Alx_Err;
	}

	// Set isInit
	me->isInit = true;

	// Return
	return Alx_Ok;
}
Alx_Status AlxUsb_DeInit_Private(AlxUsb* me)
{
	// Local variables
	USBH_StatusTypeDef status = USBH_FAIL;

	// Stop
	status = USBH_Stop(&me->usbh);
	if (status != USBH_OK)
	{
		ALX_USB_TRACE_ERR("FAIL: USBH_Stop() status %ld", status);
		return Alx_Err;
	}

	// DeInit
	status = USBH_DeInit(&me->usbh);
	if (status != USBH_OK)
	{
		ALX_USB_TRACE_ERR("FAIL: USBH_DeInit() status %ld", status);
		return Alx_Err;
	}

	// Clear
	memset(&me->usbh, 0, sizeof(me->usbh));
	me->usbh_event = 0;
	me->usbhMsc_isReady = false;
	me->isReady = false;

	// Clear isInit
	me->isInit = false;

	// Return
	return Alx_Ok;
}
static void AlxUsb_Event_Callback(USBH_HandleTypeDef* phost, uint8_t id)
{
	// Void
	(void)phost;

	// Set
	alxUsb_me->usbh_event = id;

	// Trace
	ALX_USB_TRACE_INF("AlxUsb_Event_Callback(%lu)", id);
}


#endif	// #if defined(ALX_C_LIB)
