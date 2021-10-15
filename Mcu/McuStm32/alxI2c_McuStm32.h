/**
  ******************************************************************************
  * @file alxI2c_McuStm32.h
  * @brief Auralix C Library - ALX I2C Module
  * @version $LastChangedRevision: 5216 $
  * @date $LastChangedDate: 2021-05-24 19:38:05 +0200 (Mon, 24 May 2021) $
  ******************************************************************************
  */

#ifndef ALX_I2C_MCU_STM32_H
#define ALX_I2C_MCU_STM32_H

#ifdef __cplusplus
extern "C" {
#endif

//******************************************************************************
// Includes
//******************************************************************************
#include "alxGlobal.h"
#include "alxTrace.h"
#include "alxAssert.h"
#include "alxIoPin.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_STM32F4) || defined(ALX_STM32G4) || defined(ALX_STM32L0)


//******************************************************************************
// Types
//******************************************************************************
typedef enum
{
	AlxI2c_Clk_100kHz = 100U,
	AlxI2c_Clk_400kHz = 400U
} AlxI2c_Clk;

typedef struct
{
	// Objects - External
	AlxIoPin* io_SCL;
	AlxIoPin* io_SDA;

	// Parameters
	AlxI2c_Clk clk;

	// Variables
	I2C_HandleTypeDef hi2c;

	// Info
	bool isInit;
	bool wasCtorCalled;
} AlxI2c;


//******************************************************************************
// Constructor
//******************************************************************************
void AlxI2c_Ctor
(
	AlxI2c* me,
	I2C_TypeDef* i2c,
	AlxIoPin* io_SCL,
	AlxIoPin* io_SDA,
	AlxI2c_Clk clk
);


#endif

#ifdef __cplusplus
}
#endif

#endif // ALX_I2C_MCU_STM32_H
