# Auralix C Library - ALX MCU LPC80x I2C Module
---
## General
- Functions below were copied from FSL so that Timeout was added
    - __static status_t AlxI2c_MasterStart(AlxI2c* me, I2C_Type* base, uint8_t address, i2c_direction_t direction, uint16_t timeout)__
    - __static status_t AlxI2c_MasterWriteBlocking(AlxI2c* me, I2C_Type* base, const void* txBuff, size_t txSize, uint32_t flags, uint16_t timeout)__
    - __static status_t AlxI2c_MasterReadBlocking(AlxI2c* me, I2C_Type* base, void* rxBuff, size_t rxSize, uint32_t flags, uint16_t timeout)__
    - __static uint32_t AlxI2c_PendingStatusWait(AlxI2c* me, I2C_Type* base, uint16_t timeout)__
- These functiones are not implemented. Assert is triggered when call any function below.
    - __Alx_Status AlxI2c_Master_StartRead(AlxI2c* me, uint16_t slaveAddr, uint8_t* data, uint16_t len, uint16_t timeout_ms)__
    - __Alx_Status AlxI2c_Master_StartReadStop(AlxI2c* me, uint16_t slaveAddr, uint8_t* data, uint16_t len, uint8_t numOfTries, uint16_t timeout_ms)__
    - __Alx_Status AlxI2c_Master_StartWrite(AlxI2c* me, uint16_t slaveAddr, const uint8_t* data, uint16_t len, uint16_t timeout_ms)__
    - __Alx_Status AlxI2c_Master_StartWriteStop(AlxI2c* me, uint16_t slaveAddr, const uint8_t* data, uint16_t len, uint8_t numOfTries, uint16_t timeout_ms)__
    - __Alx_Status AlxI2c_Master_Stop(AlxI2c* me, uint16_t timeout_ms)__
    - __Alx_Status AlxI2c_Master_IsSlaveReady(AlxI2c* me, uint16_t slaveAddr, uint8_t numOfTries, uint16_t timeout_ms)__

---
## Ctor Arguments
- __AlxI2c* me__ - Pointer to I2c object
- __I2C_Type* i2c__ - I2c peripheral base pointer
- __AlxIoPin* io_SCL__ - Pointer to SCL IoPin
- __AlxIoPin* io_SDA__ - Pointer to SDA IoPin
- __AlxI2c_Clk clk__ - I2c clock config enum
