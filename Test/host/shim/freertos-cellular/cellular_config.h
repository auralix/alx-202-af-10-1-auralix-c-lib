/**
  ******************************************************************************
  * @file		cellular_config.h
  * @brief		Auralix C Library - Configuration Matrix Fake - FreeRTOS Cellular Interface
  * @copyright	Copyright (C) Auralix d.o.o. All rights reserved.
  *
  * Compile-only: the options a product's cellular_config.h sets and the alx
  * code reads, so the configuration matrix compiles every alx line under
  * ALX_FREE_RTOS_CELLULAR (Test/config/matrix.toml). The connection monitoring
  * is configured on - alxNet.c compiles more with it and nothing only without
  * it - against an address of the documentation range (RFC 5737).
  ******************************************************************************
  **/

#ifndef ALX_SHIM_FREERTOS_CELLULAR_CELLULAR_CONFIG_H
#define ALX_SHIM_FREERTOS_CELLULAR_CELLULAR_CONFIG_H

#define CELLULAR_APN_NAME "matrix.apn"
#define CELLULAR_PDN_CONTEXT_ID (1U)
#define CELLULAR_PDN_CONTEXT_ID_MIN (1U)
#define CELLULAR_PDN_CONTEXT_ID_MAX (16U)
#define CELLULAR_IP_ADDRESS_MAX_SIZE (40U)
#define CELLULAR_APN_MAX_SIZE (64U)
#define CELLULAR_PDN_USERNAME_MAX_SIZE (32U)
#define CELLULAR_PDN_PASSWORD_MAX_SIZE (32U)
#define CELLULAR_CONNECTION_MONITORING_PING "192.0.2.1"

#endif	// ALX_SHIM_FREERTOS_CELLULAR_CELLULAR_CONFIG_H
