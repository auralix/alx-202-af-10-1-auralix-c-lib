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
// Private Functions
//******************************************************************************
static void AlxFtp_Reset(AlxFtp* me);


//******************************************************************************
// Constructor
//******************************************************************************
void AlxFtp_Ctor
(
	AlxFtp* me,
	AlxNet* alxNet
)
{
	// Parameters
	me->alxNet = alxNet;

	// Fields
	me->serverAddr = "";
	me->serverAddrIsHostname = false;
	me->serverPort = 0;
	me->clientUsername = "";
	me->clientPassword = "";

	// Variables
	AlxSocket_Ctor(&me->alxSocket_Ctrl);
	AlxSocket_Ctor(&me->alxSocket_Data);
	memset(me->buff, 0, sizeof(me->buff));

	// Info
	me->wasCtorCalled = true;
	me->isClientLoggedIn = false;
}


//******************************************************************************
// Fields
//******************************************************************************
void AlxFtp_Client_SetServerAddr(AlxFtp* me, const char* serverAddr)
{
	// Assert
	ALX_FTP_ASSERT(me->wasCtorCalled == true);
	ALX_FTP_ASSERT(me->isClientLoggedIn == false);

	// Set
	me->serverAddr = serverAddr;
}
void AlxFtp_Client_SetServerAddrIsHostname(AlxFtp* me, bool serverAddrIsHostname)
{
	// Assert
	ALX_FTP_ASSERT(me->wasCtorCalled == true);
	ALX_FTP_ASSERT(me->isClientLoggedIn == false);

	// Set
	me->serverAddrIsHostname = serverAddrIsHostname;
}
void AlxFtp_Client_SetServerPort(AlxFtp* me, uint16_t serverPort)
{
	// Assert
	ALX_FTP_ASSERT(me->wasCtorCalled == true);
	ALX_FTP_ASSERT(me->isClientLoggedIn == false);

	// Set
	me->serverPort = serverPort;
}
void AlxFtp_Client_SetClientUsername(AlxFtp* me, const char* clientUsername)
{
	// Assert
	ALX_FTP_ASSERT(me->wasCtorCalled == true);
	ALX_FTP_ASSERT(me->isClientLoggedIn == false);

	// Set
	me->clientUsername = clientUsername;
}
void AlxFtp_Client_SetClientPassword(AlxFtp* me, const char* clientPassword)
{
	// Assert
	ALX_FTP_ASSERT(me->wasCtorCalled == true);
	ALX_FTP_ASSERT(me->isClientLoggedIn == false);

	// Set
	me->clientPassword = clientPassword;
}


//******************************************************************************
// Functions
//******************************************************************************
Alx_Status AlxFtp_Client_Login(AlxFtp* me)
{
	//------------------------------------------------------------------------------
	// Assert
	//------------------------------------------------------------------------------
	ALX_FTP_ASSERT(me->wasCtorCalled == true);
	ALX_FTP_ASSERT(me->isClientLoggedIn == false);


	//------------------------------------------------------------------------------
	// Local Variables
	//------------------------------------------------------------------------------
	Alx_Status status = Alx_Err;
	int32_t statusLen = 0;
	int32_t len = 0;
	char alxSocket_Ctrl_Ip[16] = "";
	uint16_t alxSocket_Ctrl_Port = 0;


	//------------------------------------------------------------------------------
	// Handle Network
	//------------------------------------------------------------------------------

	// Check if network connected
	bool isNetConnected = AlxNet_IsConnected(me->alxNet);
	if (isNetConnected != true)
	{
		ALX_FTP_TRACE_WRN("FAIL: AlxNet_IsConnected()");
		AlxFtp_Reset(me);
		return Alx_Err;
	}

	// Set control socket IP, resolve if hostname format
	if (me->serverAddrIsHostname)
	{
		status = AlxNet_Dns_GetHostByName(me->alxNet, me->serverAddr, alxSocket_Ctrl_Ip);
		if (status != Alx_Ok)
		{
			ALX_FTP_TRACE_WRN("FAIL: AlxNet_Dns_GetHostByName() status %ld", status);
			AlxFtp_Reset(me);
			return Alx_Err;
		}
	}
	else
	{
		strcpy(alxSocket_Ctrl_Ip, me->serverAddr);
	}

	// Set control socket port
	alxSocket_Ctrl_Port = me->serverPort;


	//------------------------------------------------------------------------------
	// Connect Control Socket
	//------------------------------------------------------------------------------

	// Set control socket timeout
	AlxSocket_SetTimeout_ms(&me->alxSocket_Ctrl, ALX_FTP_SOCKET_CTRL_TIMEOUT_ms);

	// Open control socket
	status = AlxSocket_Open(&me->alxSocket_Ctrl, me->alxNet, AlxSocket_Protocol_Tcp);
	if (status != Alx_Ok)
	{
		ALX_FTP_TRACE_WRN("FAIL: AlxSocket_Open() status %ld", status);
		AlxFtp_Reset(me);
		return Alx_Err;
	}

	// Connect control socket
	status = AlxSocket_Connect(&me->alxSocket_Ctrl, alxSocket_Ctrl_Ip, alxSocket_Ctrl_Port);
	if (status != Alx_Ok)
	{
		ALX_FTP_TRACE_WRN("FAIL: AlxSocket_Connect() status %ld alxSocket_Ctrl_Ip %s alxSocket_Ctrl_Port %u", status, alxSocket_Ctrl_Ip, alxSocket_Ctrl_Port);
		AlxFtp_Reset(me);
		return Alx_Err;
	}

	// Receive response
	statusLen = AlxSocket_Recv(&me->alxSocket_Ctrl, me->buff, ALX_FTP_BUFF_LEN);
	if (statusLen <= 0)
	{
		ALX_FTP_TRACE_WRN("FAIL: AlxSocket_Recv() statusLen %ld buff %.100s", statusLen, me->buff);
		AlxFtp_Reset(me);
		return Alx_Err;
	}

	// Check response - Expected response: "220" -> Service ready for new user
	if(strncmp(me->buff, "220", 3) != 0)
	{
		ALX_FTP_TRACE_WRN("FAIL: strncmp() buff %.100s", me->buff);
		AlxFtp_Reset(me);
		return Alx_Err;
	}


	//------------------------------------------------------------------------------
	// Send Username
	//------------------------------------------------------------------------------

	// Send username
	len = sprintf(me->buff, "user %s\r\n", me->clientUsername);
	statusLen = AlxSocket_Send(&me->alxSocket_Ctrl, me->buff, len);
	if (statusLen != len)
	{
		ALX_FTP_TRACE_WRN("FAIL: AlxSocket_Send() statusLen %ld buff %.100s len %lu", statusLen, me->buff, len);
		AlxFtp_Reset(me);
		return Alx_Err;
	}

	// Receive response
	statusLen = AlxSocket_Recv(&me->alxSocket_Ctrl, me->buff, ALX_FTP_BUFF_LEN);
	if (statusLen <= 0)
	{
		ALX_FTP_TRACE_WRN("FAIL: AlxSocket_Recv() statusLen %ld buff %.100s", statusLen, me->buff);
		AlxFtp_Reset(me);
		return Alx_Err;
	}

	// Check response - Expected response: "331" -> User name OK, need password
	if (strncmp(&me->buff, "331", 3) != 0)
	{
		ALX_FTP_TRACE_WRN("FAIL: strncmp() buff %.100s", me->buff);
		AlxFtp_Reset(me);
		return Alx_Err;
	}


	//------------------------------------------------------------------------------
	// Send Password
	//------------------------------------------------------------------------------

	// Send password
	len = sprintf(me->buff, "pass %s\r\n", me->clientPassword);
	statusLen = AlxSocket_Send(&me->alxSocket_Ctrl, me->buff, len);
	if (statusLen != len)
	{
		ALX_FTP_TRACE_WRN("FAIL: AlxSocket_Send() statusLen %ld buff %.100s len %lu", statusLen, me->buff, len);
		AlxFtp_Reset(me);
		return Alx_Err;
	}

	// Receive response
	statusLen = AlxSocket_Recv(&me->alxSocket_Ctrl, me->buff, ALX_FTP_BUFF_LEN);
	if (statusLen <= 0)
	{
		ALX_FTP_TRACE_WRN("FAIL: AlxSocket_Recv() statusLen %ld buff %.100s", statusLen, me->buff);
		AlxFtp_Reset(me);
		return Alx_Err;
	}

	// Check response - Expected response: "230" -> User logged in, proceed
	if (strncmp(me->buff, "230", 3) != 0)
	{
		ALX_FTP_TRACE_WRN("FAIL: strncmp() buff %.100s", me->buff);
		AlxFtp_Reset(me);
		return Alx_Err;
	}


	//------------------------------------------------------------------------------
	// Send Select Binary Type Transfer Command
	//------------------------------------------------------------------------------

	// Send command
	len = sprintf(me->buff, "type I\r\n");
	statusLen = AlxSocket_Send(&me->alxSocket_Ctrl, me->buff, len);
	if (statusLen != len)
	{
		ALX_FTP_TRACE_WRN("FAIL: AlxSocket_Send() statusLen %ld buff %.100s len %lu", statusLen, me->buff, len);
		AlxFtp_Reset(me);
		return Alx_Err;
	}

	// Receive response
	statusLen = AlxSocket_Recv(&me->alxSocket_Ctrl, me->buff, ALX_FTP_BUFF_LEN);
	if (statusLen <= 0)
	{
		ALX_FTP_TRACE_WRN("FAIL: AlxSocket_Recv() statusLen %ld buff %.100s", statusLen, me->buff);
		AlxFtp_Reset(me);
		return Alx_Err;
	}

	// Check response
	if (strncmp(me->buff, "200", 3) != 0)
	{
		ALX_FTP_TRACE_WRN("FAIL: strncmp() buff %.100s", me->buff);
		AlxFtp_Reset(me);
		return Alx_Err;
	}


	//------------------------------------------------------------------------------
	// Return
	//------------------------------------------------------------------------------

	// Set
	me->isClientLoggedIn = true;

	// Return
	return Alx_Ok;
}
Alx_Status AlxFtp_Client_Logout(AlxFtp* me)
{
	//------------------------------------------------------------------------------
	// Assert
	//------------------------------------------------------------------------------
	ALX_FTP_ASSERT(me->wasCtorCalled == true);
	ALX_FTP_ASSERT(me->isClientLoggedIn == true);


	//------------------------------------------------------------------------------
	// Local Variables
	//------------------------------------------------------------------------------
	Alx_Status status = Alx_Err;
	int32_t statusLen = 0;
	int32_t len = 0;


	//------------------------------------------------------------------------------
	// Send Quit Command
	//------------------------------------------------------------------------------

	// Send command
	len = sprintf(me->buff, "quit\r\n");
	statusLen = AlxSocket_Send(&me->alxSocket_Ctrl, me->buff, len);
	if (statusLen != len)
	{
		ALX_FTP_TRACE_WRN("FAIL: AlxSocket_Send() statusLen %ld buff %.100s len %lu", statusLen, me->buff, len);
		AlxFtp_Reset(me);
		return Alx_Err;
	}

	// Receive response
	statusLen = AlxSocket_Recv(&me->alxSocket_Ctrl, me->buff, ALX_FTP_BUFF_LEN);
	if (statusLen <= 0)
	{
		ALX_FTP_TRACE_WRN("FAIL: AlxSocket_Recv() statusLen %ld buff %.100s", statusLen, me->buff);
		AlxFtp_Reset(me);
		return Alx_Err;
	}

	// Check response - Expected response: "221" -> Service closing control connection - AS: Often no replay
	if (strncmp(me->buff, "221", 3) != 0)
	{
		ALX_FTP_TRACE_WRN("FAIL: strncmp() buff %.100s", me->buff);
		AlxFtp_Reset(me);
		return Alx_Err;
	}


	//------------------------------------------------------------------------------
	// Close Control Socket
	//------------------------------------------------------------------------------
	status = AlxSocket_Close(&me->alxSocket_Ctrl);
	if (status != Alx_Ok)
	{
		ALX_FTP_TRACE_WRN("FAIL: AlxSocket_Close() status %ld", status);
		AlxFtp_Reset(me);
		return Alx_Err;
	}


	//------------------------------------------------------------------------------
	// Return
	//------------------------------------------------------------------------------

	// Clear
	me->isClientLoggedIn = false;

	// Return
	return Alx_Ok;
}
Alx_Status AlxFtp_Client_UploadFile(AlxFtp* me, const char* localFilePath, const char* remoteFilePath)
{
	//------------------------------------------------------------------------------
	// Assert
	//------------------------------------------------------------------------------
	ALX_FTP_ASSERT(me->wasCtorCalled == true);
	ALX_FTP_ASSERT(me->isClientLoggedIn == true);


	//------------------------------------------------------------------------------
	// Local Variables
	//------------------------------------------------------------------------------
	Alx_Status status = Alx_Err;
	int32_t statusLen = 0;
	int32_t len = 0;
	char alxSocket_Data_Ip[16] = "";
	uint16_t alxSocket_Data_Port = 0;


	//------------------------------------------------------------------------------
	// Send Enter Passive Mode Command
	//------------------------------------------------------------------------------

	// Send command
	len = sprintf(me->buff, "pasv\r\n");
	statusLen = AlxSocket_Send(&me->alxSocket_Ctrl, me->buff, len);
	if (statusLen != len)
	{
		ALX_FTP_TRACE_WRN("FAIL: AlxSocket_Send() statusLen %ld buff %.100s len %lu", statusLen, me->buff, len);
		AlxFtp_Reset(me);
		return Alx_Err;
	}

	// Receive response
	statusLen = AlxSocket_Recv(&me->alxSocket_Ctrl, me->buff, ALX_FTP_BUFF_LEN);
	if (statusLen <= 0)
	{
		ALX_FTP_TRACE_WRN("FAIL: AlxSocket_Recv() statusLen %ld buff %.100s", statusLen, me->buff);
		AlxFtp_Reset(me);
		return Alx_Err;
	}

	// Check response - Expected response: "227" -> Entering passive mode
	if (strncmp(me->buff, "227", 3) != 0)
	{
		ALX_FTP_TRACE_WRN("FAIL: strncmp() buff %.100s", me->buff);
		AlxFtp_Reset(me);
		return Alx_Err;
	}


	//------------------------------------------------------------------------------
	// Parse PASV Response Tuple + Set Data Socket IP & Port
	//------------------------------------------------------------------------------

	// Zero terminate response
	me->buff[statusLen] = 0;

	// Find '('
	char* ptr = strchr(me->buff, '(');
	if (ptr == NULL)
	{
		ALX_FTP_TRACE_WRN("FAIL: strchr() buff %.100s", me->buff);
		AlxFtp_Reset(me);
		return Alx_Err;
	}

	// Parse tuple
	uint8_t tuple[6] = {};
	int32_t sscanfStatus = sscanf(ptr, "(%u,%u,%u,%u,%u,%u)", &tuple[0], &tuple[1], &tuple[2], &tuple[3], &tuple[4], &tuple[5]);
	if (sscanfStatus != 6)
	{
		ALX_FTP_TRACE_WRN("FAIL: sscanf() buff %.100s", me->buff);
		AlxFtp_Reset(me);
		return Alx_Err;
	}

	// Set data socket IP
	sprintf(alxSocket_Data_Ip, "%u.%u.%u.%u", tuple[0], tuple[1], tuple[2], tuple[3]);

	// Set data socket port
	alxSocket_Data_Port = (tuple[4] * 256) + tuple[5];


	//------------------------------------------------------------------------------
	// Connect Data Socket
	//------------------------------------------------------------------------------

	// Set data socket timeout
	AlxSocket_SetTimeout_ms(&me->alxSocket_Data, ALX_FTP_SOCKET_DATA_TIMEOUT_ms);

	// Open data socket
	status = AlxSocket_Open(&me->alxSocket_Data, me->alxNet, AlxSocket_Protocol_Tcp);
	if (status != Alx_Ok)
	{
		ALX_FTP_TRACE_WRN("FAIL: AlxSocket_Open() status %ld", status);
		AlxFtp_Reset(me);
		return Alx_Err;
	}

	// Connect data socket
	for (uint32_t i = 0; i < ALX_FTP_SOCKET_DATA_CONNECT_TRY_COUNT; i++)
	{
		// Connect
		status = AlxSocket_Connect(&me->alxSocket_Data, alxSocket_Data_Ip, alxSocket_Data_Port);
		if (status == Alx_Ok)
		{
			break;
		}

		// Wait
		AlxOsDelay_ms(&alxOsDelay, 1);
	}
	if (status != Alx_Ok)
	{
		ALX_FTP_TRACE_WRN("FAIL: AlxSocket_Connect() status %ld alxSocket_Data_Ip %s alxSocket_Data_Port %u", status, alxSocket_Data_Ip, alxSocket_Data_Port);
		AlxFtp_Reset(me);
		return Alx_Err;
	}


	//------------------------------------------------------------------------------
	// Send command to initiate sending file to the server
	//------------------------------------------------------------------------------

	// Send command
	len = sprintf(me->buff, "stor %s\r\n", remoteFilePath);
	statusLen = AlxSocket_Send(&me->alxSocket_Ctrl, me->buff, len);
	if (statusLen != len)
	{
		ALX_FTP_TRACE_WRN("FAIL: AlxSocket_Send() statusLen %ld buff %.100s len %lu", statusLen, me->buff, len);
		AlxFtp_Reset(me);
		return Alx_Err;
	}

	// Receive response
	statusLen = AlxSocket_Recv(&me->alxSocket_Ctrl, me->buff, ALX_FTP_BUFF_LEN);
	if (statusLen <= 0)
	{
		ALX_FTP_TRACE_WRN("FAIL: AlxSocket_Recv() statusLen %ld buff %.100s", statusLen, me->buff);
		AlxFtp_Reset(me);
		return Alx_Err;
	}

	// Expected response: "150" -> File status okay, about to open data connection OR "125" -> Data connection already open, transfer starting
	if ((strncmp(me->buff, "150", 3) != 0) && (strncmp(me->buff, "125", 3) != 0))
	{
		ALX_FTP_TRACE_WRN("FAIL: strncmp() buff %.100s", me->buff);
		AlxFtp_Reset(me);
		return Alx_Err;
	}


	//------------------------------------------------------------------------------
	// Upload File
	//------------------------------------------------------------------------------
	const char* testStr = "Hello world!!!\r\n";
	len = strlen(testStr);
	statusLen = AlxSocket_Send(&me->alxSocket_Data, testStr, len);
	if (statusLen != len)
	{
		ALX_FTP_TRACE_WRN("FAIL: AlxSocket_Send() statusLen %ld testStr %.100s len %lu", statusLen, testStr, len);
		AlxFtp_Reset(me);
		return Alx_Err;
	}


	//------------------------------------------------------------------------------
	// Close Data Socket
	//------------------------------------------------------------------------------

	// Close data socket
	status = AlxSocket_Close(&me->alxSocket_Data);
	if (status != Alx_Ok)
	{
		ALX_FTP_TRACE_WRN("FAIL: alxSocket_Data() status %ld", status);
		AlxFtp_Reset(me);
		return Alx_Err;
	}

	// Receive response
	statusLen = AlxSocket_Recv(&me->alxSocket_Ctrl, me->buff, ALX_FTP_BUFF_LEN);
	if (statusLen <= 0)
	{
		ALX_FTP_TRACE_WRN("FAIL: AlxSocket_Recv() statusLen %ld buff %.100s", statusLen, me->buff);
		AlxFtp_Reset(me);
		return Alx_Err;
	}

	// Check response - Expected response: "226" -> Closing data connection, requested file action successful
	if (strncmp(me->buff, "226", 3) != 0)
	{
		ALX_FTP_TRACE_WRN("FAIL: strncmp() buff %.100s", me->buff);
		AlxFtp_Reset(me);
		return Alx_Err;
	}


	//------------------------------------------------------------------------------
	// Return
	//------------------------------------------------------------------------------
	return Alx_Ok;
}


//******************************************************************************
// Private Functions
//******************************************************************************
static void AlxFtp_Reset(AlxFtp* me)
{
	// Local variables
	Alx_Status status = Alx_Err;

	// Close data socket
	status = AlxSocket_Close(&me->alxSocket_Data);
	if (status != Alx_Ok)
	{
		ALX_FTP_TRACE_WRN("FAIL: alxSocket_Data() status %ld", status);
	}

	// Close control socket
	status = AlxSocket_Close(&me->alxSocket_Ctrl);
	if (status != Alx_Ok)
	{
		ALX_FTP_TRACE_WRN("FAIL: alxSocket_Ctrl() status %ld", status);
	}

	// Clear
	memset(me->buff, 0, sizeof(me->buff));
	me->isClientLoggedIn = false;
}


#endif	// #if defined(ALX_C_LIB)
