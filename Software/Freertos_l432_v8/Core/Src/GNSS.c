#include "GNSS.h"
#include <stdio.h>
#include "math.h"

union u_Short uShort;
union i_Short iShort;
union u_Long uLong;
union i_Long iLong;

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


#include "usart.h"

static void UBX_Send_Message(GNSS_StateHandle *gnss, uint8_t msgClass, uint8_t msgID, uint8_t *payload, uint16_t payloadLen) {

    uint16_t totalLen = 6 + payloadLen + 2;
    if (totalLen > sizeof(gnss->uartTXbuf)) {
        return;
    }
    gnss->uartTXbuf[0] = 0xB5;
    gnss->uartTXbuf[1] = 0x62;
    gnss->uartTXbuf[2] = msgClass;
    gnss->uartTXbuf[3] = msgID;
    gnss->uartTXbuf[4] = payloadLen & 0xFF;
    gnss->uartTXbuf[5] = (payloadLen >> 8) & 0xFF;

    for (uint16_t i = 0; i < payloadLen; i++) {
        gnss->uartTXbuf[6 + i] = payload[i];
    }

    uint8_t CK_A = 0, CK_B = 0;

    for (uint16_t i = 2; i < 6 + payloadLen; i++) {
        CK_A += gnss->uartTXbuf[i];
        CK_B += CK_A;
    }

    gnss->uartTXbuf[6 + payloadLen] = CK_A;
    gnss->uartTXbuf[6 + payloadLen + 1] = CK_B;
    HAL_UART_Transmit(gnss->huart, gnss->uartTXbuf, totalLen, 100);
}

/* ------------------------------------------------------------------------- */
/* FONCTION : Envoyer un UBX-CFG-VALSET                                      */
/* ------------------------------------------------------------------------- */
static void GPS_Send_VALSET(GNSS_StateHandle *gnss, uint8_t layer, uint8_t *cfgData, uint16_t cfgDataLen) {
    uint16_t totalLen = 4 + cfgDataLen;
    uint8_t payload[totalLen];

    payload[0] = 0x00;    // Version (0)
    payload[1] = layer;   // Layer (1=RAM)
    payload[2] = 0x00;    // Reserved
    payload[3] = 0x00;    // Reserved

    for (uint16_t i = 0; i < cfgDataLen; i++) {
        payload[4 + i] = cfgData[i];
    }

    UBX_Send_Message(gnss, 0x06, 0x8A, payload, totalLen);
}

/* ------------------------------------------------------------------------- */
/* FONCTION : Envoyer un UBX-CFG-VALGET                                      */
/* ------------------------------------------------------------------------- */
static void GPS_Send_VALGET(GNSS_StateHandle *gnss, uint8_t layer, uint32_t *keys, uint8_t numKeys) {
    uint16_t totalLen = 4 + (numKeys * 4);
    uint8_t payload[totalLen];

    payload[0] = 0x00;    // Version (0)
    payload[1] = layer;   // Layer (0 = RAM)
    payload[2] = 0x00;    // Position (Low byte)
    payload[3] = 0x00;    // Position (High byte)

    for (uint8_t i = 0; i < numKeys; i++) {
        payload[4 + (i * 4) + 0] = (keys[i] & 0xFF);
        payload[4 + (i * 4) + 1] = ((keys[i] >> 8) & 0xFF);
        payload[4 + (i * 4) + 2] = ((keys[i] >> 16) & 0xFF);
        payload[4 + (i * 4) + 3] = ((keys[i] >> 24) & 0xFF);
    }

    UBX_Send_Message(gnss, 0x06, 0x8B, payload, totalLen);
}

void GPS_Set_LowPower_10s(GNSS_StateHandle *gnss) {
    uint8_t payload[] = {
        // Clé 1 : CFG-PM-POSUPDATEPERIOD (0x40D00002) - 10s
        0x02, 0x00, 0xD0, 0x40,
        0x0A, 0x00, 0x00, 0x00,

        // Clé 2 : CFG-PM-MAXACQTIME (0x20D00007) - Timeout 15s
        0x07, 0x00, 0xD0, 0x20,
        0x0F,

        // Clé 3 : CFG-PM-ONTIME (0x30D00005) - 0s
        0x05, 0x00, 0xD0, 0x30,
        0x00, 0x00,

        // Clé 4 : CFG-PM-OPERATEMODE (0x20D00001) - PSMOO (1)
        0x01, 0x00, 0xD0, 0x20,
        0x01
    };

    // On envoie au layer RAM (0x01)
    GPS_Send_VALSET(gnss, 0x01, payload, sizeof(payload));
}

void GPS_Set_Normal_Mode(GNSS_StateHandle *gnss) {
    uint8_t payload[] = {
        // CFG-PM-OPERATEMODE (0x20D00001) -> Valeur 0 (Continu)
        0x01, 0x00, 0xD0, 0x20,
        0x00
    };

    GPS_Send_VALSET(gnss, 0x01, payload, sizeof(payload));
}

/* ------------------------------------------------------------------------- */
/* VÉRIFIER LES PARAMÈTRES COURANTS                                          */
/* ------------------------------------------------------------------------- */
void GPS_Verify_Config(GNSS_StateHandle *gnss) {
    uint32_t keys_to_check[] = {
        0x40D00002, // POSUPDATEPERIOD
        0x20D00001  // OPERATEMODE
    };
    // On demande à lire les valeurs actuelles en RAM (Layer 0 pour VALGET)
    GPS_Send_VALGET(gnss, 0, keys_to_check, 2);
}



double distancecalc(double lat1, double lat2, double long1, double long2){
	double distance=0;
	distance=(double) 6371000*acosl(fmin(1,sinl(lat1*(M_PI/180))*sinl(lat2*(M_PI/180))+cosl(lat1*(M_PI/180))*cosl(lat2*(M_PI/180))*cosl((long2-long1)*(M_PI/180))));

	return distance;
}
