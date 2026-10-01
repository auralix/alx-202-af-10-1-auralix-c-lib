/**
  ******************************************************************************
  * @file		lpc55s6xHostShim.h
  * @brief		Auralix C Library - Configuration Matrix Fake - MCUXpresso SDK (LPC55S6x)
  * @copyright	Copyright (C) Auralix d.o.o. All rights reserved.
  *
  * Compile-only: every name of MCUXpresso SDK (LPC55S6x) the alx code uses, written from
  * that use, so the configuration matrix compiles every alx line under
  * ALX_LPC55S6X (Test/config/matrix.toml). Nothing here is the vendor's and
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

#ifndef ALX_SHIM_MCUX_LPC55S6X_LPC55S6XHOSTSHIM_H
#define ALX_SHIM_MCUX_LPC55S6X_LPC55S6XHOSTSHIM_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "cmsisHostShim.h"


typedef struct alxShimLpc55s6x_Sub
{
	uint32_t CLOCK_CTRL;
	uint32_t FIFO0Watermark;
	uint32_t FIFO1Watermark;
	uint32_t MSTCTL;
	uint32_t MSTDAT;
	uint32_t XO32M_CTRL;
	uint32_t baudRate_Bps;
	uint32_t bitCountPerChar;
	uint32_t chainedNextCommandNumber;
	uint32_t channelAFIFOSelect;
	uint32_t channelBFIFOSelect;
	uint32_t channelNumber;
	uint32_t clockFreq_Hz;
	uint32_t clockPolarity;
	uint32_t commandIdSource;
	uint32_t configFlags;
	uint32_t convPauseDelay;
	uint32_t convValue;
	uint32_t conversionAverageMode;
	uint32_t conversionResolutionMode;
	uint32_t dataSize;
	uint32_t dataWidth;
	uint32_t delayPower;
	uint32_t direction;
	uint32_t enableAnalogPreliminary;
	uint32_t enableAutoChannelIncrement;
	uint32_t enableContinuousSCLK;
	uint32_t enableConvPause;
	uint32_t enableHardwareFlowControl;
	uint32_t enableHardwareTrigger;
	uint32_t enableInDozeMode;
	uint32_t enableLoopback;
	uint32_t enableMaster;
	uint32_t enableMode32k;
	uint32_t enableRx;
	uint32_t enableTimeout;
	uint32_t enableTx;
	uint32_t enableWaitTrigger;
	uint32_t enableWatchdogProtect;
	uint32_t enableWatchdogReset;
	uint32_t enableWwdt;
	uint32_t flags;
	uint32_t frameDelay;
	uint32_t hardwareAverageMode;
	uint32_t hardwareCompareMode;
	uint32_t hardwareCompareValueHigh;
	uint32_t hardwareCompareValueLow;
	uint32_t input;
	uint32_t loopCount;
	uint32_t loopCountIndex;
	uint32_t loopback;
	uint32_t mode;
	uint32_t outputLogic;
	uint32_t parityMode;
	uint32_t phase;
	uint32_t pinDirection;
	uint32_t pllRate;
	uint32_t pllctrl;
	uint32_t pllndec;
	uint32_t pllpdec;
	uint32_t polarity;
	uint32_t postDelay;
	uint32_t powerLevelMode;
	uint32_t powerUpDelay;
	uint32_t preDelay;
	uint32_t prescale;
	uint32_t priority;
	uint32_t referenceVoltageSource;
	uint32_t rxWatermark;
	uint32_t sampleChannelMode;
	uint32_t sampleTimeMode;
	uint32_t sselNum;
	uint32_t sselPol;
	uint32_t stopBitCount;
	uint32_t syncMode;
	uint32_t targetCommandId;
	uint32_t timeoutValue;
	uint32_t timeout_Ms;
	uint32_t transferDelay;
	uint32_t triggerIdSource;
	uint32_t triggerPriorityPolicy;
	uint32_t txWatermark;
	uint32_t warningValue;
	uint32_t windowValue;
	void* rxData;
	void* txData;
	uint32_t* DIR;
	uint32_t* MR;
	uint32_t** PIO;
	uint32_t* pllsscg;
} alxShimLpc55s6x_Sub;

typedef struct alxShimLpc55s6x_Struct
{
	uint32_t CLOCK_CTRL;
	uint32_t FIFO0Watermark;
	uint32_t FIFO1Watermark;
	uint32_t MSTCTL;
	uint32_t MSTDAT;
	uint32_t XO32M_CTRL;
	uint32_t baudRate_Bps;
	uint32_t bitCountPerChar;
	uint32_t chainedNextCommandNumber;
	uint32_t channelAFIFOSelect;
	uint32_t channelBFIFOSelect;
	uint32_t channelNumber;
	uint32_t clockFreq_Hz;
	uint32_t clockPolarity;
	uint32_t commandIdSource;
	uint32_t configFlags;
	uint32_t convPauseDelay;
	uint32_t convValue;
	uint32_t conversionAverageMode;
	uint32_t conversionResolutionMode;
	uint32_t dataSize;
	uint32_t dataWidth;
	uint32_t delayPower;
	uint32_t direction;
	uint32_t enableAnalogPreliminary;
	uint32_t enableAutoChannelIncrement;
	uint32_t enableContinuousSCLK;
	uint32_t enableConvPause;
	uint32_t enableHardwareFlowControl;
	uint32_t enableHardwareTrigger;
	uint32_t enableInDozeMode;
	uint32_t enableLoopback;
	uint32_t enableMaster;
	uint32_t enableMode32k;
	uint32_t enableRx;
	uint32_t enableTimeout;
	uint32_t enableTx;
	uint32_t enableWaitTrigger;
	uint32_t enableWatchdogProtect;
	uint32_t enableWatchdogReset;
	uint32_t enableWwdt;
	uint32_t flags;
	uint32_t frameDelay;
	uint32_t hardwareAverageMode;
	uint32_t hardwareCompareMode;
	uint32_t hardwareCompareValueHigh;
	uint32_t hardwareCompareValueLow;
	uint32_t input;
	uint32_t loopCount;
	uint32_t loopCountIndex;
	uint32_t loopback;
	uint32_t mode;
	uint32_t outputLogic;
	uint32_t parityMode;
	uint32_t phase;
	uint32_t pinDirection;
	uint32_t pllRate;
	uint32_t pllctrl;
	uint32_t pllndec;
	uint32_t pllpdec;
	uint32_t polarity;
	uint32_t postDelay;
	uint32_t powerLevelMode;
	uint32_t powerUpDelay;
	uint32_t preDelay;
	uint32_t prescale;
	uint32_t priority;
	uint32_t referenceVoltageSource;
	uint32_t rxWatermark;
	uint32_t sampleChannelMode;
	uint32_t sampleTimeMode;
	uint32_t sselNum;
	uint32_t sselPol;
	uint32_t stopBitCount;
	uint32_t syncMode;
	uint32_t targetCommandId;
	uint32_t timeoutValue;
	uint32_t timeout_Ms;
	uint32_t transferDelay;
	uint32_t triggerIdSource;
	uint32_t triggerPriorityPolicy;
	uint32_t txWatermark;
	uint32_t warningValue;
	uint32_t windowValue;
	alxShimLpc55s6x_Sub delayConfig;
	void* rxData;
	void* txData;
	uint32_t* DIR;
	uint32_t* MR;
	uint32_t** PIO;
	uint32_t* pllsscg;
} alxShimLpc55s6x_Struct;

typedef alxShimLpc55s6x_Struct CTIMER_Type;
typedef alxShimLpc55s6x_Struct I2C_Type;
typedef alxShimLpc55s6x_Struct SPI_Type;
typedef alxShimLpc55s6x_Struct USART_Type;
typedef alxShimLpc55s6x_Struct ctimer_config_t;
typedef alxShimLpc55s6x_Struct gpio_pin_config_t;
typedef uint32_t i2c_direction_t;
typedef alxShimLpc55s6x_Struct i2c_master_config_t;
typedef uint32_t inputmux_connection_t;
typedef alxShimLpc55s6x_Struct lpadc_config_t;
typedef alxShimLpc55s6x_Struct lpadc_conv_command_config_t;
typedef alxShimLpc55s6x_Struct lpadc_conv_result_t;
typedef alxShimLpc55s6x_Struct lpadc_conv_trigger_config_t;
typedef uint32_t lpadc_sample_channel_mode_t;
typedef alxShimLpc55s6x_Struct pint_pin_enable_t;
typedef uint32_t pint_pin_int_t;
typedef alxShimLpc55s6x_Struct pll_setup_t;
typedef alxShimLpc55s6x_Struct spi_master_config_t;
typedef alxShimLpc55s6x_Struct spi_transfer_t;
typedef uint32_t status_t;
typedef alxShimLpc55s6x_Struct usart_config_t;
typedef alxShimLpc55s6x_Struct wwdt_config_t;

extern alxShimLpc55s6x_Struct alxShimLpc55s6x_Instance;
#define ANACTRL (&alxShimLpc55s6x_Instance)
#define CTIMER0 (&alxShimLpc55s6x_Instance)
#define CTIMER1 (&alxShimLpc55s6x_Instance)
#define CTIMER2 (&alxShimLpc55s6x_Instance)
#define CTIMER3 (&alxShimLpc55s6x_Instance)
#define CTIMER4 (&alxShimLpc55s6x_Instance)
#define GPIO (&alxShimLpc55s6x_Instance)
#define I2C0 (&alxShimLpc55s6x_Instance)
#define I2C1 (&alxShimLpc55s6x_Instance)
#define I2C2 (&alxShimLpc55s6x_Instance)
#define I2C3 (&alxShimLpc55s6x_Instance)
#define I2C4 (&alxShimLpc55s6x_Instance)
#define I2C5 (&alxShimLpc55s6x_Instance)
#define I2C6 (&alxShimLpc55s6x_Instance)
#define I2C7 (&alxShimLpc55s6x_Instance)
#define IOCON (&alxShimLpc55s6x_Instance)
#define SPI0 (&alxShimLpc55s6x_Instance)
#define SPI1 (&alxShimLpc55s6x_Instance)
#define SPI2 (&alxShimLpc55s6x_Instance)
#define SPI3 (&alxShimLpc55s6x_Instance)
#define SPI4 (&alxShimLpc55s6x_Instance)
#define SPI5 (&alxShimLpc55s6x_Instance)
#define SPI6 (&alxShimLpc55s6x_Instance)
#define SPI7 (&alxShimLpc55s6x_Instance)
#define SPI8 (&alxShimLpc55s6x_Instance)
#define SYSCON (&alxShimLpc55s6x_Instance)
#define USART0 (&alxShimLpc55s6x_Instance)
#define USART1 (&alxShimLpc55s6x_Instance)
#define USART2 (&alxShimLpc55s6x_Instance)
#define USART3 (&alxShimLpc55s6x_Instance)
#define USART4 (&alxShimLpc55s6x_Instance)
#define USART5 (&alxShimLpc55s6x_Instance)
#define USART6 (&alxShimLpc55s6x_Instance)
#define USART7 (&alxShimLpc55s6x_Instance)
#define ADC0 (0x0100U)
#define ANACTRL_XO32M_CTRL_ENABLE_SYSTEM_CLK_OUT_MASK (0x0101U)
#define FLEXCOMM0_IRQn ((IRQn_Type)1)
#define FLEXCOMM1_IRQn ((IRQn_Type)2)
#define FLEXCOMM2_IRQn ((IRQn_Type)3)
#define FLEXCOMM3_IRQn ((IRQn_Type)4)
#define FLEXCOMM4_IRQn ((IRQn_Type)5)
#define FLEXCOMM5_IRQn ((IRQn_Type)6)
#define FLEXCOMM6_IRQn ((IRQn_Type)7)
#define FLEXCOMM7_IRQn ((IRQn_Type)8)
#define I2C_MSTCTL_MSTCONTINUE_MASK (0x010AU)
#define I2C_MSTCTL_MSTSTART_MASK (0x010BU)
#define I2C_MSTCTL_MSTSTOP_MASK (0x010CU)
#define I2C_STAT_MSTCODE_NACKADR (0x010DU)
#define I2C_STAT_MSTCODE_NACKDAT (0x010EU)
#define I2C_STAT_MSTCODE_RXREADY (0x010FU)
#define I2C_STAT_MSTCODE_TXREADY (0x0110U)
#define I2C_STAT_MSTSTATE_MASK (0x0111U)
#define I2C_STAT_MSTSTATE_SHIFT (2U)
#define INPUTMUX (0x0113U)
#define IOCON_FUNC0 (0x0114U)
#define IOCON_FUNC1 (0x15U)
#define IOCON_FUNC10 (0x0116U)
#define IOCON_FUNC11 (0x17U)
#define IOCON_FUNC2 (0x18U)
#define IOCON_FUNC3 (0x19U)
#define IOCON_FUNC4 (0x1AU)
#define IOCON_FUNC5 (0x1BU)
#define IOCON_FUNC6 (0x1CU)
#define IOCON_FUNC7 (0x1DU)
#define IOCON_FUNC8 (0x011EU)
#define IOCON_FUNC9 (0x011FU)
#define IOCON_MODE_INACT (0x0120U)
#define IOCON_MODE_PULLDOWN (0x0121U)
#define IOCON_MODE_PULLUP (0x0122U)
#define IOCON_MODE_REPEATER (0x0123U)
#define PINT (0x0124U)
#define PIN_INT0_IRQn ((IRQn_Type)9)
#define PIN_INT1_IRQn ((IRQn_Type)10)
#define PIN_INT2_IRQn ((IRQn_Type)11)
#define PIN_INT3_IRQn ((IRQn_Type)12)
#define PIN_INT4_IRQn ((IRQn_Type)13)
#define PIN_INT5_IRQn ((IRQn_Type)14)
#define PIN_INT6_IRQn ((IRQn_Type)15)
#define PIN_INT7_IRQn ((IRQn_Type)16)
#define PLL_SETUPFLAG_WAITLOCK (0x012DU)
#define SYSCON_PLL0CTRL_CLKEN_MASK (0x012EU)
#define SYSCON_PLL0SSCG1_SEL_EXT_MASK (0x012FU)
#define WWDT (0x0130U)
#define kADC0_RST_SHIFT_RSTn (1U)
#define kCLOCK_BusClk (0x0132U)
#define kCLOCK_CoreSysClk (0x0133U)
#define kCLOCK_DivAdcAsyncClk (0x0134U)
#define kCLOCK_DivAhbClk (0x0135U)
#define kCLOCK_DivWdtClk (0x0136U)
#define kCLOCK_FlexComm0 (0x0137U)
#define kCLOCK_FlexComm1 (0x0138U)
#define kCLOCK_FlexComm2 (0x0139U)
#define kCLOCK_FlexComm3 (0x013AU)
#define kCLOCK_FlexComm4 (0x013BU)
#define kCLOCK_FlexComm5 (0x013CU)
#define kCLOCK_FlexComm6 (0x013DU)
#define kCLOCK_FlexComm7 (0x013EU)
#define kCLOCK_Gpio0 (0x013FU)
#define kCLOCK_Gpio1 (0x0140U)
#define kCLOCK_Gpio2 (0x0141U)
#define kCLOCK_Gpio3 (0x0142U)
#define kCLOCK_Iocon (0x0143U)
#define kCLOCK_Pint (0x0144U)
#define kCTIMER_Capture_0 (0x0145U)
#define kCTIMER_Match_0 (0x0146U)
#define kCTIMER_Match_1 (0x0147U)
#define kCTIMER_Match_2 (0x0148U)
#define kCTIMER_Match_3 (0x0149U)
#define kCTIMER_TimerMode (0x014AU)
#define kEXT_CLK_to_PLL0 (0x014BU)
#define kFC0_RST_SHIFT_RSTn (12U)
#define kFC1_RST_SHIFT_RSTn (13U)
#define kFC2_RST_SHIFT_RSTn (14U)
#define kFC3_RST_SHIFT_RSTn (15U)
#define kFC4_RST_SHIFT_RSTn (0U)
#define kFC5_RST_SHIFT_RSTn (1U)
#define kFC6_RST_SHIFT_RSTn (2U)
#define kFC7_RST_SHIFT_RSTn (3U)
#define kFRO12M_to_MAIN_CLK (0x0154U)
#define kFRO12M_to_PLL0 (0x0155U)
#define kFRO1M_to_CTIMER0 (0x0156U)
#define kFRO1M_to_CTIMER1 (0x0157U)
#define kFRO1M_to_CTIMER2 (0x0158U)
#define kFRO1M_to_CTIMER3 (0x0159U)
#define kFRO1M_to_CTIMER4 (0x015AU)
#define kFRO_HF_to_MAIN_CLK (0x015BU)
#define kGPIO0_RST_SHIFT_RSTn (12U)
#define kGPIO1_RST_SHIFT_RSTn (13U)
#define kGPIO2_RST_SHIFT_RSTn (14U)
#define kGPIO3_RST_SHIFT_RSTn (15U)
#define kGPIO_DigitalInput (0x0160U)
#define kGPIO_DigitalOutput (0x0161U)
#define kI2C_EventTimeoutFlag (0x0162U)
#define kI2C_MasterAddrNackFlag (0x0163U)
#define kI2C_MasterAllClearFlags (0x0164U)
#define kI2C_MasterArbitrationLostFlag (0x0165U)
#define kI2C_MasterPendingFlag (0x0166U)
#define kI2C_MasterStartStopErrorFlag (0x0167U)
#define kI2C_Read (0x0168U)
#define kI2C_SclTimeoutFlag (0x0169U)
#define kI2C_TransferDefaultFlag (0x016AU)
#define kI2C_TransferNoStartFlag (0x016BU)
#define kI2C_TransferNoStopFlag (0x016CU)
#define kI2C_Write (0x016DU)
#define kINPUTMUX_GpioPort0Pin0ToPintsel (0x016EU)
#define kINPUTMUX_GpioPort0Pin10ToPintsel (0x016FU)
#define kINPUTMUX_GpioPort0Pin11ToPintsel (0x0170U)
#define kINPUTMUX_GpioPort0Pin12ToPintsel (0x0171U)
#define kINPUTMUX_GpioPort0Pin13ToPintsel (0x0172U)
#define kINPUTMUX_GpioPort0Pin14ToPintsel (0x0173U)
#define kINPUTMUX_GpioPort0Pin15ToPintsel (0x0174U)
#define kINPUTMUX_GpioPort0Pin16ToPintsel (0x0175U)
#define kINPUTMUX_GpioPort0Pin17ToPintsel (0x0176U)
#define kINPUTMUX_GpioPort0Pin18ToPintsel (0x0177U)
#define kINPUTMUX_GpioPort0Pin19ToPintsel (0x0178U)
#define kINPUTMUX_GpioPort0Pin1ToPintsel (0x0179U)
#define kINPUTMUX_GpioPort0Pin20ToPintsel (0x017AU)
#define kINPUTMUX_GpioPort0Pin21ToPintsel (0x017BU)
#define kINPUTMUX_GpioPort0Pin22ToPintsel (0x017CU)
#define kINPUTMUX_GpioPort0Pin23ToPintsel (0x017DU)
#define kINPUTMUX_GpioPort0Pin24ToPintsel (0x017EU)
#define kINPUTMUX_GpioPort0Pin25ToPintsel (0x017FU)
#define kINPUTMUX_GpioPort0Pin26ToPintsel (0x0180U)
#define kINPUTMUX_GpioPort0Pin27ToPintsel (0x0181U)
#define kINPUTMUX_GpioPort0Pin28ToPintsel (0x0182U)
#define kINPUTMUX_GpioPort0Pin29ToPintsel (0x0183U)
#define kINPUTMUX_GpioPort0Pin2ToPintsel (0x0184U)
#define kINPUTMUX_GpioPort0Pin30ToPintsel (0x0185U)
#define kINPUTMUX_GpioPort0Pin31ToPintsel (0x0186U)
#define kINPUTMUX_GpioPort0Pin3ToPintsel (0x0187U)
#define kINPUTMUX_GpioPort0Pin4ToPintsel (0x0188U)
#define kINPUTMUX_GpioPort0Pin5ToPintsel (0x0189U)
#define kINPUTMUX_GpioPort0Pin6ToPintsel (0x018AU)
#define kINPUTMUX_GpioPort0Pin7ToPintsel (0x018BU)
#define kINPUTMUX_GpioPort0Pin8ToPintsel (0x018CU)
#define kINPUTMUX_GpioPort0Pin9ToPintsel (0x018DU)
#define kINPUTMUX_GpioPort1Pin0ToPintsel (0x018EU)
#define kINPUTMUX_GpioPort1Pin10ToPintsel (0x018FU)
#define kINPUTMUX_GpioPort1Pin11ToPintsel (0x0190U)
#define kINPUTMUX_GpioPort1Pin12ToPintsel (0x0191U)
#define kINPUTMUX_GpioPort1Pin13ToPintsel (0x0192U)
#define kINPUTMUX_GpioPort1Pin14ToPintsel (0x0193U)
#define kINPUTMUX_GpioPort1Pin15ToPintsel (0x0194U)
#define kINPUTMUX_GpioPort1Pin16ToPintsel (0x0195U)
#define kINPUTMUX_GpioPort1Pin17ToPintsel (0x0196U)
#define kINPUTMUX_GpioPort1Pin18ToPintsel (0x0197U)
#define kINPUTMUX_GpioPort1Pin19ToPintsel (0x0198U)
#define kINPUTMUX_GpioPort1Pin1ToPintsel (0x0199U)
#define kINPUTMUX_GpioPort1Pin20ToPintsel (0x019AU)
#define kINPUTMUX_GpioPort1Pin21ToPintsel (0x019BU)
#define kINPUTMUX_GpioPort1Pin22ToPintsel (0x019CU)
#define kINPUTMUX_GpioPort1Pin23ToPintsel (0x019DU)
#define kINPUTMUX_GpioPort1Pin24ToPintsel (0x019EU)
#define kINPUTMUX_GpioPort1Pin25ToPintsel (0x019FU)
#define kINPUTMUX_GpioPort1Pin26ToPintsel (0x01A0U)
#define kINPUTMUX_GpioPort1Pin27ToPintsel (0x01A1U)
#define kINPUTMUX_GpioPort1Pin28ToPintsel (0x01A2U)
#define kINPUTMUX_GpioPort1Pin29ToPintsel (0x01A3U)
#define kINPUTMUX_GpioPort1Pin2ToPintsel (0x01A4U)
#define kINPUTMUX_GpioPort1Pin30ToPintsel (0x01A5U)
#define kINPUTMUX_GpioPort1Pin31ToPintsel (0x01A6U)
#define kINPUTMUX_GpioPort1Pin3ToPintsel (0x01A7U)
#define kINPUTMUX_GpioPort1Pin4ToPintsel (0x01A8U)
#define kINPUTMUX_GpioPort1Pin5ToPintsel (0x01A9U)
#define kINPUTMUX_GpioPort1Pin6ToPintsel (0x01AAU)
#define kINPUTMUX_GpioPort1Pin7ToPintsel (0x01ABU)
#define kINPUTMUX_GpioPort1Pin8ToPintsel (0x01ACU)
#define kINPUTMUX_GpioPort1Pin9ToPintsel (0x01ADU)
#define kLPADC_ConversionAverage1 (0x01AEU)
#define kLPADC_ConversionResolutionStandard (0x01AFU)
#define kLPADC_HardwareAverageCount1 (0x01B0U)
#define kLPADC_HardwareCompareDisabled (0x01B1U)
#define kLPADC_PowerLevelAlt1 (0x01B2U)
#define kLPADC_ReferenceVoltageAlt2 (0x01B3U)
#define kLPADC_SampleChannelSingleEndSideA (0x01B4U)
#define kLPADC_SampleChannelSingleEndSideB (0x01B5U)
#define kLPADC_SampleTimeADCK3 (0x01B6U)
#define kLPADC_TriggerPriorityPreemptImmediately (0x01B7U)
#define kMAIN_CLK_to_ADC_CLK (0x01B8U)
#define kMAIN_CLK_to_CTIMER0 (0x01B9U)
#define kMAIN_CLK_to_CTIMER1 (0x01BAU)
#define kMAIN_CLK_to_CTIMER2 (0x01BBU)
#define kMAIN_CLK_to_CTIMER3 (0x01BCU)
#define kMAIN_CLK_to_CTIMER4 (0x01BDU)
#define kMAIN_CLK_to_FLEXCOMM0 (0x01BEU)
#define kMAIN_CLK_to_FLEXCOMM1 (0x01BFU)
#define kMAIN_CLK_to_FLEXCOMM2 (0x01C0U)
#define kMAIN_CLK_to_FLEXCOMM3 (0x01C1U)
#define kMAIN_CLK_to_FLEXCOMM4 (0x01C2U)
#define kMAIN_CLK_to_FLEXCOMM5 (0x01C3U)
#define kMAIN_CLK_to_FLEXCOMM6 (0x01C4U)
#define kMAIN_CLK_to_FLEXCOMM7 (0x01C5U)
#define kNONE_to_PLL0 (0x01C6U)
#define kPDRUNCFG_PD_FRO192M (0x01C7U)
#define kPDRUNCFG_PD_FRO1M (0x01C8U)
#define kPDRUNCFG_PD_LDOGPADC (0x01C9U)
#define kPDRUNCFG_PD_PLL0 (0x01CAU)
#define kPDRUNCFG_PD_PLL0_SSCG (0x01CBU)
#define kPINT_PinInt0 (0x01CCU)
#define kPINT_PinInt1 (0x01CDU)
#define kPINT_PinInt2 (0x01CEU)
#define kPINT_PinInt3 (0x01CFU)
#define kPINT_PinInt4 (0x01D0U)
#define kPINT_PinInt5 (0x01D1U)
#define kPINT_PinInt6 (0x01D2U)
#define kPINT_PinInt7 (0x01D3U)
#define kPINT_RST_SHIFT_RSTn (4U)
#define kPLL0_to_MAIN_CLK (0x01D5U)
#define kSPI_ClockPhaseFirstEdge (0x01D6U)
#define kSPI_ClockPhaseSecondEdge (0x01D7U)
#define kSPI_ClockPolarityActiveHigh (0x01D8U)
#define kSPI_ClockPolarityActiveLow (0x01D9U)
#define kSPI_Data8Bits (0x01DAU)
#define kSPI_FrameAssert (0x01DBU)
#define kSPI_MsbFirst (0x01DCU)
#define kSPI_RxFifo1 (0x01DDU)
#define kSPI_SpolActiveAllLow (0x01DEU)
#define kSPI_Ssel0 (0x01DFU)
#define kSPI_TxFifo0 (0x01E0U)
#define kStatus_Fail (0x01E1U)
#define kStatus_I2C_Addr_Nak (0x01E2U)
#define kStatus_I2C_ArbitrationLost (0x01E3U)
#define kStatus_I2C_Nak (0x01E4U)
#define kStatus_I2C_StartStopError (0x01E5U)
#define kStatus_I2C_Timeout (0x01E6U)
#define kStatus_I2C_UnexpectedState (0x01E7U)
#define kStatus_PLL_Success (0x01E8U)
#define kStatus_Success (0x01E9U)
#define kUSART_8BitsPerChar (0x01EAU)
#define kUSART_OneStopBit (0x01EBU)
#define kUSART_ParityDisabled (0x01ECU)
#define kUSART_RxFifo1 (0x01EDU)
#define kUSART_RxLevelInterruptEnable (0x01EEU)
#define kUSART_RxSampleOnFallingEdge (0x01EFU)
#define kUSART_SyncModeDisabled (0x01F0U)
#define kUSART_TxFifo0 (0x01F1U)

uint32_t alxShimLpc55s6x_Call(int unused, ...);
void* alxShimLpc55s6x_CallPtr(int unused, ...);
#define CLOCK_AttachClk(...) alxShimLpc55s6x_Call(0, ##__VA_ARGS__)
#define CLOCK_DisableClock(...) alxShimLpc55s6x_Call(0, ##__VA_ARGS__)
#define CLOCK_EnableClock(...) alxShimLpc55s6x_Call(0, ##__VA_ARGS__)
#define CLOCK_GetFlexCommClkFreq(...) alxShimLpc55s6x_Call(0, ##__VA_ARGS__)
#define CLOCK_GetFreq(...) alxShimLpc55s6x_Call(0, ##__VA_ARGS__)
#define CLOCK_GetWdtClkFreq(...) alxShimLpc55s6x_Call(0, ##__VA_ARGS__)
#define CLOCK_SetClkDiv(...) alxShimLpc55s6x_Call(0, ##__VA_ARGS__)
#define CLOCK_SetFLASHAccessCyclesForFreq(...) alxShimLpc55s6x_Call(0, ##__VA_ARGS__)
#define CLOCK_SetPLL0Freq(...) alxShimLpc55s6x_Call(0, ##__VA_ARGS__)
#define CLOCK_SetupExtClocking(...) alxShimLpc55s6x_Call(0, ##__VA_ARGS__)
#define CTIMER_Deinit(...) alxShimLpc55s6x_Call(0, ##__VA_ARGS__)
#define CTIMER_Init(...) alxShimLpc55s6x_Call(0, ##__VA_ARGS__)
#define CTIMER_SetupPwm(...) alxShimLpc55s6x_Call(0, ##__VA_ARGS__)
#define CTIMER_StartTimer(...) alxShimLpc55s6x_Call(0, ##__VA_ARGS__)
#define GPIO_PinInit(...) alxShimLpc55s6x_Call(0, ##__VA_ARGS__)
#define GPIO_PinRead(...) alxShimLpc55s6x_Call(0, ##__VA_ARGS__)
#define GPIO_PinWrite(...) alxShimLpc55s6x_Call(0, ##__VA_ARGS__)
#define GPIO_PortClear(...) alxShimLpc55s6x_Call(0, ##__VA_ARGS__)
#define GPIO_PortSet(...) alxShimLpc55s6x_Call(0, ##__VA_ARGS__)
#define GPIO_PortToggle(...) alxShimLpc55s6x_Call(0, ##__VA_ARGS__)
#define I2C_ClearStatusFlags(...) alxShimLpc55s6x_Call(0, ##__VA_ARGS__)
#define I2C_GetStatusFlags(...) alxShimLpc55s6x_Call(0, ##__VA_ARGS__)
#define I2C_MasterDeinit(...) alxShimLpc55s6x_Call(0, ##__VA_ARGS__)
#define I2C_MasterInit(...) alxShimLpc55s6x_Call(0, ##__VA_ARGS__)
#define INPUTMUX_AttachSignal(...) alxShimLpc55s6x_Call(0, ##__VA_ARGS__)
#define INPUTMUX_Deinit(...) alxShimLpc55s6x_Call(0, ##__VA_ARGS__)
#define INPUTMUX_Init(...) alxShimLpc55s6x_Call(0, ##__VA_ARGS__)
#define IOCON_PinMuxSet(...) alxShimLpc55s6x_Call(0, ##__VA_ARGS__)
#define LPADC_Deinit(...) alxShimLpc55s6x_Call(0, ##__VA_ARGS__)
#define LPADC_DoAutoCalibration(...) alxShimLpc55s6x_Call(0, ##__VA_ARGS__)
#define LPADC_DoSoftwareTrigger(...) alxShimLpc55s6x_Call(0, ##__VA_ARGS__)
#define LPADC_EnableOffsetCalibration(...) alxShimLpc55s6x_Call(0, ##__VA_ARGS__)
#define LPADC_GetConvResult(...) alxShimLpc55s6x_Call(0, ##__VA_ARGS__)
#define LPADC_Init(...) alxShimLpc55s6x_Call(0, ##__VA_ARGS__)
#define LPADC_SetConvCommandConfig(...) alxShimLpc55s6x_Call(0, ##__VA_ARGS__)
#define LPADC_SetConvTriggerConfig(...) alxShimLpc55s6x_Call(0, ##__VA_ARGS__)
#define PINT_DisableCallbackByIndex(...) alxShimLpc55s6x_Call(0, ##__VA_ARGS__)
#define PINT_EnableCallbackByIndex(...) alxShimLpc55s6x_Call(0, ##__VA_ARGS__)
#define PINT_Init(...) alxShimLpc55s6x_Call(0, ##__VA_ARGS__)
#define PINT_PinInterruptClrStatus(...) alxShimLpc55s6x_Call(0, ##__VA_ARGS__)
#define PINT_PinInterruptConfig(...) alxShimLpc55s6x_Call(0, ##__VA_ARGS__)
#define POWER_DisablePD(...) alxShimLpc55s6x_Call(0, ##__VA_ARGS__)
#define POWER_EnablePD(...) alxShimLpc55s6x_Call(0, ##__VA_ARGS__)
#define RESET_PeripheralReset(...) alxShimLpc55s6x_Call(0, ##__VA_ARGS__)
#define SPI_Deinit(...) alxShimLpc55s6x_Call(0, ##__VA_ARGS__)
#define SPI_MasterInit(...) alxShimLpc55s6x_Call(0, ##__VA_ARGS__)
#define SPI_MasterTransferBlocking(...) alxShimLpc55s6x_Call(0, ##__VA_ARGS__)
#define SYSCON_PLL0CTRL_SELI(...) alxShimLpc55s6x_Call(0, ##__VA_ARGS__)
#define SYSCON_PLL0CTRL_SELP(...) alxShimLpc55s6x_Call(0, ##__VA_ARGS__)
#define SYSCON_PLL0NDEC_NDIV(...) alxShimLpc55s6x_Call(0, ##__VA_ARGS__)
#define SYSCON_PLL0PDEC_PDIV(...) alxShimLpc55s6x_Call(0, ##__VA_ARGS__)
#define SYSCON_PLL0SSCG1_MDIV_EXT(...) alxShimLpc55s6x_Call(0, ##__VA_ARGS__)
#define USART_Deinit(...) alxShimLpc55s6x_Call(0, ##__VA_ARGS__)
#define USART_DisableInterrupts(...) alxShimLpc55s6x_Call(0, ##__VA_ARGS__)
#define USART_EnableInterrupts(...) alxShimLpc55s6x_Call(0, ##__VA_ARGS__)
#define USART_Init(...) alxShimLpc55s6x_Call(0, ##__VA_ARGS__)
#define USART_ReadByte(...) alxShimLpc55s6x_Call(0, ##__VA_ARGS__)
#define USART_WriteBlocking(...) alxShimLpc55s6x_Call(0, ##__VA_ARGS__)
#define WWDT_Init(...) alxShimLpc55s6x_Call(0, ##__VA_ARGS__)
#define WWDT_Refresh(...) alxShimLpc55s6x_Call(0, ##__VA_ARGS__)


#endif	// ALX_SHIM_MCUX_LPC55S6X_LPC55S6XHOSTSHIM_H
