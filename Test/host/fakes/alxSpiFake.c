/**
  ******************************************************************************
  * @file		alxSpiFake.c
  * @brief		Auralix C Library - ALX SPI Module - PC Unit Test Fake
  * @copyright	Copyright (C) Auralix d.o.o. All rights reserved.
  *
  * A link-time fake of the SPI master, named after the module it fakes, so a
  * device driver above it can be exercised on a PC. It is a recorder and a
  * script: every byte the master sends (MOSI) is recorded in order, and every
  * byte it receives (MISO) comes from a queue the test fills the way the part
  * would answer. A receive with the queue empty reads zeros, as a bus with
  * nothing driving MISO low-or-high would read whatever the pull gives. Chip
  * select is counted and its state kept, so a test can see that each transfer
  * was framed. Nothing is timed, nothing fails unless the test asks it to.
  *
  * On an MCU family the module's own constructor exists and is defined here
  * under the same guard the module header uses.
  ******************************************************************************
  **/

//******************************************************************************
// Includes
//******************************************************************************
#include "alxSpi.h"
#include <string.h>


//******************************************************************************
// Private Variables
//******************************************************************************
#define ALX_SPI_FAKE_BUFF_LEN 1024

static uint8_t alxSpiFake_mosi[ALX_SPI_FAKE_BUFF_LEN];
static uint32_t alxSpiFake_mosiLen;
static uint8_t alxSpiFake_miso[ALX_SPI_FAKE_BUFF_LEN];
static uint32_t alxSpiFake_misoLen;
static uint32_t alxSpiFake_misoPos;
static uint32_t alxSpiFake_csAssertCount;
static bool alxSpiFake_isCsAsserted;
static uint32_t alxSpiFake_initCount;
static uint32_t alxSpiFake_deInitCount;
static int32_t alxSpiFake_forcedStatus = Alx_Ok;
static uint32_t alxSpiFake_callCount = 0;	// every call of the contract, whatever it answered (A97)
static uint32_t alxSpiFake_failAt = 0;		// the call count at which one call answers Alx_Err; 0 disarms


//******************************************************************************
// Prototypes - the DLL export surface (no separate header for test fakes)
//******************************************************************************
void AlxSpiFake_Reset(void);
void AlxSpiFake_QueueMiso(const uint8_t* data, uint32_t len);
uint32_t AlxSpiFake_MosiLen(void);
uint32_t AlxSpiFake_Mosi(uint8_t* data, uint32_t len);
uint32_t AlxSpiFake_MisoLeft(void);
uint32_t AlxSpiFake_CsAssertCount(void);
bool AlxSpiFake_IsCsAsserted(void);
uint32_t AlxSpiFake_InitCount(void);
uint32_t AlxSpiFake_DeInitCount(void);
void AlxSpiFake_SetForcedStatus(int32_t status);
void AlxSpiFake_FailAt(uint32_t nth);
uint32_t AlxSpiFake_CallCount(void);


//******************************************************************************
// Private Functions
//******************************************************************************
static void AlxSpiFake_Send(const uint8_t* data, uint16_t len)
{
	for (uint32_t i = 0; i < len; i++)
	{
		if (alxSpiFake_mosiLen < ALX_SPI_FAKE_BUFF_LEN)
		{
			alxSpiFake_mosi[alxSpiFake_mosiLen] = data[i];
			alxSpiFake_mosiLen++;
		}
	}
}
static void AlxSpiFake_Receive(uint8_t* data, uint16_t len)
{
	for (uint32_t i = 0; i < len; i++)
	{
		if (alxSpiFake_misoPos < alxSpiFake_misoLen)
		{
			data[i] = alxSpiFake_miso[alxSpiFake_misoPos];
			alxSpiFake_misoPos++;
		}
		else
		{
			data[i] = 0x00;
		}
	}
}


//******************************************************************************
// The fake's own controls
//******************************************************************************
void AlxSpiFake_Reset(void)
{
	memset(alxSpiFake_mosi, 0, sizeof(alxSpiFake_mosi));
	memset(alxSpiFake_miso, 0, sizeof(alxSpiFake_miso));
	alxSpiFake_mosiLen = 0;
	alxSpiFake_misoLen = 0;
	alxSpiFake_misoPos = 0;
	alxSpiFake_csAssertCount = 0;
	alxSpiFake_isCsAsserted = false;
	alxSpiFake_initCount = 0;
	alxSpiFake_deInitCount = 0;
	alxSpiFake_forcedStatus = Alx_Ok;
	alxSpiFake_callCount = 0;
	alxSpiFake_failAt = 0;
}
void AlxSpiFake_QueueMiso(const uint8_t* data, uint32_t len)
{
	for (uint32_t i = 0; i < len; i++)
	{
		if (alxSpiFake_misoLen < ALX_SPI_FAKE_BUFF_LEN)
		{
			alxSpiFake_miso[alxSpiFake_misoLen] = data[i];
			alxSpiFake_misoLen++;
		}
	}
}
uint32_t AlxSpiFake_MosiLen(void)
{
	return alxSpiFake_mosiLen;
}
uint32_t AlxSpiFake_Mosi(uint8_t* data, uint32_t len)
{
	uint32_t n = alxSpiFake_mosiLen;
	if (n > len)
	{
		n = len;
	}
	memcpy(data, alxSpiFake_mosi, n);
	return n;
}
uint32_t AlxSpiFake_MisoLeft(void)
{
	return alxSpiFake_misoLen - alxSpiFake_misoPos;
}
uint32_t AlxSpiFake_CsAssertCount(void)
{
	return alxSpiFake_csAssertCount;
}
bool AlxSpiFake_IsCsAsserted(void)
{
	return alxSpiFake_isCsAsserted;
}
uint32_t AlxSpiFake_InitCount(void)
{
	return alxSpiFake_initCount;
}
uint32_t AlxSpiFake_DeInitCount(void)
{
	return alxSpiFake_deInitCount;
}
void AlxSpiFake_SetForcedStatus(int32_t status)
{
	alxSpiFake_forcedStatus = status;
}
void AlxSpiFake_FailAt(uint32_t nth)
{
	// The nth call from now answers Alx_Err, the others the forced status. 0 disarms.
	alxSpiFake_failAt = (nth == 0) ? 0 : alxSpiFake_callCount + nth;
}
uint32_t AlxSpiFake_CallCount(void)
{
	return alxSpiFake_callCount;
}

static Alx_Status AlxSpiFake_Answer(void)
{
	alxSpiFake_callCount++;
	if (alxSpiFake_failAt != 0 && alxSpiFake_callCount == alxSpiFake_failAt)
	{
		return Alx_Err;
	}
	return (Alx_Status)(alxSpiFake_forcedStatus);
}


//******************************************************************************
// The faked module's own contract
//******************************************************************************
#if defined(ALX_STM32F4) || defined(ALX_STM32F7) || defined(ALX_STM32G4) || defined(ALX_STM32L0) || defined(ALX_STM32L4)
void AlxSpi_Ctor(AlxSpi* me, SPI_TypeDef* spi, AlxIoPin* do_SCK, AlxIoPin* do_MOSI, AlxIoPin* di_MISO, AlxIoPin* do_nCS, AlxSpi_Mode mode, AlxClk* clk, AlxSpi_Clk spiClk, bool isWriteReadLowLevel)
{
	(void)me; (void)spi; (void)do_SCK; (void)do_MOSI; (void)di_MISO; (void)do_nCS; (void)mode;
	(void)clk; (void)spiClk; (void)isWriteReadLowLevel;
}
#endif

Alx_Status AlxSpi_Init(AlxSpi* me)
{
	(void)me;
	alxSpiFake_initCount++;
	return AlxSpiFake_Answer();
}
Alx_Status AlxSpi_DeInit(AlxSpi* me)
{
	(void)me;
	alxSpiFake_deInitCount++;
	return AlxSpiFake_Answer();
}
Alx_Status AlxSpi_Master_Write(AlxSpi* me, uint8_t* writeData, uint16_t len, uint8_t numOfTries, uint16_t timeout_ms)
{
	(void)me; (void)numOfTries; (void)timeout_ms;
	AlxSpiFake_Send(writeData, len);
	return AlxSpiFake_Answer();
}
Alx_Status AlxSpi_Master_Read(AlxSpi* me, uint8_t* readData, uint16_t len, uint8_t numOfTries, uint16_t timeout_ms)
{
	(void)me; (void)numOfTries; (void)timeout_ms;
	AlxSpiFake_Receive(readData, len);
	return AlxSpiFake_Answer();
}
Alx_Status AlxSpi_Master_WriteRead(AlxSpi* me, uint8_t* writeData, uint8_t* readData, uint16_t len, uint8_t numOfTries, uint16_t timeout_ms)
{
	(void)me; (void)numOfTries; (void)timeout_ms;
	AlxSpiFake_Send(writeData, len);
	AlxSpiFake_Receive(readData, len);
	return AlxSpiFake_Answer();
}
void AlxSpi_Master_AssertCs(AlxSpi* me)
{
	(void)me;
	alxSpiFake_csAssertCount++;
	alxSpiFake_isCsAsserted = true;
}
void AlxSpi_Master_DeAssertCs(AlxSpi* me)
{
	(void)me;
	alxSpiFake_isCsAsserted = false;
}
