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

typedef struct
{
	AlxSerialPort port;
	AlxParamItem prettyJsonEn;
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
		&ctx->prettyJsonEn,
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

	AlxParamMgmt_Ctor(&ctx->paramMgmt, &ctx->prettyJsonEn, 1);

	AlxCli_Ctor
	(
		&ctx->cli,
		&ctx->port,
		NULL,				// alxId - the guarded optional, same as the product
		&ctx->paramMgmt,
		&ctx->prettyJsonEn,
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
