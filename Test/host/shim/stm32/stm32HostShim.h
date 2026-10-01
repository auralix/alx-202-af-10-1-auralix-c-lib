/**
  ******************************************************************************
  * @file		stm32HostShim.h
  * @brief		Auralix C Library - Configuration Matrix Fake - STM32 CMSIS and HAL
  * @copyright	Copyright (C) Auralix d.o.o. All rights reserved.
  *
  * Compile-only: the STM32 device headers, HAL and LL drivers as far as the alx
  * code uses them, for every STM32 family the library supports, written from
  * that use, so the configuration matrix compiles every alx line under an
  * ALX_STM32xx platform (Test/config/matrix.toml). Nothing here is the vendor's
  * and nothing here runs; the real one is the product's.
  *
  * It makes no claim about the vendor's signatures and layouts, only that every
  * name the alx code uses exists, so the compilers check the alx code itself:
  * every peripheral's registers are one block holding every register the code
  * names, every handle one shape holding every member, every init structure
  * one other, the struct-valued members of those a third; a call or a
  * function-like macro of the HAL and LL forwards its arguments to one variadic
  * function, so they are evaluated and typed as written and nothing more is
  * asked of them. A constant has a value no other constant has. Whether a call
  * matches the vendor's prototype is the real SDK's question: the device repos
  * build against theirs.
  *
  * Generated from the alx code by the scratch tool of ALX-1564 night goal 4;
  * edited by hand from then on.
  ******************************************************************************
  **/

#ifndef ALX_SHIM_STM32_STM32HOSTSHIM_H
#define ALX_SHIM_STM32_STM32HOSTSHIM_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>


//******************************************************************************
// Interrupt numbers - the device header defines them before the core
//******************************************************************************
typedef enum
{
	NonMaskableInt_IRQn = -14,
	HardFault_IRQn = -13,
	SVCall_IRQn = -5,
	PendSV_IRQn = -2,
	SysTick_IRQn = -1,
	CAN1_RX0_IRQn = 0,
	CAN1_TX_IRQn = 1,
	CAN2_RX0_IRQn = 2,
	CAN2_TX_IRQn = 3,
	CAN3_RX0_IRQn = 4,
	CAN3_TX_IRQn = 5,
	EXTI0_IRQn = 6,
	EXTI10_IRQn = 7,
	EXTI11_IRQn = 8,
	EXTI12_IRQn = 9,
	EXTI13_IRQn = 10,
	EXTI14_IRQn = 11,
	EXTI15_10_IRQn = 12,
	EXTI15_IRQn = 13,
	EXTI1_IRQn = 14,
	EXTI2_IRQn = 15,
	EXTI3_IRQn = 16,
	EXTI4_IRQn = 17,
	EXTI5_IRQn = 18,
	EXTI6_IRQn = 19,
	EXTI7_IRQn = 20,
	EXTI8_IRQn = 21,
	EXTI9_5_IRQn = 22,
	EXTI9_IRQn = 23,
	FDCAN1_IT0_IRQn = 24,
	FDCAN2_IT0_IRQn = 25,
	FDCAN3_IT0_IRQn = 26,
	LPUART1_IRQn = 27,
	LPUART2_IRQn = 28,
	OTG_FS_IRQn = 29,
	OTG_HS_IRQn = 30,
	SAI1_IRQn = 31,
	SAI2_IRQn = 32,
	SDMMC1_IRQn = 33,
	SDMMC2_IRQn = 34,
	UART10_IRQn = 35,
	UART1_IRQn = 36,
	UART2_IRQn = 37,
	UART3_IRQn = 38,
	UART4_IRQn = 39,
	UART5_IRQn = 40,
	UART6_IRQn = 41,
	UART7_IRQn = 42,
	UART8_IRQn = 43,
	UART9_IRQn = 44,
	USART10_IRQn = 45,
	USART1_IRQn = 46,
	USART2_IRQn = 47,
	USART3_6_IRQn = 48,
	USART3_8_IRQn = 49,
	USART3_IRQn = 50,
	USART4_5_IRQn = 51,
	USART6_IRQn = 52,
	USART7_IRQn = 53,
	USART8_IRQn = 54,
	USART9_IRQn = 55
} IRQn_Type;
#define ALX_SHIM_IRQN_TYPE
#include "cmsisHostShim.h"


//******************************************************************************
// Enumerations and scalar types
//******************************************************************************
typedef enum
{
	HAL_OK = 0x00U,
	HAL_ERROR = 0x01U,
	HAL_BUSY = 0x02U,
	HAL_TIMEOUT = 0x03U
} HAL_StatusTypeDef;
typedef enum
{
	DISABLE = 0U,
	ENABLE = 1U
} FunctionalState;
typedef enum
{
	RESET = 0U,
	SET = 1U
} FlagStatus;
typedef enum
{
	GPIO_PIN_RESET = 0U,
	GPIO_PIN_SET = 1U
} GPIO_PinState;
typedef enum
{
	HAL_UNLOCKED = 0x00U,
	HAL_LOCKED = 0x01U
} HAL_LockTypeDef;
typedef uint32_t HAL_MMC_CardStateTypeDef;
typedef FlagStatus ITStatus;


//******************************************************************************
// The shapes: registers, the struct-valued members, init structures, handles
//******************************************************************************
typedef struct AlxShimStm32_Regs
{
	volatile uint32_t AHBCLKDivider;
	volatile uint32_t APB1CLKDivider;
	volatile uint32_t APB2CLKDivider;
	volatile uint32_t APB3CLKDivider;
	volatile uint32_t AdcClockSelection;
	volatile uint32_t AddressingMode;
	volatile uint32_t AdvFeatureInit;
	volatile uint32_t Alternate;
	volatile uint32_t AsynchPrediv;
	volatile uint32_t AudioFrequency;
	volatile uint32_t AudioMode;
	volatile uint32_t AutoBusOff;
	volatile uint32_t AutoReloadPreload;
	volatile uint32_t AutoRetransmission;
	volatile uint32_t AutoWakeUp;
	volatile uint32_t Banks;
	volatile uint32_t BaudRate;
	volatile uint32_t BaudRatePrescaler;
	volatile uint32_t BitRateSwitch;
	volatile uint32_t BlkHWRequest;
	volatile uint32_t BlockNbr;
	volatile uint32_t BlockSize;
	volatile uint32_t BusOff;
	volatile uint32_t BusWide;
	volatile uint32_t CLKPhase;
	volatile uint32_t CLKPolarity;
	volatile uint32_t CRCCalculation;
	volatile uint32_t CRCLength;
	volatile uint32_t CRCPolynomial;
	volatile uint32_t CSR;
	volatile uint32_t Channel;
	volatile uint32_t ClockDiv;
	volatile uint32_t ClockDivider;
	volatile uint32_t ClockDivision;
	volatile uint32_t ClockEdge;
	volatile uint32_t ClockPowerSave;
	volatile uint32_t ClockPrescaler;
	volatile uint32_t ClockSource;
	volatile uint32_t ClockSpeed;
	volatile uint32_t ClockStrobing;
	volatile uint32_t ClockType;
	volatile uint32_t CompandingMode;
	volatile uint32_t ContinuousConvMode;
	volatile uint32_t ConversionDataManagement;
	volatile uint32_t CounterMode;
	volatile uint32_t DAC_ConnectOnChipPeripheral;
	volatile uint32_t DAC_HighFrequency;
	volatile uint32_t DAC_HoldTime;
	volatile uint32_t DAC_OutputBuffer;
	volatile uint32_t DAC_RefreshTime;
	volatile uint32_t DAC_SampleAndHold;
	volatile uint32_t DAC_SampleTime;
	volatile uint32_t DAC_Trigger;
	volatile uint32_t DAC_TrimmingValue;
	volatile uint32_t DAC_UserTrimming;
	volatile uint32_t DFSDMConfig;
	volatile uint32_t DLC;
	volatile uint32_t DMAContinuousRequests;
	volatile uint32_t DR;
	volatile uint32_t DataAlign;
	volatile uint32_t DataAlignment;
	volatile uint32_t DataExchange;
	volatile uint32_t DataLength;
	volatile uint32_t DataPrescaler;
	volatile uint32_t DataSize;
	volatile uint32_t DataSyncJumpWidth;
	volatile uint32_t DataTimeSeg1;
	volatile uint32_t DataTimeSeg2;
	volatile uint8_t Date;
	volatile uint32_t DayLightSaving;
	volatile uint32_t DestBurstLength;
	volatile uint32_t DestDataWidth;
	volatile uint32_t DestInc;
	volatile uint32_t Direction;
	volatile uint32_t DiscontinuousConvMode;
	volatile uint32_t DstAddress;
	volatile uint32_t DualAddressMode;
	volatile uint32_t DutyCycle;
	volatile uint32_t EOCSelection;
	volatile uint32_t EWI;
	volatile uint32_t ErrorStateIndicator;
	volatile uint32_t ExtFiltersNbr;
	volatile uint32_t ExtId;
	volatile uint32_t ExternalTrigConv;
	volatile uint32_t ExternalTrigConvEdge;
	volatile uint32_t FDFormat;
	volatile uint32_t FIFOMode;
	volatile uint32_t FIFOThreshold;
	volatile uint32_t FdcanClockSelection;
	volatile uint32_t FilterActivation;
	volatile uint32_t FilterBank;
	volatile uint32_t FilterConfig;
	volatile uint32_t FilterFIFOAssignment;
	volatile uint32_t FilterID1;
	volatile uint32_t FilterID2;
	volatile uint32_t FilterIdHigh;
	volatile uint32_t FilterIdLow;
	volatile uint32_t FilterIndex;
	volatile uint32_t FilterMaskIdHigh;
	volatile uint32_t FilterMaskIdLow;
	volatile uint32_t FilterMode;
	volatile uint32_t FilterScale;
	volatile uint32_t FilterType;
	volatile uint32_t FirstBit;
	volatile uint32_t FrameFormat;
	volatile uint32_t GainCompensation;
	volatile uint32_t GeneralCallMode;
	volatile uint32_t HSEPredivValue;
	volatile uint32_t HSEState;
	volatile uint32_t HSI14CalibrationValue;
	volatile uint32_t HSI14State;
	volatile uint32_t HSI48State;
	volatile uint32_t HSICalibrationValue;
	volatile uint32_t HSIState;
	volatile uint32_t HardwareFlowControl;
	volatile uint32_t Host_channels;
	volatile uint32_t HourFormat;
	volatile uint8_t Hours;
	volatile uint32_t HwFlowCtl;
	volatile uint32_t IDE;
	volatile uint32_t IdType;
	volatile uint32_t Identifier;
	volatile uint32_t LSEState;
	volatile uint32_t LSIDiv;
	volatile uint32_t LSIState;
	volatile uint32_t LeftBitShift;
	volatile uint32_t LinkAllocatedPort;
	volatile uint32_t LinkStepMode;
	volatile uint32_t LinkedListMode;
	volatile uint32_t LowPowerAutoPowerOff;
	volatile uint32_t LowPowerAutoWait;
	volatile uint32_t LowPowerFrequencyMode;
	volatile uint32_t MSICalibrationValue;
	volatile uint32_t MSIClockRange;
	volatile uint32_t MSIKClockRange;
	volatile uint32_t MSIKState;
	volatile uint32_t MSIState;
	volatile uint32_t Mckdiv;
	volatile uint32_t MemBurst;
	volatile uint32_t MemDataAlignment;
	volatile uint32_t MemInc;
	volatile uint32_t MessageMarker;
	volatile uint8_t Minutes;
	volatile uint32_t Mode;
	volatile uint32_t MonoStereoMode;
	volatile uint8_t Month;
	volatile uint32_t NSS;
	volatile uint32_t NSSPMode;
	volatile uint32_t NbPages;
	volatile uint32_t NbSectors;
	volatile uint32_t NbrOfConversion;
	volatile uint32_t NbrOfDiscConversion;
	volatile uint32_t NoDivider;
	volatile uint32_t NoStretchMode;
	volatile uint32_t NodeType;
	volatile uint32_t NominalPrescaler;
	volatile uint32_t NominalSyncJumpWidth;
	volatile uint32_t NominalTimeSeg1;
	volatile uint32_t NominalTimeSeg2;
	volatile uint32_t OCFastMode;
	volatile uint32_t OCIdleState;
	volatile uint32_t OCMode;
	volatile uint32_t OCNIdleState;
	volatile uint32_t OCNPolarity;
	volatile uint32_t OCPolarity;
	volatile uint32_t Offset;
	volatile uint32_t OffsetNumber;
	volatile uint32_t OffsetRightShift;
	volatile uint32_t OffsetSaturation;
	volatile uint32_t OffsetSign;
	volatile uint32_t OffsetSignedSaturation;
	volatile uint32_t OneBitSampling;
	volatile uint32_t OscillatorType;
	volatile uint32_t OutPut;
	volatile uint32_t OutPutPolarity;
	volatile uint32_t OutPutType;
	volatile uint32_t OutputDrive;
	volatile uint32_t OverSampling;
	volatile uint32_t Overrun;
	volatile uint32_t OverrunDisable;
	volatile uint32_t OversamplingMode;
	volatile uint32_t OversamplingStopReset;
	volatile uint32_t OwnAddress1;
	volatile uint32_t OwnAddress2;
	volatile uint32_t OwnAddress2Masks;
	volatile uint32_t PLLDIV;
	volatile uint32_t PLLFRACN;
	volatile uint32_t PLLI2SDivQ;
	volatile uint32_t PLLI2SN;
	volatile uint32_t PLLI2SQ;
	volatile uint32_t PLLI2SR;
	volatile uint32_t PLLM;
	volatile uint32_t PLLMBOOST;
	volatile uint32_t PLLMUL;
	volatile uint32_t PLLN;
	volatile uint32_t PLLP;
	volatile uint32_t PLLQ;
	volatile uint32_t PLLR;
	volatile uint32_t PLLRGE;
	volatile uint32_t PLLSAIDivQ;
	volatile uint32_t PLLSAIDivR;
	volatile uint32_t PLLSAIN;
	volatile uint32_t PLLSAIQ;
	volatile uint32_t PLLSAIR;
	volatile uint32_t PLLSource;
	volatile uint32_t PLLState;
	volatile uint32_t PREDIV;
	volatile uint32_t PRER;
	volatile uint32_t Page;
	volatile uint32_t Parity;
	volatile uint32_t Period;
	volatile uint32_t PeriphBurst;
	volatile uint32_t PeriphClockSelection;
	volatile uint32_t PeriphDataAlignment;
	volatile uint32_t PeriphInc;
	volatile uint32_t Pin;
	volatile uint32_t Prescaler;
	volatile uint32_t Priority;
	volatile uint32_t Protocol;
	volatile uint32_t ProtocolException;
	volatile uint32_t Pull;
	volatile uint32_t Pulse;
	volatile uint32_t RTCClockSelection;
	volatile uint32_t RTR;
	volatile uint32_t Rank;
	volatile uint32_t Ratio;
	volatile uint32_t ReceiveFifoLocked;
	volatile uint32_t Reload;
	volatile uint32_t RepetitionCounter;
	volatile uint32_t Request;
	volatile uint32_t Resolution;
	volatile uint32_t RightBitShift;
	volatile uint32_t RxFrameType;
	volatile uint32_t SHSIState;
	volatile uint32_t SR;
	volatile uint32_t SSR;
	volatile uint32_t SYSCLKSource;
	volatile uint32_t SamplingMode;
	volatile uint32_t SamplingTime;
	volatile uint32_t SamplingTimeCommon;
	volatile uint32_t SamplingTimeCommon1;
	volatile uint32_t SamplingTimeCommon2;
	volatile uint32_t ScanConvMode;
	volatile uint32_t Sdmmc1ClockSelection;
	volatile uint32_t SecondFraction;
	volatile uint8_t Seconds;
	volatile uint32_t Sector;
	volatile uint32_t SingleDiff;
	volatile uint32_t SlaveStartFilterBank;
	volatile uint32_t Sof_enable;
	volatile uint32_t Speed;
	volatile uint32_t SrcAddress;
	volatile uint32_t SrcBurstLength;
	volatile uint32_t SrcDataWidth;
	volatile uint32_t SrcInc;
	volatile uint32_t StdFiltersNbr;
	volatile uint32_t StdId;
	volatile uint32_t StopBits;
	volatile uint32_t StoreOperation;
	volatile uint32_t SubSeconds;
	volatile uint32_t SyncJumpWidth;
	volatile uint32_t SynchPrediv;
	volatile uint32_t Synchro;
	volatile uint32_t SynchroExt;
	volatile uint32_t TIMPresSelection;
	volatile uint32_t TIMode;
	volatile uint8_t TimeFormat;
	volatile uint32_t TimeSeg1;
	volatile uint32_t TimeSeg2;
	volatile uint32_t TimeTriggeredMode;
	volatile uint32_t Timing;
	volatile uint32_t Transceiver;
	volatile uint32_t TransferAllocatedPort;
	volatile uint32_t TransferEventMode;
	volatile uint32_t TransmitFifoPriority;
	volatile FunctionalState TransmitGlobalTime;
	volatile uint32_t TransmitPause;
	volatile uint32_t TriState;
	volatile uint32_t TriggerFrequencyMode;
	volatile uint32_t TriggerMode;
	volatile uint32_t TriggerPolarity;
	volatile uint32_t TriggerSelection;
	volatile uint32_t TriggeredMode;
	volatile uint32_t TxEventFifoControl;
	volatile uint32_t TxFifoQueueMode;
	volatile uint32_t TxFrameType;
	volatile uint32_t TypeErase;
	volatile uint32_t UsbClockSelection;
	volatile uint32_t VoltageRange;
	volatile uint32_t VrefProtection;
	volatile uint8_t WeekDay;
	volatile uint32_t Window;
	volatile uint32_t WordLength;
	volatile uint8_t Year;
	volatile uint32_t battery_charging_enable;
	volatile uint32_t dev_endpoints;
	volatile uint32_t dma_enable;
	volatile uint32_t ep0_mps;
	volatile uint8_t ep_is_in;
	volatile uint32_t low_power_enable;
	volatile uint32_t lpm_enable;
	volatile uint32_t phy_itface;
	volatile uint32_t speed;
	volatile uint8_t toggle_in;
	volatile uint8_t toggle_out;
	volatile uint32_t use_dedicated_ep1;
	volatile uint32_t use_external_vbus;
	volatile uint32_t vbus_sensing_enable;
} AlxShimStm32_Regs;

typedef struct AlxShimStm32_Sub
{
	uint32_t AHBCLKDivider;
	uint32_t APB1CLKDivider;
	uint32_t APB2CLKDivider;
	uint32_t APB3CLKDivider;
	uint32_t AdcClockSelection;
	uint32_t AddressingMode;
	uint32_t AdvFeatureInit;
	uint32_t Alternate;
	uint32_t AsynchPrediv;
	uint32_t AudioFrequency;
	uint32_t AudioMode;
	uint32_t AutoBusOff;
	uint32_t AutoReloadPreload;
	uint32_t AutoRetransmission;
	uint32_t AutoWakeUp;
	uint32_t Banks;
	uint32_t BaudRate;
	uint32_t BaudRatePrescaler;
	uint32_t BitRateSwitch;
	uint32_t BlkHWRequest;
	uint32_t BlockNbr;
	uint32_t BlockSize;
	uint32_t BusOff;
	uint32_t BusWide;
	uint32_t CLKPhase;
	uint32_t CLKPolarity;
	uint32_t CRCCalculation;
	uint32_t CRCLength;
	uint32_t CRCPolynomial;
	uint32_t CSR;
	uint32_t Channel;
	uint32_t ClockDiv;
	uint32_t ClockDivider;
	uint32_t ClockDivision;
	uint32_t ClockEdge;
	uint32_t ClockPowerSave;
	uint32_t ClockPrescaler;
	uint32_t ClockSource;
	uint32_t ClockSpeed;
	uint32_t ClockStrobing;
	uint32_t ClockType;
	uint32_t CompandingMode;
	uint32_t ContinuousConvMode;
	uint32_t ConversionDataManagement;
	uint32_t CounterMode;
	uint32_t DAC_ConnectOnChipPeripheral;
	uint32_t DAC_HighFrequency;
	uint32_t DAC_HoldTime;
	uint32_t DAC_OutputBuffer;
	uint32_t DAC_RefreshTime;
	uint32_t DAC_SampleAndHold;
	uint32_t DAC_SampleTime;
	uint32_t DAC_Trigger;
	uint32_t DAC_TrimmingValue;
	uint32_t DAC_UserTrimming;
	uint32_t DFSDMConfig;
	uint32_t DLC;
	uint32_t DMAContinuousRequests;
	uint32_t DR;
	uint32_t DataAlign;
	uint32_t DataAlignment;
	uint32_t DataExchange;
	uint32_t DataLength;
	uint32_t DataPrescaler;
	uint32_t DataSize;
	uint32_t DataSyncJumpWidth;
	uint32_t DataTimeSeg1;
	uint32_t DataTimeSeg2;
	uint8_t Date;
	uint32_t DayLightSaving;
	uint32_t DestBurstLength;
	uint32_t DestDataWidth;
	uint32_t DestInc;
	uint32_t Direction;
	uint32_t DiscontinuousConvMode;
	uint32_t DstAddress;
	uint32_t DualAddressMode;
	uint32_t DutyCycle;
	uint32_t EOCSelection;
	uint32_t EWI;
	uint32_t ErrorStateIndicator;
	uint32_t ExtFiltersNbr;
	uint32_t ExtId;
	uint32_t ExternalTrigConv;
	uint32_t ExternalTrigConvEdge;
	uint32_t FDFormat;
	uint32_t FIFOMode;
	uint32_t FIFOThreshold;
	uint32_t FdcanClockSelection;
	uint32_t FilterActivation;
	uint32_t FilterBank;
	uint32_t FilterConfig;
	uint32_t FilterFIFOAssignment;
	uint32_t FilterID1;
	uint32_t FilterID2;
	uint32_t FilterIdHigh;
	uint32_t FilterIdLow;
	uint32_t FilterIndex;
	uint32_t FilterMaskIdHigh;
	uint32_t FilterMaskIdLow;
	uint32_t FilterMode;
	uint32_t FilterScale;
	uint32_t FilterType;
	uint32_t FirstBit;
	uint32_t FrameFormat;
	uint32_t GainCompensation;
	uint32_t GeneralCallMode;
	uint32_t HSEPredivValue;
	uint32_t HSEState;
	uint32_t HSI14CalibrationValue;
	uint32_t HSI14State;
	uint32_t HSI48State;
	uint32_t HSICalibrationValue;
	uint32_t HSIState;
	uint32_t HardwareFlowControl;
	uint32_t Host_channels;
	uint32_t HourFormat;
	uint8_t Hours;
	uint32_t HwFlowCtl;
	uint32_t IDE;
	uint32_t IdType;
	uint32_t Identifier;
	uint32_t LSEState;
	uint32_t LSIDiv;
	uint32_t LSIState;
	uint32_t LeftBitShift;
	uint32_t LinkAllocatedPort;
	uint32_t LinkStepMode;
	uint32_t LinkedListMode;
	uint32_t LowPowerAutoPowerOff;
	uint32_t LowPowerAutoWait;
	uint32_t LowPowerFrequencyMode;
	uint32_t MSICalibrationValue;
	uint32_t MSIClockRange;
	uint32_t MSIKClockRange;
	uint32_t MSIKState;
	uint32_t MSIState;
	uint32_t Mckdiv;
	uint32_t MemBurst;
	uint32_t MemDataAlignment;
	uint32_t MemInc;
	uint32_t MessageMarker;
	uint8_t Minutes;
	uint32_t Mode;
	uint32_t MonoStereoMode;
	uint8_t Month;
	uint32_t NSS;
	uint32_t NSSPMode;
	uint32_t NbPages;
	uint32_t NbSectors;
	uint32_t NbrOfConversion;
	uint32_t NbrOfDiscConversion;
	uint32_t NoDivider;
	uint32_t NoStretchMode;
	uint32_t NodeType;
	uint32_t NominalPrescaler;
	uint32_t NominalSyncJumpWidth;
	uint32_t NominalTimeSeg1;
	uint32_t NominalTimeSeg2;
	uint32_t OCFastMode;
	uint32_t OCIdleState;
	uint32_t OCMode;
	uint32_t OCNIdleState;
	uint32_t OCNPolarity;
	uint32_t OCPolarity;
	uint32_t Offset;
	uint32_t OffsetNumber;
	uint32_t OffsetRightShift;
	uint32_t OffsetSaturation;
	uint32_t OffsetSign;
	uint32_t OffsetSignedSaturation;
	uint32_t OneBitSampling;
	uint32_t OscillatorType;
	uint32_t OutPut;
	uint32_t OutPutPolarity;
	uint32_t OutPutType;
	uint32_t OutputDrive;
	uint32_t OverSampling;
	uint32_t Overrun;
	uint32_t OverrunDisable;
	uint32_t OversamplingMode;
	uint32_t OversamplingStopReset;
	uint32_t OwnAddress1;
	uint32_t OwnAddress2;
	uint32_t OwnAddress2Masks;
	uint32_t PLLDIV;
	uint32_t PLLFRACN;
	uint32_t PLLI2SDivQ;
	uint32_t PLLI2SN;
	uint32_t PLLI2SQ;
	uint32_t PLLI2SR;
	uint32_t PLLM;
	uint32_t PLLMBOOST;
	uint32_t PLLMUL;
	uint32_t PLLN;
	uint32_t PLLP;
	uint32_t PLLQ;
	uint32_t PLLR;
	uint32_t PLLRGE;
	uint32_t PLLSAIDivQ;
	uint32_t PLLSAIDivR;
	uint32_t PLLSAIN;
	uint32_t PLLSAIQ;
	uint32_t PLLSAIR;
	uint32_t PLLSource;
	uint32_t PLLState;
	uint32_t PREDIV;
	uint32_t PRER;
	uint32_t Page;
	uint32_t Parity;
	uint32_t Period;
	uint32_t PeriphBurst;
	uint32_t PeriphClockSelection;
	uint32_t PeriphDataAlignment;
	uint32_t PeriphInc;
	uint32_t Pin;
	uint32_t Prescaler;
	uint32_t Priority;
	uint32_t Protocol;
	uint32_t ProtocolException;
	uint32_t Pull;
	uint32_t Pulse;
	uint32_t RTCClockSelection;
	uint32_t RTR;
	uint32_t Rank;
	uint32_t Ratio;
	uint32_t ReceiveFifoLocked;
	uint32_t Reload;
	uint32_t RepetitionCounter;
	uint32_t Request;
	uint32_t Resolution;
	uint32_t RightBitShift;
	uint32_t RxFrameType;
	uint32_t SHSIState;
	uint32_t SR;
	uint32_t SSR;
	uint32_t SYSCLKSource;
	uint32_t SamplingMode;
	uint32_t SamplingTime;
	uint32_t SamplingTimeCommon;
	uint32_t SamplingTimeCommon1;
	uint32_t SamplingTimeCommon2;
	uint32_t ScanConvMode;
	uint32_t Sdmmc1ClockSelection;
	uint32_t SecondFraction;
	uint8_t Seconds;
	uint32_t Sector;
	uint32_t SingleDiff;
	uint32_t SlaveStartFilterBank;
	uint32_t Sof_enable;
	uint32_t Speed;
	uint32_t SrcAddress;
	uint32_t SrcBurstLength;
	uint32_t SrcDataWidth;
	uint32_t SrcInc;
	uint32_t StdFiltersNbr;
	uint32_t StdId;
	uint32_t StopBits;
	uint32_t StoreOperation;
	uint32_t SubSeconds;
	uint32_t SyncJumpWidth;
	uint32_t SynchPrediv;
	uint32_t Synchro;
	uint32_t SynchroExt;
	uint32_t TIMPresSelection;
	uint32_t TIMode;
	uint8_t TimeFormat;
	uint32_t TimeSeg1;
	uint32_t TimeSeg2;
	uint32_t TimeTriggeredMode;
	uint32_t Timing;
	uint32_t Transceiver;
	uint32_t TransferAllocatedPort;
	uint32_t TransferEventMode;
	uint32_t TransmitFifoPriority;
	FunctionalState TransmitGlobalTime;
	uint32_t TransmitPause;
	uint32_t TriState;
	uint32_t TriggerFrequencyMode;
	uint32_t TriggerMode;
	uint32_t TriggerPolarity;
	uint32_t TriggerSelection;
	uint32_t TriggeredMode;
	uint32_t TxEventFifoControl;
	uint32_t TxFifoQueueMode;
	uint32_t TxFrameType;
	uint32_t TypeErase;
	uint32_t UsbClockSelection;
	uint32_t VoltageRange;
	uint32_t VrefProtection;
	uint8_t WeekDay;
	uint32_t Window;
	uint32_t WordLength;
	uint8_t Year;
	uint32_t battery_charging_enable;
	uint32_t dev_endpoints;
	uint32_t dma_enable;
	uint32_t ep0_mps;
	uint8_t ep_is_in;
	uint32_t low_power_enable;
	uint32_t lpm_enable;
	uint32_t phy_itface;
	uint32_t speed;
	uint8_t toggle_in;
	uint8_t toggle_out;
	uint32_t use_dedicated_ep1;
	uint32_t use_external_vbus;
	uint32_t vbus_sensing_enable;
} AlxShimStm32_Sub;

typedef struct AlxShimStm32_Init
{
	uint32_t AHBCLKDivider;
	uint32_t APB1CLKDivider;
	uint32_t APB2CLKDivider;
	uint32_t APB3CLKDivider;
	uint32_t AdcClockSelection;
	uint32_t AddressingMode;
	uint32_t AdvFeatureInit;
	uint32_t Alternate;
	uint32_t AsynchPrediv;
	uint32_t AudioFrequency;
	uint32_t AudioMode;
	uint32_t AutoBusOff;
	uint32_t AutoReloadPreload;
	uint32_t AutoRetransmission;
	uint32_t AutoWakeUp;
	uint32_t Banks;
	uint32_t BaudRate;
	uint32_t BaudRatePrescaler;
	uint32_t BitRateSwitch;
	uint32_t BlkHWRequest;
	uint32_t BlockNbr;
	uint32_t BlockSize;
	uint32_t BusOff;
	uint32_t BusWide;
	uint32_t CLKPhase;
	uint32_t CLKPolarity;
	uint32_t CRCCalculation;
	uint32_t CRCLength;
	uint32_t CRCPolynomial;
	uint32_t CSR;
	uint32_t Channel;
	uint32_t ClockDiv;
	uint32_t ClockDivider;
	uint32_t ClockDivision;
	uint32_t ClockEdge;
	uint32_t ClockPowerSave;
	uint32_t ClockPrescaler;
	uint32_t ClockSource;
	uint32_t ClockSpeed;
	uint32_t ClockStrobing;
	uint32_t ClockType;
	uint32_t CompandingMode;
	uint32_t ContinuousConvMode;
	uint32_t ConversionDataManagement;
	uint32_t CounterMode;
	uint32_t DAC_ConnectOnChipPeripheral;
	uint32_t DAC_HighFrequency;
	uint32_t DAC_HoldTime;
	uint32_t DAC_OutputBuffer;
	uint32_t DAC_RefreshTime;
	uint32_t DAC_SampleAndHold;
	uint32_t DAC_SampleTime;
	uint32_t DAC_Trigger;
	uint32_t DAC_TrimmingValue;
	uint32_t DAC_UserTrimming;
	uint32_t DFSDMConfig;
	uint32_t DLC;
	uint32_t DMAContinuousRequests;
	uint32_t DR;
	uint32_t DataAlign;
	uint32_t DataAlignment;
	uint32_t DataExchange;
	uint32_t DataLength;
	uint32_t DataPrescaler;
	uint32_t DataSize;
	uint32_t DataSyncJumpWidth;
	uint32_t DataTimeSeg1;
	uint32_t DataTimeSeg2;
	uint8_t Date;
	uint32_t DayLightSaving;
	uint32_t DestBurstLength;
	uint32_t DestDataWidth;
	uint32_t DestInc;
	uint32_t Direction;
	uint32_t DiscontinuousConvMode;
	uint32_t DstAddress;
	uint32_t DualAddressMode;
	uint32_t DutyCycle;
	uint32_t EOCSelection;
	uint32_t EWI;
	uint32_t ErrorStateIndicator;
	uint32_t ExtFiltersNbr;
	uint32_t ExtId;
	uint32_t ExternalTrigConv;
	uint32_t ExternalTrigConvEdge;
	uint32_t FDFormat;
	uint32_t FIFOMode;
	uint32_t FIFOThreshold;
	uint32_t FdcanClockSelection;
	uint32_t FilterActivation;
	uint32_t FilterBank;
	uint32_t FilterConfig;
	uint32_t FilterFIFOAssignment;
	uint32_t FilterID1;
	uint32_t FilterID2;
	uint32_t FilterIdHigh;
	uint32_t FilterIdLow;
	uint32_t FilterIndex;
	uint32_t FilterMaskIdHigh;
	uint32_t FilterMaskIdLow;
	uint32_t FilterMode;
	uint32_t FilterScale;
	uint32_t FilterType;
	uint32_t FirstBit;
	uint32_t FrameFormat;
	uint32_t GainCompensation;
	uint32_t GeneralCallMode;
	uint32_t HSEPredivValue;
	uint32_t HSEState;
	uint32_t HSI14CalibrationValue;
	uint32_t HSI14State;
	uint32_t HSI48State;
	uint32_t HSICalibrationValue;
	uint32_t HSIState;
	uint32_t HardwareFlowControl;
	uint32_t Host_channels;
	uint32_t HourFormat;
	uint8_t Hours;
	uint32_t HwFlowCtl;
	uint32_t IDE;
	uint32_t IdType;
	uint32_t Identifier;
	uint32_t LSEState;
	uint32_t LSIDiv;
	uint32_t LSIState;
	uint32_t LeftBitShift;
	uint32_t LinkAllocatedPort;
	uint32_t LinkStepMode;
	uint32_t LinkedListMode;
	uint32_t LowPowerAutoPowerOff;
	uint32_t LowPowerAutoWait;
	uint32_t LowPowerFrequencyMode;
	uint32_t MSICalibrationValue;
	uint32_t MSIClockRange;
	uint32_t MSIKClockRange;
	uint32_t MSIKState;
	uint32_t MSIState;
	uint32_t Mckdiv;
	uint32_t MemBurst;
	uint32_t MemDataAlignment;
	uint32_t MemInc;
	uint32_t MessageMarker;
	uint8_t Minutes;
	uint32_t Mode;
	uint32_t MonoStereoMode;
	uint8_t Month;
	uint32_t NSS;
	uint32_t NSSPMode;
	uint32_t NbPages;
	uint32_t NbSectors;
	uint32_t NbrOfConversion;
	uint32_t NbrOfDiscConversion;
	uint32_t NoDivider;
	uint32_t NoStretchMode;
	uint32_t NodeType;
	uint32_t NominalPrescaler;
	uint32_t NominalSyncJumpWidth;
	uint32_t NominalTimeSeg1;
	uint32_t NominalTimeSeg2;
	uint32_t OCFastMode;
	uint32_t OCIdleState;
	uint32_t OCMode;
	uint32_t OCNIdleState;
	uint32_t OCNPolarity;
	uint32_t OCPolarity;
	uint32_t Offset;
	uint32_t OffsetNumber;
	uint32_t OffsetRightShift;
	uint32_t OffsetSaturation;
	uint32_t OffsetSign;
	uint32_t OffsetSignedSaturation;
	uint32_t OneBitSampling;
	uint32_t OscillatorType;
	uint32_t OutPut;
	uint32_t OutPutPolarity;
	uint32_t OutPutType;
	uint32_t OutputDrive;
	uint32_t OverSampling;
	uint32_t Overrun;
	uint32_t OverrunDisable;
	uint32_t OversamplingMode;
	uint32_t OversamplingStopReset;
	uint32_t OwnAddress1;
	uint32_t OwnAddress2;
	uint32_t OwnAddress2Masks;
	uint32_t PLLDIV;
	uint32_t PLLFRACN;
	uint32_t PLLI2SDivQ;
	uint32_t PLLI2SN;
	uint32_t PLLI2SQ;
	uint32_t PLLI2SR;
	uint32_t PLLM;
	uint32_t PLLMBOOST;
	uint32_t PLLMUL;
	uint32_t PLLN;
	uint32_t PLLP;
	uint32_t PLLQ;
	uint32_t PLLR;
	uint32_t PLLRGE;
	uint32_t PLLSAIDivQ;
	uint32_t PLLSAIDivR;
	uint32_t PLLSAIN;
	uint32_t PLLSAIQ;
	uint32_t PLLSAIR;
	uint32_t PLLSource;
	uint32_t PLLState;
	uint32_t PREDIV;
	uint32_t PRER;
	uint32_t Page;
	uint32_t Parity;
	uint32_t Period;
	uint32_t PeriphBurst;
	uint32_t PeriphClockSelection;
	uint32_t PeriphDataAlignment;
	uint32_t PeriphInc;
	uint32_t Pin;
	uint32_t Prescaler;
	uint32_t Priority;
	uint32_t Protocol;
	uint32_t ProtocolException;
	uint32_t Pull;
	uint32_t Pulse;
	uint32_t RTCClockSelection;
	uint32_t RTR;
	uint32_t Rank;
	uint32_t Ratio;
	uint32_t ReceiveFifoLocked;
	uint32_t Reload;
	uint32_t RepetitionCounter;
	uint32_t Request;
	uint32_t Resolution;
	uint32_t RightBitShift;
	uint32_t RxFrameType;
	uint32_t SHSIState;
	uint32_t SR;
	uint32_t SSR;
	uint32_t SYSCLKSource;
	uint32_t SamplingMode;
	uint32_t SamplingTime;
	uint32_t SamplingTimeCommon;
	uint32_t SamplingTimeCommon1;
	uint32_t SamplingTimeCommon2;
	uint32_t ScanConvMode;
	uint32_t Sdmmc1ClockSelection;
	uint32_t SecondFraction;
	uint8_t Seconds;
	uint32_t Sector;
	uint32_t SingleDiff;
	uint32_t SlaveStartFilterBank;
	uint32_t Sof_enable;
	uint32_t Speed;
	uint32_t SrcAddress;
	uint32_t SrcBurstLength;
	uint32_t SrcDataWidth;
	uint32_t SrcInc;
	uint32_t StdFiltersNbr;
	uint32_t StdId;
	uint32_t StopBits;
	uint32_t StoreOperation;
	uint32_t SubSeconds;
	uint32_t SyncJumpWidth;
	uint32_t SynchPrediv;
	uint32_t Synchro;
	uint32_t SynchroExt;
	uint32_t TIMPresSelection;
	uint32_t TIMode;
	uint8_t TimeFormat;
	uint32_t TimeSeg1;
	uint32_t TimeSeg2;
	uint32_t TimeTriggeredMode;
	uint32_t Timing;
	uint32_t Transceiver;
	uint32_t TransferAllocatedPort;
	uint32_t TransferEventMode;
	uint32_t TransmitFifoPriority;
	FunctionalState TransmitGlobalTime;
	uint32_t TransmitPause;
	uint32_t TriState;
	uint32_t TriggerFrequencyMode;
	uint32_t TriggerMode;
	uint32_t TriggerPolarity;
	uint32_t TriggerSelection;
	uint32_t TriggeredMode;
	uint32_t TxEventFifoControl;
	uint32_t TxFifoQueueMode;
	uint32_t TxFrameType;
	uint32_t TypeErase;
	uint32_t UsbClockSelection;
	uint32_t VoltageRange;
	uint32_t VrefProtection;
	uint8_t WeekDay;
	uint32_t Window;
	uint32_t WordLength;
	uint8_t Year;
	uint32_t battery_charging_enable;
	uint32_t dev_endpoints;
	uint32_t dma_enable;
	uint32_t ep0_mps;
	uint8_t ep_is_in;
	uint32_t low_power_enable;
	uint32_t lpm_enable;
	uint32_t phy_itface;
	uint32_t speed;
	uint8_t toggle_in;
	uint8_t toggle_out;
	uint32_t use_dedicated_ep1;
	uint32_t use_external_vbus;
	uint32_t vbus_sensing_enable;
	AlxShimStm32_Sub AdvancedInit;
	AlxShimStm32_Sub DAC_SampleAndHoldConfig;
	AlxShimStm32_Sub DataHandlingConfig;
	AlxShimStm32_Sub Init;
	AlxShimStm32_Sub InitLinkedList;
	AlxShimStm32_Sub Oversample;
	AlxShimStm32_Sub Oversampling;
	AlxShimStm32_Sub PLL;
	AlxShimStm32_Sub PLLI2S;
	AlxShimStm32_Sub PLLSAI;
	AlxShimStm32_Sub RepeatBlockConfig;
	AlxShimStm32_Sub TriggerConfig;
} AlxShimStm32_Init;

typedef struct AlxShimStm32_Handle
{
	uint32_t AHBCLKDivider;
	uint32_t APB1CLKDivider;
	uint32_t APB2CLKDivider;
	uint32_t APB3CLKDivider;
	uint32_t AdcClockSelection;
	uint32_t AddressingMode;
	uint32_t AdvFeatureInit;
	uint32_t Alternate;
	uint32_t AsynchPrediv;
	uint32_t AudioFrequency;
	uint32_t AudioMode;
	uint32_t AutoBusOff;
	uint32_t AutoReloadPreload;
	uint32_t AutoRetransmission;
	uint32_t AutoWakeUp;
	uint32_t Banks;
	uint32_t BaudRate;
	uint32_t BaudRatePrescaler;
	uint32_t BitRateSwitch;
	uint32_t BlkHWRequest;
	uint32_t BlockNbr;
	uint32_t BlockSize;
	uint32_t BusOff;
	uint32_t BusWide;
	uint32_t CLKPhase;
	uint32_t CLKPolarity;
	uint32_t CRCCalculation;
	uint32_t CRCLength;
	uint32_t CRCPolynomial;
	uint32_t CSR;
	uint32_t Channel;
	uint32_t ClockDiv;
	uint32_t ClockDivider;
	uint32_t ClockDivision;
	uint32_t ClockEdge;
	uint32_t ClockPowerSave;
	uint32_t ClockPrescaler;
	uint32_t ClockSource;
	uint32_t ClockSpeed;
	uint32_t ClockStrobing;
	uint32_t ClockType;
	uint32_t CompandingMode;
	uint32_t ContinuousConvMode;
	uint32_t ConversionDataManagement;
	uint32_t CounterMode;
	uint32_t DAC_ConnectOnChipPeripheral;
	uint32_t DAC_HighFrequency;
	uint32_t DAC_HoldTime;
	uint32_t DAC_OutputBuffer;
	uint32_t DAC_RefreshTime;
	uint32_t DAC_SampleAndHold;
	uint32_t DAC_SampleTime;
	uint32_t DAC_Trigger;
	uint32_t DAC_TrimmingValue;
	uint32_t DAC_UserTrimming;
	uint32_t DFSDMConfig;
	uint32_t DLC;
	uint32_t DMAContinuousRequests;
	uint32_t DR;
	uint32_t DataAlign;
	uint32_t DataAlignment;
	uint32_t DataExchange;
	uint32_t DataLength;
	uint32_t DataPrescaler;
	uint32_t DataSize;
	uint32_t DataSyncJumpWidth;
	uint32_t DataTimeSeg1;
	uint32_t DataTimeSeg2;
	uint8_t Date;
	uint32_t DayLightSaving;
	uint32_t DestBurstLength;
	uint32_t DestDataWidth;
	uint32_t DestInc;
	uint32_t Direction;
	uint32_t DiscontinuousConvMode;
	uint32_t DstAddress;
	uint32_t DualAddressMode;
	uint32_t DutyCycle;
	uint32_t EOCSelection;
	uint32_t EWI;
	uint32_t ErrorStateIndicator;
	uint32_t ExtFiltersNbr;
	uint32_t ExtId;
	uint32_t ExternalTrigConv;
	uint32_t ExternalTrigConvEdge;
	uint32_t FDFormat;
	uint32_t FIFOMode;
	uint32_t FIFOThreshold;
	uint32_t FdcanClockSelection;
	uint32_t FilterActivation;
	uint32_t FilterBank;
	uint32_t FilterConfig;
	uint32_t FilterFIFOAssignment;
	uint32_t FilterID1;
	uint32_t FilterID2;
	uint32_t FilterIdHigh;
	uint32_t FilterIdLow;
	uint32_t FilterIndex;
	uint32_t FilterMaskIdHigh;
	uint32_t FilterMaskIdLow;
	uint32_t FilterMode;
	uint32_t FilterScale;
	uint32_t FilterType;
	uint32_t FirstBit;
	uint32_t FrameFormat;
	uint32_t GainCompensation;
	uint32_t GeneralCallMode;
	uint32_t HSEPredivValue;
	uint32_t HSEState;
	uint32_t HSI14CalibrationValue;
	uint32_t HSI14State;
	uint32_t HSI48State;
	uint32_t HSICalibrationValue;
	uint32_t HSIState;
	uint32_t HardwareFlowControl;
	uint32_t Host_channels;
	uint32_t HourFormat;
	uint8_t Hours;
	uint32_t HwFlowCtl;
	uint32_t IDE;
	uint32_t IdType;
	uint32_t Identifier;
	uint32_t LSEState;
	uint32_t LSIDiv;
	uint32_t LSIState;
	uint32_t LeftBitShift;
	uint32_t LinkAllocatedPort;
	uint32_t LinkStepMode;
	uint32_t LinkedListMode;
	uint32_t LowPowerAutoPowerOff;
	uint32_t LowPowerAutoWait;
	uint32_t LowPowerFrequencyMode;
	uint32_t MSICalibrationValue;
	uint32_t MSIClockRange;
	uint32_t MSIKClockRange;
	uint32_t MSIKState;
	uint32_t MSIState;
	uint32_t Mckdiv;
	uint32_t MemBurst;
	uint32_t MemDataAlignment;
	uint32_t MemInc;
	uint32_t MessageMarker;
	uint8_t Minutes;
	uint32_t Mode;
	uint32_t MonoStereoMode;
	uint8_t Month;
	uint32_t NSS;
	uint32_t NSSPMode;
	uint32_t NbPages;
	uint32_t NbSectors;
	uint32_t NbrOfConversion;
	uint32_t NbrOfDiscConversion;
	uint32_t NoDivider;
	uint32_t NoStretchMode;
	uint32_t NodeType;
	uint32_t NominalPrescaler;
	uint32_t NominalSyncJumpWidth;
	uint32_t NominalTimeSeg1;
	uint32_t NominalTimeSeg2;
	uint32_t OCFastMode;
	uint32_t OCIdleState;
	uint32_t OCMode;
	uint32_t OCNIdleState;
	uint32_t OCNPolarity;
	uint32_t OCPolarity;
	uint32_t Offset;
	uint32_t OffsetNumber;
	uint32_t OffsetRightShift;
	uint32_t OffsetSaturation;
	uint32_t OffsetSign;
	uint32_t OffsetSignedSaturation;
	uint32_t OneBitSampling;
	uint32_t OscillatorType;
	uint32_t OutPut;
	uint32_t OutPutPolarity;
	uint32_t OutPutType;
	uint32_t OutputDrive;
	uint32_t OverSampling;
	uint32_t Overrun;
	uint32_t OverrunDisable;
	uint32_t OversamplingMode;
	uint32_t OversamplingStopReset;
	uint32_t OwnAddress1;
	uint32_t OwnAddress2;
	uint32_t OwnAddress2Masks;
	uint32_t PLLDIV;
	uint32_t PLLFRACN;
	uint32_t PLLI2SDivQ;
	uint32_t PLLI2SN;
	uint32_t PLLI2SQ;
	uint32_t PLLI2SR;
	uint32_t PLLM;
	uint32_t PLLMBOOST;
	uint32_t PLLMUL;
	uint32_t PLLN;
	uint32_t PLLP;
	uint32_t PLLQ;
	uint32_t PLLR;
	uint32_t PLLRGE;
	uint32_t PLLSAIDivQ;
	uint32_t PLLSAIDivR;
	uint32_t PLLSAIN;
	uint32_t PLLSAIQ;
	uint32_t PLLSAIR;
	uint32_t PLLSource;
	uint32_t PLLState;
	uint32_t PREDIV;
	uint32_t PRER;
	uint32_t Page;
	uint32_t Parity;
	uint32_t Period;
	uint32_t PeriphBurst;
	uint32_t PeriphClockSelection;
	uint32_t PeriphDataAlignment;
	uint32_t PeriphInc;
	uint32_t Pin;
	uint32_t Prescaler;
	uint32_t Priority;
	uint32_t Protocol;
	uint32_t ProtocolException;
	uint32_t Pull;
	uint32_t Pulse;
	uint32_t RTCClockSelection;
	uint32_t RTR;
	uint32_t Rank;
	uint32_t Ratio;
	uint32_t ReceiveFifoLocked;
	uint32_t Reload;
	uint32_t RepetitionCounter;
	uint32_t Request;
	uint32_t Resolution;
	uint32_t RightBitShift;
	uint32_t RxFrameType;
	uint32_t SHSIState;
	uint32_t SR;
	uint32_t SSR;
	uint32_t SYSCLKSource;
	uint32_t SamplingMode;
	uint32_t SamplingTime;
	uint32_t SamplingTimeCommon;
	uint32_t SamplingTimeCommon1;
	uint32_t SamplingTimeCommon2;
	uint32_t ScanConvMode;
	uint32_t Sdmmc1ClockSelection;
	uint32_t SecondFraction;
	uint8_t Seconds;
	uint32_t Sector;
	uint32_t SingleDiff;
	uint32_t SlaveStartFilterBank;
	uint32_t Sof_enable;
	uint32_t Speed;
	uint32_t SrcAddress;
	uint32_t SrcBurstLength;
	uint32_t SrcDataWidth;
	uint32_t SrcInc;
	uint32_t StdFiltersNbr;
	uint32_t StdId;
	uint32_t StopBits;
	uint32_t StoreOperation;
	uint32_t SubSeconds;
	uint32_t SyncJumpWidth;
	uint32_t SynchPrediv;
	uint32_t Synchro;
	uint32_t SynchroExt;
	uint32_t TIMPresSelection;
	uint32_t TIMode;
	uint8_t TimeFormat;
	uint32_t TimeSeg1;
	uint32_t TimeSeg2;
	uint32_t TimeTriggeredMode;
	uint32_t Timing;
	uint32_t Transceiver;
	uint32_t TransferAllocatedPort;
	uint32_t TransferEventMode;
	uint32_t TransmitFifoPriority;
	FunctionalState TransmitGlobalTime;
	uint32_t TransmitPause;
	uint32_t TriState;
	uint32_t TriggerFrequencyMode;
	uint32_t TriggerMode;
	uint32_t TriggerPolarity;
	uint32_t TriggerSelection;
	uint32_t TriggeredMode;
	uint32_t TxEventFifoControl;
	uint32_t TxFifoQueueMode;
	uint32_t TxFrameType;
	uint32_t TypeErase;
	uint32_t UsbClockSelection;
	uint32_t VoltageRange;
	uint32_t VrefProtection;
	uint8_t WeekDay;
	uint32_t Window;
	uint32_t WordLength;
	uint8_t Year;
	uint32_t battery_charging_enable;
	uint32_t dev_endpoints;
	uint32_t dma_enable;
	uint32_t ep0_mps;
	uint8_t ep_is_in;
	uint32_t low_power_enable;
	uint32_t lpm_enable;
	uint32_t phy_itface;
	uint32_t speed;
	uint8_t toggle_in;
	uint8_t toggle_out;
	uint32_t use_dedicated_ep1;
	uint32_t use_external_vbus;
	uint32_t vbus_sensing_enable;
	AlxShimStm32_Init AdvancedInit;
	AlxShimStm32_Init DAC_SampleAndHoldConfig;
	AlxShimStm32_Init DataHandlingConfig;
	AlxShimStm32_Init Init;
	AlxShimStm32_Init InitLinkedList;
	AlxShimStm32_Init Oversample;
	AlxShimStm32_Init Oversampling;
	AlxShimStm32_Init PLL;
	AlxShimStm32_Init PLLI2S;
	AlxShimStm32_Init PLLSAI;
	AlxShimStm32_Init RepeatBlockConfig;
	AlxShimStm32_Init TriggerConfig;
	AlxShimStm32_Regs* Instance;
	struct AlxShimStm32_Handle* DMA_Handle;
	struct AlxShimStm32_Handle* DMA_Handle1;
	struct AlxShimStm32_Handle* DMA_Handle2;
	struct AlxShimStm32_Handle* hdmarx;
	struct AlxShimStm32_Handle* hdmatx;
	struct AlxShimStm32_Handle* hdmarx_;
	void* Parent;
	void* pData;
	AlxShimStm32_Sub hc[16];
	HAL_LockTypeDef Lock;
	volatile uint32_t State;
	volatile uint32_t ErrorCode;
} AlxShimStm32_Handle;

typedef AlxShimStm32_Init ADC_ChannelConfTypeDef;
typedef AlxShimStm32_Handle ADC_HandleTypeDef;
typedef AlxShimStm32_Regs ADC_TypeDef;
typedef AlxShimStm32_Init CAN_FilterTypeDef;
typedef AlxShimStm32_Handle CAN_HandleTypeDef;
typedef AlxShimStm32_Init CAN_RxHeaderTypeDef;
typedef AlxShimStm32_Init CAN_TxHeaderTypeDef;
typedef AlxShimStm32_Regs CAN_TypeDef;
typedef AlxShimStm32_Init DAC_ChannelConfTypeDef;
typedef AlxShimStm32_Handle DAC_HandleTypeDef;
typedef AlxShimStm32_Regs DAC_TypeDef;
typedef AlxShimStm32_Handle DMA_HandleTypeDef;
typedef AlxShimStm32_Init DMA_NodeConfTypeDef;
typedef AlxShimStm32_Init DMA_NodeTypeDef;
typedef AlxShimStm32_Init DMA_QListTypeDef;
typedef AlxShimStm32_Regs DMA_TypeDef;
typedef AlxShimStm32_Init FDCAN_FilterTypeDef;
typedef AlxShimStm32_Regs FDCAN_GlobalTypeDef;
typedef AlxShimStm32_Handle FDCAN_HandleTypeDef;
typedef AlxShimStm32_Init FDCAN_ProtocolStatusTypeDef;
typedef AlxShimStm32_Init FDCAN_RxHeaderTypeDef;
typedef AlxShimStm32_Init FDCAN_TxHeaderTypeDef;
typedef AlxShimStm32_Init FLASH_EraseInitTypeDef;
typedef AlxShimStm32_Init GPIO_InitTypeDef;
typedef AlxShimStm32_Regs GPIO_TypeDef;
typedef AlxShimStm32_Handle HCD_HandleTypeDef;
typedef AlxShimStm32_Regs HCD_TypeDef;
typedef AlxShimStm32_Handle I2C_HandleTypeDef;
typedef AlxShimStm32_Regs I2C_TypeDef;
typedef AlxShimStm32_Handle IWDG_HandleTypeDef;
typedef AlxShimStm32_Handle MMC_HandleTypeDef;
typedef AlxShimStm32_Regs MMC_TypeDef;
typedef AlxShimStm32_Init RCC_ClkInitTypeDef;
typedef AlxShimStm32_Init RCC_OscInitTypeDef;
typedef AlxShimStm32_Init RCC_PeriphCLKInitTypeDef;
typedef AlxShimStm32_Init RTC_DateTypeDef;
typedef AlxShimStm32_Handle RTC_HandleTypeDef;
typedef AlxShimStm32_Init RTC_TimeTypeDef;
typedef AlxShimStm32_Regs SAI_Block_TypeDef;
typedef AlxShimStm32_Handle SAI_HandleTypeDef;
typedef AlxShimStm32_Handle SPI_HandleTypeDef;
typedef AlxShimStm32_Regs SPI_TypeDef;
typedef AlxShimStm32_Handle TIM_HandleTypeDef;
typedef AlxShimStm32_Init TIM_OC_InitTypeDef;
typedef AlxShimStm32_Regs TIM_TypeDef;
typedef AlxShimStm32_Handle UART_HandleTypeDef;
typedef AlxShimStm32_Regs USART_TypeDef;


//******************************************************************************
// Peripheral instances - one register block each
//******************************************************************************
extern AlxShimStm32_Regs alxShimStm32_ADC1;
#define ADC1 (&alxShimStm32_ADC1)
extern AlxShimStm32_Regs alxShimStm32_ADC2;
#define ADC2 (&alxShimStm32_ADC2)
extern AlxShimStm32_Regs alxShimStm32_ADC3;
#define ADC3 (&alxShimStm32_ADC3)
extern AlxShimStm32_Regs alxShimStm32_ADC4;
#define ADC4 (&alxShimStm32_ADC4)
extern AlxShimStm32_Regs alxShimStm32_ADC5;
#define ADC5 (&alxShimStm32_ADC5)
extern AlxShimStm32_Regs alxShimStm32_CAN1;
#define CAN1 (&alxShimStm32_CAN1)
extern AlxShimStm32_Regs alxShimStm32_CAN2;
#define CAN2 (&alxShimStm32_CAN2)
extern AlxShimStm32_Regs alxShimStm32_CAN3;
#define CAN3 (&alxShimStm32_CAN3)
extern AlxShimStm32_Regs alxShimStm32_DAC1;
#define DAC1 (&alxShimStm32_DAC1)
extern AlxShimStm32_Regs alxShimStm32_DAC2;
#define DAC2 (&alxShimStm32_DAC2)
extern AlxShimStm32_Regs alxShimStm32_DAC3;
#define DAC3 (&alxShimStm32_DAC3)
extern AlxShimStm32_Regs alxShimStm32_DAC4;
#define DAC4 (&alxShimStm32_DAC4)
extern AlxShimStm32_Regs alxShimStm32_DFSDM1_Channel0;
#define DFSDM1_Channel0 (&alxShimStm32_DFSDM1_Channel0)
extern AlxShimStm32_Regs alxShimStm32_DMA1;
#define DMA1 (&alxShimStm32_DMA1)
extern AlxShimStm32_Regs alxShimStm32_DMA1_Channel1;
#define DMA1_Channel1 (&alxShimStm32_DMA1_Channel1)
extern AlxShimStm32_Regs alxShimStm32_DMA1_Channel5;
#define DMA1_Channel5 (&alxShimStm32_DMA1_Channel5)
extern AlxShimStm32_Regs alxShimStm32_DMA2;
#define DMA2 (&alxShimStm32_DMA2)
extern AlxShimStm32_Regs alxShimStm32_DMA2_Stream0;
#define DMA2_Stream0 (&alxShimStm32_DMA2_Stream0)
extern AlxShimStm32_Regs alxShimStm32_DMA2_Stream1;
#define DMA2_Stream1 (&alxShimStm32_DMA2_Stream1)
extern AlxShimStm32_Regs alxShimStm32_DMA2_Stream2;
#define DMA2_Stream2 (&alxShimStm32_DMA2_Stream2)
extern AlxShimStm32_Regs alxShimStm32_DMA3;
#define DMA3 (&alxShimStm32_DMA3)
extern AlxShimStm32_Regs alxShimStm32_DMAMUX1;
#define DMAMUX1 (&alxShimStm32_DMAMUX1)
extern AlxShimStm32_Regs alxShimStm32_FDCAN1;
#define FDCAN1 (&alxShimStm32_FDCAN1)
extern AlxShimStm32_Regs alxShimStm32_FDCAN2;
#define FDCAN2 (&alxShimStm32_FDCAN2)
extern AlxShimStm32_Regs alxShimStm32_FDCAN3;
#define FDCAN3 (&alxShimStm32_FDCAN3)
extern AlxShimStm32_Regs alxShimStm32_GPDMA1;
#define GPDMA1 (&alxShimStm32_GPDMA1)
extern AlxShimStm32_Regs alxShimStm32_GPDMA1_Channel0;
#define GPDMA1_Channel0 (&alxShimStm32_GPDMA1_Channel0)
extern AlxShimStm32_Regs alxShimStm32_GPIOA;
#define GPIOA (&alxShimStm32_GPIOA)
extern AlxShimStm32_Regs alxShimStm32_GPIOB;
#define GPIOB (&alxShimStm32_GPIOB)
extern AlxShimStm32_Regs alxShimStm32_GPIOC;
#define GPIOC (&alxShimStm32_GPIOC)
extern AlxShimStm32_Regs alxShimStm32_GPIOD;
#define GPIOD (&alxShimStm32_GPIOD)
extern AlxShimStm32_Regs alxShimStm32_GPIOE;
#define GPIOE (&alxShimStm32_GPIOE)
extern AlxShimStm32_Regs alxShimStm32_GPIOF;
#define GPIOF (&alxShimStm32_GPIOF)
extern AlxShimStm32_Regs alxShimStm32_GPIOG;
#define GPIOG (&alxShimStm32_GPIOG)
extern AlxShimStm32_Regs alxShimStm32_GPIOH;
#define GPIOH (&alxShimStm32_GPIOH)
extern AlxShimStm32_Regs alxShimStm32_GPIOI;
#define GPIOI (&alxShimStm32_GPIOI)
extern AlxShimStm32_Regs alxShimStm32_GPIOJ;
#define GPIOJ (&alxShimStm32_GPIOJ)
extern AlxShimStm32_Regs alxShimStm32_GPIOK;
#define GPIOK (&alxShimStm32_GPIOK)
extern AlxShimStm32_Regs alxShimStm32_GPIOL;
#define GPIOL (&alxShimStm32_GPIOL)
extern AlxShimStm32_Regs alxShimStm32_GPIOM;
#define GPIOM (&alxShimStm32_GPIOM)
extern AlxShimStm32_Regs alxShimStm32_GPION;
#define GPION (&alxShimStm32_GPION)
extern AlxShimStm32_Regs alxShimStm32_I2C1;
#define I2C1 (&alxShimStm32_I2C1)
extern AlxShimStm32_Regs alxShimStm32_I2C2;
#define I2C2 (&alxShimStm32_I2C2)
extern AlxShimStm32_Regs alxShimStm32_I2C3;
#define I2C3 (&alxShimStm32_I2C3)
extern AlxShimStm32_Regs alxShimStm32_I2C4;
#define I2C4 (&alxShimStm32_I2C4)
extern AlxShimStm32_Regs alxShimStm32_I2C5;
#define I2C5 (&alxShimStm32_I2C5)
extern AlxShimStm32_Regs alxShimStm32_I2C6;
#define I2C6 (&alxShimStm32_I2C6)
extern AlxShimStm32_Regs alxShimStm32_IWDG;
#define IWDG (&alxShimStm32_IWDG)
extern AlxShimStm32_Regs alxShimStm32_LPUART1;
#define LPUART1 (&alxShimStm32_LPUART1)
extern AlxShimStm32_Regs alxShimStm32_LPUART2;
#define LPUART2 (&alxShimStm32_LPUART2)
extern AlxShimStm32_Regs alxShimStm32_RCC;
#define RCC (&alxShimStm32_RCC)
extern AlxShimStm32_Regs alxShimStm32_RTC;
#define RTC (&alxShimStm32_RTC)
extern AlxShimStm32_Regs alxShimStm32_SAI1;
#define SAI1 (&alxShimStm32_SAI1)
extern AlxShimStm32_Regs alxShimStm32_SAI1_Block_A;
#define SAI1_Block_A (&alxShimStm32_SAI1_Block_A)
extern AlxShimStm32_Regs alxShimStm32_SAI1_Block_B;
#define SAI1_Block_B (&alxShimStm32_SAI1_Block_B)
extern AlxShimStm32_Regs alxShimStm32_SAI2;
#define SAI2 (&alxShimStm32_SAI2)
extern AlxShimStm32_Regs alxShimStm32_SAI2_Block_A;
#define SAI2_Block_A (&alxShimStm32_SAI2_Block_A)
extern AlxShimStm32_Regs alxShimStm32_SAI2_Block_B;
#define SAI2_Block_B (&alxShimStm32_SAI2_Block_B)
extern AlxShimStm32_Regs alxShimStm32_SDMMC1;
#define SDMMC1 (&alxShimStm32_SDMMC1)
extern AlxShimStm32_Regs alxShimStm32_SDMMC2;
#define SDMMC2 (&alxShimStm32_SDMMC2)
extern AlxShimStm32_Regs alxShimStm32_SPI1;
#define SPI1 (&alxShimStm32_SPI1)
extern AlxShimStm32_Regs alxShimStm32_SPI2;
#define SPI2 (&alxShimStm32_SPI2)
extern AlxShimStm32_Regs alxShimStm32_SPI3;
#define SPI3 (&alxShimStm32_SPI3)
extern AlxShimStm32_Regs alxShimStm32_SPI4;
#define SPI4 (&alxShimStm32_SPI4)
extern AlxShimStm32_Regs alxShimStm32_SPI5;
#define SPI5 (&alxShimStm32_SPI5)
extern AlxShimStm32_Regs alxShimStm32_SPI6;
#define SPI6 (&alxShimStm32_SPI6)
extern AlxShimStm32_Regs alxShimStm32_TIM1;
#define TIM1 (&alxShimStm32_TIM1)
extern AlxShimStm32_Regs alxShimStm32_TIM10;
#define TIM10 (&alxShimStm32_TIM10)
extern AlxShimStm32_Regs alxShimStm32_TIM11;
#define TIM11 (&alxShimStm32_TIM11)
extern AlxShimStm32_Regs alxShimStm32_TIM12;
#define TIM12 (&alxShimStm32_TIM12)
extern AlxShimStm32_Regs alxShimStm32_TIM13;
#define TIM13 (&alxShimStm32_TIM13)
extern AlxShimStm32_Regs alxShimStm32_TIM14;
#define TIM14 (&alxShimStm32_TIM14)
extern AlxShimStm32_Regs alxShimStm32_TIM15;
#define TIM15 (&alxShimStm32_TIM15)
extern AlxShimStm32_Regs alxShimStm32_TIM16;
#define TIM16 (&alxShimStm32_TIM16)
extern AlxShimStm32_Regs alxShimStm32_TIM17;
#define TIM17 (&alxShimStm32_TIM17)
extern AlxShimStm32_Regs alxShimStm32_TIM18;
#define TIM18 (&alxShimStm32_TIM18)
extern AlxShimStm32_Regs alxShimStm32_TIM19;
#define TIM19 (&alxShimStm32_TIM19)
extern AlxShimStm32_Regs alxShimStm32_TIM2;
#define TIM2 (&alxShimStm32_TIM2)
extern AlxShimStm32_Regs alxShimStm32_TIM20;
#define TIM20 (&alxShimStm32_TIM20)
extern AlxShimStm32_Regs alxShimStm32_TIM21;
#define TIM21 (&alxShimStm32_TIM21)
extern AlxShimStm32_Regs alxShimStm32_TIM22;
#define TIM22 (&alxShimStm32_TIM22)
extern AlxShimStm32_Regs alxShimStm32_TIM3;
#define TIM3 (&alxShimStm32_TIM3)
extern AlxShimStm32_Regs alxShimStm32_TIM4;
#define TIM4 (&alxShimStm32_TIM4)
extern AlxShimStm32_Regs alxShimStm32_TIM5;
#define TIM5 (&alxShimStm32_TIM5)
extern AlxShimStm32_Regs alxShimStm32_TIM6;
#define TIM6 (&alxShimStm32_TIM6)
extern AlxShimStm32_Regs alxShimStm32_TIM7;
#define TIM7 (&alxShimStm32_TIM7)
extern AlxShimStm32_Regs alxShimStm32_TIM8;
#define TIM8 (&alxShimStm32_TIM8)
extern AlxShimStm32_Regs alxShimStm32_TIM9;
#define TIM9 (&alxShimStm32_TIM9)
extern AlxShimStm32_Regs alxShimStm32_UART1;
#define UART1 (&alxShimStm32_UART1)
extern AlxShimStm32_Regs alxShimStm32_UART10;
#define UART10 (&alxShimStm32_UART10)
extern AlxShimStm32_Regs alxShimStm32_UART2;
#define UART2 (&alxShimStm32_UART2)
extern AlxShimStm32_Regs alxShimStm32_UART3;
#define UART3 (&alxShimStm32_UART3)
extern AlxShimStm32_Regs alxShimStm32_UART4;
#define UART4 (&alxShimStm32_UART4)
extern AlxShimStm32_Regs alxShimStm32_UART5;
#define UART5 (&alxShimStm32_UART5)
extern AlxShimStm32_Regs alxShimStm32_UART6;
#define UART6 (&alxShimStm32_UART6)
extern AlxShimStm32_Regs alxShimStm32_UART7;
#define UART7 (&alxShimStm32_UART7)
extern AlxShimStm32_Regs alxShimStm32_UART8;
#define UART8 (&alxShimStm32_UART8)
extern AlxShimStm32_Regs alxShimStm32_UART9;
#define UART9 (&alxShimStm32_UART9)
extern AlxShimStm32_Regs alxShimStm32_USART1;
#define USART1 (&alxShimStm32_USART1)
extern AlxShimStm32_Regs alxShimStm32_USART10;
#define USART10 (&alxShimStm32_USART10)
extern AlxShimStm32_Regs alxShimStm32_USART2;
#define USART2 (&alxShimStm32_USART2)
extern AlxShimStm32_Regs alxShimStm32_USART3;
#define USART3 (&alxShimStm32_USART3)
extern AlxShimStm32_Regs alxShimStm32_USART4;
#define USART4 (&alxShimStm32_USART4)
extern AlxShimStm32_Regs alxShimStm32_USART5;
#define USART5 (&alxShimStm32_USART5)
extern AlxShimStm32_Regs alxShimStm32_USART6;
#define USART6 (&alxShimStm32_USART6)
extern AlxShimStm32_Regs alxShimStm32_USART7;
#define USART7 (&alxShimStm32_USART7)
extern AlxShimStm32_Regs alxShimStm32_USART8;
#define USART8 (&alxShimStm32_USART8)
extern AlxShimStm32_Regs alxShimStm32_USART9;
#define USART9 (&alxShimStm32_USART9)
extern AlxShimStm32_Regs alxShimStm32_USB_OTG_FS;
#define USB_OTG_FS (&alxShimStm32_USB_OTG_FS)
extern AlxShimStm32_Regs alxShimStm32_USB_OTG_HS;
#define USB_OTG_HS (&alxShimStm32_USB_OTG_HS)


//******************************************************************************
// Constants - each a value no other has, so a switch over them stays a switch
//******************************************************************************
#define ADC_CALIB_OFFSET (0x0100U)
#define ADC_CFGR_DFSDMCFG (0x0101U)
#define ADC_CHANNEL_0 (0x0102U)
#define ADC_CHANNEL_1 (0x0103U)
#define ADC_CHANNEL_10 (0x0104U)
#define ADC_CHANNEL_11 (0x0105U)
#define ADC_CHANNEL_12 (0x0106U)
#define ADC_CHANNEL_13 (0x0107U)
#define ADC_CHANNEL_14 (0x0108U)
#define ADC_CHANNEL_15 (0x0109U)
#define ADC_CHANNEL_16 (0x010AU)
#define ADC_CHANNEL_2 (0x010BU)
#define ADC_CHANNEL_3 (0x010CU)
#define ADC_CHANNEL_4 (0x010DU)
#define ADC_CHANNEL_5 (0x010EU)
#define ADC_CHANNEL_6 (0x010FU)
#define ADC_CHANNEL_7 (0x0110U)
#define ADC_CHANNEL_8 (0x0111U)
#define ADC_CHANNEL_9 (0x0112U)
#define ADC_CHANNEL_TEMPSENSOR (0x0113U)
#define ADC_CHANNEL_TEMPSENSOR_ADC1 (0x0114U)
#define ADC_CHANNEL_TEMPSENSOR_ADC5 (0x0115U)
#define ADC_CHANNEL_VBAT (0x0116U)
#define ADC_CHANNEL_VLCD (0x0117U)
#define ADC_CHANNEL_VREFINT (0x0118U)
#define ADC_CLOCK_ASYNC_DIV1 (0x0119U)
#define ADC_CLOCK_ASYNC_DIV8 (0x011AU)
#define ADC_CLOCK_SYNC_PCLK_DIV4 (0x011BU)
#define ADC_CLOCK_SYNC_PCLK_DIV8 (0x011CU)
#define ADC_CONVERSIONDATA_DMA_CIRCULAR (0x011DU)
#define ADC_DATAALIGN_RIGHT (0x011EU)
#define ADC_DFSDM_MODE_DISABLE (0x011FU)
#define ADC_EOC_SEQ_CONV (0x0120U)
#define ADC_EXTERNALTRIGCONVEDGE_NONE (0x0121U)
#define ADC_LEFTBITSHIFT_NONE (0x0122U)
#define ADC_LOW_POWER_NONE (0x0123U)
#define ADC_OFFSET_NONE (0x0124U)
#define ADC_OVR_DATA_PRESERVED (0x0125U)
#define ADC_RANK_CHANNEL_NUMBER (0x0126U)
#define ADC_REGULAR_RANK_1 (0x0127U)
#define ADC_REGULAR_RANK_10 (0x0128U)
#define ADC_REGULAR_RANK_11 (0x0129U)
#define ADC_REGULAR_RANK_12 (0x012AU)
#define ADC_REGULAR_RANK_13 (0x012BU)
#define ADC_REGULAR_RANK_14 (0x012CU)
#define ADC_REGULAR_RANK_15 (0x012DU)
#define ADC_REGULAR_RANK_16 (0x012EU)
#define ADC_REGULAR_RANK_2 (0x012FU)
#define ADC_REGULAR_RANK_3 (0x0130U)
#define ADC_REGULAR_RANK_4 (0x0131U)
#define ADC_REGULAR_RANK_5 (0x0132U)
#define ADC_REGULAR_RANK_6 (0x0133U)
#define ADC_REGULAR_RANK_7 (0x0134U)
#define ADC_REGULAR_RANK_8 (0x0135U)
#define ADC_REGULAR_RANK_9 (0x0136U)
#define ADC_RESOLUTION_12B (0x0137U)
#define ADC_SAMPLING_MODE_NORMAL (0x0138U)
#define ADC_SCAN_DIRECTION_FORWARD (0x0139U)
#define ADC_SCAN_ENABLE (0x013AU)
#define ADC_SINGLE_ENDED (0x013BU)
#define ADC_SOFTWARE_START (0x013CU)
#define ADC_TRIGGER_FREQ_HIGH (0x013DU)
#define ADC_VREF_PPROT_NONE (0x013EU)
#define CAN_BS1_13TQ (0x013FU)
#define CAN_BS1_15TQ (0x0140U)
#define CAN_BS2_2TQ (0x0141U)
#define CAN_FILTERMODE_IDMASK (0x0142U)
#define CAN_FILTERSCALE_32BIT (0x0143U)
#define CAN_ID_EXT (0x0144U)
#define CAN_ID_STD (0x0145U)
#define CAN_IT_RX_FIFO0_MSG_PENDING (0x0146U)
#define CAN_IT_TX_MAILBOX_EMPTY (0x0147U)
#define CAN_MODE_NORMAL (0x0148U)
#define CAN_RTR_DATA (0x0149U)
#define CAN_RTR_REMOTE (0x014AU)
#define CAN_RX_FIFO0 (0x014BU)
#define CAN_SJW_1TQ (0x014CU)
#define DAC_ALIGN_12B_R (0x014DU)
#define DAC_CHANNEL_1 (0x014EU)
#define DAC_CHANNEL_2 (0x014FU)
#define DAC_CHIPCONNECT_DISABLE (0x0150U)
#define DAC_HIGH_FREQUENCY_INTERFACE_MODE_AUTOMATIC (0x0151U)
#define DAC_OUTPUTBUFFER_ENABLE (0x0152U)
#define DAC_SAMPLEANDHOLD_DISABLE (0x0153U)
#define DAC_TRIGGER_NONE (0x0154U)
#define DAC_TRIMMING_FACTORY (0x0155U)
#define DMA_BREQ_SINGLE_BURST (0x0156U)
#define DMA_CHANNEL_0 (0x0157U)
#define DMA_CHANNEL_1 (0x0158U)
#define DMA_CHANNEL_2 (0x0159U)
#define DMA_CHANNEL_PRIV (0x015AU)
#define DMA_CIRCULAR (0x015BU)
#define DMA_DATA_RIGHTALIGN_ZEROPADDED (0x015CU)
#define DMA_DEST_ALLOCATED_PORT0 (0x015DU)
#define DMA_DEST_DATAWIDTH_WORD (0x015EU)
#define DMA_DINC_INCREMENTED (0x015FU)
#define DMA_EXCHANGE_NONE (0x0160U)
#define DMA_FIFOMODE_DISABLE (0x0161U)
#define DMA_GPDMA_LINEAR_NODE (0x0162U)
#define DMA_LINKEDLIST_CIRCULAR (0x0163U)
#define DMA_LINK_ALLOCATED_PORT0 (0x0164U)
#define DMA_LOW_PRIORITY_LOW_WEIGHT (0x0165U)
#define DMA_LSM_FULL_EXECUTION (0x0166U)
#define DMA_MBURST_SINGLE (0x0167U)
#define DMA_MDATAALIGN_WORD (0x0168U)
#define DMA_MINC_ENABLE (0x0169U)
#define DMA_NORMAL (0x016AU)
#define DMA_PBURST_SINGLE (0x016BU)
#define DMA_PDATAALIGN_WORD (0x016CU)
#define DMA_PERIPH_TO_MEMORY (0x016DU)
#define DMA_PINC_DISABLE (0x016EU)
#define DMA_PRIORITY_LOW (0x016FU)
#define DMA_REQUEST_0 (0x0170U)
#define DMA_REQUEST_ADC1 (0x0171U)
#define DMA_SINC_FIXED (0x0172U)
#define DMA_SRC_ALLOCATED_PORT0 (0x0173U)
#define DMA_SRC_DATAWIDTH_WORD (0x0174U)
#define DMA_TCEM_BLOCK_TRANSFER (0x0175U)
#define DMA_TCEM_LAST_LL_ITEM_TRANSFER (0x0176U)
#define DMA_TRIG_POLARITY_MASKED (0x0177U)
#define FDCAN_ACCEPT_IN_RX_FIFO0 (0x0178U)
#define FDCAN_BRS_OFF (0x0179U)
#define FDCAN_CLASSIC_CAN (0x017AU)
#define FDCAN_CLOCK_DIV1 (0x017BU)
#define FDCAN_DATA_FRAME (0x017CU)
#define FDCAN_DLC_BYTES_0 (0x017DU)
#define FDCAN_DLC_BYTES_1 (0x017EU)
#define FDCAN_DLC_BYTES_2 (0x017FU)
#define FDCAN_DLC_BYTES_3 (0x0180U)
#define FDCAN_DLC_BYTES_4 (0x0181U)
#define FDCAN_DLC_BYTES_5 (0x0182U)
#define FDCAN_DLC_BYTES_6 (0x0183U)
#define FDCAN_DLC_BYTES_7 (0x0184U)
#define FDCAN_DLC_BYTES_8 (0x0185U)
#define FDCAN_ESI_PASSIVE (0x0186U)
#define FDCAN_EXTENDED_ID (0x0187U)
#define FDCAN_FILTER_DISABLE (0x0188U)
#define FDCAN_FILTER_RANGE (0x0189U)
#define FDCAN_FILTER_REMOTE (0x018AU)
#define FDCAN_FRAME_CLASSIC (0x018BU)
#define FDCAN_IT_RX_FIFO0_NEW_MESSAGE (0x018CU)
#define FDCAN_IT_TX_FIFO_EMPTY (0x018DU)
#define FDCAN_MODE_NORMAL (0x018EU)
#define FDCAN_NO_TX_EVENTS (0x018FU)
#define FDCAN_REMOTE_FRAME (0x0190U)
#define FDCAN_RX_FIFO0 (0x0191U)
#define FDCAN_STANDARD_ID (0x0192U)
#define FDCAN_TX_FIFO_OPERATION (0x0193U)
#define FLASHSIZE_BASE (0x0194U)
#define FLASH_BANK_2 (0x0195U)
#define FLASH_FLAG_BSY (0x0196U)
#define FLASH_FLAG_ECCC (0x0197U)
#define FLASH_FLAG_ECCD (0x0198U)
#define FLASH_FLAG_EOP (0x0199U)
#define FLASH_FLAG_ERSERR (0x019AU)
#define FLASH_FLAG_FASTERR (0x019BU)
#define FLASH_FLAG_MISERR (0x019CU)
#define FLASH_FLAG_OPERR (0x019DU)
#define FLASH_FLAG_OPTVERR (0x019EU)
#define FLASH_FLAG_PEMPTY (0x019FU)
#define FLASH_FLAG_PGAERR (0x01A0U)
#define FLASH_FLAG_PGPERR (0x01A1U)
#define FLASH_FLAG_PGSERR (0x01A2U)
#define FLASH_FLAG_PROGERR (0x01A3U)
#define FLASH_FLAG_RDERR (0x01A4U)
#define FLASH_FLAG_SIZERR (0x01A5U)
#define FLASH_FLAG_WRPERR (0x01A6U)
#define FLASH_LATENCY_0 (0x01A7U)
#define FLASH_LATENCY_1 (0x01A8U)
#define FLASH_LATENCY_4 (0x01A9U)
#define FLASH_LATENCY_5 (0x01AAU)
#define FLASH_LATENCY_7 (0x01ABU)
#define FLASH_SECTOR_10 (0x01ACU)
#define FLASH_SECTOR_12 (0x01ADU)
#define FLASH_TYPEERASE_PAGES (0x01AEU)
#define FLASH_TYPEERASE_SECTORS (0x01AFU)
#define FLASH_TYPEPROGRAM_DOUBLEWORD (0x01B0U)
#define FLASH_TYPEPROGRAM_WORD (0x01B1U)
#define FLASH_VOLTAGE_RANGE_3 (0x01B2U)
#define GPDMA1_REQUEST_ADC1 (0x01B3U)
#define GPIO_AF4_TIM3 (0x01B4U)
#define GPIO_MODE_AF_PP (0x01B5U)
#define GPIO_NOPULL (0x01B6U)
#define GPIO_PIN_0 (0x0001U)
#define GPIO_PIN_1 (0x0002U)
#define GPIO_PIN_10 (0x0400U)
#define GPIO_PIN_11 (0x0800U)
#define GPIO_PIN_12 (0x1000U)
#define GPIO_PIN_13 (0x2000U)
#define GPIO_PIN_14 (0x4000U)
#define GPIO_PIN_15 (0x8000U)
#define GPIO_PIN_2 (0x0004U)
#define GPIO_PIN_3 (0x0008U)
#define GPIO_PIN_4 (0x0010U)
#define GPIO_PIN_5 (0x0020U)
#define GPIO_PIN_6 (0x0040U)
#define GPIO_PIN_7 (0x0080U)
#define GPIO_PIN_8 (0x0100U)
#define GPIO_PIN_9 (0x0200U)
#define GPIO_SPEED_HIGH (0x01C7U)
#define HAL_MMC_CARD_TRANSFER (0x01C8U)
#define HCD_PHY_EMBEDDED (0x01C9U)
#define HCD_SPEED_FULL (0x01CAU)
#define HCD_SPEED_HIGH (0x01CBU)
#define HCD_SPEED_LOW (0x01CCU)
#define HSE_VALUE (0x01CDU)
#define HSI_VALUE (0x01CEU)
#define I2C_ADDRESSINGMODE_7BIT (0x01CFU)
#define I2C_DUALADDRESS_DISABLE (0x01D0U)
#define I2C_DUTYCYCLE_2 (0x01D1U)
#define I2C_GENERALCALL_DISABLE (0x01D2U)
#define I2C_MEMADD_SIZE_16BIT (0x01D3U)
#define I2C_MEMADD_SIZE_8BIT (0x01D4U)
#define I2C_NOSTRETCH_DISABLE (0x01D5U)
#define I2C_OA2_NOMASK (0x01D6U)
#define IWDG_EWI_DISABLE (0x01D7U)
#define IWDG_PRESCALER_128 (0x01D8U)
#define IWDG_PRESCALER_16 (0x01D9U)
#define IWDG_PRESCALER_256 (0x01DAU)
#define IWDG_PRESCALER_32 (0x01DBU)
#define IWDG_PRESCALER_4 (0x01DCU)
#define IWDG_PRESCALER_64 (0x01DDU)
#define IWDG_PRESCALER_8 (0x01DEU)
#define IWDG_WINDOW_DISABLE (0x01DFU)
#define LL_GPIO_PULL_DOWN (0x01E0U)
#define LL_GPIO_PULL_UP (0x01E1U)
#define LL_SPI_RX_FIFO_TH_QUARTER (0x01E2U)
#define PWR_CR5_R1MODE (0x01E3U)
#define PWR_FLAG_VOS (0x01E4U)
#define PWR_REGULATOR_VOLTAGE_SCALE1 (0x01E5U)
#define PWR_REGULATOR_VOLTAGE_SCALE1_BOOST (0x01E6U)
#define PWR_REGULATOR_VOLTAGE_SCALE3 (0x01E7U)
#define RCC_ADCPCLK2_DIV2 (0x01E8U)
#define RCC_CLOCKTYPE_HCLK (0x01E9U)
#define RCC_CLOCKTYPE_PCLK1 (0x01EAU)
#define RCC_CLOCKTYPE_PCLK2 (0x01EBU)
#define RCC_CLOCKTYPE_PCLK3 (0x01ECU)
#define RCC_CLOCKTYPE_SYSCLK (0x01EDU)
#define RCC_CSR_BORRSTF (0x01EEU)
#define RCC_CSR_FWRSTF (0x01EFU)
#define RCC_CSR_IWDGRSTF (0x01F0U)
#define RCC_CSR_LPWRRSTF (0x01F1U)
#define RCC_CSR_OBLRSTF (0x01F2U)
#define RCC_CSR_PINRSTF (0x01F3U)
#define RCC_CSR_PORRSTF (0x01F4U)
#define RCC_CSR_SFTRSTF (0x01F5U)
#define RCC_CSR_WWDGRSTF (0x01F6U)
#define RCC_FDCANCLKSOURCE_PCLK1 (0x01F7U)
#define RCC_HCLK_DIV1 (0x01F8U)
#define RCC_HCLK_DIV2 (0x01F9U)
#define RCC_HCLK_DIV4 (0x01FAU)
#define RCC_HSE_OFF (0x01FBU)
#define RCC_HSE_ON (0x01FCU)
#define RCC_HSE_PREDIV_DIV1 (0x01FDU)
#define RCC_HSI14CALIBRATION_DEFAULT (0x01FEU)
#define RCC_HSI14_OFF (0x01FFU)
#define RCC_HSI48_OFF (0x0200U)
#define RCC_HSI48_SUPPORT (0x0201U)
#define RCC_HSICALIBRATION_DEFAULT (0x0202U)
#define RCC_HSI_OFF (0x0203U)
#define RCC_HSI_ON (0x0204U)
#define RCC_LSEDRIVE_HIGH (0x0205U)
#define RCC_LSEDRIVE_LOW (0x0206U)
#define RCC_LSEDRIVE_MEDIUMHIGH (0x0207U)
#define RCC_LSEDRIVE_MEDIUMLOW (0x0208U)
#define RCC_LSE_BYPASS (0x0209U)
#define RCC_LSE_OFF (0x020AU)
#define RCC_LSE_ON (0x020BU)
#define RCC_LSI_DIV1 (0x020CU)
#define RCC_LSI_OFF (0x020DU)
#define RCC_MSICALIBRATION_DEFAULT (0x020EU)
#define RCC_MSIKRANGE_4 (0x020FU)
#define RCC_MSIK_OFF (0x0210U)
#define RCC_MSIRANGE_3 (0x0211U)
#define RCC_MSIRANGE_4 (0x0212U)
#define RCC_MSIRANGE_5 (0x0213U)
#define RCC_MSIRANGE_6 (0x0214U)
#define RCC_MSI_ON (0x0215U)
#define RCC_OSCILLATORTYPE_HSE (0x0216U)
#define RCC_OSCILLATORTYPE_HSI (0x0217U)
#define RCC_OSCILLATORTYPE_LSE (0x0218U)
#define RCC_OSCILLATORTYPE_LSI (0x0219U)
#define RCC_OSCILLATORTYPE_MSI (0x021AU)
#define RCC_PERIPHCLK_ADC (0x021BU)
#define RCC_PERIPHCLK_FDCAN (0x021CU)
#define RCC_PERIPHCLK_RTC (0x021DU)
#define RCC_PERIPHCLK_SAI_PLLI2S (0x021EU)
#define RCC_PERIPHCLK_SDMMC1 (0x021FU)
#define RCC_PLLMBOOST_DIV1 (0x0220U)
#define RCC_PLLM_DIV3 (0x0221U)
#define RCC_PLLM_DIV4 (0x0222U)
#define RCC_PLLP_DIV2 (0x0223U)
#define RCC_PLLP_DIV5 (0x0224U)
#define RCC_PLLQ_DIV2 (0x0225U)
#define RCC_PLLR_DIV2 (0x0226U)
#define RCC_PLLSOURCE_HSE (0x0227U)
#define RCC_PLLSOURCE_HSI (0x0228U)
#define RCC_PLLSOURCE_HSI_DIV2 (0x0229U)
#define RCC_PLLSOURCE_NONE (0x022AU)
#define RCC_PLLVCIRANGE_0 (0x022BU)
#define RCC_PLL_DIV2 (0x022CU)
#define RCC_PLL_MUL16 (0x022DU)
#define RCC_PLL_MUL4 (0x022EU)
#define RCC_PLL_MUL6 (0x022FU)
#define RCC_PLL_MUL8 (0x0230U)
#define RCC_PLL_NONE (0x0231U)
#define RCC_PLL_OFF (0x0232U)
#define RCC_PLL_ON (0x0233U)
#define RCC_PREDIV_DIV1 (0x0234U)
#define RCC_RTCCLKSOURCE_LSE (0x0235U)
#define RCC_SDMMC1CLKSOURCE_PLLP (0x0236U)
#define RCC_SHSI_OFF (0x0237U)
#define RCC_SYSCLKSOURCE_HSE (0x0238U)
#define RCC_SYSCLKSOURCE_HSI (0x0239U)
#define RCC_SYSCLKSOURCE_MSI (0x023AU)
#define RCC_SYSCLKSOURCE_PLLCLK (0x023BU)
#define RCC_SYSCLK_DIV1 (0x023CU)
#define RTC_BKP_DR0 (0x023DU)
#define RTC_DAYLIGHTSAVING_NONE (0x023EU)
#define RTC_FORMAT_BIN (0x023FU)
#define RTC_HOURFORMAT_24 (0x0240U)
#define RTC_OUTPUT_DISABLE (0x0241U)
#define RTC_SHIFTADD1S_RESET (0x0242U)
#define RTC_SHIFTADD1S_SET (0x0243U)
#define RTC_SMOOTHCALIB_PERIOD_32SEC (0x0244U)
#define RTC_SMOOTHCALIB_PLUSPULSES_RESET (0x0245U)
#define RTC_STOREOPERATION_RESET (0x0246U)
#define SAI_ASYNCHRONOUS (0x0247U)
#define SAI_AUDIO_FREQUENCY_16K (0x0248U)
#define SAI_CLKSOURCE_PLLI2S (0x0249U)
#define SAI_FIFOTHRESHOLD_EMPTY (0x024AU)
#define SAI_FIFOTHRESHOLD_FULL (0x024BU)
#define SAI_I2S_STANDARD (0x024CU)
#define SAI_MASTERDIVIDER_ENABLE (0x024DU)
#define SAI_MODEMASTER_RX (0x024EU)
#define SAI_MODEMASTER_TX (0x024FU)
#define SAI_MODESLAVE_RX (0x0250U)
#define SAI_MODESLAVE_TX (0x0251U)
#define SAI_NOCOMPANDING (0x0252U)
#define SAI_OUTPUTDRIVE_DISABLE (0x0253U)
#define SAI_OUTPUT_NOTRELEASED (0x0254U)
#define SAI_STEREOMODE (0x0255U)
#define SAI_SYNCEXT_DISABLE (0x0256U)
#define SAI_SYNCHRONOUS (0x0257U)
#define SAI_xIMR_FREQIE (0x0258U)
#define SAI_xSR_FREQ (0x0259U)
#define SDMMC_BUS_WIDE_1B (0x025AU)
#define SDMMC_BUS_WIDE_4B (0x025BU)
#define SDMMC_BUS_WIDE_8B (0x025CU)
#define SDMMC_CLOCK_EDGE_RISING (0x025DU)
#define SDMMC_CLOCK_POWER_SAVE_DISABLE (0x025EU)
#define SDMMC_HARDWARE_FLOW_CONTROL_ENABLE (0x025FU)
#define SDMMC_TRANSCEIVER_DISABLE (0x0260U)
#define SPI_BAUDRATEPRESCALER_128 (0x0261U)
#define SPI_BAUDRATEPRESCALER_16 (0x0262U)
#define SPI_BAUDRATEPRESCALER_2 (0x0263U)
#define SPI_BAUDRATEPRESCALER_256 (0x0264U)
#define SPI_BAUDRATEPRESCALER_32 (0x0265U)
#define SPI_BAUDRATEPRESCALER_4 (0x0266U)
#define SPI_BAUDRATEPRESCALER_64 (0x0267U)
#define SPI_BAUDRATEPRESCALER_8 (0x0268U)
#define SPI_CRCCALCULATION_DISABLE (0x0269U)
#define SPI_CRC_LENGTH_DATASIZE (0x026AU)
#define SPI_DATASIZE_8BIT (0x026BU)
#define SPI_DIRECTION_2LINES (0x026CU)
#define SPI_FIRSTBIT_MSB (0x026DU)
#define SPI_MODE_MASTER (0x026EU)
#define SPI_NSS_PULSE_DISABLE (0x026FU)
#define SPI_NSS_SOFT (0x0270U)
#define SPI_PHASE_1EDGE (0x0271U)
#define SPI_PHASE_2EDGE (0x0272U)
#define SPI_POLARITY_HIGH (0x0273U)
#define SPI_POLARITY_LOW (0x0274U)
#define SPI_TIMODE_DISABLE (0x0275U)
#define SystemCoreClock (0x0276U)
#define TIM3_TI2_GPIOB5_AF4 (0x0277U)
#define TIM_AUTORELOAD_PRELOAD_ENABLE (0x0278U)
#define TIM_CHANNEL_1 (0x0279U)
#define TIM_CHANNEL_2 (0x027AU)
#define TIM_CHANNEL_3 (0x027BU)
#define TIM_CHANNEL_4 (0x027CU)
#define TIM_CHANNEL_5 (0x027DU)
#define TIM_CHANNEL_6 (0x027EU)
#define TIM_CLOCKDIVISION_DIV1 (0x027FU)
#define TIM_COUNTERMODE_UP (0x0280U)
#define TIM_OCFAST_DISABLE (0x0281U)
#define TIM_OCIDLESTATE_RESET (0x0282U)
#define TIM_OCMODE_PWM1 (0x0283U)
#define TIM_OCNIDLESTATE_RESET (0x0284U)
#define TIM_OCNPOLARITY_HIGH (0x0285U)
#define TIM_OCPOLARITY_HIGH (0x0286U)
#define UART_ADVFEATURE_NO_INIT (0x0287U)
#define UART_ADVFEATURE_OVERRUN_DISABLE (0x0288U)
#define UART_ADVFEATURE_RXOVERRUNDISABLE_INIT (0x0289U)
#define UART_FLAG_RXNE (0x028AU)
#define UART_FLAG_TXE (0x028BU)
#define UART_HWCONTROL_NONE (0x028CU)
#define UART_IT_RXNE (0x028DU)
#define UART_IT_TXE (0x028EU)
#define UART_LINBREAKDETECTLENGTH_11B (0x028FU)
#define UART_MODE_TX (0x0290U)
#define UART_MODE_TX_RX (0x0291U)
#define UART_ONE_BIT_SAMPLE_DISABLE (0x0292U)
#define UART_OVERSAMPLING_16 (0x0293U)
#define UART_PARITY_NONE (0x0294U)
#define UART_PRESCALER_DIV1 (0x0295U)
#define UART_STOPBITS_1 (0x0296U)
#define UART_WORDLENGTH_8B (0x0297U)
#define UID_BASE (0x0298U)
#define USART_CR2_LINEN (0x0299U)
#define USART_PRESC_PRESCALER (0x029AU)
#define __ADC_RESOLUTION__ (0x029BU)
#define __VREFINT_ADC_DATA__ (0x029CU)


//******************************************************************************
// Calls and function-like macros - forwarded to one variadic function
//******************************************************************************
uint32_t alxShimStm32_Call(int unused, ...);
#define ALX_SHIM_STM32_CALL(...) alxShimStm32_Call(0, ##__VA_ARGS__)
#define UNUSED(X) (void)(X)
#define __HAL_LINKDMA(HANDLE, PPP_DMA_FIELD, DMA_HANDLE) do { (HANDLE)->PPP_DMA_FIELD = &(DMA_HANDLE); (DMA_HANDLE).Parent = (HANDLE); } while (0)
#define HAL_ADCEx_Calibration_Start(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define HAL_ADC_ConfigChannel(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define HAL_ADC_DeInit(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define HAL_ADC_Init(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define HAL_ADC_Start_DMA(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define HAL_CAN_ActivateNotification(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define HAL_CAN_AddTxMessage(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define HAL_CAN_ConfigFilter(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define HAL_CAN_DeInit(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define HAL_CAN_DeactivateNotification(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define HAL_CAN_GetRxFifoFillLevel(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define HAL_CAN_GetRxMessage(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define HAL_CAN_GetTxMailboxesFreeLevel(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define HAL_CAN_IRQHandler(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define HAL_CAN_Init(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define HAL_CAN_Start(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define HAL_DAC_ConfigChannel(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define HAL_DAC_DeInit(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define HAL_DAC_Init(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define HAL_DAC_SetValue(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define HAL_DAC_Start(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define HAL_DMAEx_List_BuildNode(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define HAL_DMAEx_List_Init(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define HAL_DMAEx_List_InsertNode(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define HAL_DMAEx_List_LinkQ(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define HAL_DMAEx_List_SetCircularMode(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define HAL_DMA_ConfigChannelAttributes(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define HAL_DMA_DeInit(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define HAL_DMA_Init(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define HAL_Delay(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define HAL_FDCAN_ActivateNotification(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define HAL_FDCAN_AddMessageToTxFifoQ(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define HAL_FDCAN_ConfigFilter(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define HAL_FDCAN_ConfigGlobalFilter(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define HAL_FDCAN_DeInit(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define HAL_FDCAN_DeactivateNotification(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define HAL_FDCAN_GetProtocolStatus(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define HAL_FDCAN_GetRxFifoFillLevel(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define HAL_FDCAN_GetRxMessage(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define HAL_FDCAN_GetTxFifoFreeLevel(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define HAL_FDCAN_IRQHandler(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define HAL_FDCAN_Init(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define HAL_FDCAN_Start(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define HAL_FLASHEx_Erase(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define HAL_FLASH_Lock(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define HAL_FLASH_Program(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define HAL_FLASH_Unlock(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define HAL_GPIO_DeInit(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define HAL_GPIO_EXTI_IRQHandler(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define HAL_GPIO_Init(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define HAL_GPIO_ReadPin(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define HAL_GPIO_TogglePin(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define HAL_GPIO_WritePin(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define HAL_GetDEVID(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define HAL_GetHalVersion(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define HAL_GetREVID(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define HAL_HCD_DeInit(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define HAL_HCD_GetCurrentFrame(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define HAL_HCD_GetCurrentSpeed(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define HAL_HCD_HC_GetURBState(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define HAL_HCD_HC_GetXferCount(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define HAL_HCD_HC_Halt(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define HAL_HCD_HC_Init(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define HAL_HCD_HC_SubmitRequest(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define HAL_HCD_IRQHandler(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define HAL_HCD_Init(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define HAL_HCD_ResetPort(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define HAL_HCD_Start(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define HAL_HCD_Stop(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define HAL_I2C_DeInit(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define HAL_I2C_Init(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define HAL_I2C_IsDeviceReady(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define HAL_I2C_Master_Receive(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define HAL_I2C_Master_Transmit(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define HAL_I2C_Mem_Read(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define HAL_I2C_Mem_Write(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define HAL_IWDG_Init(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define HAL_IWDG_Refresh(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define HAL_LIN_Init(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define HAL_LIN_SendBreak(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define HAL_MMC_CardCIDTypeDef(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define HAL_MMC_CardCSDTypeDef(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define HAL_MMC_CardInfoTypeDef(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define HAL_MMC_CardStateTypeDef(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define HAL_MMC_DeInit(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define HAL_MMC_GetCardCID(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define HAL_MMC_GetCardCSD(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define HAL_MMC_GetCardInfo(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define HAL_MMC_GetCardState(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define HAL_MMC_IRQHandler(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define HAL_MMC_Init(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define HAL_MMC_ReadBlocks_DMA(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define HAL_MMC_WriteBlocks_DMA(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define HAL_NVIC_ClearPendingIRQ(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define HAL_NVIC_DisableIRQ(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define HAL_NVIC_EnableIRQ(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define HAL_NVIC_SetPriority(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define HAL_NVIC_SystemReset(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define HAL_PWREx_ControlVoltageScaling(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define HAL_PWREx_EnableOverDrive(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define HAL_PWREx_EnableVddA(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define HAL_PWREx_EnableVddIO2(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define HAL_PWR_DisableBkUpAccess(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define HAL_PWR_EnableBkUpAccess(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define HAL_RCCEx_PeriphCLKConfig(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define HAL_RCC_ClockConfig(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define HAL_RCC_EnableCSS(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define HAL_RCC_GetHCLKFreq(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define HAL_RCC_GetPCLK1Freq(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define HAL_RCC_GetPCLK2Freq(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define HAL_RCC_GetPCLK3Freq(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#if !defined(STM32F4) && !defined(STM32F7)
#define HAL_RCC_GetResetSource(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#endif
#define HAL_RCC_GetSysClockFreq(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define HAL_RCC_NMI_IRQHandler(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define HAL_RCC_OscConfig(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define HAL_RTCEx_BKUPRead(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define HAL_RTCEx_BKUPWrite(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define HAL_RTCEx_SetSmoothCalib(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define HAL_RTCEx_SetSynchroShift(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define HAL_RTC_DeInit(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define HAL_RTC_GetDate(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define HAL_RTC_GetTime(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define HAL_RTC_Init(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define HAL_RTC_SetDate(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define HAL_RTC_SetTime(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define HAL_SAI_DeInit(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define HAL_SAI_InitProtocol(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define HAL_SPI_DeInit(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define HAL_SPI_Init(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define HAL_SPI_Receive(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define HAL_SPI_Transmit(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define HAL_SPI_TransmitReceive(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define HAL_StatusTypeDef(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define HAL_TIMEx_RemapConfig(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define HAL_TIM_Base_Init(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define HAL_TIM_Base_Start(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define HAL_TIM_Base_Stop(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define HAL_TIM_PWM_ConfigChannel(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define HAL_TIM_PWM_DeInit(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define HAL_TIM_PWM_Init(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define HAL_TIM_PWM_Start(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define HAL_UART_DeInit(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define HAL_UART_Init(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define HAL_UART_Transmit(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define LL_GPIO_SetPinPull(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define LL_SPI_Enable(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define LL_SPI_IsActiveFlag_RXNE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define LL_SPI_IsActiveFlag_TXE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define LL_SPI_ReceiveData8(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define LL_SPI_SetRxFIFOThreshold(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define LL_SPI_TransmitData8(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define LL_USART_ReceiveData8(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define LL_USART_TransmitData8(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_FLASH_CLEAR_FLAG(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_FLASH_GET_FLAG(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_GPIO_EXTI_CLEAR_IT(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_PWR_GET_FLAG(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_PWR_VOLTAGESCALING_CONFIG(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_ADC12_CLK_DISABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_ADC12_CLK_ENABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_ADC12_FORCE_RESET(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_ADC12_RELEASE_RESET(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#if !defined(STM32G4) && !defined(STM32L4)
#define __HAL_RCC_ADC1_CLK_DISABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#endif
#if !defined(STM32G4) && !defined(STM32L4)
#define __HAL_RCC_ADC1_CLK_ENABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#endif
#if !defined(STM32F4) && !defined(STM32F7) && !defined(STM32G4) && !defined(STM32L4)
#define __HAL_RCC_ADC1_FORCE_RESET(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#endif
#if !defined(STM32F4) && !defined(STM32F7) && !defined(STM32G4) && !defined(STM32L4)
#define __HAL_RCC_ADC1_RELEASE_RESET(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#endif
#if !defined(STM32G4)
#define __HAL_RCC_ADC2_CLK_DISABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#endif
#if !defined(STM32G4)
#define __HAL_RCC_ADC2_CLK_ENABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#endif
#if !defined(STM32F4) && !defined(STM32F7) && !defined(STM32G4)
#define __HAL_RCC_ADC2_FORCE_RESET(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#endif
#if !defined(STM32F4) && !defined(STM32F7) && !defined(STM32G4)
#define __HAL_RCC_ADC2_RELEASE_RESET(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#endif
#define __HAL_RCC_ADC345_CLK_DISABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_ADC345_CLK_ENABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_ADC345_FORCE_RESET(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_ADC345_RELEASE_RESET(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#if !defined(STM32G4)
#define __HAL_RCC_ADC3_CLK_DISABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#endif
#if !defined(STM32G4)
#define __HAL_RCC_ADC3_CLK_ENABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#endif
#if !defined(STM32F4) && !defined(STM32F7) && !defined(STM32G4)
#define __HAL_RCC_ADC3_FORCE_RESET(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#endif
#if !defined(STM32F4) && !defined(STM32F7) && !defined(STM32G4)
#define __HAL_RCC_ADC3_RELEASE_RESET(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#endif
#if !defined(STM32G4)
#define __HAL_RCC_ADC4_CLK_DISABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#endif
#if !defined(STM32G4)
#define __HAL_RCC_ADC4_CLK_ENABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#endif
#if !defined(STM32G4)
#define __HAL_RCC_ADC4_FORCE_RESET(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#endif
#if !defined(STM32G4)
#define __HAL_RCC_ADC4_RELEASE_RESET(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#endif
#if !defined(STM32G4)
#define __HAL_RCC_ADC5_CLK_DISABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#endif
#if !defined(STM32G4)
#define __HAL_RCC_ADC5_CLK_ENABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#endif
#if !defined(STM32G4)
#define __HAL_RCC_ADC5_FORCE_RESET(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#endif
#if !defined(STM32G4)
#define __HAL_RCC_ADC5_RELEASE_RESET(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#endif
#define __HAL_RCC_ADC_CLK_DISABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_ADC_CLK_ENABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_ADC_FORCE_RESET(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_ADC_RELEASE_RESET(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_CAN1_CLK_DISABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_CAN1_CLK_ENABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_CAN1_FORCE_RESET(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_CAN1_RELEASE_RESET(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_CAN2_CLK_DISABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_CAN2_CLK_ENABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_CAN2_FORCE_RESET(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_CAN2_RELEASE_RESET(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_CAN3_CLK_DISABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_CAN3_CLK_ENABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_CAN3_FORCE_RESET(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_CAN3_RELEASE_RESET(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_CLEAR_RESET_FLAGS(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#if !defined(STM32F4) && !defined(STM32F7) && !defined(STM32L0)
#define __HAL_RCC_DAC1_CLK_DISABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#endif
#if !defined(STM32F4) && !defined(STM32F7) && !defined(STM32L0)
#define __HAL_RCC_DAC1_CLK_ENABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#endif
#if !defined(STM32F4) && !defined(STM32F7) && !defined(STM32L0)
#define __HAL_RCC_DAC1_FORCE_RESET(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#endif
#if !defined(STM32F4) && !defined(STM32F7) && !defined(STM32L0)
#define __HAL_RCC_DAC1_RELEASE_RESET(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#endif
#define __HAL_RCC_DAC2_CLK_DISABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_DAC2_CLK_ENABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_DAC2_FORCE_RESET(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_DAC2_RELEASE_RESET(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_DAC3_CLK_DISABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_DAC3_CLK_ENABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_DAC3_FORCE_RESET(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_DAC3_RELEASE_RESET(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_DAC4_CLK_DISABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_DAC4_CLK_ENABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_DAC4_FORCE_RESET(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_DAC4_RELEASE_RESET(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_DAC_CLK_DISABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_DAC_CLK_ENABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_DAC_FORCE_RESET(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_DAC_RELEASE_RESET(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_DMA1_CLK_DISABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_DMA1_CLK_ENABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_DMA1_FORCE_RESET(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_DMA1_RELEASE_RESET(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_DMA2_CLK_DISABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_DMA2_CLK_ENABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_DMA2_FORCE_RESET(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_DMA2_RELEASE_RESET(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_DMA3_CLK_DISABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_DMA3_CLK_ENABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_DMA3_FORCE_RESET(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_DMA3_RELEASE_RESET(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_DMAMUX1_CLK_DISABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_DMAMUX1_CLK_ENABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_DMAMUX1_FORCE_RESET(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_DMAMUX1_RELEASE_RESET(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_FDCAN_CLK_DISABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_FDCAN_CLK_ENABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_FDCAN_FORCE_RESET(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_FDCAN_RELEASE_RESET(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_GPDMA1_CLK_DISABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_GPDMA1_CLK_ENABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_GPDMA1_FORCE_RESET(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_GPDMA1_RELEASE_RESET(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_GPIOA_CLK_ENABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_GPIOB_CLK_ENABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_GPIOC_CLK_ENABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_GPIOD_CLK_ENABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_GPIOE_CLK_ENABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_GPIOF_CLK_ENABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_GPIOG_CLK_ENABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_GPIOH_CLK_ENABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_GPIOI_CLK_ENABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_GPIOJ_CLK_ENABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_GPIOK_CLK_ENABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_GPIOL_CLK_ENABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_GPIOM_CLK_ENABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_GPION_CLK_ENABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_I2C1_CLK_DISABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_I2C1_CLK_ENABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_I2C1_FORCE_RESET(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_I2C1_RELEASE_RESET(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_I2C2_CLK_DISABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_I2C2_CLK_ENABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_I2C2_FORCE_RESET(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_I2C2_RELEASE_RESET(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_I2C3_CLK_DISABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_I2C3_CLK_ENABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_I2C3_FORCE_RESET(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_I2C3_RELEASE_RESET(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_I2C4_CLK_DISABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_I2C4_CLK_ENABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_I2C4_FORCE_RESET(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_I2C4_RELEASE_RESET(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_I2C5_CLK_DISABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_I2C5_CLK_ENABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_I2C5_FORCE_RESET(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_I2C5_RELEASE_RESET(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_I2C6_CLK_DISABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_I2C6_CLK_ENABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_I2C6_FORCE_RESET(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_I2C6_RELEASE_RESET(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_LPUART1_CLK_DISABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_LPUART1_CLK_ENABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_LPUART1_FORCE_RESET(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_LPUART1_RELEASE_RESET(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_LPUART2_CLK_DISABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_LPUART2_CLK_ENABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_LPUART2_FORCE_RESET(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_LPUART2_RELEASE_RESET(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_LSEDRIVE_CONFIG(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_PWR_CLK_ENABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_RTC_DISABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_RTC_ENABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_SAI1_CLK_DISABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_SAI1_CLK_ENABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_SAI1_FORCE_RESET(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_SAI1_RELEASE_RESET(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_SAI2_CLK_DISABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_SAI2_CLK_ENABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_SAI2_FORCE_RESET(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_SAI2_RELEASE_RESET(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_SDMMC1_CLK_DISABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_SDMMC1_CLK_ENABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_SDMMC1_FORCE_RESET(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_SDMMC1_RELEASE_RESET(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_SDMMC2_CLK_DISABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_SDMMC2_CLK_ENABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_SDMMC2_FORCE_RESET(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_SDMMC2_RELEASE_RESET(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_SPI1_CLK_DISABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_SPI1_CLK_ENABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_SPI1_FORCE_RESET(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_SPI1_RELEASE_RESET(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_SPI2_CLK_DISABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_SPI2_CLK_ENABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_SPI2_FORCE_RESET(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_SPI2_RELEASE_RESET(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_SPI3_CLK_DISABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_SPI3_CLK_ENABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_SPI3_FORCE_RESET(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_SPI3_RELEASE_RESET(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_SPI4_CLK_DISABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_SPI4_CLK_ENABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_SPI4_FORCE_RESET(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_SPI4_RELEASE_RESET(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_SPI5_CLK_DISABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_SPI5_CLK_ENABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_SPI5_FORCE_RESET(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_SPI5_RELEASE_RESET(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_SPI6_CLK_DISABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_SPI6_CLK_ENABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_SPI6_FORCE_RESET(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_SPI6_RELEASE_RESET(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_SYSCFG_CLK_ENABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_TIM10_CLK_DISABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_TIM10_CLK_ENABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_TIM10_FORCE_RESET(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_TIM10_RELEASE_RESET(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_TIM11_CLK_DISABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_TIM11_CLK_ENABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_TIM11_FORCE_RESET(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_TIM11_RELEASE_RESET(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_TIM12_CLK_DISABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_TIM12_CLK_ENABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_TIM12_FORCE_RESET(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_TIM12_RELEASE_RESET(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_TIM13_CLK_DISABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_TIM13_CLK_ENABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_TIM13_FORCE_RESET(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_TIM13_RELEASE_RESET(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_TIM14_CLK_DISABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_TIM14_CLK_ENABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_TIM14_FORCE_RESET(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_TIM14_RELEASE_RESET(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_TIM15_CLK_DISABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_TIM15_CLK_ENABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_TIM15_FORCE_RESET(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_TIM15_RELEASE_RESET(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_TIM16_CLK_DISABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_TIM16_CLK_ENABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_TIM16_FORCE_RESET(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_TIM16_RELEASE_RESET(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_TIM17_CLK_DISABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_TIM17_CLK_ENABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_TIM17_FORCE_RESET(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_TIM17_RELEASE_RESET(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_TIM18_CLK_DISABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_TIM18_CLK_ENABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_TIM18_FORCE_RESET(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_TIM18_RELEASE_RESET(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_TIM19_CLK_DISABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_TIM19_CLK_ENABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_TIM19_FORCE_RESET(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_TIM19_RELEASE_RESET(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_TIM1_CLK_DISABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_TIM1_CLK_ENABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_TIM1_FORCE_RESET(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_TIM1_RELEASE_RESET(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_TIM20_CLK_DISABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_TIM20_CLK_ENABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_TIM20_FORCE_RESET(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_TIM20_RELEASE_RESET(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_TIM21_CLK_DISABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_TIM21_CLK_ENABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_TIM21_FORCE_RESET(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_TIM21_RELEASE_RESET(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_TIM22_CLK_DISABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_TIM22_CLK_ENABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_TIM22_FORCE_RESET(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_TIM22_RELEASE_RESET(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_TIM2_CLK_DISABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_TIM2_CLK_ENABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_TIM2_FORCE_RESET(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_TIM2_RELEASE_RESET(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_TIM3_CLK_DISABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_TIM3_CLK_ENABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_TIM3_FORCE_RESET(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_TIM3_RELEASE_RESET(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_TIM4_CLK_DISABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_TIM4_CLK_ENABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_TIM4_FORCE_RESET(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_TIM4_RELEASE_RESET(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_TIM5_CLK_DISABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_TIM5_CLK_ENABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_TIM5_FORCE_RESET(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_TIM5_RELEASE_RESET(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_TIM6_CLK_DISABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_TIM6_CLK_ENABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_TIM6_FORCE_RESET(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_TIM6_RELEASE_RESET(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_TIM7_CLK_DISABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_TIM7_CLK_ENABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_TIM7_FORCE_RESET(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_TIM7_RELEASE_RESET(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_TIM8_CLK_DISABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_TIM8_CLK_ENABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_TIM8_FORCE_RESET(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_TIM8_RELEASE_RESET(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_TIM9_CLK_DISABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_TIM9_CLK_ENABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_TIM9_FORCE_RESET(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_TIM9_RELEASE_RESET(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_UART10_CLK_DISABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_UART10_CLK_ENABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_UART10_FORCE_RESET(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_UART10_RELEASE_RESET(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_UART1_CLK_DISABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_UART1_CLK_ENABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_UART1_FORCE_RESET(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_UART1_RELEASE_RESET(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_UART2_CLK_DISABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_UART2_CLK_ENABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_UART2_FORCE_RESET(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_UART2_RELEASE_RESET(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_UART3_CLK_DISABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_UART3_CLK_ENABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_UART3_FORCE_RESET(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_UART3_RELEASE_RESET(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_UART4_CLK_DISABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_UART4_CLK_ENABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_UART4_FORCE_RESET(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_UART4_RELEASE_RESET(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_UART5_CLK_DISABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_UART5_CLK_ENABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_UART5_FORCE_RESET(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_UART5_RELEASE_RESET(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_UART6_CLK_DISABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_UART6_CLK_ENABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_UART6_FORCE_RESET(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_UART6_RELEASE_RESET(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_UART7_CLK_DISABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_UART7_CLK_ENABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_UART7_FORCE_RESET(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_UART7_RELEASE_RESET(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_UART8_CLK_DISABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_UART8_CLK_ENABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_UART8_FORCE_RESET(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_UART8_RELEASE_RESET(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_UART9_CLK_DISABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_UART9_CLK_ENABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_UART9_FORCE_RESET(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_UART9_RELEASE_RESET(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_USART10_CLK_DISABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_USART10_CLK_ENABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_USART10_FORCE_RESET(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_USART10_RELEASE_RESET(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_USART1_CLK_DISABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_USART1_CLK_ENABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_USART1_FORCE_RESET(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_USART1_RELEASE_RESET(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_USART2_CLK_DISABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_USART2_CLK_ENABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_USART2_FORCE_RESET(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_USART2_RELEASE_RESET(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_USART3_CLK_DISABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_USART3_CLK_ENABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_USART3_FORCE_RESET(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_USART3_RELEASE_RESET(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_USART4_CLK_DISABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_USART4_CLK_ENABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_USART4_FORCE_RESET(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_USART4_RELEASE_RESET(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_USART5_CLK_DISABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_USART5_CLK_ENABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_USART5_FORCE_RESET(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_USART5_RELEASE_RESET(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_USART6_CLK_DISABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_USART6_CLK_ENABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_USART6_FORCE_RESET(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_USART6_RELEASE_RESET(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_USART7_CLK_DISABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_USART7_CLK_ENABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_USART7_FORCE_RESET(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_USART7_RELEASE_RESET(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_USART8_CLK_DISABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_USART8_CLK_ENABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_USART8_FORCE_RESET(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_USART8_RELEASE_RESET(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_USART9_CLK_DISABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_USART9_CLK_ENABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_USART9_FORCE_RESET(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_USART9_RELEASE_RESET(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_USB_OTG_FS_CLK_DISABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_USB_OTG_FS_CLK_ENABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_USB_OTG_FS_FORCE_RESET(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_USB_OTG_FS_RELEASE_RESET(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_USB_OTG_HS_CLK_DISABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_USB_OTG_HS_CLK_ENABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_USB_OTG_HS_FORCE_RESET(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RCC_USB_OTG_HS_RELEASE_RESET(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_RTC_RESET_HANDLE_STATE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_SAI_CLEAR_FLAG(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_SAI_DISABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_SAI_DISABLE_IT(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_SAI_ENABLE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_SAI_ENABLE_IT(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_TIM_GET_COUNTER(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_TIM_SET_COMPARE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_TIM_SET_COUNTER(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_UART_DISABLE_IT(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_UART_ENABLE_IT(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_UART_GET_FLAG(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __HAL_UART_GET_IT_SOURCE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __LL_ADC_CALC_DATA_TO_VOLTAGE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#define __LL_ADC_CALC_TEMPERATURE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#if !defined(STM32F1)
#define __LL_ADC_CALC_VREFANALOG_VOLTAGE(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)
#endif
#define __LL_DAC_CALC_VOLTAGE_TO_DATA(...) ALX_SHIM_STM32_CALL(__VA_ARGS__)


//******************************************************************************
// The callbacks and interrupt handlers the alx code defines - the HAL declares them
//******************************************************************************
void EXTI0_IRQHandler(void);
void EXTI10_IRQHandler(void);
void EXTI11_IRQHandler(void);
void EXTI12_IRQHandler(void);
void EXTI13_IRQHandler(void);
void EXTI14_IRQHandler(void);
void EXTI15_10_IRQHandler(void);
void EXTI15_IRQHandler(void);
void EXTI1_IRQHandler(void);
void EXTI2_IRQHandler(void);
void EXTI3_IRQHandler(void);
void EXTI4_IRQHandler(void);
void EXTI5_IRQHandler(void);
void EXTI6_IRQHandler(void);
void EXTI7_IRQHandler(void);
void EXTI8_IRQHandler(void);
void EXTI9_5_IRQHandler(void);
void EXTI9_IRQHandler(void);
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan);
void HAL_CAN_TxMailbox0CompleteCallback(CAN_HandleTypeDef *hcan);
void HAL_CAN_TxMailbox1CompleteCallback(CAN_HandleTypeDef *hcan);
void HAL_CAN_TxMailbox2CompleteCallback(CAN_HandleTypeDef *hcan);
void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hcan, uint32_t RxFifo0ITs);
void HAL_FDCAN_TxFifoEmptyCallback(FDCAN_HandleTypeDef *hcan);
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin);
void HAL_GPIO_EXTI_Falling_Callback(uint16_t GPIO_Pin);
void HAL_GPIO_EXTI_Rising_Callback(uint16_t GPIO_Pin);
void HAL_HCD_Connect_Callback(HCD_HandleTypeDef* hhcd);
void HAL_HCD_Disconnect_Callback(HCD_HandleTypeDef* hhcd);
void HAL_HCD_PortDisabled_Callback(HCD_HandleTypeDef* hhcd);
void HAL_HCD_PortEnabled_Callback(HCD_HandleTypeDef* hhcd);
void HAL_HCD_SOF_Callback(HCD_HandleTypeDef* hhcd);
void HAL_MMC_RxCpltCallback(MMC_HandleTypeDef *hmmc);
void HAL_MMC_TxCpltCallback(MMC_HandleTypeDef *hmmc);
void HAL_RCC_CSSCallback(void);
void HAL_RTC_MspDeInit(RTC_HandleTypeDef *hrtc);
void HAL_RTC_MspInit(RTC_HandleTypeDef *hrtc);

#endif	// ALX_SHIM_STM32_STM32HOSTSHIM_H
