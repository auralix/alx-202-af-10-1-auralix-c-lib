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
	#define ALX_MCU_BOOT_FLASH_MAP_BACKEND_TRACE_WRN(...) ALX_TRACE_WRN(ALX_MCU_BOOT_FLASH_MAP_BACKEND_FILE, __VA_ARGS__)
	#define ALX_MCU_BOOT_FLASH_MAP_BACKEND_TRACE_INF(...) ALX_TRACE_INF(ALX_MCU_BOOT_FLASH_MAP_BACKEND_FILE, __VA_ARGS__)
#else
	#define ALX_MCU_BOOT_FLASH_MAP_BACKEND_TRACE_WRN(...) do{} while (false)
	#define ALX_MCU_BOOT_FLASH_MAP_BACKEND_TRACE_INF(...) do{} while (false)
#endif


//******************************************************************************
// Private Defines
//******************************************************************************
#if defined(ALX_STM32F4)
#define HAL_FLASH_PROGRAM_TYPE FLASH_TYPEPROGRAM_WORD
#define HAL_FLASH_PROGRAM_LEN 4
#endif
#if defined(ALX_STM32F7)
#define HAL_FLASH_PROGRAM_TYPE FLASH_TYPEPROGRAM_WORD
#define HAL_FLASH_PROGRAM_LEN 4
#endif
#if defined(ALX_STM32L4)
#define HAL_FLASH_PROGRAM_TYPE FLASH_TYPEPROGRAM_DOUBLEWORD
#define HAL_FLASH_PROGRAM_LEN 8
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
static uint32_t Flash_GetBankNum(uint32_t addr);
static uint32_t Flash_GetSectorNum(uint32_t addr);
static const struct flash_area* Flash_GetArea(uint8_t id);
static bool Flash_Read(uint32_t addr, void* dst, uint32_t len);
static bool Flash_Write(uint32_t addr, const void* src, uint32_t len);
static bool Flash_Erase(uint32_t addr, uint32_t len);


//******************************************************************************
// Functions
//******************************************************************************
int flash_area_open(uint8_t id, const struct flash_area** fapp)
{
	// TV: Copied from: https://github.com/mcu-tools/mcuboot/blob/v2.1.0/boot/espressif/port/esp_mcuboot.c

	// Trace
	ALX_MCU_BOOT_FLASH_MAP_BACKEND_TRACE_INF("%s: ID=%d", __func__, (int)id);

	// Get
	const struct flash_area* area = Flash_GetArea(id);

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
		ALX_MCU_BOOT_FLASH_MAP_BACKEND_TRACE_INF("%s: Out of Bounds (0x%x vs 0x%x)", __func__, (int)end_offset, (int)fap->fa_size);
		return -1;
	}


	//------------------------------------------------------------------------------
	// Read
	//------------------------------------------------------------------------------
	const uint32_t addr = fap->fa_off + off;
//	ALX_MCU_BOOT_FLASH_MAP_BACKEND_TRACE_INF("%s: Addr: 0x%08x Length: %d", __func__, (int)addr, (int)len);	// TV: Uncomment for debug
	bool success = Flash_Read(addr, dst, len);
	if (!success)
	{
		ALX_MCU_BOOT_FLASH_MAP_BACKEND_TRACE_INF("%s: Flash read failed", __func__);
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
		ALX_MCU_BOOT_FLASH_MAP_BACKEND_TRACE_INF("%s: Out of Bounds (0x%x vs 0x%x)", __func__, (int)end_offset, (int)fap->fa_size);
		return -1;
	}


	//------------------------------------------------------------------------------
	// Write
	//------------------------------------------------------------------------------
	const uint32_t addr = fap->fa_off + off;
//	ALX_MCU_BOOT_FLASH_MAP_BACKEND_TRACE_INF("%s: Addr: 0x%08x Length: %d", __func__, (int)addr, (int)len);	// TV: Uncomment for debug
	bool success = Flash_Write(addr, src, len);
	if (!success)
	{
		ALX_MCU_BOOT_FLASH_MAP_BACKEND_TRACE_INF("%s: Flash write failed", __func__);
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
		ALX_MCU_BOOT_FLASH_MAP_BACKEND_TRACE_INF("%s: Not aligned on sector Offset: 0x%x Length: 0x%x", __func__, (int)off, (int)len);
		return -1;
	}


	//------------------------------------------------------------------------------
	// Erase
	//------------------------------------------------------------------------------
	const uint32_t addr = fap->fa_off + off;
//	ALX_MCU_BOOT_FLASH_MAP_BACKEND_TRACE_INF("%s: Addr: 0x%08x Length: %d", __func__, (int)addr, (int)len);	// TV: Uncomment for debug
	bool success = Flash_Erase(addr, len);
	if (!success)
	{
		ALX_MCU_BOOT_FLASH_MAP_BACKEND_TRACE_INF("%s: Flash erase failed", __func__);
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
			ALX_MCU_BOOT_FLASH_MAP_BACKEND_TRACE_INF("%s: Erase at 0x%x Failed", __func__, (int)val);
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
	const struct flash_area* fa = Flash_GetArea(fa_id);
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
static uint32_t Flash_GetBankNum(uint32_t addr)
{
	// Local variables
	uint32_t bankNum = 0;

	// Prepare
	#if defined(ALX_STM32F4) || defined(ALX_STM32L4)
	if ((0x08000000 <= addr) && (addr < 0x08100000))
	{
		bankNum = 1;
	}
	else if ((0x08100000 <= addr) && (addr < 0x08200000))
	{
		bankNum = 2;
	}
	else
	{
		ALX_MCU_BOOT_FLASH_MAP_BACKEND_ASSERT(false);	// We should never get here
	}
	#endif

	// Return
	return bankNum;
}
static uint32_t Flash_GetSectorNum(uint32_t addr)
{
	// Local variables
	uint32_t sectorNum = 0;

	// Prepare
	#if defined(ALX_STM32F4)
	if ((0x08000000 <= addr) && (addr < 0x08010000))
	{
		//sectorNum = 0 + (addr - 0x08000000) / 0x4000;		// Sector 0..3 - 16kB
		ALX_MCU_BOOT_FLASH_MAP_BACKEND_ASSERT(false);		// Not supported
	}
	else if ((0x08010000 <= addr) && (addr < 0x08020000))
	{
		//sectorNum = 4;									// Sector 4 - 64kB
		ALX_MCU_BOOT_FLASH_MAP_BACKEND_ASSERT(false);		// Not supported
	}
	else if ((0x08020000 <= addr) && (addr < 0x08100000))
	{
		sectorNum = 5 + (addr - 0x08020000) / 0x20000;		// Sectors 5..11 - 128kB
	}
	else if ((0x08100000 <= addr) && (addr < 0x08110000))
	{
		//sectorNum = 12 + (addr - 0x08100000) / 0x4000;	// Sector 12..15 - 16kB
		ALX_MCU_BOOT_FLASH_MAP_BACKEND_ASSERT(false);		// Not supported
	}
	else if ((0x08110000 <= addr) && (addr < 0x08120000))
	{
		//sectorNum = 16;									// Sector 16 - 64kB
		ALX_MCU_BOOT_FLASH_MAP_BACKEND_ASSERT(false);		// Not supported
	}
	else if ((0x08120000 <= addr) && (addr < 0x08200000))
	{
		sectorNum = 17 + (addr - 0x08120000) / 0x20000;		// Sectors 5..11 - 128kB
	}
	else
	{
		ALX_MCU_BOOT_FLASH_MAP_BACKEND_ASSERT(false);		// We should never get here
	}
	#endif
	#if defined(ALX_STM32L4)
	sectorNum = (addr - 0x08000000) / ALX_MCU_BOOT_FLASH_SECTOR_SIZE;
	#endif

	// Return
	return sectorNum;
}
static const struct flash_area* Flash_GetArea(uint8_t id)
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
static bool Flash_Read(uint32_t addr, void* dst, uint32_t len)
{
	// TV: Copied from: https://github.com/hcd-bdltd/stm32g4-mcuboot/blob/68c9c7d36feb7a70120a7f94f6ddd3e6907c8a23/boot/src/flash_map_backend/flash_map_backend.c

	// NOTE: Internal flash is memory mapped, so we can just copy the data
	memcpy(dst, (void*)addr, len);

	// Return
	return true;
}
static bool Flash_Write(uint32_t addr, const void* src, uint32_t len)
{
	//------------------------------------------------------------------------------
	// Unlock FLASH
	//------------------------------------------------------------------------------
	HAL_FLASH_Unlock();


	//------------------------------------------------------------------------------
	// Clear FLASH Flags
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
	#if defined(ALX_STM32L4)
	__HAL_FLASH_CLEAR_FLAG
	(
		FLASH_FLAG_EOP |
		FLASH_FLAG_OPERR |
		FLASH_FLAG_PROGERR |
		FLASH_FLAG_WRPERR |
		FLASH_FLAG_PGAERR |
		FLASH_FLAG_SIZERR |
		FLASH_FLAG_PGSERR |
		FLASH_FLAG_MISERR |
		FLASH_FLAG_FASTERR |
		FLASH_FLAG_RDERR |
		FLASH_FLAG_OPTVERR |
		FLASH_FLAG_BSY |
		FLASH_FLAG_ECCC |
		FLASH_FLAG_ECCD
	);
	if (__HAL_FLASH_GET_FLAG(FLASH_FLAG_PEMPTY) != 0)
	{
		__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_PEMPTY);
	}
	#endif


	//------------------------------------------------------------------------------
	// Write FLASH
	//------------------------------------------------------------------------------
	for (uint32_t i = 0; i < len; i = i + HAL_FLASH_PROGRAM_LEN)
	{
		// Prepare
		uint64_t data = 0;
		memcpy(&data, src + i, HAL_FLASH_PROGRAM_LEN);

		// Write
		HAL_StatusTypeDef status = HAL_FLASH_Program(HAL_FLASH_PROGRAM_TYPE, addr + i, data);
		if (status != HAL_OK)
		{
			// Trace
			ALX_MCU_BOOT_FLASH_MAP_BACKEND_TRACE_INF("%s: HAL_FLASH_Program failed", __func__);

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
static bool Flash_Erase(uint32_t addr, uint32_t len)
{
	//------------------------------------------------------------------------------
	// Unlock FLASH
	//------------------------------------------------------------------------------
	HAL_FLASH_Unlock();


	//------------------------------------------------------------------------------
	// Clear FLASH Flags
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
	#if defined(ALX_STM32L4)
	__HAL_FLASH_CLEAR_FLAG
	(
		FLASH_FLAG_EOP |
		FLASH_FLAG_OPERR |
		FLASH_FLAG_PROGERR |
		FLASH_FLAG_WRPERR |
		FLASH_FLAG_PGAERR |
		FLASH_FLAG_SIZERR |
		FLASH_FLAG_PGSERR |
		FLASH_FLAG_MISERR |
		FLASH_FLAG_FASTERR |
		FLASH_FLAG_RDERR |
		FLASH_FLAG_OPTVERR |
		FLASH_FLAG_BSY |
		FLASH_FLAG_ECCC |
		FLASH_FLAG_ECCD
	);
	if (__HAL_FLASH_GET_FLAG(FLASH_FLAG_PEMPTY) != 0)
	{
		__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_PEMPTY);
	}
	#endif


	//------------------------------------------------------------------------------
	// Prepare
	//------------------------------------------------------------------------------
	uint32_t bankNum = Flash_GetBankNum(addr);
	uint32_t byteAddrFirst = addr;
	uint32_t byteAddrLast = addr + len - 1;
	uint32_t sectorNumFirst = Flash_GetSectorNum(byteAddrFirst);
	uint32_t sectorNumLast = Flash_GetSectorNum(byteAddrLast);
	uint32_t numOfSectorsToErase = sectorNumLast - sectorNumFirst + 1;
	FLASH_EraseInitTypeDef eraseInitStruct = {};
	uint32_t sectorError = 0;
	#if defined(ALX_STM32F4)
	eraseInitStruct.TypeErase = FLASH_TYPEERASE_SECTORS;
	eraseInitStruct.Banks = bankNum;
	eraseInitStruct.Sector = sectorNumFirst;
	eraseInitStruct.NbSectors = numOfSectorsToErase;
	eraseInitStruct.VoltageRange = FLASH_VOLTAGE_RANGE_3;
	#endif
	#if defined(ALX_STM32L4)
	eraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;
	eraseInitStruct.Banks = bankNum;
	eraseInitStruct.Page = sectorNumFirst;
	eraseInitStruct.NbPages = numOfSectorsToErase;
	#endif


	//------------------------------------------------------------------------------
	// Erase FLASH
	//------------------------------------------------------------------------------
	if (HAL_FLASHEx_Erase(&eraseInitStruct, &sectorError) != HAL_OK)
	{
		// Trace
		ALX_MCU_BOOT_FLASH_MAP_BACKEND_TRACE_INF("%s: HAL_FLASHEx_Erase failed error: %d", __func__, (int)sectorError);

		// Lock FLASH
		HAL_FLASH_Lock();

		// Return
		return false;
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


#endif	// #if defined(ALX_C_LIB) && defined(ALX_MCU_BOOT)
