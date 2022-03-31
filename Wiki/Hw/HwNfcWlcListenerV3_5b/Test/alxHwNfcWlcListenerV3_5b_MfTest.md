# Auralix C Library - ALX HW NFC WLC Listener V3_5b MF Test Module

## __G01_BringUp__
---
Everything is according to new testing system

## __G02_I2cCrn120__
---
- Testing I2c Functions using CRN120

### __Configurations__
- __AlxCk__: AlxClk_Config_McuLpc80x_FroOsc_30MHz_Mainclk_15MHz_CoreSysClk_15MHz
- __AlxTrace__: AlxGlobal_BaudRate_115200
- __AlxI2c__: AlxI2c_Clk_McuLpc80x_BitRate_400kHz

### __Test List__ 
- __AlxHwNfcWlcListenerV3_5b_Main_MfTest_G02_I2cCrn120_T01_Led(me)__
	- /
- __AlxHwNfcWlcListenerV3_5b_Main_MfTest_G02_I2cCrn120_T02_IsSlaveReady(me)__
	- When NACK is returned
	- ![AlxHwNfcWlcListenerV3_5b_Main_MfTest_G02_I2cCrn120_T02_IsSlaveReady_01](Img/AlxHwNfcWlcListenerV3_5b_Main_MfTest_G02_I2cCrn120_T02_IsSlaveReady_01.jpg)
	- When ACK is returned
	- ![AlxHwNfcWlcListenerV3_5b_Main_MfTest_G02_I2cCrn120_T02_IsSlaveReady_02](Img/AlxHwNfcWlcListenerV3_5b_Main_MfTest_G02_I2cCrn120_T02_IsSlaveReady_02.jpg)
- __AlxHwNfcWlcListenerV3_5b_Main_MfTest_G02_I2cCrn120_T03_ReadRegOperation(me)__
	- ![AlxHwNfcWlcListenerV3_5b_Main_MfTest_G02_I2cCrn120_T03_ReadRegOperation_01](Img/AlxHwNfcWlcListenerV3_5b_Main_MfTest_G02_I2cCrn120_T03_ReadRegOperation_01.jpg)
- __AlxHwNfcWlcListenerV3_5b_Main_MfTest_G02_I2cCrn120_T04_ReadOperation(me)__
	- ![AlxHwNfcWlcListenerV3_5b_Main_MfTest_G02_I2cCrn120_T04_ReadOperation_01](Img/AlxHwNfcWlcListenerV3_5b_Main_MfTest_G02_I2cCrn120_T04_ReadOperation_01.jpg)
- __AlxHwNfcWlcListenerV3_5b_Main_MfTest_G02_I2cCrn120_T05_WriteRegOperation(me)__
	- ![AlxHwNfcWlcListenerV3_5b_Main_MfTest_G02_I2cCrn120_T05_WriteRegOperation_01](Img/AlxHwNfcWlcListenerV3_5b_Main_MfTest_G02_I2cCrn120_T05_WriteRegOperation_01.jpg)
- __AlxHwNfcWlcListenerV3_5b_Main_MfTest_G02_I2cCrn120_T06_WriteOperation(me)__
	- ![AlxHwNfcWlcListenerV3_5b_Main_MfTest_G02_I2cCrn120_T06_WriteOperation_01](Img/AlxHwNfcWlcListenerV3_5b_Main_MfTest_G02_I2cCrn120_T06_WriteOperation_01.jpg)