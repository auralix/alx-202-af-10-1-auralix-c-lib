/**
  ******************************************************************************
  * @file		alxMemSafeTestHelpers.c
  * @brief		Auralix C Library - ALX Memory Safe Module - PC test helpers
  * @copyright	Copyright (C) Auralix d.o.o. All rights reserved.
  *
  * Opaque-handle constructor for the complete safe-store chain under test,
  * wired the SAME way the product wires it (ALX-1513):
  * AlxCrc (CRC32) -> AlxMemRaw (faked by alxMemRawFake.c) -> AlxMemSafe
  * (copy A / copy B, copyLen 5) -> AlxParamGroup (five uint8 items with the
  * product's keys and factory defaults) -> AlxParamStore (one group).
  * Every layer is reachable from the tests so each contract can be proven
  * on its own and end to end. A new context over the same fake memory models
  * a reboot.
  ******************************************************************************
  **/

#include "alxMemSafe.h"
#include "alxParamGroup.h"
#include "alxParamStore.h"
#include "alxParamItem.h"

#include <stdlib.h>

#define ALX_MEM_SAFE_TEST_NUM_OF_ITEMS 5
#define ALX_MEM_SAFE_TEST_COPY_LEN 5			// 5 x uint8, same as the product

typedef struct
{
	AlxCrc crc;
	AlxMemRaw memRaw;
	uint8_t buff1[16];
	uint8_t buff2[16];
	AlxMemSafe memSafe;
	AlxParamItem items[ALX_MEM_SAFE_TEST_NUM_OF_ITEMS];
	AlxParamItem* itemArr[ALX_MEM_SAFE_TEST_NUM_OF_ITEMS];
	uint8_t valBuff[ALX_MEM_SAFE_TEST_COPY_LEN];
	uint8_t valStoredBuff[ALX_MEM_SAFE_TEST_COPY_LEN];
	uint8_t valToStoreBuff[ALX_MEM_SAFE_TEST_COPY_LEN];
	AlxParamGroup group;
	AlxParamGroup* groupArr[1];
	AlxParamStore store;
	Alx_Status storeInitStatus[1];
} AlxMemSafeTest_Ctx;

// Exported test API
AlxMemSafeTest_Ctx* AlxMemSafeTest_New(uint32_t copyAddrA, uint32_t copyAddrB, uint8_t memSafeTries, uint8_t memRawTries);
void AlxMemSafeTest_Delete(AlxMemSafeTest_Ctx* ctx);
uint32_t AlxMemSafeTest_CopyLen(void);
uint32_t AlxMemSafeTest_NumOfItems(void);
int32_t AlxMemSafeTest_MemSafeRead(AlxMemSafeTest_Ctx* ctx, uint8_t* data, uint32_t len);
int32_t AlxMemSafeTest_MemSafeWrite(AlxMemSafeTest_Ctx* ctx, uint8_t* data, uint32_t len);
bool AlxMemSafeTest_MemSafeIsReadDone(AlxMemSafeTest_Ctx* ctx);
bool AlxMemSafeTest_MemSafeIsReadErr(AlxMemSafeTest_Ctx* ctx);
bool AlxMemSafeTest_MemSafeIsWriteDone(AlxMemSafeTest_Ctx* ctx);
bool AlxMemSafeTest_MemSafeIsWriteErr(AlxMemSafeTest_Ctx* ctx);
int32_t AlxMemSafeTest_GroupInit(AlxMemSafeTest_Ctx* ctx);
bool AlxMemSafeTest_GroupIsValStoredBuffDiff(AlxMemSafeTest_Ctx* ctx);
int32_t AlxMemSafeTest_StoreInit(AlxMemSafeTest_Ctx* ctx);
void AlxMemSafeTest_StoreHandle(AlxMemSafeTest_Ctx* ctx);
bool AlxMemSafeTest_StoreIsErr(AlxMemSafeTest_Ctx* ctx);
uint32_t AlxMemSafeTest_ItemGet(AlxMemSafeTest_Ctx* ctx, uint32_t index);
int32_t AlxMemSafeTest_ItemSet(AlxMemSafeTest_Ctx* ctx, uint32_t index, uint32_t val);
uint32_t AlxMemSafeTest_ItemGetDef(AlxMemSafeTest_Ctx* ctx, uint32_t index);
uint32_t AlxMemSafeTest_CrcCalc(uint32_t config, uint8_t* data, uint32_t len);
bool AlxMemSafeTest_CrcIsOk(uint32_t config, uint8_t* dataWithCrc, uint32_t lenWithCrc, uint32_t* validatedCrc);
uint32_t AlxMemSafeTest_CrcLen(uint32_t config);
int32_t AlxMemSafeTest_Status_Ok(void);
int32_t AlxMemSafeTest_Status_Err(void);
int32_t AlxMemSafeTest_Status_ErrNumOfTries(void);
int32_t AlxMemSafeTest_Status_BothCopyErr(void);
int32_t AlxMemSafeTest_Status_BothOkSame_UseA(void);
int32_t AlxMemSafeTest_Status_BothOkDiff_UseA(void);
int32_t AlxMemSafeTest_Status_AOkBErr_UseA(void);
int32_t AlxMemSafeTest_Status_AErrBOk_UseB(void);

static void AlxMemSafeTest_OutOfMemory(void)
{
	exit(1);	// test infrastructure - fail fast on OOM
}

AlxMemSafeTest_Ctx* AlxMemSafeTest_New(uint32_t copyAddrA, uint32_t copyAddrB, uint8_t memSafeTries, uint8_t memRawTries)
{
	AlxMemSafeTest_Ctx* ctx = (AlxMemSafeTest_Ctx*)calloc(1, sizeof(AlxMemSafeTest_Ctx));
	memset(ctx, 0xFF, sizeof(AlxMemSafeTest_Ctx));	// poison BEFORE the Ctors: a forgotten field init must show (not 0xAA - clang tests a bool by its low bit)
	if (ctx == NULL)
	{
		AlxMemSafeTest_OutOfMemory();
		return NULL;
	}

	// Same chain and the same numbers as the product (ALX-1513)
	AlxCrc_Ctor(&ctx->crc, AlxCrc_Config_Crc32);
	AlxMemRaw_Ctor(&ctx->memRaw);
	AlxMemSafe_Ctor
	(
		&ctx->memSafe,
		&ctx->memRaw,
		&ctx->crc,
		copyAddrA,
		copyAddrB,
		ALX_MEM_SAFE_TEST_COPY_LEN,
		false,				// nonBlockingEnable
		memSafeTries,
		memRawTries,
		100,				// memRawReadWriteTimeout_ms
		ctx->buff1,
		sizeof(ctx->buff1),
		ctx->buff2,
		sizeof(ctx->buff2)
	);

	// The product's five stored params: key, factory default, 0..100, out-of-range ignored.
	// The KEYS are generic on purpose - this is a public library and the client's parameter
	// vocabulary is not ours to publish. Nothing reads them by name: what the tests measure is
	// the count, the widths and the defaults.
	static const char* const keys[ALX_MEM_SAFE_TEST_NUM_OF_ITEMS] = { "PARAM_1_pct", "PARAM_2_pct", "PARAM_3_pct", "PARAM_4_pct", "PARAM_5_pct" };
	static const uint8_t defs[ALX_MEM_SAFE_TEST_NUM_OF_ITEMS] = { 0, 0, 50, 100, 70 };
	for (uint32_t i = 0; i < ALX_MEM_SAFE_TEST_NUM_OF_ITEMS; i++)
	{
		AlxParamItem_CtorUint8
		(
			&ctx->items[i],
			NULL,				// paramKvStore - none, the group stores the items
			AlxParamItem_Param,
			keys[i],
			i,					// id
			NULL,				// groupKey
			0,					// groupId
			defs[i],			// valDef
			0,					// valMin
			100,				// valMax
			AlxParamItem_Ignore,
			false,				// isEnum
			NULL,				// enumArr
			0,					// enumArrLen
			"",					// valUnit
			false				// valChangeTakesEffectAfterReset
		);
		ctx->itemArr[i] = &ctx->items[i];
	}

	AlxParamGroup_Ctor
	(
		&ctx->group,
		&ctx->memSafe,
		"AlxParamGroup",
		ALX_MEM_SAFE_TEST_COPY_LEN,
		ctx->valBuff,
		ctx->valStoredBuff,
		ctx->valToStoreBuff,
		ctx->itemArr,
		ALX_MEM_SAFE_TEST_NUM_OF_ITEMS,
		3					// initNumOfTries
	);
	ctx->groupArr[0] = &ctx->group;
	AlxParamStore_Ctor(&ctx->store, ctx->groupArr, 1);
	ctx->storeInitStatus[0] = Alx_Err;

	return ctx;
}

void AlxMemSafeTest_Delete(AlxMemSafeTest_Ctx* ctx)
{
	free(ctx);
}

uint32_t AlxMemSafeTest_CopyLen(void)
{
	return ALX_MEM_SAFE_TEST_COPY_LEN;
}

uint32_t AlxMemSafeTest_NumOfItems(void)
{
	return ALX_MEM_SAFE_TEST_NUM_OF_ITEMS;
}

int32_t AlxMemSafeTest_MemSafeRead(AlxMemSafeTest_Ctx* ctx, uint8_t* data, uint32_t len)
{
	Alx_Status status = AlxMemSafe_Read(&ctx->memSafe, data, len);
	return (int32_t)status;
}

int32_t AlxMemSafeTest_MemSafeWrite(AlxMemSafeTest_Ctx* ctx, uint8_t* data, uint32_t len)
{
	Alx_Status status = AlxMemSafe_Write(&ctx->memSafe, data, len);
	return (int32_t)status;
}

bool AlxMemSafeTest_MemSafeIsReadDone(AlxMemSafeTest_Ctx* ctx)
{
	return AlxMemSafe_IsReadDone(&ctx->memSafe);
}

bool AlxMemSafeTest_MemSafeIsReadErr(AlxMemSafeTest_Ctx* ctx)
{
	return AlxMemSafe_IsReadErr(&ctx->memSafe);
}

bool AlxMemSafeTest_MemSafeIsWriteDone(AlxMemSafeTest_Ctx* ctx)
{
	return AlxMemSafe_IsWriteDone(&ctx->memSafe);
}

bool AlxMemSafeTest_MemSafeIsWriteErr(AlxMemSafeTest_Ctx* ctx)
{
	return AlxMemSafe_IsWriteErr(&ctx->memSafe);
}

int32_t AlxMemSafeTest_GroupInit(AlxMemSafeTest_Ctx* ctx)
{
	Alx_Status status = AlxParamGroup_Init(&ctx->group);
	return (int32_t)status;
}

bool AlxMemSafeTest_GroupIsValStoredBuffDiff(AlxMemSafeTest_Ctx* ctx)
{
	return AlxParamGroup_IsValStoredBuffDiff(&ctx->group);
}

int32_t AlxMemSafeTest_StoreInit(AlxMemSafeTest_Ctx* ctx)
{
	Alx_Status status = AlxParamStore_Init(&ctx->store, ctx->storeInitStatus, 1);
	return (int32_t)status;
}

void AlxMemSafeTest_StoreHandle(AlxMemSafeTest_Ctx* ctx)
{
	AlxParamStore_Handle(&ctx->store);
}

bool AlxMemSafeTest_StoreIsErr(AlxMemSafeTest_Ctx* ctx)
{
	return AlxParamStore_IsErr(&ctx->store);
}

uint32_t AlxMemSafeTest_ItemGet(AlxMemSafeTest_Ctx* ctx, uint32_t index)
{
	if (index >= ALX_MEM_SAFE_TEST_NUM_OF_ITEMS)
	{
		return UINT32_MAX;
	}
	return AlxParamItem_GetValUint8(&ctx->items[index]);
}

int32_t AlxMemSafeTest_ItemSet(AlxMemSafeTest_Ctx* ctx, uint32_t index, uint32_t val)
{
	if (index >= ALX_MEM_SAFE_TEST_NUM_OF_ITEMS)
	{
		return (int32_t)Alx_Err;
	}
	Alx_Status status = AlxParamItem_SetValUint8(&ctx->items[index], (uint8_t)val);
	return (int32_t)status;
}

uint32_t AlxMemSafeTest_ItemGetDef(AlxMemSafeTest_Ctx* ctx, uint32_t index)
{
	if (index >= ALX_MEM_SAFE_TEST_NUM_OF_ITEMS)
	{
		return UINT32_MAX;
	}
	return AlxParamItem_GetValDefUint8(&ctx->items[index]);
}

uint32_t AlxMemSafeTest_CrcCalc(uint32_t config, uint8_t* data, uint32_t len)
{
	AlxCrc crc;
	AlxCrc_Ctor(&crc, (AlxCrc_Config)config);
	return AlxCrc_Calc(&crc, data, len);
}

bool AlxMemSafeTest_CrcIsOk(uint32_t config, uint8_t* dataWithCrc, uint32_t lenWithCrc, uint32_t* validatedCrc)
{
	AlxCrc crc;
	AlxCrc_Ctor(&crc, (AlxCrc_Config)config);
	return AlxCrc_IsOk(&crc, dataWithCrc, lenWithCrc, validatedCrc);
}

uint32_t AlxMemSafeTest_CrcLen(uint32_t config)
{
	AlxCrc crc;
	AlxCrc_Ctor(&crc, (AlxCrc_Config)config);
	return AlxCrc_GetLen(&crc);
}

int32_t AlxMemSafeTest_Status_Ok(void)					{ return (int32_t)Alx_Ok; }
int32_t AlxMemSafeTest_Status_Err(void)					{ return (int32_t)Alx_Err; }
int32_t AlxMemSafeTest_Status_ErrNumOfTries(void)		{ return (int32_t)Alx_ErrNumOfTries; }
int32_t AlxMemSafeTest_Status_BothCopyErr(void)			{ return (int32_t)AlxSafe_BothCopyErr_OrigErr; }
int32_t AlxMemSafeTest_Status_BothOkSame_UseA(void)		{ return (int32_t)AlxSafe_BothCopyOkCrcSame_OrigDontCare_UseCopyA; }
int32_t AlxMemSafeTest_Status_BothOkDiff_UseA(void)		{ return (int32_t)AlxSafe_BothCopyOkCrcDiff_OrigDontCare_UseCopyA; }
int32_t AlxMemSafeTest_Status_AOkBErr_UseA(void)		{ return (int32_t)AlxSafe_CopyAOkCopyBErr_OrigDontCare_UseCopyA; }
int32_t AlxMemSafeTest_Status_AErrBOk_UseB(void)		{ return (int32_t)AlxSafe_CopyAErrCopyBOk_OrigDontCare_UseCopyB; }


//------------------------------------------------------------------------------
// Parameter item - the string format conversion (ALX-1553)
//------------------------------------------------------------------------------
// A standalone AlxParamItem, deliberately OUTSIDE the store chain above: what is
// under test here is AlxParamItem_SetVal_StrFormat / _GetVal_StrFormat, not
// persistence. One item per context, built with the type and the range the test
// asks for, so a test can state its range and then say what the CLI would do
// with a value outside it. The CLI is where this conversion is reached in a
// product (set-param), and a bench measurement showed a value that does not fit
// the target type being accepted as a different value - this is where that can
// be pinned without a device.

typedef struct
{
	AlxParamItem item;
	uint32_t type;
} AlxParamItemStrTest_Ctx;

AlxParamItemStrTest_Ctx* AlxParamItemStrTest_New(uint32_t type, double valDef, double valMin, double valMax);
void AlxParamItemStrTest_Delete(AlxParamItemStrTest_Ctx* ctx);
int32_t AlxParamItemStrTest_SetStr(AlxParamItemStrTest_Ctx* ctx, const char* val);
int32_t AlxParamItemStrTest_GetStr(AlxParamItemStrTest_Ctx* ctx, char* out, uint32_t maxLenWithNullTerm);
double AlxParamItemStrTest_GetNum(AlxParamItemStrTest_Ctx* ctx);

AlxParamItemStrTest_Ctx* AlxParamItemStrTest_New(uint32_t type, double valDef, double valMin, double valMax)
{
	AlxParamItemStrTest_Ctx* ctx = calloc(1, sizeof(AlxParamItemStrTest_Ctx));
	if (ctx == NULL)
	{
		return NULL;
	}
	ctx->type = type;

	// id 0, no kv store, no group: the item stands alone
	switch (type)
	{
		case 0:
			AlxParamItem_CtorUint8(&ctx->item, NULL, AlxParamItem_Param, "TEST_ITEM", 0, NULL, 0,
				(uint8_t)valDef, (uint8_t)valMin, (uint8_t)valMax, AlxParamItem_Ignore, false, NULL, 0, "", false);
			break;
		case 1:
			AlxParamItem_CtorUint16(&ctx->item, NULL, AlxParamItem_Param, "TEST_ITEM", 0, NULL, 0,
				(uint16_t)valDef, (uint16_t)valMin, (uint16_t)valMax, AlxParamItem_Ignore, false, NULL, 0, "", false);
			break;
		case 2:
			AlxParamItem_CtorUint32(&ctx->item, NULL, AlxParamItem_Param, "TEST_ITEM", 0, NULL, 0,
				(uint32_t)valDef, (uint32_t)valMin, (uint32_t)valMax, AlxParamItem_Ignore, false, NULL, 0, "", false);
			break;
		case 3:
			AlxParamItem_CtorInt8(&ctx->item, NULL, AlxParamItem_Param, "TEST_ITEM", 0, NULL, 0,
				(int8_t)valDef, (int8_t)valMin, (int8_t)valMax, AlxParamItem_Ignore, false, NULL, 0, "", false);
			break;
		case 4:
			AlxParamItem_CtorInt16(&ctx->item, NULL, AlxParamItem_Param, "TEST_ITEM", 0, NULL, 0,
				(int16_t)valDef, (int16_t)valMin, (int16_t)valMax, AlxParamItem_Ignore, false, NULL, 0, "", false);
			break;
		case 5:
			AlxParamItem_CtorInt32(&ctx->item, NULL, AlxParamItem_Param, "TEST_ITEM", 0, NULL, 0,
				(int32_t)valDef, (int32_t)valMin, (int32_t)valMax, AlxParamItem_Ignore, false, NULL, 0, "", false);
			break;
		case 6:
			AlxParamItem_CtorFloat(&ctx->item, NULL, AlxParamItem_Param, "TEST_ITEM", 0, NULL, 0,
				(float)valDef, (float)valMin, (float)valMax, AlxParamItem_Ignore, false, NULL, 0, "", false);
			break;
		case 7:
			AlxParamItem_CtorBool(&ctx->item, NULL, AlxParamItem_Param, "TEST_ITEM", 0, NULL, 0,
				valDef != 0.0, "", false);
			break;
		default:
			free(ctx);
			return NULL;
	}
	return ctx;
}

void AlxParamItemStrTest_Delete(AlxParamItemStrTest_Ctx* ctx)
{
	free(ctx);
}

int32_t AlxParamItemStrTest_SetStr(AlxParamItemStrTest_Ctx* ctx, const char* val)
{
	// SetVal_StrFormat takes a writable buffer; the caller's string is const
	char buff[128] = "";
	uint32_t i = 0;
	while (val[i] != '\0' && i < (sizeof(buff) - 1))
	{
		buff[i] = val[i];
		i++;
	}
	buff[i] = '\0';
	Alx_Status status = AlxParamItem_SetVal_StrFormat(&ctx->item, buff);
	return (int32_t)status;
}

int32_t AlxParamItemStrTest_GetStr(AlxParamItemStrTest_Ctx* ctx, char* out, uint32_t maxLenWithNullTerm)
{
	Alx_Status status = AlxParamItem_GetVal_StrFormat(&ctx->item, out, maxLenWithNullTerm);
	return (int32_t)status;
}

double AlxParamItemStrTest_GetNum(AlxParamItemStrTest_Ctx* ctx)
{
	// each value lands in a variable of its own type first: casting a function call result
	// straight to double is -Wbad-function-cast, which the strict warning set treats as an error
	switch (ctx->type)
	{
		case 0:  { uint8_t v = AlxParamItem_GetValUint8(&ctx->item);   return (double)v; }
		case 1:  { uint16_t v = AlxParamItem_GetValUint16(&ctx->item); return (double)v; }
		case 2:  { uint32_t v = AlxParamItem_GetValUint32(&ctx->item); return (double)v; }
		case 3:  { int8_t v = AlxParamItem_GetValInt8(&ctx->item);     return (double)v; }
		case 4:  { int16_t v = AlxParamItem_GetValInt16(&ctx->item);   return (double)v; }
		case 5:  { int32_t v = AlxParamItem_GetValInt32(&ctx->item);   return (double)v; }
		case 6:  { float v = AlxParamItem_GetValFloat(&ctx->item);     return (double)v; }
		case 7:  { bool v = AlxParamItem_GetValBool(&ctx->item);       if (v) { return 1.0; } return 0.0; }
		default: return 0.0;
	}
}


//------------------------------------------------------------------------------
// Parameter item - the metadata it is CONSTRUCTED with, read back (ALX-1553)
//------------------------------------------------------------------------------
// The shim above reaches one corner of alxParamItem: the string conversion. The
// module has 105 functions and that corner is 25 of them, so four fifths of a
// module the whole parameter path leans on had never been called.
//
// What this adds is the other half of the constructor's contract. An item is
// built from seventeen arguments - key, id, group, default, min, max, unit, the
// out-of-range handle - and every one of them has a getter. Nothing asserted
// that what went in comes back out, which is exactly the shape of defect a
// product cannot see: a wrong default or a wrong unit is not a crash, it is a
// value that quietly ships.
//
// The type axis is the library's own AlxParamItem_DataType, NOT the private
// numbering the string shim uses - one mapping in this file is enough.
//
// Values cross the boundary as int64_t or double, so a test can name each
// type's edges exactly: int64_t is exact for every integer type covered here
// and double is exact for float. Uint64 above INT64_MAX is the one value this
// cannot carry, and no test asks for it.

typedef struct
{
	AlxParamItem item;
	AlxParamItem_DataType dataType;
	char key[64];
	char groupKey[64];
	char unit[64];
} AlxParamItemMetaTest_Ctx;

AlxParamItemMetaTest_Ctx* AlxParamItemMetaTest_New(uint32_t dataType, const char* key, uint32_t id, const char* groupKey, uint32_t groupId, int64_t defI, int64_t minI, int64_t maxI, double defF, double minF, double maxF, uint32_t outOfRange, const char* valUnit, bool afterReset);
void AlxParamItemMetaTest_Delete(AlxParamItemMetaTest_Ctx* ctx);
uint32_t AlxParamItemMetaTest_GetDataType(AlxParamItemMetaTest_Ctx* ctx);
uint32_t AlxParamItemMetaTest_GetParamType(AlxParamItemMetaTest_Ctx* ctx);
const char* AlxParamItemMetaTest_GetKey(AlxParamItemMetaTest_Ctx* ctx);
uint32_t AlxParamItemMetaTest_GetId(AlxParamItemMetaTest_Ctx* ctx);
const char* AlxParamItemMetaTest_GetGroupKey(AlxParamItemMetaTest_Ctx* ctx);
uint32_t AlxParamItemMetaTest_GetGroupId(AlxParamItemMetaTest_Ctx* ctx);
uint32_t AlxParamItemMetaTest_GetValOutOfRangeHandle(AlxParamItemMetaTest_Ctx* ctx);
const char* AlxParamItemMetaTest_GetValUnit(AlxParamItemMetaTest_Ctx* ctx);
bool AlxParamItemMetaTest_GetValChangeTakesEffectAfterReset(AlxParamItemMetaTest_Ctx* ctx);
uint32_t AlxParamItemMetaTest_GetBuffLen(AlxParamItemMetaTest_Ctx* ctx);
uint32_t AlxParamItemMetaTest_GetValLen(AlxParamItemMetaTest_Ctx* ctx);
bool AlxParamItemMetaTest_GetIsEnum(AlxParamItemMetaTest_Ctx* ctx);
int64_t AlxParamItemMetaTest_GetValDefI(AlxParamItemMetaTest_Ctx* ctx);
int64_t AlxParamItemMetaTest_GetValMinI(AlxParamItemMetaTest_Ctx* ctx);
int64_t AlxParamItemMetaTest_GetValMaxI(AlxParamItemMetaTest_Ctx* ctx);
double AlxParamItemMetaTest_GetValDefF(AlxParamItemMetaTest_Ctx* ctx);
double AlxParamItemMetaTest_GetValMinF(AlxParamItemMetaTest_Ctx* ctx);
double AlxParamItemMetaTest_GetValMaxF(AlxParamItemMetaTest_Ctx* ctx);
int64_t AlxParamItemMetaTest_GetValI(AlxParamItemMetaTest_Ctx* ctx);
double AlxParamItemMetaTest_GetValF(AlxParamItemMetaTest_Ctx* ctx);
int32_t AlxParamItemMetaTest_SetValI(AlxParamItemMetaTest_Ctx* ctx, int64_t val);
int32_t AlxParamItemMetaTest_SetValF(AlxParamItemMetaTest_Ctx* ctx, double val);
void AlxParamItemMetaTest_SetValToDef(AlxParamItemMetaTest_Ctx* ctx);

AlxParamItemMetaTest_Ctx* AlxParamItemMetaTest_New(uint32_t dataType, const char* key, uint32_t id, const char* groupKey, uint32_t groupId, int64_t defI, int64_t minI, int64_t maxI, double defF, double minF, double maxF, uint32_t outOfRange, const char* valUnit, bool afterReset)
{
	AlxParamItemMetaTest_Ctx* ctx = calloc(1, sizeof(AlxParamItemMetaTest_Ctx));
	if (ctx == NULL)
	{
		return NULL;
	}
	ctx->dataType = (AlxParamItem_DataType)dataType;
	strncpy(ctx->key, key, 63);
	strncpy(ctx->groupKey, groupKey, 63);
	strncpy(ctx->unit, valUnit, 63);

	AlxParamItem_ValOutOfRangeHandle handle = (AlxParamItem_ValOutOfRangeHandle)outOfRange;
	switch (ctx->dataType)
	{
		case AlxParamItem_Uint8:
			AlxParamItem_CtorUint8(&ctx->item, NULL, AlxParamItem_Param, ctx->key, id, ctx->groupKey, groupId,
				(uint8_t)defI, (uint8_t)minI, (uint8_t)maxI, handle, false, NULL, 0, ctx->unit, afterReset);
			break;
		case AlxParamItem_Uint16:
			AlxParamItem_CtorUint16(&ctx->item, NULL, AlxParamItem_Param, ctx->key, id, ctx->groupKey, groupId,
				(uint16_t)defI, (uint16_t)minI, (uint16_t)maxI, handle, false, NULL, 0, ctx->unit, afterReset);
			break;
		case AlxParamItem_Uint32:
			AlxParamItem_CtorUint32(&ctx->item, NULL, AlxParamItem_Param, ctx->key, id, ctx->groupKey, groupId,
				(uint32_t)defI, (uint32_t)minI, (uint32_t)maxI, handle, false, NULL, 0, ctx->unit, afterReset);
			break;
		case AlxParamItem_Uint64:
			AlxParamItem_CtorUint64(&ctx->item, NULL, AlxParamItem_Param, ctx->key, id, ctx->groupKey, groupId,
				(uint64_t)defI, (uint64_t)minI, (uint64_t)maxI, handle, false, NULL, 0, ctx->unit, afterReset);
			break;
		case AlxParamItem_Int8:
			AlxParamItem_CtorInt8(&ctx->item, NULL, AlxParamItem_Param, ctx->key, id, ctx->groupKey, groupId,
				(int8_t)defI, (int8_t)minI, (int8_t)maxI, handle, false, NULL, 0, ctx->unit, afterReset);
			break;
		case AlxParamItem_Int16:
			AlxParamItem_CtorInt16(&ctx->item, NULL, AlxParamItem_Param, ctx->key, id, ctx->groupKey, groupId,
				(int16_t)defI, (int16_t)minI, (int16_t)maxI, handle, false, NULL, 0, ctx->unit, afterReset);
			break;
		case AlxParamItem_Int32:
			AlxParamItem_CtorInt32(&ctx->item, NULL, AlxParamItem_Param, ctx->key, id, ctx->groupKey, groupId,
				(int32_t)defI, (int32_t)minI, (int32_t)maxI, handle, false, NULL, 0, ctx->unit, afterReset);
			break;
		case AlxParamItem_Int64:
			AlxParamItem_CtorInt64(&ctx->item, NULL, AlxParamItem_Param, ctx->key, id, ctx->groupKey, groupId,
				defI, minI, maxI, handle, false, NULL, 0, ctx->unit, afterReset);
			break;
		case AlxParamItem_Float:
			AlxParamItem_CtorFloat(&ctx->item, NULL, AlxParamItem_Param, ctx->key, id, ctx->groupKey, groupId,
				(float)defF, (float)minF, (float)maxF, handle, false, NULL, 0, ctx->unit, afterReset);
			break;
		case AlxParamItem_Double:
			AlxParamItem_CtorDouble(&ctx->item, NULL, AlxParamItem_Param, ctx->key, id, ctx->groupKey, groupId,
				defF, minF, maxF, handle, false, NULL, 0, ctx->unit, afterReset);
			break;
		case AlxParamItem_Bool:
			// no range and no enum: the ctor takes the default, the unit and the reset flag
			AlxParamItem_CtorBool(&ctx->item, NULL, AlxParamItem_Param, ctx->key, id, ctx->groupKey, groupId,
				defI != 0, ctx->unit, afterReset);
			break;
		case AlxParamItem_Arr:
		case AlxParamItem_Str:
		default:
			free(ctx);
			return NULL;
	}
	return ctx;
}

void AlxParamItemMetaTest_Delete(AlxParamItemMetaTest_Ctx* ctx)
{
	free(ctx);
}

uint32_t AlxParamItemMetaTest_GetDataType(AlxParamItemMetaTest_Ctx* ctx)
{
	AlxParamItem_DataType value = AlxParamItem_GetDataType(&ctx->item);
	return (uint32_t)value;
}
uint32_t AlxParamItemMetaTest_GetParamType(AlxParamItemMetaTest_Ctx* ctx)
{
	AlxParamItem_ParamType value = AlxParamItem_GetParamType(&ctx->item);
	return (uint32_t)value;
}
const char* AlxParamItemMetaTest_GetKey(AlxParamItemMetaTest_Ctx* ctx)
{
	return AlxParamItem_GetKey(&ctx->item);
}
uint32_t AlxParamItemMetaTest_GetId(AlxParamItemMetaTest_Ctx* ctx)
{
	return AlxParamItem_GetId(&ctx->item);
}
const char* AlxParamItemMetaTest_GetGroupKey(AlxParamItemMetaTest_Ctx* ctx)
{
	return AlxParamItem_GetGroupKey(&ctx->item);
}
uint32_t AlxParamItemMetaTest_GetGroupId(AlxParamItemMetaTest_Ctx* ctx)
{
	return AlxParamItem_GetGroupId(&ctx->item);
}
uint32_t AlxParamItemMetaTest_GetValOutOfRangeHandle(AlxParamItemMetaTest_Ctx* ctx)
{
	AlxParamItem_ValOutOfRangeHandle value = AlxParamItem_GetValOutOfRangeHandle(&ctx->item);
	return (uint32_t)value;
}
const char* AlxParamItemMetaTest_GetValUnit(AlxParamItemMetaTest_Ctx* ctx)
{
	return AlxParamItem_GetValUnit(&ctx->item);
}
bool AlxParamItemMetaTest_GetValChangeTakesEffectAfterReset(AlxParamItemMetaTest_Ctx* ctx)
{
	return AlxParamItem_GetValChangeTakesEffectAfterReset(&ctx->item);
}
uint32_t AlxParamItemMetaTest_GetBuffLen(AlxParamItemMetaTest_Ctx* ctx)
{
	return AlxParamItem_GetBuffLen(&ctx->item);
}
uint32_t AlxParamItemMetaTest_GetValLen(AlxParamItemMetaTest_Ctx* ctx)
{
	return AlxParamItem_GetValLen(&ctx->item);
}
bool AlxParamItemMetaTest_GetIsEnum(AlxParamItemMetaTest_Ctx* ctx)
{
	return AlxParamItem_GetIsEnum(&ctx->item);
}

int64_t AlxParamItemMetaTest_GetValDefI(AlxParamItemMetaTest_Ctx* ctx)
{
	switch (ctx->dataType)
	{
		case AlxParamItem_Uint8:	return (int64_t)AlxParamItem_GetValDefUint8(&ctx->item);
		case AlxParamItem_Uint16:	return (int64_t)AlxParamItem_GetValDefUint16(&ctx->item);
		case AlxParamItem_Uint32:	return (int64_t)AlxParamItem_GetValDefUint32(&ctx->item);
		case AlxParamItem_Uint64:	return (int64_t)AlxParamItem_GetValDefUint64(&ctx->item);
		case AlxParamItem_Int8:		return (int64_t)AlxParamItem_GetValDefInt8(&ctx->item);
		case AlxParamItem_Int16:	return (int64_t)AlxParamItem_GetValDefInt16(&ctx->item);
		case AlxParamItem_Int32:	return (int64_t)AlxParamItem_GetValDefInt32(&ctx->item);
		case AlxParamItem_Int64:	return AlxParamItem_GetValDefInt64(&ctx->item);
		case AlxParamItem_Bool:		return AlxParamItem_GetValDefBool(&ctx->item);
		case AlxParamItem_Float:
		case AlxParamItem_Double:
		case AlxParamItem_Arr:
		case AlxParamItem_Str:
		default:					return 0;
	}
}
int64_t AlxParamItemMetaTest_GetValMinI(AlxParamItemMetaTest_Ctx* ctx)
{
	switch (ctx->dataType)
	{
		case AlxParamItem_Uint8:	return (int64_t)AlxParamItem_GetValMinUint8(&ctx->item);
		case AlxParamItem_Uint16:	return (int64_t)AlxParamItem_GetValMinUint16(&ctx->item);
		case AlxParamItem_Uint32:	return (int64_t)AlxParamItem_GetValMinUint32(&ctx->item);
		case AlxParamItem_Uint64:	return (int64_t)AlxParamItem_GetValMinUint64(&ctx->item);
		case AlxParamItem_Int8:		return (int64_t)AlxParamItem_GetValMinInt8(&ctx->item);
		case AlxParamItem_Int16:	return (int64_t)AlxParamItem_GetValMinInt16(&ctx->item);
		case AlxParamItem_Int32:	return (int64_t)AlxParamItem_GetValMinInt32(&ctx->item);
		case AlxParamItem_Int64:	return AlxParamItem_GetValMinInt64(&ctx->item);
		case AlxParamItem_Float:
		case AlxParamItem_Double:
		case AlxParamItem_Bool:
		case AlxParamItem_Arr:
		case AlxParamItem_Str:
		default:					return 0;
	}
}
int64_t AlxParamItemMetaTest_GetValMaxI(AlxParamItemMetaTest_Ctx* ctx)
{
	switch (ctx->dataType)
	{
		case AlxParamItem_Uint8:	return (int64_t)AlxParamItem_GetValMaxUint8(&ctx->item);
		case AlxParamItem_Uint16:	return (int64_t)AlxParamItem_GetValMaxUint16(&ctx->item);
		case AlxParamItem_Uint32:	return (int64_t)AlxParamItem_GetValMaxUint32(&ctx->item);
		case AlxParamItem_Uint64:	return (int64_t)AlxParamItem_GetValMaxUint64(&ctx->item);
		case AlxParamItem_Int8:		return (int64_t)AlxParamItem_GetValMaxInt8(&ctx->item);
		case AlxParamItem_Int16:	return (int64_t)AlxParamItem_GetValMaxInt16(&ctx->item);
		case AlxParamItem_Int32:	return (int64_t)AlxParamItem_GetValMaxInt32(&ctx->item);
		case AlxParamItem_Int64:	return AlxParamItem_GetValMaxInt64(&ctx->item);
		case AlxParamItem_Float:
		case AlxParamItem_Double:
		case AlxParamItem_Bool:
		case AlxParamItem_Arr:
		case AlxParamItem_Str:
		default:					return 0;
	}
}
double AlxParamItemMetaTest_GetValDefF(AlxParamItemMetaTest_Ctx* ctx)
{
	if (ctx->dataType == AlxParamItem_Float)
	{
		return (double)AlxParamItem_GetValDefFloat(&ctx->item);
	}
	return AlxParamItem_GetValDefDouble(&ctx->item);
}
double AlxParamItemMetaTest_GetValMinF(AlxParamItemMetaTest_Ctx* ctx)
{
	if (ctx->dataType == AlxParamItem_Float)
	{
		return (double)AlxParamItem_GetValMinFloat(&ctx->item);
	}
	return AlxParamItem_GetValMinDouble(&ctx->item);
}
double AlxParamItemMetaTest_GetValMaxF(AlxParamItemMetaTest_Ctx* ctx)
{
	if (ctx->dataType == AlxParamItem_Float)
	{
		return (double)AlxParamItem_GetValMaxFloat(&ctx->item);
	}
	return AlxParamItem_GetValMaxDouble(&ctx->item);
}

int64_t AlxParamItemMetaTest_GetValI(AlxParamItemMetaTest_Ctx* ctx)
{
	switch (ctx->dataType)
	{
		case AlxParamItem_Uint8:	return (int64_t)AlxParamItem_GetValUint8(&ctx->item);
		case AlxParamItem_Uint16:	return (int64_t)AlxParamItem_GetValUint16(&ctx->item);
		case AlxParamItem_Uint32:	return (int64_t)AlxParamItem_GetValUint32(&ctx->item);
		case AlxParamItem_Uint64:	return (int64_t)AlxParamItem_GetValUint64(&ctx->item);
		case AlxParamItem_Int8:		return (int64_t)AlxParamItem_GetValInt8(&ctx->item);
		case AlxParamItem_Int16:	return (int64_t)AlxParamItem_GetValInt16(&ctx->item);
		case AlxParamItem_Int32:	return (int64_t)AlxParamItem_GetValInt32(&ctx->item);
		case AlxParamItem_Int64:	return AlxParamItem_GetValInt64(&ctx->item);
		case AlxParamItem_Bool:		return AlxParamItem_GetValBool(&ctx->item);
		case AlxParamItem_Float:
		case AlxParamItem_Double:
		case AlxParamItem_Arr:
		case AlxParamItem_Str:
		default:					return 0;
	}
}
double AlxParamItemMetaTest_GetValF(AlxParamItemMetaTest_Ctx* ctx)
{
	if (ctx->dataType == AlxParamItem_Float)
	{
		return (double)AlxParamItem_GetValFloat(&ctx->item);
	}
	return AlxParamItem_GetValDouble(&ctx->item);
}

int32_t AlxParamItemMetaTest_SetValI(AlxParamItemMetaTest_Ctx* ctx, int64_t val)
{
	Alx_Status status = Alx_Err;
	switch (ctx->dataType)
	{
		case AlxParamItem_Uint8:	status = AlxParamItem_SetValUint8(&ctx->item, (uint8_t)val);	break;
		case AlxParamItem_Uint16:status = AlxParamItem_SetValUint16(&ctx->item, (uint16_t)val);	break;
		case AlxParamItem_Uint32:status = AlxParamItem_SetValUint32(&ctx->item, (uint32_t)val);	break;
		case AlxParamItem_Uint64:status = AlxParamItem_SetValUint64(&ctx->item, (uint64_t)val);	break;
		case AlxParamItem_Int8:	status = AlxParamItem_SetValInt8(&ctx->item, (int8_t)val);	break;
		case AlxParamItem_Int16:	status = AlxParamItem_SetValInt16(&ctx->item, (int16_t)val);	break;
		case AlxParamItem_Int32:	status = AlxParamItem_SetValInt32(&ctx->item, (int32_t)val);	break;
		case AlxParamItem_Int64:	status = AlxParamItem_SetValInt64(&ctx->item, val);	break;
		case AlxParamItem_Bool:		status = AlxParamItem_SetValBool(&ctx->item, val != 0);	break;
		case AlxParamItem_Float:
		case AlxParamItem_Double:
		case AlxParamItem_Arr:
		case AlxParamItem_Str:
		default:					break;
	}
	return (int32_t)status;
}
int32_t AlxParamItemMetaTest_SetValF(AlxParamItemMetaTest_Ctx* ctx, double val)
{
	Alx_Status status;
	if (ctx->dataType == AlxParamItem_Float)
	{
		status = AlxParamItem_SetValFloat(&ctx->item, (float)val);
	}
	else
	{
		status = AlxParamItem_SetValDouble(&ctx->item, val);
	}
	return (int32_t)status;
}
void AlxParamItemMetaTest_SetValToDef(AlxParamItemMetaTest_Ctx* ctx)
{
	AlxParamItem_SetValToDef(&ctx->item);
}

#define ALX_PARAM_ITEM_META_ENUM_MAX 16
// the longest enum list any test declares; the context carries its own copy because
// AlxParamItem keeps the caller's pointer rather than copying the array

//------------------------------------------------------------------------------
// Parameter item - the enum list (ALX-1553)
//------------------------------------------------------------------------------
// An item may declare a LIST of the values it will accept, and then a set is
// checked against that list instead of against the minimum and the maximum.
// That is a second, separate gate, and it is where the module's only two static
// functions live: AlxParamItem_AreEnumArrValFromLowToHigh, which the constructor
// asks whether the list is ordered, and AlxParamItem_IsEnumOnList, which every
// set asks whether the value is a member.
//
// Worth knowing which of the two an elided build still runs. The ordering check
// is reached ONLY from inside ALX_PARAM_ITEM_ASSERT, so a module compiled with
// no assert form discards the call along with the expression and an out-of-order
// list is simply accepted. The membership check is not - it sits in the set path
// itself and runs in every build. This group compiles asserts ON, so both run
// here.
//
// The list crosses as int64_t or double for the same reason the values do, and
// is copied into the context because AlxParamItem keeps the caller's pointer
// rather than a copy of the array.

typedef struct
{
	AlxParamItem item;
	AlxParamItem_DataType dataType;
	union
	{
		uint8_t u8[ALX_PARAM_ITEM_META_ENUM_MAX];
		uint16_t u16[ALX_PARAM_ITEM_META_ENUM_MAX];
		uint32_t u32[ALX_PARAM_ITEM_META_ENUM_MAX];
		uint64_t u64[ALX_PARAM_ITEM_META_ENUM_MAX];
		int8_t i8[ALX_PARAM_ITEM_META_ENUM_MAX];
		int16_t i16[ALX_PARAM_ITEM_META_ENUM_MAX];
		int32_t i32[ALX_PARAM_ITEM_META_ENUM_MAX];
		int64_t i64[ALX_PARAM_ITEM_META_ENUM_MAX];
		float f32[ALX_PARAM_ITEM_META_ENUM_MAX];
		double f64[ALX_PARAM_ITEM_META_ENUM_MAX];
	} list;
} AlxParamItemEnumTest_Ctx;

AlxParamItemEnumTest_Ctx* AlxParamItemEnumTest_New(uint32_t dataType, const int64_t* listI, const double* listF, uint8_t listLen, int64_t defI, int64_t minI, int64_t maxI, double defF, double minF, double maxF, uint32_t outOfRange);
void AlxParamItemEnumTest_Delete(AlxParamItemEnumTest_Ctx* ctx);
bool AlxParamItemEnumTest_GetIsEnum(AlxParamItemEnumTest_Ctx* ctx);
uint8_t AlxParamItemEnumTest_GetEnumArrLen(AlxParamItemEnumTest_Ctx* ctx);
int64_t AlxParamItemEnumTest_GetEnumArrAtI(AlxParamItemEnumTest_Ctx* ctx, uint8_t index);
double AlxParamItemEnumTest_GetEnumArrAtF(AlxParamItemEnumTest_Ctx* ctx, uint8_t index);
int32_t AlxParamItemEnumTest_SetValI(AlxParamItemEnumTest_Ctx* ctx, int64_t val);
int32_t AlxParamItemEnumTest_SetValF(AlxParamItemEnumTest_Ctx* ctx, double val);
int64_t AlxParamItemEnumTest_GetValI(AlxParamItemEnumTest_Ctx* ctx);
double AlxParamItemEnumTest_GetValF(AlxParamItemEnumTest_Ctx* ctx);

AlxParamItemEnumTest_Ctx* AlxParamItemEnumTest_New(uint32_t dataType, const int64_t* listI, const double* listF, uint8_t listLen, int64_t defI, int64_t minI, int64_t maxI, double defF, double minF, double maxF, uint32_t outOfRange)
{
	if (listLen > ALX_PARAM_ITEM_META_ENUM_MAX)
	{
		return NULL;
	}
	AlxParamItemEnumTest_Ctx* ctx = calloc(1, sizeof(AlxParamItemEnumTest_Ctx));
	if (ctx == NULL)
	{
		return NULL;
	}
	ctx->dataType = (AlxParamItem_DataType)dataType;

	for (uint8_t i = 0; i < listLen; i++)
	{
		switch (ctx->dataType)
		{
			case AlxParamItem_Uint8:	ctx->list.u8[i] = (uint8_t)listI[i];		break;
			case AlxParamItem_Uint16:	ctx->list.u16[i] = (uint16_t)listI[i];		break;
			case AlxParamItem_Uint32:	ctx->list.u32[i] = (uint32_t)listI[i];		break;
			case AlxParamItem_Uint64:	ctx->list.u64[i] = (uint64_t)listI[i];		break;
			case AlxParamItem_Int8:		ctx->list.i8[i] = (int8_t)listI[i];			break;
			case AlxParamItem_Int16:	ctx->list.i16[i] = (int16_t)listI[i];		break;
			case AlxParamItem_Int32:	ctx->list.i32[i] = (int32_t)listI[i];		break;
			case AlxParamItem_Int64:	ctx->list.i64[i] = listI[i];				break;
			case AlxParamItem_Float:	ctx->list.f32[i] = (float)listF[i];			break;
			case AlxParamItem_Double:	ctx->list.f64[i] = listF[i];				break;
			case AlxParamItem_Bool:
			case AlxParamItem_Arr:
			case AlxParamItem_Str:
			default:
				free(ctx);
				return NULL;
		}
	}

	AlxParamItem_ValOutOfRangeHandle handle = (AlxParamItem_ValOutOfRangeHandle)outOfRange;
	switch (ctx->dataType)
	{
		case AlxParamItem_Uint8:
			AlxParamItem_CtorUint8(&ctx->item, NULL, AlxParamItem_Param, "ENUM_ITEM", 0, "GRP", 0,
				(uint8_t)defI, (uint8_t)minI, (uint8_t)maxI, handle, true, ctx->list.u8, listLen, "", false);
			break;
		case AlxParamItem_Uint16:
			AlxParamItem_CtorUint16(&ctx->item, NULL, AlxParamItem_Param, "ENUM_ITEM", 0, "GRP", 0,
				(uint16_t)defI, (uint16_t)minI, (uint16_t)maxI, handle, true, ctx->list.u16, listLen, "", false);
			break;
		case AlxParamItem_Uint32:
			AlxParamItem_CtorUint32(&ctx->item, NULL, AlxParamItem_Param, "ENUM_ITEM", 0, "GRP", 0,
				(uint32_t)defI, (uint32_t)minI, (uint32_t)maxI, handle, true, ctx->list.u32, listLen, "", false);
			break;
		case AlxParamItem_Uint64:
			AlxParamItem_CtorUint64(&ctx->item, NULL, AlxParamItem_Param, "ENUM_ITEM", 0, "GRP", 0,
				(uint64_t)defI, (uint64_t)minI, (uint64_t)maxI, handle, true, ctx->list.u64, listLen, "", false);
			break;
		case AlxParamItem_Int8:
			AlxParamItem_CtorInt8(&ctx->item, NULL, AlxParamItem_Param, "ENUM_ITEM", 0, "GRP", 0,
				(int8_t)defI, (int8_t)minI, (int8_t)maxI, handle, true, ctx->list.i8, listLen, "", false);
			break;
		case AlxParamItem_Int16:
			AlxParamItem_CtorInt16(&ctx->item, NULL, AlxParamItem_Param, "ENUM_ITEM", 0, "GRP", 0,
				(int16_t)defI, (int16_t)minI, (int16_t)maxI, handle, true, ctx->list.i16, listLen, "", false);
			break;
		case AlxParamItem_Int32:
			AlxParamItem_CtorInt32(&ctx->item, NULL, AlxParamItem_Param, "ENUM_ITEM", 0, "GRP", 0,
				(int32_t)defI, (int32_t)minI, (int32_t)maxI, handle, true, ctx->list.i32, listLen, "", false);
			break;
		case AlxParamItem_Int64:
			AlxParamItem_CtorInt64(&ctx->item, NULL, AlxParamItem_Param, "ENUM_ITEM", 0, "GRP", 0,
				defI, minI, maxI, handle, true, ctx->list.i64, listLen, "", false);
			break;
		case AlxParamItem_Float:
			AlxParamItem_CtorFloat(&ctx->item, NULL, AlxParamItem_Param, "ENUM_ITEM", 0, "GRP", 0,
				(float)defF, (float)minF, (float)maxF, handle, true, ctx->list.f32, listLen, "", false);
			break;
		case AlxParamItem_Double:
			AlxParamItem_CtorDouble(&ctx->item, NULL, AlxParamItem_Param, "ENUM_ITEM", 0, "GRP", 0,
				defF, minF, maxF, handle, true, ctx->list.f64, listLen, "", false);
			break;
		case AlxParamItem_Bool:
		case AlxParamItem_Arr:
		case AlxParamItem_Str:
		default:
			free(ctx);
			return NULL;
	}
	return ctx;
}

void AlxParamItemEnumTest_Delete(AlxParamItemEnumTest_Ctx* ctx)
{
	free(ctx);
}

bool AlxParamItemEnumTest_GetIsEnum(AlxParamItemEnumTest_Ctx* ctx)
{
	return AlxParamItem_GetIsEnum(&ctx->item);
}

uint8_t AlxParamItemEnumTest_GetEnumArrLen(AlxParamItemEnumTest_Ctx* ctx)
{
	uint8_t len = 0;
	uint8_t* u8 = NULL;
	uint16_t* u16 = NULL;
	uint32_t* u32 = NULL;
	uint64_t* u64 = NULL;
	int8_t* i8 = NULL;
	int16_t* i16 = NULL;
	int32_t* i32 = NULL;
	int64_t* i64 = NULL;
	float* f32 = NULL;
	double* f64 = NULL;
	switch (ctx->dataType)
	{
		case AlxParamItem_Uint8:	AlxParamItem_GetEnumArrUint8(&ctx->item, &u8, &len);	break;
		case AlxParamItem_Uint16:	AlxParamItem_GetEnumArrUint16(&ctx->item, &u16, &len);	break;
		case AlxParamItem_Uint32:	AlxParamItem_GetEnumArrUint32(&ctx->item, &u32, &len);	break;
		case AlxParamItem_Uint64:	AlxParamItem_GetEnumArrUint64(&ctx->item, &u64, &len);	break;
		case AlxParamItem_Int8:		AlxParamItem_GetEnumArrInt8(&ctx->item, &i8, &len);		break;
		case AlxParamItem_Int16:	AlxParamItem_GetEnumArrInt16(&ctx->item, &i16, &len);	break;
		case AlxParamItem_Int32:	AlxParamItem_GetEnumArrInt32(&ctx->item, &i32, &len);	break;
		case AlxParamItem_Int64:	AlxParamItem_GetEnumArrInt64(&ctx->item, &i64, &len);	break;
		case AlxParamItem_Float:	AlxParamItem_GetEnumArrFloat(&ctx->item, &f32, &len);	break;
		case AlxParamItem_Double:	AlxParamItem_GetEnumArrDouble(&ctx->item, &f64, &len);	break;
		case AlxParamItem_Bool:
		case AlxParamItem_Arr:
		case AlxParamItem_Str:
		default:															break;
	}
	return len;
}

int64_t AlxParamItemEnumTest_GetEnumArrAtI(AlxParamItemEnumTest_Ctx* ctx, uint8_t index)
{
	uint8_t len = 0;
	uint8_t* u8 = NULL;
	uint16_t* u16 = NULL;
	uint32_t* u32 = NULL;
	uint64_t* u64 = NULL;
	int8_t* i8 = NULL;
	int16_t* i16 = NULL;
	int32_t* i32 = NULL;
	int64_t* i64 = NULL;
	switch (ctx->dataType)
	{
		case AlxParamItem_Uint8:	AlxParamItem_GetEnumArrUint8(&ctx->item, &u8, &len);	return (int64_t)u8[index];
		case AlxParamItem_Uint16:	AlxParamItem_GetEnumArrUint16(&ctx->item, &u16, &len);	return (int64_t)u16[index];
		case AlxParamItem_Uint32:	AlxParamItem_GetEnumArrUint32(&ctx->item, &u32, &len);	return (int64_t)u32[index];
		case AlxParamItem_Uint64:	AlxParamItem_GetEnumArrUint64(&ctx->item, &u64, &len);	return (int64_t)u64[index];
		case AlxParamItem_Int8:		AlxParamItem_GetEnumArrInt8(&ctx->item, &i8, &len);		return (int64_t)i8[index];
		case AlxParamItem_Int16:	AlxParamItem_GetEnumArrInt16(&ctx->item, &i16, &len);	return (int64_t)i16[index];
		case AlxParamItem_Int32:	AlxParamItem_GetEnumArrInt32(&ctx->item, &i32, &len);	return (int64_t)i32[index];
		case AlxParamItem_Int64:	AlxParamItem_GetEnumArrInt64(&ctx->item, &i64, &len);	return i64[index];
		case AlxParamItem_Float:
		case AlxParamItem_Double:
		case AlxParamItem_Bool:
		case AlxParamItem_Arr:
		case AlxParamItem_Str:
		default:																			return 0;
	}
}

double AlxParamItemEnumTest_GetEnumArrAtF(AlxParamItemEnumTest_Ctx* ctx, uint8_t index)
{
	uint8_t len = 0;
	float* f32 = NULL;
	double* f64 = NULL;
	if (ctx->dataType == AlxParamItem_Float)
	{
		AlxParamItem_GetEnumArrFloat(&ctx->item, &f32, &len);
		return (double)f32[index];
	}
	AlxParamItem_GetEnumArrDouble(&ctx->item, &f64, &len);
	return f64[index];
}

int32_t AlxParamItemEnumTest_SetValI(AlxParamItemEnumTest_Ctx* ctx, int64_t val)
{
	Alx_Status status = Alx_Err;
	switch (ctx->dataType)
	{
		case AlxParamItem_Uint8:	status = AlxParamItem_SetValUint8(&ctx->item, (uint8_t)val);	break;
		case AlxParamItem_Uint16:status = AlxParamItem_SetValUint16(&ctx->item, (uint16_t)val);	break;
		case AlxParamItem_Uint32:status = AlxParamItem_SetValUint32(&ctx->item, (uint32_t)val);	break;
		case AlxParamItem_Uint64:status = AlxParamItem_SetValUint64(&ctx->item, (uint64_t)val);	break;
		case AlxParamItem_Int8:	status = AlxParamItem_SetValInt8(&ctx->item, (int8_t)val);	break;
		case AlxParamItem_Int16:	status = AlxParamItem_SetValInt16(&ctx->item, (int16_t)val);	break;
		case AlxParamItem_Int32:	status = AlxParamItem_SetValInt32(&ctx->item, (int32_t)val);	break;
		case AlxParamItem_Int64:	status = AlxParamItem_SetValInt64(&ctx->item, val);	break;
		case AlxParamItem_Float:
		case AlxParamItem_Double:
		case AlxParamItem_Bool:
		case AlxParamItem_Arr:
		case AlxParamItem_Str:
		default:					break;
	}
	return (int32_t)status;
}

int32_t AlxParamItemEnumTest_SetValF(AlxParamItemEnumTest_Ctx* ctx, double val)
{
	Alx_Status status;
	if (ctx->dataType == AlxParamItem_Float)
	{
		status = AlxParamItem_SetValFloat(&ctx->item, (float)val);
	}
	else
	{
		status = AlxParamItem_SetValDouble(&ctx->item, val);
	}
	return (int32_t)status;
}

int64_t AlxParamItemEnumTest_GetValI(AlxParamItemEnumTest_Ctx* ctx)
{
	switch (ctx->dataType)
	{
		case AlxParamItem_Uint8:	return (int64_t)AlxParamItem_GetValUint8(&ctx->item);
		case AlxParamItem_Uint16:	return (int64_t)AlxParamItem_GetValUint16(&ctx->item);
		case AlxParamItem_Uint32:	return (int64_t)AlxParamItem_GetValUint32(&ctx->item);
		case AlxParamItem_Uint64:	return (int64_t)AlxParamItem_GetValUint64(&ctx->item);
		case AlxParamItem_Int8:		return (int64_t)AlxParamItem_GetValInt8(&ctx->item);
		case AlxParamItem_Int16:	return (int64_t)AlxParamItem_GetValInt16(&ctx->item);
		case AlxParamItem_Int32:	return (int64_t)AlxParamItem_GetValInt32(&ctx->item);
		case AlxParamItem_Int64:	return AlxParamItem_GetValInt64(&ctx->item);
		case AlxParamItem_Float:
		case AlxParamItem_Double:
		case AlxParamItem_Bool:
		case AlxParamItem_Arr:
		case AlxParamItem_Str:
		default:					return 0;
	}
}

double AlxParamItemEnumTest_GetValF(AlxParamItemEnumTest_Ctx* ctx)
{
	if (ctx->dataType == AlxParamItem_Float)
	{
		return (double)AlxParamItem_GetValFloat(&ctx->item);
	}
	return AlxParamItem_GetValDouble(&ctx->item);
}

int32_t AlxMemSafeTest_Status_ErrEnum(void);
int32_t AlxMemSafeTest_Status_ErrConv(void);

int32_t AlxMemSafeTest_Status_ErrEnum(void)					{ return (int32_t)AlxParamItem_ErrEnum; }
int32_t AlxMemSafeTest_Status_ErrConv(void)					{ return (int32_t)AlxParamItem_ErrConv; }


//------------------------------------------------------------------------------
// Parameter item - the two types that carry a buffer (ALX-1553)
//------------------------------------------------------------------------------
// Arr and Str are the last of alxParamItem's 105 functions that no test reached,
// and they are shaped unlike the scalars: the caller supplies the storage. Arr
// takes THREE buffers - the live value, the default, and the length they share -
// and Str takes one buffer plus a length that counts the null terminator.
//
// That is the whole reason they need a shim of their own rather than another
// case in the scalar one: a standalone item of these types is not a value, it is
// a value plus the memory the caller lent it, and who owns that memory is the
// part worth proving. The context owns it here, so a test cannot accidentally
// prove something about a Python buffer that has already been freed.

#define ALX_PARAM_ITEM_BUFF_MAX 64

// AlxParamItem keeps the CALLER's pointer for key, groupKey and valUnit - it does not copy
// them. In a product they are string literals and that is free; a test that hands over a
// temporary is reading freed memory the moment the call returns, and it reads correctly for
// exactly as long as nothing reuses the page. So every context here owns its own strings.
typedef struct
{
	AlxParamItem item;
	AlxParamItem_DataType dataType;
	char key[ALX_PARAM_ITEM_BUFF_MAX];
	char unit[ALX_PARAM_ITEM_BUFF_MAX];
	uint8_t valBuff[ALX_PARAM_ITEM_BUFF_MAX];
	uint8_t valDefBuff[ALX_PARAM_ITEM_BUFF_MAX];
	uint8_t valDefSrc[ALX_PARAM_ITEM_BUFF_MAX];
	char strDef[ALX_PARAM_ITEM_BUFF_MAX];
} AlxParamItemBuffTest_Ctx;

AlxParamItemBuffTest_Ctx* AlxParamItemBuffTest_NewArr(const uint8_t* valDef, uint32_t valBuffLen, const char* key, const char* valUnit);
AlxParamItemBuffTest_Ctx* AlxParamItemBuffTest_NewStr(const char* valDef, uint32_t buffLen, const char* key, const char* valUnit);
void AlxParamItemBuffTest_Delete(AlxParamItemBuffTest_Ctx* ctx);
uint32_t AlxParamItemBuffTest_GetBuffLen(AlxParamItemBuffTest_Ctx* ctx);
uint32_t AlxParamItemBuffTest_GetValLen(AlxParamItemBuffTest_Ctx* ctx);
const char* AlxParamItemBuffTest_GetKey(AlxParamItemBuffTest_Ctx* ctx);
const char* AlxParamItemBuffTest_GetValUnit(AlxParamItemBuffTest_Ctx* ctx);
uint32_t AlxParamItemBuffTest_GetDataType(AlxParamItemBuffTest_Ctx* ctx);
void AlxParamItemBuffTest_GetValArr(AlxParamItemBuffTest_Ctx* ctx, uint8_t* out);
void AlxParamItemBuffTest_SetValArr(AlxParamItemBuffTest_Ctx* ctx, const uint8_t* val);
void AlxParamItemBuffTest_GetValDefArr(AlxParamItemBuffTest_Ctx* ctx, uint8_t* out);
int32_t AlxParamItemBuffTest_GetValStr(AlxParamItemBuffTest_Ctx* ctx, char* out, uint32_t maxLenWithNullTerm);
int32_t AlxParamItemBuffTest_SetValStr(AlxParamItemBuffTest_Ctx* ctx, const char* val);
const char* AlxParamItemBuffTest_GetValDefStr(AlxParamItemBuffTest_Ctx* ctx);
void AlxParamItemBuffTest_SetValToDef(AlxParamItemBuffTest_Ctx* ctx);

AlxParamItemBuffTest_Ctx* AlxParamItemBuffTest_NewArr(const uint8_t* valDef, uint32_t valBuffLen, const char* key, const char* valUnit)
{
	if (valBuffLen > ALX_PARAM_ITEM_BUFF_MAX)
	{
		return NULL;
	}
	AlxParamItemBuffTest_Ctx* ctx = calloc(1, sizeof(AlxParamItemBuffTest_Ctx));
	if (ctx == NULL)
	{
		return NULL;
	}
	ctx->dataType = AlxParamItem_Arr;
	memcpy(ctx->valDefSrc, valDef, valBuffLen);
	strncpy(ctx->key, key, ALX_PARAM_ITEM_BUFF_MAX - 1);
	strncpy(ctx->unit, valUnit, ALX_PARAM_ITEM_BUFF_MAX - 1);

	AlxParamItem_CtorArr(&ctx->item, NULL, AlxParamItem_Param, ctx->key, 0, "GRP", 0,
		ctx->valDefSrc, ctx->valBuff, ctx->valDefBuff, valBuffLen,
		AlxParamItem_Ignore, ctx->unit, false);
	return ctx;
}

AlxParamItemBuffTest_Ctx* AlxParamItemBuffTest_NewStr(const char* valDef, uint32_t buffLen, const char* key, const char* valUnit)
{
	if (buffLen > ALX_PARAM_ITEM_BUFF_MAX)
	{
		return NULL;
	}
	AlxParamItemBuffTest_Ctx* ctx = calloc(1, sizeof(AlxParamItemBuffTest_Ctx));
	if (ctx == NULL)
	{
		return NULL;
	}
	ctx->dataType = AlxParamItem_Str;
	strncpy(ctx->strDef, valDef, ALX_PARAM_ITEM_BUFF_MAX - 1);
	strncpy(ctx->key, key, ALX_PARAM_ITEM_BUFF_MAX - 1);
	strncpy(ctx->unit, valUnit, ALX_PARAM_ITEM_BUFF_MAX - 1);

	AlxParamItem_CtorStr(&ctx->item, NULL, AlxParamItem_Param, ctx->key, 0, "GRP", 0,
		ctx->strDef, AlxParamItem_Ignore, ctx->unit, false, ctx->valBuff, buffLen);
	return ctx;
}

void AlxParamItemBuffTest_Delete(AlxParamItemBuffTest_Ctx* ctx)
{
	free(ctx);
}

uint32_t AlxParamItemBuffTest_GetBuffLen(AlxParamItemBuffTest_Ctx* ctx)
{
	return AlxParamItem_GetBuffLen(&ctx->item);
}
uint32_t AlxParamItemBuffTest_GetValLen(AlxParamItemBuffTest_Ctx* ctx)
{
	return AlxParamItem_GetValLen(&ctx->item);
}
const char* AlxParamItemBuffTest_GetKey(AlxParamItemBuffTest_Ctx* ctx)
{
	return AlxParamItem_GetKey(&ctx->item);
}
const char* AlxParamItemBuffTest_GetValUnit(AlxParamItemBuffTest_Ctx* ctx)
{
	return AlxParamItem_GetValUnit(&ctx->item);
}
uint32_t AlxParamItemBuffTest_GetDataType(AlxParamItemBuffTest_Ctx* ctx)
{
	AlxParamItem_DataType value = AlxParamItem_GetDataType(&ctx->item);
	return (uint32_t)value;
}

void AlxParamItemBuffTest_GetValArr(AlxParamItemBuffTest_Ctx* ctx, uint8_t* out)
{
	AlxParamItem_GetValArr(&ctx->item, out);
}
void AlxParamItemBuffTest_SetValArr(AlxParamItemBuffTest_Ctx* ctx, const uint8_t* val)
{
	uint8_t scratch[ALX_PARAM_ITEM_BUFF_MAX];
	// an Arr item's length is valLen - AlxParamItem_CtorArr never sets buffLen for it
	memcpy(scratch, val, AlxParamItem_GetValLen(&ctx->item));
	AlxParamItem_SetValArr(&ctx->item, scratch);
}
void AlxParamItemBuffTest_GetValDefArr(AlxParamItemBuffTest_Ctx* ctx, uint8_t* out)
{
	void* ptr = AlxParamItem_GetValDefArr(&ctx->item);
	memcpy(out, ptr, AlxParamItem_GetValLen(&ctx->item));
}

int32_t AlxParamItemBuffTest_GetValStr(AlxParamItemBuffTest_Ctx* ctx, char* out, uint32_t maxLenWithNullTerm)
{
	Alx_Status status = AlxParamItem_GetValStr(&ctx->item, out, maxLenWithNullTerm);
	return (int32_t)status;
}
int32_t AlxParamItemBuffTest_SetValStr(AlxParamItemBuffTest_Ctx* ctx, const char* val)
{
	char scratch[ALX_PARAM_ITEM_BUFF_MAX];
	strncpy(scratch, val, ALX_PARAM_ITEM_BUFF_MAX - 1);
	scratch[ALX_PARAM_ITEM_BUFF_MAX - 1] = '\0';
	Alx_Status status = AlxParamItem_SetValStr(&ctx->item, scratch);
	return (int32_t)status;
}
const char* AlxParamItemBuffTest_GetValDefStr(AlxParamItemBuffTest_Ctx* ctx)
{
	return AlxParamItem_GetValDefStr(&ctx->item);
}
void AlxParamItemBuffTest_SetValToDef(AlxParamItemBuffTest_Ctx* ctx)
{
	AlxParamItem_SetValToDef(&ctx->item);
}
