// Features: all-nomcuboot - every feature on; the network through Wiznet; no MCUboot.
// Written by harness.host.matrix from alxConfig_TEMPLATE.h; do not edit, run
// `python -m harness.host.matrix` from Test/ instead.
#define ALX_CMSIS_DSP
#define ALX_FATFS
#define ALX_FREE_RTOS
#undef ALX_FREE_RTOS_CELLULAR
#define ALX_LFS
#undef ALX_LW_IP
#define ALX_MBEDTLS
#undef ALX_MCU_BOOT
#define ALX_USBH
#define ALX_WIZNET
