/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : usbd_custom_hid_if.h
  * @version        : v1.0_Cube
  * @brief          : Header for usbd_custom_hid_if.c file.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USBD_CUSTOM_HID_IF_H__
#define __USBD_CUSTOM_HID_IF_H__

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "usbd_customhid.h"

/* USER CODE BEGIN INCLUDE */

/* USER CODE END INCLUDE */

/** @addtogroup STM32_USB_OTG_DEVICE_LIBRARY
  * @brief For Usb device.
  * @{
  */

/** @defgroup USBD_CUSTOM_HID USBD_CUSTOM_HID
  * @brief Usb custom human interface device module.
  * @{
  */

/** @defgroup USBD_CUSTOM_HID_Exported_Defines USBD_CUSTOM_HID_Exported_Defines
  * @brief Defines.
  * @{
  */

/* USER CODE BEGIN EXPORTED_DEFINES */
 // Define Report IDs
 #define REPORT_ID_SET_EFFECT          0x01
 #define REPORT_ID_CONTROL_EFFECT      0x02
 #define REPORT_ID_PID_STATE           0x03
 #define REPORT_ID_SUPPORTED_EFFECTS   0x04
 #define REPORT_ID_EFFECT_STATE        0x05
 #define REPORT_ID_DEVICE_GAIN         0x06
 #define REPORT_ID_CUSTOM_FORCE_DATA   0x07
 #define REPORT_ID_EFFECT_BLOCK_FREE   0x08
 #define REPORT_ID_POOL_REPORT         0x09

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

 // Define additional constants for logical ranges and maximum sizes
 #define REPORT_SIZE                   64  // Maximum report size
 #define LOGICAL_MINIMUM               0x00
 #define LOGICAL_MAXIMUM               0xFF

/* USER CODE END EXPORTED_DEFINES */

/**
  * @}
  */

/** @defgroup USBD_CUSTOM_HID_Exported_Types USBD_CUSTOM_HID_Exported_Types
  * @brief Types.
  * @{
  */

/* USER CODE BEGIN EXPORTED_TYPES */

/* USER CODE END EXPORTED_TYPES */

/**
  * @}
  */

/** @defgroup USBD_CUSTOM_HID_Exported_Macros USBD_CUSTOM_HID_Exported_Macros
  * @brief Aliases.
  * @{
  */

/* USER CODE BEGIN EXPORTED_MACRO */

/* USER CODE END EXPORTED_MACRO */

/**
  * @}
  */

/** @defgroup USBD_CUSTOM_HID_Exported_Variables USBD_CUSTOM_HID_Exported_Variables
  * @brief Public variables.
  * @{
  */

/** CUSTOMHID Interface callback. */
extern USBD_CUSTOM_HID_ItfTypeDef USBD_CustomHID_fops_FS;

/* USER CODE BEGIN EXPORTED_VARIABLES */

/* USER CODE END EXPORTED_VARIABLES */

/**
  * @}
  */

/** @defgroup USBD_CUSTOM_HID_Exported_FunctionsPrototype USBD_CUSTOM_HID_Exported_FunctionsPrototype
  * @brief Public functions declaration.
  * @{
  */

/* USER CODE BEGIN EXPORTED_FUNCTIONS */

/* USER CODE END EXPORTED_FUNCTIONS */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* __USBD_CUSTOM_HID_IF_H__ */

