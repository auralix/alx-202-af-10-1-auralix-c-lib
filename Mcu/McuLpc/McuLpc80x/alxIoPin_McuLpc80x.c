/**
  ******************************************************************************
  * @file alxIoPin_McuLpc80x.c
  * @brief Auralix C Library - ALX IO Pin Module
  * @version $LastChangedRevision: 4270 $
  * @date $LastChangedDate: 2021-03-05 19:02:52 +0100 (Fri, 05 Mar 2021) $
  ******************************************************************************
  */

//******************************************************************************
// Includes
//******************************************************************************
#include "alxIoPin_McuLpc80x.h"
#include "alxIoPin.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_LPC80x)


//******************************************************************************
// Private Functions
//******************************************************************************
static swm_port_pin_type_t AlxIoPin_GetSwmPortPinIndex(AlxIoPin* me);
static bool AlxIoPin_CheckIfSwmUsed(AlxIoPin* me);
static swm_select_movable_t AlxIoPin_GetSwmMoveFunc(AlxIoPin* me);
static swm_select_fixed_pin_t AlxIoPin_GetSwmMFixFunc(AlxIoPin* me);
static syscon_connection_t AlxIoPin_GetIrqPortPinSel(AlxIoPin* me);
static IRQn_Type AlxIoPin_GetIrqType(AlxIoPin* me);
static void AlxIoPin_PINT_Init(AlxIoPin* me, PINT_Type* base); // MF: I have to rewrite FSL func without "Periph_Reset" in it so we can init more pins


//******************************************************************************
// Weak Functions
//******************************************************************************
void AlxIoPinIrq_Foreground_Callback_Pin0();
void AlxIoPinIrq_Foreground_Callback_Pin1();
void AlxIoPinIrq_Foreground_Callback_Pin2();
void AlxIoPinIrq_Foreground_Callback_Pin3();
void AlxIoPinIrq_Foreground_Callback_Pin4();
void AlxIoPinIrq_Foreground_Callback_Pin5();
void AlxIoPinIrq_Foreground_Callback_Pin6();
void AlxIoPinIrq_Foreground_Callback_Pin7();


//******************************************************************************
// Specific Functions
//******************************************************************************
void AlxIoPin_Ctor
(
	AlxIoPin* me,
	uint8_t port,
	uint8_t pin,
	AlxIoPin_Func func,
	uint32_t mode,
	bool isOpenDrain,
	bool dir,
	bool val,
	AlxIoPinIrq* irqPtr
)
{
	// Assert
	if ((alxDbgPin.pin == pin) && (alxDbgPin.port == port)) { ALX_IO_PIN_ASSERT(false); } // MF: Checks Pin and Port are nat the same as DBG Pin

	// Parameters
	me->port = port;
	me->pin = pin;
	me->func = func;
	me->mode = mode;
	me->isOpenDrain = isOpenDrain;
	me->dir = dir;
	me->val = val;
	me->irqPtr = irqPtr;

	// Variables
	me->swmFunc_isMovable = false;

	// Info
	me->isInit = false;
	me->wasCtorCalled = true;
}
void AlxIoPin_Init(AlxIoPin* me)
{
	ALX_IO_PIN_ASSERT(me->isInit == false);
	ALX_IO_PIN_ASSERT(me->wasCtorCalled == true);

	// #1 Set isInit attribute
	me->isInit = true;

	AlxIoPin_Write(me, me->val); // Set initial output value, before config

	// #2 Init IOCON
	uint8_t ioconPortPinIndex = AlxPROTECTED_IoPin_GetIoconPortPinIndex(me->pin, me->port);

	CLOCK_EnableClock(kCLOCK_Iocon);

	// Set Mode
	AlxPROTECTED_IoPin_SetIoconMode(me->pin, me->port, me->mode);

	// Set Open Drain
	if (me->isOpenDrain)
	{
		IOCON->PIO[ioconPortPinIndex] |= (0x1 << 10U);
	}
	else
	{
		IOCON->PIO[ioconPortPinIndex] &= ~(0x1 << 10U);
	}
	CLOCK_DisableClock(kCLOCK_Iocon);

	// #3.1 Init if SWM
	if (AlxIoPin_CheckIfSwmUsed(me))
	{
		CLOCK_EnableClock(kCLOCK_Swm);
		if (me->swmFunc_isMovable)
		{
			// #3.1.1 Init if Movable
			swm_select_movable_t swmMoveFunc = AlxIoPin_GetSwmMoveFunc(me);
			swm_port_pin_type_t swmPortPinIndex = AlxIoPin_GetSwmPortPinIndex(me);
			SWM_SetMovablePinSelect(SWM0, swmMoveFunc, swmPortPinIndex);
		}
		else
		{
			// #3.1.2 Init if Fixed
			swm_select_fixed_pin_t swmFixFunc = AlxIoPin_GetSwmMFixFunc(me);
			SWM_SetFixedPinSelect(SWM0, swmFixFunc, true);
		}
		CLOCK_DisableClock(kCLOCK_Swm);
	}

	// #3.2 Init if IRQ
	if (me->func == AlxIoPin_Func_IRQ)
	{
		syscon_connection_t irqPortPinSel = AlxIoPin_GetIrqPortPinSel(me);
		IRQn_Type irqType = AlxIoPin_GetIrqType(me);

		// #3.2.1 Attach IRQ to right Pin
		SYSCON_AttachSignal(SYSCON, me->irqPtr->irqPin, irqPortPinSel);

		// #3.2.2 Init PINT Periphery
		AlxIoPin_PINT_Init(me, PINT); // "EnableClk" happens here.

		// #3.2.3 Enable IRQ
		PINT_PinInterruptConfig(PINT, me->irqPtr->irqPin, me->irqPtr->irqType, ALX_NULL_PTR); // MF: "ALX_NULL_PTR" because we'll use "PIN_INTX_IRQHandler" from startup.s
		NVIC_SetPriority(irqType, (uint32_t)me->irqPtr->irqPriority); // MF: Set IRQ Priority
		PINT_EnableCallbackByIndex(PINT, me->irqPtr->irqPin); // MF: Enable IRQ
	}

	// #3.3 Init if GPIO
	if (me->func == AlxIoPin_Func_GPIO)
	{
		gpio_pin_config_t gpioConfig;

		if (me->dir)	gpioConfig.pinDirection = kGPIO_DigitalOutput;
		else			gpioConfig.pinDirection = kGPIO_DigitalInput;

		gpioConfig.outputLogic = 0U; // Clears output bit

		GPIO_PinInit(GPIO, me->port, me->pin, &gpioConfig);
	}

	AlxIoPin_Write(me, me->val); // Set initial output value, after config

	ALX_IO_PIN_ASSERT(me->isInit == true);
}
void AlxIoPin_DeInit(AlxIoPin* me)
{
	ALX_IO_PIN_ASSERT(me->isInit == true);
	ALX_IO_PIN_ASSERT(me->wasCtorCalled == true);

	// #1 DeInit IOCON
	uint8_t ioconPortPinIndex = AlxPROTECTED_IoPin_GetIoconPortPinIndex(me->pin, me->port);

	CLOCK_EnableClock(kCLOCK_Iocon);
	IOCON->PIO[ioconPortPinIndex] &= ~(0x1 << 3U); // Reset Mode (00) Inactive (no pull-down/pull-up)
	IOCON->PIO[ioconPortPinIndex] &= ~(0x1 << 4U);
	IOCON->PIO[ioconPortPinIndex] &= ~(0x1 << 10U); // Reset Open Drain (0)
	CLOCK_DisableClock(kCLOCK_Iocon);

	// #2.1 DeInit SWM if used
	if (AlxIoPin_CheckIfSwmUsed(me))
	{
		CLOCK_EnableClock(kCLOCK_Swm);
		if (me->swmFunc_isMovable)
		{
			swm_select_movable_t swmMoveFunc = AlxIoPin_GetSwmMoveFunc(me);
			SWM_SetMovablePinSelect(SWM0, swmMoveFunc, kSWM_PortPin_Reset);
		}
		else
		{
			swm_select_fixed_pin_t swmFixFunc = AlxIoPin_GetSwmMFixFunc(me);
			SWM_SetFixedPinSelect(SWM0, swmFixFunc, false);
		}
		CLOCK_DisableClock(kCLOCK_Swm);
	}

	// #2.2 DeInit if IRQ
	if (me->func == AlxIoPin_Func_IRQ)
	{
		PINT_DisableCallbackByIndex(PINT, me->irqPtr->irqPin); // MF: Disable IRQ
	}

	// #2.3 DeInit if GPIO
	if (me->func == AlxIoPin_Func_GPIO)
	{
		GPIO->DIR[me->port] &= ~(1U << me->pin); // Reset Dir (0)
	}

	// #3 Clear isInit attribute
	me->isInit = false;
}
bool AlxIoPin_Read(AlxIoPin* me)
{
	ALX_IO_PIN_ASSERT(me->isInit == true);
	ALX_IO_PIN_ASSERT(me->wasCtorCalled == true);

	if (GPIO_PinRead(GPIO, me->port, me->pin) == 1) { return true; }
	else { return false; }
}
void AlxIoPin_Write(AlxIoPin* me, bool val)
{
	(void)val;
	ALX_IO_PIN_ASSERT(me->isInit == true);
	ALX_IO_PIN_ASSERT(me->wasCtorCalled == true);

	if (val) { GPIO_PinWrite(GPIO, me->port, me->pin, 1U); }
	else { GPIO_PinWrite(GPIO, me->port, me->pin, 0U); }
}
void AlxIoPin_Set(AlxIoPin* me)
{
	ALX_IO_PIN_ASSERT(me->isInit == true);
	ALX_IO_PIN_ASSERT(me->wasCtorCalled == true);

	GPIO_PortSet(GPIO, me->port, (1U << me->pin));
}
void AlxIoPin_Reset(AlxIoPin* me)
{
	ALX_IO_PIN_ASSERT(me->isInit == true);
	ALX_IO_PIN_ASSERT(me->wasCtorCalled == true);

	GPIO_PortClear(GPIO, me->port, (1U << me->pin));
}
void AlxIoPin_Toggle(AlxIoPin* me)
{
	ALX_IO_PIN_ASSERT(me->isInit == true);
	ALX_IO_PIN_ASSERT(me->wasCtorCalled == true);

	GPIO_PortToggle(GPIO, me->port, (1U << me->pin));
}
void AlxIoPin_Lock(AlxIoPin* me)
{
	// TODO
}
void AlxIoPin_IrqHandler(AlxIoPin* me)
{
	// TODO
}
void AlxIoPin_Config_PullNone(AlxIoPin* me)
{
	// TODO
}
void AlxIoPin_Config_PullUp(AlxIoPin* me)
{
	// TODO
}
void AlxIoPin_Config_PullDown(AlxIoPin* me)
{
	// TODO
}
void AlxIoPin_Config_AssertOn(AlxIoPin* me)
{
	// TODO
}
void AlxIoPin_Config_AssertOff(AlxIoPin* me)
{
	// TODO
}


//******************************************************************************
// Private Functions
//******************************************************************************
static swm_port_pin_type_t AlxIoPin_GetSwmPortPinIndex(AlxIoPin* me)
{
	if (me->port == 0)		return me->pin;
	else					return (me->pin + 32U);
}
static bool AlxIoPin_CheckIfSwmUsed(AlxIoPin* me)
{
	#if defined(ALX_LPC80x)
	if (me->func <= 38)
	{
		me->swmFunc_isMovable = true;
		return true;
	}
	if ((me->func > 38) && (me->func <= 62))
	{
		me->swmFunc_isMovable = false;
		return true;
	}
	#endif

	#if defined(ALX_LPC84x)
	if (me->func <= 59)
	{
		me->func_isMovable = true;
		return true;
	}
	if ((me->func > 59) && (me->func <= 98))
	{
		me->func_isMovable = false;
		return true;
	}
	#endif

	else { return false; }
}
static swm_select_movable_t AlxIoPin_GetSwmMoveFunc(AlxIoPin* me)
{
	ALX_IO_PIN_ASSERT(me->swmFunc_isMovable == true);

	#if defined(ALX_LPC80x)
	if (me->func == AlxIoPin_Func_Swm_USART0_TXD)		return kSWM_USART0_TXD;
	if (me->func == AlxIoPin_Func_Swm_USART0_RXD)		return kSWM_USART0_RXD;
	if (me->func == AlxIoPin_Func_Swm_USART0_RTS)		return kSWM_USART0_RTS;
	if (me->func == AlxIoPin_Func_Swm_USART0_CTS)		return kSWM_USART0_CTS;
	if (me->func == AlxIoPin_Func_Swm_USART0_SCLK)		return kSWM_USART0_SCLK;

	if (me->func == AlxIoPin_Func_Swm_USART1_TXD)		return kSWM_USART1_TXD;
	if (me->func == AlxIoPin_Func_Swm_USART1_RXD)		return kSWM_USART1_RXD;
	if (me->func == AlxIoPin_Func_Swm_USART1_SCLK)		return kSWM_USART1_SCLK;

	if (me->func == AlxIoPin_Func_Swm_SPI0_SCK)			return kSWM_SPI0_SCK;
	if (me->func == AlxIoPin_Func_Swm_SPI0_MOSI)		return kSWM_SPI0_MOSI;
	if (me->func == AlxIoPin_Func_Swm_SPI0_MISO)		return kSWM_SPI0_MISO;
	if (me->func == AlxIoPin_Func_Swm_SPI0_SSEL0)		return kSWM_SPI0_SSEL0;
	if (me->func == AlxIoPin_Func_Swm_SPI0_SSEL1)		return kSWM_SPI0_SSEL1;

	if (me->func == AlxIoPin_Func_Swm_T0_CAP_CHN0)		return kSWM_T0_CAP_CHN0;
	if (me->func == AlxIoPin_Func_Swm_T0_CAP_CHN1)		return kSWM_T0_CAP_CHN1;
	if (me->func == AlxIoPin_Func_Swm_T0_CAP_CHN2)		return kSWM_T0_CAP_CHN2;

	if (me->func == AlxIoPin_Func_Swm_T0_MAT_CHN0)		return kSWM_T0_MAT_CHN0;
	if (me->func == AlxIoPin_Func_Swm_T0_MAT_CHN1)		return kSWM_T0_MAT_CHN1;
	if (me->func == AlxIoPin_Func_Swm_T0_MAT_CHN2)		return kSWM_T0_MAT_CHN2;
	if (me->func == AlxIoPin_Func_Swm_T0_MAT_CHN3)		return kSWM_T0_MAT_CHN3;

	if (me->func == AlxIoPin_Func_Swm_I2C0_SDA)			return kSWM_I2C0_SDA;
	if (me->func == AlxIoPin_Func_Swm_I2C0_SCL)			return kSWM_I2C0_SCL;

	if (me->func == AlxIoPin_Func_Swm_ACMP_OUT)			return kSWM_ACMP_OUT;
	if (me->func == AlxIoPin_Func_Swm_CLKOUT)			return kSWM_CLKOUT;
	if (me->func == AlxIoPin_Func_Swm_GPIO_INT_BMAT)	return kSWM_GPIO_INT_BMAT;

	if (me->func == AlxIoPin_Func_Swm_LVLSHFT_IN0)		return kSWM_LVLSHFT_IN0;
	if (me->func == AlxIoPin_Func_Swm_LVLSHFT_IN1)		return kSWM_LVLSHFT_IN1;
	if (me->func == AlxIoPin_Func_Swm_LVLSHFT_OUT0)		return kSWM_LVLSHFT_OUT0;
	if (me->func == AlxIoPin_Func_Swm_LVLSHFT_OUT1)		return kSWM_LVLSHFT_OUT1;

	if (me->func == AlxIoPin_Func_Swm_I2C1_SDA)			return kSWM_I2C1_SDA;
	if (me->func == AlxIoPin_Func_Swm_I2C1_SCL)			return kSWM_I2C1_SCL;

	if (me->func == AlxIoPin_Func_Swm_PLU_CLKIN_IN)		return kSWM_PLU_CLKIN_IN;

	if (me->func == AlxIoPin_Func_Swm_CAPT_X0)			return kSWM_CAPT_X0;
	if (me->func == AlxIoPin_Func_Swm_CAPT_X1)			return kSWM_CAPT_X1;
	if (me->func == AlxIoPin_Func_Swm_CAPT_X2)			return kSWM_CAPT_X2;
	if (me->func == AlxIoPin_Func_Swm_CAPT_X3)			return kSWM_CAPT_X3;
	if (me->func == AlxIoPin_Func_Swm_CAPT_X4)			return kSWM_CAPT_X4;
	if (me->func == AlxIoPin_Func_Swm_CAPT_YL)			return kSWM_CAPT_YL;
	if (me->func == AlxIoPin_Func_Swm_CAPT_YH)			return kSWM_CAPT_YH;
	#endif

	#if defined(ALX_LPC84x)
	if (me->func == AlxIoPin_Func_Swm_USART0_TXD)		return kSWM_USART0_TXD;
	if (me->func == AlxIoPin_Func_Swm_USART0_RXD)		return kSWM_USART0_RXD;
	if (me->func == AlxIoPin_Func_Swm_USART0_RTS)		return kSWM_USART0_RTS;
	if (me->func == AlxIoPin_Func_Swm_USART0_CTS)		return kSWM_USART0_CTS;
	if (me->func == AlxIoPin_Func_Swm_USART0_SCLK)		return kSWM_USART0_SCLK;

	if (me->func == AlxIoPin_Func_Swm_USART1_TXD)		return kSWM_USART1_TXD;
	if (me->func == AlxIoPin_Func_Swm_USART1_RXD)		return kSWM_USART1_RXD;
	if (me->func == AlxIoPin_Func_Swm_USART1_RTS)		return kSWM_USART1_RTS;
	if (me->func == AlxIoPin_Func_Swm_USART1_CTS)		return kSWM_USART1_CTS;
	if (me->func == AlxIoPin_Func_Swm_USART1_SCLK)		return kSWM_USART1_SCLK;

	if (me->func == AlxIoPin_Func_Swm_USART2_TXD)		return kSWM_USART2_TXD;
	if (me->func == AlxIoPin_Func_Swm_USART2_RXD)		return kSWM_USART2_RXD;
	if (me->func == AlxIoPin_Func_Swm_USART2_RTS)		return kSWM_USART2_RTS;
	if (me->func == AlxIoPin_Func_Swm_USART2_CTS)		return kSWM_USART2_CTS;
	if (me->func == AlxIoPin_Func_Swm_USART2_SCLK)		return kSWM_USART2_SCLK;

	if (me->func == AlxIoPin_Func_Swm_SPI0_SCK)			return kSWM_SPI0_SCK;
	if (me->func == AlxIoPin_Func_Swm_SPI0_MOSI)		return kSWM_SPI0_MOSI;
	if (me->func == AlxIoPin_Func_Swm_SPI0_MISO)		return kSWM_SPI0_MISO;
	if (me->func == AlxIoPin_Func_Swm_SPI0_SSEL0)		return kSWM_SPI0_SSEL0;
	if (me->func == AlxIoPin_Func_Swm_SPI0_SSEL1)		return kSWM_SPI0_SSEL1;
	if (me->func == AlxIoPin_Func_Swm_SPI0_SSEL2)		return kSWM_SPI0_SSEL2;
	if (me->func == AlxIoPin_Func_Swm_SPI0_SSEL3)		return kSWM_SPI0_SSEL3;

	if (me->func == AlxIoPin_Func_Swm_SPI1_SCK)			return kSWM_SPI1_SCK;
	if (me->func == AlxIoPin_Func_Swm_SPI1_MOSI)		return kSWM_SPI1_MOSI;
	if (me->func == AlxIoPin_Func_Swm_SPI1_MISO)		return kSWM_SPI1_MISO;
	if (me->func == AlxIoPin_Func_Swm_SPI1_SSEL0)		return kSWM_SPI1_SSEL0;
	if (me->func == AlxIoPin_Func_Swm_SPI1_SSEL1)		return kSWM_SPI1_SSEL1;

	if (me->func == AlxIoPin_Func_Swm_SCT_PIN0)			return kSWM_SCT_PIN0;
	if (me->func == AlxIoPin_Func_Swm_SCT_PIN1)			return kSWM_SCT_PIN1;
	if (me->func == AlxIoPin_Func_Swm_SCT_PIN2)			return kSWM_SCT_PIN2;
	if (me->func == AlxIoPin_Func_Swm_SCT_PIN3)			return kSWM_SCT_PIN3;
	if (me->func == AlxIoPin_Func_Swm_SCT_OUT0)			return kSWM_SCT_OUT0;
	if (me->func == AlxIoPin_Func_Swm_SCT_OUT1)			return kSWM_SCT_OUT1;
	if (me->func == AlxIoPin_Func_Swm_SCT_OUT2)			return kSWM_SCT_OUT2;
	if (me->func == AlxIoPin_Func_Swm_SCT_OUT3)			return kSWM_SCT_OUT3;
	if (me->func == AlxIoPin_Func_Swm_SCT_OUT4)			return kSWM_SCT_OUT4;
	if (me->func == AlxIoPin_Func_Swm_SCT_OUT5)			return kSWM_SCT_OUT5;
	if (me->func == AlxIoPin_Func_Swm_SCT_OUT6)			return kSWM_SCT_OUT6;

	if (me->func == AlxIoPin_Func_Swm_I2C1_SDA)			return kSWM_I2C1_SDA;
	if (me->func == AlxIoPin_Func_Swm_I2C1_SCL)			return kSWM_I2C1_SCL;
	if (me->func == AlxIoPin_Func_Swm_I2C2_SDA)			return kSWM_I2C2_SDA;
	if (me->func == AlxIoPin_Func_Swm_I2C2_SCL)			return kSWM_I2C2_SCL;
	if (me->func == AlxIoPin_Func_Swm_I2C3_SDA)			return kSWM_I2C3_SDA;
	if (me->func == AlxIoPin_Func_Swm_I2C3_SCL)			return kSWM_I2C3_SCL;

	if (me->func == AlxIoPin_Func_Swm_ACMP_OUT)			return kSWM_ACMP_OUT;
	if (me->func == AlxIoPin_Func_Swm_CLKOUT)			return kSWM_CLKOUT;
	if (me->func == AlxIoPin_Func_Swm_GPIO_INT_BMAT)	return kSWM_GPIO_INT_BMAT;

	if (me->func == AlxIoPin_Func_Swm_USART3_TXD)		return kSWM_USART3_TXD;
	if (me->func == AlxIoPin_Func_Swm_USART3_RXD)		return kSWM_USART3_RXD;
	if (me->func == AlxIoPin_Func_Swm_USART3_SCLK)		return kSWM_USART3_SCLK;

	if (me->func == AlxIoPin_Func_Swm_USART4_TXD)		return kSWM_USART4_TXD;
	if (me->func == AlxIoPin_Func_Swm_USART4_RXD)		return kSWM_USART4_RXD;
	if (me->func == AlxIoPin_Func_Swm_USART4_SCLK)		return kSWM_USART4_SCLK;

	if (me->func == AlxIoPin_Func_Swm_T0_MAT_CHN0)		return kSWM_T0_MAT_CHN0;
	if (me->func == AlxIoPin_Func_Swm_T0_MAT_CHN1)		return kSWM_T0_MAT_CHN1;
	if (me->func == AlxIoPin_Func_Swm_T0_MAT_CHN2)		return kSWM_T0_MAT_CHN2;
	if (me->func == AlxIoPin_Func_Swm_T0_MAT_CHN3)		return kSWM_T0_MAT_CHN3;
	if (me->func == AlxIoPin_Func_Swm_T0_CAP_CHN0)		return kSWM_T0_CAP_CHN0;
	if (me->func == AlxIoPin_Func_Swm_T0_CAP_CHN1)		return kSWM_T0_CAP_CHN1;
	if (me->func == AlxIoPin_Func_Swm_T0_CAP_CHN2)		return kSWM_T0_CAP_CHN2;
	#endif

	ALX_IO_PIN_ASSERT(false); // We shouldn't get here
}
static swm_select_fixed_pin_t AlxIoPin_GetSwmMFixFunc(AlxIoPin* me)
{
	ALX_IO_PIN_ASSERT(me->swmFunc_isMovable == false);

	#if defined(ALX_LPC80x)
	if (me->func == AlxIoPin_Func_Swm_ACMP_INPUT1)		return kSWM_ACMP_INPUT1;
	if (me->func == AlxIoPin_Func_Swm_ACMP_INPUT2)		return kSWM_ACMP_INPUT2;
	if (me->func == AlxIoPin_Func_Swm_ACMP_INPUT3)		return kSWM_ACMP_INPUT3;
	if (me->func == AlxIoPin_Func_Swm_ACMP_INPUT4)		return kSWM_ACMP_INPUT4;

	if (me->func == AlxIoPin_Func_Swm_SWCLK)			return kSWM_SWCLK;
	if (me->func == AlxIoPin_Func_Swm_SWDIO)			return kSWM_SWDIO;
	if (me->func == AlxIoPin_Func_Swm_RESETN)			return kSWM_RESETN;
	if (me->func == AlxIoPin_Func_Swm_CLKIN)			return kSWM_CLKIN;
	if (me->func == AlxIoPin_Func_Swm_WKCLKIN)			return kSWM_WKCLKIN;
	if (me->func == AlxIoPin_Func_Swm_VDDCMP)			return kSWM_VDDCMP;

	if (me->func == AlxIoPin_Func_Swm_ADC_CHN0)			return kSWM_ADC_CHN0;
	if (me->func == AlxIoPin_Func_Swm_ADC_CHN1)			return kSWM_ADC_CHN1;
	if (me->func == AlxIoPin_Func_Swm_ADC_CHN2)			return kSWM_ADC_CHN2;
	if (me->func == AlxIoPin_Func_Swm_ADC_CHN3)			return kSWM_ADC_CHN3;
	if (me->func == AlxIoPin_Func_Swm_ADC_CHN4)			return kSWM_ADC_CHN4;
	if (me->func == AlxIoPin_Func_Swm_ADC_CHN5)			return kSWM_ADC_CHN5;
	if (me->func == AlxIoPin_Func_Swm_ADC_CHN6)			return kSWM_ADC_CHN6;
	if (me->func == AlxIoPin_Func_Swm_ADC_CHN7)			return kSWM_ADC_CHN7;
	if (me->func == AlxIoPin_Func_Swm_ADC_CHN8)			return kSWM_ADC_CHN8;
	if (me->func == AlxIoPin_Func_Swm_ADC_CHN9)			return kSWM_ADC_CHN9;
	if (me->func == AlxIoPin_Func_Swm_ADC_CHN10)		return kSWM_ADC_CHN10;
	if (me->func == AlxIoPin_Func_Swm_ADC_CHN11)		return kSWM_ADC_CHN11;

	if (me->func == AlxIoPin_Func_Swm_ACMP_INPUT5)		return kSWM_ACMP_INPUT5;
	if (me->func == AlxIoPin_Func_Swm_DAC_OUT0)		return kSWM_DAC_OUT0;
	#endif

	#if defined(ALX_LPC84x)
	if (me->func == AlxIoPin_Func_Swm_ACMP_INPUT1)		return kSWM_ACMP_INPUT1;
	if (me->func == AlxIoPin_Func_Swm_ACMP_INPUT2)		return kSWM_ACMP_INPUT2;
	if (me->func == AlxIoPin_Func_Swm_ACMP_INPUT3)		return kSWM_ACMP_INPUT3;
	if (me->func == AlxIoPin_Func_Swm_ACMP_INPUT4)		return kSWM_ACMP_INPUT4;
	if (me->func == AlxIoPin_Func_Swm_ACMP_INPUT5)		return kSWM_ACMP_INPUT5;

	if (me->func == AlxIoPin_Func_Swm_SWCLK)			return kSWM_SWCLK;
	if (me->func == AlxIoPin_Func_Swm_SWDIO)			return kSWM_SWDIO;
	if (me->func == AlxIoPin_Func_Swm_XTALIN)			return kSWM_XTALIN;
	if (me->func == AlxIoPin_Func_Swm_XTALOUT)			return kSWM_XTALOUT;
	if (me->func == AlxIoPin_Func_Swm_RESETN)			return kSWM_RESETN;
	if (me->func == AlxIoPin_Func_Swm_CLKIN)			return kSWM_CLKIN;
	if (me->func == AlxIoPin_Func_Swm_VDDCMP)			return kSWM_VDDCMP;
	if (me->func == AlxIoPin_Func_Swm_I2C0_SDA)			return kSWM_I2C0_SDA;
	if (me->func == AlxIoPin_Func_Swm_I2C0_SCL)			return kSWM_I2C0_SCL;

	if (me->func == AlxIoPin_Func_Swm_ADC_CHN0)			return kSWM_ADC_CHN0;
	if (me->func == AlxIoPin_Func_Swm_ADC_CHN1)			return kSWM_ADC_CHN1;
	if (me->func == AlxIoPin_Func_Swm_ADC_CHN2)			return kSWM_ADC_CHN2;
	if (me->func == AlxIoPin_Func_Swm_ADC_CHN3)			return kSWM_ADC_CHN3;
	if (me->func == AlxIoPin_Func_Swm_ADC_CHN4)			return kSWM_ADC_CHN4;
	if (me->func == AlxIoPin_Func_Swm_ADC_CHN5)			return kSWM_ADC_CHN5;
	if (me->func == AlxIoPin_Func_Swm_ADC_CHN6)			return kSWM_ADC_CHN6;
	if (me->func == AlxIoPin_Func_Swm_ADC_CHN7)			return kSWM_ADC_CHN7;
	if (me->func == AlxIoPin_Func_Swm_ADC_CHN8)			return kSWM_ADC_CHN8;
	if (me->func == AlxIoPin_Func_Swm_ADC_CHN9)			return kSWM_ADC_CHN9;
	if (me->func == AlxIoPin_Func_Swm_ADC_CHN10)		return kSWM_ADC_CHN10;
	if (me->func == AlxIoPin_Func_Swm_ADC_CHN11)		return kSWM_ADC_CHN11;

	if (me->func == AlxIoPin_Func_Swm_DAC_OUT0)			return kSWM_DAC_OUT0;
	if (me->func == AlxIoPin_Func_Swm_DAC_OUT1)			return kSWM_DAC_OUT1;
	if (me->func == AlxIoPin_Func_Swm_CAPT_X0)			return kSWM_CAPT_X0;
	if (me->func == AlxIoPin_Func_Swm_CAPT_X1)			return kSWM_CAPT_X1;
	if (me->func == AlxIoPin_Func_Swm_CAPT_X2)			return kSWM_CAPT_X2;
	if (me->func == AlxIoPin_Func_Swm_CAPT_X3)			return kSWM_CAPT_X3;
	if (me->func == AlxIoPin_Func_Swm_CAPT_X4)			return kSWM_CAPT_X4;
	if (me->func == AlxIoPin_Func_Swm_CAPT_X5)			return kSWM_CAPT_X5;
	if (me->func == AlxIoPin_Func_Swm_CAPT_X6)			return kSWM_CAPT_X6;
	if (me->func == AlxIoPin_Func_Swm_CAPT_X7)			return kSWM_CAPT_X7;
	if (me->func == AlxIoPin_Func_Swm_CAPT_X8)			return kSWM_CAPT_X8;
	if (me->func == AlxIoPin_Func_Swm_CAPT_YL)			return kSWM_CAPT_YL;
	if (me->func == AlxIoPin_Func_Swm_CAPT_YH)			return kSWM_CAPT_YH;
	#endif

	ALX_IO_PIN_ASSERT(false); // We shouldn't get here
}
static syscon_connection_t AlxIoPin_GetIrqPortPinSel(AlxIoPin* me)
{
	if (me->port == 0 && me->pin == 0)				return kSYSCON_GpioPort0Pin0ToPintsel;
	if (me->port == 0 && me->pin == 1)				return kSYSCON_GpioPort0Pin1ToPintsel;
	if (me->port == 0 && me->pin == 2)				return kSYSCON_GpioPort0Pin2ToPintsel;
	if (me->port == 0 && me->pin == 3)				return kSYSCON_GpioPort0Pin3ToPintsel;
	if (me->port == 0 && me->pin == 4)				return kSYSCON_GpioPort0Pin4ToPintsel;
	if (me->port == 0 && me->pin == 5)				return kSYSCON_GpioPort0Pin5ToPintsel;

	if (me->port == 0 && me->pin == 7)				return kSYSCON_GpioPort0Pin7ToPintsel;
	if (me->port == 0 && me->pin == 8)				return kSYSCON_GpioPort0Pin8ToPintsel;
	if (me->port == 0 && me->pin == 9)				return kSYSCON_GpioPort0Pin9ToPintsel;
	if (me->port == 0 && me->pin == 10)				return kSYSCON_GpioPort0Pin10ToPintsel;
	if (me->port == 0 && me->pin == 11)				return kSYSCON_GpioPort0Pin11ToPintsel;
	if (me->port == 0 && me->pin == 12)				return kSYSCON_GpioPort0Pin12ToPintsel;
	if (me->port == 0 && me->pin == 13)				return kSYSCON_GpioPort0Pin13ToPintsel;
	if (me->port == 0 && me->pin == 14)				return kSYSCON_GpioPort0Pin14ToPintsel;
	if (me->port == 0 && me->pin == 15)				return kSYSCON_GpioPort0Pin15ToPintsel;
	if (me->port == 0 && me->pin == 16)				return kSYSCON_GpioPort0Pin16ToPintsel;
	if (me->port == 0 && me->pin == 17)				return kSYSCON_GpioPort0Pin17ToPintsel;
	if (me->port == 0 && me->pin == 18)				return kSYSCON_GpioPort0Pin18ToPintsel;
	if (me->port == 0 && me->pin == 19)				return kSYSCON_GpioPort0Pin19ToPintsel;
	if (me->port == 0 && me->pin == 20)				return kSYSCON_GpioPort0Pin20ToPintsel;
	if (me->port == 0 && me->pin == 21)				return kSYSCON_GpioPort0Pin21ToPintsel;
	if (me->port == 0 && me->pin == 22)				return kSYSCON_GpioPort0Pin22ToPintsel;
	if (me->port == 0 && me->pin == 23)				return kSYSCON_GpioPort0Pin23ToPintsel;
	if (me->port == 0 && me->pin == 24)				return kSYSCON_GpioPort0Pin24ToPintsel;
	if (me->port == 0 && me->pin == 25)				return kSYSCON_GpioPort0Pin25ToPintsel;
	if (me->port == 0 && me->pin == 26)				return kSYSCON_GpioPort0Pin26ToPintsel;
	if (me->port == 0 && me->pin == 27)				return kSYSCON_GpioPort0Pin27ToPintsel;
	if (me->port == 0 && me->pin == 28)				return kSYSCON_GpioPort0Pin28ToPintsel;
	if (me->port == 0 && me->pin == 29)				return kSYSCON_GpioPort0Pin29ToPintsel;
	if (me->port == 0 && me->pin == 30)				return kSYSCON_GpioPort0Pin30ToPintsel;

	ALX_IO_PIN_ASSERT(false); // We shouldn't get here
	return 0;
}
static IRQn_Type AlxIoPin_GetIrqType(AlxIoPin* me)
{
	if (me->irqPtr->irqPin == kPINT_PinInt0)		return PIN_INT0_IRQn;
	if (me->irqPtr->irqPin == kPINT_PinInt1)		return PIN_INT1_IRQn;
	if (me->irqPtr->irqPin == kPINT_PinInt2)		return PIN_INT2_IRQn;
	if (me->irqPtr->irqPin == kPINT_PinInt3)		return PIN_INT3_IRQn;
	if (me->irqPtr->irqPin == kPINT_PinInt4)		return PIN_INT4_IRQn;
	if (me->irqPtr->irqPin == kPINT_PinInt5)		return PIN_INT5_IRQn;
	if (me->irqPtr->irqPin == kPINT_PinInt6)		return PIN_INT6_IRQn;
	if (me->irqPtr->irqPin == kPINT_PinInt7)		return PIN_INT7_IRQn;

	ALX_IO_PIN_ASSERT(false); // We shouldn't get here
	return 0;
}
static void AlxIoPin_PINT_Init(AlxIoPin* me, PINT_Type* base)
{
	ALX_IO_PIN_ASSERT(base != NULL);
	uint32_t pmcfg = 0;
	uint8_t pintcount = 0;

	pintcount = FSL_FEATURE_PINT_NUMBER_OF_CONNECTED_OUTPUTS;

	/* Disable all bit slices for pint*/
	for (uint32_t i = 0; i < pintcount; i++)
	{
		pmcfg = pmcfg | ((uint32_t)kPINT_PatternMatchNever << (PININT_BITSLICE_CFG_START + (i * 3U)));
	}

	CLOCK_EnableClock(kCLOCK_GpioInt);

	/* Disable all pattern match bit slices */
	base->PMCFG = pmcfg;
}

//******************************************************************************
// Weak Functions
//******************************************************************************
ALX_WEAK void AlxIoPinIrq_Foreground_Callback_Pin0()
{
	ALX_IO_PIN_TRACE("Define AlxIoPinIrq_Foreground_Callback_Pin0");
	ALX_IO_PIN_ASSERT(false);
}
ALX_WEAK void AlxIoPinIrq_Foreground_Callback_Pin1()
{
	ALX_IO_PIN_TRACE("Define AlxIoPinIrq_Foreground_Callback_Pin1");
	ALX_IO_PIN_ASSERT(false);
}
ALX_WEAK void AlxIoPinIrq_Foreground_Callback_Pin2()
{
	ALX_IO_PIN_TRACE("Define AlxIoPinIrq_Foreground_Callback_Pin2");
	ALX_IO_PIN_ASSERT(false);
}
ALX_WEAK void AlxIoPinIrq_Foreground_Callback_Pin3()
{
	ALX_IO_PIN_TRACE("Define AlxIoPinIrq_Foreground_Callback_Pin3");
	ALX_IO_PIN_ASSERT(false);
}
ALX_WEAK void AlxIoPinIrq_Foreground_Callback_Pin4()
{
	ALX_IO_PIN_TRACE("Define AlxIoPinIrq_Foreground_Callback_Pin4");
	ALX_IO_PIN_ASSERT(false);
}
ALX_WEAK void AlxIoPinIrq_Foreground_Callback_Pin5()
{
	ALX_IO_PIN_TRACE("Define AlxIoPinIrq_Foreground_Callback_Pin5");
	ALX_IO_PIN_ASSERT(false);
}
ALX_WEAK void AlxIoPinIrq_Foreground_Callback_Pin6()
{
	ALX_IO_PIN_TRACE("Define AlxIoPinIrq_Foreground_Callback_Pin6");
	ALX_IO_PIN_ASSERT(false);
}
ALX_WEAK void AlxIoPinIrq_Foreground_Callback_Pin7()
{
	ALX_IO_PIN_TRACE("Define AlxIoPinIrq_Foreground_Callback_Pin7");
	ALX_IO_PIN_ASSERT(false);
}


//******************************************************************************
// IRQ Handlers
//******************************************************************************
void PIN_INT0_IRQHandler(void)
{
	AlxIoPinIrq_Foreground_Callback_Pin0();
	PINT_PinInterruptClrStatus(PINT, kPINT_PinInt0); // Clear IRQ Flag
}
void PIN_INT1_IRQHandler(void)
{
	AlxIoPinIrq_Foreground_Callback_Pin1();
	PINT_PinInterruptClrStatus(PINT, kPINT_PinInt1); // Clear IRQ Flag
}
void PIN_INT2_IRQHandler(void)
{
	AlxIoPinIrq_Foreground_Callback_Pin2();
	PINT_PinInterruptClrStatus(PINT, kPINT_PinInt2); // Clear IRQ Flag
}
void PIN_INT3_IRQHandler(void)
{
	AlxIoPinIrq_Foreground_Callback_Pin3();
	PINT_PinInterruptClrStatus(PINT, kPINT_PinInt3); // Clear IRQ Flag
}
void PIN_INT4_IRQHandler(void)
{
	AlxIoPinIrq_Foreground_Callback_Pin4();
	PINT_PinInterruptClrStatus(PINT, kPINT_PinInt4); // Clear IRQ Flag
}
void PIN_INT5_IRQHandler(void)
{
	AlxIoPinIrq_Foreground_Callback_Pin5();
	PINT_PinInterruptClrStatus(PINT, kPINT_PinInt5); // Clear IRQ Flag
}
void PIN_INT6_IRQHandler(void)
{
	AlxIoPinIrq_Foreground_Callback_Pin6();
	PINT_PinInterruptClrStatus(PINT, kPINT_PinInt6); // Clear IRQ Flag
}
void PIN_INT7_IRQHandler(void)
{
	AlxIoPinIrq_Foreground_Callback_Pin7();
	PINT_PinInterruptClrStatus(PINT, kPINT_PinInt7); // Clear IRQ Flag
}


#endif // Module Guard
