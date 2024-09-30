/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define D2_Pin GPIO_PIN_2
#define D2_GPIO_Port GPIOE
#define D3_Pin GPIO_PIN_3
#define D3_GPIO_Port GPIOE
#define D4_Pin GPIO_PIN_4
#define D4_GPIO_Port GPIOE
#define D5_Pin GPIO_PIN_5
#define D5_GPIO_Port GPIOE
#define D6_Pin GPIO_PIN_6
#define D6_GPIO_Port GPIOE
#define D7_Pin GPIO_PIN_7
#define D7_GPIO_Port GPIOE
#define D8_Pin GPIO_PIN_8
#define D8_GPIO_Port GPIOE
#define D9_Pin GPIO_PIN_9
#define D9_GPIO_Port GPIOE
#define D10_Pin GPIO_PIN_10
#define D10_GPIO_Port GPIOE
#define D11_Pin GPIO_PIN_11
#define D11_GPIO_Port GPIOE
#define D12_Pin GPIO_PIN_12
#define D12_GPIO_Port GPIOE
#define D13_Pin GPIO_PIN_13
#define D13_GPIO_Port GPIOE
#define D14_Pin GPIO_PIN_14
#define D14_GPIO_Port GPIOE
#define D15_Pin GPIO_PIN_15
#define D15_GPIO_Port GPIOE
#define CS_Pin GPIO_PIN_10
#define CS_GPIO_Port GPIOB
#define T_IRQ_Pin GPIO_PIN_1
#define T_IRQ_GPIO_Port GPIOD
#define RESET_Pin GPIO_PIN_3
#define RESET_GPIO_Port GPIOD
#define RD_Pin GPIO_PIN_4
#define RD_GPIO_Port GPIOD
#define WR_Pin GPIO_PIN_5
#define WR_GPIO_Port GPIOD
#define RS_Pin GPIO_PIN_6
#define RS_GPIO_Port GPIOD
#define T_CS_Pin GPIO_PIN_7
#define T_CS_GPIO_Port GPIOD
#define D0_Pin GPIO_PIN_0
#define D0_GPIO_Port GPIOE
#define D1_Pin GPIO_PIN_1
#define D1_GPIO_Port GPIOE
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
