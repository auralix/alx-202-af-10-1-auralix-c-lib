/**
  ******************************************************************************
  * @file alxCrn120.h
  * @brief Auralix C Library - ALX NFC WLC Communication Receiver CRN120 Module
  * @version $LastChangedRevision: 5305 $
  * @date $LastChangedDate: 2021-06-01 19:13:59 +0200 (Tue, 01 Jun 2021) $
  ******************************************************************************
  */

#ifndef ALX_CRN120_H
#define ALX_CRN120_H

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
#define ALX_CRN120_FILE "alxCrn120.h"

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

//------------------------------------------------------------------------------
// Register 00h
//------------------------------------------------------------------------------
typedef enum
{
	StaticLock_Block_CC_Off = 0b0,
	StaticLock_Block_CC_On  = 0b1
} AlxCrn120_00h_StaticLock_Block_CC;

typedef enum
{
	StaticLock_Block_9_4_Off = 0b0,
	StaticLock_Block_9_4_On  = 0b1
} AlxCrn120_00h_StaticLock_Block_9_4;

typedef enum
{
	StaticLock_Block_15_10_Off = 0b0,
	StaticLock_Block_15_10_On  = 0b1
} AlxCrn120_00h_StaticLock_Block_15_10;

typedef enum
{
	StaticLock_Page_CC_Off = 0b0,
	StaticLock_Page_CC_On  = 0b1
} AlxCrn120_00h_StaticLock_Page_CC;

typedef enum
{
	StaticLock_Page_4_Off = 0b0,
	StaticLock_Page_4_On  = 0b1
} AlxCrn120_00h_StaticLock_Page_4;

typedef enum
{
	StaticLock_Page_5_Off = 0b0,
	StaticLock_Page_5_On  = 0b1
} AlxCrn120_00h_StaticLock_Page_5;

typedef enum
{
	StaticLock_Page_6_Off = 0b0,
	StaticLock_Page_6_On  = 0b1
} AlxCrn120_00h_StaticLock_Page_6;

typedef enum
{
	StaticLock_Page_7_Off = 0b0,
	StaticLock_Page_7_On  = 0b1
} AlxCrn120_00h_StaticLock_Page_7;

typedef enum
{
	StaticLock_Page_8_Off = 0b0,
	StaticLock_Page_8_On  = 0b1
} AlxCrn120_00h_StaticLock_Page_8;

typedef enum
{
	StaticLock_Page_9_Off = 0b0,
	StaticLock_Page_9_On  = 0b1
} AlxCrn120_00h_StaticLock_Page_9;

typedef enum
{
	StaticLock_Page_10_Off = 0b0,
	StaticLock_Page_10_On  = 0b1
} AlxCrn120_00h_StaticLock_Page_10;

typedef enum
{
	StaticLock_Page_11_Off = 0b0,
	StaticLock_Page_11_On  = 0b1
} AlxCrn120_00h_StaticLock_Page_11;

typedef enum
{
	StaticLock_Page_12_Off = 0b0,
	StaticLock_Page_12_On  = 0b1
} AlxCrn120_00h_StaticLock_Page_12;

typedef enum
{
	StaticLock_Page_13_Off = 0b0,
	StaticLock_Page_13_On  = 0b1
} AlxCrn120_00h_StaticLock_Page_13;

typedef enum
{
	StaticLock_Page_14_Off = 0b0,
	StaticLock_Page_14_On  = 0b1
} AlxCrn120_00h_StaticLock_Page_14;

typedef enum
{
	StaticLock_Page_15_Off = 0b0,
	StaticLock_Page_15_On  = 0b1
} AlxCrn120_00h_StaticLock_Page_15;

typedef union
{
	struct __attribute__((packed))
	{
		uint8_t	 Addr : 8;
		uint64_t Serial_number : 48;
		uint32_t Internal : 24;

		AlxCrn120_00h_StaticLock_Block_CC BLCC : 1;
		AlxCrn120_00h_StaticLock_Block_9_4 BL9_14 : 1;
		AlxCrn120_00h_StaticLock_Block_15_10 BL15_10 : 1;
		AlxCrn120_00h_StaticLock_Page_CC LCC : 1;
		AlxCrn120_00h_StaticLock_Page_4 L4 : 1;
		AlxCrn120_00h_StaticLock_Page_5 L5 : 1;
		AlxCrn120_00h_StaticLock_Page_6 L6 : 1;
		AlxCrn120_00h_StaticLock_Page_7 L7 : 1;
		AlxCrn120_00h_StaticLock_Page_8 L8 : 1;
		AlxCrn120_00h_StaticLock_Page_9 L9 : 1;
		AlxCrn120_00h_StaticLock_Page_10 L10 : 1;
		AlxCrn120_00h_StaticLock_Page_11 L11 : 1;
		AlxCrn120_00h_StaticLock_Page_12 L12 : 1;
		AlxCrn120_00h_StaticLock_Page_13 L13 : 1;
		AlxCrn120_00h_StaticLock_Page_14 L14 : 1;
		AlxCrn120_00h_StaticLock_Page_15 L15 : 1;

		uint32_t Capability_Container_CC : 32;
	};
	uint8_t raw[16];
} AlxCrn120_RegVal_00h;

//------------------------------------------------------------------------------	// MF Fuka, nevem tocno ce je Protected_user_memory 32bit al 64bit. NFC ma naslove po 4byte, tak da je potemtakem 0xE0 pri NFC enak 0x38 pri I2C, torej bi mogo bit Protected_user_memory 64bit
// Register 38h
//------------------------------------------------------------------------------
typedef enum
{
	DynamicLock_Page_16_31_Off = 0b0,
	DynamicLock_Page_16_31_On  = 0b1
} AlxCrn120_38h_DynamicLock_Page_16_31;
typedef enum
{
	DynamicLock_Page_32_47_Off = 0b0,
	DynamicLock_Page_32_47_On  = 0b1
} AlxCrn120_38h_DynamicLock_Page_32_47;
typedef enum
{
	DynamicLock_Page_48_63_Off = 0b0,
	DynamicLock_Page_48_63_On  = 0b1
} AlxCrn120_38h_DynamicLock_Page_48_63;
typedef enum
{
	DynamicLock_Page_64_79_Off = 0b0,
	DynamicLock_Page_64_79_On  = 0b1
} AlxCrn120_38h_DynamicLock_Page_64_79;
typedef enum
{
	DynamicLock_Page_80_95_Off = 0b0,
	DynamicLock_Page_80_95_On  = 0b1
} AlxCrn120_38h_DynamicLock_Page_80_95;
typedef enum
{
	DynamicLock_Page_96_111_Off = 0b0,
	DynamicLock_Page_96_111_On  = 0b1
} AlxCrn120_38h_DynamicLock_Page_96_111;
typedef enum
{
	DynamicLock_Page_112_127_Off = 0b0,
	DynamicLock_Page_112_127_On  = 0b1
} AlxCrn120_38h_DynamicLock_Page_112_127;
typedef enum
{
	DynamicLock_Page_128_143_Off = 0b0,
	DynamicLock_Page_128_143_On  = 0b1
} AlxCrn120_38h_DynamicLock_Page_128_143;
typedef enum
{
	DynamicLock_Page_144_159_Off = 0b0,
	DynamicLock_Page_144_159_On  = 0b1
} AlxCrn120_38h_DynamicLock_Page_144_159;
typedef enum
{
	DynamicLock_Page_160_175_Off = 0b0,
	DynamicLock_Page_160_175_On  = 0b1
} AlxCrn120_38h_DynamicLock_Page_160_175;
typedef enum
{
	DynamicLock_Page_176_191_Off = 0b0,
	DynamicLock_Page_176_191_On  = 0b1
} AlxCrn120_38h_DynamicLock_Page_176_191;
typedef enum
{
	DynamicLock_Page_192_207_Off = 0b0,
	DynamicLock_Page_192_207_On  = 0b1
} AlxCrn120_38h_DynamicLock_Page_192_207;
typedef enum
{
	DynamicLock_Page_208_223_Off = 0b0,
	DynamicLock_Page_208_223_On  = 0b1
} AlxCrn120_38h_DynamicLock_Page_208_223;
typedef enum
{
	DynamicLock_Page_224_225_Off = 0b0,
	DynamicLock_Page_224_225_On  = 0b1
} AlxCrn120_38h_DynamicLock_Page_224_225;
typedef union
{
	struct __attribute__((packed))
	{
		uint64_t Protected_user_memory	: 64;

		AlxCrn120_38h_DynamicLock_Page_16_31 L16_31 : 1;
		AlxCrn120_38h_DynamicLock_Page_32_47 L32_47 : 1;
		AlxCrn120_38h_DynamicLock_Page_48_63 L48_63 : 1;
		AlxCrn120_38h_DynamicLock_Page_64_79 L64_79 : 1;
		AlxCrn120_38h_DynamicLock_Page_80_95 L80_95 : 1;
		AlxCrn120_38h_DynamicLock_Page_96_111 L96_111 : 1;
		AlxCrn120_38h_DynamicLock_Page_112_127 L112_127 : 1;
		AlxCrn120_38h_DynamicLock_Page_128_143 L128_143 : 1;
		AlxCrn120_38h_DynamicLock_Page_144_159 L144_159 : 1;
		AlxCrn120_38h_DynamicLock_Page_160_175 L160_175 : 1;
		AlxCrn120_38h_DynamicLock_Page_176_191 L176_191 : 1;
		AlxCrn120_38h_DynamicLock_Page_192_207 L192_207 : 1;
		AlxCrn120_38h_DynamicLock_Page_208_223 L208_223 : 1;
		AlxCrn120_38h_DynamicLock_Page_224_225 L224_225 : 1;
		bool unused_6_Rfui : 1;
		bool unused_7_Rfui : 1;
		uint8_t unused : 8;		// MF: We won't use 2nd Byte of "Dynamic Lock Bytes"
		uint8_t _00h : 8;

		uint32_t RFU : 24;
		uint8_t AUTH0 : 8;
	};
	uint8_t raw[16];
} AlxCrn120_RegVal_38h;

//------------------------------------------------------------------------------
// Register 39h
//------------------------------------------------------------------------------
typedef union
{
	struct __attribute__((packed))
	{
		uint8_t ACCESS	: 8;
		uint32_t RFU	: 24;
		uint32_t PWD	: 24;
		uint16_t PACK	: 16;
		uint16_t RFU_	: 16;
		uint8_t PT_I2C	: 8;
		uint32_t RFU_2	: 24;
	};
	uint8_t raw[16];
} AlxCrn120_RegVal_39h;

//------------------------------------------------------------------------------
// Register 3Ah
//------------------------------------------------------------------------------
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
} AlxCrn120_RegVal_3Ah_ConfigurationReg;

//------------------------------------------------------------------------------
// Register FEh
//------------------------------------------------------------------------------
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
} AlxCrn120_RegVal_FEh_SessionReg;


//******************************************************************************
// Register Structures
//******************************************************************************
typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxCrn120_RegVal_00h val;
} AlxCrn120_Reg_00h;

typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxCrn120_RegVal_38h val;
} AlxCrn120_Reg_38h;

typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxCrn120_RegVal_39h val;
} AlxCrn120_Reg_39h;

typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxCrn120_RegVal_3Ah_ConfigurationReg val;
} AlxCrn120_Reg_3Ah_ConfigurationReg;

typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxCrn120_RegVal_FEh_SessionReg val;
} AlxCrn120_Reg_FEh_SessionReg;


//******************************************************************************
// Main Register Structure
//******************************************************************************
typedef struct
{
	AlxCrn120_Reg_00h					_0x00;
	AlxCrn120_Reg_38h					_0x38;
	AlxCrn120_Reg_39h					_0x39;
	AlxCrn120_Reg_3Ah_ConfigurationReg	_0x3A_ConfigurationReg;
	AlxCrn120_Reg_FEh_SessionReg		_0xFE_SessionReg;
} AlxCrn120_Reg;


//******************************************************************************
// Types
//******************************************************************************
typedef enum
{
	AlxCrn120_UsrMemAddr_0x01 = 0x01,
	AlxCrn120_UsrMemAddr_0x02 = 0x02,
	AlxCrn120_UsrMemAddr_0x03 = 0x03,
	AlxCrn120_UsrMemAddr_0x04 = 0x04,
	// TODO
	AlxCrn120_UsrMemAddr_0x37 = 0x37,
} AlxCrn120_UsrMemAddr;
typedef enum
{
	AlxCrn120_SramAddr_0xF8 = 0xF8,
	AlxCrn120_SramAddr_0xF9 = 0xF9,
	AlxCrn120_SramAddr_0xFA = 0xFA,
	AlxCrn120_SramAddr_0xFB = 0xFB
} AlxCrn120_SramAddr;
typedef struct
{
	// Objects - External
	AlxI2c* i2c;

	// Parameters
	uint8_t i2cAddr;
	bool i2cCheckWithRead;
	uint8_t i2cNumOfTries;
	uint16_t i2cTimeout_ms;

	// Variables
	AlxCrn120_Reg regblock;

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
	bool i2cCheckWithRead,
	uint8_t i2cNumOfTries,
	uint16_t i2cTimeout_ms
);


//******************************************************************************
// Functions
//******************************************************************************
Alx_Status AlxCrn120_Init(AlxCrn120* me);
Alx_Status AlxCrn120_DeInit(AlxCrn120* me);
Alx_Status AlxCrn120_UsrMem(AlxCrn120* me, bool toRead, AlxCrn120_UsrMemAddr addr); // MF: Addr can be 0x01-0b37 and "9.7 READ and WRITE Operation" have to be used, meaning 16bytes in one read/write
Alx_Status AlxCrn120_Sram(AlxCrn120* me, bool toRead, AlxCrn120_SramAddr addr); // MF: Addr can be 0xF8, 0xF9, 0xFA or 0xFB and "9.7 READ and WRITE Operation" have to be used, meaning 16bytes in one read/write


#ifdef __cplusplus
}
#endif

#endif // ALX_CRN120_H
