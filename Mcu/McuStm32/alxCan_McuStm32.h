/**
  ******************************************************************************
  * @file alxCan_McuStm32.h
  * @brief Auralix C Library - ALX CAN Module
  * @version $LastChangedRevision: 5930 $
  * @date $LastChangedDate: 2021-09-03 15:45:15 +0200 (Fri, 03 Sep 2021) $
  ******************************************************************************
  */

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
#if (defined(ALX_STM32F4) && defined(HAL_CAN_MODULE_ENABLED)) || (defined(ALX_STM32G4) && defined(HAL_FDCAN_MODULE_ENABLED))


//******************************************************************************
// Types
//******************************************************************************
typedef enum
{
	#if defined(STM32F4)
	AlxCan_Clk_McuStm32F4_CanClk_250kbps_Pclk1Apb1_45MHz = 0,
	#endif
	#if defined(STM32G4)
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
	#if defined(STM32G4)
	#endif

	// Variables
	#if defined(STM32F4)
	CAN_HandleTypeDef hcan;
	CAN_FilterTypeDef fcan;
	#endif
	#if defined(STM32G4)
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
void AlxCan_Ctor
(
	AlxCan* me,
	#if defined(STM32F4)
	CAN_TypeDef* can,
	#endif
	#if defined(STM32G4)
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

#endif // (defined(ALX_STM32F4) || defined(ALX_STM32G4)) && defined(HAL_CAN_MODULE_ENABLED)

#ifdef __cplusplus
}
#endif

#endif // ALX_CAN_MCU_STM32_H
