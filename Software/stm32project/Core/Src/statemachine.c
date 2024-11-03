/*
 * statemachine.c
 *
 *  Created on: May 27, 2024
 *      Author: mathi
 */


#include "main.h"

#include "ssd1306.h"
#include "math.h"
#include "statemachine.h"
#include "nmea_parse.h"
#include <stdio.h>
#include "usbd_def.h"
#include "usbd_core.h"
#include "spif.h"
#include "spi.h"




extern USBD_HandleTypeDef hUsbDeviceFS;
extern UART_HandleTypeDef hlpuart1;
extern DMA_HandleTypeDef hdma_lpuart_rx;
extern SPIF_HandleTypeDef hspif1;



extern char str[20];
extern uint8_t DataBuffer[DataBuffer_SIZE];
extern uint8_t flashwrite[256];
extern uint8_t flashread[256];
extern uint8_t usbbuffer[64];
extern uint8_t indexbuffer[50];
extern uint8_t bufferscreen[50];


extern STATE_TYPE state;
extern HEURE hrstate;
extern SPEED spdstate;
extern POS posstate;
extern CHRONO chronostate;
extern GIF gifstate;
extern USBSTATE usbstate;
extern BALISESTATE balisestate;


extern int BTN_A;
extern int BTN_B;
extern GPS myData;

extern float vitmax;
extern float seconde;
extern float min;
extern uint32_t starttime;
extern uint32_t calctime;
extern float temp;
extern float vbat;
extern float t1;
extern float t2;
extern float t3;
extern float moy;
extern float framerate;
extern int flashbufferlen;

extern int pageoffset;
extern int pagenumber;
extern int sectoreraseen;

extern int erasetime;
extern int erasedisplay;
extern int usbtransmiten;
extern float usbpercent;

extern int doubledonnee;
extern double distanceparcouru;
extern double oldlat;
extern double oldlong;
extern int cptdoubledonnee;


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
									 pace=9999;
								 }


				  switch(spdstate){


				 case STATE_SUMMARY:
					 if(myData.fix == 1){
					 				 						char * str = (char*)malloc(sizeof(char)*20);
					 				 						snprintf(str,15, "MaxV=%.1f",vitmax*3.6);
					 				 						ssd1306_SetCursor(32, 32);
					 				 						ssd1306_WriteString(str, Font_6x8, White);
					 				 						snprintf(str,15, "V=%0.1f",(myData.speed)*3.6);
					 				 						ssd1306_SetCursor(32, 42);
					 				 						ssd1306_WriteString(str, Font_6x8, White);
					 				 						snprintf(str,15, "p=%0.0fmin%0.0f s",floor(pace),floor(sec));
					 				 						ssd1306_SetCursor(32, 52);
					 				 						ssd1306_WriteString(str, Font_6x8, White);
					 				 						free(str);
					 				 					}
					 				 	else{ //if the GPS doesn't have a fix, print a message
					 				 						char *str = (char*)malloc(sizeof(char)*20);
					 				 						ssd1306_SetCursor(32, 32);
					 				 						ssd1306_WriteString("Speed 1", Font_6x8, White);
					 				 						ssd1306_SetCursor(32, 44);
					 				 						ssd1306_WriteString("Wait GPS", Font_6x8, White);
					 				 						batterygauge(vbat,35, 54,1);
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
					 				 						batterygauge(vbat,35, 54,1);

					 				 						free(str);
					 }
					 else{
						 char *str = (char*)malloc(sizeof(char)*20);// message qui sra dans tous les etat si l'on ne capte pas de sattelites.
						 ssd1306_SetCursor(32, 32);
						 ssd1306_WriteString("Speed 3", Font_6x8, White);
						 ssd1306_SetCursor(32, 44);
						 ssd1306_WriteString("Wait GPS", Font_6x8, White);
						 batterygauge(vbat,35, 54,1);
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
				 					BTN_B=0;
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
				  						ssd1306_SetCursor(32, 54);
				  						batterygauge(vbat,35, 54,1);
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
							BTN_B=0;


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
			 			 						snprintf(str,20, "v=%0.2fV",vbat);
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
			 			 						snprintf(str,15, "vbat=%0.2fV",vbat);
			 			 						ssd1306_WriteString(str, Font_6x8, White);
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

			  case STATE_BALISE:
				  ssd1306_Fill(Black);
				  switch(balisestate){
				  case BALISESTATE1:
					  ssd1306_SetCursor(32,32);
					  ssd1306_WriteString("balise",Font_6x8,White);
					  ssd1306_SetCursor(32,40);
					  ssd1306_WriteString("do nothing",Font_6x8,White);

					  if(BTN_B>=1){
						  balisestate++;
						  BTN_B=0;
						  BTN_A=0;
					  }
					  if(BTN_A>=1){
					  				  			 	state++;
					  				  			 	BTN_A=0;
					  				  			 	BTN_B=0;


					  				  	}



					  break;
				  case BALISESTATE2:
					  oldlat=myData.latitude;
					  oldlong=myData.longitude;
					  nmea_parse(&myData, DataBuffer);
					  distanceparcouru=distanceparcouru+distancecalc(oldlat, myData.latitude, oldlong, myData.longitude);


					  if(pagenumber+1<MAX_WRITE_PAGE){

					  flashbufferlen=csvframe((uint8_t *)flashwrite,temp,vbat,&myData,myData.satelliteCount,myData.hdop);
					  writebuffertoflash((uint8_t*)flashwrite,flashbufferlen);
					  if(doubledonnee==0){
						  ssd1306_SetCursor(32,32);
						  snprintf((uint8_t *)bufferscreen,50,"p=%d",pagenumber);
						  ssd1306_WriteString((uint8_t*)bufferscreen,Font_7x10,White);
					  }
					  else{
						  ssd1306_SetCursor(32,32);
						  snprintf((uint8_t *)bufferscreen,50,"d=%0.1lfKm",distanceparcouru);
						  ssd1306_WriteString((uint8_t*)bufferscreen,Font_7x10,White);
					  }
					  ssd1306_SetCursor(32,42);
					  snprintf((uint8_t *)bufferscreen,50, "sat=%d",myData.satelliteCount);
					  ssd1306_WriteString((uint8_t*)bufferscreen,Font_6x8,White);
					  batterygauge(vbat,34, 50,1);
					  ssd1306_SetCursor(60,50);
					  snprintf((uint8_t *)bufferscreen,50, "%0.2fV",vbat);
					  ssd1306_WriteString((uint8_t*)bufferscreen,Font_6x8,White);
					  if(cptdoubledonnee==3){
						  doubledonnee=1-doubledonnee;
						  cptdoubledonnee=0;
					  }
					  cptdoubledonnee+=1;
					  HAL_Delay(1000);

					  if(BTN_B>=1){
					  						  balisestate--;
					  						  BTN_B=0;
					  						  BTN_A=0;
					  					  }

					  }



					  else{
					 						  balisestate=2;
					 					  }



					  break;

				  case BALISESTATE3:
					  ssd1306_SetCursor(32,32);
					  ssd1306_WriteString("fin de",Font_6x8,White);
					  ssd1306_SetCursor(32,40);
					  ssd1306_WriteString("memoire",Font_6x8,White);
					  ssd1306_SetCursor(32,48);
					  snprintf((uint8_t *)bufferscreen,50,"%d,%d",pageoffset,pagenumber);
					  ssd1306_WriteString((uint8_t*)bufferscreen,Font_6x8,White);
					  if(BTN_A>=1){
					  				  			 	state++;
					  				  			 	BTN_A=0;
					  				  			 	BTN_B=0;


					  				  	}





					  break;



				  }


				  break;


			  case STATE_USB:
				  ssd1306_Fill(Black);
				  ssd1306_SetCursor(32,32);
				  ssd1306_WriteString("usb",Font_6x8,White);
				  switch(usbstate){
				 				  case USBSTATE1:
				 					  ssd1306_SetCursor(32,40);
				 					  ssd1306_WriteString("do nothing",Font_6x8,White);
				 					  usbtransmiten=0;

				 					  if(BTN_B>=1){
				 						  usbstate++;
				 						  BTN_B=0;
				 						  BTN_A=0;
				 					  }
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
				 				  case USBSTATE2:
				 					 ssd1306_SetCursor(32,40);
				 					ssd1306_WriteString("Push A",Font_6x8,White);
				 					ssd1306_SetCursor(32,48);
				 					ssd1306_WriteString("to erase",Font_6x8,White);

				 					if(erasedisplay==1){

				 						snprintf((uint8_t *)bufferscreen,50,"t=%0.2f",(float)erasetime/1000);
				 						ssd1306_SetCursor(32,56);
				 						ssd1306_WriteString((uint8_t*)bufferscreen,Font_6x8,White);
				 					}


				 					if(BTN_A>=1){
				 						erasetime=HAL_GetTick();
				 						SPIF_EraseChip(&hspif1);
				 						erasetime=HAL_GetTick()-erasetime;
				 						erasedisplay=1;
				 						pageoffset=0;
				 						pagenumber=0;
				 						storeindex();
				 						BTN_A=0;
				 						BTN_B=0;
				 					}

				 					if(BTN_B>=1){

					 						usbstate++;
					 						  BTN_B=0;
					 						  BTN_A=0;
					 					  }

				 			  break;

				 				 case USBSTATE3:

				 					ssd1306_Fill(Black);
				 					ssd1306_SetCursor(32,40);
				 					ssd1306_WriteString("write",Font_6x8,White);


				 					if(pagenumber>0){


				 					int i=0;
				 					if(usbtransmiten==0){
				 						while(i<pagenumber){
				 							ssd1306_Fill(Black);
				 							ssd1306_SetCursor(32,32);
				 							ssd1306_WriteString("write",Font_6x8,White);
				 							SPIF_ReadPage(&hspif1,i, (uint8_t *)flashread, 256, 0);
				 							CDC_Transmit_FS((uint8_t * )flashread,256);
				 							percentage((float) (i*100)/pagenumber);
				 							ssd1306_UpdateScreen();
				 							HAL_Delay(125);
				 							i++;
				 						}
				 						SPIF_ReadPage(&hspif1,pagenumber, (uint8_t *)flashread, pageoffset, 0);
				 						CDC_Transmit_FS((uint8_t * )flashread,pageoffset);
				 						HAL_Delay(125);
				 						int taillefin=0;
										taillefin = snprintf((uint8_t *)usbbuffer,64,"kawakobeme\n\r");
				 						CDC_Transmit_FS((uint8_t *)usbbuffer,taillefin);

				 						usbtransmiten=1;
				 						}
				 					else{
				 						usbpercent=1;

				 					}
				 					ssd1306_Fill(Black);
				 					ssd1306_SetCursor(32,45);
				 					ssd1306_WriteString("finish",Font_7x10,White);
				 					}
				 					else{
				 							ssd1306_Fill(Black);
				 							ssd1306_SetCursor(32,40);
				 							ssd1306_WriteString("write",Font_6x8,White);
				 							ssd1306_SetCursor(32,48);
				 							ssd1306_WriteString("nothing",Font_6x8,White);

				 						}




				 					if(BTN_B>=1){
				 									 						usbstate--;
				 									 						usbstate--;
				 									 						  BTN_B=0;
				 									 						  BTN_A=0;
				 									 					  }
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
				  break;
	}
return ;
}






















