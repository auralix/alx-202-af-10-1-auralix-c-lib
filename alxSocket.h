/**
  ******************************************************************************
  * @file		alxSocket.h
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
// Include Guard
//******************************************************************************
#ifndef ALX_SOCKET_H
#define ALX_SOCKET_H

#ifdef __cplusplus
extern "C" {
#endif


//******************************************************************************
// Includes
//******************************************************************************
#include "alxGlobal.h"
#include "alxTrace.h"
#include "alxAssert.h"
#include "alxNet.h"

#if defined(ALX_MBEDTLS)
#include "mbedtls/x509.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/ssl.h"
#endif

//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB) && (defined(ALX_FREE_RTOS_CELLULAR) || defined(ALX_WIZNET))


//******************************************************************************
// Preprocessor
//******************************************************************************
#define ALX_SOCKET_MODULE "AlxSocket.h"

// Assert //
#if defined(ALX_SOCKET_ASSERT_BKPT_ENABLE)
	#define ALX_SOCKET_ASSERT(expr) ALX_ASSERT_BKPT(ALX_SOCKET_MODULE, expr)
#elif defined(ALX_SOCKET_ASSERT_TRACE_ENABLE)
	#define ALX_SOCKET_ASSERT(expr) ALX_ASSERT_TRACE(ALX_SOCKET_MODULE, expr)
#elif defined(ALX_SOCKET_ASSERT_RST_ENABLE)
	#define ALX_SOCKET_ASSERT(expr) ALX_ASSERT_RST(ALX_SOCKET_MODULE, expr)
#else
	#define ALX_SOCKET_ASSERT(expr) do{} while (false)
#endif

// Trace //
#if defined(ALX_SOCKET_TRACE_ENABLE)
	#define ALX_SOCKET_TRACE_WRN(...) ALX_TRACE_WRN(ALX_SOCKET_MODULE, __VA_ARGS__)
	#define ALX_SOCKET_TRACE_INF(...) ALX_TRACE_INF(ALX_SOCKET_MODULE, __VA_ARGS__)
#else
	#define ALX_SOCKET_TRACE_WRN(...) do{} while (false)
	#define ALX_SOCKET_TRACE_INF(...) do{} while (false)
#endif


//******************************************************************************
// Types
//******************************************************************************
typedef enum
{
	AlxSocket_Protocol_Undefined,
	AlxSocket_Protocol_Udp,
	AlxSocket_Protocol_Tcp,
	AlxSocket_Protocol_Tls
} AlxSocket_Protocol;

typedef struct
{
	int wiz_socket;
	uint8_t wiz_protocol;
	uint8_t dst_ip[4];
	uint8_t mcast_ip[4];
	uint16_t dst_port;
	uint16_t my_port;
	bool wiz_sock_opened;
	int backlog;
} AlxWizSocketData;

typedef enum
{
	SSL_INITIALIZED_CACERT = 1,
	SSL_INITIALIZED_CLCERT,
	SSL_INITIALIZED_CLKEY,
	SSL_INITIALIZED_DRBG_CONTEXT,
	SSL_INITIALIZED_SSL_CONFIG,
	SSL_INITIALIZED_SSL_CONTEXT,
} AlxSslInitStateType;

#if defined(ALX_MBEDTLS)
typedef struct
{
	mbedtls_x509_crt x509_ca_certificate;
	mbedtls_x509_crt x509_cl_certificate;
	mbedtls_pk_context pkey;
	mbedtls_ctr_drbg_context drbg_context;
	mbedtls_ssl_config ssl_config;
	mbedtls_ssl_context ssl_context;
	AlxSslInitStateType init_state;
	char domain[32];
} AlxTlsData;
#endif

#if defined(ALX_FREE_RTOS_CELLULAR)

	// Event group bits
#define EVENT_BITS_SOCKET_CONNECT		0x01
#define EVENT_BITS_SOCKET_DATA_READY	0x02
#define EVENT_BITS_SOCKET_CLOSE			0x04

typedef struct
{
	CellularSocketHandle_t socket;
	CellularSocketProtocol_t protocol;
	EventGroupHandle_t event_group;	// event group used to dispatch events from cellular callbacks
	CellularUrcEvent_t URC_error;	// Unsolicited return code from cellular stack
	CellularSocketAddress_t sockAddr;
	uint16_t my_port;
	bool cellular_sock_opened;
	int backlog;
}AlxCellularSocketData;
#endif

typedef struct
{
	// Defines

	// Parameters

	// Variables
	AlxNet* alxNet;
	AlxSocket_Protocol protocol;
	#if defined(ALX_WIZNET)
	AlxWizSocketData socket_data;
	#endif
	#if defined(ALX_FREE_RTOS_CELLULAR)
	AlxCellularSocketData cellular_socket;
	StaticEventGroup_t xEventGroupBuffer;
	#endif
	#if defined(ALX_MBEDTLS)
	AlxTlsData tls_data;
	#endif
	uint32_t timeout;

	// Info
	bool wasCtorCalled;
	bool isOpened;
} AlxSocket;


//******************************************************************************
// Constructor
//******************************************************************************
void AlxSocket_Ctor
(
	AlxSocket* me
);


//******************************************************************************
// Functions
//******************************************************************************
Alx_Status AlxSocket_Open(AlxSocket* me, AlxNet* alxNet, AlxSocket_Protocol protocol);
Alx_Status AlxSocket_Close(AlxSocket* me);
Alx_Status AlxSocket_Connect(AlxSocket* me, const char* ip, uint16_t port);
Alx_Status AlxSocket_Bind(AlxSocket* me, uint16_t port);
Alx_Status AlxSocket_Bind_Mcast(AlxSocket* me, const char* ip, uint16_t port);
Alx_Status AlxSocket_Listen(AlxSocket* me, uint8_t backlog);
AlxSocket* AlxSocket_Accept(AlxSocket* me);
int32_t AlxSocket_Send(AlxSocket* me, void* data, uint32_t len);
int32_t AlxSocket_Recv(AlxSocket* me, void* data, uint32_t len);
void AlxSocket_SetTimeout_ms(AlxSocket* me, uint32_t timeout_ms);
#if defined(ALX_MBEDTLS)
Alx_Status AlxSocket_InitTls(AlxSocket* me, const char *server_domain, const unsigned char *ca_cert, const unsigned char *cl_cert, const unsigned char *cl_key);
#endif


#endif	// #if defined(ALX_C_LIB) && (defined(ALX_FREE_RTOS_CELLULAR) || defined(ALX_WIZNET))

#ifdef __cplusplus
}
#endif

#endif	// #ifndef ALX_SOCKET_H
