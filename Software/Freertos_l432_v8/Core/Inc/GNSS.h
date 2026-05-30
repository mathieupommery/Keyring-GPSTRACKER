#ifndef INC_GNSS_H_
#define INC_GNSS_H_

#include "main.h"

union u_Short
{
	uint8_t bytes[2];
	unsigned short uShort;
};

union i_Short
{
	uint8_t bytes[2];
	signed short iShort;
};

union u_Long
{
	uint8_t bytes[4];
	unsigned long uLong;
};

union i_Long
{
	uint8_t bytes[4];
	signed long iLong;
};

typedef struct
{
	UART_HandleTypeDef *huart;

	uint8_t uniqueID[4];
	uint8_t circular_buffer[512];
	uint8_t uartWorkingBuffer[128];
	uint8_t uartTXbuf[128];
	uint16_t write_index;
	uint16_t read_index;
	unsigned short year;
	uint8_t yearBytes[2];
	uint8_t month;
	uint8_t day;
	uint8_t hour;
	uint8_t min;
	uint8_t sec;
	uint8_t fixType;//2==2d fix and 3==3dfix 0==no fix and 1==deadreckoning
	uint8_t numSV;//sat number used in nav solution
	uint8_t satCount;

	signed long lon;
	uint8_t lonBytes[4];
	signed long lat;
	uint8_t latBytes[4];
	float fLon;
	float fLat;

	signed long height;
	float fheight;


	signed long hMSL;
	uint8_t hMSLBytes[4];
	float fhMSL;

	unsigned long hAcc;
	float fhACC;
	unsigned long vAcc;
	float fvACC;

	signed long gSpeed;
	uint8_t gSpeedBytes[4];
	float fgSpeed;

	signed long headMot;

	float distance_parcouru;

}GNSS_StateHandle;


void GNSS_Init(GNSS_StateHandle *GNSS, UART_HandleTypeDef *huart);
void GNSS_ParsePVTData(GNSS_StateHandle *GNSS);
HAL_StatusTypeDef GNSS_Process(GNSS_StateHandle * GNSS);
double distancecalc(double lat1, double lat2, double long1, double long2);
void GPS_Set_LowPower_10s(GNSS_StateHandle *gnss);
void GPS_Set_Normal_Mode(GNSS_StateHandle *gnss);
void GPS_Verify_Config(GNSS_StateHandle *gnss);
#endif /* INC_GNSS_H_ */



