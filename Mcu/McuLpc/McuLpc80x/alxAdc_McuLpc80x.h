/**
  ******************************************************************************
  * @file alxAdc_McuLpc80x.h
  * @brief Auralix C Library - ALX ADC Module
  * @version $LastChangedRevision: 4270 $
  * @date $LastChangedDate: 2021-03-05 19:02:52 +0100 (Fri, 05 Mar 2021) $
  ******************************************************************************
  */

#ifndef ALX_ADC_MCU_LPC80X_H
#define ALX_ADC_MCU_LPC80X_H

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


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_LPC80X)


//******************************************************************************
// Types
//******************************************************************************
typedef struct
{
	// Obejcts - External
	AlxIoPin** ioPinArr;
	AlxClk* clk;

	// Parameters
	Alx_Ch* chArr;
	uint8_t numOfIoPinsAndCh;
	#if defined(ALX_ADC_OPTIMIZE_SIZE) || defined(ALX_OPTIMIZE_SIZE_ALL)
	uint32_t vRef_mV;
	#else
	float vRef_V;
	#endif

	// Variables
	adc_config_t adcConfig;
	adc_conv_seq_config_t adcConvSeqConfig;
	adc_result_info_t adcResult;

	// Info
	bool isInit;
	bool wasCtorCalled;
} AlxAdc;


//******************************************************************************
// Constructor
//******************************************************************************
void AlxAdc_Ctor
(
	AlxAdc* me,
	AlxIoPin** ioPinArr,
	Alx_Ch* chArr,
	uint8_t numOfIoPinsAndCh,
	AlxClk* clk,
	#if defined(ALX_ADC_OPTIMIZE_SIZE) || defined(ALX_OPTIMIZE_SIZE_ALL)
	uint32_t vRef_mV
	#else
	float vRef_V
	#endif
);


#endif // Module Guard

#ifdef __cplusplus
}
#endif

#endif // ALX_ADC_MCU_LPC80X_H
