/**
  ******************************************************************************
  * @file		alxConfig.h
  * @brief		Auralix C Library - ALX Configuration File
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
// Include Guard
//******************************************************************************
#ifndef ALX_CONFIG_H
#define ALX_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif


//******************************************************************************
// Configuration
//******************************************************************************


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// Modules
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// ALX - Global
//------------------------------------------------------------------------------
//#define ALX_PC
#define ALX_NEWLIB_NANO
//#define ALX_MCU_BOOT
//#define ALX_CMSIS_DSP
//#define ALX_FREE_RTOS
//#define ALX_FREE_RTOS_CELLULAR
//#define ALX_LW_IP
//#define ALX_FATFS
//#define ALX_LFS
//#define ALX_WIZNET
//#define ALX_MBED
//#define ALX_MBED_CLOUD_CLIENT


//------------------------------------------------------------------------------
// ALX - mcuboot - MCUboot FLASH Map Backend Module
//------------------------------------------------------------------------------
//#define ALX_MCU_BOOT_FLASH_DEVICE_ID 0
//#define ALX_MCU_BOOT_FLASH_SECTOR_SIZE (128*1024) // 128kB - 131072, 0x00020000
//#define ALX_MCU_BOOT_BOOTLOADER_SIZE (128*1024) // 128kB - 131072, 0x00020000
//#define ALX_MCU_BOOT_IMAGE_SIZE (896*1024) // 896kB - 917504, 0x000E0000
//#define ALX_MCU_BOOT_BOOTLOADER_OFFSET (0x08000000)
//#define ALX_MCU_BOOT_IMAGE_PRIMARY_OFFSET (0x08020000)
//#define ALX_MCU_BOOT_IMAGE_SECONDARY_OFFSET (0x08120000))


//------------------------------------------------------------------------------
// ALX - Bootloader
//------------------------------------------------------------------------------
//#define ALX_BOOT_A
//#define ALX_BOOT_A_PRE_COMP_BOOT_HDR_FILE "alxBoot_GENERATED.h"
//#define ALX_BOOT_A_PRE_COMP_BOOT_HDR_FILE "2406071858_ALX-202-AF-27-1_NucleoF429Zi_Boot_CFw_V0-0-0_9f672fc.h"
//#define ALX_BOOT_B
//#define ALX_BOOT_B_ID_FW_ARTF "ALX-202-AF-27-1"
//#define ALX_BOOT_B_ID_FW_NAME "NucleoF429Zi_Boot_CFw"


//------------------------------------------------------------------------------
// ALX - ID
//------------------------------------------------------------------------------

// FW - Language C
#define ALX_LANG_C_VER_MIN_REQUIRED 199901L // C99

// FW - Language C Library
#define ALX_LANG_C_LIB_VER_MIN_REQUIRED 0x02050000 // 2.5.0

// FW - Language C++
#define ALX_LANG_CPP_VER_MIN_REQUIRED 201402L // C++14

// FW - Language C++ Library
#define ALX_LANG_CPP_LIB_VER_MIN_REQUIRED 20170814

// FW - Compiler
#define ALX_GCC
#define ALX_COMP_VER_MIN_REQUIRED 0x07020000 // 7.2.0

// FW - CMSIS Core
#define ALX_CMSIS_CORE
#define ALX_CMSIS_CORE_VER_REQUIRED 0x00050001 // 5.1

// HW - STM32
//#define ALX_STM32F0
//#define ALX_STM32F0_CMSIS_VER_REQUIRED 0x01010100 // 1.1.1.0
//#define ALX_STM32F0_HAL_VER_REQUIRED 0x01010100 // 1.1.1.0

//#define ALX_STM32F1
//#define ALX_STM32F1_CMSIS_VER_REQUIRED 0x01010100 // 1.1.1.0
//#define ALX_STM32F1_HAL_VER_REQUIRED 0x01010100 // 1.1.1.0

#define ALX_STM32F4
#define ALX_STM32F4_CMSIS_VER_REQUIRED 0x01010100 // 1.1.1.0
#define ALX_STM32F4_HAL_VER_REQUIRED 0x01010100 // 1.1.1.0

//#define ALX_STM32F7
//#define ALX_STM32F7_CMSIS_VER_REQUIRED 0x01010100 // 1.1.1.0
//#define ALX_STM32F7_HAL_VER_REQUIRED 0x01010100 // 1.1.1.0

//#define ALX_STM32G4
//#define ALX_STM32G4_CMSIS_VER_REQUIRED 0x01010100 // 1.1.1.0
//#define ALX_STM32G4_HAL_VER_REQUIRED 0x01010100 // 1.1.1.0

//#define ALX_STM32L0
//#define ALX_STM32L0_CMSIS_VER_REQUIRED 0x01010100 // 1.1.1.0
//#define ALX_STM32L0_HAL_VER_REQUIRED 0x01010100 // 1.1.1.0

//#define ALX_STM32L4
//#define ALX_STM32L4_CMSIS_VER_REQUIRED 0x01010100 // 1.1.1.0
//#define ALX_STM32L4_HAL_VER_REQUIRED 0x01010100 // 1.1.1.0

//#define ALX_STM32U5
//#define ALX_STM32U5_CMSIS_VER_REQUIRED 0x01030000 // 1.3.0.0
//#define ALX_STM32U5_HAL_VER_REQUIRED 0x01030000 // 1.3.0.0

// HW - LPC
//#define ALX_LPC17XX
//#define ALX_LPC55S6X
//#define ALX_LPC80X
//#define ALX_LPC84X


//------------------------------------------------------------------------------
// ALX - MCU - IO Pin IRQ
//------------------------------------------------------------------------------
//#define ALX_IO_PIN_IRQ_HANDLERS_OFF




//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// General
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// C/C++
//------------------------------------------------------------------------------
#define ALX_C_LIB
//#define ALX_CPP_LIB


//------------------------------------------------------------------------------
// Assert - BKPT
//------------------------------------------------------------------------------

//// ALX - EXT
//#define ALX_ADAU1961_ASSERT_BKPT_ENABLE
//#define ALX_ADS8678_ASSERT_BKPT_ENABLE
//#define ALX_ADS114S08_ASSERT_BKPT_ENABLE
//#define ALX_ADXL355_ASSERT_BKPT_ENABLE
//#define ALX_BQ24600_ASSERT_BKPT_ENABLE
//#define ALX_BTS724G_ASSERT_BKPT_ENABLE
//#define ALX_INA228_ASSERT_BKPT_ENABLE
//#define ALX_MCP73831_ASSERT_BKPT_ENABLE
//#define ALX_NTCG103JF103FT1S_ASSERT_BKPT_ENABLE
//#define ALX_PCAL6416A_ASSERT_BKPT_ENABLE
//#define ALX_TMP1075_ASSERT_BKPT_ENABLE
//#define ALX_TPA3255_ASSERT_BKPT_ENABLE
//#define ALX_VEML6040_ASSERT_BKPT_ENABLE
//#define ALX_VNH7040_ASSERT_BKPT_ENABLE
//
//// ALX - MCU
//#define ALX_ADC_ASSERT_BKPT_ENABLE
//#define ALX_CAN_ASSERT_BKPT_ENABLE
//#define ALX_CLK_ASSERT_BKPT_ENABLE
//#define ALX_DAC_ASSERT_BKPT_ENABLE
//#define ALX_I2C_ASSERT_BKPT_ENABLE
//#define ALX_I2S_ASSERT_BKPT_ENABLE
//#define ALX_IO_PIN_ASSERT_BKPT_ENABLE
//#define ALX_IO_PIN_IRQ_ASSERT_BKPT_ENABLE
//#define ALX_MMC_ASSERT_BKPT_ENABLE
//#define ALX_PWM_ASSERT_BKPT_ENABLE
//#define ALX_RST_ASSERT_BKPT_ENABLE
//#define ALX_RTC_ASSERT_BKPT_ENABLE
//#define ALX_SERIAL_PORT_ASSERT_BKPT_ENABLE
//#define ALX_SPI_ASSERT_BKPT_ENABLE
//// AlxTrace
//#define ALX_WDT_ASSERT_BKPT_ENABLE
//
//// ALX - mcuboot
//#define ALX_MCU_BOOT_FLASH_MAP_BACKEND_ASSERT_BKPT_ENABLE
//
//// ALX
//// AlxAssert
//#define ALX_AUDIO_ASSERT_BKPT_ENABLE
//#define ALX_AUDIO_PLAYER_ASSERT_BKPT_ENABLE
//#define ALX_AUDIO_VOL_ASSERT_BKPT_ENABLE
//#define ALX_AVG_ASSERT_BKPT_ENABLE
//#define ALX_BOOL_ASSERT_BKPT_ENABLE
//#define ALX_BOOT_ASSERT_BKPT_ENABLE
//#define ALX_BOUND_ASSERT_BKPT_ENABLE
//// AlxBuild
//#define ALX_CAN_PARSER_ASSERT_BKPT_ENABLE
//#define ALX_CLI_ASSERT_BKPT_ENABLE
//#define ALX_CRC_ASSERT_BKPT_ENABLE
//#define ALX_DELAY_ASSERT_BKPT_ENABLE
//#define ALX_FIFO_ASSERT_BKPT_ENABLE
//#define ALX_FILT_GLITCH_BOOL_ASSERT_BKPT_ENABLE
//#define ALX_FILT_GLITCH_UINT32_ASSERT_BKPT_ENABLE
//#define ALX_FS_ASSERT_BKPT_ENABLE
//#define ALX_FTOA_ASSERT_BKPT_ENABLE
//#define ALX_GLOBAL_ASSERT_BKPT_ENABLE
//#define ALX_HYS1_ASSERT_BKPT_ENABLE
//#define ALX_HYS2_ASSERT_BKPT_ENABLE
//#define ALX_ID_ASSERT_BKPT_ENABLE
//// AlxInc
//#define ALX_INTERP_LIN_ASSERT_BKPT_ENABLE
//#define ALX_LFS_CONFIG_ASSERT_BKPT_ENABLE
//#define ALX_LIN_ASSERT_BKPT_ENABLE
//#define ALX_LIN_FUN_ASSERT_BKPT_ENABLE
//#define ALX_LOGGER_ASSERT_BKPT_ENABLE
//#define ALX_MATH_ASSERT_BKPT_ENABLE
//#define ALX_MEM_RAW_ASSERT_BKPT_ENABLE
//#define ALX_MEM_SAFE_ASSERT_BKPT_ENABLE
//#define ALX_NET_ASSERT_BKPT_ENABLE
//#define ALX_OS_CRITICAL_SECTION_ASSERT_BKPT_ENABLE
//#define ALX_OS_DELAY_ASSERT_BKPT_ENABLE
//#define ALX_OS_DELAY_UNTIL_ASSERT_BKPT_ENABLE
//#define ALX_OS_EVENT_FLAG_GROUP_ASSERT_BKPT_ENABLE
//#define ALX_OS_KERNEL_ASSERT_BKPT_ENABLE
//#define ALX_OS_MUTEX_ASSERT_BKPT_ENABLE
//#define ALX_OS_THREAD_ASSERT_BKPT_ENABLE
//#define ALX_PARAM_GROUP_ASSERT_BKPT_ENABLE
//#define ALX_PARAM_ITEM_ASSERT_BKPT_ENABLE
//#define ALX_PARAM_KV_STORE_ASSERT_BKPT_ENABLE
//#define ALX_PARAM_MGMT_ASSERT_BKPT_ENABLE
//#define ALX_PARAM_STORE_ASSERT_BKPT_ENABLE
//#define ALX_PWR_ASSERT_BKPT_ENABLE
//#define ALX_RANGE_ASSERT_BKPT_ENABLE
//#define ALX_ROT_SW_ASSERT_BKPT_ENABLE
//#define ALX_RTC_GLOBAL_ASSERT_BKPT_ENABLE
//#define ALX_SD_ASSERT_BKPT_ENABLE
//#define ALX_SOCKET_ASSERT_BKPT_ENABLE
//#define ALX_TEMP_SENS_RTD_VDIV_ASSERT_BKPT_ENABLE
//// AlxTick
//#define ALX_TIM_SW_ASSERT_BKPT_ENABLE
//#define ALX_VDIV_ASSERT_BKPT_ENABLE
//
//// ALX - C++
//#define ALX_FTP_CLIENT_ASSERT_BKPT_ENABLE
//#define ALX_NTP_CLIENT_ASSERT_BKPT_ENABLE


//------------------------------------------------------------------------------
// Assert - TRACE
//------------------------------------------------------------------------------

//// ALX - EXT
//#define ALX_ADAU1961_ASSERT_TRACE_ENABLE
//#define ALX_ADS8678_ASSERT_TRACE_ENABLE
//#define ALX_ADS114S08_ASSERT_TRACE_ENABLE
//#define ALX_ADXL355_ASSERT_TRACE_ENABLE
//#define ALX_BQ24600_ASSERT_TRACE_ENABLE
//#define ALX_BTS724G_ASSERT_TRACE_ENABLE
//#define ALX_INA228_ASSERT_TRACE_ENABLE
//#define ALX_MCP73831_ASSERT_TRACE_ENABLE
//#define ALX_NTCG103JF103FT1S_ASSERT_TRACE_ENABLE
//#define ALX_PCAL6416A_ASSERT_TRACE_ENABLE
//#define ALX_TMP1075_ASSERT_TRACE_ENABLE
//#define ALX_TPA3255_ASSERT_TRACE_ENABLE
//#define ALX_VEML6040_ASSERT_TRACE_ENABLE
//#define ALX_VNH7040_ASSERT_TRACE_ENABLE
//
//// ALX - MCU
//#define ALX_ADC_ASSERT_TRACE_ENABLE
//#define ALX_CAN_ASSERT_TRACE_ENABLE
//#define ALX_CLK_ASSERT_TRACE_ENABLE
//#define ALX_DAC_ASSERT_TRACE_ENABLE
//#define ALX_I2C_ASSERT_TRACE_ENABLE
//#define ALX_I2S_ASSERT_TRACE_ENABLE
//#define ALX_IO_PIN_ASSERT_TRACE_ENABLE
//#define ALX_IO_PIN_IRQ_ASSERT_TRACE_ENABLE
//#define ALX_MMC_ASSERT_TRACE_ENABLE
//#define ALX_PWM_ASSERT_TRACE_ENABLE
//#define ALX_RST_ASSERT_TRACE_ENABLE
//#define ALX_RTC_ASSERT_TRACE_ENABLE
//#define ALX_SERIAL_PORT_ASSERT_TRACE_ENABLE
//#define ALX_SPI_ASSERT_TRACE_ENABLE
//// AlxTrace
//#define ALX_WDT_ASSERT_TRACE_ENABLE
//
//// ALX - mcuboot
//#define ALX_MCU_BOOT_FLASH_MAP_BACKEND_ASSERT_TRACE_ENABLE
//
//// ALX
//// AlxAssert
//#define ALX_AUDIO_ASSERT_TRACE_ENABLE
//#define ALX_AUDIO_PLAYER_ASSERT_TRACE_ENABLE
//#define ALX_AUDIO_VOL_ASSERT_TRACE_ENABLE
//#define ALX_AVG_ASSERT_TRACE_ENABLE
//#define ALX_BOOL_ASSERT_TRACE_ENABLE
//#define ALX_BOOT_ASSERT_TRACE_ENABLE
//#define ALX_BOUND_ASSERT_TRACE_ENABLE
//// AlxBuild
//#define ALX_CAN_PARSER_ASSERT_TRACE_ENABLE
//#define ALX_CLI_ASSERT_TRACE_ENABLE
//#define ALX_CRC_ASSERT_TRACE_ENABLE
//#define ALX_DELAY_ASSERT_TRACE_ENABLE
//#define ALX_FIFO_ASSERT_TRACE_ENABLE
//#define ALX_FILT_GLITCH_BOOL_ASSERT_TRACE_ENABLE
//#define ALX_FILT_GLITCH_UINT32_ASSERT_TRACE_ENABLE
//#define ALX_FS_ASSERT_TRACE_ENABLE
//#define ALX_FTOA_ASSERT_TRACE_ENABLE
//#define ALX_GLOBAL_ASSERT_TRACE_ENABLE
//#define ALX_HYS1_ASSERT_TRACE_ENABLE
//#define ALX_HYS2_ASSERT_TRACE_ENABLE
//#define ALX_ID_ASSERT_TRACE_ENABLE
//// AlxInc
//#define ALX_INTERP_LIN_ASSERT_TRACE_ENABLE
//#define ALX_LFS_CONFIG_ASSERT_TRACE_ENABLE
//#define ALX_LIN_ASSERT_TRACE_ENABLE
//#define ALX_LIN_FUN_ASSERT_TRACE_ENABLE
//#define ALX_LOGGER_ASSERT_TRACE_ENABLE
//#define ALX_MATH_ASSERT_TRACE_ENABLE
//#define ALX_MEM_RAW_ASSERT_TRACE_ENABLE
//#define ALX_MEM_SAFE_ASSERT_TRACE_ENABLE
//#define ALX_NET_ASSERT_TRACE_ENABLE
//#define ALX_OS_CRITICAL_SECTION_ASSERT_TRACE_ENABLE
//#define ALX_OS_DELAY_ASSERT_TRACE_ENABLE
//#define ALX_OS_DELAY_UNTIL_ASSERT_TRACE_ENABLE
//#define ALX_OS_EVENT_FLAG_GROUP_ASSERT_TRACE_ENABLE
//#define ALX_OS_KERNEL_ASSERT_TRACE_ENABLE
//#define ALX_OS_MUTEX_ASSERT_TRACE_ENABLE
//#define ALX_OS_THREAD_ASSERT_TRACE_ENABLE
//#define ALX_PARAM_GROUP_ASSERT_TRACE_ENABLE
//#define ALX_PARAM_ITEM_ASSERT_TRACE_ENABLE
//#define ALX_PARAM_KV_STORE_ASSERT_TRACE_ENABLE
//#define ALX_PARAM_MGMT_ASSERT_TRACE_ENABLE
//#define ALX_PARAM_STORE_ASSERT_TRACE_ENABLE
//#define ALX_PWR_ASSERT_TRACE_ENABLE
//#define ALX_RANGE_ASSERT_TRACE_ENABLE
//#define ALX_ROT_SW_ASSERT_TRACE_ENABLE
//#define ALX_RTC_GLOBAL_ASSERT_TRACE_ENABLE
//#define ALX_SD_ASSERT_TRACE_ENABLE
//#define ALX_SOCKET_ASSERT_TRACE_ENABLE
//#define ALX_TEMP_SENS_RTD_VDIV_ASSERT_TRACE_ENABLE
//// AlxTick
//#define ALX_TIM_SW_ASSERT_TRACE_ENABLE
//#define ALX_VDIV_ASSERT_TRACE_ENABLE
//
//// ALX - C++
//#define ALX_FTP_CLIENT_ASSERT_TRACE_ENABLE
//#define ALX_NTP_CLIENT_ASSERT_TRACE_ENABLE


//------------------------------------------------------------------------------
// Assert - RST
//------------------------------------------------------------------------------

// ALX - EXT
#define ALX_ADAU1961_ASSERT_RST_ENABLE
#define ALX_ADS8678_ASSERT_RST_ENABLE
#define ALX_ADS114S08_ASSERT_RST_ENABLE
#define ALX_ADXL355_ASSERT_RST_ENABLE
#define ALX_BQ24600_ASSERT_RST_ENABLE
#define ALX_BTS724G_ASSERT_RST_ENABLE
#define ALX_INA228_ASSERT_RST_ENABLE
#define ALX_MCP73831_ASSERT_RST_ENABLE
#define ALX_NTCG103JF103FT1S_ASSERT_RST_ENABLE
#define ALX_PCAL6416A_ASSERT_RST_ENABLE
#define ALX_TMP1075_ASSERT_RST_ENABLE
#define ALX_TPA3255_ASSERT_RST_ENABLE
#define ALX_VEML6040_ASSERT_RST_ENABLE
#define ALX_VNH7040_ASSERT_RST_ENABLE

// ALX - MCU
#define ALX_ADC_ASSERT_RST_ENABLE
#define ALX_CAN_ASSERT_RST_ENABLE
#define ALX_CLK_ASSERT_RST_ENABLE
#define ALX_DAC_ASSERT_RST_ENABLE
#define ALX_I2C_ASSERT_RST_ENABLE
#define ALX_I2S_ASSERT_RST_ENABLE
#define ALX_IO_PIN_ASSERT_RST_ENABLE
#define ALX_IO_PIN_IRQ_ASSERT_RST_ENABLE
#define ALX_MMC_ASSERT_RST_ENABLE
#define ALX_PWM_ASSERT_RST_ENABLE
#define ALX_RST_ASSERT_RST_ENABLE
#define ALX_RTC_ASSERT_RST_ENABLE
#define ALX_SERIAL_PORT_ASSERT_RST_ENABLE
#define ALX_SPI_ASSERT_RST_ENABLE
// AlxTrace
#define ALX_WDT_ASSERT_RST_ENABLE

// ALX - mcuboot
#define ALX_MCU_BOOT_FLASH_MAP_BACKEND_ASSERT_RST_ENABLE

// ALX
// AlxAssert
#define ALX_AUDIO_ASSERT_RST_ENABLE
#define ALX_AUDIO_PLAYER_ASSERT_RST_ENABLE
#define ALX_AUDIO_VOL_ASSERT_RST_ENABLE
#define ALX_AVG_ASSERT_RST_ENABLE
#define ALX_BOOL_ASSERT_RST_ENABLE
#define ALX_BOOT_ASSERT_RST_ENABLE
#define ALX_BOUND_ASSERT_RST_ENABLE
// AlxBuild
#define ALX_CAN_PARSER_ASSERT_RST_ENABLE
#define ALX_CLI_ASSERT_RST_ENABLE
#define ALX_CRC_ASSERT_RST_ENABLE
#define ALX_DELAY_ASSERT_RST_ENABLE
#define ALX_FIFO_ASSERT_RST_ENABLE
#define ALX_FILT_GLITCH_BOOL_ASSERT_RST_ENABLE
#define ALX_FILT_GLITCH_UINT32_ASSERT_RST_ENABLE
#define ALX_FS_ASSERT_RST_ENABLE
#define ALX_FTOA_ASSERT_RST_ENABLE
#define ALX_GLOBAL_ASSERT_RST_ENABLE
#define ALX_HYS1_ASSERT_RST_ENABLE
#define ALX_HYS2_ASSERT_RST_ENABLE
#define ALX_ID_ASSERT_RST_ENABLE
// AlxInc
#define ALX_INTERP_LIN_ASSERT_RST_ENABLE
#define ALX_LFS_CONFIG_ASSERT_RST_ENABLE
#define ALX_LIN_ASSERT_RST_ENABLE
#define ALX_LIN_FUN_ASSERT_RST_ENABLE
#define ALX_LOGGER_ASSERT_RST_ENABLE
#define ALX_MATH_ASSERT_RST_ENABLE
#define ALX_MEM_RAW_ASSERT_RST_ENABLE
#define ALX_MEM_SAFE_ASSERT_RST_ENABLE
#define ALX_NET_ASSERT_RST_ENABLE
#define ALX_OS_CRITICAL_SECTION_ASSERT_RST_ENABLE
#define ALX_OS_DELAY_ASSERT_RST_ENABLE
#define ALX_OS_DELAY_UNTIL_ASSERT_RST_ENABLE
#define ALX_OS_EVENT_FLAG_GROUP_ASSERT_RST_ENABLE
#define ALX_OS_KERNEL_ASSERT_RST_ENABLE
#define ALX_OS_MUTEX_ASSERT_RST_ENABLE
#define ALX_OS_THREAD_ASSERT_RST_ENABLE
#define ALX_PARAM_GROUP_ASSERT_RST_ENABLE
#define ALX_PARAM_ITEM_ASSERT_RST_ENABLE
#define ALX_PARAM_KV_STORE_ASSERT_RST_ENABLE
#define ALX_PARAM_MGMT_ASSERT_RST_ENABLE
#define ALX_PARAM_STORE_ASSERT_RST_ENABLE
#define ALX_PWR_ASSERT_RST_ENABLE
#define ALX_RANGE_ASSERT_RST_ENABLE
#define ALX_ROT_SW_ASSERT_RST_ENABLE
#define ALX_RTC_GLOBAL_ASSERT_RST_ENABLE
#define ALX_SD_ASSERT_RST_ENABLE
#define ALX_SOCKET_ASSERT_RST_ENABLE
#define ALX_TEMP_SENS_RTD_VDIV_ASSERT_RST_ENABLE
// AlxTick
#define ALX_TIM_SW_ASSERT_RST_ENABLE
#define ALX_VDIV_ASSERT_RST_ENABLE

// ALX - C++
#define ALX_FTP_CLIENT_ASSERT_RST_ENABLE
#define ALX_NTP_CLIENT_ASSERT_RST_ENABLE


//------------------------------------------------------------------------------
// Trace
//------------------------------------------------------------------------------
#define ALX_TRACE_LEVEL ALX_TRACE_LEVEL_INF

// ALX - EXT
#define ALX_ADAU1961_TRACE_ENABLE
#define ALX_ADS8678_TRACE_ENABLE
#define ALX_ADS114S08_TRACE_ENABLE
#define ALX_ADXL355_TRACE_ENABLE
#define ALX_BQ24600_TRACE_ENABLE
#define ALX_BTS724G_TRACE_ENABLE
#define ALX_INA228_TRACE_ENABLE
#define ALX_MCP73831_TRACE_ENABLE
#define ALX_NTCG103JF103FT1S_TRACE_ENABLE
#define ALX_PCAL6416A_TRACE_ENABLE
#define ALX_TMP1075_TRACE_ENABLE
#define ALX_TPA3255_TRACE_ENABLE
#define ALX_VEML6040_TRACE_ENABLE
#define ALX_VNH7040_TRACE_ENABLE

// ALX - MCU
#define ALX_ADC_TRACE_ENABLE
#define ALX_CAN_TRACE_ENABLE
#define ALX_CLK_TRACE_ENABLE
#define ALX_DAC_TRACE_ENABLE
#define ALX_I2C_TRACE_ENABLE
#define ALX_I2S_TRACE_ENABLE
#define ALX_IO_PIN_TRACE_ENABLE
#define ALX_IO_PIN_IRQ_TRACE_ENABLE
#define ALX_MMC_TRACE_ENABLE
#define ALX_PWM_TRACE_ENABLE
#define ALX_RST_TRACE_ENABLE
#define ALX_RTC_TRACE_ENABLE
#define ALX_SERIAL_PORT_TRACE_ENABLE
#define ALX_SPI_TRACE_ENABLE
// AlxTrace
#define ALX_WDT_TRACE_ENABLE

// ALX - mcuboot
#define ALX_MCU_BOOT_FLASH_MAP_BACKEND_TRACE_ENABLE

// ALX
// AlxAssert
#define ALX_AUDIO_TRACE_ENABLE
#define ALX_AUDIO_PLAYER_TRACE_ENABLE
#define ALX_AUDIO_VOL_TRACE_ENABLE
#define ALX_AVG_TRACE_ENABLE
#define ALX_BOOL_TRACE_ENABLE
#define ALX_BOOT_TRACE_ENABLE
#define ALX_BOUND_TRACE_ENABLE
// AlxBuild
#define ALX_CAN_PARSER_TRACE_ENABLE
#define ALX_CLI_TRACE_ENABLE
#define ALX_CRC_TRACE_ENABLE
#define ALX_DELAY_TRACE_ENABLE
#define ALX_FIFO_TRACE_ENABLE
#define ALX_FILT_GLITCH_BOOL_TRACE_ENABLE
#define ALX_FILT_GLITCH_UINT32_TRACE_ENABLE
#define ALX_FS_TRACE_ENABLE
#define ALX_FTOA_TRACE_ENABLE
#define ALX_GLOBAL_TRACE_ENABLE
#define ALX_HYS1_TRACE_ENABLE
#define ALX_HYS2_TRACE_ENABLE
#define ALX_ID_TRACE_ENABLE
// AlxInc
#define ALX_INTERP_LIN_TRACE_ENABLE
#define ALX_LFS_CONFIG_TRACE_ENABLE
#define ALX_LIN_TRACE_ENABLE
#define ALX_LIN_FUN_TRACE_ENABLE
#define ALX_LOGGER_TRACE_ENABLE
#define ALX_MATH_TRACE_ENABLE
#define ALX_MEM_RAW_TRACE_ENABLE
#define ALX_MEM_SAFE_TRACE_ENABLE
#define ALX_NET_TRACE_ENABLE
#define ALX_OS_CRITICAL_SECTION_TRACE_ENABLE
#define ALX_OS_DELAY_TRACE_ENABLE
#define ALX_OS_DELAY_UNTIL_TRACE_ENABLE
#define ALX_OS_EVENT_FLAG_GROUP_TRACE_ENABLE
#define ALX_OS_KERNEL_TRACE_ENABLE
#define ALX_OS_MUTEX_TRACE_ENABLE
#define ALX_OS_THREAD_TRACE_ENABLE
#define ALX_PARAM_GROUP_TRACE_ENABLE
#define ALX_PARAM_ITEM_TRACE_ENABLE
#define ALX_PARAM_KV_STORE_TRACE_ENABLE
#define ALX_PARAM_MGMT_TRACE_ENABLE
#define ALX_PARAM_STORE_TRACE_ENABLE
#define ALX_PWR_TRACE_ENABLE
#define ALX_RANGE_TRACE_ENABLE
#define ALX_ROT_SW_TRACE_ENABLE
#define ALX_RTC_GLOBAL_TRACE_ENABLE
#define ALX_SD_TRACE_ENABLE
#define ALX_SOCKET_TRACE_ENABLE
#define ALX_TEMP_SENS_RTD_VDIV_TRACE_ENABLE
// AlxTick
#define ALX_TIM_SW_TRACE_ENABLE
#define ALX_VDIV_TRACE_ENABLE

// ALX - C++
#define ALX_FTP_CLIENT_TRACE_ENABLE
#define ALX_NTP_CLIENT_TRACE_ENABLE


//------------------------------------------------------------------------------
// Optimize Size
//------------------------------------------------------------------------------
//#define ALX_ADC_OPTIMIZE_SIZE
//#define ALX_PWM_OPTIMIZE_SIZE




//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// Configuration File - User
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//#include "alxConfigUsr.h"


#ifdef __cplusplus
}
#endif

#endif	// #ifndef ALX_CONFIG_H
