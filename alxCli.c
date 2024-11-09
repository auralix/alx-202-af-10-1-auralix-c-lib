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


//******************************************************************************
// Weak Functions
//******************************************************************************
bool AlxCli_Handle_Callback(AlxCli* me);
bool AlxCli_Help_Callback(AlxCli* me);


//******************************************************************************
// Constructor
//******************************************************************************
void AlxCli_Ctor
(
	AlxCli* me,
	AlxSerialPort* alxSerialPort,
	AlxId* alxId,
	AlxParamMgmt* alxParamMgmt,
	AlxParamItem* PRETTY_JSON_EN,
	void* buff,
	uint32_t buffLen
)
{
	// Parameters
	me->alxSerialPort = alxSerialPort;
	me->alxId = alxId;
	me->alxParamMgmt = alxParamMgmt;
	me->PRETTY_JSON_EN = PRETTY_JSON_EN;
	me->buff = buff;
	me->buffLen = buffLen;

	// Variables
	me->buffLenUsed = 0;

	// Info
	me->wasCtorCalled = true;
}


//******************************************************************************
// Functions
//******************************************************************************
void AlxCli_Handle(AlxCli* me)
{
	//------------------------------------------------------------------------------
	//------------------------------------------------------------------------------
	// Assert
	//------------------------------------------------------------------------------
	//------------------------------------------------------------------------------
	ALX_CLI_ASSERT(me->wasCtorCalled == true);




	//------------------------------------------------------------------------------
	//------------------------------------------------------------------------------
	// Handle Write Event
	//------------------------------------------------------------------------------
	//------------------------------------------------------------------------------
	if (me->buffLenUsed > 0)
	{
		// Write
		ALX_CLI_ASSERT(AlxSerialPort_Write(me->alxSerialPort, me->buff, me->buffLenUsed) == Alx_Ok);

		// Clear
		memset(me->buff, 0, me->buffLen);
		me->buffLenUsed = 0;
	}




	//------------------------------------------------------------------------------
	//------------------------------------------------------------------------------
	// Handle Read Command
	//------------------------------------------------------------------------------
	//------------------------------------------------------------------------------
	uint32_t cmdLen = 0;
	if (AlxSerialPort_ReadStrUntil(me->alxSerialPort, me->buff, "\r\n", me->buffLen, &cmdLen) == Alx_Ok)
	{
		while (1)
		{
			//------------------------------------------------------------------------------
			// Append \r\n
			//------------------------------------------------------------------------------
			strcat(me->buff, "\r\n");


			//------------------------------------------------------------------------------
			// Help Command
			//------------------------------------------------------------------------------
			if (strcmp(me->buff, "help\r\n") == 0)
			{
				//------------------------------------------------------------------------------
				// JSON Header
				//------------------------------------------------------------------------------
				strcpy
				(
					me->buff,
					"{\r\n"
					"    \"status\":\"success\",\r\n"
					"    \"data\":\r\n"
					"    {\r\n"
				);


				//------------------------------------------------------------------------------
				// JSON Body
				//------------------------------------------------------------------------------
				strcat(me->buff, "        \"help\":\"Help Command - Gets CLI help info\",\r\n");
				strcat(me->buff, "        \"reset\":\"Reset Command - Triggers device reset\",\r\n");
				strcat(me->buff, "        \"id\":\"ID Command - Gets device ID info\",\r\n");
				strcat(me->buff, "        \"get\":\"Get Command - Gets device properties\",\r\n");
				strcat(me->buff, "        \"get-param\":\"Get Parameters Command - Gets device parameters\",\r\n");
				strcat(me->buff, "        \"get-var\":\"Get Variables Command - Gets device variables\",\r\n");
				strcat(me->buff, "        \"get-flag\":\"Get Flags Command - Gets device flags\",\r\n");
				strcat(me->buff, "        \"get-const\":\"Get Constants Command - Gets device constants\",\r\n");
				strcat(me->buff, "        \"get-trig\":\"Get Triggers Command - Gets device triggers\",\r\n");
				strcat(me->buff, "        \"set-param\":\"Set Parameter Command - Sets specified device parameter value: set-param --key <param_key> --val <param_val_to_set>\"");
				bool isImplemented = AlxCli_Help_Callback(me);
				if (isImplemented == false)
				{
					strcat(me->buff, "\r\n");
				}


				//------------------------------------------------------------------------------
				// JSON Footer
				//------------------------------------------------------------------------------
				strcat
				(
					me->buff,
					"    }\r\n"
					"}\r\n"
				);


				//------------------------------------------------------------------------------
				// Send Response
				//------------------------------------------------------------------------------
				ALX_CLI_ASSERT(AlxSerialPort_WriteStr(me->alxSerialPort, me->buff) == Alx_Ok);


				//------------------------------------------------------------------------------
				// Break
				//------------------------------------------------------------------------------
				break;
			}


			//------------------------------------------------------------------------------
			// Reset Command
			//------------------------------------------------------------------------------
			if (strcmp(me->buff, "reset\r\n") == 0)
			{
				// Prepare response
				AlxCli_PrepareResponse(me, AlxCli_ResponseType_Success);

				// Send response
				ALX_CLI_ASSERT(AlxSerialPort_WriteStr(me->alxSerialPort, me->buff) == Alx_Ok);

				// Reset MCU
				NVIC_SystemReset();

				// Break
				break;
			}


			//------------------------------------------------------------------------------
			// ID Command
			//------------------------------------------------------------------------------
			if (strcmp(me->buff, "id\r\n") == 0)
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
				if (AlxParamItem_GetValBool(me->PRETTY_JSON_EN))
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

				// Break
				break;
			}


			//------------------------------------------------------------------------------
			// Get Command
			//------------------------------------------------------------------------------
			if (strcmp(me->buff, "get\r\n") == 0)
			{
				AlxCli_Get(me, false, ALX_NULL);
				break;
			}


			//------------------------------------------------------------------------------
			// Get Parameters Command
			//------------------------------------------------------------------------------
			if (strcmp(me->buff, "get-param\r\n") == 0)
			{
				AlxCli_Get(me, true, AlxParamItem_Param);
				break;
			}


			//------------------------------------------------------------------------------
			// Get Variables Command
			//------------------------------------------------------------------------------
			if (strcmp(me->buff, "get-var\r\n") == 0)
			{
				AlxCli_Get(me, true, AlxParamItem_Var);
				break;
			}


			//------------------------------------------------------------------------------
			// Get Flags Command
			//------------------------------------------------------------------------------
			if (strcmp(me->buff, "get-flag\r\n") == 0)
			{
				AlxCli_Get(me, true, AlxParamItem_Flag);
				break;
			}


			//------------------------------------------------------------------------------
			// Get Constants Command
			//------------------------------------------------------------------------------
			if (strcmp(me->buff, "get-const\r\n") == 0)
			{
				AlxCli_Get(me, true, AlxParamItem_Const);
				break;
			}


			//------------------------------------------------------------------------------
			// Get Triggers Command
			//------------------------------------------------------------------------------
			if (strcmp(me->buff, "get-trig\r\n") == 0)
			{
				AlxCli_Get(me, true, AlxParamItem_Trig);
				break;
			}


			//------------------------------------------------------------------------------
			// Set Parameter Command
			//------------------------------------------------------------------------------
			if (strncmp(me->buff, "set-param", strlen("set-param")) == 0)
			{
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
						"set-param --key %s --val %s\r\n",
						key,
						val);
					if (sscanfStatus != 2)
					{
						AlxCli_PrepareResponse(me, AlxCli_ResponseType_ErrArg);
						break;
					}


					//------------------------------------------------------------------------------
					// Set
					//------------------------------------------------------------------------------
					Alx_Status status = AlxParamMgmt_ByKey_SetVal_StrFormat(me->alxParamMgmt, key, val);
					if (status != Alx_Ok)
					{
						AlxCli_PrepareResponse(me, AlxCli_ResponseType_ErrArg);
						break;
					}


					//------------------------------------------------------------------------------
					// Break
					//------------------------------------------------------------------------------
					AlxCli_PrepareResponse(me, AlxCli_ResponseType_Success);
					break;
				}


				//------------------------------------------------------------------------------
				// Send Response
				//------------------------------------------------------------------------------
				ALX_CLI_ASSERT(AlxSerialPort_WriteStr(me->alxSerialPort, me->buff) == Alx_Ok);


				//------------------------------------------------------------------------------
				// Break
				//------------------------------------------------------------------------------
				break;
			}


			//------------------------------------------------------------------------------
			// Callback
			//------------------------------------------------------------------------------
			bool wasCmdHandled = AlxCli_Handle_Callback(me);
			if (wasCmdHandled)
			{
				break;
			}


			//------------------------------------------------------------------------------
			// Invalid Command
			//------------------------------------------------------------------------------

			// Flush serial port RX FIFO
			AlxSerialPort_FlushRxFifo(me->alxSerialPort);

			// Prepare response
			AlxCli_PrepareResponse(me, AlxCli_ResponseType_ErrCmd);

			// Send response
			ALX_CLI_ASSERT(AlxSerialPort_WriteStr(me->alxSerialPort, me->buff) == Alx_Ok);

			// Break
			break;
		}
	}


	//------------------------------------------------------------------------------
	// Clear
	//------------------------------------------------------------------------------
	memset(me->buff, 0, me->buffLen);
}
void AlxCli_PrepareEvent(AlxCli* me, const uint8_t* data, uint32_t len)
{
	// Assert
	ALX_CLI_ASSERT(me->wasCtorCalled == true);

	// Set
	memcpy(me->buff, data, len);
	me->buffLenUsed = len;
}
void AlxCli_PrepareResponse(AlxCli* me, AlxCli_ResponseType responseType)
{
	//------------------------------------------------------------------------------
	// Assert
	//------------------------------------------------------------------------------
	ALX_CLI_ASSERT(me->wasCtorCalled == true);


	//------------------------------------------------------------------------------
	// Prepare
	//------------------------------------------------------------------------------
	const char* statusStr = "";
	const char* messageStr = "";
	if (responseType == AlxCli_ResponseType_Success)
	{
		statusStr = "success";
		messageStr = NULL;
	}
	else if (responseType == AlxCli_ResponseType_ErrCmd)
	{
		statusStr = "error";
		messageStr = "Command invalid";
	}
	else if (responseType == AlxCli_ResponseType_ErrArg)
	{
		statusStr = "error";
		messageStr = "Arguments invalid";
	}
	else if (responseType == AlxCli_ResponseType_ErrCrc)
	{
		statusStr = "error";
		messageStr = "CRC mismatch";
	}
	else if (responseType == AlxCli_ResponseType_ErrTimeout)
	{
		statusStr = "error";
		messageStr = "Command timeout";
	}
	else
	{
		ALX_CLI_ASSERT(false);	// We shouldn't get here
	}


	//------------------------------------------------------------------------------
	// Set
	//------------------------------------------------------------------------------
	if (AlxParamItem_GetValBool(me->PRETTY_JSON_EN))
	{
		if (messageStr == NULL)
		{
			sprintf
			(
				me->buff,
				"{\r\n"
				"    \"status\":\"%s\"\r\n"
				"}\r\n",
				statusStr
			);
		}
		else
		{
			sprintf
			(
				me->buff,
				"{\r\n"
				"    \"status\":\"%s\",\r\n"
				"    \"message\":\"%s\"\r\n"
				"}\r\n",
				statusStr,
				messageStr
			);
		}
	}
	else
	{
		if (messageStr == NULL)
		{
			sprintf
			(
				me->buff,
				"{"
					"\"status\":\"%s\""
				"}\r\n",
				statusStr
			);
		}
		else
		{
			sprintf
			(
				me->buff,
				"{"
					"\"status\":\"%s\","
					"\"message\":\"%s\""
				"}\r\n",
				statusStr,
				messageStr
			);
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
	if (AlxParamItem_GetValBool(me->PRETTY_JSON_EN))
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
			if (AlxParamItem_GetValBool(me->PRETTY_JSON_EN))
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
			if (AlxParamItem_GetValBool(me->PRETTY_JSON_EN))
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
			if (AlxParamItem_GetValBool(me->PRETTY_JSON_EN))
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
			if (AlxParamItem_GetValBool(me->PRETTY_JSON_EN))
			{
				strcat(me->buff, "\r\n");
			}
			else
			{
				// Do nothing
			}
		}

		// Send
		ALX_CLI_ASSERT(AlxSerialPort_WriteStr(me->alxSerialPort, me->buff) == Alx_Ok);
	}


	//------------------------------------------------------------------------------
	// JSON Footer
	//------------------------------------------------------------------------------

	// Prepare
	if (AlxParamItem_GetValBool(me->PRETTY_JSON_EN))
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


//******************************************************************************
// Weak Functions
//******************************************************************************
ALX_WEAK bool AlxCli_Handle_Callback(AlxCli* me)
{
	(void)me;
	return false;
}
ALX_WEAK bool AlxCli_Help_Callback(AlxCli* me)
{
	(void)me;
	return false;
}


#endif	// #if defined(ALX_C_LIB)
