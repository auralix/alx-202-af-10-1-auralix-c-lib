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
#include "alxSpi.h"
#include "alxOsMutex.h"
#include "alxTick.h"
#include "alxOsDelay.h"
#include "alxOsThread.h"
#include "alxSocket.h"

#include "socket.h"
#include "dhcp.h"
#include "dns.h"

//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB)

//******************************************************************************
// Defines
//******************************************************************************
#define CONNECT_TIMEOUT 5000

#define THREAD_STACK_SIZE_WORDS 2048
#define THREAD_PRIORITY 3

#define DNS_RETRY_COUNT 1 // 3 sec.

#define WIZ_BUFFER_SIZE 2048
//******************************************************************************
// Private Variables
//******************************************************************************
static AlxOsMutex alxSpiMutex;
static AlxSpi *alxSpi;
static bool instance_guard = false;

static wiz_NetInfo wiz_net_info =
{
	.mac = { 0x00, 0x08, 0xDC, 0x12, 0x34, 0x56 },
	.ip = { 192, 168, 11, 2 },
	.sn = { 255, 255, 255, 0 },
	.gw = { 192, 168, 11, 1 },
	.dns = { 8, 8, 8, 8 },
	.dhcp = NETINFO_DHCP // DHCP enable/disable
};
static uint8_t wiz_buffer[WIZ_BUFFER_SIZE];
static uint64_t dhcp_ip_leased_until = 0;
static AlxOsMutex alxDhcpMutex;
static bool dhcp_isRunning = false;

static uint8_t dns_target_domain[256];
static uint8_t dns_response_ip[4];
static DnsTaskState dns_retval;
static AlxOsMutex alxDnsMutex;

AlxSocket alxDhcpSocket, alxDnsSocket;

//******************************************************************************
// Private Functions
//******************************************************************************
// Helpers
void ip2str(const uint8_t *ip, char *ipstr)
{
	sprintf(ipstr, "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
}
void str2ip(const char *ipstr, uint8_t *ip)
{
	int a[4];
	sscanf(ipstr, "%d.%d.%d.%d", &a[0], &a[1], &a[2], &a[3]);
	ip[0] = a[0];
	ip[1] = a[1];
	ip[2] = a[2];
	ip[3] = a[3];
}
void mac2str(const uint8_t *mac, char *macstr)
{
	sprintf(macstr, "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
}
void str2mac(const char *macstr, uint8_t *mac)
{
	int m[6];
	sscanf(macstr, "%02X:%02X:%02X:%02X:%02X:%02X", &m[0], &m[1], &m[2], &m[3], &m[4], &m[5]);
	mac[0] = m[0];
	mac[1] = m[1];
	mac[2] = m[2];
	mac[3] = m[3];
	mac[4] = m[4];
	mac[5] = m[5];
}

// ioLibrary Callbacks
static void CrisEnter(void)
{
	AlxOsMutex_Lock(&alxSpiMutex);
}
static void CrisExit(void)
{
	AlxOsMutex_Unlock(&alxSpiMutex);
}
static void Select(void) {
	AlxIoPin_Reset(alxSpi->do_nCS);
}
static void Unselect(void) {
	AlxIoPin_Set(alxSpi->do_nCS);
}
static void ReadBurst(uint8_t* buff, uint16_t len) {
	ALX_NET_ASSERT(AlxSpi_Master_Read(alxSpi, buff, len, 2, 2000) == Alx_Ok);
}
static void WriteBurst(uint8_t* buff, uint16_t len) {
	ALX_NET_ASSERT(AlxSpi_Master_Write(alxSpi, buff, len, 2, 2000) == Alx_Ok);
}
static uint8_t ReadByte(void) {
	uint8_t byte;
	ReadBurst(&byte, sizeof(byte));
	return byte;
}
static void WriteByte(uint8_t byte) {
	WriteBurst(&byte, sizeof(byte));
}

static void print_network_information(wiz_NetInfo net_info)
{
	uint8_t tmp_str[8] = {
		0,
	};

	ctlnetwork(CN_GET_NETINFO, (void *)&net_info);
	ctlwizchip(CW_GET_ID, (void *)tmp_str);

	if (net_info.dhcp == NETINFO_DHCP)
	{
		ALX_TRACE_FORMAT("==========================================\r\n");
		ALX_TRACE_FORMAT(" %s network configuration : DHCP\r\n\n", (char *)tmp_str);
	}
	else
	{
		ALX_TRACE_FORMAT("==========================================\r\n");
		ALX_TRACE_FORMAT(" %s network configuration : static\r\n\n", (char *)tmp_str);
	}

	ALX_TRACE_FORMAT(" MAC         : %02X:%02X:%02X:%02X:%02X:%02X\r\n", net_info.mac[0], net_info.mac[1], net_info.mac[2], net_info.mac[3], net_info.mac[4], net_info.mac[5]);
	ALX_TRACE_FORMAT(" IP          : %d.%d.%d.%d\r\n", net_info.ip[0], net_info.ip[1], net_info.ip[2], net_info.ip[3]);
	ALX_TRACE_FORMAT(" Subnet Mask : %d.%d.%d.%d\r\n", net_info.sn[0], net_info.sn[1], net_info.sn[2], net_info.sn[3]);
	ALX_TRACE_FORMAT(" Gateway     : %d.%d.%d.%d\r\n", net_info.gw[0], net_info.gw[1], net_info.gw[2], net_info.gw[3]);
	ALX_TRACE_FORMAT(" DNS         : %d.%d.%d.%d\r\n", net_info.dns[0], net_info.dns[1], net_info.dns[2], net_info.dns[3]);
	ALX_TRACE_FORMAT("==========================================\r\n");
}

// DHCP callbacks and task
static void wizchip_dhcp_assign(void)
{
	getIPfromDHCP(wiz_net_info.ip);
	getGWfromDHCP(wiz_net_info.gw);
	getSNfromDHCP(wiz_net_info.sn);
	getDNSfromDHCP(wiz_net_info.dns);
	wiz_net_info.dhcp = NETINFO_DHCP;

	/* Network initialize */
	ctlnetwork(CN_SET_NETINFO, &wiz_net_info);
	print_network_information(wiz_net_info);
	ALX_TRACE_FORMAT("DHCP leased time : %ld seconds\r\n", getDHCPLeasetime());
}
static void wizchip_dhcp_conflict(void)
{
	ALX_TRACE_FORMAT("Conflict IP from DHCP\r\n");

	// halt or reset or any...
	while (1)
	{
		AlxOsDelay_ms(&alxOsDelay, 1000 * 1000);
	}
}
static void dhcp_task(void *argument)
{
	AlxNet* me = (AlxNet*) argument;
	int retval = 0;
	uint8_t link;
	uint32_t dhcp_retry = 0;
	dhcp_mode state = NETINFO_STATIC;

	reg_dhcp_cbfunc(wizchip_dhcp_assign, wizchip_dhcp_assign, wizchip_dhcp_conflict);
	while (1)
	{
		dhcp_isRunning = false;
		AlxOsMutex_Lock(&alxDhcpMutex); // blocks if DHCP is disabled
		// open a ssocket to get socket number only
		// DHCP is handled by ioLibrary
		dhcp_isRunning = true;
		if (AlxSocket_Open(&alxDhcpSocket, me, AlxSocket_Protocol_Udp) == Alx_Ok)
		{
			while (1)
			{
				if (me->isNetConnected)
				{
					if ((state == NETINFO_STATIC) && me->enable_dhcp)
					{
						ALX_TRACE_FORMAT("DHCP client started\r\n");
						DHCP_init(alxDhcpSocket.socket_data.wiz_socket, wiz_buffer);
						dhcp_ip_leased_until = 0;
						state = NETINFO_DHCP;
					}

					if ((state == NETINFO_DHCP) && !me->enable_dhcp)
					{
						ALX_TRACE_FORMAT("DHCP client stopped\r\n");
						DHCP_stop();
						state = NETINFO_STATIC;
						break; // break to mutex
					}

					if (state == NETINFO_DHCP)
					{
						link = wizphy_getphylink();
						if (link == PHY_LINK_OFF)
						{
							ALX_TRACE_FORMAT("PHY_LINK_OFF\r\n");
							DHCP_stop();
							while (1)
							{
								link = wizphy_getphylink();
								if (link == PHY_LINK_ON)
								{
									DHCP_init(alxDhcpSocket.socket_data.wiz_socket, wiz_buffer);
									dhcp_retry = 0;
									break;
								}
								AlxOsDelay_ms(&alxOsDelay, 1000);
							}
						}

						retval = DHCP_run();
						if (retval == DHCP_IP_LEASED)
						{
							if (dhcp_ip_leased_until == 0)
							{
								dhcp_retry = 0;
								ALX_TRACE_FORMAT("DHCP success\r\n");
								dhcp_ip_leased_until = AlxTick_Get_sec(&alxTick) + getDHCPLeasetime();
								ip2str(wiz_net_info.dns, me->dns[0]);
							}
						}
						else if (retval == DHCP_FAILED)
						{
							dhcp_ip_leased_until = 0;
							ALX_TRACE_FORMAT("DHCP timeout occurred, retry: %d\r\n", dhcp_retry);
						}
					}
				}
				AlxOsDelay_ms(&alxOsDelay, 1000);
			}
			AlxSocket_Close(&alxDhcpSocket); // close the socket
		}
	}
}

// DNS task
static void dns_task(void *argument)
{
	AlxNet* me = (AlxNet*) argument;
	uint8_t dns_ip[4];

	while (1)
	{
		AlxOsMutex_Lock(&alxDnsMutex);
		str2ip(me->dns[0], dns_ip);
		if (AlxSocket_Open(&alxDnsSocket, me, AlxSocket_Protocol_Udp) == Alx_Ok)
		{
			dns_retval = DnsTaskRunning;
			DNS_init(alxDnsSocket.socket_data.wiz_socket, wiz_buffer);
			int dns_retry = 0;
			int counter;
			while (1)
			{
				if (DNS_run(dns_ip, dns_target_domain, dns_response_ip) > 0)
				{
					ALX_TRACE_FORMAT("DNS success\r\n");
					ALX_TRACE_FORMAT("Target domain : %s\r\n", dns_target_domain);
					ALX_TRACE_FORMAT("IP of target domain : %d.%d.%d.%d\r\n", dns_response_ip[0], dns_response_ip[1], dns_response_ip[2], dns_response_ip[3]);
					dns_retval = DnsTaskSuccess;
					break;
				}
				else
				{
					dns_retry++;
					if (dns_retry <= DNS_RETRY_COUNT)
					{
						ALX_TRACE_FORMAT("DNS timeout occurred and retry %d\r\n", dns_retry);
					}
				}

				if (dns_retry > DNS_RETRY_COUNT)
				{
					ALX_TRACE_FORMAT("DNS failed\r\n");
					dns_retval = DnsTaskTimeout;
					break;
				}

				AlxOsDelay_ms(&alxOsDelay, 10);
				counter++;
				if (counter == 100)
				{
					counter = 0;
					DNS_time_handler();
				}
			}
			AlxSocket_Close(&alxDnsSocket);
		}
	}
}

// 1 sec tick task
static void tick_task(void *argument)
{
	UNUSED(argument);
	while (1)
	{
		DNS_time_handler();
		DHCP_time_handler();
		AlxOsDelay_ms(&alxOsDelay, 1000);
	}
}

//******************************************************************************
// Constructor
//******************************************************************************
void AlxNet_Ctor
(
	AlxNet* me,
	AlxNet_Config config,
	AlxSpi* alxSpi,
	AlxIoPin* do_nRST,
	AlxIoPin* di_nINT
)
{
	// Parameters
	me->config = config;
	me->alxSpi = alxSpi;
	me->do_nRST = do_nRST;
	me->di_nINT = di_nINT;
	// Variables
	AlxOsMutex_Ctor(&me->alxMutex);

	// Info
	me->wasCtorCalled = true;
}


//******************************************************************************
// Functions
//******************************************************************************
Alx_Status AlxNet_Init(AlxNet *me)
{
	AlxOsMutex_Lock(&me->alxMutex);
	if (instance_guard == false)
	{
		// only one instance allowed
		if (instance_guard)
		{
			return Alx_Err;
		}
		instance_guard = true;
		AlxOsMutex_Unlock(&me->alxMutex);

		// Set Callbacks and SPI for ioLibrary_Driver
		reg_wizchip_cris_cbfunc(CrisEnter, CrisExit);
		reg_wizchip_cs_cbfunc(Select, Unselect);
		reg_wizchip_spi_cbfunc(ReadByte, WriteByte);
		reg_wizchip_spiburst_cbfunc(ReadBurst, WriteBurst);

		// Init SPI
		AlxOsMutex_Ctor(&alxSpiMutex);
		alxSpi = me->alxSpi;

		AlxNet_Disconnect(me);

		if (AlxSpi_Init(me->alxSpi) != Alx_Ok)
		{
			return Alx_Err;
		}

		// start DHCP
		AlxOsMutex_Ctor(&alxDhcpMutex);
		AlxOsMutex_Lock(&alxDhcpMutex);
		AlxSocket_Ctor(&alxDhcpSocket);
		AlxOsThread dhcp_thread;
		AlxOsThread_Ctor(&dhcp_thread,
			dhcp_task,
			"DHCP_Task",
			THREAD_STACK_SIZE_WORDS * 4,
			(void *)me,
			THREAD_PRIORITY);
		AlxOsThread_Start(&dhcp_thread);

		// start DNS
		AlxOsMutex_Ctor(&alxDnsMutex);
		AlxOsMutex_Lock(&alxDnsMutex);
		AlxSocket_Ctor(&alxDnsSocket);
		AlxOsThread dns_thread;
		AlxOsThread_Ctor(&dns_thread,
			dns_task,
			"DNS_Task",
			THREAD_STACK_SIZE_WORDS * 4,
			(void *)me,
			THREAD_PRIORITY);
		AlxOsThread_Start(&dns_thread);

		// start 1 sec tick
		AlxOsThread tick_thread;
		AlxOsThread_Ctor(&tick_thread,
			tick_task,
			"TICK_Task",
			THREAD_STACK_SIZE_WORDS * 4,
			(void *)me,
			THREAD_PRIORITY);
		AlxOsThread_Start(&tick_thread);

		AlxOsMutex_Unlock(&me->alxMutex);
		return Alx_Ok;
	}
	AlxOsMutex_Unlock(&me->alxMutex);
	return Alx_Err;
}

Alx_Status AlxNet_Connect(AlxNet* me)
{
	// Assert
	ALX_NET_ASSERT(me->wasCtorCalled == true);

	AlxOsMutex_Lock(&me->alxMutex);
	uint8_t temp;
	uint32_t start = AlxTick_Get_ms(&alxTick);
	do
	{
		if (ctlwizchip(CW_GET_PHYLINK, (void *)&temp) == -1)
		{
			ALX_NET_TRACE_FORMAT("Unknown PHY link status\r\n");
			AlxOsMutex_Unlock(&me->alxMutex);
			return Alx_Err;
		}
		if (AlxTick_Get_ms(&alxTick) - start > CONNECT_TIMEOUT)
		{
			AlxOsMutex_Unlock(&me->alxMutex);
			return Alx_ErrNumOfTries;
		}
	} while (temp == PHY_LINK_OFF);

	uint8_t rx_tx_buff_sizes[] = {2, 2, 2, 2, 2, 2, 2, 2};
	if (wizchip_init(rx_tx_buff_sizes, rx_tx_buff_sizes) != 0)
	{
		AlxOsMutex_Unlock(&me->alxMutex);
		return Alx_Err;
	}

	me->isNetConnected = true;
	AlxOsMutex_Unlock(&me->alxMutex);

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

	AlxOsMutex_Lock(&me->alxMutex);
	AlxIoPin_Reset(me->do_nRST); // Reset W5500
	AlxDelay_ms(10);
	AlxIoPin_Set(me->do_nRST);
	AlxDelay_ms(10);

	memset(me->mac, 0, sizeof(me->mac));
	memset(me->ip, 0, sizeof(me->ip));
	memset(me->netmask, 0, sizeof(me->netmask));
	memset(me->gateway, 0, sizeof(me->gateway));
	memset(me->mac, 0, sizeof(me->mac));
	memset(me->dns, 0, sizeof(me->dns));

	me->isNetConnected = false;
	AlxOsMutex_Unlock(&me->alxMutex);
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
	return me->isNetConnected;
}
void AlxNet_SetMac(AlxNet* me, const char* mac)
{
	// Assert
	ALX_NET_ASSERT(me->wasCtorCalled == true);
	ALX_NET_ASSERT(strlen(mac) == 17);

	AlxOsMutex_Lock(&me->alxMutex);
	str2mac(mac, wiz_net_info.mac);
	strcpy(me->mac, mac);
	setSHAR(wiz_net_info.mac);
	AlxOsMutex_Unlock(&me->alxMutex);

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
	ALX_NET_ASSERT(strlen(ip) < sizeof(me->ip));

	AlxOsMutex_Lock(&me->alxMutex);
	strcpy(me->ip, ip);
	uint8_t addr[4];
	str2ip(ip, addr);
	setSIPR(addr);
	AlxOsMutex_Unlock(&me->alxMutex);
}
void AlxNet_SetNetmask(AlxNet* me, const char* netmask)
{
	// Assert
	ALX_NET_ASSERT(me->wasCtorCalled == true);
	ALX_NET_ASSERT(strlen(netmask) < sizeof(me->netmask));

	AlxOsMutex_Lock(&me->alxMutex);
	strcpy(me->netmask, netmask);
	uint8_t addr[4];
	str2ip(netmask, addr);
	setSUBR(addr);
	AlxOsMutex_Unlock(&me->alxMutex);
}
void AlxNet_SetGateway(AlxNet* me, const char* gateway)
{
	// Assert
	ALX_NET_ASSERT(me->wasCtorCalled == true);
	ALX_NET_ASSERT(strlen(gateway) < sizeof(me->gateway));

	AlxOsMutex_Lock(&me->alxMutex);
	strcpy(me->gateway, gateway);
	uint8_t addr[4];
	str2ip(gateway, addr);
	setGAR(addr);
	AlxOsMutex_Unlock(&me->alxMutex);
}
const char* AlxNet_GetMac(AlxNet* me)
{
	// Assert
	ALX_NET_ASSERT(me->wasCtorCalled == true);

	uint8_t bin_mac[6];
	AlxOsMutex_Lock(&me->alxMutex);
	getSHAR(bin_mac);
	mac2str(bin_mac, me->mac);
	AlxOsMutex_Unlock(&me->alxMutex);

	// Return
	return me->mac;
}
const char* AlxNet_GetIp(AlxNet* me)
{
	// Assert
	ALX_NET_ASSERT(me->wasCtorCalled == true);

	uint8_t addr[4];
	AlxOsMutex_Lock(&me->alxMutex);
	getSIPR(addr);
	ip2str(addr, me->ip);
	AlxOsMutex_Unlock(&me->alxMutex);

	// Return
	return me->ip;
}
const char* AlxNet_GetNetmask(AlxNet* me)
{
	// Assert
	ALX_NET_ASSERT(me->wasCtorCalled == true);

	uint8_t addr[4];
	AlxOsMutex_Lock(&me->alxMutex);
	getSUBR(addr);
	ip2str(addr, me->netmask);
	AlxOsMutex_Unlock(&me->alxMutex);

	// Return
	return me->netmask;
}
const char* AlxNet_GetGateway(AlxNet* me)
{
	// Assert
	ALX_NET_ASSERT(me->wasCtorCalled == true);

	uint8_t addr[4];
	AlxOsMutex_Lock(&me->alxMutex);
	getGAR(addr);
	ip2str(addr, me->gateway);
	AlxOsMutex_Unlock(&me->alxMutex);

	// Return
	return me->gateway;
}
void AlxNet_Dns_SetIp(AlxNet* me, uint8_t dnsId, const char* ip)
{
	// Assert
	ALX_NET_ASSERT(me->wasCtorCalled == true);
	ALX_NET_ASSERT(dnsId < 4);
	ALX_NET_ASSERT(strlen(ip) < sizeof(me->ip));

	AlxOsMutex_Lock(&me->alxMutex);
	strcpy(me->dns[dnsId], ip);
	AlxOsMutex_Unlock(&me->alxMutex);

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
	ALX_NET_ASSERT(strlen(hostname) < sizeof(dns_target_domain));

	AlxOsMutex_Lock(&me->alxMutex);
	strcpy((char *)dns_target_domain, hostname);
	dns_retval = DnsTaskRunning;
	AlxOsMutex_Unlock(&alxDnsMutex);

	while (dns_retval == DnsTaskRunning)
	{
		// timeout in DNS task
	}

	switch (dns_retval)
	{
	case DnsTaskSuccess:
		ip2str(dns_response_ip, ip);
		AlxOsMutex_Unlock(&me->alxMutex);
		return Alx_Ok;
	case DnsTaskTimeout:
		AlxOsMutex_Unlock(&me->alxMutex);
		return Alx_ErrNumOfTries;
	default:
		break;
	}
	AlxOsMutex_Unlock(&me->alxMutex);

	// https://github.com/Wiznet/ioLibrary_Driver/blob/master/Internet/DNS/dns.h
	// int8_t DNS_run(uint8_t * dns_ip, uint8_t * name, uint8_t * ip_from_dns);
	// https://github.com/WIZnet-MbedEthernet/WIZnetInterface/blob/master/WIZnetInterface.h
	// virtual nsapi_error_t gethostbyname(const char *host, SocketAddress *address, nsapi_version_t version = NSAPI_UNSPEC, const char *interface_name = NULL);

	// https://github.com/ARMmbed/mbed-os/blob/master/connectivity/netsocket/include/netsocket/NetworkInterface.h
	// virtual nsapi_error_t gethostbyname(const char *host, SocketAddress *address, nsapi_version_t version = NSAPI_UNSPEC, const char *interface_name = NULL);

	// https://github.com/lwip-tcpip/lwip/blob/master/src/include/lwip/dns.h
	// err_t dns_gethostbyname(const char *hostname, ip_addr_t *addr, dns_found_callback found, void *callback_arg);

	// Return
	return Alx_Err;
}
void AlxNet_Dhcp_Enable(AlxNet* me, bool enable)
{
	// Assert
	ALX_NET_ASSERT(me->wasCtorCalled == true);

	AlxOsMutex_Lock(&me->alxMutex);
	if (enable)
	{
		AlxOsMutex_Unlock(&alxDhcpMutex);
		me->enable_dhcp = true;
		while (!dhcp_isRunning)
		{
			AlxOsDelay_ms(&alxOsDelay, 1);
		}
	}
	else
	{
		me->enable_dhcp = false;
		while (dhcp_isRunning)
		{
			AlxOsDelay_ms(&alxOsDelay, 1);
		}
	}
	AlxOsMutex_Unlock(&me->alxMutex);

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
	return (AlxTick_Get_sec(&alxTick) < dhcp_ip_leased_until);
}


//******************************************************************************
// Private Functions
//******************************************************************************


#endif	// #if defined(ALX_C_LIB)
