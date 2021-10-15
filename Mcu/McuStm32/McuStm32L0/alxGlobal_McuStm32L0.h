/**
  ******************************************************************************
  * @file alxGlobal_McuStm32L0.h
  * @brief Auralix C Library - ALX Global Module
  * @version $LastChangedRevision: 4317 $
  * @date $LastChangedDate: 2021-03-10 20:24:20 +0100 (Wed, 10 Mar 2021) $
  ******************************************************************************
  */

#ifndef ALX_GLOBAL_MCU_STM32L0_H
#define ALX_GLOBAL_MCU_STM32L0_H

#ifdef __cplusplus
extern "C" {
#endif

//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_STM32L0)


//******************************************************************************
// Includes
//******************************************************************************
#include "stm32l0xx.h"
#include "stm32l0xx_hal.h"
#include "stm32_hal_legacy.h"
#include "stm32l0xx_ll_adc.h"
#include "stm32l0xx_ll_bus.h"
#include "stm32l0xx_ll_comp.h"
#include "stm32l0xx_ll_cortex.h"
#include "stm32l0xx_ll_crc.h"
#include "stm32l0xx_ll_crs.h"
#include "stm32l0xx_ll_dac.h"
#include "stm32l0xx_ll_dma.h"
#include "stm32l0xx_ll_exti.h"
#include "stm32l0xx_ll_gpio.h"
#include "stm32l0xx_ll_i2c.h"
#include "stm32l0xx_ll_iwdg.h"
#include "stm32l0xx_ll_lptim.h"
#include "stm32l0xx_ll_lpuart.h"
#include "stm32l0xx_ll_pwr.h"
#include "stm32l0xx_ll_rcc.h"
#include "stm32l0xx_ll_rng.h"
#include "stm32l0xx_ll_rtc.h"
#include "stm32l0xx_ll_spi.h"
#include "stm32l0xx_ll_system.h"
#include "stm32l0xx_ll_tim.h"
#include "stm32l0xx_ll_usart.h"
#include "stm32l0xx_ll_usb.h"
#include "stm32l0xx_ll_utils.h"
#include "stm32l0xx_ll_wwdg.h"
#include "system_stm32l0xx.h"


//******************************************************************************
// Preprocessor
//******************************************************************************

// AlxI2c_McuStm32
#define ALX_I2C_MCU_STM32_CLK_100kHz 0 // TODO
#define ALX_I2C_MCU_STM32_CLK_400kHz 0x0010020A // _400kHz_8MHz_100nsRise_10nsFall_AnalogFiltOn_DigitalFilt0, on STM32F3 -> Used I2C timing configuration tool (AN4235) and the associated software STSW-STM32126, measured 380kHz, if analog filter is disabled we can get 400kHz..

// AlxDac_McuStm32
#define __HAL_RCC_DAC1_CLK_ENABLE		__HAL_RCC_DAC_CLK_ENABLE
#define __HAL_RCC_DAC1_CLK_DISABLE		__HAL_RCC_DAC_CLK_DISABLE
#define __HAL_RCC_DAC1_FORCE_RESET		__HAL_RCC_DAC_FORCE_RESET
#define __HAL_RCC_DAC1_RELEASE_RESET	__HAL_RCC_DAC_RELEASE_RESET

// AlxId //
#define ALX_STM32_CMSIS_VERSION_MAIN	__STM32L0xx_CMSIS_VERSION_MAIN
#define ALX_STM32_CMSIS_VERSION_SUB1	__STM32L0xx_CMSIS_VERSION_SUB1
#define ALX_STM32_CMSIS_VERSION_SUB2	__STM32L0xx_CMSIS_VERSION_SUB2
#define ALX_STM32_CMSIS_VERSION_RC		__STM32L0xx_CMSIS_VERSION_RC
#define ALX_STM32_CMSIS_VERSION			__STM32L0xx_CMSIS_VERSION
#define ALX_STM32_CMSIS_VER_REQUIRED	ALX_STM32L0_CMSIS_VER_REQUIRED
#define ALX_STM32_HAL_VER_REQUIRED		ALX_STM32L0_HAL_VER_REQUIRED
#define ALX_STM32_STR "STM32L0"

#endif

#ifdef __cplusplus
}
#endif

#endif // ALX_GLOBAL_MCU_STM32L0_H
