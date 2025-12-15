/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
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
#include "ssd1306.h"
#include "statemachine.h"
#include "GNSS.h"
#include "usb_device.h"
#include "w25q.h"
#include "pwr.h"
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

extern AppStateMachineContext state_struct;
extern GNSS_StateHandle GNSSData;
extern SPIF_HandleTypeDef hspif1;
extern Buttons_t gButtons;
extern AdcContext_t gAdc;
/* USER CODE END Variables */
osThreadId MainTaskHandle;
osThreadId SensorTaskHandle;
osThreadId TrackerTaskHandle;
osThreadId PWRTaskHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartMainTask(void const * argument);
void StartSensorTask(void const * argument);
void StartTrackerTask(void const * argument);
void StartPWRTask(void const * argument);

extern void MX_USB_DEVICE_Init(void);
void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );

/* USER CODE BEGIN GET_IDLE_TASK_MEMORY */
static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];

void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize )
{
  *ppxIdleTaskTCBBuffer = &xIdleTaskTCBBuffer;
  *ppxIdleTaskStackBuffer = &xIdleStack[0];
  *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
  /* place for user code */
}
/* USER CODE END GET_IDLE_TASK_MEMORY */

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
  /* definition and creation of MainTask */
  osThreadDef(MainTask, StartMainTask, osPriorityNormal, 0, 512);
  MainTaskHandle = osThreadCreate(osThread(MainTask), NULL);

  /* definition and creation of SensorTask */
  osThreadDef(SensorTask, StartSensorTask, osPriorityNormal, 0, 256);
  SensorTaskHandle = osThreadCreate(osThread(SensorTask), NULL);

  /* definition and creation of TrackerTask */
  osThreadDef(TrackerTask, StartTrackerTask, osPriorityNormal, 0, 512);
  TrackerTaskHandle = osThreadCreate(osThread(TrackerTask), NULL);

  /* definition and creation of PWRTask */
  osThreadDef(PWRTask, StartPWRTask, osPriorityHigh, 0, 256);
  PWRTaskHandle = osThreadCreate(osThread(PWRTask), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_StartMainTask */
/**
  * @brief  Function implementing the MainTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartMainTask */
void StartMainTask(void const * argument)
{
  /* init code for USB_DEVICE */
  MX_USB_DEVICE_Init();
  /* USER CODE BEGIN StartMainTask */
  TickType_t xLastWakeTime;
  const TickType_t period = pdMS_TO_TICKS(50);
  xLastWakeTime = xTaskGetTickCount();
  /* Infinite loop */
  for(;;)
  {
	  HAL_GPIO_TogglePin(LED_GREEN_GPIO_Port,LED_GREEN_Pin);
	  StateMachine_Run(&state_struct,&GNSSData,&gButtons,&gAdc);
	  ssd1306_UpdateScreen();
	  vTaskDelayUntil(&xLastWakeTime, period);
  }
  /* USER CODE END StartMainTask */
}

/* USER CODE BEGIN Header_StartSensorTask */
/**
* @brief Function implementing the SensorTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartSensorTask */
void StartSensorTask(void const * argument)
{
  /* USER CODE BEGIN StartSensorTask */
	  TickType_t xLastWakeTime;
	  const TickType_t period = pdMS_TO_TICKS(50);
	  xLastWakeTime = xTaskGetTickCount();
  /* Infinite loop */
  for(;;)
  {
	  if(GNSSData.received_flag==1){

	  		GNSS_ParsePVTData(&GNSSData);
	  		GNSSData.received_flag=0;
	  }

	  vTaskDelayUntil(&xLastWakeTime, period);
  }
  /* USER CODE END StartSensorTask */
}

/* USER CODE BEGIN Header_StartTrackerTask */
/**
* @brief Function implementing the TrackerTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTrackerTask */
void StartTrackerTask(void const * argument)
{
  /* USER CODE BEGIN StartTrackerTask */
	  TickType_t xLastWakeTime;
	  const TickType_t period = pdMS_TO_TICKS(200);
	  xLastWakeTime = xTaskGetTickCount();
  /* Infinite loop */
  for(;;)
  {
	  HAL_GPIO_TogglePin(LED_BLUE_GPIO_Port,LED_BLUE_Pin);

	  vTaskDelayUntil(&xLastWakeTime, period);
  }
  /* USER CODE END StartTrackerTask */
}

/* USER CODE BEGIN Header_StartPWRTask */
/**
* @brief Function implementing the PWRTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartPWRTask */
void StartPWRTask(void const * argument)
{
  /* USER CODE BEGIN StartPWRTask */
	  TickType_t xLastWakeTime;
	  const TickType_t period = pdMS_TO_TICKS(5);
	  xLastWakeTime = xTaskGetTickCount();
  /* Infinite loop */
  for(;;)
  {
	  PWR_ProcessPWButton(&gButtons);
	  vTaskDelayUntil(&xLastWakeTime, period);
  }
  /* USER CODE END StartPWRTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */
