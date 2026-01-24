/**
  ******************************************************************************
  * @file		alxMax17263.c
  * @brief		Auralix C Library - ALX Battery charger
  * @copyright	Copyright (C) Auralix d.o.o. All rights reserved.
  *
  * @section License
  *
  * SPDX-License-Identifier: GPL-3.0-or-later
  *
  * This file is part of Auralix C Library.
  *
  * Auralix C Library is free software: you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation, either version 3
  * of the License, or (at your option) any later version.
  *
  * Auralix C Library is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  * GNU General Public License for more details.
  *
  * You should have received a copy of the GNU General Public License
  * along with Auralix C Library. If not, see <https://www.gnu.org/licenses/>.
  ******************************************************************************
  **/

//******************************************************************************
// Includes
//******************************************************************************
#include "alxMax17263.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB)

/**** Globals ****/
uint16_t max1726x_regs[256];
uint16_t max1726x_serialnum[8];
max1726x_ez_config_t max1726x_ez_config;
max1726x_short_ini_t max1726x_short_ini;
max1726x_full_ini_t max1726x_full_ini;
max1726x_learned_parameters_t max1726x_learned_parameters;

//******************************************************************************
// Constructor
//******************************************************************************

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		i2c
  * @param[in]		IINLIM Input current limit
  * @param[in]		JEITA_VSET High Temperature Voltage Setting
  * @param[in]		BATFET_DIS Enable ship mode
  */
void AlxMax17263_Ctor
(
	AlxMax17263* me,
	AlxI2c* i2c
)
{

	// Parameters
	me->i2c = i2c;
	me->i2cAddr = MAX1726X_I2C_ADDR;
	me->i2cCheckWithRead = true;
	me->i2cNumOfTries = 3;
	me->i2cTimeout_ms = 500;

	me->wasCtorCalled = true;
	me->isInitPeriph = false;
	me->isInit = false;
}


//******************************************************************************
// Functions
//******************************************************************************

/**
  * @brief
  * @param[in,out]	me
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxMax17263_InitPeriph(AlxMax17263* me)
{
	// Assert
	ALX_MAX17263_ASSERT(me->wasCtorCalled == true);
	ALX_MAX17263_ASSERT(me->isInitPeriph == false);
	// isInit -> Don't care

	// Local variables
	Alx_Status status = Alx_Err;

	// Init I2C
	status = AlxI2c_Init(me->i2c);
	if (status != Alx_Ok)
	{
		ALX_MAX17263_TRACE_ERR("Err");
		return status;
	}

	// Set isInitPeriph
	me->isInitPeriph = true;

	// Return
	return Alx_Ok;
}

/**
  * @brief
  * @param[in,out]	me
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxMax17263_DeInitPeriph(AlxMax17263* me)
{
	// Assert
	ALX_MAX17263_ASSERT(me->wasCtorCalled == true);
	ALX_MAX17263_ASSERT(me->isInitPeriph == true);
	// isInit -> Don't care

	// Local variables
	Alx_Status status = Alx_Err;

	// DeInit I2C
	status = AlxI2c_DeInit(me->i2c);
	if (status != Alx_Ok)
	{
		ALX_MAX17263_TRACE_ERR("Err");
		return status;
	}

	// Reset isInitPeriph
	me->isInitPeriph = false;

	// Return
	return Alx_Ok;
}

/**
  * @brief
  * @param[in,out]	me
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxMax17263_Init(AlxMax17263* me)
{
	// Assert
	ALX_MAX17263_ASSERT(me->wasCtorCalled == true);
	ALX_MAX17263_ASSERT(me->isInit == false);

	// Local variables
	Alx_Status status = Alx_Err;

	// Multiple drivers use the same I2C instance, so it could already be initialized.
	if (me->i2c->isInit == false)
	{
		AlxMax17263_InitPeriph(me);
	}
	else
	{
		me->isInitPeriph = true;
	}

	// Check if slave is ready
	status = AlxI2c_Master_IsSlaveReady(me->i2c, me->i2cAddr, me->i2cNumOfTries, me->i2cTimeout_ms);
	if (status != Alx_Ok)
	{
		ALX_MAX17263_TRACE_ERR("Err");
		return status;
	}

	// Get ICs serial number
	uint16_t sn[8] = { 0 };
	status = maxim_max1726x_get_serial_number(me, sn);
	if (status != Alx_Ok)
	{
		ALX_MAX17263_TRACE_ERR("Err");
		return status;
	}

	snprintf(me->data.serial, sizeof(me->data.serial), "%04x%04x%04x%04x%04x%04x%04x%04x", sn[7], sn[6], sn[5], sn[4], sn[3], sn[2], sn[1], sn[0]);

	// Set isInit
	me->isInit = true;
	// Return
	return Alx_Ok;
}

/**
  * @brief
  * @param[in,out]	me
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxMax17263_DeInit(AlxMax17263* me)
{
	// Assert
	ALX_MAX17263_ASSERT(me->wasCtorCalled == true);
	ALX_MAX17263_ASSERT(me->isInitPeriph == true);
	ALX_MAX17263_ASSERT(me->isInit == true);

	// Reset isInit
	me->isInit = false;

	// Return
	return Alx_Ok;
}

/**
  * @brief Poll MAX fuel gauge and handle changes
  * @param[in,out]	me
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxMax17263_Handle(AlxMax17263* me, max1726_data_t *data)
{
	if (1 == maxim_max1726x_check_por(me))
	{
		me->data.reset_happened = true;

		if(Alx_Ok != maxim_max1726x_wait_dnr(me)) return Alx_Err;
		if(Alx_Ok != maxim_max1726x_initialize_ez_config(me)) return Alx_Err;
		//if (Alx_Ok != maxim_max1726x_restore_learned_parameters(me, &me->data.learned_params)) return Alx_Err;
		if (Alx_Ok != maxim_max1726x_config_led(me)) return Alx_Err;
		if (Alx_Ok != maxim_max1726x_clear_por(me)) return Alx_Err;

	}
	else
	{
		me->data.reset_happened = false;
		//uint16_t old_cycles_bit2 = ((uint16_t)me->data.Cycles) & 0x4 ;

		if(Alx_Ok != maxim_max1726x_get_data(me, MAX1726X_REPSOC_REG, &data->RepSOC)) return Alx_Err;
		if(Alx_Ok != maxim_max1726x_get_data(me, MAX1726X_REPCAP_REG, &data->RepCAP)) return Alx_Err;
		if(Alx_Ok != maxim_max1726x_get_data(me, MAX1726X_TTE_REG, &data->TTE)) return Alx_Err;
		if(Alx_Ok != maxim_max1726x_get_data(me, MAX1726X_TTF_REG, &data->TTF)) return Alx_Err;
		if(Alx_Ok != maxim_max1726x_get_data(me, MAX1726X_FULLCAP_REG, &data->FulLCAP)) return Alx_Err;
		if(Alx_Ok != maxim_max1726x_get_data(me, MAX1726X_CYCLES_REG, &data->Cycles)) return Alx_Err;
		if(Alx_Ok != maxim_max1726x_get_data(me, MAX1726X_AVGCURRENT_REG, &data->AvgCurrent)) return Alx_Err;
		if(Alx_Ok != maxim_max1726x_get_data(me, MAX1726X_AVGTA_REG, &data->AvgTA)) return Alx_Err;
		if(Alx_Ok != maxim_max1726x_get_data(me, MAX1726X_AVGVCELL_REG, &data->AvgVCell)) return Alx_Err;
		if (Alx_Ok != maxim_max1726x_get_data(me, MAX1726X_FULLCAPNOM_REG, &data->FullCapNom)) return Alx_Err;
		if (Alx_Ok != maxim_max1726x_get_data(me, MAX1726X_FULLCAPREP_REG, &data->FullCapRep)) return Alx_Err;
		if (Alx_Ok != maxim_max1726x_get_data(me, MAX1726X_RCOMP0_REG, &data->RComp0)) return Alx_Err;
		if (Alx_Ok != maxim_max1726x_get_data(me, MAX1726X_TEMPCO_REG, &data->TempC0)) return Alx_Err;

		/* Not used because if battery is taken out, nemo reboots and loses learned parameters */

		// Handle learned parameters every two calls of this function
		// Or as per software implementation guide, every time bit 2 of Cycles register changes
		//static uint8_t count = 0;
		//count++;
		//if (count == 2 || ((uint16_t)me->data.Cycles & 0x4) != old_cycles_bit2)
		//{
		//	count = 0;
		//	if(Alx_Ok != maxim_max1726x_save_learned_parameters(me, &me->data.learned_params)) return Alx_Err;
		//}
		// Goto step 3.2
	}

	return Alx_Ok;
}

/**** Functions ****/

/* ************************************************************************* */
Alx_Status maxim_max1726x_write_reg(AlxMax17263* me, uint8_t reg_addr, uint16_t *reg_data)
{
	uint8_t i2c_data[3];

	i2c_data[0] = reg_addr;
	i2c_data[1] = (*reg_data) & 0xFF;
	i2c_data[2] = (*reg_data) >> 8;

	return AlxI2c_Master_StartWriteStop(me->i2c, me->i2cAddr, i2c_data, 3, me->i2cNumOfTries, me->i2cTimeout_ms);
}

/* ************************************************************************* */
Alx_Status maxim_max1726x_read_reg(AlxMax17263* me, uint8_t reg_addr, uint16_t *reg_data)
{
	Alx_Status ret;
	uint8_t i2c_data[2];

	// Example https://github.com/analogdevicesinc/MAXREFDES1260/blob/4bd0c99d12e87625e8813a0806b3705cad43d62b/firmware/Source/max1726x.c#L72
	// requires repeated start. I2C MEM operations implement it so use this instead of normal write.
	ret = AlxI2c_Master_StartReadMemStop(me->i2c,
									me->i2cAddr,
									reg_addr,
									AlxI2c_Master_MemAddrLen_8bit,
									i2c_data,
									2,
									me->i2cNumOfTries,
									me->i2cTimeout_ms);

	*reg_data = i2c_data[1];
	*reg_data = ((*reg_data) << 8) | i2c_data[0];
	return ret;
}

/* ************************************************************************* */
Alx_Status maxim_max1726x_write_and_verify_reg(AlxMax17263* me, uint8_t reg_addr, uint16_t *reg_data)
{
	uint8_t i2c_data[3];
	uint16_t readback_data;
	int8_t retry;

	retry = 3;

	while (retry > 0)
	{
		i2c_data[0] = reg_addr;
		i2c_data[1] = (*reg_data) & 0xFF;
		i2c_data[2] = (*reg_data) >> 8;
		AlxI2c_Master_StartWriteStop(me->i2c, me->i2cAddr, i2c_data, 3, me->i2cNumOfTries, me->i2cTimeout_ms);

		AlxOsDelay_ms(&alxOsDelay, 10); // about 10ms

		i2c_data[0] = 0x00;
		i2c_data[1] = 0x00;
		i2c_data[2] = 0x00;
		AlxI2c_Master_StartReadMemStop(me->i2c,
										me->i2cAddr,
										reg_addr,
										AlxI2c_Master_MemAddrLen_8bit,
										i2c_data,
										2,
										me->i2cNumOfTries,
										me->i2cTimeout_ms);
		readback_data = i2c_data[1];
		readback_data = (readback_data << 8) | i2c_data[0];

		if (readback_data == (*reg_data))
		{
			return Alx_Ok; 	// no error
		}
		else
		{
			retry--;
		}
	}

	return Alx_Err;	// error
}

/* ************************************************************************* */
uint8_t maxim_max1726x_check_por(AlxMax17263* me)
{
	maxim_max1726x_read_reg(me, MAX1726X_STATUS_REG, &max1726x_regs[MAX1726X_STATUS_REG]);

	if ((max1726x_regs[MAX1726X_STATUS_REG] & 0x0002) == 0x0000)
	{
		return 0;	// No power on reset
	}
	else
	{
		return 1;	// Power on reset
	}
}

/* ************************************************************************* */
Alx_Status maxim_max1726x_clear_por(AlxMax17263* me)
{
	maxim_max1726x_read_reg(me, MAX1726X_STATUS_REG, &max1726x_regs[MAX1726X_STATUS_REG]);

	max1726x_regs[MAX1726X_STATUS_REG] = max1726x_regs[MAX1726X_STATUS_REG] & 0xFFFD;

	return maxim_max1726x_write_and_verify_reg(me, MAX1726X_STATUS_REG, &max1726x_regs[MAX1726X_STATUS_REG]);
}

/* ************************************************************************* */
Alx_Status maxim_max1726x_wait_dnr(AlxMax17263* me)
{
	Alx_Status ret = maxim_max1726x_read_reg(me, MAX1726X_FSTAT_REG, &max1726x_regs[MAX1726X_FSTAT_REG]);
	if(ret != Alx_Ok) return ret;

	while ((max1726x_regs[MAX1726X_FSTAT_REG] & 0x0001) == 0x0001)
	{
		AlxOsDelay_ms(&alxOsDelay, 10); // about 10ms
		ret = maxim_max1726x_read_reg(me, MAX1726X_FSTAT_REG, &max1726x_regs[MAX1726X_FSTAT_REG]);
		if(ret != Alx_Ok) return ret;
	}

	return ret;
}

/* ************************************************************************* */
Alx_Status maxim_max1726x_config_led(AlxMax17263* me)
{
	Alx_Status ret = Alx_Ok;

	max1726x_regs[MAX1726X_LEDCFG1_REG] = 0x60B5;
	max1726x_regs[MAX1726X_LEDCFG2_REG] = 0x7F8F;
	max1726x_regs[MAX1726X_LEDCFG3_REG] = 0xC000;

	if ((ret = maxim_max1726x_write_reg(me, MAX1726X_LEDCFG1_REG, &max1726x_regs[MAX1726X_LEDCFG1_REG])) != Alx_Ok) return ret;
	if ((ret = maxim_max1726x_write_reg(me, MAX1726X_LEDCFG2_REG, &max1726x_regs[MAX1726X_LEDCFG2_REG])) != Alx_Ok) return ret;
	if ((ret = maxim_max1726x_write_reg(me, MAX1726X_LEDCFG3_REG, &max1726x_regs[MAX1726X_LEDCFG3_REG])) != Alx_Ok) return ret;

	return ret;
}

/* ************************************************************************* */
Alx_Status maxim_max1726x_initialize_ez_config(AlxMax17263* me)
{
	Alx_Status ret = Alx_Ok;
	uint16_t tempdata;

	/// customer must provide the battery parameters accordingly
	/// here the values are default for two serials of 18650 bat

	// Values are copied from config tool Dewesoft is using
	max1726x_ez_config.designcap  = 0x2EE0; // 12000mAh
	max1726x_ez_config.ichgterm   = 0x00A0;
	max1726x_ez_config.modelcfg   = 0x8000;
	max1726x_ez_config.vempty     = 0x9661;

	/// Store original HibCFG value
	if((ret = maxim_max1726x_read_reg(me, MAX1726X_HIBCFG_REG, &max1726x_regs[MAX1726X_HIBCFG_REG])) != Alx_Ok) return ret;

	/// Exit Hibernate Mode step
	tempdata = 0x0090;
	if((ret = maxim_max1726x_write_reg(me, 0x60, &tempdata)) != Alx_Ok) return ret;
	tempdata = 0x0000;
	if((ret = maxim_max1726x_write_reg(me, MAX1726X_HIBCFG_REG, &tempdata)) != Alx_Ok) return ret;
	if((ret = maxim_max1726x_write_reg(me, 0x60, &tempdata)) != Alx_Ok) return ret;

	/// OPTION 1 EZ Config (No INI file is needed)
	max1726x_regs[MAX1726X_DESIGNCAP_REG] = max1726x_ez_config.designcap;
	max1726x_regs[MAX1726X_ICHGTERM_REG] = max1726x_ez_config.ichgterm;
	max1726x_regs[MAX1726X_VEMPTY_REG] = max1726x_ez_config.vempty;
	max1726x_regs[MAX1726X_MODELCFG_REG] = max1726x_ez_config.modelcfg;

	if((ret = maxim_max1726x_write_reg(me, MAX1726X_DESIGNCAP_REG, &max1726x_regs[MAX1726X_DESIGNCAP_REG])) != Alx_Ok) return ret;
	if((ret = maxim_max1726x_write_reg(me, MAX1726X_ICHGTERM_REG, &max1726x_regs[MAX1726X_ICHGTERM_REG])) != Alx_Ok) return ret;
	if((ret = maxim_max1726x_write_reg(me, MAX1726X_VEMPTY_REG, &max1726x_regs[MAX1726X_VEMPTY_REG])) != Alx_Ok) return ret;
	if((ret = maxim_max1726x_write_reg(me, MAX1726X_MODELCFG_REG, &max1726x_regs[MAX1726X_MODELCFG_REG])) != Alx_Ok) return ret;


	//Poll ModelCFG.Refresh bit, do not continue until ModelCFG.Refresh==0
	if((ret = maxim_max1726x_read_reg(me, MAX1726X_MODELCFG_REG, &max1726x_regs[MAX1726X_MODELCFG_REG])) != Alx_Ok) return ret;

	while ((max1726x_regs[MAX1726X_MODELCFG_REG] & 0x8000) == 0x8000)
	{
		AlxOsDelay_ms(&alxOsDelay, 10); // about 10ms
		if((ret = maxim_max1726x_read_reg(me, MAX1726X_MODELCFG_REG, &max1726x_regs[MAX1726X_MODELCFG_REG])) != Alx_Ok) return ret;
	}

	/// Restore Original HibCFG value
	if((ret = maxim_max1726x_write_reg(me, MAX1726X_HIBCFG_REG, &max1726x_regs[MAX1726X_HIBCFG_REG])) != Alx_Ok) return ret;

}

/* ************************************************************************* */
void maxim_max1726x_initialize_short_ini(AlxMax17263* me)
{
	uint16_t tempdata;

	/// customer must provide the battery parameters, get the parameters from MAXIM INI file
	max1726x_short_ini.designcap  = 0x2EE0; // 10000mAh
	max1726x_short_ini.ichgterm   = 0x00A0;
	max1726x_short_ini.modelcfg   = 0x0000;
	max1726x_short_ini.learncfg   = 0x0000; // Optional
	max1726x_short_ini.fullsocthr = 0x0000; // Optional
	max1726x_short_ini.qrtable00  = 0x1050;
	max1726x_short_ini.qrtable10  = 0x0014;
	max1726x_short_ini.qrtable20  = 0x0000; // Optional
	max1726x_short_ini.qrtable30  = 0x0000; // Optional
	max1726x_short_ini.vempty     = 0x965A;
	max1726x_short_ini.rcomp0     = 0x0070;
	max1726x_short_ini.tempco     = 0x223E;
	/// customer must provide the battery parameters, get the parameters from MAXIM INI file


	/// Store original HibCFG value
	maxim_max1726x_read_reg(me, MAX1726X_HIBCFG_REG, &max1726x_regs[MAX1726X_HIBCFG_REG]);

	/// Exit Hibernate Mode step
	tempdata = 0x0090;
	maxim_max1726x_write_reg(me, 0x60, &tempdata);
	tempdata = 0x0000;
	maxim_max1726x_write_reg(me, MAX1726X_HIBCFG_REG, &tempdata);
	maxim_max1726x_write_reg(me, 0x60, &tempdata);

	/// OPTION 2 Custom Short INI without OCV Table
	max1726x_regs[MAX1726X_DESIGNCAP_REG] = max1726x_short_ini.designcap;
	max1726x_regs[MAX1726X_ICHGTERM_REG] = max1726x_short_ini.ichgterm;
	max1726x_regs[MAX1726X_VEMPTY_REG] = max1726x_short_ini.vempty;
	max1726x_regs[MAX1726X_MODELCFG_REG] = max1726x_short_ini.modelcfg;
	max1726x_regs[MAX1726X_LEARNCFG_REG] = max1726x_short_ini.learncfg;
	max1726x_regs[MAX1726X_FULLSOCTHR_REG] = max1726x_short_ini.fullsocthr;
	max1726x_regs[MAX1726X_QRTABLE00_REG] = max1726x_short_ini.qrtable00;
	max1726x_regs[MAX1726X_QRTABLE10_REG] = max1726x_short_ini.qrtable10;
	max1726x_regs[MAX1726X_QRTABLE20_REG] = max1726x_short_ini.qrtable20;
	max1726x_regs[MAX1726X_QRTABLE30_REG] = max1726x_short_ini.qrtable30;
	max1726x_regs[MAX1726X_RCOMP0_REG] = max1726x_short_ini.rcomp0;
	max1726x_regs[MAX1726X_TEMPCO_REG] = max1726x_short_ini.tempco;

	maxim_max1726x_write_reg(me, MAX1726X_DESIGNCAP_REG, &max1726x_regs[MAX1726X_DESIGNCAP_REG]);
	maxim_max1726x_write_reg(me, MAX1726X_ICHGTERM_REG, &max1726x_regs[MAX1726X_ICHGTERM_REG]);
	maxim_max1726x_write_reg(me, MAX1726X_VEMPTY_REG, &max1726x_regs[MAX1726X_VEMPTY_REG]);
	// maxim_max1726x_write_and_verify_reg(me, MAX1726X_LEARNCFG_REG, &max1726x_regs[MAX1726X_LEARNCFG_REG]);	// optional
	// maxim_max1726x_write_and_verify_reg(me, MAX1726X_FULLSOCTHR_REG, &max1726x_regs[MAX1726X_FULLSOCTHR_REG]);	// optional
	maxim_max1726x_write_reg(me, MAX1726X_MODELCFG_REG, &max1726x_regs[MAX1726X_MODELCFG_REG]);


	//Poll ModelCFG.Refresh bit, do not continue until ModelCFG.Refresh==0
	maxim_max1726x_read_reg(me, MAX1726X_MODELCFG_REG, &max1726x_regs[MAX1726X_MODELCFG_REG]);

	while ((max1726x_regs[MAX1726X_MODELCFG_REG] & 0x8000) == 0x8000)
	{
		AlxOsDelay_ms(&alxOsDelay, 10); // about 10ms
		maxim_max1726x_read_reg(me, MAX1726X_MODELCFG_REG, &max1726x_regs[MAX1726X_MODELCFG_REG]);
	}

	maxim_max1726x_write_reg(me, MAX1726X_RCOMP0_REG, &max1726x_regs[MAX1726X_RCOMP0_REG]);
	maxim_max1726x_write_reg(me, MAX1726X_TEMPCO_REG, &max1726x_regs[MAX1726X_TEMPCO_REG]);
	maxim_max1726x_write_reg(me, MAX1726X_QRTABLE00_REG, &max1726x_regs[MAX1726X_QRTABLE00_REG]);
	maxim_max1726x_write_reg(me, MAX1726X_QRTABLE10_REG, &max1726x_regs[MAX1726X_QRTABLE10_REG]);
	// maxim_max1726x_write_reg(MAX1726X_QRTABLE20_REG, &max1726x_regs[MAX1726X_QRTABLE20_REG]);	// optional
	// maxim_max1726x_write_reg(MAX1726X_QRTABLE30_REG, &max1726x_regs[MAX1726X_QRTABLE30_REG]);	// optional

	/// Restore Original HibCFG value
	maxim_max1726x_write_reg(me, MAX1726X_HIBCFG_REG, &max1726x_regs[MAX1726X_HIBCFG_REG]);

}

/* ************************************************************************* */
void maxim_max1726x_initialize_full_ini(AlxMax17263* me)
{
	uint16_t tempdata;
	uint8_t ret;


	/// customer must provide the battery parameters, get the parameters from MAXIM INI file
	max1726x_full_ini.designcap  = 0x06aE;
	max1726x_full_ini.fullcaprep = 0x06aE;
	max1726x_full_ini.ichgterm   = 0x0100;
	max1726x_full_ini.modelcfg   = 0x8410;
	max1726x_full_ini.qrtable00  = 0x1050;
	max1726x_full_ini.qrtable10  = 0x0014;
	max1726x_full_ini.qrtable20  = 0x1300; // optional
	max1726x_full_ini.qrtable30  = 0x0C00; // optional
	max1726x_full_ini.vempty     = 0x965A;
	max1726x_full_ini.rcomp0     = 0x0070;
	max1726x_full_ini.tempco     = 0x223E;
	max1726x_full_ini.learncfg   = 0x0000; // optional
	max1726x_full_ini.relaxcfg   = 0x0000; // optional
	max1726x_full_ini.config     = 0x0000; // optional
	max1726x_full_ini.config2    = 0x0000; // optional
	max1726x_full_ini.fullsocthr = 0x5F05; // optional
	max1726x_full_ini.tgain      = 0x0000; // optional
	max1726x_full_ini.toff       = 0x0000; // optional
	max1726x_full_ini.curve      = 0x0000; // optional



	max1726x_full_ini.modeldata0[0]  = 0x0000;
	max1726x_full_ini.modeldata0[1]  = 0x0000;
	max1726x_full_ini.modeldata0[2]  = 0x0000;
	max1726x_full_ini.modeldata0[3]  = 0x0000;
	max1726x_full_ini.modeldata0[4]  = 0x0000;
	max1726x_full_ini.modeldata0[5]  = 0x0000;
	max1726x_full_ini.modeldata0[6]  = 0x0000;
	max1726x_full_ini.modeldata0[7]  = 0x0000;
	max1726x_full_ini.modeldata0[8]  = 0x0000;
	max1726x_full_ini.modeldata0[9]  = 0x0000;
	max1726x_full_ini.modeldata0[10] = 0x0000;
	max1726x_full_ini.modeldata0[11] = 0x0000;
	max1726x_full_ini.modeldata0[12] = 0x0000;
	max1726x_full_ini.modeldata0[13] = 0x0000;
	max1726x_full_ini.modeldata0[14] = 0x0000;
	max1726x_full_ini.modeldata0[15] = 0x0000;

	max1726x_full_ini.modeldata1[0]  = 0x0000;
	max1726x_full_ini.modeldata1[1]  = 0x0000;
	max1726x_full_ini.modeldata1[2]  = 0x0000;
	max1726x_full_ini.modeldata1[3]  = 0x0000;
	max1726x_full_ini.modeldata1[4]  = 0x0000;
	max1726x_full_ini.modeldata1[5]  = 0x0000;
	max1726x_full_ini.modeldata1[6]  = 0x0000;
	max1726x_full_ini.modeldata1[7]  = 0x0000;
	max1726x_full_ini.modeldata1[8]  = 0x0000;
	max1726x_full_ini.modeldata1[9]  = 0x0000;
	max1726x_full_ini.modeldata1[10] = 0x0000;
	max1726x_full_ini.modeldata1[11] = 0x0000;
	max1726x_full_ini.modeldata1[12] = 0x0000;
	max1726x_full_ini.modeldata1[13] = 0x0000;
	max1726x_full_ini.modeldata1[14] = 0x0000;
	max1726x_full_ini.modeldata1[15] = 0x0000;
	/// customer must provide the battery parameters, get the parameters from MAXIM INI file



	/// Store original HibCFG value
	maxim_max1726x_read_reg(me, MAX1726X_HIBCFG_REG, &max1726x_regs[MAX1726X_HIBCFG_REG]);

	/// Exit Hibernate Mode step
	tempdata = 0x0090;
	maxim_max1726x_write_reg(me, 0x60, &tempdata);
	tempdata = 0x0000;
	maxim_max1726x_write_reg(me, MAX1726X_HIBCFG_REG, &tempdata);
	maxim_max1726x_write_reg(me, 0x60, &tempdata);

	/// OPTION 3 Custom Full INI with OCV Table
	max1726x_regs[MAX1726X_DESIGNCAP_REG] = max1726x_full_ini.designcap;
	max1726x_regs[MAX1726X_FULLCAPREP_REG] = max1726x_full_ini.fullcaprep;
	max1726x_regs[MAX1726X_ICHGTERM_REG] = max1726x_full_ini.ichgterm;
	max1726x_regs[MAX1726X_VEMPTY_REG] = max1726x_full_ini.vempty;
	max1726x_regs[MAX1726X_MODELCFG_REG] = max1726x_full_ini.modelcfg;
	max1726x_regs[MAX1726X_QRTABLE00_REG] = max1726x_full_ini.qrtable00;
	max1726x_regs[MAX1726X_QRTABLE10_REG] = max1726x_full_ini.qrtable10;
	max1726x_regs[MAX1726X_QRTABLE20_REG] = max1726x_full_ini.qrtable20; // optional
	max1726x_regs[MAX1726X_QRTABLE30_REG] = max1726x_full_ini.qrtable30; // optional
	max1726x_regs[MAX1726X_RCOMP0_REG] = max1726x_full_ini.rcomp0;
	max1726x_regs[MAX1726X_TEMPCO_REG] = max1726x_full_ini.tempco;
	max1726x_regs[MAX1726X_LEARNCFG_REG] = max1726x_full_ini.learncfg; // optional
	max1726x_regs[MAX1726X_RELAXCFG_REG] = max1726x_full_ini.relaxcfg; // optional
	max1726x_regs[MAX1726X_CONFIG_REG] = max1726x_full_ini.config; // optional
	max1726x_regs[MAX1726X_CONFIG2_REG] = max1726x_full_ini.config2; // optional
	max1726x_regs[MAX1726X_FULLSOCTHR_REG] = max1726x_full_ini.fullsocthr; // optional
	max1726x_regs[MAX1726X_TGAIN_REG] = max1726x_full_ini.tgain; // optional
	max1726x_regs[MAX1726X_TOFF_REG] = max1726x_full_ini.toff; // optional
	max1726x_regs[MAX1726X_CURVE_REG] = max1726x_full_ini.curve; // optional



	/// Write/Read/Verify the Custom Model Data
	ret = 1;
	while (ret != 0)
	{
		/// Unlock Model Access
		maxim_max1726x_unlock_model_data(me);

		AlxOsDelay_ms(&alxOsDelay, 10); // about 10ms

		/// Write/Read/Verify the Custom Model Data
		ret = maxim_max1726x_write_model_data(me, max1726x_full_ini.modeldata0, max1726x_full_ini.modeldata1);
	}

	/// Lock Model Access
	ret = 1;
	while (ret != 0)
	{
		/// Lock Model Access
		maxim_max1726x_lock_model_data(me);

		AlxOsDelay_ms(&alxOsDelay, 10); // about 10ms

		ret = maxim_max1726x_verify_model_data_locked(me);
	}

	/// Write Custom Parameters
	max1726x_regs[MAX1726X_REPCAP_REG] = 0x0000;
	maxim_max1726x_write_reg(me, MAX1726X_REPCAP_REG, &max1726x_regs[MAX1726X_REPCAP_REG]);

	maxim_max1726x_write_reg(me, MAX1726X_DESIGNCAP_REG, &max1726x_regs[MAX1726X_DESIGNCAP_REG]);
	maxim_max1726x_write_reg(me, MAX1726X_FULLCAPREP_REG, &max1726x_regs[MAX1726X_FULLCAPREP_REG]);


	max1726x_regs[MAX1726X_DQACC_REG] = max1726x_regs[MAX1726X_DESIGNCAP_REG] / 2;
	max1726x_regs[MAX1726X_DPACC_REG] = 0x0C80;
	maxim_max1726x_write_reg(me, MAX1726X_DQACC_REG, &max1726x_regs[MAX1726X_DQACC_REG]);
	maxim_max1726x_write_reg(me, MAX1726X_DPACC_REG, &max1726x_regs[MAX1726X_DPACC_REG]);

	maxim_max1726x_write_reg(me, MAX1726X_ICHGTERM_REG, &max1726x_regs[MAX1726X_ICHGTERM_REG]);
	maxim_max1726x_write_reg(me, MAX1726X_VEMPTY_REG, &max1726x_regs[MAX1726X_VEMPTY_REG]);
	maxim_max1726x_write_reg(me, MAX1726X_RCOMP0_REG, &max1726x_regs[MAX1726X_RCOMP0_REG]);
	maxim_max1726x_write_reg(me, MAX1726X_TEMPCO_REG, &max1726x_regs[MAX1726X_TEMPCO_REG]);
	maxim_max1726x_write_reg(me, MAX1726X_QRTABLE00_REG, &max1726x_regs[MAX1726X_QRTABLE00_REG]);
	maxim_max1726x_write_reg(me, MAX1726X_QRTABLE10_REG, &max1726x_regs[MAX1726X_QRTABLE10_REG]);

	// maxim_max1726x_write_reg(me, MAX1726X_QRTABLE20_REG, &max1726x_regs[MAX1726X_QRTABLE20_REG]);			// optional
	// maxim_max1726x_write_reg(me, MAX1726X_QRTABLE30_REG, &max1726x_regs[MAX1726X_QRTABLE30_REG]);			// optional
	// maxim_max1726x_write_and_verify_reg(me, MAX1726X_LEARNCFG_REG, &max1726x_regs[MAX1726X_LEARNCFG_REG]);	// optional
	// maxim_max1726x_write_reg(me, MAX1726X_RELAXCFG_REG, &max1726x_regs[MAX1726X_RELAXCFG_REG]);				// optional
	// maxim_max1726x_write_reg(me, MAX1726X_CONFIG_REG, &max1726x_regs[MAX1726X_CONFIG_REG]);					// optional
	// maxim_max1726x_write_reg(me, MAX1726X_CONFIG2_REG, &max1726x_regs[MAX1726X_CONFIG2_REG]);				// optional
	// maxim_max1726x_write_reg(me, MAX1726X_FULLSOCTHR_REG, &max1726x_regs[MAX1726X_FULLSOCTHR_REG]);			// optional
	// maxim_max1726x_write_reg(me, MAX1726X_TGAIN_REG, &max1726x_regs[MAX1726X_TGAIN_REG]);					// optional
	// maxim_max1726x_write_reg(me, MAX1726X_TOFF_REG, &max1726x_regs[MAX1726X_TOFF_REG]);						// optional
	// maxim_max1726x_write_reg(me, MAX1726X_CURVE_REG, &max1726x_regs[MAX1726X_CURVE_REG]);					// optional



	/// Initiate Model Loading, by setting the LdMdl bit in the Config2 register
	maxim_max1726x_read_reg(me, MAX1726X_CONFIG2_REG, &max1726x_regs[MAX1726X_CONFIG2_REG]);
	max1726x_regs[MAX1726X_CONFIG2_REG] = max1726x_regs[MAX1726X_CONFIG2_REG] | 0x0020;
	maxim_max1726x_write_reg(me, MAX1726X_CONFIG2_REG, &max1726x_regs[MAX1726X_CONFIG2_REG]);


	/// Poll Config2.LdMdl bit, do not continue until Config2.LdMdl==0
	maxim_max1726x_read_reg(me, MAX1726X_CONFIG2_REG, &max1726x_regs[MAX1726X_CONFIG2_REG]);

	while ((max1726x_regs[MAX1726X_CONFIG2_REG] & 0x0020) == 0x0020)
	{
		AlxOsDelay_ms(&alxOsDelay, 10); // about 10ms
		maxim_max1726x_read_reg(me, MAX1726X_CONFIG2_REG, &max1726x_regs[MAX1726X_CONFIG2_REG]);
	}

	/// Update QRTable20 and QRTable30
	maxim_max1726x_write_and_verify_reg(me, MAX1726X_QRTABLE20_REG, &max1726x_regs[MAX1726X_QRTABLE20_REG]);
	maxim_max1726x_write_and_verify_reg(me, MAX1726X_QRTABLE30_REG, &max1726x_regs[MAX1726X_QRTABLE30_REG]);


	/// Restore Original HibCFG value
	maxim_max1726x_write_reg(me, MAX1726X_HIBCFG_REG, &max1726x_regs[MAX1726X_HIBCFG_REG]);


}

/* ************************************************************************* */
Alx_Status maxim_max1726x_get_data(AlxMax17263* me, uint8_t reg, float *value)
{
	float temp = 0;
	Alx_Status ret = maxim_max1726x_read_reg(me, reg, &max1726x_regs[reg]);
	if(ret != Alx_Ok) return ret;

	temp = (float)max1726x_regs[reg];

	// Some registers require additional work
	switch(reg)
	{
		case MAX1726X_REPCAP_REG:
			temp *= (0.000005f / RSENSE);
			break;
		case MAX1726X_FULLCAP_REG:
			temp *= (0.000005f / RSENSE);
			break;
		case MAX1726X_REPSOC_REG:
			temp /= 256.0f;
			break;
		case MAX1726X_TTE_REG:
			temp *= 5.625f;
			temp = temp / 60 / 60;
			break;
		case MAX1726X_TTF_REG:
			temp *= 5.625f;
			temp = temp / 60 / 60;
			break;
		case MAX1726X_AVGCURRENT_REG:
		{
			int16_t twos = (int16_t)max1726x_regs[reg];
			temp = (float)(((float)twos / RSENSE) * 1.5625) / 1000000;
			break;
		}
		case MAX1726X_AVGTA_REG:
			temp /= 256.0f;
			break;
		case MAX1726X_AVGVCELL_REG:
			temp *= 0.000078125f;
			break;

		case MAX1726X_TEMPCO_REG:
			temp /= 256.0f;
			break;
		case MAX1726X_RCOMP0_REG:
			temp *= (1.0f / 4096.0f);
			break;
		case MAX1726X_FULLCAPREP_REG:
			temp *= (0.000005f / RSENSE);
			break;
		case MAX1726X_FULLCAPNOM_REG:
			temp *= (0.000005f / RSENSE);
			break;
		default:
			break;
	}

	*value = temp;

	return ret;
}

/* ************************************************************************* */
Alx_Status maxim_max1726x_get_repcap(AlxMax17263* me, float Rsense, float *value)
{
	Alx_Status ret = maxim_max1726x_read_reg(me, MAX1726X_REPCAP_REG, &max1726x_regs[MAX1726X_REPCAP_REG]);
	if(ret != Alx_Ok) return ret;

	*value = (float)max1726x_regs[MAX1726X_REPCAP_REG] * 5.0f / (float)Rsense;
	return ret;
}

/* ************************************************************************* */
Alx_Status maxim_max1726x_get_repsoc(AlxMax17263* me, float *value)
{
	Alx_Status ret = maxim_max1726x_read_reg(me, MAX1726X_REPSOC_REG, &max1726x_regs[MAX1726X_REPSOC_REG]);
	if(ret != Alx_Ok) return ret;

	*value = (float)max1726x_regs[MAX1726X_REPSOC_REG] / 256.0f;
	return ret;
}

/* ************************************************************************* */
Alx_Status maxim_max1726x_get_fullcaprep(AlxMax17263* me, float *value)
{
	float fullcaprep;
	Alx_Status ret = maxim_max1726x_read_reg(me, MAX1726X_FULLCAPREP_REG, &max1726x_regs[MAX1726X_FULLCAPREP_REG]);
	if(ret != Alx_Ok) return ret;

	*value = (float)max1726x_regs[MAX1726X_FULLCAPREP_REG];
	return ret;
}

/* ************************************************************************* */
Alx_Status maxim_max1726x_get_tte(AlxMax17263* me, float *value)
{
	float tte;
	Alx_Status ret = maxim_max1726x_read_reg(me, MAX1726X_TTE_REG, &max1726x_regs[MAX1726X_TTE_REG]);
	if(ret != Alx_Ok) return ret;

	*value = (float)max1726x_regs[MAX1726X_TTE_REG] * 5.625f;
	return ret;
}

/* ************************************************************************* */
Alx_Status maxim_max1726x_get_cycles(AlxMax17263* me, float *value)
{
	float cycles;
	Alx_Status ret = maxim_max1726x_read_reg(me, MAX1726X_CYCLES_REG, &max1726x_regs[MAX1726X_CYCLES_REG]);
	if(ret != Alx_Ok) return ret;

	*value = (float)max1726x_regs[MAX1726X_CYCLES_REG];
	return ret;
}

/* ************************************************************************* */
Alx_Status maxim_max1726x_get_avgta(AlxMax17263* me, float *value)
{
	float avgta;
	Alx_Status ret = maxim_max1726x_read_reg(me, MAX1726X_AVGTA_REG, &max1726x_regs[MAX1726X_AVGTA_REG]);
	if(ret != Alx_Ok) return ret;

	*value = (float)max1726x_regs[MAX1726X_AVGTA_REG];
	return ret;
}

/* ************************************************************************* */
Alx_Status maxim_max1726x_get_ttf(AlxMax17263* me, float *value)
{
	float ttf;
	Alx_Status ret = maxim_max1726x_read_reg(me, MAX1726X_TTF_REG, &max1726x_regs[MAX1726X_TTF_REG]);
	if(ret != Alx_Ok) return ret;

	*value = (float)max1726x_regs[MAX1726X_TTF_REG] * 5.625f;
	return ret;
}

/* ************************************************************************* */
Alx_Status maxim_max1726x_get_avgvcell(AlxMax17263* me, float *value)
{
	float ttf;
	Alx_Status ret = maxim_max1726x_read_reg(me, MAX1726X_AVGVCELL_REG, &max1726x_regs[MAX1726X_AVGVCELL_REG]);
	if(ret != Alx_Ok) return ret;

	*value = (float)max1726x_regs[MAX1726X_AVGVCELL_REG];
	return ret;
}

/* ************************************************************************* */
Alx_Status maxim_max1726x_get_avgcurr(AlxMax17263* me, float *value)
{
	float ttf;
	Alx_Status ret = maxim_max1726x_read_reg(me, MAX1726X_AVGCURRENT_REG, &max1726x_regs[MAX1726X_AVGCURRENT_REG]);
	if(ret != Alx_Ok) return ret;

	*value = (float)max1726x_regs[MAX1726X_AVGCURRENT_REG];
	return ret;
}

/* ************************************************************************* */
Alx_Status maxim_max1726x_save_learned_parameters(AlxMax17263* me, max1726x_learned_parameters_t *lp)
{
	Alx_Status ret;

	if((ret = maxim_max1726x_read_reg(me, MAX1726X_RCOMP0_REG, &max1726x_regs[MAX1726X_RCOMP0_REG]) != Alx_Ok)) return ret;
	if((ret = maxim_max1726x_read_reg(me, MAX1726X_TEMPCO_REG, &max1726x_regs[MAX1726X_TEMPCO_REG]) != Alx_Ok)) return ret;
	if((ret = maxim_max1726x_read_reg(me, MAX1726X_FULLCAPREP_REG, &max1726x_regs[MAX1726X_FULLCAPREP_REG]) != Alx_Ok)) return ret;
	if((ret = maxim_max1726x_read_reg(me, MAX1726X_CYCLES_REG, &max1726x_regs[MAX1726X_CYCLES_REG]) != Alx_Ok)) return ret;
	if((ret = maxim_max1726x_read_reg(me, MAX1726X_FULLCAPNOM_REG, &max1726x_regs[MAX1726X_FULLCAPNOM_REG]) != Alx_Ok)) return ret;


	lp->saved_rcomp0 = max1726x_regs[MAX1726X_RCOMP0_REG];
	lp->saved_tempco = max1726x_regs[MAX1726X_TEMPCO_REG];
	lp->saved_fullcaprep = max1726x_regs[MAX1726X_FULLCAPREP_REG];
	lp->saved_cycles = max1726x_regs[MAX1726X_CYCLES_REG];
	lp->saved_fullcapnom = max1726x_regs[MAX1726X_FULLCAPNOM_REG];

	return Alx_Ok;
}

/* ************************************************************************* */
Alx_Status maxim_max1726x_restore_learned_parameters(AlxMax17263* me, max1726x_learned_parameters_t *lp)
{

	Alx_Status ret;

	max1726x_regs[MAX1726X_RCOMP0_REG] = lp->saved_rcomp0;
	max1726x_regs[MAX1726X_TEMPCO_REG] = lp->saved_tempco;
	max1726x_regs[MAX1726X_FULLCAPREP_REG] = lp->saved_fullcaprep;
	max1726x_regs[MAX1726X_CYCLES_REG] = lp->saved_cycles;
	// Write dQacc to 200% of Capacity and dPacc to 200%
	max1726x_regs[MAX1726X_DPACC_REG] = 0x0C80;
	max1726x_regs[MAX1726X_DQACC_REG] = (lp->saved_fullcapnom) / 2;


	if ((ret = maxim_max1726x_write_and_verify_reg(me, MAX1726X_RCOMP0_REG, &max1726x_regs[MAX1726X_RCOMP0_REG]) != Alx_Ok)) return ret;
	if ((ret = maxim_max1726x_write_and_verify_reg(me, MAX1726X_TEMPCO_REG, &max1726x_regs[MAX1726X_TEMPCO_REG]) != Alx_Ok)) return ret;
	if ((ret = maxim_max1726x_write_and_verify_reg(me, MAX1726X_FULLCAPREP_REG, &max1726x_regs[MAX1726X_FULLCAPREP_REG]) != Alx_Ok)) return ret;;
	if ((ret = maxim_max1726x_write_and_verify_reg(me, MAX1726X_DPACC_REG, &max1726x_regs[MAX1726X_DPACC_REG]) != Alx_Ok)) return ret;
	if ((ret = maxim_max1726x_write_and_verify_reg(me, MAX1726X_DQACC_REG, &max1726x_regs[MAX1726X_DQACC_REG]) != Alx_Ok)) return ret;
	if ((ret = maxim_max1726x_write_and_verify_reg(me, MAX1726X_CYCLES_REG, &max1726x_regs[MAX1726X_CYCLES_REG]) != Alx_Ok)) return ret;

	return Alx_Ok;
}

/* ************************************************************************* */
Alx_Status maxim_max1726x_get_serial_number(AlxMax17263* me, uint16_t *sn)
{
	Alx_Status ret;
	if((ret = maxim_max1726x_read_reg(me, MAX1726X_CONFIG2_REG, &max1726x_regs[MAX1726X_CONFIG2_REG])) != Alx_Ok) return ret;

	// clear AtRateEn bit and DPEn bit in Config2 register
	max1726x_regs[MAX1726X_CONFIG2_REG] = max1726x_regs[MAX1726X_CONFIG2_REG] & 0xCFFF;
	if((ret = maxim_max1726x_write_and_verify_reg(me, MAX1726X_CONFIG2_REG, &max1726x_regs[MAX1726X_CONFIG2_REG])) != Alx_Ok) return ret;
	AlxOsDelay_ms(&alxOsDelay, 500);

	if((ret = maxim_max1726x_read_reg(me, MAX1726X_MAXPEAKPOWER_REG, &max1726x_regs[MAX1726X_MAXPEAKPOWER_REG])) != Alx_Ok) return ret;
	if((ret = maxim_max1726x_read_reg(me, MAX1726X_SUSPEAKPOWER_REG, &max1726x_regs[MAX1726X_SUSPEAKPOWER_REG])) != Alx_Ok) return ret;
	if((ret = maxim_max1726x_read_reg(me, MAX1726X_MPPCURRENT_REG, &max1726x_regs[MAX1726X_MPPCURRENT_REG])) != Alx_Ok) return ret;
	if((ret = maxim_max1726x_read_reg(me, MAX1726X_SPPCURRENT_REG, &max1726x_regs[MAX1726X_SPPCURRENT_REG])) != Alx_Ok) return ret;
	if((ret = maxim_max1726x_read_reg(me, MAX1726X_ATQRESIDUAL_REG, &max1726x_regs[MAX1726X_ATQRESIDUAL_REG])) != Alx_Ok) return ret;
	if((ret = maxim_max1726x_read_reg(me, MAX1726X_ATTTE_REG, &max1726x_regs[MAX1726X_ATTTE_REG])) != Alx_Ok) return ret;
	if((ret = maxim_max1726x_read_reg(me, MAX1726X_ATAVSOC_REG, &max1726x_regs[MAX1726X_ATAVSOC_REG])) != Alx_Ok) return ret;
	if((ret = maxim_max1726x_read_reg(me, MAX1726X_ATAVCAP_REG, &max1726x_regs[MAX1726X_ATAVCAP_REG])) != Alx_Ok) return ret;


	sn[0] = max1726x_regs[MAX1726X_MAXPEAKPOWER_REG];
	sn[1] = max1726x_regs[MAX1726X_SUSPEAKPOWER_REG];
	sn[2] = max1726x_regs[MAX1726X_MPPCURRENT_REG];
	sn[3] = max1726x_regs[MAX1726X_SPPCURRENT_REG];
	sn[4] = max1726x_regs[MAX1726X_ATQRESIDUAL_REG];
	sn[5] = max1726x_regs[MAX1726X_ATTTE_REG];
	sn[6] = max1726x_regs[MAX1726X_ATAVSOC_REG];
	sn[7] = max1726x_regs[MAX1726X_ATAVCAP_REG];


	// set AtRateEn bit and DPEn bit in Config2 register
	max1726x_regs[MAX1726X_CONFIG2_REG] = max1726x_regs[MAX1726X_CONFIG2_REG] | 0x3000;
	if((ret = maxim_max1726x_write_and_verify_reg(me, MAX1726X_CONFIG2_REG, &max1726x_regs[MAX1726X_CONFIG2_REG])) != Alx_Ok) return ret;
	AlxOsDelay_ms(&alxOsDelay, 500); // about 40ms

	return Alx_Ok;
}

/* ************************************************************************* */
void maxim_max1726x_unlock_model_data(AlxMax17263* me)
{
	uint16_t tempdata;

	tempdata = 0x0059;
	maxim_max1726x_write_reg(me, 0x62, &tempdata);
	tempdata = 0x00C4;
	maxim_max1726x_write_reg(me, 0x63, &tempdata);
}

/* ************************************************************************* */
uint8_t maxim_max1726x_write_model_data(AlxMax17263* me, uint16_t *data0, uint16_t *data1)
{
	uint8_t err_num;

	uint8_t i;
	uint16_t readback0[16];
	uint16_t readback1[16];

	err_num = 32;

	for (i = 0; i < 16; i++)
	{
		maxim_max1726x_write_reg(me, MAX1726X_MODELDATA0_START_REG + i, &data0[i]);
	}
	for (i = 0; i < 16; i++)
	{
		maxim_max1726x_write_reg(me, MAX1726X_MODELDATA1_START_REG + i, &data1[i]);
	}

	AlxOsDelay_ms(&alxOsDelay, 10); // about 10ms

	for (i = 0; i < 16; i++)
	{
		maxim_max1726x_read_reg(me, MAX1726X_MODELDATA0_START_REG + i, &readback0[i]);
	}
	for (i = 0; i < 16; i++)
	{
		maxim_max1726x_read_reg(me, MAX1726X_MODELDATA1_START_REG + i, &readback1[i]);
	}

	for (i = 0; i < 16; i++)
	{
		if (readback0[i] == data0[i])
		{
			err_num--;
		}
		if (readback1[i] == data1[i])
		{
			err_num--;
		}
	}



	return err_num;
}

/* ************************************************************************* */
void maxim_max1726x_lock_model_data(AlxMax17263* me)
{
	uint16_t tempdata;

	tempdata = 0x0000;
	maxim_max1726x_write_reg(me, 0x62, &tempdata);
	maxim_max1726x_write_reg(me, 0x63, &tempdata);
}

/* ************************************************************************* */
uint8_t maxim_max1726x_verify_model_data_locked(AlxMax17263* me)
{
	uint8_t err_num;
	uint8_t i;
	uint16_t readback0[16];
	uint16_t readback1[16];


	err_num = 32;

	for (i = 0; i < 16; i++)
	{
		maxim_max1726x_read_reg(me, MAX1726X_MODELDATA0_START_REG + i, &readback0[i]);
	}
	for (i = 0; i < 16; i++)
	{
		maxim_max1726x_read_reg(me, MAX1726X_MODELDATA1_START_REG + i, &readback1[i]);
	}

	for (i = 0; i < 16; i++)
	{
		if (readback0[i] == 0x0000)
		{
			err_num--;
		}
		if (readback1[i] == 0x0000)
		{
			err_num--;
		}
	}


	return err_num;
}




#endif	// #if defined(ALX_C_LIB)