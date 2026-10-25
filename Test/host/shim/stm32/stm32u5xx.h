/**
  ******************************************************************************
  * @file		stm32u5xx.h
  * @brief		Auralix C Library - Configuration Matrix Fake - STM32 CMSIS and HAL
  * @copyright	Copyright (C) Auralix d.o.o. All rights reserved.
  *
  * Compile-only: the STM32U5 device header as far as the alx code reads it - the
  * family, its CMSIS version (the one alxConfig_TEMPLATE.h requires) and its
  * CAN - and the STM32 fake every family shares (stm32HostShim.h). The HAL and
  * LL headers of the family include this, as theirs include the device header.
  ******************************************************************************
  **/

#ifndef ALX_SHIM_STM32_STM32U5XX_H
#define ALX_SHIM_STM32_STM32U5XX_H

#define STM32U5
#define __STM32U5_CMSIS_VERSION_MAIN (0x01U)
#define __STM32U5_CMSIS_VERSION_SUB1 (0x03U)
#define __STM32U5_CMSIS_VERSION_SUB2 (0x00U)
#define __STM32U5_CMSIS_VERSION_RC (0x00U)
#define __STM32U5_CMSIS_VERSION ((__STM32U5_CMSIS_VERSION_MAIN << 24) | (__STM32U5_CMSIS_VERSION_SUB1 << 16) | (__STM32U5_CMSIS_VERSION_SUB2 << 8) | (__STM32U5_CMSIS_VERSION_RC))
#define HAL_FDCAN_MODULE_ENABLED
#include "stm32HostShim.h"

#endif	// ALX_SHIM_STM32_STM32U5XX_H
