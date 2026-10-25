/**
  ******************************************************************************
  * @file		FreeRTOS.h
  * @brief		Auralix C Library - Configuration Matrix Fake - FreeRTOS
  * @copyright	Copyright (C) Auralix d.o.o. All rights reserved.
  *
  * Compile-only: the FreeRTOS kernel API as far as the alx code uses it, written from that
  * use, so the configuration matrix compiles every alx line under ALX_FREE_RTOS
  * (Test/config/matrix.toml). Nothing here is the vendor's and nothing here
  * runs; the real one is the product's.
  *
  * The types are the ones FreeRTOS gives them on a Cortex-M port, so the compilers
  * judge every format string and every conversion of the alx code against the real
  * widths: BaseType_t is a long, TickType_t a 32-bit unsigned, and the stack depth
  * of xTaskCreate the uint16_t a FreeRTOSConfig.h leaves configSTACK_DEPTH_TYPE at.
  * The semaphore calls are macros in FreeRTOS and functions here; that changes no
  * caller.
  ******************************************************************************
  **/

#ifndef ALX_SHIM_FREERTOS_FREERTOS_H
#define ALX_SHIM_FREERTOS_FREERTOS_H

#include <stdint.h>
#include <stddef.h>


//******************************************************************************
// Port and configuration
//******************************************************************************
typedef uint32_t StackType_t;
typedef long BaseType_t;
typedef unsigned long UBaseType_t;
typedef uint32_t TickType_t;
#define configSTACK_DEPTH_TYPE uint16_t
#define configMAX_PRIORITIES 7
#define configTICK_RATE_HZ ((TickType_t)1000)
#define portMAX_DELAY ((TickType_t)0xFFFFFFFFUL)
#define pdFALSE ((BaseType_t)0)
#define pdTRUE ((BaseType_t)1)
#define pdPASS (pdTRUE)
#define pdFAIL (pdFALSE)
#define pdMS_TO_TICKS(xTimeInMs) ((TickType_t)(((TickType_t)(xTimeInMs) * (TickType_t)configTICK_RATE_HZ) / (TickType_t)1000U))
void vPortYield(void);
#define portYIELD_FROM_ISR(x) do { if ((x) != pdFALSE) { vPortYield(); } } while (0)


//******************************************************************************
// Tasks
//******************************************************************************
typedef struct tskTaskControlBlock* TaskHandle_t;
typedef void (*TaskFunction_t)(void* pvParameters);
typedef enum
{
	eRunning = 0,
	eReady,
	eBlocked,
	eSuspended,
	eDeleted,
	eInvalid
} eTaskState;

BaseType_t xTaskCreate(TaskFunction_t pxTaskCode, const char* const pcName, const configSTACK_DEPTH_TYPE uxStackDepth, void* const pvParameters, UBaseType_t uxPriority, TaskHandle_t* const pxCreatedTask);
void vTaskDelete(TaskHandle_t xTaskToDelete);
void vTaskDelay(const TickType_t xTicksToDelay);
void vTaskDelayUntil(TickType_t* const pxPreviousWakeTime, const TickType_t xTimeIncrement);
TickType_t xTaskGetTickCount(void);
eTaskState eTaskGetState(TaskHandle_t xTask);
void vTaskStartScheduler(void);
void vTaskEnterCritical(void);
void vTaskExitCritical(void);
#define taskENTER_CRITICAL() vTaskEnterCritical()
#define taskEXIT_CRITICAL() vTaskExitCritical()
#define taskYIELD() vPortYield()


//******************************************************************************
// Semaphores
//******************************************************************************
typedef struct QueueDefinition* SemaphoreHandle_t;
typedef struct { void* pvDummy[8]; UBaseType_t uxDummy[4]; } StaticSemaphore_t;

SemaphoreHandle_t xSemaphoreCreateBinaryStatic(StaticSemaphore_t* pxSemaphoreBuffer);
BaseType_t xSemaphoreGive(SemaphoreHandle_t xSemaphore);
BaseType_t xSemaphoreTake(SemaphoreHandle_t xSemaphore, TickType_t xBlockTime);
UBaseType_t uxSemaphoreGetCount(SemaphoreHandle_t xSemaphore);


//******************************************************************************
// Event groups
//******************************************************************************
typedef TickType_t EventBits_t;
typedef struct EventGroupDef_t* EventGroupHandle_t;
typedef struct { TickType_t xDummy; void* pvDummy[5]; UBaseType_t uxDummy; } StaticEventGroup_t;

EventGroupHandle_t xEventGroupCreate(void);
EventGroupHandle_t xEventGroupCreateStatic(StaticEventGroup_t* pxEventGroupBuffer);
EventBits_t xEventGroupSetBits(EventGroupHandle_t xEventGroup, const EventBits_t uxBitsToSet);
BaseType_t xEventGroupSetBitsFromISR(EventGroupHandle_t xEventGroup, const EventBits_t uxBitsToSet, BaseType_t* pxHigherPriorityTaskWoken);
EventBits_t xEventGroupClearBits(EventGroupHandle_t xEventGroup, const EventBits_t uxBitsToClear);
EventBits_t xEventGroupWaitBits(EventGroupHandle_t xEventGroup, const EventBits_t uxBitsToWaitFor, const BaseType_t xClearOnExit, const BaseType_t xWaitForAllBits, TickType_t xTicksToWait);
EventBits_t xEventGroupSync(EventGroupHandle_t xEventGroup, const EventBits_t uxBitsToSet, const EventBits_t uxBitsToWaitFor, TickType_t xTicksToWait);

#endif	// ALX_SHIM_FREERTOS_FREERTOS_H
