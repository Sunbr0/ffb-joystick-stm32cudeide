/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : usbd_custom_hid_if.c
  * @version        : v1.0_Cube
  * @brief          : USB Device Custom HID interface file.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "usbd_custom_hid_if.h"

/* USER CODE BEGIN INCLUDE */
#include "logger.h"
/* USER CODE END INCLUDE */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/

/* USER CODE END PV */

/** @addtogroup STM32_USB_OTG_DEVICE_LIBRARY
  * @brief Usb device.
  * @{
  */

/** @addtogroup USBD_CUSTOM_HID
  * @{
  */

/** @defgroup USBD_CUSTOM_HID_Private_TypesDefinitions USBD_CUSTOM_HID_Private_TypesDefinitions
  * @brief Private types.
  * @{
  */

/* USER CODE BEGIN PRIVATE_TYPES */
typedef struct __attribute__((packed))
	{ // FFB: PID Pool Feature Report
	uint8_t		reportId;	// =3
	uint16_t	ramPoolSize;	// ?
	uint8_t		maxSimultaneousEffects;	// ?? 40?
	uint8_t		memoryManagement;	// Bits: 0=DeviceManagedPool, 1=SharedParameterBlocks
	} USB_FFBReport_PIDPool_Feature_Data_t;
USB_FFBReport_PIDPool_Feature_Data_t pool_report;

typedef struct __attribute__((packed))
	{ // FFB: Create New Effect Report
	uint8_t		reportId;	// =3
	uint8_t		effectType;	// ?
	uint16_t	byteCount;	// ?? 40?
	} USB_FFBReport_CreateNewEffect_Feature_Data_t;
	USB_FFBReport_CreateNewEffect_Feature_Data_t create_new_effect_report;

typedef struct
	{ // FFB: PID Block Load Feature Report
	uint8_t	reportId;	// =2
	uint8_t effectBlockIndex;	// 1..40
	uint8_t	loadStatus;	// 1=Success,2=Full,3=Error
	uint16_t	ramPoolAvailable;	// =0 or 0xFFFF?
	} USB_FFBReport_PIDBlockLoad_Feature_Data_t;
	USB_FFBReport_PIDBlockLoad_Feature_Data_t block_load_report;
/* USER CODE END PRIVATE_TYPES */

/**
  * @}
  */

/** @defgroup USBD_CUSTOM_HID_Private_Defines USBD_CUSTOM_HID_Private_Defines
  * @brief Private defines.
  * @{
  */

/* USER CODE BEGIN PRIVATE_DEFINES */
#define FEATURE 0x03
#define USB_EFFECT_CONSTANT		0x01
#define USB_EFFECT_RAMP			0x02
#define USB_EFFECT_SQUARE 		0x03
#define USB_EFFECT_SINE 		0x04
#define USB_EFFECT_TRIANGLE		0x05
#define USB_EFFECT_SAWTOOTHDOWN	0x06
#define USB_EFFECT_SAWTOOTHUP	0x07
#define USB_EFFECT_SPRING		0x08
#define USB_EFFECT_DAMPER		0x09
#define USB_EFFECT_INERTIA		0x0A
#define USB_EFFECT_FRICTION		0x0B
#define USB_EFFECT_CUSTOM		0x0C
/* USER CODE END PRIVATE_DEFINES */

/**
  * @}
  */

/** @defgroup USBD_CUSTOM_HID_Private_Macros USBD_CUSTOM_HID_Private_Macros
  * @brief Private macros.
  * @{
  */

/* USER CODE BEGIN PRIVATE_MACRO */

/* USER CODE END PRIVATE_MACRO */

/**
  * @}
  */

/** @defgroup USBD_CUSTOM_HID_Private_Variables USBD_CUSTOM_HID_Private_Variables
  * @brief Private variables.
  * @{
  */

/** Usb HID report descriptor. */
__ALIGN_BEGIN static uint8_t CUSTOM_HID_ReportDesc_FS[USBD_CUSTOM_HID_REPORT_DESC_SIZE] __ALIGN_END =
{
  /* USER CODE BEGIN 0 */
		0x05,0x01,	// USAGE_PAGE (Generic Desktop)
		0x09,0x04,	// USAGE (04)
		0xA1,0x01,	// COLLECTION (Application)
			0x85,0x01,	// REPORT_ID (1)

			// FFP input descriptor:
		    0x09, 0x01,       // USAGE (Pointer)
		    0xA1, 0x00,       // COLLECTION (Physical)
		    0x09, 0x30,       // USAGE (X)
		    0x09, 0x31,       // USAGE (Y)
		    0x09, 0x32,       // USAGE (Z)
		    0x15, 0x00,       // LOGICAL_MINIMUM (0)
		    0x26, 0xFF, 0x01, // LOGICAL_MAXIMUM (511)
		    0x35, 0x00,       // PHYSICAL_MINIMUM (0)
		    0x46, 0xFF, 0x03, // PHYSICAL_MAXIMUM (1023)
		    0x95, 0x03,       // REPORT_COUNT (3)
		    0x75, 0x10,       // REPORT_SIZE (16 bits)
		    0x81, 0x02,       // INPUT (Data, Variable, Absolute)

				0x09, 0x35,       // USAGE (Rz)
				0x15, 0x00,       // LOGICAL_MINIMUM (0)
				0x25, 0x3F,       // LOGICAL_MAXIMUM (63)
				0x35, 0x00,       // PHYSICAL_MINIMUM (0)
				0x45, 0x2E,       // PHYSICAL_MAXIMUM (46)
				0x95, 0x01,       // REPORT_COUNT (1)
				0x75, 0x06,       // REPORT_SIZE (6 bits)
				0x81, 0x02,       // INPUT (Data, Variable, Absolute)

				0x75, 0x02,		//     REPORT_SIZE (2)
				0x95, 0x01,		//     REPORT_COUNT (1)
				0x81, 0x01,		//     INPUT (Cnst,Ary,Abs)		 2b Fill


				0x65, 0x00,		//     UNIT (None)

				0x09, 0x33,     //   USAGE (Rx)
				0x09, 0x34,		//   USAGE (Ry)
			    0x15, 0x00,       // LOGICAL_MINIMUM (0)
				0x26, 0xFF, 0x00, // LOGICAL_MAXIMUM (255)
				0x35, 0x00,       // PHYSICAL_MINIMUM (0)
				0x45, 0xFF,       // PHYSICAL_MAXIMUM (255)
				0x75, 0x08,       // REPORT_SIZE (8 bits)
				0x95, 0x02,       // REPORT_COUNT (2)
				0x81, 0x02,       // INPUT (Data, Variable, Absolute)

		    0xC0,             // END_COLLECTION

			0x09, 0x36,		//   USAGE (Rudder - hmm...actual rudder code 0xBA does not seem to work in Windows - so use slider)
			0x15, 0x81,       // LOGICAL_MINIMUM (-128)
		    0x25, 0x7F,       // LOGICAL_MAXIMUM (127)
		    0x45, 0xFF,       // PHYSICAL_MAXIMUM (255)
		    0x75, 0x08,       // REPORT_SIZE (8 bits)
		    0x95, 0x01,       // REPORT_COUNT (1)
			0x81, 0x02,       // INPUT (Data, Variable, Absolute)

			0x05, 0x02,                    //   USAGE_PAGE (Simulation Controls)

			0x09, 0xbb,		//   USAGE (Throttle)
		    0x15, 0xC0,       // LOGICAL_MINIMUM (-64)
		    0x25, 0x3F,       // LOGICAL_MAXIMUM (63)
		    0x45, 0x7F,       // PHYSICAL_MAXIMUM (127)
		    0x75, 0x08,       // REPORT_SIZE (8 bits)
		    0x95, 0x01,       // REPORT_COUNT (1)
			0x81, 0x02,       // INPUT (Data, Variable, Absolute)		 // Throttle

		    0x05, 0x09,       // USAGE_PAGE (Button)
		    0x19, 0x01,       // USAGE_MINIMUM (Button 1)
		    0x29, 0x08,       // USAGE_MAXIMUM (Button 8)
		    0x15, 0x00,       // LOGICAL_MINIMUM (0)
		    0x25, 0x01,       // LOGICAL_MAXIMUM (1)
			0x45, 0x01,		//     PHYSICAL_MAXIMUM (1)
		    0x75, 0x01,       // REPORT_SIZE (1 bit)
		    0x95, 0x08,       // REPORT_COUNT (8)
		    0x81, 0x02,       // INPUT (Data, Variable, Absolute)

		    0x19, 0x09,       // USAGE_MINIMUM (Button 9)
		    0x29, 0x10,       // USAGE_MAXIMUM (Button 16)
		    0x75, 0x01,       // REPORT_SIZE (1 bit)
		    0x95, 0x08,       // REPORT_COUNT (8)
		    0x81, 0x02,       // INPUT (Data, Variable, Absolute)

			0x05, 0x01,       // USAGE_PAGE (Generic Desktop)
			0x09, 0x39,		//     USAGE (Hat switch)
			0x95, 0x01,		//     REPORT_COUNT (1)
			0x75, 0x04,		//     REPORT_SIZE (4)
			0x15, 0x00,		//     LOGICAL_MINIMUM (0)
			0x25, 0x07,		//     LOGICAL_MAXIMUM (7)
			0x46, 0x3B, 0x01,	//     PHYSICAL_MAXIMUM (315)
			0x65, 0x14,		//     UNIT (Eng Rot:Angular Pos)
			0x81, 0x42,		//     INPUT (Data,Var,Abs,Null)	 4b Hat

			0x75, 0x04,		//     REPORT_SIZE (4)
			0x95, 0x01,		//     REPORT_COUNT (1)
			0x81, 0x01,		//     INPUT (Cnst,Ary,Abs)		 4b Fill

			0x55, 0x00, 	// ( UNIT_EXPONENT ( 0))
			0x65, 0x00, 	// ( UNIT ( None))
		/*
		// FF2 input descriptor:

			0x09,0x01,	// USAGE (01)
			0xA1,0x00,	// COLLECTION (Physical)
				0x66,0x00,0x00,	// UNIT (None)
				0x55,0x00,	// UNIT_EXPONENT (00)
				0x16,0x00,0xFE,	// LOGICAL_MINIMUM (-512)
				0x26,0xFF,0x01,	// LOGICAL_MAXIMUM (512)
				0x35,0x00,	// PHYSICAL_MINIMUM (00)
				0x46,0xFF,0x03,	// PHYSICAL_MAXIMUM (1023)
				0x95,0x01,	// REPORT_COUNT (01)
				0x75,0x0A,	// REPORT_SIZE (0A)
				0x09,0x30,	// USAGE (X)
				0x81,0x02,	// INPUT (Data,Var,Abs)
				0x75,0x06,	// REPORT_SIZE (06)
				0x81,0x01,	// INPUT (Constant,Ary,Abs)
				0x09,0x31,	// USAGE (Y)
				0x75,0x0A,	// REPORT_SIZE (0A)
				0x81,0x02,	// INPUT (Data,Var,Abs)
				0x75,0x06,	// REPORT_SIZE (06)
				0x95,0x01,	// REPORT_COUNT (01)
				0x81,0x01,	// INPUT (Constant,Ary,Abs)
				0x15,0xE0,	// LOGICAL_MINIMUM (-32)
				0x25,0x1F,	// LOGICAL_MAXIMUM (31)
				0x35,0x00,	// PHYSICAL_MINIMUM (00)
				0x45,0x3F,	// PHYSICAL_MAXIMUM (63)
				0x66,0x14,0x00,	// UNIT (Eng Rot:Angular Pos)
				0x09,0x35,	// USAGE (Rz)
				0x81,0x02,	// INPUT (Data,Var,Abs)
				0x75,0x02,	// REPORT_SIZE (02)
				0x81,0x01,	// INPUT (Constant,Ary,Abs)
				0x66,0x00,0x00,	// UNIT (None)
			0xC0,	// END COLLECTION ()
			0x75,0x07,	// REPORT_SIZE (07)
			0x95,0x01,	// REPORT_COUNT (01)
			0x15,0x00,	// LOGICAL_MINIMUM (00)
			0x25,0x7F,	// LOGICAL_MAXIMUM (7F)
			0x35,0x00,	// PHYSICAL_MINIMUM (00)
			0x45,0x7F,	// PHYSICAL_MAXIMUM (7F)
			0x09,0x36,	// USAGE (Slider)
			0x81,0x02,	// INPUT (Data,Var,Abs)
			0x75,0x01,	// REPORT_SIZE (01)
			0x81,0x01,	// INPUT (Constant,Ary,Abs)
			0x09,0x39,	// USAGE (Hat switch)
			0x15,0x00,	// LOGICAL_MINIMUM (00)
			0x25,0x07,	// LOGICAL_MAXIMUM (07)
			0x35,0x00,	// PHYSICAL_MINIMUM (00)
			0x46,0x3B,0x01,	// PHYSICAL_MAXIMUM (315)
			0x66,0x14,0x00,	// UNIT (Eng Rot:Angular Pos)
			0x75,0x04,	// REPORT_SIZE (04)
			0x95,0x01,	// REPORT_COUNT (01)
			0x81,0x42,	// INPUT (Data,Var,Abs,Null)
			0x95,0x01,	// REPORT_COUNT (01)
			0x81,0x01,	// INPUT (Constant,Ary,Abs)
			0x66,0x00,0x00,	// UNIT (None)
			0x05,0x09,	// USAGE_PAGE (Button)
			0x19,0x01,	// USAGE_MINIMUM (Button 1)
			0x29,0x08,	// USAGE_MAXIMUM (Button 8)
			0x15,0x00,	// LOGICAL_MINIMUM (00)
			0x25,0x01,	// LOGICAL_MAXIMUM (01)
			0x35,0x00,	// PHYSICAL_MINIMUM (00)
			0x45,0x01,	// PHYSICAL_MAXIMUM (01)
			0x95,0x08,	// REPORT_COUNT (08)
			0x75,0x01,	// REPORT_SIZE (01)
			0x81,0x02,	// INPUT (Data,Var,Abs)
			0x95,0x04,	// REPORT_COUNT (04)
			0x75,0x08,	// REPORT_SIZE (08)
			0x81,0x01,	// INPUT (Constant,Ary,Abs)
		*/

			0x05,0x0F,	// USAGE_PAGE (Physical Interface)
			0x09,0x92,	// USAGE (PID State Report)
			0xA1,0x02,	// COLLECTION (Logical)
				0x85,0x02,	// REPORT_ID (02)
				0x09,0x9F,	// USAGE (Device Paused)
				0x09,0xA0,	// USAGE (Actuators Enabled)
				0x09,0xA4,	// USAGE (Safety Switch)
				0x09,0xA5,	// USAGE (Actuator Override Switch)
				0x09,0xA6,	// USAGE (Actuator Power)
				0x15,0x00,	// LOGICAL_MINIMUM (00)
				0x25,0x01,	// LOGICAL_MINIMUM (01)
				0x35,0x00,	// PHYSICAL_MINIMUM (00)
				0x45,0x01,	// PHYSICAL_MAXIMUM (01)
				0x75,0x01,	// REPORT_SIZE (01)
				0x95,0x05,	// REPORT_COUNT (05)
				0x81,0x02,	// INPUT (Data,Var,Abs)
				0x95,0x03,	// REPORT_COUNT (03)
				0x81,0x03,	// INPUT (Constant,Var,Abs)
				0x09,0x94,	// USAGE (Effect Playing)
				0x15,0x00,	// LOGICAL_MINIMUM (00)
				0x25,0x01,	// LOGICAL_MAXIMUM (01)
				0x35,0x00,	// PHYSICAL_MINIMUM (00)
				0x45,0x01,	// PHYSICAL_MAXIMUM (01)
				0x75,0x01,	// REPORT_SIZE (01)
				0x95,0x01,	// REPORT_COUNT (01)
				0x81,0x02,	// INPUT (Data,Var,Abs)
				0x09,0x22,	// USAGE (Effect Block Index)
				0x15,0x01,	// LOGICAL_MINIMUM (01)
				0x25,0x28,	// LOGICAL_MAXIMUM (28)
				0x35,0x01,	// PHYSICAL_MINIMUM (01)
				0x45,0x28,	// PHYSICAL_MAXIMUM (28)
				0x75,0x07,	// REPORT_SIZE (07)
				0x95,0x01,	// REPORT_COUNT (01)
				0x81,0x02,	// INPUT (Data,Var,Abs)
			0xC0,	// END COLLECTION ()

			0x09,0x21,	// USAGE (Set Effect Report)
			0xA1,0x02,	// COLLECTION (Logical)
				0x85,0x01,	// REPORT_ID (01)
				0x09,0x22,	// USAGE (Effect Block Index)
				0x15,0x01,	// LOGICAL_MINIMUM (01)
				0x25,0x28,	// LOGICAL_MAXIMUM (28)
				0x35,0x01,	// PHYSICAL_MINIMUM (01)
				0x45,0x28,	// PHYSICAL_MAXIMUM (28)
				0x75,0x08,	// REPORT_SIZE (08)
				0x95,0x01,	// REPORT_COUNT (01)
				0x91,0x02,	// OUTPUT (Data,Var,Abs)
				0x09,0x25,	// USAGE (25)
				0xA1,0x02,	// COLLECTION (Logical)
					0x09,0x26,	// USAGE (26)
					0x09,0x27,	// USAGE (27)
					0x09,0x30,	// USAGE (30)
					0x09,0x31,	// USAGE (31)
					0x09,0x32,	// USAGE (32)
					0x09,0x33,	// USAGE (33)
					0x09,0x34,	// USAGE (34)
					0x09,0x40,	// USAGE (40)
					0x09,0x41,	// USAGE (41)
					0x09,0x42,	// USAGE (42)
					0x09,0x43,	// USAGE (43)
					0x09,0x28,	// USAGE (28)
					0x25,0x0C,	// LOGICAL_MAXIMUM (0C)
					0x15,0x01,	// LOGICAL_MINIMUM (01)
					0x35,0x01,	// PHYSICAL_MINIMUM (01)
					0x45,0x0C,	// PHYSICAL_MAXIMUM (0C)
					0x75,0x08,	// REPORT_SIZE (08)
					0x95,0x01,	// REPORT_COUNT (01)
					0x91,0x00,	// OUTPUT (Data)
				0xC0,	// END COLLECTION ()
				0x09,0x50,	// USAGE (Duration)
				0x09,0x54,	// USAGE (Trigger Repeat Interval)
				0x09,0x51,	// USAGE (Sample Period)
				0x15,0x00,	// LOGICAL_MINIMUM (00)
				0x26,0xFF,0x7F,	// LOGICAL_MAXIMUM (7F FF)
				0x35,0x00,	// PHYSICAL_MINIMUM (00)
				0x46,0xFF,0x7F,	// PHYSICAL_MAXIMUM (7F FF)
				0x66,0x03,0x10,	// UNIT (Eng Lin:Time)
				0x55,0xFD,	// UNIT_EXPONENT (-3)
				0x75,0x10,	// REPORT_SIZE (10)
				0x95,0x03,	// REPORT_COUNT (03)
				0x91,0x02,	// OUTPUT (Data,Var,Abs)
				0x55,0x00,	// UNIT_EXPONENT (00)
				0x66,0x00,0x00,	// UNIT (None)
				0x09,0x52,	// USAGE (Gain)
				0x15,0x00,	// LOGICAL_MINIMUM (00)
				0x26,0xFF,0x00,	// LOGICAL_MAXIMUM (00 FF)
				0x35,0x00,	// PHYSICAL_MINIMUM (00)
				0x46,0x10,0x27,	// PHYSICAL_MAXIMUM (10000)
				0x75,0x08,	// REPORT_SIZE (08)
				0x95,0x01,	// REPORT_COUNT (01)
				0x91,0x02,	// OUTPUT (Data,Var,Abs)
				0x09,0x53,	// USAGE (Trigger Button)
				0x15,0x01,	// LOGICAL_MINIMUM (01)
				0x25,0x08,	// LOGICAL_MAXIMUM (08)
				0x35,0x01,	// PHYSICAL_MINIMUM (01)
				0x45,0x08,	// PHYSICAL_MAXIMUM (08)
				0x75,0x08,	// REPORT_SIZE (08)
				0x95,0x01,	// REPORT_COUNT (01)
				0x91,0x02,	// OUTPUT (Data,Var,Abs)
				0x09,0x55,	// USAGE (Axes Enable)
				0xA1,0x02,	// COLLECTION (Logical)
					0x05,0x01,	// USAGE_PAGE (Generic Desktop)
					0x09,0x30,	// USAGE (X)
					0x09,0x31,	// USAGE (Y)
					0x15,0x00,	// LOGICAL_MINIMUM (00)
					0x25,0x01,	// LOGICAL_MAXIMUM (01)
					0x75,0x01,	// REPORT_SIZE (01)
					0x95,0x02,	// REPORT_COUNT (02)
					0x91,0x02,	// OUTPUT (Data,Var,Abs)
				0xC0,	// END COLLECTION ()
				0x05,0x0F,	// USAGE_PAGE (Physical Interface)
				0x09,0x56,	// USAGE (Direction Enable)
				0x95,0x01,	// REPORT_COUNT (01)
				0x91,0x02,	// OUTPUT (Data,Var,Abs)
				0x95,0x05,	// REPORT_COUNT (05)
				0x91,0x03,	// OUTPUT (Constant,Var,Abs)
				0x09,0x57,	// USAGE (Direction)
				0xA1,0x02,	// COLLECTION (Logical)
					0x0B,0x01,0x00,0x0A,0x00,
					0x0B,0x02,0x00,0x0A,0x00,
					0x66,0x14,0x00,	// UNIT (Eng Rot:Angular Pos)
					0x55,0xFE,	// UNIT_EXPONENT (FE)
					0x15,0x00,	// LOGICAL_MINIMUM (00)
					0x26,0xB4,0x00,	// LOGICAL_MAXIMUM (00 B4)
					0x35,0x00,	// PHYSICAL_MINIMUM (00)
					0x47,0xA0,0x8C,0x00,0x00,	// PHYSICAL_MAXIMUM (00 00 8C A0)
					0x66,0x00,0x00,	// UNIT (None)
					0x75,0x08,	// REPORT_SIZE (08)
					0x95,0x02,	// REPORT_COUNT (02)
					0x91,0x02,	// OUTPUT (Data,Var,Abs)
					0x55,0x00,	// UNIT_EXPONENT (00)
					0x66,0x00,0x00,	// UNIT (None)
				0xC0,	// END COLLECTION ()
				0x05,0x0F,	// USAGE_PAGE (Physical Interface)
		//		0x09,0xA7,	// USAGE (Start Delay)
				0x66,0x03,0x10,	// UNIT (Eng Lin:Time)
				0x55,0xFD,	// UNIT_EXPONENT (-3)
				0x15,0x00,	// LOGICAL_MINIMUM (00)
				0x26,0xFF,0x7F,	// LOGICAL_MAXIMUM (7F FF)
				0x35,0x00,	// PHYSICAL_MINIMUM (00)
				0x46,0xFF,0x7F,	// PHYSICAL_MAXIMUM (7F FF)
				0x75,0x10,	// REPORT_SIZE (10)
				0x95,0x01,	// REPORT_COUNT (01)
		//		0x91,0x02,	// OUTPUT (Data,Var,Abs)
				0x66,0x00,0x00,	// UNIT (None)
				0x55,0x00,	// UNIT_EXPONENT (00)
			0xC0,	// END COLLECTION ()

			0x05,0x0F,	// USAGE_PAGE (Physical Interface)
			0x09,0x5A,	// USAGE (Set Envelope Report)
			0xA1,0x02,	// COLLECTION (Logical)
				0x85,0x02,	// REPORT_ID (02)
				0x09,0x22,	// USAGE (Effect Block Index)
				0x15,0x01,	// LOGICAL_MINIMUM (01)
				0x25,0x28,	// LOGICAL_MAXIMUM (28)
				0x35,0x01,	// PHYSICAL_MINIMUM (01)
				0x45,0x28,	// PHYSICAL_MAXIMUM (28)
				0x75,0x08,	// REPORT_SIZE (08)
				0x95,0x01,	// REPORT_COUNT (01)
				0x91,0x02,	// OUTPUT (Data,Var,Abs)
				0x09,0x5B,	// USAGE (Attack Level)
				0x09,0x5D,	// USAGE (Fade Level)
				0x15,0x00,	// LOGICAL_MINIMUM (00)
				0x26,0xFF,0x00,	// LOGICAL_MAXIMUM (00 FF)
				0x35,0x00,	// PHYSICAL_MINIMUM (00)
				0x46,0x10,0x27,	// PHYSICAL_MAXIMUM (10000)
				0x95,0x02,	// REPORT_COUNT (02)
				0x91,0x02,	// OUTPUT (Data,Var,Abs)
				0x09,0x5C,	// USAGE (5C)
				0x09,0x5E,	// USAGE (5E)
				0x66,0x03,0x10,	// UNIT (Eng Lin:Time)
				0x55,0xFD,	// UNIT_EXPONENT (-3)
				0x26,0xFF,0x7F,	// LOGICAL_MAXIMUM (7F FF)
				0x46,0xFF,0x7F,	// PHYSICAL_MAXIMUM (7F FF)
				0x75,0x10,	// REPORT_SIZE (10)
				0x91,0x02,	// OUTPUT (Data,Var,Abs)
				0x45,0x00,	// PHYSICAL_MAXIMUM (00)
				0x66,0x00,0x00,	// UNIT (None)
				0x55,0x00,	// UNIT_EXPONENT (00)
			0xC0,	// END COLLECTION ()

			0x09,0x5F,	// USAGE (Set Condition Report)
			0xA1,0x02,	// COLLECTION (Logical)
				0x85,0x03,	// REPORT_ID (03)
				0x09,0x22,	// USAGE (Effect Block Index)
				0x15,0x01,	// LOGICAL_MINIMUM (01)
				0x25,0x28,	// LOGICAL_MAXIMUM (28)
				0x35,0x01,	// PHYSICAL_MINIMUM (01)
				0x45,0x28,	// PHYSICAL_MAXIMUM (28)
				0x75,0x08,	// REPORT_SIZE (08)
				0x95,0x01,	// REPORT_COUNT (01)
				0x91,0x02,	// OUTPUT (Data,Var,Abs)
				0x09,0x23,	// USAGE (Parameter Block Offset)
				0x15,0x00,	// LOGICAL_MINIMUM (00)
				0x25,0x01,	// LOGICAL_MAXIMUM (01)
				0x35,0x00,	// PHYSICAL_MINIMUM (00)
				0x45,0x01,	// PHYSICAL_MAXIMUM (01)
				0x75,0x04,	// REPORT_SIZE (04)
				0x95,0x01,	// REPORT_COUNT (01)
				0x91,0x02,	// OUTPUT (Data,Var,Abs)
				0x09,0x58,	// USAGE (Type Specific Block Offset)
				0xA1,0x02,	// COLLECTION (Logical)
					0x0B,0x01,0x00,0x0A,0x00,	// USAGE (Instance 1)
					0x0B,0x02,0x00,0x0A,0x00,	// USAGE (Instance 2)
					0x75,0x02,	// REPORT_SIZE (02)
					0x95,0x02,	// REPORT_COUNT (02)
					0x91,0x02,	// OUTPUT (Data,Var,Abs)
				0xC0,	// END COLLECTION ()
				0x15,0x80,	// LOGICAL_MINIMUM (80)
				0x25,0x7F,	// LOGICAL_MAXIMUM (7F)
				0x36,0xF0,0xD8,	// PHYSICAL_MINIMUM (-10000)
				0x46,0x10,0x27,	// PHYSICAL_MAXIMUM (10000)
				0x09,0x60,	// USAGE (CP Offset)
				0x75,0x08,	// REPORT_SIZE (08)
				0x95,0x01,	// REPORT_COUNT (01)
				0x91,0x02,	// OUTPUT (Data,Var,Abs)
				0x36,0xF0,0xD8,	// PHYSICAL_MINIMUM (-10000)
				0x46,0x10,0x27,	// PHYSICAL_MAXIMUM (10000)
				0x09,0x61,	// USAGE (Positive Coefficient)
		//		0x09,0x62,	// USAGE (Negative Coefficient)
				0x95,0x01,	// REPORT_COUNT (01)	// ???? WAS 2 with "negative coeff"
				0x91,0x02,	// OUTPUT (Data,Var,Abs)
				0x15,0x00,	// LOGICAL_MINIMUM (00)
				0x26,0xFF,0x00,	// LOGICAL_MAXIMUM (00 FF)
				0x35,0x00,	// PHYSICAL_MINIMUM (00)
				0x46,0x10,0x27,	// PHYSICAL_MAXIMUM (10000)
				0x09,0x63,	// USAGE (Positive Saturation)
				0x09,0x64,	// USAGE (Negative Saturation)
				0x75,0x08,	// REPORT_SIZE (08)
				0x95,0x02,	// REPORT_COUNT (02)
		//		0x91,0x02,	// OUTPUT (Data,Var,Abs)
		//		0x09,0x65,	// USAGE (Dead Band )
				0x46,0x10,0x27,	// PHYSICAL_MAXIMUM (10000)
				0x95,0x01,	// REPORT_COUNT (01)
		//		0x91,0x02,	// OUTPUT (Data,Var,Abs)
			0xC0,	// END COLLECTION ()

			0x09,0x6E,	// USAGE (Set Periodic Report)
			0xA1,0x02,	// COLLECTION (Logical)
				0x85,0x04,	// REPORT_ID (04)
				0x09,0x22,	// USAGE (Effect Block Index)
				0x15,0x01,	// LOGICAL_MINIMUM (01)
				0x25,0x28,	// LOGICAL_MAXIMUM (28)
				0x35,0x01,	// PHYSICAL_MINIMUM (01)
				0x45,0x28,	// PHYSICAL_MAXIMUM (28)
				0x75,0x08,	// REPORT_SIZE (08)
				0x95,0x01,	// REPORT_COUNT (01)
				0x91,0x02,	// OUTPUT (Data,Var,Abs)
				0x09,0x70,	// USAGE (Magnitude)
				0x15,0x00,	// LOGICAL_MINIMUM (00)
				0x26,0xFF,0x00,	// LOGICAL_MAXIMUM (00 FF)
				0x35,0x00,	// PHYSICAL_MINIMUM (00)
				0x46,0x10,0x27,	// PHYSICAL_MAXIMUM (10000)
				0x75,0x08,	// REPORT_SIZE (08)
				0x95,0x01,	// REPORT_COUNT (01)
				0x91,0x02,	// OUTPUT (Data,Var,Abs)
				0x09,0x6F,	// USAGE (Offset)
				0x15,0x80,	// LOGICAL_MINIMUM (80)
				0x25,0x7F,	// LOGICAL_MAXIMUM (7F)
				0x36,0xF0,0xD8,	// PHYSICAL_MINIMUM (-10000)
				0x46,0x10,0x27,	// PHYSICAL_MAXIMUM (10000)
				0x95,0x01,	// REPORT_COUNT (01)
				0x91,0x02,	// OUTPUT (Data,Var,Abs)
				0x09,0x71,	// USAGE (Phase)
				0x66,0x14,0x00,	// UNIT (Eng Rot:Angular Pos)
				0x55,0xFE,	// UNIT_EXPONENT (FE)
				0x15,0x00,	// LOGICAL_MINIMUM (00)
				0x26,0xFF,0x00,	// LOGICAL_MAXIMUM (00 FF)
				0x35,0x00,	// PHYSICAL_MINIMUM (00)
				0x47,0xA0,0x8C,0x00,0x00,	// PHYSICAL_MAXIMUM (00 00 8C A0)
				0x91,0x02,	// OUTPUT (Data,Var,Abs)
				0x09,0x72,	// USAGE (Period)
				0x26,0xFF,0x7F,	// LOGICAL_MAXIMUM (7F FF)
				0x46,0xFF,0x7F,	// PHYSICAL_MAXIMUM (7F FF)
				0x66,0x03,0x10,	// UNIT (Eng Lin:Time)
				0x55,0xFD,	// UNIT_EXPONENT (-3)
				0x75,0x10,	// REPORT_SIZE (10)
				0x95,0x01,	// REPORT_COUNT (01)
				0x91,0x02,	// OUTPUT (Data,Var,Abs)
				0x66,0x00,0x00,	// UNIT (None)
				0x55,0x00,	// UNIT_EXPONENT (00)
			0xC0,	// END COLLECTION ()

			0x09,0x73,	// USAGE (Set Constant Force Report)
			0xA1,0x02,	// COLLECTION (Logical)
				0x85,0x05,	// REPORT_ID (05)
				0x09,0x22,	// USAGE (Effect Block Index)
				0x15,0x01,	// LOGICAL_MINIMUM (01)
				0x25,0x28,	// LOGICAL_MAXIMUM (28)
				0x35,0x01,	// PHYSICAL_MINIMUM (01)
				0x45,0x28,	// PHYSICAL_MAXIMUM (28)
				0x75,0x08,	// REPORT_SIZE (08)
				0x95,0x01,	// REPORT_COUNT (01)
				0x91,0x02,	// OUTPUT (Data,Var,Abs)
				0x09,0x70,	// USAGE (Magnitude)
				0x16,0x01,0xFF,	// LOGICAL_MINIMUM (-255)
				0x26,0xFF,0x00,	// LOGICAL_MAXIMUM (255)
				0x36,0xF0,0xD8,	// PHYSICAL_MINIMUM (-10000)
				0x46,0x10,0x27,	// PHYSICAL_MAXIMUM (10000)
				0x75,0x10,	// REPORT_SIZE (10)
				0x95,0x01,	// REPORT_COUNT (01)
				0x91,0x02,	// OUTPUT (Data,Var,Abs)
			0xC0,	// END COLLECTION ()

			0x09,0x74,	// USAGE (Set Ramp Force Report)
			0xA1,0x02,	// COLLECTION (Logical)
				0x85,0x06,	// REPORT_ID (06)
				0x09,0x22,	// USAGE (Effect Block Index)
				0x15,0x01,	// LOGICAL_MINIMUM (01)
				0x25,0x28,	// LOGICAL_MAXIMUM (28)
				0x35,0x01,	// PHYSICAL_MINIMUM (01)
				0x45,0x28,	// PHYSICAL_MAXIMUM (28)
				0x75,0x08,	// REPORT_SIZE (08)
				0x95,0x01,	// REPORT_COUNT (01)
				0x91,0x02,	// OUTPUT (Data,Var,Abs)
				0x09,0x75,	// USAGE (Ramp Start)
				0x09,0x76,	// USAGE (Ramp End)
				0x15,0x80,	// LOGICAL_MINIMUM (-128)
				0x25,0x7F,	// LOGICAL_MAXIMUM (127)
				0x36,0xF0,0xD8,	// PHYSICAL_MINIMUM (-10000)
				0x46,0x10,0x27,	// PHYSICAL_MAXIMUM (10000)
				0x75,0x08,	// REPORT_SIZE (08)
				0x95,0x02,	// REPORT_COUNT (02)
				0x91,0x02,	// OUTPUT (Data,Var,Abs)
			0xC0,	// END COLLECTION ()

			0x09,0x68,	// USAGE (Custom Force Data Report)
			0xA1,0x02,	// COLLECTION (Logical)
				0x85,0x07,	// REPORT_ID (07)
				0x09,0x22,	// USAGE (Effect Block Index)
				0x15,0x01,	// LOGICAL_MINIMUM (01)
				0x25,0x28,	// LOGICAL_MAXIMUM (28)
				0x35,0x01,	// PHYSICAL_MINIMUM (01)
				0x45,0x28,	// PHYSICAL_MAXIMUM (28)
				0x75,0x08,	// REPORT_SIZE (08)
				0x95,0x01,	// REPORT_COUNT (01)
				0x91,0x02,	// OUTPUT (Data,Var,Abs)
				0x09,0x6C,	// USAGE (Custom Force Data Offset)
				0x15,0x00,	// LOGICAL_MINIMUM (00)
				0x26,0x10,0x27,	// LOGICAL_MAXIMUM (10000)
				0x35,0x00,	// PHYSICAL_MINIMUM (00)
				0x46,0x10,0x27,	// PHYSICAL_MAXIMUM (10000)
				0x75,0x10,	// REPORT_SIZE (10)
				0x95,0x01,	// REPORT_COUNT (01)
				0x91,0x02,	// OUTPUT (Data,Var,Abs)
				0x09,0x69,	// USAGE (Custom Force Data)
				0x15,0x81,	// LOGICAL_MINIMUM (-127)
				0x25,0x7F,	// LOGICAL_MAXIMUM (127)
				0x35,0x00,	// PHYSICAL_MINIMUM (00)
				0x46,0xFF,0x00,	// PHYSICAL_MAXIMUM (255)
				0x75,0x08,	// REPORT_SIZE (08)
				0x95,0x0C,	// REPORT_COUNT (0C)
				0x92,0x02,0x01,	// OUTPUT ( Data,Var,Abs,Buf)
			0xC0,	// END COLLECTION ()

			0x09,0x66,	// USAGE (Download Force Sample)
			0xA1,0x02,	// COLLECTION (Logical)
				0x85,0x08,	// REPORT_ID (08)
				0x05,0x01,	// USAGE_PAGE (Generic Desktop)
				0x09,0x30,	// USAGE (X)
				0x09,0x31,	// USAGE (Y)
				0x15,0x81,	// LOGICAL_MINIMUM (-127)
				0x25,0x7F,	// LOGICAL_MAXIMUM (127)
				0x35,0x00,	// PHYSICAL_MINIMUM (00)
				0x46,0xFF,0x00,	// PHYSICAL_MAXIMUM (255)
				0x75,0x08,	// REPORT_SIZE (08)
				0x95,0x02,	// REPORT_COUNT (02)
				0x91,0x02,	// OUTPUT (Data,Var,Abs)
			0xC0,	// END COLLECTION ()

			0x05,0x0F,	// USAGE_PAGE (Physical Interface)
			0x09,0x77,	// USAGE (Effect Operation Report)
			0xA1,0x02,	// COLLECTION (Logical)
				0x85,0x0A,	// REPORT_ID (0A)
				0x09,0x22,	// USAGE (Effect Block Index)
				0x15,0x01,	// LOGICAL_MINIMUM (01)
				0x25,0x28,	// LOGICAL_MAXIMUM (28)
				0x35,0x01,	// PHYSICAL_MINIMUM (01)
				0x45,0x28,	// PHYSICAL_MAXIMUM (28)
				0x75,0x08,	// REPORT_SIZE (08)
				0x95,0x01,	// REPORT_COUNT (01)
				0x91,0x02,	// OUTPUT (Data,Var,Abs)
				0x09,0x78,	// USAGE (78)
				0xA1,0x02,	// COLLECTION (Logical)
					0x09,0x79,	// USAGE (Op Effect Start)
					0x09,0x7A,	// USAGE (Op Effect Start Solo)
					0x09,0x7B,	// USAGE (Op Effect Stop)
					0x15,0x01,	// LOGICAL_MINIMUM (01)
					0x25,0x03,	// LOGICAL_MAXIMUM (03)
					0x75,0x08,	// REPORT_SIZE (08)
					0x95,0x01,	// REPORT_COUNT (01)
					0x91,0x00,	// OUTPUT (Data,Ary,Abs)
				0xC0,	// END COLLECTION ()
				0x09,0x7C,	// USAGE (Loop Count)
				0x15,0x00,	// LOGICAL_MINIMUM (00)
				0x26,0xFF,0x00,	// LOGICAL_MAXIMUM (00 FF)
				0x35,0x00,	// PHYSICAL_MINIMUM (00)
				0x46,0xFF,0x00,	// PHYSICAL_MAXIMUM (00 FF)
				0x91,0x02,	// OUTPUT (Data,Var,Abs)
			0xC0,	// END COLLECTION ()

			0x09,0x90,	// USAGE (PID Block Free Report)
			0xA1,0x02,	// COLLECTION (Logical)
				0x85,0x0B,	// REPORT_ID (0B)
				0x09,0x22,	// USAGE (Effect Block Index)
				0x25,0x28,	// LOGICAL_MAXIMUM (28)
				0x15,0x01,	// LOGICAL_MINIMUM (01)
				0x35,0x01,	// PHYSICAL_MINIMUM (01)
				0x45,0x28,	// PHYSICAL_MAXIMUM (28)
				0x75,0x08,	// REPORT_SIZE (08)
				0x95,0x01,	// REPORT_COUNT (01)
				0x91,0x02,	// OUTPUT (Data,Var,Abs)
			0xC0,	// END COLLECTION ()

			0x09,0x96,	// USAGE (PID Device Control)
			0xA1,0x02,	// COLLECTION (Logical)
				0x85,0x0C,	// REPORT_ID (0C)
				0x09,0x97,	// USAGE (DC Enable Actuators)
				0x09,0x98,	// USAGE (DC Disable Actuators)
				0x09,0x99,	// USAGE (DC Stop All Effects)
				0x09,0x9A,	// USAGE (DC Device Reset)
				0x09,0x9B,	// USAGE (DC Device Pause)
				0x09,0x9C,	// USAGE (DC Device Continue)
				0x15,0x01,	// LOGICAL_MINIMUM (01)
				0x25,0x06,	// LOGICAL_MAXIMUM (06)
				0x75,0x08,	// REPORT_SIZE (08)
				0x95,0x01,	// REPORT_COUNT (01)
				0x91,0x00,	// OUTPUT (Data)
			0xC0,	// END COLLECTION ()

			0x09,0x7D,	// USAGE (Device Gain Report)
			0xA1,0x02,	// COLLECTION (Logical)
				0x85,0x0D,	// REPORT_ID (0D)
				0x09,0x7E,	// USAGE (Device Gain)
				0x15,0x00,	// LOGICAL_MINIMUM (00)
				0x26,0xFF,0x00,	// LOGICAL_MAXIMUM (00 FF)
				0x35,0x00,	// PHYSICAL_MINIMUM (00)
				0x46,0x10,0x27,	// PHYSICAL_MAXIMUM (10000)
				0x75,0x08,	// REPORT_SIZE (08)
				0x95,0x01,	// REPORT_COUNT (01)
				0x91,0x02,	// OUTPUT (Data,Var,Abs)
			0xC0,	// END COLLECTION ()

			0x09,0x6B,	// USAGE (Set Custom Force Report)
			0xA1,0x02,	// COLLECTION (Logical)
				0x85,0x0E,	// REPORT_ID (0E)
				0x09,0x22,	// USAGE (Effect Block Index)
				0x15,0x01,	// LOGICAL_MINIMUM (01)
				0x25,0x28,	// LOGICAL_MAXIMUM (28)
				0x35,0x01,	// PHYSICAL_MINIMUM (01)
				0x45,0x28,	// PHYSICAL_MAXIMUM (28)
				0x75,0x08,	// REPORT_SIZE (08)
				0x95,0x01,	// REPORT_COUNT (01)
				0x91,0x02,	// OUTPUT (Data,Var,Abs)
				0x09,0x6D,	// USAGE (Sample Count)
				0x15,0x00,	// LOGICAL_MINIMUM (00)
				0x26,0xFF,0x00,	// LOGICAL_MAXIMUM (00 FF)
				0x35,0x00,	// PHYSICAL_MINIMUM (00)
				0x46,0xFF,0x00,	// PHYSICAL_MAXIMUM (00 FF)
				0x75,0x08,	// REPORT_SIZE (08)
				0x95,0x01,	// REPORT_COUNT (01)
				0x91,0x02,	// OUTPUT (Data,Var,Abs)
				0x09,0x51,	// USAGE (Sample Period)
				0x66,0x03,0x10,	// UNIT (Eng Lin:Time)
				0x55,0xFD,	// UNIT_EXPONENT (-3)
				0x15,0x00,	// LOGICAL_MINIMUM (00)
				0x26,0xFF,0x7F,	// LOGICAL_MAXIMUM (32767)
				0x35,0x00,	// PHYSICAL_MINIMUM (00)
				0x46,0xFF,0x7F,	// PHYSICAL_MAXIMUM (32767)
				0x75,0x10,	// REPORT_SIZE (10)
				0x95,0x01,	// REPORT_COUNT (01)
				0x91,0x02,	// OUTPUT (Data,Var,Abs)
				0x55,0x00,	// UNIT_EXPONENT (00)
				0x66,0x00,0x00,	// UNIT (None)
			0xC0,	// END COLLECTION ()

			0x09,0xAB,	// USAGE (Create New Effect Report)
			0xA1,0x02,	// COLLECTION (Logical)
				0x85,0x05,	// REPORT_ID (05)
				0x09,0x25,	// USAGE (Effect Type)
				0xA1,0x02,	// COLLECTION (Logical)
					0x09,0x26,	// USAGE (26)
					0x09,0x27,	// USAGE (27)
					0x09,0x30,	// USAGE (30)
					0x09,0x31,	// USAGE (31)
					0x09,0x32,	// USAGE (32)
					0x09,0x33,	// USAGE (33)
					0x09,0x34,	// USAGE (34)
					0x09,0x40,	// USAGE (40)
					0x09,0x41,	// USAGE (41)
					0x09,0x42,	// USAGE (42)
					0x09,0x43,	// USAGE (43)
					0x09,0x28,	// USAGE (28)
					0x25,0x0C,	// LOGICAL_MAXIMUM (0C)
					0x15,0x01,	// LOGICAL_MINIMUM (01)
					0x35,0x01,	// PHYSICAL_MINIMUM (01)
					0x45,0x0C,	// PHYSICAL_MAXIMUM (0C)
					0x75,0x08,	// REPORT_SIZE (08)
					0x95,0x01,	// REPORT_COUNT (01)
					0xB1,0x00,	// FEATURE (Data)
				0xC0,	// END COLLECTION ()
				0x05,0x01,	// USAGE_PAGE (Generic Desktop)
				0x09,0x3B,	// USAGE (Byte Count)
				0x15,0x00,	// LOGICAL_MINIMUM (00)
				0x26,0xFF,0x01,	// LOGICAL_MAXIMUM (511)
				0x35,0x00,	// PHYSICAL_MINIMUM (00)
				0x46,0xFF,0x01,	// PHYSICAL_MAXIMUM (511)
				0x75,0x0A,	// REPORT_SIZE (0A)
				0x95,0x01,	// REPORT_COUNT (01)
				0xB1,0x02,	// FEATURE (Data,Var,Abs)
				0x75,0x06,	// REPORT_SIZE (06)
				0xB1,0x01,	// FEATURE (Constant,Ary,Abs)
			0xC0,	// END COLLECTION ()

			0x05,0x0F,	// USAGE_PAGE (Physical Interface)
			0x09,0x89,	// USAGE (PID Block Load Report)
			0xA1,0x02,	// COLLECTION (Logical)
				0x85,0x06,	// REPORT_ID (06)
				0x09,0x22,	// USAGE (Effect Block Index)
				0x25,0x28,	// LOGICAL_MAXIMUM (28)
				0x15,0x01,	// LOGICAL_MINIMUM (01)
				0x35,0x01,	// PHYSICAL_MINIMUM (01)
				0x45,0x28,	// PHYSICAL_MAXIMUM (28)
				0x75,0x08,	// REPORT_SIZE (08)
				0x95,0x01,	// REPORT_COUNT (01)
				0xB1,0x02,	// FEATURE (Data,Var,Abs)
				0x09,0x8B,	// USAGE (Block Load Status)
				0xA1,0x02,	// COLLECTION (Logical)
					0x09,0x8C,	// USAGE (Block Load Success)
					0x09,0x8D,	// USAGE (Block Load Full)
					0x09,0x8E,	// USAGE (Block Load Error)
					0x25,0x03,	// LOGICAL_MAXIMUM (03)
					0x15,0x01,	// LOGICAL_MINIMUM (01)
					0x35,0x01,	// PHYSICAL_MINIMUM (01)
					0x45,0x03,	// PHYSICAL_MAXIMUM (03)
					0x75,0x08,	// REPORT_SIZE (08)
					0x95,0x01,	// REPORT_COUNT (01)
					0xB1,0x00,	// FEATURE (Data)
				0xC0,	// END COLLECTION ()
				0x09,0xAC,	// USAGE (RAM Pool Available)
				0x15,0x00,	// LOGICAL_MINIMUM (00)
				0x27,0xFF,0xFF,0x00,0x00,	// LOGICAL_MAXIMUM (00 00 FF FF)
				0x35,0x00,	// PHYSICAL_MINIMUM (00)
				0x47,0xFF,0xFF,0x00,0x00,	// PHYSICAL_MAXIMUM (00 00 FF FF)
				0x75,0x10,	// REPORT_SIZE (10)
				0x95,0x01,	// REPORT_COUNT (01)
				0xB1,0x00,	// FEATURE (Data)
			0xC0,	// END COLLECTION ()

			0x09,0x7F,	// USAGE (PID Pool Report)
			0xA1,0x02,	// COLLECTION (Logical)
				0x85,0x07,	// REPORT_ID (07)
				0x09,0x80,	// USAGE (RAM Pool Size)
				0x75,0x10,	// REPORT_SIZE (10)
				0x95,0x01,	// REPORT_COUNT (01)
				0x15,0x00,	// LOGICAL_MINIMUM (00)
				0x35,0x00,	// PHYSICAL_MINIMUM (00)
				0x27,0xFF,0xFF,0x00,0x00,	// LOGICAL_MAXIMUM (00 00 FF FF)
				0x47,0xFF,0xFF,0x00,0x00,	// PHYSICAL_MAXIMUM (00 00 FF FF)
				0xB1,0x02,	// FEATURE (Data,Var,Abs)
				0x09,0x83,	// USAGE (Simultaneous Effects Max)
				0x26,0xFF,0x00,	// LOGICAL_MAXIMUM (00 FF)
				0x46,0xFF,0x00,	// PHYSICAL_MAXIMUM (00 FF)
				0x75,0x08,	// REPORT_SIZE (08)
				0x95,0x01,	// REPORT_COUNT (01)
				0xB1,0x02,	// FEATURE (Data,Var,Abs)
				0x09,0xA9,	// USAGE (Device Managed Pool)
				0x09,0xAA,	// USAGE (Shared Parameter Blocks)
				0x75,0x01,	// REPORT_SIZE (01)
				0x95,0x02,	// REPORT_COUNT (02)
				0x15,0x00,	// LOGICAL_MINIMUM (00)
				0x25,0x01,	// LOGICAL_MAXIMUM (01)
				0x35,0x00,	// PHYSICAL_MINIMUM (00)
				0x45,0x01,	// PHYSICAL_MAXIMUM (01)
				0xB1,0x02,	// FEATURE (Data,Var,Abs)
				0x75,0x06,	// REPORT_SIZE (06)
				0x95,0x01,	// REPORT_COUNT (01)
				0xB1,0x03,	// FEATURE ( Cnst,Var,Abs)
			0xC0,	// END COLLECTION ()
//		0xC0,	// END COLLECTION ()
  /* USER CODE END 0 */
  0xC0    /*     END_COLLECTION	             */
};

/* USER CODE BEGIN PRIVATE_VARIABLES */

/* USER CODE END PRIVATE_VARIABLES */

/**
  * @}
  */

/** @defgroup USBD_CUSTOM_HID_Exported_Variables USBD_CUSTOM_HID_Exported_Variables
  * @brief Public variables.
  * @{
  */
extern USBD_HandleTypeDef hUsbDeviceFS;

/* USER CODE BEGIN EXPORTED_VARIABLES */

/* USER CODE END EXPORTED_VARIABLES */
/**
  * @}
  */

/** @defgroup USBD_CUSTOM_HID_Private_FunctionPrototypes USBD_CUSTOM_HID_Private_FunctionPrototypes
  * @brief Private functions declaration.
  * @{
  */

static int8_t CUSTOM_HID_Init_FS(void);
static int8_t CUSTOM_HID_DeInit_FS(void);
static int8_t CUSTOM_HID_OutEvent_FS(uint8_t event_idx, uint8_t state, USBD_HandleTypeDef *pdev);
#ifdef USBD_CUSTOMHID_CTRL_REQ_COMPLETE_CALLBACK_ENABLED
static int8_t CUSTOM_HID_CtrlReqComplete_FS(uint8_t request, uint16_t wLength, USBD_HandleTypeDef *pdev);
#endif /* USBD_CUSTOMHID_CTRL_REQ_COMPLETE_CALLBACK_ENABLED */
static uint8_t* CUSTOM_HID_GetReport(USBD_SetupReqTypedef *req, uint16_t *length, USBD_HandleTypeDef *pdev);
//static int8_t USBD_CUSTOM_HID_SendReport_FS(uint8_t *report, uint16_t len);


/**
  * @}
  */

USBD_CUSTOM_HID_ItfTypeDef USBD_CustomHID_fops_FS =
{
  CUSTOM_HID_ReportDesc_FS,
  CUSTOM_HID_Init_FS,
  CUSTOM_HID_DeInit_FS,
  CUSTOM_HID_OutEvent_FS,
#ifdef USBD_CUSTOMHID_CTRL_REQ_COMPLETE_CALLBACK_ENABLED
  CUSTOM_HID_CtrlReqComplete_FS, // Add the CtrlReqComplete callback
#endif /* USBD_CUSTOMHID_CTRL_REQ_COMPLETE_CALLBACK_ENABLED */
  CUSTOM_HID_GetReport           // Already added if handling GET_REPORT
};

/** @defgroup USBD_CUSTOM_HID_Private_Functions USBD_CUSTOM_HID_Private_Functions
  * @brief Private functions.
  * @{
  */

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Initializes the CUSTOM HID media low layer
  * @retval USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CUSTOM_HID_Init_FS(void)
{
  /* USER CODE BEGIN 4 */
  return (USBD_OK);
  /* USER CODE END 4 */
}

/**
  * @brief  DeInitializes the CUSTOM HID media low layer
  * @retval USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CUSTOM_HID_DeInit_FS(void)
{
  /* USER CODE BEGIN 5 */
	log_message("CUSTOM_HID_DeInit_FS Called");
  return (USBD_OK);
  /* USER CODE END 5 */
}

/* OutEvent: Handle and log outgoing HID reports */
int8_t CUSTOM_HID_OutEvent_FS(uint8_t event_idx, uint8_t state, USBD_HandleTypeDef *pdev) {
    uint8_t *report_buf = ((USBD_CUSTOM_HID_HandleTypeDef *)pdev->pClassData)->Report_buf;

    if (report_buf) {
        uint8_t report_id = report_buf[0];
//        log_message("OutEvent: Event ID = %u, State = %u, Report ID = 0x%02X", event_idx, state, report_id);

        switch (report_id) {
            case 0x01: // Example for Set Effect Report
//                log_message("Processing Set Effect Report");
                // Add handling logic for Report ID 0x01
                break;

            case 0x02: // Example for PID State Report
//                log_message("Processing PID State Report");
                // Add handling logic for Report ID 0x02
                break;
            case 0x05: // Create New Effect Report
            	create_new_effect_report.reportId = report_buf[0];
            	create_new_effect_report.effectType = report_buf[1];
            	create_new_effect_report.byteCount = report_buf[2];
            	log_message("Create New Effect Report Request Received");
            	break;

            case 0x0C: // Device Control Report
//            	log_message("Processing Device Control Report");
            	// Parse and process report_buf as needed
				break;

            // Add cases for other Report IDs as needed
            default:
//                log_message("Unknown Report ID: 0x%02X", report_id);
                break;


        }
    } else {
//        log_message("OutEvent: Null report buffer.");
    }
    return USBD_OK;
}

/* USER CODE BEGIN 7 */
#ifdef USBD_CUSTOMHID_CTRL_REQ_COMPLETE_CALLBACK_ENABLED
int8_t CUSTOM_HID_CtrlReqComplete_FS(uint8_t request, uint16_t wLength, USBD_HandleTypeDef *pdev) {
    log_message("CtrlReqComplete: Request = 0x%02X, Length = %u", request, wLength);

    switch (request) {
        case CUSTOM_HID_REQ_SET_REPORT:
            log_message("CtrlReqComplete: Handling SET_REPORT");
            // Parse and process the report buffer
            break;

        case CUSTOM_HID_REQ_GET_REPORT:
            log_message("CtrlReqComplete: Handling GET_REPORT");
            break;

        default:
            log_message("CtrlReqComplete: Unknown request type.");
            break;
    }
    return USBD_OK;
}
#endif /* USBD_CUSTOMHID_CTRL_REQ_COMPLETE_CALLBACK_ENABLED */

uint8_t* CUSTOM_HID_GetReport(USBD_SetupReqTypedef *req, uint16_t *length, USBD_HandleTypeDef *pdev) {
    static uint8_t report[64];
    uint8_t report_id = req->wValue & 0xFF;
    uint8_t report_type = (req->wValue >> 8) & 0xFF;

//    log_message("GetReport: Request Type = 0x%02X, Report ID = 0x%02X, Length = %u", report_type, report_id, *length);

    switch (report_type) {
		case FEATURE:
			switch (report_id) {
				case 0x07: // GET PID Pool Report
					pool_report.reportId = 0x07; // Report ID
					pool_report.ramPoolSize = 0xFFFF;
					pool_report.maxSimultaneousEffects = 0x10;	// FFP supports playing up to 16 simultaneous effects
					pool_report.memoryManagement = 0x03; // 0b00000011
					*length = 5;
//					log_message("Returning Pool Report: ID:%02X %02X %02X", pool_report.reportId,
//																			pool_report.ramPoolSize,
//																			*length);
					return (uint8_t *) &pool_report;

				case 0x06:
					log_message("PID Block Load Report Request");
					block_load_report.reportId = 0x06;
					block_load_report.effectBlockIndex = create_new_effect_report.effectType;
					block_load_report.loadStatus = 0x04; // 0b00000100
					block_load_report.ramPoolAvailable = 0xFFFF;
					*length = 5;

					return (uint8_t *) &block_load_report;

					break;
				default:
					log_message("Unknown Report ID: 0x%02X", report_id);
					*length = 0;
					break;
			}
    }
    return report;
}

/**
  * @brief  Send the report to the Host
  * @param  report: The report to be sent
  * @param  len: The report length
  * @retval USBD_OK if all operations are OK else USBD_FAIL
  */

//static int8_t USBD_CUSTOM_HID_SendReport_FS(uint8_t *report, uint16_t len)
//{
//  return USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, report, len);
//}
/* USER CODE END 7 */

/* USER CODE BEGIN PRIVATE_FUNCTIONS_IMPLEMENTATION */

/* USER CODE END PRIVATE_FUNCTIONS_IMPLEMENTATION */
/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

