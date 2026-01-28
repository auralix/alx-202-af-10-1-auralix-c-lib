/**
  ******************************************************************************
  * @file		alxSerialPort_McuStm32.c
  * @brief		Auralix C Library - ALX Serial Port MCU STM32 Module
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
#include "alxSerialPort_McuStm32.h"
#include "alxSerialPort.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB) && (defined(ALX_STM32F0) || defined(ALX_STM32F4) || defined(ALX_STM32F7) || defined(ALX_STM32G4) || defined(ALX_STM32L0) || defined(ALX_STM32L4) || defined(ALX_STM32U5))


//******************************************************************************
// Private Functions
//******************************************************************************


//------------------------------------------------------------------------------
// General
//------------------------------------------------------------------------------
static Alx_Status AlxSerialPort_Reset(AlxSerialPort* me);
static void AlxSerialPort_Periph_EnableClk(AlxSerialPort* me);
static void AlxSerialPort_Periph_DisableClk(AlxSerialPort* me);
static void AlxSerialPort_Periph_ForceReset(AlxSerialPort* me);
static void AlxSerialPort_Periph_ReleaseReset(AlxSerialPort* me);
static void AlxSerialPort_Periph_EnableIrq(AlxSerialPort* me);
static void AlxSerialPort_Periph_DisableIrq(AlxSerialPort* me);


//******************************************************************************
// Constructor
//******************************************************************************

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		config
  * @param[in]		uart
  * @param[in]		do_TX
  * @param[in]		di_RX
  * @param[in]		baudRate
  * @param[in]		dataWidth
  * @param[in]		stopBits
  * @param[in]		parity
  * @param[in]		txFifoBuff
  * @param[in]		txFifoBuffLen
  * @param[in]		rxFifoBuff
  * @param[in]		rxFifoBuffLen
  * @param[in]		irqPriority
  * @param[in]		do_DBG_Tx
  * @param[in]		do_DBG_Rx
  */
void AlxSerialPort_Ctor
(
	AlxSerialPort* me,
	AlxSerialPort_Config config,
	USART_TypeDef* uart,
	AlxIoPin* do_TX,
	AlxIoPin* di_RX,
	AlxGlobal_BaudRate baudRate,
	uint32_t dataWidth,
	uint32_t stopBits,
	uint32_t parity,
	uint8_t* txFifoBuff,
	uint32_t txFifoBuffLen,
	uint8_t* rxFifoBuff,
	uint32_t rxFifoBuffLen,
	Alx_IrqPriority irqPriority,
	AlxIoPin* do_DBG_Tx,
	AlxIoPin* do_DBG_Rx
)
{
	//------------------------------------------------------------------------------
	// Parameters
	//------------------------------------------------------------------------------
	me->config = config;
	me->uart = uart;
	me->do_TX = do_TX;
	me->di_RX = di_RX;
	me->baudRate = baudRate;
	me->dataWidth = dataWidth;
	me->stopBits = stopBits;
	me->parity = parity;
	me->txFifoBuff = txFifoBuff;
	me->txFifoBuffLen = txFifoBuffLen;
	me->rxFifoBuff = rxFifoBuff;
	me->rxFifoBuffLen = rxFifoBuffLen;
	me->irqPriority = irqPriority;
	me->do_DBG_Tx = do_DBG_Tx;
	me->do_DBG_Rx = do_DBG_Rx;


	//------------------------------------------------------------------------------
	// Parameters - Private
	//------------------------------------------------------------------------------

	// txFifoUsed
	if
	(
		config == AlxSerialPort_Config_Standard_TxIrqFifo_RxIrqFifo ||
		config == AlxSerialPort_Config_LinMaster_TxIrqFifo_RxIrqFifo ||
		config == AlxSerialPort_Config_LinSlave_TxIrqFifo_RxIrqFifo
	)
	{
		me->txFifoUsed = true;
	}
	else
	{
		me->txFifoUsed = false;
	}

	// rxFifoUsed
	if
	(
		config == AlxSerialPort_Config_Standard_TxBlocking_RxIrqFifo ||
		config == AlxSerialPort_Config_Standard_TxIrqFifo_RxIrqFifo ||
		config == AlxSerialPort_Config_LinMaster_TxBlocking_RxIrqFifo ||
		config == AlxSerialPort_Config_LinMaster_TxIrqFifo_RxIrqFifo ||
		config == AlxSerialPort_Config_LinSlave_TxBlocking_RxIrqFifo ||
		config == AlxSerialPort_Config_LinSlave_TxIrqFifo_RxIrqFifo
	)
	{
		me->rxFifoUsed = true;
	}
	else
	{
		me->rxFifoUsed = false;
	}

	// linMaster
	if
	(
		config == AlxSerialPort_Config_LinMaster_TxBlocking_RxIrqFifo ||
		config == AlxSerialPort_Config_LinMaster_TxIrqFifo_RxIrqFifo
	)
	{
		me->linMaster = true;
	}
	else
	{
		me->linMaster = false;
	}

	// linSlave
	if
	(
		config == AlxSerialPort_Config_LinSlave_TxBlocking_RxIrqFifo ||
		config == AlxSerialPort_Config_LinSlave_TxIrqFifo_RxIrqFifo
	)
	{
		me->linSlave = true;
	}
	else
	{
		me->linSlave = false;
	}


	//------------------------------------------------------------------------------
	// Variables
	//------------------------------------------------------------------------------
	me->huart.Instance = uart;
	me->huart.Init.BaudRate = (uint32_t)baudRate;
	me->huart.Init.WordLength = dataWidth;
	me->huart.Init.StopBits = stopBits;
	me->huart.Init.Parity = parity;
	me->huart.Init.Mode = UART_MODE_TX_RX;
	me->huart.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	me->huart.Init.OverSampling = UART_OVERSAMPLING_16;
	#if defined(ALX_STM32F0)
	me->huart.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
	me->huart.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_RXOVERRUNDISABLE_INIT;
	me->huart.AdvancedInit.OverrunDisable = UART_ADVFEATURE_OVERRUN_DISABLE;
	#endif
	#if defined(ALX_STM32F7)
	me->huart.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
	me->huart.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_RXOVERRUNDISABLE_INIT;
	me->huart.AdvancedInit.OverrunDisable = UART_ADVFEATURE_OVERRUN_DISABLE;
	#endif
	#if defined(ALX_STM32G4)
	me->huart.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
	me->huart.Init.ClockPrescaler = UART_PRESCALER_DIV1;
	me->huart.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_RXOVERRUNDISABLE_INIT;
	me->huart.AdvancedInit.OverrunDisable = UART_ADVFEATURE_OVERRUN_DISABLE;
	#endif
	#if defined(ALX_STM32L0)
	me->huart.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
	me->huart.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_RXOVERRUNDISABLE_INIT;
	me->huart.AdvancedInit.OverrunDisable = UART_ADVFEATURE_OVERRUN_DISABLE;
	#endif
	#if defined(ALX_STM32L4)
	me->huart.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
	#if defined(USART_PRESC_PRESCALER)
	me->huart.Init.ClockPrescaler = UART_PRESCALER_DIV1;
	#endif
	me->huart.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_RXOVERRUNDISABLE_INIT;
	me->huart.AdvancedInit.OverrunDisable = UART_ADVFEATURE_OVERRUN_DISABLE;
	#endif
	#if defined(ALX_STM32U5)
	me->huart.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
	me->huart.Init.ClockPrescaler = UART_PRESCALER_DIV1;
	me->huart.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_RXOVERRUNDISABLE_INIT;
	me->huart.AdvancedInit.OverrunDisable = UART_ADVFEATURE_OVERRUN_DISABLE;
	#endif

	if (me->txFifoUsed) AlxFifo_Ctor(&me->txFifo, txFifoBuff, txFifoBuffLen);
	if (me->rxFifoUsed) AlxFifo_Ctor(&me->rxFifo, rxFifoBuff, rxFifoBuffLen);


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
Alx_Status AlxSerialPort_Init(AlxSerialPort* me)
{
	// Assert
	ALX_SERIAL_PORT_ASSERT(me->wasCtorCalled == true);
	ALX_SERIAL_PORT_ASSERT(me->isInit == false);

	// Flush TX & RX FIFO
	if (me->txFifoUsed) AlxFifo_Flush(&me->txFifo);
	if (me->rxFifoUsed) AlxFifo_Flush(&me->rxFifo);

	// Init GPIO
	AlxIoPin_Init(me->do_TX);
	AlxIoPin_Init(me->di_RX);

	// Release UART periphery reset
	AlxSerialPort_Periph_ReleaseReset(me);

	// Enable UART periphery clock
	AlxSerialPort_Periph_EnableClk(me);

	// Init UART
	if (me->linMaster || me->linSlave)
	{
		// Assert
		ALX_SERIAL_PORT_ASSERT(me->dataWidth == UART_WORDLENGTH_8B);
		ALX_SERIAL_PORT_ASSERT(me->stopBits == UART_STOPBITS_1);
		ALX_SERIAL_PORT_ASSERT(me->parity == UART_PARITY_NONE);

		// Init
		#if (defined(ALX_STM32F0) || defined(ALX_STM32F4)) && defined(USART_CR2_LINEN)
		HAL_StatusTypeDef statusHal = HAL_LIN_Init(&me->huart, UART_LINBREAKDETECTLENGTH_11B);
		if (statusHal != HAL_OK)
		{
			ALX_SERIAL_PORT_TRACE_ERR("FAIL: HAL_LIN_Init() statusHal %u", statusHal);
			return Alx_Err;
		}
		#endif
	}
	else
	{
		// Init
		HAL_StatusTypeDef statusHal = HAL_UART_Init(&me->huart);
		if (statusHal != HAL_OK)
		{
			ALX_SERIAL_PORT_TRACE_ERR("FAIL: HAL_UART_Init() statusHal %u", statusHal);
			return Alx_Err;
		}
	}

	// Enable UART IRQ
	__HAL_UART_ENABLE_IT(&me->huart, UART_IT_RXNE);
	AlxSerialPort_Periph_EnableIrq(me);

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
Alx_Status AlxSerialPort_DeInit(AlxSerialPort* me)
{
	// Assert
	ALX_SERIAL_PORT_ASSERT(me->wasCtorCalled == true);
	ALX_SERIAL_PORT_ASSERT(me->isInit == true);

	// Disable UART IRQ
	AlxSerialPort_Periph_DisableIrq(me);
	if (me->txFifoUsed) __HAL_UART_DISABLE_IT(&me->huart, UART_IT_TXE);
	__HAL_UART_DISABLE_IT(&me->huart, UART_IT_RXNE);

	// DeInit UART
	HAL_StatusTypeDef statusHal = HAL_UART_DeInit(&me->huart);
	if (statusHal != HAL_OK)
	{
		ALX_SERIAL_PORT_TRACE_ERR("FAIL: HAL_UART_DeInit() statusHal %u", statusHal);
		return Alx_Err;
	}

	// Force UART periphery reset
	AlxSerialPort_Periph_ForceReset(me);

	// Disable UART periphery clock
	AlxSerialPort_Periph_DisableClk(me);

	// DeInit GPIO
	AlxIoPin_DeInit(me->do_TX);
	AlxIoPin_DeInit(me->di_RX);

	// Flush TX & RX FIFO
	if (me->txFifoUsed) AlxFifo_Flush(&me->txFifo);
	if (me->rxFifoUsed) AlxFifo_Flush(&me->rxFifo);

	// Clear isInit
	me->isInit = false;

	// Return
	return Alx_Ok;
}

/**
  * @brief
  * @param[in,out]	me
  * @param[out]		data
  * @param[in]		len
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxSerialPort_Read(AlxSerialPort* me, uint8_t* data, uint32_t len)
{
	// Assert
	ALX_SERIAL_PORT_ASSERT(me->wasCtorCalled == true);
	ALX_SERIAL_PORT_ASSERT(me->isInit == true);
	ALX_SERIAL_PORT_ASSERT(me->rxFifoUsed == true);

	// Read RX FIFO
	AlxGlobal_DisableIrq();
	Alx_Status status = AlxFifo_Read(&me->rxFifo, data, len);
	AlxGlobal_EnableIrq();

	// Return
	return status;
}

/**
  * @brief
  * @param[in,out]	me
  * @param[out]		str
  * @param[in]		delim
  * @param[in]		maxLen
  * @param[out]		numRead
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxSerialPort_ReadStrUntil(AlxSerialPort* me, char* str, const char* delim, uint32_t maxLen, uint32_t* numRead)
{
	// Assert
	ALX_SERIAL_PORT_ASSERT(me->wasCtorCalled == true);
	ALX_SERIAL_PORT_ASSERT(me->isInit == true);
	ALX_SERIAL_PORT_ASSERT(me->rxFifoUsed == true);
	ALX_SERIAL_PORT_ASSERT(me->linMaster == false);
	ALX_SERIAL_PORT_ASSERT(me->linSlave == false);

	// Read RX FIFO
	AlxGlobal_DisableIrq();
	Alx_Status status = AlxFifo_ReadStrUntil(&me->rxFifo, str, delim, maxLen, numRead);
	AlxGlobal_EnableIrq();

	// Return
	return status;
}

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		data
  * @param[in]		len
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxSerialPort_Write(AlxSerialPort* me, const uint8_t* data, uint32_t len)
{
	//------------------------------------------------------------------------------
	// Assert
	//------------------------------------------------------------------------------
	ALX_SERIAL_PORT_ASSERT(me->wasCtorCalled == true);
	ALX_SERIAL_PORT_ASSERT(me->isInit == true);


	//------------------------------------------------------------------------------
	// Local Variables
	//------------------------------------------------------------------------------
	HAL_StatusTypeDef statusHal = HAL_ERROR;
	Alx_Status status = Alx_Err;


	//------------------------------------------------------------------------------
	// LIN Break Condition
	//------------------------------------------------------------------------------
	if (me->linMaster)
	{
		#if (defined(ALX_STM32F0) || defined(ALX_STM32F4)) && defined(USART_CR2_LINEN)
		statusHal = HAL_LIN_SendBreak(&me->huart);
		if (statusHal != HAL_OK)
		{
			ALX_SERIAL_PORT_TRACE_ERR("FAIL: HAL_LIN_SendBreak() statusHal %u", statusHal);
			return Alx_Err;
		}
		#endif
	}


	//------------------------------------------------------------------------------
	// Write
	//------------------------------------------------------------------------------
	if (me->txFifoUsed)
	{
		// Write TX FIFO
		AlxGlobal_DisableIrq();
		status = AlxFifo_Write(&me->txFifo, data, len);
		AlxGlobal_EnableIrq();

		// If UART TX IRQ NOT enabled, enable it
		if (__HAL_UART_GET_IT_SOURCE(&me->huart, UART_IT_TXE) == false)
		{
			__HAL_UART_ENABLE_IT(&me->huart, UART_IT_TXE);
		}
	}
	else
	{
		// TX data blocking
		if (me->do_DBG_Tx != NULL) AlxIoPin_Set(me->do_DBG_Tx);
		statusHal = HAL_UART_Transmit(&me->huart, (uint8_t*)data, len, ALX_SERIAL_PORT_TX_TIMEOUT_ms);
		if (me->do_DBG_Tx != NULL) AlxIoPin_Reset(me->do_DBG_Tx);
		if (statusHal != HAL_OK)
		{
			// Trace
			ALX_SERIAL_PORT_TRACE_ERR("FAIL: HAL_UART_Transmit() statusHal %u", statusHal);

			// Reset
			status = AlxSerialPort_Reset(me);
			if (status != Alx_Ok)
			{
				ALX_SERIAL_PORT_TRACE_ERR("FAIL: AlxSerialPort_Reset() status %u", status);
				return Alx_Err;
			}

			// Return
			return Alx_Err;
		}

		// Set
		status = Alx_Ok;
	}


	//------------------------------------------------------------------------------
	// Return
	//------------------------------------------------------------------------------
	return status;
}

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		str
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxSerialPort_WriteStr(AlxSerialPort* me, const char* str)
{
	// Assert
	ALX_SERIAL_PORT_ASSERT(me->wasCtorCalled == true);
	ALX_SERIAL_PORT_ASSERT(me->isInit == true);
	ALX_SERIAL_PORT_ASSERT(me->linMaster == false);
	ALX_SERIAL_PORT_ASSERT(me->linSlave == false);

	// Return
	return AlxSerialPort_Write(me, (const uint8_t*)str, strlen(str));
}

/**
  * @brief
  * @param[in,out]	me
  */
void AlxSerialPort_IrqHandler(AlxSerialPort* me)
{
	//------------------------------------------------------------------------------
	// TX Data
	//------------------------------------------------------------------------------
	if (me->txFifoUsed)
	{
		if (__HAL_UART_GET_FLAG(&me->huart, UART_FLAG_TXE))
		{
			uint8_t txData = 0;
			Alx_Status status = AlxFifo_Read(&me->txFifo, &txData, sizeof(txData));
			if (status == Alx_Ok)
			{
				// TX data from TX FIFO
				if (me->do_DBG_Tx != NULL) AlxIoPin_Set(me->do_DBG_Tx);
				LL_USART_TransmitData8(me->huart.Instance, txData);	// Clears TXE
				if (me->do_DBG_Tx != NULL) AlxIoPin_Reset(me->do_DBG_Tx);
			}
			else
			{
				// Disable UART TX IRQ, no more data in TX FIFO
				__HAL_UART_DISABLE_IT(&me->huart, UART_IT_TXE);
			}
		}
	}


	//------------------------------------------------------------------------------
	// RX Data
	//------------------------------------------------------------------------------
	if (__HAL_UART_GET_FLAG(&me->huart, UART_FLAG_RXNE))
	{
		#if defined(ALX_STM32F4)
		// Overrun error handling, periphery doesn't have overrun error disable functionality.
		// We clear overrun flag with sequence of first reading status register, and then data register.
		volatile const uint32_t dummy = me->huart.Instance->SR;
		#endif

		// No overrun error handling, overrun error must be disabled @ Uart initialization.
		if (me->do_DBG_Rx != NULL) AlxIoPin_Set(me->do_DBG_Rx);
		uint8_t rxData = LL_USART_ReceiveData8(me->huart.Instance);	// Clears RXNE
		if (me->do_DBG_Rx != NULL) AlxIoPin_Reset(me->do_DBG_Rx);
		AlxFifo_Write(&me->rxFifo, &rxData, 1);
	}
}

/**
  * @brief
  * @param[in,out]	me
  */
void AlxSerialPort_FlushTxFifo(AlxSerialPort* me)
{
	// Assert
	ALX_SERIAL_PORT_ASSERT(me->wasCtorCalled == true);

	// Flush
	if (me->txFifoUsed) AlxFifo_Flush(&me->txFifo);
}

/**
  * @brief
  * @param[in,out]	me
  * @return
  */
uint32_t AlxSerialPort_GetTxFifoNumOfEntries(AlxSerialPort* me)
{
	// Assert
	ALX_SERIAL_PORT_ASSERT(me->wasCtorCalled == true);

	// Get
	if (me->txFifoUsed)
	{
		return AlxFifo_GetNumOfEntries(&me->txFifo);
	}
	else
	{
		return 0;
	}
}

/**
  * @brief
  * @param[in,out]	me
  */
void AlxSerialPort_FlushRxFifo(AlxSerialPort* me)
{
	// Assert
	ALX_SERIAL_PORT_ASSERT(me->wasCtorCalled == true);

	// Flush
	if (me->rxFifoUsed) AlxFifo_Flush(&me->rxFifo);
}

/**
  * @brief
  * @param[in,out]	me
  * @return
  */
uint32_t AlxSerialPort_GetRxFifoNumOfEntries(AlxSerialPort* me)
{
	// Assert
	ALX_SERIAL_PORT_ASSERT(me->wasCtorCalled == true);

	// Get
	if (me->rxFifoUsed)
	{
		return AlxFifo_GetNumOfEntries(&me->txFifo);
	}
	else
	{
		return 0;
	}
}


//******************************************************************************
// Private Functions
//******************************************************************************


//------------------------------------------------------------------------------
// General
//------------------------------------------------------------------------------
static Alx_Status AlxSerialPort_Reset(AlxSerialPort* me)
{
	// Disable UART IRQ
	AlxSerialPort_Periph_DisableIrq(me);
	if (me->txFifoUsed) __HAL_UART_DISABLE_IT(&me->huart, UART_IT_TXE);
	__HAL_UART_DISABLE_IT(&me->huart, UART_IT_RXNE);

	// DeInit UART
	HAL_StatusTypeDef statusHal = HAL_UART_DeInit(&me->huart);
	if (statusHal != HAL_OK)
	{
		ALX_SERIAL_PORT_TRACE_ERR("FAIL: HAL_UART_DeInit() statusHal %u", statusHal);
		return Alx_Err;
	}

	// Force UART periphery reset
	AlxSerialPort_Periph_ForceReset(me);

	// Flush TX & RX FIFO
	if (me->txFifoUsed) AlxFifo_Flush(&me->txFifo);
	if (me->rxFifoUsed) AlxFifo_Flush(&me->rxFifo);

	// Clear isInit
	me->isInit = false;

	// Release UART periphery reset
	AlxSerialPort_Periph_ReleaseReset(me);

	// Init UART
	if (me->linMaster || me->linSlave)
	{
		// Assert
		ALX_SERIAL_PORT_ASSERT(me->dataWidth == UART_WORDLENGTH_8B);
		ALX_SERIAL_PORT_ASSERT(me->stopBits == UART_STOPBITS_1);
		ALX_SERIAL_PORT_ASSERT(me->parity == UART_PARITY_NONE);

		// Init
		#if (defined(ALX_STM32F0) || defined(ALX_STM32F4)) && defined(USART_CR2_LINEN)
		HAL_StatusTypeDef statusHal = HAL_LIN_Init(&me->huart, UART_LINBREAKDETECTLENGTH_11B);
		if (statusHal != HAL_OK)
		{
			ALX_SERIAL_PORT_TRACE_ERR("FAIL: HAL_LIN_Init() statusHal %u", statusHal);
			return Alx_Err;
		}
		#endif
	}
	else
	{
		// Init
		HAL_StatusTypeDef statusHal = HAL_UART_Init(&me->huart);
		if (statusHal != HAL_OK)
		{
			ALX_SERIAL_PORT_TRACE_ERR("FAIL: HAL_UART_Init() statusHal %u", statusHal);
			return Alx_Err;
		}
	}

	// Enable UART RX IRQ
	__HAL_UART_ENABLE_IT(&me->huart, UART_IT_RXNE);
	AlxSerialPort_Periph_EnableIrq(me);

	// Set isInit
	me->isInit = true;

	// Return
	return Alx_Ok;
}
static void AlxSerialPort_Periph_EnableClk(AlxSerialPort* me)
{
	#ifdef USART1
	if (me->huart.Instance == USART1)	{ __HAL_RCC_USART1_CLK_ENABLE(); return; }
	#endif
	#ifdef USART2
	if (me->huart.Instance == USART2)	{ __HAL_RCC_USART2_CLK_ENABLE(); return; }
	#endif
	#ifdef USART3
	if (me->huart.Instance == USART3)	{ __HAL_RCC_USART3_CLK_ENABLE(); return; }
	#endif
	#ifdef USART4
	if (me->huart.Instance == USART4)	{ __HAL_RCC_USART4_CLK_ENABLE(); return; }
	#endif
	#ifdef USART5
	if (me->huart.Instance == USART5)	{ __HAL_RCC_USART5_CLK_ENABLE(); return; }
	#endif
	#ifdef USART6
	if (me->huart.Instance == USART6)	{ __HAL_RCC_USART6_CLK_ENABLE(); return; }
	#endif
	#ifdef USART7
	if (me->huart.Instance == USART7)	{ __HAL_RCC_USART7_CLK_ENABLE(); return; }
	#endif
	#ifdef USART8
	if (me->huart.Instance == USART8)	{ __HAL_RCC_USART8_CLK_ENABLE(); return; }
	#endif
	#ifdef USART9
	if (me->huart.Instance == USART9)	{ __HAL_RCC_USART9_CLK_ENABLE(); return; }
	#endif
	#ifdef USART10
	if (me->huart.Instance == USART10)	{ __HAL_RCC_USART10_CLK_ENABLE(); return; }
	#endif
	#ifdef UART1
	if (me->huart.Instance == UART1)	{ __HAL_RCC_UART1_CLK_ENABLE(); return; }
	#endif
	#ifdef UART2
	if (me->huart.Instance == UART2)	{ __HAL_RCC_UART2_CLK_ENABLE(); return; }
	#endif
	#ifdef UART3
	if (me->huart.Instance == UART3)	{ __HAL_RCC_UART3_CLK_ENABLE(); return; }
	#endif
	#ifdef UART4
	if (me->huart.Instance == UART4)	{ __HAL_RCC_UART4_CLK_ENABLE(); return; }
	#endif
	#ifdef UART5
	if (me->huart.Instance == UART5)	{ __HAL_RCC_UART5_CLK_ENABLE(); return; }
	#endif
	#ifdef UART6
	if (me->huart.Instance == UART6)	{ __HAL_RCC_UART6_CLK_ENABLE(); return; }
	#endif
	#ifdef UART7
	if (me->huart.Instance == UART7)	{ __HAL_RCC_UART7_CLK_ENABLE(); return; }
	#endif
	#ifdef UART8
	if (me->huart.Instance == UART8)	{ __HAL_RCC_UART8_CLK_ENABLE(); return; }
	#endif
	#ifdef UART9
	if (me->huart.Instance == UART9)	{ __HAL_RCC_UART9_CLK_ENABLE(); return; }
	#endif
	#ifdef UART10
	if (me->huart.Instance == UART10)	{ __HAL_RCC_UART10_CLK_ENABLE(); return; }
	#endif
	#ifdef LPUART1
	if (me->huart.Instance == LPUART1)	{ __HAL_RCC_LPUART1_CLK_ENABLE(); return; }
	#endif
	#ifdef LPUART2
	if (me->huart.Instance == LPUART2)	{ __HAL_RCC_LPUART2_CLK_ENABLE(); return; }
	#endif

	ALX_SERIAL_PORT_ASSERT(false);	// We should not get here
}
static void AlxSerialPort_Periph_DisableClk(AlxSerialPort* me)
{
	#ifdef USART1
	if (me->huart.Instance == USART1)	{ __HAL_RCC_USART1_CLK_DISABLE(); return; }
	#endif
	#ifdef USART2
	if (me->huart.Instance == USART2)	{ __HAL_RCC_USART2_CLK_DISABLE(); return; }
	#endif
	#ifdef USART3
	if (me->huart.Instance == USART3)	{ __HAL_RCC_USART3_CLK_DISABLE(); return; }
	#endif
	#ifdef USART4
	if (me->huart.Instance == USART4)	{ __HAL_RCC_USART4_CLK_DISABLE(); return; }
	#endif
	#ifdef USART5
	if (me->huart.Instance == USART5)	{ __HAL_RCC_USART5_CLK_DISABLE(); return; }
	#endif
	#ifdef USART6
	if (me->huart.Instance == USART6)	{ __HAL_RCC_USART6_CLK_DISABLE(); return; }
	#endif
	#ifdef USART7
	if (me->huart.Instance == USART7)	{ __HAL_RCC_USART7_CLK_DISABLE(); return; }
	#endif
	#ifdef USART8
	if (me->huart.Instance == USART8)	{ __HAL_RCC_USART8_CLK_DISABLE(); return; }
	#endif
	#ifdef USART9
	if (me->huart.Instance == USART9)	{ __HAL_RCC_USART9_CLK_DISABLE(); return; }
	#endif
	#ifdef USART10
	if (me->huart.Instance == USART10)	{ __HAL_RCC_USART10_CLK_DISABLE(); return; }
	#endif
	#ifdef UART1
	if (me->huart.Instance == UART1)	{ __HAL_RCC_UART1_CLK_DISABLE(); return; }
	#endif
	#ifdef UART2
	if (me->huart.Instance == UART2)	{ __HAL_RCC_UART2_CLK_DISABLE(); return; }
	#endif
	#ifdef UART3
	if (me->huart.Instance == UART3)	{ __HAL_RCC_UART3_CLK_DISABLE(); return; }
	#endif
	#ifdef UART4
	if (me->huart.Instance == UART4)	{ __HAL_RCC_UART4_CLK_DISABLE(); return; }
	#endif
	#ifdef UART5
	if (me->huart.Instance == UART5)	{ __HAL_RCC_UART5_CLK_DISABLE(); return; }
	#endif
	#ifdef UART6
	if (me->huart.Instance == UART6)	{ __HAL_RCC_UART6_CLK_DISABLE(); return; }
	#endif
	#ifdef UART7
	if (me->huart.Instance == UART7)	{ __HAL_RCC_UART7_CLK_DISABLE(); return; }
	#endif
	#ifdef UART8
	if (me->huart.Instance == UART8)	{ __HAL_RCC_UART8_CLK_DISABLE(); return; }
	#endif
	#ifdef UART9
	if (me->huart.Instance == UART9)	{ __HAL_RCC_UART9_CLK_DISABLE(); return; }
	#endif
	#ifdef UART10
	if (me->huart.Instance == UART10)	{ __HAL_RCC_UART10_CLK_DISABLE(); return; }
	#endif
	#ifdef LPUART1
	if (me->huart.Instance == LPUART1)	{ __HAL_RCC_LPUART1_CLK_DISABLE(); return; }
	#endif
	#ifdef LPUART2
	if (me->huart.Instance == LPUART2)	{ __HAL_RCC_LPUART2_CLK_DISABLE(); return; }
	#endif

	ALX_SERIAL_PORT_ASSERT(false);	// We should not get here
}
static void AlxSerialPort_Periph_ForceReset(AlxSerialPort* me)
{
	#ifdef USART1
	if (me->huart.Instance == USART1)	{ __HAL_RCC_USART1_FORCE_RESET(); return; }
	#endif
	#ifdef USART2
	if (me->huart.Instance == USART2)	{ __HAL_RCC_USART2_FORCE_RESET(); return; }
	#endif
	#ifdef USART3
	if (me->huart.Instance == USART3)	{ __HAL_RCC_USART3_FORCE_RESET(); return; }
	#endif
	#ifdef USART4
	if (me->huart.Instance == USART4)	{ __HAL_RCC_USART4_FORCE_RESET(); return; }
	#endif
	#ifdef USART5
	if (me->huart.Instance == USART5)	{ __HAL_RCC_USART5_FORCE_RESET(); return; }
	#endif
	#ifdef USART6
	if (me->huart.Instance == USART6)	{ __HAL_RCC_USART6_FORCE_RESET(); return; }
	#endif
	#ifdef USART7
	if (me->huart.Instance == USART7)	{ __HAL_RCC_USART7_FORCE_RESET(); return; }
	#endif
	#ifdef USART8
	if (me->huart.Instance == USART8)	{ __HAL_RCC_USART8_FORCE_RESET(); return; }
	#endif
	#ifdef USART9
	if (me->huart.Instance == USART9)	{ __HAL_RCC_USART9_FORCE_RESET(); return; }
	#endif
	#ifdef USART10
	if (me->huart.Instance == USART10)	{ __HAL_RCC_USART10_FORCE_RESET(); return; }
	#endif
	#ifdef UART1
	if (me->huart.Instance == UART1)	{ __HAL_RCC_UART1_FORCE_RESET(); return; }
	#endif
	#ifdef UART2
	if (me->huart.Instance == UART2)	{ __HAL_RCC_UART2_FORCE_RESET(); return; }
	#endif
	#ifdef UART3
	if (me->huart.Instance == UART3)	{ __HAL_RCC_UART3_FORCE_RESET(); return; }
	#endif
	#ifdef UART4
	if (me->huart.Instance == UART4)	{ __HAL_RCC_UART4_FORCE_RESET(); return; }
	#endif
	#ifdef UART5
	if (me->huart.Instance == UART5)	{ __HAL_RCC_UART5_FORCE_RESET(); return; }
	#endif
	#ifdef UART6
	if (me->huart.Instance == UART6)	{ __HAL_RCC_UART6_FORCE_RESET(); return; }
	#endif
	#ifdef UART7
	if (me->huart.Instance == UART7)	{ __HAL_RCC_UART7_FORCE_RESET(); return; }
	#endif
	#ifdef UART8
	if (me->huart.Instance == UART8)	{ __HAL_RCC_UART8_FORCE_RESET(); return; }
	#endif
	#ifdef UART9
	if (me->huart.Instance == UART9)	{ __HAL_RCC_UART9_FORCE_RESET(); return; }
	#endif
	#ifdef UART10
	if (me->huart.Instance == UART10)	{ __HAL_RCC_UART10_FORCE_RESET(); return; }
	#endif
	#ifdef LPUART1
	if (me->huart.Instance == LPUART1)	{ __HAL_RCC_LPUART1_FORCE_RESET(); return; }
	#endif
	#ifdef LPUART2
	if (me->huart.Instance == LPUART2)	{ __HAL_RCC_LPUART2_FORCE_RESET(); return; }
	#endif

	ALX_SERIAL_PORT_ASSERT(false);	// We should not get here
}
static void AlxSerialPort_Periph_ReleaseReset(AlxSerialPort* me)
{
	#ifdef USART1
	if (me->huart.Instance == USART1)	{ __HAL_RCC_USART1_RELEASE_RESET(); return; }
	#endif
	#ifdef USART2
	if (me->huart.Instance == USART2)	{ __HAL_RCC_USART2_RELEASE_RESET(); return; }
	#endif
	#ifdef USART3
	if (me->huart.Instance == USART3)	{ __HAL_RCC_USART3_RELEASE_RESET(); return; }
	#endif
	#ifdef USART4
	if (me->huart.Instance == USART4)	{ __HAL_RCC_USART4_RELEASE_RESET(); return; }
	#endif
	#ifdef USART5
	if (me->huart.Instance == USART5)	{ __HAL_RCC_USART5_RELEASE_RESET(); return; }
	#endif
	#ifdef USART6
	if (me->huart.Instance == USART6)	{ __HAL_RCC_USART6_RELEASE_RESET(); return; }
	#endif
	#ifdef USART7
	if (me->huart.Instance == USART7)	{ __HAL_RCC_USART7_RELEASE_RESET(); return; }
	#endif
	#ifdef USART8
	if (me->huart.Instance == USART8)	{ __HAL_RCC_USART8_RELEASE_RESET(); return; }
	#endif
	#ifdef USART9
	if (me->huart.Instance == USART9)	{ __HAL_RCC_USART9_RELEASE_RESET(); return; }
	#endif
	#ifdef USART10
	if (me->huart.Instance == USART10)	{ __HAL_RCC_USART10_RELEASE_RESET(); return; }
	#endif
	#ifdef UART1
	if (me->huart.Instance == UART1)	{ __HAL_RCC_UART1_RELEASE_RESET(); return; }
	#endif
	#ifdef UART2
	if (me->huart.Instance == UART2)	{ __HAL_RCC_UART2_RELEASE_RESET(); return; }
	#endif
	#ifdef UART3
	if (me->huart.Instance == UART3)	{ __HAL_RCC_UART3_RELEASE_RESET(); return; }
	#endif
	#ifdef UART4
	if (me->huart.Instance == UART4)	{ __HAL_RCC_UART4_RELEASE_RESET(); return; }
	#endif
	#ifdef UART5
	if (me->huart.Instance == UART5)	{ __HAL_RCC_UART5_RELEASE_RESET(); return; }
	#endif
	#ifdef UART6
	if (me->huart.Instance == UART6)	{ __HAL_RCC_UART6_RELEASE_RESET(); return; }
	#endif
	#ifdef UART7
	if (me->huart.Instance == UART7)	{ __HAL_RCC_UART7_RELEASE_RESET(); return; }
	#endif
	#ifdef UART8
	if (me->huart.Instance == UART8)	{ __HAL_RCC_UART8_RELEASE_RESET(); return; }
	#endif
	#ifdef UART9
	if (me->huart.Instance == UART9)	{ __HAL_RCC_UART9_RELEASE_RESET(); return; }
	#endif
	#ifdef UART10
	if (me->huart.Instance == UART10)	{ __HAL_RCC_UART10_RELEASE_RESET(); return; }
	#endif
	#ifdef LPUART1
	if (me->huart.Instance == LPUART1)	{ __HAL_RCC_LPUART1_RELEASE_RESET(); return; }
	#endif
	#ifdef LPUART2
	if (me->huart.Instance == LPUART2)	{ __HAL_RCC_LPUART2_RELEASE_RESET(); return; }
	#endif

	ALX_SERIAL_PORT_ASSERT(false);	// We should not get here
}
static void AlxSerialPort_Periph_EnableIrq(AlxSerialPort* me)
{
	#ifdef USART1
	if (me->huart.Instance == USART1)	{ HAL_NVIC_SetPriority(USART1_IRQn, me->irqPriority, 0); HAL_NVIC_EnableIRQ(USART1_IRQn); return; }
	#endif
	#ifdef USART2
	if (me->huart.Instance == USART2)	{ HAL_NVIC_SetPriority(USART2_IRQn, me->irqPriority, 0); HAL_NVIC_EnableIRQ(USART2_IRQn); return; }
	#endif
	#ifdef USART3
	if (me->huart.Instance == USART3)	{ HAL_NVIC_SetPriority(USART3_IRQn, me->irqPriority, 0); HAL_NVIC_EnableIRQ(USART3_IRQn); return; }
	#endif
	#ifdef USART4
	if (me->huart.Instance == USART4)	{ HAL_NVIC_SetPriority(USART4_5_IRQn, me->irqPriority, 0); HAL_NVIC_EnableIRQ(USART4_5_IRQn); return; }
	#endif
	#ifdef USART5
	if (me->huart.Instance == USART5)	{ HAL_NVIC_SetPriority(USART4_5_IRQn, me->irqPriority, 0); HAL_NVIC_EnableIRQ(USART4_5_IRQn); return; }
	#endif
	#ifdef USART6
	if (me->huart.Instance == USART6)	{ HAL_NVIC_SetPriority(USART6_IRQn, me->irqPriority, 0); HAL_NVIC_EnableIRQ(USART6_IRQn); return; }
	#endif
	#ifdef USART7
	if (me->huart.Instance == USART7)	{ HAL_NVIC_SetPriority(USART7_IRQn, me->irqPriority, 0); HAL_NVIC_EnableIRQ(USART7_IRQn); return; }
	#endif
	#ifdef USART8
	if (me->huart.Instance == USART8)	{ HAL_NVIC_SetPriority(USART8_IRQn, me->irqPriority, 0); HAL_NVIC_EnableIRQ(USART8_IRQn); return; }
	#endif
	#ifdef USART9
	if (me->huart.Instance == USART9)	{ HAL_NVIC_SetPriority(USART9_IRQn, me->irqPriority, 0); HAL_NVIC_EnableIRQ(USART9_IRQn); return; }
	#endif
	#ifdef USART10
	if (me->huart.Instance == USART10)	{ HAL_NVIC_SetPriority(USART10_IRQn, me->irqPriority, 0); HAL_NVIC_EnableIRQ(USART10_IRQn); return; }
	#endif
	#ifdef UART1
	if (me->huart.Instance == UART1)	{ HAL_NVIC_SetPriority(UART1_IRQn, me->irqPriority, 0); HAL_NVIC_EnableIRQ(UART1_IRQn); return; }
	#endif
	#ifdef UART2
	if (me->huart.Instance == UART2)	{ HAL_NVIC_SetPriority(UART2_IRQn, me->irqPriority, 0); HAL_NVIC_EnableIRQ(UART2_IRQn); return; }
	#endif
	#ifdef UART3
	if (me->huart.Instance == UART3)	{ HAL_NVIC_SetPriority(UART3_IRQn, me->irqPriority, 0); HAL_NVIC_EnableIRQ(UART3_IRQn); return; }
	#endif
	#ifdef UART4
	if (me->huart.Instance == UART4)	{ HAL_NVIC_SetPriority(UART4_IRQn, me->irqPriority, 0); HAL_NVIC_EnableIRQ(UART4_IRQn); return; }
	#endif
	#ifdef UART5
	if (me->huart.Instance == UART5)	{ HAL_NVIC_SetPriority(UART5_IRQn, me->irqPriority, 0); HAL_NVIC_EnableIRQ(UART5_IRQn); return; }
	#endif
	#ifdef UART6
	if (me->huart.Instance == UART6)	{ HAL_NVIC_SetPriority(UART6_IRQn, me->irqPriority, 0); HAL_NVIC_EnableIRQ(UART6_IRQn); return; }
	#endif
	#ifdef UART7
	if (me->huart.Instance == UART7)	{ HAL_NVIC_SetPriority(UART7_IRQn, me->irqPriority, 0); HAL_NVIC_EnableIRQ(UART7_IRQn); return; }
	#endif
	#ifdef UART8
	if (me->huart.Instance == UART8)	{ HAL_NVIC_SetPriority(UART8_IRQn, me->irqPriority, 0); HAL_NVIC_EnableIRQ(UART8_IRQn); return; }
	#endif
	#ifdef UART9
	if (me->huart.Instance == UART9)	{ HAL_NVIC_SetPriority(UART9_IRQn, me->irqPriority, 0); HAL_NVIC_EnableIRQ(UART9_IRQn); return; }
	#endif
	#ifdef UART10
	if (me->huart.Instance == UART10)	{ HAL_NVIC_SetPriority(UART10_IRQn, me->irqPriority, 0); HAL_NVIC_EnableIRQ(UART10_IRQn); return; }
	#endif
	#ifdef LPUART1
	if (me->huart.Instance == LPUART1)	{ HAL_NVIC_SetPriority(LPUART1_IRQn, me->irqPriority, 0); HAL_NVIC_EnableIRQ(LPUART1_IRQn); return; }
	#endif
	#ifdef LPUART2
	if (me->huart.Instance == LPUART2)	{ HAL_NVIC_SetPriority(LPUART2_IRQn, me->irqPriority, 0); HAL_NVIC_EnableIRQ(LPUART2_IRQn); return; }
	#endif

	ALX_SERIAL_PORT_ASSERT(false);	// We should not get here
}
static void AlxSerialPort_Periph_DisableIrq(AlxSerialPort* me)
{
	#ifdef USART1
	if (me->huart.Instance == USART1)	{ HAL_NVIC_DisableIRQ(USART1_IRQn); HAL_NVIC_ClearPendingIRQ(USART1_IRQn); return; }
	#endif
	#ifdef USART2
	if (me->huart.Instance == USART2)	{ HAL_NVIC_DisableIRQ(USART2_IRQn); HAL_NVIC_ClearPendingIRQ(USART2_IRQn); return; }
	#endif
	#ifdef USART3
	if (me->huart.Instance == USART3)	{ HAL_NVIC_DisableIRQ(USART3_IRQn); HAL_NVIC_ClearPendingIRQ(USART3_IRQn); return; }
	#endif
	#ifdef USART4
	if (me->huart.Instance == USART4)	{ HAL_NVIC_DisableIRQ(USART4_5_IRQn); HAL_NVIC_ClearPendingIRQ(USART4_5_IRQn); return; }
	#endif
	#ifdef USART5
	if (me->huart.Instance == USART5)	{ HAL_NVIC_DisableIRQ(USART4_5_IRQn); HAL_NVIC_ClearPendingIRQ(USART4_5_IRQn); return; }
	#endif
	#ifdef USART6
	if (me->huart.Instance == USART6)	{ HAL_NVIC_DisableIRQ(USART6_IRQn); HAL_NVIC_ClearPendingIRQ(USART6_IRQn); return; }
	#endif
	#ifdef USART7
	if (me->huart.Instance == USART7)	{ HAL_NVIC_DisableIRQ(USART7_IRQn); HAL_NVIC_ClearPendingIRQ(USART7_IRQn); return; }
	#endif
	#ifdef USART8
	if (me->huart.Instance == USART8)	{ HAL_NVIC_DisableIRQ(USART8_IRQn); HAL_NVIC_ClearPendingIRQ(USART8_IRQn); return; }
	#endif
	#ifdef USART9
	if (me->huart.Instance == USART9)	{ HAL_NVIC_DisableIRQ(USART9_IRQn); HAL_NVIC_ClearPendingIRQ(USART9_IRQn); return; }
	#endif
	#ifdef USART10
	if (me->huart.Instance == USART10)	{ HAL_NVIC_DisableIRQ(USART10_IRQn); HAL_NVIC_ClearPendingIRQ(USART10_IRQn); return; }
	#endif
	#ifdef UART1
	if (me->huart.Instance == UART1)	{ HAL_NVIC_DisableIRQ(UART1_IRQn); HAL_NVIC_ClearPendingIRQ(UART1_IRQn); return; }
	#endif
	#ifdef UART2
	if (me->huart.Instance == UART2)	{ HAL_NVIC_DisableIRQ(UART2_IRQn); HAL_NVIC_ClearPendingIRQ(UART2_IRQn); return; }
	#endif
	#ifdef UART3
	if (me->huart.Instance == UART3)	{ HAL_NVIC_DisableIRQ(UART3_IRQn); HAL_NVIC_ClearPendingIRQ(UART3_IRQn); return; }
	#endif
	#ifdef UART4
	if (me->huart.Instance == UART4)	{ HAL_NVIC_DisableIRQ(UART4_IRQn); HAL_NVIC_ClearPendingIRQ(UART4_IRQn); return; }
	#endif
	#ifdef UART5
	if (me->huart.Instance == UART5)	{ HAL_NVIC_DisableIRQ(UART5_IRQn); HAL_NVIC_ClearPendingIRQ(UART5_IRQn); return; }
	#endif
	#ifdef UART6
	if (me->huart.Instance == UART6)	{ HAL_NVIC_DisableIRQ(UART6_IRQn); HAL_NVIC_ClearPendingIRQ(UART6_IRQn); return; }
	#endif
	#ifdef UART7
	if (me->huart.Instance == UART7)	{ HAL_NVIC_DisableIRQ(UART7_IRQn); HAL_NVIC_ClearPendingIRQ(UART7_IRQn); return; }
	#endif
	#ifdef UART8
	if (me->huart.Instance == UART8)	{ HAL_NVIC_DisableIRQ(UART8_IRQn); HAL_NVIC_ClearPendingIRQ(UART8_IRQn); return; }
	#endif
	#ifdef UART9
	if (me->huart.Instance == UART9)	{ HAL_NVIC_DisableIRQ(UART9_IRQn); HAL_NVIC_ClearPendingIRQ(UART9_IRQn); return; }
	#endif
	#ifdef UART10
	if (me->huart.Instance == UART10)	{ HAL_NVIC_DisableIRQ(UART10_IRQn); HAL_NVIC_ClearPendingIRQ(UART10_IRQn); return; }
	#endif
	#ifdef LPUART1
	if (me->huart.Instance == LPUART1)	{ HAL_NVIC_DisableIRQ(LPUART1_IRQn); HAL_NVIC_ClearPendingIRQ(LPUART1_IRQn); return; }
	#endif
	#ifdef LPUART2
	if (me->huart.Instance == LPUART2)	{ HAL_NVIC_DisableIRQ(LPUART2_IRQn); HAL_NVIC_ClearPendingIRQ(LPUART2_IRQn); return; }
	#endif

	ALX_SERIAL_PORT_ASSERT(false);	// We should not get here
}


#endif	// #if defined(ALX_C_LIB) && (defined(ALX_STM32F0) || defined(ALX_STM32F4) || defined(ALX_STM32F7) || defined(ALX_STM32G4) || defined(ALX_STM32L0) || defined(ALX_STM32L4) || defined(ALX_STM32U5))
