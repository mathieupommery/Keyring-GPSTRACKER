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

#include "usb_device.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "ssd1306.h"
#include "statemachine.h"
#include "GNSS.h"
#include "stm32wbxx_hal.h"
#include "bmp581.h"
#include "spi.h"
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
extern GNSS_StateHandle GNSSData;
extern int received_flag;
extern double oldlat;
extern double oldlong;
extern double distanceparcouru;
extern int pageoffset;
extern int pagenumber;
extern double bmpalt;
extern float vitmax;
extern BALISESTATE balisestate;
extern BMP_t * bmp581;
extern float vbat;
extern float temp;
extern uint8_t flashwrite[256];
extern int flashbufferlen;
extern double P0;
extern double bmppress;
/* USER CODE END Variables */
/* Definitions for statemachine */
osThreadId_t statemachineHandle;
const osThreadAttr_t statemachine_attributes = {
  .name = "statemachine",
  .priority = (osPriority_t) osPriorityNormal5,
  .stack_size = 2048 * 4
};
/* Definitions for Parsetask */
osThreadId_t ParsetaskHandle;
const osThreadAttr_t Parsetask_attributes = {
  .name = "Parsetask",
  .priority = (osPriority_t) osPriorityNormal4,
  .stack_size = 256 * 4
};
/* Definitions for myTaskstore */
osThreadId_t myTaskstoreHandle;
const osThreadAttr_t myTaskstore_attributes = {
  .name = "myTaskstore",
  .priority = (osPriority_t) osPriorityNormal4,
  .stack_size = 256 * 4
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartstatemachineTask(void *argument);
void StartTaskparse(void *argument);
void StartTaskstore(void *argument);

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
  /* creation of statemachine */
  statemachineHandle = osThreadNew(StartstatemachineTask, NULL, &statemachine_attributes);

  /* creation of Parsetask */
  ParsetaskHandle = osThreadNew(StartTaskparse, NULL, &Parsetask_attributes);

  /* creation of myTaskstore */
  myTaskstoreHandle = osThreadNew(StartTaskstore, NULL, &myTaskstore_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  osThreadSuspend(myTaskstoreHandle);
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartstatemachineTask */
/**
  * @brief  Function implementing the statemachine thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartstatemachineTask */
void StartstatemachineTask(void *argument)
{
  /* init code for USB_Device */
  MX_USB_Device_Init();
  /* USER CODE BEGIN StartstatemachineTask */
  /* Infinite loop */
  for(;;)
  {
		statemachine();
		ssd1306_UpdateScreen();
	    osDelay(50);
  }
  /* USER CODE END StartstatemachineTask */
}

/* USER CODE BEGIN Header_StartTaskparse */
/**
* @brief Function implementing the Parsetask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTaskparse */
void StartTaskparse(void *argument)
{
  /* USER CODE BEGIN StartTaskparse */
  /* Infinite loop */
  for(;;)
  {
	  if(received_flag==1){

	  		GNSS_ParsePVTData(&GNSSData);
	  		bmp581_read_precise_normal(&bmp581);
	  		  received_flag=0;


	  	  }
	HAL_GPIO_TogglePin(GPIOB,GPIO_PIN_0);
    osDelay(100);
  }
  /* USER CODE END StartTaskparse */
}

/* USER CODE BEGIN Header_StartTaskstore */
/**
* @brief Function implementing the myTaskstore thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTaskstore */
void StartTaskstore(void *argument)
{
  /* USER CODE BEGIN StartTaskstore */
  /* Infinite loop */
  for(;;)
  {
	  HAL_GPIO_TogglePin(GPIOB,GPIO_PIN_1);
	  if(GNSSData.fgSpeed>=vitmax){
							  									 vitmax=GNSSData.fgSpeed;
							  								 }



								distanceparcouru=distanceparcouru + distancecalc(oldlat,GNSSData.fLat,oldlong, GNSSData.fLon);
								  oldlat=GNSSData.fLat;
								  oldlong=GNSSData.fLon;
								  //ecrire ici


    osDelay(200);
  }
  /* USER CODE END StartTaskstore */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

