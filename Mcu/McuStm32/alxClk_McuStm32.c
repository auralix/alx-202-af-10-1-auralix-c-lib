/**
  ******************************************************************************
  * @file		alxClk_McuStm32.c
  * @brief		Auralix C Library - ALX Clock MCU STM32 Module
  * @copyright	Copyright (C) 2020-2022 Auralix d.o.o. All rights reserved.
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
#include "alxClk_McuStm32.h"
#include "alxClk.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB) && (defined(ALX_STM32F1) || defined(ALX_STM32F4) || defined(ALX_STM32G4) || defined(ALX_STM32L0) || defined(ALX_STM32L4))


//******************************************************************************
// Private Functions
//******************************************************************************
static void AlxClk_PeriphGpio_EnableClk(AlxClk* me);
static bool AlxClk_AreClkNok(AlxClk* me);
#if defined(STM32F1)
static void AlxClk_Ctor_McuStm32F1_Sysclk_8MHz_Pclk1Apb1_8MHz_Pclk2Apb2_8MHz_Hsi_8MHz_Default(AlxClk* me);
static void AlxClk_Ctor_McuStm32F1_Sysclk_64MHz_Pclk1Apb1_32MHz_Pclk2Apb2_64MHz_Hsi_8MHz(AlxClk* me);
static void AlxClk_Ctor_McuStm32F1_Sysclk_64MHz_Pclk1Apb1_32MHz_Pclk2Apb2_64MHz_Hse_8MHz(AlxClk* me);
#endif
#if defined(STM32F4)
static void AlxClk_Ctor_McuStm32F4_Sysclk_16MHz_Pclk1Apb1_16MHz_Pclk2Apb2_16MHz_Hsi_16MHz_Default(AlxClk* me);
static void AlxClk_Ctor_McuStm32F4_Sysclk_180MHz_Pclk1Apb1_45MHz_Pclk2Apb2_90MHz_Hsi_16MHz(AlxClk* me);
static void AlxClk_Ctor_McuStm32F4_Sysclk_180MHz_Pclk1Apb1_45MHz_Pclk2Apb2_90MHz_Hse_12MHz(AlxClk* me);
static void AlxClk_Ctor_McuStm32F4_Sysclk_180MHz_Pclk1Apb1_45MHz_Pclk2Apb2_90MHz_Hse_25MHz(AlxClk* me);
#endif
#if defined(STM32G4)
static void AlxClk_Ctor_McuStm32G4_Sysclk_16MHz_Pclk1Apb1_16MHz_Pclk2Apb2_16MHz_Hsi_16MHz_Default(AlxClk* me);
static void AlxClk_Ctor_McuStm32G4_Sysclk_170MHz_Pclk1Apb1_170MHz_Pclk2Apb2_170MHz_Hsi_16MHz(AlxClk* me);
static void AlxClk_Ctor_McuStm32G4_Sysclk_170MHz_Pclk1Apb1_170MHz_Pclk2Apb2_170MHz_Hse_12MHz(AlxClk* me);
#endif
#if defined(STM32L0)
static void AlxClk_Ctor_McuStm32L0_Sysclk_2MHz1_Pclk1Apb1_2MHz1_Pclk2Apb2_2MHz1_Msi_2MHz1_Default(AlxClk* me);
static void AlxClk_Ctor_McuStm32L0_Sysclk_32MHz_Pclk1Apb1_32MHz_Pclk2Apb2_32MHz_Hsi_16MHz(AlxClk* me);
static void AlxClk_Ctor_McuStm32L0_Sysclk_524kHz_Pclk1Apb1_524kHz_Pclk2Apb2_524kHz_Msi_2MHz1(AlxClk* me);
#endif
#if defined(STM32L4)
static void AlxClk_Ctor_McuStm32L4_Sysclk_4MHz_Pclk1Apb1_4MHz_Pclk2Apb2_4MHz_Msi_4MHz_Default(AlxClk* me);
static void AlxClk_Ctor_McuStm32L4_Sysclk_16MHz_Pclk1Apb1_16MHz_Pclk2Apb2_16MHz_Hsi_16MHz(AlxClk* me);
#endif


//******************************************************************************
// Constructor
//******************************************************************************
ALX_WEAK void AlxClk_Ctor
(
	AlxClk* me,
	AlxClk_Config config
)
{
	// Parameters
	me->config = config;

	// Variables
	#if defined(STM32F1)
	if		(me->config == AlxClk_Config_McuStm32F1_Sysclk_8MHz_Pclk1Apb1_8MHz_Pclk2Apb2_8MHz_Hsi_8MHz_Default)					{ AlxClk_Ctor_McuStm32F1_Sysclk_8MHz_Pclk1Apb1_8MHz_Pclk2Apb2_8MHz_Hsi_8MHz_Default(me);		me->backupHsiEnable = false;	}
	else if	(me->config == AlxClk_Config_McuStm32F1_Sysclk_64MHz_Pclk1Apb1_32MHz_Pclk2Apb2_64MHz_Hsi_8MHz)						{ AlxClk_Ctor_McuStm32F1_Sysclk_64MHz_Pclk1Apb1_32MHz_Pclk2Apb2_64MHz_Hsi_8MHz(me);				me->backupHsiEnable = false;	}
	else if	(me->config == AlxClk_Config_McuStm32F1_Sysclk_64MHz_Pclk1Apb1_32MHz_Pclk2Apb2_64MHz_Hse_8MHz)						{ AlxClk_Ctor_McuStm32F1_Sysclk_64MHz_Pclk1Apb1_32MHz_Pclk2Apb2_64MHz_Hse_8MHz(me);				me->backupHsiEnable = false;	}
	else if	(me->config == AlxClk_Config_McuStm32F1_Sysclk_64MHz_Pclk1Apb1_32MHz_Pclk2Apb2_64MHz_Hse_8MHz_BackupHsi_8Mhz)		{ AlxClk_Ctor_McuStm32F1_Sysclk_64MHz_Pclk1Apb1_32MHz_Pclk2Apb2_64MHz_Hse_8MHz(me);				me->backupHsiEnable = true;		}
	#endif
	#if defined(STM32F4)
	if		(me->config == AlxClk_Config_McuStm32F4_Sysclk_16MHz_Pclk1Apb1_16MHz_Pclk2Apb2_16MHz_Hsi_16MHz_Default)				{ AlxClk_Ctor_McuStm32F4_Sysclk_16MHz_Pclk1Apb1_16MHz_Pclk2Apb2_16MHz_Hsi_16MHz_Default(me);	me->backupHsiEnable = false;	}
	else if	(me->config == AlxClk_Config_McuStm32F4_Sysclk_180MHz_Pclk1Apb1_45MHz_Pclk2Apb2_90MHz_Hsi_16MHz)					{ AlxClk_Ctor_McuStm32F4_Sysclk_180MHz_Pclk1Apb1_45MHz_Pclk2Apb2_90MHz_Hsi_16MHz(me);			me->backupHsiEnable = false;	}
	else if	(me->config == AlxClk_Config_McuStm32F4_Sysclk_180MHz_Pclk1Apb1_45MHz_Pclk2Apb2_90MHz_Hse_12MHz)					{ AlxClk_Ctor_McuStm32F4_Sysclk_180MHz_Pclk1Apb1_45MHz_Pclk2Apb2_90MHz_Hse_12MHz(me);			me->backupHsiEnable = false;	}
	else if	(me->config == AlxClk_Config_McuStm32F4_Sysclk_180MHz_Pclk1Apb1_45MHz_Pclk2Apb2_90MHz_Hse_12MHz_BackupHsi_16Mhz)	{ AlxClk_Ctor_McuStm32F4_Sysclk_180MHz_Pclk1Apb1_45MHz_Pclk2Apb2_90MHz_Hse_12MHz(me);			me->backupHsiEnable = true;		}
	else if	(me->config == AlxClk_Config_McuStm32F4_Sysclk_180MHz_Pclk1Apb1_45MHz_Pclk2Apb2_90MHz_Hse_25MHz)					{ AlxClk_Ctor_McuStm32F4_Sysclk_180MHz_Pclk1Apb1_45MHz_Pclk2Apb2_90MHz_Hse_25MHz(me);			me->backupHsiEnable = false;	}
	#endif
	#if defined(STM32G4)
	if		(me->config == AlxClk_Config_McuStm32G4_Sysclk_16MHz_Pclk1Apb1_16MHz_Pclk2Apb2_16MHz_Hsi_16MHz_Default)				{ AlxClk_Ctor_McuStm32G4_Sysclk_16MHz_Pclk1Apb1_16MHz_Pclk2Apb2_16MHz_Hsi_16MHz_Default(me);	me->backupHsiEnable = false;	}
	else if	(me->config == AlxClk_Config_McuStm32G4_Sysclk_170MHz_Pclk1Apb1_170MHz_Pclk2Apb2_170MHz_Hsi_16MHz)					{ AlxClk_Ctor_McuStm32G4_Sysclk_170MHz_Pclk1Apb1_170MHz_Pclk2Apb2_170MHz_Hsi_16MHz(me);			me->backupHsiEnable = false;	}
	else if	(me->config == AlxClk_Config_McuStm32G4_Sysclk_170MHz_Pclk1Apb1_170MHz_Pclk2Apb2_170MHz_Hse_12MHz)					{ AlxClk_Ctor_McuStm32G4_Sysclk_170MHz_Pclk1Apb1_170MHz_Pclk2Apb2_170MHz_Hse_12MHz(me);			me->backupHsiEnable = false;	}
	else if	(me->config == AlxClk_Config_McuStm32G4_Sysclk_170MHz_Pclk1Apb1_170MHz_Pclk2Apb2_170MHz_Hse_12MHz_BackupHsi_16Mhz)	{ AlxClk_Ctor_McuStm32G4_Sysclk_170MHz_Pclk1Apb1_170MHz_Pclk2Apb2_170MHz_Hse_12MHz(me);			me->backupHsiEnable = true;		}
	#endif
	#if defined(STM32L0)
	if		(me->config == AlxClk_Config_McuStm32L0_Sysclk_2MHz1_Pclk1Apb1_2MHz1_Pclk2Apb2_2MHz1_Msi_2MHz1_Default)				{ AlxClk_Ctor_McuStm32L0_Sysclk_2MHz1_Pclk1Apb1_2MHz1_Pclk2Apb2_2MHz1_Msi_2MHz1_Default(me);	me->backupHsiEnable = false;	}
	else if	(me->config == AlxClk_Config_McuStm32L0_Sysclk_32MHz_Pclk1Apb1_32MHz_Pclk2Apb2_32MHz_Hsi_16MHz)						{ AlxClk_Ctor_McuStm32L0_Sysclk_32MHz_Pclk1Apb1_32MHz_Pclk2Apb2_32MHz_Hsi_16MHz(me);			me->backupHsiEnable = false;	}
	else if	(me->config == AlxClk_Config_McuStm32L0_Sysclk_524kHz_Pclk1Apb1_524kHz_Pclk2Apb2_524kHz_Msi_2MHz1)					{ AlxClk_Ctor_McuStm32L0_Sysclk_524kHz_Pclk1Apb1_524kHz_Pclk2Apb2_524kHz_Msi_2MHz1(me);			me->backupHsiEnable = false;	}
	#endif
	#if defined(STM32L4)
	if		(me->config == AlxClk_Config_McuStm32L4_Sysclk_4MHz_Pclk1Apb1_4MHz_Pclk2Apb2_4MHz_Msi_4MHz_Default)					{ AlxClk_Ctor_McuStm32L4_Sysclk_4MHz_Pclk1Apb1_4MHz_Pclk2Apb2_4MHz_Msi_4MHz_Default(me);		me->backupHsiEnable = false;	}
	else if	(me->config == AlxClk_Config_McuStm32L4_Sysclk_16MHz_Pclk1Apb1_16MHz_Pclk2Apb2_16MHz_Hsi_16MHz)						{ AlxClk_Ctor_McuStm32L4_Sysclk_16MHz_Pclk1Apb1_16MHz_Pclk2Apb2_16MHz_Hsi_16MHz(me);			me->backupHsiEnable = false;	}
	#endif
	else																														{ ALX_CLK_ASSERT(false); return; } // We shouldn't get here

	me->isBackupHsiUsed = false;
	me->systemCoreClock = 0;
	me->sysclk = 0;
	me->hclk = 0;
	me->pclk1Apb1 = 0;
	me->pclk2Apb2 = 0;

	// Info
	me->isInit = false;
	me->wasCtorCalled = true;
}


//******************************************************************************
// Functions
//******************************************************************************
ALX_WEAK Alx_Status AlxClk_Init(AlxClk* me)
{
	//ALX_CLK_ASSERT(me->isInit == false);
	ALX_CLK_ASSERT(me->wasCtorCalled == true);

	// #1 Init Oscilator
	if (me->backupHsiEnable) HAL_RCC_EnableCSS();
	if(HAL_RCC_OscConfig(&me->iosc) != HAL_OK)
	{
		// #5.1 If backup HSI is used
		if (me->backupHsiEnable)
		{
			// Reset wasCtorCalled
			me->wasCtorCalled = false;

			// Ctor
			#if defined(STM32F4)
			if		(me->config == AlxClk_Config_McuStm32F4_Sysclk_180MHz_Pclk1Apb1_45MHz_Pclk2Apb2_90MHz_Hse_12MHz_BackupHsi_16Mhz)	{ AlxClk_Ctor_McuStm32F4_Sysclk_180MHz_Pclk1Apb1_45MHz_Pclk2Apb2_90MHz_Hsi_16MHz(me);	me->isBackupHsiUsed = true; }
			#endif
			#if defined(STM32F1)
			if (me->config == AlxClk_Config_McuStm32F1_Sysclk_64MHz_Pclk1Apb1_32MHz_Pclk2Apb2_64MHz_Hse_8MHz_BackupHsi_8Mhz)	{ AlxClk_Ctor_McuStm32F1_Sysclk_64MHz_Pclk1Apb1_32MHz_Pclk2Apb2_64MHz_Hsi_8MHz(me); }
			#endif
			#if defined(STM32G4)
			if (me->config == AlxClk_Config_McuStm32G4_Sysclk_170MHz_Pclk1Apb1_170MHz_Pclk2Apb2_170MHz_Hse_12MHz_BackupHsi_16Mhz)	{ AlxClk_Ctor_McuStm32G4_Sysclk_170MHz_Pclk1Apb1_170MHz_Pclk2Apb2_170MHz_Hsi_16MHz(me);	me->isBackupHsiUsed = true; }
			#endif
			else																														{ ALX_CLK_ASSERT(false); return Alx_Err; } // We shouldn't get here

			// Set wasCtorCalled
			me->wasCtorCalled = true;

			// Init
			if(HAL_RCC_OscConfig(&me->iosc) != HAL_OK)	{ ALX_CLK_TRACE("ErrOscBackup"); return Alx_Err; }
		}
		// #5.2 Trace & Return
		else
		{
			ALX_CLK_TRACE("ErrOsc");
			return Alx_Err;
		}
	}

	// #2 Enable GPIO Periphery Clocks
	AlxClk_PeriphGpio_EnableClk(me);

	// #3 Enable PWR Periphery Clock
	__HAL_RCC_PWR_CLK_ENABLE();

	// #4 Init Power Regulator
	// STM32F4, STM32G4, STM32L4
	#if defined(ALX_STM32F4) || defined(ALX_STM32G4) || defined(ALX_STM32L4)
	if(HAL_PWREx_ControlVoltageScaling(me->pwrRegVoltageScale) != HAL_OK) { ALX_CLK_TRACE("ErrPwrRegVoltageScale"); return Alx_Err; };
	#endif
	#if defined(STM32F427xx) || defined(STM32F437xx) || defined(STM32F429xx) || defined(STM32F439xx) ||\
		defined(STM32F446xx) || defined(STM32F469xx) || defined(STM32F479xx)
	if (me->isPwrRegOverDrive)
	{
		if(HAL_PWREx_EnableOverDrive() != HAL_OK) { ALX_CLK_TRACE("ErrPwrRegOverDrive"); return Alx_Err; };
	}
	#endif

	// STM32L0
	#if defined(ALX_STM32L0)
	while(__HAL_PWR_GET_FLAG(PWR_FLAG_VOS));
	__HAL_PWR_VOLTAGESCALING_CONFIG(me->pwrRegVoltageScale);
	while(__HAL_PWR_GET_FLAG(PWR_FLAG_VOS));
	#endif

	// #5 Enable SYSCFG Periphery Clock
	__HAL_RCC_SYSCFG_CLK_ENABLE();

	// #6 Init Clocks
	if(HAL_RCC_ClockConfig(&me->iclk, me->flashLatency) != HAL_OK)	{ ALX_CLK_TRACE("ErrClk"); return Alx_Err; }

	// #7 Check Clocks
	if(AlxClk_AreClkNok(me)) { ALX_CLK_TRACE("ErrCheck"); return Alx_Err; }

	// #8 Set isInit
	me->isInit = true;

	// #9 Return OK
	return Alx_Ok;
}
ALX_WEAK uint32_t AlxClk_GetClk_Hz(AlxClk* me, AlxClk_Clk clk)
{
	ALX_CLK_ASSERT(me->wasCtorCalled == true);

	if(me->isInit)
	{
		if(clk == AlxClk_Clk_McuStm32_SystemCoreClock )		return me->systemCoreClock;
		if(clk == AlxClk_Clk_McuStm32_Sysclk )				return me->sysclk;
		if(clk == AlxClk_Clk_McuStm32_Hclk )				return me->hclk;
		if(clk == AlxClk_Clk_McuStm32_Pclk1Apb1 )			return me->pclk1Apb1;
		if(clk == AlxClk_Clk_McuStm32_Pclk2Apb2 )			return me->pclk2Apb2;
	}

	if(clk == AlxClk_Clk_McuStm32_SystemCoreClock_Ctor )	return me->systemCoreClock_Ctor;
	if(clk == AlxClk_Clk_McuStm32_Sysclk_Ctor )				return me->sysclk_Ctor;
	if(clk == AlxClk_Clk_McuStm32_Hclk_Ctor )				return me->hclk_Ctor;
	if(clk == AlxClk_Clk_McuStm32_Pclk1Apb1_Ctor )			return me->pclk1Apb1_Ctor;
	if(clk == AlxClk_Clk_McuStm32_Pclk2Apb2_Ctor )			return me->pclk2Apb2_Ctor;
	if(clk == AlxClk_Clk_McuStm32_Pclk1Apb1Tim_Ctor )		return me->pclk1Apb1Tim_Ctor;
	if(clk == AlxClk_Clk_McuStm32_Pclk2Apb2Tim_Ctor )		return me->pclk2Apb2Tim_Ctor;
	if(clk == AlxClk_Clk_McuStm32_MainPllInputClk_Ctor )	return me->mainPllInputClk_Ctor;

	ALX_CLK_ASSERT(false); // We shouldn't get here
	return 0;
}
void AlxClk_Irq_Handle(AlxClk* me)
{
	(void)me;
	HAL_RCC_NMI_IRQHandler();
}


//******************************************************************************
// Private Functions
//******************************************************************************
static void AlxClk_PeriphGpio_EnableClk(AlxClk* me)
{
	#if defined(GPIOA)
	__HAL_RCC_GPIOA_CLK_ENABLE();
	#endif
	#if defined(GPIOB)
	__HAL_RCC_GPIOB_CLK_ENABLE();
	#endif
	#if defined(GPIOC)
	__HAL_RCC_GPIOC_CLK_ENABLE();
	#endif
	#if defined(GPIOD)
	__HAL_RCC_GPIOD_CLK_ENABLE();
	#endif
	#if defined(GPIOE)
	__HAL_RCC_GPIOE_CLK_ENABLE();
	#endif
	#if defined(GPIOF)
	__HAL_RCC_GPIOF_CLK_ENABLE();
	#endif
	#if defined(GPIOG)
	__HAL_RCC_GPIOG_CLK_ENABLE();
	#endif
	#if defined(GPIOH)
	__HAL_RCC_GPIOH_CLK_ENABLE();
	#endif
	#if defined(GPIOI)
	__HAL_RCC_GPIOI_CLK_ENABLE();
	#endif
	#if defined(GPIOJ)
	__HAL_RCC_GPIOJ_CLK_ENABLE();
	#endif
	#if defined(GPIOK)
	__HAL_RCC_GPIOK_CLK_ENABLE();
	#endif
	#if defined(GPIOL)
	__HAL_RCC_GPIOL_CLK_ENABLE();
	#endif
	#if defined(GPIOM)
	__HAL_RCC_GPIOM_CLK_ENABLE();
	#endif
	#if defined(GPION)
	__HAL_RCC_GPION_CLK_ENABLE();
	#endif
}
static bool AlxClk_AreClkNok(AlxClk* me)
{
	me->systemCoreClock = SystemCoreClock;
	me->sysclk = HAL_RCC_GetSysClockFreq();
	me->hclk = HAL_RCC_GetHCLKFreq();
	me->pclk1Apb1 = HAL_RCC_GetPCLK1Freq();
	me->pclk2Apb2 = HAL_RCC_GetPCLK2Freq();

	if		(SystemCoreClock != me->systemCoreClock_Ctor)	{ ALX_CLK_TRACE("ErrSystemCoreClock");	return true; }
	else if	(me->sysclk != me->sysclk_Ctor)					{ ALX_CLK_TRACE("ErrSysclk");			return true; }
	else if	(me->hclk != me->hclk_Ctor)						{ ALX_CLK_TRACE("ErrHclk");				return true; }
	else if	(me->pclk1Apb1 != me->pclk1Apb1_Ctor)			{ ALX_CLK_TRACE("ErrPclk1Apb1");		return true; }
	else if	(me->pclk2Apb2 != me->pclk2Apb2_Ctor)			{ ALX_CLK_TRACE("ErrPclk2Apb2");		return true; }
	else													{ return false; }
}
#if defined(ALX_STM32F1)
static void AlxClk_Ctor_McuStm32F1_Sysclk_8MHz_Pclk1Apb1_8MHz_Pclk2Apb2_8MHz_Hsi_8MHz_Default(AlxClk* me)
{

	// #1 Set power regulator
	me->pwrRegVoltageScale = false;
	me->isPwrRegOverDrive = false;

	// #2 Set oscillators
	me->iosc.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	me->iosc.HSIState = RCC_HSI_ON;
	me->iosc.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	me->iosc.PLL.PLLState = RCC_PLL_OFF;
	me->iosc.PLL.PLLSource = 0;
	me->iosc.PLL.PLLMUL = 0;


	// #3 Set clocks
	me->iclk.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	me->iclk.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
	me->iclk.AHBCLKDivider = RCC_SYSCLK_DIV1;
	me->iclk.APB1CLKDivider = RCC_HCLK_DIV1;
	me->iclk.APB2CLKDivider = RCC_HCLK_DIV1;

	// #4 Set flash latency
	me->flashLatency = FLASH_LATENCY_0;

	// #5 Set expected clocks
	me->systemCoreClock_Ctor = 8000000UL;
	me->sysclk_Ctor = 8000000UL;
	me->hclk_Ctor = 8000000UL;
	me->pclk1Apb1_Ctor = 8000000UL;
	me->pclk2Apb2_Ctor = 8000000UL;
	me->pclk1Apb1Tim_Ctor = 8000000UL;
	me->pclk2Apb2Tim_Ctor = 8000000UL;
	me->mainPllInputClk_Ctor = 0;
}
static void AlxClk_Ctor_McuStm32F1_Sysclk_64MHz_Pclk1Apb1_32MHz_Pclk2Apb2_64MHz_Hsi_8MHz(AlxClk* me)
{
	// #1 Set power regulator
	me->pwrRegVoltageScale = false;
	me->isPwrRegOverDrive = false;

	// #2 Set oscillators
	me->iosc.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	me->iosc.HSIState = RCC_HSI_ON;
	me->iosc.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	me->iosc.PLL.PLLState = RCC_PLL_ON;
	me->iosc.PLL.PLLSource = RCC_PLLSOURCE_HSI_DIV2;
	me->iosc.PLL.PLLMUL = RCC_PLL_MUL16;

	// #3 Set clocks
	me->iclk.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	me->iclk.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	me->iclk.AHBCLKDivider = RCC_SYSCLK_DIV1;
	me->iclk.APB1CLKDivider = RCC_HCLK_DIV2;
	me->iclk.APB2CLKDivider = RCC_HCLK_DIV1;

	// #4 Set flash latency
	me->flashLatency = FLASH_LATENCY_1; // Change to 1 JK, goes to hard fault if its 0

	// #5 Set expected clocks
	me->systemCoreClock_Ctor = 64000000UL;
	me->sysclk_Ctor = 64000000UL;
	me->hclk_Ctor = 64000000UL;
	me->pclk1Apb1_Ctor = 32000000UL;
	me->pclk2Apb2_Ctor = 64000000UL;
	me->pclk1Apb1Tim_Ctor = 64000000UL;
	me->pclk2Apb2Tim_Ctor = 64000000UL;
	me->mainPllInputClk_Ctor = 0;
}
static void AlxClk_Ctor_McuStm32F1_Sysclk_64MHz_Pclk1Apb1_32MHz_Pclk2Apb2_64MHz_Hse_8MHz(AlxClk* me)
{
	// #1 Set power regulator
	me->pwrRegVoltageScale = false;
	me->isPwrRegOverDrive = false;

	// #2 Set oscillators
	me->iosc.OscillatorType = RCC_OSCILLATORTYPE_HSE;	// copied from cube
	me->iosc.HSEState = RCC_HSE_ON;						// copied from cube
	me->iosc.HSEPredivValue = RCC_HSE_PREDIV_DIV1;		// copied from cube
	me->iosc.PLL.PLLState = RCC_PLL_ON;				// same as cube and alx
	me->iosc.PLL.PLLSource = RCC_PLLSOURCE_HSE;		// same as cube and alx
	me->iosc.PLL.PLLMUL = RCC_PLL_MUL8;				// same as cube and alx

	// #3 Set clocks
	me->iclk.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2; // same as cube and alx
	me->iclk.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;	// same as cube, not sure??
	me->iclk.AHBCLKDivider = RCC_SYSCLK_DIV1;	// same as cube
	me->iclk.APB1CLKDivider = RCC_HCLK_DIV2;	// same as cube
	me->iclk.APB2CLKDivider = RCC_HCLK_DIV1;	// same as cube

	// #4 Set flash latency
	me->flashLatency = FLASH_LATENCY_1;		// Cube set this to 0, did not work. Googling suggested this is a bug with cube and this should larger than 0. 1 Worked fine.

	// #5 Set expected clocks
	me->systemCoreClock_Ctor =	64000000UL;		// ??
	me->sysclk_Ctor =			64000000UL;		// 64 MHz OK - same as cube
	me->hclk_Ctor =				64000000UL;		// 64 MHz OK - same as cube
	me->pclk1Apb1_Ctor =		32000000UL;		// 32 MHz OK - same as cube
	me->pclk2Apb2_Ctor =		64000000UL;		// 64 MHz OK - same as cube
	me->pclk1Apb1Tim_Ctor =		64000000UL;		// 64 MHz OK - same as cube
	me->pclk2Apb2Tim_Ctor =		64000000UL;		// 64 MHz OK - same as cube
	me->mainPllInputClk_Ctor =	8000000UL;		// 8Mhz
}
#endif
#if defined(ALX_STM32F4)
static void AlxClk_Ctor_McuStm32F4_Sysclk_16MHz_Pclk1Apb1_16MHz_Pclk2Apb2_16MHz_Hsi_16MHz_Default(AlxClk* me)
{
	// #1 Set power regulator
	me->pwrRegVoltageScale = PWR_REGULATOR_VOLTAGE_SCALE1;
	me->isPwrRegOverDrive = false;

	// #2 Set oscillators
	me->iosc.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	me->iosc.HSEState = RCC_HSE_OFF;
	me->iosc.LSEState = RCC_LSE_OFF;
	me->iosc.HSIState = RCC_HSI_ON;
	me->iosc.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	me->iosc.LSIState = RCC_LSI_OFF;
	me->iosc.PLL.PLLState = RCC_PLL_OFF;
	me->iosc.PLL.PLLSource = 0;
	me->iosc.PLL.PLLM = 0;
	me->iosc.PLL.PLLN = 0;
	me->iosc.PLL.PLLP = 0;
	me->iosc.PLL.PLLQ = 0;
	#if defined(STM32F410Tx) || defined(STM32F410Cx) || defined(STM32F410Rx) || defined(STM32F446xx) || defined(STM32F469xx) ||\
		defined(STM32F479xx) || defined(STM32F412Zx) || defined(STM32F412Vx) || defined(STM32F412Rx) || defined(STM32F412Cx) ||\
		defined(STM32F413xx) || defined(STM32F423xx)
	me->iosc.PLL.PLLR = 0;
	#endif

	// #3 Set clocks
	me->iclk.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	me->iclk.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
	me->iclk.AHBCLKDivider = RCC_SYSCLK_DIV1;
	me->iclk.APB1CLKDivider = RCC_HCLK_DIV1;
	me->iclk.APB2CLKDivider = RCC_HCLK_DIV1;

	// #4 Set flash latency
	me->flashLatency = FLASH_LATENCY_0;

	// #5 Set expected clocks
	me->systemCoreClock_Ctor = 16000000UL;
	me->sysclk_Ctor = 16000000UL;
	me->hclk_Ctor = 16000000UL;
	me->pclk1Apb1_Ctor = 16000000UL;
	me->pclk2Apb2_Ctor = 16000000UL;
	me->pclk1Apb1Tim_Ctor = 16000000UL;
	me->pclk2Apb2Tim_Ctor = 16000000UL;
	me->mainPllInputClk_Ctor = 0;
}
static void AlxClk_Ctor_McuStm32F4_Sysclk_180MHz_Pclk1Apb1_45MHz_Pclk2Apb2_90MHz_Hsi_16MHz(AlxClk* me)
{
	#if defined(STM32F427xx) || defined(STM32F437xx) || defined(STM32F429xx) || defined(STM32F439xx) ||\
		defined(STM32F446xx) || defined(STM32F469xx) || defined(STM32F479xx)
	// #1 Set power regulator
	me->pwrRegVoltageScale = PWR_REGULATOR_VOLTAGE_SCALE1;
	me->isPwrRegOverDrive = true;

	// #2 Set oscillators
	me->iosc.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	me->iosc.HSEState = RCC_HSE_OFF;
	me->iosc.LSEState = RCC_LSE_OFF;
	me->iosc.HSIState = RCC_HSI_ON;
	me->iosc.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	me->iosc.LSIState = RCC_LSI_OFF;
	me->iosc.PLL.PLLState = RCC_PLL_ON;
	me->iosc.PLL.PLLSource = RCC_PLLSOURCE_HSI;
	me->iosc.PLL.PLLM = 8; // MF: So that VCO input frequency is 2MHz (that way it limits PLL jitter)
	me->iosc.PLL.PLLN = 180;
	me->iosc.PLL.PLLP = RCC_PLLP_DIV2;
	me->iosc.PLL.PLLQ = 15; // MF: Sets 48MHz for USB OTG FS, SDIO and random number generator
	#if defined(STM32F410Tx) || defined(STM32F410Cx) || defined(STM32F410Rx) || defined(STM32F446xx) || defined(STM32F469xx) ||\
		defined(STM32F479xx) || defined(STM32F412Zx) || defined(STM32F412Vx) || defined(STM32F412Rx) || defined(STM32F412Cx) ||\
		defined(STM32F413xx) || defined(STM32F423xx)
	me->iosc.PLL.PLLR = 0;
	#endif

	// #3 Set clocks
	me->iclk.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	me->iclk.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	me->iclk.AHBCLKDivider = RCC_SYSCLK_DIV1;
	me->iclk.APB1CLKDivider = RCC_HCLK_DIV4; // MF: Max 45MHz
	me->iclk.APB2CLKDivider = RCC_HCLK_DIV2; // MF: Max 90MHz

	// #4 Set flash latency
	me->flashLatency = FLASH_LATENCY_5;

	// #5 Set expected clocks
	me->systemCoreClock_Ctor = 180000000UL;
	me->sysclk_Ctor = 180000000UL;
	me->hclk_Ctor = 180000000UL;
	me->pclk1Apb1_Ctor = 45000000UL;
	me->pclk2Apb2_Ctor = 90000000UL;
	me->pclk1Apb1Tim_Ctor = 45000000UL * 2;
	me->pclk2Apb2Tim_Ctor = 90000000UL * 2;
	me->mainPllInputClk_Ctor = HSI_VALUE / me->iosc.PLL.PLLM;
	#else
	ALX_CLK_ASSERT(false); // We shouldn't get here
	#endif
}
static void AlxClk_Ctor_McuStm32F4_Sysclk_180MHz_Pclk1Apb1_45MHz_Pclk2Apb2_90MHz_Hse_12MHz(AlxClk* me)
{
	#if defined(STM32F427xx) || defined(STM32F437xx) || defined(STM32F429xx) || defined(STM32F439xx) ||\
	defined(STM32F446xx) || defined(STM32F469xx) || defined(STM32F479xx)
	// #1 Set power regulator
	me->pwrRegVoltageScale = PWR_REGULATOR_VOLTAGE_SCALE1;
	me->isPwrRegOverDrive = true;

	// #2 Set oscillators
	me->iosc.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	me->iosc.HSEState = RCC_HSE_ON;
	me->iosc.LSEState = RCC_LSE_OFF;
	me->iosc.HSIState = RCC_HSI_ON;
	me->iosc.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	me->iosc.LSIState = RCC_LSI_OFF;
	me->iosc.PLL.PLLState = RCC_PLL_ON;
	me->iosc.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	me->iosc.PLL.PLLM = 6; // MF: So that VCO input frequency is 2MHz (that way it limits PLL jitter)
	me->iosc.PLL.PLLN = 180;
	me->iosc.PLL.PLLP = RCC_PLLP_DIV2;
	me->iosc.PLL.PLLQ = 15; // MF: Sets 48MHz for USB OTG FS, SDIO and random number generator
	#if defined(STM32F410Tx) || defined(STM32F410Cx) || defined(STM32F410Rx) || defined(STM32F446xx) || defined(STM32F469xx) ||\
		defined(STM32F479xx) || defined(STM32F412Zx) || defined(STM32F412Vx) || defined(STM32F412Rx) || defined(STM32F412Cx) ||\
		defined(STM32F413xx) || defined(STM32F423xx)
	me->iosc.PLL.PLLR = 0;
	#endif

	// #3 Set clocks
	me->iclk.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	me->iclk.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	me->iclk.AHBCLKDivider = RCC_SYSCLK_DIV1;
	me->iclk.APB1CLKDivider = RCC_HCLK_DIV4; // MF: Max 45MHz
	me->iclk.APB2CLKDivider = RCC_HCLK_DIV2; // MF: Max 90MHz

	// #4 Set flash latency
	me->flashLatency = FLASH_LATENCY_5;

	// #5 Set expected clocks
	me->systemCoreClock_Ctor = 180000000UL;
	me->sysclk_Ctor = 180000000UL;
	me->hclk_Ctor = 180000000UL;
	me->pclk1Apb1_Ctor = 45000000UL;
	me->pclk2Apb2_Ctor = 90000000UL;
	me->pclk1Apb1Tim_Ctor = 45000000UL * 2;
	me->pclk2Apb2Tim_Ctor = 90000000UL * 2;
	me->mainPllInputClk_Ctor = HSE_VALUE / me->iosc.PLL.PLLM;
	#else
	ALX_CLK_ASSERT(false); // We shouldn't get here
	#endif
}
static void AlxClk_Ctor_McuStm32F4_Sysclk_180MHz_Pclk1Apb1_45MHz_Pclk2Apb2_90MHz_Hse_25MHz(AlxClk* me)
{
	#if defined(STM32F427xx) || defined(STM32F437xx) || defined(STM32F429xx) || defined(STM32F439xx) ||\
	defined(STM32F446xx) || defined(STM32F469xx) || defined(STM32F479xx)
	// #1 Set power regulator
	me->pwrRegVoltageScale = PWR_REGULATOR_VOLTAGE_SCALE1;
	me->isPwrRegOverDrive = true;

	// #2 Set oscillators
	me->iosc.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	me->iosc.HSEState = RCC_HSE_ON;
	me->iosc.LSEState = RCC_LSE_OFF;
	me->iosc.HSIState = RCC_HSI_ON;
	me->iosc.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	me->iosc.LSIState = RCC_LSI_OFF;
	me->iosc.PLL.PLLState = RCC_PLL_ON;
	me->iosc.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	me->iosc.PLL.PLLM = 25; // VCO input frequency is 1MHz
	me->iosc.PLL.PLLN = 360;
	me->iosc.PLL.PLLP = RCC_PLLP_DIV2;
	me->iosc.PLL.PLLQ = 15;
	#if defined(STM32F410Tx) || defined(STM32F410Cx) || defined(STM32F410Rx) || defined(STM32F446xx) || defined(STM32F469xx) ||\
		defined(STM32F479xx) || defined(STM32F412Zx) || defined(STM32F412Vx) || defined(STM32F412Rx) || defined(STM32F412Cx) ||\
		defined(STM32F413xx) || defined(STM32F423xx)
	me->iosc.PLL.PLLR = 0;
	#endif

	// #3 Set clocks
	me->iclk.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	me->iclk.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	me->iclk.AHBCLKDivider = RCC_SYSCLK_DIV1;
	me->iclk.APB1CLKDivider = RCC_HCLK_DIV4; // MF: Max 45MHz
	me->iclk.APB2CLKDivider = RCC_HCLK_DIV2; // MF: Max 90MHz

	// #4 Set flash latency
	me->flashLatency = FLASH_LATENCY_5;

	// #5 Set expected clocks
	me->systemCoreClock_Ctor = 180000000UL;
	me->sysclk_Ctor = 180000000UL;
	me->hclk_Ctor = 180000000UL;
	me->pclk1Apb1_Ctor = 45000000UL;
	me->pclk2Apb2_Ctor = 90000000UL;
	me->pclk1Apb1Tim_Ctor = 45000000UL * 2;
	me->pclk2Apb2Tim_Ctor = 90000000UL * 2;
	me->mainPllInputClk_Ctor = HSE_VALUE / me->iosc.PLL.PLLM;
	#else
	ALX_CLK_ASSERT(false); // We shouldn't get here
	#endif
}
#endif
#if defined(ALX_STM32G4)
static void AlxClk_Ctor_McuStm32G4_Sysclk_16MHz_Pclk1Apb1_16MHz_Pclk2Apb2_16MHz_Hsi_16MHz_Default(AlxClk* me)
{
	// #1 Set voltage regulator scale
	me->pwrRegVoltageScale = PWR_REGULATOR_VOLTAGE_SCALE1;
	me->isPwrRegOverDrive = false;

	// #2 Set oscillators
	me->iosc.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	me->iosc.HSEState = RCC_HSE_OFF;
	me->iosc.LSEState = RCC_LSE_OFF;
	me->iosc.HSIState = RCC_HSI_ON;
	me->iosc.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	me->iosc.LSIState = RCC_LSI_OFF;
	me->iosc.HSI48State = RCC_HSI48_OFF;
	me->iosc.PLL.PLLState = RCC_PLL_OFF;
	me->iosc.PLL.PLLSource = RCC_PLLSOURCE_NONE;
	me->iosc.PLL.PLLM = 0;
	me->iosc.PLL.PLLN = 0;
	me->iosc.PLL.PLLP = 0;
	me->iosc.PLL.PLLQ = 0;
	me->iosc.PLL.PLLR = 0;

	// #3 Set clocks
	me->iclk.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	me->iclk.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
	me->iclk.AHBCLKDivider = RCC_SYSCLK_DIV1;
	me->iclk.APB1CLKDivider = RCC_HCLK_DIV1;
	me->iclk.APB2CLKDivider = RCC_HCLK_DIV1;

	// #4 Set flash latency
	me->flashLatency = FLASH_LATENCY_0;

	// #5 Set expected clocks
	me->systemCoreClock_Ctor = 16000000UL;
	me->sysclk_Ctor = 16000000UL;
	me->hclk_Ctor = 16000000UL;
	me->pclk1Apb1_Ctor = 16000000UL;
	me->pclk2Apb2_Ctor = 16000000UL;
	me->pclk1Apb1Tim_Ctor = 16000000UL;
	me->pclk2Apb2Tim_Ctor = 16000000UL;
}
static void AlxClk_Ctor_McuStm32G4_Sysclk_170MHz_Pclk1Apb1_170MHz_Pclk2Apb2_170MHz_Hsi_16MHz(AlxClk* me)
{
	// #1 Set voltage regulator scale
	me->pwrRegVoltageScale = PWR_REGULATOR_VOLTAGE_SCALE1_BOOST;
	me->isPwrRegOverDrive = false;

	// #2 Set oscillators
	me->iosc.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	me->iosc.HSEState = RCC_HSE_OFF;
	me->iosc.LSEState = RCC_LSE_OFF;
	me->iosc.HSIState = RCC_HSI_ON;
	me->iosc.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	me->iosc.LSIState = RCC_LSI_OFF;
	me->iosc.HSI48State = RCC_HSI48_OFF;
	me->iosc.PLL.PLLState = RCC_PLL_ON;
	me->iosc.PLL.PLLSource = RCC_PLLSOURCE_HSI;
	me->iosc.PLL.PLLM = RCC_PLLM_DIV4;
	me->iosc.PLL.PLLN = 85;
	me->iosc.PLL.PLLP = RCC_PLLP_DIV2;
	me->iosc.PLL.PLLQ = RCC_PLLQ_DIV2;
	me->iosc.PLL.PLLR = RCC_PLLR_DIV2;

	// #3 Set clocks
	me->iclk.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	me->iclk.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	me->iclk.AHBCLKDivider = RCC_SYSCLK_DIV1;
	me->iclk.APB1CLKDivider = RCC_HCLK_DIV1;
	me->iclk.APB2CLKDivider = RCC_HCLK_DIV1;

	// #4 Set flash latency
	me->flashLatency = FLASH_LATENCY_4;

	// #5 Set expected clocks
	me->systemCoreClock_Ctor = 170000000UL;
	me->sysclk_Ctor = 170000000UL;
	me->hclk_Ctor = 170000000UL;
	me->pclk1Apb1_Ctor = 170000000UL;
	me->pclk2Apb2_Ctor = 170000000UL;
	me->pclk1Apb1Tim_Ctor = 170000000UL;
	me->pclk2Apb2Tim_Ctor = 170000000UL;
}
static void AlxClk_Ctor_McuStm32G4_Sysclk_170MHz_Pclk1Apb1_170MHz_Pclk2Apb2_170MHz_Hse_12MHz(AlxClk* me)
{
	// #1 Set voltage regulator scale
	me->pwrRegVoltageScale = PWR_REGULATOR_VOLTAGE_SCALE1_BOOST;
	me->isPwrRegOverDrive = false;

	// #2 Set oscillators
	me->iosc.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	me->iosc.HSEState = RCC_HSE_ON;
	me->iosc.LSEState = RCC_LSE_OFF;
	me->iosc.HSIState = RCC_HSI_ON;
	me->iosc.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	me->iosc.LSIState = RCC_LSI_OFF;
	me->iosc.HSI48State = RCC_HSI48_OFF;
	me->iosc.PLL.PLLState = RCC_PLL_ON;
	me->iosc.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	me->iosc.PLL.PLLM = RCC_PLLM_DIV3;
	me->iosc.PLL.PLLN = 85;
	me->iosc.PLL.PLLP = RCC_PLLP_DIV2;
	me->iosc.PLL.PLLQ = RCC_PLLQ_DIV2;
	me->iosc.PLL.PLLR = RCC_PLLR_DIV2;

	// #3 Set clocks
	me->iclk.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	me->iclk.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	me->iclk.AHBCLKDivider = RCC_SYSCLK_DIV1;
	me->iclk.APB1CLKDivider = RCC_HCLK_DIV1;
	me->iclk.APB2CLKDivider = RCC_HCLK_DIV1;

	// #4 Set flash latency
	me->flashLatency = FLASH_LATENCY_4;

	// #5 Set expected clocks
	me->systemCoreClock_Ctor = 170000000UL;
	me->sysclk_Ctor = 170000000UL;
	me->hclk_Ctor = 170000000UL;
	me->pclk1Apb1_Ctor = 170000000UL;
	me->pclk2Apb2_Ctor = 170000000UL;
	me->pclk1Apb1Tim_Ctor = 170000000UL;
	me->pclk2Apb2Tim_Ctor = 170000000UL;
}
#endif
#if defined(ALX_STM32L0)
static void AlxClk_Ctor_McuStm32L0_Sysclk_524kHz_Pclk1Apb1_524kHz_Pclk2Apb2_524kHz_Msi_2MHz1(AlxClk* me)
{
	// #1 Set voltage regulator scale
	me->pwrRegVoltageScale = PWR_REGULATOR_VOLTAGE_SCALE3;
	me->isPwrRegOverDrive = false;

	// #2 Set oscillators
	me->iosc.OscillatorType = RCC_OSCILLATORTYPE_MSI;
	me->iosc.HSEState = RCC_HSE_OFF;
	me->iosc.LSEState = RCC_LSE_OFF;
	me->iosc.HSIState = RCC_HSI_OFF;
	me->iosc.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	me->iosc.LSIState = RCC_LSI_OFF;
	me->iosc.HSI48State = RCC_HSI48_OFF;
	me->iosc.MSIState = RCC_MSI_ON;
	me->iosc.MSICalibrationValue = RCC_MSICALIBRATION_DEFAULT;
	me->iosc.MSIClockRange = RCC_MSIRANGE_3;
	me->iosc.PLL.PLLState = RCC_PLL_OFF;
	me->iosc.PLL.PLLSource = 0;
	me->iosc.PLL.PLLMUL = 0;
	me->iosc.PLL.PLLDIV = 0;

	// #3 Set clocks
	me->iclk.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	me->iclk.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;
	me->iclk.AHBCLKDivider = RCC_SYSCLK_DIV1;
	me->iclk.APB1CLKDivider = RCC_HCLK_DIV1;
	me->iclk.APB2CLKDivider = RCC_HCLK_DIV1;

	// #4 Set flash latency
	me->flashLatency = FLASH_LATENCY_0;

	// #5 Set expected clocks
	me->systemCoreClock_Ctor = 2097152UL;
	me->sysclk_Ctor = 2097152UL;
	me->hclk_Ctor = 2097152UL;
	me->pclk1Apb1_Ctor = 2097152UL;
	me->pclk2Apb2_Ctor = 2097152UL;
	me->pclk1Apb1Tim_Ctor = 2097152UL;
	me->pclk2Apb2Tim_Ctor = 2097152UL;
}
static void AlxClk_Ctor_McuStm32L0_Sysclk_2MHz1_Pclk1Apb1_2MHz1_Pclk2Apb2_2MHz1_Msi_2MHz1_Default(AlxClk* me)
{
	// #1 Set voltage regulator scale
	me->pwrRegVoltageScale = PWR_REGULATOR_VOLTAGE_SCALE3;
	me->isPwrRegOverDrive = false;

	// #2 Set oscillators
	me->iosc.OscillatorType = RCC_OSCILLATORTYPE_MSI;
	me->iosc.HSEState = RCC_HSE_OFF;
	me->iosc.LSEState = RCC_LSE_OFF;
	me->iosc.HSIState = RCC_HSI_OFF;
	me->iosc.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	me->iosc.LSIState = RCC_LSI_OFF;
	me->iosc.HSI48State = RCC_HSI48_OFF;
	me->iosc.MSIState = RCC_MSI_ON;
	me->iosc.MSICalibrationValue = RCC_MSICALIBRATION_DEFAULT;
	me->iosc.MSIClockRange = RCC_MSIRANGE_5;
	me->iosc.PLL.PLLState = RCC_PLL_OFF;
	me->iosc.PLL.PLLSource = 0;
	me->iosc.PLL.PLLMUL = 0;
	me->iosc.PLL.PLLDIV = 0;

	// #3 Set clocks
	me->iclk.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	me->iclk.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;
	me->iclk.AHBCLKDivider = RCC_SYSCLK_DIV1;
	me->iclk.APB1CLKDivider = RCC_HCLK_DIV1;
	me->iclk.APB2CLKDivider = RCC_HCLK_DIV1;

	// #4 Set flash latency
	me->flashLatency = FLASH_LATENCY_0;

	// #5 Set expected clocks
	me->systemCoreClock_Ctor = 2097152UL;
	me->sysclk_Ctor = 2097152UL;
	me->hclk_Ctor = 2097152UL;
	me->pclk1Apb1_Ctor = 2097152UL;
	me->pclk2Apb2_Ctor = 2097152UL;
	me->pclk1Apb1Tim_Ctor = 2097152UL;
	me->pclk2Apb2Tim_Ctor = 2097152UL;
}
static void AlxClk_Ctor_McuStm32L0_Sysclk_32MHz_Pclk1Apb1_32MHz_Pclk2Apb2_32MHz_Hsi_16MHz(AlxClk* me)
{
	// #1 Set voltage regulator scale
	me->pwrRegVoltageScale = PWR_REGULATOR_VOLTAGE_SCALE1;
	me->isPwrRegOverDrive = false;

	// #2 Set oscillators
	me->iosc.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	me->iosc.HSEState = RCC_HSE_OFF;
	me->iosc.LSEState = RCC_LSE_OFF;
	me->iosc.HSIState = RCC_HSI_ON;
	me->iosc.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	me->iosc.LSIState = RCC_LSI_OFF;
	me->iosc.HSI48State = RCC_HSI48_OFF;
	me->iosc.MSIState = RCC_MSI_ON;
	me->iosc.MSICalibrationValue = RCC_MSICALIBRATION_DEFAULT;
	me->iosc.MSIClockRange = RCC_MSIRANGE_5;
	me->iosc.PLL.PLLState = RCC_PLL_ON;
	me->iosc.PLL.PLLSource = RCC_PLLSOURCE_HSI;
	me->iosc.PLL.PLLMUL = RCC_PLL_MUL4;
	me->iosc.PLL.PLLDIV = RCC_PLL_DIV2;

	// #3 Set clocks
	me->iclk.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	me->iclk.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	me->iclk.AHBCLKDivider = RCC_SYSCLK_DIV1;
	me->iclk.APB1CLKDivider = RCC_HCLK_DIV1;
	me->iclk.APB2CLKDivider = RCC_HCLK_DIV1;

	// #4 Set flash latency
	me->flashLatency = FLASH_LATENCY_1;

	// #5 Set expected clocks
	me->systemCoreClock_Ctor = 32000000UL;
	me->sysclk_Ctor = 32000000UL;
	me->hclk_Ctor = 32000000UL;
	me->pclk1Apb1_Ctor = 32000000UL;
	me->pclk2Apb2_Ctor = 32000000UL;
	me->pclk1Apb1Tim_Ctor = 32000000UL;
	me->pclk2Apb2Tim_Ctor = 32000000UL;
}
#endif
#if defined(ALX_STM32L4)
static void AlxClk_Ctor_McuStm32L4_Sysclk_4MHz_Pclk1Apb1_4MHz_Pclk2Apb2_4MHz_Msi_4MHz_Default(AlxClk* me)
{
	// #1 Set voltage regulator scale
	me->pwrRegVoltageScale = PWR_REGULATOR_VOLTAGE_SCALE1;
	me->isPwrRegOverDrive = false;

	// #2 Set oscillators
	me->iosc.OscillatorType = RCC_OSCILLATORTYPE_MSI;
	me->iosc.HSEState = RCC_HSE_OFF;
	me->iosc.LSEState = RCC_LSE_OFF;
	me->iosc.HSIState = RCC_HSI_OFF;
	me->iosc.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	me->iosc.LSIState = RCC_LSI_OFF;
	me->iosc.HSI48State = RCC_HSI48_OFF;
	me->iosc.MSIState = RCC_MSI_ON;
	me->iosc.MSICalibrationValue = RCC_MSICALIBRATION_DEFAULT;
	me->iosc.MSIClockRange = RCC_MSIRANGE_6;
	me->iosc.PLL.PLLState = RCC_PLL_OFF;
	me->iosc.PLL.PLLSource = 0;
	me->iosc.PLL.PLLM = 1;
	me->iosc.PLL.PLLN = 8;
	me->iosc.PLL.PLLP = 7;
	me->iosc.PLL.PLLQ = 2;
	me->iosc.PLL.PLLR = 2;

	// #3 Set clocks
	me->iclk.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	me->iclk.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;
	me->iclk.AHBCLKDivider = RCC_SYSCLK_DIV1;
	me->iclk.APB1CLKDivider = RCC_HCLK_DIV1;
	me->iclk.APB2CLKDivider = RCC_HCLK_DIV1;

	// #4 Set flash latency
	me->flashLatency = FLASH_LATENCY_0;

	// #5 Set expected clocks
	me->systemCoreClock_Ctor = 4000000UL;
	me->sysclk_Ctor = 4000000UL;
	me->hclk_Ctor = 4000000UL;
	me->pclk1Apb1_Ctor = 4000000UL;
	me->pclk2Apb2_Ctor = 4000000UL;
	me->pclk1Apb1Tim_Ctor = 4000000UL;
	me->pclk2Apb2Tim_Ctor = 4000000UL;
	me->mainPllInputClk_Ctor = 0;
}
static void AlxClk_Ctor_McuStm32L4_Sysclk_16MHz_Pclk1Apb1_16MHz_Pclk2Apb2_16MHz_Hsi_16MHz(AlxClk* me)
{
	// #1 Set voltage regulator scale
	me->pwrRegVoltageScale = PWR_REGULATOR_VOLTAGE_SCALE1;
	me->isPwrRegOverDrive = false;

	// #2 Set oscillators
	me->iosc.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	me->iosc.HSEState = RCC_HSE_OFF;
	me->iosc.LSEState = RCC_LSE_OFF;
	me->iosc.HSIState = RCC_HSI_ON;
	me->iosc.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	me->iosc.LSIState = RCC_LSI_OFF;
	me->iosc.HSI48State = RCC_HSI48_OFF;
	me->iosc.MSIState = RCC_MSI_OFF;
	me->iosc.MSICalibrationValue = RCC_MSICALIBRATION_DEFAULT;
	me->iosc.MSIClockRange = RCC_MSIRANGE_5;
	me->iosc.PLL.PLLState = RCC_PLL_OFF;
	me->iosc.PLL.PLLSource = 0;
	me->iosc.PLL.PLLM = 0;
	me->iosc.PLL.PLLN = 0;
	me->iosc.PLL.PLLP = 0;
	me->iosc.PLL.PLLQ = 0;
	me->iosc.PLL.PLLR = 0;

	// #3 Set clocks
	me->iclk.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	me->iclk.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
	me->iclk.AHBCLKDivider = RCC_SYSCLK_DIV1;
	me->iclk.APB1CLKDivider = RCC_HCLK_DIV1;
	me->iclk.APB2CLKDivider = RCC_HCLK_DIV1;

	// #4 Set flash latency
	me->flashLatency = FLASH_LATENCY_0;

	// #5 Set expected clocks
	me->systemCoreClock_Ctor = 16000000UL;
	me->sysclk_Ctor = 16000000UL;
	me->hclk_Ctor = 16000000UL;
	me->pclk1Apb1_Ctor = 16000000UL;
	me->pclk2Apb2_Ctor = 16000000UL;
	me->pclk1Apb1Tim_Ctor = 16000000UL;
	me->pclk2Apb2Tim_Ctor = 16000000UL;
	me->mainPllInputClk_Ctor = 0;
}
#endif


//******************************************************************************
// Callback Functions
//******************************************************************************
void HAL_RCC_CSSCallback()
{
	HAL_NVIC_SystemReset();
}


#endif	// #if defined(ALX_C_LIB) && (defined(ALX_STM32F1) || defined(ALX_STM32F4) || defined(ALX_STM32G4) || defined(ALX_STM32L0) || defined(ALX_STM32L4))
