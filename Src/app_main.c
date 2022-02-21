/**
  ******************************************************************************
  * @file    app_main.c
  * @author  IBronx MDE team
  * @brief   Main application program
  *          This file provides main application functions to execute all the
  *          operations
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
#include "app_main.h"
#include "cmsis_os.h"
#include "errorcode.h"
#include "pca9505_control.h"
#include "screw_feeder.h"
#include "screw_controller.h"
//#include "led_control.h"
#include "logger.h"
#include "usb_device.h"

/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

mainState_t mainState;
osSemaphoreId_t osSmp_StartBtn;
osSemaphoreId_t osSmp_ScrewCount;
osEventFlagsId_t osFlag_ScrewCtrl;
osEventFlagsId_t osFlag_ScrewFeeder;
osEventFlagsId_t osFlag_Main;

static uint16_t tickCount_StartButton;

/* Definitions for feederTask */
osThreadId_t feederTaskHandle;
const osThreadAttr_t feederTask_attributes = {
  .name = "feederTask",
  .stack_size = 640 * 4,
  .priority = (osPriority_t) osPriorityNormal3,
};

/* Definitions for screwController */
osThreadId_t screwControllerHandle;
const osThreadAttr_t screwController_attributes = {
  .name = "screwController",
  .stack_size = 640 * 4,
  .priority = (osPriority_t) osPriorityNormal2,
};

/* function prototypes -------------------------------------------------------*/

/**
  * @brief  Function implementing the mainTask thread.
  * @param  argument: Not used
  * @retval None
  */
void StartMainTask(void *argument)
{
  uint32_t tick = osKernelGetTickCount();
  mainState = STATE_MAIN_INIT;

  uint32_t tickCount = 0;
  for(;;)
  {
    tick += TASK_MAIN_DELAY_MS;
    tickCount++;
    switch(mainState)
    {
      case STATE_MAIN_INIT:
        main_task_Init();
        break;
      case STATE_MAIN_START:
        main_task_Preparation();
        break;
      case STATE_MAIN_RUNNING:
        main_task_Running();
        break;
      case STATE_MAIN_START_IDLE:
        main_task_Idle(tickCount);
        break;
    }
    osDelayUntil(tick);
  }

  // delete the main thread, in case accidentally break the loop
  osThreadTerminate(NULL);
}

/**
  * @brief  Execute task initialization when under STATE_MAIN_INIT
  * @param  None
  * @retval None
  */
void main_task_Init(void)
{
  SEGGER_SYSVIEW_Print("[MAINTASK] - STATE_MAIN_INIT");

  osSmp_StartBtn = osSemaphoreNew(1, 0, NULL);
  osSmp_ScrewCount = osSemaphoreNew(1, 0, NULL);
  osFlag_ScrewCtrl = osEventFlagsNew(NULL);
  osFlag_ScrewFeeder = osEventFlagsNew(NULL);
  osFlag_Main = osEventFlagsNew(NULL);
  main_CreateSubThreads();

  // Init Logger
  //logger_Init();

  IO_Expander_Init();

  MX_USB_DEVICE_Init();
  //rgbled_TurnOffLED();

  // configure default Solenoid state
  PCA9505_SetOutputPin(SOLENOID_ROTARY_PORT, SOLENOID_ROTARY_PIN, SOLENOID_ROTARY_BACKWARD);
  PCA9505_SetOutputPin(SOLENOID_VACUUM_PORT, SOLENOID_VACUUM_PIN, SOLENOID_VACUUM_OFF);
  PCA9505_SetOutputPin(SOLENOID_DISPATCH_PORT, SOLENOID_DISPATCH_PIN, SOLENOID_DISPATCH_OFF);
  PCA9505_SetOutputPin(SOLENOID_FEEDER_PORT, SOLENOID_FEEDER_PIN, SOLENOID_FEEDER_UP);

  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

  // save the log when program Init
  logger_LogInfo("[MAIN] - Program Start from here....", LOGGER_NULL_STRING);

  // save the log for IO Port Expander
  if (osEventFlagsGet(osFlag_Main) & MAIN_IO_EXPANDER_FLAG)
    logger_LogInfo("[MAIN] - IO Port Expander has been initialized", LOGGER_NULL_STRING);
  else
    logger_LogError("[MAIN] - IO Port Expander failed to initialize", LOGGER_NULL_STRING);

  mainState = STATE_MAIN_START_IDLE;
}

/**
  * @brief  Preparation before the screw operation
  * @param  None
  * @retval None
  */
void main_task_Preparation(void)
{
  logger_LogInfo("[MAIN] - Preparation before the Screw operation", LOGGER_NULL_STRING);

  // configure default Solenoid state
  PCA9505_SetOutputPin(SOLENOID_ROTARY_PORT, SOLENOID_ROTARY_PIN, SOLENOID_ROTARY_BACKWARD);
  osDelay(50);
  PCA9505_SetOutputPin(SOLENOID_VACUUM_PORT, SOLENOID_VACUUM_PIN, SOLENOID_VACUUM_OFF);
  osDelay(50);
  PCA9505_SetOutputPin(SOLENOID_DISPATCH_PORT, SOLENOID_DISPATCH_PIN, SOLENOID_DISPATCH_OFF);
  osDelay(50);
  PCA9505_SetOutputPin(SOLENOID_FEEDER_PORT, SOLENOID_FEEDER_PIN, SOLENOID_FEEDER_UP);
  osDelay(50);

  mainState = STATE_MAIN_RUNNING;
}

/**
  * @brief  Execute the screw operation
  * @param  None
  * @retval None
  */
void main_task_Running(void)
{
  logger_LogInfo("[MAIN] - Start the Screw Operation", LOGGER_NULL_STRING);

  // trigger ScrewController & ScrewFeeder Task to running screw operation
  osEventFlagsSet(osFlag_ScrewCtrl, HAYASHI_OPERATION_START_FLAG);
  osEventFlagsSet(osFlag_ScrewFeeder, FEEDER_OPERATION_START_FLAG);

  osSemaphoreRelease(osSmp_StartBtn);

  mainState = STATE_MAIN_START_IDLE;
}

/**
  * @brief  Monitoring Start/Stop button
  * @param  None
  * @retval None
  */
void main_task_Idle(uint32_t tickCount)
{
  GPIO_PinState pin_state = HAL_GPIO_ReadPin(START_BTN_GPIO_Port, START_BTN_Pin);
  if (pin_state == GPIO_PIN_RESET)
  {
    tickCount_StartButton++;
    if (tickCount_StartButton == 1)
      main_StartbuttonHandler();

    // allow Button to trigger again after 2 seconds
    if (tickCount_StartButton >= 10)
      tickCount_StartButton = 0;
  }
  else
  {
    tickCount_StartButton = 0;
  }

  // print the message
  if ((tickCount % 10) == 0)
    SEGGER_SYSVIEW_Print("[MAIN] - ");
}

/**
  * @brief  Create the sub thread from main thread, each sub thread will execute its own task
  * @param  None
  * @retval None
  */
void main_CreateSubThreads(void)
{
  taskENTER_CRITICAL();

  // creation of feederTask
  feederTaskHandle = osThreadNew(StartFeederTask, NULL, &feederTask_attributes);

  // creation of screwCtrlTask
  screwControllerHandle = osThreadNew(StartScrewCtrlTask, NULL, &screwController_attributes);

  taskEXIT_CRITICAL();
}

/**
  * @brief  Change the current main task state, so that it can proceed to next state
  * @param  state:  Main task state
  * @retval None
  */
void main_ChangeCurrentState(mainState_t state)
{
  mainState = state;
}

/**
  * @brief  Start/Stop Button handler
  * @param  None
  * @retval None
  */
void main_StartbuttonHandler(void)
{
  if (osSemaphoreGetCount(osSmp_StartBtn) >= 1)
  {
    //rgbled_TurnOffLED();
    osEventFlagsSet(osFlag_ScrewCtrl, HAYASHI_OPERATION_STOP_FLAG);
    osEventFlagsSet(osFlag_ScrewFeeder, FEEDER_OPERATION_STOP_FLAG);
    osSemaphoreAcquire(osSmp_StartBtn, 0U);

    logger_LogInfo("[EXTI] - Receive Stop button signal", LOGGER_NULL_STRING);
  }
  else
  {
    // clear the IO interrupt flags
    if (!(osEventFlagsGet(osFlag_Main) & MAIN_IO_EXPANDER_FLAG))
      SEGGER_SYSVIEW_Print("[EXTI] - IO PORT is not ready to use");
//    else
//      IO_Expander_ClearInterrupt();

    if (osSemaphoreGetCount(osSmp_ScrewCount) >= 1)
    {
      SEGGER_SYSVIEW_Print("[MAIN] - Reset screw count");
      osSemaphoreAcquire(osSmp_ScrewCount, 100U);
    }

    IO_Expander_Init();

    logger_LogInfo("[EXTI] - Receive Start button signal", LOGGER_NULL_STRING);
    main_ChangeCurrentState(STATE_MAIN_START);
  }
}


 /************************ (C) COPYRIGHT IBronx *****************END OF FILE****/

