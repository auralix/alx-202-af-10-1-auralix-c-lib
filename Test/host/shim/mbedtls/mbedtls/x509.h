/**
  ******************************************************************************
  * @file		x509.h
  * @brief		Auralix C Library - Configuration Matrix Fake - Mbed TLS
  * @copyright	Copyright (C) Auralix d.o.o. All rights reserved.
  *
  * Compile-only: the Mbed TLS 3 certificate and key API as far as the alx code
  * uses it - the ASN.1 buffers, names and sequences a verify callback walks,
  * the certificate chain, the private key - written from that use, so the
  * configuration matrix compiles every alx line under ALX_MBEDTLS
  * (Test/config/matrix.toml). Nothing here is the vendor's and nothing here
  * runs; the real one is the product's.
  ******************************************************************************
  **/

#ifndef ALX_SHIM_MBEDTLS_MBEDTLS_X509_H
#define ALX_SHIM_MBEDTLS_MBEDTLS_X509_H

#include <stddef.h>
#include <stdint.h>


//******************************************************************************
// ASN.1
//******************************************************************************
#define MBEDTLS_ASN1_TAG_VALUE_MASK 0x1F

typedef struct mbedtls_asn1_buf
{
	int tag;
	size_t len;
	unsigned char* p;
} mbedtls_asn1_buf;

typedef struct mbedtls_asn1_sequence
{
	mbedtls_asn1_buf buf;
	struct mbedtls_asn1_sequence* next;
} mbedtls_asn1_sequence;

typedef struct mbedtls_asn1_named_data
{
	mbedtls_asn1_buf oid;
	mbedtls_asn1_buf val;
	struct mbedtls_asn1_named_data* next;
	unsigned char next_merged;
} mbedtls_asn1_named_data;


//******************************************************************************
// X.509
//******************************************************************************
#define MBEDTLS_X509_SAN_DNS_NAME 2
#define MBEDTLS_X509_SAN_IP_ADDRESS 7
#define MBEDTLS_X509_BADCERT_CN_MISMATCH 0x04

typedef mbedtls_asn1_buf mbedtls_x509_buf;
typedef mbedtls_asn1_named_data mbedtls_x509_name;
typedef mbedtls_asn1_sequence mbedtls_x509_sequence;

typedef struct mbedtls_x509_crt
{
	mbedtls_x509_buf raw;
	mbedtls_x509_name subject;
	mbedtls_x509_sequence subject_alt_names;
	struct mbedtls_x509_crt* next;
} mbedtls_x509_crt;

typedef struct mbedtls_x509_crl mbedtls_x509_crl;

void mbedtls_x509_crt_init(mbedtls_x509_crt* crt);
void mbedtls_x509_crt_free(mbedtls_x509_crt* crt);
int mbedtls_x509_crt_parse(mbedtls_x509_crt* chain, const unsigned char* buf, size_t buflen);
size_t mbedtls_x509_crt_parse_cn_inet_pton(const char* cn, void* dst);


//******************************************************************************
// Private key
//******************************************************************************
typedef struct mbedtls_pk_context
{
	const void* pk_info;
	void* pk_ctx;
} mbedtls_pk_context;

void mbedtls_pk_init(mbedtls_pk_context* ctx);
void mbedtls_pk_free(mbedtls_pk_context* ctx);
int mbedtls_pk_parse_key(mbedtls_pk_context* ctx, const unsigned char* key, size_t keylen, const unsigned char* pwd, size_t pwdlen, int (*f_rng)(void*, unsigned char*, size_t), void* p_rng);

#endif	// ALX_SHIM_MBEDTLS_MBEDTLS_X509_H
