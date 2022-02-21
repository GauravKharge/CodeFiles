/**
  ******************************************************************************
  * @file    logger.h
  * @author  IBronx MDE team
  * @brief   logger header file to record operation events
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
#ifndef INC_LOGGER_H_
#define INC_LOGGER_H_

#ifdef __cplusplus
 extern "C" {
#endif

 /* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

 /* Exported types ------------------------------------------------------------*/

#define LOGGER_STR_LEN          300
#define LOGGER_PATH_LEN         30
#define LOGGER_LOG_DIR          "0:/LOG"
#define LOGGER_TYPE_INFO        "Info"
#define LOGGER_TYPE_ERROR       "Error"
#define LOGGER_TYPE_WARN        "Warn"
#define LOGGER_NULL_STRING      '\0'

 /* Exported constants --------------------------------------------------------*/
 /* Exported macro ------------------------------------------------------------*/
 /* Exported functions ------------------------------------------------------- */
 void logger_Init(void);
 uint32_t logger_SaveLogEvents(const char* sState, const char* sMsg);
 void logger_LogInfo(const char* sMsg, const char* sArg);
 void logger_LogWarn(const char* sMsg, const char* sArg);
 void logger_LogError(const char* sMsg, const char* sArg);




#ifdef __cplusplus
}
#endif

#endif /* INC_LOGGER_H_ */


 /************************ (C) COPYRIGHT IBronx *****************END OF FILE****/
