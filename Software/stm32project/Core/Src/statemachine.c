/*
 * statemachine.c
 *
 *  Created on: May 27, 2024
 *      Author: mathi
 */


#include "main.h"
#include "QMC5883.h"
#include "ssd1306.h"
#include "math.h"
#include "statemachine.h"
#include "nmea_parse.h"
#include <stdio.h>
#include "usbd_def.h"
#include "usbd_core.h"
#include "usbd_hid.h"


extern int BTN_A;
extern int BTN_B;
extern uint8_t RxBuffer[RxBuffer_SIZE];
extern uint8_t DataBuffer[DataBuffer_SIZE];
extern STATE_TYPE state;
extern GPS myData;
float vitmax=0.0;
COMPTEUR statecpt=STATE_HEURES;//etat de lka sous machine a etat finis utilisé pour le compte a rebour
extern int boumheure;
extern int boumminutes;
extern int choose;
extern HEURE hrstate;
extern SPEED spdstate;
extern POS posstate;
extern CHRONO chronostate;
extern KEYBOARD keyboardstate;




float seconde=0;
float min=0;
extern __IO uint32_t uwTick;
uint32_t starttime=0;
uint32_t calctime=0;
extern float temp;
uint8_t usbbuffer[64];
uint8_t usbtransmitbuf[64];
uint8_t gpscommand[100];
int keycount=0;
extern keyboardHID keyboardhid;
char str[20];
QMC_t compasdata;
extern USBD_HandleTypeDef hUsbDeviceFS;
extern UART_HandleTypeDef hlpuart1;
extern DMA_HandleTypeDef hdma_lpuart_rx;
//notre machine à etat, le coeur de notre code, elle est amenée à changer afin d'avoir de nouvelles dispositions sur nos ecran de données

const unsigned char speciale[] = {//logo arduino
		  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
		  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
		  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
		  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
		  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
		  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
		  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
		  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
		  0xff, 0xff, 0x01, 0xff, 0xf0, 0x1f, 0xff, 0xff,
		  0xff, 0xf8, 0xfe, 0x3f, 0x8f, 0xe3, 0xff, 0xff,
		  0xff, 0xf7, 0xff, 0xdf, 0x7f, 0xfd, 0xff, 0xff,
		  0xff, 0xef, 0xff, 0xee, 0xff, 0xfe, 0xff, 0xff,
		  0xff, 0xdf, 0xff, 0xf5, 0xfc, 0x7f, 0x7f, 0xff,
		  0xff, 0xbf, 0xff, 0xfb, 0xfc, 0x7f, 0xbf, 0xff,
		  0xff, 0xbe, 0x00, 0xfb, 0xe0, 0x0f, 0xbf, 0xff,
		  0xff, 0xbe, 0x00, 0xfb, 0xe0, 0x0f, 0xbf, 0xff,
		  0xff, 0xbe, 0x00, 0xfb, 0xe0, 0x0f, 0xbf, 0xff,
		  0xff, 0xbf, 0xff, 0xfb, 0xfc, 0x7f, 0xbf, 0xff,
		  0xff, 0xdf, 0xff, 0xf5, 0xfc, 0x7f, 0x7f, 0xff,
		  0xff, 0xef, 0xff, 0xee, 0xff, 0xfe, 0xff, 0xff,
		  0xff, 0xf7, 0xff, 0xdf, 0x7f, 0xfd, 0xff, 0xff,
		  0xff, 0xf8, 0xfe, 0x3f, 0x8f, 0xe3, 0xff, 0xff,
		  0xff, 0xff, 0x01, 0xff, 0xf0, 0x1f, 0xff, 0xff,
		  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
		  0xff, 0xfc, 0xc6, 0x36, 0x8b, 0x67, 0xff, 0xff,
		  0xff, 0xfb, 0x5a, 0xd6, 0xd9, 0x5b, 0xff, 0xff,
		  0xff, 0xf8, 0x46, 0xd6, 0xda, 0x5b, 0xff, 0xff,
		  0xff, 0xfb, 0x56, 0xd6, 0xdb, 0x5b, 0xff, 0xff,
		  0xff, 0xfb, 0x5a, 0x39, 0x8b, 0x67, 0xff, 0xff,
		  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
		  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
		  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
		};



//le code qui permet d'avoir les ecran de donnée que l'on souhaite, changeable a souhait on peut d'ailleur rajouter des état ou rajouter des fonctionnalité au sein de chaque état, pas encore décidé de la version définitive.
void statemachine(void){
	switch(state){
	 case STATE_SPEED:
				 ssd1306_Fill(Black);
				 ssd1306_SetCursor(32, 32);
				 nmea_parse(&myData, DataBuffer);
				 if(myData.speed>=vitmax){
									 vitmax=myData.speed;
								 }
								 float pace=0;
								 float sec=0;
								 if (myData.speed!=0){
									 pace=1000/(60*myData.speed);
									 sec=(pace-floor(pace))*60;
								 }
								 else {
									 pace=9999;//en cas de division par 0, techniquement le temps devient infini mais ce n'est pas intérréssant
								 }


				  switch(spdstate){


				 case STATE_SUMMARY:
					 if(myData.fix == 1){ //if the GPS has a fix, print the data
					 				 						char * str = (char*)malloc(sizeof(char)*20);
					 				 						snprintf(str,15, "MaxV=%.1f",vitmax*3.6);//amélioration possible la stocker en eeprom
					 				 						ssd1306_SetCursor(32, 32);
					 				 						ssd1306_WriteString(str, Font_6x8, White);
					 				 						snprintf(str,15, "V=%0.1f",(myData.speed)*3.6);
					 				 						ssd1306_SetCursor(32, 42);
					 				 						ssd1306_WriteString(str, Font_6x8, White);
					 				 						snprintf(str,15, "p=%0.0fmin%0.0f s",floor(pace),floor(sec));//affichage au format minute puis seconde
					 				 						ssd1306_SetCursor(32, 52);
					 				 						ssd1306_WriteString(str, Font_6x8, White);
					 				 						free(str);
					 				 					}
					 				 	else{ //if the GPS doesn't have a fix, print a message
					 				 						char *str = (char*)malloc(sizeof(char)*20);// message qui sra dans tous les etat si l'on ne capte pas de sattelites.
					 				 						ssd1306_SetCursor(32, 32);
					 				 						ssd1306_WriteString("Speed 1", Font_6x8, White);
					 				 						ssd1306_SetCursor(32, 44);
					 				 						ssd1306_WriteString("Wait GPS", Font_6x8, White);
					 				 						free(str);
					 				 					}
					 if(BTN_B>=1){
					 					 				 					spdstate++;
					 					 				 					BTN_B=0;
					 					 				  	}



					 break;
				 case STATE_COMPTEUR:
					 if(myData.fix == 1){ //if the GPS has a fix, print the data
					 				 						char * str = (char*)malloc(sizeof(char)*20);
					 				 						snprintf(str,15, "V=%0.1f",(myData.speed)*3.6);
					 				 						ssd1306_SetCursor(32, 54);
					 				 						ssd1306_WriteString(str, Font_6x8, White);
					 				 						ssd1306_DrawRectangle(32, 38, 95, 53, White);
					 				 						ssd1306_FillRectangle(32, 38,(int) floor(32+(myData.speed*0.63)), 53, White);

					 				 						free(str);
					 				 					}
					 				 	else{ //if the GPS doesn't have a fix, print a message
					 				 						char *str = (char*)malloc(sizeof(char)*20);// message qui sra dans tous les etat si l'on ne capte pas de sattelites.
					 				 						ssd1306_SetCursor(32, 32);
					 				 						ssd1306_WriteString("Speed 2", Font_6x8, White);
					 				 						ssd1306_SetCursor(32, 44);
					 				 						ssd1306_WriteString("Wait GPS", Font_6x8, White);
					 				 						free(str);
					 				 					}
					 if(BTN_B>=1){
					 				 					spdstate++;
					 				 					BTN_B=0;
					 				  	}
					 break;
				 case STATE_GRAPH:
					 if(myData.fix == 1){
					 				 						char *str = (char*)malloc(sizeof(char)*20);// message qui sra dans tous les etat si l'on ne capte pas de sattelites.

					 				 						ssd1306_SetCursor(32, 32);
					 				 						snprintf(str,15, "%0.1f",(myData.speed)*3.6);
					 				 						ssd1306_WriteString(str, Font_11x18, White);
					 				 						ssd1306_SetCursor(32, 54);
					 				 						ssd1306_WriteString("kmh", Font_6x8, White);

					 				 						free(str);
					 }
					 else{
						 char *str = (char*)malloc(sizeof(char)*20);// message qui sra dans tous les etat si l'on ne capte pas de sattelites.
						 ssd1306_SetCursor(32, 32);
						 ssd1306_WriteString("Speed 3", Font_6x8, White);
						 ssd1306_SetCursor(32, 44);
						 ssd1306_WriteString("Wait GPS", Font_6x8, White);
						 free(str);
					 }

					 if(BTN_B>=1){
					 					 				 					spdstate--;
					 					 				 					spdstate--;
					 					 				 					BTN_B=0;


					 					 				  	}
					 break;


				 }
				 	if(BTN_A>=1){
				 					state++;
				 					BTN_A=0;
				  	}
				  break;


	case STATE_POS:
			  ssd1306_Fill(Black);
			  nmea_parse(&myData, DataBuffer);
			  switch(posstate){

			  case STATE_SUMMARY1:
				  if(myData.fix == 1){
				  				  char * str = (char*)malloc(sizeof(char)*20);//tout le long, on utilisera le meme buffer str pour tous les affichages
				  						ssd1306_SetCursor(32, 32);
				  						snprintf(str,15, "Latitude:");
				  						ssd1306_WriteString(str, Font_6x8, White);
				  						snprintf(str,15, "%0.7f",myData.latitude);//pas forcement utile d'afficher 7 decimales apres la virgule, 6 donne une precision au metre ce qui est le max du gps
				  						ssd1306_SetCursor(32, 40);
				  						ssd1306_WriteString(str, Font_6x8, White);
				  						snprintf(str,15, "Longitude:");
				  						ssd1306_SetCursor(32, 48);
				  						ssd1306_WriteString(str, Font_6x8, White);
				  						snprintf(str,15, "%0.7f",myData.longitude);
				  						ssd1306_SetCursor(32, 56);
				  						ssd1306_WriteString(str, Font_6x8, White);
				  						free(str);
				  					}
				  				else{ //if the GPS doesn't have a fix, print a message
				  						char *str = (char*)malloc(sizeof(char)*20);
				  						ssd1306_SetCursor(32, 32);
				  						ssd1306_WriteString("Pos1", Font_6x8, White);
				  						ssd1306_SetCursor(32, 44);
				  						ssd1306_WriteString("Wait GPS", Font_6x8, White);
				  						free(str);
				  					}
				  if(BTN_B>=1){
					  posstate++;
					  BTN_B=0;
				  }





				  break;
			  case STATE_LAT:
				  if(myData.fix == 1){
				  				  char * str = (char*)malloc(sizeof(char)*20);//tout le long, on utilisera le meme buffer str pour tous les affichages
				  						ssd1306_SetCursor(32, 32);
				  						snprintf(str,15, "LatSide:");
				  						ssd1306_WriteString(str, Font_6x8, White);
				  						snprintf(str,15, "%c",myData.latSide);//pas forcement utile d'afficher 7 decimales apres la virgule, 6 donne une precision au metre ce qui est le max du gps
				  						ssd1306_SetCursor(32, 40);
				  						ssd1306_WriteString(str, Font_6x8, White);
				  						snprintf(str,15, "Latitude:");
				  						ssd1306_SetCursor(32, 48);
				  						ssd1306_WriteString(str, Font_6x8, White);
				  						snprintf(str,15, "%0.7f",myData.latitude);
				  						ssd1306_SetCursor(32, 56);
				  						ssd1306_WriteString(str, Font_6x8, White);
				  						free(str);
				  					}
				  				else{ //if the GPS doesn't have a fix, print a message
				  						char *str = (char*)malloc(sizeof(char)*20);
				  						ssd1306_SetCursor(32, 32);
				  						ssd1306_WriteString("Pos2", Font_6x8, White);
				  						ssd1306_SetCursor(32, 44);
				  						ssd1306_WriteString("Wait GPS", Font_6x8, White);
				  						free(str);
				  					}
				  if(BTN_B>=1){
				  					  posstate++;
				  					  BTN_B=0;
				  				  }



				  break;
			  case STATE_LONG:
				  if(myData.fix == 1){
				  				  char * str = (char*)malloc(sizeof(char)*20);//tout le long, on utilisera le meme buffer str pour tous les affichages
				  						ssd1306_SetCursor(32, 32);
				  						snprintf(str,15, "LonSide:");
				  						ssd1306_WriteString(str, Font_6x8, White);
				  						snprintf(str,15, "%c",myData.lonSide);//pas forcement utile d'afficher 7 decimales apres la virgule, 6 donne une precision au metre ce qui est le max du gps
				  						ssd1306_SetCursor(32, 40);
				  						ssd1306_WriteString(str, Font_6x8, White);
				  						snprintf(str,15, "Longitude:");
				  						ssd1306_SetCursor(32, 48);
				  						ssd1306_WriteString(str, Font_6x8, White);
				  						snprintf(str,15, "%0.7f",myData.longitude);
				  						ssd1306_SetCursor(32, 56);
				  						ssd1306_WriteString(str, Font_6x8, White);
				  						free(str);
				  					}
				  				else{ //if the GPS doesn't have a fix, print a message
				  						char *str = (char*)malloc(sizeof(char)*20);
				  						ssd1306_SetCursor(32, 32);
				  						ssd1306_WriteString("Pos3", Font_6x8, White);
				  						ssd1306_SetCursor(32, 44);
				  						ssd1306_WriteString("Wait GPS", Font_6x8, White);
				  						free(str);
				  					}
				  if(BTN_B>=1){
				  					  posstate++;
				  					  BTN_B=0;
				  				  }


				  break;
			  case STATE_ALT:
				  if(myData.fix == 1){
				  				  char * str = (char*)malloc(sizeof(char)*20);//tout le long, on utilisera le meme buffer str pour tous les affichages
				  						ssd1306_SetCursor(32, 32);
				  						snprintf(str,15, "altitude:");
				  						ssd1306_WriteString(str, Font_6x8, White);
				  						snprintf(str,15, "%0.1f m",myData.altitude);//pas forcement utile d'afficher 7 decimales apres la virgule, 6 donne une precision au metre ce qui est le max du gps
				  						ssd1306_SetCursor(32, 40);
				  						ssd1306_WriteString(str, Font_6x8, White);
				  						snprintf(str,15, "Pressure:");
				  						ssd1306_SetCursor(32, 48);
				  						ssd1306_WriteString(str, Font_6x8, White);
				  						snprintf(str,15, "%0.1fhpa",1000*expf((-0.0001148)*(myData.altitude)));
				  						ssd1306_SetCursor(32, 56);
				  						ssd1306_WriteString(str, Font_6x8, White);
				  						free(str);
				  					}
				  				else{ //if the GPS doesn't have a fix, print a message
				  						char *str = (char*)malloc(sizeof(char)*20);
				  						ssd1306_SetCursor(32, 32);
				  						ssd1306_WriteString("Pos4", Font_6x8, White);
				  						ssd1306_SetCursor(32, 44);
				  						ssd1306_WriteString("Wait GPS", Font_6x8, White);
				  						free(str);
				  					}
				  if(BTN_B>=1){
				  					posstate--;
				  					posstate--;
				  					posstate--;
				  					BTN_B=0;
				  				  }
				  break;
			  }

					if(BTN_A>=1){
							state++;
							BTN_A=0;
						}
			  break;

		  case STATE_HEURE:
			  ssd1306_Fill(Black);
			  nmea_parse(&myData, DataBuffer);
			  int heure=0;
			  heure=(myData.lastMeasure[0]&0x0f)*10+(myData.lastMeasure[1]&0x0f);
			  if((heure+2)>=24){
				  heure=heure-22;
			  }
			  else{
				  heure=heure+2;
			  }

			  if(myData.fix == 1){

				  switch(hrstate){
				  case STATE_DIGIT:
					  char * str = (char*)malloc(sizeof(char)*20);
					  ssd1306_SetCursor(32, 32);
					  ssd1306_WriteString("hr GMT+2:", Font_6x8, White);
					 ssd1306_SetCursor(34, 42);
					 if(heure>=10){
					snprintf(str,15, "%d:%c%c",heure,myData.lastMeasure[2],myData.lastMeasure[3]);
					}
					else{
					 	snprintf(str,15, "0%d:%c%c",heure,myData.lastMeasure[2],myData.lastMeasure[3]);
					}
					ssd1306_WriteString(str, Font_7x10, White);
					ssd1306_SetCursor(34, 52);
					snprintf(str,15, "%c%c sec",myData.lastMeasure[4],myData.lastMeasure[5]);
					ssd1306_WriteString(str, Font_6x8, White);
					free(str);
					if(BTN_B>=1){
												hrstate++;
												BTN_B=0;
											}




					  break;
				  case STATE_OLD:
					  if(heure>12){
						  heure=heure-12;
					  }
					  int minute=0;
					  minute=(myData.lastMeasure[2]&0x0f)*10+(myData.lastMeasure[3]&0x0f);
					  ssd1306_DrawCircle(64, 48, 12, White);
					  ssd1306_Line(64,48,64+floor(6*cos((3-heure)*M_PI/6)),48-floor(6*sin((3-heure)*M_PI/6)),White);
					  ssd1306_Line(64,48,64+floor(12*cos((15-minute)*M_PI/30)),48-floor(12*sin((15-minute)*M_PI/30)),White);
					  for(int i=0;i<=12;i++){
						  ssd1306_Line(64+floor(10*cos((i)*M_PI/6)),48+floor(10*sin((i)*M_PI/6)),64+floor(12*cos((i)*M_PI/6)),48+floor(12*sin((i)*M_PI/6)),White);
					  }
					  if(BTN_B>=1){
					  	hrstate--;
					  	BTN_B=0;
					  }


					  break;

				  }

			  			 					}
			  			 	else{ //if the GPS doesn't have a fix, print a message
			  			 						char *str = (char*)malloc(sizeof(char)*20);
			  			 						ssd1306_SetCursor(32, 44);
			  			 						ssd1306_WriteString("Wait GPS", Font_6x8, White);
			  			 						free(str);
			  			 	}
				  if(BTN_A>=1){
			  		state++;
			  		BTN_A=0;
			  		BTN_B=0;
			  		}
			  break;
		  case STATE_INFO:
			  ssd1306_Fill(Black);
			  nmea_parse(&myData, DataBuffer);
			  if(myData.fix == 1){ //if the GPS has a fix, print the data
			 			 						char * str = (char*)malloc(sizeof(char)*20);
			 			 						snprintf(str,15, "hdop=%.1f",myData.hdop);//sert a	connaitre la qualitée du fix si proche de 1 voir inférieur alors le fix est tres bon
			 			 						ssd1306_SetCursor(32, 32);
			 			 						ssd1306_WriteString(str, Font_6x8, White);
			 			 						snprintf(str,15, "SatNb :%d",myData.satelliteCount);
			 			 						ssd1306_SetCursor(32, 42);
			 			 						ssd1306_WriteString(str, Font_6x8, White);
			 			 						ssd1306_SetCursor(32, 50);
			 			 						snprintf(str,15,  "T=%0.2fC",temp);
			 			 						ssd1306_WriteString(str, Font_6x8, White);
			 			 						free(str);
			 			 					}
			 			 	else{ //if the GPS doesn't have a fix, print a message
			 			 						char *str = (char*)malloc(sizeof(char)*20);
			 			 						ssd1306_SetCursor(32, 32);
			 			 						ssd1306_WriteString("INFO", Font_6x8, White);
			 			 						ssd1306_SetCursor(32, 41);
			 			 						ssd1306_WriteString("Wait GPS", Font_6x8, White);
			 			 						ssd1306_SetCursor(32, 50);
			 			 						snprintf(str,15, "T=%0.2fC",temp);
			 			 						ssd1306_WriteString(str, Font_6x8, White);
			 			 						free(str);
			 			 					}
			  if(BTN_A>=1){
			  		state++;
			  		BTN_A=0;
			  		BTN_B=0;
			  		}
			  break;

		  case STATE_CHRONOMETER:

			  ssd1306_Fill(Black);
			  ssd1306_SetCursor(32, 32);
			  char *str = (char*)malloc(sizeof(char)*20);
			  ssd1306_WriteString("chrono", Font_6x8, White);
			  ssd1306_SetCursor(32, 40);

			  switch(chronostate){
			  case STATE_RESET:
				  min=0;
				  seconde=0;
				  calctime=0;
				  	 if(BTN_B>=1){
				  		chronostate++;
				  		BTN_B=0;
				  		starttime=uwTick;

				  }


				  break;
			  case STATE_RUN:
				  calctime=uwTick-starttime;

				  if(BTN_B>=1){
				  		chronostate++;
				  		BTN_B=0;
			  }


				  break;
			  case STATE_PAUSE:
				  if(BTN_B>=1){
				  			chronostate--;
				  			chronostate--;
				  			BTN_B=0;
			 }

				  break;
			  }

			  min=floor((float) calctime/60000);
			  seconde=(float) ((calctime-(min*60000))/1000);
			  snprintf(str,15, "%0.0fmin",min);
			  ssd1306_WriteString(str, Font_7x10, White);
			  ssd1306_SetCursor(32, 50);
			  snprintf(str,15, "%0.3fsec",seconde);
			  ssd1306_WriteString(str, Font_6x8, White);
			  free(str);


			  if(BTN_A>=1){
			 	state++;
			 	BTN_A=0;
			 	BTN_B=0;


	}
			  break;
			  case STATE_SPEEDTEST:
				  ssd1306_Fill(Black);
				  ssd1306_SetCursor(32, 32);
				  ssd1306_WriteString("clavierusb", Font_6x8, White);
				  ssd1306_SetCursor(32, 40);

				  switch(keyboardstate){

				  case STATE_MARCHE:
					  ssd1306_WriteString("NI", Font_6x8, White);
					  keyboardhid.MODIFIER = 0x02;
					  keyboardhid.KEYCODE1 = 0x11;
					  keyboardhid.KEYCODE2 = 0x0C;
					  USBD_HID_SendReport(&hUsbDeviceFS, &keyboardhid, sizeof (keyboardhid));
					  HAL_Delay (50);
					  keyboardhid.MODIFIER = 0x00;
					  keyboardhid.KEYCODE1 = 0x00;
					  keyboardhid.KEYCODE2 = 0x00;
					  keyboardhid.KEYCODE3 = 0x00;
					  keyboardhid.KEYCODE4 = 0x00;
					  keyboardhid.KEYCODE5 = 0x00;
					  keyboardhid.KEYCODE6 = 0x00;
					  USBD_HID_SendReport(&hUsbDeviceFS, &keyboardhid, sizeof (keyboardhid));
					  HAL_Delay (50);

					  if(BTN_B>=1){
						  keyboardstate++;
						  BTN_B=0;
					  }


					  break;
				  case STATE_REPOS:

					  ssd1306_WriteString("repos", Font_6x8, White);

					  if(BTN_B>=1){
						  keyboardstate--;
						  BTN_B=0;
					  }


					  break;
				  }

				  if(BTN_A>=1){
				  				  				  			 	state++;
				  				  				  			 	BTN_A=0;
				  				  				  			 	BTN_B=0;
				  				  				  			uint8_t baudchange[]={0x24, 0x50, 0x4D, 0x54,0x4B ,0x32, 0x35 ,0x31 ,0x2C ,0x33, 0x38, 0x34 ,0x30, 0x30 ,0x2A, 0x32 ,0x37, 0x0A};
				  				  				  		__HAL_DMA_DISABLE_IT(&hdma_lpuart_rx, DMA_IT_HT);
				  				  				  	HAL_UART_Transmit_IT(&hlpuart1,(uint8_t *)baudchange,18);
				  				  				  			uint8_t disablenmea[]={0x24, 0x50, 0x4D, 0x54, 0x4B, 0x33, 0x31, 0x34, 0x2C, 0x30, 0x2C, 0x31, 0x2C, 0x30, 0x2C, 0x30, 0x2C, 0x30, 0x2C, 0x30, 0x2C, 0x30, 0x2C, 0x30, 0x2C, 0x30,
				  				  				  					0x2C, 0x30, 0x2C, 0x30, 0x2C, 0x30, 0x2C, 0x30, 0x2C, 0x30, 0x2C, 0x30, 0x2C, 0x30, 0x2C, 0x30, 0x2C, 0x30, 0x2C, 0x30, 0x2A, 0x32, 0x39};//$PMTK314,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*29
				  				  				  		HAL_UART_Transmit_IT(&hlpuart1,(uint8_t *)disablenmea,49);
				  				  				  		uint8_t updaterate[]={0x24, 0x50, 0x4D, 0x54, 0x4B, 0x32, 0x32, 0x30, 0x2C, 0x20, 0x32, 0x30, 0x30, 0x2A, 0x32, 0x43};//$PMTK220, 200*2C
				  				  				  	HAL_UART_Transmit_IT(&hlpuart1,(uint8_t *)updaterate,16);
				  				  				  		HAL_UART_Abort(&hlpuart1);
				  				  				  			HAL_UART_DeInit(&hlpuart1);
				  				  				  			hlpuart1.Init.BaudRate = 38400;
				  				  				  			HAL_UART_Init(&hlpuart1);
				  				  				  			HAL_UART_Abort(&hlpuart1);
				  				  				  			HAL_UART_Receive_DMA(&hlpuart1, (uint8_t *)RxBuffer, RxBuffer_SIZE);

				  				  				  	}






				  break;
			  case STATE_MEMTEST:
				  ssd1306_Fill(Black);
				  ssd1306_SetCursor(32, 32);
				  ssd1306_WriteString("test", Font_6x8, White);
				 nmea_parse(&myData, DataBuffer);

				 			 			 						snprintf(str,15, "spd=%.1f",myData.speed);//sert a	connaitre la qualitée du fix si proche de 1 voir inférieur alors le fix est tres bon
				 			 			 						ssd1306_SetCursor(32, 40);
				 			 			 						ssd1306_WriteString(str, Font_6x8, White);
				 			 			 						snprintf(str,15, "SatNb :%d",myData.satelliteCount);
				 			 			 						ssd1306_SetCursor(32, 48);
				 			 			 						ssd1306_WriteString(str, Font_6x8, White);
				 			 			 						free(str);



				  if(BTN_A>=1){
				  				  			 	state--;
				  				  			 	state--;
				  				  			 	state--;
				  				  			 	state--;
				  				  			 	state--;
				  				  			 	state--;
				  				  			 	BTN_A=0;
				  				  			 	BTN_B=0;
				  				  	}




				  break;


	}

return ;
}



















//		  case STATE_CHOOSE:
//		  		if(choose==0){//choose se met a jour à 1 dès que le choix de l'heure de fin a été fait ainsi on nous le redemande jamais avant que le compte a rebours soit finis
//
//		  			ssd1306_Fill(Black);
//		  			ssd1306_SetCursor(32, 32);
//		  			ssd1306_WriteString("choose_HR", Font_6x8, White);
//		  		switch(statecpt){
//		  			case STATE_HEURES:
//		  				if(BTN_A>=1){
//		  					(boumheure)=(boumheure)+1;// on choisi l'heure avec le bouton a et on passe aux minutes avec le bouton b
//		  					BTN_A=0;
//		  				}
//		  				if(boumheure>=24){
//		  					boumheure=0;
//		  				}
//		  				if(BTN_B>=1){
//		  					statecpt++;
//		  					BTN_B=0;
//		  				}
//		  				ssd1306_SetCursor(34, 42);
//		  				snprintf(str,15, "%d:%d",boumheure,boumminutes);
//		  				ssd1306_WriteString(str, Font_11x18, White);
//
//
//
//
//		  				break;
//		  			case STATE_MINUTE:
//		  				ssd1306_Fill(Black);
//		  				ssd1306_SetCursor(32, 32);
//		  				ssd1306_WriteString("choose_MIN", Font_6x8, White);
//		  				if(BTN_A>=1){
//		  					(boumminutes)=(boumminutes)+1;
//		  					BTN_A=0;
//		  				}
//		  				if(boumminutes>=59){
//		  					boumminutes=0;
//		  				}
//		  				if(BTN_B>=1){
//		  					statecpt++;
//		  					BTN_B=0;
//		  				}
//		  				ssd1306_SetCursor(34, 42);
//		  				snprintf(str,15, "%d:%d",boumheure,boumminutes);
//		  				ssd1306_WriteString(str, Font_11x18, White);
//
//		  				break;
//		  			case STATE_VALID:// on rappelle l'heure choisie puis on remet tout a zero sauf choose
//		  				ssd1306_Fill(Black);
//		  				char * str = (char*)malloc(sizeof(char)*20);
//		  				ssd1306_SetCursor(32, 32);
//		  				ssd1306_WriteString("choosed_HR:", Font_6x8, White);
//		  				ssd1306_SetCursor(34, 42);
//		  				snprintf(str,15, "%d:%d",boumheure,boumminutes);
//		  				ssd1306_WriteString(str, Font_11x18, White);
//		  				ssd1306_UpdateScreen();
//		  				HAL_Delay(1000);
//		  				choose=1;
//		  				free(str);
//		  				statecpt--;
//		  				statecpt--;
//		  				break;
//	}
//		  		}
//		  		else{
//		  			ssd1306_Fill(Black);
//		  			nmea_parse(&myData, DataBuffer);
//		  			ssd1306_SetCursor(32, 32);
//		  			ssd1306_WriteString("countdown", Font_6x8, White);// mode compte a rebours apres le choix
//		  						  int heure=0;
//
//
//
//		  						  if(myData.fix == 1){ //if the GPS has a fix, print the data
//		  							heure=(myData.lastMeasure[0]&0x0f)*10+(myData.lastMeasure[1]&0x0f);
//		  							if((heure+2)>=24){
//		  									 heure=heure-22;
//		  									  	}
//		  								else{
//		  									heure=heure+2;
//		  									  	}
//
//		  							  	char * str = (char*)malloc(sizeof(char)*20);
//		  						  		ssd1306_SetCursor(34, 42);
//		  						  		if((boumminutes-((myData.lastMeasure[2]&0x0F)*10+(myData.lastMeasure[3]&0x0F)))>=0){
//		  						  			 					snprintf(str,15, "%d:%d",(boumheure-heure),(boumminutes-((myData.lastMeasure[2]&0x0F)*10+(myData.lastMeasure[3]&0x0F))));// on s'assure de bien afficher le temps restant
//
//		  						  			 					}
//		  						  		else{
//		  						  			 					snprintf(str,15, "%d:%d",(boumheure-heure)-1,60+(boumminutes-((myData.lastMeasure[2]&0x0F)*10+(myData.lastMeasure[3]&0x0F))));
//		  						  			 					}
//		  						  			 				ssd1306_WriteString(str, Font_7x10, White);
//		  						  			 				snprintf(str,15, "%d sec",60-(((myData.lastMeasure[4]&0x0F)*10)+(myData.lastMeasure[5]&0x0F)));
//		  						  			 				ssd1306_SetCursor(34, 52);
//		  						  			 				ssd1306_WriteString(str, Font_6x8, White);
//		  						  			 				free(str);
//		  						  		if((boumheure-heure)<=0&&(boumminutes-((myData.lastMeasure[2]&0x0F)*10+(myData.lastMeasure[3]&0x0F)))<=0){// si le compte a rebours tombe a zero on allume un gpio, on affiche un message puis on reset definitivement le compte a rebours
//		  						  			 							HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,1);
//		  						  			 						ssd1306_Fill(Black);
//
//		  						  			 						ssd1306_SetCursor(32, 48);
//
//		  						  			 						ssd1306_WriteString("explosion", Font_7x10, White);
//		  						  			 						ssd1306_UpdateScreen();
//		  						  			 						HAL_Delay(2000);
//		  						  			 							HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,0);
//		  						  			 							boumheure=0;
//		  						  			 							boumminutes=0;
//		  						  			 							choose=0;
//		  						  			 							state--;
//		  						  			 							state--;//state=state-4 ne marche pas pour une raison que nous ignorons
//		  						  			 							state--;
//		  						  			 							state--;
//		  						  			 							BTN_A=0;
//		  						  			 						}
//		  						  			 					}
//		  						  			 	else{ //if the GPS doesn't have a fix, print a message
//		  						  			 						char *str = (char*)malloc(sizeof(char)*20);
//		  						  			 						ssd1306_SetCursor(32, 44);
//		  						  			 						ssd1306_WriteString("Wait GPS", Font_6x8, White);
//		  						  			 						free(str);
//		  						  			 	}
//
//
//
//
//
//
//
//
//
//		  						  if(BTN_A>=1){
//		  							state--;
//		  							state--;//state=state-4 ne marche pas pour une raison que nous ignorons
//		  							state--;
//		  							state--;
//		  							BTN_A=0;
//		  							}
//
//
//
//
//		  		}
//		  		 break;









//				  HAL_Delay(100);
//				  for(int i=0;i<10;i++){
//					  displaybuf[i]=usbbuffer[i];
//				  }
//				  ssd1306_WriteString((uint8_t*)displaybuf, Font_6x8, White);
//
//
//				  if(strcmp((uint8_t*)displaybuf,"temp")==0){
//					  ssd1306_SetCursor(32, 48);
//					  snprintf((uint8_t*)usbtransmitbuf,64, "la temperature du processeur est de:%0.2fC\n",temp);
//					  snprintf(str,15, "T=%0.2fC",temp);
//					  ssd1306_WriteString(str, Font_6x8, White);
//				  }
//				  else{
//					  ssd1306_SetCursor(32, 48);
//					  ssd1306_WriteString("nonvalide", Font_6x8, White);
//					  snprintf((uint8_t*)usbtransmitbuf,64, "Veuillez ecrire quelque chose\n");
//
//				  }
//				  CDC_Transmit_FS((uint8_t * )usbtransmitbuf,strlen(usbtransmitbuf));



//				  ee_read(0, 2048, (uint8_t *)eepromold);
//				  while((eepromold[eeindex]&0xFF)!=0xFF){
//					  eeindex++;
//				  }
//				  ee_format(0);
//
//				  memset((uint8_t*)strb,'0',10);
//				  memset((uint8_t*)eepromnew,'a',2048);
//				  memcpy((uint8_t*)eepromnew,(uint8_t*)eepromold,eeindex);
//
//				  snprintf((uint8_t*)strb,10, "%0.2f\n",temp);
//				  memcpy((uint8_t*)eepromnew[eeindex],(uint8_t*)strb,10);
//				  eeindex=eeindex+10;
//				  ee_write(0, 2048, (uint8_t *)eepromnew);
//				  ssd1306_SetCursor(32, 42);
//				  snprintf(str,15, "%d",eeindex);
//				  ssd1306_WriteString(str, Font_6x8, White);
//				  HAL_Delay(100);
//				  if(eeindex>=2048){
//					  eeindex=0;
//				  }
















