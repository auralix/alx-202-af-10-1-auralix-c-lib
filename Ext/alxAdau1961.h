/**
  ******************************************************************************
  * @file alxAdau1961.h
  * @brief Auralix C Library - ALX Audio Codec ADAU1961 Module
  * @version $LastChangedRevision: 4584 $
  * @date $LastChangedDate: 2021-04-03 03:11:01 +0200 (Sat, 03 Apr 2021) $
  ******************************************************************************
  */

#ifndef ALX_ADAU1961_H
#define ALX_ADAU1961_H

#ifdef __cplusplus
extern "C" {
#endif

//******************************************************************************
// Includes
//******************************************************************************
#include "alxGlobal.h"
#include "alxTrace.h"
#include "alxAssert.h"
#include "alxTimSw.h"
#include "alxI2c.h"
#include "alxI2s.h"


//******************************************************************************
// Preprocessor
//******************************************************************************
#define ALX_ADAU1961_FILE "alxAdau1961"

// Assert //
#if defined(_ALX_ADAU1961_ASSERT_BKPT) || defined(_ALX_ASSERT_BKPT_ALL)
	#define ALX_ADAU1961_ASSERT(expr) ALX_ASSERT_BKPT(ALX_ADAU1961_FILE, expr)
#elif defined(_ALX_ADAU1961_ASSERT_TRACE) || defined(_ALX_ASSERT_TRACE_ALL)
	#define ALX_ADAU1961_ASSERT(expr) ALX_ASSERT_TRACE(ALX_ADAU1961_FILE, expr)
#elif defined(_ALX_ADAU1961_ASSERT_RST) || defined(_ALX_ASSERT_RST_ALL)
	#define ALX_ADAU1961_ASSERT(expr) ALX_ASSERT_RST(ALX_ADAU1961_FILE, expr)
#else
	#define ALX_ADAU1961_ASSERT(expr) do{} while (false)
#endif

// Trace //
#if defined(_ALX_ADAU1961_TRACE) || defined(_ALX_TRACE_ALL)
	#define ALX_ADAU1961_TRACE(...) ALX_TRACE_STD(ALX_ADAU1961_FILE, __VA_ARGS__)
#else
	#define ALX_ADAU1961_TRACE(...) do{} while (false)
#endif


//******************************************************************************
// Register Values Unions
//******************************************************************************
// General Enums //
typedef enum
{
	MixerDisabled = 0b0,
	MixerEnabled  = 0b1
} AlxAdau1961_General_MixerEnable;
typedef enum
{
	GainMixer1Mixer2_Mute  = 0b000,
	GainMixer1Mixer2_n12dB = 0b001,
	GainMixer1Mixer2_n9dB  = 0b010,
	GainMixer1Mixer2_n6dB  = 0b011,
	GainMixer1Mixer2_n3dB  = 0b100,
	GainMixer1Mixer2_0dB   = 0b101,
	GainMixer1Mixer2_3dB   = 0b110,
	GainMixer1Mixer2_6dB   = 0b111
} AlxAdau1961_General_GainMixer1Mixer2;
typedef enum
{
	DiffPGAInputGain_Mute = 0b00,
	DiffPGAInputGain_0dB  = 0b01,
	DiffPGAInputGain_20dB = 0b10
} AlxAdau1961_General_DiffPGAInputGain;
typedef enum
{
	GainMixer3Mixer4_Mute  = 0b0000,
	GainMixer3Mixer4_n15dB = 0b0001,
	GainMixer3Mixer4_n12dB = 0b0010,
	GainMixer3Mixer4_n9dB  = 0b0011,
	GainMixer3Mixer4_n6dB  = 0b0100,
	GainMixer3Mixer4_n3dB  = 0b0101,
	GainMixer3Mixer4_0dB   = 0b0110,
	GainMixer3Mixer4_3dB   = 0b0111,
	GainMixer3Mixer4_6dB   = 0b1000
} AlxAdau1961_General_GainMixer3Mixer4;
typedef enum
{
	OutputMode_LineOutput = 0b0,
	OutputMode_HPOutput   = 0b1
} AlxAdau1961_General_OutputMode;
typedef enum
{
	OutputMute_Mute   = 0b0,
	OutputMute_UnMute = 0b1
} AlxAdau1961_General_OutputMute;
typedef enum
{
	PullConfig_PullUp   = 0b00,
//	PullConfig_Reserved = 0b01,
	PullConfig_None     = 0b10,
	PullConfig_PullDown = 0b11
} AlxAdau1961_General_PullConfig;

//------------------------------------------------------------------------------
// R0: Clock Control
//------------------------------------------------------------------------------
typedef enum
{
	CoreClk_Disabled = 0,
	CoreClk_Enabled = 1
} AlxAdau1961_R0_CoreClkEnable;
typedef enum
{
	InputClkFreq_256  = 0b00,
	InputClkFreq_512  = 0b01,
	InputClkFreq_768  = 0b10,
	InputClkFreq_1024 = 0b11
} AlxAdau1961_R0_InputClkFreq;
typedef enum
{
	ClkSourceSelect_MCLKpin = 0b0,
	ClkSourceSelect_PLLClk  = 0b1
} AlxAdau1961_R0_ClkSourceSelect;
typedef union
{
	struct __attribute__((packed))
	{
		AlxAdau1961_R0_CoreClkEnable COREN : 1;
		AlxAdau1961_R0_InputClkFreq INFREQ : 2;
		AlxAdau1961_R0_ClkSourceSelect CLKSRC : 1;
		uint8_t unused_4_7: 4; // bits 4..7 are unused
	};
	uint8_t raw;
} AlxAdau1961_RegVal_R0_ClockControl;

//------------------------------------------------------------------------------
// R1: PLL Control
//------------------------------------------------------------------------------
typedef enum
{
	PllControl_X_1 = 0b00,
	PllControl_X_2 = 0b01,
	PllControl_X_3 = 0b10,
	PllControl_X_4 = 0b11
} AlxAdau1961_R1_PllControl_X;
typedef enum
{
	PllControl_R_2 = 0b0010,
	PllControl_R_3 = 0b0011,
	PllControl_R_4 = 0b0100,
	PllControl_R_5 = 0b0101,
	PllControl_R_6 = 0b0110,
	PllControl_R_7 = 0b0111,
	PllControl_R_8 = 0b1000
} AlxAdau1961_R1_PllControl_R;
typedef enum
{
	PllEnable_Disabled = 0b0,
	PllEnable_Enabled  = 0b1
} AlxAdau1961_R1_PllEnable;
typedef union
{
	struct __attribute__((packed))
	{
		uint8_t M_MSB;
		uint8_t M_LSB;
		uint8_t N_MSB;
		uint8_t N_LSB;
		uint8_t Type: 1;
		AlxAdau1961_R1_PllControl_X X : 2;
		AlxAdau1961_R1_PllControl_R R : 4;
		uint8_t unused_4_7_7: 1; // byte 4, bit 7 is unused
		AlxAdau1961_R1_PllEnable PLLEN : 1;
		uint8_t Lock: 1;
		uint8_t unused_5_2_7: 6; // byte 5, bits 2..7 are unused
	};
	uint8_t raw[6];
} AlxAdau1961_RegVal_R1_PllControl;

//------------------------------------------------------------------------------
// R2: Digital Microphone/Jack Detection Control
//------------------------------------------------------------------------------
typedef enum
{
	JackDetectPolarity_High = 0b0,
	JackDetectPolarity_Low  = 0b1
} AlxAdau1961_R2_JackDetectPolarity;
typedef enum
{
	JackDetectMicInFunc_Off          = 0b00,
	JackDetectMicInFunc_On           = 0b01,
	JackDetectMicInFunc_DigiMicInput = 0b10,
} AlxAdau1961_R2_JackDetectMicInFunc;
typedef enum
{
	JackDebounceTime_5ms  = 0b00,
	JackDebounceTime_10ms = 0b01,
	JackDebounceTime_20ms = 0b10,
	JackDebounceTime_40ms = 0b11
} AlxAdau1961_R2_JackDebounceTime;
typedef union
{
	struct __attribute__((packed))
	{
		AlxAdau1961_R2_JackDetectPolarity JDPOL : 1;
		uint8_t unused_1_3: 3; // bits 1..3 are unused
		AlxAdau1961_R2_JackDetectMicInFunc JDFUNC : 2;
		AlxAdau1961_R2_JackDebounceTime JDDB : 2;
	};
	uint8_t raw;
} AlxAdau1961_RegVal_R2_DigMicJackDetect;

//AlxAdau1961_RegVal_R3_Reserved

//------------------------------------------------------------------------------
// R4: Record Mixer Left (Mixer 1) Control 0
//------------------------------------------------------------------------------
typedef union
{
	struct __attribute__((packed))
	{
		AlxAdau1961_General_MixerEnable MX1EN : 1;
		AlxAdau1961_General_GainMixer1Mixer2 LINNG : 3;
		AlxAdau1961_General_GainMixer1Mixer2 LINPG : 3;
		uint8_t unused_7: 1; // bit 7 is unused
	};
	uint8_t raw;
} AlxAdau1961_RegVal_R4_RecMixerLeft0;

//------------------------------------------------------------------------------
// R5: Record Mixer Left(Mixer 1) Control 1
//------------------------------------------------------------------------------
typedef union
{
	struct __attribute__((packed))
	{
		AlxAdau1961_General_GainMixer1Mixer2 MX1AUXG : 3;
		AlxAdau1961_General_DiffPGAInputGain LDBOOST : 2;
		uint8_t unused_5_7: 3; // bits 5..7 are unused
	};
	uint8_t raw;
} AlxAdau1961_RegVal_R5_RecMixerLeft1;

//------------------------------------------------------------------------------
// R6: Record Mixer Right (Mixer 2) Control 0
//------------------------------------------------------------------------------
typedef union
{
	struct __attribute__((packed))
	{
		AlxAdau1961_General_MixerEnable MX2EN : 1;
		AlxAdau1961_General_GainMixer1Mixer2 RINNG : 3;
		AlxAdau1961_General_GainMixer1Mixer2 RINPG : 3;
		uint8_t unused_7: 1; // bit 7 is unused
	};
	uint8_t raw;
} AlxAdau1961_RegVal_R6_RecMixerRight0;

//------------------------------------------------------------------------------
// R7: Record Mixer Right (Mixer 2) Control 1
//------------------------------------------------------------------------------
typedef union
{
	struct __attribute__((packed))
	{
		AlxAdau1961_General_GainMixer1Mixer2 MX2AUXG : 3;
		AlxAdau1961_General_DiffPGAInputGain RDBOOST : 2;
		uint8_t unused_5_7: 3; // bits 5..7 are unused
	};
	uint8_t raw;
} AlxAdau1961_RegVal_R7_RecMixerRight1;

//------------------------------------------------------------------------------
// R8: Left Differential Input Volume Control
//------------------------------------------------------------------------------
typedef enum
{
	DiffPGAEnable_Disabled = 0b0,
	DiffPGAEnable_Enabled  = 0b1
} AlxAdau1961_R8_R9_DiffPGAEnable;
typedef enum
{
	DiffInputMuteControl_Mute   = 0b0,
	DiffInputMuteControl_UnMute = 0b1
} AlxAdau1961_R8_R9_DiffInputMuteControl;
typedef union
{
	struct __attribute__((packed))
	{
		AlxAdau1961_R8_R9_DiffPGAEnable LDEN : 1;
		AlxAdau1961_R8_R9_DiffInputMuteControl LDMUTE : 1;
		uint8_t LDVOL: 6;		// -12dB to 35.25dB with 0.75dB step
	};
	uint8_t raw;
} AlxAdau1961_RegVal_R8_LeftDiffInputVol;
typedef union
{
	struct __attribute__((packed))
	{
		AlxAdau1961_R8_R9_DiffPGAEnable RDEN : 1;
		AlxAdau1961_R8_R9_DiffInputMuteControl RDMUTE : 1;
		uint8_t RDVOL: 6;		// -12dB to 35.25dB with 0.75dB step
	};
	uint8_t raw;
} AlxAdau1961_RegVal_R9_RightDiffInputVol;

//------------------------------------------------------------------------------
//R10: Record Microphone Bias Control
//------------------------------------------------------------------------------
typedef enum
{
	MicBiasOutputEnable_Disabled = 0b0,
	MicBiasOutputEnable_Enabled  = 0b1
} AlxAdau1961_R10_MicBiasOutputEnable;
typedef enum
{
	MicVolBias_AVDD_0V90 = 0b0,
	MicVolBias_AVDD_0V65 = 0b1
} AlxAdau1961_R10_MicVolBias;
typedef enum
{
	MicPerformanceOperation_Normal          = 0b0,
	MicPerformanceOperation_HighPerformance = 0b1
} AlxAdau1961_R10_MicPerformanceOperation;
typedef union
{
	struct __attribute__((packed))
	{
		AlxAdau1961_R10_MicBiasOutputEnable MBIEN : 1;
		uint8_t unused_1: 1; // bit 1 is unused
		AlxAdau1961_R10_MicVolBias MBI : 1;
		AlxAdau1961_R10_MicPerformanceOperation MPERF : 1;
		uint8_t unused_4_7: 4;  // bits 4..7 are unused
	};
	uint8_t raw;
} AlxAdau1961_RegVal_R10_RecordMicBias;

//------------------------------------------------------------------------------
// R11: ALC Control 0
//------------------------------------------------------------------------------
typedef enum
{
	ALCSelect_Off       = 0b000,
	ALCSelect_RightOnly = 0b001,
	ALCSelect_LeftOnly  = 0b010,
	ALCSelect_Stereo    = 0b011
} AlxAdau1961_R11_ALCSelect;
typedef enum
{
	MaxALCGain_n12db = 0b000,
	MaxALCGain_n6db  = 0b001,
	MaxALCGain_0db   = 0b010,
	MaxALCGain_6db   = 0b011,
	MaxALCGain_12db  = 0b100,
	MaxALCGain_18db  = 0b101,
	MaxALCGain_24db  = 0b110,
	MaxALCGain_30db  = 0b111
} AlxAdau1961_R11_MaxALCGain;
typedef enum
{
	PGAVolSlewTimeALC_24ms = 0b00,
	PGAVolSlewTimeALC_48ms = 0b01,
	PGAVolSlewTimeALC_96ms = 0b10,
	PGAVolSlewTimeALC_Off  = 0b11
} AlxAdau1961_R11_PGAVolSlewTimeALC;
typedef union
{
	struct __attribute__((packed))
	{
		AlxAdau1961_R11_ALCSelect ALCSEL : 3;
		AlxAdau1961_R11_MaxALCGain ALCMAX : 3;
		AlxAdau1961_R11_PGAVolSlewTimeALC PGASLEW : 2;
	};
	uint8_t raw;
} AlxAdau1961_RegVal_R11_Alc0;

//------------------------------------------------------------------------------
// R12: ALC Control 1
//------------------------------------------------------------------------------
typedef enum
{
	ALCTarget_n28dB5 = 0b0000,
	ALCTarget_n27dB  = 0b0001,
	ALCTarget_n25dB5 = 0b0010,
	ALCTarget_n24dB  = 0b0011,
	ALCTarget_n22dB5 = 0b0100,
	ALCTarget_n21dB  = 0b0101,
	ALCTarget_n19dB5 = 0b0110,
	ALCTarget_n18dB  = 0b0111,
	ALCTarget_n16dB5 = 0b1000,
	ALCTarget_n15dB  = 0b1001,
	ALCTarget_n13dB5 = 0b1010,
	ALCTarget_n12dB  = 0b1011,
	ALCTarget_n10dB5 = 0b1100,
	ALCTarget_n9dB   = 0b1101,
	ALCTarget_n7dB5  = 0b1110,
	ALCTarget_n6dB   = 0b1111
} AlxAdau1961_R12_ALCTarget;
typedef enum
{
	LCHoldTime_2ms67    = 0b0000,
	LCHoldTime_5ms34    = 0b0001,
	LCHoldTime_10ms68   = 0b0010,
	LCHoldTime_21ms36   = 0b0011,
	LCHoldTime_42ms72   = 0b0100,
	LCHoldTime_85ms44   = 0b0101,
	LCHoldTime_170ms88  = 0b0110,
	LCHoldTime_341ms76  = 0b0111,
	LCHoldTime_683ms52  = 0b1000,
	LCHoldTime_1sec367  = 0b1001,
	LCHoldTime_2sec7341 = 0b1010,
	LCHoldTime_5sec4682 = 0b1011,
	LCHoldTime_10sec936 = 0b1100,
	LCHoldTime_21sec873 = 0b1101,
	LCHoldTime_43sec745 = 0b1110,
	LCHoldTime_87sec491 = 0b1111
} AlxAdau1961_R12_ALCHoldTime;
typedef union
{
	struct __attribute__((packed))
	{
		AlxAdau1961_R12_ALCTarget ALCTARG : 4;
		AlxAdau1961_R12_ALCHoldTime ALCHOLD : 4;
	};
	uint8_t raw;
} AlxAdau1961_RegVal_R12_Alc1;

//------------------------------------------------------------------------------
// R13: ALC Control 2
//------------------------------------------------------------------------------
typedef enum
{
	ALCDecayTime_24ms     = 0b0000,
	ALCDecayTime_48ms     = 0b0001,
	ALCDecayTime_96ms     = 0b0010,
	ALCDecayTime_192ms    = 0b0011,
	ALCDecayTime_384ms    = 0b0100,
	ALCDecayTime_768ms    = 0b0101,
	ALCDecayTime_1sec54   = 0b0110,
	ALCDecayTime_3sec07   = 0b0111,
	ALCDecayTime_6sec14   = 0b1000,
	ALCDecayTime_12sec29  = 0b1001,
	ALCDecayTime_24sec58  = 0b1010,
	ALCDecayTime_49sec15  = 0b1011,
	ALCDecayTime_98sec30  = 0b1100,
	ALCDecayTime_196sec61 = 0b1101,
	ALCDecayTime_393sec22 = 0b1110,
	ALCDecayTime_786sec43 = 0b1111
} AlxAdau1961_R13_ALCDecayTime;
typedef enum
{
	ALCAttackTime_6ms      = 0b0000,
	ALCAttackTime_12ms     = 0b0001,
	ALCAttackTime_24ms     = 0b0010,
	ALCAttackTime_48ms     = 0b0011,
	ALCAttackTime_96ms     = 0b0100,
	ALCAttackTime_192ms    = 0b0101,
	ALCAttackTime_384ms    = 0b0110,
	ALCAttackTime_768ms    = 0b0111,
	ALCAttackTime_1sec54   = 0b1000,
	ALCAttackTime_3sec07   = 0b1001,
	ALCAttackTime_6sec14   = 0b1010,
	ALCAttackTime_12sec29  = 0b1011,
	ALCAttackTime_24sec58  = 0b1100,
	ALCAttackTime_49sec15  = 0b1101,
	ALCAttackTime_98sec30  = 0b1110,
	ALCAttackTime_196sec61 = 0b1111
} AlxAdau1961_R13_ALCAttackTime;
typedef union
{
	struct __attribute__((packed))
	{
		AlxAdau1961_R13_ALCDecayTime ALCDEC : 4;
		AlxAdau1961_R13_ALCAttackTime ALCATCK : 4;
	};
	uint8_t raw;
} AlxAdau1961_RegVal_R13_Alc2;

//------------------------------------------------------------------------------
// R14: ALC Control 3
//------------------------------------------------------------------------------
typedef enum
{
	NoiseGateEnable_Disabled = 0b0,
	NoiseGateEnable_Enabled  = 0b1
} AlxAdau1961_R14_NoiseGateEnable;
typedef enum
{
	NoiseGateType_HoldPGAConstant = 0b00,
	NoiseGateType_MuteADCOutput   = 0b01,
	NoiseGateType_FadeToPGAMinVal = 0b10,
	NoiseGateType_FadeToMute      = 0b11
} AlxAdau1961_R14_NoiseGateType;
typedef union
{
	struct __attribute__((packed))
	{
		uint8_t NGTHR: 5;		// -76.5dB to -30dB with 1.5dB step
		AlxAdau1961_R14_NoiseGateEnable NGEN : 1;
		AlxAdau1961_R14_NoiseGateType NGTYP : 2;
	};
	uint8_t raw;
} AlxAdau1961_RegVal_R14_Alc3;

//------------------------------------------------------------------------------
// R15: Serial Port Control 0
//------------------------------------------------------------------------------
typedef enum
{
	PortMode_Slave  = 0b0,
	PortMode_Master = 0b1
} AlxAdau1961_R15_PortMode;
typedef enum
{
	ChPerFrame_Stereo = 0b00,
	ChPerFrame_TDM4   = 0b01
} AlxAdau1961_R15_ChPerFrame;
typedef enum
{
	LRCLKPolarity_FallingEdge = 0b0,
	LRCLKPolarity_RisingEdge  = 0b1
} AlxAdau1961_R15_LRCLKPolarity;
typedef enum
{
	BCLKPolarity_FallingEdge = 0b0,
	BCLKPolarity_RisingEdge  = 0b1
} AlxAdau1961_R15_BCLKPolarity;
typedef enum
{
	LRCLKMode_50pctDutyCycle = 0b0,
	LRCLKMode_PulseMode      = 0b1
} AlxAdau1961_R15_LRCLKMode;
typedef enum
{
	DitherEnable_Disabled = 0b0,
	DitherEnable_Enabled  = 0b1
} AlxAdau1961_R15_DitherEnable;
typedef union
{
	struct __attribute__((packed))
	{
		AlxAdau1961_R15_PortMode MS : 1;
		AlxAdau1961_R15_ChPerFrame CHPF : 2;
		AlxAdau1961_R15_LRCLKPolarity LRPOL : 1;
		AlxAdau1961_R15_BCLKPolarity BPOL : 1;
		AlxAdau1961_R15_LRCLKMode LRMOD : 1;
		uint8_t unused_6: 1; // bit 6 is unused
		AlxAdau1961_R15_DitherEnable DITHEN : 1;
	};
	uint8_t raw;
} AlxAdau1961_RegVal_R15_SerialPort0;

//------------------------------------------------------------------------------
// R16: Serial Port Control 1
//------------------------------------------------------------------------------
typedef enum
{
	DataDelay_1  = 0b00,
	DataDelay_0  = 0b01,
	DataDelay_8  = 0b10,
	DataDelay_16 = 0b11
} AlxAdau1961_R16_DataDelay;
typedef enum
{
	MSBPosition_MSBFirst = 0b0,
	MSBPosition_LSBFirst = 0b1
} AlxAdau1961_R16_MSBPosition;
typedef enum
{
	DACChPosition_LeftFirst  = 0b0,
	DACChPosition_RightFirst = 0b1
} AlxAdau1961_R16_DACChPosition;
typedef enum
{
	ADCChPosition_LeftFirst  = 0b0,
	ADCChPosition_RightFirst = 0b1
} AlxAdau1961_R16_ADCChPosition;
typedef enum
{
	NumBitClkCycles_64  = 0b000,
	NumBitClkCycles_32  = 0b001,
	NumBitClkCycles_48  = 0b010,
	NumBitClkCycles_128 = 0b011,
	NumBitClkCycles_256 = 0b100
} AlxAdau1961_R16_NumBitClkCycles;
typedef union
{
	struct __attribute__((packed))
	{
		AlxAdau1961_R16_DataDelay LRDEL : 2;
		AlxAdau1961_R16_MSBPosition MSBP : 1;
		AlxAdau1961_R16_DACChPosition DATDM : 1;
		AlxAdau1961_R16_ADCChPosition ADTDM : 1;
		AlxAdau1961_R16_NumBitClkCycles BPF : 3;
	};
	uint8_t raw;
} AlxAdau1961_RegVal_R16_SerialPort1;

//------------------------------------------------------------------------------
// R17: Converter Control 0
//------------------------------------------------------------------------------
typedef enum
{
	ConvSamplRate_fs         = 0b000,
	ConvSamplRate_fs_div6    = 0b001,
	ConvSamplRate_fs_div4    = 0b010,
	ConvSamplRate_fs_div3    = 0b011,
	ConvSamplRate_fs_div2    = 0b100,
	ConvSamplRate_fs_div1pt5 = 0b101,
	ConvSamplRate_fs_div0pt5 = 0b110
} AlxAdau1961_R17_ConvSamplRate;
typedef enum
{
	ADCOversamplRatio_128x = 0b0,
	ADCOversamplRatio_64x  = 0b1
} AlxAdau1961_R17_ADCOversamplRatio;
typedef enum
{
	DACOversamplRatio_128x = 0b0,
	DACOversamplRatio_64x  = 0b1
} AlxAdau1961_R17_DACOversamplRatio;
typedef enum
{
	DACSerialDataSelection_FirstPair  = 0b00,
	DACSerialDataSelection_SecondPair = 0b01,
	DACSerialDataSelection_ThirdPair  = 0b10,
	DACSerialDataSelection_FourthPair = 0b11
} AlxAdau1961_R17_DACSerialDataSelection;
typedef union
{
	struct __attribute__((packed))
	{
		AlxAdau1961_R17_ConvSamplRate CONVSR : 3;
		AlxAdau1961_R17_ADCOversamplRatio ADOSR : 1;
		AlxAdau1961_R17_DACOversamplRatio DAOSR : 1;
		AlxAdau1961_R17_DACSerialDataSelection DAPAIR : 2;
		uint8_t unused_7 : 1; // bit 7 is unused
	};
	uint8_t raw;
} AlxAdau1961_RegVal_R17_Converter0;

//------------------------------------------------------------------------------
// R18: Converter Control 1
//------------------------------------------------------------------------------
typedef enum
{
	ADCSerialDataSelect_FirstPair  = 0b00,
	ADCSerialDataSelect_SecondPair = 0b01,
	ADCSerialDataSelect_ThirdPair  = 0b10,
	ADCSerialDataSelect_FourthPair = 0b11
} AlxAdau1961_R18_ADCSerialDataSelect;
typedef union
{
	struct __attribute__((packed))
	{
		AlxAdau1961_R18_ADCSerialDataSelect ADPAIR : 2;
		uint8_t unused_2_7: 6; // bits 4..7 are unused
	};
	uint8_t raw;
} AlxAdau1961_RegVal_R18_Converter1;

//------------------------------------------------------------------------------
// R19: ADC Control
//------------------------------------------------------------------------------
typedef enum
{
	ADCEnable_BothOff = 0b00,
	ADCEnable_LeftOn  = 0b01,
	ADCEnable_RightOn = 0b10,
	ADCEnable_BothOn  = 0b11
} AlxAdau1961_R19_ADCEnable;
typedef enum
	{
	DigiMicInputSelect_InOff = 0b0,
	DigiMicInputSelect_InOn  = 0b1
} AlxAdau1961_R19_DigiMicInputSelect;
typedef enum
{
	DigiMicChSwap_Normal   = 0b0,
	DigiMicChSwap_SwapLRCh = 0b1
} AlxAdau1961_R19_DigiMicChSwap;
typedef enum
{
	DigiMicDataPolarity_Inverted = 0b0,
	DigiMicDataPolarity_Normal   = 0b1
} AlxAdau1961_R19_DigiMicDataPolarity;
typedef enum
{
	HPF_Off = 0b0,
	HPF_On  = 0b1
} AlxAdau1961_R19_HPF;
typedef enum
{
	InvertInputPolarity_Normal   = 0b0,
	InvertInputPolarity_Inverted = 0b1
} AlxAdau1961_R19_InvertInputPolarity;
typedef union
{
	struct __attribute__((packed))
	{
		AlxAdau1961_R19_ADCEnable ADCEN : 2;
		AlxAdau1961_R19_DigiMicInputSelect INSEL : 1;
		AlxAdau1961_R19_DigiMicChSwap DMSW : 1;
		AlxAdau1961_R19_DigiMicDataPolarity DMPOL : 1;
		AlxAdau1961_R19_HPF HPF : 1;
		AlxAdau1961_R19_InvertInputPolarity ADCPOL : 1;
		uint8_t unused_7 : 1; // bit 7 is unused
	};
	uint8_t raw;
} AlxAdau1961_RegVal_R19_AdcControl;

//------------------------------------------------------------------------------
// R20: Left Input Digital Volume
//------------------------------------------------------------------------------
typedef union
{
	struct __attribute__((packed))
	{
		uint8_t LADVOL;			// 0dB to -95.625dB with 0.375dB step
	};
	uint8_t raw;
} AlxAdau1961_RegVal_R20_LeftDigitalVol;

//------------------------------------------------------------------------------
// R21: Right Input Digital Volume
//------------------------------------------------------------------------------
typedef union
{
	struct __attribute__((packed))
	{
		uint8_t RADVOL;			// 0dB to -95.625dB with 0.375dB step
	};
	uint8_t raw;
} AlxAdau1961_RegVal_R21_RightDigitalVol;

//------------------------------------------------------------------------------
// R22: Playback Mixer Left (Mixer 3) Control 0
//------------------------------------------------------------------------------
typedef enum
{
	MixInputMute_Muted   = 0b0,
	MixInputMute_UnMuted = 0b1
} AlxAdau1961_R22_R24_MixInputMute;
typedef union
{
	struct __attribute__((packed))
	{
		AlxAdau1961_General_MixerEnable MX3EN : 1;
		AlxAdau1961_General_GainMixer3Mixer4 MX3AUXG : 4;
		AlxAdau1961_R22_R24_MixInputMute MX3LM : 1;
		AlxAdau1961_R22_R24_MixInputMute MX3RM : 1;
		uint8_t unused_7 : 1; // bit 7 is unused
	};
	uint8_t raw;
} AlxAdau1961_RegVal_R22_PlayMixerLeft0;

//------------------------------------------------------------------------------
// R23: Playback Mixer Left (Mixer 3) Control 1
//------------------------------------------------------------------------------
typedef union
{
	struct __attribute__((packed))
	{
		AlxAdau1961_General_GainMixer3Mixer4 MX3G1 : 4;
		AlxAdau1961_General_GainMixer3Mixer4 MX3G2 : 4;
	};
	uint8_t raw;
} AlxAdau1961_RegVal_R23_PlayMixerLeft1;

//------------------------------------------------------------------------------
// R24: Playback Mixer Right (Mixer 4) Control 0
//------------------------------------------------------------------------------
typedef union
{
	struct __attribute__((packed))
	{
		AlxAdau1961_General_MixerEnable MX4EN : 1;
		AlxAdau1961_General_GainMixer3Mixer4 MX4AUXG : 4;
		AlxAdau1961_R22_R24_MixInputMute MX4LM : 1;
		AlxAdau1961_R22_R24_MixInputMute MX4RM : 1;
		uint8_t unused_7: 1; // bit 7 is unused
	};
	uint8_t raw;
} AlxAdau1961_RegVal_R24_PlayMixerRight0;

//------------------------------------------------------------------------------
// R25: Playback Mixer Right (Mixer 4) Control 1
//------------------------------------------------------------------------------
typedef union
{
	struct __attribute__((packed))
	{
		AlxAdau1961_General_GainMixer3Mixer4 MX4G1 : 4;
		AlxAdau1961_General_GainMixer3Mixer4 MX4G2 : 4;
	};
	uint8_t raw;
} AlxAdau1961_RegVal_R25_PlayMixerRight1;

//------------------------------------------------------------------------------
// R26: Playback L/R Mixer Left (Mixer 5) Line Output Control
//------------------------------------------------------------------------------
typedef enum
{
	MixerInputGainBoost_Mute = 0b00,
	MixerInputGainBoost_0dB  = 0b01,
	MixerInputGainBoost_6dB  = 0b10
} AlxAdau1961_R26_R27_MixerInputGainBoost;
typedef union
{
	struct __attribute__((packed))
	{
		AlxAdau1961_General_MixerEnable MX5EN : 1;
		AlxAdau1961_R26_R27_MixerInputGainBoost MX5G3 : 2;
		AlxAdau1961_R26_R27_MixerInputGainBoost MX5G4 : 2;
		uint8_t unused_5_7: 3; // bits 5..7 are unused
	};
	uint8_t raw;
} AlxAdau1961_RegVal_R26_PlayLrMixerLeft;

//------------------------------------------------------------------------------
// R27: Playback L/R Mixer Right (Mixer 6) Line Output Control
//------------------------------------------------------------------------------
typedef union
{
	struct __attribute__((packed))
	{
		AlxAdau1961_General_MixerEnable MX6EN : 1;
		AlxAdau1961_R26_R27_MixerInputGainBoost MX6G3 : 2;
		AlxAdau1961_R26_R27_MixerInputGainBoost MX6G4 : 2;
		uint8_t unused_5_7: 3; // bits 5..7 are unused
	};
	uint8_t raw;
} AlxAdau1961_RegVal_R27_PlayLrMixerRight;

//------------------------------------------------------------------------------
// R28: Playback L/R Mixer Mono Output (Mixer 7) Control
//------------------------------------------------------------------------------
typedef enum
{
	PlaybackMixer7_CommonMode = 0b00,
	PlaybackMixer7_0dB        = 0b01,
	PlaybackMixer7_6dB        = 0b10
} AlxAdau1961_R26_R27_PlaybackMixer7;
typedef union
{
	struct __attribute__((packed))
	{
		AlxAdau1961_General_MixerEnable MX7EN : 1;
		AlxAdau1961_R26_R27_PlaybackMixer7 MX7 : 2;
		uint8_t unused_3_7 : 5; // bits 3..7 are unused
	};
	uint8_t raw;
} AlxAdau1961_RegVal_R28_PlayLrMixerMono;

//------------------------------------------------------------------------------
// R29: Playback Headphone Left Volume Control
//------------------------------------------------------------------------------
typedef enum
{
	HPEnable_Disabled = 0b0,
	HPEnable_Enabled  = 0b1
} AlxAdau1961_R29_HPEnable;
typedef enum
{
	HPMute_Mute   = 0b0,
	HPMute_UnMute = 0b1
} AlxAdau1961_R29_R30_HPMute;
typedef union
{
	struct __attribute__((packed))
	{
		AlxAdau1961_R29_HPEnable HPEN : 1;
		AlxAdau1961_R29_R30_HPMute LHPM : 1;
		uint8_t LHPVOL: 6;		// -57dB to 6dB with 1dB step
	};
	uint8_t raw;
} AlxAdau1961_RegVal_R29_PlayHpLeftVol;

//------------------------------------------------------------------------------
// R30: Playback Headphone Right Volume Control
//------------------------------------------------------------------------------
typedef enum
{
	OutputMode_EnableLineOut = 0b0,
	OutputMode_EnableHPOut   = 0b1
} AlxAdau1961_R30_OutputMode;
typedef union
{
	struct __attribute__((packed))
	{
		AlxAdau1961_R30_OutputMode HPMODE : 1;
		AlxAdau1961_R29_R30_HPMute RHPM : 1;
		uint8_t RHPVOL: 6;		// -57dB to 6dB with 1dB step
	};
	uint8_t raw;
} AlxAdau1961_RegVal_R30_PlayHpRightVol;

//------------------------------------------------------------------------------
// R31: Playback Line Output Left Volume Control
//------------------------------------------------------------------------------
typedef union
{
	struct __attribute__((packed))
	{
		AlxAdau1961_General_OutputMode LOMODE : 1;
		AlxAdau1961_General_OutputMute LOUTM : 1;
		uint8_t LOUTVOL: 6;		// -57dB to 6dB with 1dB step
	};
	uint8_t raw;
} AlxAdau1961_RegVal_R31_LineOutputLeftVol;

//------------------------------------------------------------------------------
// R32: Playback Line Output Right Volume Control
//------------------------------------------------------------------------------
typedef union
{
	struct __attribute__((packed))
	{
		AlxAdau1961_General_OutputMode ROMODE : 1;
		AlxAdau1961_General_OutputMute ROUTM : 1;
		uint8_t ROUTVOL: 6;		// -57dB to 6dB with 1dB step
	};
	uint8_t raw;
} AlxAdau1961_RegVal_R32_LineOutputRightVol;

//------------------------------------------------------------------------------
// R33: Playback Mono Output Control
//------------------------------------------------------------------------------
typedef union
{
	struct __attribute__((packed))
	{
		AlxAdau1961_General_OutputMode MOMODE : 1;
		AlxAdau1961_General_OutputMute MONOM : 1;
		uint8_t MONOVOL: 6;		// -57dB to 6dB with 1dB step
	};
	uint8_t raw;
} AlxAdau1961_RegVal_R33_PlayMonoOutput;

//------------------------------------------------------------------------------
// R34: Playback Pop/Click Suppression
//------------------------------------------------------------------------------
typedef enum
{
	AnalVolSlew_21ms25 = 0b00,
	AnalVolSlew_42ms5  = 0b01,
	AnalVolSlew_85ms   = 0b10,
	AnalVolSlew_Off    = 0b11
} AlxAdau1961_R34_AnalVolSlew;
typedef enum
{
	PopSuppressDisable_Enabled  = 0b0,
	PopSuppressDisable_Disabled = 0b1
} AlxAdau1961_R34_PopSuppressDisable;
typedef enum
{
	PopSuppressMode_Normal = 0b0,
	PopSuppressMode_LowPwr = 0b1
} AlxAdau1961_R34_PopSuppressMode;
typedef union
{
	struct __attribute__((packed))
	{
		uint8_t unused_0: 1;  // bit 0 is unused
		AlxAdau1961_R34_AnalVolSlew ASLEW : 2;
		AlxAdau1961_R34_PopSuppressDisable POPLESS : 1;
		AlxAdau1961_R34_PopSuppressMode POPMODE : 1;
		uint8_t unused_5_7 : 3;  // bits 5..7 are unused
	};
	uint8_t raw;
} AlxAdau1961_RegVal_R34_PopClickSuppress;

//------------------------------------------------------------------------------
// R35: Playback Power Management
//------------------------------------------------------------------------------
typedef enum
{
	PlaybackChEnable_Disabled = 0b0,
	PlaybackChEnable_Enabled  = 0b1
} AlxAdau1961_R35_PlaybackChEnable;
typedef union
{
	struct __attribute__((packed))
	{
		AlxAdau1961_R35_PlaybackChEnable PLEN : 1;
		AlxAdau1961_R35_PlaybackChEnable PREN : 1;
		uint8_t unused_2_7: 6; // bits 2..7 are unused
	};
	uint8_t raw;
} AlxAdau1961_RegVal_R35_PlayPowerMgmt;

//------------------------------------------------------------------------------
// R36: DAC Control 0
//------------------------------------------------------------------------------
typedef enum
{
	DACEnable_BothOff = 0b00,
	DACEnable_LChOn   = 0b01,
	DACEnable_RChOn   = 0b10,
	DACEnable_BothOn  = 0b11
} AlxAdau1961_R36_DACEnable;
typedef enum
{
	DACEmphFilter_Disabled = 0b0,
	DACEmphFilter_Enabled  = 0b1
} AlxAdau1961_R36_DACEmphFilter;
typedef enum
{
	DACPolarity_Normal   = 0b0,
	DACPolarity_Inverted = 0b1
} AlxAdau1961_R36_DACPolarity;
typedef enum
{
	DACMonoMode_Stereo     = 0b00,
	DACMonoMode_LChMono    = 0b01,
	DACMonoMode_RChMono    = 0b10,
	DACMonoMode_BothChMono = 0b11
} AlxAdau1961_R36_DACMonoMode;
typedef union
{
	struct __attribute__((packed))
	{
		AlxAdau1961_R36_DACEnable DACEN : 2;
		AlxAdau1961_R36_DACEmphFilter DEMPH : 1;
		uint8_t unused_3_4: 2;    // bits 3..4 are unused
		AlxAdau1961_R36_DACPolarity DACPOL : 1;
		AlxAdau1961_R36_DACMonoMode DACMONO : 2;
	};
	uint8_t raw;
} AlxAdau1961_RegVal_R36_DacControl0;

//------------------------------------------------------------------------------
// R37: DAC Control 1
//------------------------------------------------------------------------------
typedef union
{
	struct __attribute__((packed))
	{
		uint8_t LDAVOL;			// 0dB to -95.625dB with 0.375dB step
	};
	uint8_t raw;
} AlxAdau1961_RegVal_R37_DacControl1;

//------------------------------------------------------------------------------
// R38: DAC Control 2
//------------------------------------------------------------------------------
typedef union
{
	struct __attribute__((packed))
	{
		uint8_t RDAVOL;			// 0dB to -95.625dB with 0.375dB step
	};
	uint8_t raw;
} AlxAdau1961_RegVal_R38_DacControl2;

//------------------------------------------------------------------------------
// R39: Serial Port Pad Control
//------------------------------------------------------------------------------
typedef union
{
	struct __attribute__((packed))
	{
		AlxAdau1961_General_PullConfig BCLKP : 2;
		AlxAdau1961_General_PullConfig LRCLKP : 2;
		AlxAdau1961_General_PullConfig DACSDP : 2;
		AlxAdau1961_General_PullConfig ADCSDP : 2;
	};
	uint8_t raw;
} AlxAdau1961_RegVal_R39_SerialPortPad;

//------------------------------------------------------------------------------
// R40: Control Port Pad Control 0
//------------------------------------------------------------------------------
typedef union
{
	struct __attribute__((packed))
	{
		AlxAdau1961_General_PullConfig SDAP : 2;
		AlxAdau1961_General_PullConfig SCLP : 2;
		AlxAdau1961_General_PullConfig CLCHP : 2;
		AlxAdau1961_General_PullConfig CDATP : 2;
	};
	uint8_t raw;
} AlxAdau1961_RegVal_R40_ControlPortPad0;

//------------------------------------------------------------------------------
// R41: Control Port Pad Control 1
//------------------------------------------------------------------------------
typedef enum
{
	PinDriveStrength_Low  = 0b0,
	PinDriveStrength_High = 0b1
} AlxAdau1961_R41_R42_PinDriveStrength;
typedef union
{
	struct __attribute__((packed))
	{
		AlxAdau1961_R41_R42_PinDriveStrength SDASTR : 1;
		uint8_t unused_1_7: 7; // bits 1..7 are unused
	};
	uint8_t raw;
} AlxAdau1961_RegVal_R41_ControlPortPad1;

//------------------------------------------------------------------------------
// R42: Jack Detect Pin Control
//------------------------------------------------------------------------------
typedef union
{
	struct __attribute__((packed))
	{
		uint8_t unused_0_1: 2; // bits 0..1 are unused
		AlxAdau1961_General_PullConfig JDP : 2;
		uint8_t unused_4: 1; // bit 4 is unused
		AlxAdau1961_R41_R42_PinDriveStrength JDSTR : 1;
		uint8_t unused_6_7: 2; // bits 6..7 are unused
	};
	uint8_t raw;
} AlxAdau1961_RegVal_R42_JackDetectPin;

//------------------------------------------------------------------------------
// R67: Dejitter Control
//------------------------------------------------------------------------------
typedef enum						// MF: Check if this is good, or al 256 values are needed
{
	DejitWindowSize_0 = 0b00000000,
	DejitWindowSize_3 = 0b00000011,
	DejitWindowSize_5 = 0b00000101,
	DejitWindowSize_test = 0b01000101
} AlxAdau1961_R67_DejitWindowSize;
typedef union
{
	struct __attribute__((packed))
	{
		AlxAdau1961_R67_DejitWindowSize DEJIT;
	};
	uint8_t raw;
} AlxAdau1961_RegVal_R67_DejitterControl;


//******************************************************************************
// Register Structures
//******************************************************************************
typedef struct
{
	uint16_t addr;
	uint8_t len;
	AlxAdau1961_RegVal_R0_ClockControl val;
} AlxAdau1961_Reg_R0_ClockControl;

typedef struct
{
	uint16_t addr;
	uint8_t len;
	AlxAdau1961_RegVal_R1_PllControl val;
} AlxAdau1961_Reg_R1_PllControl;

typedef struct
{
	uint16_t addr;
	uint8_t len;
	AlxAdau1961_RegVal_R2_DigMicJackDetect val;
} AlxAdau1961_Reg_R2_DigMicJackDetect;

//AlxAdau1961_Reg_R3_Reserved

typedef struct
{
	uint16_t addr;
	uint8_t len;
	AlxAdau1961_RegVal_R4_RecMixerLeft0 val;
} AlxAdau1961_Reg_R4_RecMixerLeft0;

typedef struct
{
	uint16_t addr;
	uint8_t len;
	AlxAdau1961_RegVal_R5_RecMixerLeft1 val;
} AlxAdau1961_Reg_R5_RecMixerLeft1;

typedef struct
{
	uint16_t addr;
	uint8_t len;
	AlxAdau1961_RegVal_R6_RecMixerRight0 val;
} AlxAdau1961_Reg_R6_RecMixerRight0;

typedef struct
{
	uint16_t addr;
	uint8_t len;
	AlxAdau1961_RegVal_R7_RecMixerRight1 val;
} AlxAdau1961_Reg_R7_RecMixerRight1;

typedef struct
{
	uint16_t addr;
	uint8_t len;
	AlxAdau1961_RegVal_R8_LeftDiffInputVol val;
} AlxAdau1961_Reg_R8_LeftDiffInputVol;

typedef struct
{
	uint16_t addr;
	uint8_t len;
	AlxAdau1961_RegVal_R9_RightDiffInputVol val;
} AlxAdau1961_Reg_R9_RightDiffInputVol;

typedef struct
{
	uint16_t addr;
	uint8_t len;
	AlxAdau1961_RegVal_R10_RecordMicBias val;
} AlxAdau1961_Reg_R10_RecordMicBias;

typedef struct
{
	uint16_t addr;
	uint8_t len;
	AlxAdau1961_RegVal_R11_Alc0 val;
} AlxAdau1961_Reg_R11_Alc0;

typedef struct
{
	uint16_t addr;
	uint8_t len;
	AlxAdau1961_RegVal_R12_Alc1 val;
} AlxAdau1961_Reg_R12_Alc1;

typedef struct
{
	uint16_t addr;
	uint8_t len;
	AlxAdau1961_RegVal_R13_Alc2 val;
} AlxAdau1961_Reg_R13_Alc2;

typedef struct
{
	uint16_t addr;
	uint8_t len;
	AlxAdau1961_RegVal_R14_Alc3 val;
} AlxAdau1961_Reg_R14_Alc3;

typedef struct
{
	uint16_t addr;
	uint8_t len;
	AlxAdau1961_RegVal_R15_SerialPort0 val;
} AlxAdau1961_Reg_R15_SerialPort0;

typedef struct
{
	uint16_t addr;
	uint8_t len;
	AlxAdau1961_RegVal_R16_SerialPort1 val;
} AlxAdau1961_Reg_R16_SerialPort1;

typedef struct
{
	uint16_t addr;
	uint8_t len;
	AlxAdau1961_RegVal_R17_Converter0 val;
} AlxAdau1961_Reg_R17_Converter0;

typedef struct
{
	uint16_t addr;
	uint8_t len;
	AlxAdau1961_RegVal_R18_Converter1 val;
} AlxAdau1961_Reg_R18_Converter1;

typedef struct
{
	uint16_t addr;
	uint8_t len;
	AlxAdau1961_RegVal_R19_AdcControl val;
} AlxAdau1961_Reg_R19_AdcControl;

typedef struct
{
	uint16_t addr;
	uint8_t len;
	AlxAdau1961_RegVal_R20_LeftDigitalVol val;
} AlxAdau1961_Reg_R20_LeftDigitalVol;

typedef struct
{
	uint16_t addr;
	uint8_t len;
	AlxAdau1961_RegVal_R21_RightDigitalVol val;
} AlxAdau1961_Reg_R21_RightDigitalVol;

typedef struct
{
	uint16_t addr;
	uint8_t len;
	AlxAdau1961_RegVal_R22_PlayMixerLeft0 val;
} AlxAdau1961_Reg_R22_PlayMixerLeft0;

typedef struct
{
	uint16_t addr;
	uint8_t len;
	AlxAdau1961_RegVal_R23_PlayMixerLeft1 val;
} AlxAdau1961_Reg_R23_PlayMixerLeft1;

typedef struct
{
	uint16_t addr;
	uint8_t len;
	AlxAdau1961_RegVal_R24_PlayMixerRight0 val;
} AlxAdau1961_Reg_R24_PlayMixerRight0;

typedef struct
{
	uint16_t addr;
	uint8_t len;
	AlxAdau1961_RegVal_R25_PlayMixerRight1 val;
} AlxAdau1961_Reg_R25_PlayMixerRight1;

typedef struct
{
	uint16_t addr;
	uint8_t len;
	AlxAdau1961_RegVal_R26_PlayLrMixerLeft val;
} AlxAdau1961_Reg_R26_PlayLrMixerLeft;

typedef struct
{
	uint16_t addr;
	uint8_t len;
	AlxAdau1961_RegVal_R27_PlayLrMixerRight val;
} AlxAdau1961_Reg_R27_PlayLrMixerRight;

typedef struct
{
	uint16_t addr;
	uint8_t len;
	AlxAdau1961_RegVal_R28_PlayLrMixerMono val;
} AlxAdau1961_Reg_R28_PlayLrMixerMono;

typedef struct
{
	uint16_t addr;
	uint8_t len;
	AlxAdau1961_RegVal_R29_PlayHpLeftVol val;
} AlxAdau1961_Reg_R29_PlayHpLeftVol;

typedef struct
{
	uint16_t addr;
	uint8_t len;
	AlxAdau1961_RegVal_R30_PlayHpRightVol val;
} AlxAdau1961_Reg_R30_PlayHpRightVol;

typedef struct
{
	uint16_t addr;
	uint8_t len;
	AlxAdau1961_RegVal_R31_LineOutputLeftVol val;
} AlxAdau1961_Reg_R31_LineOutputLeftVol;

typedef struct
{
	uint16_t addr;
	uint8_t len;
	AlxAdau1961_RegVal_R32_LineOutputRightVol val;
} AlxAdau1961_Reg_R32_LineOutputRightVol;

typedef struct
{
	uint16_t addr;
	uint8_t len;
	AlxAdau1961_RegVal_R33_PlayMonoOutput val;
} AlxAdau1961_Reg_R33_PlayMonoOutput;

typedef struct
{
	uint16_t addr;
	uint8_t len;
	AlxAdau1961_RegVal_R34_PopClickSuppress val;
} AlxAdau1961_Reg_R34_PopClickSuppress;

typedef struct
{
	uint16_t addr;
	uint8_t len;
	AlxAdau1961_RegVal_R35_PlayPowerMgmt val;
} AlxAdau1961_Reg_R35_PlayPowerMgmt;

typedef struct
{
	uint16_t addr;
	uint8_t len;
	AlxAdau1961_RegVal_R36_DacControl0 val;
} AlxAdau1961_Reg_R36_DacControl0;

typedef struct
{
	uint16_t addr;
	uint8_t len;
	AlxAdau1961_RegVal_R37_DacControl1 val;
} AlxAdau1961_Reg_R37_DacControl1;

typedef struct
{
	uint16_t addr;
	uint8_t len;
	AlxAdau1961_RegVal_R38_DacControl2 val;
} AlxAdau1961_Reg_R38_DacControl2;

typedef struct
{
	uint16_t addr;
	uint8_t len;
	AlxAdau1961_RegVal_R39_SerialPortPad val;
} AlxAdau1961_Reg_R39_SerialPortPad;

typedef struct
{
	uint16_t addr;
	uint8_t len;
	AlxAdau1961_RegVal_R40_ControlPortPad0 val;
} AlxAdau1961_Reg_R40_ControlPortPad0;

typedef struct
{
	uint16_t addr;
	uint8_t len;
	AlxAdau1961_RegVal_R41_ControlPortPad1 val;
} AlxAdau1961_Reg_R41_ControlPortPad1;

typedef struct
{
	uint16_t addr;
	uint8_t len;
	AlxAdau1961_RegVal_R42_JackDetectPin val;
} AlxAdau1961_Reg_R42_JackDetectPin;

typedef struct
{
	uint16_t addr;
	uint8_t len;
	AlxAdau1961_RegVal_R67_DejitterControl val;
} AlxAdau1961_Reg_R67_DejitterControl;


//******************************************************************************
// Main Register Structure
//******************************************************************************
typedef struct
{
	AlxAdau1961_Reg_R0_ClockControl R0_ClockControl;
	AlxAdau1961_Reg_R1_PllControl R1_PllControl;
	AlxAdau1961_Reg_R2_DigMicJackDetect R2_DigMicJackDetect;
	//AlxAdau1961_Reg_R3_Reserved
	AlxAdau1961_Reg_R4_RecMixerLeft0 R4_RecMixerLeft0;
	AlxAdau1961_Reg_R5_RecMixerLeft1 R5_RecMixerLeft1;
	AlxAdau1961_Reg_R6_RecMixerRight0 R6_RecMixerRight0;
	AlxAdau1961_Reg_R7_RecMixerRight1 R7_RecMixerRight1;
	AlxAdau1961_Reg_R8_LeftDiffInputVol R8_LeftDiffInputVol;
	AlxAdau1961_Reg_R9_RightDiffInputVol R9_RightDiffInputVol;
	AlxAdau1961_Reg_R10_RecordMicBias R10_RecordMicBias;
	AlxAdau1961_Reg_R11_Alc0 R11_Alc0;
	AlxAdau1961_Reg_R12_Alc1 R12_Alc1;
	AlxAdau1961_Reg_R13_Alc2 R13_Alc2;
	AlxAdau1961_Reg_R14_Alc3 R14_Alc3;
	AlxAdau1961_Reg_R15_SerialPort0 R15_SerialPort0;
	AlxAdau1961_Reg_R16_SerialPort1 R16_SerialPort1;
	AlxAdau1961_Reg_R17_Converter0 R17_Converter0;
	AlxAdau1961_Reg_R18_Converter1 R18_Converter1;
	AlxAdau1961_Reg_R19_AdcControl R19_AdcControl;
	AlxAdau1961_Reg_R20_LeftDigitalVol R20_LeftDigitalVol;
	AlxAdau1961_Reg_R21_RightDigitalVol R21_RightDigitalVol;
	AlxAdau1961_Reg_R22_PlayMixerLeft0 R22_PlayMixerLeft0;
	AlxAdau1961_Reg_R23_PlayMixerLeft1 R23_PlayMixerLeft1;
	AlxAdau1961_Reg_R24_PlayMixerRight0 R24_PlayMixerRight0;
	AlxAdau1961_Reg_R25_PlayMixerRight1 R25_PlayMixerRight1;
	AlxAdau1961_Reg_R26_PlayLrMixerLeft R26_PlayLrMixerLeft;
	AlxAdau1961_Reg_R27_PlayLrMixerRight R27_PlayLrMixerRight;
	AlxAdau1961_Reg_R28_PlayLrMixerMono R28_PlayLrMixerMono;
	AlxAdau1961_Reg_R29_PlayHpLeftVol R29_PlayHpLeftVol;
	AlxAdau1961_Reg_R30_PlayHpRightVol R30_PlayHpRightVol;
	AlxAdau1961_Reg_R31_LineOutputLeftVol R31_LineOutputLeftVol;
	AlxAdau1961_Reg_R32_LineOutputRightVol R32_LineOutputRightVol;
	AlxAdau1961_Reg_R33_PlayMonoOutput R33_PlayMonoOutput;
	AlxAdau1961_Reg_R34_PopClickSuppress R34_PopClickSuppress;
	AlxAdau1961_Reg_R35_PlayPowerMgmt R35_PlayPowerMgmt;
	AlxAdau1961_Reg_R36_DacControl0 R36_DacControl0;
	AlxAdau1961_Reg_R37_DacControl1 R37_DacControl1;
	AlxAdau1961_Reg_R38_DacControl2 R38_DacControl2;
	AlxAdau1961_Reg_R39_SerialPortPad R39_SerialPortPad;
	AlxAdau1961_Reg_R40_ControlPortPad0 R40_ControlPortPad0;
	AlxAdau1961_Reg_R41_ControlPortPad1 R41_ControlPortPad1;
	AlxAdau1961_Reg_R42_JackDetectPin R42_JackDetectPin;
	AlxAdau1961_Reg_R67_DejitterControl R67_DejitterControl;
} AlxAdau1961_Reg;


//******************************************************************************
// Types
//******************************************************************************
typedef struct
{
	// Parameters - Const
	int8_t MUTE_GAIN_dB;

	float IN_DIFF_GAIN_MIN_dB;
	float IN_DIFF_GAIN_MAX_dB;
	float IN_DIFF_GAIN_PER_BIT_dB;
	float IN_DIFF_GAIN_OFFSET_dB;
	float IN_DIFF_PGA_BOOST_THRESHOLD_dB;	// When set gain by user is larger then this value, PGA gain boost will be set to +20dB
	float IN_DIFF_PGA_BOOST_REDUCE_VAL_dB;	// When set gain by user is larger than IN_DIFF_PGA_BOOST_THRESHOLD_dB, we will reduce it by this val
											// We must reduce set gain because PGA gain boost is set to +20dB (we will reduce it by 20.25dB, to get number which is multiple of 0.75dB to have uniform 0.75dB/bit, for easier handling)
	int8_t IN_AUX_GAIN_MIN_dB;
	int8_t IN_AUX_GAIN_MAX_dB;
	int8_t IN_AUX_GAIN_PER_BIT_dB;
	int8_t IN_AUX_GAIN_OFFSET_dB;
	int8_t IN_AUX_MUTE_OFFSET_dB;

	int8_t OUT_LINE_GAIN_MIN_dB;
	int8_t OUT_LINE_GAIN_MAX_dB;
	int8_t OUT_LINE_GAIN_PER_BIT_dB;
	int8_t OUT_LINE_GAIN_OFFSET_dB;

	// Objects - External
	AlxI2s* i2s;
	AlxI2c* i2c;

	// Parameters
	uint8_t i2cAddr;
	bool i2cCheckWithRead;
	uint8_t i2cNumOfTries;
	uint16_t i2cTimeout_ms;

	// Variables
	AlxAdau1961_Reg reg;

	// Info
	bool isInit;
	bool wasCtorCalled;
} AlxAdau1961;


//******************************************************************************
// Constructor
//******************************************************************************
void AlxAdau1961_Ctor
(
	AlxAdau1961* me,
	AlxI2s* i2s,
	AlxI2c* i2c,
	uint8_t i2cAddr,
	bool i2cCheckWithRead,
	uint8_t i2cNumOfTries,
	uint16_t i2cTimeout_ms
);


//******************************************************************************
// Functions
//******************************************************************************
Alx_Status AlxAdau1961_Init(AlxAdau1961* me);
Alx_Status AlxAdau1961_DeInit(AlxAdau1961* me);
void AlxAdau1961_Foreground_Handle(AlxAdau1961* me);
Alx_Status AlxAdau1961_InDiffL_SetGain_dB(AlxAdau1961* me, float gain_dB);		// Mute -> gain_dB = -120
Alx_Status AlxAdau1961_InDiffR_SetGain_dB(AlxAdau1961* me, float gain_dB);		// Mute -> gain_dB = -120
Alx_Status AlxAdau1961_InAuxL_SetGain_dB(AlxAdau1961* me, int8_t gain_dB);		// Mute -> gain_dB = -120
Alx_Status AlxAdau1961_InAuxR_SetGain_dB(AlxAdau1961* me, int8_t gain_dB);		// Mute -> gain_dB = -120
Alx_Status AlxAdau1961_OutLineL_SetGain_dB(AlxAdau1961* me, int8_t gain_dB);	// Mute -> gain_dB = -120
Alx_Status AlxAdau1961_OutLineR_SetGain_dB(AlxAdau1961* me, int8_t gain_dB);	// Mute -> gain_dB = -120


#ifdef __cplusplus
}
#endif

#endif // ALX_ADAU1961_H




































//typedef enum
//{
//	// In
//	AlxAdau1961_Ch_LIN  = 0,
//	AlxAdau1961_Ch_RIN  = 1,
//	AlxAdau1961_Ch_LAUX = 2,
//	AlxAdau1961_Ch_RAUX = 3,
//
//	// Out
//	AlxAdau1961_Ch_LOUT = 4,
//	AlxAdau1961_Ch_ROUT = 5
//} AlxAdau1961_Ch;



//Alx_Status AlxAdau1961_StartStream				(AlxAdau1961* me);	//TODO
//Alx_Status AlxAdau1961_StopStream				(AlxAdau1961* me);	//TODO
///*Alx_Status AlxAdau1961_Dac_SetGain_dB			(AlxAdau1961* me, uint8_t* dacId, float* gain_dB);	//TODO
//Alx_Status AlxAdau1961_Dac_SetGain_pct			(AlxAdau1961* me, uint8_t* dacId, float* gain_pct);	//TODO
//Alx_Status AlxAdau1961_Dac_SetVolume_dB			(AlxAdau1961* me, uint8_t* dacId, float* volume_dB);
//Alx_Status AlxAdau1961_Dac_SetVolume_pct		(AlxAdau1961* me, uint8_t* dacId, float* volume_pct);	//TODO
//Alx_Status AlxAdau1961_Dac_Mute					(AlxAdau1961* me, uint8_t* dacId);*/







//Alx_Status AlxAdau1961_Ch_Config				(AlxAdau1961* me, AlxAdau1961_Ch* chIn, AlxAdau1961_Ch* chOut);
//Alx_Status AlxAdau1961_Ch_In_SetGain_dB			(AlxAdau1961* me, AlxAdau1961_Ch* ch, float* gain_dB);	// gain_dB should be uint8_t
//Alx_Status AlxAdau1961_Ch_In_SetVolume_pct		(AlxAdau1961* me, AlxAdau1961_Ch* ch, float* volume_pct);
//Alx_Status AlxAdau1961_Ch_Out_SetGain_dB		(AlxAdau1961* me, AlxAdau1961_Ch* ch, float* gain_dB);	// gain_dB should be uint8_t
//Alx_Status AlxAdau1961_Ch_Out_SetVolume_pct		(AlxAdau1961* me, AlxAdau1961_Ch* ch, float* volume_pct);
//Alx_Status AlxAdau1961_Ch_Mute					(AlxAdau1961* me, AlxAdau1961_Ch ch);
//Alx_Status AlxAdau1961_Ch_UnMute				(AlxAdau1961* me, AlxAdau1961_Ch ch);
