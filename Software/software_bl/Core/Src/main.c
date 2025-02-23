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
#include "ipcc.h"
#include "usart.h"
#include "memorymap.h"
#include "rf.h"
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
#include "nmea_parse.h"
#include "stm32wbxx_hal.h"
#include "spif.h"
#include "PADS.h"
#include "tarvos.h"
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
KEYBOARD keyboardstate=STATE_MARCHE;
USBSTATE usbstate=USBSTATE1;
BALISESTATE balisestate=BALISESTATE1;
TARVOSSTATE tarvosstate=TARVOS1;
BAROSTATE barostate=BARO1;


uint16_t oldPos = 0;
uint16_t newPos = 0;
uint16_t TarvosRXbufferoldpos;
uint16_t TarvosRXbuffernewpos;
uint8_t RxBuffer[RxBuffer_SIZE];
uint8_t DataBuffer[DataBuffer_SIZE];
uint8_t tarvos_RX_Buffer[TarvosRxBufferSize];
uint8_t tarvos_RX_Tampon[TarvosRxTamponSize];
GPS myData;
int BTN_A=0;
int BTN_B=0;



SPIF_HandleTypeDef hspif1;
extern SPI_HandleTypeDef hspi1;
extern TIM_HandleTypeDef htim16;
extern TIM_HandleTypeDef htim17;

const unsigned char startimg[] = {
		 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
		  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
		  0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03,
		  0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x03,
		  0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x83,
		  0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x43,
		  0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x12, 0x43,
		  0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0a, 0x83,
		  0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x03,
		  0xc0, 0x3e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x03,
		  0xc0, 0xff, 0x80, 0x00, 0x00, 0x00, 0x00, 0x07, 0x03,
		  0xc3, 0xff, 0xe0, 0x3f, 0x07, 0xf8, 0x3f, 0xca, 0x83,
		  0xc7, 0xff, 0xf0, 0x3f, 0x07, 0xf8, 0x3f, 0xd2, 0x43,
		  0xc7, 0xe3, 0xf0, 0x3f, 0x07, 0xf8, 0x3f, 0xc2, 0x43,
		  0xcf, 0xc1, 0xf9, 0xc0, 0x07, 0x07, 0x38, 0x02, 0x83,
		  0xcf, 0x80, 0xf9, 0xc0, 0x07, 0x07, 0x38, 0x03, 0x03,
		  0xcf, 0x80, 0xf9, 0xc0, 0x07, 0x07, 0x38, 0x00, 0x03,
		  0xcf, 0x80, 0xf9, 0xc0, 0x07, 0x07, 0x38, 0x00, 0x03,
		  0xcf, 0xc1, 0xf9, 0xc7, 0xe7, 0x07, 0x3f, 0xc0, 0x03,
		  0xcf, 0xe3, 0xf9, 0xc7, 0xe7, 0x07, 0x3f, 0xc0, 0x03,
		  0xc7, 0xff, 0xf1, 0xc7, 0xe7, 0xf8, 0x3f, 0xc0, 0x03,
		  0xc7, 0xff, 0xf1, 0xc0, 0xe7, 0xf8, 0x01, 0xc0, 0x03,
		  0xc3, 0xff, 0xe1, 0xc0, 0xe7, 0xf8, 0x01, 0xc0, 0x03,
		  0xc3, 0xff, 0xe1, 0xc0, 0xe7, 0x00, 0x01, 0xc0, 0x03,
		  0xc1, 0xff, 0xc1, 0xc0, 0xe7, 0x00, 0x01, 0xc0, 0x03,
		  0xc1, 0xff, 0x80, 0x3f, 0x07, 0x00, 0x3f, 0xc0, 0x03,
		  0xc0, 0xff, 0x80, 0x3f, 0x07, 0x00, 0x3f, 0xc0, 0x03,
		  0xc0, 0xff, 0x00, 0x3f, 0x07, 0x00, 0x3f, 0xc0, 0x03,
		  0xc0, 0x7f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03,
		  0xc0, 0x7e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03,
		  0xc0, 0x3e, 0x00, 0x00, 0x00, 0x22, 0x40, 0xf0, 0x03,
		  0xc0, 0x3c, 0x00, 0x00, 0x00, 0x22, 0xc0, 0x80, 0x03,
		  0xc0, 0x1c, 0x00, 0x00, 0x00, 0x14, 0x40, 0xe0, 0x03,
		  0xc0, 0x00, 0x00, 0x00, 0x00, 0x14, 0x40, 0x10, 0x03,
		  0xc0, 0x00, 0x00, 0x00, 0x00, 0x08, 0xe4, 0xe0, 0x03,
		  0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03,
		  0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03,
		  0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03,
		  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
		  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
		};


//const unsigned char startimg[] = {
//0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe,
//0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x3e,
//0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf9, 0xce,
//0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfa, 0x2e,
//0x7f, 0xfc, 0x1f, 0xff, 0xff, 0xff, 0xf6, 0xb6,
//0x7f, 0xf0, 0x07, 0xff, 0xff, 0xff, 0xf6, 0x76,
//0x7f, 0xc0, 0x01, 0xff, 0xff, 0xff, 0xfa, 0xae,
//0x7f, 0x80, 0x00, 0xff, 0xff, 0xff, 0xf9, 0xce,
//0x7f, 0x81, 0xc0, 0xff, 0xff, 0xff, 0xfe, 0x3e,
//0x7f, 0x03, 0xe0, 0x7f, 0xff, 0xff, 0xff, 0xfe,
//0x7f, 0x07, 0xf0, 0x7f, 0x0f, 0x03, 0xf0, 0x3e,
//0x7f, 0x07, 0xf0, 0x7f, 0x0f, 0x03, 0xf0, 0x3e,
//0x7f, 0x07, 0xf0, 0x7c, 0xff, 0x3c, 0xcf, 0xfe,
//0x7f, 0x03, 0xe0, 0x7c, 0xff, 0x3c, 0xcf, 0xfe,
//0x7f, 0x01, 0xc0, 0x7c, 0xc3, 0x03, 0xf0, 0xfe,
//0x7f, 0x80, 0x00, 0xfc, 0xc3, 0x03, 0xf0, 0xfe,
//0x7f, 0x80, 0x00, 0xfc, 0xf3, 0x3f, 0xff, 0x3e,
//0x7f, 0xc0, 0x01, 0xfc, 0xf3, 0x3f, 0xff, 0x3e,
//0x7f, 0xc0, 0x01, 0xff, 0x0f, 0x3f, 0xc0, 0xfe,
//0x7f, 0xe0, 0x03, 0xff, 0x0f, 0x3f, 0xc0, 0xfe,
//0x7f, 0xe0, 0x07, 0xff, 0xff, 0xff, 0xff, 0xfe,
//0x7f, 0xf0, 0x07, 0xff, 0xff, 0xff, 0xff, 0xfe,
//0x7f, 0xf0, 0x0f, 0xff, 0xff, 0x76, 0xfe, 0x7e,
//0x7f, 0xf8, 0x0f, 0xff, 0xff, 0x74, 0xfd, 0xbe,
//0x7f, 0xf8, 0x1f, 0xff, 0xff, 0xae, 0xfd, 0xbe,
//0x7f, 0xfc, 0x1f, 0xff, 0xff, 0xae, 0xfd, 0xbe,
//0x7f, 0xfc, 0x3f, 0xff, 0xff, 0xdc, 0x6e, 0x7e,
//0x7f, 0xfe, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xfe,
//0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe,
//0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe,
//0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//};

uint16_t rawdata[3];
float temp=0.0;
float vrefint=0;
float vbat=0;


uint8_t flashwrite[256];
uint8_t flashread[256];
extern uint8_t indexbuffer[50];
int pageoffset=0;
int pagenumber=0;
int sectoreraseen=0;
uint8_t numbuf1[10];
uint8_t numbuf2[10];
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
int flashbufferlen=0;

int erasetime=0;
int erasedisplay=0;
int usbtransmiten=0;
float usbpercent=0;

int doubledonnee=0;
int cptdoubledonnee=0;
double distanceparcouru=0;
double oldlat=0;
double oldlong=0;

int32_t baropress=0;
int16_t barotemp=0;
int baroenableinit=0;
float altibaro=0;

int enablewrite=0;

double distance1sec=0;


uint8_t SEC=0;
uint8_t HR=0;
uint8_t MINUTE=0;
uint8_t JOURS=10;
uint8_t MOIS=11;
uint16_t ANNEE=2024;
MOIS_STATE mois=JANVIER;
int settimeen=0;


int boutonAtime=0;
int boutonBtime=0;
int tbtn1=0;
int tbtn2=0;
int BTN_B_LONG=0;
int BTN_A_LONG=0;




int bluetoothsend=0;
uint8_t blereceivebuf[256];
uint8_t bletransmitbuf[256];
uint8_t receivedtrame[64];



ECRANBALISESTATE ecranstate=ECRANBALISESTATE1;
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
	if(GPIO_Pin==GPIO_PIN_14){

		if(HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_14)== GPIO_PIN_RESET){

					boutonAtime=0;
					tbtn1=HAL_GetTick();


				}
				else{
					BTN_A++;

					boutonAtime=HAL_GetTick()-tbtn1;
					tbtn1=0;


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
			BTN_B++;

			boutonBtime=HAL_GetTick()-tbtn2;
			tbtn2=0;


		}

		if(boutonBtime>=400){
			BTN_B_LONG++;
			BTN_B=0;
		}



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
  /* Config code for STM32_WPAN (HSE Tuning must be done before system clock configuration) */
  MX_APPE_Config();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* Configure the peripherals common clocks */
  PeriphCommonClock_Config();

  /* IPCC initialisation */
  MX_IPCC_Init();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_RF_Init();
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_ADC1_Init();
  MX_I2C1_Init();
  MX_I2C3_Init();
  MX_LPUART1_UART_Init();
  MX_TIM2_Init();
  MX_TIM16_Init();
  MX_TIM17_Init();
  MX_SPI1_Init();
  MX_USART1_UART_Init();
  MX_USB_Device_Init();
  MX_RTC_Init();
  /* USER CODE BEGIN 2 */



  	ssd1306_Init();

  	HAL_Delay(10);
  	ssd1306_Fill(Black);

  	ssd1306_DrawBitmap(30, 12, startimg, 72, 40, White);
  	ssd1306_UpdateScreen();


  	HAL_ADC_Start_DMA(&hadc1,(uint32_t*)rawdata, 3);
  	HAL_TIM_Base_Start(&htim2);
  	HAL_TIM_Base_Start_IT(&htim16);
  	//HAL_TIM_Base_Start_IT(&htim17);

  	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_0,GPIO_PIN_SET);

  	HAL_Delay(100);
  	HAL_UART_Abort(&hlpuart1);
  	HAL_UART_Receive_DMA(&hlpuart1, (uint8_t *)RxBuffer, RxBuffer_SIZE);


  	INIT_PERM_TARVOS(0x15, 0x16);


  	HAL_UART_Abort(&huart1);
  	HAL_UART_Receive_DMA(&huart1, (uint8_t *)tarvos_RX_Tampon, TarvosRxTamponSize);


  	memset(flashread,'1',256);
  	memset(flashwrite,'\0',256);
  	memset((uint8_t *)bufferscreen ,'\0',50);



  	SPIF_Init(&hspif1, &hspi1, GPIOB, GPIO_PIN_7);





  	getindex();

  	ssd1306_Fill(Black);


  	state=STATE_SPEED;
  	BTN_A=0;
  	BTN_A_LONG=0;

  	HAL_Delay(500);


  	//LL_HSEM_1StepLock( HSEM, 5 );
  /* USER CODE END 2 */

  /* Init code for STM32_WPAN */
  MX_APPE_Init();

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */
    MX_APPE_Process();

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
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV2;
  RCC_OscInitStruct.PLL.PLLN = 8;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure the SYSCLKSource, HCLK, PCLK1 and PCLK2 clocks dividers
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK4|RCC_CLOCKTYPE_HCLK2
                              |RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.AHBCLK2Divider = RCC_SYSCLK_DIV2;
  RCC_ClkInitStruct.AHBCLK4Divider = RCC_SYSCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
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
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

  /** Initializes the peripherals clock
  */
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_SMPS|RCC_PERIPHCLK_RFWAKEUP
                              |RCC_PERIPHCLK_USB|RCC_PERIPHCLK_ADC;
  PeriphClkInitStruct.PLLSAI1.PLLN = 6;
  PeriphClkInitStruct.PLLSAI1.PLLP = RCC_PLLP_DIV2;
  PeriphClkInitStruct.PLLSAI1.PLLQ = RCC_PLLQ_DIV2;
  PeriphClkInitStruct.PLLSAI1.PLLR = RCC_PLLR_DIV2;
  PeriphClkInitStruct.PLLSAI1.PLLSAI1ClockOut = RCC_PLLSAI1_USBCLK|RCC_PLLSAI1_ADCCLK;
  PeriphClkInitStruct.UsbClockSelection = RCC_USBCLKSOURCE_PLLSAI1;
  PeriphClkInitStruct.AdcClockSelection = RCC_ADCCLKSOURCE_PLLSAI1;
  PeriphClkInitStruct.RFWakeUpClockSelection = RCC_RFWKPCLKSOURCE_HSE_DIV1024;
  PeriphClkInitStruct.SmpsClockSelection = RCC_SMPSCLKSOURCE_HSI;
  PeriphClkInitStruct.SmpsDivSelection = RCC_SMPSCLKDIV_RANGE1;

  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN Smps */
  LL_HSEM_1StepLock( HSEM, 5 );

  /* USER CODE END Smps */
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
