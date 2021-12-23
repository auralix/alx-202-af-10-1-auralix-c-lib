# Auralix C Library - ALX HW Nucleo-F429ZI JS Test Module PCA9431

## Used HW PCBs
- HwNfcWlcListenerV3_7	--[ID2100105](https://auralix.eu.teamwork.com/#/tasks/25808230)  
- HwNfcWlcPollerV3_7	--[ID2100138](https://auralix.eu.teamwork.com/#/tasks/25808264)  
- Nucleo-F429ZI			--[ID2100212](https://auralix.eu.teamwork.com/#/tasks/25927281)  


## Supply of PCA9431 on HwNfcWlcListenerV3_7
- PCA9431 must be supplied on pin VRECT
	- Minimum voltage 4V on pin VRECT, but to everything inside PCA9431 work normally need at least 5V (LDO output could be 5V)
	- Recommended supplied voltage is 6V on pin VRECT
	- Maximum supplied voltage is 12V on pin VRECT
	- 2 options to achieve that:  
		- **a)**  
			* connect 5-12V from power-supply to red header (VRECT) on NXP WLC Listener board
			* GND from power-supply to black header (GND) or to GND Busbar on NXP WLC Listener board
			* see image [alxHwPca_ListenerPowerSupplyPca9431.jpg](Img/HwPca9431/ListenerPowerSupplyPca9431.jpg)  
		- **b)**  
			* add 3-5V power supply on NXP WLC Poller and put together both antennas (Listener and Poller)
			* (so supply on should between Listener between 3-12V- depends on type of antenna,distance,matching...)
## Connection between HW
### Connection between HwNfcWlcPollerV3_7 and HwNfcWlcListenerV3_7.
	- chosen option **b)** of Supply of PCA9431 on HwNfcWlcListenerV3_7
		* add 3-5V power supply on NXP WLC Poller and put together both antennas (Listener and Poller)
		* img [HwNfcWlcPollerV3_7 and HwNfcWlcListenerV3_7](..\..\..\Img\testHwNucleoF429Zi & HwNfcWlcListenerV3_7\Connection_Hw_Wlc_PollerV3_7&ListenerV3_7.jpg)
		
### Connection between HwNfcWlcListenerV3_7 and Nucleo-F429ZI
	-i2c--Scl
		* J201_pin 4 -- I2C.SCL on HwNfcWlcListenerV3_7 [white wire](..\..\..\Img\testHwNucleoF429Zi & HwNfcWlcListenerV3_7\Connection_Hw_Wlc_PollerV3_7&ListenerV3_7.jpg)
		* pin PB10 on Nucleo-F429ZI [white wire](..\..\..\Img\testHwNucleoF429Zi & HwNfcWlcListenerV3_7\Nucleo-F429ZI.jpg)
	-i2c--Sda
		* J201_pin 1 -- I2C.SDA on HwNfcWlcListenerV3_7 [green wire](..\..\..\Img\testHwNucleoF429Zi & HwNfcWlcListenerV3_7\Connection_Hw_Wlc_PollerV3_7&ListenerV3_7.jpg)
		* pin PB10 on Nucleo-F429ZI [green wire](..\..\..\Img\testHwNucleoF429Zi & HwNfcWlcListenerV3_7\Nucleo-F429ZI.jpg)
	- GND
		* J201_pin 3 -- GND on HwNfcWlcListenerV3_7 [black wire](..\..\..\Img\testHwNucleoF429Zi & HwNfcWlcListenerV3_7\Connection_Hw_Wlc_PollerV3_7&ListenerV3_7.jpg)
		* one of GND pins on Nucleo-F429ZI [black wire](..\..\..\Img\testHwNucleoF429Zi & HwNfcWlcListenerV3_7\Nucleo-F429ZI.jpg)
	-interrupt
		* J202_pin 10 -- PCA943X_nINT on HwNfcWlcListenerV3_7 [yellow wire](..\..\..\Img\testHwNucleoF429Zi & HwNfcWlcListenerV3_7\Connection_Hw_Wlc_PollerV3_7&ListenerV3_7.jpg)
		* pin PC3 on Nucleo-F429ZI [yellow wire](..\..\..\Img\testHwNucleoF429Zi & HwNfcWlcListenerV3_7\Nucleo-F429ZI.jpg)
	-sleep-EN
		* J202_pin 9 -- PCA943X_EN n HwNfcWlcListenerV3_7 [orange wire](..\..\..\Img\testHwNucleoF429Zi & HwNfcWlcListenerV3_7\Connection_Hw_Wlc_PollerV3_7&ListenerV3_7.jpg)
		* pin PA9 on Nucleo-F429ZI [orange wire](..\..\..\Img\testHwNucleoF429Zi & HwNfcWlcListenerV3_7\Nucleo-F429ZI.jpg)
### I2c pull up	
	-i2c--pull up **in this case connected to power-supply**
		*	J201_pin 2 -- VDD_PULL on HwNfcWlcListenerV3_7 [purple wire](..\..\..\Img\testHwNucleoF429Zi & HwNfcWlcListenerV3_7\Connection_Hw_Wlc_PollerV3_7&ListenerV3_7.jpg)
		*	J202_pin 24 -- GND on HwNfcWlcListenerV3_7 [blue wire](..\..\..\Img\testHwNucleoF429Zi & HwNfcWlcListenerV3_7\Connection_Hw_Wlc_PollerV3_7&ListenerV3_7.jpg)
		
### Connection to saleae logic analyzer
	*Ch 0 -> I2C.SDA [green wire](..\..\..\Img\testHwNucleoF429Zi & HwNfcWlcListenerV3_7\alxWiki_alxHwNucleoF429Zi_JsTest_AllHw.jpg)
	*Ch 1 -> I2C.SCL [white wire](..\..\..\Img\testHwNucleoF429Zi & HwNfcWlcListenerV3_7\alxWiki_alxHwNucleoF429Zi_JsTest_AllHw.jpg)
	*GND  -> [black wire](..\..\..\Img\testHwNucleoF429Zi & HwNfcWlcListenerV3_7\alxWiki_alxHwNucleoF429Zi_JsTest_AllHw.jpg)
	
## Tests
### AlxPca9431_Init(&Pca9431)
- 1) Init GPIO
- 2) Init I2C
- 4) Set registers values to default [i2c_AlxPca9431_Init_SetToDefault](..\..\..\Img\testHwNucleoF429Zi & HwNfcWlcListenerV3_7\i2c_AlxPca9431_Init_SetToDefault.jpg)
- 6) Set registers values - WEAK [AlxPca9431_RegStruct_SetVal](..\..\..\Img\testHwNucleoF429Zi & HwNfcWlcListenerV3_7\i2c_AlxPca9431_Init_SetSpecificReg.jpg)
- 7) Write registers
	* image [AlxPca9431_Init_dec](..\..\..\Img\testHwNucleoF429Zi & HwNfcWlcListenerV3_7\i2c_AlxPca9431_Init_dec.jpg)
	* image [AlxPca9431_Init_hex_1/2](..\..\..\Img\testHwNucleoF429Zi & HwNfcWlcListenerV3_7\i2c_AlxPca9431_Init_hex_1.jpg)
	* image [AlxPca9431_Init_hex_2/2](..\..\..\Img\testHwNucleoF429Zi & HwNfcWlcListenerV3_7\i2c_AlxPca9431_Init_hex_2.jpg)
### AlxPca9431_Reg_ReadAndClearInterrupt(&Pca9431)
- image [i2cAlxPca9431_ReadAndclearInterrupt](..\..\..\Img\testHwNucleoF429Zi & HwNfcWlcListenerV3_7\i2cAlxPca9431_ReadAndclearInterrupt.jpg)
	* zoom 1st:
		- image [i2cAlxPca9431_ReadAndclearInterrupt_zoom_1st_bin](..\..\..\Img\testHwNucleoF429Zi & HwNfcWlcListenerV3_7\i2cAlxPca9431_ReadAndclearInterrupt_zoom_1st_bin.jpg)
		- image [i2cAlxPca9431_ReadAndclearInterrupt_zoom_1st_dec](..\..\..\Img\testHwNucleoF429Zi & HwNfcWlcListenerV3_7\i2cAlxPca9431_ReadAndclearInterrupt_zoom_1st_dec.jpg)
		- image [i2cAlxPca9431_ReadAndclearInterrupt_zoom_1st_hex](..\..\..\Img\testHwNucleoF429Zi & HwNfcWlcListenerV3_7\i2cAlxPca9431_ReadAndclearInterrupt_zoom_1st_hex.jpg)
	* zoom 2nd:
		- image [i2cAlxPca9431_ReadAndclearInterrupt_zoom_2nd_bin](..\..\..\Img\testHwNucleoF429Zi & HwNfcWlcListenerV3_7\i2cAlxPca9431_ReadAndclearInterrupt_zoom_2nd_bin.jpg)
		- image [i2cAlxPca9431_ReadAndclearInterrupt_zoom_2nd_dec](..\..\..\Img\testHwNucleoF429Zi & HwNfcWlcListenerV3_7\i2cAlxPca9431_ReadAndclearInterrupt_zoom_2nd_dec.jpg)
		- image [i2cAlxPca9431_ReadAndclearInterrupt_zoom_2nd_hex](..\..\..\Img\testHwNucleoF429Zi & HwNfcWlcListenerV3_7\i2cAlxPca9431_ReadAndclearInterrupt_zoom_2nd_hex.jpg)
	* zoom 3rd:
		- image [i2cAlxPca9431_ReadAndclearInterrupt_zoom_3rd_bin](..\..\..\Img\testHwNucleoF429Zi & HwNfcWlcListenerV3_7\i2cAlxPca9431_ReadAndclearInterrupt_zoom_3rd_bin.jpg)
		- image [i2cAlxPca9431_ReadAndclearInterrupt_zoom_3rd_dec](..\..\..\Img\testHwNucleoF429Zi & HwNfcWlcListenerV3_7\i2cAlxPca9431_ReadAndclearInterrupt_zoom_3rd_dec.jpg)
		- image [i2cAlxPca9431_ReadAndclearInterrupt_zoom_3rd_hex](..\..\..\Img\testHwNucleoF429Zi & HwNfcWlcListenerV3_7\i2cAlxPca9431_ReadAndclearInterrupt_zoom_3rd_hex.jpg)
### AlxPca9431_LdoVout_GetVoltage_V(&Pca9431, &LdoVout_Voltage_V)
- image [i2cAlxPca9431_LdoVou_GetVoltage_bin](..\..\..\Img\testHwNucleoF429Zi & HwNfcWlcListenerV3_7\i2cAlxPca9431_LdoVou_GetVoltage_bin.jpg)
- image [i2cAlxPca9431_LdoVou_GetVoltage_dec](..\..\..\Img\testHwNucleoF429Zi & HwNfcWlcListenerV3_7\i2cAlxPca9431_LdoVou_GetVoltage_dec.jpg)
- image [i2cAlxPca9431_LdoVou_GetVoltage_hex](..\..\..\Img\testHwNucleoF429Zi & HwNfcWlcListenerV3_7\i2cAlxPca9431_LdoVou_GetVoltage_hex.jpg)
### AlxPca9431_LdoVout_GetCurrent_A(&Pca9431, &LdoVout_Current_A)
- image [i2cAlxPca9431_LdoVou_GetCurrent_bin](..\..\..\Img\testHwNucleoF429Zi & HwNfcWlcListenerV3_7\i2cAlxPca9431_LdoVou_GetCurrent_bin.jpg)
- image [i2cAlxPca9431_LdoVou_GetCurrent_dec](..\..\..\Img\testHwNucleoF429Zi & HwNfcWlcListenerV3_7\i2cAlxPca9431_LdoVou_GetCurrent_dec.jpg)
- image [i2cAlxPca9431_LdoVou_GetCurrent_hex](..\..\..\Img\testHwNucleoF429Zi & HwNfcWlcListenerV3_7\i2cAlxPca9431_LdoVou_GetCurrent_hex.jpg)
### AlxPca9431_Rect_GetVoltage_V(&Pca9431, &Rect_Voltage_V)
- image [i2cAlxPca9431_Rect_GetVoltage_bin](..\..\..\Img\testHwNucleoF429Zi & HwNfcWlcListenerV3_7\i2cAlxPca9431_Rect_GetVoltage_bin.jpg)
- image [i2cAlxPca9431_Rect_GetVoltage_dec](..\..\..\Img\testHwNucleoF429Zi & HwNfcWlcListenerV3_7\i2cAlxPca9431_Rect_GetVoltage_dec.jpg)
- image [i2cAlxPca9431_Rect_GetVoltage_hex](..\..\..\Img\testHwNucleoF429Zi & HwNfcWlcListenerV3_7\i2cAlxPca9431_Rect_GetVoltage_hex.jpg)
### Changing voltage on **VRECT** pin. 
	-changing voltage from  4,5V to 3V
	- interrupt at 4V (Low voltage on VRECT pin)
	- PCA9431 shut down below 3,3V
	- image [i2cAlxPca9431_VrectChangingPowerSupply](..\..\..\Img\testHwNucleoF429Zi & HwNfcWlcListenerV3_7\i2cAlxPca9431_VrectChangingPowerSupply.jpg)