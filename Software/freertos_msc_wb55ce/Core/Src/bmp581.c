/*
 * bmp581.c
 *
 *  Created on: Mar 8, 2025
 *      Author: mathi
 */


#include "bmp581.h"
#include "math.h"

extern I2C_HandleTypeDef hi2c1;

double bmptemp=0.0;
double bmppress=0.0;
double bmpalt=0.0;
extern float temp;
uint8_t odrcheck=0;
float P0=101325.0;

//Ox18 donc 11000 pour ODR donc 5hz en mode normal avec oversampling a 128 pour la pression et 8 pour la temperature

uint8_t bmp581_init_precise_normal(BMP_t * bmp581){

	uint8_t OSR_tmask = 0b01111011;
	uint8_t ODR_tmask = 0b01101001;
	uint8_t DSP_conf_mask = 0b00101011;
	uint8_t DSP_conf_mask2 = 0b00010010;
	int check=0;

	if(HAL_I2C_Mem_Write(&hi2c1, BMP581_WRITE_ADDR, BMP581_OSR_CONFIG, 1, &OSR_tmask, 1, 100)!=HAL_OK){
			check=1;
		}
	if(HAL_I2C_Mem_Write(&hi2c1, BMP581_WRITE_ADDR, BMP581_ODR_CONFIG, 1, &ODR_tmask, 1, 100)!=HAL_OK){
					check=1;
				}
	if(HAL_I2C_Mem_Write(&hi2c1, BMP581_WRITE_ADDR, BMP581_DSP_CONFIG, 1, &DSP_conf_mask, 1, 100)!=HAL_OK){
				check=1;
			}
	if(HAL_I2C_Mem_Write(&hi2c1, BMP581_WRITE_ADDR, BMP581_DSP_CONFIG, 1, &DSP_conf_mask2, 1, 100)!=HAL_OK){
					check=1;
				}
	if(HAL_I2C_Mem_Read(&hi2c1, BMP581_READ_ADDR, BMP581_OSR_EFF, 1, &odrcheck, 1, 100)!=HAL_OK){
					check=1;
				}


	return check;

}




uint8_t bmp581_read_precise_normal(BMP_t * bmp581){
		int check=0;
		uint8_t recarray[6];
		int32_t intbuffertemp=0;
		int32_t intbufferpres=0;
		if(HAL_I2C_Mem_Read(&hi2c1, BMP581_READ_ADDR, BMP581_TEMP_DATA_XLSB, 1, &recarray, 6, 100)!=HAL_OK){
					check=1;
				}


		if(check==0){

		intbuffertemp=(recarray[2]<<16)|(recarray[1]<<8)|(recarray[0]);
		intbufferpres=(recarray[5]<<16)|(recarray[4]<<8)|(recarray[3]);
		bmp581->temps=(float)intbuffertemp/65536.0;
		bmp581->press=(float) intbufferpres/64.0;
		bmp581->altitude=(float) ((8.314*298.15)/(9.80665*0.028964))*logf(P0/(bmp581->press));

		}

		return check;
}
