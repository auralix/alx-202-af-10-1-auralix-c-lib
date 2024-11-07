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
//#define ALX_DBG_WIZNET

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

static void __attribute__((unused)) print_network_information(wiz_NetInfo net_info)
{
	uint8_t tmp_str[8] = {
		0,
	};

	ctlnetwork(CN_GET_NETINFO, (void *)&net_info);
	ctlwizchip(CW_GET_ID, (void *)tmp_str);

	if (net_info.dhcp == NETINFO_DHCP)
	{
		ALX_NET_TRACE_INF("==========================================");
		ALX_NET_TRACE_INF(" %s network configuration : DHCP", (char *)tmp_str);
	}
	else
	{
		ALX_NET_TRACE_INF("==========================================");
		ALX_NET_TRACE_INF(" %s network configuration : static", (char *)tmp_str);
	}

	ALX_NET_TRACE_INF(" MAC         : %02X:%02X:%02X:%02X:%02X:%02X", net_info.mac[0], net_info.mac[1], net_info.mac[2], net_info.mac[3], net_info.mac[4], net_info.mac[5]);
	ALX_NET_TRACE_INF(" IP          : %d.%d.%d.%d", net_info.ip[0], net_info.ip[1], net_info.ip[2], net_info.ip[3]);
	ALX_NET_TRACE_INF(" Subnet Mask : %d.%d.%d.%d", net_info.sn[0], net_info.sn[1], net_info.sn[2], net_info.sn[3]);
	ALX_NET_TRACE_INF(" Gateway     : %d.%d.%d.%d", net_info.gw[0], net_info.gw[1], net_info.gw[2], net_info.gw[3]);
	ALX_NET_TRACE_INF(" DNS         : %d.%d.%d.%d", net_info.dns[0], net_info.dns[1], net_info.dns[2], net_info.dns[3]);
	ALX_NET_TRACE_INF("==========================================");
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
//	ALX_NET_TRACE_INF("DHCP leased time : %ld seconds", getDHCPLeasetime());
}
static void wizchip_dhcp_conflict(void)
{
	ALX_NET_TRACE_INF("Conflict IP from DHCP");

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
//						ALX_NET_TRACE_INF("DHCP client started");
						DHCP_init(alxDhcpSocket.socket_data.wiz_socket, wiz_buffer);
						dhcp_ip_leased_until = 0;
						state = NETINFO_DHCP;
					}

					if ((state == NETINFO_DHCP) && !me->enable_dhcp)
					{
//						ALX_NET_TRACE_INF("DHCP client stopped");
						DHCP_stop();
						state = NETINFO_STATIC;
						break; // break to mutex
					}

					if (state == NETINFO_DHCP)
					{
						link = wizphy_getphylink();
						if (link == PHY_LINK_OFF)
						{
							ALX_NET_TRACE_INF("PHY_LINK_OFF");
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
//								ALX_NET_TRACE_INF("DHCP success");
								dhcp_ip_leased_until = AlxTick_Get_sec(&alxTick) + getDHCPLeasetime();
								ip2str(wiz_net_info.dns, me->dns[0]);
							}
						}
						else if (retval == DHCP_FAILED)
						{
							dhcp_ip_leased_until = 0;
							ALX_NET_TRACE_INF("DHCP timeout occurred, retry: %d", dhcp_retry);
						}
					}
				}
				AlxOsDelay_ms(&alxOsDelay, 1000);
			}
			AlxSocket_Close(&alxDhcpSocket); // close the socket
		}
	}
}

#ifdef ALX_DBG_WIZNET
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
	ALX_NET_TRACE_INF("--- WIZNET STATUS ----");
	ALX_NET_TRACE_INF("SR[i]: %02x->%02x %02x->%02x %02x->%02x %02x->%02x %02x->%02x %02x->%02x %02x->%02x %02x->%02x",
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
	ALX_NET_TRACE_INF("PhyLink: %02x->%02x", dbg_wiz_staus[0].link, dbg_wiz_staus[1].link);
	ALX_NET_TRACE_INF("PhyConf: mode:%02x->%02x duplex:%02x->%02x speed:%02x->%02x",
		dbg_wiz_staus[0].phyconf.mode,
		dbg_wiz_staus[1].phyconf.mode,
		dbg_wiz_staus[0].phyconf.duplex,
		dbg_wiz_staus[1].phyconf.duplex,
		dbg_wiz_staus[0].phyconf.speed,
		dbg_wiz_staus[1].phyconf.speed);
}
#endif

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
//					ALX_NET_TRACE_INF("DNS success");
//					ALX_NET_TRACE_INF("Target domain : %s", dns_target_domain);
//					ALX_NET_TRACE_INF("IP of target domain : %d.%d.%d.%d", dns_response_ip[0], dns_response_ip[1], dns_response_ip[2], dns_response_ip[3]);
					dns_retval = DnsTaskSuccess;
#ifdef ALX_DBG_WIZNET
					dbg_get();
#endif
					break;
				}
				else
				{
					dns_retry++;
					if (dns_retry <= DNS_RETRY_COUNT)
					{
						ALX_NET_TRACE_INF("DNS timeout occurred, retry %d", dns_retry);
#ifdef ALX_DBG_WIZNET
						dbg_get();
						dbg_dump();
#endif
					}
				}

				if (dns_retry > DNS_RETRY_COUNT)
				{
					ALX_NET_TRACE_INF("DNS failed for %s", dns_target_domain);
					dns_retval = DnsTaskTimeout;
#ifdef ALX_DBG_WIZNET
					dbg_get();
					dbg_dump();
					wizphy_reset();
#endif
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
		//ALX_NET_TRACE_INF("CELLULAR RSSI: %d", signalInfo.rssi);
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

#if defined(ALX_FREE_RTOS_CELLULAR)

#define MODEM_REGISTER_TIMEOUT 60000
#define MODEM_SIM_TIMEOUT 20000
#define MODEM_ACTIVATE_PDN_TIMEOUT 150000
#define MODEM_CONNECTION_MONITORING_TIMEOUT 10000
#define MODEM_LOW_LEVEL_ERR_THRESHOLD 3

typedef enum
{
	State_ModemGoOff,
	State_ModemOff,
	State_ModemOn,
	State_SimOk,
	State_ModemRegistered,
	State_ModemConnected
} ConenctionStateType;

typedef enum
{
	Cmd_Work,
	Cmd_PowerCycle,
	Cmd_Disconnect
} HandleConnectionCommandType;

static ConenctionStateType cellular_HandleConnection(AlxNet *me, HandleConnectionCommandType cmd)
{
	static ConenctionStateType connection_state = State_ModemOff;
	static uint64_t start_time = 0;
	static int pdn_deact_cnt = 0;
	static int low_level_err = 0;

	if ((cmd == Cmd_PowerCycle) || (low_level_err > MODEM_LOW_LEVEL_ERR_THRESHOLD))
	{
		connection_state = State_ModemGoOff;
	}

	ALX_NET_TRACE_INF("Cellular handler state: %d", connection_state);
	switch (connection_state)
	{
	case State_ModemGoOff:
		{
			if (Cellular_Cleanup(me->cellular.handle) == CELLULAR_SUCCESS)
			{
				me->cellular.handle = NULL;
			}
			low_level_err = 0;
			connection_state = State_ModemOff;
			break;
		}
	case State_ModemOff:
		{
			me->isNetConnected = false;
			if (Cellular_Init(&me->cellular.handle, me->cellular.CommIntf) == CELLULAR_SUCCESS) // power on with GPIO key simulation, auto-bauding
			{
				start_time = AlxTick_Get_ms(&alxTick);
				connection_state = State_ModemOn;
			}
			else
			{
				connection_state = State_ModemGoOff;
			}
			break;
		}
	case State_ModemOn:
		{
			while (true)
			{
				if (Cellular_GetSimCardStatus(me->cellular.handle, &me->cellular.simStatus) != CELLULAR_SUCCESS)
				{
					ALX_NET_TRACE_INF(("Cellular SIM failure"));
					connection_state = State_ModemGoOff;
				}

				if (AlxTick_Get_ms(&alxTick) - start_time > MODEM_SIM_TIMEOUT)
				{
					ALX_NET_TRACE_WRN("SIM ready timed out");
					connection_state = State_ModemGoOff;
					break;
				}

				if ((me->cellular.simStatus.simCardState == CELLULAR_SIM_CARD_INSERTED) &&
					(me->cellular.simStatus.simCardLockState == CELLULAR_SIM_CARD_READY))
				{
					connection_state = State_SimOk;
					start_time = AlxTick_Get_ms(&alxTick);
					break;
				}

				vTaskDelay(pdMS_TO_TICKS(CELLULAR_SIM_CARD_WAIT_INTERVAL_MS));
			}
			break;
		}
	case State_SimOk:
		{
			while (true)
			{
				CellularError_t cellularStatus = Cellular_GetServiceStatus(me->cellular.handle, &me->cellular.serviceStatus);

				if ((cellularStatus == CELLULAR_SUCCESS) &&
				    ((me->cellular.serviceStatus.psRegistrationStatus == REGISTRATION_STATUS_REGISTERED_HOME) ||
				      (me->cellular.serviceStatus.psRegistrationStatus == REGISTRATION_STATUS_ROAMING_REGISTERED)))
				{
					connection_state = State_ModemRegistered;
					start_time = AlxTick_Get_ms(&alxTick);
					break;
				}

				if (AlxTick_Get_ms(&alxTick) - start_time > MODEM_REGISTER_TIMEOUT)
				{
					ALX_NET_TRACE_WRN("Registering to network timed out");
					connection_state = State_ModemGoOff;
					break;
				}

				vTaskDelay(pdMS_TO_TICKS(CELLULAR_PDN_CONNECT_WAIT_INTERVAL_MS));
			}
			break;
		}
	case State_ModemRegistered:
		{
			me->cellular.cellularContext = CELLULAR_PDN_CONTEXT_ID;
			CellularPdnConfig_t pdnConfig = { CELLULAR_PDN_CONTEXT_IPV4, CELLULAR_PDN_AUTH_NONE, "internet", "", "" }; // ToDo: parameter!
			Cellular_SetPdnConfig(me->cellular.handle, me->cellular.cellularContext, &pdnConfig);

			while (true)
			{
				if (AlxTick_Get_ms(&alxTick) - start_time > MODEM_ACTIVATE_PDN_TIMEOUT)
				{
					ALX_NET_TRACE_WRN("Activating PDN timed out");
					connection_state = State_ModemGoOff;
					break;
				}

				if (Cellular_ActivatePdn(me->cellular.handle, me->cellular.cellularContext) == CELLULAR_SUCCESS)
				{
					if (Cellular_GetIPAddress(me->cellular.handle, me->cellular.cellularContext, me->ip, sizeof(me->ip)) == CELLULAR_SUCCESS)
					{
						me->isNetConnected = true;
						start_time = AlxTick_Get_ms(&alxTick);
						connection_state = State_ModemConnected;
						break;
					}
					ALX_NET_TRACE_WRN("Cellular_GetIPAddress failure");
					// contimnue to PDN deactivation
				}

				if (Cellular_DeactivatePdn(me->cellular.handle, me->cellular.cellularContext) != CELLULAR_SUCCESS)
				{
					if (pdn_deact_cnt++ > 3)
					{
						connection_state = State_ModemGoOff;
						break;
					}
					break;
				}
				else
				{
					// PDN deact successfull
					pdn_deact_cnt = 0;
					start_time = AlxTick_Get_ms(&alxTick);
					connection_state = State_ModemOn;
					break;
				}

				vTaskDelay(pdMS_TO_TICKS(CELLULAR_PDN_CONNECT_WAIT_INTERVAL_MS));
			}
			break;
		}
	case State_ModemConnected:
		{
			if (cmd == Cmd_Disconnect)
			{
				me->isNetConnected = false;
				start_time = AlxTick_Get_ms(&alxTick);
				connection_state = State_ModemOn;
				break;
			}

			// periodically do connection monitoring
			if (AlxTick_Get_ms(&alxTick) - start_time > MODEM_CONNECTION_MONITORING_TIMEOUT)
			{
				start_time = AlxTick_Get_ms(&alxTick);

				// update RSSI info
				if (Cellular_GetInfo(me->cellular.handle, &me->cellular.signalQuality) != CELLULAR_SUCCESS)
				{
					low_level_err++;
				}

				// monitor PDN status
				uint8_t NumStatus = 0;
				CellularPdnStatus_t PdnStatusBuffers[CELLULAR_PDN_CONTEXT_ID_MAX - CELLULAR_PDN_CONTEXT_ID_MIN + 1U] = { 0 };

				if (Cellular_GetPdnStatus(me->cellular.handle, PdnStatusBuffers, (CELLULAR_PDN_CONTEXT_ID_MAX - CELLULAR_PDN_CONTEXT_ID_MIN + 1U), &NumStatus) == CELLULAR_SUCCESS)
				{
					bool pdnStatus = false;
					for (int i = 0U; i < NumStatus; i++) {
						if ((PdnStatusBuffers[i].contextId == me->cellular.cellularContext) && (PdnStatusBuffers[i].state == 1)) {
							pdnStatus = true;
							break;
						}
					}
					if (pdnStatus == false) {
						ALX_NET_TRACE_WRN("Cellular PDN is not activated");
						me->isNetConnected = false;
						start_time = AlxTick_Get_ms(&alxTick);
						connection_state = State_ModemOn;
						break;
					}
				}
				else
				{
					ALX_NET_TRACE_WRN("Cellular_GetPdnStatus failure");
					low_level_err++;
				}

				// monitor service status
				CellularServiceStatus_t serviceStatus;
				if (Cellular_GetServiceStatus(me->cellular.handle, &serviceStatus) == CELLULAR_SUCCESS)
				{
					if (serviceStatus.psRegistrationStatus != REGISTRATION_STATUS_REGISTERED_HOME &&
						serviceStatus.psRegistrationStatus != REGISTRATION_STATUS_ROAMING_REGISTERED)
					{
						me->isNetConnected = false;
						start_time = AlxTick_Get_ms(&alxTick);
						connection_state = State_ModemOn;
					}
				}
				else
				{
					low_level_err++;
				}
			}
			break;
		}

	default:
		// should never happen
		connection_state = State_ModemGoOff;
		break;
	}
	return connection_state;
}
#endif

#if defined(ALX_WIZNET)
// saved when wiznet network is disconencted
static char saved_mac[ALX_NET_MAC_SIZE];
static char saved_ip[ALX_NET_IP_ADDRESS_SIZE];
static char saved_netmask[ALX_NET_IP_ADDRESS_SIZE];
static char saved_gateway[ALX_NET_IP_ADDRESS_SIZE];
static char saved_dns[4][ALX_NET_IP_ADDRESS_SIZE];
#endif

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
		AlxOsMutex_Lock(&me->alxMutex);
		uint64_t start_time = AlxTick_Get_ms(&alxTick);
		cellular_HandleConnection(me, Cmd_PowerCycle);
		while (AlxTick_Get_ms(&alxTick) - start_time < 120000)
		{
			if (cellular_HandleConnection(me, Cmd_Work) == State_ModemRegistered)
			{
				AlxOsMutex_Unlock(&me->alxMutex);
				return Alx_Ok;
			}
		}

		AlxOsMutex_Unlock(&me->alxMutex);
		return Alx_Err;
	}
	#endif

	return Alx_Ok;
}

#define AlxOsMutex_Lock(m) ALX_NET_TRACE_INF("alxNet Lock"); AlxOsMutex_Lock(m)
#define AlxOsMutex_Unlock(m) ALX_NET_TRACE_INF("alxNet Unlock"); AlxOsMutex_Unlock(m)

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
				ALX_NET_TRACE_INF("Unknown PHY link status");
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
		uint64_t start_time = AlxTick_Get_ms(&alxTick);
		while (AlxTick_Get_ms(&alxTick) - start_time < 120000)
		{
			if (cellular_HandleConnection(me, Cmd_Work) == State_ModemConnected)
			{
				AlxOsMutex_Unlock(&me->alxMutex);
				return Alx_Ok;
			}
			AlxOsDelay_ms(&alxOsDelay, 100);
		}

		AlxOsMutex_Unlock(&me->alxMutex);
		vTaskDelay(pdMS_TO_TICKS(CELLULAR_PDN_CONNECT_WAIT_INTERVAL_MS));
		return Alx_Err;
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

		strcpy(saved_mac, me->mac);
		strcpy(saved_ip, me->ip);
		strcpy(saved_netmask, me->netmask);
		strcpy(saved_gateway, me->gateway);
		memcpy(saved_dns, me->dns, sizeof(saved_dns));

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
		AlxOsMutex_Lock(&me->alxMutex);
		cellular_HandleConnection(me, Cmd_Disconnect);
		me->isNetConnected = false;
		AlxOsMutex_Unlock(&me->alxMutex);
	}
	#endif

	// Return
	return Alx_Ok;
}
Alx_Status AlxNet_Restart(AlxNet* me)
{
	// Assert
	ALX_NET_ASSERT(me->wasCtorCalled == true);

#if defined(ALX_WIZNET)
	if (me->config == AlxNet_Config_Wiznet)
	{
		AlxNet_Disconnect(me);
	}
#endif

	// Connect CELLULAR to PDN (Packet Data Network)
#if defined(ALX_FREE_RTOS_CELLULAR)
	if (me->config == AlxNet_Config_FreeRtos_Cellular)
	{
		AlxOsMutex_Lock(&me->alxMutex);
		cellular_HandleConnection(me, Cmd_PowerCycle);
		me->isNetConnected = false;
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

#if defined(ALX_WIZNET)
	if (!me->isNetConnected)
	{
		AlxNet_Connect(me);
		AlxNet_SetMac(me, saved_mac);
		AlxNet_SetIp(me, saved_ip);
		AlxNet_SetGateway(me, saved_gateway);
		AlxNet_SetNetmask(me, saved_netmask);
		memcpy(me->dns, saved_dns, sizeof(me->dns));
	}
#endif

#if defined(ALX_FREE_RTOS_CELLULAR)
	if (me->config == AlxNet_Config_FreeRtos_Cellular)
	{
		AlxOsMutex_Lock(&me->alxMutex);
		cellular_HandleConnection(me, Cmd_Work);
		AlxOsMutex_Unlock(&me->alxMutex);
	}
#endif

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
