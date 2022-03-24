/**
  ******************************************************************************
  * @file alxHwLpcXpresso55S69_MfTest.h
  * @brief Auralix HW LPC Xpresso 55S69 C Library - MF Test Module
  * @version $LastChangedRevision: 4937 $
  * @date $LastChangedDate: 2021-05-02 22:05:40 +0200 (Sun, 02 May 2021) $
  ******************************************************************************
  */

#ifndef ALX_HW_LPC_XPRESSO_55S69_MF_TEST_H
#define ALX_HW_LPC_XPRESSO_55S69_MF_TEST_H

#ifdef __cplusplus
extern "C" {
#endif

//******************************************************************************
// Includes
//******************************************************************************
#include <alxHwLpcXpresso55S69_Main.h>


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_HW_LPC_XPRESSO_55S69_C_TEST)
#if defined(ALX_TEST_MF)


//******************************************************************************
//******************************************************************************
// G01_BringUp
//******************************************************************************
//******************************************************************************


//******************************************************************************
// Types
//******************************************************************************
typedef struct
{
	// Objects
	AlxHwLpcXpresso55S69_Main alxHwLpcXpresso55S69_Main;

	// Variables
	#if defined ALX_OPTIMIZE_SIZE_ALL
	uint32_t ai_P0_23_ADC_CH0_voltage_mV;
	uint32_t ai_P0_16_ADC_CH8_voltage_mV;
	#else
	float ai_P0_23_ADC_CH0_voltage_V;
	float ai_P0_16_ADC_CH8_voltage_V;
	#endif

	#if defined ALX_OPTIMIZE_SIZE_ALL
	uint32_t do_P1_4_PWM1_duty_permil;
	uint32_t do_P1_7_PWM2_duty_permil;
	#else
	float do_P1_4_PWM1_duty_pct;
	float do_P1_7_PWM2_duty_pct;
	#endif

	// Info
	bool wasCtorCalled;
	bool isInit;
} AlxHwLpcXpresso55S69_MfTest_G01_BringUp;


//******************************************************************************
// Private Functions
//******************************************************************************
static inline void AlxHwLpcXpresso55S69_MfTest_G01_BringUp_T01_Led(AlxHwLpcXpresso55S69_MfTest_G01_BringUp* me)
{
	// Assert
	(void)me;

	// Init
	//AlxIoPin_Init(&me->alxHwLpcXpresso55S69_Main.alxIoPin.do_P1_4_UsrLED_BL);
	AlxIoPin_Init(&me->alxHwLpcXpresso55S69_Main.alxIoPin.do_P1_6_UsrLED_RD);
	//AlxIoPin_Init(&me->alxHwLpcXpresso55S69_Main.alxIoPin.do_P1_7_UsrLED_GR);
	//AlxIoPin_Init(&me->alxHwLpcXpresso55S69_Main.alxIoPin.do_P1_9_GPIO);	// MF: To see toggling with Logic Analyzer

	while (1)
	{
		//AlxIoPin_Toggle(&me->alxHwLpcXpresso55S69_Main.alxIoPin.do_P1_4_UsrLED_BL);
		AlxIoPin_Toggle(&me->alxHwLpcXpresso55S69_Main.alxIoPin.do_P1_6_UsrLED_RD);
		//AlxIoPin_Toggle(&me->alxHwLpcXpresso55S69_Main.alxIoPin.do_P1_7_UsrLED_GR);
		//AlxIoPin_Toggle(&me->alxHwLpcXpresso55S69_Main.alxIoPin.do_P1_9_GPIO);

		AlxDelay_ms(500);
	}
}
static inline void AlxHwLpcXpresso55S69_MfTest_G01_BringUp_T02_Trace(AlxHwLpcXpresso55S69_MfTest_G01_BringUp* me)
{
	// Assert
	(void)me;

	while (1)
	{
		ALX_TRACE_FORMAT("T02_Trace\r\n");

		AlxDelay_ms(1000);
	}
}
static inline void AlxHwLpcXpresso55S69_MfTest_G01_BringUp_T03_Adc(AlxHwLpcXpresso55S69_MfTest_G01_BringUp* me)
{
	// Assert
	(void)me;

	// Init
	AlxAdc_Init(&me->alxHwLpcXpresso55S69_Main.alxAdc);

	while (1)
	{
		#if defined ALX_OPTIMIZE_SIZE_ALL
		me->ai_P0_23_ADC_CH0_voltage_mV = AlxAdc_GetVoltage_mV(&me->alxHwLpcXpresso55S69_Main.alxAdc, me->alxHwLpcXpresso55S69_Main.adcChArr[0]);
		me->ai_P0_16_ADC_CH8_voltage_mV = AlxAdc_GetVoltage_mV(&me->alxHwLpcXpresso55S69_Main.alxAdc, me->alxHwLpcXpresso55S69_Main.adcChArr[1]);
		AlxTrace_WriteFormat(&alxTrace, "Ch0, P1 = %d\r\n", me->ai_P0_23_ADC_CH0_voltage_mV);
		AlxTrace_WriteFormat(&alxTrace, "Ch8, P2 = %d\r\n", me->ai_P0_16_ADC_CH8_voltage_mV);
		#else
		me->ai_P0_23_ADC_CH0_voltage_V = AlxAdc_GetVoltage_V(&me->alxHwLpcXpresso55S69_Main.alxAdc, me->alxHwLpcXpresso55S69_Main.adcChArr[0]);
		me->ai_P0_16_ADC_CH8_voltage_V = AlxAdc_GetVoltage_V(&me->alxHwLpcXpresso55S69_Main.alxAdc, me->alxHwLpcXpresso55S69_Main.adcChArr[1]);
		AlxTrace_WriteFormat(&alxTrace, "Ch0, P1 = %d\r\n", me->ai_P0_23_ADC_CH0_voltage_V);
		AlxTrace_WriteFormat(&alxTrace, "Ch8, P2 = %d\r\n", me->ai_P0_16_ADC_CH8_voltage_V);
		#endif

		AlxDelay_ms(20);
	}
}
static inline void AlxHwLpcXpresso55S69_MfTest_G01_BringUp_T04_Pwm(AlxHwLpcXpresso55S69_MfTest_G01_BringUp* me)
{
	// Assert
	(void)me;

	// Init
	AlxPwm_Init(&me->alxHwLpcXpresso55S69_Main.alxPwm);

	// Setup Delay	// MF: To see how duty changes from what is in Ctor to what is set below
	AlxDelay_ms(500);

	while (1)
	{
		#if defined ALX_OPTIMIZE_SIZE_ALL
		AlxPwm_SetDuty_permil(&me->alxHwLpcXpresso55S69_Main.alxPwm, me->alxHwLpcXpresso55S69_Main.pwmChArr[0], me->do_P1_4_PWM1_duty_permil);
		AlxPwm_SetDuty_permil(&me->alxHwLpcXpresso55S69_Main.alxPwm, me->alxHwLpcXpresso55S69_Main.pwmChArr[1], me->do_P1_7_PWM2_duty_permil);
		#else
		AlxPwm_SetDuty_pct(&me->alxHwLpcXpresso55S69_Main.alxPwm, me->alxHwLpcXpresso55S69_Main.pwmChArr[0], me->do_P1_4_PWM1_duty_pct);
		AlxPwm_SetDuty_pct(&me->alxHwLpcXpresso55S69_Main.alxPwm, me->alxHwLpcXpresso55S69_Main.pwmChArr[1], me->do_P1_7_PWM2_duty_pct);
		#endif

		AlxDelay_ms(500);
	}
}
static inline void AlxHwLpcXpresso55S69_MfTest_G01_BringUp_T05_Spi_01(AlxHwLpcXpresso55S69_MfTest_G01_BringUp* me)
{
	// Assert
	(void)me;

	// Init
	AlxSpi_Init(&me->alxHwLpcXpresso55S69_Main.alxSpi7);

	// Setup Delay
	AlxDelay_ms(50);

	// Variables
	uint8_t srcBuffWrite[2];
	uint8_t srcBuffRead[2];
	srcBuffWrite[0] = 0b11111111; // MF: Writing (0b1) to 7Fh register returns CTN530 Verion and shoudl be 0x1A
	srcBuffWrite[1] = 0b11111111;
	srcBuffRead[0] = 0b00000000;
	srcBuffRead[1] = 0b00000000;

	while (1)
	{
		// WriteRead
		AlxSpi_Master_AssertCs(&me->alxHwLpcXpresso55S69_Main.alxSpi7);
		if (AlxSpi_Master_WriteRead(&me->alxHwLpcXpresso55S69_Main.alxSpi7, srcBuffWrite, srcBuffRead, sizeof(srcBuffWrite), 2, 0) != Alx_Ok)	{ ALX_TRACE_FORMAT("Pujhnalo\r\n"); }
		AlxSpi_Master_DeAssertCs(&me->alxHwLpcXpresso55S69_Main.alxSpi7);

		AlxDelay_ms(500);
	}
}
static inline void AlxHwLpcXpresso55S69_MfTest_G01_BringUp_T05_Spi_02(AlxHwLpcXpresso55S69_MfTest_G01_BringUp* me)
{
	// Assert
	(void)me;

	// Variables
	uint8_t srcBuffWrite[2];
	uint8_t srcBuffRead[2];
	srcBuffWrite[0] = 0b00010001;	// MF: Writing (0b1) to 08h register (to get default value is 0x10)
	srcBuffWrite[1] = 0b00000000;
	srcBuffRead[0] = 0b00000000;
	srcBuffRead[1] = 0b00000000;

	// Init
	AlxSpi_Init(&me->alxHwLpcXpresso55S69_Main.alxSpi7);

	// Setup Delay
	AlxDelay_ms(100);

	while (1)
	{
		// Read default value from Reg
		AlxSpi_Master_AssertCs(&me->alxHwLpcXpresso55S69_Main.alxSpi7);
		if (AlxSpi_Master_WriteRead(&me->alxHwLpcXpresso55S69_Main.alxSpi7, srcBuffWrite, srcBuffRead, sizeof(srcBuffWrite), 2, 0) != Alx_Ok)	{ ALX_TRACE_FORMAT("Pujhnalo\r\n"); }
		AlxSpi_Master_DeAssertCs(&me->alxHwLpcXpresso55S69_Main.alxSpi7);

		// Write
		AlxSpi_Master_AssertCs(&me->alxHwLpcXpresso55S69_Main.alxSpi7);
		srcBuffWrite[0] = 0b00010000;
		srcBuffWrite[1] = 0b11111111;	// MF: Write 0xFF to 08h register
		if (AlxSpi_Master_Write(&me->alxHwLpcXpresso55S69_Main.alxSpi7, srcBuffWrite, sizeof(srcBuffWrite), 2, 0) != Alx_Ok)	{ ALX_TRACE_FORMAT("Pujhnalo\r\n"); }
		AlxSpi_Master_DeAssertCs(&me->alxHwLpcXpresso55S69_Main.alxSpi7);

		// Read written value from Reg
		AlxSpi_Master_AssertCs(&me->alxHwLpcXpresso55S69_Main.alxSpi7);
		srcBuffWrite[0] = 0b00010001;
		srcBuffWrite[1] = 0b00000000;
		if (AlxSpi_Master_WriteRead(&me->alxHwLpcXpresso55S69_Main.alxSpi7, srcBuffWrite, srcBuffRead, sizeof(srcBuffWrite), 2, 0) != Alx_Ok)	{ ALX_TRACE_FORMAT("Pujhnalo\r\n"); }
		AlxSpi_Master_DeAssertCs(&me->alxHwLpcXpresso55S69_Main.alxSpi7);

		AlxDelay_ms(500);
	}
}
static inline void AlxHwLpcXpresso55S69_MfTest_G01_BringUp_T05_Spi_03(AlxHwLpcXpresso55S69_MfTest_G01_BringUp* me)
{
	// Assert
	(void)me;

	// Init
	AlxSpi_Init(&me->alxHwLpcXpresso55S69_Main.alxSpi3);

	// Setup Delay
	AlxDelay_ms(500);

	// Variables
	uint8_t srcBuffWrite[2];
	uint8_t srcBuffRead[2];
	srcBuffWrite[0] = 0b10001111; // MF: Writing (0b1) to 0x0F register returns Device_ID and shoudl be 0x44
	srcBuffWrite[1] = 0b10001111;
	srcBuffRead[0] = 0b00000000;
	srcBuffRead[1] = 0b00000000;

	while (1)
	{
		// WriteRead
		AlxSpi_Master_AssertCs(&me->alxHwLpcXpresso55S69_Main.alxSpi3);
		if (AlxSpi_Master_WriteRead(&me->alxHwLpcXpresso55S69_Main.alxSpi3, srcBuffWrite, srcBuffRead, sizeof(srcBuffWrite), 2, 0) != Alx_Ok)	{ ALX_TRACE_FORMAT("Pujhnalo\r\n"); }
		AlxSpi_Master_DeAssertCs(&me->alxHwLpcXpresso55S69_Main.alxSpi3);

		AlxDelay_ms(500);
	}
}
static inline void AlxHwLpcXpresso55S69_MfTest_G01_BringUp_T06_Clk(AlxHwLpcXpresso55S69_MfTest_G01_BringUp* me)
{
	// Init
	AlxIoPin_Init(&me->alxHwLpcXpresso55S69_Main.alxIoPin.do_P0_26_CLKOUT);

	if		(alxClk.config == AlxClk_Config_McuLpc55S6x_MainClk_12MHz_AhbClk_6MHz_FroOsc_12MHz_Default)	// MF. Clock Out on P0_26 should be 1MHz
	{
		CLOCK_SetClkDiv(kCLOCK_DivClkOut, 12U, true);
		CLOCK_AttachClk(kMAIN_CLK_to_CLKOUT);
	}
	else if (alxClk.config == AlxClk_Config_McuLpc55S6x_MainClk_96MHz_AhbClk_96MHz_FroOsc_96MHz)	// MF. Clock Out on P0_26 should be 1MHz
	{
		CLOCK_SetClkDiv(kCLOCK_DivClkOut, 96U, true);
		CLOCK_AttachClk(kMAIN_CLK_to_CLKOUT);
	}
	else if (alxClk.config == AlxClk_Config_McuLpc55S6x_MainClk_150MHz_AhbClk_150MHz_FroOsc_12MHz_Pll0)	// MF. Clock Out on P0_26 should be 1MHz
	{
		CLOCK_SetClkDiv(kCLOCK_DivClkOut, 150U, true);
		CLOCK_AttachClk(kMAIN_CLK_to_CLKOUT);
	}
	else if (alxClk.config == AlxClk_Config_McuLpc55S6x_MainClk_150MHz_AhbClk_150MHz_ExtOsc_16MHz)	// MF. Clock Out on P0_26 should be 1MHz
	{
		CLOCK_SetClkDiv(kCLOCK_DivClkOut, 150U, true);
		CLOCK_AttachClk(kMAIN_CLK_to_CLKOUT);
	}
	else { ALX_TRACE_FORMAT("Svinjak\r\n"); }

	while (1) {}
}


//******************************************************************************
// Constructor & Functions
//******************************************************************************
static inline void AlxHwLpcXpresso55S69_MfTest_G01_BringUp_Ctor(AlxHwLpcXpresso55S69_MfTest_G01_BringUp* me)
{
	// Ctor
	AlxHwLpcXpresso55S69_Main_Ctor(&me->alxHwLpcXpresso55S69_Main);

	// Variables
	#if defined ALX_OPTIMIZE_SIZE_ALL
	me->ai_P0_23_ADC_CH0_voltage_mV = 0ul;
	me->ai_P0_16_ADC_CH8_voltage_mV = 0ul;
	#else
	me->ai_P0_23_ADC_CH0_voltage_V = 0.f;
	me->ai_P0_16_ADC_CH8_voltage_V = 0.f;
	#endif

	#if defined ALX_OPTIMIZE_SIZE_ALL
	me->do_P1_4_PWM1_duty_permil = 111ul;
	me->do_P1_7_PWM2_duty_permil = 999ul;
	#else
	me->do_P1_4_PWM1_duty_pct = 11.11f;
	me->do_P1_7_PWM2_duty_pct = 88.88f;
	#endif

	// Info
	me->wasCtorCalled = true;
	me->isInit = false;
}
static inline void AlxHwLpcXpresso55S69_MfTest_G01_BringUp_Init(AlxHwLpcXpresso55S69_MfTest_G01_BringUp* me)
{
	// Init
	AlxClk_Init(&alxClk);
	AlxTrace_Init(&alxTrace);

	// IoPinIrq
	AlxIoPinIrq_Init(&me->alxHwLpcXpresso55S69_Main.alxIrqPin_IRQ1); // MF: IoPinIrq is initialized for all tests

	// Info
	me->isInit = true;
}
static inline void AlxHwLpcXpresso55S69_MfTest_G01_BringUp_Run(AlxHwLpcXpresso55S69_MfTest_G01_BringUp* me)
{
	//AlxHwLpcXpresso55S69_MfTest_G01_BringUp_T01_Led(me);
	AlxHwLpcXpresso55S69_MfTest_G01_BringUp_T02_Trace(me);
	//AlxHwLpcXpresso55S69_MfTest_G01_BringUp_T03_Adc(me);
	//AlxHwLpcXpresso55S69_MfTest_G01_BringUp_T04_Pwm(me);
	//AlxHwLpcXpresso55S69_MfTest_G01_BringUp_T05_Spi_01(me);
	//AlxHwLpcXpresso55S69_MfTest_G01_BringUp_T05_Spi_02(me);
	//AlxHwLpcXpresso55S69_MfTest_G01_BringUp_T05_Spi_03(me);
	//AlxHwLpcXpresso55S69_MfTest_G01_BringUp_T06_Clk(me);
}


//******************************************************************************
//******************************************************************************
// G02_BringUpRtos
//******************************************************************************
//******************************************************************************


//******************************************************************************
// Types
//******************************************************************************
#if defined(ALX_FREE_RTOS)
typedef struct
{
	// Objects
	AlxHwLpcXpresso55S69_Main alxHwLpcXpresso55S69_Main;

	// Semaphores
	SemaphoreHandle_t Mutex;

	// Info
	bool wasCtorCalled;
	bool isInit;
} AlxHwLpcXpresso55S69_MfTest_G02_BringUpRtos;


//******************************************************************************
// Variables
//******************************************************************************
extern volatile uint32_t G02_counter;
extern volatile SemaphoreHandle_t IsrTraceSem;
extern TaskHandle_t T11_TraceIsrHandle;


//******************************************************************************
// Private Functions
//******************************************************************************
static inline void G02_BringUpRtos_T01_Led_Task(void *pvParameters)
{
	// Prepare me
	AlxHwLpcXpresso55S69_MfTest_G02_BringUpRtos* me = (AlxHwLpcXpresso55S69_MfTest_G02_BringUpRtos*)pvParameters;

	while (1)
	{
		// Lock Mutex
		if (xSemaphoreTake(me->Mutex, portMAX_DELAY) == pdTRUE)
		{
			// Toggle LED
			AlxIoPin_Toggle(&me->alxHwLpcXpresso55S69_Main.alxIoPin.do_P1_6_UsrLED_RD);

			// Unlock Mutex
			xSemaphoreGive(me->Mutex);
		}
		else { continue; }

		vTaskDelay(500 / portTICK_PERIOD_MS);
	}
}
static inline void G02_BringUpRtos_T02_Trace_Task(void *pvParameters)
{
	while (1)
	{
		ALX_TRACE_FORMAT("T02_Trace_Task\r\n");

		vTaskDelay(1000 / portTICK_PERIOD_MS);
	}
}
static inline void G02_BringUpRtos_T03_Spi_Task(void *pvParameters)
{
	// Prepare me
	AlxHwLpcXpresso55S69_MfTest_G02_BringUpRtos* me = (AlxHwLpcXpresso55S69_MfTest_G02_BringUpRtos*)pvParameters;

	// Variables
	uint8_t srcBuffWrite[2];
	uint8_t srcBuffRead[2];
	srcBuffWrite[0] = 0b11111111; // MF: Writing (0b1) to 7Fh register returns CTN530 Verion and shoudl be 0x1A
	srcBuffWrite[1] = 0b11111111;
	srcBuffRead[0] = 0b00000000;
	srcBuffRead[1] = 0b00000000;

	while (1)
	{
		// Lock Mutex
		if (xSemaphoreTake(me->Mutex, portMAX_DELAY) == pdTRUE)
		{
			// WriteRead
			AlxSpi_Master_AssertCs(&me->alxHwLpcXpresso55S69_Main.alxSpi7);
			if (AlxSpi_Master_WriteRead(&me->alxHwLpcXpresso55S69_Main.alxSpi7, srcBuffWrite, srcBuffRead, sizeof(srcBuffWrite), 2, 0) != Alx_Ok)	{ ALX_TRACE_FORMAT("Pujhnalo\r\n"); }
			AlxSpi_Master_DeAssertCs(&me->alxHwLpcXpresso55S69_Main.alxSpi7);

			// Unlock Mutex
			xSemaphoreGive(me->Mutex);
		}
		else { continue; }

		vTaskDelay(200 / portTICK_PERIOD_MS);
	}
}
static inline void G02_BringUpRtos_T04_Spi_Acc_Task(void *pvParameters)
{
	// Prepare me
	AlxHwLpcXpresso55S69_MfTest_G02_BringUpRtos* me = (AlxHwLpcXpresso55S69_MfTest_G02_BringUpRtos*)pvParameters;

	// Variables
	uint8_t srcBuffWrite[2];
	uint8_t srcBuffRead[2];
	srcBuffWrite[0] = 0b10001111; // MF: Writing (0b1) to 0x0F register returns Device_ID and shoudl be 0x44
	srcBuffWrite[1] = 0b10001111;
	srcBuffRead[0] = 0b00000000;
	srcBuffRead[1] = 0b00000000;

	while (1)
	{
		// Lock Mutex
		if (xSemaphoreTake(me->Mutex, portMAX_DELAY) == pdTRUE)
		{
			// WriteRead
			AlxSpi_Master_AssertCs(&me->alxHwLpcXpresso55S69_Main.alxSpi3);
			if (AlxSpi_Master_WriteRead(&me->alxHwLpcXpresso55S69_Main.alxSpi3, srcBuffWrite, srcBuffRead, sizeof(srcBuffWrite), 2, 0) != Alx_Ok)	{ ALX_TRACE_FORMAT("Pujhnalo\r\n"); }
			AlxSpi_Master_DeAssertCs(&me->alxHwLpcXpresso55S69_Main.alxSpi3);

			// Unlock Mutex
			xSemaphoreGive(me->Mutex);
		}
		else { continue; }

		vTaskDelay(200 / portTICK_PERIOD_MS);
	}
}
static inline void G02_BringUpRtos_T05_Trace2_Task(void *pvParameters)
{
	while (1)
	{
		ALX_TRACE_FORMAT("T05_Trace2_Task\r\n");

		vTaskDelay(1000 / portTICK_PERIOD_MS);
	}
}
static inline void G02_BringUpRtos_T06_ThreadSwitching01(void *pvParameters)
{
	while (1)
	{
		GPIO->NOT[0] = (1U << 1);	// MF Port = 0, Pin = 1
	}
}
static inline void G02_BringUpRtos_T07_ThreadSwitching02(void *pvParameters)
{
	while (1)
	{
		GPIO->NOT[1] = (1U << 9);	// MF Port = 1, Pin = 9
	}
}
static inline void G02_BringUpRtos_T08_TraceGlobVar(void *pvParameters)
{
	// Prepare me
	AlxHwLpcXpresso55S69_MfTest_G02_BringUpRtos* me = (AlxHwLpcXpresso55S69_MfTest_G02_BringUpRtos*)pvParameters;

	while (1)
	{
		// Lock Mutex
		if (xSemaphoreTake(me->Mutex, portMAX_DELAY) == pdTRUE)
		{
			// Get global variable
			taskENTER_CRITICAL();	// MF: If IRQ happens inbetween critical section, it is not dropped. IRS will trigger as soon as current execution leaves the critical section
			uint32_t traceCounter = G02_counter;
			taskEXIT_CRITICAL();

			// Trace
			ALX_TRACE_FORMAT("G02_counter:   %lu\r\n", traceCounter);

			// Unlock Mutex
			xSemaphoreGive(me->Mutex);
		}
		else { continue; }

		vTaskDelay(1000 / portTICK_PERIOD_MS);
	}
}
static inline void G02_BringUpRtos_T09_TraceGlobVar2(void *pvParameters)
{
	// Prepare me
	AlxHwLpcXpresso55S69_MfTest_G02_BringUpRtos* me = (AlxHwLpcXpresso55S69_MfTest_G02_BringUpRtos*)pvParameters;

	while (1)
	{
		// Lock Mutex
		if (xSemaphoreTake(me->Mutex, portMAX_DELAY) == pdTRUE)
		{
			// Get global variable
			taskENTER_CRITICAL();
			uint32_t traceCounter2 = G02_counter * 2;
			taskEXIT_CRITICAL();

			// Trace
			ALX_TRACE_FORMAT("G02_counter*2: %lu\r\n\r\n", traceCounter2);

			// Unlock Mutex
			xSemaphoreGive(me->Mutex);
		}
		else { continue; }

		vTaskDelay(1000 / portTICK_PERIOD_MS);
	}
}
static inline void G02_BringUpRtos_T10_TraceIsrSem(void *pvParameters)
{
	// Prepare me
	AlxHwLpcXpresso55S69_MfTest_G02_BringUpRtos* me = (AlxHwLpcXpresso55S69_MfTest_G02_BringUpRtos*)pvParameters;

	while (1)
	{
		// Wait for IRQ (WakeUp button being pressed) to happen
		xSemaphoreTake(IsrTraceSem, portMAX_DELAY);

		// Lock Mutex
		if (xSemaphoreTake(me->Mutex, portMAX_DELAY) == pdTRUE)
		{
			// Trace
			ALX_TRACE_FORMAT("WakeUp Button Pressed - Semaphore\r\n");

			// Unlock Mutex
			xSemaphoreGive(me->Mutex);
		}
		else { continue; }
	}
}
static inline void G02_BringUpRtos_T11_TraceIsrNotify(void *pvParameters)
{
	// Prepare me
	AlxHwLpcXpresso55S69_MfTest_G02_BringUpRtos* me = (AlxHwLpcXpresso55S69_MfTest_G02_BringUpRtos*)pvParameters;

	while (1)
	{
		// Wait for IRQ (WakeUp button being pressed) to happen
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

		// Lock Mutex
		if (xSemaphoreTake(me->Mutex, portMAX_DELAY) == pdTRUE)
		{
			// Trace
			ALX_TRACE_FORMAT("WakeUp Button Pressed - Notify\r\n");

			// Unlock Mutex
			xSemaphoreGive(me->Mutex);
		}
		else { continue; }
	}
}
static inline void G02_BringUpRtos_T12_SpiThrdTest_Thrd1(void *pvParameters)
{
	// Prepare me
	AlxHwLpcXpresso55S69_MfTest_G02_BringUpRtos* me = (AlxHwLpcXpresso55S69_MfTest_G02_BringUpRtos*)pvParameters;

	// Variables
	uint8_t srcBuffWrite[2];
	uint8_t srcBuffRead[2];
	srcBuffWrite[0] = 0b11010101;
	srcBuffWrite[1] = 0b11111111;
	srcBuffRead[0] = 0b00000000;
	srcBuffRead[1] = 0b00000000;

	while (1)
	{
		// WriteRead
		AlxSpi_Master_AssertCs(&me->alxHwLpcXpresso55S69_Main.alxSpi7);
		if (AlxSpi_Master_WriteRead(&me->alxHwLpcXpresso55S69_Main.alxSpi7, srcBuffWrite, srcBuffRead, sizeof(srcBuffWrite), 2, 0) != Alx_Ok)	{ ALX_TRACE_FORMAT("Pujhnalo\r\n"); }
		AlxSpi_Master_DeAssertCs(&me->alxHwLpcXpresso55S69_Main.alxSpi7);
	}
}
static inline void G02_BringUpRtos_T13_SpiThrdTest_Thrd2(void *pvParameters)
{
	// Prepare me
	AlxHwLpcXpresso55S69_MfTest_G02_BringUpRtos* me = (AlxHwLpcXpresso55S69_MfTest_G02_BringUpRtos*)pvParameters;

	// Variables
	uint8_t srcBuffWrite[2];
	uint8_t srcBuffRead[2];
	srcBuffWrite[0] = 0b11111111; // MF: Writing (0b1) to 7Fh register returns CTN530 Verion and shoudl be 0x1A
	srcBuffWrite[1] = 0b11111111;
	srcBuffRead[0] = 0b00000000;
	srcBuffRead[1] = 0b00000000;

	while (1)
	{
		// WriteRead
		AlxSpi_Master_AssertCs(&me->alxHwLpcXpresso55S69_Main.alxSpi7);
		if (AlxSpi_Master_WriteRead(&me->alxHwLpcXpresso55S69_Main.alxSpi7, srcBuffWrite, srcBuffRead, sizeof(srcBuffWrite), 2, 0) != Alx_Ok)	{ ALX_TRACE_FORMAT("Pujhnalo\r\n"); }
		AlxSpi_Master_DeAssertCs(&me->alxHwLpcXpresso55S69_Main.alxSpi7);
	}
}

static inline void AlxHwLpcXpresso55S69_MfTest_G02_BringUpRtos_T01_Led(AlxHwLpcXpresso55S69_MfTest_G02_BringUpRtos* me)
{
	// Assert
	(void)me;

	// Init
	AlxIoPin_Init(&me->alxHwLpcXpresso55S69_Main.alxIoPin.do_P1_6_UsrLED_RD);

	// Create Rtos Task / Thread
	BaseType_t status = xTaskCreate
	(
		G02_BringUpRtos_T01_Led_Task,
		"G02_BringUpRtos_T01_Led_Task",
		configMINIMAL_STACK_SIZE + 100,
		me,
		(configMAX_PRIORITIES - 1),
		NULL
	);

	// Check Task Creation Status
	if (status != pdPASS) { ALX_TRACE_FORMAT("G02_BringUpRtos_T01_Led_Task creaton failed!\r\n"); }
}
static inline void AlxHwLpcXpresso55S69_MfTest_G02_BringUpRtos_T02_Trace(AlxHwLpcXpresso55S69_MfTest_G02_BringUpRtos* me)
{
	// Assert
	(void)me;

	// Create Rtos Task / Thread
	BaseType_t status = xTaskCreate
	(
		G02_BringUpRtos_T02_Trace_Task,
		"G02_BringUpRtos_T02_Trace_Task",
		configMINIMAL_STACK_SIZE + 100,
		me,
		(configMAX_PRIORITIES - 1),
		NULL
	);

	// Check Task Creation Status
	if (status != pdPASS) { ALX_TRACE_FORMAT("G02_BringUpRtos_T02_Trace_Task creaton failed!\r\n"); }
}
static inline void AlxHwLpcXpresso55S69_MfTest_G02_BringUpRtos_T03_Spi(AlxHwLpcXpresso55S69_MfTest_G02_BringUpRtos* me)
{
	// Assert
	(void)me;

	// Init
	AlxSpi_Init(&me->alxHwLpcXpresso55S69_Main.alxSpi7);

	// Create Rtos Task / Thread
	BaseType_t status = xTaskCreate
	(
		G02_BringUpRtos_T03_Spi_Task,
		"G02_BringUpRtos_T03_Spi_Task",
		configMINIMAL_STACK_SIZE + 100,
		me,
		(configMAX_PRIORITIES - 1),
		NULL
	);

	// Check Task Creation Status
	if (status != pdPASS) { ALX_TRACE_FORMAT("G02_BringUpRtos_T03_Spi_Task creaton failed!\r\n"); }
}
static inline void AlxHwLpcXpresso55S69_MfTest_G02_BringUpRtos_T04_Spi_Acc(AlxHwLpcXpresso55S69_MfTest_G02_BringUpRtos* me)
{
	// Assert
	(void)me;

	// Init
	AlxSpi_Init(&me->alxHwLpcXpresso55S69_Main.alxSpi3);

	// Create Rtos Task / Thread
	BaseType_t status = xTaskCreate
	(
		G02_BringUpRtos_T04_Spi_Acc_Task,
		"G02_BringUpRtos_T04_Spi_Acc_Task",
		configMINIMAL_STACK_SIZE + 100,
		me,
		(configMAX_PRIORITIES - 1),
		NULL
	);

	// Check Task Creation Status
	if (status != pdPASS) { ALX_TRACE_FORMAT("G02_BringUpRtos_T04_Spi_Acc_Task creaton failed!\r\n"); }
}
static inline void AlxHwLpcXpresso55S69_MfTest_G02_BringUpRtos_T05_Trace2(AlxHwLpcXpresso55S69_MfTest_G02_BringUpRtos* me)
{
	// Assert
	(void)me;

	// Create Rtos Task / Thread
	BaseType_t status = xTaskCreate
	(
		G02_BringUpRtos_T05_Trace2_Task,
		"G02_BringUpRtos_T05_Trace2_Task",
		configMINIMAL_STACK_SIZE + 100,
		me,
		(configMAX_PRIORITIES - 1),
		NULL
	);

	// Check Task Creation Status
	if (status != pdPASS) { ALX_TRACE_FORMAT("G02_BringUpRtos_T05_Trace2_Task creaton failed!\r\n"); }
}
static inline void AlxHwLpcXpresso55S69_MfTest_G02_BringUpRtos_T06_ThreadSwitching01(AlxHwLpcXpresso55S69_MfTest_G02_BringUpRtos* me)
{
	// Assert
	(void)me;

	// Init
	AlxIoPin_Init(&me->alxHwLpcXpresso55S69_Main.alxIoPin.do_P0_1_DBG01);

	// Create Rtos Task / Thread
	BaseType_t status = xTaskCreate
	(
		G02_BringUpRtos_T06_ThreadSwitching01,
		"G02_BringUpRtos_T06_ThreadSwitching01",
		configMINIMAL_STACK_SIZE,
		me,
		(configMAX_PRIORITIES - 1),
		NULL
	);

	// Check Task Creation Status
	if (status != pdPASS) { ALX_TRACE_FORMAT("G02_BringUpRtos_T06_ThreadSwitching01_Task creaton failed!\r\n"); }
}
static inline void AlxHwLpcXpresso55S69_MfTest_G02_BringUpRtos_T07_ThreadSwitching02(AlxHwLpcXpresso55S69_MfTest_G02_BringUpRtos* me)
{
	// Assert
	(void)me;

	// Init
	AlxIoPin_Init(&me->alxHwLpcXpresso55S69_Main.alxIoPin.do_P1_9_GPIO);

	// Create Rtos Task / Thread
	BaseType_t status = xTaskCreate
	(
		G02_BringUpRtos_T07_ThreadSwitching02,
		"G02_BringUpRtos_T07_ThreadSwitching02",
		configMINIMAL_STACK_SIZE,
		me,
		(configMAX_PRIORITIES - 1),
		NULL
	);

	// Check Task Creation Status
	if (status != pdPASS) { ALX_TRACE_FORMAT("G02_BringUpRtos_T07_ThreadSwitching02_Task creaton failed!\r\n"); }
}
static inline void AlxHwLpcXpresso55S69_MfTest_G02_BringUpRtos_T08_TraceGlobVar(AlxHwLpcXpresso55S69_MfTest_G02_BringUpRtos* me)
{
	// Assert
	(void)me;

	// Create Rtos Task / Thread
	BaseType_t status = xTaskCreate
	(
		G02_BringUpRtos_T08_TraceGlobVar,
		"G02_BringUpRtos_T08_TraceGlobVar",
		configMINIMAL_STACK_SIZE + 100,
		me,
		(configMAX_PRIORITIES - 1),
		NULL
	);

	// Check Task Creation Status
	if (status != pdPASS) { ALX_TRACE_FORMAT("G02_BringUpRtos_T08_TraceGlobVar creaton failed!\r\n"); }
}
static inline void AlxHwLpcXpresso55S69_MfTest_G02_BringUpRtos_T09_TraceGlobVar2(AlxHwLpcXpresso55S69_MfTest_G02_BringUpRtos* me)
{
	// Assert
	(void)me;

	// Create Rtos Task / Thread
	BaseType_t status = xTaskCreate
	(
		G02_BringUpRtos_T09_TraceGlobVar2,
		"G02_BringUpRtos_T09_TraceGlobVar2",
		configMINIMAL_STACK_SIZE + 100,
		me,
		(configMAX_PRIORITIES - 1),
		NULL
	);

	// Check Task Creation Status
	if (status != pdPASS) { ALX_TRACE_FORMAT("G02_BringUpRtos_T09_TraceGlobVar2 creaton failed!\r\n"); }
}
static inline void AlxHwLpcXpresso55S69_MfTest_G02_BringUpRtos_T10_TraceIsrSem(AlxHwLpcXpresso55S69_MfTest_G02_BringUpRtos* me)
{
	// Assert
	(void)me;

	// Create Rtos Task / Thread
	BaseType_t status = xTaskCreate
	(
		G02_BringUpRtos_T10_TraceIsrSem,
		"G02_BringUpRtos_T10_TraceIsr",
		configMINIMAL_STACK_SIZE + 100,
		me,
		configMAX_PRIORITIES,
		NULL
	);

	// Check Task Creation Status
	if (status != pdPASS) { ALX_TRACE_FORMAT("G02_BringUpRtos_T10_TraceIsr creaton failed!\r\n"); }
}
static inline void AlxHwLpcXpresso55S69_MfTest_G02_BringUpRtos_T11_TraceIsrNotify(AlxHwLpcXpresso55S69_MfTest_G02_BringUpRtos* me)
{
	// Assert
	(void)me;

	// Create Rtos Task / Thread
	BaseType_t status = xTaskCreate
	(
		G02_BringUpRtos_T11_TraceIsrNotify,
		"G02_BringUpRtos_T11_TraceIsrNotify",
		configMINIMAL_STACK_SIZE + 100,
		me,
		configMAX_PRIORITIES,
		&T11_TraceIsrHandle
	);

	// Check Task Creation Status
	if (status != pdPASS) { ALX_TRACE_FORMAT("G02_BringUpRtos_T11_TraceIsrNotify creaton failed!\r\n"); }
}
static inline void AlxHwLpcXpresso55S69_MfTest_G02_BringUpRtos_T12_SpiThrdTest(AlxHwLpcXpresso55S69_MfTest_G02_BringUpRtos* me)
{
	// Assert
	(void)me;

	// Init
	AlxSpi_Init(&me->alxHwLpcXpresso55S69_Main.alxSpi7);

	//--------------------------
	// Thread 01
	//--------------------------
	// Create Rtos Task / Thread
	BaseType_t status = xTaskCreate
	(
		G02_BringUpRtos_T12_SpiThrdTest_Thrd1,
		"G02_BringUpRtos_T12_SpiThrdTest_Thrd1",
		configMINIMAL_STACK_SIZE + 100,
		me,
		configMAX_PRIORITIES,
		&T11_TraceIsrHandle
	);

	//--------------------------
	// Thread 02
	//--------------------------
	// Check Task Creation Status
	if (status != pdPASS) { ALX_TRACE_FORMAT("G02_BringUpRtos_T12_SpiThrdTest_Thrd1 creaton failed!\r\n"); }

	// Create Rtos Task / Thread
	status = xTaskCreate
	(
		G02_BringUpRtos_T13_SpiThrdTest_Thrd2,
		"G02_BringUpRtos_T13_SpiThrdTest_Thrd2",
		configMINIMAL_STACK_SIZE + 100,
		me,
		configMAX_PRIORITIES,
		&T11_TraceIsrHandle);

	// Check Task Creation Status
	if (status != pdPASS) { ALX_TRACE_FORMAT("G02_BringUpRtos_T13_SpiThrdTest_Thrd2 creaton failed!\r\n"); }
}


//******************************************************************************
// Constructor & Functions
//******************************************************************************
static inline void AlxHwLpcXpresso55S69_MfTest_G02_BringUpRtos_Ctor(AlxHwLpcXpresso55S69_MfTest_G02_BringUpRtos* me)
{
	// Ctor
	AlxHwLpcXpresso55S69_Main_Ctor(&me->alxHwLpcXpresso55S69_Main);

	// Info
	me->wasCtorCalled = true;
	me->isInit = false;
}
static inline void AlxHwLpcXpresso55S69_MfTest_G02_BringUpRtos_Init(AlxHwLpcXpresso55S69_MfTest_G02_BringUpRtos* me)
{
	// Init
	AlxClk_Init(&alxClk);
	AlxTrace_Init(&alxTrace);

	// IoPinIrq
	AlxIoPinIrq_Init(&me->alxHwLpcXpresso55S69_Main.alxIrqPin_IRQ1);		// MF: IoPinIrq is initialized for all tests

	// Create Semmaphore
	me->Mutex = xSemaphoreCreateBinary();
	if (me->Mutex == NULL)
	{
		while(1){}
	}
	xSemaphoreGive(me->Mutex);

	IsrTraceSem = xSemaphoreCreateBinary();
	if (IsrTraceSem == NULL)
	{
		while (1) {}
	}

	// Info
	me->isInit = true;
}
static inline void AlxHwLpcXpresso55S69_MfTest_G02_BringUpRtos_Run(AlxHwLpcXpresso55S69_MfTest_G02_BringUpRtos* me)
{
	AlxHwLpcXpresso55S69_MfTest_G02_BringUpRtos_T01_Led(me);
	//AlxHwLpcXpresso55S69_MfTest_G02_BringUpRtos_T02_Trace(me);
	AlxHwLpcXpresso55S69_MfTest_G02_BringUpRtos_T03_Spi(me);
	AlxHwLpcXpresso55S69_MfTest_G02_BringUpRtos_T04_Spi_Acc(me);
	//AlxHwLpcXpresso55S69_MfTest_G02_BringUpRtos_T05_Trace2(me);
	//AlxHwLpcXpresso55S69_MfTest_G02_BringUpRtos_T06_ThreadSwitching01(me);
	//AlxHwLpcXpresso55S69_MfTest_G02_BringUpRtos_T07_ThreadSwitching02(me);
	//AlxHwLpcXpresso55S69_MfTest_G02_BringUpRtos_T08_TraceGlobVar(me);
	//AlxHwLpcXpresso55S69_MfTest_G02_BringUpRtos_T09_TraceGlobVar2(me);
	//AlxHwLpcXpresso55S69_MfTest_G02_BringUpRtos_T10_TraceIsrSem(me);
	//AlxHwLpcXpresso55S69_MfTest_G02_BringUpRtos_T11_TraceIsrNotify(me);
	//AlxHwLpcXpresso55S69_MfTest_G02_BringUpRtos_T12_SpiThrdTest(me);	// MF: This won't work, since alxSpi is NOT thread safe
}
#endif // #if defined(ALX_FREE_RTOS)


//******************************************************************************
//******************************************************************************
// G03_IoExpander
//******************************************************************************
//******************************************************************************


//******************************************************************************
// Types
//******************************************************************************
typedef struct
{
	// Objects
	AlxHwLpcXpresso55S69_Main alxHwLpcXpresso55S69_Main;

	// Semaphores
	SemaphoreHandle_t Mutex;

	// Info
	bool wasCtorCalled;
	bool isInit;
} AlxHwLpcXpresso55S69_MfTest_G03_IoExpander;


//******************************************************************************
// Private Functions
//******************************************************************************
static inline void AlxHwLpcXpresso55S69_MfTest_G03_IoExpander_T01_ReadAccReg(AlxHwLpcXpresso55S69_MfTest_G03_IoExpander* me)
{
	// Assert
	(void)me;

	// Variables
	uint8_t devAddr = 0b00110000;	// MF: SAO pin si on GND so devAddr = 0x18, last bit = 0b0 (write)
	uint16_t memAddr = 0x0FU;		// MF: Device_ID (0x0F) Address. Sould return 0x44(b01000100)
	uint8_t i2cData[1] = { 0 };

	// Init
	AlxI2c_Init(&me->alxHwLpcXpresso55S69_Main.alxI2c);

	while (1)
	{
		AlxI2c_Master_StartReadMemStop(&me->alxHwLpcXpresso55S69_Main.alxI2c, devAddr, memAddr, AlxI2c_Master_MemAddrLen_8bit, i2cData, 1, 20, 100);

		AlxDelay_ms(50);
	}
}
static inline void AlxHwLpcXpresso55S69_MfTest_G03_IoExpander_T02_ReadIoExpReg(AlxHwLpcXpresso55S69_MfTest_G03_IoExpander*me)
{
	// Assert
	(void)me;

	// Variables
	uint8_t devAddr = 0b01000000;	// MF: ADDR pin is on GND so devAddr = 0x20, last bit = 0b0 (write)
	uint16_t memAddr = 0x02U;		// MF: Output port 0 Address. Sould return 0xFF(0b11111111) which is default number
	uint8_t i2cData[1] = { 0 };

	// Init
	AlxI2c_Init(&me->alxHwLpcXpresso55S69_Main.alxI2c);

	while (1)
	{
		AlxI2c_Master_StartReadMemStop(&me->alxHwLpcXpresso55S69_Main.alxI2c, devAddr, memAddr, AlxI2c_Master_MemAddrLen_8bit, i2cData, 1, 20, 100);

		AlxDelay_ms(50);
	}
}
static inline void AlxHwLpcXpresso55S69_MfTest_G03_IoExpander_T03_WriteReadIoExpReg(AlxHwLpcXpresso55S69_MfTest_G03_IoExpander*me)
{
	// Assert
	(void)me;

	// Variables
	uint8_t devAddr = 0b01000000;	// MF: ADDR pin is on GND so devAddr = 0x20, last bit = 0b0 (write)
	uint16_t memAddr = 0x02U;		// MF: Output port 0 Address. Sould return 0xFF(0b11111111) which is default number
	uint8_t i2cData[1] = { 0 };

	// Init
	AlxI2c_Init(&me->alxHwLpcXpresso55S69_Main.alxI2c);

	while (1)
	{
		// Read Reg before write - Sould return 0xFF(0b11111111)
		AlxI2c_Master_StartReadMemStop(&me->alxHwLpcXpresso55S69_Main.alxI2c, devAddr, memAddr, AlxI2c_Master_MemAddrLen_8bit, i2cData, 2, 20, 100);

		// Write
		uint8_t data[2];
		data[0] = 0b10101010;
		data[1] = 0b10101010;
		AlxI2c_Master_StartWriteMemStop_Multi(&me->alxHwLpcXpresso55S69_Main.alxI2c, devAddr, memAddr, AlxI2c_Master_MemAddrLen_8bit, data, 2, false, 20, 100);

		// Read Reg after write - Sould return 0xAA(0b10101010)
		AlxI2c_Master_StartReadMemStop(&me->alxHwLpcXpresso55S69_Main.alxI2c, devAddr, memAddr, AlxI2c_Master_MemAddrLen_8bit, i2cData, 2, 20, 100);

		AlxDelay_ms(50);
	}
}
static inline void AlxHwLpcXpresso55S69_MfTest_G03_IoExpander_T04_TogglePins(AlxHwLpcXpresso55S69_MfTest_G03_IoExpander*me)
{
	// Assert
	(void)me;

	// Variables
	uint8_t devAddr = 0b01000000;	// MF: ADDR pin is on GND so devAddr = 0x20, last bit = 0b0 (write)
	uint16_t memAddr = 0x00U;		// MF: Output port 0 Address. Sould return 0xFF(0b11111111) which is default number
	uint8_t i2cData[2] = { 0 };

	// Init I2c
	AlxI2c_Init(&me->alxHwLpcXpresso55S69_Main.alxI2c);

	// Write - Config all Pins as "Output"
	memAddr = 0x06U;
	i2cData[0] = 0b00000000;
	i2cData[1] = 0b00000000;
	AlxI2c_Master_StartWriteMemStop_Multi(&me->alxHwLpcXpresso55S69_Main.alxI2c, devAddr, memAddr, AlxI2c_Master_MemAddrLen_8bit, i2cData, 2, true, 20, 100);

	// Set Output port register
	memAddr = 0x02U;

	while (1)
	{
		// Write - Set Pins to LOW
		i2cData[0] = 0b00000000;
		i2cData[1] = 0b00000000;
		AlxI2c_Master_StartWriteMemStop_Multi(&me->alxHwLpcXpresso55S69_Main.alxI2c, devAddr, memAddr, AlxI2c_Master_MemAddrLen_8bit, i2cData, 2, true, 20, 100);

		AlxDelay_ms(50);

		// Write -Set Pins to HIGH
		i2cData[0] = 0b11111111;
		i2cData[1] = 0b11111111;
		AlxI2c_Master_StartWriteMemStop_Multi(&me->alxHwLpcXpresso55S69_Main.alxI2c, devAddr, memAddr, AlxI2c_Master_MemAddrLen_8bit, i2cData, 2, true, 20, 100);

		AlxDelay_ms(50);
	}
}


//******************************************************************************
// Constructor & Functions
//******************************************************************************
static inline void AlxHwLpcXpresso55S69_MfTest_G03_IoExpander_Ctor(AlxHwLpcXpresso55S69_MfTest_G03_IoExpander* me)
{
	// Ctor
	AlxHwLpcXpresso55S69_Main_Ctor(&me->alxHwLpcXpresso55S69_Main);

	// Info
	me->wasCtorCalled = true;
	me->isInit = false;
}
static inline void AlxHwLpcXpresso55S69_MfTest_G03_IoExpander_Init(AlxHwLpcXpresso55S69_MfTest_G03_IoExpander* me)
{
	// Init
	AlxClk_Init(&alxClk);
	AlxTrace_Init(&alxTrace);

	// IoPinIrq
	AlxIoPinIrq_Init(&me->alxHwLpcXpresso55S69_Main.alxIrqPin_IRQ1);		// MF: IoPinIrq is initialized for all tests

	// Info
	me->isInit = true;
}
static inline void AlxHwLpcXpresso55S69_MfTest_G03_IoExpander_Run(AlxHwLpcXpresso55S69_MfTest_G03_IoExpander* me)
{
	//AlxHwLpcXpresso55S69_MfTest_G03_IoExpander_T01_ReadAccReg(me);	// MF: This was tested on "2533020201601" Acc meter
	//AlxHwLpcXpresso55S69_MfTest_G03_IoExpander_T02_ReadIoExpReg(me);
	//AlxHwLpcXpresso55S69_MfTest_G03_IoExpander_T03_WriteReadIoExpReg(me);
	AlxHwLpcXpresso55S69_MfTest_G03_IoExpander_T04_TogglePins(me);
}


#endif // #if defined(ALX_TEST_MF)
#endif // #if defined(ALX_HW_LPC_XPRESSO_55S69_C_TEST)


#ifdef __cplusplus
}
#endif


#endif // ALX_HW_LPC_XPRESSO_55S69_MF_TEST_H
