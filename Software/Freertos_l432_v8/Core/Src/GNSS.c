#include "GNSS.h"
#include <stdio.h>
#include "math.h"

union u_Short uShort;
union i_Short iShort;
union u_Long uLong;
union i_Long iLong;

/*!
 * Structure initialization.
 * @param GNSS Pointer to main GNSS structure.
 * @param huart Pointer to uart handle.
 */
void GNSS_Init(GNSS_StateHandle *GNSS, UART_HandleTypeDef *huart) {
	GNSS->huart = huart;
	GNSS->year = 0;
	GNSS->month = 0;
	GNSS->day = 0;
	GNSS->hour = 0;
	GNSS->min = 0;
	GNSS->sec = 0;
	GNSS->fixType = 0;
	GNSS->numSV = 0;
	GNSS->satCount = 0;
	GNSS->lon = 0;
	GNSS->lat = 0;
	GNSS->height = 0;
	GNSS->hMSL = 0;
	GNSS->hAcc = 0;
	GNSS->vAcc = 0;
	GNSS->gSpeed = 0;
	GNSS->headMot = 0;
	GNSS->write_index=0;
	GNSS->read_index=0;
	HAL_Delay(100);
}

void GNSS_ParsePVTData(GNSS_StateHandle *GNSS) {
	uShort.bytes[0] = GNSS->uartWorkingBuffer[10];
	GNSS->yearBytes[0]=GNSS->uartWorkingBuffer[10];
	uShort.bytes[1] = GNSS->uartWorkingBuffer[11];
	GNSS->yearBytes[1]=GNSS->uartWorkingBuffer[11];
	GNSS->year = uShort.uShort;
	GNSS->month = GNSS->uartWorkingBuffer[12];
	GNSS->day = GNSS->uartWorkingBuffer[13];
	GNSS->hour = GNSS->uartWorkingBuffer[14];
	GNSS->min = GNSS->uartWorkingBuffer[15];
	GNSS->sec = GNSS->uartWorkingBuffer[16];
	GNSS->fixType = GNSS->uartWorkingBuffer[26];
	GNSS->numSV = GNSS->uartWorkingBuffer[29];

	for (int var = 0; var < 4; ++var) {
		iLong.bytes[var] = GNSS->uartWorkingBuffer[var + 30];
		GNSS->lonBytes[var]= GNSS->uartWorkingBuffer[var + 30];
	}
	GNSS->lon = iLong.iLong;
	GNSS->fLon=(float)iLong.iLong/10000000.0;

	for (int var = 0; var < 4; ++var) {
		iLong.bytes[var] = GNSS->uartWorkingBuffer[var + 34];
		GNSS->latBytes[var]=GNSS->uartWorkingBuffer[var + 34];
	}
	GNSS->lat = iLong.iLong;
	GNSS->fLat=(float)iLong.iLong/10000000.0;

	for (int var = 0; var < 4; ++var) {
		iLong.bytes[var] = GNSS->uartWorkingBuffer[var + 38];
	}
	GNSS->height = iLong.iLong;
	GNSS->fheight=(float)GNSS->height/1000.0;


	for (int var = 0; var < 4; ++var) {
		iLong.bytes[var] = GNSS->uartWorkingBuffer[var + 42];
		GNSS->hMSLBytes[var] = GNSS->uartWorkingBuffer[var + 42];
	}
	GNSS->hMSL = iLong.iLong;
	GNSS->fhMSL=(float)GNSS->hMSL/1000.0;

	for (int var = 0; var < 4; ++var) {
		uLong.bytes[var] = GNSS->uartWorkingBuffer[var + 46];
	}
	GNSS->hAcc = uLong.uLong;
	GNSS->fhACC=(float)GNSS->hAcc/1000.0;

	for (int var = 0; var < 4; ++var) {
		uLong.bytes[var] = GNSS->uartWorkingBuffer[var + 50];
	}
	GNSS->vAcc = uLong.uLong;
	GNSS->fvACC=(float)GNSS->vAcc/1000.0;

	for (int var = 0; var < 4; ++var) {
		iLong.bytes[var] = GNSS->uartWorkingBuffer[var + 66];
		GNSS->gSpeedBytes[var] = GNSS->uartWorkingBuffer[var + 66];
	}
	GNSS->gSpeed = iLong.iLong;
	GNSS->fgSpeed=(float)GNSS->gSpeed/1000.0;

	for (int var = 0; var < 4; ++var) {
		iLong.bytes[var] = GNSS->uartWorkingBuffer[var + 70];
	}
	GNSS->headMot = iLong.iLong * 1e-5;
}

HAL_StatusTypeDef GNSS_Process(GNSS_StateHandle * GNSS){

	HAL_StatusTypeDef result=HAL_BUSY;
	uint16_t index=GNSS->read_index;
	uint16_t available=0;
	uint8_t state=0;

	if (GNSS->write_index >= GNSS->read_index) {
		available = GNSS->write_index - GNSS->read_index;
	}
	else {
		available = (512 - GNSS->read_index) + GNSS->write_index;
	}



	while(available>=2){

		index=GNSS->read_index;

		if (GNSS->write_index >= GNSS->read_index) {
			available = GNSS->write_index - GNSS->read_index;
		}
		else {
			available = (512 - GNSS->read_index) + GNSS->write_index;
		}

	    if (available < 2)
	    	{
	    	return HAL_BUSY;
	    	}

			if (GNSS->circular_buffer[index] == 0xB5 && GNSS->circular_buffer[(index+1)%512] == 0x62) {

				if (GNSS->circular_buffer[(index+2)%512] == 0x01 && GNSS->circular_buffer[(index+3)%512] == 0x07) {
					if(available >=100){
					for(int i = 0; i<100;i++){
						GNSS->uartWorkingBuffer[i]=GNSS->circular_buffer[(index+i)%512];
					}
					GNSS_ParsePVTData(GNSS);
					HAL_GPIO_TogglePin(LED_BLUE_GPIO_Port,LED_BLUE_Pin);
					GNSS->read_index= (GNSS->read_index + 90)%512;
					return HAL_OK;
					}
					else{
						return HAL_BUSY;
					}

					}
			}
			GNSS->read_index= (GNSS->read_index + 1)%512;

}
	return result;
}



double distancecalc(double lat1, double lat2, double long1, double long2){
	double distance=0;
	distance=(double) 6371000*acosl(fmin(1,sinl(lat1*(M_PI/180))*sinl(lat2*(M_PI/180))+cosl(lat1*(M_PI/180))*cosl(lat2*(M_PI/180))*cosl((long2-long1)*(M_PI/180))));

	return distance;
}
