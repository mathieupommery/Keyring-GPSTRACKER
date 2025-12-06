/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
#include "main.h"
#include "cmsis_os.h"
#include "adc.h"
#include "dma.h"
#include "fatfs.h"
#include "i2c.h"
#include "usart.h"
#include "rtc.h"
#include "spi.h"
#include "tim.h"
#include "usb_device.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "math.h"
#include "ssd1306.h"
#include "statemachine.h"
#include "GNSS.h"
#include "w25q.h"
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

/* USER CODE BEGIN PV */

STATE_TYPE state=STATE_SPEED;
SPEED spdstate=STATE_GROS;
POS posstate=STATE_SUMMARY1;
CHRONO chronostate=STATE_RESET;
BALISESTATE balisestate=BALISESTATE1;
ACCELSTATE accelstate=WAITFORGPS;
uint8_t workingbuffer[110];
int BTN_A=0;
int BTN_B=0;

uint16_t rawdata[3];
float temp=0.0;
float vrefint=0;
float vbat=0;


uint8_t bufferscreen[50];
uint8_t longbufferscreen[256];
uint8_t usbbuffer[64];
uint16_t offsetforscroltext=0;
uint16_t scrolltextmax=0;

float vitmax=0.0;
float seconde=0;
float min=0;

uint32_t starttime=0;
uint32_t calctime=0;
uint32_t timehandler=0;
float t1=0;
float t2=0;
float t3=0;
float moy=0;
float framerate=0;


int doubledonnee=0;
int cptdoubledonnee=0;
double distanceparcouru=0;
double oldlat=0;
double oldlong=0;




float altibaro=0;


uint8_t SEC=0;
uint8_t HR=0;
uint8_t MINUTE=0;
uint8_t JOURS=10;
uint8_t MOIS=11;
uint16_t ANNEE=2024;
int settimeen=0;


int boutonAtime=0;
int boutonBtime=0;
int tbtn1=0;
int tbtn2=0;
int BTN_B_LONG=0;
int BTN_A_LONG=0;


ECRANBALISESTATE ecranstate=ECRANBALISESTATE1;
GNSS_StateHandle GNSSData;
SPIF_HandleTypeDef hspif1;

int received_flag=0;

int modesansechec=0;

int flag_usb_mounted=0;

float tscal2=1365.0;
float tscal1=1034.0;



const unsigned char startimg[] = {
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe,
0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x3e,
0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf9, 0xce,
0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfa, 0x2e,
0x7f, 0xfc, 0x1f, 0xff, 0xff, 0xff, 0xf6, 0xb6,
0x7f, 0xf0, 0x07, 0xff, 0xff, 0xff, 0xf6, 0x76,
0x7f, 0xc0, 0x01, 0xff, 0xff, 0xff, 0xfa, 0xae,
0x7f, 0x80, 0x00, 0xff, 0xff, 0xff, 0xf9, 0xce,
0x7f, 0x81, 0xc0, 0xff, 0xff, 0xff, 0xfe, 0x3e,
0x7f, 0x03, 0xe0, 0x7f, 0xff, 0xff, 0xff, 0xfe,
0x7f, 0x07, 0xf0, 0x7f, 0x0f, 0x03, 0xf0, 0x3e,
0x7f, 0x07, 0xf0, 0x7f, 0x0f, 0x03, 0xf0, 0x3e,
0x7f, 0x07, 0xf0, 0x7c, 0xff, 0x3c, 0xcf, 0xfe,
0x7f, 0x03, 0xe0, 0x7c, 0xff, 0x3c, 0xcf, 0xfe,
0x7f, 0x01, 0xc0, 0x7c, 0xc3, 0x03, 0xf0, 0xfe,
0x7f, 0x80, 0x00, 0xfc, 0xc3, 0x03, 0xf0, 0xfe,
0x7f, 0x80, 0x00, 0xfc, 0xf3, 0x3f, 0xff, 0x3e,
0x7f, 0xc0, 0x01, 0xfc, 0xf3, 0x3f, 0xff, 0x3e,
0x7f, 0xc0, 0x01, 0xff, 0x0f, 0x3f, 0xc0, 0xfe,
0x7f, 0xe0, 0x03, 0xff, 0x0f, 0x3f, 0xc0, 0xfe,
0x7f, 0xe0, 0x07, 0xff, 0xff, 0xff, 0xff, 0xfe,
0x7f, 0xf0, 0x07, 0xff, 0xff, 0xff, 0xff, 0xfe,
0x7f, 0xf0, 0x0f, 0xff, 0xff, 0x76, 0xfe, 0x7e,
0x7f, 0xf8, 0x0f, 0xff, 0xff, 0x74, 0xfd, 0xbe,
0x7f, 0xf8, 0x1f, 0xff, 0xff, 0xae, 0xfd, 0xbe,
0x7f, 0xfc, 0x1f, 0xff, 0xff, 0xae, 0xfd, 0xbe,
0x7f, 0xfc, 0x3f, 0xff, 0xff, 0xdc, 0x6e, 0x7e,
0x7f, 0xfe, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xfe,
0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe,
0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void MX_FREERTOS_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)//lors d'un appuie sur un bouton, le systeme s'interrompt afin d'arriver dans cette fonction redefinie avec en parametre d'entre , le bouton sur lequel l'on a appuiyé
{
	if(GPIO_Pin==GPIO_PIN_14){

		if(HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_14)== GPIO_PIN_RESET){
					boutonAtime=0;
					tbtn1=HAL_GetTick();
				}
				else{
					boutonAtime=HAL_GetTick()-tbtn1;
					tbtn1=0;
				}


		if(boutonAtime>=50 && boutonAtime<=400){
			BTN_A++;
			BTN_A_LONG=0;
		}
		if(boutonAtime>=400){
			BTN_A_LONG++;
			BTN_A=0;
		}

	}
	if(GPIO_Pin==GPIO_PIN_15){

		if(HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_15)== GPIO_PIN_RESET){
			boutonBtime=0;
			tbtn2=HAL_GetTick();
		}
		else{
			boutonBtime=HAL_GetTick()-tbtn2;
			tbtn2=0;
	}
		if(boutonBtime>=50 && boutonBtime<=400){
			BTN_B++;
			BTN_B_LONG=0;
		}
		if(boutonBtime>=400){
			BTN_B_LONG++;
			BTN_B=0;
		}



	}

}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc){
	if(hadc->Instance==ADC1){
		vrefint=(float) ((4095.0*1.212)/rawdata[0]);
		temp=(float) (((100.0)/(tscal2-tscal1))*(rawdata[1]*(vrefint/3.0)-tscal1))+30.0;
		vbat=(float) 2*(rawdata[2]/4095.0)*vrefint;
	}
	HAL_ADC_Start_DMA(&hadc1,(uint32_t*)rawdata, 3);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
	if(huart->Instance==LPUART1){
		received_flag=1;
		memcpy((uint8_t*)GNSSData.uartWorkingBuffer,(uint8_t *) workingbuffer,100);
		__HAL_DMA_DISABLE_IT(&hdma_lpuart1_rx, DMA_IT_HT);
		HAL_UART_Receive_DMA(&hlpuart1, (uint8_t *)workingbuffer, 100);

	}

}




/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  void (*boot_jump)(void);

  __HAL_RCC_GPIOC_CLK_ENABLE();
   GPIO_InitStruct.Pin = GPIO_PIN_15|GPIO_PIN_14;
   GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
   GPIO_InitStruct.Pull = GPIO_NOPULL;
   HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
   /* If both Select and Reset button held down at boot time, then immediately
    * jump to DFU bootloader, rather than start the Snickerdoodle application.
    */
   if ((HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_14) == GPIO_PIN_RESET) &&
 	  (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_15) == GPIO_PIN_RESET))
   {
 	  HAL_DeInit();
 	  boot_jump = (void (*)(void))(*((uint32_t *)(SYS_MEM_START_ADDR + 4)));
 	  __set_MSP(*(__IO uint32_t*)SYS_MEM_START_ADDR);

 	  /* NOTE WELL: This call never returns: */
 	  boot_jump();
   }
   if(HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_14)==GPIO_PIN_RESET){
	   modesansechec=1;
   }
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_I2C1_Init();
  MX_LPUART1_UART_Init();
  MX_SPI1_Init();
  MX_ADC1_Init();
  MX_TIM6_Init();
  MX_FATFS_Init();
  MX_RTC_Init();
  /* USER CODE BEGIN 2 */

	ssd1306_Init();
	HAL_Delay(10);
	ssd1306_Fill(Black);

	ssd1306_DrawBitmap(32, 32, startimg, 64, 32, White);
	ssd1306_UpdateScreen();


	HAL_ADC_Start_DMA(&hadc1,(uint32_t*)rawdata, 3);
	HAL_TIM_Base_Start(&htim6);

	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_0,GPIO_PIN_SET);
	HAL_Delay(100);
	GNSS_Init(&GNSSData, &hlpuart1);


	HAL_UART_Abort(&hlpuart1);
	HAL_UART_Receive_DMA(&hlpuart1, (uint8_t *)workingbuffer, 100);


	if(SPIF_Init(&hspif1,&hspi1,GPIOB,GPIO_PIN_7)==true){
		ssd1306_SetCursor(32, 32);
		ssd1306_Fill(Black);
		ssd1306_WriteString("OK", Font_7x10, White);
		ssd1306_UpdateScreen();
		HAL_Delay(1000);
	}
	else{
		ssd1306_SetCursor(32, 32);
		ssd1306_Fill(Black);
		ssd1306_WriteString("PBM", Font_7x10, White);
		ssd1306_UpdateScreen();
		HAL_Delay(1000);

	}

	ssd1306_Fill(Black);
	state=STATE_SPEED;
	BTN_A=0;
	BTN_A_LONG=0;

	HAL_Delay(200);
  /* USER CODE END 2 */

  /* Call init function for freertos objects (in cmsis_os2.c) */
  MX_FREERTOS_Init();

  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI48|RCC_OSCILLATORTYPE_HSI
                              |RCC_OSCILLATORTYPE_LSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSI48State = RCC_HSI48_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 10;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM1 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM1)
  {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
