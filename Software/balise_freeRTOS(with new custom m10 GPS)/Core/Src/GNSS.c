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

	HAL_UART_Transmit_DMA(&huart,uart1outprotnmeadisable,sizeof(uart1outprotnmeadisable)/(sizeof(uint8_t)));
	  HAL_Delay(30);
	  HAL_UART_Transmit_DMA(&huart,uart1outprotubxenable,sizeof(uart1outprotubxenable)/(sizeof(uint8_t)));
	  HAL_Delay(30);
	  HAL_UART_Transmit_DMA(&huart,setGPS_GAL_GLONASS,sizeof(setGPS_GAL_BEIDOU_GLONASS)/(sizeof(uint8_t)));
	  HAL_Delay(30);
	  HAL_UART_Transmit_DMA(&huart,meas_rate_5hz,sizeof(meas_rate_10hz)/(sizeof(uint8_t)));
	  HAL_Delay(30);
	  HAL_UART_Transmit_DMA(&huart,ubx_pvt_every_1meas,sizeof(ubx_pvt_every_1meas)/(sizeof(uint8_t)));

	HAL_Delay(100);
}

/*!
 * Searching for a header in data buffer and matching class and message ID to buffer data.
 * @param GNSS Pointer to main GNSS structure.
 */
void GNSS_ParseBuffer(GNSS_StateHandle *GNSS) {
	uint8_t var=0;
		if (GNSS->uartWorkingBuffer[var] == 0xB5
			&& GNSS->uartWorkingBuffer[var + 1] == 0x62) {

				if (GNSS->uartWorkingBuffer[var + 2] == 0x01
						&& GNSS->uartWorkingBuffer[var + 3] == 0x07) { //Look at: 32.17.17.1 u-blox 8 Receiver description
					GNSS_ParsePVTData(GNSS);

				}
		}
}

/*!
 * Make request for unique chip ID data.
 * @param GNSS Pointer to main GNSS structure.
 */
void GNSS_GetUniqID(GNSS_StateHandle *GNSS) {
	HAL_UART_Transmit_DMA(GNSS->huart, getDeviceID,
			sizeof(getDeviceID) / sizeof(uint8_t));
	HAL_UART_Receive_IT(GNSS->huart, GNSS->uartWorkingBuffer, 17);
}

/*!
 * Make request for UTC time solution data.
 * @param GNSS Pointer to main GNSS structure.
 */
void GNSS_GetNavigatorData(GNSS_StateHandle *GNSS) {
	HAL_UART_Transmit_DMA(GNSS->huart, getNavigatorData,
			sizeof(getNavigatorData) / sizeof(uint8_t));
	HAL_UART_Receive_IT(GNSS->huart, GNSS->uartWorkingBuffer, 28);
}

/*!
 * Make request for geodetic position solution data.
 * @param GNSS Pointer to main GNSS structure.
 */
void GNSS_GetPOSLLHData(GNSS_StateHandle *GNSS) {
	HAL_UART_Transmit_DMA(GNSS->huart, getPOSLLHData,
			sizeof(getPOSLLHData) / sizeof(uint8_t));
	HAL_UART_Receive_IT(GNSS->huart, GNSS->uartWorkingBuffer, 36);
}

/*!
 * Make request for navigation position velocity time solution data.
 * @param GNSS Pointer to main GNSS structure.
 */
void GNSS_GetPVTData(GNSS_StateHandle *GNSS) {
	HAL_UART_Transmit_DMA(GNSS->huart, getPVTData,
			sizeof(getPVTData) / sizeof(uint8_t));
	HAL_UART_Receive_IT(GNSS->huart, GNSS->uartWorkingBuffer, 100);
}

/*!
 * Make request for satellite information data.
 * @param GNSS Pointer to main GNSS structure.
 */
void GNSS_GetNAVSATData(GNSS_StateHandle *GNSS) {
	HAL_UART_Transmit_DMA(GNSS->huart, getNAVSATData,
			sizeof(getNAVSATData) / sizeof(uint8_t));
	HAL_UART_Receive_IT(GNSS->huart, GNSS->uartWorkingBuffer, 28);
}

/*!
 * Parse data to unique chip ID standard.
 * Look at: 32.19.1.1 u-blox 8 Receiver description
 * @param GNSS Pointer to main GNSS structure.
 */
void GNSS_ParseUniqID(GNSS_StateHandle *GNSS) {
	for (int var = 0; var < 5; ++var) {
		GNSS->uniqueID[var] = GNSS->uartWorkingBuffer[10 + var];
	}
}

/*!
 * Changing the GNSS mode.
 * Look at: 32.10.19 u-blox 8 Receiver description
 */
void GNSS_SetMode(GNSS_StateHandle *GNSS, short gnssMode) {
	if (gnssMode == 0) {
		HAL_UART_Transmit_DMA(GNSS->huart, setPortableMode,sizeof(setPortableMode) / sizeof(uint8_t));
	} else if (gnssMode == 1) {
		HAL_UART_Transmit_DMA(GNSS->huart, setStationaryMode,sizeof(setStationaryMode) / sizeof(uint8_t));
	} else if (gnssMode == 2) {
		HAL_UART_Transmit_DMA(GNSS->huart, setPedestrianMode,sizeof(setPedestrianMode) / sizeof(uint8_t));
	} else if (gnssMode == 3) {
		HAL_UART_Transmit_DMA(GNSS->huart, setAutomotiveMode,sizeof(setAutomotiveMode) / sizeof(uint8_t));
	} else if (gnssMode == 4) {
		HAL_UART_Transmit_DMA(GNSS->huart, setAutomotiveMode,sizeof(setAutomotiveMode) / sizeof(uint8_t));
	} else if (gnssMode == 5) {
		HAL_UART_Transmit_DMA(GNSS->huart, setAirbone1GMode,sizeof(setAirbone1GMode) / sizeof(uint8_t));
	} else if (gnssMode == 6) {
		HAL_UART_Transmit_DMA(GNSS->huart, setAirbone2GMode,sizeof(setAirbone2GMode) / sizeof(uint8_t));
	} else if (gnssMode == 7) {
		HAL_UART_Transmit_DMA(GNSS->huart, setAirbone4GMode,sizeof(setAirbone4GMode) / sizeof(uint8_t));
	} else if (gnssMode == 8) {
		HAL_UART_Transmit_DMA(GNSS->huart, setWirstMode,sizeof(setWirstMode) / sizeof(uint8_t));
	} else if (gnssMode == 9) {
		HAL_UART_Transmit_DMA(GNSS->huart, setBikeMode,sizeof(setBikeMode) / sizeof(uint8_t));
	}
}
/*!
 * Parse data to navigation position velocity time solution standard.
 * Look at: 32.17.15.1 u-blox 8 Receiver description.
 * @param GNSS Pointer to main GNSS structure.
 */
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

/*!
 * Parse data to UTC time solution standard.
 * Look at: 32.17.30.1 u-blox 8 Receiver description.
 * @param GNSS Pointer to main GNSS structure.
 */


void GNSS_ParseNAVSATData(GNSS_StateHandle *GNSS) {
	GNSS->satCount = GNSS->uartWorkingBuffer[11];
}



/*!
 *  Sends the basic configuration: Activation of the UBX standard, change Baudrate to 115200 and activate all satellite systems
 * @param GNSS Pointer to main GNSS structure.
 */
void GNSS_LoadConfig(GNSS_StateHandle *GNSS) {




}


/*!
 *  Creates a checksum based on UBX standard.
 * @param class Class value from UBX doc.
 * @param messageID MessageID value from UBX doc.
 * @param dataLength Data length value from UBX doc.
 * @param payload Just payload.
 * @return  Returns checksum.
 */
uint8_t GNSS_Checksum(uint8_t class, uint8_t messageID, uint8_t dataLength,uint8_t *payload) {
//todo: If you want to build your own UBX message, then you have to
//		implement UBX checksum algorith, as stated in integration
//		manual.
	return 0;
}




double distancecalc(double lat1, double lat2, double long1, double long2){
	double distance=0;
	distance=(double) 6371000*acosl(fmin(1,sinl(lat1*(M_PI/180))*sinl(lat2*(M_PI/180))+cosl(lat1*(M_PI/180))*cosl(lat2*(M_PI/180))*cosl((long2-long1)*(M_PI/180))));

	return distance;
}
