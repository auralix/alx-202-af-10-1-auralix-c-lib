/**
  ******************************************************************************
  * @file alxDac_McuStm32.h
  * @brief Auralix C Library - ALX DAC Module
  * @version $LastChangedRevision: 5216 $
  * @date $LastChangedDate: 2021-05-24 19:38:05 +0200 (Mon, 24 May 2021) $
  ******************************************************************************
  */

#ifndef ALX_DAC_MCU_STM32_H
#define ALX_DAC_MCU_STM32_H

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


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_STM32F4) || defined(ALX_STM32G4) || defined(ALX_STM32L0)


//******************************************************************************
// Types
//******************************************************************************
typedef struct
{
	// Objects - External
	AlxIoPin** ioPinArr;

	// Parameters
	Alx_Ch* chArr;
	float* setVoltageDefaultArr_V;
	uint8_t numOfCh;
	uint32_t resolution;
	bool isVrefInt_V;
	float vrefExt_V;

	// Variables
	DAC_HandleTypeDef hdac;
	DAC_ChannelConfTypeDef chdac[ALX_DAC_BUFF_LEN];
	Alx_Ch ch[ALX_DAC_BUFF_LEN];
	float setVoltageDefault_V[ALX_DAC_BUFF_LEN];

	// Info
	bool isInit;
	bool wasCtorCalled;
} AlxDac_Mcu;


//******************************************************************************
// Constructor
//******************************************************************************
void AlxDacMcu_Ctor
(
	AlxDac_Mcu* me,
	DAC_TypeDef* dac,
	AlxIoPin** ioPinArr,
	Alx_Ch* chArr,
	float* setVoltageDefaultArr_V,
	uint8_t numOfCh,
	bool isVrefInt_V,
	float vrefExt_V
);


#endif

#ifdef __cplusplus
}
#endif

#endif // ALX_DAC_MCU_STM32_H
