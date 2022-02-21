/**
  ******************************************************************************
  * @file    logger.c
  * @author  IBronx MDE team
  * @brief   logger header file to record operation events
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
#include "logger.h"
//#include "fatfs.h"
#include "app_main.h"
#include "errorcode.h"
#include "SEGGER_SYSVIEW.h"
#include "cmsis_os.h"
//#include "flash_control.h"

#include <stdio.h>
#include <string.h>
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
char logger_filepath[LOGGER_PATH_LEN];
char logger_line_buf[LOGGER_STR_LEN];
char logger_string_buf[LOGGER_STR_LEN];
//uint8_t logger_filename[FLASH_FILENAME_SIZE];
uint32_t loggerFileName;

extern osEventFlagsId_t osFlag_Main;
/* Private function prototypes -----------------------------------------------*/
/* function prototypes -------------------------------------------------------*/

/**
* @brief  Logger Initialization
* @param  None
* @retval None
*/
void logger_Init(void)
{
//  FRESULT res;
//  if (BSP_SD_IsDetected() == SD_PRESENT)
//  {
//    osEventFlagsSet(osFlag_Main, MAIN_SD_PRESENT_FLAG);
//    if ((res = f_mount(&SDFatFS, (TCHAR const*)SDPath, 1)) == FR_OK)
//    {
//      osEventFlagsSet(osFlag_Main, MAIN_MOUNT_SDCARD_FLAG);
//      SEGGER_SYSVIEW_Print("[LOG] - Mount SDCARD Success");
//
//      res =  f_mkdir(LOGGER_LOG_DIR);
//      if (res == FR_OK || FR_EXIST)
//      {
//        osEventFlagsSet(osFlag_Main, MAIN_CREATE_FOLDERS_FLAG);
//        SEGGER_SYSVIEW_Print("[LOG] - Created Log folder");
//
//        // read the log file name
//        flash_read_LogFileName(logger_filename);
//        loggerFileName = logger_filename[0] + (logger_filename[1] << 8) +
//            (logger_filename[2] << 16) + (logger_filename[3] << 24);
//
//        // set log file name as number
//        if (loggerFileName == 0xFFFFFFFF)
//          loggerFileName = 10000;
//        else
//          loggerFileName = loggerFileName + 1;
//
//        logger_filename[0] = loggerFileName & 0xFF;
//        logger_filename[1] = (loggerFileName >> 8) & 0xFF;
//        logger_filename[2] = (loggerFileName >> 16) & 0xFF;
//        logger_filename[3] = (loggerFileName >> 24) & 0xFF;
//        flash_write_LogFileName(logger_filename);
//      }
//      else
//      {
//        osEventFlagsClear(osFlag_Main, MAIN_CREATE_FOLDERS_FLAG);
//        SEGGER_SYSVIEW_Error("[LOG] - Failed to create Log folder");
//      }
//    }
//    else
//    {
//      osEventFlagsClear(osFlag_Main, MAIN_MOUNT_SDCARD_FLAG);
//      SEGGER_SYSVIEW_Error("[LOG] - Failed to mount SDCARD");
//    }
//  }
//  else
//  {
//    osEventFlagsClear(osFlag_Main, MAIN_SD_PRESENT_FLAG);
//    SEGGER_SYSVIEW_Error("[LOG] - SDCARD is not present");
//  }
}

/**
* @brief  Log the Info Event into log file
* @param  sMsg       Event log message
* @param  sArg       Input argument
  @retval None
*/
void logger_LogInfo(const char* sMsg, const char* sArg)
{
  memset(logger_string_buf, '\0', sizeof(logger_string_buf));
  if (strlen(sArg) > 0)
    sprintf(logger_string_buf, "%s: %s", sMsg, sArg);
  else
    sprintf(logger_string_buf, "%s", sMsg);

  logger_SaveLogEvents(LOGGER_TYPE_INFO, logger_string_buf);
  SEGGER_SYSVIEW_Print(logger_string_buf);
}

/**
* @brief  Log the Warn Event into log file
* @param  sMsg       Event log message
* @param  sArg       Input argument
  @retval None
*/
void logger_LogWarn(const char* sMsg, const char* sArg)
{
  memset(logger_string_buf, '\0', sizeof(logger_string_buf));
  if (strlen(sArg) > 0)
    sprintf(logger_string_buf, "%s: %s", sMsg, sArg);
  else
    sprintf(logger_string_buf, "%s", sMsg);

  logger_SaveLogEvents(LOGGER_TYPE_WARN, logger_string_buf);
  SEGGER_SYSVIEW_Warn(logger_string_buf);
}

/**
* @brief  Log the Error Event into log file
* @param  sMsg       Event log message
* @param  sArg       Input argument
  @retval None
*/
void logger_LogError(const char* sMsg, const char* sArg)
{
  memset(logger_string_buf, '\0', sizeof(logger_string_buf));
  if (strlen(sArg) > 0)
    sprintf(logger_string_buf, "%s: %s", sMsg, sArg);
  else
    sprintf(logger_string_buf, "%s", sMsg);

  logger_SaveLogEvents(LOGGER_TYPE_ERROR, logger_string_buf);
  SEGGER_SYSVIEW_Error(logger_string_buf);
}

/**
* @brief  Save log events into log file
* @param  sState     Event log state
* @param  errorcode  Error Code
* @param  sMsg       Event log message
  @retval rc:        If pass then return PER_NO_ERROR, otherwise error code
*/
uint32_t logger_SaveLogEvents(const char* sState, const char* sMsg)
{
  uint32_t rc = PER_ERROR_SDCARD_FAILED_WRITE;
  //uint32_t timestamp = SEGGER_SYSVIEW_GET_TIMESTAMP();

//  memset(logger_line_buf, 0, sizeof(logger_line_buf));
//  int size = sprintf(logger_line_buf, "%6s - %s.\n", sState, sMsg);
//
//  // make sure SD CARD is presented & mounted before
//  if ((osEventFlagsGet(osFlag_Main) & MAIN_SD_PRESENT_FLAG) &&
//      (osEventFlagsGet(osFlag_Main) & MAIN_MOUNT_SDCARD_FLAG) &&
//      (osEventFlagsGet(osFlag_Main) & MAIN_CREATE_FOLDERS_FLAG))
//  {
//    //sprintf(logger_filepath, "%s/%lu.LOG", LOGGER_LOG_DIR, loggerFileName);
//    sprintf(logger_filepath, "%s/log.LOG", LOGGER_LOG_DIR);
//
//    // create a file with read write access and open it
//    if(f_open(&SDFile, logger_filepath, FA_OPEN_APPEND | FA_WRITE) == FR_OK)
//    {
//      osEventFlagsSet(osFlag_Main, MAIN_OPEN_FILE_FLAG);
//
//      // write distance data into files
//      uint32_t bw;
//      FRESULT res = f_write(&SDFile, logger_line_buf, size, (void *)&bw);
//      if((bw > 0) && (res == FR_OK))
//      {
//        rc = PER_NO_ERROR;
//      }
//    }
//
//    f_close(&SDFile);
//    osEventFlagsClear(osFlag_Main, MAIN_OPEN_FILE_FLAG);
//  }

  return rc;
}


/************************ (C) COPYRIGHT IBronx *****************END OF FILE****/
