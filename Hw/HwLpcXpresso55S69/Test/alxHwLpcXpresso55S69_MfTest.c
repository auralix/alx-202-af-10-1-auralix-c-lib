/**
  ******************************************************************************
  * @file alxHwLpcXpresso55S69_MainTest.c
  * @brief Auralix HW LPC Xpresso 55S69 C Library - Main Test Module
  * @version $LastChangedRevision: 4937 $
  * @date $LastChangedDate: 2021-05-02 22:05:40 +0200 (Sun, 02 May 2021) $
  ******************************************************************************
  */


//******************************************************************************
// Includes
//******************************************************************************
#include <alxConfig.h>
#include <alxHwLpcXpresso55S69_MainTest.h>


//******************************************************************************
// Variables
//******************************************************************************
#if defined(ALX_FREE_RTOS)
volatile uint32_t G02_counter = 0;
volatile SemaphoreHandle_t IsrTraceSem = { 0 };
TaskHandle_t T11_TraceIsrHandle = { 0 };
#endif
