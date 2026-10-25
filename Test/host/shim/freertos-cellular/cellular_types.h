/**
  ******************************************************************************
  * @file		cellular_types.h
  * @brief		Auralix C Library - Configuration Matrix Fake - FreeRTOS Cellular Interface
  * @copyright	Copyright (C) Auralix d.o.o. All rights reserved.
  *
  * Compile-only: the FreeRTOS cellular interface types and API as far as the
  * alx code uses them - the handle and the comm interface, SIM, registration
  * and PDN status, the sockets and their callbacks - written from that use, so
  * the configuration matrix compiles every alx line under
  * ALX_FREE_RTOS_CELLULAR (Test/config/matrix.toml). Nothing here is the
  * vendor's and nothing here runs; the real one is the product's.
  ******************************************************************************
  **/

#ifndef ALX_SHIM_FREERTOS_CELLULAR_CELLULAR_TYPES_H
#define ALX_SHIM_FREERTOS_CELLULAR_CELLULAR_TYPES_H

#include <stdint.h>
#include "cellular_config.h"


//******************************************************************************
// Handle, comm interface, errors
//******************************************************************************
typedef struct CellularContext* CellularHandle_t;

typedef struct CellularCommInterface
{
	int (*open)(void* pCommInterfaceHandle);
	int (*send)(void* pCommInterfaceHandle, const uint8_t* pData, uint32_t dataLength, uint32_t timeoutMilliseconds, uint32_t* pDataSentLength);
	int (*recv)(void* pCommInterfaceHandle, uint8_t* pBuffer, uint32_t bufferLength, uint32_t timeoutMilliseconds, uint32_t* pDataReceivedLength);
	int (*close)(void* pCommInterfaceHandle);
} CellularCommInterface_t;

typedef enum CellularError
{
	CELLULAR_SUCCESS = 0,
	CELLULAR_INVALID_HANDLE,
	CELLULAR_MODEM_NOT_READY,
	CELLULAR_LIBRARY_NOT_OPEN,
	CELLULAR_LIBRARY_ALREADY_OPEN,
	CELLULAR_BAD_PARAMETER,
	CELLULAR_NO_MEMORY,
	CELLULAR_TIMEOUT,
	CELLULAR_SOCKET_CLOSED,
	CELLULAR_SOCKET_NOT_CONNECTED,
	CELLULAR_INTERNAL_FAILURE,
	CELLULAR_RESOURCE_CREATION_FAIL,
	CELLULAR_UNSUPPORTED,
	CELLULAR_NOT_ALLOWED,
	CELLULAR_UNKNOWN
} CellularError_t;


//******************************************************************************
// SIM and registration
//******************************************************************************
typedef enum CellularSimCardState
{
	CELLULAR_SIM_CARD_REMOVED = 0,
	CELLULAR_SIM_CARD_INSERTED,
	CELLULAR_SIM_CARD_STATUS_UNKNOWN
} CellularSimCardState_t;

typedef enum CellularSimCardLockState
{
	CELLULAR_SIM_CARD_READY = 0,
	CELLULAR_SIM_CARD_PIN,
	CELLULAR_SIM_CARD_PUK,
	CELLULAR_SIM_CARD_LOCK_UNKNOWN
} CellularSimCardLockState_t;

typedef struct CellularSimCardStatus
{
	CellularSimCardState_t simCardState;
	CellularSimCardLockState_t simCardLockState;
} CellularSimCardStatus_t;

typedef enum CellularNetworkRegistrationStatus
{
	REGISTRATION_STATUS_NOT_REGISTERED_NOT_SEARCHING = 0,
	REGISTRATION_STATUS_REGISTERED_HOME = 1,
	REGISTRATION_STATUS_NOT_REGISTERED_SEARCHING = 2,
	REGISTRATION_STATUS_REGISTRATION_DENIED = 3,
	REGISTRATION_STATUS_UNKNOWN = 4,
	REGISTRATION_STATUS_ROAMING_REGISTERED = 5
} CellularNetworkRegistrationStatus_t;

typedef struct CellularServiceStatus
{
	CellularNetworkRegistrationStatus_t csRegistrationStatus;
	CellularNetworkRegistrationStatus_t psRegistrationStatus;
	char operatorName[32];
} CellularServiceStatus_t;

typedef struct cellularSignalQuality
{
	uint8_t rssi;
	uint8_t ber;
} cellularSignalQuality_t;


//******************************************************************************
// PDN
//******************************************************************************
typedef enum CellularPdnContextType
{
	CELLULAR_PDN_CONTEXT_IPV4 = 1,
	CELLULAR_PDN_CONTEXT_IPV6,
	CELLULAR_PDN_CONTEXT_IPV4V6
} CellularPdnContextType_t;

typedef enum CellularPdnAuthType
{
	CELLULAR_PDN_AUTH_NONE = 0,
	CELLULAR_PDN_AUTH_PAP,
	CELLULAR_PDN_AUTH_CHAP,
	CELLULAR_PDN_AUTH_PAP_OR_CHAP
} CellularPdnAuthType_t;

typedef struct CellularPdnConfig
{
	CellularPdnContextType_t pdnContextType;
	CellularPdnAuthType_t pdnAuthType;
	char apnName[CELLULAR_APN_MAX_SIZE + 1U];
	char username[CELLULAR_PDN_USERNAME_MAX_SIZE + 1U];
	char password[CELLULAR_PDN_PASSWORD_MAX_SIZE + 1U];
} CellularPdnConfig_t;

typedef enum CellularIPAddressType
{
	CELLULAR_IP_ADDRESS_V4 = 0,
	CELLULAR_IP_ADDRESS_V6
} CellularIPAddressType_t;

typedef struct CellularIPAddress
{
	CellularIPAddressType_t ipAddressType;
	char ipAddress[CELLULAR_IP_ADDRESS_MAX_SIZE + 1U];
} CellularIPAddress_t;

typedef struct CellularPdnStatus
{
	uint8_t contextId;
	uint8_t state;
	CellularPdnContextType_t pdnContextType;
	CellularIPAddress_t ipAddress;
} CellularPdnStatus_t;


//******************************************************************************
// Sockets
//******************************************************************************
typedef struct CellularSocketContext* CellularSocketHandle_t;

typedef enum CellularSocketDomain { CELLULAR_SOCKET_DOMAIN_AF_INET = 0 } CellularSocketDomain_t;
typedef enum CellularSocketType { CELLULAR_SOCKET_TYPE_DGRAM = 0, CELLULAR_SOCKET_TYPE_STREAM } CellularSocketType_t;
typedef enum CellularSocketProtocol { CELLULAR_SOCKET_PROTOCOL_UDP = 0, CELLULAR_SOCKET_PROTOCOL_TCP } CellularSocketProtocol_t;
typedef enum CellularSocketAccessMode { CELLULAR_ACCESSMODE_BUFFER = 0, CELLULAR_ACCESSMODE_DIRECT_PUSH } CellularSocketAccessMode_t;
typedef enum CellularUrcEvent
{
	CELLULAR_URC_EVENT_NETWORK_CS_REGISTRATION = 0,
	CELLULAR_URC_EVENT_NETWORK_PS_REGISTRATION,
	CELLULAR_URC_SOCKET_OPENED,
	CELLULAR_URC_SOCKET_OPEN_FAILED
} CellularUrcEvent_t;

typedef struct CellularSocketAddress
{
	CellularIPAddress_t ipAddress;
	uint16_t port;
} CellularSocketAddress_t;

typedef void (*CellularSocketOpenCallback_t)(CellularUrcEvent_t urcEvent, CellularSocketHandle_t socketHandle, void* pCallbackContext);
typedef void (*CellularSocketClosedCallback_t)(CellularSocketHandle_t socketHandle, void* pCallbackContext);
typedef void (*CellularSocketDataReadyCallback_t)(CellularSocketHandle_t socketHandle, void* pCallbackContext);
typedef void (*CellularUrcGenericCallback_t)(const char* pRawData, void* pCallbackContext);


//******************************************************************************
// API
//******************************************************************************
CellularError_t Cellular_Init(CellularHandle_t* pCellularHandle, const CellularCommInterface_t* pCommInterface);
CellularError_t Cellular_Cleanup(CellularHandle_t cellularHandle);
CellularError_t Cellular_RegisterUrcGenericCallback(CellularHandle_t cellularHandle, CellularUrcGenericCallback_t genericCallback, void* pCallbackContext);
CellularError_t Cellular_GetSimCardStatus(CellularHandle_t cellularHandle, CellularSimCardStatus_t* pSimCardStatus);
CellularError_t Cellular_GetServiceStatus(CellularHandle_t cellularHandle, CellularServiceStatus_t* pServiceStatus);
CellularError_t Cellular_SetPdnConfig(CellularHandle_t cellularHandle, uint8_t contextId, const CellularPdnConfig_t* pPdnConfig);
CellularError_t Cellular_ActivatePdn(CellularHandle_t cellularHandle, uint8_t contextId);
CellularError_t Cellular_DeactivatePdn(CellularHandle_t cellularHandle, uint8_t contextId);
CellularError_t Cellular_GetPdnStatus(CellularHandle_t cellularHandle, CellularPdnStatus_t* pPdnStatusBuffers, uint8_t numStatusBuffers, uint8_t* pNumStatus);
CellularError_t Cellular_GetIPAddress(CellularHandle_t cellularHandle, uint8_t contextId, char* pBuffer, uint32_t bufferLength);
CellularError_t Cellular_GetHostByName(CellularHandle_t cellularHandle, uint8_t contextId, const char* pcHostName, char* pResolvedAddress);
CellularError_t Cellular_CreateSocket(CellularHandle_t cellularHandle, uint8_t pdnContextId, CellularSocketDomain_t socketDomain, CellularSocketType_t socketType, CellularSocketProtocol_t socketProtocol, CellularSocketHandle_t* pSocketHandle);
CellularError_t Cellular_SocketConnect(CellularHandle_t cellularHandle, CellularSocketHandle_t socketHandle, CellularSocketAccessMode_t dataAccessMode, const CellularSocketAddress_t* pRemoteSocketAddress);
CellularError_t Cellular_SocketSend(CellularHandle_t cellularHandle, CellularSocketHandle_t socketHandle, const uint8_t* pData, uint32_t dataLength, uint32_t* pSentDataLength);
CellularError_t Cellular_SocketRecv(CellularHandle_t cellularHandle, CellularSocketHandle_t socketHandle, uint8_t* pBuffer, uint32_t bufferLength, uint32_t* pReceivedDataLength);
CellularError_t Cellular_SocketClose(CellularHandle_t cellularHandle, CellularSocketHandle_t socketHandle);
CellularError_t Cellular_SocketRegisterSocketOpenCallback(CellularHandle_t cellularHandle, CellularSocketHandle_t socketHandle, CellularSocketOpenCallback_t socketOpenCallback, void* pCallbackContext);
CellularError_t Cellular_SocketRegisterClosedCallback(CellularHandle_t cellularHandle, CellularSocketHandle_t socketHandle, CellularSocketClosedCallback_t closedCallback, void* pCallbackContext);
CellularError_t Cellular_SocketRegisterDataReadyCallback(CellularHandle_t cellularHandle, CellularSocketHandle_t socketHandle, CellularSocketDataReadyCallback_t dataReadyCallback, void* pCallbackContext);

#endif	// ALX_SHIM_FREERTOS_CELLULAR_CELLULAR_TYPES_H
