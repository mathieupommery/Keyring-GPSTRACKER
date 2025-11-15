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
#include <stdio.h>
#include "usbd_def.h"
#include "usbd_core.h"
#include "spif.h"
#include "spi.h"
#include "tim.h"
#include "bmp581.h"
#include "rtc.h"
#include "GNSS.h"
#include "cmsis_os.h"

extern osThreadId BALISEHandle;
extern osThreadId USBTRANSMITHandle;
extern SPIF_HandleTypeDef hspif1;
extern BMP_t * bmp581;

extern uint8_t bufferscreen[50];
extern uint8_t flashread[2048];
extern STATE_TYPE state;
extern SPEED spdstate;
extern POS posstate;
extern CHRONO chronostate;
extern BALISESTATE balisestate;
extern ECRANBALISESTATE ecranstate;
extern ACCELSTATE accelstate;

extern int BTN_A;
extern int BTN_B;
extern int BTN_B_LONG;
extern int BTN_A_LONG;



extern float vitmax;
extern float seconde;
extern float min;


extern uint32_t starttime;
extern uint32_t calctime;
extern uint32_t timehandler;

extern double bmppress;



extern float temp;
extern float vbat;


extern double distanceparcouru;
extern double oldlat;
extern double oldlong;

extern int enablewrite;

extern double distance1sec;

extern uint8_t SEC;
extern uint8_t HR;
extern uint8_t MINUTE;
extern uint8_t JOURS;
extern uint8_t MOIS;
extern uint16_t ANNEE;
extern int settimeen;



extern double bmpalt;
extern GNSS_StateHandle GNSSData;

int timecounter=0;
int counterforstart=0;
int indexcounterforstart=0;

int flag_50kmh=0;
int flag_100kmh=0;
float time50kmh=0.0;
float time100kmh=0.0;

extern int flag_usb_mounted;

void statemachine(void){
	switch(state){
	 case STATE_SPEED:

		ssd1306_Fill(Black);

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


						ssd1306_SetCursor(32, 32);
						snprintf((char *)bufferscreen,15, "%0.1f",(GNSSData.fgSpeed)*3.6);
						ssd1306_WriteString((char *)bufferscreen, Font_16x24, White);
						ssd1306_SetCursor(32, 56);
						ssd1306_WriteString("Vit(kmh)", Font_6x8, White);
						batterygauge(vbat,79, 57,1);

					 if(BTN_B>=1){
							spdstate++;
							BTN_B=0;


					 					 				  	}




				  break;
				 case STATE_GROS1:

					 ssd1306_SetCursor(32, 32);
					 snprintf((char *)bufferscreen,15, "%0.1f",vitmax*3.6);
					 ssd1306_WriteString((char *)bufferscreen, Font_16x24, White);
					 ssd1306_SetCursor(32, 56);
					 ssd1306_WriteString("maxV", Font_6x8, White);
					 batterygauge(vbat,79, 57,1);





					 if(BTN_B>=1){
					 							spdstate++;
					 							BTN_B=0;


					 					 					 				  	}



					 break;
				  case STATE_SUMMARY:
				  						ssd1306_SetCursor(32, 32);
				  						snprintf((char *)bufferscreen,15, "%0.0fmin%0.0fs",floor(pace),floor(sec));
				  						ssd1306_WriteString((char *)bufferscreen, Font_7x10, White);
				  						ssd1306_SetCursor(32, 42);
				  						ssd1306_WriteString("pace", Font_6x8, White);
				  						ssd1306_SetCursor(32, 56);
				  						snprintf((char *)bufferscreen,15, "V=%0.1fkmh",vitmax*3.6);
				  						ssd1306_WriteString((char *)bufferscreen, Font_6x8, White);
				  						batterygauge(vbat,79, 43,1);



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
				  									 	 if(BTN_B_LONG>=1){
				  									 					  							  vitmax=0;
				  									 					  							BTN_B_LONG=0;
				  									 					  						}
				  									 	 break;


				  case STATE_BALISE:
					  ssd1306_Fill(Black);
					  ssd1306_SetCursor(32, 32);
					  snprintf((char *)bufferscreen,15, "usb: %d",flag_usb_mounted);
					  ssd1306_WriteString((char *)bufferscreen, Font_7x10, White);

					  if(settimeen==0){

						  if(GNSSData.fixType>=1){
							settimeen=1;
							HR=GNSSData.hour;
							MINUTE=GNSSData.min;
							SEC=GNSSData.sec;

						  }
					  }







					  switch(balisestate){
					  case BALISESTATE1:
						  ssd1306_SetCursor(32,32);

						  if(BTN_B_LONG>=1){
							balisestate++;
							BTN_B_LONG=0;
							BTN_A=0;
							oldlat=GNSSData.fLat;
							oldlong=GNSSData.fLon;
							//osThreadResume(BALISEHandle);

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
						  ssd1306_SetCursor(32,32);

						  switch(ecranstate){


						  case ECRANBALISESTATE1:
							  snprintf((char *)bufferscreen,50,"no data");
							  ssd1306_WriteString((char *)bufferscreen,Font_7x10,White);
							  ssd1306_SetCursor(32,42);
							  ssd1306_WriteString("PageNb",Font_6x8,White);


							  if(BTN_B>=1){
														  ecranstate++;
													  	BTN_B=0;
													  	BTN_A=0;
													  	}




							  break;
						  case ECRANBALISESTATE2:

							  snprintf((char  *)bufferscreen,50,"%0.3lf",distanceparcouru/1000);
							  ssd1306_WriteString((char *)bufferscreen,Font_7x10,White);
							  ssd1306_SetCursor(32,42);
							  ssd1306_WriteString("Dist(km)",Font_6x8,White);


							  if(BTN_B>=1){
							  														  ecranstate++;
							  													  	BTN_B=0;
							  													  	BTN_A=0;
							  													  	}
							  break;
						  case ECRANBALISESTATE3:
							  snprintf((char  *)bufferscreen,50,"%0.1f",GNSSData.fgSpeed*3.6);
							  ssd1306_WriteString((char *)bufferscreen,Font_7x10,White);
							  ssd1306_SetCursor(32,42);
							  ssd1306_WriteString("Vit(kmh)",Font_6x8,White);


							  if(BTN_B>=1){
							  														  ecranstate++;
							  													  	BTN_B=0;
							  													  	BTN_A=0;
							  													  	}
							  break;
						  case ECRANBALISESTATE4:
							  snprintf((char  *)bufferscreen,50,"%0.1lf",bmpalt);
							  							  ssd1306_WriteString((char *)bufferscreen,Font_7x10,White);
							  							ssd1306_SetCursor(32,42);
							  							ssd1306_WriteString("alt(m)",Font_6x8,White);


							  if(BTN_B>=1){


							  														  ecranstate++;
							  													  	BTN_B=0;
							  													  	BTN_A=0;
							  													  	}
							  break;
						  case ECRANBALISESTATE5:
							  snprintf((char  *)bufferscreen,50,"%0.1f",vbat);
							  ssd1306_WriteString((char *)bufferscreen,Font_7x10,White);
							  ssd1306_SetCursor(32,42);
							  ssd1306_WriteString("Vbat(V)",Font_6x8,White);


							  if(BTN_B>=1){


							  														  ecranstate++;
							  													  	BTN_B=0;
							  													  	BTN_A=0;
							  													  	}
							  break;
						  case ECRANBALISESTATE6:
							  snprintf((char  *)bufferscreen,50,"%0.1f",vitmax*3.6);
							  							  ssd1306_WriteString((char *)bufferscreen,Font_7x10,White);
							  							ssd1306_SetCursor(32,42);
							  							ssd1306_WriteString("MaxV",Font_6x8,White);


							  if(BTN_B>=1){
							  														ecranstate--;
							  														ecranstate--;
							  														ecranstate--;
							  														ecranstate--;
							  														ecranstate--;
							  													  	BTN_B=0;
							  													  	BTN_A=0;
							  													  	}
							  break;







						  }


						 	batterygauge(vbat,79, 42,1);
						 	ssd1306_SetCursor(32,52);
						 	snprintf((char  *)bufferscreen,50, "%0.2fV",vbat);
						 	ssd1306_WriteString((char *)bufferscreen,Font_6x8,White);
						 	ssd1306_SetCursor(65,52);
						 	snprintf((char  *)bufferscreen,50, "sat=%d",GNSSData.numSV);
						 	ssd1306_WriteString((char *)bufferscreen,Font_6x8,White);

						  if(BTN_B_LONG>=1){
						  						  balisestate--;
						  						  BTN_B_LONG=0;
						  						  BTN_A=0;
						  						//osThreadSuspend(BALISEHandle);
						  						HAL_GPIO_WritePin(GPIOB,GPIO_PIN_1,GPIO_PIN_RESET);
						  					  }


						  break;

					  case BALISESTATE3:
						  ssd1306_SetCursor(32,32);
						  ssd1306_WriteString("fin de",Font_6x8,White);
						  ssd1306_SetCursor(32,42);
						  ssd1306_WriteString("memoire",Font_6x8,White);
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

			  switch(posstate){

			  case STATE_SUMMARY1:

						ssd1306_SetCursor(32, 32);
						snprintf((char *)bufferscreen,15, "Latitude:");
						ssd1306_WriteString((char *)bufferscreen, Font_6x8, White);
						snprintf((char *)bufferscreen,15, "%0.7f",GNSSData.fLat);//pas forcement utile d'afficher 7 decimales apres la virgule, 6 donne une precision au metre ce qui est le max du gps
						ssd1306_SetCursor(32, 40);
						ssd1306_WriteString((char *)bufferscreen, Font_6x8, White);
						snprintf((char *)bufferscreen,15, "Longitude:");
						ssd1306_SetCursor(32, 48);
						ssd1306_WriteString((char *)bufferscreen, Font_6x8, White);
						snprintf((char *)bufferscreen,15, "%0.7f",GNSSData.fLon);
						ssd1306_SetCursor(32, 56);
						ssd1306_WriteString((char *)bufferscreen, Font_6x8, White);
				  if(BTN_B>=1){
					  posstate++;
					  BTN_B=0;
				  }



				  break;
			  case STATE_INFO:
				ssd1306_Fill(Black);

				snprintf((char *)bufferscreen,15, "hacc=%0.2fm",GNSSData.fhACC);//sert a	connaitre la qualitée du fix si proche de 1 voir inférieur alors le fix est tres bon
				ssd1306_SetCursor(32, 32);
				ssd1306_WriteString((char *)bufferscreen, Font_7x10, White);
				snprintf((char *)bufferscreen,20, "v=%0.2fV",vbat);
				ssd1306_SetCursor(32, 42);
				ssd1306_WriteString((char *)bufferscreen, Font_7x10, White);
				ssd1306_SetCursor(32, 52);
				snprintf((char *)bufferscreen,15,  "T=%0.2fC",temp);
				ssd1306_WriteString((char *)bufferscreen, Font_7x10, White);
				 if(BTN_B>=1){
								  					  posstate++;
								  					  BTN_B=0;
								  				  }
								  if(BTN_B_LONG>=1){
													  posstate--;
													BTN_B_LONG=0;
												}

				  break;
			  case STATE_INFO2:
			  				ssd1306_Fill(Black);

			  				snprintf((char *)bufferscreen,15, "Satnum");//sert a	connaitre la qualitée du fix si proche de 1 voir inférieur alors le fix est tres bon
			  				ssd1306_SetCursor(32, 32);
			  				ssd1306_WriteString((char *)bufferscreen, Font_6x8, White);
			  				snprintf((char *)bufferscreen,20, "%d sat",GNSSData.numSV);
			  				ssd1306_SetCursor(32, 40);
			  				ssd1306_WriteString((char *)bufferscreen, Font_6x8, White);
			  				ssd1306_SetCursor(32, 48);
			  				snprintf((char *)bufferscreen,15,  "DateJ/M/Y");
			  				ssd1306_WriteString((char *)bufferscreen, Font_6x8, White);
			  				ssd1306_SetCursor(32, 56);
			  				snprintf((char *)bufferscreen,15,  "%d/%d/%d",GNSSData.day,GNSSData.month,GNSSData.year);
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


				  						ssd1306_SetCursor(32, 32);
				  						snprintf((char *)bufferscreen,15, "altitude:");
				  						ssd1306_WriteString((char *)bufferscreen, Font_6x8, White);
				  						snprintf((char *)bufferscreen,15, "%0.2f m",GNSSData.fhMSL);//pas forcement utile d'afficher 7 decimales apres la virgule, 6 donne une precision au metre ce qui est le max du gps
				  						ssd1306_SetCursor(32, 40);
				  						ssd1306_WriteString((char *)bufferscreen, Font_6x8, White);
				  						snprintf((char *)bufferscreen,15, "Accuracy(m)");
				  						ssd1306_SetCursor(32, 48);
				  						ssd1306_WriteString((char *)bufferscreen, Font_6x8, White);
				  						snprintf((char *)bufferscreen,15, "%0.1f",GNSSData.fvACC);
				  						ssd1306_SetCursor(32, 56);
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
			  case STATE_ALTBARO:
				  						ssd1306_SetCursor(32, 32);
				  						snprintf((char *)bufferscreen,15, "baroalt:");
				  						ssd1306_WriteString((char *)bufferscreen, Font_6x8, White);
				  						snprintf((char *)bufferscreen,15, "%0.2l m",bmp581->altitude);//pas forcement utile d'afficher 7 decimales apres la virgule, 6 donne une precision au metre ce qui est le max du gps
				  						ssd1306_SetCursor(32, 40);
				  						ssd1306_WriteString((char *)bufferscreen, Font_6x8, White);
				  						snprintf((char *)bufferscreen,15, "baropress:");
				  						ssd1306_SetCursor(32, 48);
				  						ssd1306_WriteString((char *)bufferscreen, Font_6x8, White);
				  						snprintf((char *)bufferscreen,15, "%0.3l kPa",bmp581->press/1000.0);
				  						ssd1306_SetCursor(32, 56);
				  						ssd1306_WriteString((char *)bufferscreen, Font_6x8, White);
				  						HAL_Delay(200);

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

			  			  if(settimeen==0){

			  							settimeen=1;
			  							HR=GNSSData.hour;
			  							MINUTE=GNSSData.min;
			  							SEC=GNSSData.sec;
			  							set_time (HR, MINUTE, SEC);




			  					  }

			  			get_time_date();


			  				  ssd1306_SetCursor(32, 32);
			  				  ssd1306_WriteString("hr GMT:", Font_6x8, White);
			  				ssd1306_SetCursor(32, 42);
			  				snprintf((char *)bufferscreen,15, "%02d:%02d",HR,MINUTE);
			  				ssd1306_WriteString((char *)bufferscreen, Font_7x10, White);
			  				ssd1306_SetCursor(32, 52);
			  				snprintf((char *)bufferscreen,15, "%02d sec",SEC);
			  				ssd1306_WriteString((char *)bufferscreen, Font_7x10, White);
			  				if(BTN_B>=1){
			  					posstate--;
			  				//	posstate--;
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
			  ssd1306_SetCursor(32, 32);
			  ssd1306_WriteString("chrono", Font_6x8, White);
			  ssd1306_SetCursor(32,40);

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
			  ssd1306_SetCursor(32, 50);
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
				  case STATE_ACCEL:
					  ssd1306_Fill(Black);
					  ssd1306_SetCursor(32,32);
					  ssd1306_WriteString("0-100",Font_6x8,White);


					  switch(accelstate){
					  case WAITFORGPS:
						  ssd1306_SetCursor(32,40);
						  ssd1306_WriteString("GPS fix",Font_7x10,White);
						  if(GNSSData.fixType>=2){
							  accelstate++;
						  }
						  if(BTN_A>=1){
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
					  case WAITFORPUSH:

						  timecounter++;
						  if(timecounter>=10){
							  timecounter=0;
						  }
						  if(timecounter%2==0){
							  ssd1306_SetCursor(32,40);
							  ssd1306_WriteString("Push B",Font_7x10,White);
						  }
						  if(BTN_B_LONG>=1){
							  accelstate++;
						  				 						BTN_B_LONG=0;

						  				 						}
						  else{
						  timecounter++;

						  if(BTN_A>=1){
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
						  }

						  break;
					  case WAITFORSTOP:
						  if(GNSSData.fgSpeed<=1.0){
							  ssd1306_SetCursor(56,40);

							  snprintf((char *)bufferscreen,15, "%d",3-counterforstart);
							  ssd1306_WriteString((char *)bufferscreen, Font_16x24, White);
							  if(counterforstart==3){
							  				accelstate++;
							  				counterforstart=0;
							  				indexcounterforstart=0;
							  				HAL_TIM_Base_Start(&htim16);
							  				htim16.Instance->CNT=0;
							  }

							  indexcounterforstart++;
							  if(indexcounterforstart%10==0){
								  counterforstart++;
							  }


						  }
						  else{
							  ssd1306_SetCursor(32,40);
							  ssd1306_WriteString("Please stop",Font_6x8,White);
							  counterforstart=0;
							  indexcounterforstart=0;

						  }
						  break;
					  case INRUN:
						  if(((GNSSData.fgSpeed*3.6)>=50.0)&&(flag_50kmh==0)){
							  time50kmh=(float)(htim16.Instance->CNT/1000.0);
							  flag_50kmh=1;
						  }

						  if(((GNSSData.fgSpeed*3.6)>=100.0)&&(flag_100kmh==0)){
							  time100kmh=(float)(htim16.Instance->CNT/1000.0);
							  flag_100kmh=1;
							  accelstate++;
							  HAL_TIM_Base_Stop(&htim16);
							  htim16.Instance->CNT=0;

						 						  }
						  ssd1306_SetCursor(32,40);
						  snprintf((char *)bufferscreen,15, "%0.1f",GNSSData.fgSpeed*3.6);
						  ssd1306_WriteString((char *)bufferscreen,Font_16x24,White);


						  if(BTN_B_LONG>=1){
							  accelstate++;
							  BTN_B_LONG=0;
							  HAL_TIM_Base_Stop(&htim16);
							  htim16.Instance->CNT=0;
						  }


						  break;
					  case RESULT:
						  ssd1306_Fill(Black);
						 ssd1306_SetCursor(32,32);
						 ssd1306_WriteString("0-50kmh",Font_6x8,White);
						 ssd1306_SetCursor(32,40);
						 snprintf((char *)bufferscreen,15, "%0.1fs",time50kmh);
						 ssd1306_WriteString((char *)bufferscreen,Font_6x8,White);
						 ssd1306_SetCursor(32,48);
						 ssd1306_WriteString("0-100kmh",Font_6x8,White);
						 ssd1306_SetCursor(32,56);
						 snprintf((char *)bufferscreen,15, "%0.1fs",time100kmh);
						 ssd1306_WriteString((char *)bufferscreen,Font_6x8,White);
						 if((BTN_A>=1)||(BTN_B>=1)||(BTN_A_LONG>=1)){

							 time50kmh=0.0;
							 time100kmh=0.0;
							 flag_50kmh=0;
							 flag_100kmh=0;
							 accelstate=WAITFORGPS;

						 }


						  if(BTN_A>=1){
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
		 					if(BTN_B>=1){

		 						BTN_B=0;
		 					}
						  break;












					  }



					  break;

	}
return ;
}
