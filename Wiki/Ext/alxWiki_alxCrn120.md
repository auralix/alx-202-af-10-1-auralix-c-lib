# Auralix C Library - ALX <description_TODO> CRN120 Module
---
## Supply of Crn120
- PCA9431 must be supplied with 3,3V on pin VCC
	- 2 options to achieve that:  
		- **a)** from MCU on HwNfcWlc**Listener** V3_5b or V3_7
			* Resistor R110 and R223 **must be placed**
				- image [ListenerCrn120SupplyResistorOff](..\Img\HwCrn120\ListenerCrn120SupplyResistorOff.jpg)
			* pin 8 -- PIO0_11/WKTCLKIN on MCU LPC804M101JHI33E must be high
		- **b)**  from CRN_VCC on HwNfcWlc**Listener** V3_5b or	V3_7
			* Resistor R110 or R223 must **not** be placed
				- image [ListenerCrn120SupplyResistorOff](..\Img\HwCrn120\ListenerCrn120SupplyResistorOff.jpg)
			* connect 3.3V to  J202_pin 6 -- CRN_VCC on HwNfcWlc**Listener** V3_5b or V3_7


