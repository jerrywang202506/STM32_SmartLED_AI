/**
  ******************************************************************************
  * @file    Applications/FreeRTOS_LED_Effects/Inc/main.h
  * @author  MCD Application Team
  * @brief   Header for main.c module
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "stm32f4_discovery.h"
#include "stm32f4_discovery_accelerometer.h"
#include "cmsis_os.h"

/* Exported types ------------------------------------------------------------*/
typedef enum
{
  MODE_CHASING = 0,
  MODE_DUAL_CHASE,
  MODE_BLINK_ALL,
  MODE_KNIGHT_RIDER,
  MODE_RANDOM,
  MODE_BINARY_COUNT,
  MODE_TILT_LEVEL,
  MODE_MOTION_CHASE,
  MODE_TILT_BALL,
  MODE_MAX
} LED_ModeTypeDef;

/* Exported constants --------------------------------------------------------*/
#define LED_EFFECTS_TASK_PERIOD_MS    10U
#define BUTTON_DEBOUNCE_MS            200U
#define ACCELERO_TASK_PERIOD_MS       50U
#define ACCELERO_TASK_STACK_SIZE      (configMINIMAL_STACK_SIZE * 2)

#define ACCELERO_TILT_THRESHOLD       200
#define ACCELERO_TILT_STRONG          450
#define ACCELERO_SHAKE_THRESHOLD      1500
#define ACCELERO_SHAKE_COOLDOWN_MS    600U

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
/* Exported functions ------------------------------------------------------- */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
