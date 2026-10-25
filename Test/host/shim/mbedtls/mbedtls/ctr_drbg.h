/**
  ******************************************************************************
  * @file		ctr_drbg.h
  * @brief		Auralix C Library - Configuration Matrix Fake - Mbed TLS
  * @copyright	Copyright (C) Auralix d.o.o. All rights reserved.
  *
  * Compile-only: the Mbed TLS 3 CTR-DRBG context as far as the alx code uses
  * it, written from that use, so the configuration matrix compiles every alx
  * line under ALX_MBEDTLS (Test/config/matrix.toml). Nothing here runs.
  ******************************************************************************
  **/

#ifndef ALX_SHIM_MBEDTLS_MBEDTLS_CTR_DRBG_H
#define ALX_SHIM_MBEDTLS_MBEDTLS_CTR_DRBG_H

typedef struct mbedtls_ctr_drbg_context
{
	unsigned char counter[16];
	int reseed_counter;
} mbedtls_ctr_drbg_context;

void mbedtls_ctr_drbg_init(mbedtls_ctr_drbg_context* ctx);
void mbedtls_ctr_drbg_free(mbedtls_ctr_drbg_context* ctx);

#endif	// ALX_SHIM_MBEDTLS_MBEDTLS_CTR_DRBG_H
