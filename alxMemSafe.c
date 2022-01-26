/**
  ******************************************************************************
  * @file alxMemSafe.c
  * @brief Auralix C Library - ALX Memory Safe Module
  * @version $LastChangedRevision: 4270 $
  * @date $LastChangedDate: 2021-03-05 19:02:52 +0100 (Fri, 05 Mar 2021) $
  ******************************************************************************
  */

//******************************************************************************
// Includes
//******************************************************************************
#include "alxMemSafe.h"


//******************************************************************************
// Constructor
//******************************************************************************
void AlxMemSafe_Ctor
(
	AlxMemSafe* me,
	AlxMemRaw* memRaw,
	AlxCrc* crc,
	uint32_t copyAddrA,
	uint32_t copyAddrB,
	uint32_t copyLen,
	bool nonBlockingEnable,
	uint8_t memSafeReadWriteNumOfTries,
	uint8_t memRawReadWriteNumOfTries,
	uint16_t memRawReadWriteTimeout_ms,
	uint8_t* buff1,
	uint32_t buff1Len,
	uint8_t* buff2,
	uint32_t buff2Len
)
{
	// Objects - External
	me->memRaw = memRaw;
	me->crc = crc;

	// Parameters
	me->copyAddrA = copyAddrA;
	me->copyAddrB = copyAddrB;
	me->copyLen = copyLen;
	me->copyCrcLen = AlxCrc_GetLen(crc);
	me->copyLenWithCrc = copyLen + me->copyCrcLen;
	me->nonBlockingEnable = nonBlockingEnable;

	me->memSafeReadWriteNumOfTries = memSafeReadWriteNumOfTries;
	me->memRawReadWriteNumOfTries = memRawReadWriteNumOfTries;
	me->memRawReadWriteTimeout_ms = memRawReadWriteTimeout_ms;

	me->buff1 = buff1;
	me->buff1Len = buff1Len;
	me->buff2 = buff2;
	me->buff2Len = buff2Len;

	// Assert
	// TO DO

	// Info
	me->wasCtorCalled = true;
	me->isInit = false;
}


//******************************************************************************
// Functions
//******************************************************************************
Alx_Status AlxMemSafe_Read(AlxMemSafe* me, uint8_t* data, uint32_t len)
{
	// Assert
	ALX_MEM_SAFE_ASSERT(me->wasCtorCalled == true);
	ALX_MEM_SAFE_ASSERT(len == me->copyLen);

	// #1 Prepare variables
	Alx_Status status = Alx_Err;

	// #2 Non-Blocking
	if (me->nonBlockingEnable)
	{
		// TODO
		ALX_MEM_SAFE_ASSERT(false);	// We should not get here
		return Alx_Err;
	}

	// #3 Blocking
	else
	{
		// #3.1 Prepare variables
		Alx_Status statusMemRaw = Alx_Err;
		me->isReadDone = false;
		me->isReadErr = true;

		// #3.2 Read & Validate CopyA & CopyB
		for (uint32_t i = 0; i < me->memSafeReadWriteNumOfTries; i++)
		{
			// #3.2.1 Read CopyA

			// Init
			statusMemRaw = AlxMemRaw_Init(me->memRaw);
			if(statusMemRaw != Alx_Ok) { ALX_MEM_SAFE_TRACE("ErrInit"); continue; }

			// Read
			statusMemRaw = AlxMemRaw_Read(me->memRaw, me->copyAddrA, me->buff1, me->copyLenWithCrc, me->memRawReadWriteNumOfTries, me->memRawReadWriteTimeout_ms);
			if(statusMemRaw != Alx_Ok) { ALX_MEM_SAFE_TRACE("ErrReadCopyA"); continue; }

			// DeInit
			statusMemRaw = AlxMemRaw_DeInit(me->memRaw);
			if(statusMemRaw != Alx_Ok) { ALX_MEM_SAFE_TRACE("ErrDeInit"); continue; }

			// #3.2.2 Check CopyA CRC
			me->isCopyAValid = AlxCrc_IsOk(me->crc, me->buff1, me->copyLenWithCrc, &me->crcCopyA);

			// #3.2.3 Read CopyB

			// Init
			statusMemRaw = AlxMemRaw_Init(me->memRaw);
			if(statusMemRaw != Alx_Ok) { ALX_MEM_SAFE_TRACE("ErrInit"); continue; }

			// Read
			statusMemRaw = AlxMemRaw_Read(me->memRaw, me->copyAddrB, me->buff2, me->copyLenWithCrc, me->memRawReadWriteNumOfTries, me->memRawReadWriteTimeout_ms);
			if(statusMemRaw != Alx_Ok) { ALX_MEM_SAFE_TRACE("ErrReadCopyB"); continue; }

			// DeInit
			statusMemRaw = AlxMemRaw_DeInit(me->memRaw);
			if(statusMemRaw != Alx_Ok) { ALX_MEM_SAFE_TRACE("ErrDeInit"); continue; }

			// #3.2.4 Check CopyB CRC
			me->isCopyBValid = AlxCrc_IsOk(me->crc, me->buff2, me->copyLenWithCrc, &me->crcCopyB);

			// #3.2.5 Handle CRC Validity
			{
				// #3.2.5.1 Both CRC NOK
				if((me->isCopyAValid == false) && (me->isCopyBValid == false))
				{
					// Break
					status = AlxMemSafe_BothNok_ResetToDef;
					break;
				}
				// #3.2.5.2 Both CRC OK & Same -> Use CopyA
				else if ((me->isCopyAValid == true) && (me->isCopyBValid == true) && (me->crcCopyA == me->crcCopyB))
				{
					// Use CopyA
					memcpy(data, me->buff1, len);

					// Break
					status = AlxMemSafe_CrcOkSame_UsedCopyA;
					break;
				}
				// #3.2.5.3 Both CRC OK & Different -> Use CopyA (CopyA is used because we always write CopyA first)
				else if ((me->isCopyAValid == true) && (me->isCopyBValid == true) && (me->crcCopyA != me->crcCopyB))
				{
					// Update CopyB with CopyA

					// Init
					statusMemRaw = AlxMemRaw_Init(me->memRaw);
					if(statusMemRaw != Alx_Ok) { ALX_MEM_SAFE_TRACE("ErrInit"); continue; }

					// Write
					statusMemRaw = AlxMemRaw_Write(me->memRaw, me->copyAddrB, me->buff1, me->copyLenWithCrc, true, me->memRawReadWriteNumOfTries, me->memRawReadWriteTimeout_ms);
					if(statusMemRaw != Alx_Ok) { ALX_MEM_SAFE_TRACE("CrcOkDiff_UsedCopyA_WriteErr"); continue; }

					// DeInit
					statusMemRaw = AlxMemRaw_DeInit(me->memRaw);
					if(statusMemRaw != Alx_Ok) { ALX_MEM_SAFE_TRACE("ErrDeInit"); continue; }

					// Use CopyA
					memcpy(data, me->buff1, len);

					// Break
					status = AlxMemSafe_CrcOkDiff_UsedCopyA;
					break;
				}
				// #3.2.5.4 CopyA OK, CopyB NOK
				else if ((me->isCopyAValid == true) && (me->isCopyBValid == false))
				{
					// Update CopyB with CopyA

					// Init
					statusMemRaw = AlxMemRaw_Init(me->memRaw);
					if(statusMemRaw != Alx_Ok) { ALX_MEM_SAFE_TRACE("ErrInit"); continue; }

					// Write
					statusMemRaw = AlxMemRaw_Write(me->memRaw, me->copyAddrB, me->buff1, me->copyLenWithCrc, true, me->memRawReadWriteNumOfTries, me->memRawReadWriteTimeout_ms);
					if(statusMemRaw != Alx_Ok) { ALX_MEM_SAFE_TRACE("CopyAOkCopyBNok_UsedCopyA_WriteErr"); continue; }

					// DeInit
					statusMemRaw = AlxMemRaw_DeInit(me->memRaw);
					if(statusMemRaw != Alx_Ok) { ALX_MEM_SAFE_TRACE("ErrDeInit"); continue; }

					// Use CopyA
					memcpy(data, me->buff1, len);

					// Break
					status = AlxMemSafe_CopyAOkCopyBNok_UsedCopyA;
					break;
				}
				// #3.2.5.5 CopyA NOK & CopyB OK
				else if ((me->isCopyAValid == false) && (me->isCopyBValid == true))
				{
					// Update CopyA with CopyB

					// Init
					statusMemRaw = AlxMemRaw_Init(me->memRaw);
					if(statusMemRaw != Alx_Ok) { ALX_MEM_SAFE_TRACE("ErrInit"); continue; }

					// Write
					statusMemRaw = AlxMemRaw_Write(me->memRaw, me->copyAddrA, me->buff2, me->copyLenWithCrc, true, me->memRawReadWriteNumOfTries, me->memRawReadWriteTimeout_ms);
					if(statusMemRaw != Alx_Ok) { ALX_MEM_SAFE_TRACE("CopyANokCopyBOk_UsedCopyB_WriteErr"); continue; }

					// DeInit
					statusMemRaw = AlxMemRaw_DeInit(me->memRaw);
					if(statusMemRaw != Alx_Ok) { ALX_MEM_SAFE_TRACE("ErrDeInit"); continue; }

					// Use CopyB
					memcpy(data, me->buff2, len);

					// Break
					status = AlxMemSafe_CopyANokCopyBOk_UsedCopyB;
					break;
				}
				// #3.2.5.6 Assert
				else
				{
					ALX_MEM_SAFE_ASSERT(false);	// We should not get here
					return Alx_Err;
				}
			}
		}

		// #3.3 Set status variables
		me->isReadDone = true;
		me->isReadErr = false;
	}

	// #4 Return
	return status;
}
Alx_Status AlxMemSafe_Write(AlxMemSafe* me, uint8_t* data, uint32_t len)
{
	// Assert
	ALX_MEM_SAFE_ASSERT(me->wasCtorCalled == true);
	ALX_MEM_SAFE_ASSERT(len == me->copyLen);

	// #1 Prepare variables
	Alx_Status status = Alx_Err;

	// #2 Non-Blocking
	if (me->nonBlockingEnable)
	{
		// TODO
		ALX_MEM_SAFE_ASSERT(false);	// We should not get here
		return Alx_Err;
	}

	// #3 Blocking
	else
	{
		// #3.1 Prepare variables
		Alx_Status statusMemRaw = Alx_Err;
		me->isWriteDone = false;
		me->isWriteErr = true;

		// #3.2 Prepare data to write
		me->crcToWrite = AlxCrc_Calc(me->crc, data, len);
		memcpy(me->buff1, data, len);
		memcpy(&me->buff1[len], &me->crcToWrite, me->copyCrcLen);

		// #3.3 Write CopyA & CopyB
		for (uint32_t i = 0; i < me->memSafeReadWriteNumOfTries; i++)
		{
			// #3.3.1 Write CopyA

			// Init
			statusMemRaw = AlxMemRaw_Init(me->memRaw);
			if(statusMemRaw != Alx_Ok) { ALX_MEM_SAFE_TRACE("ErrInit"); continue; }

			// Write
			statusMemRaw = AlxMemRaw_Write(me->memRaw, me->copyAddrA, me->buff1, me->copyLenWithCrc, true, me->memRawReadWriteNumOfTries, me->memRawReadWriteTimeout_ms);
			if(statusMemRaw != Alx_Ok) { ALX_MEM_SAFE_TRACE("ErrWriteCopyA"); continue; }

			// DeInit
			statusMemRaw = AlxMemRaw_DeInit(me->memRaw);
			if(statusMemRaw != Alx_Ok) { ALX_MEM_SAFE_TRACE("ErrDeInit"); continue; }

			// #3.3.2 Write CopyB (use buff1)

			// Init
			statusMemRaw = AlxMemRaw_Init(me->memRaw);
			if(statusMemRaw != Alx_Ok) { ALX_MEM_SAFE_TRACE("ErrInit"); continue; }

			// Write
			statusMemRaw = AlxMemRaw_Write(me->memRaw, me->copyAddrB, me->buff1, me->copyLenWithCrc, true, me->memRawReadWriteNumOfTries, me->memRawReadWriteTimeout_ms);
			if(statusMemRaw != Alx_Ok) { ALX_MEM_SAFE_TRACE("ErrWriteCopyB"); continue; }

			// DeInit
			statusMemRaw = AlxMemRaw_DeInit(me->memRaw);
			if(statusMemRaw != Alx_Ok) { ALX_MEM_SAFE_TRACE("ErrDeInit"); continue; }

			// #3.3.3 Break
			status = Alx_Ok;
			me->isWriteDone = true;
			me->isWriteErr = false;
			break;
		}
	}

	// #4 Return
	return status;
}
bool AlxMemSafe_IsReadDone(AlxMemSafe* me)
{
	// Assert
	ALX_MEM_SAFE_ASSERT(me->wasCtorCalled == true);

	// #1 Return
	return me->isReadDone;
}
bool AlxMemSafe_IsReadErr(AlxMemSafe* me)
{
	// Assert
	ALX_MEM_SAFE_ASSERT(me->wasCtorCalled == true);

	// #1 Return
	return me->isReadErr;
}
bool AlxMemSafe_IsWriteDone(AlxMemSafe* me)
{
	// Assert
	ALX_MEM_SAFE_ASSERT(me->wasCtorCalled == true);

	// #1 Return
	return me->isWriteDone;
}
bool AlxMemSafe_IsWriteErr(AlxMemSafe* me)
{
	// Assert
	ALX_MEM_SAFE_ASSERT(me->wasCtorCalled == true);

	// #1 Return
	return me->isWriteErr;
}
