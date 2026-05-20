/**
  ******************************************************************************
  * @file    Applications/FreeRTOS_LED_Effects/Src/main.c
  * @author  MCD Application Team
  * @brief   FreeRTOS LED Effects application for STM32F4-Discovery.
  *          Demonstrates GPIO control with multiple LED chasing patterns
  *          using a 10 ms periodic task under FreeRTOS.
  *          Now enhanced with on-board MEMS accelerometer (g-sensor) support:
  *          tilt-controlled LED patterns, motion-reactive chasing, and a
  *          virtual gravity ball that rolls across the LEDs.
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

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"
#include <stdlib.h>

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define LED_EFFECTS_TASK_STACK_SIZE   (configMINIMAL_STACK_SIZE * 2)
#define BUTTON_TASK_STACK_SIZE        (configMINIMAL_STACK_SIZE)

/* Tilt direction encoding */
#define TILT_NONE     0
#define TILT_LEFT    -1
#define TILT_RIGHT    1
#define TILT_FRONT   -2
#define TILT_BACK     2

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static volatile uint8_t button_pressed = 0U;
static volatile uint8_t shake_detected = 0U;
static LED_ModeTypeDef current_mode = MODE_CHASING;

static int16_t accelero_xyz[3] = {0, 0, 0};
static volatile int8_t tilt_direction = TILT_NONE;

/* Private function prototypes -----------------------------------------------*/
static void LED_Effects_Task(void const *argument);
static void Button_Task(void const *argument);
static void Accelerometer_Task(void const *argument);
static void SystemClock_Config(void);
static void Error_Handler(void);
static void All_LEDs_Off(void);
static void All_LEDs_On(void);
static void Switch_Mode(void);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
  /* STM32F4xx HAL library initialization:
       - Configure the Flash prefetch, instruction and Data caches
       - Configure the Systick to generate an interrupt each 1 msec
       - Set NVIC Group Priority to 4
       - Global MSP (MCU Support Package) initialization
     */
  HAL_Init();

  /* Configure the system clock to 168 MHz */
  SystemClock_Config();

  /* Initialize all LEDs on STM32F4-Discovery */
  BSP_LED_Init(LED3);
  BSP_LED_Init(LED4);
  BSP_LED_Init(LED5);
  BSP_LED_Init(LED6);
  All_LEDs_Off();

  /* Initialize USER Button with EXTI interrupt */
  BSP_PB_Init(BUTTON_KEY, BUTTON_MODE_EXTI);

  /* Initialize on-board MEMS accelerometer (auto-detects LIS302DL / LIS3DSH) */
  if (BSP_ACCELERO_Init() != ACCELERO_OK)
  {
    Error_Handler();
  }

  /* Seed random generator with accelerometer noise for unique sequences each boot */
  BSP_ACCELERO_GetXYZ(accelero_xyz);
  srand((unsigned int)(accelero_xyz[0] ^ accelero_xyz[1] ^ accelero_xyz[2] ^ HAL_GetTick()));

  /* Create LED effects task */
  osThreadDef(LED_Effects, LED_Effects_Task, osPriorityNormal, 0, LED_EFFECTS_TASK_STACK_SIZE);
  osThreadCreate(osThread(LED_Effects), NULL);

  /* Create button handling task */
  osThreadDef(Button, Button_Task, osPriorityAboveNormal, 0, BUTTON_TASK_STACK_SIZE);
  osThreadCreate(osThread(Button), NULL);

  /* Create accelerometer sampling task */
  osThreadDef(Accelerometer, Accelerometer_Task, osPriorityNormal, 0, ACCELERO_TASK_STACK_SIZE);
  osThreadCreate(osThread(Accelerometer), NULL);

  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */
  for (;;)
  {
  }
}

/**
  * @brief  Switch to the next LED mode with visual feedback.
  * @param  None
  * @retval None
  */
static void Switch_Mode(void)
{
  current_mode = (LED_ModeTypeDef)((current_mode + 1) % MODE_MAX);
  All_LEDs_Off();
  osDelay(50);
  All_LEDs_On();
  osDelay(100);
  All_LEDs_Off();
  osDelay(50);
}

/**
  * @brief  LED Effects Task - runs every 10 ms to update LED patterns.
  * @param  argument: Not used
  * @retval None
  */
static void LED_Effects_Task(void const *argument)
{
  (void)argument;

  static uint32_t tick_counter = 0U;
  static uint32_t step_counter = 0U;
  static int8_t knight_dir = 1;
  static int8_t knight_pos = 0;
  static uint8_t binary_val = 0U;
  static LED_ModeTypeDef last_mode = MODE_MAX;

  /* Tilt ball physics state (fixed-point position 0..300 maps to 4 LEDs) */
  static int16_t ball_pos = 150; /* start centered */
  static int16_t ball_vel = 0;

  Led_TypeDef led_map[4] = {LED4, LED3, LED5, LED6};

  for (;;)
  {
    LED_ModeTypeDef mode = current_mode;

    /* Reset local state whenever mode changes */
    if (mode != last_mode)
    {
      last_mode = mode;
      tick_counter = 0U;
      step_counter = 0U;
      knight_pos = 0;
      knight_dir = 1;
      binary_val = 0U;
      ball_pos = 150;
      ball_vel = 0;
    }

    /* Handle shake-triggered mode switch */
    if (shake_detected != 0U)
    {
      shake_detected = 0U;
      Switch_Mode();
      tick_counter = 0U;
      continue;
    }

    tick_counter++;

    switch (mode)
    {
      /* MODE 0: Single LED chasing (flowing water) */
      case MODE_CHASING:
        if (tick_counter >= 10U) /* 100 ms step */
        {
          tick_counter = 0U;
          All_LEDs_Off();
          BSP_LED_On(led_map[step_counter % 4U]);
          step_counter++;
        }
        break;

      /* MODE 1: Dual LED chase */
      case MODE_DUAL_CHASE:
        if (tick_counter >= 10U) /* 100 ms step */
        {
          tick_counter = 0U;
          All_LEDs_Off();
          BSP_LED_On(led_map[step_counter % 4U]);
          BSP_LED_On(led_map[(step_counter + 2U) % 4U]);
          step_counter++;
        }
        break;

      /* MODE 2: All LEDs blink together */
      case MODE_BLINK_ALL:
        if (tick_counter >= 50U) /* 500 ms toggle */
        {
          tick_counter = 0U;
          BSP_LED_Toggle(LED3);
          BSP_LED_Toggle(LED4);
          BSP_LED_Toggle(LED5);
          BSP_LED_Toggle(LED6);
        }
        break;

      /* MODE 3: Knight Rider scanning effect */
      case MODE_KNIGHT_RIDER:
        if (tick_counter >= 8U) /* 80 ms step */
        {
          tick_counter = 0U;
          All_LEDs_Off();
          BSP_LED_On(led_map[knight_pos]);
          knight_pos += knight_dir;
          if (knight_pos >= 3)
          {
            knight_pos = 3;
            knight_dir = -1;
          }
          else if (knight_pos <= 0)
          {
            knight_pos = 0;
            knight_dir = 1;
          }
        }
        break;

      /* MODE 4: Random sparkle */
      case MODE_RANDOM:
        if (tick_counter >= 15U) /* 150 ms update */
        {
          tick_counter = 0U;
          All_LEDs_Off();
          if ((rand() % 100U) < 70U)
          {
            BSP_LED_On(led_map[rand() % 4U]);
          }
          if ((rand() % 100U) < 50U)
          {
            BSP_LED_On(led_map[rand() % 4U]);
          }
        }
        break;

      /* MODE 5: Binary counter */
      case MODE_BINARY_COUNT:
        if (tick_counter >= 20U) /* 200 ms step */
        {
          tick_counter = 0U;
          All_LEDs_Off();
          if (binary_val & 0x01U) BSP_LED_On(LED4);
          if (binary_val & 0x02U) BSP_LED_On(LED3);
          if (binary_val & 0x04U) BSP_LED_On(LED5);
          if (binary_val & 0x08U) BSP_LED_On(LED6);
          binary_val++;
        }
        break;

      /* MODE 6: Tilt Level (bubble level)
         Board flat -> center LEDs (LED3 + LED5)
         Tilt left  -> LED4 (strong: LED4 only)
         Tilt right -> LED6 (strong: LED6 only)           */
      case MODE_TILT_LEVEL:
        if (tick_counter >= 5U) /* 50 ms update */
        {
          tick_counter = 0U;
          All_LEDs_Off();

          int16_t x = accelero_xyz[0];

          if (x < -ACCELERO_TILT_STRONG)
          {
            BSP_LED_On(LED4);
          }
          else if (x < -ACCELERO_TILT_THRESHOLD)
          {
            BSP_LED_On(LED4);
            BSP_LED_On(LED3);
          }
          else if (x > ACCELERO_TILT_STRONG)
          {
            BSP_LED_On(LED6);
          }
          else if (x > ACCELERO_TILT_THRESHOLD)
          {
            BSP_LED_On(LED5);
            BSP_LED_On(LED6);
          }
          else
          {
            BSP_LED_On(LED3);
            BSP_LED_On(LED5);
          }
        }
        break;

      /* MODE 7: Motion Chase - direction and speed controlled by tilt.
         Tilt left  -> chase flows left, speed increases with angle.
         Tilt right -> chase flows right, speed increases with angle.
         Level      -> pause at current LED.                         */
      case MODE_MOTION_CHASE:
      {
        int16_t x = accelero_xyz[0];
        int32_t abs_x = (x < 0) ? -(int32_t)x : (int32_t)x;
        uint32_t threshold;

        if (abs_x > ACCELERO_TILT_THRESHOLD)
        {
          /* Map tilt magnitude to step period: 100 ms down to ~30 ms */
          threshold = 10U - (abs_x / 60U);
          if (threshold < 3U)
          {
            threshold = 3U;
          }
        }
        else
        {
          /* No significant tilt: hold current LED */
          threshold = 0xFFFFFFFFU;
        }

        if (tick_counter >= threshold)
        {
          tick_counter = 0U;
          All_LEDs_Off();
          if (x < 0)
          {
            /* Tilt left: flow left (decrease index) */
            step_counter = (step_counter + 3U) % 4U;
          }
          else
          {
            /* Tilt right: flow right */
            step_counter = (step_counter + 1U) % 4U;
          }
          BSP_LED_On(led_map[step_counter % 4U]);
        }
        break;
      }

      /* MODE 8: Tilt Ball - virtual ball rolls under gravity with bounce.
         Tilt board to accelerate ball; friction slows it down.
         Ball bounces off the ends.                                */
      case MODE_TILT_BALL:
        if (tick_counter >= 5U) /* 50 ms physics step */
        {
          tick_counter = 0U;
          int16_t x = accelero_xyz[0];

          /* Acceleration proportional to tilt (fixed-point) */
          int16_t acc = (x / 64);
          ball_vel += acc;

          /* Clamp velocity to prevent runaway */
          if (ball_vel > 60)
          {
            ball_vel = 60;
          }
          else if (ball_vel < -60)
          {
            ball_vel = -60;
          }

          /* Simple friction */
          if (ball_vel > 0)
          {
            ball_vel -= 1;
          }
          else if (ball_vel < 0)
          {
            ball_vel += 1;
          }

          ball_pos += ball_vel;

          /* Bounds with elastic bounce */
          if (ball_pos < 0)
          {
            ball_pos = 0;
            ball_vel = -(ball_vel / 2);
          }
          else if (ball_pos > 300)
          {
            ball_pos = 300;
            ball_vel = -(ball_vel / 2);
          }

          All_LEDs_Off();
          uint8_t idx = (uint8_t)(ball_pos / 100);
          if (idx > 3)
          {
            idx = 3;
          }
          BSP_LED_On(led_map[idx]);

          /* Light neighbour LED when ball straddles two positions */
          uint8_t frac = (uint8_t)((ball_pos % 100) / 10);
          if (frac < 3 && idx > 0)
          {
            BSP_LED_On(led_map[idx - 1]);
          }
          else if (frac > 7 && idx < 3)
          {
            BSP_LED_On(led_map[idx + 1]);
          }
        }
        break;

      default:
        All_LEDs_Off();
        break;
    }

    /* 10 ms periodic delay */
    osDelay(LED_EFFECTS_TASK_PERIOD_MS);
  }
}

/**
  * @brief  Button Task - handles mode switching with debounce.
  * @param  argument: Not used
  * @retval None
  */
static void Button_Task(void const *argument)
{
  (void)argument;
  LED_ModeTypeDef last_mode = MODE_MAX;

  for (;;)
  {
    if (button_pressed != 0U)
    {
      osDelay(BUTTON_DEBOUNCE_MS);
      button_pressed = 0U;

      /* Switch to next mode */
      current_mode = (LED_ModeTypeDef)((current_mode + 1) % MODE_MAX);

      /* Visual feedback: flash all LEDs briefly when mode changes */
      if (current_mode != last_mode)
      {
        last_mode = current_mode;
        Switch_Mode();
      }
    }

    osDelay(50);
  }
}

/**
  * @brief  Accelerometer Task - samples MEMS sensor and detects gestures.
  * @param  argument: Not used
  * @retval None
  */
static void Accelerometer_Task(void const *argument)
{
  (void)argument;
  int16_t prev_x = 0;
  int16_t prev_y = 0;
  int16_t prev_z = 0;
  uint32_t shake_cooldown_ticks = 0;

  for (;;)
  {
    BSP_ACCELERO_GetXYZ((int16_t *)accelero_xyz);

    int16_t x = accelero_xyz[0];
    int16_t y = accelero_xyz[1];
    int16_t z = accelero_xyz[2];

    /* Simple tilt classification for potential future use */
    if (x < -ACCELERO_TILT_THRESHOLD)
    {
      tilt_direction = TILT_LEFT;
    }
    else if (x > ACCELERO_TILT_THRESHOLD)
    {
      tilt_direction = TILT_RIGHT;
    }
    else if (y < -ACCELERO_TILT_THRESHOLD)
    {
      tilt_direction = TILT_FRONT;
    }
    else if (y > ACCELERO_TILT_THRESHOLD)
    {
      tilt_direction = TILT_BACK;
    }
    else
    {
      tilt_direction = TILT_NONE;
    }

    /* Shake detection via delta magnitude between samples */
    int32_t dx = (int32_t)x - (int32_t)prev_x;
    int32_t dy = (int32_t)y - (int32_t)prev_y;
    int32_t dz = (int32_t)z - (int32_t)prev_z;
    uint32_t delta = (uint32_t)((dx * dx + dy * dy + dz * dz) / 1000);

    if (shake_cooldown_ticks > 0)
    {
      shake_cooldown_ticks--;
    }

    if ((delta > ACCELERO_SHAKE_THRESHOLD) && (shake_cooldown_ticks == 0))
    {
      shake_detected = 1U;
      shake_cooldown_ticks = ACCELERO_SHAKE_COOLDOWN_MS / ACCELERO_TASK_PERIOD_MS;
    }

    prev_x = x;
    prev_y = y;
    prev_z = z;

    osDelay(ACCELERO_TASK_PERIOD_MS);
  }
}

/**
  * @brief  EXTI line detection callbacks.
  * @param  GPIO_Pin: Specifies the pins connected EXTI line
  * @retval None
  */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  if (GPIO_Pin == KEY_BUTTON_PIN)
  {
    button_pressed = 1U;
  }
}

/**
  * @brief  Turn off all LEDs.
  * @param  None
  * @retval None
  */
static void All_LEDs_Off(void)
{
  BSP_LED_Off(LED3);
  BSP_LED_Off(LED4);
  BSP_LED_Off(LED5);
  BSP_LED_Off(LED6);
}

/**
  * @brief  Turn on all LEDs.
  * @param  None
  * @retval None
  */
static void All_LEDs_On(void)
{
  BSP_LED_On(LED3);
  BSP_LED_On(LED4);
  BSP_LED_On(LED5);
  BSP_LED_On(LED6);
}

/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow :
  *            System Clock source            = PLL (HSE)
  *            SYSCLK(Hz)                     = 168000000
  *            HCLK(Hz)                       = 168000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 4
  *            APB2 Prescaler                 = 2
  *            HSE Frequency(Hz)              = 8000000
  *            PLL_M                          = 8
  *            PLL_N                          = 336
  *            PLL_P                          = 2
  *            PLL_Q                          = 7
  *            VDD(V)                         = 3.3
  *            Main regulator output voltage  = Scale1 mode
  *            Flash Latency(WS)              = 5
  * @param  None
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;

  /* Enable Power Control clock */
  __HAL_RCC_PWR_CLK_ENABLE();

  /* The voltage scaling allows optimizing the power consumption when the device is
     clocked below the maximum system frequency, to update the voltage scaling value
     regarding system frequency refer to product datasheet. */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /* Enable HSE Oscillator and activate PLL with HSE as source */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2
     clocks dividers */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK |
                                  RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }

  /* STM32F405x/407x/415x/417x Revision Z and upper devices: prefetch is supported */
  if (HAL_GetREVID() >= 0x1001)
  {
    /* Enable the Flash prefetch */
    __HAL_FLASH_PREFETCH_BUFFER_ENABLE();
  }
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
void Error_Handler(void)
{
  /* Turn LED3 (green) on to indicate error */
  BSP_LED_On(LED3);
  while (1)
  {
  }
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif
