# Auralix C Library - ALX MCU LPC80x Trace Module
---
## General
- BaudRate is configurable
- Other hardcoded settings:
    - Data Bits: __8__
    - Parity: __None__
    - Stop Bits: __1__
    - Flow Control: __None__

---
## Ctor Arguments
- __AlxTrace* me__ - Pointer to Trace object
- __uint8_t port__ - Port
- __uint8_t pin__ - Pin
- __USART_Type* usart__ - Usart peripheral base pointer
- __AlxGlobal_BaudRate baudRate__ - BaudRate in bits per second
