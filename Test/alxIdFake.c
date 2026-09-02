/**
  ******************************************************************************
  * @file		alxIdFake.c
  * @brief		Auralix C Library - ALX ID Module - PC link-time fake
  * @copyright	Copyright (C) Auralix d.o.o. All rights reserved.
  *
  * Tier-2 fake, named by the FAKED module. The CLI under test is constructed
  * with alxId = NULL (the guarded optional, same as the product), so the id
  * command is disabled and none of these can ever run - they exist only to
  * satisfy the linker for alxCli.c's id-command branch.
  ******************************************************************************
  **/

#include "alxId.h"

#include <stdlib.h>

static const char* AlxIdFake_Unreachable(void)
{
	// Test harness bug: the id command ran although alxId is supposed to be NULL
	exit(1);
}

bool AlxId_GetFwIsBootUsed(AlxId* me)                { (void)me; AlxIdFake_Unreachable(); return false; }
const char* AlxId_GetFwArtf(AlxId* me)               { (void)me; return AlxIdFake_Unreachable(); }
const char* AlxId_GetFwName(AlxId* me)               { (void)me; return AlxIdFake_Unreachable(); }
const char* AlxId_GetFwVerStr(AlxId* me)             { (void)me; return AlxIdFake_Unreachable(); }
const char* AlxId_GetFwBinStr(AlxId* me)             { (void)me; return AlxIdFake_Unreachable(); }
const char* AlxId_GetFwBootArtf(AlxId* me)           { (void)me; return AlxIdFake_Unreachable(); }
const char* AlxId_GetFwBootName(AlxId* me)           { (void)me; return AlxIdFake_Unreachable(); }
const char* AlxId_GetFwBootVerStr(AlxId* me)         { (void)me; return AlxIdFake_Unreachable(); }
const char* AlxId_GetFwBootBinStr(AlxId* me)         { (void)me; return AlxIdFake_Unreachable(); }
const char* AlxId_GetHwPcbArtf(AlxId* me)            { (void)me; return AlxIdFake_Unreachable(); }
const char* AlxId_GetHwPcbName(AlxId* me)            { (void)me; return AlxIdFake_Unreachable(); }
const char* AlxId_GetHwPcbVerStr(AlxId* me)          { (void)me; return AlxIdFake_Unreachable(); }
const char* AlxId_GetHwBomArtf(AlxId* me)            { (void)me; return AlxIdFake_Unreachable(); }
const char* AlxId_GetHwBomName(AlxId* me)            { (void)me; return AlxIdFake_Unreachable(); }
const char* AlxId_GetHwBomVerStr(AlxId* me)          { (void)me; return AlxIdFake_Unreachable(); }
uint8_t AlxId_GetHwId(AlxId* me)                     { (void)me; AlxIdFake_Unreachable(); return 0; }
const char* AlxId_GetHwMcuUniqueIdStr(AlxId* me)     { (void)me; return AlxIdFake_Unreachable(); }
