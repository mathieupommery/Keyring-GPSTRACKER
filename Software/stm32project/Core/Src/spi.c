/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    spi.c
  * @brief   This file provides code for the configuration
  *          of the SPI instances.
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
#include "spi.h"


/* USER CODE BEGIN 0 */
#include <stdio.h>
#include <string.h>
#include "statemachine.h"
#include "spif.h"
#include "nmea_parse.h"
#include "ssd1306.h"
extern float temp;
extern float vbat;
extern GPS myData;
uint8_t indexbuffer[50];//pageindex,pagenumber,sectornumber
extern SPIF_HandleTypeDef hspif1;
extern int pageoffset;
extern int pagenumber;
extern uint8_t numbuf1[10];
extern uint8_t numbuf2[10];
extern int sectoreraseen;
/* USER CODE END 0 */

SPI_HandleTypeDef hspi1;

/* SPI1 init function */
void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 7;
  hspi1.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
  hspi1.Init.NSSPMode = SPI_NSS_PULSE_ENABLE;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

void HAL_SPI_MspInit(SPI_HandleTypeDef* spiHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(spiHandle->Instance==SPI1)
  {
  /* USER CODE BEGIN SPI1_MspInit 0 */

  /* USER CODE END SPI1_MspInit 0 */
    /* SPI1 clock enable */
    __HAL_RCC_SPI1_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**SPI1 GPIO Configuration
    PA5     ------> SPI1_SCK
    PA6     ------> SPI1_MISO
    PB5     ------> SPI1_MOSI
    */
    GPIO_InitStruct.Pin = GPIO_PIN_5|GPIO_PIN_6;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF5_SPI1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_5;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF5_SPI1;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* USER CODE BEGIN SPI1_MspInit 1 */

  /* USER CODE END SPI1_MspInit 1 */
  }
}

void HAL_SPI_MspDeInit(SPI_HandleTypeDef* spiHandle)
{

  if(spiHandle->Instance==SPI1)
  {
  /* USER CODE BEGIN SPI1_MspDeInit 0 */

  /* USER CODE END SPI1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_SPI1_CLK_DISABLE();

    /**SPI1 GPIO Configuration
    PA5     ------> SPI1_SCK
    PA6     ------> SPI1_MISO
    PB5     ------> SPI1_MOSI
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_5|GPIO_PIN_6);

    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_5);

  /* USER CODE BEGIN SPI1_MspDeInit 1 */

  /* USER CODE END SPI1_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */
int csvframe(uint8_t* buffer,float temp,float vbat,GPS * gpsdata,int otherval1,float otherval2){
	int framesize=0;
	framesize=snprintf((uint8_t*)buffer,256, "%0.2f,%0.2f,%0.2f,%0.7f,%0.7f,%0.2f,%d,%0.2f\n\r",temp,vbat,(gpsdata->speed)*3.6,gpsdata->latitude,gpsdata->longitude,gpsdata->altitude,otherval1,otherval2);
	return framesize;
}


void storeindex(void){
	int writepage=MAX_WRITE_PAGE+1;
	uint8_t writebuffer[100];
	memset((uint8_t*)writebuffer,'\0',100);
	snprintf((uint8_t*)writebuffer,100, "%d$%d$",pageoffset,pagenumber);
	SPIF_EraseSector(&hspif1, (int)floor((writepage)/16));
	SPIF_WritePage(&hspif1,writepage, (uint8_t *)writebuffer, 100,0);

}

void getindex(void){
	int readpage=MAX_WRITE_PAGE+1;
	SPIF_ReadPage(&hspif1, readpage, (uint8_t *)indexbuffer, 50, 0);
	memset(numbuf1,'$',10);
	memset(numbuf2,'$',10);
	int cnt=0;
	if((indexbuffer[0]&0x0F)<10 ){
	while(indexbuffer[cnt]!='$'){

			  numbuf1[cnt]=indexbuffer[cnt];
			  cnt++;
		  }
		  cnt++;
		  int cnt1=0;
		  while(indexbuffer[cnt]!='$'){

		  		  numbuf2[cnt1]=indexbuffer[cnt];
		  		  cnt1++;
		  		  cnt++;
		  	  }

		 pageoffset=atoi((uint8_t*)numbuf1);
		 pagenumber=atoi((uint8_t*)numbuf2);
	}
	else{
		storeindex();
	}

}

void writebuffertoflash(uint8_t * buffer,int bufferlenght){
	if((pagenumber+1)%16==0){
		if(sectoreraseen==0){
		SPIF_EraseSector(&hspif1, (int)floor((pagenumber+1)/16));
		sectoreraseen=1;
		}

		  }
	if(bufferlenght+pageoffset<256){
		SPIF_WritePage(&hspif1,pagenumber, (uint8_t *)buffer, bufferlenght,pageoffset);
		pageoffset=pageoffset+bufferlenght;
	}
	else{
		SPIF_WritePage(&hspif1,pagenumber, (uint8_t *)buffer, 256-pageoffset,pageoffset);
		HAL_Delay(100);
		SPIF_WritePage(&hspif1,pagenumber+1, (uint8_t *)buffer+(256-pageoffset), bufferlenght-(256-pageoffset),0);
		pagenumber=pagenumber+1;
		sectoreraseen=0;
		pageoffset=(bufferlenght-(256-pageoffset));
	}
	storeindex();

}

/* USER CODE END 1 */