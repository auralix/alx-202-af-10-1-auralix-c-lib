/**
  ******************************************************************************
  * @file alxI2c_McuLpc80x.h
  * @brief Auralix C Library - ALX I2C Module
  * @version $LastChangedRevision: 4270 $
  * @date $LastChangedDate: 2021-03-05 19:02:52 +0100 (Fri, 05 Mar 2021) $
  ******************************************************************************
  */

#ifndef ALX_I2C_MCU_LPC80X_H
#define ALX_I2C_MCU_LPC80X_H

#ifdef __cplusplus
extern "C" {
#endif

//******************************************************************************
// Includes
//******************************************************************************
#include "alxGlobal.h"
#include "alxTrace.h"
#include "alxAssert.h"
#include "alxTimSw.h"
#include "alxIoPin.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_LPC80X)


//******************************************************************************
// Types
//******************************************************************************
typedef enum
{
	AlxI2c_Clk_McuLpc80x_BitRate_100kHz_I2cFuncClk_15MHz = 100000U,
	AlxI2c_Clk_McuLpc80x_BitRate_400kHz_I2cFuncClk_15MHz = 400000U,
} AlxI2c_Clk;

typedef struct
{
	// Objects - External
	AlxIoPin* io_SCL;
	AlxIoPin* io_SDA;

	// Parameters
	I2C_Type* i2c;
	AlxI2c_Clk clk;

	// Variables
	i2c_master_config_t i2cConfig;
	AlxTimSw tim;

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
	I2C_Type* i2c,
	AlxIoPin* io_SCL,
	AlxIoPin* io_SDA,
	AlxI2c_Clk clk
);


#endif

#ifdef __cplusplus
}
#endif

#endif // ALX_I2C_MCU_LPC80X_H
