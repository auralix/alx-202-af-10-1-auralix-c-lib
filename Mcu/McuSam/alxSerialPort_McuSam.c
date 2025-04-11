/**
  ******************************************************************************
  * @file		alxSerialPort_McuSam.c
  * @brief		Auralix C Library - ALX Serial Port MCU SAM Module
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
#include "alxSerialPort_McuSam.h"
#include "alxSerialPort.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB) && defined(ALX_SAM)


//******************************************************************************
// Private Functions
//******************************************************************************
static Alx_Status AlxSerialPort_Reset(AlxSerialPort* me);
static void AlxSerialPort_Periph_EnableClk(AlxSerialPort* me);
static void AlxSerialPort_Periph_DisableClk(AlxSerialPort* me);
static void AlxSerialPort_Periph_EnableIrq(AlxSerialPort* me);
static void AlxSerialPort_Periph_DisableIrq(AlxSerialPort* me);


//******************************************************************************
// Weak Functions
//******************************************************************************
void AlxSerialPort_RxData_Callback(AlxSerialPort* me, uint8_t data);
void AlxSerialPort_LinSlaveRxBreak_Callback(AlxSerialPort* me);


//******************************************************************************
// Constructor
//******************************************************************************

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		hw
  * @param[in]		config
  * @param[in]		do_TX
  * @param[in]		di_RX
  * @param[in]		txFifoBuff
  * @param[in]		txFifoBuffLen
  * @param[in]		rxFifoBuff
  * @param[in]		rxFifoBuffLen
  * @param[in]		irqPriority
  * @param[in]		do_LIN_MASTER_BREAK
  * @param[in]		linMasterBreakLength_forLoopCycles
  * @param[in]		do_DBG_Tx
  * @param[in]		do_DBG_Rx
  */
void AlxSerialPort_Ctor
(
	AlxSerialPort* me,
	AlxSerialPort_Config config,
	void* hw,
	AlxIoPin* do_TX,
	AlxIoPin* di_RX,
	uint8_t* txFifoBuff,
	uint32_t txFifoBuffLen,
	uint8_t* rxFifoBuff,
	uint32_t rxFifoBuffLen,
	Alx_IrqPriority irqPriority,
	AlxIoPin* do_LIN_MASTER_BREAK,
	uint32_t linMasterBreakLength_forLoopCycles,
	AlxIoPin* do_DBG_Tx,
	AlxIoPin* do_DBG_Rx
)
{
	//------------------------------------------------------------------------------
	// Parameters
	//------------------------------------------------------------------------------
	me->config = config;
	me->hw = hw;
	me->do_TX = do_TX;
	me->di_RX = di_RX;
	me->txFifoBuff = txFifoBuff;
	me->txFifoBuffLen = txFifoBuffLen;
	me->rxFifoBuff = rxFifoBuff;
	me->rxFifoBuffLen = rxFifoBuffLen;
	me->irqPriority = irqPriority;
	me->do_LIN_MASTER_BREAK = do_LIN_MASTER_BREAK;
	me->linMasterBreakLength_forLoopCycles = linMasterBreakLength_forLoopCycles;
	me->do_DBG_Tx = do_DBG_Tx;
	me->do_DBG_Rx = do_DBG_Rx;


	//------------------------------------------------------------------------------
	// Parameters - Private
	//------------------------------------------------------------------------------

	// txFifoUsed
	if
	(
		(config == AlxSerialPort_Config_Standard_TxIrqFifo_RxIrqFifo) ||
		(config == AlxSerialPort_Config_Standard_TxIrqFifo_RxIrqCallback) ||
		(config == AlxSerialPort_Config_LinMaster_TxIrqFifo_RxIrqFifo) ||
		(config == AlxSerialPort_Config_LinMaster_TxIrqFifo_RxIrqCallback) ||
		(config == AlxSerialPort_Config_LinSlave_TxIrqFifo_RxIrqFifo) ||
		(config == AlxSerialPort_Config_LinSlave_TxIrqFifo_RxIrqCallback))
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
		(config == AlxSerialPort_Config_Standard_TxBlocking_RxIrqFifo) ||
		(config == AlxSerialPort_Config_Standard_TxIrqFifo_RxIrqFifo) ||
		(config == AlxSerialPort_Config_LinMaster_TxBlocking_RxIrqFifo) ||
		(config == AlxSerialPort_Config_LinMaster_TxIrqFifo_RxIrqFifo) ||
		(config == AlxSerialPort_Config_LinSlave_TxBlocking_RxIrqFifo) ||
		(config == AlxSerialPort_Config_LinSlave_TxIrqFifo_RxIrqFifo)
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
		(config == AlxSerialPort_Config_LinMaster_TxBlocking_RxIrqFifo) ||
		(config == AlxSerialPort_Config_LinMaster_TxBlocking_RxIrqCallback) ||
		(config == AlxSerialPort_Config_LinMaster_TxIrqFifo_RxIrqFifo) ||
		(config == AlxSerialPort_Config_LinMaster_TxIrqFifo_RxIrqCallback)
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
		(config == AlxSerialPort_Config_LinSlave_TxBlocking_RxIrqFifo) ||
		(config == AlxSerialPort_Config_LinSlave_TxBlocking_RxIrqCallback) ||
		(config == AlxSerialPort_Config_LinSlave_TxIrqFifo_RxIrqFifo) ||
		(config == AlxSerialPort_Config_LinSlave_TxIrqFifo_RxIrqCallback)
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
	memset(&me->descr, 0, sizeof(me->descr));
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

	// Enable UART periphery clock
	_pm_enable_bus_clock(PM_BUS_APBC, me->hw);
	AlxSerialPort_Periph_EnableClk(me);

	// Init UART
	usart_sync_init(&me->descr, me->hw, ALX_NULL);	// TV: Always returns OK, resets periphery
	usart_sync_enable(&me->descr);					// TV: Always returns OK

	// Enable UART IRQ
	hri_sercomusart_set_INTEN_RXC_bit(me->hw);
	if (me->linSlave) hri_sercomusart_set_INTEN_RXBRK_bit(me->hw);
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
	if (me->txFifoUsed) hri_sercomusart_clear_INTEN_DRE_bit(me->hw);
	hri_sercomusart_clear_INTEN_RXC_bit(me->hw);
	if (me->linSlave) hri_sercomusart_clear_INTEN_RXBRK_bit(me->hw);

	// DeInit UART
	usart_sync_deinit(&me->descr);	// TV: Always returns OK, disables & resets periphery

	// Disable UART periphery clock
	_pm_disable_bus_clock(PM_BUS_APBC, me->hw);
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
	Alx_Status status = Alx_Err;
	struct io_descriptor *io = NULL;
	usart_sync_get_io_descriptor(&me->descr, &io);


	//------------------------------------------------------------------------------
	// LIN TX Break
	//------------------------------------------------------------------------------
	if (me->linMaster)
	{
		if (me->do_LIN_MASTER_BREAK != NULL)
		{
			// Set GPIO to LOW
			AlxIoPin_DeInit(me->do_TX);
			AlxIoPin_Init(me->do_LIN_MASTER_BREAK);

			// Wait
			for (volatile uint32_t i = 0; i < me->linMasterBreakLength_forLoopCycles; i++);

			// Set GPIO to HIGH
			AlxIoPin_DeInit(me->do_LIN_MASTER_BREAK);
			AlxIoPin_Init(me->do_TX);
		}
	}


	//------------------------------------------------------------------------------
	// Write
	//------------------------------------------------------------------------------
	if (me->txFifoUsed)
	{
		// Write TX FIFO
		AlxGlobal_DisableIrq();
		status = AlxFifo_WriteMulti(&me->txFifo, data, len);
		AlxGlobal_EnableIrq();

		// If UART TX IRQ NOT enabled, enable it
		if (hri_sercomusart_get_INTEN_DRE_bit(me->hw) == false)
		{
			hri_sercomusart_set_INTEN_DRE_bit(me->hw);
		}
	}
	else
	{
		// TX data blocking
		if (me->do_DBG_Tx != NULL) AlxIoPin_Set(me->do_DBG_Tx);
		int32_t len_Expected = io_write(io, data, len);
		if (me->do_DBG_Tx != NULL) AlxIoPin_Reset(me->do_DBG_Tx);
		if ((int32_t)len != len_Expected)
		{
			// Trace
			ALX_SERIAL_PORT_TRACE("FAIL: io_write() len %ld len_Expected %ld", len, len_Expected);

			// Reset
			status = AlxSerialPort_Reset(me);
			if (status != Alx_Ok)
			{
				ALX_SERIAL_PORT_TRACE("FAIL: AlxSerialPort_Reset() status %u", status);
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
		if (hri_sercomusart_get_INTFLAG_DRE_bit(me->hw))
		{
			uint8_t data = 0;
			Alx_Status status = AlxFifo_Read(&me->txFifo, &data, sizeof(data));
			if (status == Alx_Ok)
			{
				// TX data from TX FIFO
				if (me->do_DBG_Tx != NULL) AlxIoPin_Set(me->do_DBG_Tx);
				hri_sercomusart_write_DATA_reg(me->hw, data);	// Clears DRE
				if (me->do_DBG_Tx != NULL) AlxIoPin_Reset(me->do_DBG_Tx);
			}
			else
			{
				// Disable UART TX IRQ, no more data in TX FIFO
				hri_sercomusart_clear_INTEN_DRE_bit(me->hw);
			}
		}
	}


	//------------------------------------------------------------------------------
	// RX Data
	//------------------------------------------------------------------------------
	if (hri_sercomusart_get_INTFLAG_RXC_bit(me->hw))
	{
		uint8_t data = hri_sercomusart_read_DATA_reg(me->hw);	// Clears RXC
		if (me->rxFifoUsed)
		{
			AlxFifo_Write(&me->rxFifo, data);
		}
		else
		{
			AlxSerialPort_RxData_Callback(me, data);
		}
	}


	//------------------------------------------------------------------------------
	// LIN Slave RX Break
	//------------------------------------------------------------------------------
	if (me->linSlave)
	{
		if (hri_sercomusart_get_INTFLAG_RXBRK_bit(me->hw))
		{
			hri_sercomusart_clear_INTFLAG_RXBRK_bit(me->hw);
			AlxSerialPort_LinSlaveRxBreak_Callback(me);
		}
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
	ALX_SERIAL_PORT_ASSERT(me->rxFifoUsed == true);

	// Flush
	AlxFifo_Flush(&me->rxFifo);
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
	ALX_SERIAL_PORT_ASSERT(me->rxFifoUsed == true);

	// Get
	return AlxFifo_GetNumOfEntries(&me->rxFifo);
}


//******************************************************************************
// Private Functions
//******************************************************************************
static Alx_Status AlxSerialPort_Reset(AlxSerialPort* me)
{
	// Disable UART IRQ
	AlxSerialPort_Periph_DisableIrq(me);
	if (me->txFifoUsed) hri_sercomusart_clear_INTEN_DRE_bit(me->hw);
	hri_sercomusart_clear_INTEN_RXC_bit(me->hw);
	if (me->linSlave) hri_sercomusart_clear_INTEN_RXBRK_bit(me->hw);

	// DeInit UART
	usart_sync_deinit(&me->descr);	// TV: Always returns OK, disables & resets periphery

	// Flush TX & RX FIFO
	if (me->txFifoUsed) AlxFifo_Flush(&me->txFifo);
	if (me->rxFifoUsed) AlxFifo_Flush(&me->rxFifo);

	// Clear isInit
	me->isInit = false;

	// Init UART
	usart_sync_init(&me->descr, me->hw, ALX_NULL);	// TV: Always returns OK, resets periphery
	usart_sync_enable(&me->descr);					// TV: Always returns OK

	// Enable UART IRQ
	hri_sercomusart_set_INTEN_RXC_bit(me->hw);
	if (me->linSlave) hri_sercomusart_set_INTEN_RXBRK_bit(me->hw);
	AlxSerialPort_Periph_EnableIrq(me);

	// Set isInit
	me->isInit = true;

	// Return
	return Alx_Ok;
}
static void AlxSerialPort_Periph_EnableClk(AlxSerialPort* me)
{
	#ifdef SERCOM0
	if (me->hw == SERCOM0)	{ _gclk_enable_channel(SERCOM0_GCLK_ID_CORE, CONF_GCLK_SERCOM0_CORE_SRC); return; }
	#endif
	#ifdef SERCOM1
	if (me->hw == SERCOM1)	{ _gclk_enable_channel(SERCOM1_GCLK_ID_CORE, CONF_GCLK_SERCOM1_CORE_SRC); return; }
	#endif
	#ifdef SERCOM2
	if (me->hw == SERCOM2)	{ _gclk_enable_channel(SERCOM2_GCLK_ID_CORE, CONF_GCLK_SERCOM2_CORE_SRC); return; }
	#endif
	#ifdef SERCOM3
	if (me->hw == SERCOM3)	{ _gclk_enable_channel(SERCOM3_GCLK_ID_CORE, CONF_GCLK_SERCOM3_CORE_SRC); return; }
	#endif
	#ifdef SERCOM4
	if (me->hw == SERCOM4)	{ _gclk_enable_channel(SERCOM4_GCLK_ID_CORE, CONF_GCLK_SERCOM4_CORE_SRC); return; }
	#endif
	#ifdef SERCOM5
	if (me->hw == SERCOM5)	{ _gclk_enable_channel(SERCOM5_GCLK_ID_CORE, CONF_GCLK_SERCOM5_CORE_SRC); return; }
	#endif

	ALX_SERIAL_PORT_ASSERT(false);	// We should not get here
}
static void AlxSerialPort_Periph_DisableClk(AlxSerialPort* me)
{
	// TV: TODO
}
static void AlxSerialPort_Periph_EnableIrq(AlxSerialPort* me)
{
	#if defined(SERCOM0)
	if (me->hw == SERCOM0)	{ NVIC_SetPriority(SERCOM0_IRQn, me->irqPriority); NVIC_EnableIRQ(SERCOM0_IRQn); return; }
	#endif
	#if defined(SERCOM1)
	if (me->hw == SERCOM1)	{ NVIC_SetPriority(SERCOM1_IRQn, me->irqPriority); NVIC_EnableIRQ(SERCOM1_IRQn); return; }
	#endif
	#if defined(SERCOM2)
	if (me->hw == SERCOM2)	{ NVIC_SetPriority(SERCOM2_IRQn, me->irqPriority); NVIC_EnableIRQ(SERCOM2_IRQn); return; }
	#endif
	#if defined(SERCOM3)
	if (me->hw == SERCOM3)	{ NVIC_SetPriority(SERCOM3_IRQn, me->irqPriority); NVIC_EnableIRQ(SERCOM3_IRQn); return; }
	#endif
	#if defined(SERCOM4)
	if (me->hw == SERCOM4)	{ NVIC_SetPriority(SERCOM4_IRQn, me->irqPriority); NVIC_EnableIRQ(SERCOM4_IRQn); return; }
	#endif
	#if defined(SERCOM5)
	if (me->hw == SERCOM5)	{ NVIC_SetPriority(SERCOM5_IRQn, me->irqPriority); NVIC_EnableIRQ(SERCOM5_IRQn); return; }
	#endif

	ALX_SERIAL_PORT_ASSERT(false);	// We should not get here
}
static void AlxSerialPort_Periph_DisableIrq(AlxSerialPort* me)
{
	#if defined(SERCOM0)
	if (me->hw == SERCOM0)	{ NVIC_DisableIRQ(SERCOM0_IRQn); NVIC_ClearPendingIRQ(SERCOM0_IRQn); return; }
	#endif
	#if defined(SERCOM1)
	if (me->hw == SERCOM1)	{ NVIC_DisableIRQ(SERCOM1_IRQn); NVIC_ClearPendingIRQ(SERCOM1_IRQn); return; }
	#endif
	#if defined(SERCOM2)
	if (me->hw == SERCOM2)	{ NVIC_DisableIRQ(SERCOM2_IRQn); NVIC_ClearPendingIRQ(SERCOM2_IRQn); return; }
	#endif
	#if defined(SERCOM3)
	if (me->hw == SERCOM3)	{ NVIC_DisableIRQ(SERCOM3_IRQn); NVIC_ClearPendingIRQ(SERCOM3_IRQn); return; }
	#endif
	#if defined(SERCOM4)
	if (me->hw == SERCOM4)	{ NVIC_DisableIRQ(SERCOM4_IRQn); NVIC_ClearPendingIRQ(SERCOM4_IRQn); return; }
	#endif
	#if defined(SERCOM5)
	if (me->hw == SERCOM5)	{ NVIC_DisableIRQ(SERCOM5_IRQn); NVIC_ClearPendingIRQ(SERCOM5_IRQn); return; }
	#endif

	ALX_SERIAL_PORT_ASSERT(false);	// We should not get here
}


//******************************************************************************
// Weak Functions
//******************************************************************************
ALX_WEAK void AlxSerialPort_RxData_Callback(AlxSerialPort* me, uint8_t data)
{
	(void)me;
	(void)data;
}
ALX_WEAK void AlxSerialPort_LinSlaveRxBreak_Callback(AlxSerialPort* me)
{
	(void)me;
}


#endif	// #if defined(ALX_C_LIB) && defined(ALX_SAM)
