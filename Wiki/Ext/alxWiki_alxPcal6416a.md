# Auralix C Library - ALX <description_TODO> PCAl6416A Module  
---
## General
- Func "Alx_Status AlxPcal6416a_Handle(AlxPcal6416a* me)" only handles reading Input registers InputPort_0 and InputPort_1 and writing to Output registers OutputPort_0 and OutputPort_1
	- All configs should be set in "ALX_WEAK void AlxPcal6416a_RegStruct_SetVal(AlxPcal6416a* me)" func
- IRQs are not implemented

---
## Ctor Arguments
- __AlxPcal6416a* me__ - Pointer to Pcal6416a object
- __AlxIoPin** ioPinArr__ - Pointer to array of IoPin pointers
- __AlxPcal6416a_PortPin* portPinArr__ - Pointer to array of Pcal6416a pins
- __AlxI2c* i2c__ - Pointer to I2c object
- __uint8_t i2cAddr__ - I2c slave address
- __uint8_t numOfIoPins__ - Number of IoPins used for Pcal6416a
- __bool i2cCheckWithRead__ - todo
- __uint8_t i2cNumOfTries__ - Number of I2c Write and Read tries
- __uint16_t i2cTimeout_ms__ - I2c timeout in ms
