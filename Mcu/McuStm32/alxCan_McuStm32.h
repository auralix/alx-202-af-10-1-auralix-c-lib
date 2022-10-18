/**
  ******************************************************************************
  * @file		alxCan_McuStm32.h
  * @brief		Auralix C Library - ALX CAN MCU STM32 Module
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
// Include Guard
//******************************************************************************
#ifndef ALX_CAN_MCU_STM32_H
#define ALX_CAN_MCU_STM32_H

#ifdef __cplusplus
extern "C" {
#endif


//******************************************************************************
// Includes
//******************************************************************************
#include "alxGlobal.h"
#include "alxTrace.h"
#include "alxAssert.h"
#include "alxIoPin.h"
#include "alxClk.h"
#include "alxFifo.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB) && ((defined(ALX_STM32F4) && defined(HAL_CAN_MODULE_ENABLED)) || (defined(ALX_STM32G4) && defined(HAL_FDCAN_MODULE_ENABLED)))


//******************************************************************************
// Types
//******************************************************************************
typedef enum
{
	#if defined(ALX_STM32F4)
	AlxCan_Clk_McuStm32F4_CanClk_250kbps_Pclk1Apb1_45MHz = 0,
	#endif
	#if defined(ALX_STM32G4)
	AlxCan_Clk_McuStm32G4_CanClk_250kbps_Pclk1Apb1_170MHz = 0,
	#endif
} AlxCan_Clk;

typedef struct
{
	// Objects - Internal
	AlxFifo txFifo;
	AlxFifo rxFifo;

	// Objects - External
	AlxIoPin* do_CAN_TX;
	AlxIoPin* di_CAN_RX;
	AlxClk* clk;

	// Parameters
	AlxCan_Clk canClk;
	uint32_t canRxFifo;
	uint8_t* txFifoBuff;
	uint32_t txFifoBuffLen;
	uint8_t* rxFifoBuff;
	uint32_t rxFifoBuffLen;
	Alx_IrqPriority txIrqPriority;
	Alx_IrqPriority rxIrqPriority;
	#if defined(ALX_STM32G4)
	#endif

	// Variables
	#if defined(ALX_STM32F4)
	CAN_HandleTypeDef hcan;
	CAN_FilterTypeDef fcan;
	#endif
	#if defined(ALX_STM32G4)
	RCC_PeriphCLKInitTypeDef periphClkInit;
	FDCAN_HandleTypeDef hcan;
	FDCAN_FilterTypeDef fcan;
	#endif

	// Info
	bool isInit;
	bool wasCtorCalled;
	bool isErr;
} AlxCan;


//******************************************************************************
// Constructor
//******************************************************************************

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		can
  * @param[in]		do_CAN_TX
  * @param[in]		di_CAN_RX
  * @param[in]		clk
  * @param[in]		canClk
  * @param[in]		txFifoBuff
  * @param[in]		txFifoBuffLen
  * @param[in]		rxFifoBuff
  * @param[in]		rxFifoBuffLen
  * @param[in]		txIrqPriority
  * @param[in]		rxIrqPriority
  */
void AlxCan_Ctor
(
	AlxCan* me,
	#if defined(ALX_STM32F4)
	CAN_TypeDef* can,
	#endif
	#if defined(ALX_STM32G4)
	FDCAN_GlobalTypeDef* can,
	#endif
	AlxIoPin* do_CAN_TX,
	AlxIoPin* di_CAN_RX,
	AlxClk* clk,
	AlxCan_Clk canClk,
	uint8_t* txFifoBuff,
	uint32_t txFifoBuffLen,
	uint8_t* rxFifoBuff,
	uint32_t rxFifoBuffLen,
	Alx_IrqPriority txIrqPriority,
	Alx_IrqPriority rxIrqPriority
);


#endif	// #if defined(ALX_C_LIB) && ((defined(ALX_STM32F4) && defined(HAL_CAN_MODULE_ENABLED)) || (defined(ALX_STM32G4) && defined(HAL_FDCAN_MODULE_ENABLED)))

#ifdef __cplusplus
}
#endif

#endif	// #ifndef ALX_CAN_MCU_STM32_H
