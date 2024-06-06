/**
  ******************************************************************************
  * @file		flash_map_backend.c
  * @brief		Auralix C Library - ALX MCUboot FLASH Map Backend Module
  * @copyright	Copyright (C) Auralix d.o.o. All rights reserved.
  *
  * @section License
  *
  * SPDX-License-Identifier: GPL-3.0-or-later
  *
  * This file is part of Auralix C Library.
  *
  * Auralix C Library is free software: you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation, either version 3
  * of the License, or (at your option) any later version.
  *
  * Auralix C Library is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  * GNU General Public License for more details.
  *
  * You should have received a copy of the GNU General Public License
  * along with Auralix C Library. If not, see <https://www.gnu.org/licenses/>.
  ******************************************************************************
  **/

//******************************************************************************
// Includes
//******************************************************************************
#include "alxGlobal.h"
#include "alxTrace.h"
#include "alxAssert.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB) && defined(ALX_MCU_BOOT)


//******************************************************************************
// Preprocessor
//******************************************************************************
#define ALX_MCU_BOOT_FLASH_MAP_BACKEND_FILE "flash_map_backend.c"

// Assert //
#if defined(ALX_MCU_BOOT_FLASH_MAP_BACKEND_ASSERT_BKPT_ENABLE)
	#define ALX_MCU_BOOT_FLASH_MAP_BACKEND_ASSERT(expr) ALX_ASSERT_BKPT(ALX_MCU_BOOT_FLASH_MAP_BACKEND_FILE, expr)
#elif defined(ALX_MCU_BOOT_FLASH_MAP_BACKEND_ASSERT_TRACE_ENABLE)
	#define ALX_MCU_BOOT_FLASH_MAP_BACKEND_ASSERT(expr) ALX_ASSERT_TRACE(ALX_MCU_BOOT_FLASH_MAP_BACKEND_FILE, expr)
#elif defined(ALX_MCU_BOOT_FLASH_MAP_BACKEND_ASSERT_RST_ENABLE)
	#define ALX_MCU_BOOT_FLASH_MAP_BACKEND_ASSERT(expr) ALX_ASSERT_RST(ALX_MCU_BOOT_FLASH_MAP_BACKEND_FILE, expr)
#else
	#define ALX_MCU_BOOT_FLASH_MAP_BACKEND_ASSERT(expr) do{} while (false)
#endif

// Trace //
#if defined(ALX_MCU_BOOT_FLASH_MAP_BACKEND_TRACE_ENABLE)
	#define ALX_MCU_BOOT_FLASH_MAP_BACKEND_TRACE(...) ALX_TRACE_STD(ALX_MCU_BOOT_FLASH_MAP_BACKEND_FILE, __VA_ARGS__)
	#define ALX_MCU_BOOT_FLASH_MAP_BACKEND_TRACE_FORMAT(...) ALX_TRACE_FORMAT(__VA_ARGS__)
#else
	#define ALX_MCU_BOOT_FLASH_MAP_BACKEND_TRACE(...) do{} while (false)
	#define ALX_MCU_BOOT_FLASH_MAP_BACKEND_TRACE_FORMAT(...) do{} while (false)
#endif


//******************************************************************************
// Private Defines
//******************************************************************************
#if defined(ALX_STM32F4)
#define HAL_FLASH_PROGRAM_TYPE FLASH_TYPEPROGRAM_WORD
#define HAL_FLASH_PROGRAM_LEN 4
#endif


//******************************************************************************
// Private Variables
//******************************************************************************
static const struct flash_area bootloader =
{
	.fa_id = 255,
	.fa_device_id = ALX_MCU_BOOT_FLASH_DEVICE_ID,
	.fa_off = ALX_MCU_BOOT_BOOTLOADER_OFFSET,
	.fa_size = ALX_MCU_BOOT_BOOTLOADER_SIZE,
};
static const struct flash_area primary_img =
{
	.fa_id = PRIMARY_ID,
	.fa_device_id = ALX_MCU_BOOT_FLASH_DEVICE_ID,
	.fa_off = ALX_MCU_BOOT_IMAGE_PRIMARY_OFFSET,
	.fa_size = ALX_MCU_BOOT_IMAGE_SIZE,
};
static const struct flash_area secondary_img =
{
	.fa_id = SECONDARY_ID,
	.fa_device_id = ALX_MCU_BOOT_FLASH_DEVICE_ID,
	.fa_off = ALX_MCU_BOOT_IMAGE_SECONDARY_OFFSET,
	.fa_size = ALX_MCU_BOOT_IMAGE_SIZE,
};
static const struct flash_area* s_flash_areas[] =
{
	&bootloader,
	&primary_img,
	&secondary_img
};


//******************************************************************************
// Private Functions
//******************************************************************************
static uint32_t prv_get_flash_page(uint32_t addr);
static const struct flash_area* prv_lookup_flash_area(uint8_t id);
static bool prv_flash_read(uint32_t addr, void* dst, uint32_t len);
static bool prv_flash_write(uint32_t addr, const void* src, uint32_t len);
static bool prv_flash_erase(uint32_t addr, uint32_t len);


//******************************************************************************
// Functions
//******************************************************************************
int flash_area_open(uint8_t id, const struct flash_area** fapp)
{
	// TV: Copied from: https://github.com/mcu-tools/mcuboot/blob/v2.1.0/boot/espressif/port/esp_mcuboot.c

	// Trace
	ALX_MCU_BOOT_FLASH_MAP_BACKEND_TRACE_FORMAT("%s: ID=%d\r\n", __func__, (int)id);

	// Get
	const struct flash_area* area = prv_lookup_flash_area(id);

	// Return
	*fapp = area;
	return area != NULL ? 0 : -1;
}
void flash_area_close(const struct flash_area* fap)
{
	// TV: Copied from: https://github.com/mcu-tools/mcuboot/blob/v2.1.0/boot/espressif/port/esp_mcuboot.c

	// Unused
	(void)fap;
}
int flash_area_read(const struct flash_area* fap, uint32_t off, void* dst, uint32_t len)
{
	// TV: Copied from: https://github.com/hcd-bdltd/stm32g4-mcuboot/blob/68c9c7d36feb7a70120a7f94f6ddd3e6907c8a23/boot/src/flash_map_backend/flash_map_backend.c


	//------------------------------------------------------------------------------
	// Check
	//------------------------------------------------------------------------------
	if (fap->fa_device_id != ALX_MCU_BOOT_FLASH_DEVICE_ID)
	{
		return -1;
	}
	const uint32_t end_offset = off + len;
	if (end_offset > fap->fa_size)
	{
		ALX_MCU_BOOT_FLASH_MAP_BACKEND_TRACE_FORMAT("%s: Out of Bounds (0x%x vs 0x%x)\r\n", __func__, (int)end_offset, (int)fap->fa_size);
		return -1;
	}


	//------------------------------------------------------------------------------
	// Read
	//------------------------------------------------------------------------------
	const uint32_t addr = fap->fa_off + off;
//	ALX_MCU_BOOT_FLASH_MAP_BACKEND_TRACE_FORMAT("%s: Addr: 0x%08x Length: %d\r\n", __func__, (int)addr, (int)len);	// TV: Uncomment for debug
	bool success = prv_flash_read(addr, dst, len);
	if (!success)
	{
		ALX_MCU_BOOT_FLASH_MAP_BACKEND_TRACE_FORMAT("%s: Flash read failed\r\n", __func__);
		return -1;
	}


	//------------------------------------------------------------------------------
	// Return
	//------------------------------------------------------------------------------
	return 0;
}
int flash_area_write(const struct flash_area* fap, uint32_t off, const void* src, uint32_t len)
{
	// TV: Copied from: https://github.com/hcd-bdltd/stm32g4-mcuboot/blob/68c9c7d36feb7a70120a7f94f6ddd3e6907c8a23/boot/src/flash_map_backend/flash_map_backend.c


	//------------------------------------------------------------------------------
	// Check
	//------------------------------------------------------------------------------
	if (fap->fa_device_id != ALX_MCU_BOOT_FLASH_DEVICE_ID)
	{
		return -1;
	}
	const uint32_t end_offset = off + len;
	if (end_offset > fap->fa_size)
	{
		ALX_MCU_BOOT_FLASH_MAP_BACKEND_TRACE_FORMAT("%s: Out of Bounds (0x%x vs 0x%x)\r\n", __func__, (int)end_offset, (int)fap->fa_size);
		return -1;
	}


	//------------------------------------------------------------------------------
	// Write
	//------------------------------------------------------------------------------
	const uint32_t addr = fap->fa_off + off;
//	ALX_MCU_BOOT_FLASH_MAP_BACKEND_TRACE_FORMAT("%s: Addr: 0x%08x Length: %d\r\n", __func__, (int)addr, (int)len);	// TV: Uncomment for debug
	bool success = prv_flash_write(addr, src, len);
	if (!success)
	{
		ALX_MCU_BOOT_FLASH_MAP_BACKEND_TRACE_FORMAT("%s: Flash write failed\r\n", __func__);
		return -1;
	}


	//------------------------------------------------------------------------------
	// Return
	//------------------------------------------------------------------------------
	return 0;
}
int flash_area_erase(const struct flash_area* fap, uint32_t off, uint32_t len)
{
	// TV: Copied from: https://github.com/hcd-bdltd/stm32g4-mcuboot/blob/68c9c7d36feb7a70120a7f94f6ddd3e6907c8a23/boot/src/flash_map_backend/flash_map_backend.c


	//------------------------------------------------------------------------------
	// Check
	//------------------------------------------------------------------------------
	if (fap->fa_device_id != ALX_MCU_BOOT_FLASH_DEVICE_ID)
	{
		return -1;
	}
	if ((len % ALX_MCU_BOOT_FLASH_SECTOR_SIZE) != 0 || (off % ALX_MCU_BOOT_FLASH_SECTOR_SIZE) != 0)
	{
		ALX_MCU_BOOT_FLASH_MAP_BACKEND_TRACE_FORMAT("%s: Not aligned on sector Offset: 0x%x Length: 0x%x\r\n", __func__, (int)off, (int)len);
		return -1;
	}


	//------------------------------------------------------------------------------
	// Erase
	//------------------------------------------------------------------------------
	const uint32_t addr = fap->fa_off + off;
//	ALX_MCU_BOOT_FLASH_MAP_BACKEND_TRACE_FORMAT("%s: Addr: 0x%08x Length: %d\r\n", __func__, (int)addr, (int)len);	// TV: Uncomment for debug
	bool success = prv_flash_erase(addr, len);
	if (!success)
	{
		ALX_MCU_BOOT_FLASH_MAP_BACKEND_TRACE_FORMAT("%s: Flash erase failed\r\n", __func__);
		return -1;
	}


	//------------------------------------------------------------------------------
	// Check
	//------------------------------------------------------------------------------
	for (size_t i = 0; i < len; i++)
	{
		uint8_t* val = (void*)(addr + i);
		if (*val != 0xff)
		{
			ALX_MCU_BOOT_FLASH_MAP_BACKEND_TRACE_FORMAT("%s: Erase at 0x%x Failed\r\n", __func__, (int)val);
			ALX_MCU_BOOT_FLASH_MAP_BACKEND_ASSERT(false);
		}
	}


	//------------------------------------------------------------------------------
	// Return
	//------------------------------------------------------------------------------
	return 0;
}
uint32_t flash_area_align(const struct flash_area* fap)
{
	// TV: Copied from: https://github.com/hcd-bdltd/stm32g4-mcuboot/blob/68c9c7d36feb7a70120a7f94f6ddd3e6907c8a23/boot/src/flash_map_backend/flash_map_backend.c

	// Unused
	(void)fap;

	// NOTE: The smallest unit a flash write can occur along, image trailers will be scaled by this size

	// Return
	return HAL_FLASH_PROGRAM_LEN;
}
uint8_t flash_area_erased_val(const struct flash_area* fap)
{
	// TV: Copied from: https://github.com/mcu-tools/mcuboot/blob/v2.1.0/boot/espressif/port/esp_mcuboot.c

	// Unused
	(void)fap;

	// Return
	return 0xFF;
}
int flash_area_get_sectors(int fa_id, uint32_t* count, struct flash_sector* sectors)
{
	// TV: Copied from: https://github.com/mcu-tools/mcuboot/blob/v2.1.0/boot/espressif/port/esp_mcuboot.c


	//------------------------------------------------------------------------------
	// Check
	//------------------------------------------------------------------------------
	const struct flash_area* fa = prv_lookup_flash_area(fa_id);
	if (fa->fa_device_id != ALX_MCU_BOOT_FLASH_DEVICE_ID)
	{
		return -1;
	}


	//------------------------------------------------------------------------------
	// Get
	//------------------------------------------------------------------------------
	const size_t sector_size = ALX_MCU_BOOT_FLASH_SECTOR_SIZE;
	uint32_t total_count = 0;
	for (size_t off = 0; off < fa->fa_size; off = off + sector_size)
	{
		// NOTE: Offset here is relative to flash area, not device
		sectors[total_count].fs_off = off;
		sectors[total_count].fs_size = sector_size;
		total_count++;
	}


	//------------------------------------------------------------------------------
	// Return
	//------------------------------------------------------------------------------
	*count = total_count;
	return 0;
}
int flash_area_id_from_image_slot(int slot)
{
	// TV: Copied from: https://github.com/mcu-tools/mcuboot/blob/v2.1.0/boot/espressif/port/esp_mcuboot.c

	return flash_area_id_from_multi_image_slot(0, slot);
}
int flash_area_id_from_multi_image_slot(int image_index, int slot)
{
	// TV: Copied from: https://github.com/hcd-bdltd/stm32g4-mcuboot/blob/68c9c7d36feb7a70120a7f94f6ddd3e6907c8a23/boot/src/flash_map_backend/flash_map_backend.c

	switch (slot)
	{
		case 0:
		{
			return FLASH_AREA_IMAGE_PRIMARY(image_index);
		}
		case 1:
		{
			return FLASH_AREA_IMAGE_SECONDARY(image_index);
		}
		case 2:
		{
			return FLASH_AREA_IMAGE_SCRATCH;
		}
	}

	return -1;
}
int flash_area_id_to_multi_image_slot(int image_index, int area_id)
{
	// TV: TODO

	ALX_MCU_BOOT_FLASH_MAP_BACKEND_ASSERT(false);
	return -1;
}


//******************************************************************************
// Private Functions
//******************************************************************************
static uint32_t prv_get_flash_page(uint32_t addr)
{
	// TV: Copied from: https://github.com/hcd-bdltd/stm32g4-mcuboot/blob/68c9c7d36feb7a70120a7f94f6ddd3e6907c8a23/boot/src/flash_map_backend/flash_map_backend.c

	return (addr - FLASH_BASE) / ALX_MCU_BOOT_FLASH_SECTOR_SIZE;
}
static const struct flash_area* prv_lookup_flash_area(uint8_t id)
{
	// TV: Copied from: https://github.com/hcd-bdltd/stm32g4-mcuboot/blob/68c9c7d36feb7a70120a7f94f6ddd3e6907c8a23/boot/src/flash_map_backend/flash_map_backend.c

	for (size_t i = 0; i < ALX_ARR_LEN(s_flash_areas); i++)
	{
		const struct flash_area* area = s_flash_areas[i];
		if (id == area->fa_id)
		{
			return area;
		}
	}

	return NULL;
}
static bool prv_flash_read(uint32_t addr, void* dst, uint32_t len)
{
	// TV: Copied from: https://github.com/hcd-bdltd/stm32g4-mcuboot/blob/68c9c7d36feb7a70120a7f94f6ddd3e6907c8a23/boot/src/flash_map_backend/flash_map_backend.c

	// NOTE: Internal flash is memory mapped, so we can just copy the data
	memcpy(dst, (void*)addr, len);

	// Return
	return true;
}
static bool prv_flash_write(uint32_t addr, const void* src, uint32_t len)
{
	//------------------------------------------------------------------------------
	// Unlock FLASH
	//------------------------------------------------------------------------------
	HAL_FLASH_Unlock();


	//------------------------------------------------------------------------------
	// Clear All FLASH Flags
	//------------------------------------------------------------------------------
	#if defined(ALX_STM32F4)
	__HAL_FLASH_CLEAR_FLAG
	(
		FLASH_FLAG_EOP |
		FLASH_FLAG_OPERR |
		FLASH_FLAG_WRPERR |
		FLASH_FLAG_PGAERR |
		FLASH_FLAG_PGPERR |
		FLASH_FLAG_PGSERR |
		FLASH_FLAG_RDERR |
		FLASH_FLAG_BSY
	);
	#endif


	//------------------------------------------------------------------------------
	// Write FLASH
	//------------------------------------------------------------------------------
	for (uint32_t i = 0; i < len; i = i + HAL_FLASH_PROGRAM_LEN)
	{
		// Prepare
		uint32_t data = 0;
		memcpy(&data, src + i, HAL_FLASH_PROGRAM_LEN);

		// Write
		HAL_StatusTypeDef status = HAL_FLASH_Program(HAL_FLASH_PROGRAM_TYPE, addr + i, data);
		if (status != HAL_OK)
		{
			// Trace
			ALX_MCU_BOOT_FLASH_MAP_BACKEND_TRACE_FORMAT("%s: HAL_FLASH_Program failed\r\n", __func__);

			// Lock FLASH
			HAL_FLASH_Lock();

			// Return
			return false;
		}
	}


	//------------------------------------------------------------------------------
	// Lock FLASH
	//------------------------------------------------------------------------------
	HAL_FLASH_Lock();


	//------------------------------------------------------------------------------
	// Return
	//------------------------------------------------------------------------------
	return true;
}
static bool prv_flash_erase(uint32_t addr, uint32_t len)
{
	// Unlock FLASH
	HAL_FLASH_Unlock();

	// Clear all FLASH flags
	#if defined(ALX_STM32F4)
	__HAL_FLASH_CLEAR_FLAG
	(
		FLASH_FLAG_EOP |
		FLASH_FLAG_OPERR |
		FLASH_FLAG_WRPERR |
		FLASH_FLAG_PGAERR |
		FLASH_FLAG_PGPERR |
		FLASH_FLAG_PGSERR |
		FLASH_FLAG_RDERR |
		FLASH_FLAG_BSY
	);
	#endif

	// Prepare
	#if defined(ALX_STM32F4)
	uint32_t firstSector = 0;
	uint32_t lastSector = 0;
	uint32_t numOfSectorsToErase = 0;
	if (addr < 0x08100000)
	{
		firstSector = prv_get_flash_page(addr) + 4;
		lastSector = prv_get_flash_page(addr + len - 1) + 4;
		numOfSectorsToErase = lastSector - firstSector + 1;
	}
	else
	{
		firstSector = prv_get_flash_page(addr) + 8;
		lastSector = prv_get_flash_page(addr + len - 1) + 8;
		numOfSectorsToErase = lastSector - firstSector + 1;
	}
	FLASH_EraseInitTypeDef eraseInitStruct =
	{
		.TypeErase = FLASH_TYPEERASE_SECTORS,
		.Banks = ALX_NULL,
		.Sector = firstSector,
		.NbSectors = numOfSectorsToErase,
		.VoltageRange = FLASH_VOLTAGE_RANGE_3,
	};
	uint32_t sectorError = 0;
	#endif

	// Erase
	#if defined(ALX_STM32F4)
	if (HAL_FLASHEx_Erase(&eraseInitStruct, &sectorError) != HAL_OK)
	{
		// Trace
		ALX_MCU_BOOT_FLASH_MAP_BACKEND_TRACE_FORMAT("%s: HAL_FLASHEx_Erase failed error: %d\r\n", __func__, (int)sectorError);

		// Lock FLASH
		HAL_FLASH_Lock();

		// Return
		return false;
	}
	#endif

	// Lock FLASH
	HAL_FLASH_Lock();

	// Return
	return true;
}


#endif	// #if defined(ALX_C_LIB) && defined(ALX_MCU_BOOT)
