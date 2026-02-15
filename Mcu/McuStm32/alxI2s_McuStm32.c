/**
  ******************************************************************************
  * @file		alxI2s_McuStm32.c
  * @brief		Auralix C Library - ALX I2S MCU STM32 Module
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
#include "alxI2s_McuStm32.h"
#include "alxI2s.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB) && defined(ALX_STM32F4) && defined(SAI1)


//******************************************************************************
// Private Functions
//******************************************************************************


//------------------------------------------------------------------------------
// General
//------------------------------------------------------------------------------
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
// Constructor
//******************************************************************************

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		i2s
  * @param[in]		do_MCLK
  * @param[in]		do_BCLK
  * @param[in]		do_LRCLK
  * @param[in]		do_SDO
  * @param[in]		di_SDI
  * @param[in]		clk
  * @param[in]		i2sClk
  * @param[in]		audioMode
  * @param[in]		dataSize
  * @param[in]		isMclkUsed
  * @param[in]		txIrqPriority
  * @param[in]		me
  * @param[in]		me
  */
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
	// Assert
	ALX_I2S_ASSERT((audioMode == SAI_MODEMASTER_TX) || (audioMode == SAI_MODEMASTER_RX));	// Currently only Master Mode is supported

	// Objects - External
	me->do_MCLK = do_MCLK;
	me->do_BCLK = do_BCLK;
	me->do_LRCLK = do_LRCLK;
	me->do_SDO = do_SDO;
	me->di_SDI = di_SDI;
	me->clk = clk;

	// Parameters
	me->i2sClk = i2sClk;
	me->protocolDataSize = protocolDataSize;
	me->isMclkUsed = isMclkUsed;
	me->irqPriority = irqPriority;
	ALX_I2S_ASSERT(AlxI2s_Ctor_IsClkOk(me));

	// Variables - PLL
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

	// Variables - Master/Slave
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

	// Variables - hsaiTx
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

	// Variables - hsaiRx
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

	// Info
	me->isInit = false;
	me->wasCtorCalled = true;
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
	if(HAL_RCCEx_PeriphCLKConfig(&me->iclk) != HAL_OK)
	{
		ALX_I2S_TRACE_ERR("ErrClk");
		return Alx_Err;
	};

	// #5 Init SAI
	if(HAL_SAI_InitProtocol(&me->hsaiRx, SAI_I2S_STANDARD, me->protocolDataSize, 2) != HAL_OK)
	{
		ALX_I2S_TRACE_ERR("ErrInit");
		return Alx_Err;
	};
	if(HAL_SAI_InitProtocol(&me->hsaiTx, SAI_I2S_STANDARD, me->protocolDataSize, 2) != HAL_OK)
	{
		ALX_I2S_TRACE_ERR("ErrInit");
		return Alx_Err;
	};

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

/**
  * @brief
  * @param[in,out]	me
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
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

/**
  * @brief
  * @param[in,out] me
  */
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


//------------------------------------------------------------------------------
// General
//------------------------------------------------------------------------------
static bool AlxI2s_Ctor_IsClkOk(AlxI2s* me)
{
	//------------------------------------------------------------------------------
	// STM32F4
	//----------------------------------------------------------------------------
	#if defined(ALX_STM32F4)
	if(me->i2sClk == AlxI2s_Clk_McuStm32F4_SampleFreq_16kHz_SaiClk_49MHz143_MainPllInputClk_2MHz)
	{
		if(2000000UL == AlxClk_GetClk_Hz(me->clk, AlxClk_Clk_McuStm32_MainPllInputClk_Ctor))
			return true;
		else
			return false;
	}
	#endif


	//------------------------------------------------------------------------------
	// Assert
	//------------------------------------------------------------------------------
	ALX_I2S_ASSERT(false);	// We should not get here
	return ALX_NULL;
}
static void AlxI2s_Periph_EnableClk(AlxI2s* me)
{
	#if defined(SAI1)
	if ((me->hsaiTx.Instance == SAI1_Block_A) || (me->hsaiTx.Instance == SAI1_Block_B))	{ __HAL_RCC_SAI1_CLK_ENABLE(); return; }
	#endif
	#if defined(SAI2)
	if ((me->hsaiTx.Instance == SAI2_Block_A) || (me->hsaiTx.Instance == SAI2_Block_B))	{ __HAL_RCC_SAI2_CLK_ENABLE(); return; }
	#endif

	ALX_I2S_ASSERT(false);	// We should not get here
}
static void AlxI2s_Periph_DisableClk(AlxI2s* me)
{
	#if defined(SAI1)
	if ((me->hsaiTx.Instance == SAI1_Block_A) || (me->hsaiTx.Instance == SAI1_Block_B))	{ __HAL_RCC_SAI1_CLK_DISABLE(); return; }
	#endif
	#if defined(SAI2)
	if ((me->hsaiTx.Instance == SAI2_Block_A) || (me->hsaiTx.Instance == SAI2_Block_B))	{ __HAL_RCC_SAI2_CLK_DISABLE(); return; }
	#endif

	ALX_I2S_ASSERT(false);	// We should not get here
}
static void AlxI2s_Periph_ForceReset(AlxI2s* me)
{
	#if defined(SAI1)
	if ((me->hsaiTx.Instance == SAI1_Block_A) || (me->hsaiTx.Instance == SAI1_Block_B))	{ __HAL_RCC_SAI1_FORCE_RESET(); return; }
	#endif
	#if defined(SAI2)
	if ((me->hsaiTx.Instance == SAI2_Block_A) || (me->hsaiTx.Instance == SAI2_Block_B))	{ __HAL_RCC_SAI2_FORCE_RESET(); return; }
	#endif

	ALX_I2S_ASSERT(false);	// We should not get here
}
static void AlxI2s_Periph_ReleaseReset(AlxI2s* me)
{
	#if defined(SAI1)
	if ((me->hsaiTx.Instance == SAI1_Block_A) || (me->hsaiTx.Instance == SAI1_Block_B))	{ __HAL_RCC_SAI1_RELEASE_RESET(); return; }
	#endif
	#if defined(SAI2)
	if ((me->hsaiTx.Instance == SAI2_Block_A) || (me->hsaiTx.Instance == SAI2_Block_B))	{ __HAL_RCC_SAI2_RELEASE_RESET(); return; }
	#endif

	ALX_I2S_ASSERT(false);	// We should not get here
}
static void AlxI2s_Periph_EnableIrq(AlxI2s* me)
{
	#ifdef SAI1
	if ((me->hsaiTx.Instance == SAI1_Block_A) || (me->hsaiTx.Instance == SAI1_Block_B))	{ HAL_NVIC_SetPriority(SAI1_IRQn, me->irqPriority, 0); HAL_NVIC_EnableIRQ(SAI1_IRQn); return; }
	#endif
	#ifdef SAI2
	if ((me->hsaiTx.Instance == SAI2_Block_A) || (me->hsaiTx.Instance == SAI2_Block_B))	{ HAL_NVIC_SetPriority(SAI2_IRQn, me->irqPriority, 0); HAL_NVIC_EnableIRQ(SAI2_IRQn); return; }
	#endif

	ALX_I2S_ASSERT(false);	// We should not get here
}
static void AlxI2s_Periph_DisableIrq(AlxI2s* me)
{
	#ifdef SAI1
	if ((me->hsaiTx.Instance == SAI1_Block_A) || (me->hsaiTx.Instance == SAI1_Block_B))	{ HAL_NVIC_DisableIRQ(SAI1_IRQn); HAL_NVIC_ClearPendingIRQ(SAI1_IRQn); return; }
	#endif
	#ifdef SAI2
	if ((me->hsaiTx.Instance == SAI2_Block_A) || (me->hsaiTx.Instance == SAI2_Block_B))	{ HAL_NVIC_DisableIRQ(SAI2_IRQn); HAL_NVIC_ClearPendingIRQ(SAI2_IRQn); return; }
	#endif

	ALX_I2S_ASSERT(false);	// We should not get here
}


//******************************************************************************
// Weak Functions
//******************************************************************************
ALX_WEAK void AlxI2s_Foreground_Callback_TxL(AlxI2s* me)
{
	(void)me;
	ALX_I2S_ASSERT(false);	// Implement in APP!
}
ALX_WEAK void AlxI2s_Foreground_Callback_TxR(AlxI2s* me)
{
	(void)me;
	ALX_I2S_ASSERT(false);	// Implement in APP!
}
ALX_WEAK void AlxI2s_Foreground_Callback_RxL(AlxI2s* me)
{
	(void)me;
	ALX_I2S_ASSERT(false);	// Implement in APP!
}
ALX_WEAK void AlxI2s_Foreground_Callback_RxR(AlxI2s* me)
{
	(void)me;
	ALX_I2S_ASSERT(false);	// Implement in APP!
}


#endif	// #if defined(ALX_C_LIB) && defined(ALX_STM32F4) && defined(SAI1)
