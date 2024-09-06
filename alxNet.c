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
#if defined(ALX_WIZNET)
#include "socket.h"
#include "dhcp.h"
#include "dns.h"
#endif


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB) && (defined(ALX_FREE_RTOS_CELLULAR) || defined(ALX_WIZNET))


//******************************************************************************
// Defines
//******************************************************************************
#define CONNECT_TIMEOUT 5000

#define THREAD_STACK_SIZE_WORDS 512
#define THREAD_PRIORITY 1

#define DNS_RETRY_COUNT 1 // 3 sec.

#define WIZ_BUFFER_SIZE 2048

#if defined(ALX_FREE_RTOS_CELLULAR)
#ifndef CELLULAR_APN_NAME
#error "CELLULAR_APN_NAME is not defined in cellular_config.h"
#endif
#define CELLULAR_SIM_CARD_WAIT_INTERVAL_MS       ( 500UL )
#define CELLULAR_MAX_SIM_RETRY                   ( 5U )
#define CELLULAR_PDN_CONNECT_WAIT_INTERVAL_MS    ( 1000UL )

#endif
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
static uint8_t wiz_buffer[WIZ_BUFFER_SIZE] __ALIGNED(4);
static uint64_t dhcp_ip_leased_until = 0;
static AlxOsMutex alxDhcpMutex;
static bool dhcp_isRunning = false;

static uint8_t dns_target_domain[256];
static uint8_t dns_response_ip[4];
static DnsTaskState dns_retval;
static AlxOsMutex alxDnsMutex;

AlxSocket alxDhcpSocket, alxDnsSocket;

#if defined(ALX_FREE_RTOS_CELLULAR)
extern CellularCommInterface_t CellularCommInterface;
#endif

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
		ALX_TRACE_FORMAT(" %s network configuration : DHCP\r\n", (char *)tmp_str);
	}
	else
	{
		ALX_TRACE_FORMAT("==========================================\r\n");
		ALX_TRACE_FORMAT(" %s network configuration : static\r\n", (char *)tmp_str);
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
//	print_network_information(wiz_net_info);
//	ALX_TRACE_FORMAT("DHCP leased time : %ld seconds\r\n", getDHCPLeasetime());
}
static void wizchip_dhcp_conflict(void)
{
	ALX_TRACE_FORMAT("Conflict IP from DHCP\r\n");

	// halt or reset or any...
	while (1)
	{
		AlxOsDelay_ms(&alxOsDelay, 1000 * 1000);
		//ALX_NET_ASSERT(false);
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
//						ALX_TRACE_FORMAT("DHCP client started\r\n");
						DHCP_init(alxDhcpSocket.socket_data.wiz_socket, wiz_buffer);
						dhcp_ip_leased_until = 0;
						state = NETINFO_DHCP;
					}

					if ((state == NETINFO_DHCP) && !me->enable_dhcp)
					{
//						ALX_TRACE_FORMAT("DHCP client stopped\r\n");
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
//								ALX_TRACE_FORMAT("DHCP success\r\n");
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

// debug info
static struct
{
	uint8_t sr[8]; // socket status regs - getSn_SR
	uint8_t link; // wizphy_getphylink();
	wiz_PhyConf phyconf; //wizphy_getphyconf(&phyconf)
} dbg_wiz_staus[2];

static void dbg_get(void)
{
	memcpy(&dbg_wiz_staus[0], &dbg_wiz_staus[1], sizeof(dbg_wiz_staus[0]));
	for (int i = 0; i < 8; i++)
	{
		dbg_wiz_staus[1].sr[i] = getSn_SR(i);
	}
	dbg_wiz_staus[1].link = wizphy_getphylink();
	wizphy_getphyconf(&dbg_wiz_staus[1].phyconf);
}

static void dbg_dump(void)
{
	ALX_TRACE_FORMAT("--- WIZNET STATUS ----\r\n");
	ALX_TRACE_FORMAT("SR[i]: %02x->%02x %02x->%02x %02x->%02x %02x->%02x %02x->%02x %02x->%02x %02x->%02x %02x->%02x\r\n",
		dbg_wiz_staus[0].sr[0],
		dbg_wiz_staus[1].sr[0],
		dbg_wiz_staus[0].sr[1],
		dbg_wiz_staus[1].sr[1],
		dbg_wiz_staus[0].sr[2],
		dbg_wiz_staus[1].sr[2],
		dbg_wiz_staus[0].sr[3],
		dbg_wiz_staus[1].sr[3],
		dbg_wiz_staus[0].sr[4],
		dbg_wiz_staus[1].sr[4],
		dbg_wiz_staus[0].sr[5],
		dbg_wiz_staus[1].sr[5],
		dbg_wiz_staus[0].sr[6],
		dbg_wiz_staus[1].sr[6],
		dbg_wiz_staus[0].sr[7],
		dbg_wiz_staus[1].sr[7]);
	ALX_TRACE_FORMAT("PhyLink: %02x->%02x\r\n", dbg_wiz_staus[0].link, dbg_wiz_staus[1].link);
	ALX_TRACE_FORMAT("PhyConf: mode:%02x->%02x duplex:%02x->%02x speed:%02x->%02x\r\n",
		dbg_wiz_staus[0].phyconf.mode,
		dbg_wiz_staus[1].phyconf.mode,
		dbg_wiz_staus[0].phyconf.duplex,
		dbg_wiz_staus[1].phyconf.duplex,
		dbg_wiz_staus[0].phyconf.speed,
		dbg_wiz_staus[1].phyconf.speed);
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
//					ALX_TRACE_FORMAT("DNS success\r\n");
//					ALX_TRACE_FORMAT("Target domain : %s\r\n", dns_target_domain);
//					ALX_TRACE_FORMAT("IP of target domain : %d.%d.%d.%d\r\n", dns_response_ip[0], dns_response_ip[1], dns_response_ip[2], dns_response_ip[3]);
					dns_retval = DnsTaskSuccess;
					dbg_get();
					break;
				}
				else
				{
					dns_retry++;
					if (dns_retry <= DNS_RETRY_COUNT)
					{
						ALX_TRACE_FORMAT("DNS timeout occurred, retry %d\r\n", dns_retry);
						dbg_get();
						dbg_dump();
					}
				}

				if (dns_retry > DNS_RETRY_COUNT)
				{
					ALX_TRACE_FORMAT("DNS failed for %s\r\n", dns_target_domain);
					dns_retval = DnsTaskTimeout;
					dbg_get();
					dbg_dump();
					wizphy_reset();
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
#if defined(ALX_FREE_RTOS_CELLULAR)
static void cellular_info_task(void *argument)
{
	AlxNet* me = (AlxNet*) argument;
	while (1)
	{
		Cellular_GetInfo(me->cellular.handle, &me->cellular.signalQuality) ;
		//ALX_TRACE_FORMAT("CELLULAR RSSI: %d\r\n", signalInfo.rssi);
		AlxOsDelay_ms(&alxOsDelay, 1000);
	}

}
#endif

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

	#if defined(ALX_FREE_RTOS_CELLULAR)
	me->cellular.CommIntf = &CellularCommInterface;
	#endif

	// Info
	me->wasCtorCalled = true;
}


//******************************************************************************
// Functions
//******************************************************************************
Alx_Status AlxNet_Init(AlxNet *me)
{
	#if defined(ALX_WIZNET)
	if (me->config == AlxNet_Config_Wiznet)
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
	#endif

	// Initialize CELLULAR stack
	#if defined(ALX_FREE_RTOS_CELLULAR)
	if (me->config == AlxNet_Config_FreeRtos_Cellular)
	{
		CellularError_t cellularStatus = CELLULAR_SUCCESS;
		uint8_t tries = 0;

		uint32_t timeoutCountLimit = (60000 / CELLULAR_PDN_CONNECT_WAIT_INTERVAL_MS) + 1U;
		uint32_t timeoutCount = 0;

		cellularStatus = Cellular_Init(&me->cellular.handle, me->cellular.CommIntf);
		if (cellularStatus != CELLULAR_SUCCESS) {
			ALX_NET_TRACE_FORMAT("Cellular_Init failure %d\r\n", cellularStatus);
		}
		else {
			/* wait until SIM is ready */
			for (tries = 0; tries < CELLULAR_MAX_SIM_RETRY; tries++) {
				cellularStatus = Cellular_GetSimCardStatus(me->cellular.handle, &me->cellular.simStatus);

				if ((cellularStatus == CELLULAR_SUCCESS) &&
				    ((me->cellular.simStatus.simCardState == CELLULAR_SIM_CARD_INSERTED) &&
				      (me->cellular.simStatus.simCardLockState == CELLULAR_SIM_CARD_READY))) {
					break;
				}
				else {
					ALX_NET_TRACE_FORMAT("Cellular SIM card state %d, Lock State %d <<<\r\n",
						me->cellular.simStatus.simCardState,
						me->cellular.simStatus.simCardLockState);
				}

				vTaskDelay(pdMS_TO_TICKS(CELLULAR_SIM_CARD_WAIT_INTERVAL_MS));
			}

			if (cellularStatus != CELLULAR_SUCCESS) {
				ALX_NET_TRACE_FORMAT(("Cellular SIM failure\r\n"));
			}
		}

		/* Rescan network. */
		if (cellularStatus == CELLULAR_SUCCESS) {
			cellularStatus = Cellular_RfOff(me->cellular.handle);

			if (cellularStatus != CELLULAR_SUCCESS) {
				ALX_NET_TRACE_FORMAT("Cellular_RfOff failure %d\r\n", cellularStatus);
			}
		}

		if (cellularStatus == CELLULAR_SUCCESS) {
			cellularStatus = Cellular_RfOn(me->cellular.handle);

			if (cellularStatus != CELLULAR_SUCCESS) {
				ALX_NET_TRACE_FORMAT("Cellular_RfOn failure %d\r\n", cellularStatus);
			}
		}

		/* Get service status. */
		if (cellularStatus == CELLULAR_SUCCESS) {
			while (timeoutCount < timeoutCountLimit) {
				cellularStatus = Cellular_GetServiceStatus(me->cellular.handle, &me->cellular.serviceStatus);

				if ((cellularStatus == CELLULAR_SUCCESS) &&
				    ((me->cellular.serviceStatus.psRegistrationStatus == REGISTRATION_STATUS_REGISTERED_HOME) ||
				      (me->cellular.serviceStatus.psRegistrationStatus == REGISTRATION_STATUS_ROAMING_REGISTERED))) {
					break;
				}

				timeoutCount++;

				if (timeoutCount >= timeoutCountLimit) {
					ALX_NET_TRACE_FORMAT("Cellular module can't be registered\r\n");
				}

				vTaskDelay(pdMS_TO_TICKS(CELLULAR_PDN_CONNECT_WAIT_INTERVAL_MS));
			}
		}

		// Create a task that will monitor cellular diagnostics
		AlxOsThread cellular_info_thread;
		AlxOsThread_Ctor(&cellular_info_thread,
			cellular_info_task,
			"Cellular_info_task",
			1024,
			(void *)me,
			THREAD_PRIORITY);
		AlxOsThread_Start(&cellular_info_thread);

		if(cellularStatus != CELLULAR_SUCCESS) {
			return Alx_Err;
		}
	}
	#endif

	return Alx_Ok;
}

Alx_Status AlxNet_Connect(AlxNet* me)
{
	// Assert
	ALX_NET_ASSERT(me->wasCtorCalled == true);

	#if defined(ALX_WIZNET)
	if (me->config == AlxNet_Config_Wiznet)
	{
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

		uint8_t rx_buff_sizes[] = { 2, 2, 2, 2, 2, 2, 2, 2 };
		uint8_t tx_buff_sizes[] = { 2, 2, 2, 2, 2, 2, 2, 2 };
		if (wizchip_init(tx_buff_sizes, rx_buff_sizes) != 0)
		{
			AlxOsMutex_Unlock(&me->alxMutex);
			return Alx_Err;
		}

		me->isNetConnected = true;
		AlxOsMutex_Unlock(&me->alxMutex);
	}
	#endif

	// Connect CELLULAR to PDN (Packet Data Network)
	#if defined(ALX_FREE_RTOS_CELLULAR)
	if (me->config == AlxNet_Config_FreeRtos_Cellular)
	{
		AlxOsMutex_Lock(&me->alxMutex);

		CellularError_t cellularStatus = CELLULAR_SUCCESS;
		me->cellular.cellularContext = CELLULAR_PDN_CONTEXT_ID;
		uint8_t NumStatus = 0;
		bool pdnStatus = false;

		CellularPdnConfig_t pdnConfig = { CELLULAR_PDN_CONTEXT_IPV4, CELLULAR_PDN_AUTH_NONE, CELLULAR_APN_NAME, "", "" };
		CellularPdnStatus_t PdnStatusBuffers[CELLULAR_PDN_CONTEXT_ID_MAX - CELLULAR_PDN_CONTEXT_ID_MIN + 1U] = { 0 };

		/* Setup the PDN config. */
		if (cellularStatus == CELLULAR_SUCCESS) {
			cellularStatus = Cellular_SetPdnConfig(me->cellular.handle, me->cellular.cellularContext, &pdnConfig);

			if (cellularStatus != CELLULAR_SUCCESS) {
				ALX_NET_TRACE_FORMAT("Cellular_SetPdnConfig failure %d\r\n", cellularStatus);
			}
		}
		if (cellularStatus == CELLULAR_SUCCESS) {
			cellularStatus = Cellular_ActivatePdn(me->cellular.handle, me->cellular.cellularContext);

			if (cellularStatus != CELLULAR_SUCCESS) {
				ALX_NET_TRACE_FORMAT("Cellular_ActivatePdn failure %d\r\n", cellularStatus);
			}
		}

		if (cellularStatus == CELLULAR_SUCCESS) {
			cellularStatus = Cellular_GetIPAddress(me->cellular.handle, me->cellular.cellularContext, me->ip, sizeof(me->ip));
			if (cellularStatus != CELLULAR_SUCCESS)
			{
				ALX_NET_TRACE_FORMAT("Cellular_GetIPAddress failure %d\r\n", cellularStatus);
			}
		}

		if (cellularStatus == CELLULAR_SUCCESS) {
			cellularStatus = Cellular_GetPdnStatus(me->cellular.handle, PdnStatusBuffers, (CELLULAR_PDN_CONTEXT_ID_MAX - CELLULAR_PDN_CONTEXT_ID_MIN + 1U), &NumStatus);
			if (cellularStatus != CELLULAR_SUCCESS) {
				ALX_NET_TRACE_FORMAT("Cellular_GetPdnStatus failure %d\r\n", cellularStatus);
			}
		}

		if (cellularStatus == CELLULAR_SUCCESS) {
			for (int i = 0U; i < NumStatus; i++) {
				if ((PdnStatusBuffers[i].contextId == me->cellular.cellularContext) && (PdnStatusBuffers[i].state == 1)) {
					pdnStatus = true;
					break;
				}
			}

			if (pdnStatus == false) {
				ALX_NET_TRACE_FORMAT("Cellular PDN is not activated <<<");
			}
		}

		if (pdnStatus != true || cellularStatus != CELLULAR_SUCCESS) {
			AlxOsMutex_Unlock(&me->alxMutex);
			return Alx_Err;
		}
		me->isNetConnected = true;
		AlxOsMutex_Unlock(&me->alxMutex);
	}
#endif

	// Return
	return Alx_Ok;
}
Alx_Status AlxNet_Disconnect(AlxNet* me)
{
	// Assert
	ALX_NET_ASSERT(me->wasCtorCalled == true);

	#if defined(ALX_WIZNET)
	if (me->config == AlxNet_Config_Wiznet)
	{
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
	}
	#endif

	// Connect CELLULAR to PDN (Packet Data Network)
	#if defined(ALX_FREE_RTOS_CELLULAR)
	if (me->config == AlxNet_Config_FreeRtos_Cellular)
	{
		CellularError_t cellularStatus = CELLULAR_SUCCESS;
		AlxOsMutex_Lock(&me->alxMutex);

		cellularStatus = Cellular_DeactivatePdn(me->cellular.handle, me->cellular.cellularContext);

		if (cellularStatus != CELLULAR_SUCCESS) {
			AlxOsMutex_Unlock(&me->alxMutex);
			return Alx_Err;
		}

		AlxOsMutex_Unlock(&me->alxMutex);
	}
	#endif

	// Return
	return Alx_Ok;
}
bool AlxNet_IsConnected(AlxNet* me)
{
	// Assert
	ALX_NET_ASSERT(me->wasCtorCalled == true);

	// Return
	return me->isNetConnected;
}
void AlxNet_SetMac(AlxNet* me, const char* mac)
{
	// Assert
	ALX_NET_ASSERT(me->wasCtorCalled == true);
	ALX_NET_ASSERT(strlen(mac) == 17);

	// Cellular does not support this feature
	#if defined(ALX_FREE_RTOS_CELLULAR)
	if (me->config == AlxNet_Config_FreeRtos_Cellular)
	{
		ALX_NET_ASSERT(0); // Assert, as cellular cannot call this functionality
	}
	#endif

	AlxOsMutex_Lock(&me->alxMutex);
	str2mac(mac, wiz_net_info.mac);
	strcpy(me->mac, mac);
	setSHAR(wiz_net_info.mac);
	AlxOsMutex_Unlock(&me->alxMutex);
}
void AlxNet_SetIp(AlxNet* me, const char* ip)
{
	// Assert
	ALX_NET_ASSERT(me->wasCtorCalled == true);
	ALX_NET_ASSERT(strlen(ip) < sizeof(me->ip));

	// Cellular does not support this feature
	#if defined(ALX_FREE_RTOS_CELLULAR)
	if (me->config == AlxNet_Config_FreeRtos_Cellular)
	{
		ALX_NET_ASSERT(0); // Assert, as cellular cannot call this functionality
	}
	#endif

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

	// Cellular does not support this feature
	#if defined(ALX_FREE_RTOS_CELLULAR)
	if (me->config == AlxNet_Config_FreeRtos_Cellular)
	{
		ALX_NET_ASSERT(0); // Assert, as cellular cannot call this functionality
	}
	#endif

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

	// Cellular does not support this feature
	#if defined(ALX_FREE_RTOS_CELLULAR)
	if (me->config == AlxNet_Config_FreeRtos_Cellular)
	{
		ALX_NET_ASSERT(0); // Assert, as cellular cannot call this functionality
	}
	#endif

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

	// Cellular does not support this feature
	#if defined(ALX_FREE_RTOS_CELLULAR)
	if (me->config == AlxNet_Config_FreeRtos_Cellular)
	{
		ALX_NET_ASSERT(0); // Assert, as cellular cannot call this functionality
	}
	#endif

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

	#if defined(ALX_WIZNET)
	if(me->config == AlxNet_Config_Wiznet)
	{
		uint8_t addr[4];
		AlxOsMutex_Lock(&me->alxMutex);
		getSIPR(addr);
		ip2str(addr, me->ip);
		AlxOsMutex_Unlock(&me->alxMutex);
	}
	#endif

	#if defined(ALX_FREE_RTOS_CELLULAR)
	else if(me->config == AlxNet_Config_FreeRtos_Cellular)
	{
		// If cellular, just return me->ip.
	}
	#endif
	// Return
	return me->ip;
}
const char* AlxNet_GetNetmask(AlxNet* me)
{
	// Assert
	ALX_NET_ASSERT(me->wasCtorCalled == true);

	// Cellular does not support this feature
	#if defined(ALX_FREE_RTOS_CELLULAR)
	if (me->config == AlxNet_Config_FreeRtos_Cellular)
	{
		ALX_NET_ASSERT(0); // Assert, as cellular cannot call this functionality
	}
	#endif

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

	// Cellular does not support this feature
	#if defined(ALX_FREE_RTOS_CELLULAR)
	if (me->config == AlxNet_Config_FreeRtos_Cellular)
	{
		ALX_NET_ASSERT(0); // Assert, as cellular cannot call this functionality
	}
	#endif

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

	// Cellular does not support this feature
	#if defined(ALX_FREE_RTOS_CELLULAR)
	if (me->config == AlxNet_Config_FreeRtos_Cellular)
	{
		ALX_NET_ASSERT(0); // Assert, as cellular cannot call this functionality
	}
	#endif

	AlxOsMutex_Lock(&me->alxMutex);
	strcpy(me->dns[dnsId], ip);
	AlxOsMutex_Unlock(&me->alxMutex);
}
Alx_Status AlxNet_Dns_GetHostByName(AlxNet* me, const char* hostname, char* ip)
{
	// Assert
	ALX_NET_ASSERT(me->wasCtorCalled == true);
	ALX_NET_ASSERT(strlen(hostname) < sizeof(dns_target_domain));

	if (me->config == AlxNet_Config_Wiznet)
	{
		AlxOsMutex_Lock(&me->alxMutex);
		strcpy((char *)dns_target_domain, hostname);
		dns_retval = DnsTaskRunning;
		AlxOsMutex_Unlock(&alxDnsMutex);

		while (dns_retval == DnsTaskRunning)
		{
			// timeout in DNS task
			AlxOsDelay_ms(&alxOsDelay, 20);
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
	}

	#if defined(ALX_FREE_RTOS_CELLULAR)
	if (me->config == AlxNet_Config_FreeRtos_Cellular)
	{
		CellularError_t ret;
		ret = Cellular_GetHostByName(me->cellular.handle, me->cellular.cellularContext, hostname, ip);
		if (ret != CELLULAR_SUCCESS) return Alx_Err;
		else return Alx_Ok;
	}
	#endif

	// Return
	return Alx_Err;
}
void AlxNet_Dhcp_Enable(AlxNet* me, bool enable)
{
	// Assert
	ALX_NET_ASSERT(me->wasCtorCalled == true);

	// Cellular does not support this feature
	#if defined(ALX_FREE_RTOS_CELLULAR)
	if (me->config == AlxNet_Config_FreeRtos_Cellular)
	{
		ALX_NET_ASSERT(0);	// Assert, as cellular cannot call this functionality
	}
	#endif

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
}
bool AlxNet_Dhcp_WasAddrSupplied(AlxNet* me)
{
	// Assert
	ALX_NET_ASSERT(me->wasCtorCalled == true);

	// Return
	return (AlxTick_Get_sec(&alxTick) < dhcp_ip_leased_until);
}

AlxNet_Config Alx_GetNetInterface(AlxNet *me)
{
	return me->config;
}

static int8_t parse_cellular_rssi_to_dbm(uint8_t input)
{
	if (input == 0) {
		return -113;
	}
	else if (input == 1) {
		return -111;
	}
	else if (input >= 2 && input <= 30) {
		return -109 + (input - 2) * 2;
	}
	else if (input == 31) {
		return -51;
	}
	else if (input == 99) {
		return 1; // Not known or not detectable
	}
	else if (input == 100) {
		return -116;
	}
	else if (input == 101) {
		return -115;
	}
	else if (input >= 102 && input <= 190) {
		return -114 + (input - 102);
	}
	else if (input == 191) {
		return -25;	// -25dbm or greater
	}
	else if (input == 199) {
		return 1; // Not known or not detectable
	}
	else if (input >= 100 && input <= 199) {
		return -114 + (input - 100); // Extended for TD-SCDMA
	}
	else {
		return 1; // Invalid input
	}
}

#if defined(ALX_FREE_RTOS_CELLULAR)
void Alx_GetCellularSignalQuality(AlxNet *me, int8_t *rssi, uint8_t *ber)
{
	if (rssi != NULL) {
		*rssi = parse_cellular_rssi_to_dbm(me->cellular.signalQuality.rssi);
	}
	if(ber != NULL)
		*ber = me->cellular.signalQuality.ber;
}
#endif


//******************************************************************************
// Private Functions
//******************************************************************************


#endif	// #if defined(ALX_C_LIB) && (defined(ALX_FREE_RTOS_CELLULAR) || defined(ALX_WIZNET))
