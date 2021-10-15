/**
  ******************************************************************************
  * @file alxTrace_McuLpc17.c
  * @brief Auralix C Library - ALX Trace Module
  * @version $LastChangedRevision: 5455 $
  * @date $LastChangedDate: 2021-06-21 00:16:06 +0200 (Mon, 21 Jun 2021) $
  ******************************************************************************
  */

//******************************************************************************
// Includes
//******************************************************************************
#include "alxTrace_McuLpc17.h"
#include "alxTrace.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_LPC17)


//******************************************************************************
// Specific Functions
//******************************************************************************
void AlxTrace_Init(AlxTrace* me)
{
	// GPIO //
	Chip_IOCON_PinMux(LPC_IOCON, me->port, me->pin, IOCON_MODE_INACT, me->func);
	Chip_IOCON_DisableOD(LPC_IOCON, me->port, me->pin);
	Chip_GPIO_SetPinDIR(LPC_GPIO, me->port, me->pin, false);
	Chip_GPIO_SetPinState(LPC_GPIO, me->port, me->pin, false);

	// UART //
	Chip_UART_Init(me->uart);
	Chip_UART_SetBaud(me->uart, me->baudRate);
	Chip_UART_ConfigData(me->uart, UART_LCR_WLEN8 | UART_LCR_SBS_1BIT | UART_LCR_PARITY_DIS);
	Chip_UART_SetupFIFOS(me->uart, UART_FCR_FIFO_EN);
	Chip_UART_TXEnable(me->uart);

	me->isInit = true;
}
void AlxTrace_DeInit(AlxTrace* me)
{
	// TODO

	me->isInit = false;
}
void AlxTrace_WriteStr(AlxTrace* me, const char* str)
{
	while (*str)
	{
		while ((Chip_UART_ReadLineStatus(me->uart) & UART_LSR_THRE) == false) ; // Wait until TX buffer is empty
		Chip_UART_SendByte(me->uart, (const uint8_t)*str++);
	}
}


#endif
