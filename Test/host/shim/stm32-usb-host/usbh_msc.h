/**
  ******************************************************************************
  * @file		usbh_msc.h
  * @brief		Auralix C Library - Configuration Matrix Fake - STM32 USB Host Library
  * @copyright	Copyright (C) Auralix d.o.o. All rights reserved.
  *
  * Compile-only: the mass storage class of the STM32 USB host library as far as
  * the alx code uses it - the class, the logical unit and its capacity, the
  * block transfers - written from that use, so the configuration matrix
  * compiles every alx line under ALX_USBH (Test/config/matrix.toml). Nothing
  * here is the vendor's and nothing here runs.
  ******************************************************************************
  **/

#ifndef ALX_SHIM_STM32_USB_HOST_USBH_MSC_H
#define ALX_SHIM_STM32_USB_HOST_USBH_MSC_H

#include <stdint.h>
#include "usbh_core.h"

typedef struct
{
	uint32_t block_nbr;
	uint16_t block_size;
} SCSI_CapacityTypeDef;

typedef struct
{
	uint8_t state;
	uint8_t error;
	USBH_StatusTypeDef prev_ready_state;
	SCSI_CapacityTypeDef capacity;
	uint8_t state_changed;
} MSC_LUNTypeDef;

extern USBH_ClassTypeDef USBH_msc;
#define USBH_MSC_CLASS (&USBH_msc)

uint8_t USBH_MSC_IsReady(USBH_HandleTypeDef* phost);
USBH_StatusTypeDef USBH_MSC_GetLUNInfo(USBH_HandleTypeDef* phost, uint8_t lun, MSC_LUNTypeDef* info);
USBH_StatusTypeDef USBH_MSC_Read(USBH_HandleTypeDef* phost, uint8_t lun, uint32_t address, uint8_t* pbuf, uint32_t length);
USBH_StatusTypeDef USBH_MSC_Write(USBH_HandleTypeDef* phost, uint8_t lun, uint32_t address, uint8_t* pbuf, uint32_t length);

#endif	// ALX_SHIM_STM32_USB_HOST_USBH_MSC_H
