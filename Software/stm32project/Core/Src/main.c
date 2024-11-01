/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
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
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "dma.h"
#include "i2c.h"
#include "usart.h"
#include "spi.h"
#include "tim.h"
#include "usb_device.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */


#include "math.h"
#include "ssd1306.h"
#include "statemachine.h"
#include "nmea_parse.h"
#include "stm32l4xx_hal.h"
#include "spif.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
//#define TS_CAL1 *((uint16_t*) 0x1FFF75A8)
//#define TS_CAL2 *((uint16_t*) 0x1FFF75CA)
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */


uint16_t oldPos = 0;//variables servants à mettre à remplir le databuffer de manière circulaire.
uint16_t newPos = 0;
uint8_t RxBuffer[RxBuffer_SIZE];//buffer permettant dans lequel le dma stocke les trames recu 64 caractères à la fois
uint8_t DataBuffer[DataBuffer_SIZE];//grand buffer dans lequel on vient traiter les trames, il est mis à jour de manière circulaire.
GPS myData;//structure stockant toutes les informations que l'on  souhaite obtenir par le gps, ainsi que les informations sur sont port de connection
int BTN_A=0;//variables servants à stocker les appuie sur le bouton a ou b, elles ne prendrons que les valeurs 0 ou 1.
int BTN_B=0;
STATE_TYPE state=STATE_SPEED;//état principaux de la machine à état
HEURE hrstate=STATE_DIGIT;
SPEED spdstate=STATE_SUMMARY;
POS posstate=STATE_SUMMARY1;
CHRONO chronostate=STATE_RESET;
KEYBOARD keyboardstate=STATE_MARCHE;
GIF gifstate=GIF1;
keyboardHID keyboardhid = {0,0,0,0,0,0,0,0};
SPIF_HandleTypeDef hspif1;
extern SPI_HandleTypeDef hspi1;

const unsigned char startimg[] = {//image de démarrage
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

uint16_t rawdata[3];
float temp=0.0;
float vrefint=0;
float vbat=0;
extern char str[20];

uint8_t flashwrite[256];
uint8_t flashread[256];
extern uint8_t indexbuffer[50];
int pageoffset=0;
int pagenumber=0;
int sectoreraseen=0;
uint8_t numbuf1[10];
uint8_t numbuf2[10];




/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void PeriphCommonClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */


void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)//lors d'un appuie sur un bouton, le systeme s'interrompt afin d'arriver dans cette fonction redefinie avec en parametre d'entre , le bouton sur lequel l'on a appuiyé
{
	if(GPIO_Pin==GPIO_PIN_8){
//		ssd1306_SetCursor(33, 44);
//		ssd1306_WriteString("btna", Font_6x8, White);
//		ssd1306_UpdateScreen();
		BTN_A++;//sert à reconnaitre lorsque le bouto na est appuyer, cette variable est mise à 1 par un e interruption.


	}
	if(GPIO_Pin==GPIO_PIN_1){
		//ssd1306_SetCursor(33, 44);
		//ssd1306_WriteString("btnb", Font_6x8, White);
		//ssd1306_UpdateScreen();
		BTN_B++;//de meme pour le bouton b

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

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

/* Configure the peripherals common clocks */
  PeriphCommonClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_I2C3_Init();
  MX_LPUART1_UART_Init();
  MX_ADC1_Init();
  MX_TIM2_Init();
  MX_USB_DEVICE_Init();
  MX_I2C1_Init();
  MX_USART1_UART_Init();
  MX_SPI1_Init();
  /* USER CODE BEGIN 2 */



	ssd1306_Init();

	HAL_Delay(100);
	ssd1306_Fill(Black);

	ssd1306_DrawBitmap(32, 32, startimg, 64, 64, White);
	ssd1306_UpdateScreen();

	HAL_Delay(500);

	HAL_ADC_Start_DMA(&hadc1,(uint32_t*)rawdata, 3);
	HAL_TIM_Base_Start(&htim2);

	HAL_UART_Abort(&hlpuart1);
	HAL_UART_Receive_DMA(&hlpuart1, (uint8_t *)RxBuffer, RxBuffer_SIZE);

	memset(flashread,'1',256);
	memset(flashwrite,'\0',256);

	SPIF_Init(&hspif1, &hspi1, GPIOB, GPIO_PIN_0);

	ssd1306_Fill(Black);

	getindex();

	snprintf((uint8_t*)str,20, "off=%d",pageoffset);
	ssd1306_SetCursor(32,40);
	ssd1306_WriteString((uint8_t*)str,Font_6x8,White);
	snprintf((uint8_t*)str,20, "page=%d",pagenumber);
	ssd1306_SetCursor(32,48);
	ssd1306_WriteString((uint8_t*)str,Font_6x8,White);
	ssd1306_UpdateScreen();

	HAL_Delay(1000);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	while (1)
	{
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
		statemachine();
		ssd1306_UpdateScreen();
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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = 0;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 20;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief Peripherals Common Clock Configuration
  * @retval None
  */
void PeriphCommonClock_Config(void)
{
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the peripherals clock
  */
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USB|RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCCLKSOURCE_PLLSAI1;
  PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_PLLSAI1;
  PeriphClkInit.PLLSAI1.PLLSAI1Source = RCC_PLLSOURCE_MSI;
  PeriphClkInit.PLLSAI1.PLLSAI1M = 1;
  PeriphClkInit.PLLSAI1.PLLSAI1N = 24;
  PeriphClkInit.PLLSAI1.PLLSAI1P = RCC_PLLP_DIV7;
  PeriphClkInit.PLLSAI1.PLLSAI1Q = RCC_PLLQ_DIV2;
  PeriphClkInit.PLLSAI1.PLLSAI1R = RCC_PLLR_DIV2;
  PeriphClkInit.PLLSAI1.PLLSAI1ClockOut = RCC_PLLSAI1_48M2CLK|RCC_PLLSAI1_ADC1CLK;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

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

#ifdef  USE_FULL_ASSERT
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
