# Auralix C/C++ Library CLI Design Specifications
- UART configuration: **115200 bps** baud rate, 8 data bits, no parity, 1 stop bit
- New line delimiter is `\r\n`
- All responses are in **serialized minified/compact JSON format** for efficient transmission and easy parsing
	- **EXAMPLE:** `{"status":"success"}\r\n`
	- **ATTENTION:** All provided examples below use human-readable JSON format for better readability and illustration

## CLI Table
- [CliTable.xlsx](CliTable.xlsx)

| Command / Event                                   | Command Format                                       | Event Name |
|---------------------------------------------------|------------------------------------------------------|------------|
| [Help Command](#help-command)                     | help                                                 | /          |
| [Reset Command](#reset-command)                   | reset                                                | /          |
| [ID Command](#id-command)                         | id                                                   | /          |
| [Get Command](#get-command)                       | get                                                  | /          |
| [Get Parameters Command](#get-parameters-command) | get-param                                            | /          |
| [Get Variables Command](#get-variables-command)   | get-var                                              | /          |
| [Get Flags Command](#get-flags-command)           | get-flag                                             | /          |
| [Get Constants Command](#get-constants-command)   | get-const                                            | /          |
| [Get Triggers Command](#get-triggers-command)     | get-trig                                             | /          |
| [Set Parameter Command](#set-parameter-command)   | set-param --key <param_key> --val <param_val_to_set> | /          |

## Help Command
- Gets CLI help info

#### Format
`help`

## Reset Command
- Triggers device reset

#### Format
`reset`

#### Use Case
1. User writes `reset` via CLI to device
2. Device responds with success:
```
{
    "status":"success"
}
```
3. Device resets

## ID Command
- Gets device ID info

#### Format
`id`

#### Use Case
1. User writes `id` via CLI to device
2. Device responds with success and ID info:
```
{
    "status":"success",
    "data":
    {
        "fwArtf":"ALX-202-AF-12-1",
        "fwName":"NucleoF429Zi_CFw",
        "fwVerStr":"0.1.0.2406082123.2726d2b810ebb4c0dd2e484c36c35839eb055719",
        "fwBinStr":"2406082123_ALX-202-AF-12-1_NucleoF429Zi_CFw_V0-1-0_2726d2b.bin",
        "hwPcbArtf":"ALX-NUCLEO-F429ZI",
        "hwPcbName":"NUCLEO-F429ZI",
        "hwPcbVerStr":"1.0.0.2401011200",
        "hwBomArtf":"ALX-NUCLEO-F429ZI",
        "hwBomName":"Default",
        "hwBomVerStr":"1.0.0.2401011200",
        "hwId":0,
        "hwMcuUniqueIdStr":"39323434343551020027003D"
    }
}
```

## Get Command
- Gets device properties

#### Format
`get`

## Get Parameters Command
- Gets device parameters

#### Format
`get-param`

#### Use Case
1. User writes `get-param` via CLI to device
2. Device responds with success and parameters values:
```
{
    "status":"success",
    "data":
    {
        "PRETTY_JSON_EN":false
    }
}
```

## Get Variables Command
- Gets device variables

#### Format
`get-var`

#### Use Case
1. User writes `get-var` via CLI to device
2. Device responds with success and variables values:
```
{
    "status":"success",
    "data":
    {
        "mcu_coreTemp_degC":55.10
    }
}
```

## Get Flags Command
- Gets device flags

#### Format
`get-flag`

## Get Constants Command
- Gets device constants

#### Format
`get-const`

## Get Triggers Command
- Gets device triggers

#### Format
`get-trig`

## Set Parameter Command
- Sets specified device parameter value

#### Format
`set-param --key <param_key> --val <param_val_to_set>`

#### Use Case
1. User writes `set-param --key PRETTY_JSON_EN --val true` via CLI to device
2. Device sets specified parameter to specified value
3. Device responds with success:
```
{
    "status":"success"
}
```
