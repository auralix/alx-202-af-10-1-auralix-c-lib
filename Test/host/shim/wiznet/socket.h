/**
  ******************************************************************************
  * @file		socket.h
  * @brief		Auralix C Library - Configuration Matrix Fake - Wiznet ioLibrary
  * @copyright	Copyright (C) Auralix d.o.o. All rights reserved.
  *
  * Compile-only: the Wiznet ioLibrary socket API as far as the alx code uses
  * it, written from that use, so the configuration matrix compiles every alx
  * line under ALX_WIZNET (Test/config/matrix.toml). Nothing here is the
  * vendor's and nothing here runs; the real one is the product's.
  ******************************************************************************
  **/

#ifndef ALX_SHIM_WIZNET_SOCKET_H
#define ALX_SHIM_WIZNET_SOCKET_H

#include <stdint.h>
#include "wizchip_conf.h"

#define SOCK_OK 1
#define SOCK_BUSY 0
#define SOCK_ERROR 0
#define SOCKERR_SOCKNUM (SOCK_ERROR - 1)
#define SOCKERR_SOCKOPT (SOCK_ERROR - 2)
#define SOCKERR_SOCKINIT (SOCK_ERROR - 3)
#define SOCKERR_SOCKCLOSED (SOCK_ERROR - 4)
#define SOCKERR_SOCKMODE (SOCK_ERROR - 5)
#define SOCKERR_SOCKFLAG (SOCK_ERROR - 6)
#define SOCKERR_SOCKSTATUS (SOCK_ERROR - 7)
#define SOCKERR_TIMEOUT (SOCK_ERROR - 13)

#define IPPROTO_IP 0
#define IPPROTO_ICMP 1
#define IPPROTO_TCP 6
#define IPPROTO_UDP 17

int8_t socket(uint8_t sn, uint8_t protocol, uint16_t port, uint8_t flag);
int8_t close(uint8_t sn);
int8_t listen(uint8_t sn);
int8_t connect(uint8_t sn, uint8_t* addr, uint16_t port);
int8_t disconnect(uint8_t sn);
int32_t send(uint8_t sn, uint8_t* buf, uint16_t len);
int32_t recv(uint8_t sn, uint8_t* buf, uint16_t len);
int32_t sendto(uint8_t sn, uint8_t* buf, uint16_t len, uint8_t* addr, uint16_t port);
int32_t recvfrom(uint8_t sn, uint8_t* buf, uint16_t len, uint8_t* addr, uint16_t* port);

#endif	// ALX_SHIM_WIZNET_SOCKET_H
