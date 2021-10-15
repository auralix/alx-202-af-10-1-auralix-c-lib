/**
  ******************************************************************************
  * @file alxSerialPort_McuStm32.h
  * @brief Auralix C Library - ALX Serial Port Module
  * @version $LastChangedRevision: 5216 $
  * @date $LastChangedDate: 2021-05-24 19:38:05 +0200 (Mon, 24 May 2021) $
  ******************************************************************************
  */

#ifndef ALX_SERIAL_PORT_MCU_STM32_H
#define ALX_SERIAL_PORT_MCU_STM32_H

#ifdef __cplusplus
extern "C" {
#endif

//******************************************************************************
// Includes
//******************************************************************************
#include "alxGlobal.h"
#include "alxTrace.h"
#include "alxAssert.h"
#include "alxFifo.h"
#include "alxIoPin.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_STM32F4) || defined(ALX_STM32G4) || defined(ALX_STM32L0)


//******************************************************************************
// Types
//******************************************************************************
typedef struct
{
	// Objects - Internal
	AlxFifo rxFifo;

	// Objects - External
	AlxIoPin* do_TX;
	AlxIoPin* di_RX;

	// Parameters
	uint16_t txTimeout_ms;
	Alx_IrqPriority rxIrqPriority;

	// Variables
	UART_HandleTypeDef huart;

	// Info
	bool isInit;
	bool wasCtorCalled;
} AlxSerialPort;


//******************************************************************************
// Constructor
//******************************************************************************
void AlxSerialPort_Ctor
(
	AlxSerialPort* me,
	USART_TypeDef* uart,
	AlxIoPin* do_TX,
	AlxIoPin* di_RX,
	AlxGlobal_BaudRate baudRate,
	uint32_t dataWidth,
	uint32_t stopBits,
	uint32_t parity,
	uint16_t txTimeout_ms,
	uint8_t* rxFifoBuff,
	uint32_t rxFifoBuffLen,
	Alx_IrqPriority rxIrqPriority
);


#endif

#ifdef __cplusplus
}
#endif

#endif // ALX_SERIAL_PORT_MCU_STM32_H
