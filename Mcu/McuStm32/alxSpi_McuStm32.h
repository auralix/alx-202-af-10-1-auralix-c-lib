/**
  ******************************************************************************
  * @file alxSpi_McuStm32.h
  * @brief Auralix C Library - ALX SPI Module
  * @version $LastChangedRevision: 5451 $
  * @date $LastChangedDate: 2021-06-18 23:31:45 +0200 (Fri, 18 Jun 2021) $
  ******************************************************************************
  */

#ifndef ALX_SPI_MCU_STM32_H
#define ALX_SPI_MCU_STM32_H

#ifdef __cplusplus
extern "C" {
#endif

//******************************************************************************
// Includes
//******************************************************************************
#include "alxGlobal.h"
#include "alxTrace.h"
#include "alxAssert.h"
#include "alxIoPin.h"
#include "alxClk.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_STM32F4) || defined(ALX_STM32G4) || defined(ALX_STM32L0)


//******************************************************************************
// Types
//******************************************************************************
typedef enum
{
	#if defined(STM32F4)
	AlxSpi_Clk_McuStm32F4_Spi1_Spi4_SpiClk_1MHz4_Pclk2Apb2_90MHz = SPI_BAUDRATEPRESCALER_64,
	AlxSpi_Clk_McuStm32F4_Spi1_Spi4_SpiClk_5MHz625_Pclk2Apb2_90MHz = SPI_BAUDRATEPRESCALER_16,
	AlxSpi_Clk_McuStm32F4_Spi1_Spi4_SpiClk_11MHz25_Pclk2Apb2_90MHz = SPI_BAUDRATEPRESCALER_8,
	AlxSpi_Clk_McuStm32F4_Spi2_Spi3_SpiClk_1MHz4_Pclk1Apb1_45MHz = SPI_BAUDRATEPRESCALER_32,
	AlxSpi_Clk_McuStm32F4_Spi2_Spi3_SpiClk_5MHz625_Pclk1Apb1_45MHz = SPI_BAUDRATEPRESCALER_8,
	AlxSpi_Clk_McuStm32F4_Spi2_Spi3_SpiClk_11MHz25_Pclk1Apb1_45MHz = SPI_BAUDRATEPRESCALER_4,
	#endif
	#if defined(STM32G4)
	AlxSpi_Clk_McuStm32G4_Spi1_Spi4_SpiClk_1MHz33_Pclk2Apb2_170MHz = SPI_BAUDRATEPRESCALER_128,
	AlxSpi_Clk_McuStm32G4_Spi2_Spi3_SpiClk_1MHz33_Pclk1Apb1_170MHz = SPI_BAUDRATEPRESCALER_128,
	#endif
	#if defined(STM32L0)
	AlxSpi_Clk_McuStm32L0_Spi1_SpiClk_1MHz_Pclk2Apb2_32MHz = SPI_BAUDRATEPRESCALER_32,
	AlxSpi_Clk_McuStm32L0_Spi2_SpiClk_1MHz_Pclk1Apb1_32MHz = SPI_BAUDRATEPRESCALER_32,
	#endif
} AlxSpi_Clk;

typedef enum
{
	AlxSpi_Mode_0 = 0,	// CPOL = 0, CPHA = 0 --> Clock is idle at 0, data is sampled at leading clock edge, shifted at trailing clock edge
	AlxSpi_Mode_1 = 1,	// CPOL = 0, CPHA = 1 --> Clock is idle at 0, data is sampled at trailing clock edge, shifted at leading clock edge
	AlxSpi_Mode_2 = 2,	// CPOL = 1, CPHA = 0 --> Clock is idle at 1, data is sampled at leading clock edge, shifted at trailing clock edge
	AlxSpi_Mode_3 = 3	// CPOL = 1, CPHA = 1 --> Clock is idle at 1, data is sampled at trailing clock edge, shifted at leading clock edge
} AlxSpi_Mode;

typedef struct
{
	// Objects - External
	AlxIoPin* do_SCK;
	AlxIoPin* do_MOSI;
	AlxIoPin* di_MISO;
	AlxIoPin* do_nCS;
	AlxClk* clk;

	// Parameters
	AlxSpi_Mode mode;
	AlxSpi_Clk spiClk;

	// Variables
	uint32_t clkPolarity;
	uint32_t clkPhase;
	SPI_HandleTypeDef hspi;

	// Info
	bool isInit;
	bool wasCtorCalled;
} AlxSpi;


//******************************************************************************
// Constructor
//******************************************************************************
void AlxSpi_Ctor
(
	AlxSpi* me,
	SPI_TypeDef* spi,
	AlxIoPin* do_SCK,
	AlxIoPin* do_MOSI,
	AlxIoPin* di_MISO,
	AlxIoPin* do_nCS,
	AlxSpi_Mode mode,
	AlxClk* clk,
	AlxSpi_Clk spiClk
);


#endif

#ifdef __cplusplus
}
#endif

#endif // ALX_SPI_MCU_STM32_H
