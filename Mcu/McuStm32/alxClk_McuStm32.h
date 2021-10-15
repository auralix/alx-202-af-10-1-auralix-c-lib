/**
  ******************************************************************************
  * @file alxClk_McuStm32.h
  * @brief Auralix C Library - ALX Clock Module
  * @version $LastChangedRevision: 6102 $
  * @date $LastChangedDate: 2021-09-22 20:44:57 +0200 (Wed, 22 Sep 2021) $
  ******************************************************************************
  */

#ifndef ALX_CLK_MCU_STM32_H
#define ALX_CLK_MCU_STM32_H

#ifdef __cplusplus
extern "C" {
#endif

//******************************************************************************
// Includes
//******************************************************************************
#include "alxGlobal.h"
#include "alxTrace.h"
#include "alxAssert.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_STM32F1) || defined(ALX_STM32F4) || defined(ALX_STM32G4) || defined(ALX_STM32L0)


//******************************************************************************
// Types
//******************************************************************************
typedef enum
{
	#if defined(STM32F1)
	AlxClk_Config_McuStm32F1_Sysclk_8MHz_Pclk1Apb1_8MHz_Pclk2Apb2_8MHz_Hsi_8MHz_Default = 0,
	AlxClk_Config_McuStm32F1_Sysclk_64MHz_Pclk1Apb1_32MHz_Pclk2Apb2_64MHz_Hsi_8MHz = 1, 
	AlxClk_Config_McuStm32F1_Sysclk_64MHz_Pclk1Apb1_32MHz_Pclk2Apb2_64MHz_Hse_8MHz = 2,					// I added this to test just Hse
	AlxClk_Config_McuStm32F1_Sysclk_64MHz_Pclk1Apb1_32MHz_Pclk2Apb2_64MHz_Hse_8MHz_BackupHsi_8Mhz = 3, 
	#endif
	#if defined(STM32F4)
	AlxClk_Config_McuStm32F4_Sysclk_16MHz_Pclk1Apb1_16MHz_Pclk2Apb2_16MHz_Hsi_16MHz_Default = 10,
	AlxClk_Config_McuStm32F4_Sysclk_180MHz_Pclk1Apb1_45MHz_Pclk2Apb2_90MHz_Hsi_16MHz = 11,
	AlxClk_Config_McuStm32F4_Sysclk_180MHz_Pclk1Apb1_45MHz_Pclk2Apb2_90MHz_Hse_12MHz = 12,
	AlxClk_Config_McuStm32F4_Sysclk_180MHz_Pclk1Apb1_45MHz_Pclk2Apb2_90MHz_Hse_12MHz_BackupHsi_16Mhz = 13,
	AlxClk_Config_McuStm32F4_Sysclk_180MHz_Pclk1Apb1_45MHz_Pclk2Apb2_90MHz_Hse_25MHz = 14,
	#endif
	#if defined(STM32G4)
	AlxClk_Config_McuStm32G4_Sysclk_16MHz_Pclk1Apb1_16MHz_Pclk2Apb2_16MHz_Hsi_16MHz_Default = 6,
	AlxClk_Config_McuStm32G4_Sysclk_170MHz_Pclk1Apb1_170MHz_Pclk2Apb2_170MHz_Hsi_16MHz = 7,
	#endif
	#if defined(STM32L0)
	AlxClk_Config_McuStm32L0_Sysclk_2MHz1_Pclk1Apb1_2MHz1_Pclk2Apb2_2MHz1_Msi_2MHz1_Default = 8,
	AlxClk_Config_McuStm32L0_Sysclk_32MHz_Pclk1Apb1_32MHz_Pclk2Apb2_32MHz_Hsi_16MHz = 9,
	AlxClk_Config_McuStm32L0_Sysclk_524kHz_Pclk1Apb1_524kHz_Pclk2Apb2_524kHz_Msi_2MHz1 = 10
	#endif
} AlxClk_Config;

typedef struct
{
	// Parameters
	AlxClk_Config config;

	// Variables
	uint32_t pwrRegVoltageScale;
	bool isPwrRegOverDrive;
	RCC_OscInitTypeDef iosc;
	RCC_ClkInitTypeDef iclk;
	uint32_t flashLatency;
	bool backupHsiEnable;
	bool isBackupHsiUsed;

	uint32_t systemCoreClock;
	uint32_t sysclk;
	uint32_t hclk;
	uint32_t pclk1Apb1;
	uint32_t pclk2Apb2;

	uint32_t systemCoreClock_Ctor;
	uint32_t sysclk_Ctor;
	uint32_t hclk_Ctor;
	uint32_t pclk1Apb1_Ctor;
	uint32_t pclk2Apb2_Ctor;
	uint32_t pclk1Apb1Tim_Ctor;
	uint32_t pclk2Apb2Tim_Ctor;
	uint32_t mainPllInputClk_Ctor;

	// Info
	bool isInit;
	bool wasCtorCalled;
} AlxClk;


//******************************************************************************
// Constructor
//******************************************************************************
void AlxClk_Ctor
(
	AlxClk* me,
	AlxClk_Config config
);


#endif

#ifdef __cplusplus
}
#endif

#endif // ALX_CLK_MCU_STM32_H
