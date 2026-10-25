/**
  ******************************************************************************
  * @file		oid.h
  * @brief		Auralix C Library - Configuration Matrix Fake - Mbed TLS
  * @copyright	Copyright (C) Auralix d.o.o. All rights reserved.
  *
  * Compile-only: the Mbed TLS object identifiers as far as the alx code uses
  * them - the common name and the comparison macro - written from that use, so
  * the configuration matrix compiles every alx line under ALX_MBEDTLS
  * (Test/config/matrix.toml). Nothing here runs.
  ******************************************************************************
  **/

#ifndef ALX_SHIM_MBEDTLS_MBEDTLS_OID_H
#define ALX_SHIM_MBEDTLS_MBEDTLS_OID_H

#include <string.h>
#include "mbedtls/x509.h"

#define MBEDTLS_OID_AT "\x55\x04"
#define MBEDTLS_OID_AT_CN MBEDTLS_OID_AT "\x03"
#define MBEDTLS_OID_SIZE(x) (sizeof(x) - 1)
#define MBEDTLS_OID_CMP(oid_str, oid_buf) ((MBEDTLS_OID_SIZE(oid_str) != (oid_buf)->len) || memcmp((oid_str), (oid_buf)->p, (oid_buf)->len) != 0)

#endif	// ALX_SHIM_MBEDTLS_MBEDTLS_OID_H
