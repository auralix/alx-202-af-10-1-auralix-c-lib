/**
  ******************************************************************************
  * @file alxCrn120.c
  * @brief Auralix C Library - ALX NFC WLC Communication Receiver CRN120 Module
  * @version $LastChangedRevision: 5305 $
  * @date $LastChangedDate: 2021-06-01 19:13:59 +0200 (Tue, 01 Jun 2021) $
  ******************************************************************************
  */

//******************************************************************************
// Includes
//******************************************************************************
#include "alxCrn120.h"


//******************************************************************************
// Private Functions
//******************************************************************************
static void AlxCrn120_RegStruct_SetAddr(AlxCrn120* me);
static void AlxCrn120_RegStruct_SetLen(AlxCrn120* me);
static void AlxCrn120_RegStruct_SetValToZero(AlxCrn120* me);
static void AlxCrn120_RegStruct_SetValToDefault(AlxCrn120* me);

static Alx_Status AlxCrn120_ReadBlock(AlxCrn120* me, uint32_t addr, uint8_t* data);
static Alx_Status AlxCrn120_WriteBlock(AlxCrn120* me, uint32_t addr, uint8_t* data);
static Alx_Status AlxCrn120_ReadBlockLen(AlxCrn120* me, uint32_t addr, uint8_t* data, uint32_t len);
static Alx_Status AlxCrn120_WriteBlockLen(AlxCrn120* me, uint32_t addr, uint8_t* data, uint32_t len, bool checkWithRead);
static Alx_Status AlxCrn120_SetSessionReg(AlxCrn120* me);


//static Alx_Status AlxCrn120_Reg_Write(AlxCrn120* me, void* reg);
//static Alx_Status AlxCrn120_Reg_Read(AlxCrn120* me, void* reg);
//static Alx_Status AlxCrn120_Reg_WriteAll(AlxCrn120* me);

//******************************************************************************
// Weak Functions
//******************************************************************************
void AlxCrn120_RegStruct_SetVal(AlxCrn120* me);


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
)
{
	// Objects - External
	me->i2c = i2c;

	// Parameters
	me->i2cAddr = i2cAddr;
	me->i2cCheckWithRead = i2cCheckWithRead;
	me->i2cNumOfTries = i2cNumOfTries;
	me->i2cTimeout_ms = i2cTimeout_ms;

	// Variables
	AlxCrn120_RegStruct_SetAddr(me);
	AlxCrn120_RegStruct_SetLen(me);
	AlxCrn120_RegStruct_SetValToZero(me);
	for (uint8_t i = 0; i < sizeof(me->uid); i++) { me->uid[i] = 0; }

	// Info
	me->isInit = false;
	me->wasCtorCalled = true;
}


//******************************************************************************
// Functions
//******************************************************************************
Alx_Status AlxCrn120_Init(AlxCrn120* me)
{
	// #1 Assert
	ALX_CRN120_ASSERT(me->isInit == false);
	ALX_CRN120_ASSERT(me->wasCtorCalled == true);

	// #2 Prepare Variable
	Alx_Status status = Alx_Err;

	// #3 Init I2C
	status = AlxI2c_Init(me->i2c);
	if (status != Alx_Ok) { ALX_CRN120_TRACE("Err_AlxI2c_Init"); return status; }

	// #4 Check if slave ready
	status = AlxI2c_Master_IsSlaveReady(me->i2c, me->i2cAddr, me->i2cNumOfTries, me->i2cTimeout_ms);
	if (status != Alx_Ok) { ALX_CRN120_TRACE("Err_AlxI2c_IsSlaveReady"); return status; }

	// #5 Read UID from CRN120
	status = AlxCrn120_ReadBlockLen(me, me->reg._00h.addr, me->uid, sizeof(me->uid));
	if (status != Alx_Ok) { ALX_CRN120_TRACE("Err_ReadBlock"); return status; }

	// #6 Set register struct values to default		// MF: We probbalby don't need this
	//AlxCrn120_RegStruct_SetValToDefault(me);

	// #7 Set registers values - WEAK
	AlxCrn120_RegStruct_SetVal(me);

	// #8 Write registers							// MF: We probbalby don't need this
	// #8.1 Write register 00h - CC
	//status = AlxCrn120_WriteBlockLen(me, me->reg._00h.addr, me->reg._00h.val.raw, sizeof(me->reg._00h.val.raw), false);
	//if (status != Alx_Ok) { ALX_CRN120_TRACE("Err_WriteBlock"); return status; }
	//AlxDelay_ms(5);	// MF: Eeprom Programing Delay

	// #8.2 Write register FEh - Session			// MF: We probbalby don't need this
	//status = AlxCrn120_SetSessionReg(me);		// MF: "checkWithRead" isn't handled here
	//if (status != Alx_Ok) { ALX_CRN120_TRACE("Err_SetSessionReg"); return status; }

	// #9 Set isInit
	me->isInit = true;

	// #10 Return OK
	return Alx_Ok;
}
Alx_Status AlxCrn120_DeInit(AlxCrn120* me)
{
	// #1 Assert
	ALX_CRN120_ASSERT(me->isInit == true);
	ALX_CRN120_ASSERT(me->wasCtorCalled == true);

	// #2 Prepare Variable
	Alx_Status status = Alx_Err;

	// #3 DeInit I2c
	status = AlxI2c_DeInit(me->i2c);
	if (status != Alx_Ok) { ALX_CRN120_TRACE("Err_AlxI2c_DeInit"); return status; }

	// #4 Reset isInit
	me->isInit = false;

	// #5 Return OK
	return Alx_Ok;
}
Alx_Status AlxCrn120_ReadEeprom(AlxCrn120*me, uint32_t addr, uint8_t* data, uint32_t len)
{
	// #1 Assert
	ALX_CRN120_ASSERT(me->isInit == true);
	ALX_CRN120_ASSERT(me->wasCtorCalled == true);
	ALX_CRN120_ASSERT(data != NULL);
	ALX_CRN120_ASSERT((addr >= ALX_CRN120_EEPROM_START) && (addr <= ALX_CRN120_EEPROM_END));
	ALX_CRN120_ASSERT((len > 0) && (len <= ALX_CRN120_EEPROM_LEN));
	ALX_CRN120_ASSERT((((addr - ALX_CRN120_EEPROM_START) * ALX_CRN120_BLOCK_LEN) + len) <= ALX_CRN120_EEPROM_LEN);

	// #2 Prepare variables
	Alx_Status status = Alx_Err;

	// #3 Read Blocks
	status = AlxCrn120_ReadBlockLen(me, addr, data, len);
	if (status != Alx_Ok) { ALX_CRN120_TRACE("Err_ReadBlock"); return status; }

	// #4 Return OK
	return Alx_Ok;
}
Alx_Status AlxCrn120_WriteEeprom(AlxCrn120*me, uint32_t addr, uint8_t* data, uint32_t len)
{
	// #1 Assert
	ALX_CRN120_ASSERT(me->isInit == true);
	ALX_CRN120_ASSERT(me->wasCtorCalled == true);
	ALX_CRN120_ASSERT(data != NULL);
	ALX_CRN120_ASSERT((addr >= ALX_CRN120_EEPROM_START) && (addr <= ALX_CRN120_EEPROM_END));
	ALX_CRN120_ASSERT((len > 0) && (len <= ALX_CRN120_EEPROM_LEN));
	ALX_CRN120_ASSERT((((addr - ALX_CRN120_EEPROM_START) * ALX_CRN120_BLOCK_LEN) + len) <= ALX_CRN120_EEPROM_LEN);

	// #2 Prepare variables
	Alx_Status status = Alx_Err;
	uint8_t numOfBlocks = len / ALX_CRN120_BLOCK_LEN;
	uint32_t lenRemainder = len % ALX_CRN120_BLOCK_LEN;
	uint32_t offset = 0;
	uint32_t AddrCount = 0;

	// #3 Write
	for (uint32_t _tryNo = 1; _tryNo <= me->i2cNumOfTries; _tryNo++)
	{
		// #3.1 Prepare Address Counter
		AddrCount = 0;

		// #3.2 Write numOfBlocks of data, leave remainder
		for (offset = 0; offset < ALX_CRN120_BLOCK_LEN * numOfBlocks; offset = offset + ALX_CRN120_BLOCK_LEN)
		{
			// #3.2.1 Write Block
			status = AlxCrn120_WriteBlock(me, addr + AddrCount, data + offset); // MF: Timeout is handled in i2c
			if (status != Alx_Ok)
			{
				ALX_CRN120_TRACE("Err_WriteBlock");
				AlxDelay_ms(5);		// MF: I'm not sure this should be here
				break;
			}

			// #3.2.2 Eeprom Programing Delay
			AlxDelay_ms(5);

			// #3.2.3 Increment Address Counter
			AddrCount++;
		}

		// #3.3 Retry if read was not Ok
		if (status != Alx_Ok) { continue; }

		// #3.4 Write remainder if needed
		if (lenRemainder != 0)
		{
			// #3.4.1 Read Block
			uint8_t buffRem[16] = { 0 };
			status = AlxCrn120_ReadBlock(me, addr + AddrCount, buffRem); // MF: Timeout is handled in i2c
			if (status != Alx_Ok)
			{
				ALX_CRN120_TRACE("Err_ReadBlock");
				continue;
			}

			// #3.4.2 Modify Data
			memcpy(buffRem, data + offset, lenRemainder);

			// #3.4.3 Write
			status = AlxCrn120_WriteBlock(me, addr + AddrCount, buffRem); // MF: Timeout is handled in i2c
			if (status != Alx_Ok)
			{
				ALX_CRN120_TRACE("Err_WriteBlock");
				continue;
			}
		}

		// #3.5 Eeprom Programing Delay
		AlxDelay_ms(5);

		// #3.6 If enabled, read & check previously written data
		if (me->i2cCheckWithRead)
		{
			// #3.6.1 Prepate Buff
			uint8_t buff[len];

			// #3.6.2 Read Written Data
			status = AlxCrn120_ReadBlockLen(me, addr, buff, len);
			if (status != Alx_Ok)
			{
				ALX_CRN120_TRACE("Err_ReadBlock");
				continue;
			}

			// #3.6.3 Compare written and read data
			if (memcmp(data, buff, len) != 0)
			{
				ALX_I2C_TRACE("Err_CheckWithRead");
				continue;
			}
		}

		// #3.7 Return OK
		return Alx_Ok;
	}

	// #4 If we are here, the number of tries error occured
	if (status != Alx_Ok)
	{
		ALX_CRN120_TRACE("Err_NumOfTries");
		return Alx_ErrNumOfTries;
	}

	// #5 Assert	// We should not get here
	ALX_CRN120_TRACE(false);
	return Alx_Err;
}
Alx_Status AlxCrn120_ReadSram(AlxCrn120*me, uint32_t addr, uint8_t* data, uint32_t len)
{
	// #1 Assert
	ALX_CRN120_ASSERT(me->isInit == true);
	ALX_CRN120_ASSERT(me->wasCtorCalled == true);
	ALX_CRN120_ASSERT(data != NULL);
	ALX_CRN120_ASSERT((addr >= ALX_CRN120_SRAM_START) && (addr <= ALX_CRN120_SRAM_END));
	ALX_CRN120_ASSERT((len > 0) && (len <= ALX_CRN120_SRAM_LEN));
	ALX_CRN120_ASSERT((((addr - ALX_CRN120_SRAM_START) * ALX_CRN120_BLOCK_LEN) + len) <= ALX_CRN120_SRAM_LEN);

	// #2 Prepare variables
	Alx_Status status = Alx_Err;

	// #3 Read Blocks
	status = AlxCrn120_ReadBlockLen(me, addr, data, len);
	if (status != Alx_Ok) { ALX_CRN120_TRACE("Err_ReadBlock"); return status; }

	// #4 Return OK
	return Alx_Ok;
}
Alx_Status AlxCrn120_WriteSram(AlxCrn120*me, uint32_t addr, uint8_t* data, uint32_t len)
{
	// #1 Assert
	ALX_CRN120_ASSERT(me->isInit == true);
	ALX_CRN120_ASSERT(me->wasCtorCalled == true);
	ALX_CRN120_ASSERT(data != NULL);
	ALX_CRN120_ASSERT((addr >= ALX_CRN120_SRAM_START) && (addr <= ALX_CRN120_SRAM_END));
	ALX_CRN120_ASSERT((len > 0) && (len <= ALX_CRN120_SRAM_LEN));
	ALX_CRN120_ASSERT((((addr - ALX_CRN120_SRAM_START) * ALX_CRN120_BLOCK_LEN) + len) <= ALX_CRN120_SRAM_LEN);

	// #2 Prepare variables
	Alx_Status status = Alx_Err;

	// #3 Write Blocks
	status = AlxCrn120_WriteBlockLen(me, addr, data, len, me->i2cCheckWithRead);
	if (status != Alx_Ok) { ALX_CRN120_TRACE("Err_WriteBlock"); return status; }

	// #4 Return OK
	return Alx_Ok;
}
Alx_Status AlxCrn120_ReadSessionReg(AlxCrn120*me, AlxCrn120_SessionRegByte rega, uint8_t* regdat)
{
	// #1 Assert
	ALX_CRN120_ASSERT(me->isInit == true);
	ALX_CRN120_ASSERT(me->wasCtorCalled == true);
	ALX_CRN120_ASSERT(regdat != NULL);

	// #2 Prepare variables
	Alx_Status status = Alx_Err;
	uint8_t regAddr = *((uint8_t*)&me->reg._FEh_SessionReg);

	// #3 Write address and byte
	for (uint32_t _tryNo = 1; _tryNo <= me->i2cNumOfTries; _tryNo++)
	{
		// #3.1 Write REGDAT
		status = AlxI2c_Master_StartWriteMemStop_Multi(me->i2c, me->i2cAddr, regAddr, AlxI2c_Master_MemAddrLen_8bit, (const uint8_t*)&rega, 1, false, me->i2cNumOfTries, me->i2cTimeout_ms);
		if (status != Alx_Ok)
		{
			ALX_CRN120_TRACE("Err_WriteData");
			continue;
		}

		status = AlxI2c_Master_StartReadStop(me->i2c, me->i2cAddr, regdat, 1, me->i2cNumOfTries, me->i2cTimeout_ms);
		if (status != Alx_Ok)
		{
			ALX_CRN120_TRACE("Err_ReadData");
			continue;
		}

		// #3.2 Return OK
		return Alx_Ok;
	}

	// #4 If we are here, the number of tries error occured
	if (status != Alx_Ok)
	{
		ALX_CRN120_TRACE("Err_NumOfTries");
		return Alx_ErrNumOfTries;
	}

	// #5 Assert	// We should not get here
	ALX_CRN120_TRACE(false);
	return Alx_Err;
}
Alx_Status AlxCrn120_WriteSessionReg(AlxCrn120*me, AlxCrn120_SessionRegByte rega, uint8_t regdat, uint8_t mask)
{
	// #1 Assert
	ALX_CRN120_ASSERT(me->isInit == true);
	ALX_CRN120_ASSERT(me->wasCtorCalled == true);
	ALX_CRN120_ASSERT(rega != AlxCrn120_SessionRegByte_I2C_CLOCK_STR); // Read Only Byte

	// #2 Prepare variables
	Alx_Status status = Alx_Err;
	uint8_t regAddr = *((uint8_t*)&me->reg._FEh_SessionReg);
	uint8_t dataWrite[3] = { (uint8_t)rega, mask, regdat };

	// #3 Write address and byte
	for (uint32_t _tryNo = 1; _tryNo <= me->i2cNumOfTries; _tryNo++)
	{
		// #3.1 Write REGDAT
		status = AlxI2c_Master_StartWriteMemStop_Multi(me->i2c, me->i2cAddr, regAddr, AlxI2c_Master_MemAddrLen_8bit, dataWrite, sizeof(dataWrite), false, me->i2cNumOfTries, me->i2cTimeout_ms);
		if (status != Alx_Ok)
		{
			ALX_CRN120_TRACE("Err_WriteData");
			continue;
		}

		// #3.2 Return OK
		return Alx_Ok;
	}

	// #4 If we are here, the number of tries error occured
	if (status != Alx_Ok)
	{
		ALX_CRN120_TRACE("Err_NumOfTries");
		return Alx_ErrNumOfTries;
	}

	// #5 Assert	// We should not get here
	ALX_CRN120_TRACE(false);
	return Alx_Err;
}
Alx_Status AlxCrn120_EnableSramMirror(AlxCrn120*me)
{
	// #1 Assert
	ALX_CRN120_ASSERT(me->isInit == true);
	ALX_CRN120_ASSERT(me->wasCtorCalled == true);

	// #2 Prepare variables
	Alx_Status status = Alx_Err;
	uint8_t regAddr = *((uint8_t*)&me->reg._FEh_SessionReg);
	me->reg._FEh_SessionReg.val.SRAM_MIRROR = NcReg_SramMirror_Enable;
	uint8_t dataWrite[3] = { (uint8_t)AlxCrn120_SessionRegByte_NC_REG, me->reg._FEh_SessionReg.val.raw[0], me->reg._FEh_SessionReg.val.raw[0] };

	// #3 Write address and byte
	for (uint32_t _tryNo = 1; _tryNo <= me->i2cNumOfTries; _tryNo++)
	{
		// #3.1 Write REGDAT
		status = AlxI2c_Master_StartWriteMemStop_Multi(me->i2c, me->i2cAddr, regAddr, AlxI2c_Master_MemAddrLen_8bit, dataWrite, sizeof(dataWrite), false, me->i2cNumOfTries, me->i2cTimeout_ms);
		if (status != Alx_Ok)
		{
			ALX_CRN120_TRACE("Err_WriteData");
			continue;
		}

		// #3.2 Return OK
		return Alx_Ok;
	}

	// #4 If we are here, the number of tries error occured
	if (status != Alx_Ok)
	{
		ALX_CRN120_TRACE("Err_NumOfTries");
		return Alx_ErrNumOfTries;
	}

	// #5 Assert	// We should not get here
	ALX_CRN120_TRACE(false);
	return Alx_Err;
}
bool AlxCrn120_IsCheckWithReadEnabled(AlxCrn120* me)
{
	// #1 Assert
	ALX_CRN120_ASSERT(me->isInit == true);
	ALX_CRN120_ASSERT(me->wasCtorCalled == true);

	// #2 Return i2cCheckWithRead
	return me->i2cCheckWithRead;
}
void AlxCrn120_CheckWithReadEnable(AlxCrn120* me)
{
	// #1 Assert
	ALX_CRN120_ASSERT(me->isInit == true);
	ALX_CRN120_ASSERT(me->wasCtorCalled == true);

	// #2 Enable i2cCheckWithRead
	me->i2cCheckWithRead = true;
}
void AlxCrn120_CheckWithReadDisable(AlxCrn120* me)
{
	// #1 Assert
	ALX_CRN120_ASSERT(me->isInit == true);
	ALX_CRN120_ASSERT(me->wasCtorCalled == true);

	// #2 Disable i2cCheckWithRead
	me->i2cCheckWithRead = false;
}


//******************************************************************************
// Private Functions
//******************************************************************************
static void AlxCrn120_RegStruct_SetAddr(AlxCrn120* me)
{
	me->reg._00h						.addr =	0x00;
	me->reg._38h						.addr =	0x38;
	me->reg._39h						.addr =	0x39;
	me->reg._3Ah_ConfigurationReg		.addr =	0x3A;
	me->reg._FEh_SessionReg				.addr =	0xFE;
}
static void AlxCrn120_RegStruct_SetLen(AlxCrn120* me)
{
	me->reg._00h						.len = sizeof(me->reg._00h						.val);
	me->reg._38h						.len = sizeof(me->reg._38h						.val);
	me->reg._39h						.len = sizeof(me->reg._39h						.val);
	me->reg._3Ah_ConfigurationReg		.len = sizeof(me->reg._3Ah_ConfigurationReg		.val);
	me->reg._FEh_SessionReg				.len = sizeof(me->reg._FEh_SessionReg			.val);
}
static void AlxCrn120_RegStruct_SetValToZero(AlxCrn120* me)
{
	me->reg._00h						.val.raw[0]		= 0b00000000;	// Addr.
	//me->reg._00h						.val.raw[1]		= 0b00000000;	// Serial number (UID)	// MF: Read Only Reg
	//me->reg._00h						.val.raw[2]		= 0b00000000;	// Serial number (UID)	// MF: Read Only Reg
	//me->reg._00h						.val.raw[3]		= 0b00000000;	// Serial number (UID)	// MF: Read Only Reg
	//me->reg._00h						.val.raw[4]		= 0b00000000;	// Serial number (UID)	// MF: Read Only Reg
	//me->reg._00h						.val.raw[5]		= 0b00000000;	// Serial number (UID)	// MF: Read Only Reg
	//me->reg._00h						.val.raw[6]		= 0b00000000;	// Serial number (UID)	// MF: Read Only Reg
	//me->reg._00h						.val.raw[7]		= 0b00000000;	// Internal
	//me->reg._00h						.val.raw[8]		= 0b00000000;	// Internal
	//me->reg._00h						.val.raw[9]		= 0b00000000;	// Internal
	me->reg._00h						.val.raw[10]	= 0b00000000;	// Static Lock Bytes
	me->reg._00h						.val.raw[11]	= 0b00000000;	// Static Lock Bytes
	me->reg._00h						.val.raw[12]	= 0b00000000;	// Capability Container (CC)
	me->reg._00h						.val.raw[13]	= 0b00000000;	// Capability Container (CC)
	me->reg._00h						.val.raw[14]	= 0b00000000;	// Capability Container (CC)
	me->reg._00h						.val.raw[15]	= 0b00000000;	// Capability Container (CC)

	//me->reg._38h						.val.raw[0]		= 0b00000000;	// Protected user memory
	//me->reg._38h						.val.raw[1]		= 0b00000000;	// Protected user memory
	//me->reg._38h						.val.raw[2]		= 0b00000000;	// Protected user memory
	//me->reg._38h						.val.raw[3]		= 0b00000000;	// Protected user memory
	//me->reg._38h						.val.raw[4]		= 0b00000000;	// Protected user memory
	//me->reg._38h						.val.raw[5]		= 0b00000000;	// Protected user memory
	//me->reg._38h						.val.raw[6]		= 0b00000000;	// Protected user memory
	//me->reg._38h						.val.raw[7]		= 0b00000000;	// Protected user memory
	me->reg._38h						.val.raw[8]		= 0b00000000;	// Dynamic lock bytes
	me->reg._38h						.val.raw[9]		= 0b00000000;	// Dynamic lock bytes
	me->reg._38h						.val.raw[10]	= 0b00000000;	// Dynamic lock bytes
	//me->reg._38h						.val.raw[11]	= 0b00000000;	// 00h
	//me->reg._38h						.val.raw[12]	= 0b00000000;	// RFU
	//me->reg._38h						.val.raw[13]	= 0b00000000;	// RFU
	//me->reg._38h						.val.raw[14]	= 0b00000000;	// RFU
	me->reg._38h						.val.raw[15]	= 0b00000000;	// AUTH0

	me->reg._39h						.val.raw[0]		= 0b00000000;	// ACCESS
	//me->reg._39h						.val.raw[1]		= 0b00000000;	// RFU
	//me->reg._39h						.val.raw[2]		= 0b00000000;	// RFU
	//me->reg._39h						.val.raw[3]		= 0b00000000;	// RFU
	me->reg._39h						.val.raw[4]		= 0b00000000;	// PWD
	me->reg._39h						.val.raw[5]		= 0b00000000;	// PWD
	me->reg._39h						.val.raw[6]		= 0b00000000;	// PWD
	me->reg._39h						.val.raw[7]		= 0b00000000;	// PWD
	me->reg._39h						.val.raw[8]		= 0b00000000;	// PACK
	me->reg._39h						.val.raw[9]		= 0b00000000;	// PACK
	//me->reg._39h						.val.raw[10]	= 0b00000000;	// RFU
	//me->reg._39h						.val.raw[11]	= 0b00000000;	// RFU
	me->reg._39h						.val.raw[12]	= 0b00000000;	// PT_I2C
	//me->reg._39h						.val.raw[13]	= 0b00000000;	// RFU
	//me->reg._39h						.val.raw[14]	= 0b00000000;	// RFU
	//me->reg._39h						.val.raw[15]	= 0b00000000;	// RFU

	me->reg._3Ah_ConfigurationReg		.val.raw[0]		= 0b00000000;	// NC_REG
	me->reg._3Ah_ConfigurationReg		.val.raw[1]		= 0b00000000;	// LAST_NDEF_BLOCK
	me->reg._3Ah_ConfigurationReg		.val.raw[2]		= 0b00000000;	// SRAM_MIRROR_BLOCK
	me->reg._3Ah_ConfigurationReg		.val.raw[3]		= 0b00000000;	// WDT_LS
	me->reg._3Ah_ConfigurationReg		.val.raw[4]		= 0b00000000;	// WDT_MS
	me->reg._3Ah_ConfigurationReg		.val.raw[5]		= 0b00000000;	// I2C_CLOCK_STR
	me->reg._3Ah_ConfigurationReg		.val.raw[6]		= 0b00000000;	// REG_LOCK
	//me->reg._3Ah_ConfigurationReg		.val.raw[7]		= 0b00000000;	// RFU
	//me->reg._3Ah_ConfigurationReg		.val.raw[8]		= 0b00000000;	// 00h
	//me->reg._3Ah_ConfigurationReg		.val.raw[9]		= 0b00000000;	// 00h
	//me->reg._3Ah_ConfigurationReg		.val.raw[10]	= 0b00000000;	// 00h
	//me->reg._3Ah_ConfigurationReg		.val.raw[11]	= 0b00000000;	// 00h
	//me->reg._3Ah_ConfigurationReg		.val.raw[12]	= 0b00000000;	// 00h
	//me->reg._3Ah_ConfigurationReg		.val.raw[13]	= 0b00000000;	// 00h
	//me->reg._3Ah_ConfigurationReg		.val.raw[14]	= 0b00000000;	// 00h
	//me->reg._3Ah_ConfigurationReg		.val.raw[15]	= 0b00000000;	// 00h

	me->reg._FEh_SessionReg				.val.raw[0]		= 0b00000000;	// NC_REG
	me->reg._FEh_SessionReg				.val.raw[1]		= 0b00000000;	// LAST_NDEF_BLOCK
	me->reg._FEh_SessionReg				.val.raw[2]		= 0b00000000;	// SRAM_MIRROR_BLOCK
	me->reg._FEh_SessionReg				.val.raw[3]		= 0b00000000;	// WDT_LS
	me->reg._FEh_SessionReg				.val.raw[4]		= 0b00000000;	// WDT_MS
	//me->reg._FEh_SessionReg			.val.raw[5]		= 0b00000000;	// I2C_CLOCK_STR	// MF: Read Only Reg
	me->reg._FEh_SessionReg				.val.raw[6]		= 0b00000000;	// NS_REG			// MF: Only bits 2 and 6 are R&W
	//me->reg._FEh_SessionReg			.val.raw[7]		= 0b00000000;	// RFU
	//me->reg._FEh_SessionReg			.val.raw[8]		= 0b00000000;	// 00h
	//me->reg._FEh_SessionReg			.val.raw[9]		= 0b00000000;	// 00h
	//me->reg._FEh_SessionReg			.val.raw[10]	= 0b00000000;	// 00h
	//me->reg._FEh_SessionReg			.val.raw[11]	= 0b00000000;	// 00h
	//me->reg._FEh_SessionReg			.val.raw[12]	= 0b00000000;	// 00h
	//me->reg._FEh_SessionReg			.val.raw[13]	= 0b00000000;	// 00h
	//me->reg._FEh_SessionReg			.val.raw[14]	= 0b00000000;	// 00h
	//me->reg._FEh_SessionReg			.val.raw[15]	= 0b00000000;	// 00h
}
static void AlxCrn120_RegStruct_SetValToDefault(AlxCrn120* me)
{
	me->reg._00h						.val.raw[0]		= 0b10101010;	// Addr.	// MF. Reading apways returns 04h
	//me->reg._00h						.val.raw[1]		= 0b00000000;	// Serial number (UID)
	//me->reg._00h						.val.raw[2]		= 0b00000000;	// Serial number (UID)
	//me->reg._00h						.val.raw[3]		= 0b00000000;	// Serial number (UID)
	//me->reg._00h						.val.raw[4]		= 0b00000000;	// Serial number (UID)
	//me->reg._00h						.val.raw[5]		= 0b00000000;	// Serial number (UID)
	//me->reg._00h						.val.raw[6]		= 0b00000000;	// Serial number (UID)
	//me->reg._00h						.val.raw[7]		= 0b00000000;	// Internal
	//me->reg._00h						.val.raw[8]		= 0b00000000;	// Internal
	//me->reg._00h						.val.raw[9]		= 0b00000000;	// Internal
	me->reg._00h						.val.raw[10]	= 0b00000000;	// Static Lock Bytes
	me->reg._00h						.val.raw[11]	= 0b00000000;	// Static Lock Bytes
	me->reg._00h						.val.raw[12]	= 0b00000000;	// Capability Container (CC)
	me->reg._00h						.val.raw[13]	= 0b00000000;	// Capability Container (CC)
	me->reg._00h						.val.raw[14]	= 0b00000000;	// Capability Container (CC)
	me->reg._00h						.val.raw[15]	= 0b00000000;	// Capability Container (CC)

	//me->reg._38h						.val.raw[0]		= 0b00000000;	// Protected user memory
	//me->reg._38h						.val.raw[1]		= 0b00000000;	// Protected user memory
	//me->reg._38h						.val.raw[2]		= 0b00000000;	// Protected user memory
	//me->reg._38h						.val.raw[3]		= 0b00000000;	// Protected user memory
	//me->reg._38h						.val.raw[4]		= 0b00000000;	// Protected user memory
	//me->reg._38h						.val.raw[5]		= 0b00000000;	// Protected user memory
	//me->reg._38h						.val.raw[6]		= 0b00000000;	// Protected user memory
	//me->reg._38h						.val.raw[7]		= 0b00000000;	// Protected user memory
	me->reg._38h						.val.raw[8]		= 0b00000000;	// Dynamic lock bytes
	me->reg._38h						.val.raw[9]		= 0b00000000;	// Dynamic lock bytes
	me->reg._38h						.val.raw[10]	= 0b00000000;	// Dynamic lock bytes
	//me->reg._38h						.val.raw[11]	= 0b00000000;	// 00h
	//me->reg._38h						.val.raw[12]	= 0b00000000;	// RFU
	//me->reg._38h						.val.raw[13]	= 0b00000000;	// RFU
	//me->reg._38h						.val.raw[14]	= 0b00000000;	// RFU
	me->reg._38h						.val.raw[15]	= 0b11111111;	// AUTH0

	me->reg._39h						.val.raw[0]		= 0b00000000;	// ACCESS
	//me->reg._39h						.val.raw[1]		= 0b00000000;	// RFU
	//me->reg._39h						.val.raw[2]		= 0b00000000;	// RFU
	//me->reg._39h						.val.raw[3]		= 0b00000000;	// RFU
	me->reg._39h						.val.raw[4]		= 0b11111111;	// PWD	// MF: Reading PWD always returns 00000000h
	me->reg._39h						.val.raw[5]		= 0b11111111;	// PWD	// MF: Reading PWD always returns 00000000h
	me->reg._39h						.val.raw[6]		= 0b11111111;	// PWD	// MF: Reading PWD always returns 00000000h
	me->reg._39h						.val.raw[7]		= 0b11111111;	// PWD	// MF: Reading PWD always returns 00000000h
	me->reg._39h						.val.raw[8]		= 0b00000000;	// PACK
	me->reg._39h						.val.raw[9]		= 0b00000000;	// PACK
	//me->reg._39h						.val.raw[10]	= 0b00000000;	// RFU
	//me->reg._39h						.val.raw[11]	= 0b00000000;	// RFU
	me->reg._39h						.val.raw[12]	= 0b00000000;	// PT_I2C
	//me->reg._39h						.val.raw[13]	= 0b00000000;	// RFU
	//me->reg._39h						.val.raw[14]	= 0b00000000;	// RFU
	//me->reg._39h						.val.raw[15]	= 0b00000000;	// RFU

	me->reg._3Ah_ConfigurationReg		.val.raw[0]		= 0b00000001;	// NC_REG
	me->reg._3Ah_ConfigurationReg		.val.raw[1]		= 0b00000000;	// LAST_NDEF_BLOCK
	me->reg._3Ah_ConfigurationReg		.val.raw[2]		= 0b11111000;	// SRAM_MIRROR_BLOCK
	me->reg._3Ah_ConfigurationReg		.val.raw[3]		= 0b01001000;	// WDT_LS
	me->reg._3Ah_ConfigurationReg		.val.raw[4]		= 0b00001000;	// WDT_MS
	me->reg._3Ah_ConfigurationReg		.val.raw[5]		= 0b00000001;	// I2C_CLOCK_STR
	me->reg._3Ah_ConfigurationReg		.val.raw[6]		= 0b00000000;	// REG_LOCK
	//me->reg._3Ah_ConfigurationReg		.val.raw[7]		= 0b00000000;	// RFU
	//me->reg._3Ah_ConfigurationReg		.val.raw[8]		= 0b00000000;	// 00h
	//me->reg._3Ah_ConfigurationReg		.val.raw[9]		= 0b00000000;	// 00h
	//me->reg._3Ah_ConfigurationReg		.val.raw[10]	= 0b00000000;	// 00h
	//me->reg._3Ah_ConfigurationReg		.val.raw[11]	= 0b00000000;	// 00h
	//me->reg._3Ah_ConfigurationReg		.val.raw[12]	= 0b00000000;	// 00h
	//me->reg._3Ah_ConfigurationReg		.val.raw[13]	= 0b00000000;	// 00h
	//me->reg._3Ah_ConfigurationReg		.val.raw[14]	= 0b00000000;	// 00h
	//me->reg._3Ah_ConfigurationReg		.val.raw[15]	= 0b00000000;	// 00h

	me->reg._FEh_SessionReg				.val.raw[0]		= 0b00000001;	// NC_REG
	me->reg._FEh_SessionReg				.val.raw[1]		= 0b00000000;	// LAST_NDEF_BLOCK
	me->reg._FEh_SessionReg				.val.raw[2]		= 0b11111000;	// SRAM_MIRROR_BLOCK
	me->reg._FEh_SessionReg				.val.raw[3]		= 0b01001000;	// WDT_LS
	me->reg._FEh_SessionReg				.val.raw[4]		= 0b00001000;	// WDT_MS
	//me->reg._FEh_SessionReg			.val.raw[5]		= 0b00000001;	// I2C_CLOCK_STR	// MF: Read Only Reg
	me->reg._FEh_SessionReg				.val.raw[6]		= 0b00000000;	// NS_REG			// MF: Only bits 2 and 6 are R&W
	//me->reg._FEh_SessionReg			.val.raw[7]		= 0b00000000;	// RFU
	//me->reg._FEh_SessionReg			.val.raw[8]		= 0b00000000;	// 00h
	//me->reg._FEh_SessionReg			.val.raw[9]		= 0b00000000;	// 00h
	//me->reg._FEh_SessionReg			.val.raw[10]	= 0b00000000;	// 00h
	//me->reg._FEh_SessionReg			.val.raw[11]	= 0b00000000;	// 00h
	//me->reg._FEh_SessionReg			.val.raw[12]	= 0b00000000;	// 00h
	//me->reg._FEh_SessionReg			.val.raw[13]	= 0b00000000;	// 00h
	//me->reg._FEh_SessionReg			.val.raw[14]	= 0b00000000;	// 00h
	//me->reg._FEh_SessionReg			.val.raw[15]	= 0b00000000;	// 00h
}
static Alx_Status AlxCrn120_ReadBlock(AlxCrn120* me, uint32_t addr, uint8_t* data)
{
	// #1 Prepare variables
	Alx_Status status = Alx_Err;
	uint8_t regAddr = addr;

	// #2 Read from Memory
	// #2.1  Write address and register
	status = AlxI2c_Master_StartWriteStop(me->i2c, me->i2cAddr, &regAddr, 1, me->i2cNumOfTries, me->i2cTimeout_ms);
	if (status != Alx_Ok) { ALX_CRN120_TRACE("Err_WriteData"); return status;}

	// #2.2 Read data
	status = AlxI2c_Master_StartReadStop(me->i2c, me->i2cAddr, data, 16, me->i2cNumOfTries, me->i2cTimeout_ms); // MF: there is fixed number of 16 bytes to read
	if (status != Alx_Ok) { ALX_CRN120_TRACE("Err_ReadData"); return status;}

	// #3 Return OK
	return Alx_Ok;
}
static Alx_Status AlxCrn120_WriteBlock(AlxCrn120* me, uint32_t addr, uint8_t* data)
{
	// #1 Prepare variables
	Alx_Status status = Alx_Err;

	// #2 Write to Memory
	status = AlxI2c_Master_StartWriteMemStop_Multi(me->i2c, me->i2cAddr, addr, AlxI2c_Master_MemAddrLen_8bit, data, 16, false, me->i2cNumOfTries, me->i2cTimeout_ms);	// MF: CheckWithRead will be handled on higher level
	if (status != Alx_Ok) { ALX_CRN120_TRACE("Err_WriteData"); return status;}

	// #3 Return OK
	return Alx_Ok;
}
static Alx_Status AlxCrn120_ReadBlockLen(AlxCrn120* me, uint32_t addr, uint8_t* data, uint32_t len)
{
	// #2 Prepare variables
	Alx_Status status = Alx_Ok;
	uint8_t numOfBlocks = len / ALX_CRN120_BLOCK_LEN;
	uint32_t lenRemainder = len % ALX_CRN120_BLOCK_LEN;
	uint32_t offset = 0;
	uint32_t AddrCount = 0;

	// #3 Read
	for (uint32_t _tryNo = 1; _tryNo <= me->i2cNumOfTries; _tryNo++)
	{
		// #3.1 Prepare Address Counter
		AddrCount = 0;

		// #3.2 Read numOfBlocks of data, leave remainder
		for (offset = 0; offset < ALX_CRN120_BLOCK_LEN * numOfBlocks; offset = offset + ALX_CRN120_BLOCK_LEN)
		{
			// #3.2.1 Read Block
			status = AlxCrn120_ReadBlock(me, addr + AddrCount, data + offset); // MF: Timeout is handled in i2c
			if (status != Alx_Ok)
			{
				ALX_CRN120_TRACE("Err_ReadBlock");
				break;
			}

			// #3.2.2 Increment Address Counter
			AddrCount++;
		}

		// #3.3 Retry if read was not Ok
		if (status != Alx_Ok) { continue; }

		// #3.4 Read remainder if needed
		if (lenRemainder != 0)
		{
			// #3.4.1 Read Block
			uint8_t buff[16] = { 0 };
			status = AlxCrn120_ReadBlock(me, addr + AddrCount, buff); // MF: Timeout is handled in i2c
			if (status != Alx_Ok)
			{
				ALX_CRN120_TRACE("Err_ReadBlock");
				continue;
			}

			// #3.4.2 Modify Data
			memcpy(data + offset, buff, lenRemainder);
		}

		// #3.5 Return OK
		return Alx_Ok;
	}

	// #4 If we are here, the number of tries error occured
	if (status != Alx_Ok)
	{
		ALX_CRN120_TRACE("Err_NumOfTries");
		return Alx_ErrNumOfTries;
	}

	// #5 Assert	// We should not get here
	ALX_CRN120_TRACE(false);
	return Alx_Err;
}
static Alx_Status AlxCrn120_WriteBlockLen(AlxCrn120* me, uint32_t addr, uint8_t* data, uint32_t len, bool checkWithRead)
{
	// #2 Prepare variables
	Alx_Status status = Alx_Ok;
	uint8_t numOfBlocks = len / ALX_CRN120_BLOCK_LEN;
	uint32_t lenRemainder = len % ALX_CRN120_BLOCK_LEN;
	uint32_t offset = 0;
	uint32_t AddrCount = 0;

	// #3 Write
	for (uint32_t _tryNo = 1; _tryNo <= me->i2cNumOfTries; _tryNo++)
	{
		// #3.1 Prepare Address Counter
		AddrCount = 0;

		// #3.2 Write numOfBlocks of data, leave remainder
		for (offset = 0; offset < ALX_CRN120_BLOCK_LEN * numOfBlocks; offset = offset + ALX_CRN120_BLOCK_LEN)
		{
			// #3.2.1 Write Block
			status = AlxCrn120_WriteBlock(me, addr + AddrCount, data + offset); // MF: Timeout is handled in i2c
			if (status != Alx_Ok)
			{
				ALX_CRN120_TRACE("Err_WriteBlock");
				break;
			}

			// #3.2.2 Increment Address Counter
			AddrCount++;
		}

		// #3.3 Retry if read was not Ok
		if (status != Alx_Ok) { continue; }

		// #3.4 Write remainder if needed
		if (lenRemainder != 0)
		{
			// #3.4.1 Read Block
			uint8_t buffRem[16] = { 0 };
			status = AlxCrn120_ReadBlock(me, addr + AddrCount, buffRem); // MF: Timeout is handled in i2c
			if (status != Alx_Ok)
			{
				ALX_CRN120_TRACE("Err_ReadBlock");
				continue;
			}

			// #3.4.2 Modify Data
			memcpy(buffRem, data + offset, lenRemainder);

			// #3.4.3 Write
			status = AlxCrn120_WriteBlock(me, addr + AddrCount, buffRem); // MF: Timeout is handled in i2c
			if (status != Alx_Ok)
			{
				ALX_CRN120_TRACE("Err_WriteBlock");
				continue;
			}
		}

		// #3.5 If enabled, read & check previously written data
		if (checkWithRead)
		{
			// #3.5.1 Prepate Buff
			uint8_t buff[len];

			// #3.5.2 Read Written Data
			status = AlxCrn120_ReadBlockLen(me, addr, buff, len);
			if (status != Alx_Ok)
			{
				ALX_CRN120_TRACE("Err_ReadBlock");
				return status;
			}

			// #3.5.3 Compare written and read data
			if (memcmp(data, buff, len) != 0)
			{
				ALX_I2C_TRACE("Err_CheckWithRead");
				return Alx_Err;
			}
		}

		// #3.6 Return OK
		return Alx_Ok;
	}

	// #4 If we are here, the number of tries error occured
	if (status != Alx_Ok)
	{
		ALX_CRN120_TRACE("Err_NumOfTries");
		return Alx_ErrNumOfTries;
	}

	// #5 Assert	// We should not get here
	ALX_CRN120_TRACE(false);
	return Alx_Err;
}
static Alx_Status AlxCrn120_SetSessionReg(AlxCrn120* me)
{
	// #1 Prepare variables
	Alx_Status status = Alx_Err;
	uint8_t regAddr = *((uint8_t*)&me->reg._FEh_SessionReg);
	uint8_t dataWrite[3] = { 0 };

	// #2 Write address and byte
	for (uint8_t i = 0; i < 7; i++)
	{
		// #2.1 Skip I2C_CLOCK_STR (5th position) because it is read only byte
		if (i == 5) { continue; }

		// #2.2 Set MEMA, REGA and MASK
		dataWrite[0] = i;
		dataWrite[1] = me->reg._FEh_SessionReg.val.raw[i];
		dataWrite[2] = me->reg._FEh_SessionReg.val.raw[i];

		// #2.3 Write REGDAT
		status = AlxI2c_Master_StartWriteMemStop_Multi(me->i2c, me->i2cAddr, regAddr, AlxI2c_Master_MemAddrLen_8bit, dataWrite, sizeof(dataWrite), false, me->i2cNumOfTries, me->i2cTimeout_ms);
		if (status != Alx_Ok) { ALX_CRN120_TRACE("Err_WriteData"); return status;}
	}

	// #3 Return OK
	return Alx_Ok;
}


//******************************************************************************
// Weak Functions
//******************************************************************************
ALX_WEAK void AlxCrn120_RegStruct_SetVal(AlxCrn120* me)
{
	(void)me;
	ALX_CRN120_TRACE("Define 'AlxAdxl355_RegStruct_SetVal' function in your application.");
	ALX_CRN120_ASSERT(false);
}
























//Alx_Status AlxCrn120_Reg_Write(AlxCrn120* me, void* reg, uint8_t* data)
//{
//	// #1 Assert
//	ALX_CRN120_ASSERT(me->isInit == true);
//	ALX_CRN120_ASSERT(me->wasCtorCalled == true);
//	ALX_CRN120_ASSERT(!((*((uint8_t*)reg) == 0x3A) || (*((uint8_t*)reg) == 0xFE))); // MF: Must NOT be registers 3Ah or FEh
//	ALX_CRN120_ASSERT(!((*((uint8_t*)reg) == 0x00) && (data[0] != 0xAA))); // MF: Makes sure that Usr doesn't change SlaveAddr
//
//	// #2 Prepare variables
//	Alx_Status status = Alx_Err;
//	uint8_t regAddr = *((uint8_t*)reg);
//
//	// Write to SRAM
//	status = AlxI2c_Master_StartWriteMemStop_Multi(me->i2c, me->i2cAddr, regAddr, AlxI2c_Master_MemAddrLen_8bit, data, 16, false, me->i2cNumOfTries, me->i2cTimeout_ms); // MF: there is fixed number of 16 bytes to write
//	if (status != Alx_Ok) { ALX_CRN120_TRACE("Err_WriteData"); return status;}
//
//	// #5 Return OK
//	return Alx_Ok;
//}
//Alx_Status AlxCrn120_Reg_Read(AlxCrn120* me, void* reg, uint8_t* data)
//{
//	// #1 Assert
//	ALX_CRN120_ASSERT(me->isInit == true);
//	ALX_CRN120_ASSERT(me->wasCtorCalled == true);
//	ALX_CRN120_ASSERT(!((*((uint8_t*)reg) == 0x3A) || (*((uint8_t*)reg) == 0xFE))); // MF: Must NOT be registers 3Ah or FEh
//
//	// #2 Prepare variables
//	Alx_Status status = Alx_Err;
//	uint8_t regAddr = *((uint8_t*)reg);
//
//	// #3 Write address and register
//	status = AlxI2c_Master_StartWriteStop(me->i2c, me->i2cAddr, &regAddr, 1, me->i2cNumOfTries, me->i2cTimeout_ms);
//	if (status != Alx_Ok) { ALX_CRN120_TRACE("Err_WriteData"); return status;}
//
//	// #4 Read data
//	status = AlxI2c_Master_StartReadStop(me->i2c, me->i2cAddr, data, 16, me->i2cNumOfTries, me->i2cTimeout_ms); // MF: there is fixed number of 16 bytes to read
//	if (status != Alx_Ok) { ALX_CRN120_TRACE("Err_ReadData"); return status;}
//
//	// #5 Return OK
//	return Alx_Ok;
//}
//Alx_Status AlxCrn120_Reg_WriteReg(AlxCrn120* me, void* reg, uint8_t* data, uint8_t byte)
//{
//	// #1 Assert
//	ALX_CRN120_ASSERT(me->isInit == true);
//	ALX_CRN120_ASSERT(me->wasCtorCalled == true);
//	ALX_CRN120_ASSERT((*((uint8_t*)reg) == 0x3A) || (*((uint8_t*)reg) == 0xFE)); // MF: Must be registers 3Ah or FEh
//
//	// #2 Prepare variables
//	Alx_Status status = Alx_Err;
//	uint8_t regAddr = *((uint8_t*)reg);
//	uint8_t dataWrite[3] = { byte, *data, *data }; // MF: Setup MEMA, REGA and MASK
//
//	// #3 Write address and byte
//	status = AlxI2c_Master_StartWriteMemStop_Multi(me->i2c, me->i2cAddr, regAddr, AlxI2c_Master_MemAddrLen_8bit, dataWrite, sizeof(dataWrite), false, me->i2cNumOfTries, me->i2cTimeout_ms);
//	if (status != Alx_Ok) { ALX_CRN120_TRACE("Err_WriteData"); return status;}
//
//	// #4 Return OK
//	return Alx_Ok;
//}
//Alx_Status AlxCrn120_Reg_ReadReg(AlxCrn120* me, void* reg, uint8_t* data, uint8_t byte)
//{
//	// #1 Assert
//	ALX_CRN120_ASSERT(me->isInit == true);
//	ALX_CRN120_ASSERT(me->wasCtorCalled == true);
//	ALX_CRN120_ASSERT((*((uint8_t*)reg) == 0x3A) || (*((uint8_t*)reg) == 0xFE)); // MF: Must be registers 3Ah or FEh
//
//	// #2 Prepare variables
//	Alx_Status status = Alx_Err;
//	uint8_t regAddr = *((uint8_t*)reg);
//
//	// #3 Write address and byte
//	status = AlxI2c_Master_StartWriteMemStop_Multi(me->i2c, me->i2cAddr, regAddr, AlxI2c_Master_MemAddrLen_8bit, &byte, 1, false, me->i2cNumOfTries, me->i2cTimeout_ms);
//	if (status != Alx_Ok) { ALX_CRN120_TRACE("Err_WriteData"); return status;}
//
//	// #4 Read byte
//	status = AlxI2c_Master_StartReadStop(me->i2c, me->i2cAddr, data, 1, me->i2cNumOfTries, me->i2cTimeout_ms);
//	if (status != Alx_Ok) { ALX_CRN120_TRACE("Err_ReadData"); return status;}
//
//	// #5 Return OK
//	return Alx_Ok;
//}
//Alx_Status AlxCrn120_Mem(AlxCrn120* me, AlxCrn120_MemAddr addr, uint8_t* data, bool toWrite)
//{
//	// #1 Assert
//	ALX_CRN120_ASSERT(me->isInit == true);
//	ALX_CRN120_ASSERT(me->wasCtorCalled == true);
//
//	// #2 Prepare variables
//	Alx_Status status = Alx_Err;
//	uint8_t regAddr = addr;
//
//	// #3 Memory Operation
//	if (toWrite)
//	{
//		// #3.1 Write to Memory
//		status = AlxI2c_Master_StartWriteMemStop_Multi(me->i2c, me->i2cAddr, addr, AlxI2c_Master_MemAddrLen_8bit, data, 16, false, me->i2cNumOfTries, me->i2cTimeout_ms); // MF: there is fixed number of 16 bytes to write
//		if (status != Alx_Ok) { ALX_CRN120_TRACE("Err_WriteData"); return status;}
//	}
//	else
//	{
//		// #3.2 Read from Memory
//		// #3.2.1  Write address and register
//		status = AlxI2c_Master_StartWriteStop(me->i2c, me->i2cAddr, &regAddr, 1, me->i2cNumOfTries, me->i2cTimeout_ms);
//		if (status != Alx_Ok) { ALX_CRN120_TRACE("Err_WriteData"); return status;}
//
//		// #3.2.2 Read data
//		status = AlxI2c_Master_StartReadStop(me->i2c, me->i2cAddr, data, 16, me->i2cNumOfTries, me->i2cTimeout_ms); // MF: there is fixed number of 16 bytes to read
//		if (status != Alx_Ok) { ALX_CRN120_TRACE("Err_ReadData"); return status;}
//	}
//
//	// #4 Return OK
//	return Alx_Ok;
//}





/*static Alx_Status AlxCrn120_Reg_Write(AlxCrn120* me, void* reg)
{
	// #1 Prepare variables
	Alx_Status status = Alx_Err;

	uint8_t regAddr = *((uint8_t*)reg);
	uint8_t regLen = *((uint8_t*)reg + sizeof(regAddr));
	uint8_t* regValPtr = (uint8_t*)reg + sizeof(regAddr) + sizeof(regLen);

	// #2 Write address and data
	status = AlxI2c_Master_StartWriteMemStop_Multi(me->i2c, me->i2cAddr, regAddr, AlxI2c_Master_MemAddrLen_8bit, regValPtr, regLen, me->i2cCheckWithRead, me->i2cNumOfTries, me->i2cTimeout_ms);
	if (status != Alx_Ok) { ALX_CRN120_TRACE("Err_WriteData"); return status;}

	// #3 Return OK
	return Alx_Ok;
}
static Alx_Status AlxCrn120_Reg_Read(AlxCrn120* me, void* reg)
{
	// #1 Prepare variables
	Alx_Status status = Alx_Err;

	uint8_t regAddr = *((uint8_t*)reg);
	uint8_t regLen = *((uint8_t*)reg + sizeof(regAddr));
	uint8_t* regValPtr = (uint8_t*)reg + sizeof(regAddr) + sizeof(regLen);

	// #2 Write address and Read data
	status = AlxI2c_Master_StartReadMemStop(me->i2c, me->i2cAddr, regAddr, AlxI2c_Master_MemAddrLen_8bit, regValPtr, regLen, me->i2cNumOfTries, me->i2cTimeout_ms);
	if (status != Alx_Ok) { ALX_CRN120_TRACE("Err_ReadData"); return status ; }

	// #3 Return OK
	return Alx_Ok;
}
static Alx_Status AlxCrn120_Reg_WriteAll(AlxCrn120* me)
{
	// #1 Prepare variables
	Alx_Status status = Alx_Err;

	// #2 Write Registers
	// #2.1 Register 00h
	status = AlxCrn120_Reg_Write(me, &me->reg._00h.val.raw[0]);
	if (status != Alx_Ok) { ALX_CRN120_TRACE("Err_00h_Byte0_Addr					"); return status;}
	status = AlxCrn120_Reg_Write(me, &me->reg._00h.val.raw[10]);
	if (status != Alx_Ok) { ALX_CRN120_TRACE("Err_00h_Byte10_StaticLockBytes		"); return status;}
	status = AlxCrn120_Reg_Write(me, &me->reg._00h.val.raw[11]);
	if (status != Alx_Ok) { ALX_CRN120_TRACE("Err_00h_Byte11_StaticLockBytes		"); return status;}
	status = AlxCrn120_Reg_Write(me, &me->reg._00h.val.raw[12]);
	if (status != Alx_Ok) { ALX_CRN120_TRACE("Err_00h_Byte12_CapabilityContainer	"); return status;}
	status = AlxCrn120_Reg_Write(me, &me->reg._00h.val.raw[13]);
	if (status != Alx_Ok) { ALX_CRN120_TRACE("Err_00h_Byte13_CapabilityContainer	"); return status;}
	status = AlxCrn120_Reg_Write(me, &me->reg._00h.val.raw[14]);
	if (status != Alx_Ok) { ALX_CRN120_TRACE("Err_00h_Byte14_CapabilityContainer	"); return status;}
	status = AlxCrn120_Reg_Write(me, &me->reg._00h.val.raw[15]);
	if (status != Alx_Ok) { ALX_CRN120_TRACE("Err_00h_Byte15_CapabilityContainer	"); return status;}

	// #2.2 Register 38h
	status = AlxCrn120_Reg_Write(me, &me->reg._38h.val.raw[8]);
	if (status != Alx_Ok) { ALX_CRN120_TRACE("Err_38h_Byte8_DynamicLockBytes		"); return status;}
	status = AlxCrn120_Reg_Write(me, &me->reg._38h.val.raw[9]);
	if (status != Alx_Ok) { ALX_CRN120_TRACE("Err_38h_Byte9_DynamicLockBytes		"); return status;}
	status = AlxCrn120_Reg_Write(me, &me->reg._38h.val.raw[10]);
	if (status != Alx_Ok) { ALX_CRN120_TRACE("Err_38h_Byte10_DynamicLockBytes		"); return status;}
	status = AlxCrn120_Reg_Write(me, &me->reg._38h.val.raw[15]);
	if (status != Alx_Ok) { ALX_CRN120_TRACE("Err_38h_Byte15_AUTH0					"); return status;}

	// #2.3 Register 39h
	status = AlxCrn120_Reg_Write(me, &me->reg._39h.val.raw[0]);
	if (status != Alx_Ok) { ALX_CRN120_TRACE("Err_39h_Byte0_ACCESS					"); return status;}
	status = AlxCrn120_Reg_Write(me, &me->reg._39h.val.raw[4]);
	if (status != Alx_Ok) { ALX_CRN120_TRACE("Err_39h_Byte4_PWD						"); return status;}
	status = AlxCrn120_Reg_Write(me, &me->reg._39h.val.raw[5]);
	if (status != Alx_Ok) { ALX_CRN120_TRACE("Err_39h_Byte5_PWD						"); return status;}
	status = AlxCrn120_Reg_Write(me, &me->reg._39h.val.raw[6]);
	if (status != Alx_Ok) { ALX_CRN120_TRACE("Err_39h_Byte6_PWD						"); return status;}
	status = AlxCrn120_Reg_Write(me, &me->reg._39h.val.raw[7]);
	if (status != Alx_Ok) { ALX_CRN120_TRACE("Err_39h_Byte7_PWD						"); return status;}
	status = AlxCrn120_Reg_Write(me, &me->reg._39h.val.raw[8]);
	if (status != Alx_Ok) { ALX_CRN120_TRACE("Err_39h_Byte8_PACK					"); return status;}
	status = AlxCrn120_Reg_Write(me, &me->reg._39h.val.raw[9]);
	if (status != Alx_Ok) { ALX_CRN120_TRACE("Err_39h_Byte9_PACK					"); return status;}
	status = AlxCrn120_Reg_Write(me, &me->reg._39h.val.raw[12]);
	if (status != Alx_Ok) { ALX_CRN120_TRACE("Err_39h_Byte12_PT_I2C					"); return status;}

	// #2.4 Register 3Ah
	status = AlxCrn120_Reg_Write(me, &me->reg._3Ah_ConfigurationReg.val.raw[0]);
	if (status != Alx_Ok) { ALX_CRN120_TRACE("Err_3Ah_Byte0_NC_REG					"); return status;}
	status = AlxCrn120_Reg_Write(me, &me->reg._3Ah_ConfigurationReg.val.raw[1]);
	if (status != Alx_Ok) { ALX_CRN120_TRACE("Err_3Ah_Byte1_LAST_NDEF_BLOCK			"); return status;}
	status = AlxCrn120_Reg_Write(me, &me->reg._3Ah_ConfigurationReg.val.raw[2]);
	if (status != Alx_Ok) { ALX_CRN120_TRACE("Err_3Ah_Byte2_SRAM_MIRROR_BLOCK		"); return status;}
	status = AlxCrn120_Reg_Write(me, &me->reg._3Ah_ConfigurationReg.val.raw[3]);
	if (status != Alx_Ok) { ALX_CRN120_TRACE("Err_3Ah_Byte3_WDT_LS					"); return status;}
	status = AlxCrn120_Reg_Write(me, &me->reg._3Ah_ConfigurationReg.val.raw[4]);
	if (status != Alx_Ok) { ALX_CRN120_TRACE("Err_3Ah_Byte4_WDT_MS					"); return status;}
	status = AlxCrn120_Reg_Write(me, &me->reg._3Ah_ConfigurationReg.val.raw[5]);
	if (status != Alx_Ok) { ALX_CRN120_TRACE("Err_3Ah_Byte5_I2C_CLOCK_STR			"); return status;}
	status = AlxCrn120_Reg_Write(me, &me->reg._3Ah_ConfigurationReg.val.raw[6]);
	if (status != Alx_Ok) { ALX_CRN120_TRACE("Err_3Ah_Byte6_REG_LOCK				"); return status;}

	// #2.5 Register FEh
	status = AlxCrn120_Reg_Write(me, &me->reg._FEh_SessionReg.val.raw[0]);
	if (status != Alx_Ok) { ALX_CRN120_TRACE("Err_FEh_Byte0_NC_REG					"); return status;}
	status = AlxCrn120_Reg_Write(me, &me->reg._FEh_SessionReg.val.raw[1]);
	if (status != Alx_Ok) { ALX_CRN120_TRACE("Err_FEh_Byte1_LAST_NDEF_BLOCK			"); return status;}
	status = AlxCrn120_Reg_Write(me, &me->reg._FEh_SessionReg.val.raw[2]);
	if (status != Alx_Ok) { ALX_CRN120_TRACE("Err_FEh_Byte2_SRAM_MIRROR_BLOCK		"); return status;}
	status = AlxCrn120_Reg_Write(me, &me->reg._FEh_SessionReg.val.raw[3]);
	if (status != Alx_Ok) { ALX_CRN120_TRACE("Err_FEh_Byte3_WDT_LS					"); return status;}
	status = AlxCrn120_Reg_Write(me, &me->reg._FEh_SessionReg.val.raw[4]);
	if (status != Alx_Ok) { ALX_CRN120_TRACE("Err_FEh_Byte4_WDT_MS					"); return status;}
	//status = AlxCrn120_Reg_Write(me, &me->reg._FEh_SessionReg.val.raw[5]);
	//if (status != Alx_Ok) { ALX_CRN120_TRACE("Err_FEh_Byte5_I2C_CLOCK_STR			"); return status;}		// MF: Read Only Reg
	status = AlxCrn120_Reg_Write(me, &me->reg._FEh_SessionReg.val.raw[6]);
	if (status != Alx_Ok) { ALX_CRN120_TRACE("Err_FEh_Byte6_NS_REG					"); return status;}		// MF: Only bits 2 and 6 are R&W

	// #3 Return Ok
	return Alx_Ok;
}*/


















//static Alx_Status AlxPca9430_Reg_WriteVal(AlxPca9430* me)
//{
//	Alx_Status status = AlxPca9430_Reg_Write(me, &me->reg._0x02_SYSTEM_INT_MASK);
//	if (status != Alx_Ok) { ALX_PCA9430_TRACE("Err_0x02_SYSTEM_INT_MASK			"); return status;}
//
//	status = AlxPca9430_Reg_Write(me, &me->reg._0x04_VRECT_INT_MASK				);
//	if (status != Alx_Ok) { ALX_PCA9430_TRACE("Err_0x04_VRECT_INT_MASK			"); return status;}
//
//	status = AlxPca9430_Reg_Write(me, &me->reg._0x06_BATCHG_INT_MASK			);
//	if (status != Alx_Ok) { ALX_PCA9430_TRACE("Err_0x06_VOUTLDO_INT_MASK		"); return status;}
//
//	status = AlxPca9430_Reg_Write(me, &me->reg._0x08_VRECT_THD					);
//	if (status != Alx_Ok) { ALX_PCA9430_TRACE("Err_0x08_VRECT_THD				"); return status;}
//
//	return Alx_Ok;
//}