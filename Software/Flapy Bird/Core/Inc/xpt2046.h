#ifndef __XPT2046_H
#define __XPT2046_H

#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_spi.h"

/*Chieu dai truc x*/
/*Chieu dai truc y*/

#define cmd_X  0x90
#define cmd_Y  0xD0

#define T_WIDTH  	 3230
#define X_T_MIN	 	 470

#define T_HEIGH		 3050
#define Y_T_MIN		 470

#define LCD_WID  	 319.0
#define LCD_HEI	  	 239.0

#define READX		 1
#define READY     	 0

#define T_CSN  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_7, GPIO_PIN_RESET)
#define T_CS HAL_GPIO_WritePin(GPIOD, GPIO_PIN_7, GPIO_PIN_SET)


uint8_t  Read_IRQ();
uint16_t XPT2046_Read(uint8_t cmd);
uint16_t  XPT2046_Calibrate(uint16_t coor, uint16_t r);


#endif
