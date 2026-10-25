/**
  ******************************************************************************
  * @file		flash_map_backend.h
  * @brief		Auralix C Library - Configuration Matrix Fake - MCUboot
  * @copyright	Copyright (C) Auralix d.o.o. All rights reserved.
  *
  * Compile-only: the MCUboot flash map API as far as the alx code uses it,
  * written from that use, so the configuration matrix compiles every alx line
  * under ALX_MCU_BOOT (Test/config/matrix.toml). Nothing here is the vendor's
  * and nothing here runs; the real one is the product's.
  *
  * The library implements this API itself (mcuboot/flash_map_backend), so the
  * prototypes are the ones its definitions have: a fake that disagreed with
  * them would be a compile error there, which is the check this header makes.
  ******************************************************************************
  **/

#ifndef ALX_SHIM_MCUBOOT_FLASH_MAP_BACKEND_FLASH_MAP_BACKEND_H
#define ALX_SHIM_MCUBOOT_FLASH_MAP_BACKEND_FLASH_MAP_BACKEND_H

#include <stdint.h>

struct flash_area
{
	uint8_t fa_id;
	uint8_t fa_device_id;
	uint16_t pad16;
	uint32_t fa_off;
	uint32_t fa_size;
};

struct flash_sector
{
	uint32_t fs_off;
	uint32_t fs_size;
};

int flash_area_open(uint8_t id, const struct flash_area** fapp);
void flash_area_close(const struct flash_area* fap);
int flash_area_read(const struct flash_area* fap, uint32_t off, void* dst, uint32_t len);
int flash_area_write(const struct flash_area* fap, uint32_t off, const void* src, uint32_t len);
int flash_area_erase(const struct flash_area* fap, uint32_t off, uint32_t len);
uint32_t flash_area_align(const struct flash_area* fap);
uint8_t flash_area_erased_val(const struct flash_area* fap);
int flash_area_get_sectors(int fa_id, uint32_t* count, struct flash_sector* sectors);
int flash_area_id_from_image_slot(int slot);
int flash_area_id_from_multi_image_slot(int image_index, int slot);
int flash_area_id_to_multi_image_slot(int image_index, int area_id);

#endif	// ALX_SHIM_MCUBOOT_FLASH_MAP_BACKEND_FLASH_MAP_BACKEND_H
