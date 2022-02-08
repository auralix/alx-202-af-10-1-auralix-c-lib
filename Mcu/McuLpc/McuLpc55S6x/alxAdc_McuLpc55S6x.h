/**
  ******************************************************************************
  * @file alxAdc_McuLpc55S6x.h
  * @brief Auralix C Library - ALX ADC Module
  * @version $LastChangedRevision: 4270 $
  * @date $LastChangedDate: 2021-03-05 19:02:52 +0100 (Fri, 05 Mar 2021) $
  ******************************************************************************
  */

#ifndef ALX_ADC_MCU_LPC55S6X_H
#define ALX_ADC_MCU_LPC55S6X_H

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
#if defined(ALX_LPC55S6X)


//******************************************************************************
// Types
//******************************************************************************
typedef enum
{
	AlxAdc_Clk_McuLpc55S6x_AdcClk_12MHz_MainClk_12MHz = 1U,		// MF: ClkDiv = 1
	AlxAdc_Clk_McuLpc55S6x_AdcClk_24MHz_MainClk_96MHz = 4U,		// MF: ClkDiv = 4
	AlxAdc_Clk_McuLpc55S6x_AdcClk_18MHz75_MainClk_150MHz = 8U	// MF: ClkDiv = 8
} AlxAdc_Clk;

typedef struct
{
	// Objects - External
	AlxIoPin** ioPinArr;
	AlxClk* clk;

	// Parameters
	Alx_Ch* chArr;
	uint8_t numOfIoPinsAndCh;
	AlxAdc_Clk adcClk;
	#if defined(ALX_ADC_OPTIMIZE_SIZE) || defined(ALX_OPTIMIZE_SIZE_ALL)
	uint32_t vRef_mV;
	#else
	float vRef_V;
	#endif

	// Variables
	lpadc_config_t adcConfig;
	lpadc_conv_command_config_t adcConvCommConfig;
	lpadc_conv_trigger_config_t adcConvTrigConfig;
	lpadc_conv_result_t adcConvResult;

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
	AlxAdc_Clk adcClk,
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

#endif // ALX_ADC_MCU_LPC55S6X_H
