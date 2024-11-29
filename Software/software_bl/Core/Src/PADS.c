/*
 * PADS.c
 *
 *  Created on: Nov 14, 2024
 *      Author: mathi
 */



#include "PADS.h"
#include "math.h"


float finaltemp=0;
float finalpress=0;
float alt=0;

int inttemp=0;
int intpress=0;



uint8_t PADS_init(I2C_HandleTypeDef *i2c){

	int check=0;
	uint8_t data=0;
	if(HAL_I2C_Mem_Read(i2c, 0xBA, 0x0F, 1, &data, 1, 100)!=HAL_OK){
		  check=1;
	  }
	if(data!=0xB3){
		check=1;
	}
	return check;


}



uint8_t PADS_single(I2C_HandleTypeDef *i2c){
	int check=0;
	uint8_t cmdhandler=0;
	uint8_t recarray[5];
	int16_t intbuffertemp=0;
	int32_t intbufferpres=0;
	if(HAL_I2C_Mem_Read(i2c, 0xBB, 0x11, 1, &cmdhandler, 1, 100)!=HAL_OK){
		check=1;
	}
	HAL_Delay(2);

	cmdhandler=cmdhandler | 0x01;

	if(HAL_I2C_Mem_Write(i2c, 0xBA, 0x11, 1, &cmdhandler, 1, 100)!=HAL_OK){
		check=1;
	}
	cmdhandler=0;
	HAL_Delay(2);

	if(HAL_I2C_Mem_Read(i2c, 0xBB, 0x28, 1, &recarray[0], 1, 100)!=HAL_OK){
		check=1;
	}
	HAL_Delay(2);
	if(HAL_I2C_Mem_Read(i2c, 0xBB, 0x29, 1, &recarray[1], 1, 100)!=HAL_OK){
		check=1;
	}
	HAL_Delay(2);
	if(HAL_I2C_Mem_Read(i2c, 0xBB, 0x2A, 1, &recarray[2], 1, 100)!=HAL_OK){
		check=1;
	}
	HAL_Delay(2);
	if(HAL_I2C_Mem_Read(i2c, 0xBB, 0x2B, 1, &recarray[3], 1, 100)!=HAL_OK){
		check=1;
	}
	HAL_Delay(2);
	if(HAL_I2C_Mem_Read(i2c, 0xBB, 0x2C, 1, &recarray[4], 1, 100)!=HAL_OK){
		check=1;
	}
	HAL_Delay(2);

	intbufferpres=(recarray[2]<<16)|(recarray[1]<<8)|(recarray[0]);
	intbuffertemp=(recarray[4]<<8)|(recarray[3]);
	inttemp=intbuffertemp;
	intpress=intbufferpres;

	finaltemp=(float)intbuffertemp*0.01;
	finalpress=(float) intbufferpres*(0.00002441406);

	alt=(float)(298.15/0.0065)*(1-powf((float)(finalpress*1000)/101325.0, (float)(287.05*0.0065)/9.80665));

	return check;






}


uint8_t PADS_continuous_init(I2C_HandleTypeDef *i2c){

	uint8_t ctrl_2=0;
	uint8_t ctrl_1=0;
	int check=0;

	if(HAL_I2C_Mem_Read(i2c, 0xBB, 0x11, 1, &ctrl_2, 1, 100)!=HAL_OK){
			check=1;
		}

	ctrl_2|=0b00000010;

	if(HAL_I2C_Mem_Write(i2c, 0xBA, 0x11, 1, &ctrl_2, 1, 100)!=HAL_OK){
			check=1;
		}

	if(HAL_I2C_Mem_Read(i2c, 0xBB, 0x10, 1, &ctrl_1, 1, 100)!=HAL_OK){
				check=1;
			}


	ctrl_1|=0b01001110;

	ctrl_1&=0b11001111;

	if(HAL_I2C_Mem_Write(i2c, 0xBA, 0x10, 1, &ctrl_1, 1, 100)!=HAL_OK){
				check=1;
			}



	return check;

}

uint8_t PADS_continuous_read(I2C_HandleTypeDef *i2c){
		int check=0;
		uint8_t recarray[5];
		int16_t intbuffertemp=0;
		int32_t intbufferpres=0;
		double tmoy=0;
		if(HAL_I2C_Mem_Read(i2c, 0xBB, 0x28, 1, &recarray[0], 1, 100)!=HAL_OK){
			check=1;
		}
		if(HAL_I2C_Mem_Read(i2c, 0xBB, 0x29, 1, &recarray[1], 1, 100)!=HAL_OK){
			check=1;
		}
		if(HAL_I2C_Mem_Read(i2c, 0xBB, 0x2A, 1, &recarray[2], 1, 100)!=HAL_OK){
			check=1;
		}
		if(HAL_I2C_Mem_Read(i2c, 0xBB, 0x2B, 1, &recarray[3], 1, 100)!=HAL_OK){
			check=1;
		}
		if(HAL_I2C_Mem_Read(i2c, 0xBB, 0x2C, 1, &recarray[4], 1, 100)!=HAL_OK){
			check=1;
		}

		if(check==0){

		intbufferpres=(recarray[2]<<16)|(recarray[1]<<8)|(recarray[0]);
		intbuffertemp=(recarray[4]<<8)|(recarray[3]);
		finaltemp=(float)intbuffertemp*0.01;
		finalpress=(float) intbufferpres*(0.00002441406);

		//alt=(double)(288.15/0.0065)*(1-pow((double)(finalpress*1000.0)/101325.0, (double)(287.05*0.0065)/(9.80665)));
		alt=(float) ((8.314*293.15)/(9.80665*0.028964))*logf((float)101325.0/(finalpress*1000.0));
		tmoy=(float) 293.15+finaltemp+(0.0065*alt)/2;
		alt=(float) ((8.314*tmoy)/(9.80665*0.028964))*logf((float)101325.0/(finalpress*1000.0));











		}

		return check;








}




