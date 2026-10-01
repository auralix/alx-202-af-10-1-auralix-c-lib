/**
  ******************************************************************************
  * @file		asf4HostShim.h
  * @brief		Auralix C Library - Configuration Matrix Fake - Atmel START (ASF4)
  * @copyright	Copyright (C) Auralix d.o.o. All rights reserved.
  *
  * Compile-only: every name of Atmel START (ASF4) the alx code uses, written from
  * that use, so the configuration matrix compiles every alx line under
  * ALX_SAM (Test/config/matrix.toml). Nothing here is the vendor's and
  * nothing here runs; the real one is the product's.
  *
  * As the STM32 fake does, it makes no claim about the vendor's signatures
  * and layouts, only that every name exists: one register block and one
  * structure shape hold every member the code names, a call forwards its
  * arguments to one variadic function, a constant has a value no other has.
  * Grown from the compilers' diagnostics by the scratch tool of ALX-1564
  * night goal 4; edited by hand from then on.
  ******************************************************************************
  **/

#ifndef ALX_SHIM_ASF4_ASF4HOSTSHIM_H
#define ALX_SHIM_ASF4_ASF4HOSTSHIM_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "cmsisHostShim.h"


typedef struct alxShimAsf4_Sub
{
	uint32_t alxShimUnused;
} alxShimAsf4_Sub;

typedef struct alxShimAsf4_Struct
{
	uint32_t alxShimUnused;
} alxShimAsf4_Struct;

struct adc_sync_descriptor
{
	uint32_t alxShimUnused;
};
struct pwm_descriptor
{
	uint32_t alxShimUnused;
};
struct usart_sync_descriptor
{
	uint32_t alxShimUnused;
};
enum gpio_direction { alxShimAsf4_gpio_direction = 0 };
enum gpio_pull_mode { alxShimAsf4_gpio_pull_mode = 0 };

extern alxShimAsf4_Struct alxShimAsf4_Instance;
#define SERCOM0 (&alxShimAsf4_Instance)
#define SERCOM1 (&alxShimAsf4_Instance)
#define SERCOM2 (&alxShimAsf4_Instance)
#define SERCOM3 (&alxShimAsf4_Instance)
#define SERCOM4 (&alxShimAsf4_Instance)
#define SERCOM5 (&alxShimAsf4_Instance)
#define TCC0 (&alxShimAsf4_Instance)
#define TCC1 (&alxShimAsf4_Instance)
#define TCC2 (&alxShimAsf4_Instance)
#define ADC (0x0100U)
#define ADC_GCLK_ID (0x0101U)
#define GPIO_DIRECTION_OFF (0x0102U)
#define GPIO_PIN_FUNCTION_OFF (0x0103U)
#define GPIO_PULL_DOWN (0x0104U)
#define GPIO_PULL_OFF (0x0105U)
#define GPIO_PULL_UP (0x0106U)
#define PM_BUS_APBC (0x0107U)
#define SERCOM0_GCLK_ID_CORE (0x0108U)
#define SERCOM0_IRQn ((IRQn_Type)1)
#define SERCOM1_GCLK_ID_CORE (0x010AU)
#define SERCOM1_IRQn ((IRQn_Type)2)
#define SERCOM2_GCLK_ID_CORE (0x010CU)
#define SERCOM2_IRQn ((IRQn_Type)3)
#define SERCOM3_GCLK_ID_CORE (0x010EU)
#define SERCOM3_IRQn ((IRQn_Type)4)
#define SERCOM4_GCLK_ID_CORE (0x0110U)
#define SERCOM4_IRQn ((IRQn_Type)5)
#define SERCOM5_GCLK_ID_CORE (0x0112U)
#define SERCOM5_IRQn ((IRQn_Type)6)
#define TCC0_GCLK_ID (0x0114U)
#define TCC1_GCLK_ID (0x0115U)
#define TCC2_GCLK_ID (0x0116U)

uint32_t alxShimAsf4_Call(int unused, ...);
void* alxShimAsf4_CallPtr(int unused, ...);
#define _gclk_enable_channel(...) alxShimAsf4_Call(0, ##__VA_ARGS__)
#define _pm_disable_bus_clock(...) alxShimAsf4_Call(0, ##__VA_ARGS__)
#define _pm_enable_bus_clock(...) alxShimAsf4_Call(0, ##__VA_ARGS__)
#define _tcc_get_pwm(...) alxShimAsf4_Call(0, ##__VA_ARGS__)
#define adc_sync_deinit(...) alxShimAsf4_Call(0, ##__VA_ARGS__)
#define adc_sync_init(...) alxShimAsf4_Call(0, ##__VA_ARGS__)
#define adc_sync_read_channel(...) alxShimAsf4_Call(0, ##__VA_ARGS__)
#define adc_sync_set_inputs(...) alxShimAsf4_Call(0, ##__VA_ARGS__)
#define gpio_get_pin_level(...) alxShimAsf4_Call(0, ##__VA_ARGS__)
#define gpio_set_pin_direction(...) alxShimAsf4_Call(0, ##__VA_ARGS__)
#define gpio_set_pin_function(...) alxShimAsf4_Call(0, ##__VA_ARGS__)
#define gpio_set_pin_level(...) alxShimAsf4_Call(0, ##__VA_ARGS__)
#define gpio_set_pin_pull_mode(...) alxShimAsf4_Call(0, ##__VA_ARGS__)
#define gpio_toggle_pin_level(...) alxShimAsf4_Call(0, ##__VA_ARGS__)
#define hri_sercomusart_clear_INTEN_DRE_bit(...) alxShimAsf4_Call(0, ##__VA_ARGS__)
#define hri_sercomusart_clear_INTEN_RXBRK_bit(...) alxShimAsf4_Call(0, ##__VA_ARGS__)
#define hri_sercomusart_clear_INTEN_RXC_bit(...) alxShimAsf4_Call(0, ##__VA_ARGS__)
#define hri_sercomusart_clear_INTFLAG_RXBRK_bit(...) alxShimAsf4_Call(0, ##__VA_ARGS__)
#define hri_sercomusart_get_INTEN_DRE_bit(...) alxShimAsf4_Call(0, ##__VA_ARGS__)
#define hri_sercomusart_get_INTFLAG_DRE_bit(...) alxShimAsf4_Call(0, ##__VA_ARGS__)
#define hri_sercomusart_get_INTFLAG_RXBRK_bit(...) alxShimAsf4_Call(0, ##__VA_ARGS__)
#define hri_sercomusart_get_INTFLAG_RXC_bit(...) alxShimAsf4_Call(0, ##__VA_ARGS__)
#define hri_sercomusart_read_DATA_reg(...) alxShimAsf4_Call(0, ##__VA_ARGS__)
#define hri_sercomusart_set_INTEN_DRE_bit(...) alxShimAsf4_Call(0, ##__VA_ARGS__)
#define hri_sercomusart_set_INTEN_RXBRK_bit(...) alxShimAsf4_Call(0, ##__VA_ARGS__)
#define hri_sercomusart_set_INTEN_RXC_bit(...) alxShimAsf4_Call(0, ##__VA_ARGS__)
#define hri_sercomusart_write_DATA_reg(...) alxShimAsf4_Call(0, ##__VA_ARGS__)
#define io_write(...) alxShimAsf4_Call(0, ##__VA_ARGS__)
#define pwm_deinit(...) alxShimAsf4_Call(0, ##__VA_ARGS__)
#define pwm_disable(...) alxShimAsf4_Call(0, ##__VA_ARGS__)
#define pwm_enable(...) alxShimAsf4_Call(0, ##__VA_ARGS__)
#define pwm_init(...) alxShimAsf4_Call(0, ##__VA_ARGS__)
#define pwm_set_parameters(...) alxShimAsf4_Call(0, ##__VA_ARGS__)
#define usart_sync_deinit(...) alxShimAsf4_Call(0, ##__VA_ARGS__)
#define usart_sync_enable(...) alxShimAsf4_Call(0, ##__VA_ARGS__)
#define usart_sync_get_io_descriptor(...) alxShimAsf4_Call(0, ##__VA_ARGS__)
#define usart_sync_init(...) alxShimAsf4_Call(0, ##__VA_ARGS__)


#endif	// ALX_SHIM_ASF4_ASF4HOSTSHIM_H
