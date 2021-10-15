/**
  ******************************************************************************
  * @file alxMemSafe.h
  * @brief Auralix C Library - ALX Memory Module
  * @version $LastChangedRevision: 4937 $
  * @date $LastChangedDate: 2021-05-02 22:05:40 +0200 (Sun, 02 May 2021) $
  ******************************************************************************
  */

#ifndef ALX_MEM_SAFE_H
#define ALX_MEM_SAFE_H

#ifdef __cplusplus
extern "C" {
#endif

//******************************************************************************
// Includes
//******************************************************************************
#include "alxGlobal.h"
#include "alxAssert.h"
#include "alxMemRaw.h"
#include "alxCrc.h"


//******************************************************************************
// Preprocessor
//******************************************************************************
#define ALX_MEM_SAFE_FILE "alxMemSafe.h"

// Assert //
#if defined(_ALX_MEM_SAFE_ASSERT_BKPT) || defined(_ALX_ASSERT_BKPT_ALL)
	#define ALX_MEM_SAFE_ASSERT(expr) ALX_ASSERT_BKPT(ALX_MEM_SAFE_FILE, expr)
#elif defined(_ALX_MEM_SAFE_ASSERT_TRACE) || defined(_ALX_ASSERT_TRACE_ALL)
	#define ALX_MEM_SAFE_ASSERT(expr) ALX_ASSERT_TRACE(ALX_MEM_SAFE_FILE, expr)
#elif defined(_ALX_MEM_SAFE_ASSERT_RST) || defined(_ALX_ASSERT_RST_ALL)
	#define ALX_MEM_SAFE_ASSERT(expr) ALX_ASSERT_RST(ALX_MEM_SAFE_FILE, expr)
#else
	#define ALX_MEM_SAFE_ASSERT(expr) do{} while (false)
#endif

// Trace //
#if defined(_ALX_MEM_SAFE_TRACE) || defined(_ALX_TRACE_ALL)
	#define ALX_MEM_SAFE_TRACE(...) ALX_TRACE_STD(ALX_MEM_SAFE_FILE, __VA_ARGS__)
#else
	#define ALX_MEM_SAFE_TRACE(...) do{} while (false)
#endif

// DbgPin //
#if defined(_ALX_MEM_SAFE_DBG_PIN) || defined(_ALX_DBG_PIN_ALL)
	#define ALX_MEM_SAFE_DBG_PIN(...) ALX_DBG_PIN_TOGGLE()
#else
	#define ALX_MEM_SAFE_DBG_PIN(...) do{} while (false)
#endif


//******************************************************************************
// Types
//******************************************************************************
typedef struct
{
	// Objects - External
	AlxMemRaw* memRaw;
	AlxCrc* crc;

	// Parameters
	uint32_t copyAddrA;
	uint32_t copyAddrB;
	uint32_t copyLen;
	uint32_t copyCrcLen;
	uint32_t copyLenWithCrc;

	bool nonBlockingEnable;
	
	uint8_t memSafeReadWriteNumOfTries;
	uint8_t memRawReadWriteNumOfTries;
	uint16_t memRawReadWriteTimeout_ms;

	uint8_t* buff1;
	uint32_t buff1Len;
	uint8_t* buff2;
	uint32_t buff2Len;

	// Variables
	uint32_t crcCopyA;
	uint32_t crcCopyB;
	bool isCopyAValid;
	bool isCopyBValid;
	uint32_t crcToWrite;
	bool isReadDone;
	bool isReadErr;
	bool isWriteDone;
	bool isWriteErr;

	// Info
	bool wasCtorCalled;
	bool isInit;
} AlxMemSafe;


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
);


//******************************************************************************
// Functions
//******************************************************************************
Alx_Status AlxMemSafe_Read(AlxMemSafe* me, uint8_t* data, uint32_t len);
Alx_Status AlxMemSafe_Write(AlxMemSafe* me, uint8_t* data, uint32_t len);
bool AlxMemSafe_IsReadDone(AlxMemSafe* me);
bool AlxMemSafe_IsReadErr(AlxMemSafe* me);
bool AlxMemSafe_IsWriteDone(AlxMemSafe* me);
bool AlxMemSafe_IsWriteErr(AlxMemSafe* me);

#ifdef __cplusplus
}
#endif

#endif // ALX_MEM_SAFE_H
