/**
  ******************************************************************************
  * @file		alxIdFake.c
  * @brief		Auralix C Library - ALX ID Module - PC Unit Test Fake
  * @copyright	Copyright (C) Auralix d.o.o. All rights reserved.
  *
  * A link-time fake of the identification module, named after the module it
  * fakes. The real module reads the compiler, the CMSIS version and the MCU's
  * unique ID out of the build and the silicon, so on a PC it would answer about
  * the PC. This one answers what the constructor was given - the firmware's
  * artefact and name, and the first known hardware instance - and neutral
  * values for everything the silicon would supply: what a test asserts about a
  * banner or a command line is never the identity, it is the shape around it.
  *
  * The hardware ID is the one thing tests steer, because a product's
  * configuration branches on it: AlxIdFake_SetHwId, before Init.
  *
  * A caller that constructs its module with alxId = NULL, as the CLI's guarded
  * optional allows, never reaches any of this; the file then only satisfies the
  * linker for the id command's branch.
  ******************************************************************************
  **/

//******************************************************************************
// Includes
//******************************************************************************
#include "alxId.h"


//******************************************************************************
// Private Variables
//******************************************************************************
static const char* alxIdFake_fwArtf = "";
static const char* alxIdFake_fwName = "";
static uint8_t alxIdFake_hwId;


//******************************************************************************
// Prototypes - the DLL export surface (no separate header for test fakes)
//******************************************************************************
void AlxIdFake_Reset(void);
void AlxIdFake_SetHwId(uint8_t hwId);


//******************************************************************************
// The fake's own controls
//******************************************************************************
void AlxIdFake_Reset(void)
{
	alxIdFake_fwArtf = "";
	alxIdFake_fwName = "";
	alxIdFake_hwId = 0;
}

void AlxIdFake_SetHwId(uint8_t hwId)
{
	alxIdFake_hwId = hwId;
}


//******************************************************************************
// The faked module's own contract
//******************************************************************************
void AlxId_Ctor
(
	AlxId* me,
	const char* fwArtf,
	const char* fwName,
	uint8_t fwVerMajor,
	uint8_t fwVerMinor,
	uint8_t fwVerPatch,
	bool fwIsBuildJobUsed,
	uint32_t fwBuildDateComp,
	bool fwIsBootUsed,
	uint32_t fwBootIdAddr,
	AlxId_HwInstance* hwInstanceKnownArr,
	uint8_t hwInstanceKnownArrLen,
	uint8_t* hwInstanceHwIdSupportedArr,
	uint8_t hwInstanceHwIdSupportedArrLen,
	AlxIoPin** hwIdIoPinArr,
	uint8_t hwIdIoPinArrLen,
	const char* hwMcuName
)
{
	(void)fwVerMajor; (void)fwVerMinor; (void)fwVerPatch; (void)fwIsBuildJobUsed;
	(void)fwBuildDateComp; (void)fwIsBootUsed; (void)fwBootIdAddr; (void)hwMcuName;
	alxIdFake_fwArtf = fwArtf;
	alxIdFake_fwName = fwName;

	// A product may read some of these straight out of the structure rather than through a
	// getter, so the fake fills the fields the real constructor fills, not just the answers.
	me->hw.instanceKnownArr = hwInstanceKnownArr;
	me->hw.instanceKnownArrLen = hwInstanceKnownArrLen;
	me->hw.instanceHwIdSupportedArr = hwInstanceHwIdSupportedArr;
	me->hw.instanceHwIdSupportedArrLen = hwInstanceHwIdSupportedArrLen;
	me->hw.idIoPinArr = hwIdIoPinArr;
	me->hw.idIoPinArrLen = hwIdIoPinArrLen;
	if (hwInstanceKnownArrLen > 0) { me->hw.instance = hwInstanceKnownArr[0]; }
}

void AlxId_Init(AlxId* me)
{
	(void)me;
}

void AlxId_Trace(AlxId* me)
{
	(void)me;
}

const char* AlxId_GetFwArtf(AlxId* me)				{ (void)me; return alxIdFake_fwArtf; }
const char* AlxId_GetFwName(AlxId* me)				{ (void)me; return alxIdFake_fwName; }
const char* AlxId_GetFwVerStr(AlxId* me)			{ (void)me; return "0.0.0"; }
const char* AlxId_GetFwBinStr(AlxId* me)			{ (void)me; return "0000000000_HOST_0-0-0_0000000.bin"; }
bool AlxId_GetFwIsBootUsed(AlxId* me)				{ (void)me; return false; }
const char* AlxId_GetFwBootArtf(AlxId* me)			{ (void)me; return ""; }
const char* AlxId_GetFwBootName(AlxId* me)			{ (void)me; return ""; }
const char* AlxId_GetFwBootVerStr(AlxId* me)		{ (void)me; return "0.0.0"; }
const char* AlxId_GetFwBootBinStr(AlxId* me)		{ (void)me; return "0000000000_HOST_Boot_0-0-0_0000000.bin"; }
uint8_t AlxId_GetHwId(AlxId* me)					{ (void)me; return alxIdFake_hwId; }
const char* AlxId_GetHwPcbArtf(AlxId* me)			{ (void)me; return ""; }
const char* AlxId_GetHwPcbName(AlxId* me)			{ (void)me; return ""; }
const char* AlxId_GetHwPcbVerStr(AlxId* me)			{ (void)me; return "0.0.0"; }
const char* AlxId_GetHwBomArtf(AlxId* me)			{ (void)me; return ""; }
const char* AlxId_GetHwBomName(AlxId* me)			{ (void)me; return ""; }
const char* AlxId_GetHwBomVerStr(AlxId* me)			{ (void)me; return "0.0.0"; }
const char* AlxId_GetHwMcuUniqueIdStr(AlxId* me)	{ (void)me; return "000000000000000000000000"; }
