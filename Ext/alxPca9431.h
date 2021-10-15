/**
  ******************************************************************************
  * @file alxPca9431.h
  * @brief Auralix C Library - ALX opis PCA9431 Module
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
#include "alxTimSw.h"
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
// General Enums //

/*******************************************************************************************				PAC9431_DeviceId	0x00	*/
typedef union
{
	struct __attribute__((packed))
	{
		uint8_t Revision : 3;	/* Revision number						*/
		uint8_t ID : 4;			/* Device ID							*/
		uint8_t unused_7 : 1;
	};
	uint8_t raw;
} AlxPca9431_RegVal_0x00_DeviceId;
	
/*******************************************************************************************				PAC943-1_SystemInt	0x01	*/
typedef enum
{	/* Thermal shutdown interrupt: */
	TempShutdownIntOff		= 0b0,	/*	R/C		0:Thermal shutdown interrupt: 0: thermal shutdown is not triggered	*/
	TempShutdownIntOn		= 0b1	/*	R/C		1: die temp ≥ TSTDN (set in THM_SHDN[2:0], thermal shutdown is triggered */
} AlxPca9431_0x01_TempShutdownInt;

typedef enum
{
	/* Die temp pre - warning interrupt */
	TempWarnIntOff		= 0b0,		/*	R/C		0: die temp < TWRN		*/
	TempWarnIntOn		= 0b1		/*	R/C		1: die temp ≥ TWRN; 	*/
	/*	TWRN threshold is configured by TEMP_WRN[1:0]					*/
} AlxPca9431_0x01_TempWarnInt;

typedef enum
{
	/* Watchdog Timer Expiration Interrupt: */
	WdTimerExpirationIntOff	= 0b0,		/*	R/C		0: The watchdog timer expiration has not happened since the last time this bit was cleared. */
	WdTimerExpirationIntOn	= 0b1		/*	R/C		1: The watchdog timer expiration has happened since the last time this bit was cleared. 	*/
	} AlxPca9431_0x01_WdTimerExpInt;
	
typedef enum
{
	/* Watchdog Timer Expiration Interrupt: */
	VpwrScpIntOff		= 0b0,		/*	R/C		0: The VPWR short circuit protection is not triggered	*/
	VpwrScpIntOn		= 0b1		/*	R/C		1: The VPWR short circuit protection triggered 			*/
} AlxPca9431_0x01_VpwrScpInt;
	
typedef union
{
	struct __attribute__((packed))
	{
		AlxPca9431_0x01_TempShutdownInt Temp_SHD_INT : 1;	/* Thermal shutdown interrupt:				*/
		AlxPca9431_0x01_TempWarnInt		Temp_WRN_INT : 1;	/* Die temp pre - warning interrupt			*/
		AlxPca9431_0x01_WdTimerExpInt	WD_TIMER_INT : 1;	/* Watchdog Timer Expiration Interrupt:		*/
		AlxPca9431_0x01_VpwrScpInt		VPWR_SCP_INT : 1;	/* VPWR short circuit protection Interrupt: */
		uint8_t unused_4_7 : 4;			/* bits 4..7 are unused											*/
	};
	uint8_t raw;
} AlxPca9431_RegVal_0x01_SystemInt;

	
/*******************************************************************************************			PAC943-1_SystemIntMask	0x02	*/
typedef enum
{ /* Temperature shut down interrupt disable: */
	Temp_ShutdownIntMaskOff		= 0b0,	/*	R/W		0: Temp_SHD_INT can trigger INT		*/
	Temp_ShutdownIntMaskOn		= 0b1	/*	R/W		1: Temp_SHD_INT trigger disabled	*/
} AlxPca9431_0x02_TempShutdownIntMask;

typedef enum
{	/* Temperature warning interrupt disable: */
	TempWarnIntMaskOff		= 0b0,		/*	R/W		0: Temp_WRN_INT can trigger INT		*/
	TempWarnIntMaskOn		= 0b1		/*	R/W		1: Temp_WRN_INT trigger disabled 	*/
	/*	TWRN threshold is configured by TEMP_WRN[1:0]	*/
} AlxPca9431_0x02_TempWarnIntMask;

typedef enum
{	/* Watchdog timer interrupt disable: */
	WdTimerExpIntMaskOff	= 0b0,		/*	R/W		0: WD_TIMER_INT can trigger INT		*/
	WdTimerExpIntMaskOn		= 0b1		/*	R/W		1: WD_TIMER_INT trigger disabled 	*/
} AlxPca9431_0x02_WdTimerExpIntMask;

typedef enum
{	/* VRECT voltage low interrupt disable: */
	VpwrScpIntMaskOff		= 0b0,		/*	R/W		0: VPWR_SCP_INT can trigger INT		*/
	VpwrScpIntMaskOn		= 0b1		/*	R/W		1: VPWR_SCP_INT trigger disabled 	*/
} AlxPca9431_0x02_VpwrScpIntMask;

typedef union
{
	struct __attribute__((packed))
	{
		AlxPca9431_0x02_TempShutdownIntMask Temp_SHD_INT_MSK : 1;	/* Temperature shut down interrupt disable:		*/
		AlxPca9431_0x02_TempWarnIntMask		Temp_WRN_INT_MSK : 1;	/* Temperature warning interrupt disable:		*/
		AlxPca9431_0x02_WdTimerExpIntMask	WD_TIMER_INT_MSK : 1;	/* Watchdog timer interrupt disable:			*/
		AlxPca9431_0x02_VpwrScpIntMask		VPWR_SCP_INT_MSK : 1;	/* VRECT voltage low interrupt disable:			*/
		uint8_t unused_4_7 : 4;				/* bits 4..7 are unused													*/
	};
	uint8_t raw;
} AlxPca9431_RegVal_0x02_SystemIntMask;

/*******************************************************************************************				PAC943-1_VRectInt	0x03	*/
typedef enum
{	/* VRECT overvoltage protection interrupt */
	VRectOvProtIntOff	= 0b0,	/*	R/C		0: VRECT overvoltage protection not triggered								*/
	VRectOvProtIntOn	= 0b1	/*	R/C		1: VRECT >OVPRECT (set in RECT_OVP[2:0]), overvoltage protection triggered	*/
} AlxPca9431_0x03_VRectOvProtInt;

typedef enum
{	/* VRECT overvoltage warning interrupt */
	VRectOvWarnIntOff	= 0b0,	/*	R/C		0: VRECT< VRECT_OVW	*/
	VRectOvWarnIntOn	= 0b1	/*	R/C		1: VRECT≥ VRECT_OVW	*/
} AlxPca9431_0x03_VRectOvWarnInt;
	
typedef enum
{	/* VRECT low voltage regulation trigger interrupt */
	VRectRegLowIntOff	= 0b0,	/*	R/C		0: VRECT low voltage regulation not triggered	*/
	VRectRegLowIntOn	= 0b1	/*	R/C		1: VRECT low voltage regulation triggered		*/
} AlxPca9431_0x03_VRectRegLowInt;

typedef enum
{	/* VRECT high voltage regulation trigger interrupt */
	VRectRegHighIntOff	= 0b0,	/*	R/C		0: VRECT high voltage regulation not triggered	*/
	VRectRegHighIntOn	= 0b1	/*	R/C		1: VRECT high voltage regulation triggered		*/
} AlxPca9431_0x03_VRectRegHighInt;

typedef enum
{	/* VRECT voltage good Interrupt: */
	VRectGoodIntOff	= 0b0,		/*	R/C		0: The VRECT voltage is not in the range of VUVLO <VRECT < VOVP	*/
	VRectGoodIntOn	= 0b1		/*	R/C		1: The VRECT voltage is in the range of VUVLO <VRECT < VOVP		*/
} AlxPca9431_0x03_VRectGoodInt;

typedef enum
{
	/* VRECT voltage ready Interrupt: */
	VRectRdyIntOff = 0b0,	/*	R/C		0: The VRECT voltage is less than VUVLO		*/
	VRectRdyIntOn = 0b1		/*	R/C		1: The VRECT voltage is larger than VUVLO	*/
} AlxPca9431_0x03_VRectRdyInt;
	
typedef union
{
	struct __attribute__((packed))
	{
		AlxPca9431_0x03_VRectOvProtInt	VRECT_OVP_INT		: 1;	/* VRECT overvoltage protection interrupt			*/
		AlxPca9431_0x03_VRectOvWarnInt	VRECT_OVW_INT		: 1;	/* VRECT overvoltage warning interrupt				*/
		uint8_t unused_2 : 1;			/* bit 2 is unused																*/
		AlxPca9431_0x03_VRectRegLowInt	VRECT_REGLOW_INT	: 1;	/* VRECT low voltage regulation trigger interrupt	*/
		AlxPca9431_0x03_VRectRegHighInt VRECT_REGHIGH_INT	: 1;	/* VRECT high voltage regulation trigger interrupt	*/
		AlxPca9431_0x03_VRectGoodInt	VRECT_GOOD_INT		: 1;	/* VRECT voltage good Interrupt:					*/
		AlxPca9431_0x03_VRectRdyInt		VRECT_RDY_INT		: 1;	/* VRECT voltage ready Interrupt:					*/
		uint8_t unused_7 : 1;			/* bit 7 is unused																*/
	};
	uint8_t raw;
} AlxPca9431_RegVal_0x03_VRectInt;
	
/*******************************************************************************************			PAC943-1_VRectIntMask	0x04	*/
typedef enum
{ /* Overvoltage protection interrupt disable: */
	VRectOvProtIntMaskOff	= 0b0,	/*	R/W		0: VRECT_OVP_INT can trigger INT	*/
	VRectOvProtIntMaskOn	= 0b1	/*	R/W		1: VRECT_OVP_INT trigger disabled	*/
} AlxPca9431_0x04_VRectOvProtIntMask;

typedef enum
{ /* VRECT overvoltage warning interrupt disable: */
	VRectOvWarnIntMaskOff	= 0b0,	/*	R/W		0: VRECT_OVW_INT can trigger INT	*/
	VRectOvWarnIntMaskOn	= 0b1	/*	R/W		1: VRECT_OVW_INT trigger disabled	*/
} AlxPca9431_0x04_VRectOvWarnIntMask;

typedef enum
{ /* VRECT low voltage regulation trigger interrupt disable:*/
	VRectRegLowIntMaskOff	= 0b0,	/*	R/W		0: VRECT_REGLOW_INT can trigger INT		*/
	VRectRegLowIntMaskOn	= 0b1	/*	R/W		1: VRECT_REGLOW_INT trigger disabled	*/
} AlxPca9431_0x04_VRectRegLowIntMask;

typedef enum
{ /* VRECT high voltage regulation trigger interrupt disable:*/
	VRectRegHighIntMaskOff	= 0b0,	/*	R/W		0: VRECT_REGHIGH_INT can trigger INT	*/
	VRectRegHighIntMaskOn	= 0b1	/*	R/W		1: VRECT_REGHIGH_INT trigger disabled	*/
} AlxPca9431_0x04_VRectRegHighIntMask;

typedef enum
{ /* VRECT voltage good interrupt disable: */
	VRectGoodIntMaskOff = 0b0,		/*	R/W		0: VRECT_GOOD_INT can trigger INT	*/
	VRectGoodIntMaskOn	= 0b1		/*	R/W		1: VRECT_GOOD_INT trigger disabled	*/
} AlxPca9431_0x04_VRectGoodIntMask;

typedef enum
{	/* VRECT voltage ready interrupt disable: */
	VRectRdyIntMaskOff	= 0b0,		/*	R/W		0: VRECT_RDY_INT can trigger INT	*/
	VRectRdyIntMaskOn	= 0b1		/*	R/W		1: VRECT_RDY_INT trigger disabled	*/
	} AlxPca9431_0x04_VRectRdyIntMask;
	
typedef union
{
	struct __attribute__((packed))
	{
		AlxPca9431_0x04_VRectOvProtIntMask	VRECT_OVP_INT_MSK		: 1;	/* VRECT overvoltage protection interrupt disable:			*/
		AlxPca9431_0x04_VRectOvWarnIntMask	VRECT_OVW_MSK			: 1;	/* VRECT overvoltage warning interrupt disable:				*/
		uint8_t unused_2 : 1;				/* bit 2 is unused																			*/
		AlxPca9431_0x04_VRectRegLowIntMask	VRECT_REGLOW_INT_MSK	: 1;	/* VRECT low voltage regulation trigger interrupt disable:	*/
		AlxPca9431_0x04_VRectRegHighIntMask VRECT_REGHIGH_INT_MSK	: 1;	/* VRECT high voltage regulation trigger interrupt disable:	*/
		AlxPca9431_0x04_VRectGoodIntMask	VRECT_GOOD_INT_MSK		: 1;	/* VRECT voltage good Interrupt disable:					*/
		AlxPca9431_0x04_VRectRdyIntMask		VRECT_RDY_INT_MSK		: 1;	/* VRECT voltage ready Interrupt disable:					*/
		uint8_t unused_7 : 1;				/* bit 7 is unused																			*/
	};
	uint8_t raw;
} AlxPca9431_RegVal_0x04_VRectIntMask;
	

/*******************************************************************************************			PAC943-1_VOutLdoIntReg	0x05	*/
typedef enum
{	/* VOUT short circuit protection Interrupt: */
	VOutLdoShortIntOff	= 0b0, /*	R/C		0: VOUT short protection circuit does not detect VOUT short	*/
	VOutLdoShortIntOn	= 0b1	/*	R/C		1: VOUT short protection circuit detects VOUT short			*/
} AlxPca9431_0x05_VOutLdoShortInt;

typedef union
{
	struct __attribute__((packed))
	{
		uint8_t unused_0_5	: 6;		/* bits 0...5 are unused										*/
		AlxPca9431_0x05_VOutLdoShortInt VOUT_SHORT_INT : 1;	/* VOUT short circuit protection Interrupt:	*/
		uint8_t unused_7	: 1;		/* bit 7 is unused												*/
	};
	uint8_t raw;
} AlxPca9431_RegVal_0x05_VOutLdoInt;

/*******************************************************************************************			PAC943-1_VOutLdoIntMask	0x06	*/
typedef enum
{	/* Analog short circuit protection Interrupt Mask: */
	VOutLdoShortIntMaskOff	= 0b0,	/*	R/W		0: VOUT_SHORT_INT is not masked	*/
	VOutLdoShortIntMaskOn	= 0b1	/*	R/W		1: VOUT_SHORT_INT is masked		*/
} AlxPca9431_0x06_VOutLdoShortIntMask;

typedef union
{
	struct __attribute__((packed))
	{
		uint8_t unused_0_5	: 6;			/* bits 0...5 are unused												*/
		AlxPca9431_0x06_VOutLdoShortIntMask VOUT_SHORT_INT_MASK : 1;	/* VOUT short circuit protection Interrupt:	*/
		uint8_t unused_7	: 1;			/* bit 7 is unused														*/
	};
	uint8_t raw;
} AlxPca9431_RegVal_0x06_VOutLdoIntMask;
	
/*******************************************************************************************			PAC943-1_VOutLdo		0x07	*/
typedef enum
{	/* Voltage of VOUT, when the device is set at LDO mode: */
	VoutLdo3V3	= 0b0,	/*	R/W		0: 3.3V	*/
	VoutLdo5V	= 0b1	/*	R/W		1: 5V	*/

} AlxPca9431_0x07_VOutLdo;

typedef union
{
	struct __attribute__((packed))
	{
		uint8_t unused_0_6 : 7;	/* bits 0...6 are unused												*/
		AlxPca9431_0x07_VOutLdo VOUT_LDO : 1;	/* Voltage of VOUT, when the device is set at LDO mode:	*/
	};
	uint8_t raw;
} AlxPca9431_RegVal_0x07_VOutLdo;
	
/*******************************************************************************************			PAC943-1_VRectThd		0x08	*/
typedef enum
{	/* Set RECT UVLO trip level: */
	VRectThd3V	= 0b00,	/*	R/W		00: 3.0V	*/
	VRectThd3V1 = 0b01,	/*	R/W		01: 3.1V	*/
	VRectThd3V2 = 0b10,	/*	R/W		00: 3.2V	*/
	VRectThd3V3 = 0b11	/*	R/W		01: 3.3V	*/
} AlxPca9431_0x08_VRectThd;

typedef union
{
	struct __attribute__((packed))
	{
		uint8_t unused_0_3 : 4;		/* bits 0...3 are unused						*/
		AlxPca9431_0x08_VRectThd	VRECT_UVLO : 2;	/* Set RECT UVLO trip level:	*/
		uint8_t unused_6_7 : 2;		/* bits 6...7 are unused						*/
	};
	uint8_t raw;
} AlxPca9431_RegVal_0x08_VRectThd;
		
/*******************************************************************************************			PAC943-1_VRectOvw		0x09	*/
typedef enum
{	/* Set VRECT overvoltage warning level: */
	VRectOvw7V		= 0b00, /*	R/W		00: 7.0V	*/
	VRectOvw7V25	= 0b01, /*	R/W		01: 7.25V	*/
	VRectOvw7V5		= 0b10, /*	R/W		00: 7.5V	*/
	VRectOvw7V75	= 0b11	/*	R/W		01: 7.75V	*/

} AlxPca9431_0x09_VRectOvw;

typedef union
{
	struct __attribute__((packed))
	{
		AlxPca9431_0x09_VRectOvw	VRECT_OVW : 2;	/* Set VRECT overvoltage warning level:	*/
		uint8_t unused_2_7 : 6;		/* bits 2...7 are unused								*/
	};
	uint8_t raw;
} AlxPca9431_RegVal_0x09_VRectOvw;
		
/*******************************************************************************************			PAC943-1_TempThd		0x0A	*/
typedef enum
{	/* Thermal shutdown temperature threshold (rising, 20°C hysteresis) setting: */
	TempShutdownThd120C		= 0b000,	/*	R/W		000: 120°C	*/
	TempShutdownThd125C		= 0b001,	/*	R/W		001: 125°C	*/
	TempShutdownThd130C		= 0b010,	/*	R/W		010: 130°C	*/
	TempShutdownThd135C		= 0b011,	/*	R/W		011: 135°C	*/
	TempShutdownThd140C		= 0b100,	/*	R/W		100: 140°C	*/
	TempShutdownThd145C		= 0b101,	/*	R/W		101: 145°C	*/
	TempShutdownThd150C		= 0b110,	/*	R/W		110: 150°C	*/
	TempShutdownThd150C_	= 0b111		/*	R/W		111: 150°C	*/
} AlxPca9431_0x0A_TempShutdownThd;

typedef enum
{ /* Die temperature warning threshold setting: */
	TempWarnThd80C	= 0b00,		/*	R/W		00: 80°C	*/
	TempWarnThd90C	= 0b01,		/*	R/W		01: 90°C	*/
	TempWarnThd100C = 0b10,		/*	R/W		10: 100°C	*/
	TempWarnThd110C = 0b11		/*	R/W		11: 110°C	*/
} AlxPca9431_0x0A_TempWarnThd;
	
typedef union
{
	struct __attribute__((packed))
	{
		AlxPca9431_0x0A_TempShutdownThd TEMP_SHDN	: 3;	/* Thermal shutdown temperature threshold (rising, 20°C hysteresis) setting:	*/
		AlxPca9431_0x0A_TempWarnThd		TEMP_WRN	: 2;	/* Die temperature warning threshold setting:									*/
		uint8_t unused_5_7 : 3;			/* bits 5...7 are unused																		*/
	};
	uint8_t raw;
} AlxPca9431_RegVal_0x0A_TempThd;
	
/*************************************************************************************		PAC943-1_WatchdogSoftStartEnMcu		0x0B	*/
typedef enum
{	/* I2C register bit to its default setting, and meanwhile, this bit is cleared and reset back to “0” as well. */
	I2CSettingDoNothing		= 0b0,	/*	R/C		*/
	I2CSettingDefaultReset	= 0b1	/*	R/C		*/
} AlxPca9431_0x0B_I2CSetting;
	
typedef enum
{	/* Enable VOUT LDO: */
	VOutLdoDisabled = 0b0,		/*	R/W		0: VOUT LDO is disabled		*/
	VOutLdoEnabled	= 0b1		/*	R/W		1: VOUT LDO is enabled		*/
} AlxPca9431_0x0B_VOutLdoEn;
	
typedef enum
{	/* VARACTOR 5-bit control setting bit: */
	VaractorDisabled	= 0b0,	/*	R/W		0: Varactor is off		*/
	VaractorEnabled		= 0b1	/*	R/W		1: Varactor is on		*/
} AlxPca9431_0x0B_VaractorEn;
	
typedef enum
{	/* Exit ECO_Mode bit: */
	EcoNotExit	= 0b0,	/*	R/W		0: The device does not exit ECO_Mode	*/
	EcoExit		= 0b1	/*	R/W		1: The device exits ECO_Mode			*/
} AlxPca9431_0x0B_EcoExit;

typedef enum
{	/* Die temperature warning threshold setting: */
	WatchdogTimerDisabled	= 0b00,	/*	R/W		00: watchdog timer disabled	*/
	WatchdogTimer16s		= 0b01,	/*	R/W		01: 16s						*/
	WatchdogTimer32s		= 0b10,	/*	R/W		10: 32s						*/
	WatchdogTimer64s		= 0b11	/*	R/W		11: 64s						*/
} AlxPca9431_0x0B_WatchdogTimer;
	
typedef union
{
	struct __attribute__((packed))
	{
		AlxPca9431_0x0B_I2CSetting		SW_RST		: 1;	/* I2C register bit to its default setting, and meanwhile, this bit is cleared and reset back to “0” as well.	*/
		AlxPca9431_0x0B_VOutLdoEn		VOUT_LDO_EN : 1;	/* Enable VOUT LDO:																								*/
		AlxPca9431_0x0B_VaractorEn		VARACTOR_EN : 1;	/* VARACTOR 5-bit control setting bit:																			*/
		uint8_t unused_3 : 1;			/* bit 3 is unused																													*/
		AlxPca9431_0x0B_EcoExit			ECO_EXIT	: 1;	/* Exit ECO_Mode bit:																							*/
		uint8_t unused_5 : 1;			/* bit 5 is unused																													*/
		AlxPca9431_0x0B_WatchdogTimer	WATCH_DOG	: 2;	/* Die temperature warning threshold setting:																	*/
	};
	uint8_t raw;
} AlxPca9431_RegVal_0x0B_WatchdogSoftStartEnMcu;
		
/*******************************************************************************************			PAC943-1_VaractorDac	0x0C	*/
typedef union
{
	struct __attribute__((packed))
	{
		uint8_t Varactor_DAC	: 5;/* VARACTOR DAC 5-bit setting, please refer to Section	*/
		uint8_t unused_5_7		: 3;	/* bits 5...7 are unused							*/
	};
	uint8_t raw;
} AlxPca9431_RegVal_0x0C_VaractorDac;
				
/*******************************************************************************************			PAC943-1_AdcControl		0x0D	*/
typedef enum
{	/* ADC averaging enable/disable */
	AdcAveragingDisabled	= 0b0,	/*	R/W		0: disable averaging, one instantaneous measurement	*/
	AdcAveragingEnabled		= 0b1	/*	R/W		1: enable averaging									*/
} AlxPca9431_0x0D_AdcAveraging;
typedef enum
{	/* ADC sample mode control */
	Adc1ShotConversion		= 0b00, /*	R/W		00: 1-shot conversion																				*/
	AdcContinuousConversion = 0b01, /*	R/W		01: continuous conversion																			*/
	AdcBurstMode			= 0b10, /*	R/W		1x: Burst mode, measure all opened channel every 1s Channel control is defined in Section 8.13.3.15	*/
	AdcBurstMode_			= 0b11	/*	R/W		1x: Burst mode, measure all opened channel every 1s Channel control is defined in Section 8.13.3.15	*/
} AlxPca9431_0x0D_AdcRate;
typedef enum
{	/* ADC enable/disable */
	AdcDisabled = 0b0,		/*	R/W		0: disable ADC	*/
	AdcEnabled	= 0b1		/*	R/W		1: enable ADC	*/

} AlxPca9431_0x0D_AdcEn;

typedef union
{
	struct __attribute__((packed))
	{
		uint8_t unused_3	: 1;		/* bit 3 is unused									*/
		AlxPca9431_0x0D_AdcAveraging	ADC_AVG_EN	: 1;	/* ADC averaging enable/disable */
		AlxPca9431_0x0D_AdcRate			ADC_RATE	: 2;	/* ADC sample mode control		*/
		AlxPca9431_0x0D_AdcEn			ADC_EN		: 1;	/* ADC enable/disable			*/
		uint8_t unused_5_7	: 3;		/* bits 5...7 are unused							*/
	};	
	uint8_t raw;
} AlxPca9431_RegVal_0x0D_AdcControl;
			
/*******************************************************************************************			PAC943-1_AdcSampleEn	0x0E	*/
typedef enum
{ /* NTC sampling */
	NtcAdcDisabled	= 0b0,	/*	R/W		0: disable NTC sampling		*/
	NtcAdcEnabled	= 0b1	/*	R/W		1: enable NTC sampling		*/
} AlxPca9431_0x0E_NtcAdcSampling;

typedef enum
{	/* OTP sampling */
	OtpAdcDisabled	= 0b0,	/*	R/W		0: disable OTP sampling		*/
	OtpAdcEnabled	= 0b1	/*	R/W		1: enable OTP sampling		*/
} AlxPca9431_0x0E_OtpAdcSampling;
	
typedef enum
{ /* IOUT sampling */
	IOutAdcDisabled = 0b0,	/*	R/W		0: disable IOUT sampling	*/
	IOutAdcEnabled	= 0b1	/*	R/W		1: enable IOUT sampling		*/
} AlxPca9431_0x0E_IOutAdcSampling;

typedef enum
{ /* VOUT sampling */
	VOutAdcDisabled = 0b0,	/*	R/W		0: disable VOUT sampling	*/
	VOutAdcEnabled	= 0b1	/*	R/W		1: enable VOUT sampling		*/
} AlxPca9431_0x0E_VOutAdcSampling;

typedef enum
{ /* IRECT sampling */
	IRectAdcDisabled	= 0b0,	/*	R/W		0: disable IRECT sampling	*/
	IRectAdcEnabled		= 0b1	/*	R/W		1: enable IRECT sampling	*/
} AlxPca9431_0x0E_IRectAdcSampling;

typedef enum
{ /* VRECT sampling */
	VRectAdcDisabled	= 0b0,	/*	R/W		0: disable VRECT sampling	*/
	VRectAdcEnabled		= 0b1	/*	R/W		1: enable VRECT sampling	*/
} AlxPca9431_0x0E_VRectAdcSampling;

typedef enum
{	/* VTUNE sampling */
	VTuneAdcDisabled	= 0b0,	/*	R/W		0: disable VTUNE sampling	*/
	VTuneAdcEnabled		= 0b1	/*	R/W		1: enable VTUNE sampling	*/
} AlxPca9431_0x0E_VTuneAdcSampling;
	
typedef union
{
	struct __attribute__((packed))
	{
		AlxPca9431_0x0E_NtcAdcSampling		NTC_ADC_EN		: 1;	/* NTC sampling		*/
		AlxPca9431_0x0E_OtpAdcSampling		OTP_ADC_EN		: 1;	/* OTP sampling		*/
		AlxPca9431_0x0E_IOutAdcSampling		IOUT_ADC_EN		: 1;	/* IOUT sampling	*/
		AlxPca9431_0x0E_VOutAdcSampling		VOUT_ADC_EN		: 1;	/* VOUT sampling	*/
		AlxPca9431_0x0E_IRectAdcSampling	IRECT_ADC_EN	: 1;	/* IRECT sampling	*/
		AlxPca9431_0x0E_VRectAdcSampling	VRECT_ADC_EN	: 1;	/* VRECT sampling	*/
		AlxPca9431_0x0E_VTuneAdcSampling	VTUNE_ADC_EN	: 1;	/* VTUNE sampling	*/
		uint8_t unused_7 : 1;				/* bit 7 is unused							*/
	};
	uint8_t raw;
} AlxPca9431_RegVal_0x0E_AdcSampleEn;
				
/*******************************************************************************************			PAC943-1_VPwrLdoConf	0x0F	*/
typedef enum
{	/* VPWR output voltage setting bits: */
	VPwrOut1V8 = 0b00,	/*	R/W		00: VPWR=1.8V	*/
	VPwrOut2V5 = 0b01,	/*	R/W		01: VPWR=2.5V	*/
	VPwrOut2V8 = 0b10,	/*	R/W		10: VPWR=2.8V	*/
	VPwrOut3V3 = 0b11	/*	R/W		11: VPWR=3.3V (Default)	*/
} AlxPca9431_0x0F_VPwrOutSet;

typedef enum
{ /* VPWR enable bit: */
	VPwrLdoOutDisabled	= 0b0,	/*	R/W		0: disable VPWR LDO output	*/
	VPwrLdoOutEnabled	= 0b1	/*	R/W		1: Enable VPWR LDO output	*/
} AlxPca9431_0x0F_VPwrLdoOutConf;


typedef union
{
	struct __attribute__((packed))
	{
		AlxPca9431_0x0F_VPwrOutSet		VPWR_VL : 2;	/* VPWR output voltage setting bits:	*/
		uint8_t unused_2_6 : 5;			/* bits 2...6 are unused								*/
		AlxPca9431_0x0F_VPwrLdoOutConf	VPWR_EN : 1;	/* VPWR enable bit:						*/
	};
	uint8_t raw;
} AlxPca9431_RegVal_0x0F_VPwrLdoConf;
			
/*******************************************************************************************				PAC943-1_Rxir_Conf	0x10	*/
typedef enum
{ /* RXIR pull down resistor on AC1 and AC2 selection bits: */
	RxirPullDown20R		= 0b000,	/*	R/W		000: 20ohm (default)*/
	RxirPullDown80R		= 0b001,	/*	R/W		001: 80ohm			*/
	RxirPullDown140R	= 0b010,	/*	R/W		010: 140ohm			*/
	RxirPullDown200R	= 0b011,	/*	R/W		011: 200ohm			*/
	RxirPullDown260R	= 0b100,	/*	R/W		100: 260ohm			*/
	RxirPullDown320R	= 0b101,	/*	R/W		101: 320ohm			*/
	RxirPullDown380R	= 0b110,	/*	R/W		110: 380ohm			*/
	RxirPullDown440R	= 0b111		/*	R/W		111: 440ohm			*/
} AlxPca9431_0x10_RxirPullDownR;

typedef enum
{	/* RXIR pull down duration selection bits: */
	RxirPullDown0ms2 = 0b000, /*	R/W		000: 0.2ms			*/
	RxirPullDown0ms4 = 0b001, /*	R/W		001: 0.4ms			*/
	RxirPullDown0ms6 = 0b010, /*	R/W		010: 0.6ms			*/
	RxirPullDown0ms8 = 0b011, /*	R/W		011: 0.8ms			*/
	RxirPullDown1ms0 = 0b100, /*	R/W		100: 1.0ms (default)*/
	RxirPullDown1ms2 = 0b101, /*	R/W		101: 1.2ms			*/
	RxirPullDown1ms4 = 0b110, /*	R/W		110: 1.4ms			*/
	RxirPullDown1ms6 = 0b111	 /*	R/W		111: 1.6ms			*/
} AlxPca9431_0x10_RxirPullDownDuration;

typedef enum
{	/* RXIR enable bit: */
	RxirPullDownDisabled	= 0b0,	/*	R/W		0: RXIR function is disabled	*/
	RxirPullDownEnabled		= 0b1	/*	R/W		1: RXIR function is enabled		*/
	/*	The bit is cleared by PCA9431 when RXIR is finished						*/
} AlxPca9431_0x10_RxirPullDownEn;
	
typedef union
{
	struct __attribute__((packed))
	{
		AlxPca9431_0x10_RxirPullDownR			RXIR_STR : 3;	/* RXIR pull down resistor on AC1 and AC2 selection bits:	*/
		AlxPca9431_0x10_RxirPullDownDuration	RXIR_DUR : 3;	/* RXIR pull down duration selection bits:					*/
		uint8_t unused_6 : 1;					/* bit 6 is unused															*/
		AlxPca9431_0x10_RxirPullDownEn			RXIR_EN : 1;	/* VPWR enable bit:											*/
	};
	uint8_t raw;
} AlxPca9431_RegVal_0x10_Rxir_Conf;
				
/*******************************************************************************************			PAC943-1_OpcLdoSetLock	0x20	*/
typedef enum
{	/* VOUT LDO over current protection setting: */
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
	OpcLdoLock = 0x15, /*	R/W		00h: OCPSET_LOCK[4:0] = 10101, VOUTLDO_OCP register can be accessed to perform I2C “write” command. */
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

	/* CHGN_LOCK[4:0] ≠ 10101, VOUTLDO_OCP register can NOT be accessed to perform I2C “write” command. 
	 * In such case when “write” command is performed on these locked registers, it returns the present register value.	*/
} AlxPca9431_0x20_OpcLdoSetLock;

typedef union
{
	struct __attribute__((packed))
	{
		uint8_t unused_0_2 : 3;			/* bits 0...2 are unused											*/
		AlxPca9431_0x20_OpcLdoSetLock	OCPSET_LOCK : 5;	/* VOUT LDO over current protection setting:	*/
	};
	uint8_t raw;
} AlxPca9431_RegVal_0x20_OpcLdoSetLock;
			
/*******************************************************************************************			PAC943-1_OpcLdoSetLock	0x21	*/
typedef enum
{ /* VOUT LDO over current protection setting: */
	VOutLdoSetOCP0mA	= 0x00, /*	R/W		00h: Set 0mA	*/
	VOutLdoSetOCP5mA	= 0x01, /*	R/W		00h: Set 5mA	*/
	VOutLdoSetOCP10mA	= 0x02, /*	R/W		00h: Set 10mA	*/
	VOutLdoSetOCP15mA	= 0x03, /*	R/W		00h: Set 15mA	*/
	VOutLdoSetOCP20mA	= 0x04, /*	R/W		00h: Set 20mA	*/
	VOutLdoSetOCP25mA	= 0x05, /*	R/W		00h: Set 25mA	*/
	VOutLdoSetOCP30mA	= 0x06, /*	R/W		00h: Set 30mA	*/
	VOutLdoSetOCP35mA	= 0x07, /*	R/W		00h: Set 35mA	*/
	VOutLdoSetOCP40mA	= 0x08, /*	R/W		00h: Set 40mA	*/
	VOutLdoSetOCP45mA	= 0x09, /*	R/W		00h: Set 45mA	*/
	VOutLdoSetOCP50mA	= 0x0A, /*	R/W		00h: Set 50mA	*/
	VOutLdoSetOCP55mA	= 0x0B, /*	R/W		00h: Set 55mA	*/
	VOutLdoSetOCP60mA	= 0x0C, /*	R/W		00h: Set 60mA	*/
	VOutLdoSetOCP65mA	= 0x0D, /*	R/W		00h: Set 65mA	*/
	VOutLdoSetOCP70mA	= 0x0E, /*	R/W		00h: Set 70mA	*/
	VOutLdoSetOCP75mA	= 0x0F, /*	R/W		00h: Set 75mA	*/
	VOutLdoSetOCP80mA	= 0x10, /*	R/W		00h: Set 80mA	*/
	VOutLdoSetOCP85mA	= 0x11, /*	R/W		00h: Set 85mA	*/
	VOutLdoSetOCP90mA	= 0x12, /*	R/W		00h: Set 90mA	*/
	VOutLdoSetOCP95mA	= 0x13, /*	R/W		00h: Set 95mA	*/
	VOutLdoSetOCP100mA	= 0x14, /*	R/W		00h: Set 100mA	*/
	VOutLdoSetOCP105mA	= 0x15, /*	R/W		00h: Set 105mA	*/
	VOutLdoSetOCP110mA	= 0x16, /*	R/W		00h: Set 110mA	*/
	VOutLdoSetOCP115mA	= 0x17, /*	R/W		00h: Set 115mA	*/
	VOutLdoSetOCP120mA	= 0x18, /*	R/W		00h: Set 120mA	*/
	VOutLdoSetOCP125mA	= 0x19, /*	R/W		00h: Set 125mA	*/
	VOutLdoSetOCP130mA	= 0x1A, /*	R/W		00h: Set 130mA	*/
	VOutLdoSetOCP135mA	= 0x1B, /*	R/W		00h: Set 135mA	*/
	VOutLdoSetOCP140mA	= 0x1C, /*	R/W		00h: Set 140mA	*/
	VOutLdoSetOCP145mA	= 0x1D, /*	R/W		00h: Set 145mA	*/
	VOutLdoSetOCP150mA	= 0x1E, /*	R/W		00h: Set 150mA	*/
	VOutLdoSetOCP155mA	= 0x1F, /*	R/W		00h: Set 155mA	*/
	VOutLdoSetOCP160mA	= 0x20, /*	R/W		00h: Set 160mA	*/
	VOutLdoSetOCP165mA	= 0x21, /*	R/W		00h: Set 165mA	*/
	VOutLdoSetOCP170mA	= 0x22, /*	R/W		00h: Set 170mA	*/
	VOutLdoSetOCP175mA	= 0x23, /*	R/W		00h: Set 175mA	*/
	VOutLdoSetOCP180mA	= 0x24, /*	R/W		00h: Set 180mA	*/
	VOutLdoSetOCP185mA	= 0x25, /*	R/W		00h: Set 185mA	*/
	VOutLdoSetOCP190mA	= 0x26, /*	R/W		00h: Set 190mA	*/
	VOutLdoSetOCP195mA	= 0x27, /*	R/W		00h: Set 195mA	*/
	VOutLdoSetOCP200mA	= 0x28, /*	R/W		00h: Set 200mA	*/
	VOutLdoSetOCP205mA	= 0x29, /*	R/W		00h: Set 205mA	*/
	VOutLdoSetOCP210mA	= 0x2A, /*	R/W		00h: Set 210mA	*/
	VOutLdoSetOCP215mA	= 0x2B, /*	R/W		00h: Set 215mA	*/
	VOutLdoSetOCP220mA	= 0x2C, /*	R/W		00h: Set 220mA	*/
	VOutLdoSetOCP225mA	= 0x2D, /*	R/W		00h: Set 225mA	*/
	VOutLdoSetOCP230mA	= 0x2E, /*	R/W		00h: Set 230mA	*/
	VOutLdoSetOCP235mA	= 0x2F, /*	R/W		00h: Set 235mA	*/
	VOutLdoSetOCP240mA	= 0x30, /*	R/W		00h: Set 240mA	*/
	VOutLdoSetOCP245mA	= 0x31, /*	R/W		00h: Set 245mA	*/
	VOutLdoSetOCP250mA	= 0x32, /*	R/W		00h: Set 250mA	*/
	VOutLdoSetOCP255mA	= 0x33, /*	R/W		00h: Set 255mA	*/
	VOutLdoSetOCP260mA	= 0x34, /*	R/W		00h: Set 260mA	*/
	VOutLdoSetOCP265mA	= 0x35, /*	R/W		00h: Set 265mA	*/
	VOutLdoSetOCP270mA	= 0x36, /*	R/W		00h: Set 270mA	*/
	VOutLdoSetOCP275mA	= 0x37, /*	R/W		00h: Set 275mA	*/
	VOutLdoSetOCP280mA	= 0x38, /*	R/W		00h: Set 280mA	*/
	VOutLdoSetOCP285mA	= 0x39, /*	R/W		00h: Set 285mA	*/
	VOutLdoSetOCP290mA	= 0x3A, /*	R/W		00h: Set 290mA	*/
	VOutLdoSetOCP295mA	= 0x3B, /*	R/W		00h: Set 295mA	*/
	VOutLdoSetOCP300mA	= 0x3C, /*	R/W		00h: Set 300mA	*/
	VOutLdoSetOCP305mA	= 0x3D, /*	R/W		00h: Set 305mA	*/
	VOutLdoSetOCP310mA	= 0x3E, /*	R/W		00h: Set 310mA	*/
	VOutLdoSetOCP315mA	= 0x3F, /*	R/W		00h: Set 315mA	*/
} AlxPca9431_0x21_VOutLdoSetOCP;

typedef union
{
	struct __attribute__((packed))
	{
		AlxPca9431_0x21_VOutLdoSetOCP	VOUTLDO_OCP : 6;	/* VOUT LDO over current protection setting:	*/
		uint8_t unused_6_7 : 2;			/* bits 6...7 are unused											*/
	};
	uint8_t raw;
} AlxPca9431_RegVal_0x21_VOutLdoSetOCP;
	
/*******************************************************************************************			PAC943-1_VRectAdcH		0x30	*/
typedef union
{
	struct __attribute__((packed))
	{
		uint8_t VRECT : 8;		/* VRectAdc 8...15	VRECT voltage resolution is LSB:15.82 mV	*/
	};
	uint8_t raw;
} AlxPca9431_RegVal_0x30_VRectAdcH;
	
/*******************************************************************************************			PAC943-1_VRectAdcL		0x31	*/
typedef union
{
	struct __attribute__((packed))
	{
		uint8_t unused_0_5 : 6;	/* bits 0...5 are unused										*/
		uint8_t VRECT : 2;		/* VRectAdc 6...7	VRECT voltage resolution is LSB:15.82 mV	*/
	};
	uint8_t raw;
} AlxPca9431_RegVal_0x31_VRectAdcL;
	
/*******************************************************************************************			PAC943-1_VTuneAdcH		0x32	*/
typedef union
{
	struct __attribute__((packed))
	{
		uint8_t VTUNE : 8;		/* VRectAdc 8...15	VTUNE voltage resolution is LSB:3.32 mV	*/
	};
	uint8_t raw;
} AlxPca9431_RegVal_0x32_VTuneAdcH;

/*******************************************************************************************			PAC943-1_VTuneAdcL		0x33	*/
typedef union
{
	struct __attribute__((packed))
	{
		uint8_t unused_0_5 : 6;	/* bits 0...5 are unused									*/
		uint8_t VTUNE : 2;		/* VRectAdc 6...7	VTUNE voltage resolution is LSB:3.32 mV	*/
	};
	uint8_t raw;
} AlxPca9431_RegVal_0x33_VTuneAdcL;
	
/*******************************************************************************************			PAC943-1_VOutAdcH		0x34	*/
typedef union
{
	struct __attribute__((packed))
	{
		uint8_t VOUT : 8;		/* VRectAdc 8...15	VOUT voltage resolution is LSB:5.27 mV	*/
	};
	uint8_t raw;
} AlxPca9431_RegVal_0x34_VOutAdcH;

/*******************************************************************************************			PAC943-1_VOutAdcL		0x35	*/
typedef union
{
	struct __attribute__((packed))
	{
		uint8_t unused_0_5 : 6;	/* bits 0...5 are unused									*/
		uint8_t VOUT : 2;		/* VRectAdc 6...7	VOUT voltage resolution is LSB:5.27 mV	*/
	};
	uint8_t raw;
} AlxPca9431_RegVal_0x35_VOutAdcL;

/*******************************************************************************************			PAC943-1_IOutAdcH		0x36	*/
typedef union
{
	struct __attribute__((packed))
	{
		uint8_t IOUT : 8;		/* VRectAdc 8...15	IOUT voltage resolution is LSB: 421.8uA	*/
	};
	uint8_t raw;
} AlxPca9431_RegVal_0x36_IOutAdcH;

/*******************************************************************************************			PAC943-1_IOutAdcL		0x37	*/
typedef union
{
	struct __attribute__((packed))
	{
		uint8_t unused_0_5 : 6;	/* bits 0...5 are unused									*/
		uint8_t IOUT : 2;		/* VRectAdc 6...7	IOUT voltage resolution is LSB: 421.8uA	*/
	};
	uint8_t raw;
} AlxPca9431_RegVal_0x37_IOutAdcL;
	
/*******************************************************************************************			PAC943-1_IRectAdcH		0x38	*/
typedef union
{
	struct __attribute__((packed))
	{
		uint8_t IRECT : 8;		/* VRectAdc 8...15	IOUT voltage resolution is LSB: 421.8uA	*/
		/*	IRECT current is the sum of VOUT and VPWR load current. In application, 
		 *	the operation current can be added to calculate the received power.				*/
	};
	uint8_t raw;
} AlxPca9431_RegVal_0x38_IRectAdcH;

/*******************************************************************************************			PAC943-1_IRectAdcL		0x39	*/
typedef union
{
	struct __attribute__((packed))
	{
		uint8_t unused_0_5 : 6;		/* bits 0...5 are unused								*/
		uint8_t IRECT : 2;		/* VRectAdc 6...7	IOUT voltage resolution is LSB: 421.8uA	*/
		/*	IRECT current is the sum of VOUT and VPWR load current. In application, 
		 *	the operation current can be added to calculate the received power.				*/
	};
	uint8_t raw;
} AlxPca9431_RegVal_0x39_IRectAdcL;
	
/*******************************************************************************	PAC943-1_AlxPca9431_0x21_ChipTempAdcMeas	0x3A	*/	
typedef enum
{ /* On chip temperature ADC measurement results: */
	ChipTempAdc_43C		= 0x00, 	//-43	  °C
	ChipTempAdc_39C		= 0x01, 	//-39	  °C
	ChipTempAdc_35C		= 0x02, 	//-35	  °C
	ChipTempAdc_30C		= 0x03, 	//-30	  °C
	ChipTempAdc_26C		= 0x04, 	//-26	  °C
	ChipTempAdc_21C		= 0x05, 	//-21	  °C
	ChipTempAdc_17C		= 0x06, 	//-17	  °C
	ChipTempAdc_12C5	= 0x07, 	//-12,5	  °C
	ChipTempAdc_8C		= 0x08, 	//-8	  °C
	ChipTempAdc_4C		= 0x09, 	//-4	  °C
	ChipTempAdc0C		= 0x0A, 	//0		  °C
	ChipTempAdc5C		= 0x0B, 	//5		  °C
	ChipTempAdc9C		= 0x0C, 	//9		  °C
	ChipTempAdc13C5		= 0x0D, 	//13,5	  °C
	ChipTempAdc18C		= 0x0E, 	//18	  °C
	ChipTempAdc22C5		= 0x0F, 	//22,5	  °C
	ChipTempAdc27C		= 0x10, 	//27	  °C
	ChipTempAdc31C		= 0x11, 	//31	  °C
	ChipTempAdc35C		= 0x12, 	//35	  °C
	ChipTempAdc40C		= 0x13, 	//40	  °C
	ChipTempAdc44C		= 0x14, 	//44	  °C
	ChipTempAdc48C		= 0x15, 	//48	  °C
	ChipTempAdc53C		= 0x16, 	//53	  °C
	ChipTempAdc57C		= 0x17, 	//57	  °C
	ChipTempAdc62C		= 0x18, 	//62	  °C
	ChipTempAdc65C5		= 0x19, 	//65,5	  °C
	ChipTempAdc71C		= 0x1A, 	//71	  °C
	ChipTempAdc75C		= 0x1B, 	//75	  °C
	ChipTempAdc79C5		= 0x1C, 	//79,5	  °C
	ChipTempAdc84C		= 0x1D, 	//84	  °C
	ChipTempAdc88C		= 0x1E, 	//88	  °C
	ChipTempAdc92C		= 0x1F, 	//92	  °C
	ChipTempAdc96C		= 0x20, 	//96	  °C
	ChipTempAdc100C5	= 0x21, 	//100,5	  °C
	ChipTempAdc105C		= 0x22, 	//105	  °C
	ChipTempAdc109C		= 0x23, 	//109	  °C
	ChipTempAdc114C		= 0x24, 	//114	  °C
	ChipTempAdc118C		= 0x25, 	//118	  °C
	ChipTempAdc122C		= 0x26, 	//122	  °C
	ChipTempAdc126C		= 0x27, 	//126	  °C
	ChipTempAdc130C		= 0x28, 	//130	  °C
	ChipTempAdc134C5	= 0x29, 	//134,5	  °C
	ChipTempAdc138C		= 0x2A, 	//138	  °C
	ChipTempAdc141C		= 0x2B, 	//141	  °C
	ChipTempAdc145C		= 0x2C, 	//145	  °C
	ChipTempAdc148C		= 0x2D, 	//148	  °C
	ChipTempAdc152C		= 0x2E, 	//152	  °C
	ChipTempAdc156C		= 0x2F, 	//156	  °C
} AlxPca9431_0x3A_ChipTempAdcMeas;

typedef union
{
	struct __attribute__((packed))
	{
		AlxPca9431_0x3A_ChipTempAdcMeas TDIE : 7;	/* On chip temperature ADC measurement results: */
	};
	uint8_t raw;
} AlxPca9431_RegVal_0x3A_ChipTempAdcMeas;

/*******************************************************************************************			PAC943-1_NTCAdcH		0x3B	*/
typedef union
{
	struct __attribute__((packed))
	{
		uint8_t NTC, : 8;		/* NTC voltage resolution is LSB: 2.64mV	*/
	};
	uint8_t raw;
} AlxPca9431_RegVal_0x3B_NTCAdcH;

/*******************************************************************************************			PAC943-1_NTCAdcL		0x3C	*/
typedef union
{
	struct __attribute__((packed))
	{
		uint8_t unused_0_5	: 6;	/* bits 0...5 are unused					*/
		uint8_t NTC	: 2;			/* NTC voltage resolution is LSB: 2.64mV	*/
	};
	uint8_t raw;
} AlxPca9431_RegVal_0x3C_NTCAdcL;
//******************************************************************************
// Register Structures
//******************************************************************************
typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxPca9431_RegVal_0x00_DeviceId val;
} AlxPca9431_Reg_0x00_DeviceId;	

typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxPca9431_RegVal_0x01_SystemInt val;
} AlxPca9431_Reg_0x01_SystemInt;
	
typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxPca9431_RegVal_0x02_SystemIntMask val;
} AlxPca9431_Reg_0x02_SystemIntMask;

typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxPca9431_RegVal_0x03_VRectInt val;
} AlxPca9431_Reg_0x03_VRectInt;

typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxPca9431_RegVal_0x04_VRectIntMask val;
} AlxPca9431_Reg_0x04_VRectIntMask;

typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxPca9431_RegVal_0x05_VOutLdoInt val;
} AlxPca9431_Reg_0x05_VOutLdoInt;
	
typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxPca9431_RegVal_0x06_VOutLdoIntMask val;
} AlxPca9431_Reg_0x06_VOutLdoIntMask;
	
typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxPca9431_RegVal_0x07_VOutLdo val;
} AlxPca9431_Reg_0x07_VOutLdo;

typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxPca9431_RegVal_0x08_VRectThd val;
} AlxPca9431_Reg_0x08_VRectThd;

typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxPca9431_RegVal_0x09_VRectOvw val;
} AlxPca9431_Reg_0x09_VRectOvw;

typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxPca9431_RegVal_0x0A_TempThd val;
} AlxPca9431_Reg_0x0A_TempThd;	
	
typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxPca9431_RegVal_0x0B_WatchdogSoftStartEnMcu val;
} AlxPca9431_Reg_0x0B_WatchdogSoftStartEnMcu;

typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxPca9431_RegVal_0x0C_VaractorDac val;
} AlxPca9431_Reg_0x0C_VaractorDac;

typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxPca9431_RegVal_0x0D_AdcControl val;
} AlxPca9431_Reg_0x0D_AdcControl;

typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxPca9431_RegVal_0x0E_AdcSampleEn val;
} AlxPca9431_Reg_0x0E_AdcSampleEn;	

	typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxPca9431_RegVal_0x0F_VPwrLdoConf val;
} AlxPca9431_Reg_0x0F_VPwrLdoConf;

typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxPca9431_RegVal_0x10_Rxir_Conf val;
} AlxPca9431_Reg_0x10_Rxir_Conf;

typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxPca9431_RegVal_0x20_OpcLdoSetLock val;
} AlxPca9431_Reg_0x20_OpcLdoSetLock;

typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxPca9431_RegVal_0x21_VOutLdoSetOCP val;
} AlxPca9431_Reg_0x21_VOutLdoSetOCP;
	
typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxPca9431_RegVal_0x30_VRectAdcH val;
} AlxPca9431_Reg_0x30_VRectAdcH;	

	typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxPca9431_RegVal_0x31_VRectAdcL val;
} AlxPca9431_Reg_0x31_VRectAdcL;

typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxPca9431_RegVal_0x32_VTuneAdcH val;
} AlxPca9431_Reg_0x32_VTuneAdcH;

typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxPca9431_RegVal_0x33_VTuneAdcL val;
} AlxPca9431_Reg_0x33_VTuneAdcL;

typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxPca9431_RegVal_0x34_VOutAdcH val;
} AlxPca9431_Reg_0x34_VOutAdcH;

	typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxPca9431_RegVal_0x35_VOutAdcL val;
} AlxPca9431_Reg_0x35_VOutAdcL;	

	typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxPca9431_RegVal_0x36_IOutAdcH val;
} AlxPca9431_Reg_0x36_IOutAdcH;

typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxPca9431_RegVal_0x37_IOutAdcL val;
} AlxPca9431_Reg_0x37_IOutAdcL;

typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxPca9431_RegVal_0x38_IRectAdcH val;
} AlxPca9431_Reg_0x38_IRectAdcH;

typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxPca9431_RegVal_0x39_IRectAdcL val;
} AlxPca9431_Reg_0x39_IRectAdcL;
	
typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxPca9431_RegVal_0x3A_ChipTempAdcMeas val;
} AlxPca9431_Reg_0x3A_ChipTempAdcMeas;

typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxPca9431_RegVal_0x3B_NTCAdcH val;
} AlxPca9431_Reg_0x3B_NTCAdcH;

typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxPca9431_RegVal_0x3C_NTCAdcL val;
} AlxPca9431_Reg_0x3C_NTCAdcL;
//******************************************************************************
// Main Register Structure
//******************************************************************************
typedef struct
{
	AlxPca9431_Reg_0x00_DeviceId					_0x00_Device_ID			;
	AlxPca9431_Reg_0x01_SystemInt					_0x01_SYSTEM_INT		;
	AlxPca9431_Reg_0x02_SystemIntMask				_0x02_SYSTEM_INT_MASK	;
	AlxPca9431_Reg_0x03_VRectInt					_0x03_VRECT_INT			;
	AlxPca9431_Reg_0x04_VRectIntMask				_0x04_VRECT_INT_MASK	;
	AlxPca9431_Reg_0x05_VOutLdoInt					_0x05_VOUTLDO_INT		;
	AlxPca9431_Reg_0x06_VOutLdoIntMask				_0x06_VOUTLDO_INT_MASK	;
	AlxPca9431_Reg_0x07_VOutLdo						_0x07_VOUT_LDO			;
	AlxPca9431_Reg_0x08_VRectThd					_0x08_VRECT_THD			;
	AlxPca9431_Reg_0x09_VRectOvw					_0x09_VRECT_OVW			;
	AlxPca9431_Reg_0x0A_TempThd						_0x0A_TEMP_THD			;
	AlxPca9431_Reg_0x0B_WatchdogSoftStartEnMcu		_0x0B_WD_EN_RST			;
	AlxPca9431_Reg_0x0C_VaractorDac					_0x0C_Varactor_DAC		;
	AlxPca9431_Reg_0x0D_AdcControl					_0x0D_ADC_CONTROL		;
	AlxPca9431_Reg_0x0E_AdcSampleEn					_0x0E_Sample_EN			;
	AlxPca9431_Reg_0x0F_VPwrLdoConf					_0x0F_VPWR_CONFIG		;
	AlxPca9431_Reg_0x10_Rxir_Conf					_0x10_RXIR_CONFIG		;
	AlxPca9431_Reg_0x20_OpcLdoSetLock				_0x20_OCPSET_LOCK		;
	AlxPca9431_Reg_0x21_VOutLdoSetOCP				_0x21_VOUTLDO_OCP		;	
	AlxPca9431_Reg_0x30_VRectAdcH					_0x30_VRECT_ADC_H		;
	AlxPca9431_Reg_0x31_VRectAdcL					_0x31_VRECT_ADC_L		;
	AlxPca9431_Reg_0x32_VTuneAdcH					_0x32_VTUNE_ADC_H		;
	AlxPca9431_Reg_0x33_VTuneAdcL					_0x33_VTUNE_ADC_L		;
	AlxPca9431_Reg_0x34_VOutAdcH					_0x34_VOUT_ADC_H		;
	AlxPca9431_Reg_0x35_VOutAdcL					_0x35_VOUT_ADC_L		;
	AlxPca9431_Reg_0x36_IOutAdcH					_0x36_IOUT_ADC_H		;
	AlxPca9431_Reg_0x37_IOutAdcL					_0x37_IOUT_ADC_L		;
	AlxPca9431_Reg_0x38_IRectAdcH					_0x38_IRECT_ADC_H		;
	AlxPca9431_Reg_0x39_IRectAdcL					_0x39_IRECT_ADC_L		;
	AlxPca9431_Reg_0x3A_ChipTempAdcMeas				_0x3A_TDIE_ADC			;
	AlxPca9431_Reg_0x3B_NTCAdcH						_0x3B_NTC_ADC_H			;
	AlxPca9431_Reg_0x3C_NTCAdcL						_0x3C_NTC_ADC_L			;
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
	
	
//Alx_Status AlxPca9431_LdoVout_SetVoltage_V(AlxPca9431* me, AlxPca9431_0x07_VOutLdo VoutLdoxV); // 3,3V or 5V => write:  VoutLdo3V3 or. VoutLdo5V
Alx_Status AlxPca9431_LdoVout_GetVoltage_V(AlxPca9431* me, float* voltage_V); // 10 bit ADC
Alx_Status AlxPca9431_LdoVout_GetCurrent_A(AlxPca9431* me, float* current_A); // 10 bit ADC
//Alx_Status AlxPca9431_LdoVPwr_SetVoltage_V(AlxPca9431* me, AlxPca9431_0x0F_VPwrOutSet VPwrOutxVx); // 1.8V, 2.5V, 2.8V, 3.3V  => write:  VPwrOut1V8 or. VPwrOut2V5 or. VPwrOut2V8 or. VPwrOut3V3
	
	
//Alx_Status AlxPca9431_Rect_Uvlo_SetVoltage_V(AlxPca9431* me, AlxPca9431_0x08_VRectThd VRectThd3Vx); // 3V, 3.1V, 3.2V, 3.3V  => write: VRectThd3V or. VRectThd3V1 or. VRectThd3V2 or. VRectThd3V3
//Alx_Status AlxPca9431_Rect_Ovw_SetVoltage_V(AlxPca9431* me, AlxPca9431_0x09_VRectOvw VRectThd7Vx); // 7V, 7.25V, 7.5V, 7.75V  => write: VRectThd7V or. VRectThd7V25 or. VRectThd7V5 or. VRectThd7V75
Alx_Status AlxPca9431_Rect_GetVoltage_V(AlxPca9431* me, float* voltage_V); // 10 bit ADC
Alx_Status AlxPca9431_Rect_GetCurrent_A(AlxPca9431* me, float* current_A); // 10 bit ADC

	
//Alx_Status AlxPca9431_TempSens_SetWarning_C(AlxPca9431* me, AlxPca9431_0x0A_TempWarnThd TempWarnThdx0C); // 80°C, 90°C ... 110°C =>  write: TempWarnThd80C or. ... or. TempWarnThd110C
//Alx_Status AlxPca9431_TempSens_SetShutdown_C(AlxPca9431* me, AlxPca9431_0x0A_TempShutdownThd TempShutdownThd1xxC); // 120°C, 125°C ... 150°C =>  write: TempShutdownThd120C or. ... or. TempShutdownThd150C
Alx_Status AlxPca9431_TempSens_GetTemp_degC(AlxPca9431* me, float* temp_degC); // On chip temperature (-43°C ... +156°C)
	
Alx_Status AlxPca9431_VTune_SetVoltage_V(AlxPca9431* me, float* voltage_V); // 0-3.3V 5 bit DAC - TODO
Alx_Status AlxPca9431_VTune_GetVoltage_V(AlxPca9431* me, float* voltage_V); // 10 bit ADC - TODO


//Alx_Status AlxPca9431_Rxir_SetDuration_s(AlxPca9431* me, AlxPca9431_0x10_RxirPullDownDuration RxirPullDownxmsx); // 0.2s, 0.4s, ... , 1.6s =>  write: RxirPullDown0ms2 or. ... or. RxirPullDown1ms6
//Alx_Status AlxPca9431_Rxir_SetResistance_ohm(AlxPca9431* me, AlxPca9431_0x10_RxirPullDownR RxirPullDownx0R); // 20ohm, 80ohm, ... , 440ohm =>  write: RxirPullDown20R or. ... or. RxirPullDown440R	
	

	
Alx_Status AlxPca9431_Ntc_GetTemp_degC(AlxPca9431* me, float* temp_degC); // external NTC voltage / battery temperature measurement - TODO

	
Alx_Status AlxPca9431_Exit_EcoMode(AlxPca9431* me);

Alx_Status AlxPca9431_Reg_ReadAndClearInterrupt(AlxPca9431* me);
	
bool AlxPca9431_IsSleep(AlxPca9431* me);
	
#ifdef __cplusplus
}
#endif

#endif // ALX_ADAU1961_H
