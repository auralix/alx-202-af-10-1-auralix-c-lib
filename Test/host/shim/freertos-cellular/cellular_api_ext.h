/**
  ******************************************************************************
  * @file		cellular_api_ext.h
  * @brief		Auralix C Library - Configuration Matrix Fake - FreeRTOS Cellular Interface
  * @copyright	Copyright (C) Auralix d.o.o. All rights reserved.
  *
  * Compile-only: the extension calls a product's modem port adds to the
  * cellular interface and the alx code uses - power down, ping, signal
  * quality - written from that use, so the configuration matrix compiles every
  * alx line under ALX_FREE_RTOS_CELLULAR (Test/config/matrix.toml).
  ******************************************************************************
  **/

#ifndef ALX_SHIM_FREERTOS_CELLULAR_CELLULAR_API_EXT_H
#define ALX_SHIM_FREERTOS_CELLULAR_CELLULAR_API_EXT_H

#include "cellular_types.h"

CellularError_t Cellular_Pdown(CellularHandle_t cellularHandle);
CellularError_t Cellular_Ping(CellularHandle_t cellularHandle, uint8_t contextId, const char* pAddress);
CellularError_t Cellular_GetInfo(CellularHandle_t cellularHandle, cellularSignalQuality_t* pSignalQuality);

#endif	// ALX_SHIM_FREERTOS_CELLULAR_CELLULAR_API_EXT_H
