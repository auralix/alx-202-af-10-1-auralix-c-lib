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
static Alx_Status AlxCrn120_ReadBlock(AlxCrn120* me, uint32_t addr, uint8_t* data);
static Alx_Status AlxCrn120_WriteBlock(AlxCrn120* me, uint32_t addr, uint8_t* data);
static Alx_Status AlxCrn120_ReadBlockLen(AlxCrn120* me, uint32_t addr, uint8_t* data, uint32_t len);
static Alx_Status AlxCrn120_WriteBlockLen(AlxCrn120* me, uint32_t addr, uint8_t* data, uint32_t len, bool checkWithRead);
static Alx_Status AlxCrn120_ReadSessionRegByte(AlxCrn120* me, AlxCrn120_SessionRegByte rega, uint8_t* regdat);
static Alx_Status AlxCrn120_WriteSessionRegByte(AlxCrn120*me, AlxCrn120_SessionRegByte rega, uint8_t regdat, uint8_t mask);

static Alx_Status AlxCrn120_TraceId(AlxCrn120* me);


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
	// Const
	me->ALX_CRN120_BLOCK_LEN				= 16U;
	me->ALX_CRN120_EEPROM_START				= 0x00;
	me->ALX_CRN120_EEPROM_END				= 0x38;
	me->ALX_CRN120_EEPROM_LEN				= 904U;
	me->ALX_CRN120_SRAM_START				= 0xF8;
	me->ALX_CRN120_SRAM_END					= 0xFB;
	me->ALX_CRN120_SRAM_LEN					= 64U;
	me->ALX_CRN120_SESSION_REG_ADDR			= 0xFE;
	me->ALX_CRN120_SESSION_REG_BYTE_LEN		= 7U;

	// Objects - External
	me->i2c = i2c;

	// Parameters
	me->i2cAddr = i2cAddr;
	me->i2cCheckWithRead = i2cCheckWithRead;
	me->i2cNumOfTries = i2cNumOfTries;
	me->i2cTimeout_ms = i2cTimeout_ms;

	// Variables
	for (uint8_t i = 0; i < sizeof(me->uid); i++) { me->uid[i] = 0; }

	// Info
	me->isInit = false;
	me->isPeriphInit = false;
	me->wasCtorCalled = true;
}


//******************************************************************************
// Functions
//******************************************************************************
Alx_Status AlxCrn120_InitMcuPeriph(AlxCrn120* me)
{
	// #1 Assert
	ALX_CRN120_ASSERT(me->isPeriphInit == false);
	ALX_CRN120_ASSERT(me->wasCtorCalled == true);

	// #2 Prepare Variable
	Alx_Status status = Alx_Err;

	// #3 Init I2C
	status = AlxI2c_Init(me->i2c);
	if (status != Alx_Ok) { ALX_CRN120_TRACE("Err"); return status; }

	// #4 Set isInit
	me->isPeriphInit = true;

	// #5 Return OK
	return Alx_Ok;
}
Alx_Status AlxCrn120_DeInitMcuPeriph(AlxCrn120* me)
{
	// #1 Assert
	ALX_CRN120_ASSERT(me->isPeriphInit == true);
	ALX_CRN120_ASSERT(me->wasCtorCalled == true);

	// #2 Prepare Variable
	Alx_Status status = Alx_Err;

	// #3 Init I2C
	status = AlxI2c_DeInit(me->i2c);
	if (status != Alx_Ok) { ALX_CRN120_TRACE("Err"); return status; }

	// #4 Set isInit
	me->isPeriphInit = false;

	// #5 Return OK
	return Alx_Ok;
}
Alx_Status AlxCrn120_Init(AlxCrn120* me)
{
	// #1 Assert
	ALX_CRN120_ASSERT(me->isInit == false);
	ALX_CRN120_ASSERT(me->isPeriphInit == true);
	ALX_CRN120_ASSERT(me->wasCtorCalled == true);

	// #2 Prepare Variable
	Alx_Status status = Alx_Err;

	// #4 Check if slave ready
	status = AlxI2c_Master_IsSlaveReady(me->i2c, me->i2cAddr, me->i2cNumOfTries, me->i2cTimeout_ms);
	if (status != Alx_Ok) { ALX_CRN120_TRACE("Err"); return status; }

	// #5 Read ID register & Trace ID
	status = AlxCrn120_TraceId(me);
	if (status != Alx_Ok) { ALX_CRN120_TRACE("Err"); return status; }

	// #9 Set isInit
	me->isInit = true;

	// #10 Return OK
	return Alx_Ok;
}
Alx_Status AlxCrn120_DeInit(AlxCrn120* me)
{
	// #1 Assert
	ALX_CRN120_ASSERT(me->isInit == true);
	ALX_CRN120_ASSERT(me->isPeriphInit == true);
	ALX_CRN120_ASSERT(me->wasCtorCalled == true);

	// #2 Prepare Variable
	//Alx_Status status = Alx_Err;	

	// #3 DeInit I2c
	//status = AlxI2c_DeInit(me->i2c);
	//if (status != Alx_Ok) { ALX_CRN120_TRACE("Err_AlxI2c_DeInit"); return status; }

	// #4 Reset isInit
	me->isInit = false;

	// #5 Return OK
	return Alx_Ok;
}
Alx_Status AlxCrn120_ReadEeprom(AlxCrn120*me, uint32_t addr, uint8_t* data, uint32_t len)
{
	// #1 Assert
	ALX_CRN120_ASSERT(me->isInit == true);
	ALX_CRN120_ASSERT(me->isPeriphInit == true);
	ALX_CRN120_ASSERT(me->wasCtorCalled == true);
	ALX_CRN120_ASSERT(data != NULL);
	ALX_CRN120_ASSERT((addr >= me->ALX_CRN120_EEPROM_START) && (addr <= me->ALX_CRN120_EEPROM_END));
	ALX_CRN120_ASSERT((len > 0) && (len <= me->ALX_CRN120_EEPROM_LEN));
	ALX_CRN120_ASSERT((((addr - me->ALX_CRN120_EEPROM_START) * me->ALX_CRN120_BLOCK_LEN) + len) <= me->ALX_CRN120_EEPROM_LEN);

	// #2 Prepare variables
	Alx_Status status = Alx_Err;

	// #3 Read Blocks
	status = AlxCrn120_ReadBlockLen(me, addr, data, len);
	if (status != Alx_Ok) { ALX_CRN120_TRACE("Err"); return status; }

	// #4 Return OK
	return Alx_Ok;
}
Alx_Status AlxCrn120_WriteEeprom(AlxCrn120*me, uint32_t addr, uint8_t* data, uint32_t len)
{
	// #1 Assert
	ALX_CRN120_ASSERT(me->isInit == true);
	ALX_CRN120_ASSERT(me->isPeriphInit == true);
	ALX_CRN120_ASSERT(me->wasCtorCalled == true);
	ALX_CRN120_ASSERT(data != NULL);
	ALX_CRN120_ASSERT((addr >= me->ALX_CRN120_EEPROM_START) && (addr <= me->ALX_CRN120_EEPROM_END));
	ALX_CRN120_ASSERT((len > 0) && (len <= me->ALX_CRN120_EEPROM_LEN));
	ALX_CRN120_ASSERT((((addr - me->ALX_CRN120_EEPROM_START) * me->ALX_CRN120_BLOCK_LEN) + len) <= me->ALX_CRN120_EEPROM_LEN);

	// #2 Prepare variables
	Alx_Status status = Alx_Err;
	uint8_t numOfBlocks = len / me->ALX_CRN120_BLOCK_LEN;
	uint32_t lenRemainder = len % me->ALX_CRN120_BLOCK_LEN;
	uint32_t offset = 0;
	uint32_t AddrCount = 0;

	// #3 Write
	for (uint32_t _tryNo = 1; _tryNo <= me->i2cNumOfTries; _tryNo++)
	{
		// #3.1 Prepare Address Counter
		AddrCount = 0;

		// #3.2 Write numOfBlocks of data, leave remainder
		for (offset = 0; offset < me->ALX_CRN120_BLOCK_LEN * numOfBlocks; offset = offset + me->ALX_CRN120_BLOCK_LEN)
		{
			// #3.2.1 Write Block
			status = AlxCrn120_WriteBlock(me, addr + AddrCount, data + offset); // MF: Timeout is handled in i2c
			if (status != Alx_Ok)
			{
				ALX_CRN120_TRACE("Err");
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
				ALX_CRN120_TRACE("Err");
				continue;
			}

			// #3.4.2 Modify Data
			memcpy(buffRem, data + offset, lenRemainder);

			// #3.4.3 Write
			status = AlxCrn120_WriteBlock(me, addr + AddrCount, buffRem); // MF: Timeout is handled in i2c
			if (status != Alx_Ok)
			{
				ALX_CRN120_TRACE("Err");
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
				ALX_CRN120_TRACE("Err");
				continue;
			}

			// #3.6.3 Compare written and read data
			if (memcmp(data, buff, len) != 0)
			{
				ALX_I2C_TRACE("Err");
				continue;
			}
		}

		// #3.7 Return OK
		return Alx_Ok;
	}

	// #4 If we are here, the number of tries error occured
	if (status != Alx_Ok)
	{
		ALX_CRN120_TRACE("Err");
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
	ALX_CRN120_ASSERT(me->isPeriphInit == true);
	ALX_CRN120_ASSERT(me->wasCtorCalled == true);
	ALX_CRN120_ASSERT(data != NULL);
	ALX_CRN120_ASSERT((addr >= me->ALX_CRN120_SRAM_START) && (addr <= me->ALX_CRN120_SRAM_END));
	ALX_CRN120_ASSERT((len > 0) && (len <= me->ALX_CRN120_SRAM_LEN));
	ALX_CRN120_ASSERT((((addr - me->ALX_CRN120_SRAM_START) * me->ALX_CRN120_BLOCK_LEN) + len) <= me->ALX_CRN120_SRAM_LEN);

	// #2 Prepare variables
	Alx_Status status = Alx_Err;

	// #3 Read Blocks
	status = AlxCrn120_ReadBlockLen(me, addr, data, len);
	if (status != Alx_Ok) { ALX_CRN120_TRACE("Err"); return status; }

	// #4 Return OK
	return Alx_Ok;
}
Alx_Status AlxCrn120_WriteSram(AlxCrn120*me, uint32_t addr, uint8_t* data, uint32_t len)
{
	// #1 Assert
	ALX_CRN120_ASSERT(me->isInit == true);
	ALX_CRN120_ASSERT(me->isPeriphInit == true);
	ALX_CRN120_ASSERT(me->wasCtorCalled == true);
	ALX_CRN120_ASSERT(data != NULL);
	ALX_CRN120_ASSERT((addr >= me->ALX_CRN120_SRAM_START) && (addr <= me->ALX_CRN120_SRAM_END));
	ALX_CRN120_ASSERT((len > 0) && (len <= me->ALX_CRN120_SRAM_LEN));
	ALX_CRN120_ASSERT((((addr - me->ALX_CRN120_SRAM_START) * me->ALX_CRN120_BLOCK_LEN) + len) <= me->ALX_CRN120_SRAM_LEN);

	// #2 Prepare variables
	Alx_Status status = Alx_Err;

	// #3 Write Blocks
	status = AlxCrn120_WriteBlockLen(me, addr, data, len, me->i2cCheckWithRead);
	if (status != Alx_Ok) { ALX_CRN120_TRACE("Err"); return status; }

	// #4 Return OK
	return Alx_Ok;
}
Alx_Status AlxCrn120_ReadSessionReg(AlxCrn120*me, AlxCrn120_SessionRegByte rega, uint8_t* regdat)
{
	// #1 Assert
	ALX_CRN120_ASSERT(me->isInit == true);
	ALX_CRN120_ASSERT(me->isPeriphInit == true);
	ALX_CRN120_ASSERT(me->wasCtorCalled == true);
	ALX_CRN120_ASSERT(regdat != NULL);

	// #2 Prepare variables
	Alx_Status status = Alx_Err;

	// #3 Read Byte
	status = AlxCrn120_ReadSessionRegByte(me, rega, regdat);
	if (status != Alx_Ok) { ALX_CRN120_TRACE("Err"); return status; }

	// #4 Return OK
	return Alx_Ok;
}
Alx_Status AlxCrn120_WriteSessionReg(AlxCrn120*me, AlxCrn120_SessionRegByte rega, uint8_t regdat, uint8_t mask)
{
	// #1 Assert
	ALX_CRN120_ASSERT(me->isInit == true);
	ALX_CRN120_ASSERT(me->isPeriphInit == true);
	ALX_CRN120_ASSERT(me->wasCtorCalled == true);
	ALX_CRN120_ASSERT(rega != AlxCrn120_SessionRegByte_I2C_CLOCK_STR); // Read Only Byte

	// #2 Prepare variables
	Alx_Status status = Alx_Err;

	// #3 Write Byte
	status = AlxCrn120_WriteSessionRegByte(me, rega, regdat, mask);
	if (status != Alx_Ok) { ALX_CRN120_TRACE("Err"); return status; }

	// #4 Return OK
	return Alx_Ok;
}
Alx_Status AlxCrn120_ReadSessionRegAll(AlxCrn120*me, uint8_t* data, uint8_t len)
{
	// #1 Assert
	ALX_CRN120_ASSERT(me->isInit == true);
	ALX_CRN120_ASSERT(me->isPeriphInit == true);
	ALX_CRN120_ASSERT(me->wasCtorCalled == true);
	ALX_CRN120_ASSERT(data != NULL);
	ALX_CRN120_ASSERT(len == me->ALX_CRN120_SESSION_REG_BYTE_LEN);

	// #2 Prepare variables
	Alx_Status status = Alx_Err;

	// #3 Read Session Reg
	for (uint8_t i = 0; i < me->ALX_CRN120_SESSION_REG_BYTE_LEN; i++)
	{
		status = AlxCrn120_ReadSessionRegByte(me, (AlxCrn120_SessionRegByte)i, &data[i]);
		if (status != Alx_Ok) { ALX_CRN120_TRACE("Err"); return status; }
	}

	// #4 Return OK
	return Alx_Ok;
}
Alx_Status AlxCrn120_WriteSessionRegAll(AlxCrn120*me, uint8_t* data, uint8_t* mask, uint8_t len)
{
	// #1 Assert
	ALX_CRN120_ASSERT(me->isInit == true);
	ALX_CRN120_ASSERT(me->isPeriphInit == true);
	ALX_CRN120_ASSERT(me->wasCtorCalled == true);
	ALX_CRN120_ASSERT(data != NULL);
	ALX_CRN120_ASSERT(mask != NULL);
	ALX_CRN120_ASSERT(len == me->ALX_CRN120_SESSION_REG_BYTE_LEN);

	// #2 Prepare variables
	Alx_Status status = Alx_Err;

	// #3 Read Session Reg
	for (uint8_t i = 0; i < me->ALX_CRN120_SESSION_REG_BYTE_LEN; i++)
	{
		status = AlxCrn120_WriteSessionRegByte(me, (AlxCrn120_SessionRegByte)i, data[i], mask[i]);
		if (status != Alx_Ok) { ALX_CRN120_TRACE("Err"); return Alx_Err; }
	}

	// #4 Return OK
	return Alx_Ok;
}
Alx_Status AlxCrn120_EnableSramMirror(AlxCrn120*me)
{
	// #1 Assert
	ALX_CRN120_ASSERT(me->isInit == true);
	ALX_CRN120_ASSERT(me->isPeriphInit == true);
	ALX_CRN120_ASSERT(me->wasCtorCalled == true);

	// #2 Prepare variables
	Alx_Status status = Alx_Err;
	uint8_t data = 0U;
	uint8_t mask = 2U;	// 2nd bit SRAM_MIRROR_ON_OFF is changed

	// #3 Write address and byte
	for (uint32_t _tryNo = 1; _tryNo <= me->i2cNumOfTries; _tryNo++)
	{
		// #3.1 Read NC_REG
		status = AlxCrn120_ReadSessionRegByte(me, AlxCrn120_SessionRegByte_NC_REG, &data);
		if (status != Alx_Ok)
		{
			ALX_CRN120_TRACE("Err");
			continue;
		}

		// #3.2 Set SRAM_MIRROR_ON_OFF bit to 1
		data |= (1UL << 1);

		// #3.3 Write NC_REG
		status = AlxCrn120_WriteSessionRegByte(me, AlxCrn120_SessionRegByte_NC_REG, data, mask);
		if (status != Alx_Ok)
		{
			ALX_CRN120_TRACE("Err");
			continue;
		}

		// #3.4 Return OK
		return Alx_Ok;
	}

	// #4 If we are here, the number of tries error occured
	if (status != Alx_Ok)
	{
		ALX_CRN120_TRACE("Err");
		return Alx_ErrNumOfTries;
	}

	// #5 Assert	// We should not get here
	ALX_CRN120_TRACE(false);
	return Alx_Err;
}
bool AlxCrn120_IsI2cCheckWithReadEnabled(AlxCrn120* me)
{
	// #1 Assert
	ALX_CRN120_ASSERT(me->isInit == true);
	ALX_CRN120_ASSERT(me->isPeriphInit == true);
	ALX_CRN120_ASSERT(me->wasCtorCalled == true);

	// #2 Return i2cCheckWithRead
	return me->i2cCheckWithRead;
}
void AlxCrn120_I2cCheckWithReadEnable(AlxCrn120* me)
{
	// #1 Assert
	ALX_CRN120_ASSERT(me->isInit == true);
	ALX_CRN120_ASSERT(me->isPeriphInit == true);
	ALX_CRN120_ASSERT(me->wasCtorCalled == true);

	// #2 Enable i2cCheckWithRead
	me->i2cCheckWithRead = true;
}
void AlxCrn120_I2cCheckWithReadDisable(AlxCrn120* me)
{
	// #1 Assert
	ALX_CRN120_ASSERT(me->isInit == true);
	ALX_CRN120_ASSERT(me->isPeriphInit == true);
	ALX_CRN120_ASSERT(me->wasCtorCalled == true);

	// #2 Disable i2cCheckWithRead
	me->i2cCheckWithRead = false;
}


//******************************************************************************
// Private Functions
//******************************************************************************
static Alx_Status AlxCrn120_ReadBlock(AlxCrn120* me, uint32_t addr, uint8_t* data)
{
	// #1 Prepare variables
	Alx_Status status = Alx_Err;
	uint8_t regAddr = addr;

	// #2 Read from Memory
	// #2.1  Write address and register
	status = AlxI2c_Master_StartWriteStop(me->i2c, me->i2cAddr, &regAddr, 1, me->i2cNumOfTries, me->i2cTimeout_ms);
	if (status != Alx_Ok) { ALX_CRN120_TRACE("Err"); return status;}

	// #2.2 Read data
	status = AlxI2c_Master_StartReadStop(me->i2c, me->i2cAddr, data, 16, me->i2cNumOfTries, me->i2cTimeout_ms); // MF: there is fixed number of 16 bytes to read
	if (status != Alx_Ok) { ALX_CRN120_TRACE("Err"); return status;}

	// #3 Return OK
	return Alx_Ok;
}
static Alx_Status AlxCrn120_WriteBlock(AlxCrn120* me, uint32_t addr, uint8_t* data)
{
	// #1 Prepare variables
	Alx_Status status = Alx_Err;

	// #2 Write to Memory
	status = AlxI2c_Master_StartWriteMemStop_Multi(me->i2c, me->i2cAddr, addr, AlxI2c_Master_MemAddrLen_8bit, data, 16, false, me->i2cNumOfTries, me->i2cTimeout_ms);	// MF: CheckWithRead will be handled on higher level
	if (status != Alx_Ok) { ALX_CRN120_TRACE("Err"); return status;}

	// #3 Return OK
	return Alx_Ok;
}
static Alx_Status AlxCrn120_ReadBlockLen(AlxCrn120* me, uint32_t addr, uint8_t* data, uint32_t len)
{
	// #2 Prepare variables
	Alx_Status status = Alx_Ok;
	uint8_t numOfBlocks = len / me->ALX_CRN120_BLOCK_LEN;
	uint32_t lenRemainder = len % me->ALX_CRN120_BLOCK_LEN;
	uint32_t offset = 0;
	uint32_t AddrCount = 0;

	// #3 Read
	for (uint32_t _tryNo = 1; _tryNo <= me->i2cNumOfTries; _tryNo++)
	{
		// #3.1 Prepare Address Counter
		AddrCount = 0;

		// #3.2 Read numOfBlocks of data, leave remainder
		for (offset = 0; offset < me->ALX_CRN120_BLOCK_LEN * numOfBlocks; offset = offset + me->ALX_CRN120_BLOCK_LEN)
		{
			// #3.2.1 Read Block
			status = AlxCrn120_ReadBlock(me, addr + AddrCount, data + offset); // MF: Timeout is handled in i2c
			if (status != Alx_Ok)
			{
				ALX_CRN120_TRACE("Err");
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
				ALX_CRN120_TRACE("Err");
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
		ALX_CRN120_TRACE("Err");
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
	uint8_t numOfBlocks = len / me->ALX_CRN120_BLOCK_LEN;
	uint32_t lenRemainder = len % me->ALX_CRN120_BLOCK_LEN;
	uint32_t offset = 0;
	uint32_t AddrCount = 0;

	// #3 Write
	for (uint32_t _tryNo = 1; _tryNo <= me->i2cNumOfTries; _tryNo++)
	{
		// #3.1 Prepare Address Counter
		AddrCount = 0;

		// #3.2 Write numOfBlocks of data, leave remainder
		for (offset = 0; offset < me->ALX_CRN120_BLOCK_LEN * numOfBlocks; offset = offset + me->ALX_CRN120_BLOCK_LEN)
		{
			// #3.2.1 Write Block
			status = AlxCrn120_WriteBlock(me, addr + AddrCount, data + offset); // MF: Timeout is handled in i2c
			if (status != Alx_Ok)
			{
				ALX_CRN120_TRACE("Err");
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
				ALX_CRN120_TRACE("Err");
				continue;
			}

			// #3.4.2 Modify Data
			memcpy(buffRem, data + offset, lenRemainder);

			// #3.4.3 Write
			status = AlxCrn120_WriteBlock(me, addr + AddrCount, buffRem); // MF: Timeout is handled in i2c
			if (status != Alx_Ok)
			{
				ALX_CRN120_TRACE("Err");
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
				ALX_CRN120_TRACE("Err");
				return status;
			}

			// #3.5.3 Compare written and read data
			if (memcmp(data, buff, len) != 0)
			{
				ALX_I2C_TRACE("Err");
				return Alx_Err;
			}
		}

		// #3.6 Return OK
		return Alx_Ok;
	}

	// #4 If we are here, the number of tries error occured
	if (status != Alx_Ok)
	{
		ALX_CRN120_TRACE("Err");
		return Alx_ErrNumOfTries;
	}

	// #5 Assert	// We should not get here
	ALX_CRN120_TRACE(false);
	return Alx_Err;
}
static Alx_Status AlxCrn120_ReadSessionRegByte(AlxCrn120* me, AlxCrn120_SessionRegByte rega, uint8_t* regdat)
{
	// #1 Prepare variables
	Alx_Status status = Alx_Err;

	// #2 Write address and byte
	for (uint32_t _tryNo = 1; _tryNo <= me->i2cNumOfTries; _tryNo++)
	{
		// #2.1 Write REGDAT
		status = AlxI2c_Master_StartWriteMemStop_Multi(me->i2c, me->i2cAddr, me->ALX_CRN120_SESSION_REG_ADDR, AlxI2c_Master_MemAddrLen_8bit, (const uint8_t*)&rega, 1, false, me->i2cNumOfTries, me->i2cTimeout_ms);
		if (status != Alx_Ok)
		{
			ALX_CRN120_TRACE("Err");
			continue;
		}

		// #2.2 Read Data
		status = AlxI2c_Master_StartReadStop(me->i2c, me->i2cAddr, regdat, 1, me->i2cNumOfTries, me->i2cTimeout_ms);
		if (status != Alx_Ok)
		{
			ALX_CRN120_TRACE("Err");
			continue;
		}

		// #2.3 Return OK
		return Alx_Ok;
	}

	// #3 If we are here, the number of tries error occured
	if (status != Alx_Ok)
	{
		ALX_CRN120_TRACE("Err");
		return Alx_ErrNumOfTries;
	}

	// #4 Assert	// We should not get here
	ALX_CRN120_TRACE(false);
	return Alx_Err;
}
static Alx_Status AlxCrn120_WriteSessionRegByte(AlxCrn120*me, AlxCrn120_SessionRegByte rega, uint8_t regdat, uint8_t mask)
{
	// #1 Prepare variables
	Alx_Status status = Alx_Err;
	uint8_t dataWrite[3] = { (uint8_t)rega, mask, regdat };

	// #2 Write address and byte
	for (uint32_t _tryNo = 1; _tryNo <= me->i2cNumOfTries; _tryNo++)
	{
		// #2.1 Write REGDAT
		status = AlxI2c_Master_StartWriteMemStop_Multi(me->i2c, me->i2cAddr, me->ALX_CRN120_SESSION_REG_ADDR, AlxI2c_Master_MemAddrLen_8bit, dataWrite, sizeof(dataWrite), false, me->i2cNumOfTries, me->i2cTimeout_ms);
		if (status != Alx_Ok)
		{
			ALX_CRN120_TRACE("Err");
			continue;
		}

		// #2.2 Return OK
		return Alx_Ok;
	}

	// #3 If we are here, the number of tries error occured
	if (status != Alx_Ok)
	{
		ALX_CRN120_TRACE("Err");
		return Alx_ErrNumOfTries;
	}

	// #4 Assert	// We should not get here
	ALX_CRN120_TRACE(false);
	return Alx_Err;
}
static Alx_Status AlxCrn120_TraceId(AlxCrn120* me)
{
	// #1 Prepare variables
	Alx_Status status = Alx_Err;

	// #2 Read Device ID register
	status = AlxCrn120_ReadBlockLen(me, 0x00, me->uid, sizeof(me->uid));
	if (status != Alx_Ok) { ALX_CRN120_TRACE("Err"); return status; }

	// #3 Trace
	ALX_CRN120_TRACE_FORMAT("\r\n");
	ALX_CRN120_TRACE_FORMAT("Auralix C Library ALX Wireless Charging Receiver CRN120 Module Identification:\r\n");
	ALX_CRN120_TRACE_FORMAT("- Device ID: 0x%02x", me->uid[0]);		// MF: The two means you always want the output to be(at least) two characters wide. The zero means if padding is necessary, to use zeros instead of spaces.
	ALX_CRN120_TRACE_FORMAT("%02x", me->uid[1]);
	ALX_CRN120_TRACE_FORMAT("%02x", me->uid[2]);
	ALX_CRN120_TRACE_FORMAT("%02x", me->uid[3]);
	ALX_CRN120_TRACE_FORMAT("%02x", me->uid[4]);
	ALX_CRN120_TRACE_FORMAT("%02x", me->uid[5]);
	ALX_CRN120_TRACE_FORMAT("%02x\r\n", me->uid[6]);
	ALX_CRN120_TRACE_FORMAT("\r\n");

	// #4 Return OK
	return Alx_Ok;
}
























//static void AlxCrn120_RegStruct_SetValToZero(AlxCrn120* me)
//{
//	me->blk._00h						.val.raw[0]		= 0b00000000;	// Addr.
//	//me->blk._00h						.val.raw[1]		= 0b00000000;	// Serial number (UID)	// MF: Read Only Reg
//	//me->blk._00h						.val.raw[2]		= 0b00000000;	// Serial number (UID)	// MF: Read Only Reg
//	//me->blk._00h						.val.raw[3]		= 0b00000000;	// Serial number (UID)	// MF: Read Only Reg
//	//me->blk._00h						.val.raw[4]		= 0b00000000;	// Serial number (UID)	// MF: Read Only Reg
//	//me->blk._00h						.val.raw[5]		= 0b00000000;	// Serial number (UID)	// MF: Read Only Reg
//	//me->blk._00h						.val.raw[6]		= 0b00000000;	// Serial number (UID)	// MF: Read Only Reg
//	//me->blk._00h						.val.raw[7]		= 0b00000000;	// Internal
//	//me->blk._00h						.val.raw[8]		= 0b00000000;	// Internal
//	//me->blk._00h						.val.raw[9]		= 0b00000000;	// Internal
//	me->blk._00h						.val.raw[10]	= 0b00000000;	// Static Lock Bytes
//	me->blk._00h						.val.raw[11]	= 0b00000000;	// Static Lock Bytes
//	me->blk._00h						.val.raw[12]	= 0b00000000;	// Capability Container (CC)
//	me->blk._00h						.val.raw[13]	= 0b00000000;	// Capability Container (CC)
//	me->blk._00h						.val.raw[14]	= 0b00000000;	// Capability Container (CC)
//	me->blk._00h						.val.raw[15]	= 0b00000000;	// Capability Container (CC)
//
//	//me->blk._38h						.val.raw[0]		= 0b00000000;	// Protected user memory
//	//me->blk._38h						.val.raw[1]		= 0b00000000;	// Protected user memory
//	//me->blk._38h						.val.raw[2]		= 0b00000000;	// Protected user memory
//	//me->blk._38h						.val.raw[3]		= 0b00000000;	// Protected user memory
//	//me->blk._38h						.val.raw[4]		= 0b00000000;	// Protected user memory
//	//me->blk._38h						.val.raw[5]		= 0b00000000;	// Protected user memory
//	//me->blk._38h						.val.raw[6]		= 0b00000000;	// Protected user memory
//	//me->blk._38h						.val.raw[7]		= 0b00000000;	// Protected user memory
//	me->blk._38h						.val.raw[8]		= 0b00000000;	// Dynamic lock bytes
//	me->blk._38h						.val.raw[9]		= 0b00000000;	// Dynamic lock bytes
//	me->blk._38h						.val.raw[10]	= 0b00000000;	// Dynamic lock bytes
//	//me->blk._38h						.val.raw[11]	= 0b00000000;	// 00h
//	//me->blk._38h						.val.raw[12]	= 0b00000000;	// RFU
//	//me->blk._38h						.val.raw[13]	= 0b00000000;	// RFU
//	//me->blk._38h						.val.raw[14]	= 0b00000000;	// RFU
//	me->blk._38h						.val.raw[15]	= 0b00000000;	// AUTH0
//
//	me->blk._39h						.val.raw[0]		= 0b00000000;	// ACCESS
//	//me->blk._39h						.val.raw[1]		= 0b00000000;	// RFU
//	//me->blk._39h						.val.raw[2]		= 0b00000000;	// RFU
//	//me->blk._39h						.val.raw[3]		= 0b00000000;	// RFU
//	me->blk._39h						.val.raw[4]		= 0b00000000;	// PWD
//	me->blk._39h						.val.raw[5]		= 0b00000000;	// PWD
//	me->blk._39h						.val.raw[6]		= 0b00000000;	// PWD
//	me->blk._39h						.val.raw[7]		= 0b00000000;	// PWD
//	me->blk._39h						.val.raw[8]		= 0b00000000;	// PACK
//	me->blk._39h						.val.raw[9]		= 0b00000000;	// PACK
//	//me->blk._39h						.val.raw[10]	= 0b00000000;	// RFU
//	//me->blk._39h						.val.raw[11]	= 0b00000000;	// RFU
//	me->blk._39h						.val.raw[12]	= 0b00000000;	// PT_I2C
//	//me->blk._39h						.val.raw[13]	= 0b00000000;	// RFU
//	//me->blk._39h						.val.raw[14]	= 0b00000000;	// RFU
//	//me->blk._39h						.val.raw[15]	= 0b00000000;	// RFU
//
//	me->blk._3Ah_ConfigurationReg		.val.raw[0]		= 0b00000000;	// NC_REG
//	me->blk._3Ah_ConfigurationReg		.val.raw[1]		= 0b00000000;	// LAST_NDEF_blk
//	me->blk._3Ah_ConfigurationReg		.val.raw[2]		= 0b00000000;	// SRAM_MIRROR_blk
//	me->blk._3Ah_ConfigurationReg		.val.raw[3]		= 0b00000000;	// WDT_LS
//	me->blk._3Ah_ConfigurationReg		.val.raw[4]		= 0b00000000;	// WDT_MS
//	me->blk._3Ah_ConfigurationReg		.val.raw[5]		= 0b00000000;	// I2C_CLOCK_STR
//	me->blk._3Ah_ConfigurationReg		.val.raw[6]		= 0b00000000;	// REG_LOCK
//	//me->blk._3Ah_ConfigurationReg		.val.raw[7]		= 0b00000000;	// RFU
//	//me->blk._3Ah_ConfigurationReg		.val.raw[8]		= 0b00000000;	// 00h
//	//me->blk._3Ah_ConfigurationReg		.val.raw[9]		= 0b00000000;	// 00h
//	//me->blk._3Ah_ConfigurationReg		.val.raw[10]	= 0b00000000;	// 00h
//	//me->blk._3Ah_ConfigurationReg		.val.raw[11]	= 0b00000000;	// 00h
//	//me->blk._3Ah_ConfigurationReg		.val.raw[12]	= 0b00000000;	// 00h
//	//me->blk._3Ah_ConfigurationReg		.val.raw[13]	= 0b00000000;	// 00h
//	//me->blk._3Ah_ConfigurationReg		.val.raw[14]	= 0b00000000;	// 00h
//	//me->blk._3Ah_ConfigurationReg		.val.raw[15]	= 0b00000000;	// 00h
//
//	me->blk._FEh_SessionReg				.val.raw[0]		= 0b00000000;	// NC_REG
//	me->blk._FEh_SessionReg				.val.raw[1]		= 0b00000000;	// LAST_NDEF_blk
//	me->blk._FEh_SessionReg				.val.raw[2]		= 0b00000000;	// SRAM_MIRROR_blk
//	me->blk._FEh_SessionReg				.val.raw[3]		= 0b00000000;	// WDT_LS
//	me->blk._FEh_SessionReg				.val.raw[4]		= 0b00000000;	// WDT_MS
//	//me->blk._FEh_SessionReg			.val.raw[5]		= 0b00000000;	// I2C_CLOCK_STR	// MF: Read Only Reg
//	me->blk._FEh_SessionReg				.val.raw[6]		= 0b00000000;	// NS_REG			// MF: Only bits 2 and 6 are R&W
//	//me->blk._FEh_SessionReg			.val.raw[7]		= 0b00000000;	// RFU
//	//me->blk._FEh_SessionReg			.val.raw[8]		= 0b00000000;	// 00h
//	//me->blk._FEh_SessionReg			.val.raw[9]		= 0b00000000;	// 00h
//	//me->blk._FEh_SessionReg			.val.raw[10]	= 0b00000000;	// 00h
//	//me->blk._FEh_SessionReg			.val.raw[11]	= 0b00000000;	// 00h
//	//me->blk._FEh_SessionReg			.val.raw[12]	= 0b00000000;	// 00h
//	//me->blk._FEh_SessionReg			.val.raw[13]	= 0b00000000;	// 00h
//	//me->blk._FEh_SessionReg			.val.raw[14]	= 0b00000000;	// 00h
//	//me->blk._FEh_SessionReg			.val.raw[15]	= 0b00000000;	// 00h
//}
//static void AlxCrn120_RegStruct_SetValToDefault(AlxCrn120* me)
//{
//	me->blk._00h						.val.raw[0]		= 0b10101010;	// Addr.	// MF. Reading apways returns 04h
//	//me->blk._00h						.val.raw[1]		= 0b00000000;	// Serial number (UID)
//	//me->blk._00h						.val.raw[2]		= 0b00000000;	// Serial number (UID)
//	//me->blk._00h						.val.raw[3]		= 0b00000000;	// Serial number (UID)
//	//me->blk._00h						.val.raw[4]		= 0b00000000;	// Serial number (UID)
//	//me->blk._00h						.val.raw[5]		= 0b00000000;	// Serial number (UID)
//	//me->blk._00h						.val.raw[6]		= 0b00000000;	// Serial number (UID)
//	//me->blk._00h						.val.raw[7]		= 0b00000000;	// Internal
//	//me->blk._00h						.val.raw[8]		= 0b00000000;	// Internal
//	//me->blk._00h						.val.raw[9]		= 0b00000000;	// Internal
//	me->blk._00h						.val.raw[10]	= 0b00000000;	// Static Lock Bytes
//	me->blk._00h						.val.raw[11]	= 0b00000000;	// Static Lock Bytes
//	me->blk._00h						.val.raw[12]	= 0b00000000;	// Capability Container (CC)
//	me->blk._00h						.val.raw[13]	= 0b00000000;	// Capability Container (CC)
//	me->blk._00h						.val.raw[14]	= 0b00000000;	// Capability Container (CC)
//	me->blk._00h						.val.raw[15]	= 0b00000000;	// Capability Container (CC)
//
//	//me->blk._38h						.val.raw[0]		= 0b00000000;	// Protected user memory
//	//me->blk._38h						.val.raw[1]		= 0b00000000;	// Protected user memory
//	//me->blk._38h						.val.raw[2]		= 0b00000000;	// Protected user memory
//	//me->blk._38h						.val.raw[3]		= 0b00000000;	// Protected user memory
//	//me->blk._38h						.val.raw[4]		= 0b00000000;	// Protected user memory
//	//me->blk._38h						.val.raw[5]		= 0b00000000;	// Protected user memory
//	//me->blk._38h						.val.raw[6]		= 0b00000000;	// Protected user memory
//	//me->blk._38h						.val.raw[7]		= 0b00000000;	// Protected user memory
//	me->blk._38h						.val.raw[8]		= 0b00000000;	// Dynamic lock bytes
//	me->blk._38h						.val.raw[9]		= 0b00000000;	// Dynamic lock bytes
//	me->blk._38h						.val.raw[10]	= 0b00000000;	// Dynamic lock bytes
//	//me->blk._38h						.val.raw[11]	= 0b00000000;	// 00h
//	//me->blk._38h						.val.raw[12]	= 0b00000000;	// RFU
//	//me->blk._38h						.val.raw[13]	= 0b00000000;	// RFU
//	//me->blk._38h						.val.raw[14]	= 0b00000000;	// RFU
//	me->blk._38h						.val.raw[15]	= 0b11111111;	// AUTH0
//
//	me->blk._39h						.val.raw[0]		= 0b00000000;	// ACCESS
//	//me->blk._39h						.val.raw[1]		= 0b00000000;	// RFU
//	//me->blk._39h						.val.raw[2]		= 0b00000000;	// RFU
//	//me->blk._39h						.val.raw[3]		= 0b00000000;	// RFU
//	me->blk._39h						.val.raw[4]		= 0b11111111;	// PWD	// MF: Reading PWD always returns 00000000h
//	me->blk._39h						.val.raw[5]		= 0b11111111;	// PWD	// MF: Reading PWD always returns 00000000h
//	me->blk._39h						.val.raw[6]		= 0b11111111;	// PWD	// MF: Reading PWD always returns 00000000h
//	me->blk._39h						.val.raw[7]		= 0b11111111;	// PWD	// MF: Reading PWD always returns 00000000h
//	me->blk._39h						.val.raw[8]		= 0b00000000;	// PACK
//	me->blk._39h						.val.raw[9]		= 0b00000000;	// PACK
//	//me->blk._39h						.val.raw[10]	= 0b00000000;	// RFU
//	//me->blk._39h						.val.raw[11]	= 0b00000000;	// RFU
//	me->blk._39h						.val.raw[12]	= 0b00000000;	// PT_I2C
//	//me->blk._39h						.val.raw[13]	= 0b00000000;	// RFU
//	//me->blk._39h						.val.raw[14]	= 0b00000000;	// RFU
//	//me->blk._39h						.val.raw[15]	= 0b00000000;	// RFU
//
//	me->blk._3Ah_ConfigurationReg		.val.raw[0]		= 0b00000001;	// NC_REG
//	me->blk._3Ah_ConfigurationReg		.val.raw[1]		= 0b00000000;	// LAST_NDEF_blk
//	me->blk._3Ah_ConfigurationReg		.val.raw[2]		= 0b11111000;	// SRAM_MIRROR_blk
//	me->blk._3Ah_ConfigurationReg		.val.raw[3]		= 0b01001000;	// WDT_LS
//	me->blk._3Ah_ConfigurationReg		.val.raw[4]		= 0b00001000;	// WDT_MS
//	me->blk._3Ah_ConfigurationReg		.val.raw[5]		= 0b00000001;	// I2C_CLOCK_STR
//	me->blk._3Ah_ConfigurationReg		.val.raw[6]		= 0b00000000;	// REG_LOCK
//	//me->blk._3Ah_ConfigurationReg		.val.raw[7]		= 0b00000000;	// RFU
//	//me->blk._3Ah_ConfigurationReg		.val.raw[8]		= 0b00000000;	// 00h
//	//me->blk._3Ah_ConfigurationReg		.val.raw[9]		= 0b00000000;	// 00h
//	//me->blk._3Ah_ConfigurationReg		.val.raw[10]	= 0b00000000;	// 00h
//	//me->blk._3Ah_ConfigurationReg		.val.raw[11]	= 0b00000000;	// 00h
//	//me->blk._3Ah_ConfigurationReg		.val.raw[12]	= 0b00000000;	// 00h
//	//me->blk._3Ah_ConfigurationReg		.val.raw[13]	= 0b00000000;	// 00h
//	//me->blk._3Ah_ConfigurationReg		.val.raw[14]	= 0b00000000;	// 00h
//	//me->blk._3Ah_ConfigurationReg		.val.raw[15]	= 0b00000000;	// 00h
//
//	me->blk._FEh_SessionReg				.val.raw[0]		= 0b00000001;	// NC_REG
//	me->blk._FEh_SessionReg				.val.raw[1]		= 0b00000000;	// LAST_NDEF_blk
//	me->blk._FEh_SessionReg				.val.raw[2]		= 0b11111000;	// SRAM_MIRROR_blk
//	me->blk._FEh_SessionReg				.val.raw[3]		= 0b01001000;	// WDT_LS
//	me->blk._FEh_SessionReg				.val.raw[4]		= 0b00001000;	// WDT_MS
//	//me->blk._FEh_SessionReg			.val.raw[5]		= 0b00000001;	// I2C_CLOCK_STR	// MF: Read Only Reg
//	me->blk._FEh_SessionReg				.val.raw[6]		= 0b00000000;	// NS_REG			// MF: Only bits 2 and 6 are R&W
//	//me->blk._FEh_SessionReg			.val.raw[7]		= 0b00000000;	// RFU
//	//me->blk._FEh_SessionReg			.val.raw[8]		= 0b00000000;	// 00h
//	//me->blk._FEh_SessionReg			.val.raw[9]		= 0b00000000;	// 00h
//	//me->blk._FEh_SessionReg			.val.raw[10]	= 0b00000000;	// 00h
//	//me->blk._FEh_SessionReg			.val.raw[11]	= 0b00000000;	// 00h
//	//me->blk._FEh_SessionReg			.val.raw[12]	= 0b00000000;	// 00h
//	//me->blk._FEh_SessionReg			.val.raw[13]	= 0b00000000;	// 00h
//	//me->blk._FEh_SessionReg			.val.raw[14]	= 0b00000000;	// 00h
//	//me->blk._FEh_SessionReg			.val.raw[15]	= 0b00000000;	// 00h
//}