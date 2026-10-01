/**
  ******************************************************************************
  * @file		alxNet.c
  * @brief		Auralix C Library - ALX Network Module
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
#include "alxNet.h"


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
void AlxNet_Ctor
(
	AlxNet* me
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
  * @retval			false
  * @retval			true
  */
bool AlxNet_IsConnected(AlxNet* me)
{
	// Assert
	ALX_NET_ASSERT(me->wasCtorCalled == true);
	(void)me;

	// Return
	return false;
}

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		hostname
  * @param[out]		ip
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxNet_Dns_GetHostByName(AlxNet* me, const char* hostname, char* ip)
{
	// Assert
	ALX_NET_ASSERT(me->wasCtorCalled == true);
	(void)me;
	(void)hostname;
	(void)ip;

	// Return
	return Alx_Err;
}


//******************************************************************************
// Private Functions
//******************************************************************************


#endif	// #if defined(ALX_C_LIB)
