/**
  ******************************************************************************
  * @file		wizchip_conf.h
  * @brief		Auralix C Library - Configuration Matrix Fake - Wiznet ioLibrary
  * @copyright	Copyright (C) Auralix d.o.o. All rights reserved.
  *
  * Compile-only: the Wiznet ioLibrary chip configuration as far as the alx code
  * uses it - the network information, the PHY, the callbacks of the SPI
  * glue - written from that use, so the configuration matrix compiles every
  * alx line under ALX_WIZNET (Test/config/matrix.toml). Nothing here is the
  * vendor's and nothing here runs; the real one is the product's.
  ******************************************************************************
  **/

#ifndef ALX_SHIM_WIZNET_WIZCHIP_CONF_H
#define ALX_SHIM_WIZNET_WIZCHIP_CONF_H

#include <stdint.h>

#define _WIZCHIP_SOCK_NUM_ 8
#define PHY_LINK_OFF 0
#define PHY_LINK_ON 1

typedef enum
{
	NETINFO_STATIC = 1,
	NETINFO_DHCP
} dhcp_mode;

typedef struct wiz_NetInfo_t
{
	uint8_t mac[6];
	uint8_t ip[4];
	uint8_t sn[4];
	uint8_t gw[4];
	uint8_t dns[4];
	dhcp_mode dhcp;
} wiz_NetInfo;

typedef struct wiz_PhyConf_t
{
	uint8_t by;
	uint8_t mode;
	uint8_t speed;
	uint8_t duplex;
} wiz_PhyConf;

typedef enum
{
	CW_RESET_WIZCHIP,
	CW_INIT_WIZCHIP,
	CW_GET_INTERRUPT,
	CW_CLR_INTERRUPT,
	CW_SET_INTRMASK,
	CW_GET_INTRMASK,
	CW_SET_INTRTIME,
	CW_GET_INTRTIME,
	CW_GET_ID,
	CW_RESET_PHY,
	CW_SET_PHYCONF,
	CW_GET_PHYCONF,
	CW_GET_PHYSTATUS,
	CW_SET_PHYPOWMODE,
	CW_GET_PHYPOWMODE,
	CW_GET_PHYLINK
} ctlwizchip_type;

typedef enum
{
	CN_SET_NETINFO,
	CN_GET_NETINFO,
	CN_SET_NETMODE,
	CN_GET_NETMODE,
	CN_SET_TIMEOUT,
	CN_GET_TIMEOUT
} ctlnetwork_type;

void reg_wizchip_cris_cbfunc(void (*cris_en)(void), void (*cris_ex)(void));
void reg_wizchip_cs_cbfunc(void (*cs_sel)(void), void (*cs_desel)(void));
void reg_wizchip_spi_cbfunc(uint8_t (*spi_rb)(void), void (*spi_wb)(uint8_t wb));
void reg_wizchip_spiburst_cbfunc(void (*spi_rb)(uint8_t* pBuf, uint16_t len), void (*spi_wb)(uint8_t* pBuf, uint16_t len));
int8_t ctlwizchip(ctlwizchip_type cwtype, void* arg);
int8_t ctlnetwork(ctlnetwork_type cntype, void* arg);
int8_t wizchip_init(uint8_t* txsize, uint8_t* rxsize);
int8_t wizphy_getphylink(void);
void wizphy_getphyconf(wiz_PhyConf* phyconf);

#include "w5500.h"

#endif	// ALX_SHIM_WIZNET_WIZCHIP_CONF_H
