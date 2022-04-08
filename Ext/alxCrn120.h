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
#include "alxDelay.h"
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

// Mem //
#define ALX_CRN120_BLOCK_LEN 16

#define ALX_CRN120_EEPROM_START 0x01
#define ALX_CRN120_EEPROM_END 0x38
#define ALX_CRN120_EEPROM_LEN 888

#define ALX_CRN120_SRAM_START 0xF8
#define ALX_CRN120_SRAM_END 0xFB
#define ALX_CRN120_SRAM_LEN 64


//******************************************************************************
// Register Values Unions
//******************************************************************************
// General Enums //
typedef enum
{
	NcReg_TransferDir_FromI2cToNfc = 0b0,
	NcReg_TransferDir_FromNfcToI2c = 0b1
} AlxCrn120_CfgSess_NcReg_TransferDir;
typedef enum
{
	NcReg_SramMirror_Disable = 0b0,
	NcReg_SramMirror_Enable  = 0b1
} AlxCrn120_CfgSess_NcReg_SramMirror;
typedef enum
{
	NcReg_EdOn_RFU        = 0b00,
	NcReg_EdOn_Soc        = 0b01,
	NcReg_EdOn_TagSelect  = 0b10,
	NcReg_EdOn_ReadToRead = 0b11
} AlxCrn120_CfgSess_NcReg_EdOn;
typedef enum			// MF: TODO Imena fukajo, nevem, neznam, ne morem
{
	NcReg_EdOff_00 = 0b00,	// if the field is switched off
	NcReg_EdOff_01 = 0b01,	// if the field is switched off or the tag is set to the HALT state
	NcReg_EdOff_10 = 0b10,	// if the field is switched off or the last page of the NDEF message has been read (defined in LAST_NDEF_BLOCK)
	NcReg_EdOff_11 = 0b11	// (if ED_ON = 11b) if the field is switched off or if last data is read by I2C(in pass - through mode NFC ---> I2C) or last data is written by I2C(in pass - through mode I2C ---> NFC)
} AlxCrn120_CfgSess_NcReg_EdOff;
typedef enum
{
	NcReg_PassThroughMode_Disable	= 0b0,
	NcReg_PassThroughMode_Enable	= 0b1
} AlxCrn120_CfgSess_NcReg_PassThroughMode;
typedef enum
{
	NcReg_NfcsI2cRst_Off = 0b0,
	NcReg_NfcsI2cRst_On  = 0b1
} AlxCrn120_CfgSess_NcReg_NfcsI2cRst;
typedef enum
{
	I2cClockStr_ClockStretching_Disable = 0b0,
	I2cClockStr_ClockStretching_Enable  = 0b1
} AlxCrn120_CfgSess_I2cClockStr_ClockStretching;

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
		// #1 Addr
		uint8_t	 Addr : 8;

		// #2 Serial number (UID)
		uint64_t Serial_number : 48;

		// #3 Internal
		uint32_t Internal : 24;

		// #4 Static lock bytes
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

		// #5 Capability Container (CC)
		uint32_t Capability_Container_CC : 32;
	};
	uint8_t raw[16];
} AlxCrn120_RegVal_00h;

//------------------------------------------------------------------------------
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
		// #1 Protected user memory
		uint64_t Protected_user_memory	: 64;	// MF: Fuka, nevem tocno ce je 32bit al 64bit. NFC ma naslove po 4byte, tak da je potemtakem 0xE0 pri NFC enak 0x38 pri I2C, torej bi moglo bit 64bit

		// #2 Dynamic lock bytes
		// #2.1 Dynamic Lock Byte 0
		AlxCrn120_38h_DynamicLock_Page_16_31 L16_31 : 1;
		AlxCrn120_38h_DynamicLock_Page_32_47 L32_47 : 1;
		AlxCrn120_38h_DynamicLock_Page_48_63 L48_63 : 1;
		AlxCrn120_38h_DynamicLock_Page_64_79 L64_79 : 1;
		AlxCrn120_38h_DynamicLock_Page_80_95 L80_95 : 1;
		AlxCrn120_38h_DynamicLock_Page_96_111 L96_111 : 1;
		AlxCrn120_38h_DynamicLock_Page_112_127 L112_127 : 1;
		AlxCrn120_38h_DynamicLock_Page_128_143 L128_143 : 1;
		// #2.2 Dynamic Lock Byte 1
		AlxCrn120_38h_DynamicLock_Page_144_159 L144_159 : 1;
		AlxCrn120_38h_DynamicLock_Page_160_175 L160_175 : 1;
		AlxCrn120_38h_DynamicLock_Page_176_191 L176_191 : 1;
		AlxCrn120_38h_DynamicLock_Page_192_207 L192_207 : 1;
		AlxCrn120_38h_DynamicLock_Page_208_223 L208_223 : 1;
		AlxCrn120_38h_DynamicLock_Page_224_225 L224_225 : 1;
		uint8_t unused_6_7_RFUI : 2; // bits 6..7 are unused
		// #2.3 Dynamic Lock Byte 3
		uint8_t unused : 8;					// MF: We won't use 2nd Byte of "Dynamic Lock Bytes"
		// #2.4 Dynamic Lock Byte 3
		uint8_t _00h : 8;

		// #3 RFU
		uint32_t RFU : 24;

		// #4 AUTH0
		uint8_t AUTH0 : 8;
	};
	uint8_t raw[16];
} AlxCrn120_RegVal_38h;

//------------------------------------------------------------------------------
// Register 39h
//------------------------------------------------------------------------------
typedef enum
{
	Access_AuthLim_Disabled        = 0b000,
	Access_AuthLim_MaxAttempts_2   = 0b001,
	Access_AuthLim_MaxAttempts_4   = 0b010,
	Access_AuthLim_MaxAttempts_8   = 0b011,
	Access_AuthLim_MaxAttempts_16  = 0b100,
	Access_AuthLim_MaxAttempts_32  = 0b101,
	Access_AuthLim_MaxAttempts_64  = 0b110,
	Access_AuthLim_MaxAttempts_128 = 0b111
} AlxCrn120_39h_Access_AuthLim;
typedef enum
{
	Access_Nfc_Prot_Write = 0b0,
	Access_Nfc_Prot_ReadAndWrite = 0b1
} AlxCrn120_39h_Access_Nfc_Prot;
typedef enum
{
	Pt_I2c_I2cProt_EntireUrsMemAccessible  = 0b00,
	Pt_I2c_I2cProt_ReadOnlyToProtectedArea = 0b01,
	Pt_I2c_I2cProt_NoAccessoProtectedArea  = 0b10
} AlxCrn120_39h_Pt_I2c_I2cProt;
typedef enum
{
	Pt_I2c_SramProt_PwdAuthDisabled = 0b0,
	Pt_I2c_SramProt_PwdAuthNeeded   = 0b1,
} AlxCrn120_39h_Pt_I2c_SramProt;
typedef union
{
	struct __attribute__((packed))
	{
		// #1 ACCESS
		AlxCrn120_39h_Access_AuthLim AUTHLIM : 3;
		uint8_t unused_3_6 : 4; // bits 3..6 are unused
		AlxCrn120_39h_Access_Nfc_Prot NFC_PROT : 1;

		// #2 RFU
		uint32_t RFU1	: 24;

		// #3 PWD
		uint32_t PWD	: 32;

		// #4 PACK
		uint16_t PACK	: 16;

		// #5 RFU
		uint16_t RFU2	: 16;

		// #6 PT_I2C
		AlxCrn120_39h_Pt_I2c_I2cProt I2C_PROT : 2;
		AlxCrn120_39h_Pt_I2c_SramProt SRAM_PROT : 1;
		uint8_t unused_3_7 : 5; // bits 3..7 are unused

		// #7 RFU
		uint32_t RFU3	: 24;
	};
	uint8_t raw[16];
} AlxCrn120_RegVal_39h;

//------------------------------------------------------------------------------
// Register 3Ah
//------------------------------------------------------------------------------
typedef enum
{
	Config_RegLock_Nfc_Changeable = 0b0,
	Config_RegLock_Nfc_Locked     = 0b1
} AlxCrn120_Config_RegLock_Nfc;
typedef enum
{
	Config_RegLock_I2c_Changeable = 0b0,
	Config_RegLock_I2c_Locked     = 0b1
} AlxCrn120_Config_RegLock_I2c;
typedef union
{
	struct __attribute__((packed))
	{
		// #1 NC_REG
		AlxCrn120_CfgSess_NcReg_TransferDir TRANSFER_DIR : 1;
		AlxCrn120_CfgSess_NcReg_SramMirror SRAM_MIRROR : 1;
		AlxCrn120_CfgSess_NcReg_EdOn ED_ON : 2;
		AlxCrn120_CfgSess_NcReg_EdOff ED_OFF : 2;
		AlxCrn120_CfgSess_NcReg_PassThroughMode PTHRU : 1;
		AlxCrn120_CfgSess_NcReg_NfcsI2cRst NFCS_I2C_RST : 1;

		// #2 LAST_NDEF_BLOCK
		uint8_t LAST_NDEF_BLOCK : 8;

		// #3 SRAM_MIRROR_BLOCK
		uint8_t SRAM_MIRROR_BLOCK : 8;

		// #4 WDT_LS
		uint8_t WDT_LS : 8;

		// #5 WDT_MS
		uint8_t WDT_MS : 8;

		// #6 I2C_CLOCK_STR
		AlxCrn120_CfgSess_I2cClockStr_ClockStretching I2C_CLOCK_STR : 1;
		uint8_t unused_1_7_RFU : 7; // bits 1..7 are unused

		// #7 REG_LOCK
		AlxCrn120_Config_RegLock_Nfc REG_LOCK_NFC : 1;
		AlxCrn120_Config_RegLock_I2c REG_LOCK_I2C : 1;
		int8_t unused_2_7_RFU : 7; // bits 2..7 are unused

		// #8 RFU
		uint8_t RFU : 8;

		// #9 00
		uint64_t _00h : 64;
	};
	uint8_t raw[16];
} AlxCrn120_RegVal_3Ah_ConfigurationReg;

typedef union
{
	struct __attribute__((packed))
	{
		AlxCrn120_CfgSess_NcReg_TransferDir TRANSFER_DIR : 1;
		AlxCrn120_CfgSess_NcReg_SramMirror SRAM_MIRROR : 1;
		AlxCrn120_CfgSess_NcReg_EdOn ED_ON : 2;
		AlxCrn120_CfgSess_NcReg_EdOff ED_OFF : 2;
		AlxCrn120_CfgSess_NcReg_PassThroughMode PTHRU : 1;
		AlxCrn120_CfgSess_NcReg_NfcsI2cRst NFCS_I2C_RST : 1;
	};
	uint8_t raw;
} AlxCrn120_RegVal_3Ah_ConfigReg_0_NcReg;
typedef union
{
	struct __attribute__((packed))
	{
		uint8_t LAST_NDEF_BLOCK : 8;
	};
	uint8_t raw;
} AlxCrn120_RegVal_3Ah_ConfigReg_1_LastNdefBlock;

//------------------------------------------------------------------------------
// Register FEh
//------------------------------------------------------------------------------
typedef enum
{
	I2cClockStr_NegAuthReached_StillPossible = 0b0,
	I2cClockStr_NegAuthReached_Locked        = 0b1
} AlxCrn120_Session_I2cClockStr_NegAuthReached;
typedef enum
{
	NsReg_RfFieldDetect_NotDetected = 0b0,
	NsReg_RfFieldDetect_Detected    = 0b1
} AlxCrn120_Session_NsReg_RfFieldPresent;
typedef enum
{
	NsReg_EepromWriteBusy_Possible = 0b0,
	NsReg_EepromWriteBusy_Busy     = 0b1
} AlxCrn120_Session_NsReg_EepromWriteBusy;
typedef enum
{
	NsReg_EepromWriteErr_Ok    = 0b0,
	NsReg_EepromWriteErr_Error = 0b1
} AlxCrn120_Session_NsReg_EepromWriteErr;
typedef enum
{
	NsReg_SramNfcReady_DataNotReady = 0b0,
	NsReg_SramNfcReady_DataReady    = 0b1
} AlxCrn120_Session_NsReg_SramNfcReady;
typedef enum
{
	NsReg_SramI2cReady_DataNotReady = 0b0,
	NsReg_SramI2cReady_DataReady    = 0b1
} AlxCrn120_Session_NsReg_SramI2cReady;
typedef enum
{
	NsReg_MemNfcLocked_NotLocked	= 0b0,
	NsReg_MemNfcLocked_Locked		= 0b1
} AlxCrn120_Session_NsReg_MemNfcLocked;
typedef enum
{
	NsReg_MemI2cLocked_NotLocked	= 0b0,
	NsReg_MemI2cLocked_Locked		= 0b1
} AlxCrn120_Session_NsReg_MemI2cLocked;
typedef enum
{
	NsReg_NdefDataRead_Read    = 0b0,
	NsReg_NdefDataRead_NotRead = 0b1
} AlxCrn120_Session_NsReg_NdefDataRead;
typedef union
{
	struct __attribute__((packed))
	{
		// #1 NC_REG
		AlxCrn120_CfgSess_NcReg_TransferDir TRANSFER_DIR : 1;
		AlxCrn120_CfgSess_NcReg_SramMirror SRAM_MIRROR : 1;
		AlxCrn120_CfgSess_NcReg_EdOn ED_ON : 2;
		AlxCrn120_CfgSess_NcReg_EdOff ED_OFF : 2;
		AlxCrn120_CfgSess_NcReg_PassThroughMode PTHRU : 1;
		AlxCrn120_CfgSess_NcReg_NfcsI2cRst NFCS_I2C_RST : 1;

		// #2 LAST_NDEF_BLOCK
		uint8_t LAST_NDEF_BLOCK : 8;

		// #3 SRAM_MIRROR_BLOCK
		uint8_t SRAM_MIRROR_BLOCK : 8;

		// #4 WDT_LS
		uint8_t WDT_LS : 8;

		// #5 WDT_MS
		uint8_t WDT_MS : 8;

		// #6 I2C_CLOCK_STR
		AlxCrn120_CfgSess_I2cClockStr_ClockStretching I2C_CLOCK_STR : 1;
		AlxCrn120_Session_I2cClockStr_NegAuthReached NEG_AUTH_REACHED : 1;
		uint8_t unused_2_7_RFU : 6; // bits 2..7 are unused

		// #7 REG_LOCK
		AlxCrn120_Session_NsReg_RfFieldPresent RF_FIELD_PRESENT : 1;
		AlxCrn120_Session_NsReg_EepromWriteBusy EEPROM_WR_BUSY : 1;
		AlxCrn120_Session_NsReg_EepromWriteErr EEPROM_WR_ERR : 1;
		AlxCrn120_Session_NsReg_SramNfcReady SRAM_NFC_READY : 1;
		AlxCrn120_Session_NsReg_SramI2cReady SRAM_I2C_READY : 1;
		AlxCrn120_Session_NsReg_MemNfcLocked NFC_LOCKED : 1;
		AlxCrn120_Session_NsReg_MemI2cLocked I2C_LOCKED : 1;
		AlxCrn120_Session_NsReg_NdefDataRead NDEF_DATA_READ : 1;

		// #8 RFU
		uint8_t RFU : 8;

		// #9 00
		uint64_t _00h : 64;
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
	AlxCrn120_Reg_00h					_00h;
	AlxCrn120_Reg_38h					_38h;
	AlxCrn120_Reg_39h					_39h;
	AlxCrn120_Reg_3Ah_ConfigurationReg	_3Ah_ConfigurationReg;
	AlxCrn120_Reg_FEh_SessionReg		_FEh_SessionReg;
} AlxCrn120_Reg;


//******************************************************************************
// Types
//******************************************************************************
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
	AlxCrn120_Reg reg;
	uint8_t uid[7];

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
Alx_Status AlxCrn120_ReadEeprom(AlxCrn120*me, uint32_t addr, uint8_t* data, uint32_t len);
Alx_Status AlxCrn120_WriteEeprom(AlxCrn120*me, uint32_t addr, uint8_t* data, uint32_t len);
Alx_Status AlxCrn120_ReadSram(AlxCrn120*me, uint32_t addr, uint8_t* data, uint32_t len);
Alx_Status AlxCrn120_WriteSram(AlxCrn120*me, uint32_t addr, uint8_t* data, uint32_t len);


//Alx_Status AlxCrn120_Reg_Write(AlxCrn120* me, void* reg, uint8_t* data);
//Alx_Status AlxCrn120_Reg_Read(AlxCrn120* me, void* reg, uint8_t* data);
//Alx_Status AlxCrn120_Reg_WriteReg(AlxCrn120* me, void* reg, uint8_t* data, uint8_t byte);
//Alx_Status AlxCrn120_Reg_ReadReg(AlxCrn120* me, void* reg, uint8_t* data, uint8_t byte);
//Alx_Status AlxCrn120_Mem(AlxCrn120* me, AlxCrn120_MemAddr addr, uint8_t* data, bool toWrite);	// MF: "9.7 READ and WRITE Operation" have to be used, meaning 16bytes in one read/write


#ifdef __cplusplus
}
#endif

#endif // ALX_CRN120_H
