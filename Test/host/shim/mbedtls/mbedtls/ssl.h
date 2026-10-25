/**
  ******************************************************************************
  * @file		ssl.h
  * @brief		Auralix C Library - Configuration Matrix Fake - Mbed TLS
  * @copyright	Copyright (C) Auralix d.o.o. All rights reserved.
  *
  * Compile-only: the Mbed TLS 3 SSL API as far as the alx code uses it - the
  * configuration, the context, the BIO and verify callbacks, the handshake
  * and the transfer calls - written from that use, so the configuration matrix
  * compiles every alx line under ALX_MBEDTLS (Test/config/matrix.toml).
  * Nothing here is the vendor's and nothing here runs.
  ******************************************************************************
  **/

#ifndef ALX_SHIM_MBEDTLS_MBEDTLS_SSL_H
#define ALX_SHIM_MBEDTLS_MBEDTLS_SSL_H

#include <stddef.h>
#include <stdint.h>
#include "mbedtls/x509.h"

#define MBEDTLS_ERR_SSL_WANT_READ -0x6900
#define MBEDTLS_ERR_SSL_WANT_WRITE -0x6880
#define MBEDTLS_SSL_IS_CLIENT 0
#define MBEDTLS_SSL_IS_SERVER 1
#define MBEDTLS_SSL_TRANSPORT_STREAM 0
#define MBEDTLS_SSL_PRESET_DEFAULT 0
#define MBEDTLS_SSL_VERIFY_NONE 0
#define MBEDTLS_SSL_VERIFY_OPTIONAL 1
#define MBEDTLS_SSL_VERIFY_REQUIRED 2
#define MBEDTLS_SSL_MAJOR_VERSION_3 3
#define MBEDTLS_SSL_MINOR_VERSION_3 3

typedef struct mbedtls_ssl_config
{
	int endpoint;
	int authmode;
	void* p_rng;
} mbedtls_ssl_config;

typedef struct mbedtls_ssl_context
{
	const mbedtls_ssl_config* conf;
	int state;
	void* p_bio;
} mbedtls_ssl_context;

typedef int mbedtls_ssl_send_t(void* ctx, const unsigned char* buf, size_t len);
typedef int mbedtls_ssl_recv_t(void* ctx, unsigned char* buf, size_t len);
typedef int mbedtls_ssl_recv_timeout_t(void* ctx, unsigned char* buf, size_t len, uint32_t timeout);

void mbedtls_ssl_init(mbedtls_ssl_context* ssl);
void mbedtls_ssl_free(mbedtls_ssl_context* ssl);
void mbedtls_ssl_config_init(mbedtls_ssl_config* conf);
void mbedtls_ssl_config_free(mbedtls_ssl_config* conf);
int mbedtls_ssl_config_defaults(mbedtls_ssl_config* conf, int endpoint, int transport, int preset);
void mbedtls_ssl_conf_authmode(mbedtls_ssl_config* conf, int authmode);
void mbedtls_ssl_conf_ca_chain(mbedtls_ssl_config* conf, mbedtls_x509_crt* ca_chain, mbedtls_x509_crl* ca_crl);
int mbedtls_ssl_conf_own_cert(mbedtls_ssl_config* conf, mbedtls_x509_crt* own_cert, mbedtls_pk_context* pk_key);
void mbedtls_ssl_conf_rng(mbedtls_ssl_config* conf, int (*f_rng)(void*, unsigned char*, size_t), void* p_rng);
void mbedtls_ssl_conf_min_version(mbedtls_ssl_config* conf, int major, int minor);
void mbedtls_ssl_conf_max_version(mbedtls_ssl_config* conf, int major, int minor);
int mbedtls_ssl_setup(mbedtls_ssl_context* ssl, const mbedtls_ssl_config* conf);
void mbedtls_ssl_set_bio(mbedtls_ssl_context* ssl, void* p_bio, mbedtls_ssl_send_t* f_send, mbedtls_ssl_recv_t* f_recv, mbedtls_ssl_recv_timeout_t* f_recv_timeout);
void mbedtls_ssl_set_verify(mbedtls_ssl_context* ssl, int (*f_vrfy)(void*, mbedtls_x509_crt*, int, uint32_t*), void* p_vrfy);
int mbedtls_ssl_handshake(mbedtls_ssl_context* ssl);
uint32_t mbedtls_ssl_get_verify_result(const mbedtls_ssl_context* ssl);
int mbedtls_ssl_read(mbedtls_ssl_context* ssl, unsigned char* buf, size_t len);
int mbedtls_ssl_write(mbedtls_ssl_context* ssl, const unsigned char* buf, size_t len);
int mbedtls_ssl_close_notify(mbedtls_ssl_context* ssl);

#endif	// ALX_SHIM_MBEDTLS_MBEDTLS_SSL_H
