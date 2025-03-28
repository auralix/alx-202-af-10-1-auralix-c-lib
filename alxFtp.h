/**
  ******************************************************************************
  * @file		alxFtp.h
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
// Include Guard
//******************************************************************************
#ifndef ALX_FTP_H
#define ALX_FTP_H

#ifdef __cplusplus
extern "C" {
#endif


//******************************************************************************
// Includes
//******************************************************************************rp
#include "alxGlobal.h"
#include "alxTrace.h"
#include "alxAssert.h"
#include "alxNet.h"
#include "alxFs.h"
#include "alxSocket.h"
#include "alxOsDelay.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB)


//******************************************************************************
// Preprocessor
//******************************************************************************
#define ALX_FTP_FILE "alxFtp.h"

// Assert //
#if defined(ALX_FTP_ASSERT_BKPT_ENABLE)
	#define ALX_FTP_ASSERT(expr) ALX_ASSERT_BKPT(ALX_FTP_FILE, expr)
#elif defined(ALX_FTP_ASSERT_TRACE_ENABLE)
	#define ALX_FTP_ASSERT(expr) ALX_ASSERT_TRACE(ALX_FTP_FILE, expr)
#elif defined(ALX_FTP_ASSERT_RST_ENABLE)
	#define ALX_FTP_ASSERT(expr) ALX_ASSERT_RST(ALX_FTP_FILE, expr)
#else
	#define ALX_FTP_ASSERT(expr) do{} while (false)
#endif

// Trace //
#if defined(ALX_FTP_TRACE_ENABLE)
	#define ALX_FTP_TRACE_WRN(...) ALX_TRACE_WRN(ALX_FTP_FILE, __VA_ARGS__)
	#define ALX_FTP_TRACE_ERR(...) ALX_TRACE_ERR(ALX_FTP_FILE, __VA_ARGS__)
	#define ALX_FTP_TRACE_DBG(...) ALX_TRACE_DBG(ALX_FTP_FILE, __VA_ARGS__)
#else
	#define ALX_FTP_TRACE_WRN(...) do{} while (false)
	#define ALX_FTP_TRACE_ERR(...) do{} while (false)
	#define ALX_FTP_TRACE_DBG(...) do{} while (false)
#endif


//******************************************************************************
// Types
//******************************************************************************
typedef struct
{
	// Defines
	#define ALX_FTP_BUFF_LEN 2000
	#define ALX_FTP_SOCKET_CTRL_TIMEOUT_ms 30000
	#define ALX_FTP_SOCKET_DATA_TIMEOUT_ms 30000
	#define ALX_FTP_SOCKET_DATA_CONNECT_TRY_COUNT 10

	// Parameters
	AlxNet* alxNet;
	AlxFs* alxFs;

	// Fields
	const char* serverAddr;
	bool serverAddrIsHostname;
	uint16_t serverPort;
	const char* clientUsername;
	const char* clientPassword;

	// Variables
	AlxSocket alxSocket_Ctrl;
	char alxSocket_Ctrl_Ip[16];
	AlxSocket alxSocket_Data;
	char buff[ALX_FTP_BUFF_LEN];

	// Info
	bool wasCtorCalled;
	bool isClientLoggedIn;
} AlxFtp;


//******************************************************************************
// Constructor
//******************************************************************************
void AlxFtp_Ctor
(
	AlxFtp* me,
	AlxNet* alxNet,
	AlxFs* alxFs
);


//******************************************************************************
// Fields
//******************************************************************************
void AlxFtp_Client_SetServerAddr(AlxFtp* me, const char* serverAddr);
void AlxFtp_Client_SetServerAddrIsHostname(AlxFtp* me, bool serverAddrIsHostname);
void AlxFtp_Client_SetServerPort(AlxFtp* me, uint16_t serverPort);
void AlxFtp_Client_SetClientUsername(AlxFtp* me, const char* clientUsername);
void AlxFtp_Client_SetClientPassword(AlxFtp* me, const char* clientPassword);


//******************************************************************************
// Functions
//******************************************************************************
Alx_Status AlxFtp_Client_Login(AlxFtp* me);
Alx_Status AlxFtp_Client_Logout(AlxFtp* me);
Alx_Status AlxFtp_Client_UploadFile(AlxFtp* me, const char* localFilePath, const char* remoteFilePath, uint32_t* fileSize, AlxOsMutex* alxOsMutex_UploadFileInChunks);


#endif	// #if defined(ALX_C_LIB)

#ifdef __cplusplus
}
#endif

#endif	// #ifndef ALX_FTP_H
