# Auralix C Library - ALX MCU LPC55S6x Trace Module
---
## General
- Every USART instance uses FLEXCOMM with the same address and ID meaning USART0 has the same address and ID as FLEXCOMM0, USART1 has the same address and ID as FLEXCOMM1,...
- __USART_Init()__ function handles everything for FlexComm perihp
- BaudRate is configurable
- Other hardcoded settings:
    - Data Bits: __8__
    - Parity: __None__
    - Stop Bits: __1__
    - Flow Control: __None__
- Trace was tested only on __Port=0 Pin=30__ and __Port=1 Pin=11__ 

---
## Ctor Arguments
- __AlxTrace* me__ - Pointer to Trace object
- __uint8_t port__ - Port
- __uint8_t pin__ - Pin
- __USART_Type* usart__ - Usart peripheral base pointer
- __AlxGlobal_BaudRate baudRate__ - BaudRate in bits per second
