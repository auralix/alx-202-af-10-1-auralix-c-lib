/**
  ******************************************************************************
  * @file		alxFtp.c
  * @brief		Auralix C Library - ALX FTP Module
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
#include "alxFtp.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB)


//******************************************************************************
// Constructor
//******************************************************************************
void AlxFtp_Ctor
(
	AlxFtp* me,
	AlxNet* alxNet,
	const char* serverAddr,
	bool serverAddrIsHostname,
	uint16_t serverPort,
	const char* clientUsername,
	const char* clientPassword
)
{
	// Parameters
	me->alxNet = alxNet;
	me->serverAddr = serverAddr;
	me->serverAddrIsHostname = serverAddrIsHostname;
	me->serverPort = serverPort;
	me->clientUsername = clientUsername;
	me->clientPassword = clientPassword;

	// Variables

	// Info
	me->wasCtorCalled = true;
	me->isClientLoggedIn = false;
}


//******************************************************************************
// Fields
//******************************************************************************
void AlxFtp_SetServerAddr(AlxFtp* me, const char* serverAddr)
{
	// Assert
	ALX_FTP_ASSERT(me->wasCtorCalled == true);

	// Set
	me->serverAddr = serverAddr;
}
void AlxFtp_SetServerAddrIsHostname(AlxFtp* me, bool serverAddrIsHostname)
{
	// Assert
	ALX_FTP_ASSERT(me->wasCtorCalled == true);

	// Set
	me->serverAddrIsHostname = serverAddrIsHostname;
}
void AlxFtp_SetServerPort(AlxFtp* me, uint16_t serverPort)
{
	// Assert
	ALX_FTP_ASSERT(me->wasCtorCalled == true);

	// Set
	me->serverPort = serverPort;
}
void AlxFtp_SetClientUsername(AlxFtp* me, const char* clientUsername)
{
	// Assert
	ALX_FTP_ASSERT(me->wasCtorCalled == true);

	// Set
	me->clientUsername = clientUsername;
}
void AlxFtp_SetClientPassword(AlxFtp* me, const char* clientPassword)
{
	// Assert
	ALX_FTP_ASSERT(me->wasCtorCalled == true);

	// Set
	me->clientPassword = clientPassword;
}


//******************************************************************************
// Functions
//******************************************************************************
Alx_Status AlxFtp_Client_Login(AlxFtp* me)
{
	// Assert
	ALX_FTP_ASSERT(me->wasCtorCalled == true);
	ALX_FTP_ASSERT(me->isClientLoggedIn == false);

	// Set
	me->isClientLoggedIn = true;

	// Return
	return Alx_Ok;
}
Alx_Status AlxFtp_Client_Logout(AlxFtp* me)
{
	// Assert
	ALX_FTP_ASSERT(me->wasCtorCalled == true);
	ALX_FTP_ASSERT(me->isClientLoggedIn == true);

	// Clear
	me->isClientLoggedIn = false;

	// Return
	return Alx_Ok;
}
Alx_Status AlxFtp_Client_UploadFile(AlxFtp* me, const char* localFilePath, const char* remoteFilePath)
{
	// Assert
	ALX_FTP_ASSERT(me->wasCtorCalled == true);
	ALX_FTP_ASSERT(me->isClientLoggedIn == true);

	// Return
	return Alx_Ok;
}


#endif	// #if defined(ALX_C_LIB)
