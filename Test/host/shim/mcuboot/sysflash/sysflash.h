/**
  ******************************************************************************
  * @file		sysflash.h
  * @brief		Auralix C Library - Configuration Matrix Fake - MCUboot
  * @copyright	Copyright (C) Auralix d.o.o. All rights reserved.
  *
  * Compile-only: the MCUboot flash area ids as far as the alx code uses them,
  * written from that use, so the configuration matrix compiles every alx line
  * under ALX_MCU_BOOT (Test/config/matrix.toml). Nothing here is the vendor's
  * and nothing here runs; the real one is the product's.
  *
  * It brings in the MCUboot configuration the way MCUboot's own sysflash.h
  * does, which is how the library's mcuboot_config.h and its user template
  * are compiled.
  ******************************************************************************
  **/

#ifndef ALX_SHIM_MCUBOOT_SYSFLASH_SYSFLASH_H
#define ALX_SHIM_MCUBOOT_SYSFLASH_SYSFLASH_H

#include "mcuboot_config/mcuboot_config.h"

#define PRIMARY_ID 0
#define SECONDARY_ID 1
#define SCRATCH_ID 2
#define FLASH_AREA_IMAGE_PRIMARY(x) (((x) == 0) ? PRIMARY_ID : PRIMARY_ID)
#define FLASH_AREA_IMAGE_SECONDARY(x) (((x) == 0) ? SECONDARY_ID : SECONDARY_ID)
#define FLASH_AREA_IMAGE_SCRATCH SCRATCH_ID

#endif	// ALX_SHIM_MCUBOOT_SYSFLASH_SYSFLASH_H
