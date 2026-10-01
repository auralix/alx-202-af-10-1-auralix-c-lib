/**
  ******************************************************************************
  * @file		usbh_core.h
  * @brief		Auralix C Library - Configuration Matrix Fake - STM32 USB Host Library
  * @copyright	Copyright (C) Auralix d.o.o. All rights reserved.
  *
  * Compile-only: the STM32 USB host library core as far as the alx code uses
  * it - the host handle, the status and the speed, the user events, the core
  * calls, and the low-level driver the library implements in
  * alxUsb_McuStm32.c - written from that use, so the configuration matrix
  * compiles every alx line under ALX_USBH (Test/config/matrix.toml). Nothing
  * here is the vendor's and nothing here runs; the real one is the product's.
  *
  * It brings in the library's usbh_conf.h, as the real core does, which is how
  * that header and its user template are compiled.
  ******************************************************************************
  **/

#ifndef ALX_SHIM_STM32_USB_HOST_USBH_CORE_H
#define ALX_SHIM_STM32_USB_HOST_USBH_CORE_H

#include <stdint.h>
#include "usbh_conf.h"

typedef enum
{
	USBH_OK = 0,
	USBH_BUSY,
	USBH_FAIL,
	USBH_NOT_SUPPORTED,
	USBH_UNRECOVERED_ERROR,
	USBH_ERROR_SPEED_UNKNOWN
} USBH_StatusTypeDef;

typedef enum
{
	USBH_SPEED_HIGH = 0U,
	USBH_SPEED_FULL = 1U,
	USBH_SPEED_LOW = 2U
} USBH_SpeedTypeDef;

typedef enum
{
	USBH_URB_IDLE = 0U,
	USBH_URB_DONE,
	USBH_URB_NOTREADY,
	USBH_URB_NYET,
	USBH_URB_ERROR,
	USBH_URB_STALL
} USBH_URBStateTypeDef;

#define HOST_USER_SELECT_CONFIGURATION 0x01U
#define HOST_USER_CLASS_ACTIVE 0x02U
#define HOST_USER_CLASS_SELECTED 0x03U
#define HOST_USER_CONNECTION 0x04U
#define HOST_USER_DISCONNECTION 0x05U
#define HOST_USER_UNRECOVERED_ERROR 0x06U

typedef struct _USBH_HandleTypeDef
{
	volatile uint32_t Timer;
	uint8_t id;
	void* pActiveClass;
	void* pData;
	void (*pUser)(struct _USBH_HandleTypeDef* pHandle, uint8_t id);
} USBH_HandleTypeDef;

typedef struct
{
	const char* Name;
	uint8_t ClassCode;
} USBH_ClassTypeDef;

USBH_StatusTypeDef USBH_Init(USBH_HandleTypeDef* phost, void (*pUsrFunc)(USBH_HandleTypeDef* phost, uint8_t id), uint8_t id);
USBH_StatusTypeDef USBH_DeInit(USBH_HandleTypeDef* phost);
USBH_StatusTypeDef USBH_RegisterClass(USBH_HandleTypeDef* phost, USBH_ClassTypeDef* pclass);
USBH_StatusTypeDef USBH_Start(USBH_HandleTypeDef* phost);
USBH_StatusTypeDef USBH_Stop(USBH_HandleTypeDef* phost);
USBH_StatusTypeDef USBH_Process(USBH_HandleTypeDef* phost);

// The low-level driver: the library implements these (alxUsb_McuStm32.c), the core calls them
USBH_StatusTypeDef USBH_LL_Init(USBH_HandleTypeDef* phost);
USBH_StatusTypeDef USBH_LL_DeInit(USBH_HandleTypeDef* phost);
USBH_StatusTypeDef USBH_LL_Start(USBH_HandleTypeDef* phost);
USBH_StatusTypeDef USBH_LL_Stop(USBH_HandleTypeDef* phost);
USBH_SpeedTypeDef USBH_LL_GetSpeed(USBH_HandleTypeDef* phost);
USBH_StatusTypeDef USBH_LL_ResetPort(USBH_HandleTypeDef* phost);
uint32_t USBH_LL_GetLastXferSize(USBH_HandleTypeDef* phost, uint8_t pipe);
USBH_StatusTypeDef USBH_LL_DriverVBUS(USBH_HandleTypeDef* phost, uint8_t state);
USBH_StatusTypeDef USBH_LL_OpenPipe(USBH_HandleTypeDef* phost, uint8_t pipe, uint8_t epnum, uint8_t dev_address, uint8_t speed, uint8_t ep_type, uint16_t mps);
USBH_StatusTypeDef USBH_LL_ClosePipe(USBH_HandleTypeDef* phost, uint8_t pipe);
USBH_StatusTypeDef USBH_LL_SubmitURB(USBH_HandleTypeDef* phost, uint8_t pipe, uint8_t direction, uint8_t ep_type, uint8_t token, uint8_t* pbuff, uint16_t length, uint8_t do_ping);
USBH_URBStateTypeDef USBH_LL_GetURBState(USBH_HandleTypeDef* phost, uint8_t pipe);
USBH_StatusTypeDef USBH_LL_SetToggle(USBH_HandleTypeDef* phost, uint8_t pipe, uint8_t toggle);
uint8_t USBH_LL_GetToggle(USBH_HandleTypeDef* phost, uint8_t pipe);
void USBH_Delay(uint32_t Delay);

// The core's callbacks, which the driver's HCD callbacks call
void USBH_LL_SetTimer(USBH_HandleTypeDef* phost, uint32_t time);
void USBH_LL_IncTimer(USBH_HandleTypeDef* phost);
void USBH_LL_PortEnabled(USBH_HandleTypeDef* phost);
void USBH_LL_PortDisabled(USBH_HandleTypeDef* phost);
USBH_StatusTypeDef USBH_LL_Connect(USBH_HandleTypeDef* phost);
USBH_StatusTypeDef USBH_LL_Disconnect(USBH_HandleTypeDef* phost);

#endif	// ALX_SHIM_STM32_USB_HOST_USBH_CORE_H
