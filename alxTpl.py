#*******************************************************************************
# @file			alxTpl.py
# @brief		Auralix C Library - ALX Template Script
# @copyright	Copyright (C) 2020-2022 Auralix d.o.o. All rights reserved.
#
# @section License
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
# This file is part of Auralix C Library.
#
# Auralix C Library is free software: you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation, either version 3
# of the License, or (at your option) any later version.
#
# Auralix C Library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with Auralix C Library. If not, see <https://www.gnu.org/licenses/>.
#*****************************************************************************


#*******************************************************************************
# Imports
#*******************************************************************************
import pathlib
import sys


#*******************************************************************************
# Script
#*******************************************************************************
def Script(brief, cVsCpp, prefix, name1, name2="", name3="", name4=""):

	# Local variables
	prefixLow =		prefix.lower()
	prefixUp =		prefix.upper()
	prefixCap =		prefix.capitalize()
	name1Up =		name1.upper()
	name1Cap =		name1.capitalize()
	name2Up =		name2.upper()
	name2Cap =		name2.capitalize()
	name3Up =		name3.upper()
	name3Cap =		name3.capitalize()
	name4Up =		name4.upper()
	name4Cap =		name4.capitalize()

	# Set nameUp & nameCap
	if (name1 != "") and (name2 == "") and (name3 == "") and (name4 == ""):
		nameUp = name1Up
		nameCap = name1Cap
	elif (name1 != "") and (name2 != "") and (name3 == "") and (name4 == ""):
		nameUp = name1Up + "_" + name2Up
		nameCap = name1Cap + name2Cap
	elif (name1 != "") and (name2 != "") and (name3 != "") and (name4 == ""):
		nameUp = name1Up + "_" + name2Up + "_" + name3Up
		nameCap = name1Cap + name2Cap + name3Cap
	elif (name1 != "") and (name2 != "") and (name3 != "") and (name4 != ""):
		nameUp = name1Up + "_" + name2Up + "_" + name3Up + "_" + name4Up
		nameCap = name1Cap + name2Cap + name3Cap + name4Cap
	else:
		raise	# We should never get here

	# Create file
	if (cVsCpp == "c"):
		if prefixLow == "alx":
			_CreateFileAlx_h(brief, prefixLow, prefixUp, prefixCap, nameUp, nameCap)
			_CreateFileAlx_c(brief, prefixLow, prefixUp, prefixCap, nameUp, nameCap)
		else:
			_CreateFile_h(brief, prefixLow, prefixUp, prefixCap, nameUp, nameCap)
			_CreateFile_c(brief, prefixLow, prefixUp, prefixCap, nameUp, nameCap)
	elif (cVsCpp == "cpp"):
		if prefixLow == "alx":
			_CreateFileAlx_hpp(brief, prefixLow, prefixUp, prefixCap, nameUp, nameCap)
		else:
			_CreateFile_hpp(brief, prefixLow, prefixUp, prefixCap, nameUp, nameCap)
	else:
		raise	# We should never get here


#*******************************************************************************
# Private Functions
#*******************************************************************************
def _CreateFileAlx_h(brief, prefixLow, prefixUp, prefixCap, nameUp, nameCap):
	# Local variables
	filePath = pathlib.Path(prefixLow + nameCap + ".h")
	fileText = """/**
  ******************************************************************************
  * @file		{prefixLow}{nameCap}.h
  * @brief		{brief}
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
// Include Guard
//******************************************************************************
#ifndef {prefixUp}_{nameUp}_H
#define {prefixUp}_{nameUp}_H

#ifdef __cplusplus
extern "C" {{
#endif


//******************************************************************************
// Includes
//******************************************************************************
#include "alxGlobal.h"
#include "alxAssert.h"
#include "alxTrace.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB)


//******************************************************************************
// Preprocessor
//******************************************************************************
#define {prefixUp}_{nameUp}_FILE "{prefixLow}{nameCap}.h"

// Assert //
#if defined(_{prefixUp}_{nameUp}_ASSERT_BKPT) || defined(_{prefixUp}_ASSERT_BKPT_ALL)
	#define {prefixUp}_{nameUp}_ASSERT(expr) ALX_ASSERT_BKPT({prefixUp}_{nameUp}_FILE, expr)
#elif defined(_{prefixUp}_{nameUp}_ASSERT_TRACE) || defined(_{prefixUp}_ASSERT_TRACE_ALL)
	#define {prefixUp}_{nameUp}_ASSERT(expr) ALX_ASSERT_TRACE({prefixUp}_{nameUp}_FILE, expr)
#elif defined(_{prefixUp}_{nameUp}_ASSERT_RST) || defined(_{prefixUp}_ASSERT_RST_ALL)
	#define {prefixUp}_{nameUp}_ASSERT(expr) ALX_ASSERT_RST({prefixUp}_{nameUp}_FILE, expr)
#else
	#define {prefixUp}_{nameUp}_ASSERT(expr) do{{}} while (false)
#endif

// Trace //
#if defined(_{prefixUp}_{nameUp}_TRACE) || defined(_{prefixUp}_TRACE_ALL)
	#define {prefixUp}_{nameUp}_TRACE(...) ALX_TRACE_STD({prefixUp}_{nameUp}_FILE, __VA_ARGS__)
	#define {prefixUp}_{nameUp}_TRACE_FORMAT(...) ALX_TRACE_FORMAT(__VA_ARGS__)
#else
	#define {prefixUp}_{nameUp}_TRACE(...) do{{}} while (false)
	#define {prefixUp}_{nameUp}_TRACE_FORMAT(...) do{{}} while (false)
#endif


//******************************************************************************
// Types
//******************************************************************************
typedef struct
{{
	// Parameters
	uint32_t* param;

	// Variables
	uint32_t var;

	// Info
	bool wasCtorCalled;
	bool isInit;
}} {prefixCap}{nameCap};


//******************************************************************************
// Constructor
//******************************************************************************
void {prefixCap}{nameCap}_Ctor
(
	{prefixCap}{nameCap}* me,
	uint32_t* param
);


//******************************************************************************
// Functions
//******************************************************************************
void {prefixCap}{nameCap}_Init({prefixCap}{nameCap}* me);
void {prefixCap}{nameCap}_Handle({prefixCap}{nameCap}* me);


#endif	// #if defined(ALX_C_LIB)

#ifdef __cplusplus
}}
#endif

#endif	// #ifndef {prefixUp}_{nameUp}_H
""".format(prefixLow=prefixLow, prefixUp=prefixUp, prefixCap=prefixCap, nameUp=nameUp, nameCap=nameCap, brief=brief)

	# Create file
	filePath.write_text(fileText)

	# Print
	print("Created - " + prefixLow + nameCap + ".h")

def _CreateFileAlx_c(brief, prefixLow, prefixUp, prefixCap, nameUp, nameCap):
	# Local variables
	filePath = pathlib.Path(prefixLow + nameCap + ".c")
	fileText = """/**
  ******************************************************************************
  * @file		{prefixLow}{nameCap}.c
  * @brief		{brief}
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
#include "{prefixLow}{nameCap}.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB)


//******************************************************************************
// Constructor
//******************************************************************************
void {prefixCap}{nameCap}_Ctor
(
	{prefixCap}{nameCap}* me,
	uint32_t* param
)
{{
	// Parameters
	me->param = param;

	// Variables
	me->var = 0;

	// Info
	me->wasCtorCalled = true;
	me->isInit = false;
}}


//******************************************************************************
// Functions
//******************************************************************************
void {prefixCap}{nameCap}_Init({prefixCap}{nameCap}* me)
{{
	// Assert
	{prefixUp}_{nameUp}_ASSERT(me->wasCtorCalled == true);
	{prefixUp}_{nameUp}_ASSERT(me->isInit == false);

	// Set isInit
	me->isInit = true;
}}
void {prefixCap}{nameCap}_Handle({prefixCap}{nameCap}* me)
{{
	// Assert
	{prefixUp}_{nameUp}_ASSERT(me->wasCtorCalled == true);
	{prefixUp}_{nameUp}_ASSERT(me->isInit == true);
}}


#endif	// #if defined(ALX_C_LIB)
""".format(prefixLow=prefixLow, prefixUp=prefixUp, prefixCap=prefixCap, nameUp=nameUp, nameCap=nameCap, brief=brief)

	# Create file
	filePath.write_text(fileText)

	# Print
	print("Created - " + prefixLow + nameCap + ".c")

def _CreateFile_h(brief, prefixLow, prefixUp, prefixCap, nameUp, nameCap):
	# Local variables
	filePath = pathlib.Path(prefixLow + nameCap + ".h")
	fileText = """/**
  ******************************************************************************
  * @file		{prefixLow}{nameCap}.h
  * @brief		{brief}
  * @copyright	Copyright (C) Auralix d.o.o. All rights reserved.
  ******************************************************************************
  */

//******************************************************************************
// Include Guard
//******************************************************************************
#ifndef {prefixUp}_{nameUp}_H
#define {prefixUp}_{nameUp}_H

#ifdef __cplusplus
extern "C" {{
#endif


//******************************************************************************
// Includes
//******************************************************************************
#include "alxGlobal.h"
#include "alxAssert.h"
#include "alxTrace.h"
#include "{prefixLow}Config.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined({prefixUp}_APP) || defined({prefixUp}_TEST)


//******************************************************************************
// Preprocessor
//******************************************************************************
#define {prefixUp}_{nameUp}_FILE "{prefixLow}{nameCap}.h"

// Assert //
#if defined(_{prefixUp}_{nameUp}_ASSERT_BKPT) || defined(_{prefixUp}_ASSERT_BKPT_ALL)
	#define {prefixUp}_{nameUp}_ASSERT(expr) ALX_ASSERT_BKPT({prefixUp}_{nameUp}_FILE, expr)
#elif defined(_{prefixUp}_{nameUp}_ASSERT_TRACE) || defined(_{prefixUp}_ASSERT_TRACE_ALL)
	#define {prefixUp}_{nameUp}_ASSERT(expr) ALX_ASSERT_TRACE({prefixUp}_{nameUp}_FILE, expr)
#elif defined(_{prefixUp}_{nameUp}_ASSERT_RST) || defined(_{prefixUp}_ASSERT_RST_ALL)
	#define {prefixUp}_{nameUp}_ASSERT(expr) ALX_ASSERT_RST({prefixUp}_{nameUp}_FILE, expr)
#else
	#define {prefixUp}_{nameUp}_ASSERT(expr) do{{}} while (false)
#endif

// Trace //
#if defined(_{prefixUp}_{nameUp}_TRACE) || defined(_{prefixUp}_TRACE_ALL)
	#define {prefixUp}_{nameUp}_TRACE(...) ALX_TRACE_STD({prefixUp}_{nameUp}_FILE, __VA_ARGS__)
	#define {prefixUp}_{nameUp}_TRACE_FORMAT(...) ALX_TRACE_FORMAT(__VA_ARGS__)
#else
	#define {prefixUp}_{nameUp}_TRACE(...) do{{}} while (false)
	#define {prefixUp}_{nameUp}_TRACE_FORMAT(...) do{{}} while (false)
#endif


//******************************************************************************
// Types
//******************************************************************************
typedef struct
{{
	// Parameters
	uint32_t* param;

	// Variables
	uint32_t var;

	// Info
	bool wasCtorCalled;
	bool isInit;
}} {prefixCap}{nameCap};


//******************************************************************************
// Constructor
//******************************************************************************
void {prefixCap}{nameCap}_Ctor
(
	{prefixCap}{nameCap}* me,
	uint32_t* param
);


//******************************************************************************
// Functions
//******************************************************************************
void {prefixCap}{nameCap}_Init({prefixCap}{nameCap}* me);
void {prefixCap}{nameCap}_Handle({prefixCap}{nameCap}* me);


#endif	// #if defined({prefixUp}_APP) || defined({prefixUp}_TEST)

#ifdef __cplusplus
}}
#endif

#endif	// #ifndef {prefixUp}_{nameUp}_H
""".format(prefixLow=prefixLow, prefixUp=prefixUp, prefixCap=prefixCap, nameUp=nameUp, nameCap=nameCap, brief=brief)

	# Create file
	filePath.write_text(fileText)

	# Print
	print("Created - " + prefixLow + nameCap + ".h")

def _CreateFile_c(brief, prefixLow, prefixUp, prefixCap, nameUp, nameCap):
	# Local variables
	filePath = pathlib.Path(prefixLow + nameCap + ".c")
	fileText = """/**
  ******************************************************************************
  * @file		{prefixLow}{nameCap}.c
  * @brief		{brief}
  * @copyright	Copyright (C) Auralix d.o.o. All rights reserved.
  ******************************************************************************
  */

//******************************************************************************
// Includes
//******************************************************************************
#include "{prefixLow}{nameCap}.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined({prefixUp}_APP) || defined({prefixUp}_TEST)


//******************************************************************************
// Constructor
//******************************************************************************
void {prefixCap}{nameCap}_Ctor
(
	{prefixCap}{nameCap}* me,
	uint32_t* param
)
{{
	// Parameters
	me->param = param;

	// Variables
	me->var = 0;

	// Info
	me->wasCtorCalled = true;
	me->isInit = false;
}}


//******************************************************************************
// Functions
//******************************************************************************
void {prefixCap}{nameCap}_Init({prefixCap}{nameCap}* me)
{{
	// Assert
	{prefixUp}_{nameUp}_ASSERT(me->wasCtorCalled == true);
	{prefixUp}_{nameUp}_ASSERT(me->isInit == false);

	// Set isInit
	me->isInit = true;
}}
void {prefixCap}{nameCap}_Handle({prefixCap}{nameCap}* me)
{{
	// Assert
	{prefixUp}_{nameUp}_ASSERT(me->wasCtorCalled == true);
	{prefixUp}_{nameUp}_ASSERT(me->isInit == true);
}}


#endif	// #if defined({prefixUp}_APP) || defined({prefixUp}_TEST)
""".format(prefixLow=prefixLow, prefixUp=prefixUp, prefixCap=prefixCap, nameUp=nameUp, nameCap=nameCap, brief=brief)

	# Create file
	filePath.write_text(fileText)

	# Print
	print("Created - " + prefixLow + nameCap + ".c")

def _CreateFileAlx_hpp(brief, prefixLow, prefixUp, prefixCap, nameUp, nameCap):
	# Local variables
	filePath = pathlib.Path(prefixLow + nameCap + ".hpp")
	fileText = """/**
  ******************************************************************************
  * @file		{prefixLow}{nameCap}.hpp
  * @brief		{brief}
  * @copyright	Copyright (C) Auralix d.o.o. All rights reserved.
  *
  * @section License
  *
  * SPDX-License-Identifier: GPL-3.0-or-later
  *
  * This file is part of Auralix C++ Library.
  *
  * Auralix C++ Library is free software: you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation, either version 3
  * of the License, or (at your option) any later version.
  *
  * Auralix C++ Library is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  * GNU General Public License for more details.
  *
  * You should have received a copy of the GNU General Public License
  * along with Auralix C++ Library. If not, see <https://www.gnu.org/licenses/>.
  ******************************************************************************
  **/

//******************************************************************************
// Include Guard
//******************************************************************************
#ifndef {prefixUp}_{nameUp}_HPP
#define {prefixUp}_{nameUp}_HPP


//******************************************************************************
// Includes
//******************************************************************************
#include "alxGlobal.hpp"
#include "{prefixLow}{nameCap}.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_CPP_LIB)


//******************************************************************************
// Code
//******************************************************************************
namespace {prefixCap}
{{
	namespace {prefixCap}{nameCap}
	{{
		//******************************************************************************
		// Class - I{nameCap}
		//******************************************************************************
		class I{nameCap}
		{{
			public:
				//------------------------------------------------------------------------------
				// Public Functions
				//------------------------------------------------------------------------------
				I{nameCap}() {{}}
				virtual ~I{nameCap}() {{}}
				virtual void Init(void) = 0;
				virtual void Handle(void) = 0;
		}};


		//******************************************************************************
		// Class - {nameCap}
		//******************************************************************************
		class {nameCap} : public I{nameCap}
		{{
			public:
				//------------------------------------------------------------------------------
				// Public Functions
				//------------------------------------------------------------------------------
				{nameCap}
				(
					uint32_t* param
				) :
					param(param)
				{{}}
				virtual ~{nameCap}() {{}}
				void Init(void) override
				{{
					// Assert
					{prefixUp}_{nameUp}_ASSERT(isInit == false);

					// Set isInit
					isInit = true;
				}}
				void Handle(void) override
				{{
					// Assert
					{prefixUp}_{nameUp}_ASSERT(isInit == true);
				}}

			private:
				//------------------------------------------------------------------------------
				// Private Variables
				//------------------------------------------------------------------------------

				// Parameters
				uint32_t* param = nullptr;

				// Variables
				uint32_t var = 0;

				// Info
				bool isInit = false;
		}};
	}}
}}


#endif	// #if defined(ALX_CPP_LIB)

#endif	// #ifndef {prefixUp}_{nameUp}_HPP
""".format(prefixLow=prefixLow, prefixUp=prefixUp, prefixCap=prefixCap, nameUp=nameUp, nameCap=nameCap, brief=brief)

	# Create file
	filePath.write_text(fileText)

	# Print
	print("Created - " + prefixLow + nameCap + ".hpp")

def _CreateFile_hpp(brief, prefixLow, prefixUp, prefixCap, nameUp, nameCap):
	# Local variables
	filePath = pathlib.Path(prefixLow + nameCap + ".hpp")
	fileText = """/**
  ******************************************************************************
  * @file		{prefixLow}{nameCap}.hpp
  * @brief		{brief}
  * @copyright	Copyright (C) Auralix d.o.o. All rights reserved.
  ******************************************************************************
  */

//******************************************************************************
// Include Guard
//******************************************************************************
#ifndef {prefixUp}_{nameUp}_HPP
#define {prefixUp}_{nameUp}_HPP


//******************************************************************************
// Includes
//******************************************************************************
#include "alxGlobal.hpp"
#include "alxAssert.h"
#include "alxTrace.h"
#include "{prefixLow}Config.hpp"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined({prefixUp}_APP) || defined({prefixUp}_TEST)


//******************************************************************************
// Preprocessor
//******************************************************************************
#define {prefixUp}_{nameUp}_FILE "{prefixLow}{nameCap}.hpp"

// Assert //
#if defined(_{prefixUp}_{nameUp}_ASSERT_BKPT) || defined(_{prefixUp}_ASSERT_BKPT_ALL)
	#define {prefixUp}_{nameUp}_ASSERT(expr) ALX_ASSERT_BKPT({prefixUp}_{nameUp}_FILE, expr)
#elif defined(_{prefixUp}_{nameUp}_ASSERT_TRACE) || defined(_{prefixUp}_ASSERT_TRACE_ALL)
	#define {prefixUp}_{nameUp}_ASSERT(expr) ALX_ASSERT_TRACE({prefixUp}_{nameUp}_FILE, expr)
#elif defined(_{prefixUp}_{nameUp}_ASSERT_RST) || defined(_{prefixUp}_ASSERT_RST_ALL)
	#define {prefixUp}_{nameUp}_ASSERT(expr) ALX_ASSERT_RST({prefixUp}_{nameUp}_FILE, expr)
#else
	#define {prefixUp}_{nameUp}_ASSERT(expr) do{{}} while (false)
#endif

// Trace //
#if defined(_{prefixUp}_{nameUp}_TRACE) || defined(_{prefixUp}_TRACE_ALL)
	#define {prefixUp}_{nameUp}_TRACE(...) ALX_TRACE_STD({prefixUp}_{nameUp}_FILE, __VA_ARGS__)
	#define {prefixUp}_{nameUp}_TRACE_FORMAT(...) ALX_TRACE_FORMAT(__VA_ARGS__)
#else
	#define {prefixUp}_{nameUp}_TRACE(...) do{{}} while (false)
	#define {prefixUp}_{nameUp}_TRACE_FORMAT(...) do{{}} while (false)
#endif


//******************************************************************************
// Code
//******************************************************************************
namespace {prefixCap}
{{
	namespace {prefixCap}{nameCap}
	{{
		//******************************************************************************
		// Class - I{nameCap}
		//******************************************************************************
		class I{nameCap}
		{{
			public:
				//------------------------------------------------------------------------------
				// Public Functions
				//------------------------------------------------------------------------------
				I{nameCap}() {{}}
				virtual ~I{nameCap}() {{}}
				virtual void Init(void) = 0;
				virtual void Handle(void) = 0;
		}};


		//******************************************************************************
		// Class - {nameCap}
		//******************************************************************************
		class {nameCap} : public I{nameCap}
		{{
			public:
				//------------------------------------------------------------------------------
				// Public Functions
				//------------------------------------------------------------------------------
				{nameCap}
				(
					uint32_t* param
				) :
					param(param)
				{{}}
				virtual ~{nameCap}() {{}}
				void Init(void) override
				{{
					// Assert
					{prefixUp}_{nameUp}_ASSERT(isInit == false);

					// Set isInit
					isInit = true;
				}}
				void Handle(void) override
				{{
					// Assert
					{prefixUp}_{nameUp}_ASSERT(isInit == true);
				}}

			private:
				//------------------------------------------------------------------------------
				// Private Variables
				//------------------------------------------------------------------------------

				// Parameters
				uint32_t* param = nullptr;

				// Variables
				uint32_t var = 0;

				// Info
				bool isInit = false;
		}};
	}}
}}


#endif	// #if defined({prefixUp}_APP) || defined({prefixUp}_TEST)

#endif	// #ifndef {prefixUp}_{nameUp}_HPP
""".format(prefixLow=prefixLow, prefixUp=prefixUp, prefixCap=prefixCap, nameUp=nameUp, nameCap=nameCap, brief=brief)

	# Create file
	filePath.write_text(fileText)

	# Print
	print("Created - " + prefixLow + nameCap + ".hpp")


#*******************************************************************************
# Run Guard
#*******************************************************************************
if __name__ == "__main__":
	# Prepare param
	cVsCpp = sys.argv[1]
	prefix = sys.argv[2]
	name1 = sys.argv[3]
	name2 = sys.argv[4]
	brief = sys.argv[5]

	# Script
	Script(cVsCpp, prefix, name1, name2, brief)
