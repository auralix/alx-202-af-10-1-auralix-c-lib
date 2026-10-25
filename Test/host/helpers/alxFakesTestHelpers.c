/**
  ******************************************************************************
  * @file		alxFakesTestHelpers.c
  * @brief		Auralix C Library - the fakes' own contract - PC Unit Test Helpers
  * @copyright	Copyright (C) Auralix d.o.o. All rights reserved.
  *
  * The fakes group links every fake in host/fakes/ into one DLL, so a test can
  * drive each fake's controls and its faked module's API directly. Most fakes
  * tell objects apart by address only, and Python hands them any distinct
  * buffer. Four touch the object itself - the I2C master's init flag, the ID's
  * hardware fields, the LED driver's requested values, the key-value store's
  * fields - so those objects are allocated here, where their size is known,
  * and read back through the getters below. The status codes a fake answers
  * are read from here too, so no test hard-codes an enum's position.
  ******************************************************************************
  **/

//******************************************************************************
// Includes
//******************************************************************************
#include "alxI2c.h"
#include "alxId.h"
#include "alxLp586x.h"
#include "alxParamKvStore.h"
#include <stdlib.h>
#include <string.h>


//******************************************************************************
// Private Variables
//******************************************************************************
static AlxId_HwInstance alxFakesTest_hwInstanceKnown[2];
static uint8_t alxFakesTest_hwIdSupported[2] = { 1, 2 };


//******************************************************************************
// Prototypes - the DLL export surface (no separate header for test helpers)
//******************************************************************************
AlxI2c* AlxFakesTest_NewI2c(void);
AlxId* AlxFakesTest_NewId(void);
AlxLp586x* AlxFakesTest_NewLp586x(void);
AlxParamKvStore* AlxFakesTest_NewParamKvStore(void);
void AlxFakesTest_Delete(void* me);
bool AlxFakesTest_I2c_IsInit(const AlxI2c* me);
void AlxFakesTest_Id_Ctor(AlxId* me, const char* fwArtf, const char* fwName);
bool AlxFakesTest_Id_HwIsTheConstructorsOwn(const AlxId* me);
bool AlxFakesTest_Lp586x_ValNew(const AlxLp586x* me, uint8_t ledNum);
bool AlxFakesTest_ParamKvStore_WasCtorCalled(const AlxParamKvStore* me);
bool AlxFakesTest_ParamKvStore_IsInit(const AlxParamKvStore* me);
const AlxFs* AlxFakesTest_ParamKvStore_Fs(const AlxParamKvStore* me);
int32_t AlxFakesTest_Status_Ok(void);
int32_t AlxFakesTest_Status_Err(void);
int32_t AlxFakesTest_Status_FifoErrEmpty(void);
int32_t AlxFakesTest_Status_SafeBothCopyErr(void);
int32_t AlxFakesTest_Status_SafeUseCopyA(void);


//******************************************************************************
// Functions
//******************************************************************************
AlxI2c* AlxFakesTest_NewI2c(void)						{ return (AlxI2c*)calloc(1, sizeof(AlxI2c)); }
AlxId* AlxFakesTest_NewId(void)							{ return (AlxId*)calloc(1, sizeof(AlxId)); }
AlxLp586x* AlxFakesTest_NewLp586x(void)					{ return (AlxLp586x*)calloc(1, sizeof(AlxLp586x)); }
AlxParamKvStore* AlxFakesTest_NewParamKvStore(void)		{ return (AlxParamKvStore*)calloc(1, sizeof(AlxParamKvStore)); }
void AlxFakesTest_Delete(void* me)						{ free(me); }

bool AlxFakesTest_I2c_IsInit(const AlxI2c* me)
{
	return me->isInit;
}

void AlxFakesTest_Id_Ctor(AlxId* me, const char* fwArtf, const char* fwName)
{
	// Two known hardware instances with a recognisable pattern, so a constructor that copied the
	// wrong one, or none, is visible.
	memset(&alxFakesTest_hwInstanceKnown[0], 0xA5, sizeof(alxFakesTest_hwInstanceKnown[0]));
	memset(&alxFakesTest_hwInstanceKnown[1], 0x5A, sizeof(alxFakesTest_hwInstanceKnown[1]));
	AlxId_Ctor(me, fwArtf, fwName, 1, 2, 3, false, 0, false, 0, alxFakesTest_hwInstanceKnown, 2,
		alxFakesTest_hwIdSupported, 2, NULL, 0, "McuName");
}

bool AlxFakesTest_Id_HwIsTheConstructorsOwn(const AlxId* me)
{
	return me->hw.instanceKnownArr == alxFakesTest_hwInstanceKnown
		&& me->hw.instanceKnownArrLen == 2
		&& me->hw.instanceHwIdSupportedArr == alxFakesTest_hwIdSupported
		&& me->hw.instanceHwIdSupportedArrLen == 2
		&& me->hw.idIoPinArr == NULL
		&& me->hw.idIoPinArrLen == 0
		&& memcmp(&me->hw.instance, &alxFakesTest_hwInstanceKnown[0], sizeof(me->hw.instance)) == 0;
}

bool AlxFakesTest_Lp586x_ValNew(const AlxLp586x* me, uint8_t ledNum)
{
	return me->valNew[ledNum];
}

bool AlxFakesTest_ParamKvStore_WasCtorCalled(const AlxParamKvStore* me)		{ return me->wasCtorCalled; }
bool AlxFakesTest_ParamKvStore_IsInit(const AlxParamKvStore* me)			{ return me->isInit; }
const AlxFs* AlxFakesTest_ParamKvStore_Fs(const AlxParamKvStore* me)		{ return me->fs; }

int32_t AlxFakesTest_Status_Ok(void)					{ return Alx_Ok; }
int32_t AlxFakesTest_Status_Err(void)					{ return Alx_Err; }
int32_t AlxFakesTest_Status_FifoErrEmpty(void)			{ return AlxFifo_ErrEmpty; }
int32_t AlxFakesTest_Status_SafeBothCopyErr(void)		{ return AlxSafe_BothCopyErr_OrigErr; }
int32_t AlxFakesTest_Status_SafeUseCopyA(void)			{ return AlxSafe_BothCopyOkCrcSame_OrigDontCare_UseCopyA; }
