/**
  ******************************************************************************
  * @file alxCrc.h
  * @brief Auralix C Library - ALX CRC Module
  * @version $LastChangedRevision: 4937 $
  * @date $LastChangedDate: 2021-05-02 22:05:40 +0200 (Sun, 02 May 2021) $
  ******************************************************************************
  */

#ifndef ALX_CRC_H
#define ALX_CRC_H

#ifdef __cplusplus
extern "C" {
#endif

//******************************************************************************
// Includes
//******************************************************************************
#include "alxGlobal.h"
#include "alxAssert.h"


//******************************************************************************
// Preprocessor
//******************************************************************************
#define ALX_CRC_FILE "alxCrc.h"

// Assert //
#if defined(_ALX_CRC_ASSERT_BKPT) || defined(_ALX_ASSERT_BKPT_ALL)
	#define ALX_CRC_ASSERT(expr) ALX_ASSERT_BKPT(ALX_CRC_FILE, expr)
#elif defined(_ALX_CRC_ASSERT_TRACE) || defined(_ALX_ASSERT_TRACE_ALL)
	#define ALX_CRC_ASSERT(expr) ALX_ASSERT_TRACE(ALX_CRC_FILE, expr)
#elif defined(_ALX_CRC_ASSERT_RST) || defined(_ALX_ASSERT_RST_ALL)
	#define ALX_CRC_ASSERT(expr) ALX_ASSERT_RST(ALX_CRC_FILE, expr)
#else
	#define ALX_CRC_ASSERT(expr) do{} while (false)
#endif

// Trace //
#if defined(_ALX_CRC_TRACE) || defined(_ALX_TRACE_ALL)
	#define ALX_CRC_TRACE(...) ALX_TRACE_STD(ALX_CRC_FILE, __VA_ARGS__)
#else
	#define ALX_CRC_TRACE(...) do{} while (false)
#endif

// DbgPin //
#if defined(_ALX_CRC_DBG_PIN) || defined(_ALX_DBG_PIN_ALL)
	#define ALX_CRC_DBG_PIN(...) ALX_DBG_PIN_TOGGLE()
#else
	#define ALX_CRC_DBG_PIN(...) do{} while (false)
#endif


//******************************************************************************
// Types
//******************************************************************************
typedef enum
{
	AlxCrc_Config_Ccitt = 0, // Crc8
	AlxCrc_Config_Crc16 = 1,
	AlxCrc_Config_Crc32 = 2
} AlxCrc_Config;

typedef struct
{
	// Objects - External

	// Parameters
	AlxCrc_Config config;

	// Variables
	uint32_t len;

	// Info
	bool wasCtorCalled;
	bool isInit;
} AlxCrc;


//******************************************************************************
// Constructor
//******************************************************************************
void AlxCrc_Ctor
(
	AlxCrc* me,
	AlxCrc_Config config
);


//******************************************************************************
// Functions
//******************************************************************************
uint32_t AlxCrc_Calc(AlxCrc* me, uint8_t* data, uint32_t len);
bool AlxCrc_IsOk(AlxCrc* me, uint8_t* dataWithCrc, uint32_t lenWithCrc, uint32_t* validatedCrc);
uint32_t AlxCrc_GetLen(AlxCrc* me);

#ifdef __cplusplus
}
#endif

#endif // ALX_CRC_H
