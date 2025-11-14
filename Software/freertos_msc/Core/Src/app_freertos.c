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
osThreadId defaultTaskHandle;
osThreadId GNSSPARSEHandle;
osThreadId BALISEHandle;
osThreadId USBTRANSMITHandle;
osThreadId BAROCALIBHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void const * argument);
void StartTask02(void const * argument);
void StartTask03(void const * argument);
void StartTask04(void const * argument);
void StartTask05(void const * argument);

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
  /* definition and creation of defaultTask */
  osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 256);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* definition and creation of GNSSPARSE */
  osThreadDef(GNSSPARSE, StartTask02, osPriorityAboveNormal, 0, 256);
  GNSSPARSEHandle = osThreadCreate(osThread(GNSSPARSE), NULL);

  /* definition and creation of BALISE */
  osThreadDef(BALISE, StartTask03, osPriorityRealtime, 0, 256);
  BALISEHandle = osThreadCreate(osThread(BALISE), NULL);

  /* definition and creation of USBTRANSMIT */
  osThreadDef(USBTRANSMIT, StartTask04, osPriorityHigh, 0, 256);
  USBTRANSMITHandle = osThreadCreate(osThread(USBTRANSMIT), NULL);

  /* definition and creation of BAROCALIB */
  osThreadDef(BAROCALIB, StartTask05, osPriorityBelowNormal, 0, 256);
  BAROCALIBHandle = osThreadCreate(osThread(BAROCALIB), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  osThreadSuspend(BALISEHandle);
  osThreadSuspend(USBTRANSMITHandle);
  osThreadSetPriority(USBTRANSMITHandle, osPriorityRealtime);
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void const * argument)
{
  /* init code for USB_Device */
  MX_USB_Device_Init();
  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
  for(;;)
  {
	statemachine();
	ssd1306_UpdateScreen();
    osDelay(50);

  }
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_StartTask02 */
/**
* @brief Function implementing the myTask02 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTask02 */
void StartTask02(void const * argument)
{
  /* USER CODE BEGIN StartTask02 */
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
  /* USER CODE END StartTask02 */
}

/* USER CODE BEGIN Header_StartTask03 */
/**
* @brief Function implementing the myTask03 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTask03 */
void StartTask03(void const * argument)
{
  /* USER CODE BEGIN StartTask03 */
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
  /* USER CODE END StartTask03 */
}

/* USER CODE BEGIN Header_StartTask04 */
/**
* @brief Function implementing the USBTRANSMIT thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTask04 */
void StartTask04(void const * argument)
{
  /* USER CODE BEGIN StartTask04 */
  /* Infinite loop */
  for(;;)
  {
	  HAL_GPIO_TogglePin(GPIOB,GPIO_PIN_1);
	  osDelay(100);
  }
  /* USER CODE END StartTask04 */
}

/* USER CODE BEGIN Header_StartTask05 */
/**
* @brief Function implementing the BAROCALIB thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTask05 */
void StartTask05(void const * argument)
{
  /* USER CODE BEGIN StartTask05 */
  /* Infinite loop */
  for(;;)
  {

	  if(GNSSData.fixType>=3){
		  P0 =(double) bmppress / powf((1 - (GNSSData.fhMSL / 44330.0f)), 5.255f);

	  }
    osDelay(20000);
  }
  /* USER CODE END StartTask05 */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

