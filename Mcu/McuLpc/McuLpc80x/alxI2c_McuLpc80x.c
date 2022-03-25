/**
  ******************************************************************************
  * @file alxI2c_McuLpc80x.c
  * @brief Auralix C Library - ALX I2C Module
  * @version $LastChangedRevision: 4270 $
  * @date $LastChangedDate: 2021-03-05 19:02:52 +0100 (Fri, 05 Mar 2021) $
  ******************************************************************************
  */

//******************************************************************************
// Includes
//******************************************************************************
#include "alxI2c_McuLpc80x.h"
#include "alxI2c.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_LPC80X)


//******************************************************************************
// Private Functions
//******************************************************************************
static uint8_t AlxI2c_GetMemAddrLen(AlxI2c_Master_MemAddrLen* memAddrLen);
static Alx_Status AlxI2c_Reset(AlxI2c* me);
static void AlxI2c_Periph_SelectClk(AlxI2c* me);

// MF: These functions are copied from FSL so that Timeout can be added
static status_t AlxI2c_MasterStart(AlxI2c* me, I2C_Type* base, uint8_t address, i2c_direction_t direction, uint16_t timeout);
static status_t AlxI2c_MasterWriteBlocking(AlxI2c* me, I2C_Type* base, const void* txBuff, size_t txSize, uint32_t flags, uint16_t timeout);
static status_t AlxI2c_MasterReadBlocking(AlxI2c* me, I2C_Type* base, void* rxBuff, size_t rxSize, uint32_t flags, uint16_t timeout);
static uint32_t AlxI2c_PendingStatusWait(AlxI2c* me, I2C_Type* base, uint16_t timeout);


//******************************************************************************
// Constructor
//******************************************************************************
void AlxI2c_Ctor
(
	AlxI2c* me,
	I2C_Type* i2c,
	AlxIoPin* io_SCL,
	AlxIoPin* io_SDA,
	AlxI2c_Clk clk
)
{
	// Assert
	(void)me;
	(void)i2c;
	(void)io_SCL;
	(void)io_SDA;
	(void)clk;

	// Objects - External
	me->io_SCL = io_SCL;
	me->io_SDA = io_SDA;

	// Parameters
	me->i2c = i2c;
	me->clk = clk;

	// Variables
	me->i2cConfig.enableMaster = true;
	me->i2cConfig.baudRate_Bps = me->clk;	// MF: baudRate = bit rate
	me->i2cConfig.enableTimeout = false;

	AlxTimSw_Ctor(&me->tim, false);

	// Info
	me->isInit = false;
	me->wasCtorCalled = true;
}


//******************************************************************************
// Specific Functions
//******************************************************************************
Alx_Status AlxI2c_Init(AlxI2c* me)
{
	// Assert
	ALX_I2C_ASSERT(me->isInit == false);
	ALX_I2C_ASSERT(me->wasCtorCalled == true);

	// #1 Init GPIO
	AlxIoPin_Init(me->io_SCL);
	AlxIoPin_Init(me->io_SDA);

	// #2 Select I2C Periphery Clock
	AlxI2c_Periph_SelectClk(me);

	// #3 Init I2C
	I2C_MasterInit(me->i2c, &me->i2cConfig, CLOCK_GetMainClkFreq());	// MF: "Periph_Reset" and "EnableClk" happens here. MF: srcClock_Hz = I2cFuncClk = MainClk

	// #4 Set isInit
	me->isInit = true;

	// #5 Return OK
	return Alx_Ok;
}
Alx_Status AlxI2c_DeInit(AlxI2c* me)
{
	// Assert
	ALX_I2C_ASSERT(me->isInit == true);
	ALX_I2C_ASSERT(me->wasCtorCalled == true);

	// #1 DeInit I2C
	I2C_MasterDeinit(me->i2c);	// MF: "DisableClk" happens here

	// #2 DeInit GPIO
	AlxIoPin_DeInit(me->io_SCL);
	AlxIoPin_DeInit(me->io_SDA);

	// #3 Reset isInit
	me->isInit = false;

	// #4 Return OK
	return Alx_Ok;
}
Alx_Status AlxI2c_Master_StartRead(AlxI2c* me, uint16_t slaveAddr, uint8_t* data, uint16_t len, uint16_t timeout_ms)
{
	// TODO
	ALX_I2C_ASSERT(false);
	return Alx_Err;
}
Alx_Status AlxI2c_Master_StartReadStop(AlxI2c* me, uint16_t slaveAddr, uint8_t* data, uint16_t len, uint8_t numOfTries, uint16_t timeout_ms)
{
	// TODO
	ALX_I2C_ASSERT(false);
	return Alx_Err;
}
Alx_Status AlxI2c_Master_StartReadMemStop(AlxI2c* me, uint16_t slaveAddr, uint16_t memAddr, AlxI2c_Master_MemAddrLen memAddrLen, uint8_t* data, uint16_t len, uint8_t numOfTries, uint16_t timeout_ms)
{
	// Assert
	ALX_I2C_ASSERT(me->isInit == true);
	ALX_I2C_ASSERT(me->wasCtorCalled == true);
	(void)me;
	(void)slaveAddr;
	(void)memAddr;
	(void)memAddrLen;
	(void)data;
	ALX_I2C_ASSERT(0 < len);
	ALX_I2C_ASSERT(0 < numOfTries);
	ALX_I2C_ASSERT(0 < timeout_ms);

	// #1 Prepare variables
	status_t status = kStatus_Fail;
	uint8_t _memAddr[2] = { (memAddr >> 8) & 0xFF, memAddr & 0xFF};		// MF: Ensures that memAddr bytes are in right order
	if(memAddrLen == AlxI2c_Master_MemAddrLen_8bit) { _memAddr[0] =	memAddr & 0xFF;	}	// JS: Ensures that memAddr for 1 byte is correct
	uint8_t _memAddrLen = AlxI2c_GetMemAddrLen(&memAddrLen);

	// #2 Start Timeout
	AlxTimSw_Start(&me->tim);

	// #3 Try read memory for number of tries
	for (uint32_t _tryNo = 1; _tryNo <= numOfTries; _tryNo++)
	{
		// #3.1 Send Start Condition
		status = AlxI2c_MasterStart(me, me->i2c, slaveAddr, kI2C_Write, timeout_ms);
		if (status != kStatus_Success)
		{
			ALX_I2C_TRACE("ErrStartCondition");
			if (AlxI2c_Reset(me) != Alx_Ok) { ALX_I2C_TRACE("ErrReInit"); return Alx_ErrReInit; }
			continue;
		}

		// #3.2 Write Slave Address
		status = AlxI2c_MasterWriteBlocking(me, me->i2c, _memAddr, _memAddrLen, kI2C_TransferDefaultFlag, timeout_ms);	// MF: Write memAddr, then Stop
		if (status != kStatus_Success)
		{
			ALX_I2C_TRACE("ErrWriteSlaveAddr");
			if (AlxI2c_Reset(me) != Alx_Ok) { ALX_I2C_TRACE("ErrReInit"); return Alx_ErrReInit; }
			continue;
		}

		// #3.3 Send Start Condition
		status = AlxI2c_MasterStart(me, me->i2c, slaveAddr, kI2C_Read, timeout_ms);
		if (status != kStatus_Success)
		{
			ALX_I2C_TRACE("ErrStartCondition");
			if (AlxI2c_Reset(me) != Alx_Ok) { ALX_I2C_TRACE("ErrReInit"); return Alx_ErrReInit; }
			continue;
		}

		// #3.4 Read Memory
		status = AlxI2c_MasterReadBlocking(me, me->i2c, data, len, kI2C_TransferDefaultFlag, timeout_ms);	// MF: Read data, then Stop
		if (status == kStatus_Success)
		{
			return Alx_Ok;	// MF: Memory read OK
		}
		else
		{
			ALX_I2C_TRACE("ErrFlsRead");
			if (AlxI2c_Reset(me) != Alx_Ok) { ALX_I2C_TRACE("ErrReInit"); return Alx_ErrReInit; }
			continue;
		}
	}

	// #4 If we are here, the number of tries error occured
	if (status != kStatus_Success)
	{
		ALX_I2C_TRACE("ErrNumOfTries");
		return Alx_ErrNumOfTries;
	}
}
Alx_Status AlxI2c_Master_StartWrite(AlxI2c* me, uint16_t slaveAddr, const uint8_t* data, uint16_t len, uint16_t timeout_ms)
{
	// TODO
	ALX_I2C_ASSERT(false);
	return Alx_Err;
}
Alx_Status AlxI2c_Master_StartWriteStop(AlxI2c* me, uint16_t slaveAddr, const uint8_t* data, uint16_t len, uint8_t numOfTries, uint16_t timeout_ms)
{
	// TODO
	ALX_I2C_ASSERT(false);
	return Alx_Err;
}
Alx_Status AlxI2c_Master_StartWriteMemStop_Single(AlxI2c* me, uint16_t slaveAddr, uint16_t memAddr, AlxI2c_Master_MemAddrLen memAddrLen, uint8_t data, bool checkWithRead, uint8_t numOfTries, uint16_t timeout_ms)
{
	return AlxI2c_Master_StartWriteMemStop_Multi(me, slaveAddr, memAddr, memAddrLen, &data, 1, checkWithRead, numOfTries, timeout_ms);
}
Alx_Status AlxI2c_Master_StartWriteMemStop_Multi(AlxI2c* me, uint16_t slaveAddr, uint16_t memAddr, AlxI2c_Master_MemAddrLen memAddrLen, const uint8_t* data, uint16_t len, bool checkWithRead, uint8_t numOfTries, uint16_t timeout_ms)
{
	// Assert
	ALX_I2C_ASSERT(me->isInit == true);
	ALX_I2C_ASSERT(me->wasCtorCalled == true);
	(void)me;
	(void)slaveAddr;
	(void)memAddr;
	(void)memAddrLen;
	(void)data;
	if (checkWithRead)	{ ALX_I2C_ASSERT((0 < len) && (len <= ALX_I2C_BUFF_LEN)); }
	else				{ ALX_I2C_ASSERT(0 < len); }
	(void)checkWithRead;
	ALX_I2C_ASSERT(0 < numOfTries);
	ALX_I2C_ASSERT(0 < timeout_ms);

	// #1 Prepare variables
	status_t status = kStatus_Fail;
	uint8_t _memAddr[2] = { (memAddr >> 8) & 0xFF, memAddr & 0xFF };	// MF: Ensures that memAddr bytes are in right order
	if (memAddrLen == AlxI2c_Master_MemAddrLen_8bit) { _memAddr[0] = memAddr & 0xFF; }	// JS: Ensures that memAddr for 1 byte is correct
	uint8_t _memAddrLen = AlxI2c_GetMemAddrLen(&memAddrLen);
	uint8_t buff[ALX_I2C_BUFF_LEN] = { 0 };

	// #2 Start Timeout
	AlxTimSw_Start(&me->tim);

	// #3 Try write memory for number of tries
	for (uint32_t _tryNo = 1; _tryNo <= numOfTries; _tryNo++)
	{
		// #3.1 Send Start Condition
		status = AlxI2c_MasterStart(me, me->i2c, slaveAddr, kI2C_Write, timeout_ms);
		if (status != kStatus_Success)
		{
			ALX_I2C_TRACE("ErrStartCondition");
			if (AlxI2c_Reset(me) != Alx_Ok) { ALX_I2C_TRACE("ErrReInit"); return Alx_ErrReInit; }
			continue;
		}

		// #3.2 Write Slave Address
		status = AlxI2c_MasterWriteBlocking(me, me->i2c, _memAddr, _memAddrLen, kI2C_TransferNoStopFlag, timeout_ms);	// MF: Write memAddr, no Stop
		if (status != kStatus_Success)
		{
			ALX_I2C_TRACE("ErrWriteSlaveAddr");
			if (AlxI2c_Reset(me) != Alx_Ok) { ALX_I2C_TRACE("ErrReInit"); return Alx_ErrReInit; }
			continue;
		}

		// #3.3 Write Memory
		status = AlxI2c_MasterWriteBlocking(me, me->i2c, data, len, kI2C_TransferNoStartFlag, timeout_ms);	// MF: Write data then Stop
		if (status == kStatus_Success)
		{
			// MF: If enabled, read & check previously written data
			if (checkWithRead)
			{
				Alx_Status status = Alx_Err;
				status = AlxI2c_Master_StartReadMemStop(me, slaveAddr, memAddr, memAddrLen, buff, len, numOfTries, timeout_ms);
				if (status == Alx_ErrNumOfTries)
				{
					ALX_I2C_TRACE("Read_ErrNumOfTries");
					continue;
				}
				else if (status != Alx_Ok)
				{
					ALX_I2C_ASSERT(false); // We should never get here
					return Alx_Err;
				}
				else if (memcmp(buff, data, len) != 0)	// MF: Check previously written data
				{
					ALX_I2C_TRACE("Read_ErrCheckWithRead");
					continue;
				}
				else
				{
					return Alx_Ok;	// MF: Memory write OK, with check with read
				}
			}
			else
			{
				return Alx_Ok;		// MF: Memory write OK, without check with read
			}
		}
		else
		{
			ALX_I2C_TRACE("ErrFlsWrite");
			if (AlxI2c_Reset(me) != Alx_Ok) { ALX_I2C_TRACE("ErrReInit"); return Alx_ErrReInit; }
			continue;
		}
	}

	// #3 If we are here, write was OK or number of tries error occured
	if (status != kStatus_Success)
	{
		ALX_I2C_TRACE("ErrNumOfTries");
		return Alx_ErrNumOfTries;
	}
}
Alx_Status AlxI2c_Master_Stop(AlxI2c* me, uint16_t timeout_ms)
{
	// TODO
	ALX_I2C_ASSERT(false);
	return Alx_Err;
}
Alx_Status AlxI2c_Master_IsSlaveReady(AlxI2c* me, uint16_t slaveAddr, uint8_t numOfTries, uint16_t timeout_ms)
{
	// Assert
	ALX_I2C_ASSERT(me->isInit == true);
	ALX_I2C_ASSERT(me->wasCtorCalled == true);
	(void)me;
	(void)slaveAddr;
	ALX_I2C_ASSERT(0 < numOfTries);
	ALX_I2C_ASSERT(0 < timeout_ms);

	//TODO
	ALX_I2C_ASSERT(false);

	// #1 Try slave
	/*HAL_StatusTypeDef status = HAL_I2C_IsDeviceReady(&me->hi2c, slaveAddr, numOfTries, timeout_ms);
	if (status != HAL_OK)
	{
		ALX_I2C_TRACE("ErrHal");
		if (AlxI2c_Reset(me) != Alx_Ok) { ALX_I2C_TRACE("ErrReset"); return Alx_Err; }
	}*/

	return Alx_Err;
}


//******************************************************************************
// Private Functions
//******************************************************************************
static uint8_t AlxI2c_GetMemAddrLen(AlxI2c_Master_MemAddrLen* memAddrLen)
{
	if		(*memAddrLen == AlxI2c_Master_MemAddrLen_8bit)		{ return 1; }
	else if (*memAddrLen == AlxI2c_Master_MemAddrLen_16bit)		{ return 2; }
	else														{ ALX_I2C_ASSERT(false); return 0; }	// MF: We shouldn't get here
}
static Alx_Status AlxI2c_Reset(AlxI2c* me)
{
	// #1 DeInit I2C
	// #1.1 DeInit I2C
	I2C_MasterDeinit(me->i2c); // MF: "DisableClk" happens here

	// #1.2 DeInit GPIO
	AlxIoPin_DeInit(me->io_SCL);
	AlxIoPin_DeInit(me->io_SDA);

	// #2 Reset isInit
	me->isInit = false;

	// #3 Init I2C
	// #3.1 Init GPIO
	AlxIoPin_Init(me->io_SCL);
	AlxIoPin_Init(me->io_SDA);

	// #3.2 Select I2C Periphery Clock
	AlxI2c_Periph_SelectClk(me);

	// #3.3 Init I2C
	I2C_MasterInit(me->i2c, &me->i2cConfig, CLOCK_GetMainClkFreq()); // MF: "Periph_Reset" and "EnableClk" happens here. MF: srcClock_Hz = I2cFuncClk = MainClk

	// #4 Set isInit
	me->isInit = true;

	// #5 Return OK
	return Alx_Ok;
}
static void AlxI2c_Periph_SelectClk(AlxI2c* me)
{
	bool isErr = true;

	#if defined(I2C0)
	if (me->i2c == I2C0)	{ CLOCK_Select(kI2C0_Clk_From_MainClk); isErr = false; }
	#endif
	#if defined(I2C1)
	if (me->i2c == I2C1)	{ CLOCK_Select(kI2C1_Clk_From_MainClk); isErr = false; }
	#endif
	#if defined(I2C2)
	if (me->i2c == I2C2)	{ CLOCK_Select(kI2C2_Clk_From_MainClk); isErr = false; }
	#endif
	#if defined(I2C3)
	if (me->i2c == I2C3)	{ CLOCK_Select(kI2C3_Clk_From_MainClk); isErr = false; }
	#endif

	if(isErr)				{ ALX_I2C_ASSERT(false); } // We shouldn't get here
}
static status_t AlxI2c_MasterStart(AlxI2c* me, I2C_Type* base, uint8_t address, i2c_direction_t direction, uint16_t timeout)
{
	if (AlxI2c_PendingStatusWait(me, base, timeout) == (uint32_t)kStatus_I2C_Timeout)
	{
		return kStatus_I2C_Timeout;
	}

	/* Write Address and RW bit to data register */
	//base->MSTDAT = (uint32_t)((uint32_t)address << 1) | ((uint32_t)direction & 1u); // JS: commented, it create wrong adress for pca9431 and crn120
	base->MSTDAT = (uint32_t)address | ((uint32_t)direction & 1u); // JS: modify for Pca and Crn
	/* Start the transfer */
	base->MSTCTL = I2C_MSTCTL_MSTSTART_MASK;

	return kStatus_Success;
}
static status_t AlxI2c_MasterWriteBlocking(AlxI2c* me, I2C_Type* base, const void* txBuff, size_t txSize, uint32_t flags, uint16_t timeout)
{
	uint32_t status;
	uint32_t master_state;
	status_t err;

	const uint8_t* buf = (const uint8_t*)txBuff;

	err = kStatus_Success;
	while (txSize != 0U)
	{
		status = AlxI2c_PendingStatusWait(me, base, timeout);

		if (status == kStatus_I2C_Timeout)
		{
			ALX_I2C_TRACE("ErrTimeout");
			return kStatus_I2C_Timeout;
		}

		if ((status & I2C_STAT_MSTARBLOSS_MASK) != 0U) { return kStatus_I2C_ArbitrationLost; }

		if ((status & I2C_STAT_MSTSTSTPERR_MASK) != 0U) { return kStatus_I2C_StartStopError; }

		master_state = (status & I2C_STAT_MSTSTATE_MASK) >> I2C_STAT_MSTSTATE_SHIFT;
		switch (master_state)
		{
		case I2C_STAT_MSTCODE_TXREADY:
			/* ready to send next byte */
			base->MSTDAT = *buf++;
			txSize--;
			base->MSTCTL = I2C_MSTCTL_MSTCONTINUE_MASK;
			break;

		case I2C_STAT_MSTCODE_NACKADR:
			/* slave nacked the address */
			err = kStatus_I2C_Addr_Nak;
			break;

		case I2C_STAT_MSTCODE_NACKDAT:
			/* slave nacked the last byte */
			err = kStatus_I2C_Nak;
			break;

		default:
			/* unexpected state */
			err = kStatus_I2C_UnexpectedState;
			break;
		}

		if (err != kStatus_Success) { return err; }
	}

	status = AlxI2c_PendingStatusWait(me, base, timeout);

	if (status == kStatus_I2C_Timeout)
	{
		ALX_I2C_TRACE("ErrTimeout");
		return kStatus_I2C_Timeout;
	}

	if ((status & (I2C_STAT_MSTARBLOSS_MASK | I2C_STAT_MSTSTSTPERR_MASK)) == 0U)
	{
		if ((flags & (uint32_t)kI2C_TransferNoStopFlag) == 0U)
		{
			/* Initiate stop */
			base->MSTCTL = I2C_MSTCTL_MSTSTOP_MASK;
			status = AlxI2c_PendingStatusWait(me, base, timeout);

			if (status == kStatus_I2C_Timeout)
			{
				ALX_I2C_TRACE("ErrTimeout");
				return kStatus_I2C_Timeout;
			}
		}
	}

	if ((status & I2C_STAT_MSTARBLOSS_MASK) != 0U) { return kStatus_I2C_ArbitrationLost; }

	if ((status & I2C_STAT_MSTSTSTPERR_MASK) != 0U) { return kStatus_I2C_StartStopError; }

	return kStatus_Success;
}
static status_t AlxI2c_MasterReadBlocking(AlxI2c* me, I2C_Type* base, void* rxBuff, size_t rxSize, uint32_t flags, uint16_t timeout)
{
	uint32_t status = 0;
	uint32_t master_state;
	status_t err;

	uint8_t* buf = (uint8_t*)(rxBuff);

	err = kStatus_Success;
	while (rxSize != 0U)
	{
		status = AlxI2c_PendingStatusWait(me, base, timeout);

		if (status == kStatus_I2C_Timeout)
		{
			ALX_I2C_TRACE("ErrTimeout");
			return kStatus_I2C_Timeout;
		}

		if ((status & (I2C_STAT_MSTARBLOSS_MASK | I2C_STAT_MSTSTSTPERR_MASK)) != 0U) { break; }

		master_state = (status & I2C_STAT_MSTSTATE_MASK) >> I2C_STAT_MSTSTATE_SHIFT;
		switch (master_state)
		{
			case I2C_STAT_MSTCODE_RXREADY:
				/* ready to receive next byte */
				*(buf++) = (uint8_t)base->MSTDAT;
				if (--rxSize != 0U)
				{
					base->MSTCTL = I2C_MSTCTL_MSTCONTINUE_MASK;
				}
				else
				{
					if ((flags & (uint32_t)kI2C_TransferNoStopFlag) == 0U)
					{
						/* initiate NAK and stop */
						base->MSTCTL = I2C_MSTCTL_MSTSTOP_MASK;
						status = AlxI2c_PendingStatusWait(me, base, timeout);

						if (status == kStatus_I2C_Timeout)
						{
							ALX_I2C_TRACE("ErrTimeout");
							return kStatus_I2C_Timeout;
						}
					}
				}
				break;

			case I2C_STAT_MSTCODE_NACKADR:
			case I2C_STAT_MSTCODE_NACKDAT:
				/* slave nacked the last byte */
				err = kStatus_I2C_Nak;
				break;

			default:
				/* unexpected state */
				err = kStatus_I2C_UnexpectedState;
				break;
		}

		if (err != kStatus_Success) { return err; }
	}

	if ((status & I2C_STAT_MSTARBLOSS_MASK) != 0U) { return kStatus_I2C_ArbitrationLost; }

	if ((status & I2C_STAT_MSTSTSTPERR_MASK) != 0U) { return kStatus_I2C_StartStopError; }

	return kStatus_Success;
}
static uint32_t AlxI2c_PendingStatusWait(AlxI2c* me, I2C_Type* base, uint16_t timeout)
{
	uint32_t status;

	do
	{
		// MF: Check Timeout
		if (AlxTimSw_IsTimeout_ms(&me->tim, timeout))
		{
			ALX_I2C_TRACE("ErrTimeout");
			return (uint32_t)kStatus_I2C_Timeout;
		}

		status = I2C_GetStatusFlags(base);
	} while ((status & I2C_STAT_MSTPENDING_MASK) == 0U);

	/* Clear controller state. */
	I2C_MasterClearStatusFlags(base, I2C_STAT_MSTARBLOSS_MASK | I2C_STAT_MSTSTSTPERR_MASK);

	return status;
}


#endif // Module Guard
