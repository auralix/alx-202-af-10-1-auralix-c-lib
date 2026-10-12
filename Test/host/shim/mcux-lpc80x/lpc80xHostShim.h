/**
  ******************************************************************************
  * @file		lpc80xHostShim.h
  * @brief		Auralix C Library - Configuration Matrix Fake - MCUXpresso SDK (LPC80x)
  * @copyright	Copyright (C) Auralix d.o.o. All rights reserved.
  *
  * Compile-only: every name of MCUXpresso SDK (LPC80x) the alx code uses, written from
  * that use, so the configuration matrix compiles every alx line under
  * ALX_LPC80X (Test/config/matrix.toml). Nothing here is the vendor's and
  * nothing here runs; the real one is the product's.
  *
  * As the STM32 fake does, it makes no claim about the vendor's signatures
  * and layouts, only that every name exists: one register block and one
  * structure shape hold every member the code names, a call forwards its
  * arguments to one variadic function, a constant has a value no other has.
  * Grown from the compilers' diagnostics by the scratch tool of ALX-1564
  * night goal 4; edited by hand from then on. Where the analysers read a
  * value or a shape (goal 7), it is the vendor's: interrupt 0, the bit-slice
  * shift, the callback a pointer.
  ******************************************************************************
  **/

#ifndef ALX_SHIM_MCUX_LPC80X_LPC80XHOSTSHIM_H
#define ALX_SHIM_MCUX_LPC80X_LPC80XHOSTSHIM_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
typedef enum
{
	NonMaskableInt_IRQn = -14,
	HardFault_IRQn = -13,
	SVCall_IRQn = -5,
	PendSV_IRQn = -2,
	SysTick_IRQn = -1,
	SPI0_IRQn = 0,
	PIN_INT0_IRQn = 1,
	PIN_INT1_IRQn = 2,
	PIN_INT2_IRQn = 3,
	PIN_INT3_IRQn = 4,
	PIN_INT4_IRQn = 5,
	PIN_INT5_IRQn = 6,
	PIN_INT6_IRQn = 7,
	PIN_INT7_IRQn = 8
} IRQn_Type;
#define ALX_SHIM_IRQN_TYPE
#include "cmsisHostShim.h"


typedef struct alxShimLpc80x_Sub
{
	uint32_t MSTCTL;
	uint32_t MSTDAT;
	uint32_t PMCFG;
	uint32_t PRESETCTRL0;
	uint32_t baudRate_Bps;
	uint32_t bitCountPerChar;
	uint32_t channelMask;
	uint32_t channelNumber;
	uint32_t clockDividerNumber;
	uint32_t clockFreq_Hz;
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
	uint32_t warningValue;
	uint32_t windowValue;
	uint32_t* DIR;
	uint32_t* PIO;
} alxShimLpc80x_Sub;

typedef struct alxShimLpc80x_Struct
{
	uint32_t MSTCTL;
	uint32_t MSTDAT;
	uint32_t PMCFG;
	uint32_t PRESETCTRL0;
	uint32_t baudRate_Bps;
	uint32_t bitCountPerChar;
	uint32_t channelMask;
	uint32_t channelNumber;
	uint32_t clockDividerNumber;
	uint32_t clockFreq_Hz;
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
	uint32_t warningValue;
	uint32_t windowValue;
	uint32_t* DIR;
	uint32_t* PIO;
} alxShimLpc80x_Struct;

typedef alxShimLpc80x_Struct CTIMER_Type;
typedef alxShimLpc80x_Struct I2C_Type;
typedef alxShimLpc80x_Struct PINT_Type;
typedef alxShimLpc80x_Struct USART_Type;
typedef alxShimLpc80x_Struct adc_config_t;
typedef alxShimLpc80x_Struct adc_conv_seq_config_t;
typedef alxShimLpc80x_Struct adc_result_info_t;
typedef alxShimLpc80x_Struct ctimer_config_t;
typedef alxShimLpc80x_Struct gpio_pin_config_t;
typedef uint32_t i2c_direction_t;
typedef alxShimLpc80x_Struct i2c_master_config_t;
typedef alxShimLpc80x_Struct pint_pin_enable_t;
typedef uint32_t pint_pin_int_t;
typedef uint32_t status_t;
typedef uint32_t swm_port_pin_type_t;
typedef uint32_t swm_select_fixed_pin_t;
typedef uint32_t swm_select_movable_t;
typedef uint32_t syscon_connection_t;
typedef alxShimLpc80x_Struct usart_config_t;
typedef alxShimLpc80x_Struct wwdt_config_t;

extern alxShimLpc80x_Struct alxShimLpc80x_Instance;
#define GPIO (&alxShimLpc80x_Instance)
#define I2C0 (&alxShimLpc80x_Instance)
#define I2C1 (&alxShimLpc80x_Instance)
#define I2C2 (&alxShimLpc80x_Instance)
#define I2C3 (&alxShimLpc80x_Instance)
#define IOCON (&alxShimLpc80x_Instance)
#define PINT (&alxShimLpc80x_Instance)
#define SYSCON (&alxShimLpc80x_Instance)
#define USART0 (&alxShimLpc80x_Instance)
#define USART1 (&alxShimLpc80x_Instance)
#define USART2 (&alxShimLpc80x_Instance)
#define USART3 (&alxShimLpc80x_Instance)
#define USART4 (&alxShimLpc80x_Instance)
#define ADC (0x0100U)
#define FSL_FEATURE_PINT_NUMBER_OF_CONNECTED_OUTPUTS (8U)
#define I2C_MSTCTL_MSTCONTINUE_MASK (0x0102U)
#define I2C_MSTCTL_MSTSTART_MASK (0x0103U)
#define I2C_MSTCTL_MSTSTOP_MASK (0x0104U)
#define I2C_STAT_MSTARBLOSS_MASK (0x0105U)
#define I2C_STAT_MSTCODE_NACKADR (0x0106U)
#define I2C_STAT_MSTCODE_NACKDAT (0x0107U)
#define I2C_STAT_MSTCODE_RXREADY (0x0108U)
#define I2C_STAT_MSTCODE_TXREADY (0x0109U)
#define I2C_STAT_MSTPENDING_MASK (0x010AU)
#define I2C_STAT_MSTSTATE_MASK (0x010BU)
#define I2C_STAT_MSTSTATE_SHIFT (12U)
#define I2C_STAT_MSTSTSTPERR_MASK (0x010DU)
#define IOCON_MODE_INACT (0x010EU)
#define IOCON_MODE_PULLDOWN (0x010FU)
#define IOCON_MODE_PULLUP (0x0110U)
#define IOCON_MODE_REPEATER (0x0111U)
#define PININT_BITSLICE_CFG_START (8U)
#define SWM0 (0x011BU)
#define WWDT (0x011CU)
#define kADC_Clk_From_Fro (0x011DU)
#define kADC_InterruptForEachConversion (0x011EU)
#define kADC_RST_N_SHIFT_RSTn (15U)
#define kADC_ThresholdCompareInRange (0x0120U)
#define kADC_ThresholdCrossingNoDetected (0x0121U)
#define kADC_TriggerPolarityNegativeEdge (0x0122U)
#define kCLOCK_DivAdcClk (0x0123U)
#define kCLOCK_Gpio0 (0x0124U)
#define kCLOCK_GpioInt (0x0125U)
#define kCLOCK_Iocon (0x0126U)
#define kCLOCK_LPOsc (0x0127U)
#define kCLOCK_Swm (0x0128U)
#define kGPIO0_RST_N_SHIFT_RSTn (9U)
#define kGPIO_DigitalInput (0x012AU)
#define kGPIO_DigitalOutput (0x012BU)
#define kI2C0_Clk_From_MainClk (0x012CU)
#define kI2C1_Clk_From_MainClk (0x012DU)
#define kI2C2_Clk_From_MainClk (0x012EU)
#define kI2C3_Clk_From_MainClk (0x012FU)
#define kI2C_Read (0x0130U)
#define kI2C_TransferDefaultFlag (0x0131U)
#define kI2C_TransferNoStartFlag (0x0132U)
#define kI2C_TransferNoStopFlag (0x0133U)
#define kI2C_Write (0x0134U)
#define kPDRUNCFG_PD_ADC0 (0x0135U)
#define kPINT_PatternMatchNever (0x0136U)
#define kPINT_PinInt0 (0x0137U)
#define kPINT_PinInt1 (0x0138U)
#define kPINT_PinInt2 (0x0139U)
#define kPINT_PinInt3 (0x013AU)
#define kPINT_PinInt4 (0x013BU)
#define kPINT_PinInt5 (0x013CU)
#define kPINT_PinInt6 (0x013DU)
#define kPINT_PinInt7 (0x013EU)
#define kSWM_ACMP_INPUT1 (0x013FU)
#define kSWM_ACMP_INPUT2 (0x0140U)
#define kSWM_ACMP_INPUT3 (0x0141U)
#define kSWM_ACMP_INPUT4 (0x0142U)
#define kSWM_ACMP_INPUT5 (0x0143U)
#define kSWM_ACMP_OUT (0x0144U)
#define kSWM_ADC_CHN0 (0x0145U)
#define kSWM_ADC_CHN1 (0x0146U)
#define kSWM_ADC_CHN10 (0x0147U)
#define kSWM_ADC_CHN11 (0x0148U)
#define kSWM_ADC_CHN2 (0x0149U)
#define kSWM_ADC_CHN3 (0x014AU)
#define kSWM_ADC_CHN4 (0x014BU)
#define kSWM_ADC_CHN5 (0x014CU)
#define kSWM_ADC_CHN6 (0x014DU)
#define kSWM_ADC_CHN7 (0x014EU)
#define kSWM_ADC_CHN8 (0x014FU)
#define kSWM_ADC_CHN9 (0x0150U)
#define kSWM_CAPT_X0 (0x0151U)
#define kSWM_CAPT_X1 (0x0152U)
#define kSWM_CAPT_X2 (0x0153U)
#define kSWM_CAPT_X3 (0x0154U)
#define kSWM_CAPT_X4 (0x0155U)
#define kSWM_CAPT_YH (0x0156U)
#define kSWM_CAPT_YL (0x0157U)
#define kSWM_CLKIN (0x0158U)
#define kSWM_CLKOUT (0x0159U)
#define kSWM_DAC_OUT0 (0x015AU)
#define kSWM_GPIO_INT_BMAT (0x015BU)
#define kSWM_I2C0_SCL (0x015CU)
#define kSWM_I2C0_SDA (0x015DU)
#define kSWM_I2C1_SCL (0x015EU)
#define kSWM_I2C1_SDA (0x015FU)
#define kSWM_LVLSHFT_IN0 (0x0160U)
#define kSWM_LVLSHFT_IN1 (0x0161U)
#define kSWM_LVLSHFT_OUT0 (0x0162U)
#define kSWM_LVLSHFT_OUT1 (0x0163U)
#define kSWM_PLU_CLKIN_IN (0x0164U)
#define kSWM_PortPin_Reset (0x0165U)
#define kSWM_RESETN (0x0166U)
#define kSWM_SPI0_MISO (0x0167U)
#define kSWM_SPI0_MOSI (0x0168U)
#define kSWM_SPI0_SCK (0x0169U)
#define kSWM_SPI0_SSEL0 (0x016AU)
#define kSWM_SPI0_SSEL1 (0x016BU)
#define kSWM_SWCLK (0x016CU)
#define kSWM_SWDIO (0x016DU)
#define kSWM_T0_CAP_CHN0 (0x016EU)
#define kSWM_T0_CAP_CHN1 (0x016FU)
#define kSWM_T0_CAP_CHN2 (0x0170U)
#define kSWM_T0_MAT_CHN0 (0x0171U)
#define kSWM_T0_MAT_CHN1 (0x0172U)
#define kSWM_T0_MAT_CHN2 (0x0173U)
#define kSWM_T0_MAT_CHN3 (0x0174U)
#define kSWM_USART0_CTS (0x0175U)
#define kSWM_USART0_RTS (0x0176U)
#define kSWM_USART0_RXD (0x0177U)
#define kSWM_USART0_SCLK (0x0178U)
#define kSWM_USART0_TXD (0x0179U)
#define kSWM_USART1_RXD (0x017AU)
#define kSWM_USART1_SCLK (0x017BU)
#define kSWM_USART1_TXD (0x017CU)
#define kSWM_USART2_TXD (0x017DU)
#define kSWM_USART3_TXD (0x017EU)
#define kSWM_USART4_TXD (0x017FU)
#define kSWM_VDDCMP (0x0180U)
#define kSWM_WKCLKIN (0x0181U)
#define kSYSCON_GpioPort0Pin0ToPintsel (0x0182U)
#define kSYSCON_GpioPort0Pin10ToPintsel (0x0183U)
#define kSYSCON_GpioPort0Pin11ToPintsel (0x0184U)
#define kSYSCON_GpioPort0Pin12ToPintsel (0x0185U)
#define kSYSCON_GpioPort0Pin13ToPintsel (0x0186U)
#define kSYSCON_GpioPort0Pin14ToPintsel (0x0187U)
#define kSYSCON_GpioPort0Pin15ToPintsel (0x0188U)
#define kSYSCON_GpioPort0Pin16ToPintsel (0x0189U)
#define kSYSCON_GpioPort0Pin17ToPintsel (0x018AU)
#define kSYSCON_GpioPort0Pin18ToPintsel (0x018BU)
#define kSYSCON_GpioPort0Pin19ToPintsel (0x018CU)
#define kSYSCON_GpioPort0Pin1ToPintsel (0x018DU)
#define kSYSCON_GpioPort0Pin20ToPintsel (0x018EU)
#define kSYSCON_GpioPort0Pin21ToPintsel (0x018FU)
#define kSYSCON_GpioPort0Pin22ToPintsel (0x0190U)
#define kSYSCON_GpioPort0Pin23ToPintsel (0x0191U)
#define kSYSCON_GpioPort0Pin24ToPintsel (0x0192U)
#define kSYSCON_GpioPort0Pin25ToPintsel (0x0193U)
#define kSYSCON_GpioPort0Pin26ToPintsel (0x0194U)
#define kSYSCON_GpioPort0Pin27ToPintsel (0x0195U)
#define kSYSCON_GpioPort0Pin28ToPintsel (0x0196U)
#define kSYSCON_GpioPort0Pin29ToPintsel (0x0197U)
#define kSYSCON_GpioPort0Pin2ToPintsel (0x0198U)
#define kSYSCON_GpioPort0Pin30ToPintsel (0x0199U)
#define kSYSCON_GpioPort0Pin3ToPintsel (0x019AU)
#define kSYSCON_GpioPort0Pin4ToPintsel (0x019BU)
#define kSYSCON_GpioPort0Pin5ToPintsel (0x019CU)
#define kSYSCON_GpioPort0Pin7ToPintsel (0x019DU)
#define kSYSCON_GpioPort0Pin8ToPintsel (0x019EU)
#define kSYSCON_GpioPort0Pin9ToPintsel (0x019FU)
#define kStatus_Fail (0x01A0U)
#define kStatus_I2C_Addr_Nak (0x01A1U)
#define kStatus_I2C_ArbitrationLost (0x01A2U)
#define kStatus_I2C_Nak (0x01A3U)
#define kStatus_I2C_StartStopError (0x01A4U)
#define kStatus_I2C_Timeout (0x01A5U)
#define kStatus_I2C_UnexpectedState (0x01A6U)
#define kStatus_Success (0x01A7U)
#define kUART0_Clk_From_MainClk (0x01A8U)
#define kUART1_Clk_From_MainClk (0x01A9U)
#define kUART2_Clk_From_MainClk (0x01AAU)
#define kUART3_Clk_From_MainClk (0x01ABU)
#define kUART4_Clk_From_MainClk (0x01ACU)
#define kUSART_8BitsPerChar (0x01ADU)
#define kUSART_OneStopBit (0x01AEU)
#define kUSART_ParityDisabled (0x01AFU)
#define kUSART_RxSampleOnFallingEdge (0x01B0U)
#define kUSART_SyncModeDisabled (0x01B1U)

uint32_t alxShimLpc80x_Call(int unused, ...);
void* alxShimLpc80x_CallPtr(int unused, ...);
#define ADC_Deinit(...) alxShimLpc80x_Call(0, ##__VA_ARGS__)
#define ADC_DoSoftwareTriggerConvSeqA(...) alxShimLpc80x_Call(0, ##__VA_ARGS__)
#define ADC_EnableConvSeqA(...) alxShimLpc80x_Call(0, ##__VA_ARGS__)
#define ADC_GetChannelConversionResult(...) alxShimLpc80x_Call(0, ##__VA_ARGS__)
#define ADC_GetConvSeqAGlobalConversionResult(...) alxShimLpc80x_Call(0, ##__VA_ARGS__)
#define ADC_Init(...) alxShimLpc80x_Call(0, ##__VA_ARGS__)
#define ADC_SetConvSeqAConfig(...) alxShimLpc80x_Call(0, ##__VA_ARGS__)
#define CLOCK_DisableClock(...) alxShimLpc80x_Call(0, ##__VA_ARGS__)
#define CLOCK_EnableClock(...) alxShimLpc80x_Call(0, ##__VA_ARGS__)
#define CLOCK_GetCoreSysClkFreq(...) alxShimLpc80x_Call(0, ##__VA_ARGS__)
#define CLOCK_GetExtClkFreq(...) alxShimLpc80x_Call(0, ##__VA_ARGS__)
#define CLOCK_GetFreq(...) alxShimLpc80x_Call(0, ##__VA_ARGS__)
#define CLOCK_GetFroFreq(...) alxShimLpc80x_Call(0, ##__VA_ARGS__)
#define CLOCK_GetMainClkFreq(...) alxShimLpc80x_Call(0, ##__VA_ARGS__)
#define CLOCK_Select(...) alxShimLpc80x_Call(0, ##__VA_ARGS__)
#define CLOCK_SetClkDivider(...) alxShimLpc80x_Call(0, ##__VA_ARGS__)
#define GPIO_PinInit(...) alxShimLpc80x_Call(0, ##__VA_ARGS__)
#define GPIO_PinRead(...) alxShimLpc80x_Call(0, ##__VA_ARGS__)
#define GPIO_PinWrite(...) alxShimLpc80x_Call(0, ##__VA_ARGS__)
#define GPIO_PortClear(...) alxShimLpc80x_Call(0, ##__VA_ARGS__)
#define GPIO_PortSet(...) alxShimLpc80x_Call(0, ##__VA_ARGS__)
#define GPIO_PortToggle(...) alxShimLpc80x_Call(0, ##__VA_ARGS__)
#define I2C_GetStatusFlags(...) alxShimLpc80x_Call(0, ##__VA_ARGS__)
#define I2C_MasterClearStatusFlags(...) alxShimLpc80x_Call(0, ##__VA_ARGS__)
#define I2C_MasterDeinit(...) alxShimLpc80x_Call(0, ##__VA_ARGS__)
#define I2C_MasterInit(...) alxShimLpc80x_Call(0, ##__VA_ARGS__)
#define PINT_DisableCallbackByIndex(...) alxShimLpc80x_Call(0, ##__VA_ARGS__)
#define PINT_EnableCallbackByIndex(...) alxShimLpc80x_Call(0, ##__VA_ARGS__)
#define PINT_PinInterruptClrStatus(...) alxShimLpc80x_Call(0, ##__VA_ARGS__)
#define PINT_PinInterruptConfig(base, intr, enable, callback) alxShimLpc80x_Call(0, (base), (intr), (enable), (void*)(callback))
#define POWER_DisablePD(...) alxShimLpc80x_Call(0, ##__VA_ARGS__)
#define POWER_EnablePD(...) alxShimLpc80x_Call(0, ##__VA_ARGS__)
#define RESET_PeripheralReset(...) alxShimLpc80x_Call(0, ##__VA_ARGS__)
#define SWM_SetFixedPinSelect(...) alxShimLpc80x_Call(0, ##__VA_ARGS__)
#define SWM_SetMovablePinSelect(...) alxShimLpc80x_Call(0, ##__VA_ARGS__)
#define SYSCON_AttachSignal(...) alxShimLpc80x_Call(0, ##__VA_ARGS__)
#define USART_Deinit(...) alxShimLpc80x_Call(0, ##__VA_ARGS__)
#define USART_Init(...) alxShimLpc80x_Call(0, ##__VA_ARGS__)
#define USART_WriteBlocking(...) alxShimLpc80x_Call(0, ##__VA_ARGS__)
#define WWDT_Init(...) alxShimLpc80x_Call(0, ##__VA_ARGS__)
#define WWDT_Refresh(...) alxShimLpc80x_Call(0, ##__VA_ARGS__)


#endif	// ALX_SHIM_MCUX_LPC80X_LPC80XHOSTSHIM_H
