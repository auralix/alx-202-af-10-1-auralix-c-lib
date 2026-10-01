/**
  ******************************************************************************
  * @file		stm32l0xx.h
  * @brief		Auralix C Library - Configuration Matrix Fake - STM32 CMSIS and HAL
  * @copyright	Copyright (C) Auralix d.o.o. All rights reserved.
  *
  * Compile-only: the STM32L0 device header as far as the alx code reads it - the
  * family, its CMSIS version (the one alxConfig_TEMPLATE.h requires) and its
  * CAN - and the STM32 fake every family shares (stm32HostShim.h). The HAL and
  * LL headers of the family include this, as theirs include the device header.
  ******************************************************************************
  **/

#ifndef ALX_SHIM_STM32_STM32L0XX_H
#define ALX_SHIM_STM32_STM32L0XX_H

#define STM32L0
#define __STM32L0xx_CMSIS_VERSION_MAIN (0x01U)
#define __STM32L0xx_CMSIS_VERSION_SUB1 (0x01U)
#define __STM32L0xx_CMSIS_VERSION_SUB2 (0x01U)
#define __STM32L0xx_CMSIS_VERSION_RC (0x00U)
#define __STM32L0xx_CMSIS_VERSION ((__STM32L0xx_CMSIS_VERSION_MAIN << 24) | (__STM32L0xx_CMSIS_VERSION_SUB1 << 16) | (__STM32L0xx_CMSIS_VERSION_SUB2 << 8) | (__STM32L0xx_CMSIS_VERSION_RC))
#include "stm32HostShim.h"

#endif	// ALX_SHIM_STM32_STM32L0XX_H
