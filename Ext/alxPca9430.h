/**
  ******************************************************************************
  * @file alxPca9431.h
  * @brief Auralix C Library - ALX opis PCA9431 Module
  * @version $LastChangedRevision: 5305 $
  * @date $LastChangedDate: 2021-06-01 19:13:59 +0200 (Tue, 01 Jun 2021) $
  ******************************************************************************
  */

#ifndef ALX_PCA9430_H
#define ALX_PCA9430_H

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
#define ALX_PCA9430_FILE "alxPca9430"
	
// Assert //
#if defined(_ALX_PCA9430_ASSERT_BKPT) || defined(_ALX_ASSERT_BKPT_ALL)
#define ALX_PCA9430_ASSERT(expr) ALX_ASSERT_BKPT(ALX_PCA9430_FILE, expr)
#elif defined(_ALX_PCA9430_ASSERT_TRACE) || defined(_ALX_ASSERT_TRACE_ALL)
#define ALX_PCA9430_ASSERT(expr) ALX_ASSERT_TRACE(ALX_PCA9430_FILE, expr)
#elif defined(_ALX_PCA9430_ASSERT_RST) || defined(_ALX_ASSERT_RST_ALL)
#define ALX_PCA9430_ASSERT(expr) ALX_ASSERT_RST(ALX_PCA9430_FILE, expr)
#else
#define ALX_PCA9430_ASSERT(expr) do{} while (false)
#endif

// Trace //
#if defined(_ALX_PCA9430_TRACE) || defined(_ALX_TRACE_ALL)
#define ALX_PCA9430_TRACE(...) ALX_TRACE_STD(ALX_PCA9430_FILE, __VA_ARGS__)
#else
#define ALX_PCA9430_TRACE(...) do{} while (false)
#endif


//******************************************************************************
// Register Values Unions
//******************************************************************************
// General Enums //

/*******************************************************************************************				PAC9430_DeviceId	0x00	*/
typedef union
{
	struct __attribute__((packed))
	{
		uint8_t Revision : 3;		/* Revision number						*/
		uint8_t ID : 4;				/* Device ID							*/
		uint8_t unused_7 : 1;
	};
	uint8_t raw;
} AlxPca9430_RegVal_0x00_DeviceId;
	
/*******************************************************************************************				PAC943-x_SystemInt	0x01	*/
typedef enum
{	/* Thermal shutdown interrupt: */
	TempShutdownIntOff	= 0b0,		/*	R/C		0:Thermal shutdown interrupt: 0: thermal shutdown is not triggered	*/
	TempShutdownIntOn	= 0b1		/*	R/C		1: die temp ≥ TSTDN (set in THM_SHDN[2:0], thermal shutdown is triggered */
} AlxPca9430_0x01_TempShutdownInt;

typedef enum
{
	/* Die temp pre - warning interrupt */
	TempWarnIntOff		= 0b0,		/*	R/C		0: die temp < TWRN		*/
	TempWarnIntOn		= 0b1		/*	R/C		1: die temp ≥ TWRN; 	*/
	/*	TWRN threshold is configured by TEMP_WRN[1:0]	*/
} AlxPca9430_0x01_TempWarnInt;

typedef enum
{
	/* Watchdog Timer Expiration Interrupt: */
	WdTimerExpirationIntOff	= 0b0, /*	R/C		0: The watchdog timer expiration has not happened since the last time this bit was cleared. */
	WdTimerExpirationIntOn		= 0b1		/*	R/C		1: The watchdog timer expiration has happened since the last time this bit was cleared. 	*/
	} AlxPca9430_0x01_WdTimerExpInt;
	
typedef enum
{
	/* Watchdog Timer Expiration Interrupt: */
	VpwrScpIntOff		= 0b0,		/*	R/C		0: The VPWR short circuit protection is not triggered	*/
	VpwrScpIntOn		= 0b1		/*	R/C		1: The VPWR short circuit protection triggered 			*/
} AlxPca9430_0x01_VpwrScpInt;
	
typedef union
{
	struct __attribute__((packed))
	{
		AlxPca9430_0x01_TempShutdownInt Temp_SHD_INT : 1;	/* Thermal shutdown interrupt:				*/
		AlxPca9430_0x01_TempWarnInt		Temp_WRN_INT : 1;	/* Die temp pre - warning interrupt			*/
		AlxPca9430_0x01_WdTimerExpInt	WD_TIMER_INT : 1;	/* Watchdog Timer Expiration Interrupt:		*/
		AlxPca9430_0x01_VpwrScpInt		VPWR_SCP_INT : 1;	/* VPWR short circuit protection Interrupt: */
		uint8_t unused_4_7 : 4;			/* bits 4..7 are unused						*/
	};
	uint8_t raw;
} AlxPca9430_RegVal_0x01_SystemInt;

/*******************************************************************************************			PAC943-x_SystemIntMask	0x02	*/
typedef enum
{ /* Temperature shut down interrupt disable: */
	Temp_ShutdownIntMaskOff	= 0b0,		/*	R/W		0: Temp_SHD_INT can trigger INT		*/
	Temp_ShutdownIntMaskOn	= 0b1		/*	R/W		1: Temp_SHD_INT trigger disabled	*/
} AlxPca9430_0x02_TempShutdownIntMask;

typedef enum
{	/* Temperature warning interrupt disable: */
	TempWarnIntMaskOff		= 0b0,		/*	R/W		0: Temp_WRN_INT can trigger INT		*/
	TempWarnIntMaskOn		= 0b1		/*	R/W		1: Temp_WRN_INT trigger disabled 	*/
	/*	TWRN threshold is configured by TEMP_WRN[1:0]	*/
} AlxPca9430_0x02_TempWarnIntMask;

typedef enum
{	/* Watchdog timer interrupt disable: */
	WdTimerExpIntMaskOff	= 0b0,		/*	R/W		0: WD_TIMER_INT can trigger INT		*/
	WdTimerExpIntMaskOn		= 0b1		/*	R/W		1: WD_TIMER_INT trigger disabled 	*/
} AlxPca9430_0x02_WdTimerExpIntMask;

typedef enum
{	/* VRECT voltage low interrupt disable: */
	VpwrScpIntMaskOff		= 0b0,		/*	R/W		0: VPWR_SCP_INT can trigger INT		*/
	VpwrScpIntMaskOn		= 0b1		/*	R/W		1: VPWR_SCP_INT trigger disabled 	*/
} AlxPca9430_0x02_VpwrScpIntMask;

typedef union
{
	struct __attribute__((packed))
	{
		AlxPca9430_0x02_TempShutdownIntMask Temp_SHD_INT_MSK : 1;	/* Temperature shut down interrupt disable:		*/
		AlxPca9430_0x02_TempWarnIntMask		Temp_WRN_INT_MSK : 1;	/* Temperature warning interrupt disable:		*/
		AlxPca9430_0x02_WdTimerExpIntMask	WD_TIMER_INT_MSK : 1;	/* Watchdog timer interrupt disable:			*/
		AlxPca9430_0x02_VpwrScpIntMask		VPWR_SCP_INT_MSK : 1;	/* VRECT voltage low interrupt disable:			*/
		uint8_t unused_4_7 : 4;				/* bits 4..7 are unused							*/
	};
	uint8_t raw;
} AlxPca9430_RegVal_0x02_SystemIntMask;

/*******************************************************************************************				PAC943-0_VRectInt	0x03	*/
typedef enum
{	/* VRECT overvoltage protection interrupt */
	VRectOvProtIntOff	= 0b0,		/*	R/C		0: VRECT overvoltage protection not triggered								*/
	VRectOvProtIntOn	= 0b1		/*	R/C		1: VRECT >OVPRECT (set in RECT_OVP[2:0]), overvoltage protection triggered	*/
} AlxPca9430_0x03_VRectOvProtInt;

typedef enum
{	/* VRECT pre-charging overvoltage warning interrupt	*/
	VRectOverVoltagePreChargingIntOff	 = 0b0,		/*	R/C		0: VRECT< VRECT_OVWPRE	*/
	VRectOverVoltagePreChargingIntOn	 = 0b1		/*	R/C		1: VRECT≥ VRECT_OVWPRE	*/
} AlxPca9430_0x03_VRectOverVoltagePreChargingInt;
	
typedef enum
{ /* Fast charging overvoltage warning interrupt		*/
	VRectFastChargingIntOff = 0b0,		/*	R/C		0: VRECT fast charging overvoltage warning not triggered	*/
	VRectFastChargingIntOn	= 0b1		/*	R/C		1: VRECT fast charging overvoltage warning triggered		*/
} AlxPca9430_0x03_VRectFastChargingInt;

typedef enum
{	/* VRECT low voltage regulation trigger interrupt */
	VRectRegLowIntOff	= 0b0,		/*	R/C		0: VRECT low voltage regulation not triggered	*/
	VRectRegLowIntOn	= 0b1		/*	R/C		1: VRECT low voltage regulation triggered		*/
} AlxPca9430_0x03_VRectRegLowInt;

typedef enum
{	/* VRECT high voltage regulation trigger interrupt */
	VRectRegHighIntOff	= 0b0,		/*	R/C		0: VRECT high voltage regulation not triggered	*/
	VRectRegHighIntOn	= 0b1		/*	R/C		1: VRECT high voltage regulation triggered		*/
} AlxPca9430_0x03_VRectRegHighInt;

typedef enum
{	/* VRECT voltage good Interrupt: */
	VRectGoodIntOff		= 0b0,		/*	R/C		0: The VRECT voltage is not in the range of VUVLO <VRECT < VOVP	*/
	VRectGoodIntOn		= 0b1		/*	R/C		1: The VRECT voltage is in the range of VUVLO <VRECT < VOVP		*/
} AlxPca9430_0x03_VRectGoodInt;

typedef enum
{
	/* VRECT voltage ready Interrupt: */
	VRectRdyIntOff		= 0b0,		/*	R/C		0: The VRECT voltage is less than VUVLO		*/
	VRectRdyIntOn		= 0b1		/*	R/C		1: The VRECT voltage is larger than VUVLO	*/
} AlxPca9430_0x03_VRectRdyInt;
	
typedef union
{
	struct __attribute__((packed))
	{
		AlxPca9430_0x03_VRectOvProtInt					VRECT_OVP_INT		: 1;	/* VRECT overvoltage protection interrupt			*/
		AlxPca9430_0x03_VRectOverVoltagePreChargingInt	VRECT_OVWPRE_INT	: 1;	/* VRECT pre-charging overvoltage warning interrupt	*/
		AlxPca9430_0x03_VRectFastChargingInt			VRECT_OVWFAST_INT	: 1;	/* Fast charging overvoltage warning interrupt		*/
		AlxPca9430_0x03_VRectRegLowInt					VRECT_REGLOW_INT	: 1;	/* VRECT low voltage regulation trigger interrupt	*/
		AlxPca9430_0x03_VRectRegHighInt					VRECT_REGHIGH_INT	: 1;	/* VRECT high voltage regulation trigger interrupt	*/
		AlxPca9430_0x03_VRectGoodInt					VRECT_GOOD_INT		: 1;	/* VRECT voltage good Interrupt:					*/
		AlxPca9430_0x03_VRectRdyInt						VRECT_RDY_INT		: 1;	/* VRECT voltage ready Interrupt:					*/
		uint8_t unused_7 : 1;						/* bit 7 is unused									*/
	};
	uint8_t raw;
} AlxPca9430_RegVal_0x03_VRectInt;
	
/*******************************************************************************************			PAC943-0_VRectIntMask	0x04	*/
typedef enum
{ /* Overvoltage protection interrupt disable: */
	VRectOvProtIntMaskOff = 0b0,						/*	R/W		0: VRECT_OVP_INT can trigger INT	*/
	VRectOvProtIntMaskOn = 0b1							/*	R/W		1: VRECT_OVP_INT trigger disabled	*/
} AlxPca9430_0x04_VRectOvProtIntMask;

typedef enum
{ /* Pre-charging overvoltage warning interrupt disable: */
	VRectOverVoltagePreChargingIntMaskOff = 0b0,		/*	R/W		0: VRECT_OVWRN_INT can trigger INT	*/
	VRectOverVoltagePreChargingIntMaskOn = 0b1			/*	R/W		1: VRECT_OVWRN_INT trigger disabled	*/
} AlxPca9430_0x04_VRectOverVoltagePreChargingIntMask;
	
typedef enum
{	/* Fast charging overvoltage warning interrupt disable:		*/
	VRectFastChargingIntMaskOff = 0b0,					/*	R/C		0: VRECT_OVWRN_INT can trigger INT	*/
	VRectFastChargingIntMaskOn	= 0b1					/*	R/C		1: VRECT_OVWRN_INT trigger disabled		*/
} AlxPca9430_0x04_VRectFastChargingInt;

typedef enum
{ /* VRECT low voltage regulation trigger interrupt disable:*/
	VRectRegLowIntMaskOff	= 0b0,	/*	R/W		0: VRECT_OVWRN_INT can trigger INT	*/
	VRectRegLowIntMaskOn	= 0b1	/*	R/W		1: VRECT_OVWRN_INT trigger disabled	*/
} AlxPca9430_0x04_VRectRegLowIntMask;

typedef enum
{ /* VRECT high voltage regulation trigger interrupt disable:*/
	VRectRegHighIntMaskOff	= 0b0,	/*	R/W		0: VRECT_REGHIGH_INT can trigger INT	*/
	VRectRegHighIntMaskOn	= 0b1	/*	R/W		1: VRECT_REGHIGH_INT trigger disabled	*/
} AlxPca9430_0x04_VRectRegHighIntMask;

typedef enum
{ /* VRECT voltage good interrupt disable: */
	VRectGoodIntMaskOff = 0b0,		/*	R/W		0: VRECT_GOOD_INT can trigger INT	*/
	VRectGoodIntMaskOn	= 0b1		/*	R/W		1: VRECT_GOOD_INT trigger disabled	*/
} AlxPca9430_0x04_VRectGoodIntMask;

typedef enum
{	/* VRECT voltage ready interrupt disable: */
	VRectRdyIntMaskOff	= 0b0,		/*	R/W		0: VRECT_RDY_INT can trigger INT	*/
	VRectRdyIntMaskOn	= 0b1		/*	R/W		1: VRECT_RDY_INT trigger disabled	*/
} AlxPca9430_0x04_VRectRdyIntMask;
	
typedef union
{
	struct __attribute__((packed))
	{
		AlxPca9430_0x04_VRectOvProtIntMask					VRECT_OVP_INT_MSK		: 1;	/* VRECT overvoltage protection interrupt disable:			*/
		AlxPca9430_0x04_VRectOverVoltagePreChargingIntMask	VRECT_OVWPRE_MSK		: 1;	/* Pre-charging overvoltage warning interrupt disable:		*/
		AlxPca9430_0x03_VRectFastChargingInt				VRECT_OVWFAST_INT_MSK	: 1;	/* Fast charging overvoltage warning interrupt disable:	*/
		AlxPca9430_0x04_VRectRegLowIntMask					VRECT_REGLOW_INT_MSK	: 1;	/* VRECT low voltage regulation trigger interrupt disable:	*/
		AlxPca9430_0x04_VRectRegHighIntMask					VRECT_REGHIGH_INT_MSK	: 1;	/* VRECT high voltage regulation trigger interrupt disable:	*/
		AlxPca9430_0x04_VRectGoodIntMask					VRECT_GOOD_INT_MSK		: 1;	/* VRECT voltage good Interrupt disable:					*/
		AlxPca9430_0x04_VRectRdyIntMask						VRECT_RDY_INT_MSK		: 1;	/* VRECT voltage ready Interrupt disable:					*/
		uint8_t unused_7 : 1;							/* bit 7 is unused											*/
	};
	uint8_t raw;
} AlxPca9430_RegVal_0x04_VRectIntMask;
	
/*******************************************************************************************			PAC943-0_BatteryChargerInterrupt	0x05	*/
typedef enum
{	/* Charger Status Interrupt: */
	ChargerOkStatusIntOff	= 0b0,	/*	R/C		0: The CHG_OK bit has not changed since the last time this bit was cleared.	*/
	ChargerOkStatusIntOn	= 0b1	/*	R/C		1: The CHG_OK bit has changed since the last time this bit was cleared		*/
} AlxPca9430_0x05_ChargerOkStatusInt;
	
typedef enum
{	/* Battery Interrupt: */
	BatteryIntOff	= 0b0,			/*	R/C		0: The VBAT_OK bit has not changed since the last time this bit was cleared.	*/
	BatteryIntOn	= 0b1			/*	R/C		1: The VBAT_OK bit has changed since the last time this bit was cleared.		*/
} AlxPca9430_0x05_BatteryInt;

typedef enum
{	/* Battery presence Interrupt: */
	BatteryDetectIntOff	= 0b0,		/*	R/C		0: The VBAT_DET_OK bit has not changed since the last time this bit was cleared.	*/
	BatteryDetectIntOn	= 0b1		/*	R/C		1: The VBAT_DET_OK bit has changed since the last time this bit was cleared.		*/
} AlxPca9430_0x05_BatteryDetectInt;
	
typedef enum
{	/* Pre-qualification Charging Timer Expiration Interrupt: */
	PreQualificationChargingTimerIntOff		= 0b0,	/*	R/C		0: The pre-qual charging timer expiration has not happened since the last time this bit was cleared.	*/
	PreQualificationChargingTimerIntIntOn	= 0b1	/*	R/C		1: The pre-qual charging timer expiration has happened since the last time this bit was cleared.		*/
} AlxPca9430_0x05_PreQualificationChargingTimerInt;

typedef enum
{	/* Fast Charging Timer Expiration Interrupt: */
	FastChargingTimerIntOff		= 0b0,				/*	R/C		0: The fast charging timer expiration has not happened since the last time this bit was cleared.	*/
	FastChargingTimerIntIntOn	= 0b1				/*	R/C		1: The fast charging timer expiration has happened since the last time this bit was cleared.		*/
} AlxPca9430_0x05_FastChargingTimerInt;
	
typedef enum
{	/* Battery short detection timer Expiration Interrupt: */
	DeadBatteryDetectionTimerIntOff	= 0b0,			/*	R/C		0: The battery short detection timer expiration has not happened since the last time this bit was cleared. Battery is not short	*/
	DeadBatteryDetectionTimerIntOn	= 0b1			/*	R/C		1: The battery short detection timer expiration has happened since the last time this bit was cleared. Battery is short			*/
} AlxPca9430_0x05_DeadBatteryDetectionTimerInt;
	
typedef enum
{ /* Analog short circuit protection Interrupt: */
	VOutChargerShortIntOff	= 0b0,					/*	R/C		0: Analog short protection circuit does not detect VOUT short	*/
	VOutChargerShortIntOn	= 0b1					/*	R/C		1: Analog short protection circuit detects VOUT short			*/
} AlxPca9430_0x05_VOutChargerShortInt;
	
typedef union
{
	struct __attribute__((packed))
	{
		AlxPca9430_0x05_ChargerOkStatusInt					CHG_OK_INT			: 1;	/* Charger Status Interrupt:								*/
		AlxPca9430_0x05_BatteryInt							VBAT_INT			: 1;	/* Battery Interrupt:										*/
		AlxPca9430_0x05_BatteryDetectInt					VBAT_DET_INT		: 1;	/* Battery presence Interrupt:								*/
		AlxPca9430_0x05_PreQualificationChargingTimerInt	ICHG_PREQ_TIMER_INT	: 1;	/* Pre-qualification Charging Timer Expiration Interrupt:	*/
		AlxPca9430_0x05_FastChargingTimerInt				ICHG_FAST_TIMER_INT	: 1;	/* Fast Charging Timer Expiration Interrupt:				*/
		AlxPca9430_0x05_DeadBatteryDetectionTimerInt		ICHG_DEAD_TIMER_INT	: 1;	/* Battery short detection timer Expiration Interrupt:		*/
		AlxPca9430_0x05_VOutChargerShortInt					VOUT_SHORT_INT		: 1;	/* Analog short circuit protection Interrupt:				*/
		uint8_t unused_7 : 1;								/* bit 7 is unused																		*/
	};
	uint8_t raw;
} AlxPca9430_RegVal_0x05_BatteryChargerInt;

/*******************************************************************************************			PAC943-0_BatteryChargerInterruptMask	0x06	*/
typedef enum
{ /* Charger Status Interrupt Mask: */
	ChargerOkStatusIntMaskOff = 0b0,		/*	R/C		0: CHG_OK_INT is not masked		*/
	ChargerOkStatusIntMaskOn = 0b1			/*	R/C		1 : CHG_OK_INT is masked		*/
} AlxPca9430_0x06_ChargerOkStatusIntMask;

typedef enum
{ /* Battery Interrupt Mask: */
	BatteryIntMaskOff	= 0b0,			/*	R/C		0: VBAT_INT is not masked	*/
	BatteryIntMaskOn	= 0b1			/*	R/C		1: VBAT_INT is masked		*/
} AlxPca9430_0x06_BatteryIntMask;

typedef enum
{ /* Battery presence Interrupt Mask: */
	BatteryDetectIntMaskOff = 0b0,			/*	R/C		0: VBAT_DET_INT is not masked	*/
	BatteryDetectIntMaskOn	= 0b1			/*	R/C		1: VBAT_DET_INT is masked		*/
} AlxPca9430_0x06_BatteryDetectIntMask;

typedef enum
{ /* Pre-qualification Charging Timer Expiration Interrupt Mask: */
	PreQualificationChargingTimerIntMaskOff = 0b0,	/*	R/C		0: ICHG_PREQ_TIMER_INT is not masked	*/
	PreQualificationChargingTimerIntMaskOn	= 0b1	/*	R/C		1: ICHG_PREQ_TIMER_INT is masked		*/
} AlxPca9430_0x06_PreQualificationChargingTimerIntMask;

typedef enum
{ /* Fast Charging Timer Expiration Interrupt Mask: */
	FastChargingTimerIntMaskOff		= 0b0,		/*	R/C		0: ICHG_FAST_TIMER_INT is not masked	*/
	FastChargingTimerIntMaskOn		= 0b1		/*	R/C		1: ICHG_FAST_TIMER_INT is masked		*/
} AlxPca9430_0x06_FastChargingTimerIntMask;

typedef enum
{ /* Battery short detection timer Expiration Interrupt Mask: */
	DeadBatteryDetectionTimerIntMaskOff = 0b0,	/*	R/C		0: ICHG_DEAD_TIMER_INT is not masked	*/
	DeadBatteryDetectionTimerIntMaskOn	= 0b1	/*	R/C		1: ICHG_DEAD_TIMER_INT is masked		*/
} AlxPca9430_0x06_DeadBatteryDetectionTimerIntMask;

typedef enum
{	/* Analog short circuit protection Interrupt Mask: */
	VOutChargerShortIntMaskOff	= 0b0,			/*	R/C		0: VOUT_SHORT_INT is not masked	*/
	VOutChargerShortIntMaskOn	= 0b1			/*	R/C		1: VOUT_SHORT_INT is masked		*/
} AlxPca9430_0x06_VOutChargerShortIntMask;

typedef union
{
	struct __attribute__((packed))
	{
		AlxPca9430_0x06_ChargerOkStatusIntMask					CHG_OK_INT_MASK			: 1;	/* Charger Status Interrupt Mask:								*/
		AlxPca9430_0x06_BatteryIntMask							VBAT_INT_MASK			: 1;	/* Battery Interrupt Mask:										*/
		AlxPca9430_0x06_BatteryDetectIntMask					VBAT_DET_INT_MASK		: 1;	/* Battery presence Interrupt Mask:								*/
		AlxPca9430_0x06_PreQualificationChargingTimerIntMask	ICHG_PREQ_TIMER_INT_MASK: 1;	/* Pre-qualification Charging Timer Expiration Interrupt Mask:	*/
		AlxPca9430_0x06_FastChargingTimerIntMask				ICHG_FAST_TIMER_INT_MASK: 1;	/* Fast Charging Timer Expiration Interrupt Mask:				*/
		AlxPca9430_0x06_DeadBatteryDetectionTimerIntMask		ICHG_DEAD_TIMER_INT_MASK: 1;	/* Battery short detection timer Expiration Interrupt Mask:		*/
		AlxPca9430_0x06_VOutChargerShortIntMask					VOUT_SHORT_INT_MASK		: 1;	/* Analog short circuit protection Interrupt Mask:				*/
		uint8_t unused_7 : 1;						/* bit 7 is unused							*/
	};
	uint8_t raw;
} AlxPca9430_RegVal_0x06_BatteryChargerIntMask;
	
/*******************************************************************************************			PAC943-0_VRectThd		0x08	*/
typedef enum
{	/* Set RECT UVLO trip level: */
	VRectThd3V	= 0b00,	/*	R/W		00: 3.0V	*/
	VRectThd3V1 = 0b01,	/*	R/W		01: 3.1V	*/
	VRectThd3V2 = 0b10,	/*	R/W		00: 3.2V	*/
	VRectThd3V3 = 0b11	/*	R/W		01: 3.3V	*/
} AlxPca9430_0x08_VRectThd;

typedef union
{
	struct __attribute__((packed))
	{
		uint8_t unused_0_3 : 4;	/* bits 0...3 are unused							*/
		AlxPca9430_0x08_VRectThd VRECT_UVLO : 2;	/* Set RECT UVLO trip level:	*/
		uint8_t unused_6_7 : 2;	/* bits 6...7 are unused							*/
	};
	uint8_t raw;
} AlxPca9430_RegVal_0x08_VRectThd;

/*******************************************************************************************			PAC943-0_VRectOvw		0x09	*/
typedef enum
{	/* Set VRECT overvoltage warning level: */
	VRectOvw7V		= 0b00, /*	R/W		00: 7.0V	*/
	VRectOvw7V25	= 0b01, /*	R/W		01: 7.25V	*/
	VRectOvw7V5		= 0b10, /*	R/W		00: 7.5V	*/
	VRectOvw7V75	= 0b11	/*	R/W		01: 7.75V	*/
} AlxPca9430_0x09_VRectOvw;
	
typedef enum
{	/* Set fast charging VRECT overvoltage warning minimum level: */
	VRectOvwFactChargingMin4V	= 0b00, /*	R/W		00: 4.0V	*/
	VRectOvwFactChargingMin4V2	= 0b01, /*	R/W		01: 4.2V	*/
	VRectOvwFactChargingMin4V4	= 0b10, /*	R/W		00: 4.4V	*/
	VRectOvwFactChargingMin4V6	= 0b11	/*	R/W		01: 4.6V	*/
} AlxPca9430_0x09_VRectOvwFactChargingMin;
	
typedef enum
{	/* Set fast charging VRECT overvoltage warning delta to VBAT: */
	VRectOvwFactChargingDelta0V5	= 0b00, /*	R/W		00: 0.5V	*/
	VRectOvwFactChargingDelta0V75	= 0b01, /*	R/W		01: 0.75V	*/
	VRectOvwFactChargingDelta1V		= 0b10, /*	R/W		00: 1.0V	*/
	VRectOvwFactChargingDelta1V25	= 0b11	/*	R/W		01: 1.25V	*/
} AlxPca9430_0x09_VRectOvwFactChargingDelta;

typedef union
{
	struct __attribute__((packed))
	{
		AlxPca9430_0x09_VRectOvw					VRECT_OVW			: 2;	/* Set VRECT overvoltage warning level:	*/
		AlxPca9430_0x09_VRectOvwFactChargingMin		VRECT_OVWFAST_MIN	: 2;	/* Set fast charging VRECT overvoltage warning minimum level: */
		AlxPca9430_0x09_VRectOvwFactChargingDelta	VRECT_OVWFAST_DELTA	: 2;	/* Set fast charging VRECT overvoltage warning delta to VBAT: */
		uint8_t unused_6_7 : 2;				/* bits 6...7 are unused				*/
	};
	uint8_t raw;
} AlxPca9430_RegVal_0x09_VRectOvw;
		
/*******************************************************************************************			PAC943-0_TempThd		0x0A	*/
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
} AlxPca9430_0x0A_TempShutdownThd;

typedef enum
{ /* Die temperature warning threshold setting: */
	TempWarnThd80C	= 0b00,				/*	R/W		00: 80°C	*/
	TempWarnThd90C	= 0b01,				/*	R/W		01: 90°C	*/
	TempWarnThd100C = 0b10,				/*	R/W		10: 100°C	*/
	TempWarnThd110C = 0b11				/*	R/W		11: 110°C	*/
} AlxPca9430_0x0A_TempWarnThd;
	
typedef union
{
	struct __attribute__((packed))
	{
		AlxPca9430_0x0A_TempShutdownThd TEMP_SHDN	: 3;	/* Thermal shutdown temperature threshold (rising, 20°C hysteresis) setting:	*/
		AlxPca9430_0x0A_TempWarnThd		TEMP_WRN	: 2;	/* Die temperature warning threshold setting:									*/
		uint8_t	unused_5_7	: 3;		/* bits 5...7 are unused																			*/
	};
	uint8_t raw;
} AlxPca9430_RegVal_0x0A_TempThd;

/*************************************************************************************		PAC943-0_WatchdogSoftStartEnMcu		0x0B	*/
typedef enum
{	/* I2C register bit to its default setting, and meanwhile, this bit is cleared and reset back to “0” as well. */
	I2CSettingDoNothing		= 0b0,		/*	R/C		*/
	I2CSettingDefaultReset	= 0b1		/*	R/C		*/
} AlxPca9430_0x0B_I2CSetting;
	
typedef enum
{	/* Enable the linear battery charger: */
	VOutBatteryChargerDisabled	= 0b0,	/*	R/W		0: linear battery charger is disabled		*/
	VOutBatteryChargerEnabled	= 0b1	/*	R/W		1: linear battery charger is enabled		*/
} AlxPca9430_0x0B_VOutBatteryChargerEn;
	
typedef enum
{	/* VARACTOR 5-bit control setting bit: */
	VaractorDisabled	= 0b0,			/*	R/W		0: Varactor is off							*/
	VaractorEnabled		= 0b1			/*	R/W		1: Varactor is on							*/
} AlxPca9430_0x0B_VaractorEn;
	
typedef enum
{	/* Exit ECO_Mode bit: */
	EcoDoesNotExit		= 0b0,			/*	R/W		0: The device does not exit ECO_Mode		*/
	EcoExit				= 0b1			/*	R/W		1: The device exits ECO_Mode				*/
} AlxPca9430_0x0B_EcoExit;

typedef enum
{	/* Die temperature warning threshold setting: */
	WatchdogTimerDisabled	= 0b00,		/*	R/W		00: watchdog timer disabled	*/
	WatchdogTimer16s		= 0b01,		/*	R/W		01: 16s						*/
	WatchdogTimer32s		= 0b10,		/*	R/W		10: 32s						*/
	WatchdogTimer64s		= 0b11		/*	R/W		11: 64s						*/
} AlxPca9430_0x0B_WatchdogTimer;
	
typedef union
{
	struct __attribute__((packed))
	{
		AlxPca9430_0x0B_I2CSetting				SW_RST : 1;				/* I2C register bit to its default setting, and meanwhile, this bit is cleared and reset back to “0” as well.	*/
		AlxPca9430_0x0B_VOutBatteryChargerEn	VOUT_LDO_EN : 1;		/* Enable the linear battery charger:																			*/
		AlxPca9430_0x0B_VaractorEn				VARACTOR_EN : 1;		/* VARACTOR 5-bit control setting bit:																			*/
		uint8_t unused_3 : 1;					/* bit 3 is unused																														*/
		AlxPca9430_0x0B_EcoExit					ECO_EXIT : 1;			/* Exit ECO_Mode bit:																							*/
		uint8_t unused_5 : 1;					/* bit 5 is unused																														*/
		AlxPca9430_0x0B_WatchdogTimer			WATCH_DOG : 2;			/* Die temperature warning threshold setting:																	*/
	};
	uint8_t raw;
} AlxPca9430_RegVal_0x0B_WatchdogSoftStartEnMcu;
	
/*******************************************************************************************			PAC943-0_VaractorDac	0x0C	*/
typedef union
{
	struct __attribute__((packed))
	{
		uint8_t Varactor_DAC : 5;	/* VARACTOR DAC 5-bit setting, please refer to Section	*/
		uint8_t unused_5_7 : 3;		/* bits 5...7 are unused								*/
	};
	uint8_t raw;
} AlxPca9430_RegVal_0x0C_VaractorDac;

/*******************************************************************************************			PAC943-1_AdcControl		0x0D	*/
typedef enum
{	/* ADC averaging enable/disable */
	AdcAveragingDisabled	= 0b0,	/*	R/W		0: disable averaging, one instantaneous measurement													*/
	AdcAveragingEnabled		= 0b1	/*	R/W		1: enable averaging																					*/
} AlxPca9430_0x0D_AdcAveraging;
typedef enum
{	/* ADC sample mode control */
	Adc1ShotConversion		= 0b00, /*	R/W		00: 1-shot conversion																				*/
	AdcContinuousConversion = 0b01, /*	R/W		01: continuous conversion																			*/
	AdcBurstMode			= 0b10, /*	R/W		1x: Burst mode, measure all opened channel every 1s Channel control is defined in Section 8.13.3.15	*/
	AdcBurstMode_			= 0b11	/*	R/W		1x: Burst mode, measure all opened channel every 1s Channel control is defined in Section 8.13.3.15	*/
} AlxPca9430_0x0D_AdcRate;
typedef enum
{	/* ADC enable/disable */
	AdcDisabled = 0b0,				/*	R/W		0: disable ADC																						*/
	AdcEnabled	= 0b1				/*	R/W		1: enable ADC																						*/

} AlxPca9430_0x0D_AdcEn;

typedef union
{
	struct __attribute__((packed))
	{
		uint8_t unused_3 : 1;			/* bit 3 is unused									*/
		AlxPca9430_0x0D_AdcAveraging	ADC_AVG_EN	: 1;	/* ADC averaging enable/disable */
		AlxPca9430_0x0D_AdcRate			ADC_RATE	: 2;	/* ADC sample mode control		*/
		AlxPca9430_0x0D_AdcEn			ADC_EN		: 1;	/* ADC enable/disable			*/
		uint8_t unused_5_7 : 3;			/* bits 5...7 are unused							*/
	};
	uint8_t raw;
} AlxPca9430_RegVal_0x0D_AdcControl;
			
/*******************************************************************************************			PAC943-0_AdcSampleEn	0x0E	*/
typedef enum
{ /* NTC sampling */
	NtcAdcDisabled	= 0b0,	/*	R/W		0: disable NTC sampling		*/
	NtcAdcEnabled	= 0b1	/*	R/W		1: enable NTC sampling		*/
} AlxPca9430_0x0E_NtcAdcSampling;

typedef enum
{	/* OTP sampling */
	OtpAdcDisabled	= 0b0,	/*	R/W		0: disable OTP sampling		*/
	OtpAdcEnabled	= 0b1	/*	R/W		1: enable OTP sampling		*/
} AlxPca9430_0x0E_OtpAdcSampling;
	
typedef enum
{ /* IOUT sampling */
	IOutAdcDisabled	= 0b0,	/*	R/W		0: disable IOUT sampling	*/
	IOutAdcEnabled	= 0b1	/*	R/W		1: enable IOUT sampling		*/
} AlxPca9430_0x0E_IOutAdcSampling;

typedef enum
{ /* VOUT sampling */
	VOutAdcDisabled = 0b0,	/*	R/W		0: disable VOUT sampling	*/
	VOutAdcEnabled	= 0b1	/*	R/W		1: enable VOUT sampling		*/
} AlxPca9430_0x0E_VOutAdcSampling;

typedef enum
{ /* IRECT sampling */
	IRectAdcDisabled	= 0b0,	/*	R/W		0: disable IRECT sampling	*/
	IRectAdcEnabled		= 0b1	/*	R/W		1: enable IRECT sampling	*/
} AlxPca9430_0x0E_IRectAdcSampling;

typedef enum
{ /* VRECT sampling */
	VRectAdcDisabled	= 0b0,	/*	R/W		0: disable VRECT sampling	*/
	VRectAdcEnabled		= 0b1	/*	R/W		1: enable VRECT sampling	*/
} AlxPca9430_0x0E_VRectAdcSampling;

typedef enum
{	/* VTUNE sampling */
	VTuneAdcDisabled	= 0b0,	/*	R/W		0: disable VTUNE sampling	*/
	VTuneAdcEnabled		= 0b1	/*	R/W		1: enable VTUNE sampling	*/
} AlxPca9430_0x0E_VTuneAdcSampling;
	
typedef union
{
	struct __attribute__((packed))
	{
		AlxPca9430_0x0E_NtcAdcSampling		NTC_ADC_EN		: 1;	/* NTC sampling		*/
		AlxPca9430_0x0E_OtpAdcSampling		OTP_ADC_EN		: 1;	/* OTP sampling		*/
		AlxPca9430_0x0E_IOutAdcSampling		IOUT_ADC_EN		: 1;	/* IOUT sampling	*/
		AlxPca9430_0x0E_VOutAdcSampling		VOUT_ADC_EN		: 1;	/* VOUT sampling	*/
		AlxPca9430_0x0E_IRectAdcSampling	IRECT_ADC_EN	: 1;	/* IRECT sampling	*/
		AlxPca9430_0x0E_VRectAdcSampling	VRECT_ADC_EN	: 1;	/* VRECT sampling	*/
		AlxPca9430_0x0E_VTuneAdcSampling	VTUNE_ADC_EN	: 1;	/* VTUNE sampling	*/
		uint8_t unused_7 : 1;				/* bit 7 is unused							*/
	};
	uint8_t raw;
} AlxPca9430_RegVal_0x0E_AdcSampleEn;

/*******************************************************************************************			PAC943-0_VPwrLdoConf	0x0F	*/
typedef enum
{	/* VPWR output voltage setting bits: */
	VPwrOut1V8 = 0b00,	/*	R/W		00: VPWR=1.8V			*/
	VPwrOut2V5 = 0b01,	/*	R/W		01: VPWR=2.5V			*/
	VPwrOut2V8 = 0b10,	/*	R/W		10: VPWR=2.8V			*/
	VPwrOut3V3 = 0b11	/*	R/W		11: VPWR=3.3V (Default)	*/
} AlxPca9430_0x0F_VPwrOutSet;

typedef enum
{ /* VPWR enable bit: */
	VPwrLdoOutDisabled	= 0b0,	/*	R/W		0: disable VPWR LDO output	*/
	VPwrLdoOutEnabled	= 0b1	/*	R/W		1: Enable VPWR LDO output	*/
} AlxPca9430_0x0F_VPwrLdoOutConf;


typedef union
{
	struct __attribute__((packed))
	{
		AlxPca9430_0x0F_VPwrOutSet		VPWR_VL : 2;	/* VPWR output voltage setting bits:	*/
		uint8_t unused_2_6 : 5;			/* bits 2...6 are unused								*/
		AlxPca9430_0x0F_VPwrLdoOutConf	VPWR_EN : 1;	/* VPWR enable bit:						*/
	};
	uint8_t raw;
} AlxPca9430_RegVal_0x0F_VPwrLdoConf;
			
/*******************************************************************************************				PAC943-0_Rxir_Conf	0x10	*/
typedef enum
{ /* RXIR pull down resistor on AC1 and AC2 selection bits: */
	RxirPullDown20R	 = 0b000,	/*	R/W		000: 20ohm (default)*/
	RxirPullDown80R	 = 0b001,	/*	R/W		001: 80ohm			*/
	RxirPullDown140R = 0b010,	/*	R/W		010: 140ohm			*/
	RxirPullDown200R = 0b011,	/*	R/W		011: 200ohm			*/
	RxirPullDown260R = 0b100,	/*	R/W		100: 260ohm			*/
	RxirPullDown320R = 0b101,	/*	R/W		101: 320ohm			*/
	RxirPullDown380R = 0b110,	/*	R/W		110: 380ohm			*/
	RxirPullDown440R = 0b111	/*	R/W		111: 440ohm			*/
} AlxPca9430_0x10_RxirPullDownR;

typedef enum
{	/* RXIR pull down duration selection bits: */
	RxirPullDown0ms2 = 0b000,	/*	R/W		000: 0.2ms			*/
	RxirPullDown0ms4 = 0b001,	/*	R/W		001: 0.4ms			*/
	RxirPullDown0ms6 = 0b010,	/*	R/W		010: 0.6ms			*/
	RxirPullDown0ms8 = 0b011,	/*	R/W		011: 0.8ms			*/
	RxirPullDown1ms0 = 0b100,	/*	R/W		100: 1.0ms (default)*/
	RxirPullDown1ms2 = 0b101,	/*	R/W		101: 1.2ms			*/
	RxirPullDown1ms4 = 0b110,	/*	R/W		110: 1.4ms			*/
	RxirPullDown1ms6 = 0b111	/*	R/W		111: 1.6ms			*/
} AlxPca9430_0x10_RxirPullDownDuration;

typedef enum
{	/* RXIR enable bit: */
	RxirPullDownDisabled	= 0b0,	/*	R/W		0: RXIR function is disabled	*/
	RxirPullDownEnabled		= 0b1	/*	R/W		1: RXIR function is enabled		*/
	/*	The bit is cleared by PCA9430 when RXIR is finished						*/
} AlxPca9430_0x10_RxirPullDownEn;
	
typedef union
{
	struct __attribute__((packed))
	{
		AlxPca9430_0x10_RxirPullDownR			RXIR_STR	: 3;	/* RXIR pull down resistor on AC1 and AC2 selection bits:	*/
		AlxPca9430_0x10_RxirPullDownDuration	RXIR_DUR	: 3;	/* RXIR pull down duration selection bits:					*/
		uint8_t unused_6 : 1;					/* bit 6 is unused																*/
		AlxPca9430_0x10_RxirPullDownEn			RXIR_EN		: 1;	/* VPWR enable bit:											*/
	};
	uint8_t raw;
} AlxPca9430_RegVal_0x10_Rxir_Conf;
				
/*******************************************************************************************			PAC943-0_ChargerSetLock	0x20	*/
typedef enum
{ /* Enable the fast charge timer and pre-qual timer: */
	FastChargeTimerDisabled = 0b0,	/*	R/W		0: fast charge timer and pre-qual timer are both disabled	*/
	FastChargeTimerEnabled	= 0b1	/*	R/W		1: fast charge timer and pre-qual timer are both enabled	*/
} AlxPca9430_0x20_FastChargeTimerEn;

typedef enum
{ /* Enable TS pin external thermistor (NTC) control in charger: */
	NtcThermistorDisabled	= 0b0,	/*	R/W		0: Thermistor (NTC) control disabled in charger	*/
	NtcThermistorEnabled	= 0b1	/*	R/W		1: Thermistor (NTC) control active in charger	*/
} AlxPca9430_0x20_NtcThermistorEn;

typedef enum
{	/* Linear battery charger constant current (CC) setting: */
	ChargerCCNotLockReturnPresentReg0x00	= 0x00, /*	R/W		00h: Return present register	*/
	ChargerCCNotLockReturnPresentReg0x01	= 0x01, /*	R/W		01h: Return present register	*/
	ChargerCCNotLockReturnPresentReg0x02	= 0x02, /*	R/W		02h: Return present register	*/
	ChargerCCNotLockReturnPresentReg0x03	= 0x03, /*	R/W		03h: Return present register	*/
	ChargerCCNotLockReturnPresentReg0x04	= 0x04, /*	R/W		04h: Return present register	*/
	ChargerCCNotLockReturnPresentReg0x05	= 0x05, /*	R/W		05h: Return present register	*/
	ChargerCCNotLockReturnPresentReg0x06	= 0x06, /*	R/W		06h: Return present register	*/
	ChargerCCNotLockReturnPresentReg0x07	= 0x07, /*	R/W		07h: Return present register	*/
	ChargerCCNotLockReturnPresentReg0x08	= 0x08, /*	R/W		08h: Return present register	*/
	ChargerCCNotLockReturnPresentReg0x09	= 0x09, /*	R/W		09h: Return present register	*/
	ChargerCCNotLockReturnPresentReg0x0A	= 0x0A, /*	R/W		0Ah: Return present register	*/
	ChargerCCNotLockReturnPresentReg0x0B	= 0x0B, /*	R/W		0Bh: Return present register	*/
	ChargerCCNotLockReturnPresentReg0x0C	= 0x0C, /*	R/W		0Ch: Return present register	*/
	ChargerCCNotLockReturnPresentReg0x0D	= 0x0D, /*	R/W		0Dh: Return present register	*/
	ChargerCCNotLockReturnPresentReg0x0E	= 0x0E, /*	R/W		0Eh: Return present register	*/
	ChargerCCNotLockReturnPresentReg0x0F	= 0x0F, /*	R/W		0Fh: Return present register	*/
	ChargerCCNotLockReturnPresentReg0x10	= 0x10, /*	R/W		10h: Return present register	*/
	ChargerCCNotLockReturnPresentReg0x11	= 0x11, /*	R/W		11h: Return present register	*/
	ChargerCCNotLockReturnPresentReg0x12	= 0x12, /*	R/W		12h: Return present register	*/
	ChargerCCNotLockReturnPresentReg0x13	= 0x13, /*	R/W		13h: Return present register	*/
	ChargerCCNotLockReturnPresentReg0x14	= 0x14, /*	R/W		14h: Return present register	*/
	ChargerCCLock = 0x15, /*	R/W		15h: OCPSET_LOCK[4:0] = 10101, these registers which are labeled as “locked by CHG_LOCK” can be accessed to perform I2C “write” command. */
	ChargerCCNotLockReturnPresentReg0x16	= 0x16, /*	R/W		16h: Return present register	*/
	ChargerCCNotLockReturnPresentReg0x17	= 0x17, /*	R/W		17h: Return present register	*/
	ChargerCCNotLockReturnPresentReg0x18	= 0x18, /*	R/W		18h: Return present register	*/
	ChargerCCNotLockReturnPresentReg0x19	= 0x19, /*	R/W		19h: Return present register	*/
	ChargerCCNotLockReturnPresentReg0x1A	= 0x1A, /*	R/W		1Ah: Return present register	*/
	ChargerCCNotLockReturnPresentReg0x1B	= 0x1B, /*	R/W		1Bh: Return present register	*/
	ChargerCCNotLockReturnPresentReg0x1C	= 0x1C, /*	R/W		1Ch: Return present register	*/
	ChargerCCNotLockReturnPresentReg0x1D	= 0x1D, /*	R/W		1Dh: Return present register	*/
	ChargerCCNotLockReturnPresentReg0x1E	= 0x1E, /*	R/W		1Eh: Return present register	*/
	ChargerCCNotLockReturnPresentReg0x1F	= 0x1F	/*	R/W		1Fh: Return present register	*/
	/* CHGN_LOCK[4:0] ≠ 10101, these registers which are labeled as “locked by CHG_LOCK” can NOT be accessed to perform I2C “write” command.
	 * In such case when “write” command is performed on these locked registers, it returns the present register value.					*/
} AlxPca9430_0x20_ChargerCCSetLock;

typedef union
{
	struct __attribute__((packed))
	{
		uint8_t unused_0 : 1;				/* bit 0 is unused																		*/
		AlxPca9430_0x20_FastChargeTimerEn		CHG_TIMER_EN : 1;	/* Enable the fast charge timer and pre-qual timer				*/
		AlxPca9430_0x20_NtcThermistorEn			NTC_EN		 : 1;	/* Enable TS pin external thermistor (NTC) control in charger:	*/
		AlxPca9430_0x20_ChargerCCSetLock		OCPSET_LOCK	 : 5;	/* Linear battery charger constant current (CC) setting:		*/
	};
	uint8_t raw;
} AlxPca9430_RegVal_0x20_ChargerCCSetLock;
	
/**************************************************************************************				PAC943-0_ChargerSetCC		0x21	*/
typedef enum
{ /* Linear battery charger constant current (CC) setting:		*/
	ChargerSetCC0mA		= 0x00, /*	R/W		00h: Set 0mA				*/
	ChargerSetCC5mA		= 0x01, /*	R/W		01h: Set 5mA				*/
	ChargerSetCC10mA	= 0x02, /*	R/W		02h: Set 10mA				*/
	ChargerSetCC15mA	= 0x03, /*	R/W		03h: Set 15mA				*/
	ChargerSetCC20mA	= 0x04, /*	R/W		04h: Set 20mA				*/
	ChargerSetCC25mA	= 0x05, /*	R/W		05h: Set 25mA				*/
	ChargerSetCC30mA	= 0x06, /*	R/W		06h: Set 30mA				*/
	ChargerSetCC35mA	= 0x07, /*	R/W		07h: Set 35mA				*/
	ChargerSetCC40mA	= 0x08, /*	R/W		08h: Set 40mA	(default)	*/
	ChargerSetCC45mA	= 0x09, /*	R/W		09h: Set 45mA				*/
	ChargerSetCC50mA	= 0x0A, /*	R/W		0Ah: Set 50mA				*/
	ChargerSetCC55mA	= 0x0B, /*	R/W		0Bh: Set 55mA				*/
	ChargerSetCC60mA	= 0x0C, /*	R/W		0Ch: Set 60mA				*/
	ChargerSetCC65mA	= 0x0D, /*	R/W		0Dh: Set 65mA				*/
	ChargerSetCC70mA	= 0x0E, /*	R/W		0Eh: Set 70mA				*/
	ChargerSetCC75mA	= 0x0F, /*	R/W		0Fh: Set 75mA				*/
	ChargerSetCC80mA	= 0x10, /*	R/W		10h: Set 80mA				*/
	ChargerSetCC85mA	= 0x11, /*	R/W		11h: Set 85mA				*/
	ChargerSetCC90mA	= 0x12, /*	R/W		12h: Set 90mA				*/
	ChargerSetCC95mA	= 0x13, /*	R/W		13h: Set 95mA				*/
	ChargerSetCC100mA	= 0x14, /*	R/W		14h: Set 100mA				*/
	ChargerSetCC105mA	= 0x15, /*	R/W		15h: Set 105mA				*/
	ChargerSetCC110mA	= 0x16, /*	R/W		16h: Set 110mA				*/
	ChargerSetCC115mA	= 0x17, /*	R/W		17h: Set 115mA				*/
	ChargerSetCC120mA	= 0x18, /*	R/W		18h: Set 120mA				*/
	ChargerSetCC125mA	= 0x19, /*	R/W		19h: Set 125mA				*/
	ChargerSetCC130mA	= 0x1A, /*	R/W		1Ah: Set 130mA				*/
	ChargerSetCC135mA	= 0x1B, /*	R/W		1Bh: Set 135mA				*/
	ChargerSetCC140mA	= 0x1C, /*	R/W		1Ch: Set 140mA				*/
	ChargerSetCC145mA	= 0x1D, /*	R/W		1Dh: Set 145mA				*/
	ChargerSetCC150mA	= 0x1E, /*	R/W		1Eh: Set 150mA				*/
	ChargerSetCC155mA	= 0x1F, /*	R/W		1Fh: Set 155mA				*/
	ChargerSetCC160mA	= 0x20, /*	R/W		20h: Set 160mA				*/
	ChargerSetCC165mA	= 0x21, /*	R/W		21h: Set 165mA				*/
	ChargerSetCC170mA	= 0x22, /*	R/W		22h: Set 170mA				*/
	ChargerSetCC175mA	= 0x23, /*	R/W		23h: Set 175mA				*/
	ChargerSetCC180mA	= 0x24, /*	R/W		24h: Set 180mA				*/
	ChargerSetCC185mA	= 0x25, /*	R/W		25h: Set 185mA				*/
	ChargerSetCC190mA	= 0x26, /*	R/W		26h: Set 190mA				*/
	ChargerSetCC195mA	= 0x27, /*	R/W		27h: Set 195mA				*/
	ChargerSetCC200mA	= 0x28, /*	R/W		28h: Set 200mA				*/
	ChargerSetCC205mA	= 0x29, /*	R/W		29h: Set 205mA				*/
	ChargerSetCC210mA	= 0x2A, /*	R/W		2Ah: Set 210mA				*/
	ChargerSetCC215mA	= 0x2B, /*	R/W		2Bh: Set 215mA				*/
	ChargerSetCC220mA	= 0x2C, /*	R/W		2Ch: Set 220mA				*/
	ChargerSetCC225mA	= 0x2D, /*	R/W		2Dh: Set 225mA				*/
	ChargerSetCC230mA	= 0x2E, /*	R/W		2Eh: Set 230mA				*/
	ChargerSetCC235mA	= 0x2F, /*	R/W		2Fh: Set 235mA				*/
	ChargerSetCC240mA	= 0x30, /*	R/W		30h: Set 240mA				*/
	ChargerSetCC245mA	= 0x31, /*	R/W		31h: Set 245mA				*/
	ChargerSetCC250mA	= 0x32, /*	R/W		32h: Set 250mA				*/
	ChargerSetCC255mA	= 0x33, /*	R/W		33h: Set 255mA				*/
	ChargerSetCC260mA	= 0x34, /*	R/W		34h: Set 260mA				*/
	ChargerSetCC265mA	= 0x35, /*	R/W		35h: Set 265mA				*/
	ChargerSetCC270mA	= 0x36, /*	R/W		36h: Set 270mA				*/
	ChargerSetCC275mA	= 0x37, /*	R/W		37h: Set 275mA				*/
	ChargerSetCC280mA	= 0x38, /*	R/W		38h: Set 280mA				*/
	ChargerSetCC285mA	= 0x39, /*	R/W		39h: Set 285mA				*/
	ChargerSetCC290mA	= 0x3A, /*	R/W		3Ah: Set 290mA				*/
	ChargerSetCC295mA	= 0x3B, /*	R/W		3Bh: Set 295mA				*/
	ChargerSetCC300mA	= 0x3C, /*	R/W		3Ch: Set 300mA				*/
	ChargerSetCC305mA	= 0x3D, /*	R/W		3Dh: Set 305mA				*/
	ChargerSetCC310mA	= 0x3E, /*	R/W		3Eh: Set 310mA				*/
	ChargerSetCC315mA	= 0x3F	/*	R/W		3Fh: Set 315mA				*/
} AlxPca9430_0x21_ChargerSetCC;

typedef union
{
	struct __attribute__((packed))
	{
		AlxPca9430_0x21_ChargerSetCC ICHG_CC : 6;	/* Linear battery charger constant current (CC) setting:	*/
		uint8_t unused_6_7 : 2;		/* bits 6...7 are unused													*/
	};
	uint8_t raw;
} AlxPca9430_RegVal_0x21_ChargerSetCC;
	
/**********************************************************************************			PAC943-0_ChargerSetTopOffCurrent	0x22	*/
typedef enum
{ /* Linear battery charger top-off charge current setting:		*/
	ChargerSetTopOffCurrent0mA	= 0x00, /*	R/W		00h: Set 0mA				*/
	ChargerSetTopOffCurrent1mA	= 0x01, /*	R/W		01h: Set 1mA				*/
	ChargerSetTopOffCurrent2mA	= 0x02, /*	R/W		02h: Set 2mA				*/
	ChargerSetTopOffCurrent3mA	= 0x03, /*	R/W		03h: Set 3mA				*/
	ChargerSetTopOffCurrent4mA	= 0x04, /*	R/W		04h: Set 4mA	(default)	*/
	ChargerSetTopOffCurrent5mA	= 0x05, /*	R/W		05h: Set 5mA				*/
	ChargerSetTopOffCurrent6mA	= 0x06, /*	R/W		06h: Set 6mA				*/
	ChargerSetTopOffCurrent7mA	= 0x07, /*	R/W		07h: Set 7mA				*/
	ChargerSetTopOffCurrent8mA	= 0x08, /*	R/W		08h: Set 8mA				*/
	ChargerSetTopOffCurrent9mA	= 0x09, /*	R/W		09h: Set 9mA				*/
	ChargerSetTopOffCurrent10mA	= 0x0A, /*	R/W		0Ah: Set 10mA				*/
	ChargerSetTopOffCurrent11mA	= 0x0B, /*	R/W		0Bh: Set 11mA				*/
	ChargerSetTopOffCurrent12mA	= 0x0C, /*	R/W		0Ch: Set 12mA				*/
	ChargerSetTopOffCurrent13mA	= 0x0D, /*	R/W		0Dh: Set 13mA				*/
	ChargerSetTopOffCurrent14mA	= 0x0E, /*	R/W		0Eh: Set 14mA				*/
	ChargerSetTopOffCurrent15mA	= 0x0F, /*	R/W		0Fh: Set 15mA				*/
	ChargerSetTopOffCurrent16mA	= 0x10, /*	R/W		10h: Set 16mA				*/
	ChargerSetTopOffCurrent17mA	= 0x11, /*	R/W		11h: Set 17mA				*/
	ChargerSetTopOffCurrent18mA	= 0x12, /*	R/W		12h: Set 18mA				*/
	ChargerSetTopOffCurrent19mA	= 0x13, /*	R/W		13h: Set 19mA				*/
	ChargerSetTopOffCurrent20mA	= 0x14, /*	R/W		14h: Set 20mA				*/
	ChargerSetTopOffCurrent21mA	= 0x15, /*	R/W		15h: Set 21mA				*/
	ChargerSetTopOffCurrent22mA	= 0x16, /*	R/W		16h: Set 22mA				*/
	ChargerSetTopOffCurrent23mA	= 0x17, /*	R/W		17h: Set 23mA				*/
	ChargerSetTopOffCurrent24mA	= 0x18, /*	R/W		18h: Set 24mA				*/
	ChargerSetTopOffCurrent25mA	= 0x19, /*	R/W		19h: Set 25mA				*/
	ChargerSetTopOffCurrent26mA	= 0x1A, /*	R/W		1Ah: Set 26mA				*/
	ChargerSetTopOffCurrent27mA	= 0x1B, /*	R/W		1Bh: Set 27mA				*/
	ChargerSetTopOffCurrent28mA	= 0x1C, /*	R/W		1Ch: Set 28mA				*/
	ChargerSetTopOffCurrent29mA	= 0x1D, /*	R/W		1Dh: Set 29mA				*/
	ChargerSetTopOffCurrent30mA	= 0x1E, /*	R/W		1Eh: Set 30mA				*/
	ChargerSetTopOffCurrent31mA	= 0x1F, /*	R/W		1Fh: Set 31mA				*/
	ChargerSetTopOffCurrent32mA	= 0x20, /*	R/W		20h: Set 32mA				*/
	ChargerSetTopOffCurrent33mA	= 0x21, /*	R/W		21h: Set 33mA				*/
	ChargerSetTopOffCurrent34mA	= 0x22, /*	R/W		22h: Set 34mA				*/
	ChargerSetTopOffCurrent35mA	= 0x23, /*	R/W		23h: Set 35mA				*/
	ChargerSetTopOffCurrent36mA	= 0x24, /*	R/W		24h: Set 36mA				*/
	ChargerSetTopOffCurrent37mA	= 0x25, /*	R/W		25h: Set 37mA				*/
	ChargerSetTopOffCurrent38mA	= 0x26, /*	R/W		26h: Set 38mA				*/
	ChargerSetTopOffCurrent39mA	= 0x27, /*	R/W		27h: Set 39mA				*/
	ChargerSetTopOffCurrent40mA	= 0x28, /*	R/W		28h: Set 40mA				*/
	ChargerSetTopOffCurrent41mA	= 0x29, /*	R/W		29h: Set 41mA				*/
	ChargerSetTopOffCurrent42mA	= 0x2A, /*	R/W		2Ah: Set 42mA				*/
	ChargerSetTopOffCurrent43mA	= 0x2B, /*	R/W		2Bh: Set 43mA				*/
	ChargerSetTopOffCurrent44mA	= 0x2C, /*	R/W		2Ch: Set 44mA				*/
	ChargerSetTopOffCurrent45mA	= 0x2D, /*	R/W		2Dh: Set 45mA				*/
	ChargerSetTopOffCurrent46mA	= 0x2E, /*	R/W		2Eh: Set 46mA				*/
	ChargerSetTopOffCurrent47mA	= 0x2F, /*	R/W		2Fh: Set 47mA				*/
	ChargerSetTopOffCurrent48mA	= 0x30, /*	R/W		30h: Set 48mA				*/
	ChargerSetTopOffCurrent49mA	= 0x31, /*	R/W		31h: Set 49mA				*/
	ChargerSetTopOffCurrent50mA	= 0x32, /*	R/W		32h: Set 50mA				*/
	ChargerSetTopOffCurrent51mA	= 0x33, /*	R/W		33h: Set 51mA				*/
	ChargerSetTopOffCurrent52mA	= 0x34, /*	R/W		34h: Set 52mA				*/
	ChargerSetTopOffCurrent53mA	= 0x35, /*	R/W		35h: Set 53mA				*/
	ChargerSetTopOffCurrent54mA	= 0x36, /*	R/W		36h: Set 54mA				*/
	ChargerSetTopOffCurrent55mA	= 0x37, /*	R/W		37h: Set 55mA				*/
	ChargerSetTopOffCurrent56mA	= 0x38, /*	R/W		38h: Set 56mA				*/
	ChargerSetTopOffCurrent57mA	= 0x39, /*	R/W		39h: Set 57mA				*/
	ChargerSetTopOffCurrent58mA	= 0x3A, /*	R/W		3Ah: Set 58mA				*/
	ChargerSetTopOffCurrent59mA	= 0x3B, /*	R/W		3Bh: Set 59mA				*/
	ChargerSetTopOffCurrent60mA	= 0x3C, /*	R/W		3Ch: Set 60mA				*/
	ChargerSetTopOffCurrent61mA	= 0x3D, /*	R/W		3Dh: Set 61mA				*/
	ChargerSetTopOffCurrent62mA	= 0x3E, /*	R/W		3Eh: Set 62mA				*/
	ChargerSetTopOffCurrent63mA	= 0x3F	/*	R/W		3Fh: Set 63mA				*/
} AlxPca9430_0x22_ChargerSetTopOffCurrent;

typedef union
{
	struct __attribute__((packed))
	{
		AlxPca9430_0x22_ChargerSetTopOffCurrent ICHG_TOPOFF : 6;		/* Linear battery charger top-off charge current setting:	*/
		uint8_t unused_6_7 : 2;		/* bits 6...7 are unused																		*/
	};
	uint8_t raw;
} AlxPca9430_RegVal_0x22_ChargerSetTopOffCurrent;
	
/******************************************************************************		PAC943-0_ChargerSetLowBatteryChargeCurrent	0x23	*/
typedef enum
{ /* Low battery charge current setting:		*/
	ChargerSetLowBatteryCurrent0mA	= 0x00, /*	R/W		00h: Set 0mA				*/
	ChargerSetLowBatteryCurrent1mA	= 0x01, /*	R/W		01h: Set 1mA				*/
	ChargerSetLowBatteryCurrent2mA	= 0x02, /*	R/W		02h: Set 2mA				*/
	ChargerSetLowBatteryCurrent3mA	= 0x03, /*	R/W		03h: Set 3mA				*/
	ChargerSetLowBatteryCurrent4mA	= 0x04, /*	R/W		04h: Set 4mA				*/
	ChargerSetLowBatteryCurrent5mA	= 0x05, /*	R/W		05h: Set 5mA				*/
	ChargerSetLowBatteryCurrent6mA	= 0x06, /*	R/W		06h: Set 6mA				*/
	ChargerSetLowBatteryCurrent7mA	= 0x07, /*	R/W		07h: Set 7mA				*/
	ChargerSetLowBatteryCurrent8mA	= 0x08, /*	R/W		08h: Set 8mA	(default)	*/
	ChargerSetLowBatteryCurrent9mA	= 0x09, /*	R/W		09h: Set 9mA				*/
	ChargerSetLowBatteryCurrent10mA	= 0x0A, /*	R/W		0Ah: Set 10mA				*/
	ChargerSetLowBatteryCurrent11mA	= 0x0B, /*	R/W		0Bh: Set 11mA				*/
	ChargerSetLowBatteryCurrent12mA	= 0x0C, /*	R/W		0Ch: Set 12mA				*/
	ChargerSetLowBatteryCurrent13mA	= 0x0D, /*	R/W		0Dh: Set 13mA				*/
	ChargerSetLowBatteryCurrent14mA	= 0x0E, /*	R/W		0Eh: Set 14mA				*/
	ChargerSetLowBatteryCurrent15mA	= 0x0F, /*	R/W		0Fh: Set 15mA				*/
	ChargerSetLowBatteryCurrent16mA	= 0x10, /*	R/W		10h: Set 16mA				*/
	ChargerSetLowBatteryCurrent17mA	= 0x11, /*	R/W		11h: Set 17mA				*/
	ChargerSetLowBatteryCurrent18mA	= 0x12, /*	R/W		12h: Set 18mA				*/
	ChargerSetLowBatteryCurrent19mA	= 0x13, /*	R/W		13h: Set 19mA				*/
	ChargerSetLowBatteryCurrent20mA	= 0x14, /*	R/W		14h: Set 20mA				*/
	ChargerSetLowBatteryCurrent21mA	= 0x15, /*	R/W		15h: Set 21mA				*/
	ChargerSetLowBatteryCurrent22mA	= 0x16, /*	R/W		16h: Set 22mA				*/
	ChargerSetLowBatteryCurrent23mA	= 0x17, /*	R/W		17h: Set 23mA				*/
	ChargerSetLowBatteryCurrent24mA	= 0x18, /*	R/W		18h: Set 24mA				*/
	ChargerSetLowBatteryCurrent25mA	= 0x19, /*	R/W		19h: Set 25mA				*/
	ChargerSetLowBatteryCurrent26mA	= 0x1A, /*	R/W		1Ah: Set 26mA				*/
	ChargerSetLowBatteryCurrent27mA	= 0x1B, /*	R/W		1Bh: Set 27mA				*/
	ChargerSetLowBatteryCurrent28mA	= 0x1C, /*	R/W		1Ch: Set 28mA				*/
	ChargerSetLowBatteryCurrent29mA	= 0x1D, /*	R/W		1Dh: Set 29mA				*/
	ChargerSetLowBatteryCurrent30mA	= 0x1E, /*	R/W		1Eh: Set 30mA				*/
	ChargerSetLowBatteryCurrent31mA	= 0x1F, /*	R/W		1Fh: Set 31mA				*/
	ChargerSetLowBatteryCurrent32mA	= 0x20, /*	R/W		20h: Set 32mA				*/
	ChargerSetLowBatteryCurrent33mA	= 0x21, /*	R/W		21h: Set 33mA				*/
	ChargerSetLowBatteryCurrent34mA	= 0x22, /*	R/W		22h: Set 34mA				*/
	ChargerSetLowBatteryCurrent35mA	= 0x23, /*	R/W		23h: Set 35mA				*/
	ChargerSetLowBatteryCurrent36mA	= 0x24, /*	R/W		24h: Set 36mA				*/
	ChargerSetLowBatteryCurrent37mA	= 0x25, /*	R/W		25h: Set 37mA				*/
	ChargerSetLowBatteryCurrent38mA	= 0x26, /*	R/W		26h: Set 38mA				*/
	ChargerSetLowBatteryCurrent39mA	= 0x27, /*	R/W		27h: Set 39mA				*/
	ChargerSetLowBatteryCurrent40mA	= 0x28, /*	R/W		28h: Set 40mA				*/
	ChargerSetLowBatteryCurrent41mA	= 0x29, /*	R/W		29h: Set 41mA				*/
	ChargerSetLowBatteryCurrent42mA	= 0x2A, /*	R/W		2Ah: Set 42mA				*/
	ChargerSetLowBatteryCurrent43mA	= 0x2B, /*	R/W		2Bh: Set 43mA				*/
	ChargerSetLowBatteryCurrent44mA	= 0x2C, /*	R/W		2Ch: Set 44mA				*/
	ChargerSetLowBatteryCurrent45mA	= 0x2D, /*	R/W		2Dh: Set 45mA				*/
	ChargerSetLowBatteryCurrent46mA	= 0x2E, /*	R/W		2Eh: Set 46mA				*/
	ChargerSetLowBatteryCurrent47mA	= 0x2F, /*	R/W		2Fh: Set 47mA				*/
	ChargerSetLowBatteryCurrent48mA	= 0x30, /*	R/W		30h: Set 48mA				*/
	ChargerSetLowBatteryCurrent49mA	= 0x31, /*	R/W		31h: Set 49mA				*/
	ChargerSetLowBatteryCurrent50mA	= 0x32, /*	R/W		32h: Set 50mA				*/
	ChargerSetLowBatteryCurrent51mA	= 0x33, /*	R/W		33h: Set 51mA				*/
	ChargerSetLowBatteryCurrent52mA	= 0x34, /*	R/W		34h: Set 52mA				*/
	ChargerSetLowBatteryCurrent53mA	= 0x35, /*	R/W		35h: Set 53mA				*/
	ChargerSetLowBatteryCurrent54mA	= 0x36, /*	R/W		36h: Set 54mA				*/
	ChargerSetLowBatteryCurrent55mA	= 0x37, /*	R/W		37h: Set 55mA				*/
	ChargerSetLowBatteryCurrent56mA	= 0x38, /*	R/W		38h: Set 56mA				*/
	ChargerSetLowBatteryCurrent57mA	= 0x39, /*	R/W		39h: Set 57mA				*/
	ChargerSetLowBatteryCurrent58mA	= 0x3A, /*	R/W		3Ah: Set 58mA				*/
	ChargerSetLowBatteryCurrent59mA	= 0x3B, /*	R/W		3Bh: Set 59mA				*/
	ChargerSetLowBatteryCurrent60mA	= 0x3C, /*	R/W		3Ch: Set 60mA				*/
	ChargerSetLowBatteryCurrent61mA	= 0x3D, /*	R/W		3Dh: Set 61mA				*/
	ChargerSetLowBatteryCurrent62mA	= 0x3E, /*	R/W		3Eh: Set 62mA				*/
	ChargerSetLowBatteryCurrent63mA	= 0x3F	/*	R/W		3Fh: Set 63mA				*/
} AlxPca9430_0x23_ChargerSetLowBatteryCurrent;

typedef union
{
	struct __attribute__((packed))
	{
		AlxPca9430_0x23_ChargerSetLowBatteryCurrent ICHG_LOW : 6;	/* Low battery charge current setting:	*/
		uint8_t unused_6_7 : 2;		/* bits 6...7 are unused												*/
	};
	uint8_t raw;
} AlxPca9430_RegVal_0x23_ChargerSetLowBatteryCurrent;
	
/******************************************************************************		PAC943-0_ChargerSetDeadBatteryChargeCurrent	0x24	*/
typedef enum
{ /* Dead battery charge current setting:		*/
	ChargerSetDeadBatteryCurrent0mA		= 0x00, /*	R/W		00h: Set 0mA				*/
	ChargerSetDeadBatteryCurrent1mA		= 0x01, /*	R/W		01h: Set 1mA				*/
	ChargerSetDeadBatteryCurrent2mA		= 0x02, /*	R/W		02h: Set 2mA				*/
	ChargerSetDeadBatteryCurrent3mA		= 0x03, /*	R/W		03h: Set 3mA				*/
	ChargerSetDeadBatteryCurrent4mA		= 0x04, /*	R/W		04h: Set 4mA	(default)	*/
	ChargerSetDeadBatteryCurrent5mA		= 0x05, /*	R/W		05h: Set 5mA				*/
	ChargerSetDeadBatteryCurrent6mA		= 0x06, /*	R/W		06h: Set 6mA				*/
	ChargerSetDeadBatteryCurrent7mA		= 0x07, /*	R/W		07h: Set 7mA				*/
	ChargerSetDeadBatteryCurrent8mA		= 0x08, /*	R/W		08h: Set 8mA				*/
	ChargerSetDeadBatteryCurrent9mA		= 0x09, /*	R/W		09h: Set 9mA				*/
	ChargerSetDeadBatteryCurrent10mA	= 0x0A, /*	R/W		0Ah: Set 10mA				*/
	ChargerSetDeadBatteryCurrent11mA	= 0x0B, /*	R/W		0Bh: Set 11mA				*/
	ChargerSetDeadBatteryCurrent12mA	= 0x0C, /*	R/W		0Ch: Set 12mA				*/
	ChargerSetDeadBatteryCurrent13mA	= 0x0D, /*	R/W		0Dh: Set 13mA				*/
	ChargerSetDeadBatteryCurrent14mA	= 0x0E, /*	R/W		0Eh: Set 14mA				*/
	ChargerSetDeadBatteryCurrent15mA	= 0x0F, /*	R/W		0Fh: Set 15mA				*/
	ChargerSetDeadBatteryCurrent16mA	= 0x10, /*	R/W		10h: Set 16mA				*/
	ChargerSetDeadBatteryCurrent17mA	= 0x11, /*	R/W		11h: Set 17mA				*/
	ChargerSetDeadBatteryCurrent18mA	= 0x12, /*	R/W		12h: Set 18mA				*/
	ChargerSetDeadBatteryCurrent19mA	= 0x13, /*	R/W		13h: Set 19mA				*/
	ChargerSetDeadBatteryCurrent20mA	= 0x14, /*	R/W		14h: Set 20mA				*/
	ChargerSetDeadBatteryCurrent21mA	= 0x15, /*	R/W		15h: Set 21mA				*/
	ChargerSetDeadBatteryCurrent22mA	= 0x16, /*	R/W		16h: Set 22mA				*/
	ChargerSetDeadBatteryCurrent23mA	= 0x17, /*	R/W		17h: Set 23mA				*/
	ChargerSetDeadBatteryCurrent24mA	= 0x18, /*	R/W		18h: Set 24mA				*/
	ChargerSetDeadBatteryCurrent25mA	= 0x19, /*	R/W		19h: Set 25mA				*/
	ChargerSetDeadBatteryCurrent26mA	= 0x1A, /*	R/W		1Ah: Set 26mA				*/
	ChargerSetDeadBatteryCurrent27mA	= 0x1B, /*	R/W		1Bh: Set 27mA				*/
	ChargerSetDeadBatteryCurrent28mA	= 0x1C, /*	R/W		1Ch: Set 28mA				*/
	ChargerSetDeadBatteryCurrent29mA	= 0x1D, /*	R/W		1Dh: Set 29mA				*/
	ChargerSetDeadBatteryCurrent30mA	= 0x1E, /*	R/W		1Eh: Set 30mA				*/
	ChargerSetDeadBatteryCurrent31mA	= 0x1F, /*	R/W		1Fh: Set 31mA				*/
	ChargerSetDeadBatteryCurrent32mA	= 0x20, /*	R/W		20h: Set 32mA				*/
	ChargerSetDeadBatteryCurrent33mA	= 0x21, /*	R/W		21h: Set 33mA				*/
	ChargerSetDeadBatteryCurrent34mA	= 0x22, /*	R/W		22h: Set 34mA				*/
	ChargerSetDeadBatteryCurrent35mA	= 0x23, /*	R/W		23h: Set 35mA				*/
	ChargerSetDeadBatteryCurrent36mA	= 0x24, /*	R/W		24h: Set 36mA				*/
	ChargerSetDeadBatteryCurrent37mA	= 0x25, /*	R/W		25h: Set 37mA				*/
	ChargerSetDeadBatteryCurrent38mA	= 0x26, /*	R/W		26h: Set 38mA				*/
	ChargerSetDeadBatteryCurrent39mA	= 0x27, /*	R/W		27h: Set 39mA				*/
	ChargerSetDeadBatteryCurrent40mA	= 0x28, /*	R/W		28h: Set 40mA				*/
	ChargerSetDeadBatteryCurrent41mA	= 0x29, /*	R/W		29h: Set 41mA				*/
	ChargerSetDeadBatteryCurrent42mA	= 0x2A, /*	R/W		2Ah: Set 42mA				*/
	ChargerSetDeadBatteryCurrent43mA	= 0x2B, /*	R/W		2Bh: Set 43mA				*/
	ChargerSetDeadBatteryCurrent44mA	= 0x2C, /*	R/W		2Ch: Set 44mA				*/
	ChargerSetDeadBatteryCurrent45mA	= 0x2D, /*	R/W		2Dh: Set 45mA				*/
	ChargerSetDeadBatteryCurrent46mA	= 0x2E, /*	R/W		2Eh: Set 46mA				*/
	ChargerSetDeadBatteryCurrent47mA	= 0x2F, /*	R/W		2Fh: Set 47mA				*/
	ChargerSetDeadBatteryCurrent48mA	= 0x30, /*	R/W		30h: Set 48mA				*/
	ChargerSetDeadBatteryCurrent49mA	= 0x31, /*	R/W		31h: Set 49mA				*/
	ChargerSetDeadBatteryCurrent50mA	= 0x32, /*	R/W		32h: Set 50mA				*/
	ChargerSetDeadBatteryCurrent51mA	= 0x33, /*	R/W		33h: Set 51mA				*/
	ChargerSetDeadBatteryCurrent52mA	= 0x34, /*	R/W		34h: Set 52mA				*/
	ChargerSetDeadBatteryCurrent53mA	= 0x35, /*	R/W		35h: Set 53mA				*/
	ChargerSetDeadBatteryCurrent54mA	= 0x36, /*	R/W		36h: Set 54mA				*/
	ChargerSetDeadBatteryCurrent55mA	= 0x37, /*	R/W		37h: Set 55mA				*/
	ChargerSetDeadBatteryCurrent56mA	= 0x38, /*	R/W		38h: Set 56mA				*/
	ChargerSetDeadBatteryCurrent57mA	= 0x39, /*	R/W		39h: Set 57mA				*/
	ChargerSetDeadBatteryCurrent58mA	= 0x3A, /*	R/W		3Ah: Set 58mA				*/
	ChargerSetDeadBatteryCurrent59mA	= 0x3B, /*	R/W		3Bh: Set 59mA				*/
	ChargerSetDeadBatteryCurrent60mA	= 0x3C, /*	R/W		3Ch: Set 60mA				*/
	ChargerSetDeadBatteryCurrent61mA	= 0x3D, /*	R/W		3Dh: Set 61mA				*/
	ChargerSetDeadBatteryCurrent62mA	= 0x3E, /*	R/W		3Eh: Set 62mA				*/
	ChargerSetDeadBatteryCurrent63mA	= 0x3F	/*	R/W		3Fh: Set 63mA				*/
	/*	Note: Current value set in ICHG_DEAD[5:0] should NOT be greater than the value set in ICHG_LOW[5:0].
	*	Note: The current default value for ICHG_DEAD[5:0] is set at 4mA, ICHG_DEAD[2] and ICHG_DEAD[4] should be MTP programmable. */
} AlxPca9430_0x24_ChargerSetDeadBatteryCurrent;
	
typedef enum
{ /* Linear battery charger dead charge timer setting for battery short detection, */
	ChargerSetDeadBatteryTimer5s	= 0b00, /*	R/W		00: 5s	*/
	ChargerSetDeadBatteryTimer10s	= 0b01, /*	R/W		01: 10s	*/
	ChargerSetDeadBatteryTimer20s	= 0b10, /*	R/W		10: 20s	*/
	ChargerSetDeadBatteryTimer40s	= 0b11	/*	R/W		11: 40s	*/
} AlxPca9430_0x24_ChargerSetDeadBatteryTimer;

typedef union
{
	struct __attribute__((packed))
	{
		AlxPca9430_0x24_ChargerSetDeadBatteryCurrent ICHG_DEAD			: 6;	/* Low battery charge current setting:											*/
		AlxPca9430_0x24_ChargerSetDeadBatteryTimer	 ICHG_DEAD_TIMER	: 2;	/* Linear battery charger dead charge timer setting for battery short detection */
	};
	uint8_t raw;
} AlxPca9430_RegVal_0x24_ChargerSetDeadBatteryCurrent;

/******************************************************************************				PAC943-0_BatterySetRegulatedVoltage	0x25	*/
typedef enum
{ /* VBATREG, linear battery charger regulated battery voltage setting:		*/
	BatterySetRegulatedVoltage3V60		= 0x00, /*	R/W		00h: Set 3.60V				*/
	BatterySetRegulatedVoltage3V62		= 0x01, /*	R/W		01h: Set 3.62V				*/
	BatterySetRegulatedVoltage3V64		= 0x02, /*	R/W		02h: Set 3.64V				*/
	BatterySetRegulatedVoltage3V66		= 0x03, /*	R/W		03h: Set 3.66V				*/
	BatterySetRegulatedVoltage3V68		= 0x04, /*	R/W		04h: Set 3.68V				*/
	BatterySetRegulatedVoltage3V70		= 0x05, /*	R/W		05h: Set 3.70V				*/
	BatterySetRegulatedVoltage3V72		= 0x06, /*	R/W		06h: Set 3.72V				*/
	BatterySetRegulatedVoltage3V74		= 0x07, /*	R/W		07h: Set 3.74V				*/
	BatterySetRegulatedVoltage3V76		= 0x08, /*	R/W		08h: Set 3.76V				*/
	BatterySetRegulatedVoltage3V78		= 0x09, /*	R/W		09h: Set 3.78V				*/
	BatterySetRegulatedVoltage3V80		= 0x0A, /*	R/W		0Ah: Set 3.80V				*/
	BatterySetRegulatedVoltage3V82		= 0x0B, /*	R/W		0Bh: Set 3.82V				*/
	BatterySetRegulatedVoltage3V84		= 0x0C, /*	R/W		0Ch: Set 3.84V				*/
	BatterySetRegulatedVoltage3V86		= 0x0D, /*	R/W		0Dh: Set 3.86V				*/
	BatterySetRegulatedVoltage3V88		= 0x0E, /*	R/W		0Eh: Set 3.88V				*/
	BatterySetRegulatedVoltage3V90		= 0x0F, /*	R/W		0Fh: Set 3.90V				*/
	BatterySetRegulatedVoltage3V92		= 0x10, /*	R/W		10h: Set 3.92V				*/
	BatterySetRegulatedVoltage3V94		= 0x11, /*	R/W		11h: Set 3.94V				*/
	BatterySetRegulatedVoltage3V96		= 0x12, /*	R/W		12h: Set 3.96V				*/
	BatterySetRegulatedVoltage3V98		= 0x13, /*	R/W		13h: Set 3.98V				*/
	BatterySetRegulatedVoltage4V00		= 0x14, /*	R/W		14h: Set 4.00V				*/
	BatterySetRegulatedVoltage4V02		= 0x15, /*	R/W		15h: Set 4.02V				*/
	BatterySetRegulatedVoltage4V04		= 0x16, /*	R/W		16h: Set 4.04V				*/
	BatterySetRegulatedVoltage4V06		= 0x17, /*	R/W		17h: Set 4.06V				*/
	BatterySetRegulatedVoltage4V08		= 0x18, /*	R/W		18h: Set 4.08V				*/
	BatterySetRegulatedVoltage4V10		= 0x19, /*	R/W		19h: Set 4.10V				*/
	BatterySetRegulatedVoltage4V12		= 0x1A, /*	R/W		1Ah: Set 4.12V				*/
	BatterySetRegulatedVoltage4V14		= 0x1B, /*	R/W		1Bh: Set 4.14V				*/
	BatterySetRegulatedVoltage4V16		= 0x1C, /*	R/W		1Ch: Set 4.16V				*/
	BatterySetRegulatedVoltage4V18		= 0x1D, /*	R/W		1Dh: Set 4.18V				*/
	BatterySetRegulatedVoltage4V20		= 0x1E, /*	R/W		1Eh: Set 4.20V	(default)	*/
	BatterySetRegulatedVoltage4V22		= 0x1F, /*	R/W		1Fh: Set 4.22V				*/
	BatterySetRegulatedVoltage4V24		= 0x20, /*	R/W		20h: Set 4.24V				*/
	BatterySetRegulatedVoltage4V26		= 0x21, /*	R/W		21h: Set 4.26V				*/
	BatterySetRegulatedVoltage4V28		= 0x22, /*	R/W		22h: Set 4.28V				*/
	BatterySetRegulatedVoltage4V30		= 0x23, /*	R/W		23h: Set 4.30V				*/
	BatterySetRegulatedVoltage4V32		= 0x24, /*	R/W		24h: Set 4.32V				*/
	BatterySetRegulatedVoltage4V34		= 0x25, /*	R/W		25h: Set 4.34V				*/
	BatterySetRegulatedVoltage4V36		= 0x26, /*	R/W		26h: Set 4.36V				*/
	BatterySetRegulatedVoltage4V38		= 0x27, /*	R/W		27h: Set 4.38V				*/
	BatterySetRegulatedVoltage4V40		= 0x28, /*	R/W		28h: Set 4.40V				*/
	BatterySetRegulatedVoltage4V42		= 0x29, /*	R/W		29h: Set 4.42V				*/
	BatterySetRegulatedVoltage4V44		= 0x2A, /*	R/W		2Ah: Set 4.44V				*/
	BatterySetRegulatedVoltage4V46		= 0x2B, /*	R/W		2Bh: Set 4.46V				*/
	BatterySetRegulatedVoltage4V48		= 0x2C, /*	R/W		2Ch: Set 4.48V				*/
	BatterySetRegulatedVoltage4V50		= 0x2D, /*	R/W		2Dh: Set 4.50V				*/
	BatterySetRegulatedVoltage4V52		= 0x2E, /*	R/W		2Eh: Set 4.52V				*/
	BatterySetRegulatedVoltage4V54		= 0x2F, /*	R/W		2Fh: Set 4.54V				*/
	BatterySetRegulatedVoltage4V56		= 0x30, /*	R/W		30h: Set 4.56V				*/
	BatterySetRegulatedVoltage4V58		= 0x31, /*	R/W		31h: Set 4.58V				*/
	BatterySetRegulatedVoltage4V60		= 0x32, /*	R/W		32h: Set 4.60V				*/
	BatterySetRegulatedVoltage4V60_		= 0x33, /*	R/W		33h: Set 4.60V				*/
	BatterySetRegulatedVoltage4V60_1	= 0x34, /*	R/W		34h: Set 4.60V				*/
	BatterySetRegulatedVoltage4V60_2	= 0x35, /*	R/W		35h: Set 4.60V				*/
	BatterySetRegulatedVoltage4V60_3	= 0x36, /*	R/W		36h: Set 4.60V				*/
	BatterySetRegulatedVoltage4V60_4	= 0x37, /*	R/W		37h: Set 4.60V				*/
	BatterySetRegulatedVoltage4V60_5	= 0x38, /*	R/W		38h: Set 4.60V				*/
	BatterySetRegulatedVoltage4V60_6	= 0x39, /*	R/W		39h: Set 4.60V				*/
	BatterySetRegulatedVoltage4V60_7	= 0x3A, /*	R/W		3Ah: Set 4.60V				*/
	BatterySetRegulatedVoltage4V60_8	= 0x3B, /*	R/W		3Bh: Set 4.60V				*/
	BatterySetRegulatedVoltage4V60_9	= 0x3C, /*	R/W		3Ch: Set 4.60V				*/
	BatterySetRegulatedVoltage4V60_10	= 0x3D, /*	R/W		3Dh: Set 4.60V				*/
	BatterySetRegulatedVoltage4V60_11	= 0x3E, /*	R/W		3Eh: Set 4.60V				*/
	BatterySetRegulatedVoltage4V60_12	= 0x3F	/*	R/W		3Fh: Set 4.60V				*/
} AlxPca9430_0x25_BatterySetRegulatedVoltage;

typedef enum
{ /* VBAT Restart charge threshold: */
	VBatteryRestartChargeTreshold140mV	= 0b0,	/*	R/W		0: 140mV	*/
	VBatteryRestartChargeTreshold240mV	= 0b1	/*	R/W		1: 240mV	*/
} AlxPca9430_0x25_VBatteryRestartChargeTreshold;

typedef union
{
	struct __attribute__((packed))
	{
		AlxPca9430_0x25_BatterySetRegulatedVoltage		VBAT_REG		: 6;	/* VBATREG, linear battery charger regulated battery voltage setting:	*/
		AlxPca9430_0x25_VBatteryRestartChargeTreshold	VBAT_RESTART	: 1;	/* VBAT Restart charge threshold:										*/
		uint8_t unused_7 : 1;		/* bit 7 is unused																									*/
	};
	uint8_t raw;
} AlxPca9430_RegVal_0x25_BatterySetRegulatedVoltage;

/******************************************************************************				PAC943-0_ChargerTimerAndNtc	0x26			*/
typedef enum
{ /* TOPOFF Timer setting: */
	ChargerSetTopOffTimer0min	= 0b00, /*	R/W		00: 0min				*/
	ChargerSetTopOffTimer6min4	= 0b01, /*	R/W		01:	6.4min	(default)	*/
	ChargerSetTopOffTimer12min8	= 0b10, /*	R/W		10: 12.8min				*/
	ChargerSetTopOffTimer19min2	= 0b11	/*	R/W		11:	19.2min				*/
} AlxPca9430_0x26_ChargerSetTopOffTimer;
	
typedef enum
{ /* Linear battery charger pre-qualification charge timer setting: */
	ChargerSetPreQualificationTimer15min	= 0b00, /*	R/W		00: 15min	*/
	ChargerSetPreQualificationTimer30min	= 0b01, /*	R/W		01:	30min	*/
	ChargerSetPreQualificationTimer45min	= 0b10, /*	R/W		10: 45min	*/
	ChargerSetPreQualificationTimer60min	= 0b11	/*	R/W		11:	60min	*/
} AlxPca9430_0x26_ChargerSetPreQualificationTimer;
	
typedef enum
{ /* Linear battery charger fast charge timer setting: */
	ChargerSetFastChargeTimer3h	= 0b00,				/*	R/W		00: 3h	*/
	ChargerSetFastChargeTimer5h	= 0b01,				/*	R/W		01:	5h	*/
	ChargerSetFastChargeTimer7h	= 0b10,				/*	R/W		10: 7h	*/
	ChargerSetFastChargeTimer9h	= 0b11				/*	R/W		11:	9h	*/
} AlxPca9430_0x26_ChargerSetFastChargeTimer;

typedef enum
{ /* Charging Safety Timer Extension: */
	ChargingSafetyTimerExtension2xOff	= 0b0,		/*	R/W		0: Both pre-qual and fast charge timer duration keeps as the values set in ICHG_PREQ_TIMER[1:0] and ICHG_FAST_TIMER[1:0]				*/
	ChargingSafetyTimerExtension2xOn	= 0b1		/*	R/W		1: Both pre-qual and fast charge timer duration is extended to 2x of the values set in ICHG_PREQ_TIMER[1:0] and ICHG_FAST_TIMER[1:0]	*/
} AlxPca9430_0x26_ChargingSafetyTimerExtension2x;
	
typedef enum
{	/* External thermistor typical resistance selection: */
	NtcSetThermistorResistance100k	= 0b0,			/*	R/W		0: 100kΩ;		*/
	NtcSetThermistorResistance10k	= 0b1			/*	R/W		1: 10kΩ			*/
} AlxPca9430_0x26_NtcSetThermistorResistance;

typedef union
{
	struct __attribute__((packed))
	{
		AlxPca9430_0x26_ChargerSetTopOffTimer			T_TOPOFF		: 2;	/* TOPOFF Timer setting:											*/
		AlxPca9430_0x26_ChargerSetPreQualificationTimer	ICHG_PREQ_TIMER	: 2;	/* Linear battery charger pre-qualification charge timer setting:	*/
		AlxPca9430_0x26_ChargerSetFastChargeTimer		ICHG_FAST_TIMER	: 2;	/* Linear battery charger fast charge timer setting:				*/
		AlxPca9430_0x26_ChargingSafetyTimerExtension2x	TIMER_2X		: 1;	/* Charging Safety Timer Extension:									*/
		AlxPca9430_0x26_NtcSetThermistorResistance		NTC_RES_SEL		: 1;	/* External thermistor typical resistance selection:				*/
	};
	uint8_t raw;
} AlxPca9430_RegVal_0x26_ChargerTimerAndNtc;
	
/**************************************************************************		PAC943-0_NtcThermistorAndThermalRegulation	0x27		*/
typedef enum
{ /* Thermal regulation threshold setting: */
	NtcThermalRegulationThreshold90C	= 0b000,	/*	R/W		000:	90°C	*/
	NtcThermalRegulationThreshold95C	= 0b001,	/*	R/W		001:	95°C	*/
	NtcThermalRegulationThreshold100C	= 0b010,	/*	R/W		010:	100°C	*/
	NtcThermalRegulationThreshold105C	= 0b011,	/*	R/W		011:	105°C	*/
	NtcThermalRegulationThreshold110C	= 0b100,	/*	R/W		100:	110°C	*/
	NtcThermalRegulationThreshold115C	= 0b101,	/*	R/W		101:	115°C	*/
	NtcThermalRegulationThreshold120C	= 0b110,	/*	R/W		110:	120°C	*/
	NtcThermalRegulationThreshold125C	= 0b111		/*	R/W		111:	125°C	*/
} AlxPca9430_0x27_NtcThermalRegulationThreshold;

typedef enum
{
	/* Set the thermistor beta value selection: */
	NtcSetThermistorBetaValue3435k = 0b000, /*	R/W		000:	3435k	*/
	NtcSetThermistorBetaValue3610k = 0b001, /*	R/W		001:	3610k	*/
	NtcSetThermistorBetaValue3934k = 0b010, /*	R/W		010:	3934k	*/
	NtcSetThermistorBetaValue3950k = 0b011, /*	R/W		011:	3950k	*/
	NtcSetThermistorBetaValue4100k = 0b100, /*	R/W		100:	4100k	*/
	NtcSetThermistorBetaValue4311k = 0b101, /*	R/W		101:	4311k	*/
	NtcSetThermistorBetaValue4543k = 0b110, /*	R/W		110:	4543k	*/
	NtcSetThermistorBetaValue4750k = 0b111	/*	R/W		111:	4750k	*/
} AlxPca9430_0x27_NtcSetThermistorBetaValue;

typedef union
{
	struct __attribute__((packed))
	{
		AlxPca9430_0x27_NtcThermalRegulationThreshold	THM_REG			: 3;	/* Thermal regulation threshold setting:		*/
		uint8_t unused_3_4 : 2;							/* bits 3...4 are unused												*/
		AlxPca9430_0x27_NtcSetThermistorBetaValue		NTC_BETA_SEL	: 3;	//* Set the thermistor beta value selection:	*/
	};
	uint8_t raw;
} AlxPca9430_RegVal_0x27_NtcThermistorAndThermalRegulation;

/**************************************************************************							PAC943-0_ChargerStatusReg1	0x28	*/
typedef enum
{ /* Charger status, the bit is reset when CHG_EN=1’b0: */
	ChargerStatusOk		= 0b0, /*	R		0: VRECT voltage is invalid. i.e. VRECT_STATUS[1:0] ≠¸0b11		*/
	ChargerStatusNotOk	= 0b1	/*	R		1: VRECT voltage is valid. i.e. VRECT_STATUS[1:0] = 0b11		*/
} AlxPca9430_0x28_ChargerStatus;
	
typedef enum
{	/* VRECT status, refer to VRECT_STATUS [1:0] for more details */
	VRectStatusOk		= 0b0,	/*	R		0: The charger has suspended due to the following conditions: TS_DETAIL_STATUS[2:0] = 001’b or 100’b; or SFTY_TIMER_STATUS[1:0] ≠ 00’b		*/
	VRectStatusNotOk	= 0b1	/*	R		1: The charger is OK																														*/
} AlxPca9430_0x28_VRectStatus;
	
typedef enum
{ /* VBAT status, refer to BAT_DETAIL_STATUS [2:0] for more details. The bit is reset when CHG_EN=1’b0: */
	VBatStatusOk	= 0b0,	/*	R		0: the battery is invalid/missing, or charger reset is active, i.e. BAT_DETAIL_STATUS[2:0] = 0b000, 0b111		*/
	VBatStatusNotOk = 0b1	/*	R		1: the battery is OK. i.e. BAT_DETAIL_STATUS[2:0] = 0b010, 0b011, 0b100, 0b101									*/
} AlxPca9430_0x28_VBatStatus;
	
typedef enum
{	/* VBAT Detection Status, the bit is reset when CHG_EN=1’b0: */
	VBatDetectionStatusOk		= 0b0,	/*	R		0: No valid battery attachment detected.	*/
	VBatDetectionStatusNotOk	= 0b1	/*	R		1: Battery attachment detected.				*/
} AlxPca9430_0x28_VBatDetectionStatus;

typedef union
{
	struct __attribute__((packed))
	{
		uint8_t unused_0_3 : 4;				/* bits 0...3 are unused																									*/
		AlxPca9430_0x28_ChargerStatus		CHG_OK		: 1;	/* Charger status, the bit is reset when CHG_EN=1’b0:													*/
		AlxPca9430_0x28_VRectStatus			VRECT_OK	: 1;	/* VRECT status, refer to VRECT_STATUS [1:0] for more details											*/
		AlxPca9430_0x28_VBatStatus			VBAT_OK		: 1;	/* VBAT status, refer to BAT_DETAIL_STATUS [2:0] for more details. The bit is reset when CHG_EN=1’b0:	*/
		AlxPca9430_0x28_VBatDetectionStatus	VBAT_DET_OK	: 1;	/* VBAT Detection Status, the bit is reset when CHG_EN=1’b0:											*/
	};
	uint8_t raw;
} AlxPca9430_RegVal_0x28_ChargerStatusReg1;

/**************************************************************************							PAC943-0_ChargerStatusReg2	0x29	*/
typedef enum
{ /* Temperature Regulation Loop Status, The bit is reset when CHG_EN=1’b0: */
	TempRegulationLoopStatusOk		= 0b0,	/*	R		0: Die junction temperature is less than the threshold set by THM_REG and the full charge current limit is available.										*/
	TempRegulationLoopStatusNotOk	= 0b1	/*	R		1: Die junction temperature is greater than the threshold set by THM_REG, and the charge current limit may be folding back to reduce power dissipation.		*/
} AlxPca9430_0x29_TempRegulationLoopStatus;
	
typedef enum
{	/* VRECT Status: */
	VRectStatusNotOkInvalidLow	= 0b00, /*	R		00: VRECT is invalid. VRECT< VRECT_UVLO													*/
	VRectStatusNotOkInvalid		= 0b01, /*	R		01: VRECT is invalid. VRECT< VBAT+VRECT2BAT_HEADROOM and VRECT>VRECT_UVLO				*/
	VRectStatusNotOkInvalidHigh	= 0b10, /*	R		10: VRECT is invalid. VRECT>VRECT_OVP													*/
	VRectStatusOkValid			= 0b11	/*	R		11: VRECT is valid. VRECT>VRECT_UVLO, VRECT>VBAT+VRECT2BAT_HEADROOM, VRECT<VRECT_OVP	*/
} AlxPca9430_0x29_VRectStatusReg2;
	
typedef union
{
	struct __attribute__((packed))
	{
		uint8_t unused_0_3	: 4;					/* bits 0...3 are unused																			*/
		AlxPca9430_0x29_TempRegulationLoopStatus	TREG_STATUS		: 1;	/* Temperature Regulation Loop Status, The bit is reset when CHG_EN=1’b0:	*/
		uint8_t unused_5	: 1;					/* bit 5 is unused																					*/
		AlxPca9430_0x29_VRectStatusReg2				VRECT_STATUS	: 2;	/* VRECT Status:															*/
	};
	uint8_t raw;
} AlxPca9430_RegVal_0x29_ChargerStatusReg2;

/**************************************************************************							PAC943-0_ChargerStatusReg3	0x2A	*/
typedef enum
{ /* The bits are reset when CHG_EN=1’b0, */
	ChargerStateIdle			= 0b000,	/*	R		000: Charger in Idle State					*/
	ChargerStateDeadBattery		= 0b001,	/*	R		001: Charger in Dead-Battery State			*/
	ChargerStateLowBattery		= 0b010,	/*	R		010: Charger in Low-Battery State			*/
	ChargerStateFastChargingCC	= 0b011,	/*	R		011: Charger in Fast Charging State (CC)	*/
	ChargerStateFastChargingCV	= 0b100,	/*	R		100: Charger in Fast Charging State (CV)	*/
	ChargerStateTopOff			= 0b101,	/*	R		101: Charger in Top-off State				*/
	ChargerStateDone			= 0b110,	/*	R		110: Charger in Done State					*/
	ChargerStateReserved		= 0b111		/*	R		111: Reserved								*/
} AlxPca9430_0x2A_ChargerState;

typedef enum
{
	/* Battery Details, the bits are reset when CHG_EN=1’b0: */
	BatteryStatusMissing	= 0b000,	/*	R		000: Battery missing, not attached.										*/
	BatteryStatusDetected	= 0b001,	/*	R		001: Battery detection in-progress										*/
	BatteryStatusOk1		= 0b010,	/*	R		010: Battery is valid, VBAT < Vbat_dead level							*/
	BatteryStatusOk2		= 0b011,	/*	R		011: Battery is valid, Vbat_dead level < VBAT < VBAT_LOW level			*/
	BatteryStatusOk3		= 0b100,	/*	R		100: Battery is valid, VBAT_LOW level < VBAT < Vbat_reg – Vbat_restart	*/
	BatteryStatusOk4		= 0b101,	/*	R		101: Battery is valid, VBAT > Vbat_reg – Vbat_restart					*/
	BatteryStatusReserved	= 0b110,	/*	R		110: reserved															*/
	BatteryStatusInReset	= 0b111		/*	R		111: battery charger is in reset										*/
} AlxPca9430_0x2A_BatteryStatus;
	
typedef union
{
	struct __attribute__((packed))
	{
		AlxPca9430_0x2A_ChargerState	BAT_CHG_STATUS		: 3;	/* The bits are reset when CHG_EN=1’b0,						*/
		uint8_t unused_3	: 1;		/* bit 3 is unused																		*/
		AlxPca9430_0x2A_ChargerState	BAT_DETAIL_STATUS	: 3;	/* Battery Details, the bits are reset when CHG_EN=1’b0:	*/
		uint8_t unused_7	: 1;		/* bit 7 is unused																		*/
	};
	uint8_t raw;
} AlxPca9430_RegVal_0x2A_ChargerStatusReg3;

/**************************************************************************							PAC943-0_ChargerStatusReg4	0x2B	*/
typedef enum
{	/* Safety Timer Status: */
	ChargerStatusTimerOk				= 0b00, /*	R		00: Safety Timers having No Effect on Battery Charging									*/
	ChargerStatusTimerPreQualExpires	= 0b01, /*	R		01: Pre-qual Timer expires, battery charging suspended									*/
	ChargerStatusTimerFastExpires		= 0b10, /*	R		10: Fast Timer expires, battery Charging suspended										*/
	ChargerStatusTimerShortFails		= 0b11	/*	R		11: Battery short test fails, battery charging suspended								*/
} AlxPca9430_0x2B_ChargerStatusTimer;
	
typedef enum
{	/* The bits are reset when CHG_EN=1’b0, */
	ChargerStatusTempOk							= 0b00, /*	R		00: Chip Temperature having No effect on Battery Charging								*/
	ChargerStatusTempAboveRegulationTreshold	= 0b01, /*	R		01: Chip Temperature above Regulation Threshold, belowShutdown Threshold				*/
	ChargerStatusTempAboveShutdownTreshold		= 0b10, /*	R		10: Chip Temperature above Shutdown Threshold											*/
	ChargerStatusTempReserved					= 0b11	/*	R		11: Reserved																			*/
} AlxPca9430_0x2B_ChargerStatusTemp;

typedef enum
{
	/* The bits are reset when CHG_EN=1’b0 */
	BatteryTempStatusNominal	= 0b000, /*	R		000: Battery Temperature Nominal, T2 ≤ T ≤ T3	*/
	BatteryTempStatusCold		= 0b001, /*	R		001: Battery Temperature is Cold, T < T1		*/
	BatteryTempStatusCool		= 0b010, /*	R		010: Battery Temperature is Cool, T1 ≤ T < T2	*/
	BatteryTempStatusWarm		= 0b011, /*	R		011: Battery Temperature is Warm, T3 < T ≤ T4	*/
	BatteryTempStatusHot		= 0b100, /*	R		100: Battery Temperature is Hot, T > T4			*/
} AlxPca9430_0x2B_BatteryTempStatus;
	
typedef union
{
	struct __attribute__((packed))
	{
		AlxPca9430_0x2B_ChargerStatusTimer	SFTY_TIMER_STATUS	: 2 ;	/* Safety Timer Status:						*/
		AlxPca9430_0x2B_ChargerStatusTemp	CHIP_TEMP_STATUS	: 2 ;	/* The bits are reset when CHG_EN=1’b0,		*/
		AlxPca9430_0x2B_BatteryTempStatus	TS_DETAIL_STATUS	: 3 ;	/* The bits are reset when CHG_EN=1’b0		*/
		uint8_t unused_7	: 1;			/* bit 7 is unused														*/
	};
	uint8_t raw;
} AlxPca9430_RegVal_0x2B_ChargerStatusReg4;
/*******************************************************************************************			PAC943-0_VRectAdcH		0x30	*/
typedef union
{
	struct __attribute__((packed))
	{
		uint8_t VRECT		: 8;	/* VRectAdc 8...15	VRECT voltage resolution is LSB:15.82 mV	*/
	};
	uint8_t raw;
} AlxPca9430_RegVal_0x30_VRectAdcH;
	
/*******************************************************************************************			PAC943-0_VRectAdcL		0x31	*/
typedef union
{
	struct __attribute__((packed))
	{
		uint8_t unused_0_5	: 6;	/* bits 0...5 are unused										*/
		uint8_t VRECT		: 2;	/* VRectAdc 6...7	VRECT voltage resolution is LSB:15.82 mV	*/
	};
	uint8_t raw;
} AlxPca9430_RegVal_0x31_VRectAdcL;
	
/*******************************************************************************************			PAC943-0_VTuneAdcH		0x32	*/
typedef union
{
	struct __attribute__((packed))
	{
		uint8_t VTUNE		: 8;	/* VRectAdc 8...15	VTUNE voltage resolution is LSB:3.32 mV	*/
	};
	uint8_t raw;
} AlxPca9430_RegVal_0x32_VTuneAdcH;

/*******************************************************************************************			PAC943-0_VTuneAdcL		0x33	*/
typedef union
{
	struct __attribute__((packed))
	{
		uint8_t unused_0_5	: 6;	/* bits 0...5 are unused									*/
		uint8_t VTUNE		: 2;	/* VRectAdc 6...7	VTUNE voltage resolution is LSB:3.32 mV	*/
	};
	uint8_t raw;
} AlxPca9430_RegVal_0x33_VTuneAdcL;
	
/*******************************************************************************************			PAC943-0_VOutAdcH		0x34	*/
typedef union
{
	struct __attribute__((packed))
	{
		uint8_t VOUT		: 8;	/* VRectAdc 8...15	VOUT voltage resolution is LSB:5.27 mV	*/
	};
	uint8_t raw;
} AlxPca9430_RegVal_0x34_VOutAdcH;

/*******************************************************************************************			PAC943-0_VOutAdcL		0x35	*/
typedef union
{
	struct __attribute__((packed))
	{
		uint8_t unused_0_5	: 6;	/* bits 0...5 are unused									*/
		uint8_t VOUT		: 2;	/* VRectAdc 6...7	VOUT voltage resolution is LSB:5.27 mV	*/
	};
	uint8_t raw;
} AlxPca9430_RegVal_0x35_VOutAdcL;

/*******************************************************************************************			PAC943-0_IOutAdcH		0x36	*/
typedef union
{
	struct __attribute__((packed))
	{
		uint8_t IOUT		: 8;	/* VRectAdc 8...15	IOUT voltage resolution is LSB: 421.9uA	*/
	};
	uint8_t raw;
} AlxPca9430_RegVal_0x36_IOutAdcH;

/*******************************************************************************************			PAC943-0_IOutAdcL		0x37	*/
typedef union
{
	struct __attribute__((packed))
	{
		uint8_t unused_0_5	: 6;	/* bits 0...5 are unused									*/
		uint8_t IOUT		: 2;	/* VRectAdc 6...7	IOUT voltage resolution is LSB: 421.9uA	*/
	};
	uint8_t raw;
} AlxPca9430_RegVal_0x37_IOutAdcL;
	
/*******************************************************************************************			PAC943-0_IRectAdcH		0x38	*/
typedef union
{
	struct __attribute__((packed))
	{
		uint8_t IRECT		: 8;		/* VRectAdc 8...15	IOUT voltage resolution is LSB: 421.8uA	*/
		/*	IRECT current is the sum of VOUT and VPWR load current. In application, 
		 *	the operation current can be added to calculate the received power.						*/
	};
	uint8_t raw;
} AlxPca9430_RegVal_0x38_IRectAdcH;

/*******************************************************************************************			PAC943-0_IRectAdcL		0x39	*/
typedef union
{
	struct __attribute__((packed))
	{
		uint8_t unused_0_5	: 6;	/* bits 0...5 are unused									*/
		uint8_t IRECT		: 2;	/* VRectAdc 6...7	IOUT voltage resolution is LSB: 421.8uA	*/
		/*	IRECT current is the sum of VOUT and VPWR load current. In application, 
		 *	the operation current can be added to calculate the received power.					*/
	};
	uint8_t raw;
} AlxPca9430_RegVal_0x39_IRectAdcL;
	
/*******************************************************************************	PAC943-0_AlxPca9430_0x21_ChipTempAdcMeas	0x3A	*/	
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
} AlxPca9430_0x3A_ChipTempAdcMeas;

typedef union
{
	struct __attribute__((packed))
	{
		AlxPca9430_0x3A_ChipTempAdcMeas TDIE : 7;	/* On chip temperature ADC measurement results: */
	};
	uint8_t raw;
} AlxPca9430_RegVal_0x3A_ChipTempAdcMeas;

/*******************************************************************************************			PAC943-0_NTCAdcH		0x3B	*/
typedef union
{
	struct __attribute__((packed))
	{
		uint8_t NTC, : 8;		/* NTC voltage resolution is LSB: 2.64mV	*/
	};
	uint8_t raw;
} AlxPca9430_RegVal_0x3B_NTCAdcH;

/*******************************************************************************************			PAC943-0_NTCAdcL		0x3C	*/
typedef union
{
	struct __attribute__((packed))
	{
		uint8_t unused_0_5	: 6;		/* bits 0...5 are unused					*/
		uint8_t NTC			: 2;		/* NTC voltage resolution is LSB: 2.64mV	*/
	};
	uint8_t raw;
} AlxPca9430_RegVal_0x3C_NTCAdcL;
//******************************************************************************
// Register Structures
//******************************************************************************
typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxPca9430_RegVal_0x00_DeviceId val;
} AlxPca9430_Reg_0x00_DeviceId;	

typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxPca9430_RegVal_0x01_SystemInt val;
} AlxPca9430_Reg_0x01_SystemInt;
	
typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxPca9430_RegVal_0x02_SystemIntMask val;
} AlxPca9430_Reg_0x02_SystemIntMask;

typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxPca9430_RegVal_0x03_VRectInt val;
} AlxPca9430_Reg_0x03_VRectInt;

typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxPca9430_RegVal_0x04_VRectIntMask val;
} AlxPca9430_Reg_0x04_VRectIntMask;

typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxPca9430_RegVal_0x05_BatteryChargerInt val;
} AlxPca9430_Reg_0x05_BatteryChargerInt;
	
typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxPca9430_RegVal_0x06_BatteryChargerIntMask val;
} AlxPca9430_Reg_0x06_BatteryChargerIntMask;
	
/* AlxPca9430_RegVal_0x07_ - unused*/

typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxPca9430_RegVal_0x08_VRectThd val;
} AlxPca9430_Reg_0x08_VRectThd;

typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxPca9430_RegVal_0x09_VRectOvw val;
} AlxPca9430_Reg_0x09_VRectOvw;

typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxPca9430_RegVal_0x0A_TempThd val;
} AlxPca9430_Reg_0x0A_TempThd;	
	
typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxPca9430_RegVal_0x0B_WatchdogSoftStartEnMcu val;
} AlxPca9430_Reg_0x0B_WatchdogSoftStartEnMcu;

typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxPca9430_RegVal_0x0C_VaractorDac val;
} AlxPca9430_Reg_0x0C_VaractorDac;

typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxPca9430_RegVal_0x0D_AdcControl val;
} AlxPca9430_Reg_0x0D_AdcControl;

typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxPca9430_RegVal_0x0E_AdcSampleEn val;
} AlxPca9430_Reg_0x0E_AdcSampleEn;	

	typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxPca9430_RegVal_0x0F_VPwrLdoConf val;
} AlxPca9430_Reg_0x0F_VPwrLdoConf;

typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxPca9430_RegVal_0x10_Rxir_Conf val;
} AlxPca9430_Reg_0x10_Rxir_Conf;

typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxPca9430_RegVal_0x20_ChargerCCSetLock val;
} AlxPca9430_Reg_0x20_ChargerCCSetLock;

typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxPca9430_RegVal_0x21_ChargerSetCC val;
} AlxPca9430_Reg_0x21_ChargerSetCC;
	
typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxPca9430_RegVal_0x22_ChargerSetTopOffCurrent val;
} AlxPca9430_Reg_0x22_ChargerSetTopOffCurrent;
	
typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxPca9430_RegVal_0x23_ChargerSetLowBatteryCurrent val;
} AlxPca9430_Reg_0x23_ChargerSetLowBatteryCurrent;
	
typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxPca9430_RegVal_0x24_ChargerSetDeadBatteryCurrent val;
} AlxPca9430_Reg_0x24_ChargerSetDeadBatteryCurrent;
	
typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxPca9430_RegVal_0x25_BatterySetRegulatedVoltage val;
} AlxPca9430_Reg_0x25_BatterySetRegulatedVoltage;
	
typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxPca9430_RegVal_0x26_ChargerTimerAndNtc val;
} AlxPca9430_Reg_0x26_ChargerTimerAndNtc;
	
typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxPca9430_RegVal_0x27_NtcThermistorAndThermalRegulation val;
} AlxPca9430_Reg_0x27_NtcThermistorAndThermalRegulation;
	
typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxPca9430_RegVal_0x28_ChargerStatusReg1 val;
} AlxPca9430_Reg_0x28_ChargerStatusReg1;
	
typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxPca9430_RegVal_0x29_ChargerStatusReg2 val;
} AlxPca9430_Reg_0x29_ChargerStatusReg2;
	
typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxPca9430_RegVal_0x2A_ChargerStatusReg3 val;
} AlxPca9430_Reg_0x2A_ChargerStatusReg3;
	
typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxPca9430_RegVal_0x2B_ChargerStatusReg4 val;
} AlxPca9430_Reg_0x2B_ChargerStatusReg4;	

typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxPca9430_RegVal_0x30_VRectAdcH val;
} AlxPca9430_Reg_0x30_VRectAdcH;	

	typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxPca9430_RegVal_0x31_VRectAdcL val;
} AlxPca9430_Reg_0x31_VRectAdcL;

typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxPca9430_RegVal_0x32_VTuneAdcH val;
} AlxPca9430_Reg_0x32_VTuneAdcH;

typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxPca9430_RegVal_0x33_VTuneAdcL val;
} AlxPca9430_Reg_0x33_VTuneAdcL;

typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxPca9430_RegVal_0x34_VOutAdcH val;
} AlxPca9430_Reg_0x34_VOutAdcH;

	typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxPca9430_RegVal_0x35_VOutAdcL val;
} AlxPca9430_Reg_0x35_VOutAdcL;	

	typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxPca9430_RegVal_0x36_IOutAdcH val;
} AlxPca9430_Reg_0x36_IOutAdcH;

typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxPca9430_RegVal_0x37_IOutAdcL val;
} AlxPca9430_Reg_0x37_IOutAdcL;

typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxPca9430_RegVal_0x38_IRectAdcH val;
} AlxPca9430_Reg_0x38_IRectAdcH;

typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxPca9430_RegVal_0x39_IRectAdcL val;
} AlxPca9430_Reg_0x39_IRectAdcL;
	
typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxPca9430_RegVal_0x3A_ChipTempAdcMeas val;
} AlxPca9430_Reg_0x3A_ChipTempAdcMeas;

typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxPca9430_RegVal_0x3B_NTCAdcH val;
} AlxPca9430_Reg_0x3B_NTCAdcH;

typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxPca9430_RegVal_0x3C_NTCAdcL val;
} AlxPca9430_Reg_0x3C_NTCAdcL;
//******************************************************************************
// Main Register Structure
//******************************************************************************
typedef struct
{
	AlxPca9430_Reg_0x00_DeviceId							_0x00_Device_ID				;
	AlxPca9430_Reg_0x01_SystemInt							_0x01_SYSTEM_INT			;
	AlxPca9430_Reg_0x02_SystemIntMask						_0x02_SYSTEM_INT_MASK		;
	AlxPca9430_Reg_0x03_VRectInt							_0x03_VRECT_INT				;
	AlxPca9430_Reg_0x04_VRectIntMask						_0x04_VRECT_INT_MASK		;
	AlxPca9430_Reg_0x05_BatteryChargerInt					_0x05_BATCHG_INT			;
	AlxPca9430_Reg_0x06_BatteryChargerIntMask				_0x06_BATCHG_INT_MASK		;
	AlxPca9430_Reg_0x08_VRectThd							_0x08_VRECT_THD				;
	AlxPca9430_Reg_0x09_VRectOvw							_0x09_VRECT_OVW				;
	AlxPca9430_Reg_0x0A_TempThd								_0x0A_TEMP_THD				;
	AlxPca9430_Reg_0x0B_WatchdogSoftStartEnMcu				_0x0B_WD_EN_RST				;
	AlxPca9430_Reg_0x0C_VaractorDac							_0x0C_Varactor_DAC			;
	AlxPca9430_Reg_0x0D_AdcControl							_0x0D_ADC_CONTROL			;
	AlxPca9430_Reg_0x0E_AdcSampleEn							_0x0E_Sample_EN				;
	AlxPca9430_Reg_0x0F_VPwrLdoConf							_0x0F_VPWR_CONFIG			;
	AlxPca9430_Reg_0x10_Rxir_Conf							_0x10_RXIR_CONFIG			;
	AlxPca9430_Reg_0x20_ChargerCCSetLock					_0x20_BQ_CONTROL			;
	AlxPca9430_Reg_0x21_ChargerSetCC						_0x21_CC_CURRENT			;
	AlxPca9430_Reg_0x22_ChargerSetTopOffCurrent				_0x22_TP_CURRENT			;
	AlxPca9430_Reg_0x23_ChargerSetLowBatteryCurrent			_0x23_LB_CURRENT			;
	AlxPca9430_Reg_0x24_ChargerSetDeadBatteryCurrent		_0x24_DB_CURRENT			;
	AlxPca9430_Reg_0x25_BatterySetRegulatedVoltage			_0x25_VBAT_REG				;
	AlxPca9430_Reg_0x26_ChargerTimerAndNtc					_0x26_BQTIMER_AND_NTC		;
	AlxPca9430_Reg_0x27_NtcThermistorAndThermalRegulation	_0x27_BQTHM_AND_SHRT		;
	AlxPca9430_Reg_0x28_ChargerStatusReg1					_0x28_Batterychargerstatus1	;
	AlxPca9430_Reg_0x29_ChargerStatusReg2					_0x29_Batterychargerstatus2	;
	AlxPca9430_Reg_0x2A_ChargerStatusReg3					_0x2A_Batterychargerstatus3	;
	AlxPca9430_Reg_0x2B_ChargerStatusReg4					_0x2B_Batterychargerstatus4	;
	AlxPca9430_Reg_0x30_VRectAdcH							_0x30_VRECT_ADC_H			;
	AlxPca9430_Reg_0x31_VRectAdcL							_0x31_VRECT_ADC_L			;
	AlxPca9430_Reg_0x32_VTuneAdcH							_0x32_VTUNE_ADC_H			;
	AlxPca9430_Reg_0x33_VTuneAdcL							_0x33_VTUNE_ADC_L			;
	AlxPca9430_Reg_0x34_VOutAdcH							_0x34_VOUT_ADC_H			;
	AlxPca9430_Reg_0x35_VOutAdcL							_0x35_VOUT_ADC_L			;
	AlxPca9430_Reg_0x36_IOutAdcH							_0x36_IOUT_ADC_H			;
	AlxPca9430_Reg_0x37_IOutAdcL							_0x37_IOUT_ADC_L			;
	AlxPca9430_Reg_0x38_IRectAdcH							_0x38_IRECT_ADC_H			;
	AlxPca9430_Reg_0x39_IRectAdcL							_0x39_IRECT_ADC_L			;
	AlxPca9430_Reg_0x3A_ChipTempAdcMeas						_0x3A_TDIE_ADC				;
	AlxPca9430_Reg_0x3B_NTCAdcH								_0x3B_NTC_ADC_H				;
	AlxPca9430_Reg_0x3C_NTCAdcL								_0x3C_NTC_ADC_L				;
} AlxPca9430_Reg;


//******************************************************************************
// Types
//******************************************************************************

typedef struct
	{
		// Parameters - Const

		// Objects - External
		AlxI2c* i2c;
		
		AlxIoPin* do_SleepEn;	// enable / disable sleep modo for PCA9430
		AlxIoPin* di_Interrupt;	// Pca9430 interrupt

		// Parameters
		uint8_t i2cAddr;
		bool i2cCheckWithRead;
		uint8_t i2cNumOfTries;
		uint16_t i2cTimeout_ms;

		// Variables
		AlxPca9430_Reg reg;

		// Info
		bool isInit;
		bool wasCtorCalled;
	} AlxPca9430;

//******************************************************************************
// Constructor
//******************************************************************************
	void AlxPca9430_Ctor
(
	AlxPca9430* me,
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

Alx_Status AlxPca9430_Init(AlxPca9430* me);
Alx_Status AlxPca9430_DeInit(AlxPca9430* me);
	
	
//Alx_Status AlxPca9431_ChargerVout_SetVoltage_V(AlxPca943* me, AlxPca9431_0x07_VOutLdo VoutLdoxV); // 3,3V or 5V => write:  VoutLdo3V3 or. VoutLdo5V
Alx_Status AlxPca9430_ChargerVout_GetVoltage_V(AlxPca9430* me, float* voltage_V); // 10 bit ADC
Alx_Status AlxPca9430_ChargerVout_GetCurrent_A(AlxPca9430* me, float* current_A); // 10 bit ADC
//Alx_Status AlxPca9431_ChargerVPwr_SetVoltage_V(AlxPca9431* me, AlxPca9431_0x0F_VPwrOutSet VPwrOutxVx); // 1.8V, 2.5V, 2.8V, 3.3V  => write:  VPwrOut1V8 or. VPwrOut2V5 or. VPwrOut2V8 or. VPwrOut3V3
	
	
//Alx_Status AlxPca9430_Rect_Uvlo_SetVoltage_V(AlxPca9430* me, AlxPca9430_0x08_VRectThd VRectThd3Vx); // 3V, 3.1V, 3.2V, 3.3V  => write: VRectThd3V or. VRectThd3V1 or. VRectThd3V2 or. VRectThd3V3
//Alx_Status AlxPca9430_Rect_Ovw_SetVoltage_V(AlxPca9430* me, AlxPca9430_0x09_VRectOvw VRectThd7Vx); // 7V, 7.25V, 7.5V, 7.75V  => write: VRectThd7V or. VRectThd7V25 or. VRectThd7V5 or. VRectThd7V75
Alx_Status AlxPca9430_Rect_GetVoltage_V(AlxPca9430* me, float* voltage_V); // 10 bit ADC
Alx_Status AlxPca9430_Rect_GetCurrent_A(AlxPca9430* me, float* current_A); // 10 bit ADC


//Alx_Status AlxPca9430_TempSens_SetWarning_C(AlxPca9430* me, AlxPca9430_0x0A_TempWarnThd TempWarnThdx0C); // 80°C, 90°C ... 110°C =>  write: TempWarnThd80C or. ... or. TempWarnThd110C
//Alx_Status AlxPca9430_TempSens_SetShutdown_C(AlxPca9430* me, AlxPca9430_0x0A_TempShutdownThd TempShutdownThd1xxC); // 120°C, 125°C ... 150°C =>  write: TempShutdownThd120C or. ... or. TempShutdownThd150C
Alx_Status AlxPca9430_TempSens_GetTemp_degC(AlxPca9430* me, float* temp_degC); // On chip temperature (-43°C ... +156°C)
	
Alx_Status AlxPca9430_VTune_SetVoltage_V(AlxPca9430* me, float* voltage_V); // 0-3.3V 5 bit DAC - TODO
Alx_Status AlxPca9430_VTune_GetVoltage_V(AlxPca9430* me, float* voltage_V); // 10 bit ADC - TODO


//Alx_Status AlxPca9430_Rxir_SetDuration_s(AlxPca9430* me, AlxPca9430_0x10_RxirPullDownDuration RxirPullDownxmsx); // 0.2s, 0.4s, ... , 1.6s =>  write: RxirPullDown0ms2 or. ... or. RxirPullDown1ms6
//Alx_Status AlxPca9430_Rxir_SetResistance_ohm(AlxPca9430* me, AlxPca9430_0x10_RxirPullDownR RxirPullDownx0R); // 20ohm, 80ohm, ... , 440ohm =>  write: RxirPullDown20R or. ... or. RxirPullDown440R	
	

	
Alx_Status AlxPca9430_Ntc_GetTemp_degC(AlxPca9430* me, float* temp_degC); // external NTC voltage / battery temperature measurement - TODO

Alx_Status AlxPca9430_Reg_ReadAndClearInterrupt(AlxPca9430* me);
	
bool AlxPca9430_IsSleep(AlxPca9430* me);
	
#ifdef __cplusplus
}
#endif

#endif // ALX_ADAU1961_H
