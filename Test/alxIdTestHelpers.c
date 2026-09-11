/**
  ******************************************************************************
  * @file		alxIdTestHelpers.c
  * @brief		Auralix C Library - ALX ID Module - PC Unit Test Helpers
  * @copyright	Copyright (C) Auralix d.o.o. All rights reserved.
  *
  * The identity block is 1464 bytes of nested structs and the constructor takes
  * a ~350 byte AlxId_HwInstance BY VALUE, so nothing about this module can be
  * driven from Python without mirroring its layout - which this suite does not
  * do. Everything the tests need to build, poison or inspect is here in C, and
  * the Python side only ever holds an opaque AlxId*.
  *
  * Construction is two phase on purpose. AlxIdTest_Alloc hands back a poisoned
  * object; the test then fills the arrays the constructor will be given - the
  * identification pins, the known instances, the supported ids, the by-value
  * instance - and only then calls a Ctor. That order is what lets the arrays be
  * INPUTS: the real constructor stores pointers to them and AlxId_Init reads
  * their contents, so a test that wants an instance the board does not know has
  * to be able to say so before Init runs.
  *
  * The poison byte is the caller's, and it is not decoration. A fresh heap is
  * mostly zeros, and zero is a legal value for nearly every field here, so a
  * constructor that forgot one would leave a plausible answer behind. Filled
  * with 0xFF first, a forgotten field reads as 255 or as a string of garbage.
  * The zero fill is kept available as the counterpart, so a test can pin what
  * the difference actually is.
  *
  * The build macros are exported rather than restated. alxBuild_GENERATED.h is
  * rewritten by alxBuild.py on every VisualGDB build, so a test that hardcoded
  * this week's hash would go red on somebody else's build of the firmware; a
  * test compares what the module stored against what the module was compiled
  * with, which is a claim that stays true. ALX_BUILD_DATE_COMP is the one
  * exception and it lives in alxIdTestDateComp.c - see that file.
  *
  * The bootloader block is read from a FIXED ADDRESS: the module keeps it as a
  * uint32_t and casts it straight to void*, so on this x64 host a malloc'd
  * buffer would be truncated to a wild pointer. AlxIdTest_FlashAlloc reserves a
  * page below 4 GB instead, which is the only way a test can put a blob where
  * the constructor will look for it.
  ******************************************************************************
  **/

//******************************************************************************
// Includes
//******************************************************************************
#include "alxId.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <stdlib.h>
#include <string.h>


//******************************************************************************
// Private variables
//******************************************************************************
#define ALX_ID_TEST_PIN_ARR_LEN 8			// more than ALX_ID_HW_ID_IO_PIN_ARR_LEN, so a test that
											// hands the module too many pins still points at real ones
#define ALX_ID_TEST_INSTANCE_ARR_LEN 8
#define ALX_ID_TEST_FLASH_ADDR 0x10000000u	// below 4 GB, so the uint32_t address survives the cast
#define ALX_ID_TEST_FLASH_LEN 0x1000u

typedef struct
{
	AlxId id;			// FIRST: the handle a test holds is &obj->id, which is also the malloc'd
						// pointer, so AlxIdTest_Delete can free what it was given
	AlxIoPin pin[ALX_ID_TEST_PIN_ARR_LEN];
	AlxIoPin* pinArr[ALX_ID_TEST_PIN_ARR_LEN];
	AlxId_HwInstance instance;
	AlxId_HwInstance knownArr[ALX_ID_TEST_INSTANCE_ARR_LEN];
	uint8_t supportedArr[ALX_ID_TEST_INSTANCE_ARR_LEN];
} AlxIdTest_Obj;


//******************************************************************************
// Prototypes - the DLL export surface (no separate header for test helpers)
//******************************************************************************
AlxId* AlxIdTest_Alloc(uint8_t poison);
void AlxIdTest_Delete(AlxId* me);
AlxIoPin* AlxIdTest_Pin(AlxId* me, uint8_t i);
AlxId_HwInstance* AlxIdTest_Instance(AlxId* me);
AlxId_HwInstance* AlxIdTest_Known(AlxId* me, uint8_t i);
void AlxIdTest_SetSupported(AlxId* me, uint8_t i, uint8_t hwId);
void AlxIdTest_InstanceSetPcb(AlxId_HwInstance* me, uint8_t id, const char* artf, const char* name, uint8_t verMajor, uint8_t verMinor, uint8_t verPatch, uint32_t verDate);
void AlxIdTest_InstanceSetBom(AlxId_HwInstance* me, const char* artf, const char* name, uint8_t verMajor, uint8_t verMinor, uint8_t verPatch, uint32_t verDate);
void AlxIdTest_Ctor(AlxId* me, const char* fwArtf, const char* fwName, uint8_t fwVerMajor, uint8_t fwVerMinor, uint8_t fwVerPatch, bool fwIsBuildJobUsed, uint32_t fwBuildDateComp, bool fwIsBootUsed, uint32_t fwBootIdAddr, uint8_t hwInstanceKnownArrLen, uint8_t hwInstanceHwIdSupportedArrLen, uint8_t hwIdIoPinArrLen, const char* hwMcuName);
void AlxIdTest_CtorNoHwId(AlxId* me, const char* fwArtf, const char* fwName, uint8_t fwVerMajor, uint8_t fwVerMinor, uint8_t fwVerPatch, bool fwIsBuildJobUsed, uint32_t fwBuildDateComp, bool fwIsBootUsed, uint32_t fwBootIdAddr, const char* hwMcuName);
bool AlxIdTest_IsInit(AlxId* me);
bool AlxIdTest_IsHwIdUsed(AlxId* me);
uint8_t AlxIdTest_IdIoPinState(AlxId* me, uint8_t i);
uint8_t AlxIdTest_CalcHwId(AlxId* me, uint8_t hwIdIoPinArrLen);
uint8_t AlxIdTest_TriState_HiZ(void);
uint8_t AlxIdTest_TriState_Hi(void);
uint8_t AlxIdTest_TriState_Lo(void);
uint8_t AlxIdTest_TriState_Undefined(void);
uint32_t AlxIdTest_FlashAlloc(void);
void AlxIdTest_FlashFree(uint32_t addr);
void AlxIdTest_FlashFill(uint32_t addr, uint8_t byte, uint32_t len);
void AlxIdTest_BootBlobWrite(uint32_t addr, uint32_t magicNum, uint32_t ver, const char* artf, const char* name, uint8_t verMajor, uint8_t verMinor, uint8_t verPatch, bool isBuildJobUsed, uint32_t buildDate, uint32_t buildDateComp, const char* buildName, const char* hash, const char* hashShort, uint32_t hashShortUint32, uint16_t crc);
bool AlxIdTest_BootBlobFillField(uint32_t addr, const char* field, uint8_t byte, uint32_t len);
uint32_t AlxIdTest_BootBlobLen(void);
uint32_t AlxIdTest_BootBlobMagicNum(void);
uint32_t AlxIdTest_BootBlobVer(void);
const char* AlxIdTest_BuildName(void);
uint32_t AlxIdTest_BuildDate(void);
uint32_t AlxIdTest_BuildNum(void);
uint32_t AlxIdTest_BuildRev(void);
const char* AlxIdTest_BuildHash(void);
const char* AlxIdTest_BuildHashShort(void);
uint32_t AlxIdTest_BuildHashShortUint32(void);


//******************************************************************************
// Private functions
//******************************************************************************
static AlxIdTest_Obj* AlxIdTest_Object(AlxId* me)
{
	return (AlxIdTest_Obj*)me;
}
static void AlxIdTest_SetField(char* field, uint32_t fieldLen, const char* val)
{
	// Bounded and zero filled, so a short name leaves NUL bytes and not whatever was there before.
	// A test that wants a field with NO terminator writes it with AlxIdTest_BootBlobFillField.
	uint32_t len = (uint32_t)strlen(val);
	if (len >= fieldLen) { len = fieldLen - 1; }
	memset(field, 0, fieldLen);
	memcpy(field, val, len);
}


//******************************************************************************
// The object
//******************************************************************************
AlxId* AlxIdTest_Alloc(uint8_t poison)
{
	AlxIdTest_Obj* obj = (AlxIdTest_Obj*)malloc(sizeof(AlxIdTest_Obj));
	if (obj == NULL)
	{
		exit(1);	// test infrastructure - fail fast on OOM
	}
	memset(obj, poison, sizeof(*obj));

	// The pin POINTER array is the one thing the test cannot fill itself - it points into this
	// object - so it is rebuilt after the poison. Everything else stays poisoned until a test
	// says otherwise, which is the whole point of the fill.
	for (uint32_t i = 0; i < ALX_ID_TEST_PIN_ARR_LEN; i++)
	{
		obj->pinArr[i] = &obj->pin[i];
	}
	return &obj->id;
}
void AlxIdTest_Delete(AlxId* me)
{
	free(me);
}
AlxIoPin* AlxIdTest_Pin(AlxId* me, uint8_t i)
{
	return &AlxIdTest_Object(me)->pin[i];
}
AlxId_HwInstance* AlxIdTest_Instance(AlxId* me)
{
	// The instance AlxIdTest_CtorNoHwId will pass BY VALUE - filled before the constructor runs,
	// because that is when the copy is taken.
	return &AlxIdTest_Object(me)->instance;
}
AlxId_HwInstance* AlxIdTest_Known(AlxId* me, uint8_t i)
{
	// One entry of the known instance array. The constructor stores the ARRAY, AlxId_Init reads
	// it, so an entry may be filled any time before Init.
	return &AlxIdTest_Object(me)->knownArr[i];
}
void AlxIdTest_SetSupported(AlxId* me, uint8_t i, uint8_t hwId)
{
	AlxIdTest_Object(me)->supportedArr[i] = hwId;
}


//******************************************************************************
// One hardware instance, the two halves a board carries
//******************************************************************************
void AlxIdTest_InstanceSetPcb(AlxId_HwInstance* me, uint8_t id, const char* artf, const char* name, uint8_t verMajor, uint8_t verMinor, uint8_t verPatch, uint32_t verDate)
{
	me->id = id;
	AlxIdTest_SetField(me->pcbArtf, sizeof(me->pcbArtf), artf);
	AlxIdTest_SetField(me->pcbName, sizeof(me->pcbName), name);
	me->pcbVerMajor = verMajor;
	me->pcbVerMinor = verMinor;
	me->pcbVerPatch = verPatch;
	me->pcbVerDate = verDate;
}
void AlxIdTest_InstanceSetBom(AlxId_HwInstance* me, const char* artf, const char* name, uint8_t verMajor, uint8_t verMinor, uint8_t verPatch, uint32_t verDate)
{
	AlxIdTest_SetField(me->bomArtf, sizeof(me->bomArtf), artf);
	AlxIdTest_SetField(me->bomName, sizeof(me->bomName), name);
	me->bomVerMajor = verMajor;
	me->bomVerMinor = verMinor;
	me->bomVerPatch = verPatch;
	me->bomVerDate = verDate;
}


//******************************************************************************
// The two constructors, over the arrays this object owns
//******************************************************************************
void AlxIdTest_Ctor(AlxId* me, const char* fwArtf, const char* fwName, uint8_t fwVerMajor, uint8_t fwVerMinor, uint8_t fwVerPatch, bool fwIsBuildJobUsed, uint32_t fwBuildDateComp, bool fwIsBootUsed, uint32_t fwBootIdAddr, uint8_t hwInstanceKnownArrLen, uint8_t hwInstanceHwIdSupportedArrLen, uint8_t hwIdIoPinArrLen, const char* hwMcuName)
{
	AlxIdTest_Obj* obj = AlxIdTest_Object(me);
	AlxId_Ctor
	(
		me,
		fwArtf,
		fwName,
		fwVerMajor,
		fwVerMinor,
		fwVerPatch,
		fwIsBuildJobUsed,
		fwBuildDateComp,
		fwIsBootUsed,
		fwBootIdAddr,
		obj->knownArr,
		hwInstanceKnownArrLen,
		obj->supportedArr,
		hwInstanceHwIdSupportedArrLen,
		obj->pinArr,
		hwIdIoPinArrLen,
		hwMcuName
	);
}
void AlxIdTest_CtorNoHwId(AlxId* me, const char* fwArtf, const char* fwName, uint8_t fwVerMajor, uint8_t fwVerMinor, uint8_t fwVerPatch, bool fwIsBuildJobUsed, uint32_t fwBuildDateComp, bool fwIsBootUsed, uint32_t fwBootIdAddr, const char* hwMcuName)
{
	AlxIdTest_Obj* obj = AlxIdTest_Object(me);
	AlxId_Ctor_NoHwId
	(
		me,
		fwArtf,
		fwName,
		fwVerMajor,
		fwVerMinor,
		fwVerPatch,
		fwIsBuildJobUsed,
		fwBuildDateComp,
		fwIsBootUsed,
		fwBootIdAddr,
		obj->instance,
		hwMcuName
	);
}


//******************************************************************************
// What no public getter exposes
//******************************************************************************
bool AlxIdTest_IsInit(AlxId* me)
{
	return me->isInit;
}
bool AlxIdTest_IsHwIdUsed(AlxId* me)
{
	// Which constructor was used, as the module recorded it - this is what AlxId_Init branches on.
	return me->isHwIdUsed;
}
uint8_t AlxIdTest_IdIoPinState(AlxId* me, uint8_t i)
{
	// The [in,out] array AlxId_CalcHwId filled: what each identification strap was read as, kept
	// by the module after the pins were deinitialised.
	return (uint8_t)me->hw.idIoPinState[i];
}
uint8_t AlxIdTest_CalcHwId(AlxId* me, uint8_t hwIdIoPinArrLen)
{
	// AlxId_CalcHwId on its own, over this object's pins and state array - the same call Init
	// makes, reachable without an Init.
	AlxIdTest_Obj* obj = AlxIdTest_Object(me);
	uint8_t hwId = 0;
	AlxId_CalcHwId(obj->pinArr, hwIdIoPinArrLen, me->hw.idIoPinState, &hwId);
	return hwId;
}
uint8_t AlxIdTest_TriState_HiZ(void)		{ return (uint8_t)AlxIoPin_TriState_HiZ; }
uint8_t AlxIdTest_TriState_Hi(void)			{ return (uint8_t)AlxIoPin_TriState_Hi; }
uint8_t AlxIdTest_TriState_Lo(void)			{ return (uint8_t)AlxIoPin_TriState_Lo; }
uint8_t AlxIdTest_TriState_Undefined(void)	{ return (uint8_t)AlxIoPin_TriState_Undefined; }


//******************************************************************************
// The bootloader's block, at an address a uint32_t can hold
//******************************************************************************
uint32_t AlxIdTest_FlashAlloc(void)
{
	// A fixed low address rather than wherever the allocator felt like: me->fwBootIdAddr is a
	// uint32_t and the module casts it to void*, so anything above 4 GB is truncated into a wild
	// pointer. Returns 0 if the page is already taken, which the caller has to notice.
	void* page = VirtualAlloc((LPVOID)(uintptr_t)ALX_ID_TEST_FLASH_ADDR, ALX_ID_TEST_FLASH_LEN, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	return (uint32_t)(uintptr_t)page;
}
void AlxIdTest_FlashFree(uint32_t addr)
{
	VirtualFree((LPVOID)(uintptr_t)addr, 0, MEM_RELEASE);
}
void AlxIdTest_FlashFill(uint32_t addr, uint8_t byte, uint32_t len)
{
	// Erased flash is every bit set, and a board that was never programmed with a bootloader is
	// what the module meets most often - that is 0xFF over the whole block, not zeros.
	memset((void*)(uintptr_t)addr, byte, len);
}
void AlxIdTest_BootBlobWrite(uint32_t addr, uint32_t magicNum, uint32_t ver, const char* artf, const char* name, uint8_t verMajor, uint8_t verMinor, uint8_t verPatch, bool isBuildJobUsed, uint32_t buildDate, uint32_t buildDateComp, const char* buildName, const char* hash, const char* hashShort, uint32_t hashShortUint32, uint16_t crc)
{
	// Built through the library's own packed type, so the layout the test writes is by definition
	// the layout the module reads. The magic number and the version are parameters rather than
	// constants because refusing a foreign block is most of what this path does.
	AlxId_FwBootId* blob = (AlxId_FwBootId*)(uintptr_t)addr;
	memset(blob, 0, sizeof(*blob));

	blob->magicNum = magicNum;
	blob->ver = ver;

	AlxIdTest_SetField(blob->fw.build.name, sizeof(blob->fw.build.name), buildName);
	blob->fw.build.date = buildDate;
	blob->fw.build.dateComp = buildDateComp;
	AlxIdTest_SetField(blob->fw.build.hash, sizeof(blob->fw.build.hash), hash);
	AlxIdTest_SetField(blob->fw.build.hashShort, sizeof(blob->fw.build.hashShort), hashShort);
	blob->fw.build.hashShortUint32 = hashShortUint32;

	AlxIdTest_SetField(blob->fw.artf, sizeof(blob->fw.artf), artf);
	AlxIdTest_SetField(blob->fw.name, sizeof(blob->fw.name), name);
	blob->fw.verMajor = verMajor;
	blob->fw.verMinor = verMinor;
	blob->fw.verPatch = verPatch;
	blob->fw.isBuildJobUsed = isBuildJobUsed;

	blob->crc = crc;
}
bool AlxIdTest_BootBlobFillField(uint32_t addr, const char* field, uint8_t byte, uint32_t len)
{
	// One character field overwritten with a repeated byte and NO terminator, which is the state
	// the module cannot see coming: it prints these fields with %s straight out of flash. The
	// field is named rather than given as an offset, so the layout stays in C.
	AlxId_FwBootId* blob = (AlxId_FwBootId*)(uintptr_t)addr;
	char* dst = NULL;
	uint32_t cap = 0;

	if (strcmp(field, "artf") == 0)				{ dst = blob->fw.artf;				cap = sizeof(blob->fw.artf); }
	else if (strcmp(field, "name") == 0)		{ dst = blob->fw.name;				cap = sizeof(blob->fw.name); }
	else if (strcmp(field, "buildName") == 0)	{ dst = blob->fw.build.name;		cap = sizeof(blob->fw.build.name); }
	else if (strcmp(field, "hash") == 0)		{ dst = blob->fw.build.hash;		cap = sizeof(blob->fw.build.hash); }
	else if (strcmp(field, "hashShort") == 0)	{ dst = blob->fw.build.hashShort;	cap = sizeof(blob->fw.build.hashShort); }
	else										{ return false; }

	if (len > cap) { len = cap; }
	memset(dst, byte, len);
	return true;
}
uint32_t AlxIdTest_BootBlobLen(void)		{ return (uint32_t)sizeof(AlxId_FwBootId); }
uint32_t AlxIdTest_BootBlobMagicNum(void)	{ return (uint32_t)ALX_ID_BOOT_ID_MAGIC_NUM; }
uint32_t AlxIdTest_BootBlobVer(void)		{ return (uint32_t)ALX_ID_BOOT_ID_VER; }


//******************************************************************************
// What this DLL was compiled with
//******************************************************************************
const char* AlxIdTest_BuildName(void)			{ return ALX_BUILD_NAME; }
uint32_t AlxIdTest_BuildDate(void)				{ return ALX_BUILD_DATE; }
uint32_t AlxIdTest_BuildNum(void)				{ return ALX_BUILD_NUM; }
uint32_t AlxIdTest_BuildRev(void)				{ return ALX_BUILD_REV; }
const char* AlxIdTest_BuildHash(void)			{ return ALX_BUILD_HASH; }
const char* AlxIdTest_BuildHashShort(void)		{ return ALX_BUILD_HASH_SHORT; }
uint32_t AlxIdTest_BuildHashShortUint32(void)	{ return ALX_BUILD_HASH_SHORT_UINT32; }
