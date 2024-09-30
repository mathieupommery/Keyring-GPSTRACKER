/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "image.h"
#include "line.h"
#include "button.h"
#include "stm32f4xx_hal.h"
#include "xpt2046.h"
#include "ssd1289.h"
#include "stdlib.h"

#define WIDTH_PILL		40 	  /*Chieu rong ong khoi*/

#define HIDDENT			50	  /*Phan ong khoi bi an ben trai*/

#define HEIGH_SCREEN 	215	  /*Chieu cao man hinh hien thi chim*/

#define DISTANCE		60		/*Khoang cach giua hai ong khoi*/

#define YES				1
#define NO				0

#define WIDTH_BIRD		26	/*Chieu rong cua hinh chim*/
#define HEIGH_BIRD 	 	19	/*Chieu cao cua hinh chim*/

#define WIDTH_LINE		320
#define HEIGH_LINE		5

#define BIRD_X			110		/*toa do X cua chim*/
#define BIRD_Y			100

#define X_COM       	136

#define FALLING			0		/* Bay xuong */
#define RISING			1		/* Bay len	*/

#define X_SCORE			75
#define Y_SCORE			223
#define X_H_SCORE		180

#define X_GAME_OVER		90
#define Y_GAME_OVER		40

#define X_GAME_SCORE	95
#define Y_GAME_SCORE	70

#define X_SOCRE_END		165
#define SCORED			69

#define X_BUT			140
#define Y_BUT			100

SPI_HandleTypeDef hspi3;

uint16_t list_heigh[15];
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
SPI_HandleTypeDef hspi3;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_SPI3_Init(void);
/* USER CODE BEGIN PFP */
void Init_Game();
void Draw_Pillar(int16_t x, uint16_t heigh);
void Init_Heigh_Pill();
void Bird_Fly(uint16_t y);
void Convert_To_String(uint16_t val);
void Print_Score(uint16_t, uint16_t x_score, uint16_t color);
void Game_Over();
int Button_Press(uint16_t x, uint16_t y, uint16_t width, uint16_t heigh);
void Welcome();
void Click_Here();
uint16_t score = 0; /*Diem so*/
uint16_t high_score = 0;


int16_t x1 = 0, x2 = 0;	/*Toa do x cua 2 ong khoi xuat hien tren man hinh*/

uint16_t heigh_pill1 = 0;	/*Chieu cao ong khoi khi bat day game*/
uint16_t heigh_pill2 = 0;

int16_t y_bird = 0;

uint16_t x_com = 0;
uint16_t h_com = 0;
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

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

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_SPI3_Init();
  /* USER CODE BEGIN 2 */
  SSD1289_Init();
  SSD1289_Clear_Screen();

//  Init_Game(); /*Khoi tao game*/

  Welcome();

  int c = 0;
  int score_old = 0;
  int game_over=NO;

  while (1)
  {
	  if(!Read_IRQ()){
		  if((game_over==NO)||Button_Press(X_BUT, Y_BUT, BUT_WIDTH, BUT_HEIGH)){
		  	  score_old = score;
		  	  c = 0;
			  Init_Game();

			  if(score_old > high_score){
			  		high_score = score_old;
			  		Print_Score(high_score, 300, Hscolor);
			  }
			  while(1){
					  if(x1 == 70){	/*Neu ong khoi 1 di qua toa to 110 thi tao ong khoi 2*/
						  x2 = 320;
						  heigh_pill2 = list_heigh[rand()%15];
					  }
					  if(x2 == 70){   /*Neu ong khoi di qua toa do 110 thi tao them ong khoi*/
						  x1 = 320;
						  heigh_pill1 = list_heigh[rand()%15];
					  }

					  /*Ve ong khoi*/
					  Draw_Pillar(x1, heigh_pill1);
					  Draw_Pillar(x2, heigh_pill2);


					  if(!Read_IRQ()){	/*Neu co tin hieu cam ung*/
						  y_bird -= 5;
						  c = 1;
					  }
					  else if(c){	// Neu chua cham lan nao thi chim van dung yen
						  y_bird += 2;
					  }

					  if(x1 > 70){
						  x_com = x1;
						  h_com = heigh_pill1-1;
					  }
					  else{
						  x_com = x2;
						  h_com = heigh_pill2-1;
					  }
					  if(((x_com==X_COM)&&((y_bird<=h_com)||(y_bird+HEIGH_BIRD>=(h_com+DISTANCE))))||
						((y_bird<=h_com+1)&&( ((X_COM>=x_com)&&
						(X_COM <= (x_com+WIDTH_PILL)))||((BIRD_X>=x_com)&&(BIRD_X<=(x_com+WIDTH_PILL)))))||
						(((y_bird+HEIGH_BIRD)>=(h_com+DISTANCE))&&
						(((X_COM>=x_com)&&(X_COM <= (x_com+WIDTH_PILL)))||((BIRD_X>=x_com)&&
						(BIRD_X<=(x_com+WIDTH_PILL)))))||
						  ((y_bird+HEIGH_BIRD)>=HEIGH_SCREEN)){

						  game_over = YES;
						  Game_Over();
						  break;
					  }

					  if(x1 == SCORED || x2 == SCORED){
						  score += 1;
						  Print_Score(score, X_SCORE, Blue);
					  }

					  if(c){
						  Bird_Fly(y_bird);
						  x1--;
						  x2--;
					  }
					  HAL_Delay(10);
			  }
	  	  }
		}
  	  }

}

void Init_Heigh_Pill(){
	int i =0;
	int value = 0;
	for(i=0; i<15; i++){
		list_heigh[i] = 150-value;
		value += 10;
	}

}

void Welcome(){
	SSD1289_Write_Back_Ground(Black);
	SSD1289_Print_String(50, 40, 2, 1, 4, White, Black, "Flapy Bird");
	SSD1289_Print_String(40, 185, 1, 1, 2, White, Black, "Click to enter the game!");
}
void Draw_Pillar(int16_t x, uint16_t heigh){
	if(x < 321){
	/*Xoa ong khoi cu*/
		int16_t x1 = x+WIDTH_PILL+1;

		SSD1289_Draw_V_Line(x1, 1, heigh, Background);				/*Xoa ong khoi top*/
		SSD1289_Draw_V_Line(x1, 1+heigh+DISTANCE, HEIGH_SCREEN, Background);   /*Xoa ong khoi bot*/

	/*Ve ong khoi moi*/
		SSD1289_Draw_V_Line(x, 1, heigh, Green);
		SSD1289_Draw_V_Line(x, 1+heigh+DISTANCE, HEIGH_SCREEN, Green);
	}
}


void Bird_Fly(uint16_t y){
	SSD1289_Fill_Color(BIRD_X, y-3, (BIRD_X+WIDTH_BIRD+5), (y+HEIGH_BIRD+5), Background);
	HAL_Delay(5);
	SSD1289_Write_Pic_Coor(BIRD_X, y, WIDTH_BIRD, HEIGH_BIRD, bird_image);
}

void Print_Score(uint16_t val, uint16_t x_score, uint16_t color){
	uint16_t a = 0, b = 0;
	char string[3] = {0, 0, '\0'};
	/*Xoa diem cu*/
	SSD1289_Fill_Color(x_score, Y_SCORE, x_score+20, 239, Yellow);
	if(val < 10){
		SSD1289_Print_Char(x_score, Y_SCORE, 2, color, Yellow, (val+48));
	}
	else{
		a = val%10 + 48;	/*Hang don vi*/
		b = val/10	+ 48;	/*Hang chuc*/
		string[0] = b;
		string[1] = a;
		SSD1289_Print_String(x_score, Y_SCORE, 1, 1, 2, color, Yellow, string);
	}

}

void Game_Over(){

	SSD1289_Fill_Color(X_GAME_OVER-40, Y_GAME_OVER-10, X_GAME_OVER+120, Y_GAME_OVER+70, Background);
	SSD1289_Print_String(X_GAME_OVER, Y_GAME_OVER, 2, 2, 3, Red, Background, "Game Over");
	SSD1289_Print_String(X_GAME_SCORE, Y_GAME_SCORE, 2, 2, 2, Red, Background, "Score: ");
	uint16_t a = 0, b = 0;
	char string[3] = {0, 0, '\0'};
	if(score < 10){
		SSD1289_Print_Char(X_SOCRE_END, Y_GAME_SCORE, 2, Red, Background, (score+48));
	}
	else{
		a = score%10 + 48;	/*Hang don vi*/
		b = score/10	+ 48;	/*Hang chuc*/
		string[0] = b;
		string[1] = a;
		SSD1289_Print_String(X_SOCRE_END, Y_GAME_SCORE, 2, 2, 2, Red, Background, string);
	}

	HAL_Delay(1500);
	SSD1289_Write_Pic_Coor(X_BUT, Y_BUT, BUT_WIDTH, BUT_HEIGH, button);
}

int Button_Press(uint16_t x, uint16_t y, uint16_t width, uint16_t heigh){
	uint16_t lcd_x = 0, lcd_y = 0;
	if(!Read_IRQ()){
		lcd_x = XPT2046_Calibrate(XPT2046_Read(cmd_X), 1);
		lcd_y = XPT2046_Calibrate(XPT2046_Read(cmd_Y), 0);

		if(((lcd_x>x)&&(lcd_x<(x+width)))&&((lcd_y>y)&&(lcd_y<(y+heigh))))
			return YES;
		else
			return NO;
	}
	return NO;
}

void Init_Game(){
	  Init_Heigh_Pill();
	  score = 0;
	  x1 = 320;
	  x2 = 600;	/*Toa do x cua 2 ong khoi xuat hien tren man hinh*/
	  heigh_pill1 = list_heigh[rand()%15];	/*Chieu cao ong khoi khi bat day game*/
	  heigh_pill2 = 100;
	  y_bird = BIRD_Y;
	  x_com = 320;
	  h_com = heigh_pill1;
	  SSD1289_Write_Back_Ground(Background);
	  SSD1289_Write_Pic_Coor(BIRD_X, BIRD_Y, WIDTH_BIRD, HEIGH_BIRD, bird_image);
	  SSD1289_Write_Pic_Coor(0, HEIGH_SCREEN, WIDTH_LINE, HEIGH_LINE, line);
	  SSD1289_Fill_Color(0, HEIGH_SCREEN+5, 319, 239, Yellow);
	  SSD1289_Print_String(10, HEIGH_SCREEN+8, 1, 1, 2, Blue, Yellow, "Score: ");
	  Print_Score(score, X_SCORE, Blue);
	  SSD1289_Print_String(X_H_SCORE, HEIGH_SCREEN+8, 1, 1, 2, Hscolor, Yellow, "High Score: ");
	  Print_Score(high_score, 300, Hscolor);
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
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
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

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief SPI3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI3_Init(void)
{

  /* USER CODE BEGIN SPI3_Init 0 */

  /* USER CODE END SPI3_Init 0 */

  /* USER CODE BEGIN SPI3_Init 1 */

  /* USER CODE END SPI3_Init 1 */
  /* SPI3 parameter configuration*/
  hspi3.Instance = SPI3;
  hspi3.Init.Mode = SPI_MODE_MASTER;
  hspi3.Init.Direction = SPI_DIRECTION_2LINES;
  hspi3.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi3.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi3.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi3.Init.NSS = SPI_NSS_SOFT;
  hspi3.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16;
  hspi3.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi3.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi3.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi3.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI3_Init 2 */

  /* USER CODE END SPI3_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOE, D2_Pin|D3_Pin|D4_Pin|D5_Pin
                          |D6_Pin|D7_Pin|D8_Pin|D9_Pin
                          |D10_Pin|D11_Pin|D12_Pin|D13_Pin
                          |D14_Pin|D15_Pin|D0_Pin|D1_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, RESET_Pin|RD_Pin|WR_Pin|RS_Pin
                          |T_CS_Pin, GPIO_PIN_SET);

  /*Configure GPIO pins : D2_Pin D3_Pin D4_Pin D5_Pin
                           D6_Pin D7_Pin D8_Pin D9_Pin
                           D10_Pin D11_Pin D12_Pin D13_Pin
                           D14_Pin D15_Pin D0_Pin D1_Pin */
  GPIO_InitStruct.Pin = D2_Pin|D3_Pin|D4_Pin|D5_Pin
                          |D6_Pin|D7_Pin|D8_Pin|D9_Pin
                          |D10_Pin|D11_Pin|D12_Pin|D13_Pin
                          |D14_Pin|D15_Pin|D0_Pin|D1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pin : CS_Pin */
  GPIO_InitStruct.Pin = CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(CS_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : T_IRQ_Pin */
  GPIO_InitStruct.Pin = T_IRQ_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(T_IRQ_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : RESET_Pin RD_Pin WR_Pin RS_Pin
                           T_CS_Pin */
  GPIO_InitStruct.Pin = RESET_Pin|RD_Pin|WR_Pin|RS_Pin
                          |T_CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

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
