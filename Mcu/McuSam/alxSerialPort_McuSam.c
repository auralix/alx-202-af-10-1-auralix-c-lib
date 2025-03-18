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
// Constructor
//******************************************************************************

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		hw
  * @param[in]		do_TX
  * @param[in]		di_RX
  * @param[in]		rxFifoBuff
  * @param[in]		rxFifoBuffLen
  * @param[in]		rxIrqPriority
  * @param[in]		lin
  */
void AlxSerialPort_Ctor
(
	AlxSerialPort* me,
	void* hw,
	AlxIoPin* do_TX,
	AlxIoPin* di_RX,
	uint8_t* rxFifoBuff,
	uint32_t rxFifoBuffLen,
	Alx_IrqPriority rxIrqPriority,
	AlxSerialPort_Lin lin
)
{
	// Parameters
	me->hw = hw;
	me->do_TX = do_TX;
	me->di_RX = di_RX;
	me->rxFifoBuff = rxFifoBuff;
	me->rxFifoBuffLen = rxFifoBuffLen;
	me->rxIrqPriority = rxIrqPriority;
	me->lin = lin;

	// Variables
	memset(&me->descr, 0, sizeof(me->descr));
	AlxFifo_Ctor(&me->rxFifo, rxFifoBuff, rxFifoBuffLen);

	// Info
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

	// Flush RX FIFO
	AlxFifo_Flush(&me->rxFifo);

	// Init GPIO
	AlxIoPin_Init(me->do_TX);
	AlxIoPin_Init(me->di_RX);

	// Enable USART clock
	_pm_enable_bus_clock(PM_BUS_APBC, me->hw);
	AlxSerialPort_Periph_EnableClk(me);

	// Init USART
	usart_sync_init(&me->descr, me->hw, ALX_NULL);	// TV: Always returns OK, resets periphery
	usart_sync_enable(&me->descr);					// TV: Always returns OK

	// Enable USART RX IRQ
	hri_sercomusart_set_INTEN_RXC_bit(me->hw);
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

	// Disable UART RX IRQ
	AlxSerialPort_Periph_DisableIrq(me);
	hri_sercomusart_clear_INTEN_RXC_bit(me->hw);

	// DeInit USART
	usart_sync_deinit(&me->descr);	// TV: Always returns OK, disables & resets periphery

	// Disable USART clock
	_pm_disable_bus_clock(PM_BUS_APBC, me->hw);
	AlxSerialPort_Periph_DisableClk(me);

	// DeInit GPIO
	AlxIoPin_DeInit(me->do_TX);
	AlxIoPin_DeInit(me->di_RX);

	// Flush RX FIFO
	AlxFifo_Flush(&me->rxFifo);

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

	// Read
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
	ALX_SERIAL_PORT_ASSERT(me->lin == AlxSerialPort_Lin_Disable);

	// Read
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
	// Assert
	ALX_SERIAL_PORT_ASSERT(me->wasCtorCalled == true);
	ALX_SERIAL_PORT_ASSERT(me->isInit == true);

	// Prepare
	struct io_descriptor *io = NULL;
	usart_sync_get_io_descriptor(&me->descr, &io);

	// Write
	int32_t len_Expected = io_write(io, data, len);
	if ((uint16_t)len != len_Expected)
	{
		ALX_SERIAL_PORT_TRACE("Err");
		if(AlxSerialPort_Reset(me) != Alx_Ok) { ALX_SERIAL_PORT_TRACE("Err"); };
		return Alx_Err;
	}

	// Return
	return Alx_Ok;
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
	ALX_SERIAL_PORT_ASSERT(me->lin == AlxSerialPort_Lin_Disable);

	// Return
	return AlxSerialPort_Write(me, (const uint8_t*)str, strlen(str));
}

/**
  * @brief
  * @param[in,out]	me
  */
void AlxSerialPort_IrqHandler(AlxSerialPort* me)
{
	uint8_t data = hri_sercomusart_read_DATA_reg(me->hw);
	AlxFifo_Write(&me->rxFifo, data);
}

/**
  * @brief
  * @param[in,out]	me
  */
void AlxSerialPort_FlushRxFifo(AlxSerialPort* me)
{
	AlxFifo_Flush(&me->rxFifo);
}

/**
  * @brief
  * @param[in,out]	me
  * @return
  */
uint32_t AlxSerialPort_GetRxFifoNumOfEntries(AlxSerialPort* me)
{
	return AlxFifo_GetNumOfEntries(&me->rxFifo);
}


//******************************************************************************
// Private Functions
//******************************************************************************
static Alx_Status AlxSerialPort_Reset(AlxSerialPort* me)
{
	// Disable UART RX IRQ
	AlxSerialPort_Periph_DisableIrq(me);
	hri_sercomusart_clear_INTEN_RXC_bit(me->hw);

	// DeInit USART
	usart_sync_deinit(&me->descr);	// TV: Always returns OK, disables & resets periphery

	// Flush RX FIFO
	AlxFifo_Flush(&me->rxFifo);

	// Clear isInit
	me->isInit = false;

	// Init USART
	usart_sync_init(&me->descr, me->hw, ALX_NULL);	// TV: Always returns OK, resets periphery
	usart_sync_enable(&me->descr);					// TV: Always returns OK

	// Enable USART RX IRQ
	hri_sercomusart_set_INTEN_RXC_bit(me->hw);
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
	if (me->hw == SERCOM0)	{ NVIC_SetPriority(SERCOM0_IRQn, me->rxIrqPriority); NVIC_EnableIRQ(SERCOM0_IRQn); return; }
	#endif
	#if defined(SERCOM1)
	if (me->hw == SERCOM1)	{ NVIC_SetPriority(SERCOM1_IRQn, me->rxIrqPriority); NVIC_EnableIRQ(SERCOM1_IRQn); return; }
	#endif
	#if defined(SERCOM2)
	if (me->hw == SERCOM2)	{ NVIC_SetPriority(SERCOM2_IRQn, me->rxIrqPriority); NVIC_EnableIRQ(SERCOM2_IRQn); return; }
	#endif
	#if defined(SERCOM3)
	if (me->hw == SERCOM3)	{ NVIC_SetPriority(SERCOM3_IRQn, me->rxIrqPriority); NVIC_EnableIRQ(SERCOM3_IRQn); return; }
	#endif
	#if defined(SERCOM4)
	if (me->hw == SERCOM4)	{ NVIC_SetPriority(SERCOM4_IRQn, me->rxIrqPriority); NVIC_EnableIRQ(SERCOM4_IRQn); return; }
	#endif
	#if defined(SERCOM5)
	if (me->hw == SERCOM5)	{ NVIC_SetPriority(SERCOM5_IRQn, me->rxIrqPriority); NVIC_EnableIRQ(SERCOM5_IRQn); return; }
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


#endif	// #if defined(ALX_C_LIB) && defined(ALX_SAM)
