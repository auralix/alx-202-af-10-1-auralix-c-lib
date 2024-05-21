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
#include "alxTick.h"

#include "socket.h"
#include "w5500.h"

//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB)

//******************************************************************************
// Private Defines
//******************************************************************************
#define SOCKET_ACCEPT_TIMEOUT   60000

//******************************************************************************
// Static Wiznet Variables
//******************************************************************************

static uint8_t wiz_socks[_WIZCHIP_SOCK_NUM_] = { Sn_MR_CLOSE, };
static AlxOsMutex alxSocketAllocMutex;

static AlxSocket wiz_server_sockets[_WIZCHIP_SOCK_NUM_]; // memory fror new sockets when accepting connections

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
	me->socket_data.wiz_socket = -1;
	if (!alxSocketAllocMutex.wasCtorCalled)
	{
		AlxOsMutex_Ctor(&alxSocketAllocMutex);
	}
	
	// Info
	me->wasCtorCalled = true;
	me->isOpened = false;
}

//******************************************************************************
// Static Wiznet Functions
//******************************************************************************
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

//******************************************************************************
// Functions
//******************************************************************************
Alx_Status AlxSocket_Open(AlxSocket* me, AlxNet* alxNet, AlxSocket_Protocol protocol)
{
	UNUSED(alxNet);
	
	// Assert
	ALX_SOCKET_ASSERT(me->wasCtorCalled == true);

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
		
	// https://github.com/Wiznet/ioLibrary_Driver/blob/master/Ethernet/socket.h
	// int8_t  socket(uint8_t sn, uint8_t protocol, uint16_t port, uint8_t flag);
	// https://github.com/WIZnet-MbedEthernet/WIZnetInterface/blob/master/WIZnetInterface.h
	// virtual nsapi_error_t socket_open(nsapi_socket_t *handle, nsapi_protocol_t proto);

	// https://github.com/ARMmbed/mbed-os/blob/master/connectivity/netsocket/include/netsocket/Socket.h
	// Doesn't have it
	// https://github.com/ARMmbed/mbed-os/blob/master/connectivity/netsocket/include/netsocket/InternetSocket.h
	// nsapi_error_t open(NetworkStack *stack);

	// https://github.com/lwip-tcpip/lwip/blob/master/src/include/lwip/sockets.h
	// int lwip_socket(int domain, int type, int protocol);

	// Return
	return ret;
}
Alx_Status AlxSocket_Close(AlxSocket* me)
{
	// Assert
	ALX_SOCKET_ASSERT(me->wasCtorCalled == true);

	if (me->socket_data.wiz_socket == -1)
	{
		return Alx_Err;
	}
	
	wiz_sock_release(me->socket_data.wiz_socket);
	if (me->socket_data.wiz_sock_opened)
	{
		close(me->socket_data.wiz_socket);
	}
	me->socket_data.wiz_socket = -1;
	
	// https://github.com/Wiznet/ioLibrary_Driver/blob/master/Ethernet/socket.h
	// int8_t  disconnect(uint8_t sn); -> First FIN packet for TCP
	// int8_t  close(uint8_t sn); -> Then standard close
	// https://github.com/WIZnet-MbedEthernet/WIZnetInterface/blob/master/WIZnetInterface.h
	// virtual nsapi_error_t socket_close(nsapi_socket_t handle);

	// https://github.com/ARMmbed/mbed-os/blob/master/connectivity/netsocket/include/netsocket/Socket.h
	// virtual nsapi_error_t close() = 0;

	// https://github.com/lwip-tcpip/lwip/blob/master/src/include/lwip/sockets.h
	// lwip_close(s)

	// Return
	return Alx_Ok;
}
Alx_Status AlxSocket_Connect(AlxSocket* me, const char* ip, uint16_t port)
{
	// Assert
	ALX_SOCKET_ASSERT(me->wasCtorCalled == true);

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
	
	//socket(me->socket_data.wiz_socket, me->socket_data.wiz_protocol, 0, 0)
	// https://github.com/Wiznet/ioLibrary_Driver/blob/master/Ethernet/socket.h
	// int8_t  connect(uint8_t sn, uint8_t * addr, uint16_t port);
	// https://github.com/WIZnet-MbedEthernet/WIZnetInterface/blob/master/WIZnetInterface.h
	// virtual nsapi_error_t socket_connect(nsapi_socket_t handle, const SocketAddress &address);

	// https://github.com/ARMmbed/mbed-os/blob/master/connectivity/netsocket/include/netsocket/Socket.h
	// virtual nsapi_error_t connect(const SocketAddress &address) = 0;

	// https://github.com/lwip-tcpip/lwip/blob/master/src/include/lwip/sockets.h
	// int lwip_connect(int s, const struct sockaddr *name, socklen_t namelen);

	// Return
	return Alx_Ok;
}
Alx_Status AlxSocket_Bind(AlxSocket* me, uint16_t port)
{
	// Assert
	ALX_SOCKET_ASSERT(me->wasCtorCalled == true);

	if (me->socket_data.wiz_socket == -1)
	{
		return Alx_Err;
	}
	me->socket_data.my_port = port;
	// https://github.com/Wiznet/ioLibrary_Driver/blob/master/Ethernet/socket.h
	// * There are @b bind() and @b accept() functions in @b Berkeley SOCKET API but, not in @b WIZnet SOCKET API. Because socket() of WIZnet is not only creating a SOCKET but also binding a local port number, ...
	// https://github.com/WIZnet-MbedEthernet/WIZnetInterface/blob/master/WIZnetInterface.h
	// virtual nsapi_error_t socket_bind(nsapi_socket_t handle, const SocketAddress &address);

	// https://github.com/ARMmbed/mbed-os/blob/master/connectivity/netsocket/include/netsocket/Socket.h
	// virtual nsapi_error_t bind(const SocketAddress &address) = 0;
	// https://github.com/ARMmbed/mbed-os/blob/master/connectivity/netsocket/include/netsocket/InternetSocket.h
	// nsapi_error_t bind(uint16_t port);

	// https://github.com/lwip-tcpip/lwip/blob/master/src/include/lwip/sockets.h
	// int lwip_bind(int s, const struct sockaddr *name, socklen_t namelen);

	// Return
	return Alx_Ok;
}
Alx_Status AlxSocket_Listen(AlxSocket* me, uint8_t backlog)
{
	// Assert
	ALX_SOCKET_ASSERT(me->wasCtorCalled == true);
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
		return listen(me->socket_data.wiz_socket);
		break;
	default:
		break;
	}
	
	// https://github.com/Wiznet/ioLibrary_Driver/blob/master/Ethernet/socket.h
	// int8_t  listen(uint8_t sn);
	// https://github.com/WIZnet-MbedEthernet/WIZnetInterface/blob/master/WIZnetInterface.h
	// virtual nsapi_error_t socket_listen(nsapi_socket_t handle, int backlog);

	// https://github.com/ARMmbed/mbed-os/blob/master/connectivity/netsocket/include/netsocket/Socket.h
	// virtual nsapi_error_t listen(int backlog = 1) = 0;

	// https://github.com/lwip-tcpip/lwip/blob/master/src/include/lwip/sockets.h
	// int lwip_listen(int s, int backlog);

	// Return
	return Alx_Ok;
}
AlxSocket* AlxSocket_Accept(AlxSocket* me)
{
	// Assert
	ALX_SOCKET_ASSERT(me->wasCtorCalled == true);
	
	// wait until clent conents to the socket
	uint8_t tmpSn_SR;
	uint64_t start = AlxTick_Get_ms(&alxTick);
	do
	{
		if (AlxTick_Get_ms(&alxTick) - start > SOCKET_ACCEPT_TIMEOUT)
		{
			// timeout
			return NULL;
		}
		tmpSn_SR = getSn_SR(me->socket_data.wiz_socket);
	} while ((tmpSn_SR != SOCK_ESTABLISHED) && (tmpSn_SR != SOCK_CLOSE_WAIT));

	// find available socket for listening fotr new connects
	int8_t available_wiz_socket = wiz_sock_alloc(AlxSocket_Protocol_Tcp);
	if ((available_wiz_socket < 0) || (me->socket_data.backlog == 0))
	{
		// no available sockets, but client has connected to server socket
		return (me);
	}
	
	// copy all data to server socket
	me->socket_data.backlog--;
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
	// https://github.com/Wiznet/ioLibrary_Driver/blob/master/Ethernet/socket.h
	// * There are @b bind() and @b accept() functions in @b Berkeley SOCKET API but, not in @b WIZnet SOCKET API. Because socket() of WIZnet is not only creating a SOCKET but also binding a local port number, ...
	// https://github.com/WIZnet-MbedEthernet/WIZnetInterface/blob/master/WIZnetInterface.h
	// virtual nsapi_error_t socket_accept(nsapi_socket_t server, nsapi_socket_t *handle, SocketAddress *address=0);

	// https://github.com/ARMmbed/mbed-os/blob/master/connectivity/netsocket/include/netsocket/Socket.h
	// virtual Socket *accept(nsapi_error_t *error = NULL) = 0;

	// https://github.com/lwip-tcpip/lwip/blob/master/src/include/lwip/sockets.h
	// int lwip_accept(int s, struct sockaddr *addr, socklen_t *addrlen);

	// Return
	return 0;
}
int32_t AlxSocket_Send(AlxSocket* me, void* data, uint32_t len)
{
	// Assert
	ALX_SOCKET_ASSERT(me->wasCtorCalled == true);
	if (me->socket_data.wiz_socket == -1)
	{
		return SOCKERR_SOCKNUM;
	}
	
	switch(me->protocol)
	{
	case AlxSocket_Protocol_Tcp:
	case AlxSocket_Protocol_Tls:
		if (getSn_SR(me->socket_data.wiz_socket) == SOCK_ESTABLISHED)
		{
			return send(me->socket_data.wiz_socket, data, len);
		}
		return SOCK_ERROR;
		break;
	case AlxSocket_Protocol_Udp:
		if (getSn_SR(me->socket_data.wiz_socket) != SOCK_UDP)
		{
			socket(me->socket_data.wiz_socket, Sn_MR_UDP, me->socket_data.my_port, 0x00);
			me->socket_data.wiz_sock_opened = true;
		}
		return sendto(me->socket_data.wiz_socket, data, len, me->socket_data.dst_ip, me->socket_data.dst_port);
		break;
	default:
		break;
	}
	
	
	// https://github.com/Wiznet/ioLibrary_Driver/blob/master/Ethernet/socket.h
	// int32_t send(uint8_t sn, uint8_t * buf, uint16_t len);
	// https://github.com/WIZnet-MbedEthernet/WIZnetInterface/blob/master/WIZnetInterface.h
	// virtual nsapi_size_or_error_t socket_send(nsapi_socket_t handle, const void *data, nsapi_size_t size);

	// https://github.com/ARMmbed/mbed-os/blob/master/connectivity/netsocket/include/netsocket/Socket.h
	// virtual nsapi_size_or_error_t send(const void *data, nsapi_size_t size) = 0;

	// https://github.com/lwip-tcpip/lwip/blob/master/src/include/lwip/sockets.h
	// ssize_t lwip_send(int s, const void *dataptr, size_t size, int flags);

	// Return
	return SOCK_ERROR;
}
int32_t AlxSocket_Recv(AlxSocket* me, void* data, uint32_t len)
{
	// Assert
	ALX_SOCKET_ASSERT(me->wasCtorCalled == true);
	if (me->socket_data.wiz_socket == -1)
		{
			return SOCKERR_SOCKNUM;
		}
	
	switch (me->protocol)
	{
	case AlxSocket_Protocol_Tcp:
	case AlxSocket_Protocol_Tls:
		if ((getSn_SR(me->socket_data.wiz_socket) == SOCK_ESTABLISHED) && (getSn_RX_RSR(me->socket_data.wiz_socket) > 0))
		{
			return recv(me->socket_data.wiz_socket, data, len);
		}
		break;
	case AlxSocket_Protocol_Udp:
		// recvfrom should be a separate function, additionally returning srv_ip and srv_port
		if (getSn_SR(me->socket_data.wiz_socket) != SOCK_UDP)
		{
			socket(me->socket_data.wiz_socket, Sn_MR_UDP, me->socket_data.my_port, 0x00);
			me->socket_data.wiz_sock_opened = true;
		}
		uint8_t srv_ip[4];
		uint16_t srv_port;
		return recvfrom(me->socket_data.wiz_socket, data, len, srv_ip, &srv_port);
		break;
	default:
		break;
	}

	return SOCK_ERROR;
	
	
	// https://github.com/Wiznet/ioLibrary_Driver/blob/master/Ethernet/socket.h
	// int32_t recv(uint8_t sn, uint8_t * buf, uint16_t len);
	// https://github.com/WIZnet-MbedEthernet/WIZnetInterface/blob/master/WIZnetInterface.h
	// virtual nsapi_size_or_error_t socket_recv(nsapi_socket_t handle, void *data, nsapi_size_t size);

	// https://github.com/ARMmbed/mbed-os/blob/master/connectivity/netsocket/include/netsocket/Socket.h
	// virtual nsapi_size_or_error_t recv(void *data, nsapi_size_t size) = 0;

	// https://github.com/lwip-tcpip/lwip/blob/master/src/include/lwip/sockets.h
	// ssize_t lwip_recv(int s, void *mem, size_t len, int flags);

	// Return
	return Alx_Ok;
}
void AlxSocket_SetTimeout_ms(AlxSocket* me)
{
	// Assert
	ALX_SOCKET_ASSERT(me->wasCtorCalled == true);

	// https://github.com/Wiznet/ioLibrary_Driver/blob/master/Ethernet/socket.h
	// Seems like we need to implemented it by out own
	// https://github.com/WIZnet-MbedEthernet/WIZnetInterface/blob/master/WIZnetInterface.h
	// Seems like we need to implemented it by out own

	// https://github.com/ARMmbed/mbed-os/blob/master/connectivity/netsocket/include/netsocket/Socket.h
	// virtual void set_timeout(int timeout) = 0;

	// https://github.com/lwip-tcpip/lwip/blob/master/src/include/lwip/sockets.h
	// int lwip_setsockopt (int s, int level, int optname, const void *optval, socklen_t optlen);
	//	// Set clientSock timeout
	//	struct timeval timeout;
	//	timeout.tv_sec = CLIENT_SOCK_TIMEOUT_sec;
	//	timeout.tv_usec = 0;
	//	sockStatusOrLen = setsockopt(clientSock, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));
	//	if (sockStatusOrLen < 0)
	//	{
	//		HCSS_MAIN_TRACE("Err: %d", sockStatusOrLen);
	//		HCSS_MAIN_ASSERT(false);
	//	}
	//	sockStatusOrLen = setsockopt(clientSock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
	//	if (sockStatusOrLen < 0)
	//	{
	//		HCSS_MAIN_TRACE("Err: %d", sockStatusOrLen);
	//		HCSS_MAIN_ASSERT(false);
	//	}
}


//******************************************************************************
// Private Functions
//******************************************************************************


#endif	// #if defined(ALX_C_LIB)
