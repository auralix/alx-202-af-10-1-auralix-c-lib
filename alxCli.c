/**
  ******************************************************************************
  * @file		alxCli.c
  * @brief		Auralix C Library - ALX CLI Module
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
#include "alxCli.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB)


//******************************************************************************
// Private Functions
//******************************************************************************
static void AlxCli_Get(AlxCli* me, bool paramTypeCheck, AlxParamItem_ParamType paramType);
static void AlxCli_PrepResp_Success(AlxCli* me);
static void AlxCli_PrepResp_ErrCmd(AlxCli* me);
static void AlxCli_PrepResp_ErrArg(AlxCli* me);


//******************************************************************************
// Constructor
//******************************************************************************
void AlxCli_Ctor
(
	AlxCli* me,
	AlxSerialPort* alxSerialPort,
	AlxId* alxId,
	AlxParamMgmt* alxParamMgmt,
	bool prettyJsonResp,
	void* buff,
	uint32_t buffLen
)
{
	// Parameters
	me->alxSerialPort = alxSerialPort;
	me->alxId = alxId;
	me->alxParamMgmt = alxParamMgmt;
	me->prettyJsonResp = prettyJsonResp;
	me->buff = buff;
	me->buffLen = buffLen;

	// Variables

	// Info
	me->wasCtorCalled = true;
}


//******************************************************************************
// Functions
//******************************************************************************
void AlxCli_Handle(AlxCli* me)
{
	//------------------------------------------------------------------------------
	// Assert
	//------------------------------------------------------------------------------
	ALX_CLI_ASSERT(me->wasCtorCalled == true);


	//------------------------------------------------------------------------------
	// Handle Command
	//------------------------------------------------------------------------------
	uint32_t cmdLen = 0;
	if (AlxSerialPort_ReadStrUntil(me->alxSerialPort, me->buff, "\r\n", me->buffLen, &cmdLen) == Alx_Ok)
	{
		//------------------------------------------------------------------------------
		// Append \r\n
		//------------------------------------------------------------------------------
		strcat(me->buff, "\r\n");


		//------------------------------------------------------------------------------
		// Reset Command
		//------------------------------------------------------------------------------
		if (strcmp(me->buff, "reset\r\n") == 0)
		{
			// Prepare response
			AlxCli_PrepResp_Success(me);

			// Send response
			ALX_CLI_ASSERT(AlxSerialPort_WriteStr(me->alxSerialPort, me->buff) == Alx_Ok);

			// Reset MCU
			NVIC_SystemReset();
		}


		//------------------------------------------------------------------------------
		// Get ID Command
		//------------------------------------------------------------------------------
		else if (strcmp(me->buff, "id\r\n") == 0)
		{
			// Get
			bool fwIsBootUsed = AlxId_GetFwIsBootUsed(me->alxId);

			const char* fwArtf = AlxId_GetFwArtf(me->alxId);
			const char* fwName = AlxId_GetFwName(me->alxId);
			const char* fwVerStr = AlxId_GetFwVerStr(me->alxId);
			const char* fwBinStr = AlxId_GetFwBinStr(me->alxId);

			const char* fwBootArtf = AlxId_GetFwBootArtf(me->alxId);
			const char* fwBootName = AlxId_GetFwBootName(me->alxId);
			const char* fwBootVerStr = AlxId_GetFwBootVerStr(me->alxId);
			const char* fwBootBinStr = AlxId_GetFwBootBinStr(me->alxId);

			const char* hwPcbArtf = AlxId_GetHwPcbArtf(me->alxId);
			const char* hwPcbName = AlxId_GetHwPcbName(me->alxId);
			const char* hwPcbVerStr = AlxId_GetHwPcbVerStr(me->alxId);

			const char* hwBomArtf = AlxId_GetHwBomArtf(me->alxId);
			const char* hwBomName = AlxId_GetHwBomName(me->alxId);
			const char* hwBomVerStr = AlxId_GetHwBomVerStr(me->alxId);

			uint8_t hwId = AlxId_GetHwId(me->alxId);
			const char* hwMcuUniqueIdStr = AlxId_GetHwMcuUniqueIdStr(me->alxId);

			// Prepare response
			if (me->prettyJsonResp)
			{
				if (fwIsBootUsed)
				{
					sprintf
					(
						me->buff,
						"{\r\n"
						"    \"status\":\"success\",\r\n"
						"    \"data\":\r\n"
						"    {\r\n"
						"        \"fwArtf\":\"%s\",\r\n"
						"        \"fwName\":\"%s\",\r\n"
						"        \"fwVerStr\":\"%s\",\r\n"
						"        \"fwBinStr\":\"%s\",\r\n"
						"        \"fwBootArtf\":\"%s\",\r\n"
						"        \"fwBootName\":\"%s\",\r\n"
						"        \"fwBootVerStr\":\"%s\",\r\n"
						"        \"fwBootBinStr\":\"%s\",\r\n"
						"        \"hwPcbArtf\":\"%s\",\r\n"
						"        \"hwPcbName\":\"%s\",\r\n"
						"        \"hwPcbVerStr\":\"%s\",\r\n"
						"        \"hwBomArtf\":\"%s\",\r\n"
						"        \"hwBomName\":\"%s\",\r\n"
						"        \"hwBomVerStr\":\"%s\",\r\n"
						"        \"hwId\":%u,\r\n"
						"        \"hwMcuUniqueIdStr\":\"%s\"\r\n"
						"    }\r\n"
						"}\r\n",
						fwArtf,
						fwName,
						fwVerStr,
						fwBinStr,
						fwBootArtf,
						fwBootName,
						fwBootVerStr,
						fwBootBinStr,
						hwPcbArtf,
						hwPcbName,
						hwPcbVerStr,
						hwBomArtf,
						hwBomName,
						hwBomVerStr,
						hwId,
						hwMcuUniqueIdStr
					);
				}
				else
				{
					sprintf
					(
						me->buff,
						"{\r\n"
						"    \"status\":\"success\",\r\n"
						"    \"data\":\r\n"
						"    {\r\n"
						"        \"fwArtf\":\"%s\",\r\n"
						"        \"fwName\":\"%s\",\r\n"
						"        \"fwVerStr\":\"%s\",\r\n"
						"        \"fwBinStr\":\"%s\",\r\n"
						"        \"hwPcbArtf\":\"%s\",\r\n"
						"        \"hwPcbName\":\"%s\",\r\n"
						"        \"hwPcbVerStr\":\"%s\",\r\n"
						"        \"hwBomArtf\":\"%s\",\r\n"
						"        \"hwBomName\":\"%s\",\r\n"
						"        \"hwBomVerStr\":\"%s\",\r\n"
						"        \"hwId\":%u,\r\n"
						"        \"hwMcuUniqueIdStr\":\"%s\"\r\n"
						"    }\r\n"
						"}\r\n",
						fwArtf,
						fwName,
						fwVerStr,
						fwBinStr,
						hwPcbArtf,
						hwPcbName,
						hwPcbVerStr,
						hwBomArtf,
						hwBomName,
						hwBomVerStr,
						hwId,
						hwMcuUniqueIdStr
					);
				}
			}
			else
			{
				if (fwIsBootUsed)
				{
					sprintf
					(
						me->buff,
						"{"
							"\"status\":\"success\","
							"\"data\":"
							"{"
								"\"fwArtf\":\"%s\","
								"\"fwName\":\"%s\","
								"\"fwVerStr\":\"%s\","
								"\"fwBinStr\":\"%s\","
								"\"fwBootArtf\":\"%s\","
								"\"fwBootName\":\"%s\","
								"\"fwBootVerStr\":\"%s\","
								"\"fwBootBinStr\":\"%s\","
								"\"hwPcbArtf\":\"%s\","
								"\"hwPcbName\":\"%s\","
								"\"hwPcbVerStr\":\"%s\","
								"\"hwBomArtf\":\"%s\","
								"\"hwBomName\":\"%s\","
								"\"hwBomVerStr\":\"%s\","
								"\"hwId\":%u,"
								"\"hwMcuUniqueIdStr\":\"%s\""
							"}"
						"}\r\n",
						fwArtf,
						fwName,
						fwVerStr,
						fwBinStr,
						fwBootArtf,
						fwBootName,
						fwBootVerStr,
						fwBootBinStr,
						hwPcbArtf,
						hwPcbName,
						hwPcbVerStr,
						hwBomArtf,
						hwBomName,
						hwBomVerStr,
						hwId,
						hwMcuUniqueIdStr
					);
				}
				else
				{
					sprintf
					(
						me->buff,
						"{"
							"\"status\":\"success\","
							"\"data\":"
							"{"
								"\"fwArtf\":\"%s\","
								"\"fwName\":\"%s\","
								"\"fwVerStr\":\"%s\","
								"\"fwBinStr\":\"%s\","
								"\"hwPcbArtf\":\"%s\","
								"\"hwPcbName\":\"%s\","
								"\"hwPcbVerStr\":\"%s\","
								"\"hwBomArtf\":\"%s\","
								"\"hwBomName\":\"%s\","
								"\"hwBomVerStr\":\"%s\","
								"\"hwId\":%u,"
								"\"hwMcuUniqueIdStr\":\"%s\""
							"}"
						"}\r\n",
						fwArtf,
						fwName,
						fwVerStr,
						fwBinStr,
						hwPcbArtf,
						hwPcbName,
						hwPcbVerStr,
						hwBomArtf,
						hwBomName,
						hwBomVerStr,
						hwId,
						hwMcuUniqueIdStr
					);
				}
			}

			// Send response
			ALX_CLI_ASSERT(AlxSerialPort_WriteStr(me->alxSerialPort, me->buff) == Alx_Ok);
		}


		//------------------------------------------------------------------------------
		// Get Command
		//------------------------------------------------------------------------------
		else if (strcmp(me->buff, "get\r\n") == 0)
		{
			AlxCli_Get(me, false, ALX_NULL);
		}


		//------------------------------------------------------------------------------
		// Get Parameters Command
		//------------------------------------------------------------------------------
		else if (strcmp(me->buff, "get-param\r\n") == 0)
		{
			AlxCli_Get(me, true, AlxParamItem_Param);
		}


		//------------------------------------------------------------------------------
		// Get Variables Command
		//------------------------------------------------------------------------------
		else if (strcmp(me->buff, "get-var\r\n") == 0)
		{
			AlxCli_Get(me, true, AlxParamItem_Var);
		}


		//------------------------------------------------------------------------------
		// Get Flags Command
		//------------------------------------------------------------------------------
		else if (strcmp(me->buff, "get-flag\r\n") == 0)
		{
			AlxCli_Get(me, true, AlxParamItem_Flag);
		}


		//------------------------------------------------------------------------------
		// Get Constants Command
		//------------------------------------------------------------------------------
		else if (strcmp(me->buff, "get-const\r\n") == 0)
		{
			AlxCli_Get(me, true, AlxParamItem_Const);
		}


		//------------------------------------------------------------------------------
		// Get Triggers Command
		//------------------------------------------------------------------------------
		else if (strcmp(me->buff, "get-trig\r\n") == 0)
		{
			AlxCli_Get(me, true, AlxParamItem_Trig);
		}


		//------------------------------------------------------------------------------
		// Set Parameter Command
		//------------------------------------------------------------------------------
		else if (strncmp(me->buff, "set-param", strlen("set-param")) == 0)
		{
			//------------------------------------------------------------------------------
			// Local Variables
			//------------------------------------------------------------------------------
			Alx_Status status = Alx_Err;


			//------------------------------------------------------------------------------
			// Handle
			//------------------------------------------------------------------------------
			while (1)
			{
				//------------------------------------------------------------------------------
				// Parse
				//------------------------------------------------------------------------------
				char key[ALX_CLI_BUFF_LEN] = "";
				char val[ALX_CLI_BUFF_LEN] = "";
				int sscanfStatus = sscanf
				(
					me->buff,
					"set-param --key %s --val %s",
					key,
					val
				);
				if (sscanfStatus != 2)
				{
					status = Alx_Err;
					break;
				}


				//------------------------------------------------------------------------------
				// Set
				//------------------------------------------------------------------------------
				status = AlxParamMgmt_ByKey_SetVal_StrFormat(me->alxParamMgmt, key, val);


				//------------------------------------------------------------------------------
				// Break
				//------------------------------------------------------------------------------
				break;
			}


			//------------------------------------------------------------------------------
			// Prepare Response
			//------------------------------------------------------------------------------
			if (status == Alx_Ok)
			{
				AlxCli_PrepResp_Success(me);
			}
			else
			{
				AlxCli_PrepResp_ErrArg(me);
			}


			//------------------------------------------------------------------------------
			// Send Response
			//------------------------------------------------------------------------------
			ALX_CLI_ASSERT(AlxSerialPort_WriteStr(me->alxSerialPort, me->buff) == Alx_Ok);
		}


		//------------------------------------------------------------------------------
		// Invalid Command
		//------------------------------------------------------------------------------
		else
		{
			// Prepare response
			AlxCli_PrepResp_ErrCmd(me);

			// Send response
			ALX_CLI_ASSERT(AlxSerialPort_WriteStr(me->alxSerialPort, me->buff) == Alx_Ok);
		}
	}
}


//******************************************************************************
// Private Functions
//******************************************************************************
static void AlxCli_Get(AlxCli* me, bool paramTypeCheck, AlxParamItem_ParamType paramType)
{
	//------------------------------------------------------------------------------
	// JSON Header
	//------------------------------------------------------------------------------

	// Prepare
	if (me->prettyJsonResp)
	{
		strcpy
		(
			me->buff,
			"{\r\n"
			"    \"status\":\"success\",\r\n"
			"    \"data\":\r\n"
			"    {\r\n"
		);
	}
	else
	{
		strcpy
		(
			me->buff,
			"{"
				"\"status\":\"success\","
				"\"data\":"
				"{"
		);
	}

	// Send
	ALX_CLI_ASSERT(AlxSerialPort_WriteStr(me->alxSerialPort, me->buff) == Alx_Ok);


	//------------------------------------------------------------------------------
	// JSON Body
	//------------------------------------------------------------------------------

	// Local variables
	uint32_t numOfParamItems = AlxParamMgmt_GetNumOfParamItems(me->alxParamMgmt);
	uint32_t numOfParamTypeItems = AlxParamMgmt_GetNumOfParamTypeItems(me->alxParamMgmt, paramType);
	uint32_t paramTypeItemsIndex = 0;

	// Loop through all parameters
	for (uint32_t i = 0; i < numOfParamItems; i++)
	{
		// If param type check enabled
		if (paramTypeCheck)
		{
			// Get param type
			AlxParamItem_ParamType _paramType = AlxParamMgmt_ByIndex_GetParamType(me->alxParamMgmt, i);

			// Check param type, if NOT selected, continue, Else increment
			if (_paramType != paramType)
			{
				continue;
			}
			else
			{
				paramTypeItemsIndex++;
			}
		}

		// Get key
		const char* key = AlxParamMgmt_ByIndex_GetKey(me->alxParamMgmt, i);

		// Get data type
		AlxParamItem_DataType dataType = AlxParamMgmt_ByIndex_GetDataType(me->alxParamMgmt, i);

		// Get value in string format
		char val[ALX_CLI_BUFF_LEN] = "";
		AlxParamMgmt_ByIndex_GetVal_StrFormat(me->alxParamMgmt, i, val, sizeof(val));

		// If string type add double quote (") around value, Else just use value
		if (dataType == AlxParamItem_Str)
		{
			if (me->prettyJsonResp)
			{
				sprintf(me->buff, "        \"%s\":\"%s\"", key, val);
			}
			else
			{
				sprintf(me->buff, "\"%s\":\"%s\"", key, val);
			}
		}
		else
		{
			if (me->prettyJsonResp)
			{
				sprintf(me->buff, "        \"%s\":%s", key, val);
			}
			else
			{
				sprintf(me->buff, "\"%s\":%s", key, val);
			}
		}

		// If NOT last line in loop, add comma
		if
		(
			((paramTypeCheck == true) && (paramTypeItemsIndex != numOfParamTypeItems)) ||
			((paramTypeCheck == false) && (i < (numOfParamItems - 1)))
		)
		{
			if (me->prettyJsonResp)
			{
				strcat(me->buff, ",\r\n");
			}
			else
			{
				strcat(me->buff, ",");
			}
		}
		else
		{
			if (me->prettyJsonResp)
			{
				strcat(me->buff, "\r\n");
			}
			else
			{
				// Do Nothing
			}
		}

		// Send
		ALX_CLI_ASSERT(AlxSerialPort_WriteStr(me->alxSerialPort, me->buff) == Alx_Ok);
	}


	//------------------------------------------------------------------------------
	// JSON Footer
	//------------------------------------------------------------------------------

	// Prepare
	if (me->prettyJsonResp)
	{
		strcpy
		(
			me->buff,
			"    }\r\n"
			"}\r\n"
		);
	}
	else
	{
		strcpy
		(
			me->buff,
				"}"
			"}\r\n"
		);
	}

	// Send
	ALX_CLI_ASSERT(AlxSerialPort_WriteStr(me->alxSerialPort, me->buff) == Alx_Ok);
}
static void AlxCli_PrepResp_Success(AlxCli* me)
{
	if (me->prettyJsonResp)
	{
		sprintf
		(
			me->buff,
			"{\r\n"
			"    \"status\":\"success\",\r\n"
			"    \"data\":null\r\n"
			"}\r\n"
		);
	}
	else
	{
		sprintf
		(
			me->buff,
			"{"
				"\"status\":\"success\","
				"\"data\":null"
			"}\r\n"
		);
	}
}
static void AlxCli_PrepResp_ErrCmd(AlxCli* me)
{
	if (me->prettyJsonResp)
	{
		sprintf
		(
			me->buff,
			"{\r\n"
			"    \"status\":\"error\",\r\n"
			"    \"message\":\"Invalid command\"\r\n"
			"}\r\n"
		);
	}
	else
	{
		sprintf
		(
			me->buff,
			"{"
				"\"status\":\"error\","
				"\"message\":\"Invalid command\""
			"}\r\n"
		);
	}
}
static void AlxCli_PrepResp_ErrArg(AlxCli* me)
{
	if (me->prettyJsonResp)
	{
		sprintf
		(
			me->buff,
			"{\r\n"
			"    \"status\":\"error\",\r\n"
			"    \"message\":\"Invalid arguments\"\r\n"
			"}\r\n"
		);
	}
	else
	{
		sprintf
		(
			me->buff,
			"{"
				"\"status\":\"error\","
				"\"message\":\"Invalid arguments\""
			"}\r\n"
		);
	}
}


#endif	// #if defined(ALX_C_LIB)
