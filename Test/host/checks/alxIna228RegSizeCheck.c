/**
  ******************************************************************************
  * @file		alxIna228RegSizeCheck.c
  * @brief		Auralix C Library - ALX INA228/INA238 Module - Register Size Check
  * @copyright	Copyright (C) Auralix d.o.o. All rights reserved.
  *
  * A compile-time check, for the TARGET compiler, that every register overlay
  * is the size of the register it describes.
  *
  * The driver takes each register's I2C transfer length from sizeof of its
  * overlay (AlxIna228_RegStruct_SetLen), so a layout that is one byte out reads
  * or writes the wrong number of bytes on the bus - silently, and only on the
  * hardware. Several of these overlays describe a register with two bit-fields
  * of DIFFERENT declared types, whose packing is decided by the ABI: the GNU
  * rules the target compiles with fold them into one unit, and the Microsoft
  * rules a PC compiler follows do not. Measured on a PC, eleven of the
  * seventeen INA238 overlays come out the wrong size; on the target, none do.
  *
  * That is why this file exists and why it is never linked. It is compiled by
  * the ANALYZE lane with arm-gcc, once per part variant, and every line below
  * is a static assertion - so a wrong size is a compiler error naming the
  * register, from the compiler whose answer counts.
  ******************************************************************************
  **/

//******************************************************************************
// Includes
//******************************************************************************
#include "alxIna228.h"


//******************************************************************************
// Checks
//******************************************************************************
#define ALX_INA228_REG_SIZE_CHECK(name, expect) \
	_Static_assert(sizeof(AlxIna228_RegVal_##name) == (expect), \
		"AlxIna228_RegVal_" #name " is not the size of the register it describes")

ALX_INA228_REG_SIZE_CHECK(0x00_CONFIG, 2);
ALX_INA228_REG_SIZE_CHECK(0x01_ADC_CONFIG, 2);
ALX_INA228_REG_SIZE_CHECK(0x02_SHUNT_CAL, 2);
#if defined(ALX_INA228)
ALX_INA228_REG_SIZE_CHECK(0x03_SHUNT_TEMPCO, 2);
ALX_INA228_REG_SIZE_CHECK(0x04_VSHUNT, 3);
ALX_INA228_REG_SIZE_CHECK(0x05_VBUS, 3);
ALX_INA228_REG_SIZE_CHECK(0x07_CURRENT, 3);
ALX_INA228_REG_SIZE_CHECK(0x09_ENERGY, 5);
ALX_INA228_REG_SIZE_CHECK(0x0A_CHARGE, 5);
#else
ALX_INA228_REG_SIZE_CHECK(0x04_VSHUNT, 2);
ALX_INA228_REG_SIZE_CHECK(0x05_VBUS, 2);
ALX_INA228_REG_SIZE_CHECK(0x07_CURRENT, 2);
#endif
ALX_INA228_REG_SIZE_CHECK(0x06_DIETEMP, 2);
ALX_INA228_REG_SIZE_CHECK(0x08_POWER, 3);
ALX_INA228_REG_SIZE_CHECK(0x0B_DIAG_ALRT, 2);
ALX_INA228_REG_SIZE_CHECK(0x0C_SOVL, 2);
ALX_INA228_REG_SIZE_CHECK(0x0D_SUVL, 2);
ALX_INA228_REG_SIZE_CHECK(0x0E_BOVL, 2);
ALX_INA228_REG_SIZE_CHECK(0x0F_BUVL, 2);
ALX_INA228_REG_SIZE_CHECK(0x10_TEMP_LIMIT, 2);
ALX_INA228_REG_SIZE_CHECK(0x11_PWR_LIMIT, 2);
ALX_INA228_REG_SIZE_CHECK(0x3E_MANUFACTURER_ID, 2);
ALX_INA228_REG_SIZE_CHECK(0x3F_DEVICE_ID, 2);
