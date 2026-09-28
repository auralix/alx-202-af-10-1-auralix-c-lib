/**
  ******************************************************************************
  * @file		alxFsFake.c
  * @brief		Auralix C Library - ALX File System Module - PC Unit Test Fake
  * @copyright	Copyright (C) Auralix d.o.o. All rights reserved.
  *
  * A link-time fake of the file system, named after the module it fakes. The
  * real one is littlefs or FatFs over a flash driver; here it is an array of
  * name/bytes pairs in RAM.
  *
  * Two things it keeps besides the files, and they are why it exists:
  *
  *   - HOW MANY FILES ARE OPEN. A caller that returns early on an error and
  *     forgets to close leaks a handle, which on a real file system is a
  *     resource and a missing sync. Counting opens against closes is the only
  *     way to see that from outside.
  *   - AN INJECTABLE FAILURE PER OPERATION. Every function the module calls can
  *     be told to fail once, or always, so the error paths are reachable at all.
  *     Without that, a module whose error handling is wrong still passes every
  *     test, because nothing ever fails.
  *
  * Formatting empties the store, the way a real format does, so a test can see
  * whether data survived.
  ******************************************************************************
  **/


//******************************************************************************
// Includes
//******************************************************************************
#include "alxFs.h"
#include <string.h>


//******************************************************************************
// Private types and variables
//******************************************************************************
#define ALX_FS_FAKE_MAX_FILES 16
#define ALX_FS_FAKE_MAX_NAME 64
#define ALX_FS_FAKE_MAX_DATA 256

typedef struct
{
	char name[ALX_FS_FAKE_MAX_NAME];
	uint8_t data[ALX_FS_FAKE_MAX_DATA];
	uint32_t len;
	bool used;
} AlxFsFake_Entry;

// The operations a test can make fail, in the order the .def and the Python wrapper name them.
typedef enum
{
	AlxFsFake_Op_Mount = 0,
	AlxFsFake_Op_UnMount,
	AlxFsFake_Op_Format,
	AlxFsFake_Op_Open,
	AlxFsFake_Op_Close,
	AlxFsFake_Op_Read,
	AlxFsFake_Op_Write,
	AlxFsFake_Op_Remove,
	AlxFsFake_Op_COUNT
} AlxFsFake_Op;

static AlxFsFake_Entry alxFsFake_entry[ALX_FS_FAKE_MAX_FILES];
static int32_t alxFsFake_failCount[AlxFsFake_Op_COUNT];		// >0 = that many more times, -1 = always
static uint32_t alxFsFake_failSkip[AlxFsFake_Op_COUNT];		// calls to let through first
static uint32_t alxFsFake_callCount[AlxFsFake_Op_COUNT];
static bool alxFsFake_isMounted;
static uint32_t alxFsFake_openCount;
static uint32_t alxFsFake_closeCount;
static uint32_t alxFsFake_formatCount;
static const char* alxFsFake_openName;		// the file the one open handle refers to
static char alxFsFake_openMode[8];			// the mode string the caller asked for


//******************************************************************************
// Prototypes - the DLL export surface (no separate header for test fakes)
//******************************************************************************
void AlxFsFake_Reset(void);
void AlxFsFake_FailNext(uint32_t op, int32_t times);
void AlxFsFake_FailSkip(uint32_t op, uint32_t calls);
uint32_t AlxFsFake_CallCount(uint32_t op);
uint32_t AlxFsFake_OpenCount(void);
uint32_t AlxFsFake_CloseCount(void);
uint32_t AlxFsFake_FormatCount(void);
uint32_t AlxFsFake_FilesHeld(void);
bool AlxFsFake_IsMounted(void);
void AlxFsFake_Put(const char* name, const uint8_t* data, uint32_t len);
uint32_t AlxFsFake_Get(const char* name, uint8_t* data, uint32_t lenMax);
bool AlxFsFake_Has(const char* name);
const char* AlxFsFake_LastOpenMode(void);


//******************************************************************************
// Private functions
//******************************************************************************
static bool AlxFsFake_ShouldFail(AlxFsFake_Op op)
{
	alxFsFake_callCount[op]++;

	// A skip lets the first N calls through untouched, so a test can fail the SECOND write of a
	// pair without touching the first - which is the power-loss case this fake exists for.
	if (alxFsFake_failSkip[op] > 0) { alxFsFake_failSkip[op]--; return false; }

	if (alxFsFake_failCount[op] < 0) { return true; }			// always
	if (alxFsFake_failCount[op] > 0) { alxFsFake_failCount[op]--; return true; }
	return false;
}

static AlxFsFake_Entry* AlxFsFake_Find(const char* name)
{
	for (uint32_t i = 0; i < ALX_FS_FAKE_MAX_FILES; i++)
	{
		if (alxFsFake_entry[i].used && strcmp(alxFsFake_entry[i].name, name) == 0)
		{
			return &alxFsFake_entry[i];
		}
	}
	return NULL;
}

static AlxFsFake_Entry* AlxFsFake_Make(const char* name)
{
	AlxFsFake_Entry* entry = AlxFsFake_Find(name);
	if (entry != NULL) { return entry; }

	for (uint32_t i = 0; i < ALX_FS_FAKE_MAX_FILES; i++)
	{
		if (alxFsFake_entry[i].used == false)
		{
			entry = &alxFsFake_entry[i];
			memset(entry, 0, sizeof(*entry));
			strncpy(entry->name, name, ALX_FS_FAKE_MAX_NAME - 1);
			entry->used = true;
			return entry;
		}
	}
	return NULL;
}


//******************************************************************************
// The fake's own controls
//******************************************************************************
void AlxFsFake_Reset(void)
{
	memset(alxFsFake_entry, 0, sizeof(alxFsFake_entry));
	memset(alxFsFake_failCount, 0, sizeof(alxFsFake_failCount));
	memset(alxFsFake_failSkip, 0, sizeof(alxFsFake_failSkip));
	memset(alxFsFake_callCount, 0, sizeof(alxFsFake_callCount));
	alxFsFake_isMounted = false;
	alxFsFake_openCount = 0;
	alxFsFake_closeCount = 0;
	alxFsFake_formatCount = 0;
	alxFsFake_openName = NULL;
	alxFsFake_openMode[0] = '\0';
}

void AlxFsFake_FailSkip(uint32_t op, uint32_t calls)
{
	// How many calls to let through before AlxFsFake_FailNext's count starts to apply.
	if (op < (uint32_t)AlxFsFake_Op_COUNT) { alxFsFake_failSkip[op] = calls; }
}

void AlxFsFake_FailNext(uint32_t op, int32_t times)
{
	// times > 0: fail that many more calls. times < 0: fail every call. times 0: stop failing.
	if (op < (uint32_t)AlxFsFake_Op_COUNT) { alxFsFake_failCount[op] = times; }
}

uint32_t AlxFsFake_CallCount(uint32_t op)
{
	if (op >= (uint32_t)AlxFsFake_Op_COUNT) { return 0; }
	return alxFsFake_callCount[op];
}

uint32_t AlxFsFake_OpenCount(void)		{ return alxFsFake_openCount; }
uint32_t AlxFsFake_CloseCount(void)		{ return alxFsFake_closeCount; }
uint32_t AlxFsFake_FormatCount(void)	{ return alxFsFake_formatCount; }
bool AlxFsFake_IsMounted(void)			{ return alxFsFake_isMounted; }

uint32_t AlxFsFake_FilesHeld(void)
{
	uint32_t held = 0;
	for (uint32_t i = 0; i < ALX_FS_FAKE_MAX_FILES; i++)
	{
		if (alxFsFake_entry[i].used) { held++; }
	}
	return held;
}

void AlxFsFake_Put(const char* name, const uint8_t* data, uint32_t len)
{
	// What is already on the "flash" before the module ever runs.
	AlxFsFake_Entry* entry = AlxFsFake_Make(name);
	if (entry == NULL || len > ALX_FS_FAKE_MAX_DATA) { return; }
	memcpy(entry->data, data, len);
	entry->len = len;
}

uint32_t AlxFsFake_Get(const char* name, uint8_t* data, uint32_t lenMax)
{
	AlxFsFake_Entry* entry = AlxFsFake_Find(name);
	if (entry == NULL) { return 0; }

	uint32_t len = lenMax;
	if (entry->len < lenMax) { len = entry->len; }
	memcpy(data, entry->data, len);
	return len;
}

bool AlxFsFake_Has(const char* name)
{
	return AlxFsFake_Find(name) != NULL;
}

const char* AlxFsFake_LastOpenMode(void)
{
	// Which mode the module asked for - "w" truncates and "r" does not, and only the
	// caller knows which it meant.
	return alxFsFake_openMode;
}


//******************************************************************************
// The faked module
//******************************************************************************
Alx_Status AlxFs_Mount(AlxFs* me)
{
	(void)me;
	if (AlxFsFake_ShouldFail(AlxFsFake_Op_Mount)) { return Alx_Err; }
	alxFsFake_isMounted = true;
	return Alx_Ok;
}

Alx_Status AlxFs_UnMount(AlxFs* me)
{
	(void)me;
	if (AlxFsFake_ShouldFail(AlxFsFake_Op_UnMount)) { return Alx_Err; }
	alxFsFake_isMounted = false;
	return Alx_Ok;
}

Alx_Status AlxFs_Format(AlxFs* me)
{
	(void)me;
	alxFsFake_formatCount++;
	if (AlxFsFake_ShouldFail(AlxFsFake_Op_Format)) { return Alx_Err; }

	// A format is a format: everything stored is gone.
	memset(alxFsFake_entry, 0, sizeof(alxFsFake_entry));
	return Alx_Ok;
}

Alx_Status AlxFs_Remove(AlxFs* me, const char* path)
{
	(void)me;
	if (AlxFsFake_ShouldFail(AlxFsFake_Op_Remove)) { return Alx_Err; }

	AlxFsFake_Entry* entry = AlxFsFake_Find(path);
	if (entry == NULL) { return Alx_Err; }
	entry->used = false;
	return Alx_Ok;
}

Alx_Status AlxFs_File_Open(AlxFs* me, AlxFs_File* file, const char* path, const char* mode)
{
	(void)me;
	(void)file;
	if (AlxFsFake_ShouldFail(AlxFsFake_Op_Open)) { return Alx_Err; }

	if (mode[0] == 'w')
	{
		// Writing truncates, the way "w" does everywhere.
		AlxFsFake_Entry* entry = AlxFsFake_Make(path);
		if (entry == NULL) { return Alx_Err; }
		entry->len = 0;
	}
	else if (AlxFsFake_Find(path) == NULL)
	{
		return Alx_Err;		// reading something that was never written
	}

	alxFsFake_openName = path;
	strncpy(alxFsFake_openMode, mode, sizeof(alxFsFake_openMode) - 1);
	alxFsFake_openCount++;
	return Alx_Ok;
}

Alx_Status AlxFs_File_Close(AlxFs* me, AlxFs_File* file)
{
	(void)me;
	(void)file;
	if (AlxFsFake_ShouldFail(AlxFsFake_Op_Close)) { return Alx_Err; }
	alxFsFake_closeCount++;
	return Alx_Ok;
}

Alx_Status AlxFs_File_Read(AlxFs* me, AlxFs_File* file, void* data, uint32_t len, uint32_t* lenActual)
{
	(void)me;
	(void)file;
	if (AlxFsFake_ShouldFail(AlxFsFake_Op_Read)) { return Alx_Err; }

	AlxFsFake_Entry* entry = AlxFsFake_Find(alxFsFake_openName);
	if (entry == NULL) { return Alx_Err; }

	uint32_t actual = len;
	if (entry->len < len) { actual = entry->len; }
	memcpy(data, entry->data, actual);
	if (lenActual != NULL) { *lenActual = actual; }
	return Alx_Ok;
}

Alx_Status AlxFs_File_Write(AlxFs* me, AlxFs_File* file, void* data, uint32_t len)
{
	(void)me;
	(void)file;
	if (AlxFsFake_ShouldFail(AlxFsFake_Op_Write)) { return Alx_Err; }

	AlxFsFake_Entry* entry = AlxFsFake_Find(alxFsFake_openName);
	if (entry == NULL || len > ALX_FS_FAKE_MAX_DATA) { return Alx_Err; }

	memcpy(entry->data, data, len);
	entry->len = len;
	return Alx_Ok;
}
