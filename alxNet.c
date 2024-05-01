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
	AlxNet* me,
	AlxNet_Config config
)
{
	// Parameters
	me->config = config;

	// Variables

	// Info
	me->wasCtorCalled = true;
	me->isInit = false;
}


//******************************************************************************
// Functions
//******************************************************************************
Alx_Status AlxNet_Connect(AlxNet* me)
{
	// Assert
	ALX_NET_ASSERT(me->wasCtorCalled == true);

	// https://github.com/Wiznet/ioLibrary_Driver/blob/master/Ethernet/W5500/w5500.h
	// TODO detailed analysis
	// https://github.com/WIZnet-MbedEthernet/WIZnetInterface/blob/master/WIZnetInterface.h
	// virtual int connect();
	// https://github.com/Wiznet/ioLibrary_Driver/blob/master/Internet/DHCP/dhcp.h
	// TODO detailed analysis

	// https://github.com/ARMmbed/mbed-os/blob/master/connectivity/netsocket/include/netsocket/NetworkInterface.h
	// virtual nsapi_error_t connect() = 0;

	// https://github.com/lwip-tcpip/lwip/blob/master/src/include/lwip/netif.h
	// struct netif *netif_add(struct netif *netif, const ip4_addr_t *ipaddr, const ip4_addr_t *netmask, const ip4_addr_t *gw, void *state, netif_init_fn init, netif_input_fn input);
	// void netif_set_default(struct netif *netif);
	// https://github.com/lwip-tcpip/lwip/blob/master/src/include/lwip/dhcp.h
	// err_t dhcp_start(struct netif *netif);
	// u8_t dhcp_supplied_address(const struct netif *netif);

	// NOTE: If DHCP enabled, it must also trigger handling of DHCP client

	// Return
	return Alx_Ok;
}
Alx_Status AlxNet_Disconnect(AlxNet* me)
{
	// Assert
	ALX_NET_ASSERT(me->wasCtorCalled == true);

	// https://github.com/Wiznet/ioLibrary_Driver/blob/master/Ethernet/W5500/w5500.h
	// TODO detailed analysis
	// https://github.com/WIZnet-MbedEthernet/WIZnetInterface/blob/master/WIZnetInterface.h
	// virtual int disconnect();
	// https://github.com/Wiznet/ioLibrary_Driver/blob/master/Internet/DHCP/dhcp.h
	// TODO detailed analysis

	// https://github.com/ARMmbed/mbed-os/blob/master/connectivity/netsocket/include/netsocket/NetworkInterface.h
	// virtual nsapi_error_t disconnect() = 0;

	// https://github.com/lwip-tcpip/lwip/blob/master/src/include/lwip/netif.h
	// TODO detailed analysis
	// https://github.com/lwip-tcpip/lwip/blob/master/src/include/lwip/dhcp.h
	// TODO detailed analysis

	// Return
	return Alx_Ok;
}
bool AlxNet_IsConnected(AlxNet* me)
{
	// Assert
	ALX_NET_ASSERT(me->wasCtorCalled == true);

	// https://github.com/Wiznet/ioLibrary_Driver/blob/master/Ethernet/W5500/w5500.h
	// TODO detailed analysis
	// https://github.com/WIZnet-MbedEthernet/WIZnetInterface/blob/master/WIZnetInterface.h
	// Doesn't have it

	// https://github.com/ARMmbed/mbed-os/blob/master/connectivity/netsocket/include/netsocket/NetworkInterface.h
	// virtual nsapi_connection_status_t get_connection_status() const;

	// https://github.com/lwip-tcpip/lwip/blob/master/src/include/lwip/netif.h
	// #define netif_is_up(netif) (((netif)->flags & NETIF_FLAG_UP) ? (u8_t)1 : (u8_t)0)

	// Return
	return true;
}
void AlxNet_SetMac(AlxNet* me, const char* mac)
{
	// Assert
	ALX_NET_ASSERT(me->wasCtorCalled == true);

	// https://github.com/Wiznet/ioLibrary_Driver/blob/master/Ethernet/W5500/w5500.h
	// TODO detailed analysis
	// https://github.com/WIZnet-MbedEthernet/WIZnetInterface/blob/master/WIZnetInterface.h
	// Doesn't have it

	// https://github.com/ARMmbed/mbed-os/blob/master/connectivity/netsocket/include/netsocket/NetworkInterface.h
	//	set_mac_address
	//	set_ip_address
	//	set_network(ip, netmask, gateway)
	//	get_mac_address
	//	get_ip_address
	//	get_netmask
	//	get_gateway

	// https://github.com/lwip-tcpip/lwip/blob/master/src/include/lwip/netif.h
	// netifStruct.hwaddr[0]
	// ip_addr_set_zero_ip4
	// ip4_addr1_val
	// ip4_addr2_val
	// ip4_addr3_val
	// ip4_addr4_val
	// netif_ip4_addr
	// netif_ip4_netmask
	// netif_ip4_gw

	// MAC in string format -> "00:18:10:3A:B8:39"
	// IP, Netmask, gateway in string format -> "123.123.123.123"
}
void AlxNet_SetIp(AlxNet* me, const char* ip)
{
	// Assert
	ALX_NET_ASSERT(me->wasCtorCalled == true);
}
void AlxNet_SetNetmask(AlxNet* me, const char* netmask)
{
	// Assert
	ALX_NET_ASSERT(me->wasCtorCalled == true);
}
void AlxNet_SetGateway(AlxNet* me, const char* gateway)
{
	// Assert
	ALX_NET_ASSERT(me->wasCtorCalled == true);
}
const char* AlxNet_GetMac(AlxNet* me)
{
	// Assert
	ALX_NET_ASSERT(me->wasCtorCalled == true);

	// Return
	return 0;
}
const char* AlxNet_GetIp(AlxNet* me)
{
	// Assert
	ALX_NET_ASSERT(me->wasCtorCalled == true);

	// Return
	return 0;
}
const char* AlxNet_GetNetmask(AlxNet* me)
{
	// Assert
	ALX_NET_ASSERT(me->wasCtorCalled == true);

	// Return
	return 0;
}
const char* AlxNet_GetGateway(AlxNet* me)
{
	// Assert
	ALX_NET_ASSERT(me->wasCtorCalled == true);

	// Return
	return 0;
}
void AlxNet_Dns_SetIp(AlxNet* me, uint8_t dnsId, const char* ip)
{
	// Assert
	ALX_NET_ASSERT(me->wasCtorCalled == true);

	// https://github.com/Wiznet/ioLibrary_Driver/blob/master/Internet/DNS/dns.h
	// int8_t DNS_run(uint8_t * dns_ip, uint8_t * name, uint8_t * ip_from_dns);
	// https://github.com/WIZnet-MbedEthernet/WIZnetInterface/blob/master/WIZnetInterface.h
	// bool setDnsServerIP(const char* ip_address);

	// https://github.com/ARMmbed/mbed-os/blob/master/connectivity/netsocket/include/netsocket/NetworkInterface.h
	// virtual nsapi_error_t add_dns_server(const SocketAddress &address, const char *interface_name);

	// https://github.com/lwip-tcpip/lwip/blob/master/src/include/lwip/dns.h
	// dns_setserver(u8_t numdns, const ip_addr_t *dnsserver);

	// NOTE: Seems like we will need to handle array of DNS IPs, lets start with array len max = 3
}
Alx_Status AlxNet_Dns_GetHostByName(AlxNet* me, const char* hostname, char* ip)
{
	// Assert
	ALX_NET_ASSERT(me->wasCtorCalled == true);

	// https://github.com/Wiznet/ioLibrary_Driver/blob/master/Internet/DNS/dns.h
	// int8_t DNS_run(uint8_t * dns_ip, uint8_t * name, uint8_t * ip_from_dns);
	// https://github.com/WIZnet-MbedEthernet/WIZnetInterface/blob/master/WIZnetInterface.h
	// virtual nsapi_error_t gethostbyname(const char *host, SocketAddress *address, nsapi_version_t version = NSAPI_UNSPEC, const char *interface_name = NULL);

	// https://github.com/ARMmbed/mbed-os/blob/master/connectivity/netsocket/include/netsocket/NetworkInterface.h
	// virtual nsapi_error_t gethostbyname(const char *host, SocketAddress *address, nsapi_version_t version = NSAPI_UNSPEC, const char *interface_name = NULL);

	// https://github.com/lwip-tcpip/lwip/blob/master/src/include/lwip/dns.h
	// err_t dns_gethostbyname(const char *hostname, ip_addr_t *addr, dns_found_callback found, void *callback_arg);

	// Return
	return Alx_Ok;
}
void AlxNet_Dhcp_Enable(AlxNet* me, bool enable)
{
	// Assert
	ALX_NET_ASSERT(me->wasCtorCalled == true);

	// https://github.com/Wiznet/ioLibrary_Driver/blob/master/Internet/DHCP/dhcp.h
	// TODO detailed analysis
	// https://github.com/WIZnet-MbedEthernet/WIZnetInterface/blob/master/WIZnetInterface.h
	// It can't be directly enabled, but can be enabled/disabled via specific functions

	// https://github.com/ARMmbed/mbed-os/blob/master/connectivity/netsocket/include/netsocket/NetworkInterface.h
	// virtual nsapi_error_t set_dhcp(bool dhcp);

	// https://github.com/lwip-tcpip/lwip/blob/master/src/include/lwip/dhcp.h
	// TODO detailed analysis
}
bool AlxNet_Dhcp_WasAddrSupplied(AlxNet* me)
{
	// Assert
	ALX_NET_ASSERT(me->wasCtorCalled == true);

	// https://github.com/Wiznet/ioLibrary_Driver/blob/master/Internet/DHCP/dhcp.h
	// TODO detailed analysis
	// https://github.com/WIZnet-MbedEthernet/WIZnetInterface/blob/master/WIZnetInterface.h
	// Doesn't have it

	// https://github.com/ARMmbed/mbed-os/blob/master/connectivity/netsocket/include/netsocket/NetworkInterface.h
	// Doesn't have it

	// https://github.com/lwip-tcpip/lwip/blob/master/src/include/lwip/dhcp.h
	// u8_t dhcp_supplied_address(const struct netif *netif);

	// Return
	return true;
}


//******************************************************************************
// Private Functions
//******************************************************************************


#endif	// #if defined(ALX_C_LIB)
