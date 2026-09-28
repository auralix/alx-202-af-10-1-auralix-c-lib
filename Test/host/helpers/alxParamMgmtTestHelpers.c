/**
  ******************************************************************************
  * @file		alxParamMgmtTestHelpers.c
  * @brief		Auralix C Library - ALX Param Mgmt Module - PC Unit Test Helpers
  * @copyright	Copyright (C) Auralix d.o.o. All rights reserved.
  *
  * AlxParamMgmt is a view over an array of AlxParamItem that a product declares
  * itself, so a test needs such an array before it can ask the module anything.
  * This builds ONE fixed table - six items, two groups, five data types and
  * both a Param and a Var - and hands back the manager over it. Fixed rather
  * than configurable so the tests can name what they expect, and so a change
  * to the table shows up as a change to this file.
  *
  * Table, in index order:
  *   0  U8_A     id 10  Uint8   GRP_A/100  def 5      min 0     max 255
  *   1  U16_B    id 11  Uint16  GRP_A/100  def 1000   min 0     max 65535
  *   2  I32_C    id 12  Int32   GRP_B/200  def -7     min -100  max 100
  *   3  F_D      id 13  Float   GRP_B/200  def 1.5    min -10   max 10
  *   4  BOOL_E   id 14  Bool    GRP_A/100  def true
  *   5  VAR_F    id 15  Uint32  GRP_B/200  def 42     min 0     max 1000   (a Var, not a Param)
  ******************************************************************************
  **/

//******************************************************************************
// Includes
//******************************************************************************
#include "alxParamMgmt.h"
#include <stdlib.h>


//******************************************************************************
// Private variables
//******************************************************************************
#define ALX_PARAM_MGMT_TEST_NUM_OF_ITEMS 6


//******************************************************************************
// Prototypes - the DLL export surface (no separate header for test helpers)
//******************************************************************************
AlxParamMgmt* AlxParamMgmtTest_New(void);
void AlxParamMgmtTest_Delete(AlxParamMgmt* me);
uint32_t AlxParamMgmtTest_NumOfItems(void);
int32_t AlxParamMgmtTest_ParamType_Param(void);
int32_t AlxParamMgmtTest_ParamType_Var(void);
int32_t AlxParamMgmtTest_DataType_Uint8(void);
int32_t AlxParamMgmtTest_DataType_Uint16(void);
int32_t AlxParamMgmtTest_DataType_Uint32(void);
int32_t AlxParamMgmtTest_DataType_Int32(void);
int32_t AlxParamMgmtTest_DataType_Float(void);
int32_t AlxParamMgmtTest_DataType_Bool(void);
int32_t AlxParamMgmtTest_Status_Ok(void);
int32_t AlxParamMgmtTest_Status_Err(void);


//******************************************************************************
// Functions
//******************************************************************************
AlxParamMgmt* AlxParamMgmtTest_New(void)
{
	AlxParamMgmt* me = (AlxParamMgmt*)malloc(sizeof(AlxParamMgmt));
	AlxParamItem* items = (AlxParamItem*)calloc(ALX_PARAM_MGMT_TEST_NUM_OF_ITEMS, sizeof(AlxParamItem));
	if ((me == NULL) || (items == NULL))
	{
		free(me);
		free(items);
		return NULL;
	}

	AlxParamItem_CtorUint8(&items[0], NULL, AlxParamItem_Param, "U8_A", 10, "GRP_A", 100,
		5, 0, 255, AlxParamItem_Ignore, false, NULL, 0, "", false);
	AlxParamItem_CtorUint16(&items[1], NULL, AlxParamItem_Param, "U16_B", 11, "GRP_A", 100,
		1000, 0, 65535, AlxParamItem_Ignore, false, NULL, 0, "", false);
	AlxParamItem_CtorInt32(&items[2], NULL, AlxParamItem_Param, "I32_C", 12, "GRP_B", 200,
		-7, -100, 100, AlxParamItem_Ignore, false, NULL, 0, "", false);
	AlxParamItem_CtorFloat(&items[3], NULL, AlxParamItem_Param, "F_D", 13, "GRP_B", 200,
		1.5f, -10.f, 10.f, AlxParamItem_Ignore, false, NULL, 0, "", false);
	AlxParamItem_CtorBool(&items[4], NULL, AlxParamItem_Param, "BOOL_E", 14, "GRP_A", 100,
		true, "", false);
	AlxParamItem_CtorUint32(&items[5], NULL, AlxParamItem_Var, "VAR_F", 15, "GRP_B", 200,
		42, 0, 1000, AlxParamItem_Ignore, false, NULL, 0, "", false);

	AlxParamMgmt_Ctor(me, items, ALX_PARAM_MGMT_TEST_NUM_OF_ITEMS);
	return me;
}
void AlxParamMgmtTest_Delete(AlxParamMgmt* me)
{
	if (me != NULL)
	{
		free(me->paramItemArr);
	}
	free(me);
}
uint32_t AlxParamMgmtTest_NumOfItems(void)
{
	return ALX_PARAM_MGMT_TEST_NUM_OF_ITEMS;
}
int32_t AlxParamMgmtTest_ParamType_Param(void)
{
	return (int32_t)AlxParamItem_Param;
}
int32_t AlxParamMgmtTest_ParamType_Var(void)
{
	return (int32_t)AlxParamItem_Var;
}
int32_t AlxParamMgmtTest_DataType_Uint8(void)
{
	return (int32_t)AlxParamItem_Uint8;
}
int32_t AlxParamMgmtTest_DataType_Uint16(void)
{
	return (int32_t)AlxParamItem_Uint16;
}
int32_t AlxParamMgmtTest_DataType_Uint32(void)
{
	return (int32_t)AlxParamItem_Uint32;
}
int32_t AlxParamMgmtTest_DataType_Int32(void)
{
	return (int32_t)AlxParamItem_Int32;
}
int32_t AlxParamMgmtTest_DataType_Float(void)
{
	return (int32_t)AlxParamItem_Float;
}
int32_t AlxParamMgmtTest_DataType_Bool(void)
{
	return (int32_t)AlxParamItem_Bool;
}
int32_t AlxParamMgmtTest_Status_Ok(void)
{
	return (int32_t)Alx_Ok;
}
int32_t AlxParamMgmtTest_Status_Err(void)
{
	return (int32_t)Alx_Err;
}
