/**
  ******************************************************************************
  * @file		lpc17xxHostShim.h
  * @brief		Auralix C Library - Configuration Matrix Fake - LPCOpen (LPC17xx)
  * @copyright	Copyright (C) Auralix d.o.o. All rights reserved.
  *
  * Compile-only: every name of LPCOpen (LPC17xx) the alx code uses, written from
  * that use, so the configuration matrix compiles every alx line under
  * ALX_LPC17XX (Test/config/matrix.toml). Nothing here is the vendor's and
  * nothing here runs; the real one is the product's.
  *
  * As the STM32 fake does, it makes no claim about the vendor's signatures
  * and layouts, only that every name exists: one register block and one
  * structure shape hold every member the code names, a call forwards its
  * arguments to one variadic function, a constant has a value no other has.
  * Grown from the compilers' diagnostics by the scratch tool of ALX-1564
  * night goal 4; edited by hand from then on.
  ******************************************************************************
  **/

#ifndef ALX_SHIM_LPCOPEN_LPC17XXHOSTSHIM_H
#define ALX_SHIM_LPCOPEN_LPC17XXHOSTSHIM_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "cmsisHostShim.h"


typedef struct alxShimLpc17xx_Sub
{
	uint32_t adcRate;
	uint32_t bitsAccuracy;
	uint32_t burstMode;
} alxShimLpc17xx_Sub;

typedef struct alxShimLpc17xx_Struct
{
	uint32_t adcRate;
	uint32_t bitsAccuracy;
	uint32_t burstMode;
} alxShimLpc17xx_Struct;

typedef uint32_t ADC_CHANNEL_T;
typedef alxShimLpc17xx_Struct ADC_CLOCK_SETUP_T;
typedef alxShimLpc17xx_Struct LPC_USART_T;

extern alxShimLpc17xx_Struct alxShimLpc17xx_Instance;
#define ADC_CH0 (0x0100U)
#define ADC_CH1 (0x0101U)
#define ADC_CH2 (0x0102U)
#define ADC_CH3 (0x0103U)
#define ADC_CH4 (0x0104U)
#define ADC_CH5 (0x0105U)
#define ADC_CH6 (0x0106U)
#define ADC_CH7 (0x0107U)
#define ADC_DR_DONE_STAT (0x0108U)
#define ADC_START_NOW (0x0109U)
#define ADC_TRIGGERMODE_RISING (0x010AU)
#define DISABLE (0x010BU)
#define ENABLE (0x010CU)
#define IOCON_FUNC0 (0x010DU)
#define IOCON_MODE_INACT (0x010EU)
#define IOCON_MODE_PULLUP (0x010FU)
#define LPC_ADC (0x0110U)
#define LPC_GPIO (0x0111U)
#define LPC_IOCON (0x0112U)
#define SET (0x0113U)
#define SYSCTL_CLOCK_GPIO (0x0114U)
#define UART_FCR_FIFO_EN (0x0115U)
#define UART_LCR_PARITY_DIS (0x0116U)
#define UART_LCR_SBS_1BIT (0x0117U)
#define UART_LCR_WLEN8 (0x0118U)
#define UART_LSR_THRE (0x0119U)

uint32_t alxShimLpc17xx_Call(int unused, ...);
void* alxShimLpc17xx_CallPtr(int unused, ...);
#define Chip_ADC_DeInit(...) alxShimLpc17xx_Call(0, ##__VA_ARGS__)
#define Chip_ADC_EnableChannel(...) alxShimLpc17xx_Call(0, ##__VA_ARGS__)
#define Chip_ADC_Init(...) alxShimLpc17xx_Call(0, ##__VA_ARGS__)
#define Chip_ADC_ReadStatus(...) alxShimLpc17xx_Call(0, ##__VA_ARGS__)
#define Chip_ADC_ReadValue(...) alxShimLpc17xx_Call(0, ##__VA_ARGS__)
#define Chip_ADC_SetBurstCmd(...) alxShimLpc17xx_Call(0, ##__VA_ARGS__)
#define Chip_ADC_SetStartMode(...) alxShimLpc17xx_Call(0, ##__VA_ARGS__)
#define Chip_Clock_DisablePeriphClock(...) alxShimLpc17xx_Call(0, ##__VA_ARGS__)
#define Chip_Clock_EnablePeriphClock(...) alxShimLpc17xx_Call(0, ##__VA_ARGS__)
#define Chip_GPIO_GetPinState(...) alxShimLpc17xx_Call(0, ##__VA_ARGS__)
#define Chip_GPIO_SetPinDIR(...) alxShimLpc17xx_Call(0, ##__VA_ARGS__)
#define Chip_GPIO_SetPinState(...) alxShimLpc17xx_Call(0, ##__VA_ARGS__)
#define Chip_GPIO_SetPinToggle(...) alxShimLpc17xx_Call(0, ##__VA_ARGS__)
#define Chip_IOCON_DisableOD(...) alxShimLpc17xx_Call(0, ##__VA_ARGS__)
#define Chip_IOCON_EnableOD(...) alxShimLpc17xx_Call(0, ##__VA_ARGS__)
#define Chip_IOCON_PinMux(...) alxShimLpc17xx_Call(0, ##__VA_ARGS__)
#define Chip_UART_ConfigData(...) alxShimLpc17xx_Call(0, ##__VA_ARGS__)
#define Chip_UART_Init(...) alxShimLpc17xx_Call(0, ##__VA_ARGS__)
#define Chip_UART_ReadLineStatus(...) alxShimLpc17xx_Call(0, ##__VA_ARGS__)
#define Chip_UART_SendByte(...) alxShimLpc17xx_Call(0, ##__VA_ARGS__)
#define Chip_UART_SetBaud(...) alxShimLpc17xx_Call(0, ##__VA_ARGS__)
#define Chip_UART_SetupFIFOS(...) alxShimLpc17xx_Call(0, ##__VA_ARGS__)
#define Chip_UART_TXEnable(...) alxShimLpc17xx_Call(0, ##__VA_ARGS__)


#endif	// ALX_SHIM_LPCOPEN_LPC17XXHOSTSHIM_H
