#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <HardwareSerial.h>
#include "ESP32_WS2812_Lib.h"
#include <Wire.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "SSD1306.h"

// --- OLED ---


#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

#define GPS_RX_PIN 17
#define GPS_TX_PIN 16
#define GPS_BAUD   115200
#define BUTTON_PIN 21

#define MY_SDA 22
#define MY_SCL 23
#define LED_PIN 19

#define FLASH_CS 18

#define GPIO_SUP_2 15
#define GPIO_SUP_1 14

#define FLASH_MOSI 7
#define FLASH_MISO 2
#define FLASH_HOLD 4
#define FLASH_WP 5

#define KILL_PIN 3
#define ADC_PIN 0
#define INT_POWER_PIN 1


#define BTN_A_PIN 20
#define BTN_B_PIN 21

#define SHORT_PRESS_MIN 50     // ms
#define LONG_PRESS_TIME 400    // ms


const float ADC_VREF = 3.3;    // Tension de référence ADC
const int ADC_RESOLUTION = 4095; // Résolution 12 bits

float vbat = 0.0;
float temp = 0.0;

// Flags d'interruption
volatile bool btnA_irq = false;
volatile bool btnB_irq = false;

// Temps de début d'appui
volatile unsigned long btnA_press_time = 0;
volatile unsigned long btnB_press_time = 0;

// Résultat interprété (exploitable dans ton programme principal)
bool BTN_A = false;
bool BTN_A_LONG = false;
bool BTN_B = false;
bool BTN_B_LONG = false;

#define LEDS_COUNT 1
#define CHANNEL    0
ESP32_WS2812 strip = ESP32_WS2812(LEDS_COUNT, LED_PIN, CHANNEL, TYPE_GRB);


SSD1306 display(0x3C);
HardwareSerial GPS_Serial(0);  // UART2 pour GPS

// === VARIABLES SHARED ENTRE TÂCHES ===
volatile uint8_t gps_numSV = 0;
volatile float gps_speed_kmh = 0.0;


TaskHandle_t TaskUSBHandle = NULL;
TaskHandle_t TaskHandle_BLE_Ping = NULL;
TaskHandle_t TaskHandle_BLE_Send = NULL;


uint8_t flag=0;

// === BUFFER GPS ===
#define UBX_BUFFER_SIZE 100
uint8_t ubx_buffer[UBX_BUFFER_SIZE];

#define UBX_MSG_SIZE 100

uint8_t ubx_trame_buffer[UBX_MSG_SIZE];
volatile bool ubx_trame_ready = false;
uint8_t bufferscreen[100];



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

i_Long iLong;
u_Long uLong;
i_Short iShot;
u_Short uShort;

typedef struct
{
	uint8_t uniqueID[4];
	uint8_t uartWorkingBuffer[101];

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

}GNSS_StateHandle;


GNSS_StateHandle GNSSData;





typedef enum{
	STATE_SPEED,
	STATE_BALISE,
	STATE_POS,
	STATE_USB,
	STATE_BLUETOOTHPING,
	STATE_BLUETOOTHSEND,

}STATE_TYPE;

typedef enum{
	STATE_GROS,
	STATE_GROS1,
	STATE_SUMMARY


}SPEED;

typedef enum{
	STATE_SUMMARY1,
	STATE_INFO,
	STATE_INFO2,
	STATE_ALT,
	STATE_ALTBARO,
	STATE_HEURE

}POS;

typedef enum{
	BALISESTATE1,
	BALISESTATE2,
	BALISESTATE3

}BALISESTATE;


typedef enum{
	ECRANBALISESTATE1,
	ECRANBALISESTATE2,
	ECRANBALISESTATE3,
	ECRANBALISESTATE4,
	ECRANBALISESTATE5,
	ECRANBALISESTATE6

}ECRANBALISESTATE;


STATE_TYPE state=STATE_SPEED;
SPEED spdstate=STATE_GROS;
POS posstate=STATE_SUMMARY1;
BALISESTATE balisestate=BALISESTATE1;
ECRANBALISESTATE ecranstate=ECRANBALISESTATE1;

float vitmax=0.0;

int doubledonnee=0;
int cptdoubledonnee=0;
double distanceparcouru=0;
double oldlat=0;
double oldlong=0;

int pagenumber=0;
int pageoffset=0;

int FLAG_USB_STATE=0;
int FLAG_BL_PING_STATE=0;
int FLAG_BL_SEND_STATE=0;


const unsigned char startimg[] = {
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe,
0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x3e,
0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf9, 0xce,
0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfa, 0x2e,
0x7f, 0xfc, 0x1f, 0xff, 0xff, 0xff, 0xf6, 0xb6,
0x7f, 0xf0, 0x07, 0xff, 0xff, 0xff, 0xf6, 0x76,
0x7f, 0xc0, 0x01, 0xff, 0xff, 0xff, 0xfa, 0xae,
0x7f, 0x80, 0x00, 0xff, 0xff, 0xff, 0xf9, 0xce,
0x7f, 0x81, 0xc0, 0xff, 0xff, 0xff, 0xfe, 0x3e,
0x7f, 0x03, 0xe0, 0x7f, 0xff, 0xff, 0xff, 0xfe,
0x7f, 0x07, 0xf0, 0x7f, 0x0f, 0x03, 0xf0, 0x3e,
0x7f, 0x07, 0xf0, 0x7f, 0x0f, 0x03, 0xf0, 0x3e,
0x7f, 0x07, 0xf0, 0x7c, 0xff, 0x3c, 0xcf, 0xfe,
0x7f, 0x03, 0xe0, 0x7c, 0xff, 0x3c, 0xcf, 0xfe,
0x7f, 0x01, 0xc0, 0x7c, 0xc3, 0x03, 0xf0, 0xfe,
0x7f, 0x80, 0x00, 0xfc, 0xc3, 0x03, 0xf0, 0xfe,
0x7f, 0x80, 0x00, 0xfc, 0xf3, 0x3f, 0xff, 0x3e,
0x7f, 0xc0, 0x01, 0xfc, 0xf3, 0x3f, 0xff, 0x3e,
0x7f, 0xc0, 0x01, 0xff, 0x0f, 0x3f, 0xc0, 0xfe,
0x7f, 0xe0, 0x03, 0xff, 0x0f, 0x3f, 0xc0, 0xfe,
0x7f, 0xe0, 0x07, 0xff, 0xff, 0xff, 0xff, 0xfe,
0x7f, 0xf0, 0x07, 0xff, 0xff, 0xff, 0xff, 0xfe,
0x7f, 0xf0, 0x0f, 0xff, 0xff, 0x76, 0xfe, 0x7e,
0x7f, 0xf8, 0x0f, 0xff, 0xff, 0x74, 0xfd, 0xbe,
0x7f, 0xf8, 0x1f, 0xff, 0xff, 0xae, 0xfd, 0xbe,
0x7f, 0xfc, 0x1f, 0xff, 0xff, 0xae, 0xfd, 0xbe,
0x7f, 0xfc, 0x3f, 0xff, 0xff, 0xdc, 0x6e, 0x7e,
0x7f, 0xfe, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xfe,
0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe,
0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};


void statemachine(void){
	switch(state){
	 case STATE_SPEED:
{
		display.Fill(Black);

				 if(GNSSData.fgSpeed>=vitmax){
									 vitmax=GNSSData.fgSpeed;
								 }
								 float pace=0;
								 float sec=0;
								 if (GNSSData.fgSpeed!=0){
									 pace=1000/(60*GNSSData.fgSpeed);
									 sec=(pace-floor(pace))*60;
								 }
								 else {
									 pace=99;
								 }


				  switch(spdstate){



				 case STATE_GROS:


						display.SetCursor(32, 32);
						snprintf((char *)bufferscreen,15, "%0.1f",(GNSSData.fgSpeed)*3.6);
						display.WriteString((char *)bufferscreen, Font_16x24, White);
						display.SetCursor(32, 56);
						display.WriteString("Vit(kmh)", Font_6x8, White);
						display.BatteryGauge(vbat,79, 57,1);

					 if(BTN_B>=1){
							spdstate=STATE_GROS1;
							BTN_B=0;


					 					 				  	}




				  break;
				 case STATE_GROS1:

					 display.SetCursor(32, 32);
					 snprintf((char *)bufferscreen,15, "%0.1f",vitmax*3.6);
					 display.WriteString((char *)bufferscreen, Font_16x24, White);
					 display.SetCursor(32, 56);
					 display.WriteString("maxV", Font_6x8, White);
					 display.BatteryGauge(vbat,79, 57,1);





					 if(BTN_B>=1){
					 							spdstate=STATE_SUMMARY;
					 							BTN_B=0;


					 					 					 				  	}



					 break;
				  case STATE_SUMMARY:
				  						display.SetCursor(32, 32);
				  						snprintf((char *)bufferscreen,15, "%0.0fmin%0.0fs",floor(pace),floor(sec));
				  						display.WriteString((char *)bufferscreen, Font_7x10, White);
				  						display.SetCursor(32, 42);
				  						display.WriteString("pace", Font_6x8, White);
				  						display.SetCursor(32, 56);
				  						snprintf((char *)bufferscreen,15, "V=%0.1fkmh",vitmax*3.6);
				  						display.WriteString((char *)bufferscreen, Font_6x8, White);
				  						display.BatteryGauge(vbat,79, 43,1);



				  					 if(BTN_B>=1){
				  								spdstate=STATE_GROS;
				  								BTN_B=0;
				  					 					 				  	}




				  					 break;
				  }
				  					if(BTN_A>=1){
				  									 					state=STATE_BALISE;
				  									 					BTN_A=0;
				  									 					BTN_B=0;
				  									  	}
				  									 	 if(BTN_B_LONG>=1){
				  									 					  							  vitmax=0;
				  									 					  							BTN_B_LONG=0;
				  									 					  						}
				  									 	 break;

}
				  case STATE_BALISE:
          {
					  display.Fill(Black);

					  switch(balisestate){
					  case BALISESTATE1:
						  display.SetCursor(32,32);
						  display.WriteString("Tracker",Font_6x8,White);
						  snprintf((char *)bufferscreen,50, "offset=%d",pageoffset);
						  display.SetCursor(32,42);
						  display.WriteString((char *)bufferscreen,Font_7x10,White);
						  snprintf((char *)bufferscreen,50, "page=%d",pagenumber);
						  display.SetCursor(32,52);
						  display.WriteString((char *)bufferscreen,Font_7x10,White);

						  if(BTN_B_LONG>=1){
							balisestate=BALISESTATE2;
							BTN_B_LONG=0;
							BTN_A=0;
						  }
						  if(BTN_A>=1){
						  				  			 	state=STATE_POS;
						  				  			 	BTN_A=0;
						  				  			 	BTN_B=0;


						  				  	}
						  if(BTN_A_LONG>=1){
									  state=STATE_SPEED;
									BTN_A_LONG=0;
									BTN_B=0;
						  					}

						  break;
					  case BALISESTATE2:
						  display.SetCursor(32,32);

						  switch(ecranstate){


						  case ECRANBALISESTATE1:
							  snprintf((char *)bufferscreen,50,"%d",pagenumber);
							  display.WriteString((char *)bufferscreen,Font_7x10,White);
							  display.SetCursor(32,42);
							  display.WriteString("PageNb",Font_6x8,White);


							  if(BTN_B>=1){
														  ecranstate=ECRANBALISESTATE2;
													  	BTN_B=0;
													  	BTN_A=0;
													  	}




							  break;
						  case ECRANBALISESTATE2:

							  snprintf((char  *)bufferscreen,50,"%0.3lf",distanceparcouru/1000);
							  display.WriteString((char *)bufferscreen,Font_7x10,White);
							  display.SetCursor(32,42);
							  display.WriteString("Dist(km)",Font_6x8,White);


							  if(BTN_B>=1){
							  														  ecranstate=ECRANBALISESTATE3;
							  													  	BTN_B=0;
							  													  	BTN_A=0;
							  													  	}
							  break;
						  case ECRANBALISESTATE3:
							  snprintf((char  *)bufferscreen,50,"%0.1f",GNSSData.fgSpeed*3.6);
							  display.WriteString((char *)bufferscreen,Font_7x10,White);
							  display.SetCursor(32,42);
							  display.WriteString("Vit(kmh)",Font_6x8,White);


							  if(BTN_B>=1){
							  														  ecranstate=ECRANBALISESTATE4;
							  													  	BTN_B=0;
							  													  	BTN_A=0;
							  													  	}
							  break;
						  case ECRANBALISESTATE4:
							  snprintf((char  *)bufferscreen,50,"%0.1lf",0.0);
							  							  display.WriteString((char *)bufferscreen,Font_7x10,White);
							  							display.SetCursor(32,42);
							  							display.WriteString("alt(m)",Font_6x8,White);


							  if(BTN_B>=1){


							  														  ecranstate=ECRANBALISESTATE5;
							  													  	BTN_B=0;
							  													  	BTN_A=0;
							  													  	}
							  break;
						  case ECRANBALISESTATE5:
							  snprintf((char  *)bufferscreen,50,"%0.1f",0.0);
							  display.WriteString((char *)bufferscreen,Font_7x10,White);
							  display.SetCursor(32,42);
							  display.WriteString("Vbat(V)",Font_6x8,White);


							  if(BTN_B>=1){


							  														  ecranstate=ECRANBALISESTATE6;
							  													  	BTN_B=0;
							  													  	BTN_A=0;
							  													  	}
							  break;
						  case ECRANBALISESTATE6:
							  snprintf((char  *)bufferscreen,50,"%0.1f",vitmax*3.6);
							  							  display.WriteString((char *)bufferscreen,Font_7x10,White);
							  							display.SetCursor(32,42);
							  							display.WriteString("MaxV",Font_6x8,White);


							  if(BTN_B>=1){
							  														ecranstate=ECRANBALISESTATE1;
							  													  	BTN_B=0;
							  													  	BTN_A=0;
							  													  	}
							  break;
						  }


						 	display.BatteryGauge(vbat,79, 42,1);
						 	display.SetCursor(32,52);
						 	snprintf((char  *)bufferscreen,50, "%0.2fV",vbat);
						 	display.WriteString((char *)bufferscreen,Font_6x8,White);
						 	display.SetCursor(65,52);
						 	snprintf((char  *)bufferscreen,50, "sat=%d",GNSSData.numSV);
						 	display.WriteString((char *)bufferscreen,Font_6x8,White);

						  if(BTN_B_LONG>=1){
						  						  balisestate=BALISESTATE3;
						  						  BTN_B_LONG=0;
						  						  BTN_A=0;
						  					  }


						  break;

					  case BALISESTATE3:
						  display.SetCursor(32,32);
						  display.WriteString("fin de",Font_6x8,White);
						  display.SetCursor(32,42);
						  display.WriteString("memoire",Font_6x8,White);
						  display.SetCursor(32,52);
						  snprintf((char *)bufferscreen,50,"%d,%d",pageoffset,pagenumber);
						  display.WriteString((char *)bufferscreen,Font_6x8,White);
						  if(BTN_A>=1){
						  				  			 	state=STATE_POS;
						  				  			 	BTN_A=0;
						  				  			 	BTN_B=0;


						  				  	}
						  if(BTN_A_LONG>=1){
						  				 									 									  			 	state=STATE_SPEED;
						  				 									 									  			 	BTN_A=0;
						  				 									 									  			 	BTN_B=0;
						  				 									 									  			 	BTN_A_LONG=0;
						  				 									 									  	}

					  }


					  break;
          }
	case STATE_POS:
  {
			  display.Fill(Black);

			  switch(posstate){

			  case STATE_SUMMARY1:

						display.SetCursor(32, 32);
						snprintf((char *)bufferscreen,15, "Latitude:");
						display.WriteString((char *)bufferscreen, Font_6x8, White);
						snprintf((char *)bufferscreen,15, "%0.7f",GNSSData.fLat);//pas forcement utile d'afficher 7 decimales apres la virgule, 6 donne une precision au metre ce qui est le max du gps
						display.SetCursor(32, 40);
						display.WriteString((char *)bufferscreen, Font_6x8, White);
						snprintf((char *)bufferscreen,15, "Longitude:");
						display.SetCursor(32, 48);
						display.WriteString((char *)bufferscreen, Font_6x8, White);
						snprintf((char *)bufferscreen,15, "%0.7f",GNSSData.fLon);
						display.SetCursor(32, 56);
						display.WriteString((char *)bufferscreen, Font_6x8, White);
				  if(BTN_B>=1){
					  posstate=STATE_INFO;
					  BTN_B=0;
				  }



				  break;
			  case STATE_INFO:
				display.Fill(Black);

				snprintf((char *)bufferscreen,15, "hacc=%0.2fm",GNSSData.fhACC);//sert a	connaitre la qualitée du fix si proche de 1 voir inférieur alors le fix est tres bon
				display.SetCursor(32, 32);
				display.WriteString((char *)bufferscreen, Font_7x10, White);
				snprintf((char *)bufferscreen,20, "v=%0.2fV",vbat);
				display.SetCursor(32, 42);
				display.WriteString((char *)bufferscreen, Font_7x10, White);
				display.SetCursor(32, 52);
				snprintf((char *)bufferscreen,15,  "T=%0.2fC",temp);
				display.WriteString((char *)bufferscreen, Font_7x10, White);
				 if(BTN_B>=1){
								  					  posstate=STATE_INFO2;
								  					  BTN_B=0;
								  				  }
								  if(BTN_B_LONG>=1){
													  posstate=STATE_SUMMARY1;
													BTN_B_LONG=0;
												}

				  break;
			  case STATE_INFO2:
			  				display.Fill(Black);

			  				snprintf((char *)bufferscreen,15, "Satnum");//sert a	connaitre la qualitée du fix si proche de 1 voir inférieur alors le fix est tres bon
			  				display.SetCursor(32, 32);
			  				display.WriteString((char *)bufferscreen, Font_6x8, White);
			  				snprintf((char *)bufferscreen,20, "%d sat",GNSSData.numSV);
			  				display.SetCursor(32, 40);
			  				display.WriteString((char *)bufferscreen, Font_6x8, White);
			  				display.SetCursor(32, 48);
			  				snprintf((char *)bufferscreen,15,  "DateJ/M/Y");
			  				display.WriteString((char *)bufferscreen, Font_6x8, White);
			  				display.SetCursor(32, 56);
			  				snprintf((char *)bufferscreen,15,  "%d/%d/%d",GNSSData.day,GNSSData.month,GNSSData.year);
			  				display.WriteString((char *)bufferscreen, Font_6x8, White);
			  				 if(BTN_B>=1){
			  								  					  posstate=STATE_ALT;
			  								  					  BTN_B=0;
			  								  				  }
			  								  if(BTN_B_LONG>=1){
			  													  posstate=STATE_INFO;
			  													BTN_B_LONG=0;
			  												}

			  				  break;
			  case STATE_ALT:


				  						display.SetCursor(32, 32);
				  						snprintf((char *)bufferscreen,15, "altitude:");
				  						display.WriteString((char *)bufferscreen, Font_6x8, White);
				  						snprintf((char *)bufferscreen,15, "%0.2f m",GNSSData.fhMSL);//pas forcement utile d'afficher 7 decimales apres la virgule, 6 donne une precision au metre ce qui est le max du gps
				  						display.SetCursor(32, 40);
				  						display.WriteString((char *)bufferscreen, Font_6x8, White);
				  						snprintf((char *)bufferscreen,15, "Accuracy(m)");
				  						display.SetCursor(32, 48);
				  						display.WriteString((char *)bufferscreen, Font_6x8, White);
				  						snprintf((char *)bufferscreen,15, "%0.1f",GNSSData.fvACC);
				  						display.SetCursor(32, 56);
				  						display.WriteString((char *)bufferscreen, Font_6x8, White);

				  if(BTN_B>=1){
				  					posstate=STATE_ALTBARO;
				  					BTN_B=0;
				  				  }
				  if(BTN_B_LONG>=1){
					  posstate=STATE_INFO2;
					BTN_B_LONG=0;
				}
				  break;
			  case STATE_ALTBARO:
				  						display.SetCursor(32, 32);
				  						snprintf((char *)bufferscreen,15, "baroalt:");
				  						display.WriteString((char *)bufferscreen, Font_6x8, White);
				  						snprintf((char *)bufferscreen,15, "%0.2lf m",0.0);//pas forcement utile d'afficher 7 decimales apres la virgule, 6 donne une precision au metre ce qui est le max du gps
				  						display.SetCursor(32, 40);
				  						display.WriteString((char *)bufferscreen, Font_6x8, White);
				  						snprintf((char *)bufferscreen,15, "baropress:");
				  						display.SetCursor(32, 48);
				  						display.WriteString((char *)bufferscreen, Font_6x8, White);
				  						snprintf((char *)bufferscreen,15, "%0.3lfkPa",0.0);
				  						display.SetCursor(32, 56);
				  						display.WriteString((char *)bufferscreen, Font_6x8, White);

				  if(BTN_B>=1){
				  					posstate=STATE_HEURE;
				  					BTN_B=0;
				  				  }
				  if(BTN_B_LONG>=1){
					  posstate=STATE_ALT;
					BTN_B_LONG=0;
				}
				  break;
			  case STATE_HEURE:
			  			  display.Fill(Black);
			  				  display.SetCursor(32, 32);
			  				  display.WriteString("hr GMT:", Font_6x8, White);
			  				display.SetCursor(32, 42);
			  				snprintf((char *)bufferscreen,15, "%02d:%02d",GNSSData.hour,GNSSData.min);
			  				display.WriteString((char *)bufferscreen, Font_7x10, White);
			  				display.SetCursor(32, 52);
			  				snprintf((char *)bufferscreen,15, "%02d sec",GNSSData.sec);
			  				display.WriteString((char *)bufferscreen, Font_7x10, White);
			  				if(BTN_B>=1){
			  					posstate=STATE_SUMMARY1;
			  					BTN_B=0;
			  				}
			  				if(BTN_B_LONG>=1){
			  							  					posstate=STATE_ALTBARO;
			  							  					BTN_B_LONG=0;
			  							  				}
			  			break;
			  }

					if(BTN_A>=1){
							state=STATE_USB;
							BTN_A=0;
							BTN_B=0;


						}
					if(BTN_A_LONG>=1){
									 									 									  			 	state=STATE_BALISE;
									 									 									  			 	BTN_A=0;
									 									 									  			 	BTN_B=0;
									 									 									  			 	BTN_A_LONG=0;
									 									 									  	}
			  break;
  }
			  case STATE_USB:
        {
					display.Fill(Black);
			  	display.SetCursor(32, 32);
			  	display.WriteString("USB:", Font_6x8, White);

						if(BTN_B_LONG>=1){
							FLAG_USB_STATE=1;
						 								BTN_B_LONG=0;
						 					}

											if(FLAG_USB_STATE){
												FLAG_USB_STATE=0;
												vTaskResume(TaskUSBHandle);
												
											}


						 					 if(BTN_A>=1){
						 						 	state=STATE_BLUETOOTHPING;
						 							BTN_A=0;
						 							BTN_B=0;

						 									  	}
						 					if(BTN_A_LONG>=1){
						 								state=STATE_POS;
						 								BTN_A=0;
						 								BTN_B=0;
						 								BTN_A_LONG=0;
						 					}

				  break;

        }
							  case STATE_BLUETOOTHPING:
        {
					display.Fill(Black);
			  	display.SetCursor(32, 32);
			  	display.WriteString("BL_PING", Font_6x8, White);

						if(BTN_B_LONG>=1){
							FLAG_BL_PING_STATE=1-FLAG_BL_PING_STATE;
						 								
						 					}
												if(FLAG_BL_PING_STATE && BTN_B_LONG){
													BTN_B_LONG=0;
												vTaskResume(TaskHandle_BLE_Ping);
											}
											if(FLAG_BL_PING_STATE==0 && BTN_B_LONG==1){
												BTN_B_LONG=0;
												vTaskSuspend(TaskHandle_BLE_Ping);
											}

						 					 if(BTN_A>=1){
						 						 	state=STATE_BLUETOOTHSEND;
						 							BTN_A=0;
						 							BTN_B=0;

						 									  	}
						 					if(BTN_A_LONG>=1){
						 								state=STATE_USB;
						 								BTN_A=0;
						 								BTN_B=0;
						 								BTN_A_LONG=0;
						 					}

				  break;

        }
				 case STATE_BLUETOOTHSEND:
        {
					display.Fill(Black);
			  	display.SetCursor(32, 32);
			  	display.WriteString("BL_SEND", Font_6x8, White);

						if(BTN_B_LONG>=1){
							FLAG_BL_SEND_STATE=1;
						 								BTN_B_LONG=0;
						 					}

											if(FLAG_BL_SEND_STATE){
												FLAG_BL_SEND_STATE=0;
												vTaskResume(TaskHandle_BLE_Send);
												
											}
						 					 if(BTN_A>=1){
						 						 	state=STATE_SPEED;
						 							BTN_A=0;
						 							BTN_B=0;

						 									  	}
						 					if(BTN_A_LONG>=1){
						 								state=STATE_BLUETOOTHPING;
						 								BTN_A=0;
						 								BTN_B=0;
						 								BTN_A_LONG=0;
						 					}

				  break;

        }
				

	}
return ;
}

























// === ISR ===
void IRAM_ATTR handleBtnA() {
  bool level = digitalRead(BTN_A_PIN);
  if (!level) {
    btnA_press_time = millis();
  } else {
    btnA_irq = true;
  }
}

void IRAM_ATTR handleBtnB() {
  bool level = digitalRead(BTN_B_PIN);
  if (!level) {
    btnB_press_time = millis();
  } else {
    btnB_irq = true;
  }
}



// === PARSE UBX-NAV-PVT ===
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


bool validateUBXChecksum(const uint8_t* msg, size_t len) {
  uint8_t ck_a = 0, ck_b = 0;
  for (size_t i = 2; i < len - 2; i++) {  // Skip sync bytes and CRC bytes
    ck_a += msg[i];
    ck_b += ck_a;
  }
  return (ck_a == msg[len - 2]) && (ck_b == msg[len - 1]);
}




void printBufferHex(const uint8_t* buf, size_t len) {
  for (size_t i = 0; i < len; i++) {
    if (buf[i] < 0x10) Serial.print("0");  // zéro en tête
    Serial.print(buf[i], HEX);
    Serial.print(" ");
  }
}
void GNSS_PrintState(const GNSS_StateHandle* GNSS) {
  Serial.println("=== GPS DATA ===");
  Serial.print("Date: ");
  Serial.print(GNSS->year); Serial.print("-");
  Serial.print(GNSS->month); Serial.print("-");
  Serial.println(GNSS->day);

  Serial.print("Time: ");
  Serial.print(GNSS->hour); Serial.print(":");
  Serial.print(GNSS->min); Serial.print(":");
  Serial.println(GNSS->sec);

  Serial.print("Fix Type: "); Serial.println(GNSS->fixType);
  Serial.print("Satellites: "); Serial.println(GNSS->numSV);

  Serial.print("Latitude: "); Serial.print(GNSS->lat);
  Serial.print(" ("); Serial.print(GNSS->fLat, 7); Serial.println(" °)");

  Serial.print("Longitude: "); Serial.print(GNSS->lon);
  Serial.print(" ("); Serial.print(GNSS->fLon, 7); Serial.println(" °)");

  Serial.print("Altitude: "); Serial.print(GNSS->height); Serial.print(" cm");
  Serial.print(" ("); Serial.print(GNSS->fheight, 2); Serial.println(" m)");

  Serial.print("MSL Height: "); Serial.print(GNSS->hMSL); Serial.print(" cm");
  Serial.print(" ("); Serial.print(GNSS->fhMSL, 2); Serial.println(" m)");

  Serial.print("Horizontal Accuracy: "); Serial.print(GNSS->hAcc);
  Serial.print(" ("); Serial.print(GNSS->fhACC, 2); Serial.println(" m)");

  Serial.print("Vertical Accuracy: "); Serial.print(GNSS->vAcc);
  Serial.print(" ("); Serial.print(GNSS->fvACC, 2); Serial.println(" m)");

  Serial.print("Ground Speed: "); Serial.print(GNSS->gSpeed);
  Serial.print(" ("); Serial.print(GNSS->fgSpeed, 2); Serial.println(" m/s)");

  Serial.print("Heading: "); Serial.println(GNSS->headMot);
  Serial.println("================");
}








void TaskUBXReceiver(void *param) {
  TickType_t xLastWakeTime = xTaskGetTickCount();

  enum { WAIT_SYNC1, WAIT_SYNC2, READ_HEADER, READ_PAYLOAD, READ_CRC } state = WAIT_SYNC1;
  uint8_t buffer[UBX_MSG_SIZE];
  size_t idx = 0;
  uint16_t payload_length = 0;


  while (1) {
    // On traite le buffer UART jusqu'à ce qu'on ait une trame complète ou pas de donnée
    while (GPS_Serial.available()) {
      uint8_t byte = GPS_Serial.read();

      switch (state) {
        case WAIT_SYNC1:
          if (byte == 0xB5) {
            buffer[0] = byte;
            state = WAIT_SYNC2;
          }
          break;

        case WAIT_SYNC2:
          if (byte == 0x62) {
            buffer[1] = byte;
            idx = 2;
            state = READ_HEADER;
          } else {
            state = WAIT_SYNC1;
          }
          break;

        case READ_HEADER:
          buffer[idx++] = byte;
          if (idx == 6) {
            if (buffer[2] == 0x01 && buffer[3] == 0x07) { // NAV-PVT
              payload_length = buffer[4] | (buffer[5] << 8);
              if (payload_length == 92) {
                state = READ_PAYLOAD;
              } else {
                state = WAIT_SYNC1;
              }
            } else {
              state = WAIT_SYNC1;
            }
          }
          break;

        case READ_PAYLOAD:
          buffer[idx++] = byte;
          if (idx == 6 + 92) {
            state = READ_CRC;
          }
          break;

        case READ_CRC:
          buffer[idx++] = byte;
          if (idx == UBX_MSG_SIZE) {
              memcpy(GNSSData.uartWorkingBuffer, buffer, UBX_MSG_SIZE);
              ubx_trame_ready = true;
              GNSS_ParsePVTData(&GNSSData);
            state = WAIT_SYNC1;
            idx=0;
          }
          break;
      }
    }

		int adc_raw = analogRead(ADC_PIN);
    vbat = ((float)adc_raw / ADC_RESOLUTION) * ADC_VREF* 2;
		temp = 25.0 + random(-10, 10) * 0.1; 


    Serial.println("task1!");
    // Attend 100 ms avant la prochaine lecture                                                                               
    vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(100));
  }
}









// === TASK 2: Affichage OLED ===
void TaskDisplay(void *param) {

  TickType_t xLastWakeTime = xTaskGetTickCount();
  

  while (1) {

    flag=1-flag;
    if(flag==0){
strip.setLedColorData(0, strip.Wheel(67));
    }
    else{
strip.setLedColorData(0, strip.Wheel(182));
    }

    statemachine();

    display.UpdateScreen();
    strip.show();
Serial.println("task2!");
    
    
    vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(100));
  }
}




void TaskButtonHandler(void *pvParameters) {
  for (;;) {
    if (btnA_irq) {
      btnA_irq = false;
      unsigned long duration = millis() - btnA_press_time;
      if (duration >= SHORT_PRESS_MIN && duration < LONG_PRESS_TIME)
        BTN_A = true;
      else if (duration >= LONG_PRESS_TIME)
        BTN_A_LONG = true;
    }

    if (btnB_irq) {
      btnB_irq = false;
      unsigned long duration = millis() - btnB_press_time;
      if (duration >= SHORT_PRESS_MIN && duration < LONG_PRESS_TIME)
        BTN_B = true;
      else if (duration >= LONG_PRESS_TIME)
        BTN_B_LONG = true;
    }

    vTaskDelay(pdMS_TO_TICKS(10)); // vérif toutes les 10ms
  }
}








void TaskUSBTransfer(void *param) {

  vTaskSuspend(NULL);
}

void task_ble_send(void* pvParameters) {
  TickType_t xLastWakeTime = xTaskGetTickCount();
  const TickType_t xFrequency = pdMS_TO_TICKS(1000); // toutes les 1s

  while (1) {

		Serial.println("taskBLE_SEND!");
    vTaskDelayUntil(&xLastWakeTime, xFrequency);
  }
}

void task_ble_ping(void* pvParameters) {
	  TickType_t xLastWakeTime = xTaskGetTickCount();
  const TickType_t xFrequency = pdMS_TO_TICKS(1000); // toutes les 1s

  while (1) {
		Serial.println("taskBLE_PING!");
   vTaskDelayUntil(&xLastWakeTime, xFrequency);
  }
}





// === SETUP ===
void setup() {
	Wire.begin(MY_SDA, MY_SCL);
	display.Init();
  display.Fill(Black);
	display.DrawBitmap(32,32, (uint8_t *) startimg, 64, 32, White);
	display.UpdateScreen();

  Serial.begin(115200);
  GPS_Serial.begin(GPS_BAUD, SERIAL_8N1, GPS_RX_PIN, GPS_TX_PIN);

  pinMode(BTN_A_PIN, INPUT_PULLUP);
  pinMode(BTN_B_PIN, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(BTN_A_PIN), handleBtnA, CHANGE);
  attachInterrupt(digitalPinToInterrupt(BTN_B_PIN), handleBtnB, CHANGE);

  strip.begin();
  strip.setBrightness(20);
  

  strip.setLedColorData(0, strip.Wheel(126));
  strip.show();
  Serial.println("slt");

	delay(500);
	display.Fill(Black);
	display.UpdateScreen();

  xTaskCreatePinnedToCore(TaskButtonHandler,"ButtonTask",2048,NULL,1,NULL,0);
  
  BaseType_t result = xTaskCreatePinnedToCore(TaskUBXReceiver, "GPS Task", 4096, NULL, 2, NULL, 0);
if (result != pdPASS) {
  Serial.println("Failed to create task1!");
}
result = xTaskCreatePinnedToCore(TaskDisplay, "Display Task", 4096, NULL, 1, NULL, 0);
if (result != pdPASS) {
  Serial.println("Failed to create task!2");
}
xTaskCreatePinnedToCore(TaskUSBTransfer, "USB Task", 4096, NULL, 1, &TaskUSBHandle, 0);
vTaskSuspend(TaskUSBHandle);

  // Tâches FreeRTOS
xTaskCreatePinnedToCore(task_ble_ping, "BLE Ping", 4096, NULL, 1, &TaskHandle_BLE_Ping, 0);
xTaskCreatePinnedToCore(task_ble_send, "BLE Send", 4096, NULL, 1, &TaskHandle_BLE_Send, 0);

vTaskSuspend(TaskHandle_BLE_Ping);
vTaskSuspend(TaskHandle_BLE_Send);






}

// --- Loop ---
void loop() {
 
 vTaskDelay(pdMS_TO_TICKS(1000));
}
