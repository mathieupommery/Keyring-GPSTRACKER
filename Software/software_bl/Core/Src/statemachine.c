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
#include "usbd_cdc_if.h"
#include "spif.h"
#include "spi.h"
#include "tim.h"
#include "PADS.h"
#include "usb.h"




extern USBD_HandleTypeDef hUsbDeviceFS;
extern UART_HandleTypeDef hlpuart1;
extern DMA_HandleTypeDef hdma_lpuart1_rx;
extern SPIF_HandleTypeDef hspif1;
extern TIM_HandleTypeDef htim17;
extern I2C_HandleTypeDef hi2c1;
extern UART_HandleTypeDef huart1;


extern uint8_t DataBuffer[DataBuffer_SIZE];
extern uint8_t flashwrite[256];
extern uint8_t flashread[256];
extern uint8_t usbbuffer[64];
extern uint8_t indexbuffer[50];
extern uint8_t bufferscreen[50];
extern uint8_t longbufferscreen[256];
extern uint16_t offsetforscroltext;
extern uint16_t scrolltextmax;

extern STATE_TYPE state;
extern SPEED spdstate;
extern POS posstate;
extern CHRONO chronostate;
extern GIF gifstate;
extern USBSTATE usbstate;
extern BALISESTATE balisestate;
extern TARVOSSTATE tarvosstate;
extern BAROSTATE barostate;


extern int BTN_A;
extern int BTN_B;
extern int BTN_B_LONG;
extern int BTN_A_LONG;


extern GPS myData;

extern float vitmax;
extern float seconde;
extern float min;


extern uint32_t starttime;
extern uint32_t calctime;
extern uint32_t timehandler;




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
extern int16_t barotemp;
extern int32_t baropress;
extern int baroenableinit;
extern float altibaro;

extern int enablewrite;

extern double distance1sec;

extern uint8_t SEC;
extern uint8_t HR;
extern uint8_t MINUTE;
extern uint8_t JOURS;
extern uint8_t MOIS;
extern uint16_t ANNEE;
extern int settimeen;

extern int inttemp;
extern int intpress;



int gputemp=0;
int gpupower=0;

uint8_t tarvosbuf[50];
int correctentarvos=0;
int i2cerrcheck=0;

char receivedbluetooth;







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

						snprintf((char *)bufferscreen,15, "MaxV=%.1f",vitmax*3.6);
						ssd1306_SetCursor(32, 12);
						ssd1306_WriteString((char *)bufferscreen, Font_6x8, White);
						snprintf((char *)bufferscreen,15, "V=%0.1f",(myData.speed)*3.6);
						ssd1306_SetCursor(32, 20);
						ssd1306_WriteString((char *)bufferscreen, Font_6x8, White);
						snprintf((char *)bufferscreen,15, "p=%0.0fmin%0.0f s",floor(pace),floor(sec));
						ssd1306_SetCursor(32, 28);
						ssd1306_WriteString((char *)bufferscreen, Font_6x8, White);



					 if(BTN_B>=1){
								spdstate++;
								BTN_B=0;
					 					 				  	}
					 if(BTN_B_LONG>=1){
							  vitmax=0;
							BTN_B_LONG=0;
						}



					 break;
				 case STATE_GROS:


						ssd1306_SetCursor(32, 12);
						snprintf((char *)bufferscreen,15, "%0.1f",(myData.speed)*3.6);
						ssd1306_WriteString((char *)bufferscreen, Font_11x18, White);
						batterygauge(vbat,35, 34,1);

					 if(BTN_B>=1){
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
				 	 if(BTN_A_LONG>=1){
				 						 				 									state++;
				 						 				 									state++;
				 						 				 									state++;
				 						 				 									state++;
				 						 				 									state++;

				 						 				 									  			 	BTN_A_LONG=0;
				 						 				 									  			 	BTN_B=0;
				 						 				 									  	}
				  break;

				  case STATE_BALISE:
					  ssd1306_Fill(Black);

					  if(settimeen==0){

						  if(myData.fix==1){
							settimeen=1;
							HR=(myData.lastMeasure[0]&0x0f)*10+(myData.lastMeasure[1]&0x0f);
							MINUTE=(myData.lastMeasure[2]&0x0f)*10+(myData.lastMeasure[3]&0x0f);
							SEC=(myData.lastMeasure[4]&0x0f)*10+(myData.lastMeasure[5]&0x0f);

						  }
					  }







					  switch(balisestate){
					  case BALISESTATE1:
						  ssd1306_SetCursor(32,12);
						  ssd1306_WriteString("Tracker",Font_6x8,White);
						  snprintf((char *)bufferscreen,50, "offset=%d",pageoffset);
						  ssd1306_SetCursor(32,22);
						  ssd1306_WriteString((char *)bufferscreen,Font_7x10,White);
						  snprintf((char *)bufferscreen,50, "page=%d",pagenumber);
						  ssd1306_SetCursor(32,32);
						  ssd1306_WriteString((char *)bufferscreen,Font_7x10,White);

						  if(BTN_B>=1){
							balisestate++;
							BTN_B=0;
							BTN_A=0;
							HAL_TIM_Base_Start_IT(&htim17);
						  }
						  if(BTN_A>=1){
						  				  			 	state++;
						  				  			 	BTN_A=0;
						  				  			 	BTN_B=0;
						  				  			 	settimeen=0;


						  				  	}
						  if(BTN_A_LONG>=1){
									  state--;
									BTN_A_LONG=0;
									BTN_B=0;
									settimeen=0;
						  					}

						  break;
					  case BALISESTATE2:

						  erasetime=HAL_GetTick();



						  if(pagenumber+1>=MAX_WRITE_PAGE){
							  HAL_TIM_Base_Stop_IT(&htim17);
							  balisestate=2;

						  }

						  if(enablewrite==1){

							  oldlat=myData.latitude;
							  oldlong=myData.longitude;
							  nmea_parse(&myData, DataBuffer);
							  if(distancecalc(oldlat, myData.latitude,oldlong, myData.longitude) > 139.0){//correspond à 500kmh pendant 1sec
								  myData.latitude=oldlat;
								  myData.longitude=oldlong;
								  flashbufferlen=csvframe((uint8_t *)flashwrite,temp,vbat,&myData,myData.satelliteCount,myData.hdop);
								  writebuffertoflash((uint8_t*)flashwrite,flashbufferlen);

							  }

							  else{
								  distanceparcouru=distanceparcouru + distancecalc(oldlat, myData.latitude,oldlong, myData.longitude);
								  flashbufferlen=csvframe((uint8_t *)flashwrite,temp,vbat,&myData,myData.satelliteCount,myData.hdop);
								  writebuffertoflash((uint8_t*)flashwrite,flashbufferlen);
							  }
							  enablewrite=0;

						  }

						  if(doubledonnee==0){
						 	ssd1306_SetCursor(32,12);
						 	snprintf((char *)bufferscreen,50,"p=%d",pagenumber);
						 	ssd1306_WriteString((char *)bufferscreen,Font_7x10,White);
						 	}
						  else{
						 	ssd1306_SetCursor(32,12);
						 	snprintf((char  *)bufferscreen,50,"d=%0.1lfm",distanceparcouru);
						 	ssd1306_WriteString((char *)bufferscreen,Font_7x10,White);
						 	}
						 	ssd1306_SetCursor(32,22);
						 	snprintf((char  *)bufferscreen,50, "sat=%d",myData.satelliteCount);
						 	ssd1306_WriteString((char *)bufferscreen,Font_6x8,White);
						 	batterygauge(vbat,34, 32,1);
						 	ssd1306_SetCursor(55,32);
						 	snprintf((char  *)bufferscreen,50, "%0.2fV",vbat);
						 	//ssd1306_WriteString((uint8_t*)bufferscreen,Font_6x8,White);

						 	if(cptdoubledonnee==3){
						 	  		doubledonnee=1-doubledonnee;
						 	  		cptdoubledonnee=0;
						 	  	 }
						 	erasetime=HAL_GetTick()-erasetime;
	//					 	if(maxtesttime<=erasetime){
	//					 		maxtesttime=erasetime;
	//					 	}
	//					 	snprintf((char  *)bufferscreen,50,"%d",maxtesttime);
	//					 	ssd1306_WriteString((char *)bufferscreen,Font_6x8,White);







						  if(BTN_B>=1){
						  						  balisestate--;
						  						  BTN_B=0;
						  						  BTN_A=0;
						  						  HAL_TIM_Base_Stop_IT(&htim17);
						  					  }
						  if(BTN_B_LONG>=1){
							distanceparcouru=0;
						  	BTN_B_LONG=0;
						  				}

						  break;

					  case BALISESTATE3:
						  ssd1306_SetCursor(32,12);
						  ssd1306_WriteString("fin de",Font_6x8,White);
						  ssd1306_SetCursor(32,20);
						  ssd1306_WriteString("memoire",Font_6x8,White);
						  ssd1306_SetCursor(32,28);
						  snprintf((char *)bufferscreen,50,"%d,%d",pageoffset,pagenumber);
						  ssd1306_WriteString((char *)bufferscreen,Font_6x8,White);
						  if(BTN_A>=1){
						  				  			 	state++;
						  				  			 	BTN_A=0;
						  				  			 	BTN_B=0;
						  				  			settimeen=0;


						  				  	}
						  if(BTN_A_LONG>=1){
						  				 									 									  			 	state--;
						  				 									 									  			 	BTN_A=0;
						  				 									 									  			 	BTN_B=0;
						  				 									 									  			 	BTN_A_LONG=0;
						  				 									 									  	}

					  }


					  break;

	case STATE_POS:
			  ssd1306_Fill(Black);
			  nmea_parse(&myData, DataBuffer);
			  switch(posstate){

			  case STATE_SUMMARY1:

						ssd1306_SetCursor(32, 12);
						snprintf((char *)bufferscreen,15, "Latitude:");
						ssd1306_WriteString((char *)bufferscreen, Font_6x8, White);
						snprintf((char *)bufferscreen,15, "%0.7f",myData.latitude);//pas forcement utile d'afficher 7 decimales apres la virgule, 6 donne une precision au metre ce qui est le max du gps
						ssd1306_SetCursor(32, 20);
						ssd1306_WriteString((char *)bufferscreen, Font_6x8, White);
						snprintf((char *)bufferscreen,15, "Longitude:");
						ssd1306_SetCursor(32, 28);
						ssd1306_WriteString((char *)bufferscreen, Font_6x8, White);
						snprintf((char *)bufferscreen,15, "%0.7f",myData.longitude);
						ssd1306_SetCursor(32, 36);
						ssd1306_WriteString((char *)bufferscreen, Font_6x8, White);
				  if(BTN_B>=1){
					  posstate++;
					  BTN_B=0;
				  }



				  break;
			  case STATE_INFO:
				  ssd1306_Fill(Black);
				  nmea_parse(&myData, DataBuffer);
				snprintf((char *)bufferscreen,15, "hdop=%.1f",myData.hdop);//sert a	connaitre la qualitée du fix si proche de 1 voir inférieur alors le fix est tres bon
				ssd1306_SetCursor(32, 12);
				ssd1306_WriteString((char *)bufferscreen, Font_6x8, White);
				snprintf((char *)bufferscreen,20, "v=%0.2fV",vbat);
				ssd1306_SetCursor(32, 20);
				ssd1306_WriteString((char *)bufferscreen, Font_6x8, White);
				ssd1306_SetCursor(32, 28);
				snprintf((char *)bufferscreen,15,  "T=%0.2fC",temp);
				ssd1306_WriteString((char *)bufferscreen, Font_6x8, White);
				 if(BTN_B>=1){
								  					  posstate++;
								  					  BTN_B=0;
								  				  }
								  if(BTN_B_LONG>=1){
													  posstate--;
													BTN_B_LONG=0;
												}

				  break;
			  case STATE_LAT:
						ssd1306_SetCursor(32, 12);
						snprintf((char *)bufferscreen,15, "LatSide:");
						ssd1306_WriteString((char *)bufferscreen, Font_6x8, White);
						snprintf((char *)bufferscreen,15, "%c",myData.latSide);//pas forcement utile d'afficher 7 decimales apres la virgule, 6 donne une precision au metre ce qui est le max du gps
						ssd1306_SetCursor(32, 20);
						ssd1306_WriteString((char *)bufferscreen, Font_6x8, White);
						snprintf((char *)bufferscreen,15, "Latitude:");
						ssd1306_SetCursor(32, 28);
						ssd1306_WriteString((char *)bufferscreen, Font_6x8, White);
						snprintf((char *)bufferscreen,15, "%0.7f",myData.latitude);
						ssd1306_SetCursor(32, 36);
						ssd1306_WriteString((char *)bufferscreen, Font_6x8, White);


				  if(BTN_B>=1){
				  					  posstate++;
				  					  BTN_B=0;
				  				  }
				  if(BTN_B_LONG>=1){
									  posstate--;
									BTN_B_LONG=0;
								}



				  break;
			  case STATE_LONG:

					ssd1306_SetCursor(32, 12);
					snprintf((char *)bufferscreen,15, "LonSide:");
					ssd1306_WriteString((char *)bufferscreen, Font_6x8, White);
					snprintf((char *)bufferscreen,15, "%c",myData.lonSide);//pas forcement utile d'afficher 7 decimales apres la virgule, 6 donne une precision au metre ce qui est le max du gps
					ssd1306_SetCursor(32, 20);
					ssd1306_WriteString((char *)bufferscreen, Font_6x8, White);
					snprintf((char *)bufferscreen,15, "Longitude:");
					ssd1306_SetCursor(32, 28);
					ssd1306_WriteString((char *)bufferscreen, Font_6x8, White);
					snprintf((char *)bufferscreen,15, "%0.7f",myData.longitude);
					ssd1306_SetCursor(32, 36);
					ssd1306_WriteString((char *)bufferscreen, Font_6x8, White);

				  if(BTN_B>=1){
				  					  posstate++;
				  					  BTN_B=0;
				  				  }
				  if(BTN_B_LONG>=1){
									  posstate--;
									BTN_B_LONG=0;
								}

				  break;
			  case STATE_ALT:


				  						ssd1306_SetCursor(32, 12);
				  						snprintf((char *)bufferscreen,15, "altitude:");
				  						ssd1306_WriteString((char *)bufferscreen, Font_6x8, White);
				  						snprintf((char *)bufferscreen,15, "%0.1f m",myData.altitude);//pas forcement utile d'afficher 7 decimales apres la virgule, 6 donne une precision au metre ce qui est le max du gps
				  						ssd1306_SetCursor(32, 20);
				  						ssd1306_WriteString((char *)bufferscreen, Font_6x8, White);
				  						snprintf((char *)bufferscreen,15, "Pressure:");
				  						ssd1306_SetCursor(32, 28);
				  						ssd1306_WriteString((char *)bufferscreen, Font_6x8, White);
				  						snprintf((char *)bufferscreen,15, "%0.1fhpa",1000*expf((-0.0001148)*(myData.altitude)));
				  						ssd1306_SetCursor(32, 36);
				  						ssd1306_WriteString((char *)bufferscreen, Font_6x8, White);

				  if(BTN_B>=1){
				  					posstate++;
				  					BTN_B=0;
				  				  }
				  if(BTN_B_LONG>=1){
					  posstate--;
					BTN_B_LONG=0;
				}
				  break;
			  case STATE_HEURE:
			  			  ssd1306_Fill(Black);
			  			  nmea_parse(&myData, DataBuffer);
			  			  if(settimeen==0){

			  							settimeen=1;
			  							HR=(myData.lastMeasure[0]&0x0f)*10+(myData.lastMeasure[1]&0x0f);
			  							MINUTE=(myData.lastMeasure[2]&0x0f)*10+(myData.lastMeasure[3]&0x0f);
			  							SEC=(myData.lastMeasure[4]&0x0f)*10+(myData.lastMeasure[5]&0x0f);



			  					  }


			  				  ssd1306_SetCursor(32, 12);
			  				  ssd1306_WriteString("hr GMT:", Font_6x8, White);
			  				ssd1306_SetCursor(32, 22);
			  				snprintf((char *)bufferscreen,15, "%02d:%02d",HR,MINUTE);
			  				ssd1306_WriteString((char *)bufferscreen, Font_7x10, White);
			  				ssd1306_SetCursor(32, 32);
			  				snprintf((char *)bufferscreen,15, "%02d sec",SEC);
			  				ssd1306_WriteString((char *)bufferscreen, Font_6x8, White);
			  				if(BTN_B>=1){
			  					posstate--;
			  					posstate--;
			  					posstate--;
			  					posstate--;
			  					posstate--;
			  					BTN_B=0;
			  				}
			  				if(BTN_B_LONG>=1){
			  							  					posstate--;
			  							  					BTN_B_LONG=0;
			  							  				}
			  			break;
			  }

					if(BTN_A>=1){
							state++;
							BTN_A=0;
							BTN_B=0;


						}
					if(BTN_A_LONG>=1){
									 									 									  			 	state--;
									 									 									  			 	BTN_A=0;
									 									 									  			 	BTN_B=0;
									 									 									  			 	BTN_A_LONG=0;
									 									 									  	}
			  break;
		  case STATE_CHRONOMETER:

			  ssd1306_Fill(Black);
			  ssd1306_SetCursor(32, 12);
			  ssd1306_WriteString("chrono", Font_6x8, White);
			  ssd1306_SetCursor(32, 32);

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
				  calctime=uwTick-starttime+timehandler;

				  if(BTN_B>=1){
				  		chronostate++;
				  		BTN_B=0;
			  }


				  break;
			  case STATE_PAUSE:
				  timehandler=calctime;

				  if(BTN_B>=1){
				  			chronostate--;
				  			BTN_B=0;
				  			starttime=uwTick;

			 }
				  if(BTN_B_LONG>=1){
				  				  	chronostate--;
				  				  	chronostate--;
				  				  	BTN_B_LONG=0;
				  				  	timehandler=0;
				  			 }


				  break;
			  }

			  min=floor((float) calctime/60000);
			  seconde=(float) ((calctime-(min*60000))/1000);
			  snprintf((char *)bufferscreen,15, "%0.0fmin",min);
			  ssd1306_WriteString((char *)bufferscreen, Font_7x10, White);
			  ssd1306_SetCursor(32, 44);
			  snprintf((char *)bufferscreen,15, "%0.3fs",seconde);
			  ssd1306_WriteString((char *)bufferscreen, Font_7x10, White);


			  if(BTN_A>=1){
			 	state++;
			 	BTN_A=0;
			 	BTN_B=0;


	}
			  if(BTN_A_LONG>=1){
			  				 									 									  			 	state--;
			  				 									 									  			 	BTN_A=0;
			  				 									 									  			 	BTN_B=0;
			  				 									 									  			 	BTN_A_LONG=0;
			  				 									 									  	}
			  break;




			  case STATE_USB:
				  ssd1306_Fill(Black);
				  ssd1306_SetCursor(32,12);
				  ssd1306_WriteString("usb",Font_6x8,White);
				  switch(usbstate){
				 				  case USBSTATE1:
				 					  ssd1306_SetCursor(32,20);
				 					  ssd1306_WriteString("do nothing",Font_6x8,White);
				 					  usbtransmiten=0;

				 					  if(BTN_B>=1){
				 						  usbstate++;
				 						  BTN_B=0;
				 						  BTN_A=0;
				 					  }
				 					 if(BTN_A>=1){
				 						 state++;

				 									  			 	BTN_A=0;
				 									  			 	BTN_B=0;


				 									  	}
				 					if(BTN_A_LONG>=1){
				 									 									 									  			 	state--;
				 									 									 									  			 	BTN_A=0;
				 									 									 									  			 	BTN_B=0;
				 									 									 									  			 	BTN_A_LONG=0;
				 									 									 									  	}



				 					  break;
				 				  case USBSTATE2:

				 					 ssd1306_SetCursor(32,20);
				 					ssd1306_WriteString("Push A",Font_6x8,White);
				 					ssd1306_SetCursor(32,28);
				 					ssd1306_WriteString("to erase",Font_6x8,White);

				 					if(erasedisplay==1){
					 					 ssd1306_Fill(Black);
					 					 ssd1306_SetCursor(32,12);
					 					ssd1306_WriteString("usb",Font_6x8,White);
					 					ssd1306_SetCursor(32,20);
					 					ssd1306_WriteString("FIN",Font_7x10,White);
					 					ssd1306_SetCursor(32,30);
				 						snprintf((char  *)bufferscreen,50,"t=%0.2f",(float)erasetime/1000);
				 						ssd1306_WriteString((char *)bufferscreen,Font_6x8,White);
				 					}


				 					if(BTN_A>=1){


				 						erasetime=HAL_GetTick();
				 						for(int i=0;i<=(int)floor((pagenumber)/16);i++){
				 							ssd1306_Fill(Black);
				 							ssd1306_SetCursor(32,32);
				 							ssd1306_WriteString("usb",Font_6x8,White);
				 							SPIF_EraseSector(&hspif1,i);
				 							if(i>=1){
				 							percentage((float) (i*100)/((int)floor((pagenumber)/16)));
				 							}
				 							else{
				 								percentage((float) 0);
				 							}
				 							ssd1306_UpdateScreen();

				 						}
				 						SPIF_EraseSector(&hspif1, (int)floor((MAX_WRITE_PAGE+1)/16));
				 						//SPIF_EraseChip(&hspif1);
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
				 					ssd1306_SetCursor(32,20);
				 					ssd1306_WriteString("write",Font_6x8,White);


				 					if(pagenumber>0){


				 					int i=0;
				 					if(usbtransmiten==0){
				 						while(i<pagenumber){
				 							ssd1306_Fill(Black);
				 							ssd1306_SetCursor(32,12);
				 							ssd1306_WriteString("write",Font_6x8,White);
				 							SPIF_ReadPage(&hspif1,i, (uint8_t  *)flashread, 256, 0);
				 							CDC_Transmit_FS((char  * )flashread,256);
				 							percentage((float) (i*100)/pagenumber);
				 							ssd1306_UpdateScreen();
				 							HAL_Delay(125);
				 							i++;
				 						}
				 						SPIF_ReadPage(&hspif1,pagenumber, (uint8_t   *)flashread, pageoffset, 0);
				 						CDC_Transmit_FS((uint8_t   * )flashread,pageoffset);
				 						HAL_Delay(125);
				 						int taillefin=0;
										taillefin = snprintf((char  *)usbbuffer,64,"findetrame\n\r");
				 						CDC_Transmit_FS((char  *)usbbuffer,taillefin);

				 						usbtransmiten=1;
				 						}
				 					else{
				 						usbpercent=1;

				 					}
				 					ssd1306_Fill(Black);
				 					ssd1306_SetCursor(32,35);
				 					ssd1306_WriteString("finish",Font_7x10,White);
				 					}
				 					else{
				 							ssd1306_Fill(Black);
				 							ssd1306_SetCursor(32,20);
				 							ssd1306_WriteString("write",Font_6x8,White);
				 							ssd1306_SetCursor(32,28);
				 							ssd1306_WriteString("nothing",Font_6x8,White);

				 						}




				 					if(BTN_B>=1){
				 									 						usbstate--;
				 									 						usbstate--;
				 									 						  BTN_B=0;
				 									 						  BTN_A=0;
				 									 					  }
				 					 if(BTN_A>=1){
				 									  			 	state++;
				 									  			 	BTN_A=0;
				 									  			 	BTN_B=0;
				 									  	}
				 					if(BTN_A_LONG>=1){
				 									 									  			 	state--;
				 									 									  			 	BTN_A=0;
				 									 									  			 	BTN_B=0;
				 									 									  			 	BTN_A_LONG=0;
				 									 									  	}
				 					 break;
				 				  }
				  break;

				  case STATE_BLUETOOTH:
					  ssd1306_Fill(Black);
					  ssd1306_SetCursor(32,12);
					  ssd1306_WriteString("bluetooth",Font_6x8,White);
					  ssd1306_SetCursor(32,20);

					  ssd1306_WriteChar(receivedbluetooth, Font_7x10, White);


				  if(BTN_A>=1){
						state--;
						state--;
						state--;
						state--;
						state--;
						BTN_A=0;
						BTN_B=0;
							}
					if(BTN_A_LONG>=1){
							state--;
							BTN_A=0;
							BTN_B=0;
							BTN_A_LONG=0;
							}



					  break;




	}
return ;
}






//					  oldlat=myData.latitude;
//					  oldlong=myData.longitude;
//					  nmea_parse(&myData, DataBuffer);
//					  if(distancecalc(oldlat, myData.latitude,oldlong, myData.longitude)>= (10*myData.speed/7.2)){
//						  myData.latitude=oldlat;
//						  myData.longitude=oldlong;
//					  }
//					  if(pagenumber+1<MAX_WRITE_PAGE){
//
//					  flashbufferlen=csvframe((uint8_t *)flashwrite,temp,vbat,&myData,myData.satelliteCount,myData.hdop);
//					  writebuffertoflash((uint8_t*)flashwrite,flashbufferlen);
//					  if(doubledonnee==0){
//						  ssd1306_SetCursor(32,32);
//						  snprintf((uint8_t *)bufferscreen,50,"p=%d",pagenumber);
//						  ssd1306_WriteString((uint8_t*)bufferscreen,Font_7x10,White);
//					  }
//					  else{
//						  ssd1306_SetCursor(32,32);
//						  snprintf((uint8_t *)bufferscreen,50,"d=%0.1lfm",*distanceparcouru);
//						  ssd1306_WriteString((uint8_t*)bufferscreen,Font_7x10,White);
//					  }
//					  ssd1306_SetCursor(32,42);
//					  snprintf((uint8_t *)bufferscreen,50, "sat=%d",myData.satelliteCount);
//					  ssd1306_WriteString((uint8_t*)bufferscreen,Font_6x8,White);
//					  batterygauge(vbat,34, 50,1);
//					  ssd1306_SetCursor(60,50);
//					  snprintf((uint8_t *)bufferscreen,50, "%0.2fV",vbat);
//					  ssd1306_WriteString((uint8_t*)bufferscreen,Font_6x8,White);
//					  if(cptdoubledonnee==3){
//						  doubledonnee=1-doubledonnee;
//						  cptdoubledonnee=0;
//					  }
//					  cptdoubledonnee+=1;
//					  HAL_Delay(1000);

	////				  if(baroenableinit==0){
	////					  PADS_init();
	////					  baroenableinit=1;
	////				  }
	//				  BTN_B=0;
	//				  }
	////				  ssd1306_Fill(Black);
	////				  ssd1306_SetCursor(32,32);
	////				  ssd1306_WriteString("baro",Font_6x8,White);
	////				  PADS_getPressure_int(&pads, &baropress);
	////				  PADS_getTemperature_int(&pads,&barotemp);
	////				  ssd1306_SetCursor(32,40);
	////				  snprintf((uint8_t *)bufferscreen,50,"P=%d",baropress);
	////				  ssd1306_WriteString((uint8_t *)bufferscreen,Font_6x8,White);
	////				  ssd1306_SetCursor(32,48);
	////				  snprintf((uint8_t *)bufferscreen,50,"T=%d",barotemp);
	////				  ssd1306_WriteString((uint8_t *)bufferscreen,Font_6x8,White);
	////				  altibaro=altitudecalc(0);
	////				  ssd1306_SetCursor(32,56);
	////				  snprintf((uint8_t *)bufferscreen,50,"h=%0.1fm",(float) altibaro);
	////				  ssd1306_WriteString((uint8_t *)bufferscreen,Font_6x8,White);


// case STATE_MONITOR:
//					  ssd1306_Fill(Black);
//					  ssd1306_SetCursor(32,12);
//					  ssd1306_WriteString("gputemp",Font_6x8,White);
//
//					  usbbuffer[60]='$';
//					  getgpu((uint8_t  *)usbbuffer);
//					  ssd1306_SetCursor(32,20);
//					  snprintf((char  *)bufferscreen,50,"T=%d*C",gputemp);
//					  ssd1306_WriteString((char  *)bufferscreen,Font_7x10,White);
//					  ssd1306_SetCursor(32,30);
//					  snprintf((char  *)bufferscreen,50,"p=%dW",(int) gpupower);
//					  ssd1306_WriteString((char  *)bufferscreen,Font_7x10,White);
//
//
//					  if(BTN_A>=1){
//					 				 									  			 	state++;
//					 				 									  			 	BTN_A=0;
//					 				 									  			 	BTN_B=0;
//					 				 									  	}
//					 				 					if(BTN_A_LONG>=1){
//					 				 									 									  			 	state--;
//					 				 									 									  			 	BTN_A=0;
//					 				 									 									  			 	BTN_B=0;
//					 				 									 									  			 	BTN_A_LONG=0;
//					 				 									 									  	}
//
//
//					  break;
//
//
//				  case STATE_TARVOS:
//					  ssd1306_Fill(Black);
//					  				  ssd1306_SetCursor(32,12);
//					  				  ssd1306_WriteString("tarvos",Font_6x8,White);
//					  				  switch(tarvosstate){
//					  				 				  case TARVOS1:
//					  				 					  ssd1306_SetCursor(32,20);
//					  				 					  ssd1306_WriteString("push B",Font_6x8,White);
//
//
//					  									  if(BTN_A>=1){
//					  											state++;
//					  											BTN_A=0;
//					  											BTN_B=0;
//					  									  }
//					  									if(BTN_A_LONG>=1){
//					  											state--;
//					  											BTN_A=0;
//					  											BTN_B=0;
//					  											BTN_A_LONG=0;
//					  									}
//
//
//
//
//
//
//
//
//					  				 					  if(BTN_B_LONG>=1){
//
//					  				 						 huart1.Instance = USART1;
//					  				 						 huart1.Init.BaudRate = 115200;
//					  				 						 huart1.Init.WordLength = UART_WORDLENGTH_8B;
//					  				 						 huart1.Init.StopBits = UART_STOPBITS_1;
//					  				 						 huart1.Init.Parity = UART_PARITY_NONE;
//					  				 						 huart1.Init.Mode = UART_MODE_TX_RX;
//					  				 						 huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
//					  				 						 huart1.Init.OverSampling = UART_OVERSAMPLING_16;
//					  				 						 huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
//					  				 						 huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
//					  				 						  if (HAL_UART_Init(&huart1) != HAL_OK)
//					  				 						  {
//					  				 						ssd1306_Fill(Black);
//					  				 						 ssd1306_WriteString("problem",Font_6x8,White);
//					  				 						ssd1306_UpdateScreen();
//					  				 						HAL_Delay(1500);
//					  				 						correctentarvos=0;
//
//					  				 						  }
//					  				 						  else{
//					  				 							correctentarvos=1;
//					  				 						  }
//					  				 						  HAL_Delay(10);
//					  				 						  tarvosstate++;
//					  				 						BTN_B_LONG=0;
//					  				 						BTN_A=0;
//					  				 					  }
//
//
//
//					  				 					  break;
//					  				 				  case TARVOS2:
//
//					  				 					 ssd1306_SetCursor(32,20);
//					  				 					ssd1306_WriteString("send1",Font_6x8,White);
//
//					  				 					if(correctentarvos==1){
//
//					  				 					snprintf((char  *)tarvosbuf,50,"cputemp=%0.2f\n\r",temp);
//					  				 					HAL_UART_Transmit(&huart1,(uint8_t  *)tarvosbuf,sizeof(tarvosbuf),HAL_MAX_DELAY);
//					  				 					HAL_Delay(1000);
//
//
//					  				 					}
//					  				 					if(BTN_B>=1){
//
//					  					 						tarvosstate++;
//					  					 						  BTN_B=0;
//					  					 						  BTN_A=0;
//					  					 					  }
//
//					  				 			  break;
//
//					  				 				 case TARVOS3:
//
//					  				 					ssd1306_Fill(Black);
//					  				 					ssd1306_SetCursor(32,12);
//					  				 					ssd1306_WriteString("send2",Font_6x8,White);
//
//					  				 					if(correctentarvos==1){
//
//					  				 						snprintf((char  *)tarvosbuf,50,"je suis à =%0.2f\n\r",vbat);
//					  				 						HAL_UART_Transmit(&huart1,(uint8_t  *)tarvosbuf,sizeof(tarvosbuf),HAL_MAX_DELAY);
//					  				 						HAL_Delay(1000);
//
//
//
//
//					  				 					}
//
//
//
//
//					  				 					if(BTN_B>=1){
//					  				 									 						tarvosstate--;
//					  				 									 						  BTN_B=0;
//					  				 									 						  BTN_A=0;
//					  				 									 					  }
//					  				 					if(BTN_B_LONG>=1){
//					  				 						HAL_UART_Abort(&huart1);
//					  				 						HAL_UART_DeInit(&huart1);
//					  				 										  				 									 						tarvosstate--;
//					  				 										  				 									 					tarvosstate--;
//					  				 										  				 									 						  BTN_B_LONG=0;
//					  				 										  				 									 						  BTN_B=0;
//					  				 										  				 									 						  BTN_A=0;
//					  				 										  				 									 					  }
//
//					  				 					 break;
//					  				 				  }
//
//
//
//
//
//
//
//					  break;
//
//
//
//					  				 case STATE_BARO:
//					  									  ssd1306_Fill(Black);
//					  									  				  ssd1306_SetCursor(32,12);
//					  									  				  ssd1306_WriteString("baro",Font_6x8,White);
//					  									  				  switch(barostate){
//					  									  				 				  case BARO1:
//					  									  				 					  ssd1306_SetCursor(32,20);
//					  									  				 					  ssd1306_WriteString("push B",Font_6x8,White);
//
//
//					  									  									  if(BTN_A>=1){
//					  									  											state++;
//					  									  											BTN_A=0;
//					  									  											BTN_B=0;
//					  									  									  }
//					  									  									if(BTN_A_LONG>=1){
//					  									  											state--;
//					  									  											BTN_A=0;
//					  									  											BTN_B=0;
//					  									  											BTN_A_LONG=0;
//					  									  									}
//
//
//
//
//
//
//
//
//					  									  				 					  if(BTN_B_LONG>=1){
//
//					  									  				 					 hi2c1.Instance = I2C1;
//					  									  				 					  hi2c1.Init.Timing = 0x00202538;
//					  									  				 					  hi2c1.Init.OwnAddress1 = 0;
//					  									  				 					  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
//					  									  				 					  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
//					  									  				 					  hi2c1.Init.OwnAddress2 = 0;
//					  									  				 					  hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
//					  									  				 					  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
//					  									  				 					  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
//					  									  				 					  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
//					  									  				 					  {
//					  									  				 						i2cerrcheck=1;
//					  									  				 					  }
//					  									  				 					  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
//					  									  				 					  {
//					  									  				 						i2cerrcheck=1;
//					  									  				 					  }
//
//					  									  				 					  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK)
//					  									  				 					  {
//					  									  				 						i2cerrcheck=1;
//					  									  				 					  }
//
//					  									  				 					  if(i2cerrcheck>=1){
//					  									  				 						ssd1306_Fill(Black);
//					  									  				 						 ssd1306_WriteString("problem",Font_6x8,White);
//					  									  				 						ssd1306_UpdateScreen();
//					  									  				 						HAL_Delay(1500);
//
//					  									  				 						  }
//					  									  				 					  else{
//					  									  				 						HAL_Delay(10);
//					  									  				 						  barostate++;
//					  									  				 						PADS_continuous_init(&hi2c1);
//
//					  									  				 					  }
//					  									  				 					BTN_B_LONG=0;
//					  									  				 					BTN_A=0;
//
//					  									  				 					  }
//
//
//
//					  									  				 					  break;
//					  									  				 				  case BARO2:
//
//					  									  				 					 ssd1306_SetCursor(32,20);
//					  									  				 					ssd1306_WriteString("read",Font_6x8,White);
//
//
//
//
//
//					  									  				 					if(i2cerrcheck==0){
//
//					  									  				 					PADS_continuous_read(&hi2c1);
//
//					  									  				 				ssd1306_SetCursor(32,28);
//
//					  									  				 			snprintf((char  *)bufferscreen,50,"t=%d",inttemp);
//					  									  				 			ssd1306_WriteString((char  *)bufferscreen,Font_6x8,White);
//					  									  				 			snprintf((char  *)bufferscreen,50,"p=%d",intpress);
//					  									  				 		ssd1306_SetCursor(32,36);
//					  									  				 	ssd1306_WriteString((char  *)bufferscreen,Font_6x8,White);
//					  									  				 	HAL_Delay(500);
//
//
//					  									  				 					}
//					  									  				 					if(BTN_B_LONG>=1){
//
//					  									  				 					HAL_I2C_DeInit(&hi2c1);
//
//					  									  					 						barostate--;
//					  									  					 						  BTN_B_LONG=0;
//					  									  					 						  BTN_A=0;
//
//					  									  					 					  }
//
//					  									  				 			  break;
//
//
//					  									  				 				  }
//
//					  									  break;
//
//
//				  case STATE_TEST:
//					  ssd1306_Fill(Black);
//					  ssd1306_SetCursor(32,12);
//					  ssd1306_WriteString("test",Font_6x8,White);
//					  memcpy((char  *)longbufferscreen,"ceci est un text plutot long, qui ne s'arrette toujours pas et pourtant il s'affiche complet",sizeof("ceci est un text plutot long, qui ne s'arrette toujours pas et pourtant il s'affiche complet"));
//					  scrolltextmax=scrollText(longbufferscreen,Font_7x10,32,20,32,offsetforscroltext);
//					  ssd1306_SetCursor(32,35);
//					  snprintf((char *)bufferscreen,50,"%d",offsetforscroltext);
//					  ssd1306_WriteString((char *)bufferscreen,Font_6x8,White);
//
//
//					  if(offsetforscroltext>=scrolltextmax+16){
//					  	  offsetforscroltext=0;
//					                }
//
//
//
//
//
//
//					  if(BTN_A>=1){
//					 				 									state--;
//					 				 									state--;
//					 				 									state--;
//					 				 									state--;
//					 				 									state--;
//					 				 									state--;
//					 				 									state--;
//					 				 									state--;
//					 				 									state--;
//					 				 									state--;
//					 				 									  			 	BTN_A=0;
//					 				 									  			 	BTN_B=0;
//					 				 									  	}
//					  if(BTN_A_LONG>=1){
//					  				 									 									  			 	state--;
//					  				 									 									  			 	BTN_A=0;
//					  				 									 									  			 	BTN_B=0;
//					  				 									 									  			 	BTN_A_LONG=0;
//					  				 									 									  	}
//
//
//
//					  break;











