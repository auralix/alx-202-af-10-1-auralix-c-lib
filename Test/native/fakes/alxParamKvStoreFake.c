/**
  ******************************************************************************
  * @file		alxParamKvStoreFake.c
  * @brief		Auralix C Library - ALX Param KV Store Module - PC link-time fake
  * @copyright	Copyright (C) Auralix d.o.o. All rights reserved.
  *
  * Tier-2 fake, named by the FAKED module. alxParamItem.c references the
  * KV-store symbols even though the CLI and ParamMgmt test items are
  * constructed WITHOUT a store (paramKvStore = NULL, same as the product's CLI
  * items), so these bodies existed only to satisfy the linker: reaching one
  * meant a test had wandered into the persistence paths, and they failed loudly
  * with Alx_Err to say so.
  *
  * THAT GUARANTEE IS KEPT. The fake is DISABLED until a test arms it, and while
  * disabled it behaves exactly as it always did. A group that constructs its
  * items without a store therefore cannot tell the difference, and a test that
  * strays into persistence still gets Alx_Err.
  *
  * Armed, it is a small in-memory key/value store, which is what
  * AlxParamItem_LoadVal and AlxParamItem_StoreVal need to be reachable at all -
  * they were the last two of alxParamItem's 105 functions no test had called
  * (ALX-1553). Both assert paramKvStore != NULL, so a stand-in is the only way
  * in on a PC.
  ******************************************************************************
  **/

#include <string.h>

#include "alxParamKvStore.h"

#define ALX_PARAM_KV_STORE_FAKE_ENTRIES 16
#define ALX_PARAM_KV_STORE_FAKE_KEY_LEN 48
#define ALX_PARAM_KV_STORE_FAKE_VAL_LEN 64

typedef struct
{
	char key[ALX_PARAM_KV_STORE_FAKE_KEY_LEN];
	uint8_t data[ALX_PARAM_KV_STORE_FAKE_VAL_LEN];
	uint32_t len;
	bool used;
} AlxParamKvStoreFake_Entry;

static AlxParamKvStoreFake_Entry alxParamKvStoreFake_entries[ALX_PARAM_KV_STORE_FAKE_ENTRIES];
static bool alxParamKvStoreFake_enabled = false;
static bool alxParamKvStoreFake_failGet = false;
static bool alxParamKvStoreFake_failSet = false;
static uint32_t alxParamKvStoreFake_getCount = 0;
static uint32_t alxParamKvStoreFake_setCount = 0;

// Exported test controls
void AlxParamKvStoreFake_Reset(void);
void AlxParamKvStoreFake_Enable(bool enable);
void AlxParamKvStoreFake_FailGet(bool fail);
void AlxParamKvStoreFake_FailSet(bool fail);
uint32_t AlxParamKvStoreFake_GetCount(void);
uint32_t AlxParamKvStoreFake_SetCount(void);
uint32_t AlxParamKvStoreFake_NumOfKeys(void);
bool AlxParamKvStoreFake_Peek(const char* key, uint8_t* out, uint32_t* len);
void AlxParamKvStoreFake_Poke(const char* key, const uint8_t* in, uint32_t len);

static AlxParamKvStoreFake_Entry* AlxParamKvStoreFake_Find(const char* key)
{
	for (uint32_t i = 0; i < ALX_PARAM_KV_STORE_FAKE_ENTRIES; i++)
	{
		AlxParamKvStoreFake_Entry* entry = &alxParamKvStoreFake_entries[i];
		if (entry->used && strncmp(entry->key, key, ALX_PARAM_KV_STORE_FAKE_KEY_LEN - 1) == 0)
		{
			return entry;
		}
	}
	return NULL;
}

static AlxParamKvStoreFake_Entry* AlxParamKvStoreFake_Claim(const char* key)
{
	AlxParamKvStoreFake_Entry* entry = AlxParamKvStoreFake_Find(key);
	if (entry != NULL)
	{
		return entry;
	}
	for (uint32_t i = 0; i < ALX_PARAM_KV_STORE_FAKE_ENTRIES; i++)
	{
		if (alxParamKvStoreFake_entries[i].used == false)
		{
			entry = &alxParamKvStoreFake_entries[i];
			entry->used = true;
			strncpy(entry->key, key, ALX_PARAM_KV_STORE_FAKE_KEY_LEN - 1);
			entry->key[ALX_PARAM_KV_STORE_FAKE_KEY_LEN - 1] = '\0';
			return entry;
		}
	}
	return NULL;
}

void AlxParamKvStoreFake_Reset(void)
{
	memset(alxParamKvStoreFake_entries, 0, sizeof(alxParamKvStoreFake_entries));
	alxParamKvStoreFake_enabled = false;
	alxParamKvStoreFake_failGet = false;
	alxParamKvStoreFake_failSet = false;
	alxParamKvStoreFake_getCount = 0;
	alxParamKvStoreFake_setCount = 0;
}

void AlxParamKvStoreFake_Enable(bool enable)
{
	alxParamKvStoreFake_enabled = enable;
}
void AlxParamKvStoreFake_FailGet(bool fail)
{
	alxParamKvStoreFake_failGet = fail;
}
void AlxParamKvStoreFake_FailSet(bool fail)
{
	alxParamKvStoreFake_failSet = fail;
}
uint32_t AlxParamKvStoreFake_GetCount(void)
{
	return alxParamKvStoreFake_getCount;
}
uint32_t AlxParamKvStoreFake_SetCount(void)
{
	return alxParamKvStoreFake_setCount;
}
uint32_t AlxParamKvStoreFake_NumOfKeys(void)
{
	uint32_t n = 0;
	for (uint32_t i = 0; i < ALX_PARAM_KV_STORE_FAKE_ENTRIES; i++)
	{
		if (alxParamKvStoreFake_entries[i].used)
		{
			n++;
		}
	}
	return n;
}

bool AlxParamKvStoreFake_Peek(const char* key, uint8_t* out, uint32_t* len)
{
	AlxParamKvStoreFake_Entry* entry = AlxParamKvStoreFake_Find(key);
	if (entry == NULL)
	{
		*len = 0;
		return false;
	}
	memcpy(out, entry->data, entry->len);
	*len = entry->len;
	return true;
}

void AlxParamKvStoreFake_Poke(const char* key, const uint8_t* in, uint32_t len)
{
	AlxParamKvStoreFake_Entry* entry = AlxParamKvStoreFake_Claim(key);
	if (entry == NULL || len > ALX_PARAM_KV_STORE_FAKE_VAL_LEN)
	{
		return;
	}
	memcpy(entry->data, in, len);
	entry->len = len;
}

Alx_Status AlxParamKvStore_Get(AlxParamKvStore* me, const char* key, void* data, uint32_t lenMax, uint32_t* lenActual)
{
	(void)me;
	if (alxParamKvStoreFake_enabled == false)
	{
		// unchanged behaviour for every group that builds its items without a store
		(void)key; (void)data; (void)lenMax; (void)lenActual;
		return Alx_Err;	// persistence is out of scope for the PC CLI tests
	}

	alxParamKvStoreFake_getCount++;
	if (alxParamKvStoreFake_failGet)
	{
		return Alx_Err;
	}

	AlxParamKvStoreFake_Entry* entry = AlxParamKvStoreFake_Find(key);
	if (entry == NULL)
	{
		return Alx_Err;		// key not found, which is the caller's first-boot path
	}
	if (entry->len > lenMax)
	{
		return Alx_Err;
	}
	memcpy(data, entry->data, entry->len);
	*lenActual = entry->len;
	return Alx_Ok;
}

Alx_Status AlxParamKvStore_Set(AlxParamKvStore* me, const char* key, void* data, uint32_t len)
{
	(void)me;
	if (alxParamKvStoreFake_enabled == false)
	{
		// unchanged behaviour for every group that builds its items without a store
		(void)key; (void)data; (void)len;
		return Alx_Err;	// persistence is out of scope for the PC CLI tests
	}

	alxParamKvStoreFake_setCount++;
	if (alxParamKvStoreFake_failSet)
	{
		return Alx_Err;
	}
	if (len > ALX_PARAM_KV_STORE_FAKE_VAL_LEN)
	{
		return Alx_Err;
	}

	AlxParamKvStoreFake_Entry* entry = AlxParamKvStoreFake_Claim(key);
	if (entry == NULL)
	{
		return Alx_Err;		// the fake is full
	}
	memcpy(entry->data, data, len);
	entry->len = len;
	return Alx_Ok;
}
