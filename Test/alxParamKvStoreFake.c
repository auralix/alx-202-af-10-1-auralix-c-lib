/**
  ******************************************************************************
  * @file		alxParamKvStoreFake.c
  * @brief		Auralix C Library - ALX Param KV Store Module - PC link-time fake
  * @copyright	Copyright (C) Auralix d.o.o. All rights reserved.
  *
  * Tier-2 fake, named by the FAKED module. alxParamItem.c references the
  * KV-store symbols even though the CLI test items are constructed WITHOUT a
  * store (paramKvStore = NULL, same as the product's CLI items), so these
  * bodies exist only to satisfy the linker. Reaching one means a test
  * wandered into the persistence paths - fail loudly with Alx_Err.
  ******************************************************************************
  **/

#include "alxParamKvStore.h"

Alx_Status AlxParamKvStore_Get(AlxParamKvStore* me, const char* key, void* data, uint32_t lenMax, uint32_t* lenActual)
{
	(void)me; (void)key; (void)data; (void)lenMax; (void)lenActual;
	return Alx_Err;	// persistence is out of scope for the PC CLI tests
}

Alx_Status AlxParamKvStore_Set(AlxParamKvStore* me, const char* key, void* data, uint32_t len)
{
	(void)me; (void)key; (void)data; (void)len;
	return Alx_Err;	// persistence is out of scope for the PC CLI tests
}
