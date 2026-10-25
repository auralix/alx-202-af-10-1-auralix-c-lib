// Target: boot-debug - the bootloader (boot role B), debug build.
// Written by harness.host.matrix from alxConfig_TEMPLATE.h; do not edit, run
// `python -m harness.host.matrix` from Test/ instead.
#define ALX_BUILD_CONFIG_DEBUG
#undef ALX_BUILD_CONFIG_FW_UP
#undef ALX_BUILD_CONFIG_NO_BOOT
#undef ALX_BOOT_A
#undef ALX_BOOT_A_PRE_COMP_BOOT_HDR_FILE
#define ALX_BOOT_B
#define ALX_BOOT_B_ID_FW_ARTF
#define ALX_BOOT_B_ID_FW_NAME
#define ALX_MCU_BOOT_FLASH_DEVICE_ID
#define ALX_MCU_BOOT_FLASH_SECTOR_SIZE
#define ALX_MCU_BOOT_BOOTLOADER_SIZE
#define ALX_MCU_BOOT_IMAGE_SIZE
#define ALX_MCU_BOOT_BOOTLOADER_OFFSET
#define ALX_MCU_BOOT_IMAGE_PRIMARY_OFFSET
#define ALX_MCU_BOOT_IMAGE_SECONDARY_OFFSET
