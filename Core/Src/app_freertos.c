/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : app_freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "foc.h"
#include "usart.h"
#include "AS5600.h"
#include "foc.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
extern TIM_HandleTypeDef htim3;
extern UART_HandleTypeDef huart1;

#define _3PI_2 4.71238898038f
extern float zero_electric_angle;
extern float AngleDegrees;
/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 128 * 4
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
void vTestTask(void *pvParameters);
/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  xTaskCreate(vTestTask,"vTestTask",512,NULL,osPriorityNormal1,NULL);
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
  for(;;)
  {
  for (int i=0;i<=100;i++) {
    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1,i);
    osDelay(10);
  }

  for (int i=100;i>=0;i--) {
    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1,i);
    osDelay(10);
  }
  osDelay(10);

    AS5600_Read();
  }
  /* USER CODE END StartDefaultTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
void vTestTask(void *pvParameters) {
  // 1. 让磁场停在电角度 0 度的位置 (Ud=3, Uq=0, angle=0)
  // setPhaseVoltage(0, 3.0f, 0);
  // vTaskDelay(3000);
  //
  // // 2. 核心：直接计算机械角度对应的电角度，不要调用 CalElectricalAngle()
  // // 这里的偏移量 = 当前的机械角度(弧度) * 极对数
  // zero_electric_angle = (1 * AngleDegrees * PI / 180.0f) * (float)7;
  // zero_electric_angle = _normalizeAngle(zero_electric_angle);
  //
  // // 3. 此时，你的 CalElectricalAngle() 算出来的结果在当前位置应该接近 0
  // setPhaseVoltage(0, 0, 0); // 停机备用

  while (1) {
    // HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_6);

    // HAL_UART_Transmit_DMA(&huart1,"Hi DMA\r\n",sizeof("Hi DMA\r\n"));

    // osDelay(1);
    // FOC_test();
  }

}
/* USER CODE END Application */

