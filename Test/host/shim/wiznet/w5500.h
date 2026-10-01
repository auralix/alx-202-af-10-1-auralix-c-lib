/**
  ******************************************************************************
  * @file		w5500.h
  * @brief		Auralix C Library - Configuration Matrix Fake - Wiznet ioLibrary
  * @copyright	Copyright (C) Auralix d.o.o. All rights reserved.
  *
  * Compile-only: the W5500 register access as far as the alx code uses it,
  * written from that use, so the configuration matrix compiles every alx line
  * under ALX_WIZNET (Test/config/matrix.toml). The register accessors are
  * macros over WIZCHIP_READ and WIZCHIP_WRITE, as in the ioLibrary, so their
  * results have the ioLibrary's types. Nothing here runs.
  ******************************************************************************
  **/

#ifndef ALX_SHIM_WIZNET_W5500_H
#define ALX_SHIM_WIZNET_W5500_H

#include <stdint.h>

#define _W5500_IO_BASE_ 0x00000000
#define _W5500_SPI_READ_ (0x00 << 2)
#define _W5500_SPI_WRITE_ (0x01 << 2)
#define WIZCHIP_CREG_BLOCK 0x00
#define WIZCHIP_SREG_BLOCK(N) (1 + 4 * (N))
#define WIZCHIP_OFFSET_INC(ADDR, N) ((ADDR) + ((N) << 8))

#define GAR (_W5500_IO_BASE_ + (0x0001 << 8) + (WIZCHIP_CREG_BLOCK << 3))
#define SUBR (_W5500_IO_BASE_ + (0x0005 << 8) + (WIZCHIP_CREG_BLOCK << 3))
#define SHAR (_W5500_IO_BASE_ + (0x0009 << 8) + (WIZCHIP_CREG_BLOCK << 3))
#define SIPR (_W5500_IO_BASE_ + (0x000F << 8) + (WIZCHIP_CREG_BLOCK << 3))
#define Sn_MR(N) (_W5500_IO_BASE_ + (0x0000 << 8) + (WIZCHIP_SREG_BLOCK(N) << 3))
#define Sn_CR(N) (_W5500_IO_BASE_ + (0x0001 << 8) + (WIZCHIP_SREG_BLOCK(N) << 3))
#define Sn_SR(N) (_W5500_IO_BASE_ + (0x0003 << 8) + (WIZCHIP_SREG_BLOCK(N) << 3))
#define Sn_PORT(N) (_W5500_IO_BASE_ + (0x0004 << 8) + (WIZCHIP_SREG_BLOCK(N) << 3))
#define Sn_DHAR(N) (_W5500_IO_BASE_ + (0x0006 << 8) + (WIZCHIP_SREG_BLOCK(N) << 3))
#define Sn_DIPR(N) (_W5500_IO_BASE_ + (0x000C << 8) + (WIZCHIP_SREG_BLOCK(N) << 3))
#define Sn_DPORT(N) (_W5500_IO_BASE_ + (0x0010 << 8) + (WIZCHIP_SREG_BLOCK(N) << 3))

#define Sn_MR_CLOSE 0x00
#define Sn_MR_TCP 0x01
#define Sn_MR_UDP 0x02
#define Sn_MR_IPRAW 0x03
#define Sn_MR_MACRAW 0x04
#define Sn_MR_MULTI 0x80
#define Sn_CR_OPEN 0x01
#define Sn_CR_LISTEN 0x02
#define Sn_CR_CONNECT 0x04
#define Sn_CR_DISCON 0x08
#define Sn_CR_CLOSE 0x10
#define Sn_CR_SEND 0x20
#define Sn_CR_RECV 0x40
#define SOCK_CLOSED 0x00
#define SOCK_INIT 0x13
#define SOCK_LISTEN 0x14
#define SOCK_ESTABLISHED 0x17
#define SOCK_CLOSE_WAIT 0x1C
#define SOCK_UDP 0x22
#define SOCK_IPRAW 0x32
#define SOCK_MACRAW 0x42

uint8_t WIZCHIP_READ(uint32_t AddrSel);
void WIZCHIP_WRITE(uint32_t AddrSel, uint8_t wb);
void WIZCHIP_READ_BUF(uint32_t AddrSel, uint8_t* pBuf, uint16_t len);
void WIZCHIP_WRITE_BUF(uint32_t AddrSel, uint8_t* pBuf, uint16_t len);
uint16_t getSn_RX_RSR(uint8_t sn);

#define setGAR(gar) WIZCHIP_WRITE_BUF(GAR, gar, 4)
#define getGAR(gar) WIZCHIP_READ_BUF(GAR, gar, 4)
#define setSUBR(subr) WIZCHIP_WRITE_BUF(SUBR, subr, 4)
#define getSUBR(subr) WIZCHIP_READ_BUF(SUBR, subr, 4)
#define setSHAR(shar) WIZCHIP_WRITE_BUF(SHAR, shar, 6)
#define getSHAR(shar) WIZCHIP_READ_BUF(SHAR, shar, 6)
#define setSIPR(sipr) WIZCHIP_WRITE_BUF(SIPR, sipr, 4)
#define getSIPR(sipr) WIZCHIP_READ_BUF(SIPR, sipr, 4)
#define getSn_SR(sn) WIZCHIP_READ(Sn_SR(sn))
#define getSn_CR(sn) WIZCHIP_READ(Sn_CR(sn))
#define setSn_CR(sn, cr) WIZCHIP_WRITE(Sn_CR(sn), cr)
#define setSn_PORT(sn, port) do { WIZCHIP_WRITE(Sn_PORT(sn), (uint8_t)((port) >> 8)); WIZCHIP_WRITE(WIZCHIP_OFFSET_INC(Sn_PORT(sn), 1), (uint8_t)(port)); } while (0)
#define setSn_DHAR(sn, dhar) WIZCHIP_WRITE_BUF(Sn_DHAR(sn), dhar, 6)
#define setSn_DIPR(sn, dipr) WIZCHIP_WRITE_BUF(Sn_DIPR(sn), dipr, 4)
#define setSn_DPORT(sn, dport) do { WIZCHIP_WRITE(Sn_DPORT(sn), (uint8_t)((dport) >> 8)); WIZCHIP_WRITE(WIZCHIP_OFFSET_INC(Sn_DPORT(sn), 1), (uint8_t)(dport)); } while (0)

#endif	// ALX_SHIM_WIZNET_W5500_H
