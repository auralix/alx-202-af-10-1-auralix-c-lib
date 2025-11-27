/**
  ******************************************************************************
  * @file		alxUsb_McuStm32.c
  * @brief		Auralix C Library - ALX USB MCU STM32 Module
  * @copyright	Copyright (C) Auralix d.o.o. All rights reserved.
  *
  * @section License
  *
  * SPDX-License-Identifier: GPL-3.0-or-later
  *
  * This file is part of Auralix C Library.
  *
  * Auralix C Library is free software: you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation, either version 3
  * of the License, or (at your option) any later version.
  *
  * Auralix C Library is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  * GNU General Public License for more details.
  *
  * You should have received a copy of the GNU General Public License
  * along with Auralix C Library. If not, see <https://www.gnu.org/licenses/>.
  ******************************************************************************
  **/

//*******************************************************************************
// Includes
//*******************************************************************************
#include "alxUsb_McuStm32.h"
#include "alxUsb.h"


//*******************************************************************************
// Module Guard
//*******************************************************************************
#if defined(ALX_C_LIB) && defined(ALX_STM32F7)


//*******************************************************************************
// Private Variables
//*******************************************************************************
static AlxUsb* alxUsb_me = NULL;


//*******************************************************************************
// Private Functions
//*******************************************************************************


//------------------------------------------------------------------------------
// Specific
//------------------------------------------------------------------------------
#if defined(ALX_USBH)
static void AlxUsb_Event_Callback(USBH_HandleTypeDef* phost, uint8_t id);
#endif


//------------------------------------------------------------------------------
// General
//------------------------------------------------------------------------------
static Alx_Status AlxUsb_Reset(AlxUsb* me);
static void AlxUsb_Periph_EnableClk(AlxUsb* me);
static void AlxUsb_Periph_DisableClk(AlxUsb* me);
static void AlxUsb_Periph_ForceReset(AlxUsb* me);
static void AlxUsb_Periph_ReleaseReset(AlxUsb* me);
static void AlxUsb_Periph_EnableIrq(AlxUsb* me);
static void AlxUsb_Periph_DisableIrq(AlxUsb* me);


//*******************************************************************************
// Constructor
//*******************************************************************************
void AlxUsb_Ctor
(
	AlxUsb*	me,
	HCD_TypeDef* usb,
	AlxIoPin* io_USB_D_P,
	AlxIoPin* io_USB_D_N,
	Alx_IrqPriority irqPriority
)
{
	// Private Variables
	alxUsb_me = me;

	// Parameters
	me->usb = usb;
	me->io_USB_D_P = io_USB_D_P;
	me->io_USB_D_N = io_USB_D_N;
	me->irqPriority = irqPriority;

	// Variables
	me->hhcd.Instance = usb;
	me->hhcd.Init.dev_endpoints = 0;				// Unused, only relevant for PCD
	me->hhcd.Init.Host_channels = 8;				// 8 is max for USB_OTG_FS
	me->hhcd.Init.dma_enable = 0;
	me->hhcd.Init.speed = HCD_SPEED_FULL;
	me->hhcd.Init.ep0_mps = 0;						// Unused, only relevant for PCD
	me->hhcd.Init.phy_itface = HCD_PHY_EMBEDDED;
	me->hhcd.Init.Sof_enable = 1;
	me->hhcd.Init.low_power_enable = 0;
	me->hhcd.Init.lpm_enable = 0;
	me->hhcd.Init.battery_charging_enable = 0;
	me->hhcd.Init.vbus_sensing_enable = 0;
	me->hhcd.Init.use_dedicated_ep1 = 0;
	me->hhcd.Init.use_external_vbus = 0;
	me->usbh_event = 0;
	me->usbhMsc_isReady = false;
	me->isReady = false;

	// Link
	#if defined(ALX_USBH)
	me->hhcd.pData = &me->usbh;
	me->usbh.pData = &me->hhcd;
	#endif

	// Info
	me->wasCtorCalled = true;
	me->isInit = false;
}


//*******************************************************************************
// Functions
//*******************************************************************************

/**
  * @brief
  * @param[in,out]	me
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxUsb_Init(AlxUsb* me)
{
	// Assert
	ALX_USB_ASSERT(me->wasCtorCalled == true);
	ALX_USB_ASSERT(me->isInit == false);

	// Init GPIO
	AlxIoPin_Init(me->io_USB_D_P);
	AlxIoPin_Init(me->io_USB_D_N);

	// Release USB periphery reset
	AlxUsb_Periph_ReleaseReset(me);

	// Enable USB periphery clock
	AlxUsb_Periph_EnableClk(me);

	// Enable USB periphery IRQ
	AlxUsb_Periph_EnableIrq(me);

	// Local variables
	#if defined(ALX_USBH)
	USBH_StatusTypeDef status = USBH_FAIL;

	// Init USB
	status = USBH_Init(&me->usbh, AlxUsb_Event_Callback, 0);
	if (status != USBH_OK)
	{
		ALX_USB_TRACE_ERR("FAIL: USBH_Init() status %ld", status);
		return Alx_Err;
	}

	// Register USB class
	status = USBH_RegisterClass(&me->usbh, USBH_MSC_CLASS);
	if (status != USBH_OK)
	{
		ALX_USB_TRACE_ERR("FAIL: USBH_RegisterClass() status %ld", status);
		return Alx_Err;
	}

	// Start USB
	status = USBH_Start(&me->usbh);
	if (status != USBH_OK)
	{
		ALX_USB_TRACE_ERR("FAIL: USBH_Start() status %ld", status);
		return Alx_Err;
	}
	#endif

	// Set isInit
	me->isInit = true;

	// Return
	return Alx_Ok;
}

/**
  * @brief
  * @param[in,out]	me
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxUsb_DeInit(AlxUsb* me)
{
	// Assert
	ALX_USB_ASSERT(me->wasCtorCalled == true);
	ALX_USB_ASSERT(me->isInit == true);

	// Local variables
	#if defined(ALX_USBH)
	USBH_StatusTypeDef status = USBH_FAIL;

	// Stop USB
	status = USBH_Stop(&me->usbh);
	if (status != USBH_OK)
	{
		ALX_USB_TRACE_ERR("FAIL: USBH_Stop() status %ld", status);
		return Alx_Err;
	}

	// DeInit USB
	status = USBH_DeInit(&me->usbh);
	if (status != USBH_OK)
	{
		ALX_USB_TRACE_ERR("FAIL: USBH_DeInit() status %ld", status);
		return Alx_Err;
	}
	#endif

	// Disable USB periphery IRQ
	AlxUsb_Periph_DisableIrq(me);

	// Force USB periphery reset
	AlxUsb_Periph_ForceReset(me);

	// Disable USB periphery clock
	AlxUsb_Periph_DisableClk(me);

	// DeInit GPIO
	AlxIoPin_DeInit(me->io_USB_D_P);
	AlxIoPin_DeInit(me->io_USB_D_N);

	// Clear
	me->usbh_event = 0;
	me->usbhMsc_isReady = false;
	me->isReady = false;

	// Clear isInit
	me->isInit = false;

	// Return
	return Alx_Ok;
}

/**
  * @brief
  * @param[in,out]	me
  */
Alx_Status AlxUsb_Handle(AlxUsb* me)
{
	//------------------------------------------------------------------------------
	// Assert
	//------------------------------------------------------------------------------
	ALX_USB_ASSERT(me->wasCtorCalled == true);
	ALX_USB_ASSERT(me->isInit == true);


	//------------------------------------------------------------------------------
	// Handle
	//------------------------------------------------------------------------------
	#if defined(ALX_USBH)

	// Process
	USBH_Process(&me->usbh);	// Always returns OK
	me->usbhMsc_isReady = USBH_MSC_IsReady(&me->usbh);

	// Handle
	if (me->usbh_event == HOST_USER_UNRECOVERED_ERROR)
	{
		// Trace
		ALX_USB_TRACE_ERR("FAIL: AlxUsb_Handle() HOST_USER_UNRECOVERED_ERROR");

		// Reset
		Alx_Status status = AlxUsb_Reset(me);
		if (status != Alx_Ok)
		{
			ALX_USB_TRACE_ERR("FAIL: AlxUsb_Reset() status %ld", status);
			return Alx_Err;
		}
	}
	else if (me->usbh_event == HOST_USER_CLASS_ACTIVE && me->usbhMsc_isReady)
	{
		me->isReady = true;
	}
	else
	{
		me->isReady = false;
	}
	#endif


	//------------------------------------------------------------------------------
	// Return
	//------------------------------------------------------------------------------
	return Alx_Ok;
}

/**
  * @brief
  * @param[in,out]	me
  * @retval			true
  * @retval			false
  */
bool AlxUsb_IsReady(AlxUsb* me)
{
	// Assert
	ALX_USB_ASSERT(me->wasCtorCalled == true);
	ALX_USB_ASSERT(me->isInit == true);

	// Return
	return me->isReady;
}

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		addr
  * @param[in]		data
  * @param[out]		len
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxUsb_Read(AlxUsb* me, uint32_t addr, uint8_t* data, uint32_t len)
{
	// Assert
	ALX_USB_ASSERT(me->wasCtorCalled == true);
	ALX_USB_ASSERT(me->isInit == true);
	ALX_USB_ASSERT(0 < len);

	// Check
	if (!me->isReady)
	{
		ALX_USB_TRACE_ERR("FAIL: isReady() addr %lu len %lu", addr, len);
		return Alx_Err;
	}

	// Read
	#if defined(ALX_USBH)
	USBH_StatusTypeDef status = USBH_MSC_Read(&me->usbh, 0, addr, data, len);
	if (status != USBH_OK)
	{
		ALX_USB_TRACE_ERR("FAIL: USBH_MSC_Read() status %ld addr %lu len %lu", status, addr, len);
		return Alx_Err;
	}
	#endif

	// Return
	return Alx_Ok;
}

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		addr
  * @param[in]		data
  * @param[in]		len
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxUsb_Write(AlxUsb* me, uint32_t addr, uint8_t* data, uint32_t len)
{
	// Assert
	ALX_USB_ASSERT(me->wasCtorCalled == true);
	ALX_USB_ASSERT(me->isInit == true);
	ALX_USB_ASSERT(0 < len);

	// Check
	if (!me->isReady)
	{
		ALX_USB_TRACE_ERR("FAIL: isReady() addr %lu len %lu", addr, len);
		return Alx_Err;
	}

	// Write
	#if defined(ALX_USBH)
	USBH_StatusTypeDef status = USBH_MSC_Write(&me->usbh, 0, addr, data, len);
	if (status != USBH_OK)
	{
		ALX_USB_TRACE_ERR("FAIL: USBH_MSC_Write() status %ld addr %lu len %lu", status, addr, len);
		return Alx_Err;
	}
	#endif

	// Return
	return Alx_Ok;
}

/**
  * @brief
  * @param[in,out]	me
  */
void AlxUsb_Irq_Handle(AlxUsb* me)
{
	HAL_HCD_IRQHandler(&me->hhcd);
}


//*******************************************************************************
// Private Functions
//*******************************************************************************


//------------------------------------------------------------------------------
// Specific
//------------------------------------------------------------------------------
#if defined(ALX_USBH)
static void AlxUsb_Event_Callback(USBH_HandleTypeDef* phost, uint8_t id)
{
	// Void
	(void)phost;

	// Set
	alxUsb_me->usbh_event = id;

	// Trace
	ALX_USB_TRACE_INF("AlxUsb_Event_Callback(%lu)", id);
}
#endif


//------------------------------------------------------------------------------
// General
//------------------------------------------------------------------------------
static Alx_Status AlxUsb_Reset(AlxUsb* me)
{
	// Local variables
	#if defined(ALX_USBH)
	USBH_StatusTypeDef status = USBH_FAIL;

	// Stop USB
	status = USBH_Stop(&me->usbh);
	if (status != USBH_OK)
	{
		ALX_USB_TRACE_ERR("FAIL: USBH_Stop() status %ld", status);
		return Alx_Err;
	}

	// DeInit USB
	status = USBH_DeInit(&me->usbh);
	if (status != USBH_OK)
	{
		ALX_USB_TRACE_ERR("FAIL: USBH_DeInit() status %ld", status);
		return Alx_Err;
	}
	#endif

	// Disable USB periphery IRQ
	AlxUsb_Periph_DisableIrq(me);

	// Force USB periphery reset
	AlxUsb_Periph_ForceReset(me);

	// Clear
	me->usbh_event = 0;
	me->usbhMsc_isReady = false;
	me->isReady = false;

	// Clear isInit
	me->isInit = false;

	// Release USB periphery reset
	AlxUsb_Periph_ReleaseReset(me);

	// Enable USB periphery IRQ
	AlxUsb_Periph_EnableIrq(me);

	// Init USB
	#if defined(ALX_USBH)
	status = USBH_Init(&me->usbh, AlxUsb_Event_Callback, 0);
	if (status != USBH_OK)
	{
		ALX_USB_TRACE_ERR("FAIL: USBH_Init() status %ld", status);
		return Alx_Err;
	}

	// Register USB class
	status = USBH_RegisterClass(&me->usbh, USBH_MSC_CLASS);
	if (status != USBH_OK)
	{
		ALX_USB_TRACE_ERR("FAIL: USBH_RegisterClass() status %ld", status);
		return Alx_Err;
	}

	// Start USB
	status = USBH_Start(&me->usbh);
	if (status != USBH_OK)
	{
		ALX_USB_TRACE_ERR("FAIL: USBH_Start() status %ld", status);
		return Alx_Err;
	}
	#endif

	// Set isInit
	me->isInit = true;

	// Return
	return Alx_Ok;
}
static void AlxUsb_Periph_EnableClk(AlxUsb* me)
{
	#if defined(USB_OTG_FS)
	if (me->hhcd.Instance == USB_OTG_FS)	{ __HAL_RCC_USB_OTG_FS_CLK_ENABLE(); return; }
	#endif
	#if defined(USB_OTG_HS)
	if (me->hhcd.Instance == USB_OTG_HS)	{ __HAL_RCC_USB_OTG_HS_CLK_ENABLE(); return; }
	#endif

	ALX_USB_ASSERT(false);	// We should never get here
}
static void AlxUsb_Periph_DisableClk(AlxUsb* me)
{
	#if defined(USB_OTG_FS)
	if (me->hhcd.Instance == USB_OTG_FS)	{ __HAL_RCC_USB_OTG_FS_CLK_DISABLE(); return; }
	#endif
	#if defined(USB_OTG_HS)
	if (me->hhcd.Instance == USB_OTG_HS)	{ __HAL_RCC_USB_OTG_HS_CLK_DISABLE(); return; }
	#endif

	ALX_USB_ASSERT(false);	// We should never get here
}
static void AlxUsb_Periph_ForceReset(AlxUsb* me)
{
	#if defined(USB_OTG_FS)
	if (me->hhcd.Instance == USB_OTG_FS)	{ __HAL_RCC_USB_OTG_FS_FORCE_RESET(); return; }
	#endif
	#if defined(USB_OTG_HS)
	if (me->hhcd.Instance == USB_OTG_HS)	{ __HAL_RCC_USB_OTG_HS_FORCE_RESET(); return; }
	#endif

	ALX_USB_ASSERT(false);	// We should never get here
}
static void AlxUsb_Periph_ReleaseReset(AlxUsb* me)
{
	#if defined(USB_OTG_FS)
	if (me->hhcd.Instance == USB_OTG_FS)	{ __HAL_RCC_USB_OTG_FS_RELEASE_RESET(); return; }
	#endif
	#if defined(USB_OTG_HS)
	if (me->hhcd.Instance == USB_OTG_HS)	{ __HAL_RCC_USB_OTG_HS_RELEASE_RESET(); return; }
	#endif

	ALX_USB_ASSERT(false);	// We should never get here
}
static void AlxUsb_Periph_EnableIrq(AlxUsb* me)
{
	#if defined(USB_OTG_FS)
	if (me->hhcd.Instance == USB_OTG_FS)	{ HAL_NVIC_SetPriority(OTG_FS_IRQn, me->irqPriority, 0); HAL_NVIC_EnableIRQ(OTG_FS_IRQn); return; }
	#endif
	#if defined(USB_OTG_HS)
	if (me->hhcd.Instance == USB_OTG_HS)	{ HAL_NVIC_SetPriority(OTG_HS_IRQn, me->irqPriority, 0); HAL_NVIC_EnableIRQ(OTG_HS_IRQn); return; }
	#endif

	ALX_USB_ASSERT(false);	// We should never get here
}
static void AlxUsb_Periph_DisableIrq(AlxUsb* me)
{
	#if defined(USB_OTG_FS)
	if (me->hhcd.Instance == USB_OTG_FS)	{ HAL_NVIC_DisableIRQ(OTG_FS_IRQn); HAL_NVIC_ClearPendingIRQ(OTG_FS_IRQn); return; }
	#endif
	#if defined(USB_OTG_HS)
	if (me->hhcd.Instance == USB_OTG_HS)	{ HAL_NVIC_DisableIRQ(OTG_HS_IRQn); HAL_NVIC_ClearPendingIRQ(OTG_HS_IRQn); return; }
	#endif

	ALX_USB_ASSERT(false);	// We should never get here
}


//******************************************************************************
// Weak Functions
//******************************************************************************
void HAL_HCD_SOF_Callback(HCD_HandleTypeDef* hhcd)
{
	#if defined(ALX_USBH)
	USBH_LL_IncTimer(hhcd->pData);
	#endif
}
void HAL_HCD_Connect_Callback(HCD_HandleTypeDef* hhcd)
{
	#if defined(ALX_USBH)
	USBH_LL_Connect(hhcd->pData);	// Always return OK
	#endif
}
void HAL_HCD_Disconnect_Callback(HCD_HandleTypeDef* hhcd)
{
	#if defined(ALX_USBH)
	USBH_LL_Disconnect(hhcd->pData);	// Always return OK
	#endif
}
void HAL_HCD_PortEnabled_Callback(HCD_HandleTypeDef* hhcd)
{
	#if defined(ALX_USBH)
	USBH_LL_PortEnabled(hhcd->pData);
	#endif
}
void HAL_HCD_PortDisabled_Callback(HCD_HandleTypeDef* hhcd)
{
	#if defined(ALX_USBH)
	USBH_LL_PortDisabled(hhcd->pData);
	#endif
}


//*******************************************************************************
// USBH Library - Functions implementation based on usbh_conf_template.c
//*******************************************************************************
#if defined(ALX_USBH)
USBH_StatusTypeDef USBH_LL_Init(USBH_HandleTypeDef* phost)
{
	// Context
	HCD_HandleTypeDef* hhcd = (HCD_HandleTypeDef*)phost->pData;

	// Init
	HAL_StatusTypeDef status = HAL_HCD_Init(hhcd);
	if (status != HAL_OK)
	{
		ALX_USB_TRACE_ERR("FAIL: HAL_HCD_Init() status %ld", status);
		return USBH_UNRECOVERED_ERROR;
	}

	// Set
	USBH_LL_SetTimer(phost, HAL_HCD_GetCurrentFrame(hhcd));

	// Return
	return USBH_OK;
}
USBH_StatusTypeDef USBH_LL_DeInit(USBH_HandleTypeDef* phost)
{
	// DeInit
	HAL_StatusTypeDef status = HAL_HCD_DeInit(phost->pData);
	if (status != HAL_OK)
	{
		ALX_USB_TRACE_ERR("FAIL: HAL_HCD_DeInit() status %ld", status);
		return USBH_UNRECOVERED_ERROR;
	}

	// Return
	return USBH_OK;
}
USBH_StatusTypeDef USBH_LL_Start(USBH_HandleTypeDef* phost)
{
	// Start
	HAL_StatusTypeDef status = HAL_HCD_Start(phost->pData);
	if (status != HAL_OK)
	{
		ALX_USB_TRACE_ERR("FAIL: HAL_HCD_Start() status %ld", status);
		return USBH_UNRECOVERED_ERROR;
	}

	// Return
	return USBH_OK;
}
USBH_StatusTypeDef USBH_LL_Stop(USBH_HandleTypeDef* phost)
{
	// Stop
	HAL_StatusTypeDef status = HAL_HCD_Stop(phost->pData);
	if (status != HAL_OK)
	{
		ALX_USB_TRACE_ERR("FAIL: HAL_HCD_Stop() status %ld", status);
		return USBH_UNRECOVERED_ERROR;
	}

	// Return
	return USBH_OK;
}
// USBH_LL_Connect
// USBH_LL_Disconnect
USBH_SpeedTypeDef USBH_LL_GetSpeed(USBH_HandleTypeDef* phost)
{
	// Get speed
	uint32_t speed = HAL_HCD_GetCurrentSpeed(phost->pData);

	// Return
	if (speed == HCD_SPEED_HIGH)
	{
		return USBH_SPEED_HIGH;
	}
	else if (speed == HCD_SPEED_FULL)
	{
		return USBH_SPEED_FULL;
	}
	else if (speed == HCD_SPEED_LOW)
	{
		return USBH_SPEED_LOW;
	}
	else
	{
		ALX_USB_ASSERT(false);	// We should never get here
		return 0;
	}
}
USBH_StatusTypeDef USBH_LL_ResetPort(USBH_HandleTypeDef* phost)
{
	// Reset port
	HAL_StatusTypeDef status = HAL_HCD_ResetPort(phost->pData);
	if (status != HAL_OK)
	{
		ALX_USB_TRACE_ERR("FAIL: HAL_HCD_ResetPort() status %ld", status);
		return USBH_UNRECOVERED_ERROR;
	}

	// Return
	return USBH_OK;
}
uint32_t USBH_LL_GetLastXferSize(USBH_HandleTypeDef* phost, uint8_t pipe)
{
	// Return
	return HAL_HCD_HC_GetXferCount(phost->pData, pipe);
}
USBH_StatusTypeDef USBH_LL_DriverVBUS(USBH_HandleTypeDef* phost, uint8_t state)
{
	// Void
	(void)(phost);
	(void)(state);

	// Return
	return USBH_OK;
}
USBH_StatusTypeDef USBH_LL_OpenPipe(USBH_HandleTypeDef* phost, uint8_t pipe, uint8_t epnum, uint8_t dev_address, uint8_t speed, uint8_t ep_type, uint16_t mps)
{
	// Open pipe
	HAL_StatusTypeDef status = HAL_HCD_HC_Init(phost->pData, pipe, epnum, dev_address, speed, ep_type, mps);
	if (status != HAL_OK)
	{
		ALX_USB_TRACE_ERR("FAIL: HAL_HCD_HC_Init() status %ld", status);
		return USBH_UNRECOVERED_ERROR;
	}

	// Return
	return USBH_OK;
}
USBH_StatusTypeDef USBH_LL_ClosePipe(USBH_HandleTypeDef* phost, uint8_t pipe)
{
	// Halt pipe
	HAL_StatusTypeDef status = HAL_HCD_HC_Halt(phost->pData, pipe);
	if (status != HAL_OK)
	{
		ALX_USB_TRACE_ERR("FAIL: HAL_HCD_HC_Halt() status %ld", status);
		return USBH_UNRECOVERED_ERROR;
	}

	// Return
	return USBH_OK;
}
USBH_StatusTypeDef USBH_LL_SubmitURB(USBH_HandleTypeDef* phost, uint8_t pipe, uint8_t direction, uint8_t ep_type, uint8_t token, uint8_t* pbuff, uint16_t length, uint8_t do_ping)
{
	// Submit URB
	HAL_StatusTypeDef status = HAL_HCD_HC_SubmitRequest(phost->pData, pipe,direction, ep_type, token, pbuff, length, do_ping);
	if (status != HAL_OK)
	{
		ALX_USB_TRACE_ERR("FAIL: HAL_HCD_HC_SubmitRequest() status %ld", status);
		return USBH_UNRECOVERED_ERROR;
	}

	// Return
	return USBH_OK;
}
USBH_URBStateTypeDef USBH_LL_GetURBState(USBH_HandleTypeDef* phost, uint8_t pipe)
{
	// Return
	return (USBH_URBStateTypeDef)HAL_HCD_HC_GetURBState(phost->pData, pipe);
}
// USBH_LL_NotifyURBChange
USBH_StatusTypeDef USBH_LL_SetToggle(USBH_HandleTypeDef* phost, uint8_t pipe, uint8_t toggle)
{
	// Context
	HCD_HandleTypeDef* hhcd = (HCD_HandleTypeDef*)phost->pData;

	// Set toggle
	if (hhcd->hc[pipe].ep_is_in)
	{
		hhcd->hc[pipe].toggle_in = toggle;
	}
	else
	{
		hhcd->hc[pipe].toggle_out = toggle;
	}

	// Return
	return USBH_OK;
}
uint8_t USBH_LL_GetToggle(USBH_HandleTypeDef* phost, uint8_t pipe)
{
	// Context
	HCD_HandleTypeDef* hhcd = (HCD_HandleTypeDef*)phost->pData;

	// Get toggle
	uint8_t toggle = 0;
	if (hhcd->hc[pipe].ep_is_in)
	{
		toggle = hhcd->hc[pipe].toggle_in;
	}
	else
	{
		toggle = hhcd->hc[pipe].toggle_out;
	}

	// Return
	return toggle;
}
// USBH_LL_SetTimer
// USBH_LL_IncTimer
void USBH_Delay(uint32_t Delay)
{
	HAL_Delay(Delay);
}
#endif


#endif	// #if defined(ALX_C_LIB) && defined(ALX_STM32F7)
