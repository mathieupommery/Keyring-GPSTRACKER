/*
 * QMC5883.c
 *
 *  Created on: 11 May 2021
 *      Author: Serdar
 */
#include"QMC5883.h"
#include "math.h"
#include "ssd1306.h"

//code recuperer et fonctionnel, il utilise une fonction init, permettant d'initialiser le capteur correctement en ecrivant dans ces registres selon le datasheet,
//puis d'une fonction read, qui lit le capteur puis met à jour la structurede donnée

//###############################################################################################################
uint8_t QMC_init(QMC_t *qmc,I2C_HandleTypeDef *i2c,uint8_t Output_Data_Rate)
{
	uint8_t array[2];
	qmc->i2c=i2c;
	qmc->Control_Register=0x11;
	array[0]=1;
	array[1]=qmc->Control_Register;

	if(Output_Data_Rate==200)qmc->Control_Register|=0b00001100;
	else if(Output_Data_Rate==100)qmc->Control_Register|=0b00001000;
	else if(Output_Data_Rate==50)qmc->Control_Register|=0b00000100;
	else if(Output_Data_Rate==10)qmc->Control_Register|=0b00000000;
	else qmc->Control_Register|=0b00001100;

	if(HAL_I2C_Mem_Write(qmc->i2c, 0x1A, 0x0B, 1, &array[0], 1, 100)!=HAL_OK)return 1;
	if(HAL_I2C_Mem_Write(qmc->i2c, 0x1A, 0x09, 1, &array[1], 1, 100)!=HAL_OK)return 1;

	return 0;
}

uint8_t QMC_read(QMC_t *qmc)
{
	  qmc->datas[0]=0;
	  HAL_I2C_Mem_Read(qmc->i2c, 0x1A, 0x06, 1, qmc->datas, 1, 100);

	  if((qmc->datas[0]&0x01)==1)
	  {
		  HAL_I2C_Mem_Read(qmc->i2c, 0x1A, 0x00, 1, qmc->datas, 6, 100);
		  qmc->Xaxis= (qmc->datas[1]<<8) | qmc->datas[0];
		  qmc->Yaxis= (qmc->datas[3]<<8) | qmc->datas[2];
		  qmc->Zaxis= (qmc->datas[5]<<8) | qmc->datas[4];

		  qmc->compas=atan2f(qmc->Xaxis,qmc->Yaxis)*180.00/M_PI;//a voir possiblement a changer pas sur encore

		  if(qmc->compas>0)
		  {
			  qmc->heading= qmc->compas;
		  }
		  else
		  {
			  qmc->heading=360+qmc->compas;
		  }

	  }
	  else
	  {
		  return 1;
	  }
return 0;
}

float QMC_readHeading(QMC_t *qmc)
{
	QMC_read(qmc);
	return qmc->heading;
}

uint8_t QMC_Standby(QMC_t *qmc)
{
	uint8_t array[1]={0};
	if(HAL_I2C_Mem_Write(qmc->i2c, 0x1A, 0x09, 1, &array[0], 1, 100)!=HAL_OK)return 1;
	return 0;
}
uint8_t QMC_Reset(QMC_t *qmc)
{
	uint8_t array[1]={0x80};
	if(HAL_I2C_Mem_Write(qmc->i2c, 0x1A, 0x0A, 1, &array[0], 1, 100)!=HAL_OK)return 1;
	return 0;
}

void QMC_arrow(float angle,uint8_t x,uint8_t y){//ici on trace les fleche pour cela on appelle la fonction drawarrow, sont 3eme paramettre est le choix de la fleche que l'on veut entre 1 et 8, remarque un test de fleche avec sinus et cosinus a été fait bien qu'il ne fut pas extremement concluant,
//un traitement numérique du signal est à envisager pour le compas.
//	if(data->heading>=0 && data->heading<45){
//		ssd1306_Fill(Black);
//		ssd1306_Drawarrow(x,y, 2,White);
//	}
//
//	if(data->heading>=45 && data->heading<90){
//		ssd1306_Fill(Black);
//		ssd1306_Drawarrow(x,y, 8,White);
//
//	}
//
//	if(data->heading>=90 && data->heading<135){
//		ssd1306_Fill(Black);
//		ssd1306_Drawarrow(x,y, 4,White);
//
//	}
//
//	if(data->heading>=135 && data->heading<180){
//		ssd1306_Fill(Black);
//		ssd1306_Drawarrow(x,y, 5,White);
//
//	}
//
//	if(data->heading>=180 && data->heading<225){
//		ssd1306_Fill(Black);
//		ssd1306_Drawarrow(x,y, 1,White);
//
//	}
//
//
//	if(data->heading>=225 && data->heading<270){
//		ssd1306_Fill(Black);
//		ssd1306_Drawarrow(x,y, 6,White);
//
//	}
//
//	if(data->heading>=270 && data->heading<315){
//		ssd1306_Fill(Black);
//		ssd1306_Drawarrow(x,y, 3,White);
//
//	}
//	if(data->heading>=315 && data->heading<=360){
//		ssd1306_Fill(Black);
//		ssd1306_Drawarrow(x,y, 7,White);
//
//	}
	ssd1306_Fill(Black);
	ssd1306_Drawarrow2(x,y, 5,angle);
}
