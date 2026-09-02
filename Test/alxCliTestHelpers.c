/**
  ******************************************************************************
  * @file		alxCliTestHelpers.c
  * @brief		Auralix C Library - ALX CLI Module - PC test helpers
  * @copyright	Copyright (C) Auralix d.o.o. All rights reserved.
  *
  * Opaque-handle constructor for a complete Tier-2 CLI under test, wired the
  * SAME way the product wires it (tlTest_BringUp.c): fake serial port +
  * real AlxParamItem PRETTY_JSON_EN (valDef true, no KV store) + real
  * AlxParamMgmt over that one item + alxId = NULL (the guarded optional).
  * NVIC_SystemReset is shimmed here: it records a flag the tests can read.
  ******************************************************************************
  **/

#include "alxCli.h"

#include <stdlib.h>

// alxSerialPortFake.c test-control API
void AlxSerialPortFake_Register(AlxSerialPort* me);
void AlxSerialPortFake_Unregister(AlxSerialPort* me);

// CMSIS shim (declaration for alxCli.c comes from Test/alxConfig.h on PC)
void NVIC_SystemReset(void);

#define ALX_CLI_TEST_BUFF_LEN 1024
#define ALX_CLI_TEST_STR_VAL_BUFF_LEN 600	// Str param value buffer -> value <= 599 chars (longer than any stack array the old parser had)

// Param table under test: the product's bool + one Str + one Uint8, so every value kind the CLI formats is present
typedef struct
{
	AlxSerialPort port;
	AlxParamItem paramItems[3];		// [0] PRETTY_JSON_EN (bool), [1] STR_TEST (str), [2] UINT8_TEST (uint8, 0..100)
	uint8_t strValBuff[ALX_CLI_TEST_STR_VAL_BUFF_LEN];
	AlxParamMgmt paramMgmt;
	AlxCli cli;
	char buff[ALX_CLI_TEST_BUFF_LEN];
} AlxCliTest_Ctx;

// Exported test API
AlxCliTest_Ctx* AlxCliTest_New(void);
void AlxCliTest_Delete(AlxCliTest_Ctx* ctx);
int32_t AlxCliTest_Status_Ok(void);
void AlxCliTest_Handle(AlxCliTest_Ctx* ctx);
AlxSerialPort* AlxCliTest_Port(AlxCliTest_Ctx* ctx);
uint32_t AlxCliTest_GetBuffLen(void);
uint32_t AlxCliTest_GetStrValBuffLen(void);
bool AlxCliTest_WasResetRequested(void);
void AlxCliTest_ClearResetRequested(void);

static bool nvicResetRequested = false;

void NVIC_SystemReset(void)
{
	nvicResetRequested = true;	// on the device this never returns; tests just observe it
}

bool AlxCliTest_WasResetRequested(void)
{
	return nvicResetRequested;
}

void AlxCliTest_ClearResetRequested(void)
{
	nvicResetRequested = false;
}

uint32_t AlxCliTest_GetBuffLen(void)
{
	return ALX_CLI_TEST_BUFF_LEN;
}

uint32_t AlxCliTest_GetStrValBuffLen(void)
{
	return ALX_CLI_TEST_STR_VAL_BUFF_LEN;
}

int32_t AlxCliTest_Status_Ok(void)
{
	return (int32_t)Alx_Ok;
}

AlxCliTest_Ctx* AlxCliTest_New(void)
{
	AlxCliTest_Ctx* ctx = (AlxCliTest_Ctx*)calloc(1, sizeof(AlxCliTest_Ctx));
	if (ctx == NULL)
	{
		exit(1);	// test infrastructure - fail fast on OOM
	}

	AlxSerialPortFake_Register(&ctx->port);

	AlxParamItem_CtorBool
	(
		&ctx->paramItems[0],
		NULL,				// paramKvStore - none, same as the product's CLI items
		AlxParamItem_Param,
		"PRETTY_JSON_EN",
		0,					// id
		NULL,				// groupKey
		0,					// groupId
		true,				// valDef - pretty JSON, same as the product
		"",					// valUnit
		false				// valChangeTakesEffectAfterReset
	);

	AlxParamItem_CtorStr
	(
		&ctx->paramItems[1],
		NULL,				// paramKvStore
		AlxParamItem_Param,
		"STR_TEST",
		1,					// id
		NULL,				// groupKey
		0,					// groupId
		"",					// valDef
		AlxParamItem_Ignore,
		"",					// valUnit
		false,				// valChangeTakesEffectAfterReset
		ctx->strValBuff,
		sizeof(ctx->strValBuff)
	);

	AlxParamItem_CtorUint8
	(
		&ctx->paramItems[2],
		NULL,				// paramKvStore
		AlxParamItem_Param,
		"UINT8_TEST",
		2,					// id
		NULL,				// groupKey
		0,					// groupId
		7,					// valDef
		0,					// valMin
		100,				// valMax
		AlxParamItem_Ignore,
		false,				// isEnum
		NULL,				// enumArr
		0,					// enumArrLen
		"",					// valUnit
		false				// valChangeTakesEffectAfterReset
	);

	AlxParamMgmt_Ctor(&ctx->paramMgmt, ctx->paramItems, 3);

	AlxCli_Ctor
	(
		&ctx->cli,
		&ctx->port,
		NULL,				// alxId - the guarded optional, same as the product
		&ctx->paramMgmt,
		&ctx->paramItems[0],
		ctx->buff,
		sizeof(ctx->buff)
	);

	return ctx;
}

void AlxCliTest_Delete(AlxCliTest_Ctx* ctx)
{
	AlxSerialPortFake_Unregister(&ctx->port);
	free(ctx);
}

void AlxCliTest_Handle(AlxCliTest_Ctx* ctx)
{
	AlxCli_Handle(&ctx->cli);
}

AlxSerialPort* AlxCliTest_Port(AlxCliTest_Ctx* ctx)
{
	return &ctx->port;
}
