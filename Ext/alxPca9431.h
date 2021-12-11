/**
  ******************************************************************************
  * @file alxPca9431.h
  * @brief Auralix C Library - ALX PCA9431 Module
  * @version $LastChangedRevision: 5079 $
  * @date $LastChangedDate: 2021-05-12 19:54:08 +0200 (Wed, 12 May 2021) $
  ******************************************************************************
  */

#ifndef ALX_PCA9431_H
#define ALX_PCA9431_H

#ifdef __cplusplus
extern "C" {
#endif

//******************************************************************************
// Includes
//******************************************************************************
#include "alxGlobal.h"
#include "alxTrace.h"
#include "alxAssert.h"
#include "alxI2c.h"


//******************************************************************************
// Preprocessor
//******************************************************************************
#define ALX_PCA9431_FILE "alxPca9431"

// Assert //
#if defined(_ALX_PCA9431_ASSERT_BKPT) || defined(_ALX_ASSERT_BKPT_ALL)
#define ALX_PCA9431_ASSERT(expr) ALX_ASSERT_BKPT(ALX_PCA9431_FILE, expr)
#elif defined(_ALX_PCA9431_ASSERT_TRACE) || defined(_ALX_ASSERT_TRACE_ALL)
#define ALX_PCA9431_ASSERT(expr) ALX_ASSERT_TRACE(ALX_PCA9431_FILE, expr)
#elif defined(_ALX_PCA9431_ASSERT_RST) || defined(_ALX_ASSERT_RST_ALL)
#define ALX_PCA9431_ASSERT(expr) ALX_ASSERT_RST(ALX_PCA9431_FILE, expr)
#else
#define ALX_PCA9431_ASSERT(expr) do{} while (false)
#endif

// Trace //
#if defined(_ALX_PCA9431_TRACE) || defined(_ALX_TRACE_ALL)
#define ALX_PCA9431_TRACE(...) ALX_TRACE_STD(ALX_PCA9431_FILE, __VA_ARGS__)
#else
#define ALX_PCA9431_TRACE(...) do{} while (false)
#endif


//******************************************************************************
// Register Values Unions
//******************************************************************************

//------------------------------------------------------------------------------
// Device ID Register (Address 00h)
//------------------------------------------------------------------------------
typedef union
{
	struct __attribute__((packed))
	{
		uint8_t Revision : 3;
		uint8_t ID : 4;
		uint8_t unused_7 : 1; // bit 7 is unused
	};
	uint8_t raw;
} AlxPca9431_RegVal_00h_DeviceId;

//------------------------------------------------------------------------------
// SYSTEM_INT Register (Address 01h)
//------------------------------------------------------------------------------
typedef enum
{
	TempShutdownInt_NotTrig = 0b0,
	TempShutdownInt_Trig    = 0b1
} AlxPca9431_01h_TempShutdownInt;
typedef enum
{
	TempWarnInt_Lesser   = 0b0, // die temp < TWRN
	TempWarnInt_Gereater = 0b1 // die temp ≥ TWRN
} AlxPca9431_01h_TempWarnInt;
typedef enum
{
	WdTimerExpInt_NotHappened = 0b0,
	WdTimerExpInt_Happened    = 0b1
} AlxPca9431_01h_WdTimerExpInt;
typedef enum
{
	VpwrScpInt_NotTrig = 0b0,
	VpwrScpInt_Trig    = 0b1
} AlxPca9431_01h_VpwrScpInt;
typedef union
{
	struct __attribute__((packed))
	{
		AlxPca9431_01h_TempShutdownInt Temp_SHD_INT : 1;
		AlxPca9431_01h_TempWarnInt Temp_WRN_INT : 1;
		AlxPca9431_01h_WdTimerExpInt WD_TIMER_INT : 1;
		AlxPca9431_01h_VpwrScpInt VPWR_SCP_INT : 1;
		uint8_t unused_4_7 : 4; // bits 4..7 are unused
	};
	uint8_t raw;
} AlxPca9431_RegVal_01h_SystemInt;

//------------------------------------------------------------------------------
// SYSTEM_INT_MASK Register (Address 02h)
//------------------------------------------------------------------------------
typedef enum
{
	TempShutdownIntMask_Trig    = 0b0,
	TempShutdownIntMask_NotTrig = 0b1
} AlxPca9431_02h_TempShutdownIntMask;
typedef enum
{
	TempWarnIntMask_Trig    = 0b0,
	TempWarnIntMask_NotTrig = 0b1
} AlxPca9431_02h_TempWarnIntMask;
typedef enum
{
	WdTimerExpIntMask_Trig    = 0b0,
	WdTimerExpIntMask_NotTrig = 0b1
} AlxPca9431_02h_WdTimerExpIntMask;
typedef enum
{
	VpwrScpIntMask_Trig    = 0b0,
	VpwrScpIntMask_NotTrig = 0b1
} AlxPca9431_02h_VpwrScpIntMask;
typedef union
{
	struct __attribute__((packed))
	{
		AlxPca9431_02h_TempShutdownIntMask Temp_SHD_INT_MSK : 1;
		AlxPca9431_02h_TempWarnIntMask Temp_WRN_INT_MSK : 1;
		AlxPca9431_02h_WdTimerExpIntMask WD_TIMER_INT_MSK : 1;
		AlxPca9431_02h_VpwrScpIntMask VPWR_SCP_INT_MSK : 1;
		uint8_t unused_4_7 : 4; // bits 4..7 are unused
	};
	uint8_t raw;
} AlxPca9431_RegVal_02h_SystemIntMask;

//------------------------------------------------------------------------------
// VRECT_INT Register (Address 03h)
//------------------------------------------------------------------------------
typedef enum
{
	VRectOvProtInt_NotTrig = 0b0,
	VRectOvProtInt_Trig    = 0b1
} AlxPca9431_03h_VRectOvProtInt;
typedef enum
{
	VRectOvWarnInt_Lesser  = 0b0,
	VRectOvWarnInt_Greater = 0b1
} AlxPca9431_03h_VRectOvWarnInt;
typedef enum
{
	VRectRegLowInt_NotTrig = 0b0,
	VRectRegLowInt_Trig    = 0b1
} AlxPca9431_03h_VRectRegLowInt;
typedef enum
{
	VRectRegHighInt_NoTrig = 0b0,
	VRectRegHighInt_Trig   = 0b1
} AlxPca9431_03h_VRectRegHighInt;
typedef enum
{
	VRectGoodInt_NotInRange = 0b0,
	VRectGoodInt_InRange    = 0b1
} AlxPca9431_03h_VRectGoodInt;
typedef enum
{
	VRectRdyInt_Lesser  = 0b0,
	VRectRdyInt_Greater = 0b1
} AlxPca9431_03h_VRectRdyInt;
typedef union
{
	struct __attribute__((packed))
	{
		AlxPca9431_03h_VRectOvProtInt VRECT_OVP_INT : 1;
		AlxPca9431_03h_VRectOvWarnInt VRECT_OVW_INT : 1;
		uint8_t unused_2 : 1; // bit 2 is unused
		AlxPca9431_03h_VRectRegLowInt VRECT_REGLOW_INT : 1;
		AlxPca9431_03h_VRectRegHighInt VRECT_REGHIGH_INT : 1;
		AlxPca9431_03h_VRectGoodInt VRECT_GOOD_INT : 1;
		AlxPca9431_03h_VRectRdyInt VRECT_RDY_INT : 1;
		uint8_t unused_7 : 1; // bit 7 is unused
	};
	uint8_t raw;
} AlxPca9431_RegVal_03h_VRectInt;

//------------------------------------------------------------------------------
// VRECT_INT_MASK Register (Address 04h)
//------------------------------------------------------------------------------
typedef enum
{
	VRectOvProtIntMask_Trig    = 0b0,
	VRectOvProtIntMask_NotTrig = 0b1
} AlxPca9431_04h_VRectOvProtIntMask;
typedef enum
{
	VRectOvWarnIntMask_Trig    = 0b0,
	VRectOvWarnIntMask_NotTrig = 0b1
} AlxPca9431_04h_VRectOvWarnIntMask;
typedef enum
{
	VRectRegLowIntMask_Trig    = 0b0,
	VRectRegLowIntMask_NotTrig = 0b1
} AlxPca9431_04h_VRectRegLowIntMask;
typedef enum
{
	VRectRegHighIntMask_Trig    = 0b0,
	VRectRegHighIntMask_NotTrig = 0b1
} AlxPca9431_04h_VRectRegHighIntMask;
typedef enum
{
	VRectGoodIntMask_Trig    = 0b0,
	VRectGoodIntMask_NotTrig = 0b1
} AlxPca9431_04h_VRectGoodIntMask;
typedef enum
{
	VRectRdyIntMask_Trig    = 0b0,
	VRectRdyIntMask_NotTrig = 0b1
} AlxPca9431_04h_VRectRdyIntMask;
typedef union
{
	struct __attribute__((packed))
	{
		AlxPca9431_04h_VRectOvProtIntMask VRECT_OVP_INT_MSK : 1;
		AlxPca9431_04h_VRectOvWarnIntMask VRECT_OVW_MSK : 1;
		uint8_t unused_2 : 1; // bit 2 is unused/
		AlxPca9431_04h_VRectRegLowIntMask VRECT_REGLOW_INT_MSK : 1;
		AlxPca9431_04h_VRectRegHighIntMask VRECT_REGHIGH_INT_MSK : 1;
		AlxPca9431_04h_VRectGoodIntMask VRECT_GOOD_INT_MSK : 1;
		AlxPca9431_04h_VRectRdyIntMask VRECT_RDY_INT_MSK : 1;
		uint8_t unused_7 : 1; // bit 7 is unused
	};
	uint8_t raw;
} AlxPca9431_RegVal_04h_VRectIntMask;

//------------------------------------------------------------------------------
// VOUTLDO_INT Register (Address 05h)
//------------------------------------------------------------------------------
typedef enum
{
	VOutLdoShortInt_NotDetect = 0b0,
	VOutLdoShortInt_Detect    = 0b1
} AlxPca9431_05h_VOutLdoShortInt;
typedef union
{
	struct __attribute__((packed))
	{
		uint8_t unused_0_5 : 6; // bits 0...5 are unused
		AlxPca9431_05h_VOutLdoShortInt VOUT_SHORT_INT : 1;
		uint8_t unused_7 : 1; // bit 7 is unused
	};
	uint8_t raw;
} AlxPca9431_RegVal_05h_VOutLdoInt;

//------------------------------------------------------------------------------
// VOUTLDO_INT_MASK Register (Address 06h)
//------------------------------------------------------------------------------
typedef enum
{
	VOutLdoShortInt_NotMasked = 0b0,
	VOutLdoShortInt_Masked    = 0b1
} AlxPca9431_06h_VOutLdoShortIntMask;
typedef union
{
	struct __attribute__((packed))
	{
		uint8_t unused_0_5 : 6; // bits 0...5 are unused
		AlxPca9431_06h_VOutLdoShortIntMask VOUT_SHORT_INT_MASK : 1;
		uint8_t unused_7 : 1; // bit 7 is unused+
	};
	uint8_t raw;
} AlxPca9431_RegVal_06h_VOutLdoIntMask;

//------------------------------------------------------------------------------
// VOUT_LDO Register (Address 07h)
//------------------------------------------------------------------------------
typedef enum
{
	VoutLdo_3V3 = 0b0,
	VoutLdo_5V  = 0b1
} AlxPca9431_07h_VOutLdo;
typedef union
{
	struct __attribute__((packed))
	{
		uint8_t unused_0_6 : 7; // bits 0...6 are unused
		AlxPca9431_07h_VOutLdo VOUT_LDO : 1; // Voltage of VOUT, when the device is set at LDO mode
	};
	uint8_t raw;
} AlxPca9431_RegVal_07h_VOutLdo;

//------------------------------------------------------------------------------
// VRECT_THD Register (Address 08h)
//------------------------------------------------------------------------------
typedef enum
{
	VRectThd_3V  = 0b00,
	VRectThd_3V1 = 0b01,
	VRectThd_3V2 = 0b10,
	VRectThd_3V3 = 0b11
} AlxPca9431_08h_VRectThd;
typedef union
{
	struct __attribute__((packed))
	{
		uint8_t unused_0_3 : 4; // bits 0...3 are unused
		AlxPca9431_08h_VRectThd	VRECT_UVLO : 2;
		uint8_t unused_6_7 : 2; // bits 6...7 are unused
	};
	uint8_t raw;
} AlxPca9431_RegVal_08h_VRectThd;

//------------------------------------------------------------------------------
// VRECT_OVW Register (Address 09h)
//------------------------------------------------------------------------------
typedef enum
{
	VRectOvw_7V   = 0b00,
	VRectOvw_7V25 = 0b01,
	VRectOvw_7V5  = 0b10,
	VRectOvw_7V75 = 0b11
} AlxPca9431_09h_VRectOvw;
typedef union
{
	struct __attribute__((packed))
	{
		AlxPca9431_09h_VRectOvw	VRECT_OVW : 2;
		uint8_t unused_2_7 : 6; // bits 2...7 are unused
	};
	uint8_t raw;
} AlxPca9431_RegVal_09h_VRectOvw;

//------------------------------------------------------------------------------
// TEMP_THD Register (Address 0Ah)
//------------------------------------------------------------------------------
typedef enum
{
	TempShutdownThd_120C  = 0b000,
	TempShutdownThd_125C  = 0b001,
	TempShutdownThd_130C  = 0b010,
	TempShutdownThd_135C  = 0b011,
	TempShutdownThd_140C  = 0b100,
	TempShutdownThd_145C  = 0b101,
	TempShutdownThd_150C  = 0b110,
	TempShutdownThd_150C_ = 0b111
} AlxPca9431_0Ah_TempShutdownThd;
typedef enum
{
	TempWarnThd_80C  = 0b00,
	TempWarnThd_90C  = 0b01,
	TempWarnThd_100C = 0b10,
	TempWarnThd_110C = 0b11
} AlxPca9431_0Ah_TempWarnThd;
typedef union
{
	struct __attribute__((packed))
	{
		AlxPca9431_0Ah_TempShutdownThd TEMP_SHDN : 3;
		AlxPca9431_0Ah_TempWarnThd TEMP_WRN : 2;
		uint8_t unused_5_7 : 3; // bits 5...7 are unused
	};
	uint8_t raw;
} AlxPca9431_RegVal_0Ah_TempThd;

//------------------------------------------------------------------------------
// WD_EN_RST Register (Address 0Bh)
//------------------------------------------------------------------------------
typedef enum
{
	I2CSetting_DoNothing    = 0b0,
	I2CSetting_DefaultReset = 0b1
} AlxPca9431_0Bh_I2CSetting;
typedef enum
{
	VOutLdoEn_Disabled = 0b0,
	VOutLdoEn_Enabled  = 0b1
} AlxPca9431_0Bh_VOutLdoEn;
typedef enum
{
	VaractorEn_Disabled = 0b0,
	VaractorEn_Enabled  = 0b1
} AlxPca9431_0Bh_VaractorEn;
typedef enum
{
	EcoExt_NotExit = 0b0,
	EcoExt_Exit    = 0b1
} AlxPca9431_0Bh_EcoExt;
typedef enum
{
	WatchdogTimer_Disabled = 0b00,
	WatchdogTimer_16s      = 0b01,
	WatchdogTimer_32s      = 0b10,
	WatchdogTimer_64s      = 0b11
} AlxPca9431_0Bh_WatchdogTimer;
typedef union
{
	struct __attribute__((packed))
	{
		AlxPca9431_0Bh_I2CSetting SW_RST: 1;
		AlxPca9431_0Bh_VOutLdoEn VOUT_LDO_EN : 1;
		AlxPca9431_0Bh_VaractorEn VARACTOR_EN : 1;
		uint8_t unused_3 : 1; // bit 3 is unused
		AlxPca9431_0Bh_EcoExt ECO_EXIT : 1;
		uint8_t unused_5 : 1; // bit 5 is unused
		AlxPca9431_0Bh_WatchdogTimer WATCH_DOG : 2;
	};
	uint8_t raw;
} AlxPca9431_RegVal_0Bh_WatchdogSoftStartEnMcu;

//------------------------------------------------------------------------------
// Varactor_DAC Register (Address 0Ch)
//------------------------------------------------------------------------------
typedef union
{
	struct __attribute__((packed))
	{
		uint8_t Varactor_DAC : 5; // VARACTOR DAC 5-bit setting, please refer to Section 8.10
		uint8_t unused_5_7 : 3; // bits 5...7 are unused
	};
	uint8_t raw;
} AlxPca9431_RegVal_0Ch_VaractorDac;

//------------------------------------------------------------------------------
// ADC_CONTROL Register (Address 0Dh)
//------------------------------------------------------------------------------
typedef enum
{
	AdcAveragingEn_Disabled = 0b0,
	AdcAveragingEn_Enabled  = 0b1
} AlxPca9431_0Dh_AdcAveragingEn;
typedef enum
{
	AdcRate_1ShotConversion      = 0b00,
	AdcRate_ContinuousConversion = 0b01,
	AdcRate_BurstMode            = 0b10, // Both Burst Modes are the same
	AdcRate_BurstMode_           = 0b11  // Both Burst Modes are the same
} AlxPca9431_0Dh_AdcRate;
typedef enum
{
	AdcEn_Disabled = 0b0,
	AdcEn_Enabled  = 0b1
} AlxPca9431_0Dh_AdcEn;
typedef union
{
	struct __attribute__((packed))
	{
		uint8_t unused_3 : 1; // bit 3 is unused
		AlxPca9431_0Dh_AdcAveragingEn ADC_AVG_EN : 1;
		AlxPca9431_0Dh_AdcRate ADC_RATE : 2; 
		AlxPca9431_0Dh_AdcEn ADC_EN : 1;
		uint8_t unused_5_7 : 3; // bits 5...7 are unused
	};
	uint8_t raw;
} AlxPca9431_RegVal_0Dh_AdcControl;

//------------------------------------------------------------------------------
// SAMPLE_EN Register (Address 0Eh)
//------------------------------------------------------------------------------
typedef enum
{
	NtcAdcSamplingEn_Disabled = 0b0,
	NtcAdcSamplingEn_Enabled  = 0b1
} AlxPca9431_0Eh_NtcAdcSamplingEn;
typedef enum
{
	OtpAdcSamplingEn_Disabled = 0b0,
	OtpAdcSamplingEn_Enabled  = 0b1
} AlxPca9431_0Eh_OtpAdcSamplingEn;
typedef enum
{
	IOutAdcSamplingEn_Disabled = 0b0,
	IOutAdcSamplingEn_Enabled  = 0b1
} AlxPca9431_0Eh_IOutAdcSamplingEn;
typedef enum
{
	VOutAdcSamplingEn_Disabled = 0b0,
	VOutAdcSamplingEn_Enabled = 0b1
} AlxPca9431_0Eh_VOutAdcSamplingEn;
typedef enum
{
	IRectAdcSamplingEn_Disabled = 0b0,
	IRectAdcSamplingEn_Enabled  = 0b1
} AlxPca9431_0Eh_IRectAdcSamplingEn;
typedef enum
{
	VRectAdcSamplingEn_Disabled = 0b0,
	VRectAdcSamplingEn_Enabled  = 0b1
} AlxPca9431_0Eh_VRectAdcSamplingEn;
typedef enum
{
	VTuneAdcSamplingEn_Disabled = 0b0,
	VTuneAdcSamplingEn_Enabled  = 0b1
} AlxPca9431_0Eh_VTuneAdcSamplingEn;
typedef union
{
	struct __attribute__((packed))
	{
		AlxPca9431_0Eh_NtcAdcSamplingEn NTC_ADC_EN : 1;
		AlxPca9431_0Eh_OtpAdcSamplingEn OTP_ADC_EN : 1;
		AlxPca9431_0Eh_IOutAdcSamplingEn IOUT_ADC_EN : 1;
		AlxPca9431_0Eh_VOutAdcSamplingEn VOUT_ADC_EN : 1;
		AlxPca9431_0Eh_IRectAdcSamplingEn IRECT_ADC_EN : 1;
		AlxPca9431_0Eh_VRectAdcSamplingEn VRECT_ADC_EN : 1;
		AlxPca9431_0Eh_VTuneAdcSamplingEn VTUNE_ADC_EN : 1;
		uint8_t unused_7 : 1; // bit 7 is unused
	};
	uint8_t raw;
} AlxPca9431_RegVal_0Eh_AdcSampleEn;

//------------------------------------------------------------------------------
// VPWR_CONFIG Register (Address 0Fh)
//------------------------------------------------------------------------------
typedef enum
{
	VPwrOut_1V8 = 0b00,
	VPwrOut_2V5 = 0b01,
	VPwrOut_2V8 = 0b10,
	VPwrOut_3V3 = 0b11
} AlxPca9431_0Fh_VPwrOut;
typedef enum
{
	VPwrLdoOutEn_Disabled = 0b0,
	VPwrLdoOutEn_Enabled  = 0b1
} AlxPca9431_0Fh_VPwrLdoOutEn;
typedef union
{
	struct __attribute__((packed))
	{
		AlxPca9431_0Fh_VPwrOut VPWR_VL : 2;
		uint8_t unused_2_6 : 5; // bits 2...6 are unused
		AlxPca9431_0Fh_VPwrLdoOutEn VPWR_EN : 1;
	};
	uint8_t raw;
} AlxPca9431_RegVal_0Fh_VPwrLdoConf;

//------------------------------------------------------------------------------
// RXIR_CONFIG Register (Address 10h)
//------------------------------------------------------------------------------
typedef enum
{
	RxirPullDown_20R  = 0b000,
	RxirPullDown_80R  = 0b001,
	RxirPullDown_140R = 0b010,
	RxirPullDown_200R = 0b011,
	RxirPullDown_260R = 0b100,
	RxirPullDown_320R = 0b101,
	RxirPullDown_380R = 0b110,
	RxirPullDown_440R = 0b111
} AlxPca9431_10h_RxirPullDown;
typedef enum
{
	RxirPullDown0ms2 = 0b000,
	RxirPullDown0ms4 = 0b001,
	RxirPullDown0ms6 = 0b010,
	RxirPullDown0ms8 = 0b011,
	RxirPullDown1ms0 = 0b100,
	RxirPullDown1ms2 = 0b101,
	RxirPullDown1ms4 = 0b110,
	RxirPullDown1ms6 = 0b111
} AlxPca9431_10h_RxirPullDownDuration;
typedef enum
{
	RxirPullDownEn_Disabled = 0b0,
	RxirPullDownEn_Enabled  = 0b1
} AlxPca9431_10h_RxirPullDownEn;
typedef union
{
	struct __attribute__((packed))
	{
		AlxPca9431_10h_RxirPullDown RXIR_STR : 3;
		AlxPca9431_10h_RxirPullDownDuration RXIR_DUR : 3;
		uint8_t unused_6 : 1; // bit 6 is unused
		AlxPca9431_10h_RxirPullDownEn RXIR_EN : 1;
	};
	uint8_t raw;
} AlxPca9431_RegVal_10h_Rxir_Conf;

//------------------------------------------------------------------------------
// OCPSET_LOCK Register (Address 20h)
//------------------------------------------------------------------------------
typedef enum
{
	OpcLdoNotLockReturnPresentReg0x00	= 0x00, /*	R/W		00h: Return present register	*/
	OpcLdoNotLockReturnPresentReg0x01	= 0x01, /*	R/W		01h: Return present register	*/
	OpcLdoNotLockReturnPresentReg0x02	= 0x02, /*	R/W		02h: Return present register	*/
	OpcLdoNotLockReturnPresentReg0x03	= 0x03, /*	R/W		03h: Return present register	*/
	OpcLdoNotLockReturnPresentReg0x04	= 0x04, /*	R/W		04h: Return present register	*/
	OpcLdoNotLockReturnPresentReg0x05	= 0x05, /*	R/W		05h: Return present register	*/
	OpcLdoNotLockReturnPresentReg0x06	= 0x06, /*	R/W		06h: Return present register	*/
	OpcLdoNotLockReturnPresentReg0x07	= 0x07, /*	R/W		07h: Return present register	*/
	OpcLdoNotLockReturnPresentReg0x08	= 0x08, /*	R/W		08h: Return present register	*/
	OpcLdoNotLockReturnPresentReg0x09	= 0x09, /*	R/W		09h: Return present register	*/
	OpcLdoNotLockReturnPresentReg0x0A	= 0x0A, /*	R/W		0Ah: Return present register	*/
	OpcLdoNotLockReturnPresentReg0x0B	= 0x0B, /*	R/W		0Bh: Return present register	*/
	OpcLdoNotLockReturnPresentReg0x0C	= 0x0C, /*	R/W		0Ch: Return present register	*/
	OpcLdoNotLockReturnPresentReg0x0D	= 0x0D, /*	R/W		0Dh: Return present register	*/
	OpcLdoNotLockReturnPresentReg0x0E	= 0x0E, /*	R/W		0Eh: Return present register	*/
	OpcLdoNotLockReturnPresentReg0x0F	= 0x0F, /*	R/W		0Fh: Return present register	*/
	OpcLdoNotLockReturnPresentReg0x10	= 0x10, /*	R/W		10h: Return present register	*/
	OpcLdoNotLockReturnPresentReg0x11	= 0x11, /*	R/W		11h: Return present register	*/
	OpcLdoNotLockReturnPresentReg0x12	= 0x12, /*	R/W		12h: Return present register	*/
	OpcLdoNotLockReturnPresentReg0x13	= 0x13, /*	R/W		13h: Return present register	*/
	OpcLdoNotLockReturnPresentReg0x14	= 0x14, /*	R/W		14h: Return present register	*/
	OpcLdoLock = 0x15,							/*	R/W		00h: OCPSET_LOCK[4:0] = 10101, VOUTLDO_OCP register can be accessed to perform I2C “write” command. */
	OpcLdoNotLockReturnPresentReg0x16	= 0x16, /*	R/W		00h: Return present register	*/
	OpcLdoNotLockReturnPresentReg0x17	= 0x17, /*	R/W		00h: Return present register	*/
	OpcLdoNotLockReturnPresentReg0x18	= 0x18, /*	R/W		00h: Return present register	*/
	OpcLdoNotLockReturnPresentReg0x19	= 0x19, /*	R/W		00h: Return present register	*/
	OpcLdoNotLockReturnPresentReg0x1A	= 0x1A, /*	R/W		00h: Return present register	*/
	OpcLdoNotLockReturnPresentReg0x1B	= 0x1B, /*	R/W		00h: Return present register	*/
	OpcLdoNotLockReturnPresentReg0x1C	= 0x1C, /*	R/W		00h: Return present register	*/
	OpcLdoNotLockReturnPresentReg0x1D	= 0x1D, /*	R/W		00h: Return present register	*/
	OpcLdoNotLockReturnPresentReg0x1E	= 0x1E, /*	R/W		00h: Return present register	*/
	OpcLdoNotLockReturnPresentReg0x1F	= 0x1F, /*	R/W		00h: Return present register	*/
} AlxPca9431_20h_OpcLdoSetLock;
typedef union
{
	struct __attribute__((packed))
	{
		uint8_t unused_0_2 : 3; // bits 0...2 are unused
		AlxPca9431_20h_OpcLdoSetLock OCPSET_LOCK : 5;	/* VOUT LDO over current protection setting:	*/
	};
	uint8_t raw;
} AlxPca9431_RegVal_20h_OpcLdoSetLock;

//------------------------------------------------------------------------------
// VOUTLDO_OCP Register (Address 21h)
//------------------------------------------------------------------------------
typedef enum
{
	VOutLdoSetOCP_0mA   = 0x00,
	VOutLdoSetOCP_5mA   = 0x01,
	VOutLdoSetOCP_10mA  = 0x02,
	VOutLdoSetOCP_15mA  = 0x03,
	VOutLdoSetOCP_20mA  = 0x04,
	VOutLdoSetOCP_25mA  = 0x05,
	VOutLdoSetOCP_30mA  = 0x06,
	VOutLdoSetOCP_35mA  = 0x07,
	VOutLdoSetOCP_40mA  = 0x08,
	VOutLdoSetOCP_45mA  = 0x09,
	VOutLdoSetOCP_50mA  = 0x0A,
	VOutLdoSetOCP_55mA  = 0x0B,
	VOutLdoSetOCP_60mA  = 0x0C,
	VOutLdoSetOCP_65mA  = 0x0D,
	VOutLdoSetOCP_70mA  = 0x0E,
	VOutLdoSetOCP_75mA  = 0x0F,
	VOutLdoSetOCP_80mA  = 0x10,
	VOutLdoSetOCP_85mA  = 0x11,
	VOutLdoSetOCP_90mA  = 0x12,
	VOutLdoSetOCP_95mA  = 0x13,
	VOutLdoSetOCP_100mA = 0x14,
	VOutLdoSetOCP_105mA = 0x15,
	VOutLdoSetOCP_110mA = 0x16,
	VOutLdoSetOCP_115mA = 0x17,
	VOutLdoSetOCP_120mA = 0x18,
	VOutLdoSetOCP_125mA = 0x19,
	VOutLdoSetOCP_130mA = 0x1A,
	VOutLdoSetOCP_135mA = 0x1B,
	VOutLdoSetOCP_140mA = 0x1C,
	VOutLdoSetOCP_145mA = 0x1D,
	VOutLdoSetOCP_150mA = 0x1E,
	VOutLdoSetOCP_155mA = 0x1F,
	VOutLdoSetOCP_160mA = 0x20,
	VOutLdoSetOCP_165mA = 0x21,
	VOutLdoSetOCP_170mA = 0x22,
	VOutLdoSetOCP_175mA = 0x23,
	VOutLdoSetOCP_180mA = 0x24,
	VOutLdoSetOCP_185mA = 0x25,
	VOutLdoSetOCP_190mA = 0x26,
	VOutLdoSetOCP_195mA = 0x27,
	VOutLdoSetOCP_200mA = 0x28,
	VOutLdoSetOCP_205mA = 0x29,
	VOutLdoSetOCP_210mA = 0x2A,
	VOutLdoSetOCP_215mA = 0x2B,
	VOutLdoSetOCP_220mA = 0x2C,
	VOutLdoSetOCP_225mA = 0x2D,
	VOutLdoSetOCP_230mA = 0x2E,
	VOutLdoSetOCP_235mA = 0x2F,
	VOutLdoSetOCP_240mA = 0x30,
	VOutLdoSetOCP_245mA = 0x31,
	VOutLdoSetOCP_250mA = 0x32,
	VOutLdoSetOCP_255mA = 0x33,
	VOutLdoSetOCP_260mA = 0x34,
	VOutLdoSetOCP_265mA = 0x35,
	VOutLdoSetOCP_270mA = 0x36,
	VOutLdoSetOCP_275mA = 0x37,
	VOutLdoSetOCP_280mA = 0x38,
	VOutLdoSetOCP_285mA = 0x39,
	VOutLdoSetOCP_290mA = 0x3A,
	VOutLdoSetOCP_295mA = 0x3B,
	VOutLdoSetOCP_300mA = 0x3C,
	VOutLdoSetOCP_305mA = 0x3D,
	VOutLdoSetOCP_310mA = 0x3E,
	VOutLdoSetOCP_315mA = 0x3F,
} AlxPca9431_21h_VOutLdoSetOCP;
typedef union
{
	struct __attribute__((packed))
	{
		AlxPca9431_21h_VOutLdoSetOCP VOUTLDO_OCP : 6;
		uint8_t unused_6_7 : 2; // bits 6...7 are unused
	};
	uint8_t raw;
} AlxPca9431_RegVal_21h_VOutLdoSetOCP;

//------------------------------------------------------------------------------
// VRECT_ADC Register (Address 30h)
//------------------------------------------------------------------------------
typedef union
{
	struct __attribute__((packed))
	{
		uint8_t VRECT : 8; // Voltage resolution is 15.82 mV
	};
	uint8_t raw;
} AlxPca9431_RegVal_30h_VRectAdcH;

//------------------------------------------------------------------------------
// VRECT_ADC Register (Address 31h)
//------------------------------------------------------------------------------
typedef union
{
	struct __attribute__((packed))
	{
		uint8_t unused_0_5 : 6; // bits 0...5 are unused
		uint8_t VRECT : 2; // Voltage resolution is 15.82 mV
	};
	uint8_t raw;
} AlxPca9431_RegVal_31h_VRectAdcL;

//------------------------------------------------------------------------------
// VTUNE_ADC Register (Address 32h)
//------------------------------------------------------------------------------
typedef union
{
	struct __attribute__((packed))
	{
		uint8_t VTUNE : 8; // Voltage resolution is 3.32 mV
	};
	uint8_t raw;
} AlxPca9431_RegVal_32h_VTuneAdcH;

//------------------------------------------------------------------------------
// VTUNE_ADC Register (Address 33h)
//------------------------------------------------------------------------------
typedef union
{
	struct __attribute__((packed))
	{
		uint8_t unused_0_5 : 6; // bits 0...5 are unused/
		uint8_t VTUNE : 2; // Voltage resolution is 3.32 mV
	};
	uint8_t raw;
} AlxPca9431_RegVal_33h_VTuneAdcL;

//------------------------------------------------------------------------------
// VOUT_ADC Register (Address 34h)
//------------------------------------------------------------------------------
typedef union
{
	struct __attribute__((packed))
	{
		uint8_t VOUT : 8; // Voltage resolution is 5.27 mV
	};
	uint8_t raw;
} AlxPca9431_RegVal_34h_VOutAdcH;

//------------------------------------------------------------------------------
// VOUT_ADC Register (Address 35h)
//------------------------------------------------------------------------------
typedef union
{
	struct __attribute__((packed))
	{
		uint8_t unused_0_5 : 6; // bits 0...5 are unused
		uint8_t VOUT : 2; // Voltage resolution is 5.27 mV
	};
	uint8_t raw;
} AlxPca9431_RegVal_35h_VOutAdcL;

//------------------------------------------------------------------------------
// IOUT_ADC Register (Address 36h)
//------------------------------------------------------------------------------
typedef union
{
	struct __attribute__((packed))
	{
		uint8_t IOUT : 8; // Voltage resolution is 421.8uA
	};
	uint8_t raw;
} AlxPca9431_RegVal_36h_IOutAdcH;

//------------------------------------------------------------------------------
// IOUT_ADC Register (Address 37h)
//------------------------------------------------------------------------------
typedef union
{
	struct __attribute__((packed))
	{
		uint8_t unused_0_5 : 6; // bits 0...5 are unused
		uint8_t IOUT : 2; // Voltage resolution is 421.8uA
	};
	uint8_t raw;
} AlxPca9431_RegVal_37h_IOutAdcL;

//------------------------------------------------------------------------------
// IRECT_ADC Register (Address 38h)
//------------------------------------------------------------------------------
typedef union
{
	struct __attribute__((packed))
	{
		uint8_t IRECT : 8; // Voltage resolution is 421.8uA
	};
	uint8_t raw;
} AlxPca9431_RegVal_38h_IRectAdcH;

//------------------------------------------------------------------------------
// IRECT_ADC Register (Address 39h)
//------------------------------------------------------------------------------
typedef union
{
	struct __attribute__((packed))
	{
		uint8_t unused_0_5 : 6; // bits 0...5 are unused
		uint8_t IRECT : 2; // Voltage resolution is 421.8uA
	};
	uint8_t raw;
} AlxPca9431_RegVal_39h_IRectAdcL;

//------------------------------------------------------------------------------
// TDIE_ADC Register (Address 3Ah)
//------------------------------------------------------------------------------
typedef enum
{
	ChipTempAdc_43C		= 0x00,
	ChipTempAdc_39C		= 0x01,
	ChipTempAdc_35C		= 0x02,
	ChipTempAdc_30C		= 0x03,
	ChipTempAdc_26C		= 0x04,
	ChipTempAdc_21C		= 0x05,
	ChipTempAdc_17C		= 0x06,
	ChipTempAdc_12C5	= 0x07,
	ChipTempAdc_8C		= 0x08,
	ChipTempAdc_4C		= 0x09,
	ChipTempAdc0C		= 0x0A,
	ChipTempAdc5C		= 0x0B,
	ChipTempAdc9C		= 0x0C,
	ChipTempAdc13C5		= 0x0D,
	ChipTempAdc18C		= 0x0E,
	ChipTempAdc22C5		= 0x0F,
	ChipTempAdc27C		= 0x10,
	ChipTempAdc31C		= 0x11,
	ChipTempAdc35C		= 0x12,
	ChipTempAdc40C		= 0x13,
	ChipTempAdc44C		= 0x14,
	ChipTempAdc48C		= 0x15,
	ChipTempAdc53C		= 0x16,
	ChipTempAdc57C		= 0x17,
	ChipTempAdc62C		= 0x18,
	ChipTempAdc65C5		= 0x19,
	ChipTempAdc71C		= 0x1A,
	ChipTempAdc75C		= 0x1B,
	ChipTempAdc79C5		= 0x1C,
	ChipTempAdc84C		= 0x1D,
	ChipTempAdc88C		= 0x1E,
	ChipTempAdc92C		= 0x1F,
	ChipTempAdc96C		= 0x20,
	ChipTempAdc100C5	= 0x21,
	ChipTempAdc105C		= 0x22,
	ChipTempAdc109C		= 0x23,
	ChipTempAdc114C		= 0x24,
	ChipTempAdc118C		= 0x25,
	ChipTempAdc122C		= 0x26,
	ChipTempAdc126C		= 0x27,
	ChipTempAdc130C		= 0x28,
	ChipTempAdc134C5	= 0x29,
	ChipTempAdc138C		= 0x2A,
	ChipTempAdc141C		= 0x2B,
	ChipTempAdc145C		= 0x2C,
	ChipTempAdc148C		= 0x2D,
	ChipTempAdc152C		= 0x2E,
	ChipTempAdc156C		= 0x2F,
} AlxPca9431_3Ah_ChipTempAdcMeas;
typedef union
{
	struct __attribute__((packed))
	{
		AlxPca9431_3Ah_ChipTempAdcMeas TDIE : 8;
	};
	uint8_t raw;
} AlxPca9431_RegVal_3Ah_ChipTempAdcMeas;

//------------------------------------------------------------------------------
// NTC_ADC Register (Address 3Bh)
//------------------------------------------------------------------------------
typedef union
{
	struct __attribute__((packed))
	{
		uint8_t NTC, : 8; // Voltage resolution is 2.64mV
	};
	uint8_t raw;
} AlxPca9431_RegVal_3Bh_NTCAdcH;

//------------------------------------------------------------------------------
// NTC_ADC Register (Address 3Ch)
//------------------------------------------------------------------------------
typedef union
{
	struct __attribute__((packed))
	{
		uint8_t unused_0_5	: 6; // bits 0...5 are unused
		uint8_t NTC	: 2; // Voltage resolution is 2.64mV
	};
	uint8_t raw;
} AlxPca9431_RegVal_3Ch_NTCAdcL;


//******************************************************************************
// Register Structures
//******************************************************************************
typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxPca9431_RegVal_00h_DeviceId val;
} AlxPca9431_Reg_00h_DeviceId;

typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxPca9431_RegVal_01h_SystemInt val;
} AlxPca9431_Reg_01h_SystemInt;

typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxPca9431_RegVal_02h_SystemIntMask val;
} AlxPca9431_Reg_02h_SystemIntMask;

typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxPca9431_RegVal_03h_VRectInt val;
} AlxPca9431_Reg_03h_VRectInt;

typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxPca9431_RegVal_04h_VRectIntMask val;
} AlxPca9431_Reg_04h_VRectIntMask;

typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxPca9431_RegVal_05h_VOutLdoInt val;
} AlxPca9431_Reg_05h_VOutLdoInt;

typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxPca9431_RegVal_06h_VOutLdoIntMask val;
} AlxPca9431_Reg_06h_VOutLdoIntMask;

typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxPca9431_RegVal_07h_VOutLdo val;
} AlxPca9431_Reg_07h_VOutLdo;

typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxPca9431_RegVal_08h_VRectThd val;
} AlxPca9431_Reg_08h_VRectThd;

typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxPca9431_RegVal_09h_VRectOvw val;
} AlxPca9431_Reg_09h_VRectOvw;

typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxPca9431_RegVal_0Ah_TempThd val;
} AlxPca9431_Reg_0Ah_TempThd;

typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxPca9431_RegVal_0Bh_WatchdogSoftStartEnMcu val;
} AlxPca9431_Reg_0Bh_WatchdogSoftStartEnMcu;

typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxPca9431_RegVal_0Ch_VaractorDac val;
} AlxPca9431_Reg_0Ch_VaractorDac;

typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxPca9431_RegVal_0Dh_AdcControl val;
} AlxPca9431_Reg_0Dh_AdcControl;

typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxPca9431_RegVal_0Eh_AdcSampleEn val;
} AlxPca9431_Reg_0Eh_AdcSampleEn;

typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxPca9431_RegVal_0Fh_VPwrLdoConf val;
} AlxPca9431_Reg_0Fh_VPwrLdoConf;

typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxPca9431_RegVal_10h_Rxir_Conf val;
} AlxPca9431_Reg_10h_Rxir_Conf;

typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxPca9431_RegVal_20h_OpcLdoSetLock val;
} AlxPca9431_Reg_20h_OpcLdoSetLock;

typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxPca9431_RegVal_21h_VOutLdoSetOCP val;
} AlxPca9431_Reg_21h_VOutLdoSetOCP;

typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxPca9431_RegVal_30h_VRectAdcH val;
} AlxPca9431_Reg_30h_VRectAdcH;

typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxPca9431_RegVal_31h_VRectAdcL val;
} AlxPca9431_Reg_31h_VRectAdcL;

typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxPca9431_RegVal_32h_VTuneAdcH val;
} AlxPca9431_Reg_32h_VTuneAdcH;

typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxPca9431_RegVal_33h_VTuneAdcL val;
} AlxPca9431_Reg_33h_VTuneAdcL;

typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxPca9431_RegVal_34h_VOutAdcH val;
} AlxPca9431_Reg_34h_VOutAdcH;

typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxPca9431_RegVal_35h_VOutAdcL val;
} AlxPca9431_Reg_35h_VOutAdcL;

typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxPca9431_RegVal_36h_IOutAdcH val;
} AlxPca9431_Reg_36h_IOutAdcH;

typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxPca9431_RegVal_37h_IOutAdcL val;
} AlxPca9431_Reg_37h_IOutAdcL;

typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxPca9431_RegVal_38h_IRectAdcH val;
} AlxPca9431_Reg_38h_IRectAdcH;

typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxPca9431_RegVal_39h_IRectAdcL val;
} AlxPca9431_Reg_39h_IRectAdcL;

typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxPca9431_RegVal_3Ah_ChipTempAdcMeas val;
} AlxPca9431_Reg_3Ah_ChipTempAdcMeas;

typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxPca9431_RegVal_3Bh_NTCAdcH val;
} AlxPca9431_Reg_3Bh_NTCAdcH;

typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxPca9431_RegVal_3Ch_NTCAdcL val;
} AlxPca9431_Reg_3Ch_NTCAdcL;


//******************************************************************************
// Main Register Structure
//******************************************************************************
typedef struct
{
	AlxPca9431_Reg_00h_DeviceId _00h_Device_ID;
	AlxPca9431_Reg_01h_SystemInt _01h_SYSTEM_INT;
	AlxPca9431_Reg_02h_SystemIntMask _02h_SYSTEM_INT_MASK;
	AlxPca9431_Reg_03h_VRectInt _03h_VRECT_INT;
	AlxPca9431_Reg_04h_VRectIntMask _04h_VRECT_INT_MASK;
	AlxPca9431_Reg_05h_VOutLdoInt _05h_VOUTLDO_INT;
	AlxPca9431_Reg_06h_VOutLdoIntMask _06h_VOUTLDO_INT_MASK;
	AlxPca9431_Reg_07h_VOutLdo _07h_VOUT_LDO;
	AlxPca9431_Reg_08h_VRectThd _08h_VRECT_THD;
	AlxPca9431_Reg_09h_VRectOvw _09h_VRECT_OVW;
	AlxPca9431_Reg_0Ah_TempThd _0Ah_TEMP_THD;
	AlxPca9431_Reg_0Bh_WatchdogSoftStartEnMcu _0Bh_WD_EN_RST;
	AlxPca9431_Reg_0Ch_VaractorDac _0Ch_Varactor_DAC;
	AlxPca9431_Reg_0Dh_AdcControl _0Dh_ADC_CONTROL;
	AlxPca9431_Reg_0Eh_AdcSampleEn _0Eh_Sample_EN;
	AlxPca9431_Reg_0Fh_VPwrLdoConf _0Fh_VPWR_CONFIG;
	AlxPca9431_Reg_10h_Rxir_Conf _10h_RXIR_CONFIG;
	AlxPca9431_Reg_20h_OpcLdoSetLock _20h_OCPSET_LOCK;
	AlxPca9431_Reg_21h_VOutLdoSetOCP _21h_VOUTLDO_OCP;
	AlxPca9431_Reg_30h_VRectAdcH _30h_VRECT_ADC_H;
	AlxPca9431_Reg_31h_VRectAdcL _31h_VRECT_ADC_L;
	AlxPca9431_Reg_32h_VTuneAdcH _32h_VTUNE_ADC_H;
	AlxPca9431_Reg_33h_VTuneAdcL _33h_VTUNE_ADC_L;
	AlxPca9431_Reg_34h_VOutAdcH _34h_VOUT_ADC_H;
	AlxPca9431_Reg_35h_VOutAdcL _35h_VOUT_ADC_L;
	AlxPca9431_Reg_36h_IOutAdcH _36h_IOUT_ADC_H;
	AlxPca9431_Reg_37h_IOutAdcL _37h_IOUT_ADC_L;
	AlxPca9431_Reg_38h_IRectAdcH _38h_IRECT_ADC_H;
	AlxPca9431_Reg_39h_IRectAdcL _39h_IRECT_ADC_L;
	AlxPca9431_Reg_3Ah_ChipTempAdcMeas _3Ah_TDIE_ADC;
	AlxPca9431_Reg_3Bh_NTCAdcH _3Bh_NTC_ADC_H;
	AlxPca9431_Reg_3Ch_NTCAdcL _3Ch_NTC_ADC_L;
} AlxPca9431_Reg;


//******************************************************************************
// Types
//******************************************************************************
typedef struct
{
	// Parameters - Const

	// Objects - External
	AlxI2c* i2c;
	AlxIoPin* do_SleepEn;	// enable / disable sleep modo for PCA9431
	AlxIoPin* di_Interrupt;	// Pca9431 interrupt

	// Parameters
	uint8_t i2cAddr;
	bool i2cCheckWithRead;
	uint8_t i2cNumOfTries;
	uint16_t i2cTimeout_ms;

	// Variables
	AlxPca9431_Reg reg;

	// Info
	bool isInit;
	bool wasCtorCalled;
} AlxPca9431;


//******************************************************************************
// Constructor
//******************************************************************************
void AlxPca9431_Ctor
(
	AlxPca9431* me,
	AlxI2c* i2c,
	uint8_t i2cAddr,
	AlxIoPin* do_SleepEn,
	AlxIoPin* di_Interrupt,
	bool i2cCheckWithRead,
	uint8_t i2cNumOfTries,
	uint16_t i2cTimeout_ms
);


//******************************************************************************
// Functions
//******************************************************************************
Alx_Status AlxPca9431_Init(AlxPca9431* me);
Alx_Status AlxPca9431_DeInit(AlxPca9431* me);
Alx_Status AlxPca9431_LdoVout_GetVoltage_V(AlxPca9431* me, float* voltage_V); // 10 bit ADC
Alx_Status AlxPca9431_LdoVout_GetCurrent_A(AlxPca9431* me, float* current_A); // 10 bit ADC
Alx_Status AlxPca9431_Rect_GetVoltage_V(AlxPca9431* me, float* voltage_V); // 10 bit ADC
Alx_Status AlxPca9431_Rect_GetCurrent_A(AlxPca9431* me, float* current_A); // 10 bit ADC
Alx_Status AlxPca9431_TempSens_GetTemp_degC(AlxPca9431* me, float* temp_degC); // On chip temperature (-43°C ... +156°C)
Alx_Status AlxPca9431_VTune_SetVoltage_V(AlxPca9431* me, float* voltage_V); // 0-3.3V 5 bit DAC - TODO
Alx_Status AlxPca9431_VTune_GetVoltage_V(AlxPca9431* me, float* voltage_V); // 10 bit ADC - TODO
Alx_Status AlxPca9431_Exit_EcoMode(AlxPca9431* me);
Alx_Status AlxPca9431_Reg_ReadAndClearInterrupt(AlxPca9431* me);


#ifdef __cplusplus
}
#endif

#endif // ALX_ADAU1961_H



























//Alx_Status AlxPca9431_LdoVout_SetVoltage_V(AlxPca9431* me, AlxPca9431_0x07_VOutLdo VoutLdoxV); // 3,3V or 5V => write:  VoutLdo3V3 or. VoutLdo5V

//Alx_Status AlxPca9431_LdoVPwr_SetVoltage_V(AlxPca9431* me, AlxPca9431_0x0F_VPwrOutSet VPwrOutxVx); // 1.8V, 2.5V, 2.8V, 3.3V  => write:  VPwrOut1V8 or. VPwrOut2V5 or. VPwrOut2V8 or. VPwrOut3V3
//Alx_Status AlxPca9431_Rect_Uvlo_SetVoltage_V(AlxPca9431* me, AlxPca9431_0x08_VRectThd VRectThd3Vx); // 3V, 3.1V, 3.2V, 3.3V  => write: VRectThd3V or. VRectThd3V1 or. VRectThd3V2 or. VRectThd3V3
//Alx_Status AlxPca9431_Rect_Ovw_SetVoltage_V(AlxPca9431* me, AlxPca9431_0x09_VRectOvw VRectThd7Vx); // 7V, 7.25V, 7.5V, 7.75V  => write: VRectThd7V or. VRectThd7V25 or. VRectThd7V5 or. VRectThd7V75

//Alx_Status AlxPca9431_TempSens_SetWarning_C(AlxPca9431* me, AlxPca9431_0x0A_TempWarnThd TempWarnThdx0C); // 80°C, 90°C ... 110°C =>  write: TempWarnThd80C or. ... or. TempWarnThd110C
//Alx_Status AlxPca9431_TempSens_SetShutdown_C(AlxPca9431* me, AlxPca9431_0x0A_TempShutdownThd TempShutdownThd1xxC); // 120°C, 125°C ... 150°C =>  write: TempShutdownThd120C or. ... or. TempShutdownThd150C


//Alx_Status AlxPca9431_Rxir_SetDuration_s(AlxPca9431* me, AlxPca9431_0x10_RxirPullDownDuration RxirPullDownxmsx); // 0.2s, 0.4s, ... , 1.6s =>  write: RxirPullDown0ms2 or. ... or. RxirPullDown1ms6
//Alx_Status AlxPca9431_Rxir_SetResistance_ohm(AlxPca9431* me, AlxPca9431_0x10_RxirPullDownR RxirPullDownx0R); // 20ohm, 80ohm, ... , 440ohm =>  write: RxirPullDown20R or. ... or. RxirPullDown440R	
//Alx_Status AlxPca9431_Ntc_GetTemp_degC(AlxPca9431* me, float* temp_degC); // external NTC voltage / battery temperature measurement - TODO


//bool AlxPca9431_IsSleep(AlxPca9431* me);
//Alx_Status AlxPca9431_Adc_GetVoltage_VRECT(AlxPca9431* me, float* voltage_V);

// Adc
// VRECT
// VTUNE
// VOUT
// IOUT
// IRECT
// NTC


