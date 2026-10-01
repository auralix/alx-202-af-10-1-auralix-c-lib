/**
  ******************************************************************************
  * @file		dhcp.h
  * @brief		Auralix C Library - Configuration Matrix Fake - Wiznet ioLibrary
  * @copyright	Copyright (C) Auralix d.o.o. All rights reserved.
  *
  * Compile-only: the Wiznet ioLibrary DHCP stack as far as the alx code uses
  * it, written from that use, so the configuration matrix compiles every alx
  * line under ALX_WIZNET (Test/config/matrix.toml). Nothing here is the
  * vendor's and nothing here runs; the real one is the product's.
  ******************************************************************************
  **/

#ifndef ALX_SHIM_WIZNET_DHCP_H
#define ALX_SHIM_WIZNET_DHCP_H

#include <stdint.h>

enum
{
	DHCP_FAILED = 0,
	DHCP_RUNNING,
	DHCP_IP_ASSIGN,
	DHCP_IP_CHANGED,
	DHCP_IP_LEASED,
	DHCP_STOPPED
};

void DHCP_init(uint8_t s, uint8_t* buf);
void DHCP_time_handler(void);
void reg_dhcp_cbfunc(void (*ip_assign)(void), void (*ip_update)(void), void (*ip_conflict)(void));
uint8_t DHCP_run(void);
void DHCP_stop(void);
void getIPfromDHCP(uint8_t* ip);
void getGWfromDHCP(uint8_t* ip);
void getSNfromDHCP(uint8_t* ip);
void getDNSfromDHCP(uint8_t* ip);
uint32_t getDHCPLeasetime(void);

#endif	// ALX_SHIM_WIZNET_DHCP_H
