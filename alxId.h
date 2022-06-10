/**
  ******************************************************************************
  * @file alxId.h
  * @brief Auralix C Library - ALX ID Module
  * @version $LastChangedRevision: 5735 $
  * @date $LastChangedDate: 2021-08-05 01:58:54 +0200 (Thu, 05 Aug 2021) $
  ******************************************************************************
  */

#ifndef ALX_ID_H
#define ALX_ID_H

#ifdef __cplusplus
extern "C" {
#endif

//******************************************************************************
// Includes
//******************************************************************************
#include "alxGlobal.h"
#include "alxBuild.h"
#include "alxBuild_GENERATED.h"
#include "alxIoPin.h"
#include "alxDelay.h"


//******************************************************************************
// Defines
//******************************************************************************
#define ALX_ID_FILE "alxId.h"
#define ALX_ID_NAME_LEN 40
#define ALX_ID_BUILD_HASH_LEN 50
#define ALX_ID_FW_VER_STR_LEN 100
#define ALX_ID_HW_ID_IO_PIN_BUFF_LEN 5
#define ALX_ID_STM32_HW_UNIQUE_ID_LEN 30


//******************************************************************************
// Macros
//******************************************************************************

// Assert //
#if defined(_ALX_ID_ASSERT_BKPT) || defined(_ALX_ASSERT_BKPT_ALL)
	#define ALX_ID_ASSERT(expr) ALX_ASSERT_BKPT(ALX_ID_FILE, expr)
#elif defined(_ALX_ID_ASSERT_TRACE) || defined(_ALX_ASSERT_TRACE_ALL)
	#define ALX_ID_ASSERT(expr) ALX_ASSERT_TRACE(ALX_ID_FILE, expr)
#elif defined(_ALX_ID_ASSERT_RST) || defined(_ALX_ASSERT_RST_ALL)
	#define ALX_ID_ASSERT(expr) ALX_ASSERT_RST(ALX_ID_FILE, expr)
#else
	#define ALX_ID_ASSERT(expr) do{} while (false)
#endif

// Trace //
#if defined(_ALX_ID_TRACE) || defined(_ALX_TRACE_ALL)
	#define ALX_ID_TRACE(...) ALX_TRACE_STD(ALX_ID_FILE, __VA_ARGS__)
	#define ALX_ID_TRACE_FORMAT(...) ALX_TRACE_FORMAT(__VA_ARGS__)
#else
	#define ALX_ID_TRACE(...) do{} while (false)
	#define ALX_ID_TRACE_FORMAT(...) do{} while (false)
#endif


//******************************************************************************
// Types
//******************************************************************************
typedef enum
{
	Alx_IoPinState_Floating = 0,
	Alx_IoPinState_High = 1,
	Alx_IoPinState_Low = 2
} AlxId_HwIdIoPinState;

typedef struct
{
	// Parameters - Const
	char name[ALX_ID_NAME_LEN];
	uint32_t date;
	uint32_t dateComp;
	uint32_t num;
	char hash[ALX_ID_BUILD_HASH_LEN];
	uint32_t rev;
} AlxId_Build;

typedef struct
{
	// Parameters - Const
	AlxId_Build build;

	// Parameters
	char artf[ALX_ID_NAME_LEN];
	char name[ALX_ID_NAME_LEN];
	uint8_t verMajor;
	uint8_t verMinor;
	uint8_t verPatch;
	uint32_t verDate;
	uint64_t ver;
	char verStr[ALX_ID_FW_VER_STR_LEN];
	bool isBuildJobUsed;
} AlxId_Fw;

typedef struct
{
	uint8_t id;

	char pcbArtf[ALX_ID_NAME_LEN];
	char pcbName[ALX_ID_NAME_LEN];
	uint8_t pcbVerMajor;
	uint8_t pcbVerMinor;
	uint8_t pcbVerPatch;
	uint32_t pcbVerDate;
	uint64_t pcbVer;

	char bomArtf[ALX_ID_NAME_LEN];
	char bomName[ALX_ID_NAME_LEN];
	uint8_t bomVerMajor;
	uint8_t bomVerMinor;
	uint8_t bomVerPatch;
	uint32_t bomVerDate;
	uint64_t bomVer;
} AlxId_HwInstance;

typedef struct
{
	// Objects - External
	AlxId_HwInstance* instanceKnownArr;
	uint8_t* instanceHwIdSupportedArr;
	AlxIoPin** idIoPinArr;

	// Parameters
	uint8_t instanceKnownArrLen;
	uint8_t instanceHwIdSupportedArrLen;
	uint8_t idIoPinArrLen;

	// Variables
	AlxId_HwInstance instance; // Not initialized in Ctor
	AlxIoPin_TriState idIoPinState[ALX_ID_HW_ID_IO_PIN_BUFF_LEN]; // Not initialized in Ctor
} AlxId_Hw;

typedef struct
{
	// Parameters - Const
	uint32_t ver;
	uint32_t verMinRequired;
} AlxId_LangC;

typedef struct
{
	// Parameters - Const
	char name[ALX_ID_NAME_LEN];
	uint32_t verMajor;
	uint32_t verMinor;
	uint32_t verPatch;
	uint32_t ver;
	uint32_t verMinRequired;
} AlxId_LangCLib;

typedef struct
{
	// Parameters - Const
	char name[ALX_ID_NAME_LEN];
	uint32_t verMajor;
	uint32_t verMinor;
	uint32_t verPatch;
	uint32_t ver;
	uint32_t verMinRequired;
} AlxId_Comp;

#ifdef ALX_CMSIS_CORE
typedef struct
{
	// Parameters - Const
	uint32_t verMajor;
	uint32_t verMinor;
	uint32_t ver;
	uint32_t verRequired;
} AlxId_CmsisCore;
#endif

#ifdef ALX_STM32
typedef struct
{
	// Parameters - Const
	uint32_t verMajor;
	uint32_t verMinor;
	uint32_t verPatch;
	uint32_t verRc;
	uint32_t ver;
	uint32_t verRequired;
} AlxId_Stm32;
#endif

#ifdef ALX_STM32
typedef struct
{
	// Parameters - Const
	uint32_t deviceId;
	uint32_t revisionId;
	uint32_t flashSize_kB;
	#ifdef ALX_STM32G4
		uint32_t packageId;
	#endif
	uint32_t uniqueId0;
	uint32_t uniqueId1;
	uint32_t uniqueId2;
	char uniqueIdStr[ALX_ID_STM32_HW_UNIQUE_ID_LEN];
} AlxId_Stm32Hw;
#endif

typedef struct
{
	// Objects - Internal
	AlxId_Fw fwApp;
	AlxId_Fw fwBoot;
	AlxId_Hw hw;
	AlxId_LangC langC;
	AlxId_LangCLib langCLib;
	AlxId_Comp comp;

	#ifdef ALX_CMSIS_CORE
	AlxId_CmsisCore cmsisCore;
	#endif

	#ifdef ALX_STM32
	AlxId_Stm32 stm32HalCmsis;
	AlxId_Stm32 stm32HalDriver;
	AlxId_Stm32Hw stm32Hw;
	#endif

	// Parameters
	bool fwIsBootloader;
	char hwMcuName[ALX_ID_NAME_LEN];

	// Info
	bool isInit;
	bool wasCtorCalled;
	bool isHwIdUsed;
} AlxId;


//******************************************************************************
// Constructor
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
	bool fwIsBootloader,
	AlxId_HwInstance* hwInstanceKnownArr,
	uint8_t hwInstanceKnownArrLen,
	uint8_t* hwInstanceHwIdSupportedArr,
	uint8_t hwInstanceHwIdSupportedArrLen,
	AlxIoPin** hwIdIoPinArr,
	uint8_t hwIdIoPinArrLen,
	const char* hwMcuName
);
void AlxId_Ctor_NoHwId
(
	AlxId* me,
	const char* fwArtf,
	const char* fwName,
	uint8_t fwVerMajor,
	uint8_t fwVerMinor,
	uint8_t fwVerPatch,
	bool fwIsBuildJobUsed,
	bool fwIsBootloader,
	AlxId_HwInstance hwInstance,
	const char* hwMcuName
);


//******************************************************************************
// Functions
//******************************************************************************
void AlxId_Init(AlxId* me);
void AlxId_Trace(AlxId* me);
uint8_t AlxId_GetHwId(AlxId* me);
const char* AlxId_GetUniqueIdStr(AlxId* me);
uint32_t AlxId_GetFwAppVerDate(AlxId* me);
const char* AlxId_GetFwAppVerStr(AlxId* me);


#ifdef __cplusplus
}
#endif

#endif // ALX_ID_H
