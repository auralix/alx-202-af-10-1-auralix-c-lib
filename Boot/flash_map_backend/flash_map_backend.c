/**
 * @file flash_map_backend.c
 * @author B Duncan LTD (info@bduncanltd.com)
 * @brief Flash map backend for MCUboot
 * @version 0.1
 * @date 2024-03-14
 *
 * @copyright Copyright (c) 2024
 *
 */

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "bootutil/bootutil.h"
#include "bootutil/bootutil_log.h"

#include "flash_map_backend/flash_map_backend.h"
#include "sysflash/sysflash.h"

#include "alxGlobal_McuStm32F4.h"
#include "alxTrace.h"

#define FLASH_PAGE_SIZE (128 * 1024)
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof(arr[0]))
#define MCUBOOT_LOG_DBG(_fmt, ...) do{AlxTrace_WriteFormat(&alxTrace, _fmt "\r\n", ##__VA_ARGS__);} while (false)
#define BOOTLOADER_SIZE (128 * 1024)
#define IMAGE_SIZE      (896 * 1024)
#define BOOTLOADER_OFFSET      (FLASH_BASE)
#define IMAGE_PRIMARY_OFFSET   (0x08020000UL)
#define IMAGE_SECONDARY_OFFSET (0x08120000UL)
#define FLASH_AREA_BOOTLOADER 255
#define FLASH_DEVICE_INTERNAL_FLASH 0

static const struct flash_area bootloader = {
	.fa_id = FLASH_AREA_BOOTLOADER,
	.fa_device_id = FLASH_DEVICE_INTERNAL_FLASH,
	.fa_off = BOOTLOADER_OFFSET,
	.fa_size = BOOTLOADER_SIZE,
};

static const struct flash_area primary_img = {
	.fa_id = PRIMARY_ID,	// TV: Changed
	.fa_device_id = FLASH_DEVICE_INTERNAL_FLASH,
	.fa_off = IMAGE_PRIMARY_OFFSET,
	.fa_size = IMAGE_SIZE,
};

static const struct flash_area secondary_img = {
	.fa_id = SECONDARY_ID,	// TV: Changed
	.fa_device_id = FLASH_DEVICE_INTERNAL_FLASH,
	.fa_off = IMAGE_SECONDARY_OFFSET,
	.fa_size = IMAGE_SIZE,
};

// static const struct flash_area scratch_img = {	// TV: Removed

static const struct flash_area *s_flash_areas[] = {
	&bootloader,
	&primary_img,
	&secondary_img	// TV: Changed
};

static uint32_t prv_get_flash_page(uint32_t addr)
{
	return (addr - FLASH_BASE) / FLASH_PAGE_SIZE;
}

static bool prv_flash_erase(uint32_t addr, uint32_t len)	// TV: Changed
{
	uint32_t first_page = 0;
	uint32_t last_page = 0;
	uint32_t nb_pages = 0;

	if (addr < 0x08100000)
	{
		first_page = prv_get_flash_page(addr) + 4;
		last_page = prv_get_flash_page(addr + len - 1) + 4;
		nb_pages = last_page - first_page + 1;
	}
	else
	{
		first_page = prv_get_flash_page(addr) + 8;
		last_page = prv_get_flash_page(addr + len - 1) + 8;
		nb_pages = last_page - first_page + 1;
	}

	uint32_t page_error = 0;
	FLASH_EraseInitTypeDef erase_init = {
		.TypeErase = FLASH_TYPEERASE_SECTORS,
		.Banks = ALX_NULL,
		.Sector = first_page,
		.NbSectors = nb_pages,
		.VoltageRange = FLASH_VOLTAGE_RANGE_3,
	};

	HAL_FLASH_Unlock();
	if (HAL_FLASHEx_Erase(&erase_init, &page_error) != HAL_OK) {
		BOOT_LOG_ERR("%s: HAL_FLASHEx_Erase failed error: %d", __func__, (int)page_error);
		HAL_FLASH_Lock();
		return false;
	}

	HAL_FLASH_Lock();
	return true;
}

static bool prv_flash_write(uint32_t addr, const void *src, uint32_t len)	// TV: Changed
{
	HAL_StatusTypeDef status;
	uint32_t word;

	HAL_FLASH_Unlock();

	for (uint32_t i = 0; i < len; i += sizeof(uint32_t)) {

		memcpy(&word, src + i, sizeof(uint32_t));
		status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, addr + i, word);

		if (status != HAL_OK) {
			BOOT_LOG_ERR("%s: HAL_FLASH_Program failed", __func__);
			HAL_FLASH_Lock();
			return false;
		}
	}

	HAL_FLASH_Lock();
	return true;
}

static bool prv_flash_read(uint32_t addr, void *dst, uint32_t len)
{
	// Internal flash is memory mapped, so we can just copy the data
	memcpy(dst, (void *)addr, len);
	return true;
}

static const struct flash_area *prv_lookup_flash_area(uint8_t id)
{
	for (size_t i = 0; i < ARRAY_SIZE(s_flash_areas); i++) {
		const struct flash_area *area = s_flash_areas[i];
		if (id == area->fa_id) {
			return area;
		}
	}
	return NULL;
}

// int flash_device_base(uint8_t fd_id, uintptr_t *ret)		// TV: Removed

int flash_area_open(uint8_t id, const struct flash_area **fa)
{
	BOOT_LOG_DBG("%s: ID=%d", __func__, (int)id);
	const struct flash_area *area = prv_lookup_flash_area(id);
	*fa = area;
	return area != NULL ? 0 : -1;
}

void flash_area_close(const struct flash_area *fa)
{
	UNUSED(fa);
	// Nothing to do
}

int flash_area_read(const struct flash_area *fa, uint32_t off, void *dst, uint32_t len)
{
	if (fa->fa_device_id != FLASH_DEVICE_INTERNAL_FLASH) {
		return -1;
	}

	const uint32_t end_offset = off + len;
	if (end_offset > fa->fa_size) {
		BOOT_LOG_ERR("%s: Out of Bounds (0x%x vs 0x%x)", __func__, (int)end_offset,
			     (int)fa->fa_size);
		return -1;
	}

	const uint32_t addr = fa->fa_off + off;
	MCUBOOT_LOG_DBG("%s: Addr: 0x%08x Length: %d", __func__, (int)addr, (int)len);
	bool success = prv_flash_read(addr, dst, len);
	if (!success) {
		BOOT_LOG_ERR("%s: Flash read failed", __func__);
		return -1;
	}

	return 0;
}

int flash_area_write(const struct flash_area *fa, uint32_t off, const void *src, uint32_t len)
{
	if (fa->fa_device_id != FLASH_DEVICE_INTERNAL_FLASH) {
		return -1;
	}

	const uint32_t end_offset = off + len;
	if (end_offset > fa->fa_size) {
		BOOT_LOG_ERR("%s: Out of Bounds (0x%x vs 0x%x)", __func__, (int)end_offset,
			     (int)fa->fa_size);
		return -1;
	}

	const uint32_t addr = fa->fa_off + off;
	MCUBOOT_LOG_DBG("%s: Addr: 0x%08x Length: %d", __func__, (int)addr, (int)len);
	bool success = prv_flash_write(addr, src, len);
	if (!success) {
		BOOT_LOG_ERR("%s: Flash write failed", __func__);
		return -1;
	}

	return 0;
}

int flash_area_erase(const struct flash_area *fa, uint32_t off, uint32_t len)
{
	if (fa->fa_device_id != FLASH_DEVICE_INTERNAL_FLASH) {
		return -1;
	}

	if ((len % FLASH_PAGE_SIZE) != 0 || (off % FLASH_PAGE_SIZE) != 0) {
		BOOT_LOG_ERR("%s: Not aligned on sector Offset: 0x%x Length: 0x%x", __func__,
			     (int)off, (int)len);
		return -1;
	}

	const uint32_t addr = fa->fa_off + off;
	MCUBOOT_LOG_DBG("%s: Addr: 0x%08x Length: %d", __func__, (int)addr, (int)len);
	bool success = prv_flash_erase(addr, len);
	if (!success) {
		BOOT_LOG_ERR("%s: Flash erase failed", __func__);
		return -1;
	}

	for (size_t i = 0; i < len; i++) {
		uint8_t *val = (void *)(addr + i);
		if (*val != 0xff) {
			BOOT_LOG_ERR("%s: Erase at 0x%x Failed", __func__, (int)val);
			assert(0);
		}
	}

	return 0;
}

uint32_t flash_area_align(const struct flash_area *fa)
{
	UNUSED(fa);
	// the smallest unit a flash write can occur along.
	// Note: Image trailers will be scaled by this size
	return 8; // 64-bit aligned
}

uint8_t flash_area_erased_val(const struct flash_area *fa)
{
	UNUSED(fa);
	// the value a byte reads when erased on storage.
	return 0xff;
}

int flash_area_get_sectors(int fa_id, uint32_t *count, struct flash_sector *sectors)
{
	const struct flash_area *fa = prv_lookup_flash_area(fa_id);
	if (fa->fa_device_id != FLASH_DEVICE_INTERNAL_FLASH) {
		return -1;
	}

	const size_t sector_size = FLASH_PAGE_SIZE;
	uint32_t total_count = 0;
	for (size_t off = 0; off < fa->fa_size; off += sector_size) {
		// Note: Offset here is relative to flash area, not device
		sectors[total_count].fs_off = off;
		sectors[total_count].fs_size = sector_size;
		total_count++;
	}

	*count = total_count;
	return 0;
}

int flash_area_id_from_multi_image_slot(int image_index, int slot)
{
	switch (slot) {
	case 0:
		return FLASH_AREA_IMAGE_PRIMARY(image_index);
	case 1:
		return FLASH_AREA_IMAGE_SECONDARY(image_index);
	case 2:
		return FLASH_AREA_IMAGE_SCRATCH;
	}

	return -1; /* flash_area_open will fail on that */
}

int flash_area_id_from_image_slot(int slot)
{
	return flash_area_id_from_multi_image_slot(0, slot);
}

int flash_area_id_to_multi_image_slot(int image_index, int area_id)		// TV: Added
{
	// TV: TODO
	return -1;
}
