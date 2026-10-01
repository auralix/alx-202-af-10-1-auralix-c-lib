/**
  ******************************************************************************
  * @file		zephyrHostShim.h
  * @brief		Auralix C Library - Configuration Matrix Fake - Zephyr
  * @copyright	Copyright (C) Auralix d.o.o. All rights reserved.
  *
  * Compile-only: every name of Zephyr the alx code uses, written from
  * that use, so the configuration matrix compiles every alx line under
  * ALX_ZEPHYR (Test/config/matrix.toml). Nothing here is the vendor's and
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

#ifndef ALX_SHIM_ZEPHYR_ZEPHYRHOSTSHIM_H
#define ALX_SHIM_ZEPHYR_ZEPHYRHOSTSHIM_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "cmsisHostShim.h"


typedef struct alxShimZephyr_Sub
{
	uint16_t acquisition_time;
	size_t buffer_size;
	bool calibrate;
	uint8_t channel_id;
	uint32_t channels;
	size_t count;
	uint32_t cs;
	uint8_t differential;
	uint32_t frequency;
	uint32_t gain;
	size_t len;
	uint32_t operation;
	uint8_t oversampling;
	uint32_t reference;
	uint8_t resolution;
	uint16_t slave;
	void* buf;
	void* buffer;
	void* buffers;
	void* options;
} alxShimZephyr_Sub;

typedef struct alxShimZephyr_Struct
{
	uint16_t acquisition_time;
	size_t buffer_size;
	bool calibrate;
	uint8_t channel_id;
	uint32_t channels;
	size_t count;
	uint32_t cs;
	uint8_t differential;
	uint32_t frequency;
	uint32_t gain;
	size_t len;
	uint32_t operation;
	uint8_t oversampling;
	uint32_t reference;
	uint8_t resolution;
	uint16_t slave;
	void* buf;
	void* buffer;
	void* buffers;
	void* options;
} alxShimZephyr_Struct;

struct adc_channel_cfg
{
	uint16_t acquisition_time;
	size_t buffer_size;
	bool calibrate;
	uint8_t channel_id;
	uint32_t channels;
	size_t count;
	uint32_t cs;
	uint8_t differential;
	uint32_t frequency;
	uint32_t gain;
	size_t len;
	uint32_t operation;
	uint8_t oversampling;
	uint32_t reference;
	uint8_t resolution;
	uint16_t slave;
	void* buf;
	void* buffer;
	void* buffers;
	void* options;
};
struct adc_sequence
{
	uint16_t acquisition_time;
	size_t buffer_size;
	bool calibrate;
	uint8_t channel_id;
	uint32_t channels;
	size_t count;
	uint32_t cs;
	uint8_t differential;
	uint32_t frequency;
	uint32_t gain;
	size_t len;
	uint32_t operation;
	uint8_t oversampling;
	uint32_t reference;
	uint8_t resolution;
	uint16_t slave;
	void* buf;
	void* buffer;
	void* buffers;
	void* options;
};
struct gpio_callback
{
	uint16_t acquisition_time;
	size_t buffer_size;
	bool calibrate;
	uint8_t channel_id;
	uint32_t channels;
	size_t count;
	uint32_t cs;
	uint8_t differential;
	uint32_t frequency;
	uint32_t gain;
	size_t len;
	uint32_t operation;
	uint8_t oversampling;
	uint32_t reference;
	uint8_t resolution;
	uint16_t slave;
	void* buf;
	void* buffer;
	void* buffers;
	void* options;
};
struct k_event
{
	uint16_t acquisition_time;
	size_t buffer_size;
	bool calibrate;
	uint8_t channel_id;
	uint32_t channels;
	size_t count;
	uint32_t cs;
	uint8_t differential;
	uint32_t frequency;
	uint32_t gain;
	size_t len;
	uint32_t operation;
	uint8_t oversampling;
	uint32_t reference;
	uint8_t resolution;
	uint16_t slave;
	void* buf;
	void* buffer;
	void* buffers;
	void* options;
};
struct k_mutex
{
	uint16_t acquisition_time;
	size_t buffer_size;
	bool calibrate;
	uint8_t channel_id;
	uint32_t channels;
	size_t count;
	uint32_t cs;
	uint8_t differential;
	uint32_t frequency;
	uint32_t gain;
	size_t len;
	uint32_t operation;
	uint8_t oversampling;
	uint32_t reference;
	uint8_t resolution;
	uint16_t slave;
	void* buf;
	void* buffer;
	void* buffers;
	void* options;
};
struct k_thread
{
	uint16_t acquisition_time;
	size_t buffer_size;
	bool calibrate;
	uint8_t channel_id;
	uint32_t channels;
	size_t count;
	uint32_t cs;
	uint8_t differential;
	uint32_t frequency;
	uint32_t gain;
	size_t len;
	uint32_t operation;
	uint8_t oversampling;
	uint32_t reference;
	uint8_t resolution;
	uint16_t slave;
	void* buf;
	void* buffer;
	void* buffers;
	void* options;
};
struct spi_buf
{
	uint16_t acquisition_time;
	size_t buffer_size;
	bool calibrate;
	uint8_t channel_id;
	uint32_t channels;
	size_t count;
	uint32_t cs;
	uint8_t differential;
	uint32_t frequency;
	uint32_t gain;
	size_t len;
	uint32_t operation;
	uint8_t oversampling;
	uint32_t reference;
	uint8_t resolution;
	uint16_t slave;
	void* buf;
	void* buffer;
	void* buffers;
	void* options;
};
struct spi_buf_set
{
	uint16_t acquisition_time;
	size_t buffer_size;
	bool calibrate;
	uint8_t channel_id;
	uint32_t channels;
	size_t count;
	uint32_t cs;
	uint8_t differential;
	uint32_t frequency;
	uint32_t gain;
	size_t len;
	uint32_t operation;
	uint8_t oversampling;
	uint32_t reference;
	uint8_t resolution;
	uint16_t slave;
	void* buf;
	void* buffer;
	void* buffers;
	void* options;
};
struct spi_config
{
	uint16_t acquisition_time;
	size_t buffer_size;
	bool calibrate;
	uint8_t channel_id;
	uint32_t channels;
	size_t count;
	uint32_t cs;
	uint8_t differential;
	uint32_t frequency;
	uint32_t gain;
	size_t len;
	uint32_t operation;
	uint8_t oversampling;
	uint32_t reference;
	uint8_t resolution;
	uint16_t slave;
	void* buf;
	void* buffer;
	void* buffers;
	void* options;
};
enum adc_gain { alxShimZephyr_adc_gain = 0 };
typedef uint32_t gpio_flags_t;
typedef uint8_t gpio_pin_t;
typedef uint32_t gpio_port_pins_t;
typedef alxShimZephyr_Struct k_thread_stack_t;
typedef struct k_thread* k_tid_t;
typedef uint32_t k_timeout_t;
typedef uint32_t spi_operation_t;

extern alxShimZephyr_Struct alxShimZephyr_Instance;
#define ADC_ACQ_TIME_DEFAULT (0x0100U)
#define ADC_REF_INTERNAL (0x0101U)
#define GPIO_DISCONNECTED (0x0102U)
#define GPIO_INT_DISABLE (0x0103U)
#define GPIO_PULL_DOWN (0x0104U)
#define GPIO_PULL_UP (0x0105U)
#define K_FOREVER (0x0106U)
#define K_NO_WAIT (0x0107U)

uint32_t alxShimZephyr_Call(int unused, ...);
void* alxShimZephyr_CallPtr(int unused, ...);
#define BIT(...) alxShimZephyr_Call(0, ##__VA_ARGS__)
#define DEVICE_DT_GET_OR_NULL(...) alxShimZephyr_CallPtr(0, ##__VA_ARGS__)
#define K_MSEC(...) alxShimZephyr_Call(0, ##__VA_ARGS__)
#define K_USEC(...) alxShimZephyr_Call(0, ##__VA_ARGS__)
#define adc_channel_setup(...) alxShimZephyr_Call(0, ##__VA_ARGS__)
#define adc_raw_to_millivolts(...) alxShimZephyr_Call(0, ##__VA_ARGS__)
#define adc_read(...) alxShimZephyr_Call(0, ##__VA_ARGS__)
#define adc_ref_internal(...) alxShimZephyr_Call(0, ##__VA_ARGS__)
#define device_get_binding(...) alxShimZephyr_CallPtr(0, ##__VA_ARGS__)
#define gpio_add_callback(...) alxShimZephyr_Call(0, ##__VA_ARGS__)
#define gpio_init_callback(...) alxShimZephyr_Call(0, ##__VA_ARGS__)
#define gpio_pin_configure(...) alxShimZephyr_Call(0, ##__VA_ARGS__)
#define gpio_pin_get(...) alxShimZephyr_Call(0, ##__VA_ARGS__)
#define gpio_pin_interrupt_configure(...) alxShimZephyr_Call(0, ##__VA_ARGS__)
#define gpio_pin_set(...) alxShimZephyr_Call(0, ##__VA_ARGS__)
#define gpio_pin_toggle(...) alxShimZephyr_Call(0, ##__VA_ARGS__)
#define gpio_remove_callback(...) alxShimZephyr_Call(0, ##__VA_ARGS__)
#define i2c_burst_read(...) alxShimZephyr_Call(0, ##__VA_ARGS__)
#define i2c_burst_write(...) alxShimZephyr_Call(0, ##__VA_ARGS__)
#define i2c_read(...) alxShimZephyr_Call(0, ##__VA_ARGS__)
#define i2c_write(...) alxShimZephyr_Call(0, ##__VA_ARGS__)
#define i2c_write_read(...) alxShimZephyr_Call(0, ##__VA_ARGS__)
#define k_event_clear(...) alxShimZephyr_Call(0, ##__VA_ARGS__)
#define k_event_init(...) alxShimZephyr_Call(0, ##__VA_ARGS__)
#define k_event_post(...) alxShimZephyr_Call(0, ##__VA_ARGS__)
#define k_event_test(...) alxShimZephyr_Call(0, ##__VA_ARGS__)
#define k_event_wait(...) alxShimZephyr_Call(0, ##__VA_ARGS__)
#define k_event_wait_all(...) alxShimZephyr_Call(0, ##__VA_ARGS__)
#define k_mutex_init(...) alxShimZephyr_Call(0, ##__VA_ARGS__)
#define k_mutex_lock(...) alxShimZephyr_Call(0, ##__VA_ARGS__)
#define k_mutex_unlock(...) alxShimZephyr_Call(0, ##__VA_ARGS__)
#define k_sleep(...) alxShimZephyr_Call(0, ##__VA_ARGS__)
#define k_thread_abort(...) alxShimZephyr_Call(0, ##__VA_ARGS__)
#define k_thread_create(...) alxShimZephyr_CallPtr(0, ##__VA_ARGS__)
#define k_thread_join(...) alxShimZephyr_Call(0, ##__VA_ARGS__)
#define k_ticks_to_ns_floor64(...) alxShimZephyr_Call(0, ##__VA_ARGS__)
#define k_uptime_ticks(...) alxShimZephyr_Call(0, ##__VA_ARGS__)
#define k_yield(...) alxShimZephyr_Call(0, ##__VA_ARGS__)
#define spi_read(...) alxShimZephyr_Call(0, ##__VA_ARGS__)
#define spi_transceive(...) alxShimZephyr_Call(0, ##__VA_ARGS__)
#define spi_write(...) alxShimZephyr_Call(0, ##__VA_ARGS__)
#define uart_poll_out(...) alxShimZephyr_Call(0, ##__VA_ARGS__)

#define CONTAINER_OF(ptr, type, field) ((type*)(void*)((char*)(ptr) - offsetof(type, field)))
#define DT_CHOSEN(prop) 0U
typedef void (*k_thread_entry_t)(void* p1, void* p2, void* p3);

// IS_ENABLED(CONFIG_X) is 1 when the row defines CONFIG_X as 1, else 0, in #if as in code
#define IS_ENABLED(config) alxShimZephyr_IsEnabled1(config)
#define alxShimZephyr_IsEnabled1(value) alxShimZephyr_IsEnabled2(alxShimZephyr_Probe_##value)
#define alxShimZephyr_Probe_1 alxShimZephyr_Unused,
#define alxShimZephyr_IsEnabled2(probe) alxShimZephyr_IsEnabled3(probe 1, 0, ~)
#define alxShimZephyr_IsEnabled3(ignore, value, ...) value

// The logging and the statically defined mutex of alxTrace_McuZephyr.c with ALX_TRACE_ZEPHYR_WITH_LOG
#define LOG_LEVEL_INF 3
#define LOG_MODULE_REGISTER(name, level) extern int alxShimZephyr_LogModule_##name
#define LOG_INF(...) ((void)alxShimZephyr_Call(0, ##__VA_ARGS__))
#define K_MUTEX_DEFINE(name) struct k_mutex name

#endif	// ALX_SHIM_ZEPHYR_ZEPHYRHOSTSHIM_H
