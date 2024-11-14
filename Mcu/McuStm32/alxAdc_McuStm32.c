/**
  ******************************************************************************
  * @file		alxAdc_McuStm32.c
  * @brief		Auralix C Library - ALX ADC MCU STM32 Module
  * @copyright	Copyright (C) Auralix d.o.o. All rights reserved.
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
#if defined(ALX_C_LIB) && (defined(ALX_STM32F0) || defined(ALX_STM32F1) || defined(ALX_STM32F4) || defined(ALX_STM32F7) || defined(ALX_STM32G4) || defined(ALX_STM32L0) || defined(ALX_STM32L4) || defined(ALX_STM32U5))


//******************************************************************************
// Private Functions
//******************************************************************************


//------------------------------------------------------------------------------
// Specific
//------------------------------------------------------------------------------
static uint32_t AlxAdc_GetVref_mV(AlxAdc* me);
static uint32_t AlxAdc_GetClkPrescaler(AlxAdc* me);
static uint32_t AlxAdc_GetCh(AlxAdc* me, Alx_Ch ch);
static uint32_t AlxAdc_GetRank(uint8_t buffPos);


//------------------------------------------------------------------------------
// General
//------------------------------------------------------------------------------
static bool AlxAdc_IsClkOk(AlxAdc* me);
static void AlxAdc_Periph_Adc_EnableClk(AlxAdc* me);
static void AlxAdc_Periph_Adc_DisableClk(AlxAdc* me);
static void AlxAdc_Periph_Adc_ForceReset(AlxAdc* me);
static void AlxAdc_Periph_Adc_ReleaseReset(AlxAdc* me);
static void AlxAdc_Periph_Dma_EnableClk(AlxAdc* me);
static void AlxAdc_Periph_Dma_DisableClk(AlxAdc* me);
static void AlxAdc_Periph_Dma_ForceReset(AlxAdc* me);
static void AlxAdc_Periph_Dma_ReleaseReset(AlxAdc* me);


//******************************************************************************
// Constructor
//******************************************************************************

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		adc
  * @param[in]		ioPinArr
  * @param[in]		numOfIoPins
  * @param[in]		chArr
  * @param[in]		numOfCh
  * @param[in]		clk
  * @param[in]		adcClk
  * @param[in]		samplingTime
  * @param[in]		isVrefInt_V
  * @param[in]		vrefExt_V
  */
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
	//------------------------------------------------------------------------------
	// Assert
	//------------------------------------------------------------------------------
	(void)me;
	(void)ioPinArr;
	ALX_ADC_ASSERT(numOfIoPins <= ALX_ADC_BUFF_LEN - 1);	// We have only 16 ranks, one is used for VrefInt
	(void)chArr;
	ALX_ADC_ASSERT(numOfCh <= ALX_ADC_BUFF_LEN);			// We have only 16 ranks
	(void)clk;
	(void)adcClk;
	(void)samplingTime;
	(void)isVrefInt_V;
	(void)vrefExt_V;


	//------------------------------------------------------------------------------
	// Parameters
	//------------------------------------------------------------------------------
	me->adc = adc;
	me->ioPinArr = ioPinArr;
	me->numOfIoPins = numOfIoPins;
	me->chArr = chArr;
	me->numOfCh = numOfCh;
	me->clk = clk;
	me->adcClk = adcClk;
	me->samplingTime = samplingTime;
	me->isVrefInt_V = isVrefInt_V;
	me->vrefExt_V = vrefExt_V;

	// Check channel sequence
	for (uint8_t i = 0; i < numOfCh - 1; i++)
	{
		uint32_t chNumCurrent = AlxAdc_GetCh(me, chArr[i]);
		uint32_t chNumNext = AlxAdc_GetCh(me, chArr[i+1]);
		ALX_ADC_ASSERT(chNumCurrent < chNumNext);	// Channel sequence must be from low to high number
	}

	// Check clock
	ALX_ADC_ASSERT(AlxAdc_IsClkOk(me));


	//------------------------------------------------------------------------------
	// Variables
	//------------------------------------------------------------------------------

	//------------------------------------------------------------------------------
	// STM32F0
	//------------------------------------------------------------------------------
	#if defined(ALX_STM32F0)

	// ADC Common
	me->hadc.Instance = adc;
	me->hadc.Init.ClockPrescaler = AlxAdc_GetClkPrescaler(me);
	me->hadc.Init.Resolution = ALX_ADC_RESOLUTION;
	me->hadc.Init.DataAlign = ADC_DATAALIGN_RIGHT;
	me->hadc.Init.ScanConvMode = ADC_SCAN_DIRECTION_FORWARD;
	me->hadc.Init.EOCSelection = ADC_EOC_SEQ_CONV;
	me->hadc.Init.LowPowerAutoWait = DISABLE;
	me->hadc.Init.LowPowerAutoPowerOff = DISABLE;
	me->hadc.Init.ContinuousConvMode = ENABLE;
	me->hadc.Init.DiscontinuousConvMode = DISABLE;
	me->hadc.Init.ExternalTrigConv = ADC_SOFTWARE_START;
	me->hadc.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
	me->hadc.Init.DMAContinuousRequests = ENABLE;
	me->hadc.Init.Overrun = ADC_OVR_DATA_PRESERVED;
	me->hadc.Init.SamplingTimeCommon = samplingTime;

	// ADC Channel
	for (uint32_t buffPos = 0; buffPos < numOfCh; buffPos++)
	{
		me->chadc[buffPos].Channel = AlxAdc_GetCh(me, *(chArr + buffPos));
		me->chadc[buffPos].Rank = AlxAdc_GetRank(buffPos);
		me->chadc[buffPos].SamplingTime = ALX_NULL;
		me->ch[buffPos] = *(chArr + buffPos);
	}

	// DMA
	if (me->hadc.Instance == ADC1)
	{
		me->dma = DMA1;
		me->hdma.Instance = DMA1_Channel1;
	}
	me->hdma.Init.Direction = DMA_PERIPH_TO_MEMORY;
	me->hdma.Init.PeriphInc = DMA_PINC_DISABLE;
	me->hdma.Init.MemInc = DMA_MINC_ENABLE;
	me->hdma.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
	me->hdma.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
	me->hdma.Init.Mode = DMA_CIRCULAR;
	me->hdma.Init.Priority = DMA_PRIORITY_LOW;
	#endif


	//------------------------------------------------------------------------------
	// STM32F1
	//------------------------------------------------------------------------------
	#if defined(ALX_STM32F1)

	// ADC Clock
	me->iclk.PeriphClockSelection = RCC_PERIPHCLK_ADC;
	me->iclk.RTCClockSelection = ALX_NULL;
	me->iclk.AdcClockSelection = AlxAdc_GetClkPrescaler(me);
	me->iclk.UsbClockSelection = ALX_NULL;

	// ADC Common
	me->hadc.Instance = adc;
	me->hadc.Init.DataAlign = ADC_DATAALIGN_RIGHT;
	me->hadc.Init.ScanConvMode = ENABLE;
	me->hadc.Init.ContinuousConvMode = ENABLE;
	me->hadc.Init.NbrOfConversion = numOfCh;
	me->hadc.Init.DiscontinuousConvMode = DISABLE;
	me->hadc.Init.NbrOfDiscConversion = ALX_NULL;
	me->hadc.Init.ExternalTrigConv = ADC_SOFTWARE_START;

	// ADC Channel
	for (uint32_t buffPos = 0; buffPos < numOfCh; buffPos++)
	{
		me->chadc[buffPos].Channel = AlxAdc_GetCh(me, *(chArr + buffPos));
		me->chadc[buffPos].Rank = AlxAdc_GetRank(buffPos);
		me->chadc[buffPos].SamplingTime = samplingTime;
		me->ch[buffPos] = *(chArr + buffPos);
	}

	// DMA
	if (me->hadc.Instance == ADC1)
	{
		me->dma = DMA1;
		me->hdma.Instance = DMA1_Channel1;
	}
	me->hdma.Init.Direction = DMA_PERIPH_TO_MEMORY;
	me->hdma.Init.PeriphInc = DMA_PINC_DISABLE;
	me->hdma.Init.MemInc = DMA_MINC_ENABLE;
	me->hdma.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
	me->hdma.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
	me->hdma.Init.Mode = DMA_CIRCULAR;
	me->hdma.Init.Priority = DMA_PRIORITY_LOW;
	#endif


	//------------------------------------------------------------------------------
	// STM32F4, STM32F7
	//------------------------------------------------------------------------------
	#if defined(ALX_STM32F4) || defined(ALX_STM32F7)

	// ADC Common
	me->hadc.Instance = adc;
	me->hadc.Init.ClockPrescaler = AlxAdc_GetClkPrescaler(me);
	me->hadc.Init.Resolution = ALX_ADC_RESOLUTION;
	me->hadc.Init.DataAlign = ADC_DATAALIGN_RIGHT;
	me->hadc.Init.ScanConvMode = ENABLE;
	me->hadc.Init.EOCSelection = ADC_EOC_SEQ_CONV;
	me->hadc.Init.ContinuousConvMode = ENABLE;
	me->hadc.Init.NbrOfConversion = numOfCh;
	me->hadc.Init.DiscontinuousConvMode = DISABLE;
	me->hadc.Init.NbrOfDiscConversion = ALX_NULL;
	me->hadc.Init.ExternalTrigConv = ADC_SOFTWARE_START;
	me->hadc.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
	me->hadc.Init.DMAContinuousRequests = ENABLE;

	// ADC Channel
	for (uint32_t buffPos = 0; buffPos < numOfCh; buffPos++)
	{
		me->chadc[buffPos].Channel = AlxAdc_GetCh(me, *(chArr + buffPos));
		me->chadc[buffPos].Rank = AlxAdc_GetRank(buffPos);
		me->chadc[buffPos].SamplingTime = samplingTime;
		me->chadc[buffPos].Offset = 0;
		me->ch[buffPos] = *(chArr + buffPos);
	}

	// DMA
	if (me->hadc.Instance == ADC1)
	{
		me->dma = DMA2;
		me->hdma.Instance = DMA2_Stream0;
		me->hdma.Init.Channel = DMA_CHANNEL_0;
	}
	if (me->hadc.Instance == ADC2) // JS: not tested
	{
		me->dma = DMA2;
		me->hdma.Instance = DMA2_Stream2;
		me->hdma.Init.Channel = DMA_CHANNEL_1;
	}
	if (me->hadc.Instance == ADC3)
	{
		me->dma = DMA2;
		me->hdma.Instance = DMA2_Stream1;
		me->hdma.Init.Channel = DMA_CHANNEL_2;
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


	//------------------------------------------------------------------------------
	// STM32G4
	//------------------------------------------------------------------------------
	#if defined(ALX_STM32G4)

	// ADC Common
	me->hadc.Instance = adc;
	me->hadc.Init.ClockPrescaler = AlxAdc_GetClkPrescaler(me);
	me->hadc.Init.Resolution = ALX_ADC_RESOLUTION;
	me->hadc.Init.DataAlign = ADC_DATAALIGN_RIGHT;
	me->hadc.Init.GainCompensation = 0;
	me->hadc.Init.ScanConvMode = ADC_SCAN_ENABLE;
	me->hadc.Init.EOCSelection = ADC_EOC_SEQ_CONV;
	me->hadc.Init.LowPowerAutoWait = DISABLE;
	me->hadc.Init.ContinuousConvMode = ENABLE;
	me->hadc.Init.NbrOfConversion = numOfCh;
	me->hadc.Init.DiscontinuousConvMode = DISABLE;
	me->hadc.Init.NbrOfDiscConversion = ALX_NULL;
	me->hadc.Init.ExternalTrigConv = ADC_SOFTWARE_START;
	me->hadc.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
	me->hadc.Init.SamplingMode = ADC_SAMPLING_MODE_NORMAL;
	me->hadc.Init.DMAContinuousRequests = ENABLE;
	me->hadc.Init.Overrun = ADC_OVR_DATA_PRESERVED;
	me->hadc.Init.OversamplingMode = DISABLE;
	me->hadc.Init.Oversampling.Ratio = ALX_NULL;
	me->hadc.Init.Oversampling.RightBitShift = ALX_NULL;
	me->hadc.Init.Oversampling.TriggeredMode = ALX_NULL;
	me->hadc.Init.Oversampling.OversamplingStopReset = ALX_NULL;

	// ADC Channel
	for (uint32_t buffPos = 0; buffPos < numOfCh; buffPos++)
	{
		me->chadc[buffPos].Channel = AlxAdc_GetCh(me, *(chArr + buffPos));
		me->chadc[buffPos].Rank = AlxAdc_GetRank(buffPos);
		me->chadc[buffPos].SamplingTime = samplingTime;
		me->chadc[buffPos].SingleDiff = ADC_SINGLE_ENDED;
		me->chadc[buffPos].OffsetNumber = ADC_OFFSET_NONE;
		me->chadc[buffPos].Offset = 0;
		me->chadc[buffPos].OffsetSign = 0;
		me->chadc[buffPos].OffsetSaturation = DISABLE;
		me->ch[buffPos] = *(chArr + buffPos);
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


	//------------------------------------------------------------------------------
	// STM32L0
	//------------------------------------------------------------------------------
	#if defined(ALX_STM32L0)

	// ADC Common
	me->hadc.Instance = adc;
	me->hadc.Init.ClockPrescaler = AlxAdc_GetClkPrescaler(me);
	me->hadc.Init.Resolution = ALX_ADC_RESOLUTION;
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
	me->hadc.Init.Oversample.Ratio = ALX_NULL;
	me->hadc.Init.Oversample.RightBitShift = ALX_NULL;
	me->hadc.Init.Oversample.TriggeredMode = ALX_NULL;

	// ADC Channel
	for (uint32_t buffPos = 0; buffPos < numOfCh; buffPos++)
	{
		me->chadc[buffPos].Channel = AlxAdc_GetCh(me, *(chArr + buffPos));
		me->chadc[buffPos].Rank = ADC_RANK_CHANNEL_NUMBER;
		me->ch[buffPos] = *(chArr + buffPos);
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


	//------------------------------------------------------------------------------
	// STM32L4
	//------------------------------------------------------------------------------
	#if defined(ALX_STM32L4)

	// ADC Common
	me->hadc.Instance = adc;
	me->hadc.Init.ClockPrescaler = AlxAdc_GetClkPrescaler(me);
	me->hadc.Init.Resolution = ALX_ADC_RESOLUTION;
	me->hadc.Init.DataAlign = ADC_DATAALIGN_RIGHT;
	me->hadc.Init.ScanConvMode = ADC_SCAN_ENABLE;
	me->hadc.Init.EOCSelection = ADC_EOC_SEQ_CONV;
	me->hadc.Init.LowPowerAutoWait = DISABLE;
	me->hadc.Init.ContinuousConvMode = ENABLE;
	me->hadc.Init.NbrOfConversion = numOfCh;
	me->hadc.Init.DiscontinuousConvMode = DISABLE;
	me->hadc.Init.NbrOfDiscConversion = ALX_NULL;
	me->hadc.Init.ExternalTrigConv = ADC_SOFTWARE_START;
	me->hadc.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
	me->hadc.Init.DMAContinuousRequests = ENABLE;
	me->hadc.Init.Overrun = ADC_OVR_DATA_PRESERVED;
	me->hadc.Init.OversamplingMode = DISABLE;
	me->hadc.Init.Oversampling.Ratio = ALX_NULL;
	me->hadc.Init.Oversampling.RightBitShift = ALX_NULL;
	me->hadc.Init.Oversampling.TriggeredMode = ALX_NULL;
	me->hadc.Init.Oversampling.OversamplingStopReset = ALX_NULL;
	#if defined(ADC_CFGR_DFSDMCFG) && defined(DFSDM1_Channel0)
	me->hadc.Init.DFSDMConfig = ADC_DFSDM_MODE_DISABLE;
	#endif

	// ADC Channel
	for (uint32_t buffPos = 0; buffPos < numOfCh; buffPos++)
	{
		me->chadc[buffPos].Channel = AlxAdc_GetCh(me, *(chArr + buffPos));
		me->chadc[buffPos].Rank = AlxAdc_GetRank(buffPos);
		me->chadc[buffPos].SamplingTime = samplingTime;
		me->chadc[buffPos].SingleDiff = ADC_SINGLE_ENDED;
		me->chadc[buffPos].OffsetNumber = ADC_OFFSET_NONE;
		me->chadc[buffPos].Offset = 0;
		me->ch[buffPos] = *(chArr + buffPos);
	}

	// DMA
	if (me->hadc.Instance == ADC1)
	{
		me->dma = DMA1;
		me->hdma.Instance = DMA1_Channel1;
		#if defined(DMAMUX1)
		me->hdma.Init.Request = DMA_REQUEST_ADC1;
		#else
		me->hdma.Init.Request = DMA_REQUEST_0;
		#endif
	}
	me->hdma.Init.Direction = DMA_PERIPH_TO_MEMORY;
	me->hdma.Init.PeriphInc = DMA_PINC_DISABLE;
	me->hdma.Init.MemInc = DMA_MINC_ENABLE;
	me->hdma.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
	me->hdma.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
	me->hdma.Init.Mode = DMA_CIRCULAR;
	me->hdma.Init.Priority = DMA_PRIORITY_LOW;
	#endif


	//------------------------------------------------------------------------------
	// STM32U5
	//------------------------------------------------------------------------------
	#if defined(ALX_STM32U5)

	// ADC Common
	me->hadc.Instance = adc;
	me->hadc.Init.ClockPrescaler = AlxAdc_GetClkPrescaler(me);
	me->hadc.Init.Resolution = ALX_ADC_RESOLUTION;
	me->hadc.Init.GainCompensation = 0;
	me->hadc.Init.ScanConvMode = ADC_SCAN_ENABLE;
	me->hadc.Init.DataAlign = ADC_DATAALIGN_RIGHT;
	me->hadc.Init.EOCSelection = ADC_EOC_SEQ_CONV;
	me->hadc.Init.LowPowerAutoWait = DISABLE;
	me->hadc.Init.LowPowerAutoPowerOff = ADC_LOW_POWER_NONE;
	me->hadc.Init.ContinuousConvMode = ENABLE;
	me->hadc.Init.NbrOfConversion = numOfCh;
	me->hadc.Init.DiscontinuousConvMode = DISABLE;
	me->hadc.Init.NbrOfDiscConversion = ALX_NULL;
	me->hadc.Init.ExternalTrigConv = ADC_SOFTWARE_START;
	me->hadc.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
	me->hadc.Init.ConversionDataManagement = ADC_CONVERSIONDATA_DMA_CIRCULAR;
	me->hadc.Init.DMAContinuousRequests = ENABLE;
	me->hadc.Init.Overrun = ADC_OVR_DATA_PRESERVED;
	me->hadc.Init.SamplingTimeCommon1 = ALX_NULL;	// TV: TODO, seems like only relevant for ADC4
	me->hadc.Init.SamplingTimeCommon2 = ALX_NULL;	// TV: TODO, seems like only relevant for ADC4
	me->hadc.Init.LeftBitShift = ADC_LEFTBITSHIFT_NONE;
	me->hadc.Init.OversamplingMode = DISABLE;
	me->hadc.Init.Oversampling.Ratio = ALX_NULL;
	me->hadc.Init.Oversampling.RightBitShift = ALX_NULL;
	me->hadc.Init.Oversampling.TriggeredMode = ALX_NULL;
	me->hadc.Init.Oversampling.OversamplingStopReset = ALX_NULL;
	me->hadc.Init.TriggerFrequencyMode = ADC_TRIGGER_FREQ_HIGH;	// TV: TODO, don't understand
	me->hadc.Init.VrefProtection = ADC_VREF_PPROT_NONE;

	// ADC Channel
	for (uint32_t buffPos = 0; buffPos < numOfCh; buffPos++)
	{
		me->chadc[buffPos].Channel = AlxAdc_GetCh(me, *(chArr + buffPos));
		me->chadc[buffPos].Rank = AlxAdc_GetRank(buffPos);
		me->chadc[buffPos].SamplingTime = samplingTime;
		me->chadc[buffPos].SingleDiff = ADC_SINGLE_ENDED;
		me->chadc[buffPos].OffsetNumber = ADC_OFFSET_NONE;
		me->chadc[buffPos].Offset = 0;
		me->chadc[buffPos].OffsetRightShift = DISABLE;			// TV: TODO, don't understand
		me->chadc[buffPos].OffsetSignedSaturation = DISABLE;	// TV: TODO, don't understand
		me->chadc[buffPos].OffsetSaturation = DISABLE;			// TV: TODO, don't understand
		me->chadc[buffPos].OffsetSign = ALX_NULL;
		me->ch[buffPos] = *(chArr + buffPos);
	}

	// DMA
	if (me->hadc.Instance == ADC1)
	{
		me->dma = GPDMA1;
		me->hdma.Instance = GPDMA1_Channel0;
		me->ncdma.Init.Request = GPDMA1_REQUEST_ADC1;
	}
	me->hdma.InitLinkedList.Priority = DMA_LOW_PRIORITY_LOW_WEIGHT;
	me->hdma.InitLinkedList.LinkStepMode = DMA_LSM_FULL_EXECUTION;
	me->hdma.InitLinkedList.LinkAllocatedPort = DMA_LINK_ALLOCATED_PORT0;
	me->hdma.InitLinkedList.TransferEventMode = DMA_TCEM_LAST_LL_ITEM_TRANSFER;
	me->hdma.InitLinkedList.LinkedListMode = DMA_LINKEDLIST_CIRCULAR;

	me->ncdma.NodeType = DMA_GPDMA_LINEAR_NODE;
	me->ncdma.Init.BlkHWRequest = DMA_BREQ_SINGLE_BURST;
	me->ncdma.Init.Direction = DMA_PERIPH_TO_MEMORY;
	me->ncdma.Init.SrcInc = DMA_SINC_FIXED;
	me->ncdma.Init.DestInc = DMA_DINC_INCREMENTED;
	me->ncdma.Init.SrcDataWidth = DMA_SRC_DATAWIDTH_WORD;
	me->ncdma.Init.DestDataWidth = DMA_DEST_DATAWIDTH_WORD;
	me->ncdma.Init.SrcBurstLength = 1;
	me->ncdma.Init.DestBurstLength = 1;
	me->ncdma.Init.TransferAllocatedPort = DMA_SRC_ALLOCATED_PORT0|DMA_DEST_ALLOCATED_PORT0;
	me->ncdma.Init.TransferEventMode = DMA_TCEM_BLOCK_TRANSFER;
	me->ncdma.Init.Mode = DMA_NORMAL;
	me->ncdma.DataHandlingConfig.DataExchange = DMA_EXCHANGE_NONE;
	me->ncdma.DataHandlingConfig.DataAlignment = DMA_DATA_RIGHTALIGN_ZEROPADDED;
	me->ncdma.TriggerConfig.TriggerMode = ALX_NULL;
	me->ncdma.TriggerConfig.TriggerPolarity = DMA_TRIG_POLARITY_MASKED;
	me->ncdma.TriggerConfig.TriggerSelection = ALX_NULL;
	memset(&me->ncdma.RepeatBlockConfig, 0, sizeof(me->ncdma.RepeatBlockConfig));
	me->ncdma.SrcAddress = 0;
	me->ncdma.DstAddress = 0;
	me->ncdma.DataSize = 0;
	#endif


	//------------------------------------------------------------------------------
	// Info
	//------------------------------------------------------------------------------
	me->wasCtorCalled = true;
	me->isInit = false;
}


//******************************************************************************
// Functions
//******************************************************************************

/**
  * @brief
  * @param[in,out]	me
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxAdc_Init(AlxAdc* me)
{
	// Assert
	ALX_ADC_ASSERT(me->wasCtorCalled == true);
	ALX_ADC_ASSERT(me->isInit == false);

	// Init GPIO
	for (uint32_t i = 0; i < me->numOfIoPins; i++)
	{
		AlxIoPin_Init((*(me->ioPinArr + i)));
	}

	// Release ADC & DMA periphery reset
	AlxAdc_Periph_Adc_ReleaseReset(me);
	AlxAdc_Periph_Dma_ReleaseReset(me);

	// Enable ADC & DMA periphery clock
	AlxAdc_Periph_Adc_EnableClk(me);
	AlxAdc_Periph_Dma_EnableClk(me);

	// Init PWR
	#if defined(ALX_STM32U5)
	HAL_PWREx_EnableVddA();
	#endif

	// Init ADC clock
	#if defined(ALX_STM32F1)
	if(HAL_RCCEx_PeriphCLKConfig(&me->iclk) != HAL_OK) { ALX_ADC_TRACE_WRN("Err"); return Alx_Err; };
	#endif

	// Init ADC
	if(HAL_ADC_Init(&me->hadc) != HAL_OK) { ALX_ADC_TRACE_WRN("Err"); return Alx_Err; }

	// Init ADC channels
	for (uint32_t i = 0; i < me->numOfCh; i++)
	{
		if (HAL_ADC_ConfigChannel(&me->hadc, &me->chadc[i]) != HAL_OK) { ALX_ADC_TRACE_WRN("Err"); return Alx_Err; }
	}

	// Calibrate ADC
	#if defined(ALX_STM32F0) || defined(ALX_STM32F1)
	if(HAL_ADCEx_Calibration_Start(&me->hadc) != HAL_OK) { ALX_ADC_TRACE_WRN("Err"); return Alx_Err; }
	#endif
	#if defined(ALX_STM32G4) || defined(ALX_STM32L0) || defined(ALX_STM32L4)
	if(HAL_ADCEx_Calibration_Start(&me->hadc, ADC_SINGLE_ENDED) != HAL_OK) { ALX_ADC_TRACE_WRN("Err"); return Alx_Err; }
	#endif
	#if defined(ALX_STM32U5)
	if(HAL_ADCEx_Calibration_Start(&me->hadc, ADC_CALIB_OFFSET, ADC_SINGLE_ENDED) != HAL_OK) { ALX_ADC_TRACE_WRN("Err"); return Alx_Err; }
	#endif

	// Init DMA
	#if defined(ALX_STM32F0) || defined(ALX_STM32F1) || defined(ALX_STM32F4) || defined(ALX_STM32F7) || defined(ALX_STM32G4) || defined(ALX_STM32L0) || defined(ALX_STM32L4)
	if(HAL_DMA_Init(&me->hdma) != HAL_OK) { ALX_ADC_TRACE_WRN("Err"); return Alx_Err; }
	#endif
	#if defined(ALX_STM32U5)
	if(HAL_DMAEx_List_BuildNode(&me->ncdma, &me->ndma) != HAL_OK) { ALX_ADC_TRACE_WRN("Err"); return Alx_Err; }
	if(HAL_DMAEx_List_InsertNode(&me->qdma, NULL, &me->ndma) != HAL_OK) { ALX_ADC_TRACE_WRN("Err"); return Alx_Err; }
	if(HAL_DMAEx_List_SetCircularMode(&me->qdma) != HAL_OK) { ALX_ADC_TRACE_WRN("Err"); return Alx_Err; }
	if(HAL_DMAEx_List_Init(&me->hdma) != HAL_OK) { ALX_ADC_TRACE_WRN("Err"); return Alx_Err; }
	if(HAL_DMAEx_List_LinkQ(&me->hdma, &me->qdma) != HAL_OK) { ALX_ADC_TRACE_WRN("Err"); return Alx_Err; }
	if(HAL_DMA_ConfigChannelAttributes(&me->hdma, DMA_CHANNEL_PRIV) != HAL_OK) { ALX_ADC_TRACE_WRN("Err"); return Alx_Err; }
	#endif

	// Link ADC & DMA
	__HAL_LINKDMA(&me->hadc, DMA_Handle, me->hdma);

	// Start ADC with DMA
	if(HAL_ADC_Start_DMA(&me->hadc, me->buff, me->numOfCh) != HAL_OK) { ALX_ADC_TRACE_WRN("Err"); return Alx_Err; }

	// Set isInit
	me->isInit = true;

	// Return
	return Alx_Ok;
}

/**
  * @brief
  * @param[in,out]	me
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxAdc_DeInit(AlxAdc* me)// JS: if using ADC1 with combination of ADC2 or. ADC3 there is bug with disable Dma for all ADCs if only deinit one of the ADCs.
{
	// Assert
	ALX_ADC_ASSERT(me->wasCtorCalled == true);
	ALX_ADC_ASSERT(me->isInit == true);

	// DeInit DMA
	if(HAL_DMA_DeInit(&me->hdma) != HAL_OK) { ALX_ADC_TRACE_WRN("Err"); return Alx_Err; }

	// DeInit ADC
	if(HAL_ADC_DeInit(&me->hadc) != HAL_OK) { ALX_ADC_TRACE_WRN("Err"); return Alx_Err; }

	// Force ADC & DMA periphery reset
	AlxAdc_Periph_Adc_ForceReset(me);
	AlxAdc_Periph_Dma_ForceReset(me);

	// Disable ADC & DMA periphery clock
	AlxAdc_Periph_Adc_DisableClk(me);
	AlxAdc_Periph_Dma_DisableClk(me);

	// DeInit GPIO
	for (uint32_t i = 0; i < me->numOfIoPins; i++)
	{
		AlxIoPin_DeInit((*(me->ioPinArr + i)));
	}

	// Clear isInit
	me->isInit = false;

	// Return
	return Alx_Ok;
}

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		ch
  * @return
  */
float AlxAdc_GetVoltage_V(AlxAdc* me, Alx_Ch ch)
{
	// Assert
	ALX_ADC_ASSERT(me->wasCtorCalled == true);
	ALX_ADC_ASSERT(me->isInit == true);

	// Get reference voltage
	uint32_t vref_mV = AlxAdc_GetVref_mV(me);

	// Get channel voltage
	for (uint32_t i = 0; i < me->numOfCh; i++)
	{
		if (ch == Alx_Ch_McuStm32_Vref)
		{
			// Get
			float vref_V = (float)vref_mV / 1000.f;

			// Return
			return vref_V;
		}
		else if (me->ch[i] == ch)
		{
			// Get
			#if defined(ALX_STM32F0) || defined(ALX_STM32F1) || defined(ALX_STM32F4) || defined(ALX_STM32F7) || defined(ALX_STM32G4) || defined(ALX_STM32L0) || defined(ALX_STM32L4)
			uint32_t voltage_mV = __LL_ADC_CALC_DATA_TO_VOLTAGE(vref_mV, me->buff[i], ALX_ADC_RESOLUTION);
			#endif
			#if defined(ALX_STM32U5)
			uint32_t voltage_mV = __LL_ADC_CALC_DATA_TO_VOLTAGE(me->hadc.Instance, vref_mV, me->buff[i], ALX_ADC_RESOLUTION);
			#endif

			// Prepare
			float voltage_V = (float)voltage_mV / 1000.f;

			// Return
			return voltage_V;
		}
	}

	// Assert
	ALX_ADC_ASSERT(false);	// We should not get here
	return ALX_NULL;
}

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		ch
  * @return
  */
uint32_t AlxAdc_GetVoltage_mV(AlxAdc* me, Alx_Ch ch)
{
	// Assert
	ALX_ADC_ASSERT(me->wasCtorCalled == true);
	ALX_ADC_ASSERT(me->isInit == true);

	// Get reference voltage
	uint32_t vref_mV = AlxAdc_GetVref_mV(me);

	// Get channel voltage
	for (uint32_t i = 0; i < me->numOfCh; i++)
	{
		if (ch == Alx_Ch_McuStm32_Vref)
		{
			return vref_mV;
		}
		else if (me->ch[i] == ch)
		{
			// Get
			#if defined(ALX_STM32F0) || defined(ALX_STM32F1) || defined(ALX_STM32F4) || defined(ALX_STM32F7) || defined(ALX_STM32G4) || defined(ALX_STM32L0) || defined(ALX_STM32L4)
			uint32_t voltage_mV = __LL_ADC_CALC_DATA_TO_VOLTAGE(vref_mV, me->buff[i], ALX_ADC_RESOLUTION);
			#endif
			#if defined(ALX_STM32U5)
			uint32_t voltage_mV = __LL_ADC_CALC_DATA_TO_VOLTAGE(me->hadc.Instance, vref_mV, me->buff[i], ALX_ADC_RESOLUTION);
			#endif

			// Return
			return voltage_mV;
		}
	}

	// Assert
	ALX_ADC_ASSERT(false);	// We should not get here
	return ALX_NULL;
}

/**
  * @brief
  * @param[in,out]	me
  * @return
  */
float AlxAdc_TempSens_GetTemp_degC(AlxAdc* me)
{
	// Assert
	ALX_ADC_ASSERT(me->wasCtorCalled == true);
	ALX_ADC_ASSERT(me->isInit == true);

	// Get reference voltage
	uint32_t vref_mV = AlxAdc_GetVref_mV(me);

	// Get temperature
	for (uint32_t i = 0; i < me->numOfCh; i++)
	{
		if (me->ch[i] == Alx_Ch_McuStm32_TempSens)
		{
			// Get
			#if defined(ALX_STM32F1) || defined(STM32F469) || defined(STM32F479xx) || defined(STM32F429xx) || defined(STM32F439xx)
			int32_t temp_degC = 0;	// Not working, problems
			ALX_ADC_ASSERT(false);
			#elif defined(ALX_STM32F0) || defined(ALX_STM32F4) || defined(ALX_STM32F7) || defined(ALX_STM32G4) || defined(ALX_STM32L0) || defined(ALX_STM32L4)
			int32_t temp_degC = __LL_ADC_CALC_TEMPERATURE(vref_mV, me->buff[i], ALX_ADC_RESOLUTION);
			#elif defined(ALX_STM32U5)
			int32_t temp_degC = __LL_ADC_CALC_TEMPERATURE(me->hadc.Instance, vref_mV, me->buff[i], ALX_ADC_RESOLUTION);
			#endif

			// Return
			return (float)temp_degC;
		}
	}

	// Assert
	ALX_ADC_ASSERT(false);	// We should not get here
	return ALX_NULL;
}


//******************************************************************************
// Private Functions
//******************************************************************************


//------------------------------------------------------------------------------
// Specific
//------------------------------------------------------------------------------
static uint32_t AlxAdc_GetVref_mV(AlxAdc* me)
{
	// Get Vref
	if (me->isVrefInt_V)
	{
		for (uint32_t i = 0; i < me->numOfCh; i++)
		{
			if (me->ch[i] == Alx_Ch_McuStm32_Vref)
			{
				// Prepare
				#if defined(ALX_STM32F0) || defined(ALX_STM32F4) || defined(ALX_STM32F7) || defined(ALX_STM32G4) || defined(ALX_STM32L0) || defined(ALX_STM32L4)
				uint32_t vref_mV = __LL_ADC_CALC_VREFANALOG_VOLTAGE(me->buff[i], ALX_ADC_RESOLUTION);
				#endif
				#if defined(ALX_STM32U5)
				uint32_t vref_mV = __LL_ADC_CALC_VREFANALOG_VOLTAGE(me->hadc.Instance, me->buff[i], ALX_ADC_RESOLUTION);
				#endif

				// Return
				return vref_mV;
			}
		}
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

	// Assert
	ALX_ADC_ASSERT(false);	// We should not get here
	return ALX_NULL;
}
static uint32_t AlxAdc_GetClkPrescaler(AlxAdc* me)
{
	//------------------------------------------------------------------------------
	// STM32F0
	//------------------------------------------------------------------------------
	#if defined(ALX_STM32F0)
	if(me->adcClk == AlxAdc_Clk_McuStm32F0_AdcClk_12MHz_Pclk1Apb1_48MHz)		return ADC_CLOCK_SYNC_PCLK_DIV4;
	#endif


	//------------------------------------------------------------------------------
	// STM32F1
	//------------------------------------------------------------------------------
	#if defined(ALX_STM32F1)
	if(me->adcClk == AlxAdc_Clk_McuStm32F1_AdcClk_4MHz_Pclk2Apb2_8MHz)			return RCC_ADCPCLK2_DIV2;
	#endif


	//------------------------------------------------------------------------------
	// STM32F4
	//------------------------------------------------------------------------------
	#if defined(ALX_STM32F4)
	if(me->adcClk == AlxAdc_Clk_McuStm32F4_AdcClk_11MHz25_Pclk2Apb2_90MHz)		return ADC_CLOCK_SYNC_PCLK_DIV8;
	if(me->adcClk == AlxAdc_Clk_McuStm32F4_AdcClk_22MHz5_Pclk2Apb2_90MHz)		return ADC_CLOCK_SYNC_PCLK_DIV4;
	#endif


	//------------------------------------------------------------------------------
	// STM32F7
	//------------------------------------------------------------------------------
	#if defined(ALX_STM32F7)
	if(me->adcClk == AlxAdc_Clk_McuStm32F7_AdcClk_13MHz5_Pclk2Apb2_108MHz)		return ADC_CLOCK_SYNC_PCLK_DIV8;
	if(me->adcClk == AlxAdc_Clk_McuStm32F7_AdcClk_27MHz_Pclk2Apb2_108MHz)		return ADC_CLOCK_SYNC_PCLK_DIV4;
	#endif


	//------------------------------------------------------------------------------
	// STM32G4
	//------------------------------------------------------------------------------
	#if defined(ALX_STM32G4)
	if(me->adcClk == AlxAdc_Clk_McuStm32G4_AdcClk_42MHz5_Pclk2Apb2_170MHz)		return ADC_CLOCK_SYNC_PCLK_DIV4;
	#endif


	//------------------------------------------------------------------------------
	// STM32L0
	//------------------------------------------------------------------------------
	#if defined(ALX_STM32L0)
	if(me->adcClk == AlxAdc_Clk_McuStm32L0_AdcClk_525kHz_Pclk2Apb2_2MHz1)		return ADC_CLOCK_SYNC_PCLK_DIV4;
	if(me->adcClk == AlxAdc_Clk_McuStm32L0_AdcClk_8MHz_Pclk2Apb2_32MHz)			return ADC_CLOCK_SYNC_PCLK_DIV4;
	#endif


	//------------------------------------------------------------------------------
	// STM32L4
	//------------------------------------------------------------------------------
	#if defined(ALX_STM32L4)
	if(me->adcClk == AlxAdc_Clk_McuStm32L4_AdcClk_20MHz_Sysclk_80MHz)			return ADC_CLOCK_SYNC_PCLK_DIV4;
	if(me->adcClk == AlxAdc_Clk_McuStm32L4_AdcClk_30MHz_Sysclk_120MHz)			return ADC_CLOCK_SYNC_PCLK_DIV4;
	#endif


	//------------------------------------------------------------------------------
	// STM32U5
	//------------------------------------------------------------------------------
	#if defined(ALX_STM32U5)
	if(me->adcClk == AlxAdc_Clk_McuStm32U5_AdcClk_16MHz_Hclk_16MHz)				return ADC_CLOCK_ASYNC_DIV1;
	if(me->adcClk == AlxAdc_Clk_McuStm32U5_AdcClk_20MHz_Hclk_160MHz)			return ADC_CLOCK_ASYNC_DIV8;
	#endif


	//------------------------------------------------------------------------------
	// Assert
	//------------------------------------------------------------------------------
	ALX_ADC_ASSERT(false);	// We should not get here
	return ALX_NULL;
}
static uint32_t AlxAdc_GetCh(AlxAdc* me, Alx_Ch ch)
{
	//------------------------------------------------------------------------------
	// STM32F0
	//------------------------------------------------------------------------------
	#if defined(ALX_STM32F0)
	if(ch == Alx_Ch_0)					return ADC_CHANNEL_0;
	if(ch == Alx_Ch_1)					return ADC_CHANNEL_1;
	if(ch == Alx_Ch_2)					return ADC_CHANNEL_2;
	if(ch == Alx_Ch_3)					return ADC_CHANNEL_3;
	if(ch == Alx_Ch_4)					return ADC_CHANNEL_4;
	if(ch == Alx_Ch_5)					return ADC_CHANNEL_5;
	if(ch == Alx_Ch_6)					return ADC_CHANNEL_6;
	if(ch == Alx_Ch_7)					return ADC_CHANNEL_7;
	if(ch == Alx_Ch_8)					return ADC_CHANNEL_8;
	if(ch == Alx_Ch_9)					return ADC_CHANNEL_9;
	if(ch == Alx_Ch_10)					return ADC_CHANNEL_10;
	if(ch == Alx_Ch_11)					return ADC_CHANNEL_11;
	if(ch == Alx_Ch_12)					return ADC_CHANNEL_12;
	if(ch == Alx_Ch_13)					return ADC_CHANNEL_13;
	if(ch == Alx_Ch_14)					return ADC_CHANNEL_14;
	if(ch == Alx_Ch_15)					return ADC_CHANNEL_15;
	if(ch == Alx_Ch_McuStm32_TempSens)	return ADC_CHANNEL_TEMPSENSOR;	// Ch16
	if(ch == Alx_Ch_McuStm32_Vref)		return ADC_CHANNEL_VREFINT;		// Ch17
	#if !defined(STM32F030x6) && !defined(STM32F030x8) && !defined(STM32F070x6) && !defined(STM32F070xB) && !defined(STM32F030xC)
	if(ch == Alx_Ch_McuStm32_Vbat)		return ADC_CHANNEL_VBAT;		// Ch18
	#endif
	#endif


	//------------------------------------------------------------------------------
	// STM32F1
	//------------------------------------------------------------------------------
	#if defined(ALX_STM32F1)
	if(ch == Alx_Ch_0)					return ADC_CHANNEL_0;
	if(ch == Alx_Ch_1)					return ADC_CHANNEL_1;
	if(ch == Alx_Ch_2)					return ADC_CHANNEL_2;
	if(ch == Alx_Ch_3)					return ADC_CHANNEL_3;
	if(ch == Alx_Ch_4)					return ADC_CHANNEL_4;
	if(ch == Alx_Ch_5)					return ADC_CHANNEL_5;
	if(ch == Alx_Ch_6)					return ADC_CHANNEL_6;
	if(ch == Alx_Ch_7)					return ADC_CHANNEL_7;
	if(ch == Alx_Ch_8)					return ADC_CHANNEL_8;
	if(ch == Alx_Ch_9)					return ADC_CHANNEL_9;
	if(ch == Alx_Ch_10)					return ADC_CHANNEL_10;
	if(ch == Alx_Ch_11)					return ADC_CHANNEL_11;
	if(ch == Alx_Ch_12)					return ADC_CHANNEL_12;
	if(ch == Alx_Ch_13)					return ADC_CHANNEL_13;
	if(ch == Alx_Ch_14)					return ADC_CHANNEL_14;
	if(ch == Alx_Ch_15)					return ADC_CHANNEL_15;
	if(ch == Alx_Ch_McuStm32_TempSens)	return ADC_CHANNEL_TEMPSENSOR;	// Ch16
	if(ch == Alx_Ch_McuStm32_Vref)		return ADC_CHANNEL_VREFINT;		// Ch17
	#endif


	//------------------------------------------------------------------------------
	// STM32F4
	//------------------------------------------------------------------------------
	#if defined(ALX_STM32F4)
	if(ch == Alx_Ch_0)					return ADC_CHANNEL_0;
	if(ch == Alx_Ch_1)					return ADC_CHANNEL_1;
	if(ch == Alx_Ch_2)					return ADC_CHANNEL_2;
	if(ch == Alx_Ch_3)					return ADC_CHANNEL_3;
	if(ch == Alx_Ch_4)					return ADC_CHANNEL_4;
	if(ch == Alx_Ch_5)					return ADC_CHANNEL_5;
	if(ch == Alx_Ch_6)					return ADC_CHANNEL_6;
	if(ch == Alx_Ch_7)					return ADC_CHANNEL_7;
	if(ch == Alx_Ch_8)					return ADC_CHANNEL_8;
	if(ch == Alx_Ch_9)					return ADC_CHANNEL_9;
	if(ch == Alx_Ch_10)					return ADC_CHANNEL_10;
	if(ch == Alx_Ch_11)					return ADC_CHANNEL_11;
	if(ch == Alx_Ch_12)					return ADC_CHANNEL_12;
	if(ch == Alx_Ch_13)					return ADC_CHANNEL_13;
	if(ch == Alx_Ch_14)					return ADC_CHANNEL_14;
	if(ch == Alx_Ch_15)					return ADC_CHANNEL_15;
	if(ch == Alx_Ch_McuStm32_TempSens)	return ADC_CHANNEL_TEMPSENSOR;	// Ch16 or Ch18
	if(ch == Alx_Ch_McuStm32_Vref)		return ADC_CHANNEL_VREFINT;		// Ch17
	if(ch == Alx_Ch_McuStm32_Vbat)		return ADC_CHANNEL_VBAT;		// Ch18
	#endif


	//------------------------------------------------------------------------------
	// STM32F7
	//------------------------------------------------------------------------------
	#if defined(ALX_STM32F7)
	if(ch == Alx_Ch_0)					return ADC_CHANNEL_0;
	if(ch == Alx_Ch_1)					return ADC_CHANNEL_1;
	if(ch == Alx_Ch_2)					return ADC_CHANNEL_2;
	if(ch == Alx_Ch_3)					return ADC_CHANNEL_3;
	if(ch == Alx_Ch_4)					return ADC_CHANNEL_4;
	if(ch == Alx_Ch_5)					return ADC_CHANNEL_5;
	if(ch == Alx_Ch_6)					return ADC_CHANNEL_6;
	if(ch == Alx_Ch_7)					return ADC_CHANNEL_7;
	if(ch == Alx_Ch_8)					return ADC_CHANNEL_8;
	if(ch == Alx_Ch_9)					return ADC_CHANNEL_9;
	if(ch == Alx_Ch_10)					return ADC_CHANNEL_10;
	if(ch == Alx_Ch_11)					return ADC_CHANNEL_11;
	if(ch == Alx_Ch_12)					return ADC_CHANNEL_12;
	if(ch == Alx_Ch_13)					return ADC_CHANNEL_13;
	if(ch == Alx_Ch_14)					return ADC_CHANNEL_14;
	if(ch == Alx_Ch_15)					return ADC_CHANNEL_15;
	if(ch == Alx_Ch_McuStm32_TempSens)	return ADC_CHANNEL_TEMPSENSOR;	// Ch18
	if(ch == Alx_Ch_McuStm32_Vref)		return ADC_CHANNEL_VREFINT;		// Ch17
	if(ch == Alx_Ch_McuStm32_Vbat)		return ADC_CHANNEL_VBAT;		// Ch18
	#endif

	//------------------------------------------------------------------------------
	// STM32G4
	//------------------------------------------------------------------------------
	#if defined(ALX_STM32G4)
	if(ch == Alx_Ch_0)				return ADC_CHANNEL_0;
	if(ch == Alx_Ch_1)				return ADC_CHANNEL_1;
	if(ch == Alx_Ch_2)				return ADC_CHANNEL_2;
	if(ch == Alx_Ch_3)				return ADC_CHANNEL_3;
	if(ch == Alx_Ch_4)				return ADC_CHANNEL_4;
	if(ch == Alx_Ch_5)				return ADC_CHANNEL_5;
	if(ch == Alx_Ch_6)				return ADC_CHANNEL_6;
	if(ch == Alx_Ch_7)				return ADC_CHANNEL_7;
	if(ch == Alx_Ch_8)				return ADC_CHANNEL_8;
	if(ch == Alx_Ch_9)				return ADC_CHANNEL_9;
	if(ch == Alx_Ch_10)				return ADC_CHANNEL_10;
	if(ch == Alx_Ch_11)				return ADC_CHANNEL_11;
	if(ch == Alx_Ch_12)				return ADC_CHANNEL_12;
	if(ch == Alx_Ch_13)				return ADC_CHANNEL_13;
	if(ch == Alx_Ch_14)				return ADC_CHANNEL_14;
	if(ch == Alx_Ch_15)				return ADC_CHANNEL_15;
	if(ch == Alx_Ch_McuStm32_TempSens)
	{
		if (me->hadc.Instance == ADC1)
		{
			return ADC_CHANNEL_TEMPSENSOR_ADC1;	// Ch16
		}
		#if !defined(STM32G431xx)
		else if (me->hadc.Instance == ADC5)
		{
			return ADC_CHANNEL_TEMPSENSOR_ADC5;	// Ch4
		}
		#endif
		else
		{
			ALX_ADC_ASSERT(false);	// We should not get here
		}
	}
	if(ch == Alx_Ch_McuStm32_Vbat)	return ADC_CHANNEL_VBAT;	// Ch17
	if(ch == Alx_Ch_McuStm32_Vref)	return ADC_CHANNEL_VREFINT;	// Ch18
	#endif


	//------------------------------------------------------------------------------
	// STM32L0
	//------------------------------------------------------------------------------
	#if defined(ALX_STM32L0)
	if(ch == Alx_Ch_0)					return ADC_CHANNEL_0;
	if(ch == Alx_Ch_1)					return ADC_CHANNEL_1;
	if(ch == Alx_Ch_2)					return ADC_CHANNEL_2;
	if(ch == Alx_Ch_3)					return ADC_CHANNEL_3;
	if(ch == Alx_Ch_4)					return ADC_CHANNEL_4;
	if(ch == Alx_Ch_5)					return ADC_CHANNEL_5;
	if(ch == Alx_Ch_6)					return ADC_CHANNEL_6;
	if(ch == Alx_Ch_7)					return ADC_CHANNEL_7;
	if(ch == Alx_Ch_8)					return ADC_CHANNEL_8;
	if(ch == Alx_Ch_9)					return ADC_CHANNEL_9;
	if(ch == Alx_Ch_10)					return ADC_CHANNEL_10;
	if(ch == Alx_Ch_11)					return ADC_CHANNEL_11;
	if(ch == Alx_Ch_12)					return ADC_CHANNEL_12;
	if(ch == Alx_Ch_13)					return ADC_CHANNEL_13;
	if(ch == Alx_Ch_14)					return ADC_CHANNEL_14;
	if(ch == Alx_Ch_15)					return ADC_CHANNEL_15;
	#if defined (STM32L053xx) || defined (STM32L063xx) || defined (STM32L073xx) || defined (STM32L083xx)
	if (ch == Alx_Ch_McuStm32_Vlcd)		return ADC_CHANNEL_VLCD;		// Ch16
	#endif
	if(ch == Alx_Ch_McuStm32_Vref)		return ADC_CHANNEL_VREFINT;		// Ch17
	if(ch == Alx_Ch_McuStm32_TempSens)	return ADC_CHANNEL_TEMPSENSOR;	// Ch18
	#endif


	//------------------------------------------------------------------------------
	// STM32L4
	//------------------------------------------------------------------------------
	#if defined(ALX_STM32L4)
	if(ch == Alx_Ch_1)					return ADC_CHANNEL_1;
	if(ch == Alx_Ch_2)					return ADC_CHANNEL_2;
	if(ch == Alx_Ch_3)					return ADC_CHANNEL_3;
	if(ch == Alx_Ch_4)					return ADC_CHANNEL_4;
	if(ch == Alx_Ch_5)					return ADC_CHANNEL_5;
	if(ch == Alx_Ch_6)					return ADC_CHANNEL_6;
	if(ch == Alx_Ch_7)					return ADC_CHANNEL_7;
	if(ch == Alx_Ch_8)					return ADC_CHANNEL_8;
	if(ch == Alx_Ch_9)					return ADC_CHANNEL_9;
	if(ch == Alx_Ch_10)					return ADC_CHANNEL_10;
	if(ch == Alx_Ch_11)					return ADC_CHANNEL_11;
	if(ch == Alx_Ch_12)					return ADC_CHANNEL_12;
	if(ch == Alx_Ch_13)					return ADC_CHANNEL_13;
	if(ch == Alx_Ch_14)					return ADC_CHANNEL_14;
	if(ch == Alx_Ch_15)					return ADC_CHANNEL_15;
	if(ch == Alx_Ch_16)					return ADC_CHANNEL_16;
	if(ch == Alx_Ch_McuStm32_Vref)		return ADC_CHANNEL_VREFINT;		// Ch0
	if(ch == Alx_Ch_McuStm32_TempSens)	return ADC_CHANNEL_TEMPSENSOR;	// Ch17
	if(ch == Alx_Ch_McuStm32_Vbat)		return ADC_CHANNEL_VBAT;		// Ch18
	#endif


	//------------------------------------------------------------------------------
	// STM32U5
	//------------------------------------------------------------------------------
	#if defined(ALX_STM32U5)
	if(ch == Alx_Ch_1)					return ADC_CHANNEL_1;
	if(ch == Alx_Ch_2)					return ADC_CHANNEL_2;
	if(ch == Alx_Ch_3)					return ADC_CHANNEL_3;
	if(ch == Alx_Ch_4)					return ADC_CHANNEL_4;
	if(ch == Alx_Ch_5)					return ADC_CHANNEL_5;
	if(ch == Alx_Ch_6)					return ADC_CHANNEL_6;
	if(ch == Alx_Ch_7)					return ADC_CHANNEL_7;
	if(ch == Alx_Ch_8)					return ADC_CHANNEL_8;
	if(ch == Alx_Ch_9)					return ADC_CHANNEL_9;
	if(ch == Alx_Ch_10)					return ADC_CHANNEL_10;
	if(ch == Alx_Ch_11)					return ADC_CHANNEL_11;
	if(ch == Alx_Ch_12)					return ADC_CHANNEL_12;
	if(ch == Alx_Ch_13)					return ADC_CHANNEL_13;
	if(ch == Alx_Ch_14)					return ADC_CHANNEL_14;
	if(ch == Alx_Ch_15)					return ADC_CHANNEL_15;
	if(ch == Alx_Ch_16)					return ADC_CHANNEL_16;
	if(ch == Alx_Ch_McuStm32_Vref)		return ADC_CHANNEL_VREFINT;		// Ch0
	if(ch == Alx_Ch_McuStm32_Vbat)		return ADC_CHANNEL_VBAT;		// Ch18
	if(ch == Alx_Ch_McuStm32_TempSens)	return ADC_CHANNEL_TEMPSENSOR;	// Ch19
	#endif


	//------------------------------------------------------------------------------
	// Assert
	//------------------------------------------------------------------------------
	ALX_ADC_ASSERT(false);	// We should not get here
	return ALX_NULL;
}
static uint32_t AlxAdc_GetRank(uint8_t buffPos)
{
	//------------------------------------------------------------------------------
	// STM32F0, STM32F1, STM32F4, STM32F7
	//------------------------------------------------------------------------------
	#if defined(ALX_STM32F0) || defined(ALX_STM32F1) || defined(ALX_STM32F4) || defined(ALX_STM32F7)
	ALX_ADC_ASSERT((0 <= buffPos) && (buffPos <= 15));
	return buffPos + 1;
	#endif


	//------------------------------------------------------------------------------
	// STM32G4, STM32L4, STM32U5
	//------------------------------------------------------------------------------
	#if defined(ALX_STM32G4) || defined(ALX_STM32L4) || defined(ALX_STM32U5)
	if(buffPos == 0) return ADC_REGULAR_RANK_1;
	if(buffPos == 1) return ADC_REGULAR_RANK_2;
	if(buffPos == 2) return ADC_REGULAR_RANK_3;
	if(buffPos == 3) return ADC_REGULAR_RANK_4;
	if(buffPos == 4) return ADC_REGULAR_RANK_5;
	if(buffPos == 5) return ADC_REGULAR_RANK_6;
	if(buffPos == 6) return ADC_REGULAR_RANK_7;
	if(buffPos == 7) return ADC_REGULAR_RANK_8;
	if(buffPos == 8) return ADC_REGULAR_RANK_9;
	if(buffPos == 9) return ADC_REGULAR_RANK_10;
	if(buffPos == 10) return ADC_REGULAR_RANK_11;
	if(buffPos == 11) return ADC_REGULAR_RANK_12;
	if(buffPos == 12) return ADC_REGULAR_RANK_13;
	if(buffPos == 13) return ADC_REGULAR_RANK_14;
	if(buffPos == 14) return ADC_REGULAR_RANK_15;
	if(buffPos == 15) return ADC_REGULAR_RANK_16;
	#endif


	//------------------------------------------------------------------------------
	// STM32L0
	//------------------------------------------------------------------------------
	#if defined(ALX_STM32L0)
	return ALX_NULL;
	#endif


	//------------------------------------------------------------------------------
	// Assert
	//------------------------------------------------------------------------------
	ALX_ADC_ASSERT(false);	// We should not get here
	return ALX_NULL;
}


//------------------------------------------------------------------------------
// General
//------------------------------------------------------------------------------
static bool AlxAdc_IsClkOk(AlxAdc* me)
{
	//------------------------------------------------------------------------------
	// STM32F0
	//------------------------------------------------------------------------------
	#if defined(ALX_STM32F0)
	if(me->adcClk == AlxAdc_Clk_McuStm32F0_AdcClk_12MHz_Pclk1Apb1_48MHz)
	{
		if(48000000 == AlxClk_GetClk_Hz(me->clk, AlxClk_Clk_McuStm32_Pclk1Apb1_Ctor))
			return true;
		else
			return false;
	}
	#endif


	//------------------------------------------------------------------------------
	// STM32F1
	//------------------------------------------------------------------------------
	#if defined(ALX_STM32F1)
	if(me->adcClk == AlxAdc_Clk_McuStm32F1_AdcClk_4MHz_Pclk2Apb2_8MHz)
	{
		if(8000000 == AlxClk_GetClk_Hz(me->clk, AlxClk_Clk_McuStm32_Pclk2Apb2_Ctor))
			return true;
		else
			return false;
	}
	#endif


	//------------------------------------------------------------------------------
	// STM32F4
	//------------------------------------------------------------------------------
	#if defined(ALX_STM32F4)
	if(me->adcClk == AlxAdc_Clk_McuStm32F4_AdcClk_22MHz5_Pclk2Apb2_90MHz)
	{
		if(90000000 == AlxClk_GetClk_Hz(me->clk, AlxClk_Clk_McuStm32_Pclk2Apb2_Ctor))
			return true;
		else
			return false;
	}
	if(me->adcClk == AlxAdc_Clk_McuStm32F4_AdcClk_11MHz25_Pclk2Apb2_90MHz)
	{
		if(90000000 == AlxClk_GetClk_Hz(me->clk, AlxClk_Clk_McuStm32_Pclk2Apb2_Ctor))
			return true;
		else
			return false;
	}
	#endif


	//------------------------------------------------------------------------------
	// STM32F7
	//------------------------------------------------------------------------------
	#if defined(ALX_STM32F7)
	if
	(
		(me->adcClk == AlxAdc_Clk_McuStm32F7_AdcClk_13MHz5_Pclk2Apb2_108MHz) ||
		(me->adcClk == AlxAdc_Clk_McuStm32F7_AdcClk_27MHz_Pclk2Apb2_108MHz)
	)
	{
		if(108000000 == AlxClk_GetClk_Hz(me->clk, AlxClk_Clk_McuStm32_Pclk2Apb2_Ctor))
			return true;
		else
			return false;
	}
	#endif


	//------------------------------------------------------------------------------
	// STM32G4
	//------------------------------------------------------------------------------
	#if defined(ALX_STM32G4)
	if(me->adcClk == AlxAdc_Clk_McuStm32G4_AdcClk_42MHz5_Pclk2Apb2_170MHz)
	{
		if(170000000 == AlxClk_GetClk_Hz(me->clk, AlxClk_Clk_McuStm32_Pclk2Apb2_Ctor))
			return true;
		else
			return false;
	}
	#endif


	//------------------------------------------------------------------------------
	// STM32L0
	//------------------------------------------------------------------------------
	#if defined(ALX_STM32L0)
	if(me->adcClk == AlxAdc_Clk_McuStm32L0_AdcClk_525kHz_Pclk2Apb2_2MHz1)
	{
		if(2097152 == AlxClk_GetClk_Hz(me->clk, AlxClk_Clk_McuStm32_Pclk2Apb2_Ctor))
			return true;
		else
			return false;
	}
	if(me->adcClk == AlxAdc_Clk_McuStm32L0_AdcClk_8MHz_Pclk2Apb2_32MHz)
	{
		if(32000000 == AlxClk_GetClk_Hz(me->clk, AlxClk_Clk_McuStm32_Pclk2Apb2_Ctor))
			return true;
		else
			return false;
	}
	#endif


	//------------------------------------------------------------------------------
	// STM32L4
	//------------------------------------------------------------------------------
	#if defined(ALX_STM32L4)
	if (me->adcClk == AlxAdc_Clk_McuStm32L4_AdcClk_20MHz_Sysclk_80MHz)
	{
		if(80000000 == AlxClk_GetClk_Hz(me->clk, AlxClk_Clk_McuStm32_Sysclk_Ctor))
			return true;
		else
			return false;
	}
	if (me->adcClk == AlxAdc_Clk_McuStm32L4_AdcClk_30MHz_Sysclk_120MHz)
	{
		if(120000000 == AlxClk_GetClk_Hz(me->clk, AlxClk_Clk_McuStm32_Sysclk_Ctor))
			return true;
		else
			return false;
	}
	#endif


	//------------------------------------------------------------------------------
	// STM32U5
	//------------------------------------------------------------------------------
	#if defined(ALX_STM32U5)
	if(me->adcClk == AlxAdc_Clk_McuStm32U5_AdcClk_16MHz_Hclk_16MHz)
	{
		if(16000000 == AlxClk_GetClk_Hz(me->clk, AlxClk_Clk_McuStm32_Hclk_Ctor))
			return true;
		else
			return false;
	}
	if(me->adcClk == AlxAdc_Clk_McuStm32U5_AdcClk_20MHz_Hclk_160MHz)
	{
		if(160000000 == AlxClk_GetClk_Hz(me->clk, AlxClk_Clk_McuStm32_Hclk_Ctor))
			return true;
		else
			return false;
	}
	#endif


	//------------------------------------------------------------------------------
	// Assert
	//------------------------------------------------------------------------------
	ALX_ADC_ASSERT(false);	// We should not get here
	return ALX_NULL;
}
static void AlxAdc_Periph_Adc_EnableClk(AlxAdc* me)
{
	#if defined(ADC1)
	if (me->hadc.Instance == ADC1)	{ __HAL_RCC_ADC1_CLK_ENABLE(); return; }
	#endif
	#if defined(ADC2)
	if (me->hadc.Instance == ADC2)	{ __HAL_RCC_ADC2_CLK_ENABLE(); return; }
	#endif
	#if defined(ADC3)
	if (me->hadc.Instance == ADC3)	{ __HAL_RCC_ADC3_CLK_ENABLE(); return; }
	#endif
	#if defined(ADC4)
	if (me->hadc.Instance == ADC4)	{ __HAL_RCC_ADC4_CLK_ENABLE(); return; }
	#endif
	#if defined(ADC5)
	if (me->hadc.Instance == ADC4)	{ __HAL_RCC_ADC5_CLK_ENABLE(); return; }
	#endif

	ALX_ADC_ASSERT(false);	// We should not get here
}
static void AlxAdc_Periph_Adc_DisableClk(AlxAdc* me)
{
	#if defined(ADC1)
	if (me->hadc.Instance == ADC1)	{ __HAL_RCC_ADC1_CLK_DISABLE(); return; }
	#endif
	#if defined(ADC2)
	if (me->hadc.Instance == ADC2)	{ __HAL_RCC_ADC2_CLK_DISABLE(); return; }
	#endif
	#if defined(ADC3)
	if (me->hadc.Instance == ADC3)	{ __HAL_RCC_ADC3_CLK_DISABLE(); return; }
	#endif
	#if defined(ADC4)
	if (me->hadc.Instance == ADC4)	{ __HAL_RCC_ADC4_CLK_DISABLE(); return; }
	#endif
	#if defined(ADC5)
	if (me->hadc.Instance == ADC4)	{ __HAL_RCC_ADC5_CLK_DISABLE(); return; }
	#endif

	ALX_ADC_ASSERT(false);	// We should not get here
}
static void AlxAdc_Periph_Adc_ForceReset(AlxAdc* me)
{
	#if defined(ADC1)
	if (me->hadc.Instance == ADC1)	{ __HAL_RCC_ADC1_FORCE_RESET(); return; }
	#endif
	#if defined(ADC2)
	if (me->hadc.Instance == ADC2)	{ __HAL_RCC_ADC2_FORCE_RESET(); return; }
	#endif
	#if defined(ADC3)
	if (me->hadc.Instance == ADC3)	{ __HAL_RCC_ADC3_FORCE_RESET(); return; }
	#endif
	#if defined(ADC4)
	if (me->hadc.Instance == ADC4)	{ __HAL_RCC_ADC4_FORCE_RESET(); return; }
	#endif
	#if defined(ADC5)
	if (me->hadc.Instance == ADC4)	{ __HAL_RCC_ADC5_FORCE_RESET(); return; }
	#endif

	ALX_ADC_ASSERT(false);	// We should not get here
}
static void AlxAdc_Periph_Adc_ReleaseReset(AlxAdc* me)
{
	#if defined(ADC1)
	if (me->hadc.Instance == ADC1)	{ __HAL_RCC_ADC1_RELEASE_RESET(); return; }
	#endif
	#if defined(ADC2)
	if (me->hadc.Instance == ADC2)	{ __HAL_RCC_ADC2_RELEASE_RESET(); return; }
	#endif
	#if defined(ADC3)
	if (me->hadc.Instance == ADC3)	{ __HAL_RCC_ADC3_RELEASE_RESET(); return; }
	#endif
	#if defined(ADC4)
	if (me->hadc.Instance == ADC4)	{ __HAL_RCC_ADC4_RELEASE_RESET(); return; }
	#endif

	ALX_ADC_ASSERT(false);	// We should not get here
}
static void AlxAdc_Periph_Dma_EnableClk(AlxAdc* me)
{
	#if defined(DMAMUX1)
	__HAL_RCC_DMAMUX1_CLK_ENABLE();
	#endif

	#if defined(DMA1)
	if (me->dma == DMA1)	{ __HAL_RCC_DMA1_CLK_ENABLE(); return; }
	#endif
	#if defined(DMA2)
	if (me->dma == DMA2)	{ __HAL_RCC_DMA2_CLK_ENABLE(); return; }
	#endif
	#if defined(DMA3)
	if (me->dma == DMA3)	{ __HAL_RCC_DMA3_CLK_ENABLE(); return; }
	#endif
	#if defined(GPDMA1)
	if (me->dma == GPDMA1)	{ __HAL_RCC_GPDMA1_CLK_ENABLE(); return; }
	#endif

	ALX_ADC_ASSERT(false);	// We should not get here
}
static void AlxAdc_Periph_Dma_DisableClk(AlxAdc* me)
{
	#if defined(DMAMUX1)
	__HAL_RCC_DMAMUX1_CLK_DISABLE();
	#endif

	#if defined(DMA1)
	if (me->dma == DMA1)	{ __HAL_RCC_DMA1_CLK_DISABLE(); return; }
	#endif
	#if defined(DMA2)
	if (me->dma == DMA2)	{ __HAL_RCC_DMA2_CLK_DISABLE(); return; }
	#endif
	#if defined(DMA3)
	if (me->dma == DMA3)	{ __HAL_RCC_DMA3_CLK_DISABLE(); return; }
	#endif
	#if defined(GPDMA1)
	if (me->dma == GPDMA1)	{ __HAL_RCC_GPDMA1_CLK_DISABLE(); return; }
	#endif

	ALX_ADC_ASSERT(false);	// We should not get here
}
static void AlxAdc_Periph_Dma_ForceReset(AlxAdc* me)
{
	#if defined(ALX_STM32F0) || defined(ALX_STM32F1)
	return;
	#endif

	#if defined(DMAMUX1)
	__HAL_RCC_DMAMUX1_FORCE_RESET();
	#endif

	#if defined(DMA1)
	if (me->dma == DMA1)	{ __HAL_RCC_DMA1_FORCE_RESET(); return; }
	#endif
	#if defined(DMA2)
	if (me->dma == DMA2)	{ __HAL_RCC_DMA2_FORCE_RESET(); return; }
	#endif
	#if defined(DMA3)
	if (me->dma == DMA3)	{ __HAL_RCC_DMA3_FORCE_RESET(); return; }
	#endif
	#if defined(GPDMA1)
	if (me->dma == GPDMA1)	{ __HAL_RCC_GPDMA1_FORCE_RESET(); return; }
	#endif

	ALX_ADC_ASSERT(false);	// We should not get here
}
static void AlxAdc_Periph_Dma_ReleaseReset(AlxAdc* me)
{
	#if defined(ALX_STM32F0) || defined(ALX_STM32F1)
	return;
	#endif

	#if defined(DMAMUX1)
	__HAL_RCC_DMAMUX1_RELEASE_RESET();
	#endif

	#if defined(DMA1)
	if (me->dma == DMA1)	{ __HAL_RCC_DMA1_RELEASE_RESET(); return; }
	#endif
	#if defined(DMA2)
	if (me->dma == DMA2)	{ __HAL_RCC_DMA2_RELEASE_RESET(); return; }
	#endif
	#if defined(DMA3)
	if (me->dma == DMA3)	{ __HAL_RCC_DMA3_RELEASE_RESET(); return; }
	#endif
	#if defined(GPDMA1)
	if (me->dma == GPDMA1)	{ __HAL_RCC_GPDMA1_RELEASE_RESET(); return; }
	#endif

	ALX_ADC_ASSERT(false);	// We should not get here
}


#endif	// #if defined(ALX_C_LIB) && (defined(ALX_STM32F0) || defined(ALX_STM32F1) || defined(ALX_STM32F4) || defined(ALX_STM32F7) || defined(ALX_STM32G4) || defined(ALX_STM32L0) || defined(ALX_STM32L4) || defined(ALX_STM32U5))
