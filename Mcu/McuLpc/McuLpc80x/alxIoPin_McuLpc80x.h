/**
  ******************************************************************************
  * @file alxIoPin_McuLpc80x.h
  * @brief Auralix C Library - ALX IO Pin Module
  * @version $LastChangedRevision: 4270 $
  * @date $LastChangedDate: 2021-03-05 19:02:52 +0100 (Fri, 05 Mar 2021) $
  ******************************************************************************
  */

#ifndef ALX_IO_PIN_MCU_LPC80x_H
#define ALX_IO_PIN_MCU_LPC80x_H

#ifdef __cplusplus
extern "C" {
#endif

//******************************************************************************
// Includes
//******************************************************************************
#include "alxGlobal.h"
#include "alxTrace.h"
#include "alxAssert.h"
#include "alxDbgPin.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_LPC80x)


//******************************************************************************
// Types
//******************************************************************************
#if defined(ALX_LPC80x)
typedef enum
{
	// SWM Movable
	AlxIoPin_Func_Swm_USART0_TXD = 0U,
	AlxIoPin_Func_Swm_USART0_RXD = 1U,
	AlxIoPin_Func_Swm_USART0_RTS = 2U,
	AlxIoPin_Func_Swm_USART0_CTS = 3U,
	AlxIoPin_Func_Swm_USART0_SCLK = 4U,

	AlxIoPin_Func_Swm_USART1_TXD = 5U,
	AlxIoPin_Func_Swm_USART1_RXD = 6U,
	AlxIoPin_Func_Swm_USART1_SCLK = 7U,

	AlxIoPin_Func_Swm_SPI0_SCK = 8U,
	AlxIoPin_Func_Swm_SPI0_MOSI = 9U,
	AlxIoPin_Func_Swm_SPI0_MISO = 10U,
	AlxIoPin_Func_Swm_SPI0_SSEL0 = 11U,
	AlxIoPin_Func_Swm_SPI0_SSEL1 = 12U,

	AlxIoPin_Func_Swm_T0_CAP_CHN0 = 13U,
	AlxIoPin_Func_Swm_T0_CAP_CHN1 = 14U,
	AlxIoPin_Func_Swm_T0_CAP_CHN2 = 15U,

	AlxIoPin_Func_Swm_T0_MAT_CHN0 = 16U,
	AlxIoPin_Func_Swm_T0_MAT_CHN1 = 17U,
	AlxIoPin_Func_Swm_T0_MAT_CHN2 = 18U,
	AlxIoPin_Func_Swm_T0_MAT_CHN3 = 19U,

	AlxIoPin_Func_Swm_I2C0_SDA = 20U,
	AlxIoPin_Func_Swm_I2C0_SCL = 21U,

	AlxIoPin_Func_Swm_ACMP_OUT = 22U,
	AlxIoPin_Func_Swm_CLKOUT = 23U,
	AlxIoPin_Func_Swm_GPIO_INT_BMAT = 24U,

	AlxIoPin_Func_Swm_LVLSHFT_IN0 = 25U,
	AlxIoPin_Func_Swm_LVLSHFT_IN1 = 26U,
	AlxIoPin_Func_Swm_LVLSHFT_OUT0 = 27U,
	AlxIoPin_Func_Swm_LVLSHFT_OUT1 = 28U,

	AlxIoPin_Func_Swm_I2C1_SDA = 29U,
	AlxIoPin_Func_Swm_I2C1_SCL = 30U,

	AlxIoPin_Func_Swm_PLU_CLKIN_IN = 31U,

	AlxIoPin_Func_Swm_CAPT_X0 = 32U,
	AlxIoPin_Func_Swm_CAPT_X1 = 33U,
	AlxIoPin_Func_Swm_CAPT_X2 = 34U,
	AlxIoPin_Func_Swm_CAPT_X3 = 35U,
	AlxIoPin_Func_Swm_CAPT_X4 = 36U,
	AlxIoPin_Func_Swm_CAPT_YL = 37U,
	AlxIoPin_Func_Swm_CAPT_YH = 38U,

	// SWM Fixed
	AlxIoPin_Func_Swm_ACMP_INPUT1 = 39U,
	AlxIoPin_Func_Swm_ACMP_INPUT2 = 40U,
	AlxIoPin_Func_Swm_ACMP_INPUT3 = 41U,
	AlxIoPin_Func_Swm_ACMP_INPUT4 = 42U,

	AlxIoPin_Func_Swm_SWCLK = 43U,
	AlxIoPin_Func_Swm_SWDIO = 44U,
	AlxIoPin_Func_Swm_RESETN = 45U,
	AlxIoPin_Func_Swm_CLKIN = 46U,
	AlxIoPin_Func_Swm_WKCLKIN = 47U,
	AlxIoPin_Func_Swm_VDDCMP = 48U,

	AlxIoPin_Func_Swm_ADC_CHN0 = 49U,
	AlxIoPin_Func_Swm_ADC_CHN1 = 50U,
	AlxIoPin_Func_Swm_ADC_CHN2 = 51U,
	AlxIoPin_Func_Swm_ADC_CHN3 = 52U,
	AlxIoPin_Func_Swm_ADC_CHN4 = 53U,
	AlxIoPin_Func_Swm_ADC_CHN5 = 54U,
	AlxIoPin_Func_Swm_ADC_CHN6 = 55U,
	AlxIoPin_Func_Swm_ADC_CHN7 = 56U,
	AlxIoPin_Func_Swm_ADC_CHN8 = 57U,
	AlxIoPin_Func_Swm_ADC_CHN9 = 58U,
	AlxIoPin_Func_Swm_ADC_CHN10 = 59U,
	AlxIoPin_Func_Swm_ADC_CHN11 = 60U,

	AlxIoPin_Func_Swm_ACMP_INPUT5 = 61U,
	AlxIoPin_Func_Swm_DAC_OUT0 = 62U,

	// IRQ
	AlxIoPin_Func_IRQ = 254U,

	// GPIO
	AlxIoPin_Func_GPIO = 255U
} AlxIoPin_Func;
#endif
#if defined(ALX_LPC84x)
typedef enum
{
	// Movable
	AlxIoPin_Func_Swm_USART0_TXD = 0U,
	AlxIoPin_Func_Swm_USART0_RXD = 1U,
	AlxIoPin_Func_Swm_USART0_RTS = 2U,
	AlxIoPin_Func_Swm_USART0_CTS = 3U,
	AlxIoPin_Func_Swm_USART0_SCLK = 4U,
	AlxIoPin_Func_Swm_USART1_TXD = 5U,
	AlxIoPin_Func_Swm_USART1_RXD = 6U,
	AlxIoPin_Func_Swm_USART1_RTS = 7U,
	AlxIoPin_Func_Swm_USART1_CTS = 8U,
	AlxIoPin_Func_Swm_USART1_SCLK = 9U,

	AlxIoPin_Func_Swm_USART2_TXD = 10U,
	AlxIoPin_Func_Swm_USART2_RXD = 11U,
	AlxIoPin_Func_Swm_USART2_RTS = 12U,
	AlxIoPin_Func_Swm_USART2_CTS = 13U,
	AlxIoPin_Func_Swm_USART2_SCLK = 14U,

	AlxIoPin_Func_Swm_SPI0_SCK = 15U,
	AlxIoPin_Func_Swm_SPI0_MOSI = 16U,
	AlxIoPin_Func_Swm_SPI0_MISO = 17U,
	AlxIoPin_Func_Swm_SPI0_SSEL0 = 18U,
	AlxIoPin_Func_Swm_SPI0_SSEL1 = 19U,
	AlxIoPin_Func_Swm_SPI0_SSEL2 = 20U,
	AlxIoPin_Func_Swm_SPI0_SSEL3 = 21U,

	AlxIoPin_Func_Swm_SPI1_SCK = 22U,
	AlxIoPin_Func_Swm_SPI1_MOSI = 23U,
	AlxIoPin_Func_Swm_SPI1_MISO = 24U,
	AlxIoPin_Func_Swm_SPI1_SSEL0 = 25U,
	AlxIoPin_Func_Swm_SPI1_SSEL1 = 26U,
	AlxIoPin_Func_Swm_SCT_PIN0 = 27U,
	AlxIoPin_Func_Swm_SCT_PIN1 = 28U,
	AlxIoPin_Func_Swm_SCT_PIN2 = 29U,
	AlxIoPin_Func_Swm_SCT_PIN3 = 30U,
	AlxIoPin_Func_Swm_SCT_OUT0 = 31U,
	AlxIoPin_Func_Swm_SCT_OUT1 = 32U,
	AlxIoPin_Func_Swm_SCT_OUT2 = 33U,
	AlxIoPin_Func_Swm_SCT_OUT3 = 34U,
	AlxIoPin_Func_Swm_SCT_OUT4 = 35U,
	AlxIoPin_Func_Swm_SCT_OUT5 = 36U,
	AlxIoPin_Func_Swm_SCT_OUT6 = 37U,
	AlxIoPin_Func_Swm_I2C1_SDA = 38U,
	AlxIoPin_Func_Swm_I2C1_SCL = 39U,
	AlxIoPin_Func_Swm_I2C2_SDA = 40U,
	AlxIoPin_Func_Swm_I2C2_SCL = 41U,
	AlxIoPin_Func_Swm_I2C3_SDA = 42U,
	AlxIoPin_Func_Swm_I2C3_SCL = 43U,

	AlxIoPin_Func_Swm_ACMP_OUT = 44U,
	AlxIoPin_Func_Swm_CLKOUT = 45U,
	AlxIoPin_Func_Swm_GPIO_INT_BMAT = 46U,

	AlxIoPin_Func_Swm_USART3_TXD = 47U,
	AlxIoPin_Func_Swm_USART3_RXD = 48U,
	AlxIoPin_Func_Swm_USART3_SCLK = 49U,

	AlxIoPin_Func_Swm_USART4_TXD = 50U,
	AlxIoPin_Func_Swm_USART4_RXD = 51U,
	AlxIoPin_Func_Swm_USART4_SCLK = 52U,
	AlxIoPin_Func_Swm_T0_MAT_CHN0 = 53U,
	AlxIoPin_Func_Swm_T0_MAT_CHN1 = 54U,
	AlxIoPin_Func_Swm_T0_MAT_CHN2 = 55U,
	AlxIoPin_Func_Swm_T0_MAT_CHN3 = 56U,
	AlxIoPin_Func_Swm_T0_CAP_CHN0 = 57U,
	AlxIoPin_Func_Swm_T0_CAP_CHN1 = 58U,
	AlxIoPin_Func_Swm_T0_CAP_CHN2 = 59U,

	// Fixed
	AlxIoPin_Func_Swm_ACMP_INPUT1 = 60U,
	AlxIoPin_Func_Swm_ACMP_INPUT2 = 61U,
	AlxIoPin_Func_Swm_ACMP_INPUT3 = 62U,
	AlxIoPin_Func_Swm_ACMP_INPUT4 = 63U,
	AlxIoPin_Func_Swm_ACMP_INPUT5 = 64U,

	AlxIoPin_Func_Swm_SWCLK = 65U,
	AlxIoPin_Func_Swm_SWDIO = 66U,
	AlxIoPin_Func_Swm_XTALIN = 67U,
	AlxIoPin_Func_Swm_XTALOUT = 68U,
	AlxIoPin_Func_Swm_RESETN = 69U,
	AlxIoPin_Func_Swm_CLKIN = 70U,
	AlxIoPin_Func_Swm_VDDCMP = 71U,
	AlxIoPin_Func_Swm_I2C0_SDA = 72U,
	AlxIoPin_Func_Swm_I2C0_SCL = 73U,

	AlxIoPin_Func_Swm_ADC_CHN0 = 74U,
	AlxIoPin_Func_Swm_ADC_CHN1 = 75U,
	AlxIoPin_Func_Swm_ADC_CHN2 = 76U,
	AlxIoPin_Func_Swm_ADC_CHN3 = 77U,
	AlxIoPin_Func_Swm_ADC_CHN4 = 78U,
	AlxIoPin_Func_Swm_ADC_CHN5 = 79U,
	AlxIoPin_Func_Swm_ADC_CHN6 = 80U,
	AlxIoPin_Func_Swm_ADC_CHN7 = 81U,
	AlxIoPin_Func_Swm_ADC_CHN8 = 82U,
	AlxIoPin_Func_Swm_ADC_CHN9 = 83U,
	AlxIoPin_Func_Swm_ADC_CHN10 = 84U,
	AlxIoPin_Func_Swm_ADC_CHN11 = 85U,

	AlxIoPin_Func_Swm_DAC_OUT0 = 86U,
	AlxIoPin_Func_Swm_DAC_OUT1 = 87U,
	AlxIoPin_Func_Swm_CAPT_X0 = 88U,
	AlxIoPin_Func_Swm_CAPT_X1 = 89U,
	AlxIoPin_Func_Swm_CAPT_X2 = 90U,
	AlxIoPin_Func_Swm_CAPT_X3 = 91U,
	AlxIoPin_Func_Swm_CAPT_X4 = 92U,
	AlxIoPin_Func_Swm_CAPT_X5 = 93U,
	AlxIoPin_Func_Swm_CAPT_X6 = 94U,
	AlxIoPin_Func_Swm_CAPT_X7 = 95U,
	AlxIoPin_Func_Swm_CAPT_X8 = 96U,
	AlxIoPin_Func_Swm_CAPT_YL = 97U,
	AlxIoPin_Func_Swm_CAPT_YH = 98U,

	// IRQ
	AlxIoPin_Func_IRQ = 254U,

	// GPIO
	AlxIoPin_Func_GPIO = 255U
} AlxIoPin_Func;
#endif


typedef struct
{
	// Parameters
	uint8_t port;
	uint8_t pin;
	AlxIoPin_Func func;
	uint32_t mode;
	bool isOpenDrain;
	bool dir;
	bool val;

	// Variables
	bool swmFunc_isMovable;

	// Info
	bool isInit;
	bool wasCtorCalled;
} AlxIoPin;


//******************************************************************************
// Constructor
//******************************************************************************
void AlxIoPin_Ctor
(
	AlxIoPin* me,
	uint8_t port,
	uint8_t pin,
	AlxIoPin_Func func,
	uint32_t mode,
	bool isOpenDrain,
	bool dir,
	bool val
);


#endif // Module Guard

#ifdef __cplusplus
}
#endif

#endif // ALX_IO_PIN_MCU_LPC80x_H
