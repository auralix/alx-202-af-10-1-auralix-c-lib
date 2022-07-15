/**
  ******************************************************************************
  * @file		AlxAdcStm32F4.c
  * @brief		ALX ADC Module
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
#include "alxAdc_McuStm32.h"
#include "alxAdc.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_STM32F1) || defined(ALX_STM32F4) || defined(ALX_STM32G4) || defined(ALX_STM32L0)


//******************************************************************************
// Private Functions
//******************************************************************************
static void AlxAdc_PeriphAdc_EnableClk(AlxAdc* me);
static void AlxAdc_PeriphAdc_DisableClk(AlxAdc* me);
static void AlxAdc_PeriphAdc_ForceReset(AlxAdc* me);
static void AlxAdc_PeriphAdc_ReleaseReset(AlxAdc* me);
static void AlxAdc_PeriphDma_EnableClk(AlxAdc* me);
static void AlxAdc_PeriphDma_DisableClk(AlxAdc* me);
static void AlxAdc_PeriphDma_ForceReset(AlxAdc* me);
static void AlxAdc_PeriphDma_ReleaseReset(AlxAdc* me);
static uint32_t AlxAdc_GetVref_mV(AlxAdc* me);
static uint32_t AlxAdc_GetCh(AlxAdc* me, Alx_Ch ch);
static uint32_t AlxAdc_GetRank(uint8_t buffPos);
static bool AlxAdc_Ctor_IsPclk2Apb2Ok(AlxAdc* me);


//******************************************************************************
// Constructor
//******************************************************************************
void AlxAdc_Ctor
(
	AlxAdc* me,
	ADC_TypeDef* adc,
	AlxIoPin** ioPinArr,
	uint8_t numOfIoPins,
	Alx_Ch* chArr,
	uint8_t numOfCh,
	AlxClk* clk,
	AlxAdc_Clk adcClk,
	uint32_t samplingTime,
	bool isVrefInt_V,
	float vrefExt_V
)
{
	// Assert
	(void)me;
	ALX_ADC_ASSERT(adc == ADC1); // Currently only ADC1 is supported
	(void)ioPinArr;
	ALX_ADC_ASSERT(numOfIoPins <= ALX_ADC_BUFF_LEN - 1); // We have only 16 ranks, one is used for VrefInt
	(void)chArr;
	ALX_ADC_ASSERT(numOfCh <= ALX_ADC_BUFF_LEN); // We have only 16 ranks
	(void)clk;
	(void)adcClk;
	(void)samplingTime;
	(void)isVrefInt_V;
	(void)vrefExt_V;

	// Objects - External
	me->ioPinArr = ioPinArr;
	me->clk = clk;

	// Parameters
	me->hadc.Instance = adc;
	me->numOfIoPins = numOfIoPins;
	me->chArr = chArr;
	me->numOfCh = numOfCh;
	me->adcClk = adcClk;
	me->isVrefInt_V = isVrefInt_V;
	me->vrefExt_V = vrefExt_V;

	// Check channel sequence
	for (uint8_t i = 0; i < numOfCh - 1; i++) ALX_ADC_ASSERT(AlxAdc_GetCh(me, chArr[i]) < AlxAdc_GetCh(me, chArr[i + 1])); // Channel sequence must be from low to high number

	// Check clock
	ALX_ADC_ASSERT(AlxAdc_Ctor_IsPclk2Apb2Ok(me));

	// Variables

	//******************************************************************************
	// STM32F1
	//******************************************************************************
	#if defined(STM32F1)
	// ADC Clock
	me->iclk.PeriphClockSelection = RCC_PERIPHCLK_ADC;
	me->iclk.RTCClockSelection = ALX_NULL;	// Not used
	me->iclk.AdcClockSelection = (uint32_t)me->adcClk;
	me->iclk.UsbClockSelection = ALX_NULL;	// Not used

	// ADC Common
	me->hadc.Init.DataAlign = ADC_DATAALIGN_RIGHT;
	me->hadc.Init.ScanConvMode = ENABLE;
	me->hadc.Init.ContinuousConvMode = ENABLE;
	me->hadc.Init.NbrOfConversion = me->numOfCh;
	me->hadc.Init.DiscontinuousConvMode = DISABLE;
	me->hadc.Init.NbrOfDiscConversion = 0;
	me->hadc.Init.ExternalTrigConv = ADC_SOFTWARE_START;

	// ADC Channel
	for (uint32_t buffPos = 0; buffPos < me->numOfCh; buffPos++)
	{
		me->chadc[buffPos].Channel = AlxAdc_GetCh(me, *(me->chArr + buffPos));
		me->chadc[buffPos].Rank = AlxAdc_GetRank(buffPos);;
		me->chadc[buffPos].SamplingTime = samplingTime;
		me->ch[buffPos] = *(me->chArr + buffPos);
	}

	// DMA
	if (me->hadc.Instance == ADC1)
	{
		me->dma = DMA1;
		me->hdma.Instance = DMA1_Channel1;;
	}
	me->hdma.Init.Direction = DMA_PERIPH_TO_MEMORY;
	me->hdma.Init.PeriphInc = DMA_PINC_DISABLE;
	me->hdma.Init.MemInc = DMA_MINC_ENABLE;
	me->hdma.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
	me->hdma.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
	me->hdma.Init.Mode = DMA_CIRCULAR;
	me->hdma.Init.Priority = DMA_PRIORITY_LOW;
	#endif


	//******************************************************************************
	// STM32F4
	//******************************************************************************
	#if defined(STM32F4)
	// Parameters
	me->resolution = ADC_RESOLUTION_12B;

	// ADC Common
	me->hadc.Init.ClockPrescaler = (uint32_t)me->adcClk;
	me->hadc.Init.Resolution = me->resolution;
	me->hadc.Init.DataAlign = ADC_DATAALIGN_RIGHT;
	me->hadc.Init.ScanConvMode = ENABLE;
	me->hadc.Init.EOCSelection = ADC_EOC_SEQ_CONV;
	me->hadc.Init.ContinuousConvMode = ENABLE;
	me->hadc.Init.NbrOfConversion = me->numOfCh;
	me->hadc.Init.DiscontinuousConvMode = DISABLE;
	me->hadc.Init.NbrOfDiscConversion = 0;
	me->hadc.Init.ExternalTrigConv = ADC_SOFTWARE_START;
	me->hadc.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
	me->hadc.Init.DMAContinuousRequests = ENABLE;

	// ADC Channel
	for (uint32_t buffPos = 0; buffPos < me->numOfCh; buffPos++)
	{
		me->chadc[buffPos].Channel = AlxAdc_GetCh(me, *(me->chArr + buffPos));
		me->chadc[buffPos].Rank = AlxAdc_GetRank(buffPos);;
		me->chadc[buffPos].SamplingTime = samplingTime;
		me->chadc[buffPos].Offset = 0;
		me->ch[buffPos] = *(me->chArr + buffPos);
	}

	// DMA
	if (me->hadc.Instance == ADC1)
	{
		me->dma = DMA2;
		me->hdma.Instance = DMA2_Stream0;
		me->hdma.Init.Channel = DMA_CHANNEL_0;
	}
	me->hdma.Init.Direction = DMA_PERIPH_TO_MEMORY;
	me->hdma.Init.PeriphInc = DMA_PINC_DISABLE;
	me->hdma.Init.MemInc = DMA_MINC_ENABLE;
	me->hdma.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
	me->hdma.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
	me->hdma.Init.Mode = DMA_CIRCULAR;
	me->hdma.Init.Priority = DMA_PRIORITY_LOW;
	me->hdma.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
	me->hdma.Init.FIFOThreshold = 0;
	me->hdma.Init.MemBurst = DMA_MBURST_SINGLE;
	me->hdma.Init.PeriphBurst = DMA_PBURST_SINGLE;
	#endif


	//******************************************************************************
	// STM32G4
	//******************************************************************************
	#if defined(STM32G4)
	// Parameters
	me->resolution = ADC_RESOLUTION_12B;
	
	// ADC Common
	me->hadc.Init.ClockPrescaler = (uint32_t)me->adcClk;
	me->hadc.Init.Resolution = me->resolution;
	me->hadc.Init.DataAlign = ADC_DATAALIGN_RIGHT;
	me->hadc.Init.GainCompensation = 0;
	me->hadc.Init.ScanConvMode = ADC_SCAN_ENABLE;
	me->hadc.Init.EOCSelection = ADC_EOC_SEQ_CONV;
	me->hadc.Init.LowPowerAutoWait = DISABLE;
	me->hadc.Init.ContinuousConvMode = ENABLE;
	me->hadc.Init.NbrOfConversion = me->numOfCh;
	me->hadc.Init.DiscontinuousConvMode = DISABLE;
	me->hadc.Init.NbrOfDiscConversion = 0;
	me->hadc.Init.ExternalTrigConv = ADC_SOFTWARE_START;
	me->hadc.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
	me->hadc.Init.SamplingMode = ADC_SAMPLING_MODE_NORMAL;
	me->hadc.Init.DMAContinuousRequests = ENABLE;
	me->hadc.Init.Overrun = ADC_OVR_DATA_PRESERVED;
	me->hadc.Init.OversamplingMode = DISABLE;
	me->hadc.Init.Oversampling.Ratio = 0;
	me->hadc.Init.Oversampling.RightBitShift = 0;
	me->hadc.Init.Oversampling.TriggeredMode = 0;
	me->hadc.Init.Oversampling.OversamplingStopReset = 0;

	// ADC Channel
	for (uint32_t buffPos = 0; buffPos < me->numOfCh; buffPos++)
	{
		me->chadc[buffPos].Channel = AlxAdc_GetCh(me, *(me->chArr + buffPos));
		me->chadc[buffPos].Rank = AlxAdc_GetRank(buffPos);
		me->chadc[buffPos].SamplingTime = samplingTime;
		me->chadc[buffPos].SingleDiff = ADC_SINGLE_ENDED;
		me->chadc[buffPos].OffsetNumber = ADC_OFFSET_NONE;
		me->chadc[buffPos].Offset = 0;
		me->chadc[buffPos].OffsetSign = 0;
		me->chadc[buffPos].OffsetSaturation = DISABLE;
		me->ch[buffPos] = *(me->chArr + buffPos);
	}

	// DMA
	if (me->hadc.Instance == ADC1)
	{
		me->dma = DMA1;
		me->hdma.Instance = DMA1_Channel5;
		me->hdma.Init.Request = DMA_REQUEST_ADC1;
	}
	me->hdma.Init.Direction = DMA_PERIPH_TO_MEMORY;
	me->hdma.Init.PeriphInc = DMA_PINC_DISABLE;
	me->hdma.Init.MemInc = DMA_MINC_ENABLE;
	me->hdma.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
	me->hdma.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
	me->hdma.Init.Mode = DMA_CIRCULAR;
	me->hdma.Init.Priority = DMA_PRIORITY_LOW;
	#endif


	//******************************************************************************
	// STM32L0
	//******************************************************************************
	#if defined(STM32L0)
	// Parameters
	me->resolution = ADC_RESOLUTION_12B;
	
	// ADC Common
	me->hadc.Init.ClockPrescaler = (uint32_t)me->adcClk;
	me->hadc.Init.Resolution = me->resolution;
	me->hadc.Init.DataAlign = ADC_DATAALIGN_RIGHT;
	me->hadc.Init.ScanConvMode = ADC_SCAN_ENABLE;
	me->hadc.Init.EOCSelection = ADC_EOC_SEQ_CONV;
	me->hadc.Init.LowPowerAutoWait = DISABLE;
	me->hadc.Init.LowPowerAutoPowerOff = DISABLE;
	me->hadc.Init.ContinuousConvMode = ENABLE;
	me->hadc.Init.DiscontinuousConvMode = DISABLE;
	me->hadc.Init.ExternalTrigConv = ADC_SOFTWARE_START;
	me->hadc.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
	me->hadc.Init.DMAContinuousRequests = ENABLE;
	me->hadc.Init.Overrun = ADC_OVR_DATA_PRESERVED;
	me->hadc.Init.LowPowerFrequencyMode = DISABLE;
	me->hadc.Init.SamplingTime = samplingTime;
	me->hadc.Init.OversamplingMode = DISABLE;
	me->hadc.Init.Oversample.Ratio = 0;
	me->hadc.Init.Oversample.RightBitShift = 0;
	me->hadc.Init.Oversample.TriggeredMode = 0;

	// ADC Channel
	for (uint32_t buffPos = 0; buffPos < me->numOfCh; buffPos++)
	{
		me->chadc[buffPos].Channel = AlxAdc_GetCh(me, *(me->chArr + buffPos));
		me->chadc[buffPos].Rank = ADC_RANK_CHANNEL_NUMBER;
		me->ch[buffPos] = *(me->chArr + buffPos);
	}

	// DMA
	if (me->hadc.Instance == ADC1)
	{
		me->dma = DMA1;
		me->hdma.Instance = DMA1_Channel1;
		me->hdma.Init.Request = DMA_REQUEST_0;
	}
	me->hdma.Init.Direction = DMA_PERIPH_TO_MEMORY;
	me->hdma.Init.PeriphInc = DMA_PINC_DISABLE;
	me->hdma.Init.MemInc = DMA_MINC_ENABLE;
	me->hdma.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
	me->hdma.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
	me->hdma.Init.Mode = DMA_CIRCULAR;
	me->hdma.Init.Priority = DMA_PRIORITY_LOW;
	#endif

	// Info
	me->isInit = false;
	me->wasCtorCalled = true;
}


//******************************************************************************
// Functions
//******************************************************************************
Alx_Status AlxAdc_Init(AlxAdc* me)
{
	ALX_ADC_ASSERT(me->isInit == false);
	ALX_ADC_ASSERT(me->wasCtorCalled == true);

	// #1 Init GPIO
	for (uint32_t i = 0; i < me->numOfIoPins; i++)
		AlxIoPin_Init((*(me->ioPinArr + i)));

	// #2 Release ADC & DMA Periphery Reset
	AlxAdc_PeriphAdc_ReleaseReset(me);
	AlxAdc_PeriphDma_ReleaseReset(me);

	// #3 Enable ADC & DMA Periphery Clock
	AlxAdc_PeriphAdc_EnableClk(me);
	AlxAdc_PeriphDma_EnableClk(me);

	#if defined(STM32F1)
	// #4 Init ADC Clock
	if(HAL_RCCEx_PeriphCLKConfig(&me->iclk) != HAL_OK) { ALX_ADC_TRACE("ErrClk"); return Alx_Err; };
	#endif

	// #5 Init ADC
	if(HAL_ADC_Init(&me->hadc) != HAL_OK) { ALX_ADC_TRACE("ErrInitAdc"); return Alx_Err; }

	// #6 Init ADC Channels
	for(uint32_t i = 0; i < me->numOfCh; i++)
		if(HAL_ADC_ConfigChannel(&me->hadc, &me->chadc[i]) != HAL_OK) { ALX_ADC_TRACE("ErrInitAdcCh"); return Alx_Err; }

	#if defined(STM32F1)
	// #7 Calibrate ADC
	if(HAL_ADCEx_Calibration_Start(&me->hadc) != HAL_OK) { ALX_ADC_TRACE("AdcCalibErr"); return Alx_Err; }
	#endif
	#if defined(STM32G4) || defined(STM32L0)
	// #7 Calibrate ADC
	if(HAL_ADCEx_Calibration_Start(&me->hadc, ADC_SINGLE_ENDED) != HAL_OK) { ALX_ADC_TRACE("AdcCalibErr"); return Alx_Err; }
	#endif

	// #8 Init DMA
	if(HAL_DMA_Init(&me->hdma) != HAL_OK) { ALX_ADC_TRACE("ErrInitDma"); return Alx_Err; }

	// #9 Link ADC & DMA
	__HAL_LINKDMA(&me->hadc, DMA_Handle, me->hdma);

	// #10 Start ADC with DMA
	if(HAL_ADC_Start_DMA(&me->hadc, me->buff, me->numOfCh) != HAL_OK) { ALX_ADC_TRACE("ErrStart"); return Alx_Err; }

	// #11 Set isInit
	me->isInit = true;

	// #12 Return OK
	return Alx_Ok;
}
Alx_Status AlxAdc_DeInit(AlxAdc* me)
{
	ALX_ADC_ASSERT(me->isInit == true);
	ALX_ADC_ASSERT(me->wasCtorCalled == true);

	// #1 DeInit DMA
	if(HAL_DMA_DeInit(&me->hdma) != HAL_OK) { ALX_ADC_TRACE("ErrDeInitDma"); return Alx_Err; }

	// #2 DeInit ADC
	if(HAL_ADC_DeInit(&me->hadc) != HAL_OK) { ALX_ADC_TRACE("ErrDeInitAdc"); return Alx_Err; }

	// #3 Force ADC & DMA Periphery Reset
	AlxAdc_PeriphAdc_ForceReset(me);
	AlxAdc_PeriphDma_ForceReset(me);

	// #4 Disable ADC & DMA Periphery Clock
	AlxAdc_PeriphAdc_DisableClk(me);
	AlxAdc_PeriphDma_DisableClk(me);

	// #5 DeInit GPIO
	for (uint32_t i = 0; i < me->numOfIoPins; i++)
		AlxIoPin_DeInit((*(me->ioPinArr + i)));

	// #6 Reset isInit
	me->isInit = false;

	// #7 Return OK
	return Alx_Ok;
}
float AlxAdc_GetVoltage_V(AlxAdc* me, Alx_Ch ch)
{
	ALX_ADC_ASSERT(me->isInit == true);
	ALX_ADC_ASSERT(me->wasCtorCalled == true);

	// #1 Get Reference Voltage
	uint32_t vref_mV = AlxAdc_GetVref_mV(me);

	// #2 Return Channel
	for (uint32_t i = 0; i < me->numOfCh; i++)
	{
		if (ch == Alx_Ch_McuStm32_Vref)
		{
			float vref_V = (float)vref_mV / 1000.f;
			return vref_V;
		}
		else if (me->ch[i] == ch)
		{
			uint32_t voltage_mV = __LL_ADC_CALC_DATA_TO_VOLTAGE(vref_mV, me->buff[i], me->resolution);
			float voltage_V = (float)voltage_mV / 1000.f;
			return voltage_V;
		}
	}

	ALX_ADC_ASSERT(false); // We shouldn't get here
	return 0;
}
uint32_t AlxAdc_GetVoltage_mV(AlxAdc* me, Alx_Ch ch)
{
	ALX_ADC_ASSERT(me->isInit == true);
	ALX_ADC_ASSERT(me->wasCtorCalled == true);

	// #1 Get Reference Voltage
	uint32_t vref_mV = AlxAdc_GetVref_mV(me);

	// #2 Return Channel
	for (uint32_t i = 0; i < me->numOfCh; i++)
	{
		if (ch == Alx_Ch_McuStm32_Vref)
		{
			return vref_mV;
		}
		else if (me->ch[i] == ch)
		{
			uint32_t voltage_mV = __LL_ADC_CALC_DATA_TO_VOLTAGE(vref_mV, me->buff[i], me->resolution);
			return voltage_mV;
		}
	}

	ALX_ADC_ASSERT(false); // We shouldn't get here
	return 0;
}
float AlxAdc_TempSens_GetTemp_degC(AlxAdc* me)
{
	ALX_ADC_ASSERT(me->isInit == true);
	ALX_ADC_ASSERT(me->wasCtorCalled == true);

	// #1 Get Reference Voltage
	uint32_t vref_mV = AlxAdc_GetVref_mV(me);

	// #2 Return Temperature Channel
	for (uint32_t i = 0; i < me->numOfCh; i++)
	{
		if (me->ch[i] == Alx_Ch_McuStm32_TempSens)
		{
			#if (defined(ALX_STM32F4) || defined(ALX_STM32G4) || defined(ALX_STM32L0)) && (!defined(STM32F469) && !defined(STM32F479xx) && !defined(STM32F429xx) && !defined(STM32F439xx))
			int32_t temp_degC = __LL_ADC_CALC_TEMPERATURE(vref_mV, me->buff[i], me->resolution);
			#endif
			
			#if defined(ALX_STM32F1) || defined(STM32F469) || defined(STM32F479xx) || defined(STM32F429xx) || defined(STM32F439xx)
			int32_t temp_degC = 0;// NE DELA
			ALX_ADC_ASSERT(false); // We shouldn't get here
			#endif
			return (float)temp_degC;
		}
	}

	ALX_ADC_ASSERT(false); // We shouldn't get here
	return 0;
}


//******************************************************************************
// Private Functions
//******************************************************************************
static void AlxAdc_PeriphAdc_EnableClk(AlxAdc* me)
{
	bool isErr = true;

	#if defined(STM32F1) || defined(STM32F4) || defined(STM32L0)
		#if defined(ADC1)
		if (me->hadc.Instance == ADC1)	{ __HAL_RCC_ADC1_CLK_ENABLE(); isErr = false; }
		#endif
		#if defined(ADC2)
		if (me->hadc.Instance == ADC2)	{ __HAL_RCC_ADC2_CLK_ENABLE(); isErr = false; }
		#endif
		#if defined(ADC3)
		if (me->hadc.Instance == ADC3)	{ __HAL_RCC_ADC3_CLK_ENABLE(); isErr = false; }
		#endif
		#if defined(ADC4)
		if (me->hadc.Instance == ADC4)	{ __HAL_RCC_ADC4_CLK_ENABLE(); isErr = false; }
		#endif
	#endif
	#if defined(STM32G4)
		#if defined(ADC1)
		if (me->hadc.Instance == ADC1)	{ __HAL_RCC_ADC12_CLK_ENABLE(); isErr = false; }
		#endif
		#if defined(ADC2)
		if (me->hadc.Instance == ADC2)	{ __HAL_RCC_ADC12_CLK_ENABLE(); isErr = false; }
		#endif
		#if defined(ADC3)
		if (me->hadc.Instance == ADC3)	{ __HAL_RCC_ADC345_CLK_ENABLE(); isErr = false; }
		#endif
		#if defined(ADC4)
		if (me->hadc.Instance == ADC4)	{ __HAL_RCC_ADC345_CLK_ENABLE(); isErr = false; }
		#endif
	#endif

	if(isErr)							{ ALX_ADC_ASSERT(false); } // We shouldn't get here
}
static void AlxAdc_PeriphAdc_DisableClk(AlxAdc* me)
{
	bool isErr = true;

	#if defined(STM32F1) || defined(STM32F4) || defined(STM32L0)
		#if defined(ADC1)
		if (me->hadc.Instance == ADC1)	{ __HAL_RCC_ADC1_CLK_DISABLE(); isErr = false; }
		#endif
		#if defined(ADC2)
		if (me->hadc.Instance == ADC2)	{ __HAL_RCC_ADC2_CLK_DISABLE(); isErr = false; }
		#endif
		#if defined(ADC3)
		if (me->hadc.Instance == ADC3)	{ __HAL_RCC_ADC3_CLK_DISABLE(); isErr = false; }
		#endif
		#if defined(ADC4)
		if (me->hadc.Instance == ADC4)	{ __HAL_RCC_ADC4_CLK_DISABLE(); isErr = false; }
		#endif
	#endif
	#if defined(STM32G4)
		#if defined(ADC1)
		if (me->hadc.Instance == ADC1)	{ __HAL_RCC_ADC12_CLK_DISABLE(); isErr = false; }
		#endif
		#if defined(ADC2)
		if (me->hadc.Instance == ADC2)	{ __HAL_RCC_ADC12_CLK_DISABLE(); isErr = false; }
		#endif
		#if defined(ADC3)
		if (me->hadc.Instance == ADC3)	{ __HAL_RCC_ADC345_CLK_DISABLE(); isErr = false; }
		#endif
		#if defined(ADC4)
		if (me->hadc.Instance == ADC4)	{ __HAL_RCC_ADC345_CLK_DISABLE(); isErr = false; }
		#endif
	#endif

	if(isErr)							{ ALX_ADC_ASSERT(false); } // We shouldn't get here
}
static void AlxAdc_PeriphAdc_ForceReset(AlxAdc* me)
{
	bool isErr = true;

	#if defined(STM32F1) || defined(STM32L0)
		#if defined(ADC1)
		if (me->hadc.Instance == ADC1)	{ __HAL_RCC_ADC1_FORCE_RESET(); isErr = false; }
		#endif
		#if defined(ADC2)
		if (me->hadc.Instance == ADC2)	{ __HAL_RCC_ADC2_FORCE_RESET(); isErr = false; }
		#endif
		#if defined(ADC3)
		if (me->hadc.Instance == ADC3)	{ __HAL_RCC_ADC3_FORCE_RESET(); isErr = false; }
		#endif
		#if defined(ADC4)
		if (me->hadc.Instance == ADC4)	{ __HAL_RCC_ADC4_FORCE_RESET(); isErr = false; }
		#endif
	#endif
	#if defined(STM32F4)
		__HAL_RCC_ADC_FORCE_RESET(); isErr = false;
	#endif
	#if defined(STM32G4)
		#if defined(ADC1)
		if (me->hadc.Instance == ADC1)	{ __HAL_RCC_ADC12_FORCE_RESET(); isErr = false; }
		#endif
		#if defined(ADC2)
		if (me->hadc.Instance == ADC2)	{ __HAL_RCC_ADC12_FORCE_RESET(); isErr = false; }
		#endif
		#if defined(ADC3)
		if (me->hadc.Instance == ADC3)	{ __HAL_RCC_ADC345_FORCE_RESET(); isErr = false; }
		#endif
		#if defined(ADC4)
		if (me->hadc.Instance == ADC4)	{ __HAL_RCC_ADC345_FORCE_RESET(); isErr = false; }
		#endif
	#endif

	if(isErr)							{ ALX_ADC_ASSERT(false); } // We shouldn't get here
}
static void AlxAdc_PeriphAdc_ReleaseReset(AlxAdc* me)
{
	bool isErr = true;

	#if defined(STM32F1) || defined(STM32L0)
		#if defined(ADC1)
		if (me->hadc.Instance == ADC1)	{ __HAL_RCC_ADC1_RELEASE_RESET(); isErr = false; }
		#endif
		#if defined(ADC2)
		if (me->hadc.Instance == ADC2)	{ __HAL_RCC_ADC2_RELEASE_RESET(); isErr = false; }
		#endif
		#if defined(ADC3)
		if (me->hadc.Instance == ADC3)	{ __HAL_RCC_ADC3_RELEASE_RESET(); isErr = false; }
		#endif
		#if defined(ADC4)
		if (me->hadc.Instance == ADC4)	{ __HAL_RCC_ADC4_RELEASE_RESET(); isErr = false; }
		#endif
	#endif
	#if defined(STM32F4)
		__HAL_RCC_ADC_RELEASE_RESET(); isErr = false;
	#endif
	#if defined(STM32G4)
		#if defined(ADC1)
		if (me->hadc.Instance == ADC1)	{ __HAL_RCC_ADC12_RELEASE_RESET(); isErr = false; }
		#endif
		#if defined(ADC2)
		if (me->hadc.Instance == ADC2)	{ __HAL_RCC_ADC12_RELEASE_RESET(); isErr = false; }
		#endif
		#if defined(ADC3)
		if (me->hadc.Instance == ADC3)	{ __HAL_RCC_ADC345_RELEASE_RESET(); isErr = false; }
		#endif
		#if defined(ADC4)
		if (me->hadc.Instance == ADC4)	{ __HAL_RCC_ADC345_RELEASE_RESET(); isErr = false; }
		#endif
		#if defined(ADC5)
		if (me->hadc.Instance == ADC5)	{ __HAL_RCC_ADC345_RELEASE_RESET(); isErr = false; }
		#endif
	#endif

	if(isErr)							{ ALX_ADC_ASSERT(false); } // We shouldn't get here
}
static void AlxAdc_PeriphDma_EnableClk(AlxAdc* me)
{
	#if defined(STM32G4)
		__HAL_RCC_DMAMUX1_CLK_ENABLE();
	#endif

	bool isErr = true;

	#if defined(DMA1)
	if (me->dma == DMA1)	{ __HAL_RCC_DMA1_CLK_ENABLE(); isErr = false; }
	#endif
	#if defined(DMA2)
	if (me->dma == DMA2)	{ __HAL_RCC_DMA2_CLK_ENABLE(); isErr = false; }
	#endif
	#if defined(DMA3)
	if (me->dma == DMA3)	{ __HAL_RCC_DMA3_CLK_ENABLE(); isErr = false; }
	#endif

	if(isErr)				{ ALX_ADC_ASSERT(false); } // We shouldn't get here
}
static void AlxAdc_PeriphDma_DisableClk(AlxAdc* me)
{
	#if defined(STM32G4)
		__HAL_RCC_DMAMUX1_CLK_DISABLE();
	#endif

	bool isErr = true;

	#if defined(DMA1)
	if (me->dma == DMA1)	{ __HAL_RCC_DMA1_CLK_DISABLE(); isErr = false; }
	#endif
	#if defined(DMA2)
	if (me->dma == DMA2)	{ __HAL_RCC_DMA2_CLK_DISABLE(); isErr = false; }
	#endif
	#if defined(DMA3)
	if (me->dma == DMA3)	{ __HAL_RCC_DMA3_CLK_DISABLE(); isErr = false; }
	#endif

	if(isErr)				{ ALX_ADC_ASSERT(false); } // We shouldn't get here
}
static void AlxAdc_PeriphDma_ForceReset(AlxAdc* me)
{
	#if defined(STM32F1)
	(void)me;
	#endif

	#if defined(STM32G4)
		__HAL_RCC_DMAMUX1_FORCE_RESET();
	#endif

	#if defined(ALX_STM32F4) || defined(ALX_STM32G4) || defined(ALX_STM32L0)
	bool isErr = true;

	#if defined(DMA1)
	if (me->dma == DMA1)	{ __HAL_RCC_DMA1_FORCE_RESET(); isErr = false; }
	#endif
	#if defined(DMA2)
	if (me->dma == DMA2)	{ __HAL_RCC_DMA2_FORCE_RESET(); isErr = false; }
	#endif
	#if defined(DMA3)
	if (me->dma == DMA3)	{ __HAL_RCC_DMA3_FORCE_RESET(); isErr = false; }
	#endif

	if(isErr)				{ ALX_ADC_ASSERT(false); } // We shouldn't get here
	#endif
}
static void AlxAdc_PeriphDma_ReleaseReset(AlxAdc* me)
{
	#if defined(STM32F1)
	(void)me;
	#endif

	#if defined(STM32G4)
		__HAL_RCC_DMAMUX1_RELEASE_RESET();
	#endif

	#if defined(ALX_STM32F4) || defined(ALX_STM32G4) || defined(ALX_STM32L0)
	bool isErr = true;

	#if defined(DMA1)
	if (me->dma == DMA1)	{ __HAL_RCC_DMA1_RELEASE_RESET(); isErr = false; }
	#endif
	#if defined(DMA2)
	if (me->dma == DMA2)	{ __HAL_RCC_DMA2_RELEASE_RESET(); isErr = false; }
	#endif
	#if defined(DMA3)
	if (me->dma == DMA3)	{ __HAL_RCC_DMA3_RELEASE_RESET(); isErr = false; }
	#endif

	if(isErr)				{ ALX_ADC_ASSERT(false); } // We shouldn't get here
	#endif
}
static uint32_t AlxAdc_GetVref_mV(AlxAdc* me)
{
	if (me->isVrefInt_V)
	{
		for (uint32_t i = 0; i < me->numOfCh; i++)
		{
			if (me->ch[i] == Alx_Ch_McuStm32_Vref)
			{
				uint32_t vref_mV = __LL_ADC_CALC_VREFANALOG_VOLTAGE(me->buff[i], me->resolution);
				return vref_mV;
			}
		}

		ALX_ADC_ASSERT(false); // We shouldn't get here
		return ALX_NULL;
	}
	else
	{
		#if defined(ALX_ADC_OPTIMIZE_SIZE)
		uint32_t vref_mV = (uint32_t)me->vrefExt_V * 1000UL;
		return vref_mV;
		#else
		float vref_mV = me->vrefExt_V * 1000.f;
		return (uint32_t)vref_mV;
		#endif
	}

	ALX_ADC_ASSERT(false); // We shouldn't get here
	return ALX_NULL;
}
static uint32_t AlxAdc_GetCh(AlxAdc* me, Alx_Ch ch)
{
	//******************************************************************************
	// STM32F1
	//******************************************************************************
	#if defined(STM32F1)
	if(ch == Alx_Ch_0 )					return ADC_CHANNEL_0 ;
	if(ch == Alx_Ch_1 )					return ADC_CHANNEL_1 ;
	if(ch == Alx_Ch_2 )					return ADC_CHANNEL_2 ;
	if(ch == Alx_Ch_3 )					return ADC_CHANNEL_3 ;
	if(ch == Alx_Ch_4 )					return ADC_CHANNEL_4 ;
	if(ch == Alx_Ch_5 )					return ADC_CHANNEL_5 ;
	if(ch == Alx_Ch_6 )					return ADC_CHANNEL_6 ;
	if(ch == Alx_Ch_7 )					return ADC_CHANNEL_7 ;
	if(ch == Alx_Ch_8 )					return ADC_CHANNEL_8 ;
	if(ch == Alx_Ch_9 )					return ADC_CHANNEL_9 ;
	if(ch == Alx_Ch_10)					return ADC_CHANNEL_10;
	if(ch == Alx_Ch_11)					return ADC_CHANNEL_11;
	if(ch == Alx_Ch_12)					return ADC_CHANNEL_12;
	if(ch == Alx_Ch_13)					return ADC_CHANNEL_13;
	if(ch == Alx_Ch_14)					return ADC_CHANNEL_14;
	if(ch == Alx_Ch_15)					return ADC_CHANNEL_15;
	if(ch == Alx_Ch_McuStm32_TempSens)	return ADC_CHANNEL_TEMPSENSOR; // Ch16
	if(ch == Alx_Ch_McuStm32_Vref)		return ADC_CHANNEL_VREFINT; // Ch17

	ALX_ADC_ASSERT(false); // We shouldn't get here
	return 0;
	#endif


	//******************************************************************************
	// STM32F4
	//******************************************************************************
	#if defined(STM32F4)
	if(ch == Alx_Ch_0 )					return ADC_CHANNEL_0 ;
	if(ch == Alx_Ch_1 )					return ADC_CHANNEL_1 ;
	if(ch == Alx_Ch_2 )					return ADC_CHANNEL_2 ;
	if(ch == Alx_Ch_3 )					return ADC_CHANNEL_3 ;
	if(ch == Alx_Ch_4 )					return ADC_CHANNEL_4 ;
	if(ch == Alx_Ch_5 )					return ADC_CHANNEL_5 ;
	if(ch == Alx_Ch_6 )					return ADC_CHANNEL_6 ;
	if(ch == Alx_Ch_7 )					return ADC_CHANNEL_7 ;
	if(ch == Alx_Ch_8 )					return ADC_CHANNEL_8 ;
	if(ch == Alx_Ch_9 )					return ADC_CHANNEL_9 ;
	if(ch == Alx_Ch_10)					return ADC_CHANNEL_10;
	if(ch == Alx_Ch_11)					return ADC_CHANNEL_11;
	if(ch == Alx_Ch_12)					return ADC_CHANNEL_12;
	if(ch == Alx_Ch_13)					return ADC_CHANNEL_13;
	if(ch == Alx_Ch_14)					return ADC_CHANNEL_14;
	if(ch == Alx_Ch_15)					return ADC_CHANNEL_15;
	if(ch == Alx_Ch_McuStm32_TempSens)	return ADC_CHANNEL_TEMPSENSOR; // Ch16 or Ch18
	if(ch == Alx_Ch_McuStm32_Vref)		return ADC_CHANNEL_VREFINT; // Ch17
	if(ch == Alx_Ch_McuStm32_Vbat)		return ADC_CHANNEL_VBAT; // Ch18

	ALX_ADC_ASSERT(false); // We shouldn't get here
	return 0;
	#endif


	//******************************************************************************
	// STM32G4
	//******************************************************************************
	#if defined(STM32G4)
	if(ch == Alx_Ch_0 )				return ADC_CHANNEL_0 ;
	if(ch == Alx_Ch_1 )				return ADC_CHANNEL_1 ;
	if(ch == Alx_Ch_2 )				return ADC_CHANNEL_2 ;
	if(ch == Alx_Ch_3 )				return ADC_CHANNEL_3 ;
	if(ch == Alx_Ch_4 )				return ADC_CHANNEL_4 ;
	if(ch == Alx_Ch_5 )				return ADC_CHANNEL_5 ;
	if(ch == Alx_Ch_6 )				return ADC_CHANNEL_6 ;
	if(ch == Alx_Ch_7 )				return ADC_CHANNEL_7 ;
	if(ch == Alx_Ch_8 )				return ADC_CHANNEL_8 ;
	if(ch == Alx_Ch_9 )				return ADC_CHANNEL_9 ;
	if(ch == Alx_Ch_10)				return ADC_CHANNEL_10;
	if(ch == Alx_Ch_11)				return ADC_CHANNEL_11;
	if(ch == Alx_Ch_12)				return ADC_CHANNEL_12;
	if(ch == Alx_Ch_13)				return ADC_CHANNEL_13;
	if(ch == Alx_Ch_14)				return ADC_CHANNEL_14;
	if(ch == Alx_Ch_15)				return ADC_CHANNEL_15;
	if(ch == Alx_Ch_McuStm32_TempSens)
	{
		#if defined(STM32G4)
			if (me->hadc.Instance == ADC1)
				return ADC_CHANNEL_TEMPSENSOR_ADC1; // Ch16
		#if !defined(STM32G431xx)
			else if (me->hadc.Instance == ADC5)
				return ADC_CHANNEL_TEMPSENSOR_ADC5; // Ch4
		#endif
			else
				ALX_ADC_ASSERT(false); // We shouldn't get here
		#endif
	}
	if(ch == Alx_Ch_McuStm32_Vbat)	return ADC_CHANNEL_VBAT; // Ch17
	if(ch == Alx_Ch_McuStm32_Vref)	return ADC_CHANNEL_VREFINT; // Ch18

	ALX_ADC_ASSERT(false); // We shouldn't get here
	return 0;
	#endif


	//******************************************************************************
	// STM32L0
	//******************************************************************************
	#if defined(STM32L0)
	if(ch == Alx_Ch_0 )					return ADC_CHANNEL_0 ;
	if(ch == Alx_Ch_1 )					return ADC_CHANNEL_1 ;
	if(ch == Alx_Ch_2 )					return ADC_CHANNEL_2 ;
	if(ch == Alx_Ch_3 )					return ADC_CHANNEL_3 ;
	if(ch == Alx_Ch_4 )					return ADC_CHANNEL_4 ;
	if(ch == Alx_Ch_5 )					return ADC_CHANNEL_5 ;
	if(ch == Alx_Ch_6 )					return ADC_CHANNEL_6 ;
	if(ch == Alx_Ch_7 )					return ADC_CHANNEL_7 ;
	if(ch == Alx_Ch_8 )					return ADC_CHANNEL_8 ;
	if(ch == Alx_Ch_9 )					return ADC_CHANNEL_9 ;
	if(ch == Alx_Ch_10)					return ADC_CHANNEL_10;
	if(ch == Alx_Ch_11)					return ADC_CHANNEL_11;
	if(ch == Alx_Ch_12)					return ADC_CHANNEL_12;
	if(ch == Alx_Ch_13)					return ADC_CHANNEL_13;
	if(ch == Alx_Ch_14)					return ADC_CHANNEL_14;
	if(ch == Alx_Ch_15)					return ADC_CHANNEL_15;
	#if defined (STM32L053xx) || defined (STM32L063xx) || defined (STM32L073xx) || defined (STM32L083xx)
	if (ch == Alx_Ch_McuStm32_Vlcd)		return ADC_CHANNEL_VLCD; // Ch16
	#endif
	if(ch == Alx_Ch_McuStm32_Vref)		return ADC_CHANNEL_VREFINT; // Ch17
	if(ch == Alx_Ch_McuStm32_TempSens)	return ADC_CHANNEL_TEMPSENSOR; // Ch18

	ALX_ADC_ASSERT(false); // We shouldn't get here
	return 0;
	#endif
}
static uint32_t AlxAdc_GetRank(uint8_t buffPos)
{
	//******************************************************************************
	// STM32F1
	//******************************************************************************
	#if defined(STM32F1)
	ALX_ADC_ASSERT((0 <= buffPos) && (buffPos <= 15));
	return buffPos + 1;
	#endif

	//******************************************************************************
	// STM32F4
	//******************************************************************************
	#if defined(STM32F4)
	ALX_ADC_ASSERT((0 <= buffPos) && (buffPos <= 15));
	return buffPos + 1;
	#endif


	//******************************************************************************
	// STM32G4
	//******************************************************************************
	#if defined(STM32G4)
	if(buffPos == 0 ) return ADC_REGULAR_RANK_1 ;
	if(buffPos == 1 ) return ADC_REGULAR_RANK_2 ;
	if(buffPos == 2 ) return ADC_REGULAR_RANK_3 ;
	if(buffPos == 3 ) return ADC_REGULAR_RANK_4 ;
	if(buffPos == 4 ) return ADC_REGULAR_RANK_5 ;
	if(buffPos == 5 ) return ADC_REGULAR_RANK_6 ;
	if(buffPos == 6 ) return ADC_REGULAR_RANK_7 ;
	if(buffPos == 7 ) return ADC_REGULAR_RANK_8 ;
	if(buffPos == 8 ) return ADC_REGULAR_RANK_9 ;
	if(buffPos == 9 ) return ADC_REGULAR_RANK_10;
	if(buffPos == 10) return ADC_REGULAR_RANK_11;
	if(buffPos == 11) return ADC_REGULAR_RANK_12;
	if(buffPos == 12) return ADC_REGULAR_RANK_13;
	if(buffPos == 13) return ADC_REGULAR_RANK_14;
	if(buffPos == 14) return ADC_REGULAR_RANK_15;
	if(buffPos == 15) return ADC_REGULAR_RANK_16;

	ALX_ADC_ASSERT(false); // We shouldn't get here
	return 0;
	#endif


	//******************************************************************************
	// STM32L0
	//******************************************************************************
	#if defined(STM32L0)
	return ALX_NULL;
	#endif
}
static bool AlxAdc_Ctor_IsPclk2Apb2Ok(AlxAdc* me)
{
	//******************************************************************************
	// STM32F1
	//******************************************************************************
	#if defined(STM32F1)
	if(me->adcClk == AlxAdc_Clk_McuStm32F1_AdcClk_4MHz_Pclk2Apb2_8MHz)
	{
		if(8000000UL == AlxClk_GetClk_Hz(me->clk, AlxClk_Clk_McuStm32_Pclk2Apb2_Ctor))
			return true;
		else
			return false;
	}

	ALX_ADC_ASSERT(false); // We shouldn't get here
	return 0;
	#endif


	//******************************************************************************
	// STM32F4
	//******************************************************************************
	#if defined(STM32F4)
	if(me->adcClk == AlxAdc_Clk_McuStm32F4_AdcClk_22MHz5_Pclk2Apb2_90MHz)
	{
		if(90000000UL == AlxClk_GetClk_Hz(me->clk, AlxClk_Clk_McuStm32_Pclk2Apb2_Ctor))
			return true;
		else
			return false;
	}
	if(me->adcClk == AlxAdc_Clk_McuStm32F4_AdcClk_11MHz25_Pclk2Apb2_90MHz)
	{
		if(90000000UL == AlxClk_GetClk_Hz(me->clk, AlxClk_Clk_McuStm32_Pclk2Apb2_Ctor))
			return true;
		else
			return false;
	}

	ALX_ADC_ASSERT(false); // We shouldn't get here
	return 0;
	#endif


	//******************************************************************************
	// STM32G4
	//******************************************************************************
	#if defined(STM32G4)
	if(me->adcClk == AlxAdc_Clk_McuStm32G4_AdcClk_42MHz5_Pclk2Apb2_170MHz)
	{
		if(170000000UL == AlxClk_GetClk_Hz(me->clk, AlxClk_Clk_McuStm32_Pclk2Apb2_Ctor))
			return true;
		else
			return false;
	}

	ALX_ADC_ASSERT(false); // We shouldn't get here
	return 0;
	#endif


	//******************************************************************************
	// STM32L0
	//******************************************************************************
	#if defined(STM32L0)
	if(me->adcClk == AlxAdc_Clk_McuStm32L0_AdcClk_525kHz_Pclk2Apb2_2MHz1)
	{
		if(2097152UL == AlxClk_GetClk_Hz(me->clk, AlxClk_Clk_McuStm32_Pclk2Apb2_Ctor))
			return true;
		else
			return false;
	}
	if(me->adcClk == AlxAdc_Clk_McuStm32L0_AdcClk_8MHz_Pclk2Apb2_32MHz)
	{
		if(32000000UL == AlxClk_GetClk_Hz(me->clk, AlxClk_Clk_McuStm32_Pclk2Apb2_Ctor))
			return true;
		else
			return false;
	}

	ALX_ADC_ASSERT(false); // We shouldn't get here
	return 0;
	#endif
}


#endif
