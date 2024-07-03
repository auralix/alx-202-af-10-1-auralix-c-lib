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
#include "alxOsMutex.h"
#include "alxOsDelay.h"
#include "alxOsThread.h"
#include "alxTick.h"
#if defined(ALX_WIZNET)
#include "socket.h"
#include "w5500.h"
#endif


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB) && (defined(ALX_FREE_RTOS_CELLULAR) || defined(ALX_WIZNET))


//******************************************************************************
// Private Defines
//******************************************************************************
#define SOCKET_YIELD() taskYIELD();
//#define SOCKET_YIELD() AlxOsDelay_ms(&alxOsDelay, 1)

#define SOCKET_DEFAULT_TIMEOUT   5000

//******************************************************************************
// Static Wiznet Variables
//******************************************************************************
#if defined(ALX_WIZNET)
static uint8_t wiz_socks[_WIZCHIP_SOCK_NUM_] = { Sn_MR_CLOSE, };
static AlxOsMutex alxSocketAllocMutex;
static AlxSocket wiz_server_sockets[_WIZCHIP_SOCK_NUM_]; // memory fror new sockets when accepting connections
#endif

static AlxOsMutex socketMutex;
static volatile uint8_t socketConnectBits = 0;

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
	me->alxNet = NULL;
	me->protocol = AlxSocket_Protocol_Undefined;
	if (!alxSocketAllocMutex.wasCtorCalled)
	{
		AlxOsMutex_Ctor(&alxSocketAllocMutex);
	}
	if (!socketMutex.wasCtorCalled)
	{
		AlxOsMutex_Ctor(&socketMutex);
	}

	me->timeout = SOCKET_DEFAULT_TIMEOUT;

	#if defined(ALX_WIZNET)
	me->socket_data.wiz_socket = -1;
	#endif
	#if defined(ALX_FREE_RTOS_CELLULAR)
	// Cannot perform any return value check here
	me->cellular_socket.event_group = xEventGroupCreate();
	#endif
	#if defined(ALX_MBEDTLS)
	me->tls_data.init_state = 0;
	#endif
	// Info
	me->wasCtorCalled = true;
}

//******************************************************************************
// Static Wiznet Functions
//******************************************************************************
#if defined(ALX_WIZNET)
static int8_t wiz_sock_alloc(uint8_t protocol)
{
	AlxOsMutex_Lock(&alxSocketAllocMutex);
	for (int i = 0; i < _WIZCHIP_SOCK_NUM_; i++)
	{
		if (wiz_socks[i] == Sn_MR_CLOSE)
		{
			wiz_socks[i] = protocol;
			AlxOsMutex_Unlock(&alxSocketAllocMutex);
			return i;
		}
	}
	AlxOsMutex_Unlock(&alxSocketAllocMutex);
	return -1;
}

static void wiz_sock_release(uint8_t wiz_socket)
{
	AlxOsMutex_Lock(&alxSocketAllocMutex);
	wiz_socks[wiz_socket] = Sn_MR_CLOSE;
	AlxOsMutex_Unlock(&alxSocketAllocMutex);
}

static uint16_t wiz_any_port(void)
{
	static uint16_t port_number = 0xc000;
	if(port_number++ == 0xFFF0) port_number = 0xc000;
	return port_number;
}

static int wiz_recv(AlxSocket* me, void* data, uint32_t len)
{
	uint32_t received_total = 0;
	uint64_t timer_start = AlxTick_Get_ms(&alxTick);
	while (1)
	{
		if (getSn_SR(me->socket_data.wiz_socket) != SOCK_ESTABLISHED)
		{
			return SOCKERR_SOCKSTATUS;
		}
		if (getSn_RX_RSR(me->socket_data.wiz_socket) > 0)
		{
			int received_chunk = recv(me->socket_data.wiz_socket, data + received_total, len - received_total);
			if (received_chunk <= 0)
			{
				return received_chunk;
			}
			received_total += received_chunk;
		}
		if (received_total >= len)
		{
			break;
		}
		if ((AlxTick_Get_ms(&alxTick) - timer_start >= me->timeout) || (received_total > 0))
		{
			break;
		}
		SOCKET_YIELD();
	}
	return received_total;
}

static int wiz_send(AlxSocket* me, void* data, uint32_t len)
{
	uint32_t sent_total = 0;
	uint64_t timer_start = AlxTick_Get_ms(&alxTick);
	while (1)
	{
		if (getSn_SR(me->socket_data.wiz_socket) != SOCK_ESTABLISHED)
		{
			return SOCKERR_SOCKSTATUS;
		}
		int sent_chunk = send(me->socket_data.wiz_socket, data + sent_total, len - sent_total);
		if (sent_chunk <= 0)
		{
			return sent_chunk;
		}
		sent_total += sent_chunk;
		if (sent_total >= len)
		{
			break;
		}
		if (AlxTick_Get_ms(&alxTick) - timer_start >= me->timeout)
		{
			break;
		}
		SOCKET_YIELD();
	}
	return sent_total;
}
#endif

//******************************************************************************
// Static Cellular Functions
//******************************************************************************
#if defined(ALX_FREE_RTOS_CELLULAR)

static void CellularSocketOpenCallback( CellularUrcEvent_t urcEvent,
	CellularSocketHandle_t socketHandle,
	void * pCallbackContext)
{
	UNUSED(socketHandle);
	if (pCallbackContext == NULL) return;
	AlxSocket *me = (AlxSocket*)(pCallbackContext);
	//ALX_SOCKET_TRACE_FORMAT("EVENT_BITS_SOCKET_CONNECT\r\n");
	me->cellular_socket.URC_error = urcEvent;
	xEventGroupSetBits(me->cellular_socket.event_group, EVENT_BITS_SOCKET_CONNECT);
}

static void CellularSocketCloseCallback(CellularSocketHandle_t socketHandle,
	void * pCallbackContext)
{
	UNUSED(socketHandle);
	if (pCallbackContext == NULL) return;
	AlxSocket *me = (AlxSocket*)(pCallbackContext);
	xEventGroupSetBits(me->cellular_socket.event_group, EVENT_BITS_SOCKET_CLOSE);
}

static void CellularSocketDataReadyCallback(CellularSocketHandle_t socketHandle,
	void * pCallbackContext)
{
	UNUSED(socketHandle);
	UNUSED(pCallbackContext);

	// Currently unused!

	//if (pCallbackContext == NULL) return;
	//AlxSocket *me = (AlxSocket*)(pCallbackContext);
	//ALX_SOCKET_TRACE_FORMAT("EVENT_BITS_SOCKET_DATA_READY\r\n");
	//xEventGroupSetBits(me->cellular_socket.event_group, EVENT_BITS_SOCKET_DATA_READY);
}
static int cell_send(AlxSocket* me, void* data, uint32_t len)
{
	uint32_t sent_length = 0;
	uint32_t sent_total = 0;

	while (1)
	{
		CellularError_t cellularStatus = Cellular_SocketSend(me->alxNet->cellular.handle, me->cellular_socket.socket, data + sent_total, len - sent_total, &sent_length);
		if (cellularStatus != CELLULAR_SUCCESS) return -1;

		if (sent_length <= 0)
		{
			return sent_length;
		}

		sent_total += sent_length;
		if (sent_total >= len)
		{
			break;
		}
		SOCKET_YIELD();
	}

	return sent_total;
}

static int cell_recv(AlxSocket* me, void* data, uint32_t len)
{
	uint32_t received_total = 0;
	uint32_t received_chunk = 0;
	int32_t total = 0;
	AlxOsMutex_Lock(&socketMutex);
	uint64_t timer_start = AlxTick_Get_ms(&alxTick);
	while (1)
	{
		CellularError_t ret = Cellular_SocketRecv(me->alxNet->cellular.handle, me->cellular_socket.socket, data + received_total, len - received_total, &received_chunk);
		if (ret != CELLULAR_SUCCESS)
		{
			total = -1;
			break;
		}

		if (received_chunk <= len)
		{
			total = received_chunk;
			if (total != 0) break;
		}

		received_total += received_chunk;

		if (received_total >= len)
		{
			total = received_total;
			break;
		}

		if ((AlxTick_Get_ms(&alxTick) - timer_start >= me->timeout) || (received_total > 0))
		{
			break;
		}

		SOCKET_YIELD();
	}

	AlxOsMutex_Unlock(&socketMutex);
	return total;
}
#endif

//******************************************************************************
// Static TLS functions
//******************************************************************************
#if defined(ALX_MBEDTLS)
static int sslRandomCallback(void *p_rng, unsigned char *output, size_t output_len)
{
	UNUSED(p_rng);
	int i;

	if (output_len <= 0)
	{
		return 1;
	}

	for (i = 0; i < output_len; i++)
	{
		*output++ = rand() % 0xff;
	}

	srand(rand());

	return 0;
}
static void sslFree(AlxTlsData* tls)
{
	if (tls->init_state >= SSL_INITIALIZED_CACERT)
	{
		mbedtls_x509_crt_free(&tls->x509_ca_certificate);
	}
	if (tls->init_state  >= SSL_INITIALIZED_CLCERT)
	{
		mbedtls_x509_crt_free(&tls->x509_cl_certificate);
	}
	if (tls->init_state  >= SSL_INITIALIZED_CLKEY)
	{
		mbedtls_pk_free(&tls->pkey);
	}
	if (tls->init_state  >= SSL_INITIALIZED_DRBG_CONTEXT)
	{
		mbedtls_ctr_drbg_free(&tls->drbg_context);
	}
	if (tls->init_state  >= SSL_INITIALIZED_SSL_CONFIG)
	{
		mbedtls_ssl_config_free(&tls->ssl_config);
	}
	if (tls->init_state  >= SSL_INITIALIZED_SSL_CONTEXT)
	{
		mbedtls_ssl_free(&tls->ssl_context);
	}
}

static int sslRecv(void *ctx, unsigned char *buf, size_t len)
{
	AlxSocket* me = (AlxSocket*)ctx;
	int ret = Alx_Err;

#if defined (ALX_WIZNET)
	if (me->alxNet->config == AlxNet_Config_Wiznet)
	{
		ret = wiz_recv(ctx, buf, len);
	}
#endif
#if defined(ALX_FREE_RTOS_CELLULAR)
	if (me->alxNet->config == AlxNet_Config_FreeRtos_Cellular)
	{
		ret = cell_recv(ctx, buf, len);
	}
#endif
	return ret;
}
static int sslSend(void *ctx, const unsigned char *buf, size_t len)
{
	AlxSocket* me = (AlxSocket*)ctx;
	int ret = Alx_Err;

#if defined (ALX_WIZNET)
	if (me->alxNet->config == AlxNet_Config_Wiznet)
	{
		ret = wiz_send(ctx, (unsigned char *)buf, len);
	}
#endif
#if defined(ALX_FREE_RTOS_CELLULAR)
	if (me->alxNet->config == AlxNet_Config_FreeRtos_Cellular)
	{
		ret = cell_send(ctx, (unsigned char *)buf, len);
	}
#endif
	return ret;
}
static Alx_Status sslHandshake(AlxSocket *me)
{
	int status;
	while ((status = mbedtls_ssl_handshake(&me->tls_data.ssl_context)) != 0)
	{
		if (status != MBEDTLS_ERR_SSL_WANT_READ && status != MBEDTLS_ERR_SSL_WANT_WRITE)
		{
			ALX_SOCKET_TRACE_FORMAT("TLS handshake error: 0x%X\r\n", status);
			sslFree(&me->tls_data);
			return Alx_Err;
		}
	}
	if ((status = mbedtls_ssl_get_verify_result(&me->tls_data.ssl_context)) != 0)
	{
		ALX_SOCKET_TRACE_FORMAT("TLS verify_result error 0x%X\r\n", status);
		sslFree(&me->tls_data);
		return Alx_Err;
	}
	ALX_SOCKET_TRACE_FORMAT("TLS handshake successful\r\n");
	return Alx_Ok;
}
#endif // if defined(MBED_TLS)

//******************************************************************************
// Functions
//******************************************************************************
Alx_Status AlxSocket_Open(AlxSocket* me, AlxNet* alxNet, AlxSocket_Protocol protocol)
{
	UNUSED(alxNet);

	me->alxNet = alxNet;
	me->protocol = protocol;

	// Assert
	ALX_SOCKET_ASSERT(me->wasCtorCalled == true);

	#if defined(ALX_WIZNET)
	if (me->alxNet->config == AlxNet_Config_Wiznet)
	{
		Alx_Status ret = Alx_Ok;
		uint8_t Sn_Protocol;

		if (me->socket_data.wiz_socket != -1)
		{
			return Alx_Err;
		}

		switch (protocol)
		{
		case AlxSocket_Protocol_Udp:
			Sn_Protocol = Sn_MR_UDP;
			break;
		case AlxSocket_Protocol_Tls:
		case AlxSocket_Protocol_Tcp:
			Sn_Protocol = Sn_MR_TCP;
			break;
		default:
			Sn_Protocol = Sn_MR_CLOSE;
			ret = Alx_Err;
			break;
		}

		if (Sn_Protocol != Sn_MR_CLOSE)
		{
			int8_t free_sock = wiz_sock_alloc(Sn_Protocol);
			if (free_sock >= 0)
			{
				me->protocol = protocol;
				me->socket_data.wiz_protocol = Sn_Protocol;
				me->socket_data.wiz_socket = free_sock;
				me->socket_data.my_port = wiz_any_port();
			}
			else
			{
				ret = AlxBound_ErrMax;
			}
		}

		me->isOpened = true;

		return ret;
	}
	#endif

	#if defined(ALX_FREE_RTOS_CELLULAR)
	if (me->alxNet->config == AlxNet_Config_FreeRtos_Cellular)
	{
		int ret = Alx_Ok;
		CellularError_t cellularStatus = CELLULAR_SUCCESS;
		CellularSocketProtocol_t socket_protocol;

		switch (protocol)
		{
		case AlxSocket_Protocol_Udp:
			socket_protocol = CELLULAR_SOCKET_PROTOCOL_UDP;
			break;
		case AlxSocket_Protocol_Tls:
		case AlxSocket_Protocol_Tcp:
			socket_protocol = CELLULAR_SOCKET_PROTOCOL_TCP;
			break;
		default:
			return Alx_Err;
		}

		cellularStatus = Cellular_CreateSocket(me->alxNet->cellular.handle,
												me->alxNet->cellular.cellularContext,
												CELLULAR_SOCKET_DOMAIN_AF_INET,
												CELLULAR_SOCKET_TYPE_DGRAM,
												socket_protocol,
												&me->cellular_socket.socket);
		if (cellularStatus == CELLULAR_SUCCESS)
		{
			me->protocol = protocol;
			me->cellular_socket.protocol = socket_protocol;
		}
		else
		{
			return Alx_Err;
		}

		cellularStatus = Cellular_SocketRegisterSocketOpenCallback(me->alxNet->cellular.handle,
																	me->cellular_socket.socket,
																	&CellularSocketOpenCallback,
																	me);
		if (cellularStatus != CELLULAR_SUCCESS)
		{
			return Alx_Err;
		}

		cellularStatus = Cellular_SocketRegisterClosedCallback(me->alxNet->cellular.handle,
																me->cellular_socket.socket,
																&CellularSocketCloseCallback,
																me);
		if (cellularStatus != CELLULAR_SUCCESS)
		{
			return Alx_Err;
		}

		cellularStatus = Cellular_SocketRegisterDataReadyCallback(me->alxNet->cellular.handle,
																	me->cellular_socket.socket,
																	&CellularSocketDataReadyCallback,
																	me);
		if (cellularStatus != CELLULAR_SUCCESS)
		{
			return Alx_Err;
		}


		me->isOpened = true;

		return ret;
	}
	#endif

	// Return
	return Alx_Err;
}
Alx_Status AlxSocket_Close(AlxSocket* me)
{
	// Assert
	ALX_SOCKET_ASSERT(me->wasCtorCalled == true);

	#if defined(ALX_WIZNET)
	if (me->alxNet->config == AlxNet_Config_Wiznet)
	{
		if (me->socket_data.wiz_socket == -1)
		{
			return Alx_Err;
		}

		wiz_sock_release(me->socket_data.wiz_socket);
		if (me->socket_data.wiz_sock_opened)
		{
			if (me->protocol == AlxSocket_Protocol_Tls)
			{
				while (mbedtls_ssl_close_notify(&me->tls_data.ssl_context) == MBEDTLS_ERR_SSL_WANT_WRITE)
				{
				}
				sslFree(&me->tls_data);
			}
			if ((me->protocol == AlxSocket_Protocol_Tcp) || (me->protocol == AlxSocket_Protocol_Tls))
			{
				if (getSn_SR(me->socket_data.wiz_socket) == SOCK_ESTABLISHED)
				{
					disconnect(me->socket_data.wiz_socket);
				}
				else
				{
					close(me->socket_data.wiz_socket);
				}
			}
			else
			{
				close(me->socket_data.wiz_socket);
			}
			me->socket_data.wiz_sock_opened = false;
		}
		me->socket_data.wiz_socket = -1;
		return Alx_Ok;
	}
	#endif

	#if defined(ALX_FREE_RTOS_CELLULAR)
	if (me->alxNet->config == AlxNet_Config_FreeRtos_Cellular)
	{
		if (me->protocol == AlxSocket_Protocol_Tls)
		{
			while (mbedtls_ssl_close_notify(&me->tls_data.ssl_context) == MBEDTLS_ERR_SSL_WANT_WRITE)
			{
			}
			sslFree(&me->tls_data);
		}
		if (NULL == me->cellular_socket.socket) return Alx_Err;
		CellularError_t cellularStatus = CELLULAR_SUCCESS;
		cellularStatus = Cellular_SocketClose(me->alxNet->cellular.handle, me->cellular_socket.socket);
		if (cellularStatus == CELLULAR_SUCCESS) return Alx_Ok;
	}
	#endif

	// Return
	return Alx_Err;
}
Alx_Status AlxSocket_Connect(AlxSocket* me, const char* ip, uint16_t port)
{
	// Assert
	ALX_SOCKET_ASSERT(me->wasCtorCalled == true);

	if (me->isOpened == false) return Alx_Err;

	#if defined(ALX_WIZNET)
	if (me->alxNet->config == AlxNet_Config_Wiznet)
	{
		if (me->socket_data.wiz_socket == -1)
		{
			return Alx_Err;
		}
		int addr[4];
		sscanf(ip, "%d.%d.%d.%d", &addr[0], &addr[1], &addr[2], &addr[3]);
		me->socket_data.dst_ip[0] = addr[0];
		me->socket_data.dst_ip[1] = addr[1];
		me->socket_data.dst_ip[2] = addr[2];
		me->socket_data.dst_ip[3] = addr[3];
		me->socket_data.dst_port = port;

		if ((me->protocol == AlxSocket_Protocol_Tcp) || (me->protocol == AlxSocket_Protocol_Tls))
		{
			if (socket(me->socket_data.wiz_socket, Sn_MR_TCP, me->socket_data.my_port, 0) != me->socket_data.wiz_socket)
			{
				return Alx_Err;
			}
			if (connect(me->socket_data.wiz_socket, me->socket_data.dst_ip, me->socket_data.dst_port) != SOCK_OK)
			{
				return SOCK_ERROR;
			}
			me->socket_data.wiz_sock_opened = true;
		}
		if (me->protocol == AlxSocket_Protocol_Tls)
		{
			return sslHandshake(me);
		}
		return Alx_Ok;
	}
	#endif

	#if defined(ALX_FREE_RTOS_CELLULAR)
	if (me->alxNet->config == AlxNet_Config_FreeRtos_Cellular)
	{
		if (NULL == me->cellular_socket.socket) return Alx_Err;
		if (NULL == me->alxNet) return Alx_Err;
		if (NULL == ip) return Alx_Err;

		strncpy(me->cellular_socket.sockAddr.ipAddress.ipAddress, ip, CELLULAR_IP_ADDRESS_MAX_SIZE + 1);
		me->cellular_socket.sockAddr.ipAddress.ipAddressType = CELLULAR_IP_ADDRESS_V4;
		me->cellular_socket.sockAddr.port = port;


		CellularError_t cellularStatus = CELLULAR_SUCCESS;
		cellularStatus = Cellular_SocketConnect(me->alxNet->cellular.handle,
												me->cellular_socket.socket,
												CELLULAR_ACCESSMODE_BUFFER,
												&me->cellular_socket.sockAddr);
		if (cellularStatus != CELLULAR_SUCCESS) {
			return Alx_Err;
		}

		EventBits_t socketConnectBits;
		socketConnectBits = xEventGroupWaitBits(me->cellular_socket.event_group, EVENT_BITS_SOCKET_CONNECT, true, false, pdMS_TO_TICKS(me->timeout));
		if (socketConnectBits & EVENT_BITS_SOCKET_CONNECT)
		{
			if (me->cellular_socket.URC_error != CELLULAR_URC_SOCKET_OPENED)
			{
				return Alx_Err;
			}
		}
		else
		{
			// TIMEOUT occured
			return AlxNet_Timeout;
		}

		if (me->protocol == AlxSocket_Protocol_Tls)
		{
			return sslHandshake(me);
		}

		return Alx_Ok;
	}
	#endif

	// Return
	return Alx_Err;
}

Alx_Status AlxSocket_Bind(AlxSocket* me, uint16_t port)
{
	// Assert
	ALX_SOCKET_ASSERT(me->wasCtorCalled == true);

	#if defined(ALX_WIZNET)
	if (me->alxNet->config == AlxNet_Config_Wiznet)
	{
		if (me->socket_data.wiz_socket == -1)
		{
			return Alx_Err;
		}
		me->socket_data.my_port = port;
		return Alx_Ok;
	}
	#endif

	#if defined(ALX_FREE_RTOS_CELLULAR)
	if (me->alxNet->config == AlxNet_Config_FreeRtos_Cellular)
	{
		return AlxNet_NotSupported;
	}
	#endif

	// Return
	return Alx_Err;
}
Alx_Status AlxSocket_Listen(AlxSocket* me, uint8_t backlog)
{
	// Assert
	ALX_SOCKET_ASSERT(me->wasCtorCalled == true);

	#if defined(ALX_WIZNET)
	if (me->alxNet->config == AlxNet_Config_Wiznet)
	{
		if (me->socket_data.wiz_socket == -1)
		{
			return SOCKERR_SOCKNUM;
		}

		switch (me->protocol)
		{
		case AlxSocket_Protocol_Tcp:
		case AlxSocket_Protocol_Tls:
			if (socket(me->socket_data.wiz_socket, Sn_MR_TCP, me->socket_data.my_port, 0) != me->socket_data.wiz_socket)
			{
				return Alx_Err;
			}
			me->socket_data.backlog = backlog;
			return listen(me->socket_data.wiz_socket) == SOCK_OK ? Alx_Ok : Alx_Err;
			break;
		default:
			break;
		}
		return Alx_Ok;
	}
	#endif

	#if defined(ALX_FREE_RTOS_CELLULAR)
	if (me->alxNet->config == AlxNet_Config_FreeRtos_Cellular)
	{
		return AlxNet_NotSupported;
	}
	#endif

	// Return
	return Alx_Err;
}
AlxSocket* AlxSocket_Accept(AlxSocket* me)
{
	// Assert
	ALX_SOCKET_ASSERT(me->wasCtorCalled == true);

	#if defined(ALX_WIZNET)
	if (me->alxNet->config == AlxNet_Config_Wiznet)
	{
		// wait until clent conents to the socket
		uint8_t tmpSn_SR;
		uint64_t start = AlxTick_Get_ms(&alxTick);
		do
		{
			if (AlxTick_Get_ms(&alxTick) - start > me->timeout)
			{
				// timeout
				return NULL;
			}
			tmpSn_SR = getSn_SR(me->socket_data.wiz_socket);
		} while ((tmpSn_SR != SOCK_ESTABLISHED) && (tmpSn_SR != SOCK_CLOSE_WAIT));
		me->socket_data.wiz_sock_opened = true;

		// find available socket for listening fotr new connects
		int8_t available_wiz_socket = wiz_sock_alloc(AlxSocket_Protocol_Tcp);
		if ((available_wiz_socket < 0) || (me->socket_data.backlog == 0))
		{
			// no available sockets, but client has connected to server socket
			return (me);
		}

		// copy all data to server socket
		if (me->socket_data.backlog > 0)
		{
			me->socket_data.backlog--;
		}
		AlxSocket* new_socket = &wiz_server_sockets[available_wiz_socket];
		memcpy(new_socket,
			me,
			sizeof(AlxSocket));
		new_socket->socket_data.wiz_socket = me->socket_data.wiz_socket;
		me->socket_data.wiz_socket = available_wiz_socket;

		// start listening
		if ((me->protocol == AlxSocket_Protocol_Tcp) || (me->protocol == AlxSocket_Protocol_Tls))
		{
			if (socket(me->socket_data.wiz_socket, Sn_MR_TCP, me->socket_data.my_port, 0) != me->socket_data.wiz_socket)
			{
				return NULL;
			}
			listen(me->socket_data.wiz_socket);
			return new_socket;
		}
		return NULL;
	}
	#endif

	#if defined(ALX_FREE_RTOS_CELLULAR)
	if (me->alxNet->config == AlxNet_Config_FreeRtos_Cellular)
	{
		return NULL;
	}
	#endif

	// Return
	return NULL;
}
int32_t AlxSocket_Send(AlxSocket* me, void* data, uint32_t len)
{
	// Assert
	ALX_SOCKET_ASSERT(me->wasCtorCalled == true);

	#if defined(ALX_WIZNET)
	if (me->alxNet->config == AlxNet_Config_Wiznet)
	{
		if (me->socket_data.wiz_socket == -1)
		{
			return SOCKERR_SOCKNUM;
		}

		switch (me->protocol)
		{
		case AlxSocket_Protocol_Tls:
			{
				return mbedtls_ssl_write(&me->tls_data.ssl_context, data, len);
				break;
			}
		case AlxSocket_Protocol_Tcp:
			{
				return wiz_send(me, data, len);
				break;
			}
		case AlxSocket_Protocol_Udp:
			{
				if (getSn_SR(me->socket_data.wiz_socket) != SOCK_UDP)
				{
					socket(me->socket_data.wiz_socket, Sn_MR_UDP, me->socket_data.my_port, 0x00);
					me->socket_data.wiz_sock_opened = true;
				}

				uint32_t sent_total = 0;
				uint64_t timer_start = AlxTick_Get_ms(&alxTick);
				while (1)
				{
					int sent_chunk = sendto(me->socket_data.wiz_socket, data + sent_total, len - sent_total, me->socket_data.dst_ip, me->socket_data.dst_port);
					if (sent_chunk <= 0)
					{
						return sent_chunk;
					}
					sent_total += sent_chunk;
					if (sent_total >= len)
					{
						break;
					}
					if (AlxTick_Get_ms(&alxTick) - timer_start >= me->timeout)
					{
						break;
					}
					SOCKET_YIELD();
				}
				return sent_total;
				break;
			}
			return sendto(me->socket_data.wiz_socket, data, len, me->socket_data.dst_ip, me->socket_data.dst_port);
			break;
		default:
			break;
		}
		return SOCK_ERROR;
	}
	#endif

	#if defined(ALX_FREE_RTOS_CELLULAR)
	if (me->alxNet->config == AlxNet_Config_FreeRtos_Cellular)
	{
		if (NULL == me->cellular_socket.socket) return 0;

		switch (me->protocol)
		{
		case AlxSocket_Protocol_Tls:
			return mbedtls_ssl_write(&me->tls_data.ssl_context, data, len);
			break;
		case AlxSocket_Protocol_Tcp:
		case AlxSocket_Protocol_Udp:
			return cell_send(me, data, len);
			break;
		default:
			break;
		}
	}
	#endif

	// Return
	return Alx_Err;
}
int32_t AlxSocket_Recv(AlxSocket* me, void* data, uint32_t len)
{
	// Assert
	ALX_SOCKET_ASSERT(me->wasCtorCalled == true);

	#if defined(ALX_WIZNET)
	if (me->alxNet->config == AlxNet_Config_Wiznet)
	{
		if (me->socket_data.wiz_socket == -1)
		{
			return SOCKERR_SOCKNUM;
		}

		switch (me->protocol)
		{
		case AlxSocket_Protocol_Tls:
			{
				return mbedtls_ssl_read(&me->tls_data.ssl_context, data, len);
				break;
			}
		case AlxSocket_Protocol_Tcp:
			{
				return wiz_recv(me, data, len);
				break;
			}
		case AlxSocket_Protocol_Udp:
			{
				// recvfrom should be a separate function, additionally returning srv_ip and srv_port
				if (getSn_SR(me->socket_data.wiz_socket) != SOCK_UDP)
				{
					socket(me->socket_data.wiz_socket, Sn_MR_UDP, me->socket_data.my_port, 0x00);
					me->socket_data.wiz_sock_opened = true;
				}

				uint8_t srv_ip[4];
				uint16_t srv_port;
				uint32_t received_total = 0;
				uint64_t timer_start = AlxTick_Get_ms(&alxTick);
				while (1)
				{
					if (getSn_RX_RSR(me->socket_data.wiz_socket) > 0)
					{
						int received_chunk = recvfrom(me->socket_data.wiz_socket, data + received_total, len - received_total, srv_ip, &srv_port);
						if (received_chunk <= 0)
						{
							return received_chunk;
						}
						received_total += received_chunk;
					}
					if (received_total >= len)
					{
						break;
					}
					if ((AlxTick_Get_ms(&alxTick) - timer_start >= me->timeout) || (received_total > 0))
					{
						break;
					}
					SOCKET_YIELD();
				}
				return received_total;
				break;
			}
		default:
			break;
		}
		return SOCK_ERROR;
	}
	#endif

	#if defined(ALX_FREE_RTOS_CELLULAR)
	if (me->alxNet->config == AlxNet_Config_FreeRtos_Cellular)
	{
		switch (me->protocol)
		{
		case AlxSocket_Protocol_Tls:
			return mbedtls_ssl_read(&me->tls_data.ssl_context, data, len);
			break;
		case AlxSocket_Protocol_Tcp:
		case AlxSocket_Protocol_Udp:
			return cell_recv(me, data, len);
			break;
		default:
			break;
		}

	}
	#endif

	// Return
	return Alx_Err;
}
void AlxSocket_SetTimeout_ms(AlxSocket* me, uint32_t timeout_ms)
{
	// Assert
	ALX_SOCKET_ASSERT(me->wasCtorCalled == true);

	me->timeout = timeout_ms;
}

#if defined(ALX_MBEDTLS)
Alx_Status AlxSocket_InitTls(AlxSocket* me, const char *server_cn, const unsigned char *ca_cert, const unsigned char *cl_cert, const unsigned char *cl_key)
{
	mbedtls_x509_crt_init(&me->tls_data.x509_ca_certificate);
	me->tls_data.init_state = SSL_INITIALIZED_CACERT;
	if (mbedtls_x509_crt_parse(&me->tls_data.x509_ca_certificate, ca_cert, strlen((const char *)ca_cert) + 1) != 0)
	{
		ALX_SOCKET_TRACE_FORMAT("Failed to parse CA certificate\r\n");
		sslFree(&me->tls_data);
		return Alx_Err;
	}
	mbedtls_x509_crt_init(&me->tls_data.x509_cl_certificate);
	me->tls_data.init_state = SSL_INITIALIZED_CLCERT;
	if (mbedtls_x509_crt_parse(&me->tls_data.x509_cl_certificate, cl_cert, strlen((const char *)cl_cert) + 1) != 0)
	{
		ALX_SOCKET_TRACE_FORMAT("Failed to parse CL certificate\r\n");
		sslFree(&me->tls_data);
		return Alx_Err;
	}
	mbedtls_pk_init(&me->tls_data.pkey);
	me->tls_data.init_state = SSL_INITIALIZED_CLKEY;
	if (mbedtls_pk_parse_key(&me->tls_data.pkey, cl_key, strlen((const char *)cl_key) + 1, NULL, 0, NULL, NULL) != 0)
	{
		ALX_SOCKET_TRACE_FORMAT("Failed to parse CL private key\r\n");
		sslFree(&me->tls_data);
		return Alx_Err;
	}

	mbedtls_ctr_drbg_init(&me->tls_data.drbg_context);
	me->tls_data.init_state = SSL_INITIALIZED_DRBG_CONTEXT;

	mbedtls_ssl_config_init(&me->tls_data.ssl_config);
	me->tls_data.init_state = SSL_INITIALIZED_SSL_CONFIG;
	if (mbedtls_ssl_config_defaults(&me->tls_data.ssl_config,
		MBEDTLS_SSL_IS_CLIENT,
		MBEDTLS_SSL_TRANSPORT_STREAM,
		MBEDTLS_SSL_PRESET_DEFAULT) != 0)
	{
		ALX_SOCKET_TRACE_FORMAT("Failed to load default SSL config\r\n");
		sslFree(&me->tls_data);
		return Alx_Err;
	}

	// use TLS 1.2 only
	mbedtls_ssl_conf_max_version(&me->tls_data.ssl_config, MBEDTLS_SSL_MAJOR_VERSION_3, MBEDTLS_SSL_MINOR_VERSION_3);
	mbedtls_ssl_conf_min_version(&me->tls_data.ssl_config, MBEDTLS_SSL_MAJOR_VERSION_3, MBEDTLS_SSL_MINOR_VERSION_3);

	// Load CA certificate
	mbedtls_ssl_conf_ca_chain(&me->tls_data.ssl_config, &me->tls_data.x509_ca_certificate, NULL);

	// Set Client certificate and private key
	if (mbedtls_ssl_conf_own_cert(&me->tls_data.ssl_config, &me->tls_data.x509_cl_certificate, &me->tls_data.pkey) != 0)
	{
		ALX_SOCKET_TRACE_FORMAT("Failed to config own certificate\r\n");
		sslFree(&me->tls_data);
		return Alx_Err;
	}

	// Strictly ensure that certificates are signed by the CA
	mbedtls_ssl_conf_authmode(&me->tls_data.ssl_config, MBEDTLS_SSL_VERIFY_REQUIRED);
	mbedtls_ssl_conf_rng(&me->tls_data.ssl_config, sslRandomCallback, &me->tls_data.drbg_context);

	mbedtls_ssl_init(&me->tls_data.ssl_context);
	me->tls_data.init_state = SSL_INITIALIZED_SSL_CONTEXT;
	if (mbedtls_ssl_setup(&me->tls_data.ssl_context, &me->tls_data.ssl_config) != 0)
	{
		ALX_SOCKET_TRACE_FORMAT("Failed to setup SSL context\r\n");
		sslFree(&me->tls_data);
		return Alx_Err;
	}

	mbedtls_ssl_set_bio(&me->tls_data.ssl_context, me, sslSend, sslRecv, NULL);

	if (mbedtls_ssl_set_hostname(&me->tls_data.ssl_context, server_cn) != 0)
	{
		ALX_SOCKET_TRACE_FORMAT("Failed to set hostname\n");
		sslFree(&me->tls_data);
		return Alx_Err;
	}

	return Alx_Ok;
}
#endif

//******************************************************************************
// Private Functions
//******************************************************************************


#endif	// #if defined(ALX_C_LIB) && (defined(ALX_FREE_RTOS_CELLULAR) || defined(ALX_WIZNET))
