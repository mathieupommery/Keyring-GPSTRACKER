/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>

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
#define PWR_BTN_Pin GPIO_PIN_0
#define PWR_BTN_GPIO_Port GPIOA
#define PWR_BTN_EXTI_IRQn EXTI0_IRQn
#define GPS_EN_Pin GPIO_PIN_1
#define GPS_EN_GPIO_Port GPIOA
#define AUX_EN_Pin GPIO_PIN_4
#define AUX_EN_GPIO_Port GPIOA
#define FLASH_CS_Pin GPIO_PIN_1
#define FLASH_CS_GPIO_Port GPIOB
#define B1_Pin GPIO_PIN_15
#define B1_GPIO_Port GPIOA
#define B1_EXTI_IRQn EXTI15_10_IRQn
#define B2_Pin GPIO_PIN_3
#define B2_GPIO_Port GPIOB
#define B2_EXTI_IRQn EXTI3_IRQn
#define LED_GREEN_Pin GPIO_PIN_5
#define LED_GREEN_GPIO_Port GPIOB
#define LED_BLUE_Pin GPIO_PIN_6
#define LED_BLUE_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */
#define SYS_MEM_START_ADDR 0x1FFF0000//OK


#define STM32L432_VREFINT_CAL_ADDR   ((volatile uint16_t*)0x1FFF75AAu)
#define STM32L432_TS_CAL1_ADDR       ((volatile uint16_t*)0x1FFF75A8u)
#define STM32L432_TS_CAL2_ADDR       ((volatile uint16_t*)0x1FFF75CAu)

#define STM32L432_VREFINT_CAL()      (*STM32L432_VREFINT_CAL_ADDR)
#define STM32L432_TS_CAL1()          (*STM32L432_TS_CAL1_ADDR)
#define STM32L432_TS_CAL2()          (*STM32L432_TS_CAL2_ADDR)

#define STM32L432_VDDA_CAL_V         (3.0f)
#define STM32L432_TS_CAL1_TEMP_C     (30.0f)
#define STM32L432_TS_CAL2_TEMP_C     (130.0f)

typedef struct
{
    uint8_t BTN_A;
    uint8_t BTN_B;

    uint8_t BTN_A_LONG;
    uint8_t BTN_B_LONG;

    uint32_t time_A_ms;
    uint32_t time_B_ms;
    uint32_t time_PW_ms;

    uint32_t pressStart_A_ms;
    uint32_t pressStart_B_ms;
    uint32_t pressStart_PW_ms;

    uint8_t PW_BTN_RISING_FLAG;
    uint8_t PW_BTN_FALLING_FLAG;

} Buttons_t;

typedef struct
{
    float temp;          // °C
    float vbat;          // V
    float vrefint;       // Vref calculée

    uint16_t raw[3];

} AdcContext_t;


/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
