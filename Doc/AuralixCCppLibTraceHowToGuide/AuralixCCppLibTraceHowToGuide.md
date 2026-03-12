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

- Because trace level is defined globally, different trace levels per module are NOT supported
	- You can only enable/disable tracing per module

## Trace Level
- Higher trace level number means more messages are traced
- Understand that **tracing always affects system behavior**, so use it reasonably and with understanding

#### ALX_TRACE_LEVEL_OFF - 0
- Tracing is globally OFF, no messages are traced in any module

#### ALX_TRACE_LEVEL_FTL - 1
- Use `_FTL` level for fatal system errors tracing
- **NOTE:** After fatal system error, **system shall always reset**

#### ALX_TRACE_LEVEL_ERR - 2
- Use `_ERR` level for function errors 
	- After that **function shall shall always reset**

#### ALX_TRACE_LEVEL_WRN - 3
- TODO

#### ALX_TRACE_LEVEL_INF - 4
- Use `_INF` level for system states/events/statuses/variables tracing
- **NOTE:**
	- The more APP specific (high level) the module is the more you can use in a ways that is most usable for this APP
	- The more lower level module is (library, NOT APP specific) the less is recommended to use it, becae here tu neveš kaj bo smiselno tracing delat katare variables

#### ALX_TRACE_LEVEL_DBG - 5
- TODO

#### ALX_TRACE_LEVEL_VRB - 6
- Use `_VRB` level for tracing 
	- After that **function shall shall always reset**
