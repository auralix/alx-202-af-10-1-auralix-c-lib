/**
  ******************************************************************************
  * @file alxHwNucleoF429Zi_TvTest.h
  * @brief Auralix HW NUCLEO-F429ZI C Library - TV Test Module
  * @version $LastChangedRevision: 4937 $
  * @date $LastChangedDate: 2021-05-02 22:05:40 +0200 (Sun, 02 May 2021) $
  ******************************************************************************
  */

#ifndef ALX_HW_NUCLEO_F429ZI_TV_TEST_H
#define ALX_HW_NUCLEO_F429ZI_TV_TEST_H

#ifdef __cplusplus
extern "C" {
#endif

//******************************************************************************
// Includes
//******************************************************************************
#include <alxHwNucleoF429Zi_Main.h>


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_HW_NUCLEO_F429ZI_C_TEST)


//******************************************************************************
//******************************************************************************
// G01_BringUp
//******************************************************************************
//******************************************************************************


//******************************************************************************
// Types
//******************************************************************************
typedef struct
{
	// Objects
	AlxHwNucleoF429Zi_Main alxHwNucleoF429Zi_Main;

	// Info
	bool wasCtorCalled;
	bool isInit;
} AlxHwNucleoF429Zi_TvTest_G01_BringUp;


//******************************************************************************
// Private Functions
//******************************************************************************
static inline void AlxHwNucleoF429Zi_TvTest_G01_BringUp_T01_Led(AlxHwNucleoF429Zi_TvTest_G01_BringUp* me)
{
	AlxIoPin_Init(&me->alxHwNucleoF429Zi_Main.alxIoPin.do_PB0_LED1_GR);
	AlxIoPin_Init(&me->alxHwNucleoF429Zi_Main.alxIoPin.do_PB7_LED2_BL);
	AlxIoPin_Init(&me->alxHwNucleoF429Zi_Main.alxIoPin.do_PB14_LED3_RD);

	while(1)
	{
		AlxIoPin_Toggle(&me->alxHwNucleoF429Zi_Main.alxIoPin.do_PB0_LED1_GR);
		AlxIoPin_Toggle(&me->alxHwNucleoF429Zi_Main.alxIoPin.do_PB7_LED2_BL);
		AlxIoPin_Toggle(&me->alxHwNucleoF429Zi_Main.alxIoPin.do_PB14_LED3_RD);
		AlxDelay_ms(100);
	}
}
static inline void AlxHwNucleoF429Zi_TvTest_G01_BringUp_T02_Trace(AlxHwNucleoF429Zi_TvTest_G01_BringUp* me)
{
	(void)me;

	while(1)
	{
		ALX_TRACE_FORMAT("T02_Trace\r\n");
		AlxDelay_ms(100);
	}
}
static inline void AlxHwNucleoF429Zi_TvTest_G01_BringUp_T03_MyFun1(uint32_t myVar)
{
	(void)myVar;
}
static inline void AlxHwNucleoF429Zi_TvTest_G01_BringUp_T03_MyFun2(uintptr_t myVar)
{
	(void)myVar;
}
static inline void AlxHwNucleoF429Zi_TvTest_G01_BringUp_T03_Typedef(AlxHwNucleoF429Zi_TvTest_G01_BringUp* me)
{
	(void)me;

	///
	/// Typedef
	///

	// Without using typedef
	struct MyStruct1
	{
		uint8_t myVar1;
		uint8_t myVar2;
	};

	struct MyStruct1 myStruct1;

	// With using typedef
	typedef struct
	{
		uint8_t myVar1;
		uint8_t myVar2;
	} MyStruct2;

	MyStruct2 myStruct2;




	///
	/// Function Pointer
	///

	// Function Pointer declaration
	void (*myFunPtr1)(uint32_t);

	// Function pointer definition
	myFunPtr1 = &AlxHwNucleoF429Zi_TvTest_G01_BringUp_T03_MyFun1;

	// Function pointer invoking
	(*myFunPtr1)(10);


	// Function Pointer declaration
	void (*myFunPtr2)(uint32_t);

	// Function pointer definition (without &)
	myFunPtr2 = AlxHwNucleoF429Zi_TvTest_G01_BringUp_T03_MyFun1;

	// Function pointer invoking (without *)
	myFunPtr2(10);


	// Function Pointer declaration (with uintptr_t -> an unsigned integer type that is capable of storing a data pointer. Which typically means that it's the same size as a pointer)
	void (*myFunPtr3)(uintptr_t myUintPtrArg);	// Argument has name, this is also OK, so everything works

	// Function pointer definition (without &)
	myFunPtr3 = AlxHwNucleoF429Zi_TvTest_G01_BringUp_T03_MyFun2;

	// Function pointer invoking (without *)
	myFunPtr3(10);




	///
	/// Function Pointer & Typedef
	///

	// Typedef function pointer
	typedef void (*MyFunPtr4)(uintptr_t);

	// Function Pointer declaration
	MyFunPtr4 myFunPtr4;

	// Function pointer definition (without &)
	myFunPtr4 = AlxHwNucleoF429Zi_TvTest_G01_BringUp_T03_MyFun2;

	// Function pointer invoking (without *)
	myFunPtr4(10);
}


//******************************************************************************
// Constructor & Functions
//******************************************************************************
static inline void AlxHwNucleoF429Zi_TvTest_G01_BringUp_Ctor(AlxHwNucleoF429Zi_TvTest_G01_BringUp* me)
{
	// Ctor
	AlxHwNucleoF429Zi_Main_Ctor(&me->alxHwNucleoF429Zi_Main);

	// Info
	me->wasCtorCalled = true;
}
static inline void AlxHwNucleoF429Zi_TvTest_G01_BringUp_Init(AlxHwNucleoF429Zi_TvTest_G01_BringUp* me)
{
	// Init
	AlxClk_Init(&alxClk);
	AlxTrace_Init(&alxTrace);

	// Info
	me->isInit = true;
}
static inline void AlxHwNucleoF429Zi_TvTest_G01_BringUp_Run(AlxHwNucleoF429Zi_TvTest_G01_BringUp* me)
{
	AlxHwNucleoF429Zi_TvTest_G01_BringUp_T01_Led(me);
	//AlxHwNucleoF429Zi_TvTest_G01_BringUp_T02_Trace(me);
	AlxHwNucleoF429Zi_TvTest_G01_BringUp_T03_Typedef(me);
}


//******************************************************************************
//******************************************************************************
// G02
//******************************************************************************
//******************************************************************************


//******************************************************************************
// Types
//******************************************************************************
typedef struct
{
	// Objects
	AlxHwNucleoF429Zi_Main alxHwNucleoF429Zi_Main;

	// Info
	bool wasCtorCalled;
	bool isInit;
} AlxHwNucleoF429Zi_TvTest_G02;


//******************************************************************************
// Private Functions
//******************************************************************************
static inline void AlxHwNucleoF429Zi_TvTest_G02_T01(AlxHwNucleoF429Zi_TvTest_G02* me)
{
	(void)me;
}
static inline void AlxHwNucleoF429Zi_TvTest_G02_T02(AlxHwNucleoF429Zi_TvTest_G02* me)
{
	(void)me;
}


//******************************************************************************
// Constructor & Functions
//******************************************************************************
static inline void AlxHwNucleoF429Zi_TvTest_G02_Ctor(AlxHwNucleoF429Zi_TvTest_G02* me)
{
	// Ctor
	AlxHwNucleoF429Zi_Main_Ctor(&me->alxHwNucleoF429Zi_Main);

	// Info
	me->wasCtorCalled = true;
}
static inline void AlxHwNucleoF429Zi_TvTest_G02_Init(AlxHwNucleoF429Zi_TvTest_G02* me)
{
	// Init
	AlxClk_Init(&alxClk);
	AlxTrace_Init(&alxTrace);

	// Info
	me->isInit = true;
}
static inline void AlxHwNucleoF429Zi_TvTest_G02_Run(AlxHwNucleoF429Zi_TvTest_G02* me)
{
	AlxHwNucleoF429Zi_TvTest_G02_T01(me);
	//AlxHwNucleoF429Zi_TvTest_G02_T02(me);
}


#endif // #if defined(ALX_HW_NUCLEO_F429ZI_C_TEST)

#ifdef __cplusplus
}
#endif

#endif // ALX_HW_NUCLEO_F429ZI_TV_TEST_H
