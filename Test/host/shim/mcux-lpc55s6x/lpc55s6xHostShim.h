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
#define GPIO (&alxShimLpc55s6x_Instance)
#define IOCON (&alxShimLpc55s6x_Instance)
#define SPI8 (&alxShimLpc55s6x_Instance)
#define SYSCON (&alxShimLpc55s6x_Instance)
#define ADC0 (0x0100U)
#define ANACTRL_XO32M_CTRL_ENABLE_SYSTEM_CLK_OUT_MASK (0x0101U)
#define I2C_MSTCTL_MSTCONTINUE_MASK (0x0102U)
#define I2C_MSTCTL_MSTSTART_MASK (0x0103U)
#define I2C_MSTCTL_MSTSTOP_MASK (0x0104U)
#define I2C_STAT_MSTCODE_NACKADR (0x0105U)
#define I2C_STAT_MSTCODE_NACKDAT (0x0106U)
#define I2C_STAT_MSTCODE_RXREADY (0x0107U)
#define I2C_STAT_MSTCODE_TXREADY (0x0108U)
#define I2C_STAT_MSTSTATE_MASK (0x0109U)
#define I2C_STAT_MSTSTATE_SHIFT (10U)
#define INPUTMUX (0x010BU)
#define IOCON_FUNC0 (0x010CU)
#define IOCON_FUNC1 (0x010DU)
#define IOCON_FUNC10 (0x010EU)
#define IOCON_FUNC11 (0x010FU)
#define IOCON_FUNC2 (0x0110U)
#define IOCON_FUNC3 (0x0111U)
#define IOCON_FUNC4 (0x0112U)
#define IOCON_FUNC5 (0x0113U)
#define IOCON_FUNC6 (0x0114U)
#define IOCON_FUNC7 (0x0115U)
#define IOCON_FUNC8 (0x0116U)
#define IOCON_FUNC9 (0x0117U)
#define IOCON_MODE_INACT (0x0118U)
#define IOCON_MODE_PULLDOWN (0x0119U)
#define IOCON_MODE_PULLUP (0x011AU)
#define IOCON_MODE_REPEATER (0x011BU)
#define PINT (0x011CU)
#define PIN_INT0_IRQn ((IRQn_Type)1)
#define PIN_INT1_IRQn ((IRQn_Type)2)
#define PIN_INT2_IRQn ((IRQn_Type)3)
#define PIN_INT3_IRQn ((IRQn_Type)4)
#define PIN_INT4_IRQn ((IRQn_Type)5)
#define PIN_INT5_IRQn ((IRQn_Type)6)
#define PIN_INT6_IRQn ((IRQn_Type)7)
#define PIN_INT7_IRQn ((IRQn_Type)8)
#define PLL_SETUPFLAG_WAITLOCK (0x0125U)
#define SYSCON_PLL0CTRL_CLKEN_MASK (0x0126U)
#define SYSCON_PLL0SSCG1_SEL_EXT_MASK (0x0127U)
#define WWDT (0x0128U)
#define kADC0_RST_SHIFT_RSTn (9U)
#define kCLOCK_BusClk (0x012AU)
#define kCLOCK_CoreSysClk (0x012BU)
#define kCLOCK_DivAdcAsyncClk (0x012CU)
#define kCLOCK_DivAhbClk (0x012DU)
#define kCLOCK_DivWdtClk (0x012EU)
#define kCLOCK_Gpio0 (0x012FU)
#define kCLOCK_Gpio1 (0x0130U)
#define kCLOCK_Gpio2 (0x0131U)
#define kCLOCK_Gpio3 (0x0132U)
#define kCLOCK_Iocon (0x0133U)
#define kCLOCK_Pint (0x0134U)
#define kCTIMER_Capture_0 (0x0135U)
#define kCTIMER_Match_0 (0x0136U)
#define kCTIMER_Match_1 (0x0137U)
#define kCTIMER_Match_2 (0x0138U)
#define kCTIMER_Match_3 (0x0139U)
#define kCTIMER_TimerMode (0x013AU)
#define kEXT_CLK_to_PLL0 (0x013BU)
#define kFRO12M_to_MAIN_CLK (0x013CU)
#define kFRO12M_to_PLL0 (0x013DU)
#define kFRO_HF_to_MAIN_CLK (0x013EU)
#define kGPIO0_RST_SHIFT_RSTn (15U)
#define kGPIO1_RST_SHIFT_RSTn (0U)
#define kGPIO2_RST_SHIFT_RSTn (1U)
#define kGPIO3_RST_SHIFT_RSTn (2U)
#define kGPIO_DigitalInput (0x0143U)
#define kGPIO_DigitalOutput (0x0144U)
#define kI2C_EventTimeoutFlag (0x0145U)
#define kI2C_MasterAddrNackFlag (0x0146U)
#define kI2C_MasterAllClearFlags (0x0147U)
#define kI2C_MasterArbitrationLostFlag (0x0148U)
#define kI2C_MasterPendingFlag (0x0149U)
#define kI2C_MasterStartStopErrorFlag (0x014AU)
#define kI2C_Read (0x014BU)
#define kI2C_SclTimeoutFlag (0x014CU)
#define kI2C_TransferDefaultFlag (0x014DU)
#define kI2C_TransferNoStartFlag (0x014EU)
#define kI2C_TransferNoStopFlag (0x014FU)
#define kI2C_Write (0x0150U)
#define kINPUTMUX_GpioPort0Pin0ToPintsel (0x0151U)
#define kINPUTMUX_GpioPort0Pin10ToPintsel (0x0152U)
#define kINPUTMUX_GpioPort0Pin11ToPintsel (0x0153U)
#define kINPUTMUX_GpioPort0Pin12ToPintsel (0x0154U)
#define kINPUTMUX_GpioPort0Pin13ToPintsel (0x0155U)
#define kINPUTMUX_GpioPort0Pin14ToPintsel (0x0156U)
#define kINPUTMUX_GpioPort0Pin15ToPintsel (0x0157U)
#define kINPUTMUX_GpioPort0Pin16ToPintsel (0x0158U)
#define kINPUTMUX_GpioPort0Pin17ToPintsel (0x0159U)
#define kINPUTMUX_GpioPort0Pin18ToPintsel (0x015AU)
#define kINPUTMUX_GpioPort0Pin19ToPintsel (0x015BU)
#define kINPUTMUX_GpioPort0Pin1ToPintsel (0x015CU)
#define kINPUTMUX_GpioPort0Pin20ToPintsel (0x015DU)
#define kINPUTMUX_GpioPort0Pin21ToPintsel (0x015EU)
#define kINPUTMUX_GpioPort0Pin22ToPintsel (0x015FU)
#define kINPUTMUX_GpioPort0Pin23ToPintsel (0x0160U)
#define kINPUTMUX_GpioPort0Pin24ToPintsel (0x0161U)
#define kINPUTMUX_GpioPort0Pin25ToPintsel (0x0162U)
#define kINPUTMUX_GpioPort0Pin26ToPintsel (0x0163U)
#define kINPUTMUX_GpioPort0Pin27ToPintsel (0x0164U)
#define kINPUTMUX_GpioPort0Pin28ToPintsel (0x0165U)
#define kINPUTMUX_GpioPort0Pin29ToPintsel (0x0166U)
#define kINPUTMUX_GpioPort0Pin2ToPintsel (0x0167U)
#define kINPUTMUX_GpioPort0Pin30ToPintsel (0x0168U)
#define kINPUTMUX_GpioPort0Pin31ToPintsel (0x0169U)
#define kINPUTMUX_GpioPort0Pin3ToPintsel (0x016AU)
#define kINPUTMUX_GpioPort0Pin4ToPintsel (0x016BU)
#define kINPUTMUX_GpioPort0Pin5ToPintsel (0x016CU)
#define kINPUTMUX_GpioPort0Pin6ToPintsel (0x016DU)
#define kINPUTMUX_GpioPort0Pin7ToPintsel (0x016EU)
#define kINPUTMUX_GpioPort0Pin8ToPintsel (0x016FU)
#define kINPUTMUX_GpioPort0Pin9ToPintsel (0x0170U)
#define kINPUTMUX_GpioPort1Pin0ToPintsel (0x0171U)
#define kINPUTMUX_GpioPort1Pin10ToPintsel (0x0172U)
#define kINPUTMUX_GpioPort1Pin11ToPintsel (0x0173U)
#define kINPUTMUX_GpioPort1Pin12ToPintsel (0x0174U)
#define kINPUTMUX_GpioPort1Pin13ToPintsel (0x0175U)
#define kINPUTMUX_GpioPort1Pin14ToPintsel (0x0176U)
#define kINPUTMUX_GpioPort1Pin15ToPintsel (0x0177U)
#define kINPUTMUX_GpioPort1Pin16ToPintsel (0x0178U)
#define kINPUTMUX_GpioPort1Pin17ToPintsel (0x0179U)
#define kINPUTMUX_GpioPort1Pin18ToPintsel (0x017AU)
#define kINPUTMUX_GpioPort1Pin19ToPintsel (0x017BU)
#define kINPUTMUX_GpioPort1Pin1ToPintsel (0x017CU)
#define kINPUTMUX_GpioPort1Pin20ToPintsel (0x017DU)
#define kINPUTMUX_GpioPort1Pin21ToPintsel (0x017EU)
#define kINPUTMUX_GpioPort1Pin22ToPintsel (0x017FU)
#define kINPUTMUX_GpioPort1Pin23ToPintsel (0x0180U)
#define kINPUTMUX_GpioPort1Pin24ToPintsel (0x0181U)
#define kINPUTMUX_GpioPort1Pin25ToPintsel (0x0182U)
#define kINPUTMUX_GpioPort1Pin26ToPintsel (0x0183U)
#define kINPUTMUX_GpioPort1Pin27ToPintsel (0x0184U)
#define kINPUTMUX_GpioPort1Pin28ToPintsel (0x0185U)
#define kINPUTMUX_GpioPort1Pin29ToPintsel (0x0186U)
#define kINPUTMUX_GpioPort1Pin2ToPintsel (0x0187U)
#define kINPUTMUX_GpioPort1Pin30ToPintsel (0x0188U)
#define kINPUTMUX_GpioPort1Pin31ToPintsel (0x0189U)
#define kINPUTMUX_GpioPort1Pin3ToPintsel (0x018AU)
#define kINPUTMUX_GpioPort1Pin4ToPintsel (0x018BU)
#define kINPUTMUX_GpioPort1Pin5ToPintsel (0x018CU)
#define kINPUTMUX_GpioPort1Pin6ToPintsel (0x018DU)
#define kINPUTMUX_GpioPort1Pin7ToPintsel (0x018EU)
#define kINPUTMUX_GpioPort1Pin8ToPintsel (0x018FU)
#define kINPUTMUX_GpioPort1Pin9ToPintsel (0x0190U)
#define kLPADC_ConversionAverage1 (0x0191U)
#define kLPADC_ConversionResolutionStandard (0x0192U)
#define kLPADC_HardwareAverageCount1 (0x0193U)
#define kLPADC_HardwareCompareDisabled (0x0194U)
#define kLPADC_PowerLevelAlt1 (0x0195U)
#define kLPADC_ReferenceVoltageAlt2 (0x0196U)
#define kLPADC_SampleChannelSingleEndSideA (0x0197U)
#define kLPADC_SampleChannelSingleEndSideB (0x0198U)
#define kLPADC_SampleTimeADCK3 (0x0199U)
#define kLPADC_TriggerPriorityPreemptImmediately (0x019AU)
#define kMAIN_CLK_to_ADC_CLK (0x019BU)
#define kNONE_to_PLL0 (0x019CU)
#define kPDRUNCFG_PD_FRO192M (0x019DU)
#define kPDRUNCFG_PD_FRO1M (0x019EU)
#define kPDRUNCFG_PD_LDOGPADC (0x019FU)
#define kPDRUNCFG_PD_PLL0 (0x01A0U)
#define kPDRUNCFG_PD_PLL0_SSCG (0x01A1U)
#define kPINT_PinInt0 (0x01A2U)
#define kPINT_PinInt1 (0x01A3U)
#define kPINT_PinInt2 (0x01A4U)
#define kPINT_PinInt3 (0x01A5U)
#define kPINT_PinInt4 (0x01A6U)
#define kPINT_PinInt5 (0x01A7U)
#define kPINT_PinInt6 (0x01A8U)
#define kPINT_PinInt7 (0x01A9U)
#define kPINT_RST_SHIFT_RSTn (10U)
#define kPLL0_to_MAIN_CLK (0x01ABU)
#define kSPI_ClockPhaseFirstEdge (0x01ACU)
#define kSPI_ClockPhaseSecondEdge (0x01ADU)
#define kSPI_ClockPolarityActiveHigh (0x01AEU)
#define kSPI_ClockPolarityActiveLow (0x01AFU)
#define kSPI_Data8Bits (0x01B0U)
#define kSPI_FrameAssert (0x01B1U)
#define kSPI_MsbFirst (0x01B2U)
#define kSPI_RxFifo1 (0x01B3U)
#define kSPI_SpolActiveAllLow (0x01B4U)
#define kSPI_Ssel0 (0x01B5U)
#define kSPI_TxFifo0 (0x01B6U)
#define kStatus_Fail (0x01B7U)
#define kStatus_I2C_Addr_Nak (0x01B8U)
#define kStatus_I2C_ArbitrationLost (0x01B9U)
#define kStatus_I2C_Nak (0x01BAU)
#define kStatus_I2C_StartStopError (0x01BBU)
#define kStatus_I2C_Timeout (0x01BCU)
#define kStatus_I2C_UnexpectedState (0x01BDU)
#define kStatus_PLL_Success (0x01BEU)
#define kStatus_Success (0x01BFU)
#define kUSART_8BitsPerChar (0x01C0U)
#define kUSART_OneStopBit (0x01C1U)
#define kUSART_ParityDisabled (0x01C2U)
#define kUSART_RxFifo1 (0x01C3U)
#define kUSART_RxLevelInterruptEnable (0x01C4U)
#define kUSART_RxSampleOnFallingEdge (0x01C5U)
#define kUSART_SyncModeDisabled (0x01C6U)
#define kUSART_TxFifo0 (0x01C7U)

uint32_t alxShimLpc55s6x_Call(int unused, ...);
void* alxShimLpc55s6x_CallPtr(int unused, ...);
#define CLOCK_AttachClk(...) alxShimLpc55s6x_Call(0, ##__VA_ARGS__)
#define CLOCK_DisableClock(...) alxShimLpc55s6x_Call(0, ##__VA_ARGS__)
#define CLOCK_EnableClock(...) alxShimLpc55s6x_Call(0, ##__VA_ARGS__)
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
