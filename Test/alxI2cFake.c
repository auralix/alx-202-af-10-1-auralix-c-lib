/**
  ******************************************************************************
  * @file		alxI2cFake.c
  * @brief		Auralix C Library - ALX I2C Module - PC Unit Test Fake
  * @copyright	Copyright (C) Auralix d.o.o. All rights reserved.
  *
  * A link-time fake of the I2C master, named after the module it fakes, so a
  * device driver above it can be exercised on a PC. It is a register file: the
  * test puts bytes at an address, the driver reads them; the driver writes
  * bytes, the test reads them back. Nothing is timed, nothing fails unless the
  * test asks it to.
  *
  * Registers are addressed by their 8-bit memory address, which is what every
  * driver in Ext/ uses, and each holds up to ALX_I2C_FAKE_REG_LEN bytes in the
  * order they travel on the wire - most significant first, as I2C devices are
  * almost always specified.
  ******************************************************************************
  **/

//******************************************************************************
// Includes
//******************************************************************************
#include "alxI2c.h"
#include <string.h>


//******************************************************************************
// Private variables
//******************************************************************************
#define ALX_I2C_FAKE_NUM_OF_REGS 256
#define ALX_I2C_FAKE_REG_LEN 8

static uint8_t alxI2cFake_reg[ALX_I2C_FAKE_NUM_OF_REGS][ALX_I2C_FAKE_REG_LEN];
static uint8_t alxI2cFake_lastWrite[ALX_I2C_FAKE_NUM_OF_REGS][ALX_I2C_FAKE_REG_LEN];
static uint32_t alxI2cFake_writeCount[ALX_I2C_FAKE_NUM_OF_REGS];
static uint32_t alxI2cFake_readCount[ALX_I2C_FAKE_NUM_OF_REGS];
static bool alxI2cFake_slaveReady = true;
static int32_t alxI2cFake_forcedStatus = Alx_Ok;


//******************************************************************************
// Prototypes - the DLL export surface (no separate header for test fakes)
//******************************************************************************
void AlxI2cFake_Reset(void);
void AlxI2cFake_SetReg(uint8_t addr, const uint8_t* data, uint8_t len);
void AlxI2cFake_GetLastWrite(uint8_t addr, uint8_t* data, uint8_t len);
uint32_t AlxI2cFake_WriteCount(uint8_t addr);
uint32_t AlxI2cFake_ReadCount(uint8_t addr);
void AlxI2cFake_SetSlaveReady(bool ready);
void AlxI2cFake_SetForcedStatus(int32_t status);


//******************************************************************************
// The fake's own controls
//******************************************************************************
void AlxI2cFake_Reset(void)
{
	memset(alxI2cFake_reg, 0, sizeof(alxI2cFake_reg));
	memset(alxI2cFake_lastWrite, 0, sizeof(alxI2cFake_lastWrite));
	memset(alxI2cFake_writeCount, 0, sizeof(alxI2cFake_writeCount));
	memset(alxI2cFake_readCount, 0, sizeof(alxI2cFake_readCount));
	alxI2cFake_slaveReady = true;
	alxI2cFake_forcedStatus = Alx_Ok;
}
void AlxI2cFake_SetReg(uint8_t addr, const uint8_t* data, uint8_t len)
{
	if (len > ALX_I2C_FAKE_REG_LEN) { len = ALX_I2C_FAKE_REG_LEN; }
	memcpy(alxI2cFake_reg[addr], data, len);
}
void AlxI2cFake_GetLastWrite(uint8_t addr, uint8_t* data, uint8_t len)
{
	if (len > ALX_I2C_FAKE_REG_LEN) { len = ALX_I2C_FAKE_REG_LEN; }
	memcpy(data, alxI2cFake_lastWrite[addr], len);
}
uint32_t AlxI2cFake_WriteCount(uint8_t addr)
{
	return alxI2cFake_writeCount[addr];
}
uint32_t AlxI2cFake_ReadCount(uint8_t addr)
{
	return alxI2cFake_readCount[addr];
}
void AlxI2cFake_SetSlaveReady(bool ready)
{
	alxI2cFake_slaveReady = ready;
}
void AlxI2cFake_SetForcedStatus(int32_t status)
{
	alxI2cFake_forcedStatus = status;
}


//******************************************************************************
// The faked module's own contract
//******************************************************************************
Alx_Status AlxI2c_Init(AlxI2c* me)
{
	me->isInit = true;
	return (Alx_Status)alxI2cFake_forcedStatus;
}
Alx_Status AlxI2c_DeInit(AlxI2c* me)
{
	me->isInit = false;
	return (Alx_Status)alxI2cFake_forcedStatus;
}
Alx_Status AlxI2c_Master_IsSlaveReady(AlxI2c* me, uint16_t slaveAddr, uint8_t numOfTries, uint16_t timeout_ms)
{
	(void)me; (void)slaveAddr; (void)numOfTries; (void)timeout_ms;
	if (alxI2cFake_slaveReady == false) { return Alx_Err; }
	return (Alx_Status)alxI2cFake_forcedStatus;
}
Alx_Status AlxI2c_Master_StartReadMemStop(AlxI2c* me, uint16_t slaveAddr, uint16_t memAddr, AlxI2c_Master_MemAddrLen memAddrLen, uint8_t* data, uint16_t len, uint8_t numOfTries, uint16_t timeout_ms)
{
	(void)me; (void)slaveAddr; (void)memAddrLen; (void)numOfTries; (void)timeout_ms;
	uint8_t addr = (uint8_t)memAddr;
	if (len > ALX_I2C_FAKE_REG_LEN) { len = ALX_I2C_FAKE_REG_LEN; }
	memcpy(data, alxI2cFake_reg[addr], len);
	alxI2cFake_readCount[addr]++;
	return (Alx_Status)alxI2cFake_forcedStatus;
}
Alx_Status AlxI2c_Master_StartWriteMemStop_Multi(AlxI2c* me, uint16_t slaveAddr, uint16_t memAddr, AlxI2c_Master_MemAddrLen memAddrLen, const uint8_t* data, uint16_t len, bool checkWithRead, uint8_t numOfTries, uint16_t timeout_ms)
{
	(void)me; (void)slaveAddr; (void)memAddrLen; (void)checkWithRead; (void)numOfTries; (void)timeout_ms;
	uint8_t addr = (uint8_t)memAddr;
	if (len > ALX_I2C_FAKE_REG_LEN) { len = ALX_I2C_FAKE_REG_LEN; }
	memcpy(alxI2cFake_reg[addr], data, len);
	memcpy(alxI2cFake_lastWrite[addr], data, len);
	alxI2cFake_writeCount[addr]++;
	return (Alx_Status)alxI2cFake_forcedStatus;
}
