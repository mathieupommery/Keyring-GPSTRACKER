/*
 * PADS.h
 *
 *  Created on: Nov 14, 2024
 *      Author: mathi
 */

#ifndef INC_PADS_H_
#define INC_PADS_H_



#endif /* INC_PADS_H_ */


#include "main.h"

uint8_t PADS_init(I2C_HandleTypeDef *i2c);

uint8_t PADS_single(I2C_HandleTypeDef *i2c);


HAL_StatusTypeDef PADS_continuous_init(I2C_HandleTypeDef *i2c);

HAL_StatusTypeDef PADS_continuous_read(I2C_HandleTypeDef *i2c);

