/**
  ******************************************************************************
  * @file		lpc84xHostShim.h
  * @brief		Auralix C Library - Configuration Matrix Fake - MCUXpresso SDK (LPC84x)
  * @copyright	Copyright (C) Auralix d.o.o. All rights reserved.
  *
  * Compile-only: every name of MCUXpresso SDK (LPC84x) the alx code uses, written from
  * that use, so the configuration matrix compiles every alx line under
  * ALX_LPC84X (Test/config/matrix.toml). Nothing here is the vendor's and
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

#ifndef ALX_SHIM_MCUX_LPC84X_LPC84XHOSTSHIM_H
#define ALX_SHIM_MCUX_LPC84X_LPC84XHOSTSHIM_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "cmsisHostShim.h"


typedef struct alxShimLpc84x_Sub
{
	uint32_t MSTCTL;
	uint32_t MSTDAT;
	uint32_t PRESETCTRL0;
	uint32_t baudRate_Bps;
	uint32_t bitCountPerChar;
	uint32_t channelMask;
	uint32_t channelNumber;
	uint32_t clockDividerNumber;
	uint32_t clockFreq_Hz;
	uint32_t clockMode;
	uint32_t clockPolarity;
	uint32_t enableContinuousSCLK;
	uint32_t enableLockOscillator;
	uint32_t enableLowPowerMode;
	uint32_t enableMaster;
	uint32_t enableRx;
	uint32_t enableSingleStep;
	uint32_t enableSyncBypass;
	uint32_t enableTimeout;
	uint32_t enableTx;
	uint32_t enableWatchdogProtect;
	uint32_t enableWatchdogReset;
	uint32_t enableWwdt;
	uint32_t interruptMode;
	uint32_t loopback;
	uint32_t outputLogic;
	uint32_t overrunFlag;
	uint32_t parityMode;
	uint32_t pinDirection;
	uint32_t result;
	uint32_t stopBitCount;
	uint32_t syncMode;
	uint32_t thresholdCompareStatus;
	uint32_t thresholdCorssingStatus;
	uint32_t timeoutValue;
	uint32_t triggerMask;
	uint32_t triggerPolarity;
	uint32_t voltageRange;
	uint32_t warningValue;
	uint32_t windowValue;
	uint32_t* DIR;
	uint32_t* PIO;
} alxShimLpc84x_Sub;

typedef struct alxShimLpc84x_Struct
{
	uint32_t MSTCTL;
	uint32_t MSTDAT;
	uint32_t PRESETCTRL0;
	uint32_t baudRate_Bps;
	uint32_t bitCountPerChar;
	uint32_t channelMask;
	uint32_t channelNumber;
	uint32_t clockDividerNumber;
	uint32_t clockFreq_Hz;
	uint32_t clockMode;
	uint32_t clockPolarity;
	uint32_t enableContinuousSCLK;
	uint32_t enableLockOscillator;
	uint32_t enableLowPowerMode;
	uint32_t enableMaster;
	uint32_t enableRx;
	uint32_t enableSingleStep;
	uint32_t enableSyncBypass;
	uint32_t enableTimeout;
	uint32_t enableTx;
	uint32_t enableWatchdogProtect;
	uint32_t enableWatchdogReset;
	uint32_t enableWwdt;
	uint32_t interruptMode;
	uint32_t loopback;
	uint32_t outputLogic;
	uint32_t overrunFlag;
	uint32_t parityMode;
	uint32_t pinDirection;
	uint32_t result;
	uint32_t stopBitCount;
	uint32_t syncMode;
	uint32_t thresholdCompareStatus;
	uint32_t thresholdCorssingStatus;
	uint32_t timeoutValue;
	uint32_t triggerMask;
	uint32_t triggerPolarity;
	uint32_t voltageRange;
	uint32_t warningValue;
	uint32_t windowValue;
	uint32_t* DIR;
	uint32_t* PIO;
} alxShimLpc84x_Struct;

typedef alxShimLpc84x_Struct I2C_Type;
typedef alxShimLpc84x_Struct USART_Type;
typedef alxShimLpc84x_Struct adc_config_t;
typedef alxShimLpc84x_Struct adc_conv_seq_config_t;
typedef alxShimLpc84x_Struct adc_result_info_t;
typedef alxShimLpc84x_Struct gpio_pin_config_t;
typedef uint32_t i2c_direction_t;
typedef alxShimLpc84x_Struct i2c_master_config_t;
typedef uint32_t status_t;
typedef uint32_t swm_port_pin_type_t;
typedef uint32_t swm_select_fixed_pin_t;
typedef uint32_t swm_select_movable_t;
typedef alxShimLpc84x_Struct usart_config_t;
typedef alxShimLpc84x_Struct wwdt_config_t;

extern alxShimLpc84x_Struct alxShimLpc84x_Instance;
#define GPIO (&alxShimLpc84x_Instance)
#define I2C0 (&alxShimLpc84x_Instance)
#define I2C1 (&alxShimLpc84x_Instance)
#define I2C2 (&alxShimLpc84x_Instance)
#define I2C3 (&alxShimLpc84x_Instance)
#define IOCON (&alxShimLpc84x_Instance)
#define SYSCON (&alxShimLpc84x_Instance)
#define USART0 (&alxShimLpc84x_Instance)
#define USART1 (&alxShimLpc84x_Instance)
#define USART2 (&alxShimLpc84x_Instance)
#define USART3 (&alxShimLpc84x_Instance)
#define USART4 (&alxShimLpc84x_Instance)
#define ADC0 (0x0100U)
#define I2C_MSTCTL_MSTCONTINUE_MASK (0x0101U)
#define I2C_MSTCTL_MSTSTART_MASK (0x0102U)
#define I2C_MSTCTL_MSTSTOP_MASK (0x0103U)
#define I2C_STAT_MSTARBLOSS_MASK (0x0104U)
#define I2C_STAT_MSTCODE_NACKADR (0x0105U)
#define I2C_STAT_MSTCODE_NACKDAT (0x0106U)
#define I2C_STAT_MSTCODE_RXREADY (0x0107U)
#define I2C_STAT_MSTCODE_TXREADY (0x0108U)
#define I2C_STAT_MSTPENDING_MASK (0x0109U)
#define I2C_STAT_MSTSTATE_MASK (0x010AU)
#define I2C_STAT_MSTSTATE_SHIFT (11U)
#define I2C_STAT_MSTSTSTPERR_MASK (0x010CU)
#define IOCON_MODE_INACT (0x010DU)
#define IOCON_MODE_PULLDOWN (0x010EU)
#define IOCON_MODE_PULLUP (0x010FU)
#define IOCON_MODE_REPEATER (0x0110U)
#define SWM0 (0x0111U)
#define WWDT (0x0112U)
#define kADC_Clk_From_Fro (0x0113U)
#define kADC_ClockSynchronousMode (0x0114U)
#define kADC_HighVoltageRange (0x0115U)
#define kADC_InterruptForEachConversion (0x0116U)
#define kADC_RST_N_SHIFT_RSTn (7U)
#define kADC_ThresholdCompareInRange (0x0118U)
#define kADC_ThresholdCrossingNoDetected (0x0119U)
#define kADC_TriggerPolarityNegativeEdge (0x011AU)
#define kCLOCK_DivAdcClk (0x011BU)
#define kCLOCK_FroOscOut30M (0x011CU)
#define kCLOCK_FroSrcFroOsc (0x011DU)
#define kCLOCK_Gpio0 (0x011EU)
#define kCLOCK_Gpio1 (0x011FU)
#define kCLOCK_Iocon (0x0120U)
#define kCLOCK_MainClkSrcFro (0x0121U)
#define kCLOCK_Swm (0x0122U)
#define kCLOCK_WdtAnaFreq600KHZ (0x0123U)
#define kCLOCK_WdtOsc (0x0124U)
#define kGPIO0_RST_N_SHIFT_RSTn (5U)
#define kGPIO1_RST_N_SHIFT_RSTn (6U)
#define kGPIO_DigitalInput (0x0127U)
#define kGPIO_DigitalOutput (0x0128U)
#define kI2C0_Clk_From_MainClk (0x0129U)
#define kI2C1_Clk_From_MainClk (0x012AU)
#define kI2C2_Clk_From_MainClk (0x012BU)
#define kI2C3_Clk_From_MainClk (0x012CU)
#define kI2C_Read (0x012DU)
#define kI2C_TransferDefaultFlag (0x012EU)
#define kI2C_TransferNoStartFlag (0x012FU)
#define kI2C_TransferNoStopFlag (0x0130U)
#define kI2C_Write (0x0131U)
#define kPDRUNCFG_PD_ADC0 (0x0132U)
#define kPDRUNCFG_PD_FRO (0x0133U)
#define kPDRUNCFG_PD_FRO_OUT (0x0134U)
#define kPDRUNCFG_PD_WDT_OSC (0x0135U)
#define kSWM_ACMP_INPUT1 (0x0136U)
#define kSWM_ACMP_INPUT2 (0x0137U)
#define kSWM_ACMP_INPUT3 (0x0138U)
#define kSWM_ACMP_INPUT4 (0x0139U)
#define kSWM_ACMP_INPUT5 (0x013AU)
#define kSWM_ACMP_OUT (0x013BU)
#define kSWM_ADC_CHN0 (0x013CU)
#define kSWM_ADC_CHN1 (0x013DU)
#define kSWM_ADC_CHN10 (0x013EU)
#define kSWM_ADC_CHN11 (0x013FU)
#define kSWM_ADC_CHN2 (0x0140U)
#define kSWM_ADC_CHN3 (0x0141U)
#define kSWM_ADC_CHN4 (0x0142U)
#define kSWM_ADC_CHN5 (0x0143U)
#define kSWM_ADC_CHN6 (0x0144U)
#define kSWM_ADC_CHN7 (0x0145U)
#define kSWM_ADC_CHN8 (0x0146U)
#define kSWM_ADC_CHN9 (0x0147U)
#define kSWM_CAPT_X0 (0x0148U)
#define kSWM_CAPT_X1 (0x0149U)
#define kSWM_CAPT_X2 (0x014AU)
#define kSWM_CAPT_X3 (0x014BU)
#define kSWM_CAPT_X4 (0x014CU)
#define kSWM_CAPT_X5 (0x014DU)
#define kSWM_CAPT_X6 (0x014EU)
#define kSWM_CAPT_X7 (0x014FU)
#define kSWM_CAPT_X8 (0x0150U)
#define kSWM_CAPT_YH (0x0151U)
#define kSWM_CAPT_YL (0x0152U)
#define kSWM_CLKIN (0x0153U)
#define kSWM_CLKOUT (0x0154U)
#define kSWM_DAC_OUT0 (0x0155U)
#define kSWM_DAC_OUT1 (0x0156U)
#define kSWM_GPIO_INT_BMAT (0x0157U)
#define kSWM_I2C0_SCL (0x0158U)
#define kSWM_I2C0_SDA (0x0159U)
#define kSWM_I2C1_SCL (0x015AU)
#define kSWM_I2C1_SDA (0x015BU)
#define kSWM_I2C2_SCL (0x015CU)
#define kSWM_I2C2_SDA (0x015DU)
#define kSWM_I2C3_SCL (0x015EU)
#define kSWM_I2C3_SDA (0x015FU)
#define kSWM_PortPin_Reset (0x0160U)
#define kSWM_RESETN (0x0161U)
#define kSWM_SCT_OUT0 (0x0162U)
#define kSWM_SCT_OUT1 (0x0163U)
#define kSWM_SCT_OUT2 (0x0164U)
#define kSWM_SCT_OUT3 (0x0165U)
#define kSWM_SCT_OUT4 (0x0166U)
#define kSWM_SCT_OUT5 (0x0167U)
#define kSWM_SCT_OUT6 (0x0168U)
#define kSWM_SCT_PIN0 (0x0169U)
#define kSWM_SCT_PIN1 (0x016AU)
#define kSWM_SCT_PIN2 (0x016BU)
#define kSWM_SCT_PIN3 (0x016CU)
#define kSWM_SPI0_MISO (0x016DU)
#define kSWM_SPI0_MOSI (0x016EU)
#define kSWM_SPI0_SCK (0x016FU)
#define kSWM_SPI0_SSEL0 (0x0170U)
#define kSWM_SPI0_SSEL1 (0x0171U)
#define kSWM_SPI0_SSEL2 (0x0172U)
#define kSWM_SPI0_SSEL3 (0x0173U)
#define kSWM_SPI1_MISO (0x0174U)
#define kSWM_SPI1_MOSI (0x0175U)
#define kSWM_SPI1_SCK (0x0176U)
#define kSWM_SPI1_SSEL0 (0x0177U)
#define kSWM_SPI1_SSEL1 (0x0178U)
#define kSWM_SWCLK (0x0179U)
#define kSWM_SWDIO (0x017AU)
#define kSWM_T0_CAP_CHN0 (0x017BU)
#define kSWM_T0_CAP_CHN1 (0x017CU)
#define kSWM_T0_CAP_CHN2 (0x017DU)
#define kSWM_T0_MAT_CHN0 (0x017EU)
#define kSWM_T0_MAT_CHN1 (0x017FU)
#define kSWM_T0_MAT_CHN2 (0x0180U)
#define kSWM_T0_MAT_CHN3 (0x0181U)
#define kSWM_USART0_CTS (0x0182U)
#define kSWM_USART0_RTS (0x0183U)
#define kSWM_USART0_RXD (0x0184U)
#define kSWM_USART0_SCLK (0x0185U)
#define kSWM_USART0_TXD (0x0186U)
#define kSWM_USART1_CTS (0x0187U)
#define kSWM_USART1_RTS (0x0188U)
#define kSWM_USART1_RXD (0x0189U)
#define kSWM_USART1_SCLK (0x018AU)
#define kSWM_USART1_TXD (0x018BU)
#define kSWM_USART2_CTS (0x018CU)
#define kSWM_USART2_RTS (0x018DU)
#define kSWM_USART2_RXD (0x018EU)
#define kSWM_USART2_SCLK (0x018FU)
#define kSWM_USART2_TXD (0x0190U)
#define kSWM_USART3_RXD (0x0191U)
#define kSWM_USART3_SCLK (0x0192U)
#define kSWM_USART3_TXD (0x0193U)
#define kSWM_USART4_RXD (0x0194U)
#define kSWM_USART4_SCLK (0x0195U)
#define kSWM_USART4_TXD (0x0196U)
#define kSWM_VDDCMP (0x0197U)
#define kSWM_XTALIN (0x0198U)
#define kSWM_XTALOUT (0x0199U)
#define kStatus_Fail (0x019AU)
#define kStatus_I2C_Addr_Nak (0x019BU)
#define kStatus_I2C_Nak (0x019CU)
#define kStatus_I2C_Timeout (0x019DU)
#define kStatus_I2C_UnexpectedState (0x019EU)
#define kStatus_Success (0x019FU)
#define kUART0_Clk_From_MainClk (0x01A0U)
#define kUART1_Clk_From_MainClk (0x01A1U)
#define kUART2_Clk_From_MainClk (0x01A2U)
#define kUART3_Clk_From_MainClk (0x01A3U)
#define kUART4_Clk_From_MainClk (0x01A4U)
#define kUSART_8BitsPerChar (0x01A5U)
#define kUSART_OneStopBit (0x01A6U)
#define kUSART_ParityDisabled (0x01A7U)
#define kUSART_RxSampleOnFallingEdge (0x01A8U)
#define kUSART_SyncModeDisabled (0x01A9U)

uint32_t alxShimLpc84x_Call(int unused, ...);
void* alxShimLpc84x_CallPtr(int unused, ...);
#define ADC_Deinit(...) alxShimLpc84x_Call(0, ##__VA_ARGS__)
#define ADC_DoSoftwareTriggerConvSeqA(...) alxShimLpc84x_Call(0, ##__VA_ARGS__)
#define ADC_EnableConvSeqA(...) alxShimLpc84x_Call(0, ##__VA_ARGS__)
#define ADC_GetChannelConversionResult(...) alxShimLpc84x_Call(0, ##__VA_ARGS__)
#define ADC_GetConvSeqAGlobalConversionResult(...) alxShimLpc84x_Call(0, ##__VA_ARGS__)
#define ADC_Init(...) alxShimLpc84x_Call(0, ##__VA_ARGS__)
#define ADC_SetConvSeqAConfig(...) alxShimLpc84x_Call(0, ##__VA_ARGS__)
#define CLOCK_DisableClock(...) alxShimLpc84x_Call(0, ##__VA_ARGS__)
#define CLOCK_EnableClock(...) alxShimLpc84x_Call(0, ##__VA_ARGS__)
#define CLOCK_GetCoreSysClkFreq(...) alxShimLpc84x_Call(0, ##__VA_ARGS__)
#define CLOCK_GetExtClkFreq(...) alxShimLpc84x_Call(0, ##__VA_ARGS__)
#define CLOCK_GetFreq(...) alxShimLpc84x_Call(0, ##__VA_ARGS__)
#define CLOCK_GetFroFreq(...) alxShimLpc84x_Call(0, ##__VA_ARGS__)
#define CLOCK_GetMainClkFreq(...) alxShimLpc84x_Call(0, ##__VA_ARGS__)
#define CLOCK_InitWdtOsc(...) alxShimLpc84x_Call(0, ##__VA_ARGS__)
#define CLOCK_Select(...) alxShimLpc84x_Call(0, ##__VA_ARGS__)
#define CLOCK_SetClkDivider(...) alxShimLpc84x_Call(0, ##__VA_ARGS__)
#define CLOCK_SetCoreSysClkDiv(...) alxShimLpc84x_Call(0, ##__VA_ARGS__)
#define CLOCK_SetFroOscFreq(...) alxShimLpc84x_Call(0, ##__VA_ARGS__)
#define CLOCK_SetFroOutClkSrc(...) alxShimLpc84x_Call(0, ##__VA_ARGS__)
#define CLOCK_SetMainClkSrc(...) alxShimLpc84x_Call(0, ##__VA_ARGS__)
#define GPIO_PinInit(...) alxShimLpc84x_Call(0, ##__VA_ARGS__)
#define GPIO_PinRead(...) alxShimLpc84x_Call(0, ##__VA_ARGS__)
#define GPIO_PinWrite(...) alxShimLpc84x_Call(0, ##__VA_ARGS__)
#define GPIO_PortClear(...) alxShimLpc84x_Call(0, ##__VA_ARGS__)
#define GPIO_PortSet(...) alxShimLpc84x_Call(0, ##__VA_ARGS__)
#define GPIO_PortToggle(...) alxShimLpc84x_Call(0, ##__VA_ARGS__)
#define I2C_GetStatusFlags(...) alxShimLpc84x_Call(0, ##__VA_ARGS__)
#define I2C_MasterClearStatusFlags(...) alxShimLpc84x_Call(0, ##__VA_ARGS__)
#define I2C_MasterDeinit(...) alxShimLpc84x_Call(0, ##__VA_ARGS__)
#define I2C_MasterInit(...) alxShimLpc84x_Call(0, ##__VA_ARGS__)
#define POWER_DisablePD(...) alxShimLpc84x_Call(0, ##__VA_ARGS__)
#define POWER_EnablePD(...) alxShimLpc84x_Call(0, ##__VA_ARGS__)
#define RESET_PeripheralReset(...) alxShimLpc84x_Call(0, ##__VA_ARGS__)
#define SWM_SetFixedPinSelect(...) alxShimLpc84x_Call(0, ##__VA_ARGS__)
#define SWM_SetMovablePinSelect(...) alxShimLpc84x_Call(0, ##__VA_ARGS__)
#define USART_Deinit(...) alxShimLpc84x_Call(0, ##__VA_ARGS__)
#define USART_Init(...) alxShimLpc84x_Call(0, ##__VA_ARGS__)
#define USART_WriteBlocking(...) alxShimLpc84x_Call(0, ##__VA_ARGS__)
#define WWDT_Init(...) alxShimLpc84x_Call(0, ##__VA_ARGS__)
#define WWDT_Refresh(...) alxShimLpc84x_Call(0, ##__VA_ARGS__)


#endif	// ALX_SHIM_MCUX_LPC84X_LPC84XHOSTSHIM_H
