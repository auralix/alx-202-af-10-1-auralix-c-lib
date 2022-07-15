/**
  ******************************************************************************
  * @file		alxI2s_McuStm32.c
  * @brief		Auralix C Library - ALX I2S Module
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
#include "alxI2s_McuStm32.h"
#include "alxI2s.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_STM32F4)


//******************************************************************************
// Private Functions
//******************************************************************************
static bool AlxI2s_Ctor_IsClkOk(AlxI2s* me);
static void AlxI2s_Periph_EnableClk(AlxI2s* me);
static void AlxI2s_Periph_DisableClk(AlxI2s* me);
static void AlxI2s_Periph_ForceReset(AlxI2s* me);
static void AlxI2s_Periph_ReleaseReset(AlxI2s* me);
static void AlxI2s_Periph_EnableIrq(AlxI2s* me);
static void AlxI2s_Periph_DisableIrq(AlxI2s* me);


//******************************************************************************
// Weak Functions
//******************************************************************************
void AlxI2s_Foreground_Callback_TxL(AlxI2s* me);
void AlxI2s_Foreground_Callback_TxR(AlxI2s* me);
void AlxI2s_Foreground_Callback_RxL(AlxI2s* me);
void AlxI2s_Foreground_Callback_RxR(AlxI2s* me);


//******************************************************************************
// Specific Functions
//******************************************************************************
void AlxI2s_Ctor
(
	AlxI2s* me,
	SAI_Block_TypeDef* i2s,
	AlxIoPin* do_MCLK,
	AlxIoPin* do_BCLK,
	AlxIoPin* do_LRCLK,
	AlxIoPin* do_SDO,
	AlxIoPin* di_SDI,
	AlxClk* clk,
	AlxI2s_Clk i2sClk,
	uint32_t audioMode,
	uint32_t protocolDataSize,
	bool isMclkUsed,
	Alx_IrqPriority irqPriority
)
{
	//******************************************************************************
	// Assert
	//******************************************************************************
	ALX_I2S_ASSERT((audioMode == SAI_MODEMASTER_TX) || (audioMode == SAI_MODEMASTER_RX));	// Currently only Master Mode is supported


	//******************************************************************************
	// Objects - External
	//******************************************************************************
	me->do_MCLK = do_MCLK;
	me->do_BCLK = do_BCLK;
	me->do_LRCLK = do_LRCLK;
	me->do_SDO = do_SDO;
	me->di_SDI = di_SDI;
	me->clk = clk;


	//******************************************************************************
	// Parameters
	//******************************************************************************
	me->i2sClk = i2sClk;
	me->protocolDataSize = protocolDataSize;
	me->isMclkUsed = isMclkUsed;
	me->irqPriority = irqPriority;
	ALX_I2S_ASSERT(AlxI2s_Ctor_IsClkOk(me));


	//******************************************************************************
	// Variables
	//******************************************************************************

	// PLL
	#if defined(STM32F427xx) || defined(STM32F437xx) || defined(STM32F429xx) || defined(STM32F439xx)
	me->iclk.PeriphClockSelection = RCC_PERIPHCLK_SAI_PLLI2S;	// MF: This sets SAI Input Clk "SAI_CK_x" to 49.142 Mhz
	me->iclk.PLLI2S.PLLI2SN = 172;
	me->iclk.PLLI2S.PLLI2SR = ALX_NULL;		// Not Used
	me->iclk.PLLI2S.PLLI2SQ = 7;
	me->iclk.PLLSAI.PLLSAIN = ALX_NULL;		// Not Used
	me->iclk.PLLSAI.PLLSAIQ = ALX_NULL;		// Not Used
	me->iclk.PLLSAI.PLLSAIR = ALX_NULL;		// Not Used
	me->iclk.PLLI2SDivQ = 1;
	me->iclk.PLLSAIDivQ = ALX_NULL;			// Not Used
	me->iclk.PLLSAIDivR = ALX_NULL;			// Not Used
	me->iclk.RTCClockSelection = ALX_NULL;	// Not Used
	me->iclk.TIMPresSelection = ALX_NULL;	// Not Used
	#endif

	// Master/Slave
	if(audioMode == SAI_MODEMASTER_TX)
	{
		// TX
		me->hsaiTx.Instance = i2s;
		me->hsaiTx.Init.AudioMode = audioMode;
		me->hsaiTx.Init.Synchro = SAI_ASYNCHRONOUS;

		// RX
		if		(me->hsaiTx.Instance == SAI1_Block_A)	{ me->hsaiRx.Instance = SAI1_Block_B; }
		else if	(me->hsaiTx.Instance == SAI1_Block_B)	{ me->hsaiRx.Instance = SAI1_Block_A; }
		else											{ ALX_I2S_ASSERT(false); }	// We shouldn't get here
		me->hsaiRx.Init.AudioMode = SAI_MODESLAVE_RX;
		me->hsaiRx.Init.Synchro = SAI_SYNCHRONOUS;
	}
	else if(audioMode == SAI_MODEMASTER_RX)
	{
		// RX
		me->hsaiRx.Instance = i2s;
		me->hsaiRx.Init.AudioMode = audioMode;
		me->hsaiRx.Init.Synchro = SAI_ASYNCHRONOUS;

		// TX
		if		(me->hsaiRx.Instance == SAI1_Block_A)	{ me->hsaiTx.Instance = SAI1_Block_B; }
		else if	(me->hsaiRx.Instance == SAI1_Block_B)	{ me->hsaiTx.Instance = SAI1_Block_A; }
		else											{ ALX_I2S_ASSERT(false); }	// We shouldn't get here
		me->hsaiTx.Init.AudioMode = SAI_MODESLAVE_TX;
		me->hsaiTx.Init.Synchro = SAI_SYNCHRONOUS;
	}
	else { ALX_I2S_ASSERT(false); }	// We shouldn't get here

	// hsaiTx
	me->hsaiTx.Init.SynchroExt = SAI_SYNCEXT_DISABLE;
	me->hsaiTx.Init.OutputDrive = SAI_OUTPUTDRIVE_DISABLE;
	me->hsaiTx.Init.NoDivider = SAI_MASTERDIVIDER_ENABLE;
	me->hsaiTx.Init.FIFOThreshold = SAI_FIFOTHRESHOLD_FULL;	//  SAI_FIFOTHRESHOLD_FULL; //
	me->hsaiTx.Init.ClockSource = SAI_CLKSOURCE_PLLI2S;
	me->hsaiTx.Init.AudioFrequency = (uint32_t)me->i2sClk;
	me->hsaiTx.Init.Mckdiv = ALX_NULL;						// TV: Set by HAL_SAI_InitProtocol()
	me->hsaiTx.Init.MonoStereoMode = SAI_STEREOMODE;
	me->hsaiTx.Init.CompandingMode = SAI_NOCOMPANDING;
	me->hsaiTx.Init.TriState = SAI_OUTPUT_NOTRELEASED;		// MF: Output will be 0, not HI-Z (if Slot Size != Data Size)
	me->hsaiTx.Init.Protocol = ALX_NULL;					// TV: Set by HAL_SAI_InitProtocol()
	me->hsaiTx.Init.DataSize = ALX_NULL;					// TV: Set by HAL_SAI_InitProtocol()
	me->hsaiTx.Init.FirstBit = ALX_NULL;					// TV: Set by HAL_SAI_InitProtocol()
	me->hsaiTx.Init.ClockStrobing = ALX_NULL;				// TV: Set by HAL_SAI_InitProtocol()

	// hsaiRx
	me->hsaiRx.Init.SynchroExt = SAI_SYNCEXT_DISABLE;
	me->hsaiRx.Init.OutputDrive = SAI_OUTPUTDRIVE_DISABLE;
	me->hsaiRx.Init.NoDivider = SAI_MASTERDIVIDER_ENABLE;
	me->hsaiRx.Init.FIFOThreshold = SAI_FIFOTHRESHOLD_EMPTY;	// SAI_FIFOTHRESHOLD_EMPTY;	//
	me->hsaiRx.Init.ClockSource = SAI_CLKSOURCE_PLLI2S;
	me->hsaiRx.Init.AudioFrequency = (uint32_t)me->i2sClk;
	me->hsaiRx.Init.Mckdiv = 0;
	me->hsaiRx.Init.MonoStereoMode = SAI_STEREOMODE;
	me->hsaiRx.Init.CompandingMode = SAI_NOCOMPANDING;
	me->hsaiRx.Init.TriState = SAI_OUTPUT_NOTRELEASED;
	me->hsaiRx.Init.Protocol = ALX_NULL;					// TV: Set by HAL_SAI_InitProtocol()
	me->hsaiRx.Init.DataSize = ALX_NULL;					// TV: Set by HAL_SAI_InitProtocol()
	me->hsaiRx.Init.FirstBit = ALX_NULL;					// TV: Set by HAL_SAI_InitProtocol()
	me->hsaiRx.Init.ClockStrobing = ALX_NULL;				// TV: Set by HAL_SAI_InitProtocol()

	// Variables
	me->sampleBuffTxL = 0;
	me->sampleBuffTxR = 0;
	me->sampleBuffRxL = 0;
	me->sampleBuffRxR = 0;
	me->isTxChLeft = true;
	me->isRxChLeft = true;
	
	
	//******************************************************************************
	// Info
	//******************************************************************************
	me->isInit = false;
	me->wasCtorCalled = true;
}
Alx_Status AlxI2s_Init(AlxI2s* me)
{
	ALX_I2S_ASSERT(me->isInit == false);
	ALX_I2S_ASSERT(me->wasCtorCalled == true);

	// #1 Init GPIO
	if(me->isMclkUsed) AlxIoPin_Init(me->do_MCLK);
	AlxIoPin_Init(me->do_BCLK);
	AlxIoPin_Init(me->do_LRCLK);
	AlxIoPin_Init(me->do_SDO);
	AlxIoPin_Init(me->di_SDI);

	// #2 Release SAI Periphery Reset
	AlxI2s_Periph_ReleaseReset(me);

	// #3 Enable SAI Periphery Clock
	AlxI2s_Periph_EnableClk(me);

	// #4 Init SAI PLL Clock
	if(HAL_RCCEx_PeriphCLKConfig(&me->iclk) != HAL_OK) { ALX_I2S_TRACE("ErrClk"); return Alx_Err; };

	// #5 Init SAI
	if(HAL_SAI_InitProtocol(&me->hsaiRx, SAI_I2S_STANDARD, me->protocolDataSize, 2) != HAL_OK)	{ ALX_I2S_TRACE("ErrInit"); return Alx_Err; };
	if(HAL_SAI_InitProtocol(&me->hsaiTx, SAI_I2S_STANDARD, me->protocolDataSize, 2) != HAL_OK)	{ ALX_I2S_TRACE("ErrInit"); return Alx_Err; };

	// #6 Enable SAI IRQ
	__HAL_SAI_ENABLE_IT(&me->hsaiTx, SAI_xIMR_FREQIE);
	__HAL_SAI_ENABLE_IT(&me->hsaiRx, SAI_xIMR_FREQIE);
	AlxI2s_Periph_EnableIrq(me);

	// #7 Set Ch Info
	me->isTxChLeft = true;
	me->isRxChLeft = true;

	// #8 Enable SAI
	if (me->hsaiTx.Init.AudioMode == SAI_MODESLAVE_TX)
	{
		__HAL_SAI_ENABLE(&me->hsaiTx);		// MF: Slave must be enabled before master
		__HAL_SAI_ENABLE(&me->hsaiRx);
	}
	else if (me->hsaiRx.Init.AudioMode == SAI_MODESLAVE_RX)
	{
		__HAL_SAI_ENABLE(&me->hsaiRx);		// MF: Slave must be enabled before master
		__HAL_SAI_ENABLE(&me->hsaiTx);
	}
	else { ALX_I2S_ASSERT(false); }			// We shouldn't get here

	// #9 Set isInit
	me->isInit = true;

	// #10 Return OK
	return Alx_Ok;
}
Alx_Status AlxI2s_DeInit(AlxI2s* me)
{
	ALX_I2S_ASSERT(me->isInit == true);
	ALX_I2S_ASSERT(me->wasCtorCalled == true);

	// #1 Disable SAI
	if (me->hsaiTx.Init.AudioMode == SAI_MODEMASTER_TX)
	{
		__HAL_SAI_DISABLE(&me->hsaiTx);		// MF: Master must be disabled first
		__HAL_SAI_DISABLE(&me->hsaiRx);
	}
	else if (me->hsaiRx.Init.AudioMode == SAI_MODEMASTER_RX)
	{
		__HAL_SAI_DISABLE(&me->hsaiRx);		// MF: Master must be disabled first
		__HAL_SAI_DISABLE(&me->hsaiTx);
	}
	else { ALX_I2S_ASSERT(false); }			// We shouldn't get here

	// #2 Disable SAI IRQ
	__HAL_SAI_DISABLE_IT(&me->hsaiTx, SAI_xIMR_FREQIE);
	__HAL_SAI_DISABLE_IT(&me->hsaiRx, SAI_xIMR_FREQIE);
	__HAL_SAI_CLEAR_FLAG(&me->hsaiTx, SAI_xIMR_FREQIE);
	__HAL_SAI_CLEAR_FLAG(&me->hsaiRx, SAI_xIMR_FREQIE);
	AlxI2s_Periph_DisableIrq(me);

	// #2 DeInit SAI
	HAL_SAI_DeInit(&me->hsaiTx);
	HAL_SAI_DeInit(&me->hsaiRx);

	// #3 Force SAI Periphery Reset
	AlxI2s_Periph_ForceReset(me);

	// #4 Disable SAI Periphery Clock
	AlxI2s_Periph_DisableClk(me);

	// #5 DeInit GPIO
	AlxIoPin_DeInit(me->do_MCLK);
	AlxIoPin_DeInit(me->do_BCLK);
	AlxIoPin_DeInit(me->do_LRCLK);
	AlxIoPin_DeInit(me->do_SDO);
	AlxIoPin_DeInit(me->di_SDI);

	// #6 Reset isInit
	me->isInit = false;

	// #7 Return OK
	return Alx_Ok;
}
void AlxI2s_Foreground_Handle(AlxI2s* me)
{
	// #1 Check if TX FIFO NOT full IRQ occurred
	if((me->hsaiTx.Instance->SR & SAI_xSR_FREQ) == SAI_xSR_FREQ)
	{
		// #1.1 Left channel data needed for transmission
		if (me->isTxChLeft)
		{
			// Callback
			AlxI2s_Foreground_Callback_TxL(me);
				
			// Write data
			me->hsaiTx.Instance->DR = me->sampleBuffTxL;
		}
		// #1.2 Right channel data needed for transmission
		else
		{
			// Callback
			AlxI2s_Foreground_Callback_TxR(me);

			// Write data
			me->hsaiTx.Instance->DR = me->sampleBuffTxR;
		}

		// #1.3 Handle TX left/right channel flag
		me->isTxChLeft = !me->isTxChLeft;
	}
	// #2 Check if RX FIFO NOT empty IRQ occurred
	else if((me->hsaiRx.Instance->SR & SAI_xSR_FREQ) == SAI_xSR_FREQ)
	{
		// #2.1 Left channel data received
		if (me->isRxChLeft)
		{
			// Read data
			me->sampleBuffRxL = me->hsaiRx.Instance->DR;

			// Callback
			AlxI2s_Foreground_Callback_RxL(me);
		}
		// #2.2 Right channel data received
		else
		{
			// Read data
			me->sampleBuffRxR = me->hsaiRx.Instance->DR;

			// Callback
			AlxI2s_Foreground_Callback_RxR(me);
		}

		// #2.3 Handle RX left/right channel flag
		me->isRxChLeft = !me->isRxChLeft;
	}
	// #3 Error
	else
	{
		ALX_I2S_ASSERT(false); // We shouldn't get here
	}
}


//******************************************************************************
// Private Functions
//******************************************************************************
static bool AlxI2s_Ctor_IsClkOk(AlxI2s* me)
{
	#if defined(STM32F4)
	if(me->i2sClk == AlxI2s_Clk_McuStm32F4_SampleFreq_16kHz_SaiClk_49MHz143_MainPllInputClk_2MHz)
	{
		if(2000000UL == AlxClk_GetClk_Hz(me->clk, AlxClk_Clk_McuStm32_MainPllInputClk_Ctor))
			return true;
		else
			return false;
	}

	#endif
	#if defined(STM32G4)
	#endif

	ALX_I2S_ASSERT(false); // We shouldn't get here
	return ALX_NULL;
}
static void AlxI2s_Periph_EnableClk(AlxI2s* me)
{
	bool isErr = true;

	#if defined(SAI1)
	if ((me->hsaiTx.Instance == SAI1_Block_A) || (me->hsaiTx.Instance == SAI1_Block_B))	{ __HAL_RCC_SAI1_CLK_ENABLE(); isErr = false; }
	#endif
	#if defined(SAI2)
	if ((me->hsaiTx.Instance == SAI2_Block_A) || (me->hsaiTx.Instance == SAI2_Block_B))	{ __HAL_RCC_SAI2_CLK_ENABLE(); isErr = false; }
	#endif

	if(isErr) { ALX_I2S_ASSERT(false); }	// We shouldn't get here
}
static void AlxI2s_Periph_DisableClk(AlxI2s* me)
{
	bool isErr = true;

	#if defined(SAI1)
	if ((me->hsaiTx.Instance == SAI1_Block_A) || (me->hsaiTx.Instance == SAI1_Block_B))	{ __HAL_RCC_SAI1_CLK_DISABLE(); isErr = false; }
	#endif
	#if defined(SAI2)
	if ((me->hsaiTx.Instance == SAI2_Block_A) || (me->hsaiTx.Instance == SAI2_Block_B))	{ __HAL_RCC_SAI2_CLK_DISABLE(); isErr = false; }
	#endif

	if(isErr) { ALX_I2S_ASSERT(false); }	// We shouldn't get here
}
static void AlxI2s_Periph_ForceReset(AlxI2s* me)
{
	bool isErr = true;

	#if defined(SAI1)
	if ((me->hsaiTx.Instance == SAI1_Block_A) || (me->hsaiTx.Instance == SAI1_Block_B))	{ __HAL_RCC_SAI1_FORCE_RESET(); isErr = false; }
	#endif
	#if defined(SAI2)
	if ((me->hsaiTx.Instance == SAI2_Block_A) || (me->hsaiTx.Instance == SAI2_Block_B))	{ __HAL_RCC_SAI2_FORCE_RESET(); isErr = false; }
	#endif

	if(isErr) { ALX_I2S_ASSERT(false); }	// We shouldn't get here
}
static void AlxI2s_Periph_ReleaseReset(AlxI2s* me)
{
	bool isErr = true;

	#if defined(SAI1)
	if ((me->hsaiTx.Instance == SAI1_Block_A) || (me->hsaiTx.Instance == SAI1_Block_B))	{ __HAL_RCC_SAI1_RELEASE_RESET(); isErr = false; }
	#endif
	#if defined(SAI2)
	if ((me->hsaiTx.Instance == SAI2_Block_A) || (me->hsaiTx.Instance == SAI2_Block_B))	{ __HAL_RCC_SAI2_RELEASE_RESET(); isErr = false; }
	#endif

	if(isErr) { ALX_I2S_ASSERT(false); }	// We shouldn't get here
}
static void AlxI2s_Periph_EnableIrq(AlxI2s* me)
{
	bool isErr = true;

	#ifdef SAI1
	if ((me->hsaiTx.Instance == SAI1_Block_A) || (me->hsaiTx.Instance == SAI1_Block_B))
	{
		HAL_NVIC_SetPriority(SAI1_IRQn, me->irqPriority, 0);
		HAL_NVIC_EnableIRQ(SAI1_IRQn);
		isErr = false;
	}
	#endif
	#ifdef SAI2
	if ((me->hsaiTx.Instance == SAI2_Block_A) || (me->hsaiTx.Instance == SAI2_Block_B))
	{
		HAL_NVIC_SetPriority(SAI2_IRQn, me->irqPriority, 0);
		HAL_NVIC_EnableIRQ(SAI2_IRQn);
		isErr = false;
	}
	#endif

	if(isErr) { ALX_I2S_ASSERT(false); }	// We shouldn't get here
}
static void AlxI2s_Periph_DisableIrq(AlxI2s* me)
{
	bool isErr = true;

	#ifdef SAI1
	if ((me->hsaiTx.Instance == SAI1_Block_A) || (me->hsaiTx.Instance == SAI1_Block_B))
	{
		HAL_NVIC_DisableIRQ(SAI1_IRQn);
		HAL_NVIC_ClearPendingIRQ(SAI1_IRQn);
		isErr = false;
	}
	#endif
	#ifdef SAI2
	if ((me->hsaiTx.Instance == SAI2_Block_A) || (me->hsaiTx.Instance == SAI2_Block_B))
	{
		HAL_NVIC_DisableIRQ(SAI2_IRQn);
		HAL_NVIC_ClearPendingIRQ(SAI2_IRQn);
		isErr = false;
	}
	#endif

	if(isErr) { ALX_I2S_ASSERT(false); }	// We shouldn't get here
}


//******************************************************************************
// Weak Functions
//******************************************************************************
ALX_WEAK void AlxI2s_Foreground_Callback_TxL(AlxI2s* me)
{
	(void)me;

	ALX_I2S_TRACE("Define AlxI2s_Foreground_Callback_TxL");
	ALX_I2S_ASSERT(false);
}
ALX_WEAK void AlxI2s_Foreground_Callback_TxR(AlxI2s* me)
{
	(void)me;

	ALX_I2S_TRACE("Define AlxI2s_Foreground_Callback_TxR");
	ALX_I2S_ASSERT(false);
}
ALX_WEAK void AlxI2s_Foreground_Callback_RxL(AlxI2s* me)
{
	(void)me;

	ALX_I2S_TRACE("Define AlxI2s_Foreground_Callback_RxL");
	ALX_I2S_ASSERT(false);
}
ALX_WEAK void AlxI2s_Foreground_Callback_RxR(AlxI2s* me)
{
	(void)me;

	ALX_I2S_TRACE("Define AlxI2s_Foreground_Callback_RxR");
	ALX_I2S_ASSERT(false);
}

#endif

























































































//void AlxI2s_Start_Write(AlxI2s* me, uint8_t* data, uint16_t len)
//{
//	ALX_I2S_ASSERT(me->isInit == true);
//	ALX_I2S_ASSERT(me->wasCtorCalled == true);
//	ALX_I2S_ASSERT(me->isTxAudio == false);
//	ALX_I2S_ASSERT((me->hsaiTx.Init.AudioMode == SAI_MODEMASTER_TX) || (me->hsaiTx.Init.AudioMode == SAI_MODESLAVE_TX));
//
//	// #1 Set isTxAudi attribute
//	me->isTxAudio = true;
//
//	// #2 Set audio sample parameters
//	me->txPrtHandle = 0;					// MF: Sample will start playing from beggining after "AlxI2s_Stop_Write_IRQ" (might not be best solution)
//	me->txAudioPtr = data;
//	me->txAudioSize = len;
//
//	// #3 Enable "FIFO request" interrupt. MF: Must be enabled here, IRQ is generated when FIFO is empty
//	NVIC_DisableIRQ(SAI1_IRQn);
//	__HAL_SAI_ENABLE_IT(&me->hsaiTx, SAI_xIMR_FREQIE);
//	NVIC_EnableIRQ(SAI1_IRQn);
//}
//void AlxI2s_Stop_Write(AlxI2s* me)
//{
//	ALX_I2S_ASSERT(me->isInit == true);
//	ALX_I2S_ASSERT(me->wasCtorCalled == true);
//	ALX_I2S_ASSERT(me->isTxAudio == true);
//
//	// #1 Set isTxAudio attribute
//	me->isTxAudio = false;
//}
//void AlxI2s_Start_Read(AlxI2s* me, uint8_t* data)
//{
//	ALX_I2S_ASSERT(me->isInit == true);
//	ALX_I2S_ASSERT(me->wasCtorCalled == true);
//	ALX_I2S_ASSERT(me->isTxAudio == false);
//	ALX_I2S_ASSERT((me->hsaiRx.Init.AudioMode == SAI_MODEMASTER_RX) || (me->hsaiRx.Init.AudioMode == SAI_MODESLAVE_RX));
//
//	// #1 Set isRxAudi attribute
//	me->isRxAudio = true;
//
//	// TODO
//
//	// #3 Enable "FIFO request" interrupt. MF: Must be enabled here, IRQ is generated when FIFO is empty
//	NVIC_DisableIRQ(SAI1_IRQn);
//	__HAL_SAI_ENABLE_IT(&me->hsaiRx, SAI_xIMR_FREQIE);
//	NVIC_EnableIRQ(SAI1_IRQn);
//}
//void AlxI2s_Stop_Read(AlxI2s* me)
//{
//	ALX_I2S_ASSERT(me->isInit == true);
//	ALX_I2S_ASSERT(me->wasCtorCalled == true);
//	ALX_I2S_ASSERT(me->isRxAudio == true);
//
//	// #1 Set isRxAudio attribute
//	me->isRxAudio = false;
//}
//void AlxI2s_Foreground_Handle(AlxI2s* me)
//{
//	ALX_I2S_ASSERT(me->isInit == true);
//	ALX_I2S_ASSERT(me->wasCtorCalled == true);
//
//	if (((me->hsaiTx.Instance->IMR & SAI_xIMR_FREQIE) == SAI_xIMR_FREQIE) &&
//		((me->hsaiTx.Instance->SR & SAI_xSR_FREQ) == SAI_xSR_FREQ))
//	{
//		// Write to FIFO unitl FREQ bit in SAI_XSR register is cleared
//		while (__HAL_SAI_GET_FLAG(&me->hsaiTx, SAI_FLAG_FREQ) == true)
//		{
//			if (me->hsaiTx.Init.DataSize == SAI_DATASIZE_8)
//			{
//				if (AlxI2s_Foreground_Handle_Tx_WriteToFifo(me, 1) == true)		{ return; }
//			}
//			else if (me->hsaiTx.Init.DataSize == SAI_DATASIZE_16)
//			{
//				if (AlxI2s_Foreground_Handle_Tx_WriteToFifo(me, 2) == true)		{ return; }
//			}
//			else if (me->hsaiTx.Init.DataSize == SAI_DATASIZE_32)
//			{
//				if (AlxI2s_Foreground_Handle_Tx_WriteToFifo(me, 4) == true)		{ return; }
//			}
//			else	{ ALX_I2S_ASSERT(false); }	// We shouldn't get here
//		}
//	}
//	if (((me->hsaiRx.Instance->IMR & SAI_xIMR_FREQIE) == SAI_xIMR_FREQIE) &&
//		((me->hsaiRx.Instance->SR & SAI_xSR_FREQ) == SAI_xSR_FREQ))
//	{
//		// Read from FIFO unitl FREQ bit in SAI_XSR register is cleared
//		while(__HAL_SAI_GET_FLAG(&me->hsaiRx, SAI_FLAG_FREQ) == true)
//		{
//			if (me->hsaiRx.Init.DataSize == SAI_DATASIZE_8)
//			{
//				if (AlxI2s_Foreground_Handle_Rx_ReadFromFifo(me, 1) == true)	{ return; }
//			}
//			else if (me->hsaiRx.Init.DataSize == SAI_DATASIZE_16)
//			{
//				if (AlxI2s_Foreground_Handle_Rx_ReadFromFifo(me, 2) == true)	{ return; }
//			}
//			else if (me->hsaiRx.Init.DataSize == SAI_DATASIZE_32)
//			{
//				if (AlxI2s_Foreground_Handle_Rx_ReadFromFifo(me, 4) == true)	{ return; }
//			}
//			else	{ ALX_I2S_ASSERT(false); }	// We shouldn't get here
//		}
//	}
//	// if ()	TODO
//}
//uint8_t AlxI2s_GetFrameLength(AlxI2s* me)
//{
//	if (me->hsaiTx.Init.DataSize == SAI_DATASIZE_8  || me->hsaiRx.Init.DataSize == SAI_DATASIZE_8 )		return 16U;	// FrameLenght is 2 times data size because 2 Channels (L R)
//	if (me->hsaiTx.Init.DataSize == SAI_DATASIZE_10 || me->hsaiRx.Init.DataSize == SAI_DATASIZE_10)		return 20U;
//	if (me->hsaiTx.Init.DataSize == SAI_DATASIZE_16 || me->hsaiRx.Init.DataSize == SAI_DATASIZE_16)		return 32U;
//	if (me->hsaiTx.Init.DataSize == SAI_DATASIZE_20 || me->hsaiRx.Init.DataSize == SAI_DATASIZE_20)		return 40U;
//	if (me->hsaiTx.Init.DataSize == SAI_DATASIZE_24 || me->hsaiRx.Init.DataSize == SAI_DATASIZE_24)		return 48U;
//	if (me->hsaiTx.Init.DataSize == SAI_DATASIZE_32 || me->hsaiRx.Init.DataSize == SAI_DATASIZE_32)		return 64U;
//	else
//	{
//		ALX_I2S_ASSERT(false);		// We shouldn't get here
//		return 255U;
//	}
//}





//HAL_RCCEx_EnablePLLI2S(&me->iclk.PLLI2S);

	// #4 Init I2S
//	HAL_SAI_Init(&me->hsaiRx);				// MF: Slave must be enabled before master
//	HAL_SAI_Init(&me->hsaiTx);

//	// #5 Set Up and Enable IRQ
//	NVIC_SetPriority(SAI1_IRQn, 0);
//	//__HAL_SAI_ENABLE_IT(&me->hsaiRx, SAI_xIMR_FREQIE);
//	NVIC_EnableIRQ(SAI1_IRQn);




//	NVIC_DisableIRQ(SAI1_IRQn);
//	me->hsaiTx.Instance->IMR &= ~(0x7F);	// Clears IM Register
//	me->hsaiRx.Instance->IMR &= ~(0x7F);	// Clears IM Register


//	// #4 DeInit Clk
//	__HAL_RCC_SAI1_CLK_DISABLE();

	// #5 DeInit  Pins








//static uint8_t AlxI2s_GetFrameLength(AlxI2s* me);
//static bool AlxI2s_Foreground_Handle_Tx_WriteToFifo(AlxI2s* me, uint8_t size);
//static bool AlxI2s_Foreground_Handle_Rx_ReadFromFifo(AlxI2s* me, uint8_t size);








//	me->hsaiTx.Init.Protocol = SAI_FREE_PROTOCOL;					// TV: Set by HAL_SAI_InitProtocol()
//	me->hsaiTx.Init.DataSize = dataSize;							// TV: Set by HAL_SAI_InitProtocol()
//	me->hsaiTx.Init.FirstBit = SAI_FIRSTBIT_MSB;					// TV: Set by HAL_SAI_InitProtocol()
//	me->hsaiTx.Init.ClockStrobing = SAI_CLOCKSTROBING_FALLINGEDGE;	// TV: Set by HAL_SAI_InitProtocol()

//	uint8_t frameLenght = AlxI2s_GetFrameLength(me);				// MF: Must be here dataSize is declared on line 119
//
//	me->hsaiTx.FrameInit.FrameLength = (uint32_t)frameLenght;
//	me->hsaiTx.FrameInit.ActiveFrameLength = (uint32_t)frameLenght / 2;
//	me->hsaiTx.FrameInit.FSDefinition = SAI_FS_CHANNEL_IDENTIFICATION;
//	me->hsaiTx.FrameInit.FSPolarity = SAI_FS_ACTIVE_LOW;
//	me->hsaiTx.FrameInit.FSOffset = SAI_FS_BEFOREFIRSTBIT;						// MF: Lahko da tudi to fuka (page 932)
//
//	me->hsaiTx.SlotInit.FirstBitOffset = 0U;
//	me->hsaiTx.SlotInit.SlotSize = SAI_SLOTSIZE_DATASIZE;
//	me->hsaiTx.SlotInit.SlotNumber = 2U;							// L and R channel
//	me->hsaiTx.SlotInit.SlotActive = SAI_SLOTACTIVE_0 | SAI_SLOTACTIVE_1;

//	me->hsaiRx.Init.Protocol = SAI_FREE_PROTOCOL;					// TV: Set by HAL_SAI_InitProtocol()
//	me->hsaiRx.Init.DataSize = dataSize;							// TV: Set by HAL_SAI_InitProtocol()
//	me->hsaiRx.Init.FirstBit = SAI_FIRSTBIT_MSB;					// TV: Set by HAL_SAI_InitProtocol()
//	me->hsaiRx.Init.ClockStrobing = SAI_CLOCKSTROBING_FALLINGEDGE;	// TV: Set by HAL_SAI_InitProtocol()

//	me->hsaiRx.FrameInit.FrameLength = (uint32_t)frameLenght;
//	me->hsaiRx.FrameInit.ActiveFrameLength = (uint32_t)frameLenght / 2;
//	me->hsaiRx.FrameInit.FSDefinition = SAI_FS_CHANNEL_IDENTIFICATION;
//	me->hsaiRx.FrameInit.FSPolarity = SAI_FS_ACTIVE_LOW;
//	me->hsaiRx.FrameInit.FSOffset = SAI_FS_BEFOREFIRSTBIT;
//
//	me->hsaiRx.SlotInit.FirstBitOffset = 0U;
//	me->hsaiRx.SlotInit.SlotSize = SAI_SLOTSIZE_DATASIZE;
//	me->hsaiRx.SlotInit.SlotNumber = 2U;
//	me->hsaiRx.SlotInit.SlotActive = SAI_SLOTACTIVE_0 | SAI_SLOTACTIVE_1;


//	me->txAudioPtr = NULL;
//	me->txAudioSize = 0;
//	me->txPrtHandle = 0;
//	me->rxAudioPtr = NULL;
//	me->rxAudioSize = 0;
//	me->rxPrtHandle = 0;


//	me->isErr = false;
//	me->isTxAudio = false;
//	me->isRxAudio = false;






//static bool AlxI2s_Foreground_Handle_Tx_WriteToFifo(AlxI2s* me, uint8_t size)	// MF: Works without pops/clicks only if audio sample starts at ~0
//{
//	if (me->txPrtHandle == size * me->txAudioSize)
//	{
//		if (me->isTxAudio == false)
//		{
//			// Disable FIFO IRQ
//			NVIC_DisableIRQ(SAI1_IRQn);
//			__HAL_SAI_DISABLE_IT(&me->hsaiTx, SAI_xIMR_FREQIE);
//			NVIC_EnableIRQ(SAI1_IRQn);
//			return true;
//		}
//		else	{ me->txPrtHandle = 0; }
//	}
//
//	if (me->hsaiTx.Init.DataSize == SAI_DATASIZE_8)
//	{
//		me->hsaiTx.Instance->DR = *(me->txAudioPtr + me->txPrtHandle);
//	}
//	else if (me->hsaiTx.Init.DataSize == SAI_DATASIZE_16)
//	{
//		me->hsaiTx.Instance->DR = *(uint16_t *)(me->txAudioPtr + me->txPrtHandle);
//	}
//	else if (me->hsaiTx.Init.DataSize == SAI_DATASIZE_32)
//	{
//		me->hsaiTx.Instance->DR = *(uint32_t *)(me->txAudioPtr + me->txPrtHandle);
//	}
//
//	me->txPrtHandle += size;
//
//	return false;
//}
//static bool AlxI2s_Foreground_Handle_Rx_ReadFromFifo(AlxI2s* me, uint8_t size)
//{
//
//	return false;
//}
