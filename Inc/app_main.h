/**
  ******************************************************************************
  * @file    app_main.h
  * @author  IBronx MDE team
  * @brief   Main Application header file
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __APP_MAIN_H_
#define __APP_MAIN_H_

#ifdef __cplusplus
 extern "C" {
#endif

 /* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

 /* Exported types ------------------------------------------------------------*/

#define TASK_MAIN_DELAY_MS          200

#define MAIN_SD_PRESENT_FLAG        0x00000001U
#define MAIN_MOUNT_SDCARD_FLAG      0x00000002U
#define MAIN_CREATE_FOLDERS_FLAG    0x00000004U
#define MAIN_OPEN_FILE_FLAG         0x00000008U
#define MAIN_IO_EXPANDER_FLAG       0x00000010U

 typedef enum
 {
   STATE_MAIN_INIT = 0,
   STATE_MAIN_START,
   STATE_MAIN_RUNNING,
   STATE_MAIN_START_IDLE,
 }mainState_t;

 /* Exported constants --------------------------------------------------------*/
 /* Exported macro ------------------------------------------------------------*/
 /* Exported functions ------------------------------------------------------- */
 void StartMainTask(void *argument);
 void main_task_Init(void);
 void main_task_Preparation(void);
 void main_task_Running(void);
 void main_task_Idle(uint32_t tickCount);

 void main_ChangeCurrentState(mainState_t state);
 void main_CreateSubThreads(void);
 void main_Interrupt_Handler(uint16_t GPIO_Pin);
 void main_StartbuttonHandler(void);

#endif /* __APP_MAIN_H_ */


 /************************ (C) COPYRIGHT IBronx *****************END OF FILE****/
