/**
  ******************************************************************************
  * @file alxSpi_McuLpc55S6x.h
  * @brief Auralix C Library - ALX SPI Module
  * @version $LastChangedRevision: 5451 $
  * @date $LastChangedDate: 2021-06-18 23:31:45 +0200 (Fri, 18 Jun 2021) $
  ******************************************************************************
  */

#ifndef ALX_SPI_MCU_Lpc55S6x_H
#define ALX_SPI_MCU_Lpc55S6x_H

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
#if defined(ALX_LPC55S6X)


//******************************************************************************
// Types
//******************************************************************************
typedef enum
{
	AlxSpi_Clk_McuLpc55S6x_SpiClk_500kHz	= 500000U,
	AlxSpi_Clk_McuLpc55S6x_SpiClk_1MHz		= 1000000U,
	AlxSpi_Clk_McuLpc55S6x_SpiClk_2MHz		= 2000000U,
	AlxSpi_Clk_McuLpc55S6x_SpiClk_3MHz		= 3000000U,
	AlxSpi_Clk_McuLpc55S6x_SpiClk_4MHz		= 4000000U,
	AlxSpi_Clk_McuLpc55S6x_SpiClk_6MHz		= 6000000U,
	AlxSpi_Clk_McuLpc55S6x_SpiClk_10MHz		= 10000000U,
	AlxSpi_Clk_McuLpc55S6x_SpiClk_12MHz		= 12000000U
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
	SPI_Type* spi;
	AlxIoPin* do_SCK;
	AlxIoPin* do_MOSI;
	AlxIoPin* di_MISO;
	AlxIoPin* do_nCS;

	// Parameters
	AlxSpi_Mode mode;
	AlxSpi_Clk spiClk;

	// Variables
	spi_master_config_t spiMasterConfig;	// MF: clkPolarity and clkPhase are in this struct
	spi_transfer_t spiTransfer;
	#if defined(ALX_FREE_RTOS)
	spi_rtos_handle_t spiMasterRtosHandle;
	#endif

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
	SPI_Type* spi,
	AlxIoPin* do_SCK,
	AlxIoPin* do_MOSI,
	AlxIoPin* di_MISO,
	AlxIoPin* do_nCS,
	AlxSpi_Mode mode,
	AlxSpi_Clk spiClk
);


#endif

#ifdef __cplusplus
}
#endif

#endif // ALX_SPI_MCU_Lpc55S6x_H
