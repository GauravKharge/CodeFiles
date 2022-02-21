/**
  ******************************************************************************
  * @file    led_control.h
  * @author  IBronx MDE team
  * @brief   Peripheral driver header file for WS28xx RGB LED control
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 IBronx.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by IBronx under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __LED_CONTROL_H_
#define __LED_CONTROL_H_

#ifdef __cplusplus
 extern "C" {
#endif

 /* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "main.h"

#include <stdbool.h>

 /* Exported types ------------------------------------------------------------*/

#define MAX_WS28XX_LED                5
#define WS2812_FREQ                   800000  // it is fixed: WS2812 require 800kHz
#define RGB_LED_PIXEL_SIZE            24
#define TOTAL_RGB_LED_PIXEL_SIZE      RGB_LED_PIXEL_SIZE * MAX_WS28XX_LED + 1
#define RGB_LED_PIN                   RGBLED_Pin
#define RGB_LED_PORT                  RGBLED_GPIO_Port

 typedef struct
 {
   uint8_t red;
   uint8_t green;
   uint8_t blue;
   bool bSet;
 }ws2812Color_t;

 /* Exported constants --------------------------------------------------------*/
 /* Exported macro ------------------------------------------------------------*/
 /* Exported functions ------------------------------------------------------- */

 void rgbled_Init(void);
 void rgbled_TurnOnLED(uint8_t red, uint8_t green, uint8_t blue);
 void rgbled_TurnOffLED(void);
 void rgbled_SetColorPixel(uint32_t* p_buf, uint8_t red, uint8_t green, uint8_t blue);
 void rgbled_TriggerTransmit(uint16_t buffer_size);
 void rgbled_DMAXferCpltCallback(DMA_HandleTypeDef *DmaHandle);

#ifdef __cplusplus
}
#endif

#endif /* __LED_CONTROL_H_ */


 /************************ (C) COPYRIGHT IBronx *****************END OF FILE****/
