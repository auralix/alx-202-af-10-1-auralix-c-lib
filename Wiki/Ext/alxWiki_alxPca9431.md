# Auralix C Library - ALX NFC WLC Power Receiver with LDO Output PCA9431 Module
---
## __Pca9431 datasheet errors__
---
- look at sub-header **PCA9431 interrupt**
- ![alxHwPca9431_datasheet_errors](Img/AlxHwPca9431_datasheet_errors.jpg)
- ![alxHwPca9431_datasheet_errors_20h_right](Img/AlxHwPca9431_datasheet_errors_20h_right.jpg)
- ![AlxHwPca9431_datasheet_errors_21h_right](Img/AlxHwPca9431_datasheet_errors_21h_right.jpg)

## __Supply of PCA9431__
---
- PCA9431 must be supplied on pin VRECT
	- Minimum voltage 4V on pin VRECT, but to everything inside PCA9431 work normally need at least 5V (LDO output could be 5V)
	- Recommended supplied voltage is 6V on pin VRECT
	- Maximum supplied voltage is 12V on pin VRECT
	- 2 options to achieve that:  
		- **a)**  
			* connect 5-12V from power-supply to red header (VRECT) on NXP WLC Listener board
			* GND from power-supply to black header (GND) or to GND Busbar on NXP WLC Listener board
			* see image [alxHwPca_ListenerPowerSupplyPca9431.jpg](../Img/HwPca9431/ListenerPowerSupplyPca9431.jpg)  
			* see image [alxHwPca_ListenerPowerSupplyPca9431_Red_BlackConn.jpg](../Img/HwPca9431/ListenerPowerSupplyPca9431_Red_BlackConn.jpg)
			
		- **b)**  
			* add 3-5V power supply on NXP WLC Poller and put together both antennas (Listener and Poller)
			* (so supply on should between Listener between 3-12V- depends on type of antenna,distance,matching...)


## __I2C communication of PCA9431__
---
- **pca_DevAdrSend = 0b11100010;** // Pca9031
- **pca_DevAdrReceive = 0b11100011;** //Pca9031

## __General__
---
- **implemented** functions: // tested and work
	- **Alx_Status AlxPca9431_Init(AlxPca9431* me**
	- **AlxPca9431_DeInit(AlxPca9431* me)**
	- **AlxPca9431_LdoVout_GetVoltage_V(AlxPca9431* me, float* voltage_V)** // 10 bit ADC, which measure voltage on PCA9431 LDO output. (0-3.3V or 0-5V)
	- **Alx_Status AlxPca9431_LdoVout_GetVoltage_mV(AlxPca9431* me, uint32_t* voltage_mV)** // **No float**, 10 bit ADC, which measure voltage on PCA9431 LDO output in mV. (0-3.3V or 0-5V)
	- **AlxPca9431_LdoVout_GetCurrent_A(AlxPca9431* me, float* current_A)** // 10 bit ADC, which measure output current of PCA9431 LDO output. (0-55mA -normal, 55mA-220mA - over-current, 220mA-600mA protection)
	- **Alx_Status AlxPca9431_LdoVout_GetCurrent_uA(AlxPca9431* me, uint32_t* current_uA))** //**No float**, 10 bit ADC, which measure output current of PCA9431 LDO output in uA. (0-55mA -normal, 55mA-220mA - over-current, 220mA-600mA protection)
	- **AlxPca9431_Rect_GetVoltage_V(AlxPca9431* me, float* voltage_V)** // 10 bit ADC, which measure voltage on supply of PCA9431 - VRECT pin (cca 3-14V - below cca 3V PCA don't respond back)
	- **Alx_Status AlxPca9431_Rect_GetVoltage_mV(AlxPca9431* me, uint32_t* voltage_mV))** //**No float**, 10 bit ADC, which measure voltage on supply of PCA9431 in mV- VRECT pin (cca 3-14V - below cca 3V PCA don't respond back)
	- **AlxPca9431_Rect_GetCurrent_A(AlxPca9431* me, float* current_A)** // 10 bit ADC, which measure input supply current of PCA9431 - VRECT pin (0-432mA)
	- **Alx_Status AlxPca9431_Rect_GetCurrent_uA(AlxPca9431* me, uint32_t* current_uA))** //**No float**, 10 bit ADC, which measure input supply current of PCA9431 in uA- VRECT pin (0-432mA)
	- **AlxPca9431_TempSens_GetTemp_degC(AlxPca9431* me, float* temp_degC)** // measure temperature of PCA9431  (-43°C - 156°C)- non-linear calculation from voltage measurement - 10bit ADC.
	- **Alx_Status AlxPca9431_TempSens_GetTemp_mDegC(AlxPca9431* me, uint32_t* temp_mDegC)** //**No float**, measure temperature of PCA9431  (-43°C - 156°C)- non-linear calculation from voltage measurement in mDegC - 10bit ADC.
	- **AlxPca9431_Exit_EcoMode(AlxPca9431* me)**
	- **AlxPca9431_Reg_ReadAndClearInterrupt(AlxPca9431* me)** // read and then clear  interrupts or group of interrupts  ( System, VRect, VOutLdo interrupts )
	
  
- **not implemented** functions. Assert is triggered when call any function below.
	- **AlxPca9431_VTune_SetVoltage_V(AlxPca9431* me, float* voltage_V)** // 0-3.3V, 5 bit DAC - TODO // to fine tune the antenna
	- **AlxPca9431_VTune_GetVoltage_V(AlxPca9431* me, float* voltage_V)** // 10 bit ADC - TODO
	
  
- **implemented private** functions: // tested and work
	- **AlxPca9431_RegStruct_SetAddr(AlxPca9431* me)**
	- **AlxPca9431_RegStruct_SetLen(AlxPca9431* me)**
	- **AlxPca9431_RegStruct_SetValToZero(AlxPca9431* me)**
	- **AlxPca9431_RegStruct_SetValToDefault(AlxPca9431* me)**
	- **AlxPca9431_Reg_Write(AlxPca9431* me, void* reg)** // write registers true I2C communication
	- **AlxPca9431_Reg_Read(AlxPca9431* me, void* reg)** // read registers true I2C communication
	- **Alx_Status AlxPca9431_Reg_WriteVal(AlxPca9431* me)**
	
  
- **implemented weak** functions: // tested and work
	- **void AlxPca9431_RegStruct_SetVal(AlxPca9431* me)** // Define basic Pca9431 Register Structure function in your application

## __PCA9431 interrupt__
---
- interrupt is automatically cleaned by reading 2 times the same register in a row  
- Delay between reading should be minimum  
- If you use trace or. something similar (which increase delay) between both reading the interrupt output pin may not work  
- image [i2cAlxPca9431_Interrupt.jpg](../Img/HwPca9431/i2cAlxPca9431_Interrupt.jpg)
 
## __PCA9431 LDO__
---
- image [i2c_AlxPca9431_Ldo3V3out.jpg](../Img/HwPca9431/i2c_AlxPca9431_Ldo3V3out.jpg)
- image [i2c_AlxPca9431_Ldo5Vout.jpg](../Img/HwPca9431/i2c_AlxPca9431_Ldo5Vout.jpg)

## __PCA9431 LDO Get Voltage 3V3__
---
- image [i2cAlxPca9431_LdoVou_GetVoltage3V3.jpg](../Img/HwPca9431/i2cAlxPca9431_LdoVou_GetVoltage3V3.jpg)

## __PCA9431 periphery__
---
- VOutLDO => 3v3 ali 5v
- VRECT_UVLO => 3 , 3.1V, 3,2V, 3.3V
- VPWR_VL => 1.8V, 2.5V, 2.8V, 3.3V
- ADCs =>10 bit VRECT,IRECT,VOUT,IOUT,chip temperature, NTC voltage 
- DAC => 5bit VTUNE external varactor
  
## __Ctor Arguments__
---
- **AlxPca9431* me **- Pointer to Pca9431 object
- **AlxI2c* i2c **- Pointer to i2c object
- **uint8_t i2cAddr **- Pca9431 i2c Address
- **AlxIoPin* do_SleepEn **- Pointer digital output pin Sleep Enable
- **AlxIoPin* di_Interrupt **- Pointer digital input pin Interrupt
- **bool i2cCheckWithRead **- True send and than read i2c massage, false only send i2c massage
- **uint8_t i2cNumOfTries **
- **uint16_t i2cTimeout_ms**

---
# __[alxWiki_Home](../../alxWiki_Home.md)__

# __[alxHwNfcWlcListenerV3_5b_JsTest.md](../Hw/HwNfcWlcListenerV3_5b/Test/alxHwNfcWlcListenerV3_5b_JsTest.md)__

# __[alxWiki_alxHwNucleoF429Zi_JsTest.md](../Hw/HwNucleoF429Zi/Test/alxWiki_alxHwNucleoF429Zi_JsTest.md)__