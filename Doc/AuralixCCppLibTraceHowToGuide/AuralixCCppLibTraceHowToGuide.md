# Auralix C/C++ Library Trace How-To Guide

## Configuration
- Trace level can be defined globally in user file `alxConfig.h`

```
alxConfig.h

#define ALX_TRACE_LEVEL ALX_TRACE_LEVEL_INF
```

- Tracing can be enabled/disabled for each module by commenting/uncommenting relevant defines in user file `alxConfig.h`

```
alxConfig.h

#define ALX_<module_name>_TRACE_ENABLE
```

- Because trace level is defined globally, **different trace levels per module are NOT supported**
	- You can only enable/disable tracing per module)

## Trace Level
- Higher trace level number means more messages are traced
- Understand that **tracing always affects system behavior**, so use it reasonably

#### ALX_TRACE_LEVEL_OFF - 0
- Tracing is globally OFF, no messages are traced in any module

#### ALX_TRACE_LEVEL_FTL - 1
- Use `_FTL` level for tracing of **fatal system errors**
- **NOTE:** After fatal system error, **system shall always reset**

#### ALX_TRACE_LEVEL_ERR - 2
- Use `_ERR` level for tracing of **errors after which function returns error status**
- **NOTE:** On APP layer, use it when operation finally fails (for example after all retries fail)

#### ALX_TRACE_LEVEL_WRN - 3
- Use `_WRN` level for tracing of **warnings/obstacles after which function can still retry/continue**
- **NOTE:** On APP layer, use it for temporary problems during operation (for example retry 1, retry 2, ...), when operation may still end with success

#### ALX_TRACE_LEVEL_INF - 4
- Use `_INF` level for tracing of **meaningful system/app information**, such as:
	- System state changes
	- System events
	- System statuses/variables
- **NOTE:**
	- The higher-level the module is (more APP specific), the more `_INF` tracing can be used in a way that is useful for the APP
	- The lower-level the module is (less APP specific), the less `_INF` tracing is recommended, because it's less clear what is useful to trace for the APP

#### ALX_TRACE_LEVEL_DBG - 5
- Use `_DBG` level for tracing of **selected useful debug information**, such as:
	- Function flow (enter/exit, where the function is)
	- Important function inputs & outputs (input argument values, return values)
	- Relevant variables at key debug points

#### ALX_TRACE_LEVEL_VRB - 6
- Use `_VRB` level for tracing of **full detailed internal/raw data**, such as:
	- Registers
	- Raw packet bytes
	- Buffers
	- Flags
	- Counters
	- Intermediate variables
- **ATTENTION:**
	- When using `_VRB` level, it's recommended to disable tracing for most modules, so that only a few specific modules that we want to debug/analyze are traced
	- This also drastically reduces the total number of trace messages, so impact on system behavior is minimized when using `_VRB` level
