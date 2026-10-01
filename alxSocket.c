/**
  ******************************************************************************
  * @file		alxSocket.c
  * @brief		Auralix C Library - ALX Socket Module
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
// Includes
//******************************************************************************
#include "alxSocket.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB)


//******************************************************************************
// Private Functions
//******************************************************************************


//******************************************************************************
// Constructor
//******************************************************************************
void AlxSocket_Ctor
(
	AlxSocket* me
)
{
	// Parameters

	// Variables

	// Info
	me->wasCtorCalled = true;
	me->isInit = false;
}


//******************************************************************************
// Functions
//******************************************************************************

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		alxNet
  * @param[in]		protocol
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxSocket_Open(AlxSocket* me, AlxNet* alxNet, AlxSocket_Protocol protocol)
{
	// Assert
	ALX_SOCKET_ASSERT(me->wasCtorCalled == true);
	(void)me;
	(void)alxNet;
	(void)protocol;

	// Return
	return Alx_Err;
}

/**
  * @brief
  * @param[in,out]	me
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxSocket_Close(AlxSocket* me)
{
	// Assert
	ALX_SOCKET_ASSERT(me->wasCtorCalled == true);
	(void)me;

	// Return
	return Alx_Err;
}

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		ip
  * @param[in]		port
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxSocket_Connect(AlxSocket* me, const char* ip, uint16_t port)
{
	// Assert
	ALX_SOCKET_ASSERT(me->wasCtorCalled == true);
	(void)me;
	(void)ip;
	(void)port;

	// Return
	return Alx_Err;
}

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		data
  * @param[in]		len
  * @return
  */
int32_t AlxSocket_Send(AlxSocket* me, void* data, uint32_t len)
{
	// Assert
	ALX_SOCKET_ASSERT(me->wasCtorCalled == true);
	(void)me;
	(void)data;
	(void)len;

	// Return
	return -1;
}

/**
  * @brief
  * @param[in,out]	me
  * @param[out]		data
  * @param[in]		len
  * @return
  */
int32_t AlxSocket_Recv(AlxSocket* me, void* data, uint32_t len)
{
	// Assert
	ALX_SOCKET_ASSERT(me->wasCtorCalled == true);
	(void)me;
	(void)data;
	(void)len;

	// Return
	return -1;
}

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		timeout_ms
  */
void AlxSocket_SetTimeout_ms(AlxSocket* me, uint32_t timeout_ms)
{
	// Assert
	ALX_SOCKET_ASSERT(me->wasCtorCalled == true);
	(void)me;
	(void)timeout_ms;
}


//******************************************************************************
// Private Functions
//******************************************************************************


#endif	// #if defined(ALX_C_LIB)
