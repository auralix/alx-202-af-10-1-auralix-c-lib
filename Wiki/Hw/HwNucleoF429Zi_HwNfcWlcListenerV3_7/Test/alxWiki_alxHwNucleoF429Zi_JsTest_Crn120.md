# Auralix C Library - ALX HW Nucleo-F429ZI JS Test Module CRN120

## **Used HW PCBs**  
- HwNfcWlcListenerV3_7	--[ID2100105](https://auralix.eu.teamwork.com/#/tasks/25808230)  
- HwNfcWlcPollerV3_7	--[ID2100138](https://auralix.eu.teamwork.com/#/tasks/25808264)  
- Nucleo-F429ZI			--[ID2100212](https://auralix.eu.teamwork.com/#/tasks/25927281)  

## Supply of Crn120  
- CRN120 must be supplied with 3,3V on pin VCC  
	- chosen option:  
		- **b)**  from CRN_VCC on HwNfcWlc**Listener**  V3_7  
			* Resistor R110 or R223 must **not** be placed  
				- image [ListenerCrn120SupplyResistorOff](../Img/HwCrn120/ListenerCrn120SupplyResistorOff.jpg)  
			* connect 3.3V to  J202_pin 6 -- CRN_VCC on HwNfcWlc**Listener** V3_5b or V3_7  

## **Tests**  

### regasession0  
- **Read**  
	// 1. Start condition (with write), address +ACK, mema +ACK, rega +ACK and stop  
	* AlxI2c_Master_StartWriteMemStop_Multi(&doi_I2c_Master, crn_DevAdrSend, CRN120_MEMA_SESSION_REGISTERS, AlxI2c_Master_MemAddrLen_8bit, regasession0, 1, false, 1, 1000)  
	// 2. Start condition (with read)+ACK, read reg data +NAK and stop  
	* AlxI2c_Master_StartReadStop(&doi_I2c_Master, crn_DevAdrSend, data, 1, 1, 1000)  
	* image [i2c_AlxCrn120_regassion0_read](../../../Img/test_HwNucleoF429Zi_HwNfcWlcListenerV3_7/i2c_AlxCrn120_regassion0_read.jpg)  
	
- **Write**  
	// 2. Start condition (with write), address +ACK, mema +ACK, rega +ACK, mask +ACK, data +ACK. stop  
	* AlxI2c_Master_StartWriteStop(&doi_I2c_Master, crn_DevAdrSend, data2, 4, 1, 1000)  
	* image [i2c_AlxCrn120_regassion0_write](../../../Img/test_HwNucleoF429Zi_HwNfcWlcListenerV3_7/i2c_AlxCrn120_regassion0_write.jpg)  

### regasession1  
- **Write**  
	* AlxI2c_Master_StartWriteMemStop_Multi(&doi_I2c_Master, crn_DevAdrSend, CRN120_MEMA_SESSION_REGISTERS, AlxI2c_Master_MemAddrLen_8bit, regasession1, 1, false, 1, 1000)  
- **Read**  
	* AlxI2c_Master_StartReadStop(&doi_I2c_Master, crn_DevAdrSend, data, 1, 1, 1000)  
- **image**  
	* image [i2c_AlxCrn120_regassion1](../../../Img/test_HwNucleoF429Zi_HwNfcWlcListenerV3_7/i2c_AlxCrn120_regassion1.jpg)  

### regasession2  
 - **Write**  
	* AlxI2c_Master_StartWriteMemStop_Multi(&doi_I2c_Master, crn_DevAdrSend, CRN120_MEMA_SESSION_REGISTERS, AlxI2c_Master_MemAddrLen_8bit, regasession2, 1, false, 1, 1000)  
- **Read**  
	* AlxI2c_Master_StartReadStop(&doi_I2c_Master, crn_DevAdrSend, data, 1, 1, 1000)  
- **image**  
	* image [i2c_AlxCrn120_regassion2](../../../Img/test_HwNucleoF429Zi_HwNfcWlcListenerV3_7/i2c_AlxCrn120_regassion2.jpg)  

### regasession3  
 - **Write**  
	* AlxI2c_Master_StartWriteMemStop_Multi(&doi_I2c_Master, crn_DevAdrSend, CRN120_MEMA_SESSION_REGISTERS, AlxI2c_Master_MemAddrLen_8bit, regasession3, 1, false, 1, 1000)  
- **Read**  
	* AlxI2c_Master_StartReadStop(&doi_I2c_Master, crn_DevAdrSend, data, 1, 1, 1000)  
- **image**  
	* image [i2c_AlxCrn120_regassion3](../../../Img/test_HwNucleoF429Zi_HwNfcWlcListenerV3_7/i2c_AlxCrn120_regassion3.jpg)  
	
### regasession4  
 - **Write**  
	* AlxI2c_Master_StartWriteMemStop_Multi(&doi_I2c_Master, crn_DevAdrSend, CRN120_MEMA_SESSION_REGISTERS, AlxI2c_Master_MemAddrLen_8bit, regasession4, 1, false, 1, 1000)  
- **Read**  
	* AlxI2c_Master_StartReadStop(&doi_I2c_Master, crn_DevAdrSend, data, 1, 1, 1000)  
- **image**  
	* image [i2c_AlxCrn120_regassion4](../../../Img/test_HwNucleoF429Zi_HwNfcWlcListenerV3_7/i2c_AlxCrn120_regassion4.jpg)  
	
### regasession5  
- **Write**  
	* AlxI2c_Master_StartWriteMemStop_Multi(&doi_I2c_Master, crn_DevAdrSend, CRN120_MEMA_SESSION_REGISTERS, AlxI2c_Master_MemAddrLen_8bit, regasession5, 1, false, 1, 1000)  
- **Read**  
	* AlxI2c_Master_StartReadStop(&doi_I2c_Master, crn_DevAdrSend, data, 1, 1, 1000)  
- **image**  
	* image [i2c_AlxCrn120_regassion5](../../../Img/test_HwNucleoF429Zi_HwNfcWlcListenerV3_7/i2c_AlxCrn120_regassion5.jpg)  