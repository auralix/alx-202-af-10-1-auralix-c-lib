/**
  ******************************************************************************
  * @file		alxMemSafeTestHelpers.c
  * @brief		Auralix C Library - ALX Memory Safe Module - PC test helpers
  * @copyright	Copyright (C) Auralix d.o.o. All rights reserved.
  *
  * Opaque-handle constructor for the complete safe-store chain under test,
  * wired the SAME way the product wires it (tlTest_BringUp.c, ALX-1513):
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

	// Same chain and the same numbers as the product (tlTest_BringUp.c, ALX-1513)
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

	// The product's five stored params: key, factory default, 0..100, out-of-range ignored
	static const char* const keys[ALX_MEM_SAFE_TEST_NUM_OF_ITEMS] = { "DRL_pct", "AFS_pct", "INPUT_DRL_1_pct", "INPUT_DRL_2_pct", "INPUT_AFS_pct" };
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
