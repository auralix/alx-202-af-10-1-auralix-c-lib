/**
  ******************************************************************************
  * @file		bootutil.h
  * @brief		Auralix C Library - Configuration Matrix Fake - MCUboot
  * @copyright	Copyright (C) Auralix d.o.o. All rights reserved.
  *
  * Compile-only: the MCUboot boot API as far as the alx code uses it - boot_go,
  * its response and the image header it points at - written from that use, so
  * the configuration matrix compiles every alx line under ALX_MCU_BOOT with
  * ALX_BOOT_B (Test/config/matrix.toml). Nothing here is the vendor's and
  * nothing here runs; the real one is the product's.
  ******************************************************************************
  **/

#ifndef ALX_SHIM_MCUBOOT_BOOTUTIL_BOOTUTIL_H
#define ALX_SHIM_MCUBOOT_BOOTUTIL_BOOTUTIL_H

#include <stdint.h>

typedef int fih_ret;
#define FIH_SUCCESS 0

struct image_version
{
	uint8_t iv_major;
	uint8_t iv_minor;
	uint16_t iv_revision;
	uint32_t iv_build_num;
};

struct image_header
{
	uint32_t ih_magic;
	uint32_t ih_load_addr;
	uint16_t ih_hdr_size;
	uint16_t ih_protect_tlv_size;
	uint32_t ih_img_size;
	uint32_t ih_flags;
	struct image_version ih_ver;
	uint32_t _pad1;
};

struct boot_rsp
{
	const struct image_header* br_hdr;
	uint8_t br_flash_dev_id;
	uint32_t br_image_off;
};

fih_ret boot_go(struct boot_rsp* rsp);

#endif	// ALX_SHIM_MCUBOOT_BOOTUTIL_BOOTUTIL_H
