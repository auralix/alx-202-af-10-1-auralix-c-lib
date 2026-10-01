/**
  ******************************************************************************
  * @file		fspHostShim.h
  * @brief		Auralix C Library - Configuration Matrix Fake - Renesas FSP
  * @copyright	Copyright (C) Auralix d.o.o. All rights reserved.
  *
  * Compile-only: every name of Renesas FSP the alx code uses, written from
  * that use, so the configuration matrix compiles every alx line under
  * ALX_RA (Test/config/matrix.toml). Nothing here is the vendor's and
  * nothing here runs; the real one is the product's.
  *
  * As the STM32 fake does, it makes no claim about the vendor's signatures
  * and layouts, only that every name exists: one register block and one
  * structure shape hold every member the code names, a call forwards its
  * arguments to one variadic function, a constant has a value no other has.
  * Grown from the compilers' diagnostics by the scratch tool of ALX-1564
  * night goal 4; edited by hand from then on.
  ******************************************************************************
  **/

#ifndef ALX_SHIM_FSP_FSPHOSTSHIM_H
#define ALX_SHIM_FSP_FSPHOSTSHIM_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "cmsisHostShim.h"


typedef struct alxShimFsp_Sub
{
	uint32_t alxShimUnused;
} alxShimFsp_Sub;

typedef struct alxShimFsp_Struct
{
	uint32_t alxShimUnused;
} alxShimFsp_Struct;


extern alxShimFsp_Struct alxShimFsp_Instance;

uint32_t alxShimFsp_Call(int unused, ...);
void* alxShimFsp_CallPtr(int unused, ...);


#endif	// ALX_SHIM_FSP_FSPHOSTSHIM_H
