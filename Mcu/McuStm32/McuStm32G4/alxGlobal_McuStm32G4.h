/**
  ******************************************************************************
  * @file alxGlobal_McuStm32G4.h
  * @brief Auralix C Library - ALX Global Module
  * @version $LastChangedRevision: 5698 $
  * @date $LastChangedDate: 2021-07-29 13:27:22 +0200 (Thu, 29 Jul 2021) $
  ******************************************************************************
  */

#ifndef ALX_GLOBAL_MCU_STM32G4_H
#define ALX_GLOBAL_MCU_STM32G4_H

#ifdef __cplusplus
extern "C" {
#endif

//******************************************************************************
// Module Guard
//******************************************************************************
//#if defined(ALX_STM32G4)


//******************************************************************************
// Includes
//******************************************************************************
#include "stm32g4xx.h"
#include "stm32g4xx_hal.h"
#include "stm32_hal_legacy.h"
#include "stm32g4xx_ll_adc.h"
#include "stm32g4xx_ll_bus.h"
#include "stm32g4xx_ll_comp.h"
#include "stm32g4xx_ll_cordic.h"
#include "stm32g4xx_ll_cortex.h"
#include "stm32g4xx_ll_crc.h"
#include "stm32g4xx_ll_crs.h"
#include "stm32g4xx_ll_dac.h"
#include "stm32g4xx_ll_dma.h"
#include "stm32g4xx_ll_dmamux.h"
#include "stm32g4xx_ll_exti.h"
#include "stm32g4xx_ll_fmac.h"
#include "stm32g4xx_ll_fmc.h"
#include "stm32g4xx_ll_gpio.h"
#include "stm32g4xx_ll_hrtim.h"
#include "stm32g4xx_ll_i2c.h"
#include "stm32g4xx_ll_iwdg.h"
#include "stm32g4xx_ll_lptim.h"
#include "stm32g4xx_ll_lpuart.h"
#include "stm32g4xx_ll_opamp.h"
#include "stm32g4xx_ll_pwr.h"
#include "stm32g4xx_ll_rcc.h"
#include "stm32g4xx_ll_rng.h"
#include "stm32g4xx_ll_rtc.h"
#include "stm32g4xx_ll_spi.h"
#include "stm32g4xx_ll_system.h"
#include "stm32g4xx_ll_tim.h"
#include "stm32g4xx_ll_ucpd.h"
#include "stm32g4xx_ll_usart.h"
#include "stm32g4xx_ll_usb.h"
#include "stm32g4xx_ll_utils.h"
#include "stm32g4xx_ll_wwdg.h"
#include "system_stm32g4xx.h"
#include "tz_context.h"


//******************************************************************************
// Preprocessor
//******************************************************************************
#define ALX_I2C_MCU_STM32_CLK_100kHz 0 // TODO
#define ALX_I2C_MCU_STM32_CLK_400kHz 0x40B285C2 // _400kHz_8MHz_100nsRise_10nsFall_AnalogFiltOn_DigitalFilt0, on STM32F3 -> Used I2C timing configuration tool (AN4235) and the associated software STSW-STM32126, measured 380kHz, if analog filter is disabled we can get 400kHz..
//0x60400D28 _400kHz_8MHz_100nsRise_10nsFall_AnalogFiltOn_DigitalFilt0
//0x40B285C2 _100kHz_8MHz_100nsRise_100nsFall_AnalogFiltOn_DigitalFilt0
#define ALX_STM32_CMSIS_VERSION_MAIN	__STM32G4_CMSIS_VERSION_MAIN
#define ALX_STM32_CMSIS_VERSION_SUB1	__STM32G4_CMSIS_VERSION_SUB1
#define ALX_STM32_CMSIS_VERSION_SUB2	__STM32G4_CMSIS_VERSION_SUB2
#define ALX_STM32_CMSIS_VERSION_RC		__STM32G4_CMSIS_VERSION_RC
#define ALX_STM32_CMSIS_VERSION			__STM32G4_CMSIS_VERSION

#define ALX_STM32_CMSIS_VER_REQUIRED	ALX_STM32G4_CMSIS_VER_REQUIRED
#define ALX_STM32_HAL_VER_REQUIRED		ALX_STM32G4_HAL_VER_REQUIRED

#define ALX_STM32_STR "STM32G4"

#endif

#ifdef __cplusplus
}
#endif // ALX_GLOBAL_MCU_STM32G4_H
