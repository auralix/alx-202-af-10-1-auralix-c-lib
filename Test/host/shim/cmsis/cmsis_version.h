/**
  ******************************************************************************
  * @file		cmsis_version.h
  * @brief		Auralix C Library - Configuration Matrix Fake - CMSIS Core
  * @copyright	Copyright (C) Auralix d.o.o. All rights reserved.
  *
  * Compile-only: the CMSIS core version, the one alxConfig_TEMPLATE.h requires
  * (ALX_CMSIS_CORE_VER_REQUIRED, 5.1), so alxId.c's check of it compiles.
  ******************************************************************************
  **/

#ifndef ALX_SHIM_CMSIS_CMSIS_VERSION_H
#define ALX_SHIM_CMSIS_CMSIS_VERSION_H

#define __CM_CMSIS_VERSION_MAIN (5U)
#define __CM_CMSIS_VERSION_SUB (1U)
#define __CM_CMSIS_VERSION ((__CM_CMSIS_VERSION_MAIN << 16U) | __CM_CMSIS_VERSION_SUB)

#endif	// ALX_SHIM_CMSIS_CMSIS_VERSION_H
