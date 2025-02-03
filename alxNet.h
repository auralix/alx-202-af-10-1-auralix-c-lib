/**
  ******************************************************************************
  * @file		alxNet.h
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
// Include Guard
//******************************************************************************
#ifndef ALX_NET_H
#define ALX_NET_H

#ifdef __cplusplus
extern "C" {
#endif


//******************************************************************************
// Includes
//******************************************************************************
#include "alxGlobal.h"
#include "alxTrace.h"
#include "alxAssert.h"
#include "alxSpi.h"
#include "alxIoPin.h"
#include "alxOsMutex.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB) && (defined(ALX_FREE_RTOS_CELLULAR) || defined(ALX_WIZNET))


//******************************************************************************
// Preprocessor
//******************************************************************************
#define ALX_NET_FILE "alxNet.h"

// Assert //
#if defined(ALX_NET_ASSERT_BKPT_ENABLE)
	#define ALX_NET_ASSERT(expr) ALX_ASSERT_BKPT(ALX_NET_FILE, expr)
#elif defined(ALX_NET_ASSERT_TRACE_ENABLE)
	#define ALX_NET_ASSERT(expr) ALX_ASSERT_TRACE(ALX_NET_FILE, expr)
#elif defined(ALX_NET_ASSERT_RST_ENABLE)
	#define ALX_NET_ASSERT(expr) ALX_ASSERT_RST(ALX_NET_FILE, expr)
#else
	#define ALX_NET_ASSERT(expr) do{} while (false)
#endif

// Trace //
#if defined(ALX_NET_TRACE_ENABLE)
	#define ALX_NET_TRACE_WRN(...) ALX_TRACE_WRN(ALX_NET_FILE, __VA_ARGS__)
	#define ALX_NET_TRACE_INF(...) ALX_TRACE_INF(ALX_NET_FILE, __VA_ARGS__)
#else
	#define ALX_NET_TRACE_WRN(...) do{} while (false)
	#define ALX_NET_TRACE_INF(...) do{} while (false)
#endif

//******************************************************************************
// Defines
//******************************************************************************
#define ALX_NET_IP_ADDRESS_SIZE 45
#define ALX_NET_MAC_SIZE 18

//******************************************************************************
// Types
//******************************************************************************
typedef enum
{
	AlxNet_Config_Undefined,
	#if defined(ALX_WIZNET)
	AlxNet_Config_Wiznet,
	#endif
	#if defined(ALX_FREE_RTOS_CELLULAR)
	AlxNet_Config_FreeRtos_Cellular
	#endif
} AlxNet_Config;

#if defined(ALX_FREE_RTOS_CELLULAR)
typedef struct
{
	// Cellular context
	CellularHandle_t handle;
	CellularCommInterface_t * CommIntf; // UART interface
	CellularSimCardStatus_t simStatus;
	CellularServiceStatus_t serviceStatus;
	cellularSignalQuality_t signalQuality;
	uint8_t cellularContext;	// Cellular context id
}AlxNet_Cellular;
#endif

typedef struct
{
	// Defines

	// Parameters
	AlxNet_Config config;
	AlxSpi* alxSpi;
	AlxIoPin* do_nRST;
	AlxIoPin* di_nINT;
	bool enable_dhcp;

	#if defined(ALX_FREE_RTOS_CELLULAR)
	AlxNet_Cellular cellular;
	#endif
	// Variables
	AlxOsMutex alxMutex;
	char mac[ALX_NET_MAC_SIZE]; // MAC in string format -> "00:18:10:3A:B8:39"
	char ip[ALX_NET_IP_ADDRESS_SIZE]; // IP, Netmask, gateway, dns in string format -> "123.123.123.123"
	char netmask[ALX_NET_IP_ADDRESS_SIZE];
	char gateway[ALX_NET_IP_ADDRESS_SIZE];
	char dns[4][ALX_NET_IP_ADDRESS_SIZE];

	// Info
	bool wasCtorCalled;
	bool isNetConnected;
} AlxNet;

typedef enum
{
	DnsTaskRunning,
	DnsTaskTimeout,
	DnsTaskSuccess
} DnsTaskState;

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
);


//******************************************************************************
// Functions
//******************************************************************************
Alx_Status AlxNet_Init(AlxNet* me);

Alx_Status AlxNet_Connect(AlxNet* me);
Alx_Status AlxNet_Disconnect(AlxNet* me);
Alx_Status AlxNet_Restart(AlxNet* me);
bool AlxNet_IsConnected(AlxNet* me);
void AlxNet_SetMac(AlxNet* me, const char* mac);
void AlxNet_SetIp(AlxNet* me, const char* ip);
void AlxNet_SetNetmask(AlxNet* me, const char* netmask);
void AlxNet_SetGateway(AlxNet* me, const char* gateway);
const char* AlxNet_GetMac(AlxNet* me);
const char* AlxNet_GetIp(AlxNet* me);
const char* AlxNet_GetNetmask(AlxNet* me);
const char* AlxNet_GetGateway(AlxNet* me);
void AlxNet_Dns_SetIp(AlxNet* me, uint8_t dnsId, const char* ip);
Alx_Status AlxNet_Dns_GetHostByName(AlxNet* me, const char* hostname, char* ip);
void AlxNet_Dhcp_Enable(AlxNet* me, bool enable);
bool AlxNet_Dhcp_WasAddrSupplied(AlxNet* me);
AlxNet_Config Alx_GetNetInterface(AlxNet* me);
#if defined(ALX_FREE_RTOS_CELLULAR)
void Alx_GetCellularSignalQuality(AlxNet *me, int8_t *rssi, uint8_t *ber);
#endif


#endif	// #if defined(ALX_C_LIB) && (defined(ALX_FREE_RTOS_CELLULAR) || defined(ALX_WIZNET))

#ifdef __cplusplus
}
#endif

#endif	// #ifndef ALX_NET_H
