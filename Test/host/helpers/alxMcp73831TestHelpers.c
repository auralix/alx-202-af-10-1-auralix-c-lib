/**
  ******************************************************************************
  * @file		alxMcp73831TestHelpers.c
  * @brief		Auralix C Library - ALX MCP73831 Battery Charger - PC Unit Test Helpers
  * @copyright	Copyright (C) Auralix d.o.o. All rights reserved.
  *
  * The driver keeps a POINTER to its one status pin, so the helper owns the pin
  * and hands it to a test. The read mode - tri-state or binary - is the
  * constructor's choice, and a test makes one charger of each. The status
  * values are exported, so a test names them rather than restating numbers.
  ******************************************************************************
  **/

//******************************************************************************
// Includes
//******************************************************************************
#include "alxMcp73831.h"
#include <stdlib.h>
#include <string.h>


//******************************************************************************
// Private Types
//******************************************************************************
typedef struct
{
	AlxMcp73831 mcp;
	AlxIoPin stat;
} AlxMcp73831Test_Obj;


//******************************************************************************
// Prototypes - the DLL export surface (no separate header for test helpers)
//******************************************************************************
AlxMcp73831Test_Obj* AlxMcp73831Test_New(bool triStateRead);
void AlxMcp73831Test_Delete(AlxMcp73831Test_Obj* me);
AlxMcp73831* AlxMcp73831Test_Mcp(AlxMcp73831Test_Obj* me);
AlxIoPin* AlxMcp73831Test_StatPin(AlxMcp73831Test_Obj* me);
int32_t AlxMcp73831Test_St(const char* name);
int32_t AlxMcp73831Test_TriState(const char* name);


//******************************************************************************
// Functions
//******************************************************************************
AlxMcp73831Test_Obj* AlxMcp73831Test_New(bool triStateRead)
{
	AlxMcp73831Test_Obj* me = (AlxMcp73831Test_Obj*)calloc(1, sizeof(AlxMcp73831Test_Obj));
	if (me == NULL) { return NULL; }
	AlxMcp73831_Ctor(&me->mcp, &me->stat, triStateRead);
	return me;
}

void AlxMcp73831Test_Delete(AlxMcp73831Test_Obj* me)
{
	free(me);
}

AlxMcp73831* AlxMcp73831Test_Mcp(AlxMcp73831Test_Obj* me)		{ return &me->mcp; }
AlxIoPin* AlxMcp73831Test_StatPin(AlxMcp73831Test_Obj* me)		{ return &me->stat; }

int32_t AlxMcp73831Test_St(const char* name)
{
	if (strcmp(name, "Err") == 0) { return (int32_t)AlxMcp73831_St_Err; }
	if (strcmp(name, "Shutdown") == 0) { return (int32_t)AlxMcp73831_St_Shutdown; }
	if (strcmp(name, "Charging") == 0) { return (int32_t)AlxMcp73831_St_Charging; }
	if (strcmp(name, "Standby") == 0) { return (int32_t)AlxMcp73831_St_Standby; }
	return -1;
}

int32_t AlxMcp73831Test_TriState(const char* name)
{
	if (strcmp(name, "HiZ") == 0) { return (int32_t)AlxIoPin_TriState_HiZ; }
	if (strcmp(name, "Hi") == 0) { return (int32_t)AlxIoPin_TriState_Hi; }
	if (strcmp(name, "Lo") == 0) { return (int32_t)AlxIoPin_TriState_Lo; }
	if (strcmp(name, "Undefined") == 0) { return (int32_t)AlxIoPin_TriState_Undefined; }
	return -1;
}
