/**
  ******************************************************************************
  * @file alxAdc_McuLpc84.h
  * @brief Auralix C Library - ALX ADC Module
  * @version $LastChangedRevision: 5455 $
  * @date $LastChangedDate: 2021-06-21 00:16:06 +0200 (Mon, 21 Jun 2021) $
  ******************************************************************************
  */

#ifndef ALX_ADC_MCU_LPC84_H
#define ALX_ADC_MCU_LPC84_H

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
#if defined(ALX_LPC84)


//******************************************************************************
// Types
//******************************************************************************
typedef struct
{
	// Obejcts - External
	AlxIoPin** channels;

	// Parameters
	uint8_t numChannels;
	uint8_t adcClkDiv;
	uint16_t voltageRefP_mV;

	// Variables
	adc_config_t adcConfig;
	adc_conv_seq_config_t adcConvSeqConfig;
	adc_result_info_t adcResult;

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
	uint8_t adcClkDiv,
	uint16_t voltageRefP_mV
);


#endif

#ifdef __cplusplus
}
#endif

#endif // ALX_ADC_MCU_LPC84_H
