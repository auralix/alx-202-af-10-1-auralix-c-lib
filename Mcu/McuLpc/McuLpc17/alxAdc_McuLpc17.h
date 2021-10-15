/**
  ******************************************************************************
  * @file alxAdc_McuLpc17.h
  * @brief Auralix C Library - ALX ADC Module
  * @version $LastChangedRevision: 5455 $
  * @date $LastChangedDate: 2021-06-21 00:16:06 +0200 (Mon, 21 Jun 2021) $
  ******************************************************************************
  */

#ifndef ALX_ADC_MCU_LPC17_H
#define ALX_ADC_MCU_LPC17_H

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
#if defined(ALX_LPC17)


//******************************************************************************
// Types
//******************************************************************************
typedef struct
{
	// Obejcts - External
	AlxIoPin** channels;

	// Parameters
	uint8_t numChannels;
	uint16_t voltageRefP_mV;

	// Variables
	ADC_CLOCK_SETUP_T adcClkSetup;
	uint16_t adcResult;

	// Info
	bool isInit;
	bool wasCtorCalled;
} AlxAdc_Mcu;


//******************************************************************************
// Constructor
//******************************************************************************
void AlxAdcMcu_Ctor
(
	AlxAdc_Mcu* me,
	AlxIoPin** channels,
	uint8_t numChannels,
	uint16_t voltageRefP_mV
);


#endif

#ifdef __cplusplus
}
#endif

#endif // ALX_ADC_MCU_LPC17_H
