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
#define ALX_CRN120_FILE "alxCrn120"
	
// Assert //
#if defined(_ALX_CRN120_ASSERT_BKPT) || defined(_ALX_ASSERT_BKPT_ALL)
#define ALX_CRN120_ASSERT(expr) ALX_ASSERT_BKPT(ALX_CRN120_FILE, expr)
#elif defined(_ALX_CRN120_ASSERT_TRACE) || defined(_ALX_ASSERT_TRACE_ALL)
#define ALX_CRN120_ASSERT(expr) ALX_ASSERT_TRACE(ALX_CRN120_FILE, expr)
#elif defined(_ALX_CRN120_ASSERT_RST) || defined(_ALX_ASSERT_RST_ALL)
#define ALX_CRN120_ASSERT(expr) ALX_ASSERT_RST(ALX_CRN120_FILE, expr)
#else
#define ALX_CRN120_ASSERT(expr) do{} while (false)
#endif

// Trace //
#if defined(_ALX_CRN120_TRACE) || defined(_ALX_TRACE_ALL)
#define ALX_CRN120_TRACE(...) ALX_TRACE_STD(ALX_CRN120_FILE, __VA_ARGS__)
#else
#define ALX_CRN120_TRACE(...) do{} while (false)
#endif


//******************************************************************************
// Register Values Unions
//******************************************************************************
// General Enums //

/*******************************************************************************************				Crn120			0x00	*/

typedef enum
{
	StaticLock_Block_CC_Off	= 0b0,		/*	R/W		0: Block_CC read / write	*/
	StaticLock_Block_CC_On	= 0b1		/*	R/W		1: Block_CC read only		*/
} AlxCrn120_0x00_StaticLock_Block_CC;

typedef enum
{
	StaticLock_Block_9_4_Off	= 0b0, /*	R/W		0: Block_Page_9_4 read / write	*/
	StaticLock_Block_9_4_On		= 0b1	/*	R/W		1: Block_Page_9_4 read only		*/
} AlxCrn120_0x00_StaticLock_Block_9_4;

typedef enum
{
	StaticLock_Block_15_10_Off	= 0b0,	/*	R/W		0: Block_Page_15_10 read / write	*/
	StaticLock_Block_15_10_On	= 0b1	/*	R/W		1: Block_Page_15_10 read only		*/
} AlxCrn120_0x00_StaticLock_Block_15_10;

typedef enum
{
	StaticLock_Page_CC_Off	= 0b0,		/*	R/W		0: Page_CC read / write		*/
	StaticLock_Page_CC_On	= 0b1		/*	R/W		1: Page_CC read only		*/
} AlxCrn120_0x00_StaticLock_Page_CC;

typedef enum
{
	StaticLock_Page_4_Off	= 0b0,		/*	R/W		0: Page_4 read / write		*/
	StaticLock_Page_4_On	= 0b1		/*	R/W		1: Page_4 read only			*/
} AlxCrn120_0x00_StaticLock_Page_4;

typedef enum
{
	StaticLock_Page_5_Off	= 0b0,		/*	R/W		0: Page_5 read / write		*/
	StaticLock_Page_5_On	= 0b1		/*	R/W		1: Page_5 read only			*/
} AlxCrn120_0x00_StaticLock_Page_5;

typedef enum
{
	StaticLock_Page_6_Off	= 0b0,		/*	R/W		0: Page_6 read / write		*/
	StaticLock_Page_6_On	= 0b1		/*	R/W		1: Page_6 read only			*/
} AlxCrn120_0x00_StaticLock_Page_6;

typedef enum
{
	StaticLock_Page_7_Off	= 0b0,		/*	R/W		0: Page_7 read / write		*/
	StaticLock_Page_7_On	= 0b1		/*	R/W		1: Page_7 read only			*/
} AlxCrn120_0x00_StaticLock_Page_7;
	
typedef enum
{
	StaticLock_Page_8_Off	= 0b0,		/*	R/W		0: Page_8 read / write		*/
	StaticLock_Page_8_On	= 0b1		/*	R/W		1: Page_8 read only			*/
} AlxCrn120_0x00_StaticLock_Page_8;

typedef enum
{
	StaticLock_Page_9_Off	= 0b0,		/*	R/W		0: Page_9 read / write		*/
	StaticLock_Page_9_On	= 0b1		/*	R/W		1: Page_9 read only			*/
} AlxCrn120_0x00_StaticLock_Page_9;

typedef enum
{
	StaticLock_Page_10_Off	= 0b0,		/*	R/W		0: Page_10 read / write		*/
	StaticLock_Page_10_On	= 0b1		/*	R/W		1: Page_10 read only		*/
} AlxCrn120_0x00_StaticLock_Page_10;

typedef enum
{
	StaticLock_Page_11_Off	= 0b0,		/*	R/W		0: Page_11 read / write		*/
	StaticLock_Page_11_On	= 0b1		/*	R/W		1: Page_11 read only		*/
} AlxCrn120_0x00_StaticLock_Page_11;

typedef enum
{
	StaticLock_Page_12_Off	= 0b0,		/*	R/W		0: Page_12 read / write		*/
	StaticLock_Page_12_On	= 0b1		/*	R/W		1: Page_12 read only		*/
} AlxCrn120_0x00_StaticLock_Page_12;

typedef enum
{
	StaticLock_Page_13_Off	= 0b0,		/*	R/W		0: Page_13 read / write		*/
	StaticLock_Page_13_On	= 0b1		/*	R/W		1: Page_13 read only		*/
} AlxCrn120_0x00_StaticLock_Page_13;

typedef enum
{
	StaticLock_Page_14_Off	= 0b0,		/*	R/W		0: Page_14 read / write		*/
	StaticLock_Page_14_On	= 0b1		/*	R/W		1: Page_14 read only		*/
} AlxCrn120_0x00_StaticLock_Page_14;

typedef enum
{
	StaticLock_Page_15_Off	= 0b0,		/*	R/W		0: Page_15 read / write		*/
	StaticLock_Page_15_On	= 0b1		/*	R/W		1: Page_15 read only		*/
} AlxCrn120_0x00_StaticLock_Page_15;
	
typedef union
{
	struct __attribute__((packed))
	{
		uint8_t	 Addr									: 8;	/* 	Adrr.									R/W		1 byte			*/
		uint64_t Serial_number							: 48;	/* 	Serial number (UID)						R/W		6 bytes			*/
		uint32_t Internal								: 24;	/* 	Internal								R/W		3 bytes			*/
		
		AlxCrn120_0x00_StaticLock_Block_CC		BLCC	: 1;	/* 	Static lock Block_CC					R/W		1 of 2 bytes	*/
		AlxCrn120_0x00_StaticLock_Block_9_4	BL9_14	: 1;	/* 	Static lock Block_9_4					R/W		1 of 2 bytes	*/
		AlxCrn120_0x00_StaticLock_Block_15_10	BL15_10	: 1;	/* 	Static lock Block_15_10					R/W		1 of 2 bytes	*/
		AlxCrn120_0x00_StaticLock_Page_CC		LCC		: 1;	/* 	Static lock Page_CC						R/W		1 of 2 bytes	*/
		AlxCrn120_0x00_StaticLock_Page_4		L4		: 1;	/* 	Static lock Page_4						R/W		1 of 2 bytes	*/
		AlxCrn120_0x00_StaticLock_Page_5		L5		: 1;	/* 	Static lock Page_5						R/W		1 of 2 bytes	*/
		AlxCrn120_0x00_StaticLock_Page_6		L6		: 1;	/* 	Static lock Page_6						R/W		1 of 2 bytes	*/
		AlxCrn120_0x00_StaticLock_Page_7		L7		: 1;	/* 	Static lock Page_7						R/W		1 of 2 bytes	*/
		
		AlxCrn120_0x00_StaticLock_Page_8		L8		: 1;	/* 	Static lock Page_8						R/W		2 of 2 bytes	*/
		AlxCrn120_0x00_StaticLock_Page_9		L9		: 1;	/* 	Static lock Page_9						R/W		2 of 2 bytes	*/
		AlxCrn120_0x00_StaticLock_Page_10		L10		: 1;	/* 	Static lock Page_10						R/W		2 of 2 bytes	*/
		AlxCrn120_0x00_StaticLock_Page_11		L11		: 1;	/* 	Static lock Page_11						R/W		2 of 2 bytes	*/
		AlxCrn120_0x00_StaticLock_Page_12		L12		: 1;	/* 	Static lock Page_12						R/W		2 of 2 bytes	*/
		AlxCrn120_0x00_StaticLock_Page_13		L13		: 1;	/* 	Static lock Page_13						R/W		2 of 2 bytes	*/
		AlxCrn120_0x00_StaticLock_Page_14		L14		: 1;	/* 	Static lock Page_14						R/W		2 of 2 bytes	*/
		AlxCrn120_0x00_StaticLock_Page_15		L15		: 1;	/* 	Static lock Page_15						R/W		2 of 2 bytes	*/
		
		uint32_t Capability_Container_CC				: 32;	/* 	Capability Container (CC)				R/W		4 bytes			*/
	};
	uint8_t raw[16];
	
} AlxCrn120_RegBlockVal_0x00_Block_0;

/*******************************************************************************************				Crn120			0x38	*/
typedef union
{
	struct __attribute__((packed))
	{
		uint64_t Protected_user_memory	: 64;		/* 	Last 8 bytes of Protected user memory	R/W		8 bytes			*/
		uint32_t Dynamic_lock_bytes		: 24;		/* 	Dynamic lock bytes						R/W		3 bytes			*/
		uint8_t _00h					: 8;		/* 	00h										R/W		1 byte			*/
		uint32_t RFU					: 24;		/* 	RFU										R/W		3 bytes			*/
		uint8_t AUTH0					: 8;		/* 	AUTH0									R/W		1 byte			*/

	};
	uint8_t raw[16];
} AlxCrn120_RegBlockVal_0x38_Block_38;

/*******************************************************************************************				Crn120			0x39	*/
typedef union
{
	struct __attribute__((packed))
	{
		uint8_t ACCESS	: 8;				/* 	ACCESS									R/W		1 byte			*/
		uint32_t RFU	: 24;				/* 	RFU										R/W		3 bytes			*/
		uint32_t PWD	: 24;				/* 	PWD										R/W		3 bytes			*/
		uint16_t PACK	: 16;				/* 	PACK									R/W		2 bytes			*/
		uint16_t RFU_	: 16;				/* 	RFU										R/W		2 bytes			*/
		uint8_t PT_I2C	: 8;				/* 	PT_I2C									R/W		1 byte			*/
		uint32_t RFU_2	: 24;				/* 	RFU										R/W		3 bytes			*/
	};
	uint8_t raw[16];
} AlxCrn120_RegBlockVal_0x39_Block_39;
	
/*******************************************************************************************				Crn120			0x3A	*/
typedef union
{
	struct __attribute__((packed))
	{
		uint8_t NR_REG				: 8;	/* 	NR_REG									R/W		1 byte			*/
		uint8_t LAST_NDEF_BLOCK		: 8;	/* 	LAST_NDEF_BLOCK							R/W		1 byte			*/
		uint8_t SRAM_MIRROR_BLOCK	: 8;	/* 	SRAM_MIRROR_BLOCK						R/W		1 byte			*/
		uint8_t WDT_LS				: 8;	/* 	WDT_LS									R/W		1 byte			*/
		uint8_t WDT_MS				: 8;	/* 	WDT_MS									R/W		1 byte			*/
		uint8_t I2C_CLOCK_STR		: 8;	/* 	I2C_CLOCK_STR							R/W		1 byte			*/
		uint8_t REG_LOCK			: 8;	/* 	REG_LOCK								R/W		1 byte			*/
		uint8_t RFU					: 8;	/* 	RFU										R/W		1 byte			*/
		uint64_t _00h				: 64;	/* 	00h										R		8 bytes			*/
	};
	uint8_t raw[16];
} AlxCrn120_RegBlockVal_0x3A_Block_ConfigurationReg;

/*******************************************************************************************				Crn120_SessionReg	0xFE	*/
	
	/********		NcReg		******/
	
typedef enum
{	/*defines the data flow direction when pass-through mode is enabled*/
	TransferData_I2C_to_NFC = 0b0,	/*	R/W		0b: from I2C to NFC interface										*/
	TransferData_NFC_to_I2C = 0b1	/*	R/W		1b: from NFC to I2C interface										*/
	/*	In case the pass-through mode is NOT enabled, this bit should be set to 1b, otherwise there is no WRITE
	access from the NFC perspective																					*/
} AlxCrn120_0xFE_NcReg_0_TransferDir;
	
typedef enum
{
	SramMirrorDisable	= 0b0,	/*	R/W		0b: SRAM mirror disabled													*/
	SramMirrorEnable	= 0b1	/*	R/W		1b: SRAM mirror enabled and mirrored SRAM starts at page SRAM_MIRROR_BLOCK	*/
} AlxCrn120_0xFE_NcReg_1_SramMirrorOnOff;
	
typedef enum
{	/*defines the event upon which the signal output on the ED pin is pulled low*/
	RFU							= 0b00, /*	R/W		00b: RFU																						*/
	ValidStartOfCommunication	= 0b01,	/*	R/W		01b: by first valid start of communication (SoC)												*/
	TagSelect					= 0b10, /*	R/W		10b: by selection of the tag																	*/
	DataReadByNtcOrI2c			= 0b11	/*	R/W		11b: (in pass-through mode NFC-->I2C) if the data is ready to be read from the I2C interface	*/
										/*			11b: (in pass-through mode I2C--> NFC) if the data is read by the NFC interface					*/
} AlxCrn120_0xFE_NcReg_2_3_EdOn;
	
typedef enum
{	/*defines the event upon which the signal output on the FD pin is released*/
	FieldOff							= 0b00, /*	R/W		00b: if the field is switched off																																						*/
	FieldOffOrHaltOn					= 0b01, /*	R/W		01b: if the field is switched off or the tag is set to the HALT state																													*/
	FieldOffOrReadLastPageNdef			= 0b10, /*	R/W		10b: if the field is switched off or the last page of the NDEF message has been read (defined in LAST_NDEF_BLOCK)																		*/
	FieldOffOrReadLastPageByNfcOrI2c	= 0b11	/*	R/W		11b: (if ED_ON = 11b) if the field is switched off or if last data is read by I2C (in pass-through mode NFC ---> I2C) or last data is written by I2C (in pass-through mode I2C---> NFC)	*/
										/*			11b: (if ED_ON = 00b or 01b or 10b) if the field is switched off																														*/
} AlxCrn120_0xFE_NcReg_4_5_EdOff;
	
typedef enum
{
	SramPassThroughModeDisable	= 0b0,	/*	R/W		0b: pass-through mode disabled													*/
	SramPassThroughModeEnable	= 0b1	/*	R/W		1b: pass-through mode using SRAM enabled and SRAM mapped to end of Sector 0.	*/
} AlxCrn120_0xFE_NcReg_6_PthruOnOff;
	
typedef enum
{	/*Enables the NFC disable feature and enables soft reset through I2C repeated start*/
	NfcSoftResetOn	= 0b0,	/*	R/W		0b: 	*/
	NfcSoftResetOff	= 0b1	/*	R/W		1b: 	*/
} AlxCrn120_0xFE_NcReg_7_NfcsI2cRstOnOff;
	
	/**********			I2C_CLOCK_STR		*******/
	
typedef enum
{ /*	Enables (1b) or disable (0b) the I2C clock stretching	*/
	ClockStretchingDisable	= 0b0, /*	R/W		0b:disable	*/
	ClockStretchingEnable	= 0b1	/*	R/W		1b:Enables 	*/
} AlxCrn120_0xFE_I2cClockStr_0_ClockStretching;
	
typedef enum
{ /*	Status bit to show the number of negative PWD_AUTH attempts reached	*/
	AuthLockNotReached	= 0b0, /*	R/W		0b: PWD_AUTH still possible	*/
	AuthLockReached		= 0b1	/*	R/W		1b: PWD_AUTH locked		 	*/
} AlxCrn120_0xFE_I2cClockStr_1_NegAuthReached;
	

	
	/**********		7	NS_REG		*******/
typedef enum
{ /*	I2C Configuration Lock Bit		*/
	NfcFieldNotDetect	= 0b0, /*	R/W		0b: NFC field is not detected	*/
	NfcFieldDetect		= 0b1	/*	R/W		1b: NFC field is detected		*/
} AlxCrn120_0xFE_7_NsReg_0_RfFieldDetect;
	typedef enum
{ /*	I2C Configuration Lock Bit		*/
	EepromWriteCyclePossible	= 0b0, /*	R/W		0b: EEPROM access possible											*/
	EepromWriteCycleBusy		= 0b1	/*	R/W		1b: EEPROM write cycle in progress - access to EEPROM disabled		*/
} AlxCrn120_0xFE_7_NsReg_1_EepromWriteCycle;
	typedef enum
{ /*	I2C Configuration Lock Bit		*/
	EepromWriteOk		= 0b0,	/*	R/W		0b: Needs to be written back via I2C to 0b to be cleared 														*/
	EepromWriteError	= 0b1	/*	R/W		1b: HV voltage error during EEPROM write or erase cycle.Needs to be written back via I2C to 0b to be cleared	*/
} AlxCrn120_0xFE_7_NsReg_2_EepromWriteError;
	typedef enum
{ /*	I2C Configuration Lock Bit		*/
	SramNfcNotReady = 0b0,	/*	R/W		0b:														*/
	SramNfcReady	= 0b1	/*	R/W		1b: data is ready in SRAM buffer to be read by NFC		*/
} AlxCrn120_0xFE_7_NsReg_3_SramNfcReady;
	typedef enum
{ /*	I2C Configuration Lock Bit		*/
	SramI2cNotReady = 0b0,	/*	R/W		0b:														*/
	SramI2cReady	= 0b1	/*	R/W		1b: data is ready in SRAM buffer to be read by I2C		*/
} AlxCrn120_0xFE_7_NsReg_4_SramI2cReady;
	typedef enum
{ /*	I2C Configuration Lock Bit		*/
	MemoryAccessNfcNotLocked	= 0b0, /*	R/W		0b:														*/
	MemoryAccessNfcLocked		= 0b1	/*	R/W		1b: Memory access is locked to the NFC interface		*/
} AlxCrn120_0xFE_7_NsReg_5_MemoryAccessNfcLocked;
	typedef enum
{ /*	I2C Configuration Lock Bit		*/
	MemoryAccessI2cNotLocked	= 0b0, /*	R/W		0b:														*/
	MemoryAccessI2cLocked		= 0b1	/*	R/W		1b: Memory access is locked to the I2C interface		*/
} AlxCrn120_0xFE_7_NsReg_6_MemoryAccessI2cLocked;
	
	typedef enum
{ /*	I2C Configuration Lock Bit		*/
	NdefDataReadAll		= 0b0, /*	R/W		0b:														*/
	NdefDataReadNotAll	= 0b1	/*	R/W		1b: all data bytes read from the address specified in LAST_NDEF_BLOCK. Bit is reset to 0b when read		*/
} AlxCrn120_0xFE_7_NsReg_7_NdefDataRead;
	
	/**********		8	REG_LOCK		*******/
typedef enum
{ /*	I2C Configuration Lock Bit		*/
	NfcConfigLockEnabled	= 0b0, /*	R/W		0b: Configuration bytes may be changed via I2C														*/
	NfcConfigLockDisable	= 0b1	/*	R/W		1b: Configuration bytes cannot be changed via I2C Once set to 1b, cannot be reset to 0b anymore. 	*/
} AlxCrn120_0xFE_8_RegLock_Rfu_0_NfcConfigLock;
typedef enum
{ /*	I2C Configuration Lock Bit		*/
	I2cConfigLockEnabled	= 0b0, /*	R/W		0b: Configuration bytes may be changed via I2C														*/
	I2cConfigLockDisable	= 0b1	/*	R/W		1b: Configuration bytes cannot be changed via I2C Once set to 1b, cannot be reset to 0b anymore. 	*/
} AlxCrn120_0xFE_8_RegLock_Rfu_1_I2cConfigLock;
typedef union
{
	struct __attribute__((packed))
	{
		uint8_t NC_REG				: 8;	/* 	NC_REG									R/W		1 byte			*/
		uint8_t LAST_NDEF_BLOCK		: 8;	/* 	LAST_NDEF_BLOCK							R/W		1 byte			*/
		uint8_t SRAM_MIRROR_BLOCK	: 8;	/* 	SRAM_MIRROR_BLOCK						R/W		1 byte			*/
		uint8_t WDT_LS				: 8;	/* 	WDT_LS									R/W		1 byte			*/
		uint8_t WDT_MS				: 8;	/* 	WDT_MS									R/W		1 byte			*/
		uint8_t I2C_CLOCK_STR		: 8;	/* 	I2C_CLOCK_STR							R/W		1 byte			*/
		uint8_t NS_REG				: 8;	/* 	NS_REG									R/W		1 byte			*/
		uint8_t RFU					: 8;	/* 	RFU										R/W		1 byte			*/
		uint64_t _00h				: 64;	/* 	00h										R		8 bytes			*/
	};
	uint8_t raw[16];
} AlxCrn120_RegBlockVal_0xFE_Block_SessionReg;


//******************************************************************************
// Register Structures
//******************************************************************************
typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxCrn120_RegBlockVal_0x00_Block_0 val;
} AlxCrn120_RegBlock_0x00_Block_0;	

typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxCrn120_RegBlockVal_0x38_Block_38 val;
} AlxCrn120_RegBlock_0x38_Block_38;	
	
typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxCrn120_RegBlockVal_0x39_Block_39 val;
} AlxCrn120_RegBlock_0x39_Block_39;	
	
typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxCrn120_RegBlockVal_0x3A_Block_ConfigurationReg val;
} AlxCrn120_RegBlock_0x3A_Block_ConfigurationReg;	
	
typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxCrn120_RegBlockVal_0xFE_Block_SessionReg val;
} AlxCrn120_RegBlock_0xFE_Block_SessionReg;	
	


//******************************************************************************
// Main Register Structure
//******************************************************************************
typedef struct
{
	AlxCrn120_RegBlock_0x00_Block_0						_0x00;
	AlxCrn120_RegBlock_0x38_Block_38					_0x38;
	AlxCrn120_RegBlock_0x39_Block_39					_0x39;
	AlxCrn120_RegBlock_0x3A_Block_ConfigurationReg		_0x3A_ConfigurationReg;
	AlxCrn120_RegBlock_0xFE_Block_SessionReg			_0xFE_SessionReg;

} AlxCrn120_RegBlock;


//******************************************************************************
// Types
//******************************************************************************

typedef struct
	{
		// Parameters - Const

		// Objects - External
		AlxI2c* i2c;
		
		AlxIoPin* di_EventDetect;		// Event detection
		AlxIoPin* di_NfcFieldDetect;	// NFC field detection

		// Parameters
		uint8_t i2cAddr;
		//bool i2cCheckWithRead;
		uint8_t i2cNumOfTries;
		uint16_t i2cTimeout_ms;

		// Variables
		AlxCrn120_RegBlock regblock;

		// Info
		bool isInit;
		bool wasCtorCalled;
	} AlxCrn120;

//******************************************************************************
// Constructor
//******************************************************************************
	void AlxCrn120_Ctor
(
	AlxCrn120* me,
	AlxI2c* i2c,
	uint8_t i2cAddr,
	AlxIoPin* di_EventDetect,	
	AlxIoPin* di_NfcFieldDetect,
	//bool i2cCheckWithRead,
	uint8_t i2cNumOfTries,
	uint16_t i2cTimeout_ms
);

//******************************************************************************
// Functions
//******************************************************************************

Alx_Status AlxCrn120_Init(AlxCrn120* me);
Alx_Status AlxCrn120_DeInit(AlxCrn120* me);
	
#ifdef __cplusplus
}
#endif

#endif // ALX_ADAU1961_H
