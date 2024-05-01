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
	me->alxNet = NULL;
	me->protocol = AlxSocket_Protocol_Undefined;

	// Info
	me->wasCtorCalled = true;
	me->isOpened = false;
}


//******************************************************************************
// Functions
//******************************************************************************
Alx_Status AlxSocket_Open(AlxSocket* me, AlxNet* alxNet, AlxSocket_Protocol protocol)
{
	// Assert
	ALX_SOCKET_ASSERT(me->wasCtorCalled == true);

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
	return Alx_Ok;
}
Alx_Status AlxSocket_Close(AlxSocket* me)
{
	// Assert
	ALX_SOCKET_ASSERT(me->wasCtorCalled == true);

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

	// https://github.com/Wiznet/ioLibrary_Driver/blob/master/Ethernet/socket.h
	// int32_t send(uint8_t sn, uint8_t * buf, uint16_t len);
	// https://github.com/WIZnet-MbedEthernet/WIZnetInterface/blob/master/WIZnetInterface.h
	// virtual nsapi_size_or_error_t socket_send(nsapi_socket_t handle, const void *data, nsapi_size_t size);

	// https://github.com/ARMmbed/mbed-os/blob/master/connectivity/netsocket/include/netsocket/Socket.h
	// virtual nsapi_size_or_error_t send(const void *data, nsapi_size_t size) = 0;

	// https://github.com/lwip-tcpip/lwip/blob/master/src/include/lwip/sockets.h
	// ssize_t lwip_send(int s, const void *dataptr, size_t size, int flags);

	// Return
	return Alx_Ok;
}
int32_t AlxSocket_Recv(AlxSocket* me, void* data, uint32_t len)
{
	// Assert
	ALX_SOCKET_ASSERT(me->wasCtorCalled == true);

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
