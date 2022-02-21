/**
  ******************************************************************************
  * @file    led_control.c
  * @author  IBronx MDE team
  * @brief   Peripheral driver for ws28xx RGB LED control
  *          This file provides firmware utility functions to support WS28xx RGB
  *          LED functions
  *
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

/* Includes ------------------------------------------------------------------*/
#include "led_control.h"
#include "main.h"
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint32_t WS2812_DMA_BUFFER[TOTAL_RGB_LED_PIXEL_SIZE];
uint32_t WS2812_IO_High[1];
uint32_t WS2812_IO_Low[1];
ws2812Color_t color[MAX_WS28XX_LED];

extern DMA_HandleTypeDef hdma_tim8_up;
extern DMA_HandleTypeDef hdma_tim8_ch1;
extern DMA_HandleTypeDef hdma_tim8_ch3;
extern TIM_HandleTypeDef htim8;

/* Private function prototypes -----------------------------------------------*/
/* function prototypes -------------------------------------------------------*/

/**
* @brief  Initialize WS28xx RGB LED
* @param  None
* @retval None
*/
void rgbled_Init(void)
{
  HAL_DMA_RegisterCallback(&hdma_tim8_ch3, HAL_DMA_XFER_CPLT_CB_ID, rgbled_DMAXferCpltCallback);

  // disable DMA
  HAL_DMA_Start(&hdma_tim8_up, (uint32_t)WS2812_IO_High, (uint32_t)&RGB_LED_PORT->BSRR, TOTAL_RGB_LED_PIXEL_SIZE);
  HAL_DMA_Start(&hdma_tim8_ch1,  (uint32_t)WS2812_DMA_BUFFER, (uint32_t)&RGB_LED_PORT->BSRR, TOTAL_RGB_LED_PIXEL_SIZE);
  HAL_DMA_Start_IT(&hdma_tim8_ch3,  (uint32_t)WS2812_IO_Low, (uint32_t)&RGB_LED_PORT->BSRR, TOTAL_RGB_LED_PIXEL_SIZE);
  __HAL_DMA_DISABLE(&hdma_tim8_up);
  __HAL_DMA_DISABLE(&hdma_tim8_ch1);
  __HAL_DMA_DISABLE(&hdma_tim8_ch3);

  // Starts the TIM Base generation
  HAL_TIM_Base_Start(&htim8);
}

/**
* @brief  Turn on WS28xx RGB LED
* @param  red:    Red color pixel
* @param  green:  Green color pixel
* @param  blue:   Blue color pixel
* @retval None
*/
void rgbled_TurnOnLED(uint8_t red, uint8_t green, uint8_t blue)
{
  int total_pixel_cnt = MAX_WS28XX_LED * RGB_LED_PIXEL_SIZE;

  for (int idx = 0; idx < total_pixel_cnt;)
  {
    rgbled_SetColorPixel(&WS2812_DMA_BUFFER[idx], red, green, blue);
    idx += RGB_LED_PIXEL_SIZE;
  }

  rgbled_TriggerTransmit(TOTAL_RGB_LED_PIXEL_SIZE);
  HAL_Delay(2);
}

/**
* @brief  Turn off WS28xx RGB LED
* @param  None
* @retval None
*/
void rgbled_TurnOffLED(void)
{
  rgbled_TurnOnLED(0, 0, 0);
}

/**
  * @brief  Convert the RGB color pixels to the DMA buffer data
  * @param  p_buf:  Data buffer pointer
  * @param  red:    Red color pixel
  * @param  green:  Green color pixel
  * @param  blue:   Blue color pixel
  * @retval None
  */
void rgbled_SetColorPixel(uint32_t* p_buf, uint8_t red, uint8_t green, uint8_t blue)
{
  // WS2812B requires serial data in the order G-R-B
  // Each bytes data is shifted out start from MSB, bit 7
  for (int bit_idx = 7; bit_idx >= 0; bit_idx--)
  {
    if ((green >> bit_idx) & 0x01)
      *(p_buf)  = RGB_LED_PIN;
    else
      *(p_buf) = RGB_LED_PIN << 16;

    p_buf++;
  }

  for (int bit_idx = 7; bit_idx >= 0; bit_idx--)
  {
    if ((red >> bit_idx) & 0x01)
      *(p_buf) = RGB_LED_PIN;
    else
      *(p_buf) = RGB_LED_PIN << 16;

    p_buf++;
  }

  for (int bit_idx = 7; bit_idx >= 0; bit_idx--)
  {
    if ((blue >> bit_idx) & 0x01)
      *(p_buf) = RGB_LED_PIN;
    else
      *(p_buf) = RGB_LED_PIN << 16;

    p_buf++;
  }
}

/**
  * @brief  Trigger DMA module to send the signal to WS28xx
  * @param  buffer_size:  DMA buffer size
  * @retval None
  */
void rgbled_TriggerTransmit(uint16_t buffer_size)
{
  // Update WS2812 IO Signal
  WS2812_IO_High[0] = RGB_LED_PIN;
  WS2812_IO_Low[0] = RGB_LED_PIN << 16;

  HAL_DMA_Start(&hdma_tim8_up, (uint32_t)WS2812_IO_High, (uint32_t)&RGB_LED_PORT->BSRR, buffer_size);
  HAL_DMA_Start(&hdma_tim8_ch1,  (uint32_t)WS2812_DMA_BUFFER, (uint32_t)&RGB_LED_PORT->BSRR, buffer_size);
  HAL_DMA_Start_IT(&hdma_tim8_ch3,  (uint32_t)WS2812_IO_Low, (uint32_t)&RGB_LED_PORT->BSRR, buffer_size);

  // clear all DMA Interrupt flags
  __HAL_DMA_CLEAR_FLAG (&hdma_tim8_up, __HAL_DMA_GET_TC_FLAG_INDEX(&hdma_tim8_up));
  __HAL_DMA_CLEAR_FLAG (&hdma_tim8_up, __HAL_DMA_GET_HT_FLAG_INDEX(&hdma_tim8_up));
  __HAL_DMA_CLEAR_FLAG (&hdma_tim8_up, __HAL_DMA_GET_TE_FLAG_INDEX(&hdma_tim8_up));

  __HAL_DMA_CLEAR_FLAG (&hdma_tim8_ch1, __HAL_DMA_GET_TC_FLAG_INDEX(&hdma_tim8_ch1));
  __HAL_DMA_CLEAR_FLAG (&hdma_tim8_ch1, __HAL_DMA_GET_HT_FLAG_INDEX(&hdma_tim8_ch1));
  __HAL_DMA_CLEAR_FLAG (&hdma_tim8_ch1, __HAL_DMA_GET_TE_FLAG_INDEX(&hdma_tim8_ch1));

  __HAL_DMA_CLEAR_FLAG (&hdma_tim8_ch3, __HAL_DMA_GET_TC_FLAG_INDEX(&hdma_tim8_ch3));
  __HAL_DMA_CLEAR_FLAG (&hdma_tim8_ch3, __HAL_DMA_GET_HT_FLAG_INDEX(&hdma_tim8_ch3));
  __HAL_DMA_CLEAR_FLAG (&hdma_tim8_ch3, __HAL_DMA_GET_TE_FLAG_INDEX(&hdma_tim8_ch3));

  // set the DMA buffer size and timer period cnt
  hdma_tim8_up.Instance->NDTR = buffer_size;
  hdma_tim8_ch1.Instance->NDTR = buffer_size;
  hdma_tim8_ch3.Instance->NDTR = buffer_size;
  htim8.Instance->CNT = 0;

  __HAL_TIM_CLEAR_FLAG(&htim8, TIM_FLAG_UPDATE | TIM_FLAG_CC1 | TIM_FLAG_CC2 | TIM_FLAG_CC3 | TIM_FLAG_CC4);

  // enable DMA channels
  __HAL_DMA_ENABLE(&hdma_tim8_up);
  __HAL_DMA_ENABLE(&hdma_tim8_ch1);
  __HAL_DMA_ENABLE(&hdma_tim8_ch3);

  // enable DMA request
  __HAL_TIM_ENABLE_DMA(&htim8, TIM_DMA_UPDATE);
  __HAL_TIM_ENABLE_DMA(&htim8, TIM_DMA_CC1);
  __HAL_TIM_ENABLE_DMA(&htim8, TIM_DMA_CC3);

  // start the timer
  __HAL_TIM_ENABLE(&htim8);
}

/**
  * @brief  DMA transfer complete callback
  * @param  DmaHandle  DMA handle
  * @retval None
  */
void rgbled_DMAXferCpltCallback(DMA_HandleTypeDef *DmaHandle)
{
  __HAL_TIM_DISABLE_DMA(&htim8, TIM_DMA_UPDATE);
  __HAL_TIM_DISABLE_DMA(&htim8, TIM_DMA_CC1);
  __HAL_TIM_DISABLE_DMA(&htim8, TIM_DMA_CC3);

  RGB_LED_PORT->BSRR = (uint32_t)RGB_LED_PIN << 16U;
}


/************************ (C) COPYRIGHT IBronx *****************END OF FILE****/
