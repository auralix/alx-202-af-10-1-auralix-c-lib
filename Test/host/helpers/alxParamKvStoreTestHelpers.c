/**
  ******************************************************************************
  * @file		alxParamKvStoreTestHelpers.c
  * @brief		Auralix C Library - ALX Parameter Key-Value Store - PC Unit Test Helpers
  * @copyright	Copyright (C) Auralix d.o.o. All rights reserved.
  *
  * The store keeps a pointer to a CALLER-owned file system, so the helper owns
  * both and hands a test one opaque handle. The file system itself is the
  * link-time fake in alxFsFake.c - this object exists only to give the store
  * something of the right type to point at.
  ******************************************************************************
  **/


//******************************************************************************
// Includes
//******************************************************************************
#include "alxParamKvStore.h"
#include <stdlib.h>
#include <string.h>


//******************************************************************************
// Private Types
//******************************************************************************
typedef struct
{
	AlxParamKvStore store;
	AlxFs fs;
} AlxParamKvStoreTest_Obj;


//******************************************************************************
// Prototypes - the DLL export surface (no separate header for test helpers)
//******************************************************************************
AlxParamKvStoreTest_Obj* AlxParamKvStoreTest_New(void);
void AlxParamKvStoreTest_Delete(AlxParamKvStoreTest_Obj* me);
AlxParamKvStore* AlxParamKvStoreTest_Store(AlxParamKvStoreTest_Obj* me);
bool AlxParamKvStoreTest_IsInit(AlxParamKvStoreTest_Obj* me);


//******************************************************************************
// Functions
//******************************************************************************
AlxParamKvStoreTest_Obj* AlxParamKvStoreTest_New(void)
{
	AlxParamKvStoreTest_Obj* me = (AlxParamKvStoreTest_Obj*)calloc(1, sizeof(AlxParamKvStoreTest_Obj));
	if (me == NULL) { return NULL; }

	AlxParamKvStore_Ctor(&me->store, &me->fs);
	return me;
}

void AlxParamKvStoreTest_Delete(AlxParamKvStoreTest_Obj* me)
{
	free(me);
}

AlxParamKvStore* AlxParamKvStoreTest_Store(AlxParamKvStoreTest_Obj* me)
{
	return &me->store;
}

bool AlxParamKvStoreTest_IsInit(AlxParamKvStoreTest_Obj* me)
{
	// The store's own idea of whether it is usable, which the error paths decide.
	return me->store.isInit;
}
