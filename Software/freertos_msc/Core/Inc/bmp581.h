/*
 * bmp581.h
 *
 *  Created on: Mar 8, 2025
 *      Author: mathi
 */

#ifndef INC_BMP581_H_
#define INC_BMP581_H_



#endif /* INC_BMP581_H_ */


#include "main.h"





typedef struct bmp_t{

	I2C_HandleTypeDef *i2c;
	uint8_t addr;
	int8_t data[6];
	float press;
	float temps;
	float altitude;




}BMP_t;


uint8_t bmp581_init_precise_normal(BMP_t * bmp581);
uint8_t bmp581_read_precise_normal(BMP_t * bmp581);

#define BMP581_I2C_ADDR 0x47
#define BMP581_WRITE_ADDR (0X47<<1)+0
#define BMP581_READ_ADDR (0X47<<1)+1


#define BMP581_CHIP_ID_REG        0x01  // Identifiant du capteur
#define BMP581_REV_ID       0x02
#define BMP581_CHIP_STATUS        0x11
#define BMP581_DRIVE_CONFIG       0x13
#define BMP581_INT_CONFIG        0x14
#define BMP581_INT_SOURCE        0x15

#define BMP581_FIFO_CONFIG        0x16
#define BMP581_FIFO_COUNT        0x17
#define BMP581_FIFO_SEL        0x18

#define BMP581_TEMP_DATA_XLSB        0x1D
#define BMP581_TEMP_DATA_LSB         0x1E
#define BMP581_TEMP_DATA_MSB        0x1F

#define BMP581_PRESS_DATA_XLSB        0x20
#define BMP581_PRESS_DATA_LSB         0x21
#define BMP581_PRESS_DATA_MSB        0x22



#define BMP581_INT_STATUS        0x27
#define BMP581_STATUS        0x28


#define BMP581_FIFO_DATA        0x29



#define BMP581_NVM_ADDR        0x2B
#define BMP581_NVM_DATA_LSB       0x2C
#define BMP581_NVM_DATA_MSB       0x2D

#define BMP581_DSP_CONFIG        0x30
#define BMP581_DSP_IIR        0x31

#define BMP581_OOR_THR_P_LSB        0x32
#define BMP581_OOR_THR_P_MSB        0x33
#define BMP581_OOR_RANGE       0x34
#define BMP581_OOR_CONFIG        0x35

#define BMP581_OSR_CONFIG      0x36
#define BMP581_ODR_CONFIG        0x37

#define BMP581_OSR_EFF      0x38

#define BMP581_CMD      0x7E






