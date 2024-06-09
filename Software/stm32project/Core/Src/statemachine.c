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
#include "ee.h"
#include "eeConfig.h"

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


char str[20];
QMC_t compasdata;
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
		  case STATE_INIT:
			  ssd1306_Fill(Black);
			  nmea_parse(&myData, DataBuffer);
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
						ssd1306_WriteString("Pos screen", Font_6x8, White);
						ssd1306_SetCursor(32, 44);
						ssd1306_WriteString("Wait gps", Font_6x8, White);
						free(str);
					}
					if(BTN_A>=1){
							state++;//un appuie sur le bouton a nous fait passer a l'etat suivant
							BTN_A=0;//important le le reset afin que la condition marche encore a letat suivant
						}
			  break;
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
				 pace=1000/(60*myData.speed);//ici on convertie la vitesse en m/s en un pace en mim/Km on ecarte les erreure possite comme la division par 0
			 }
			 else {
				 pace=9999;//en cas de division par 0, techniquement le temps devient infini mais ce n'est pas intérréssant
			 }
			 if((pace-floor(pace))>=0.60){//conversion en minute
				 pace=pace+1;
				 sec=(pace-floor(pace))-0.60;
			 }
			 if(myData.fix == 1){ //if the GPS has a fix, print the data
			 						char * str = (char*)malloc(sizeof(char)*20);
			 						snprintf(str,15, "Vitmax=%.1f",vitmax*3.6);//amélioration possible la stocker en eeprom
			 						ssd1306_SetCursor(32, 32);
			 						ssd1306_WriteString(str, Font_6x8, White);
			 						snprintf(str,15, "V=%0.1f",(myData.speed)*3.6);
			 						ssd1306_SetCursor(32, 42);
			 						ssd1306_WriteString(str, Font_6x8, White);
			 						snprintf(str,15, "p=%0.0fmin%0.0f s",floor(pace),floor(sec*100));//affichage au format minute puis seconde
			 						ssd1306_SetCursor(32, 52);
			 						ssd1306_WriteString(str, Font_6x8, White);
			 						free(str);
			 					}
			 	else{ //if the GPS doesn't have a fix, print a message
			 						char *str = (char*)malloc(sizeof(char)*20);// message qui sra dans tous les etat si l'on ne capte pas de sattelites.
			 						ssd1306_SetCursor(32, 32);
			 						ssd1306_WriteString("Speed screen", Font_6x8, White);
			 						ssd1306_SetCursor(32, 44);
			 						ssd1306_WriteString("Wait gps", Font_6x8, White);
			 						free(str);
			 					}
			 if(BTN_B>=1){
			 vitmax=0;
			 BTN_B=0;//on reset la vitesse max par un appuie sur le bouton b
			 	}
			 				HAL_Delay(100);
			 				if(BTN_A>=1){
			 					state++;
			 					BTN_A=0;
			  	}
			  break;
		  case STATE_HEURE:
			  ssd1306_Fill(Black);
			  nmea_parse(&myData, DataBuffer);
			  ssd1306_SetCursor(32, 32);
			  ssd1306_WriteString("hr GMT+2:", Font_6x8, White);
			  int heure=0;
			  heure=(myData.lastMeasure[0]&0x0f)*10+(myData.lastMeasure[1]&0x0f);//conversion de l'heure en gmt+2 (heure de londre/paris)
			  if((heure+2)>=24){
				  heure=heure-22;
			  }
			  else{
				  heure=heure+2;
			  }

			  if(myData.fix == 1){ //if the GPS has a fix, print the data
			  			 						char * str = (char*)malloc(sizeof(char)*20);
			  			 						ssd1306_SetCursor(34, 42);
			  			 						snprintf(str,15, "%d:%c%c",heure,myData.lastMeasure[2],myData.lastMeasure[3]);
			  			 						ssd1306_WriteString(str, Font_11x18, White);//affichage de l'heure en grand
//			  			 						snprintf(str,15, "%c%c sec",myData.lastMeasure[4],myData.lastMeasure[5]);
//			  			 						ssd1306_WriteString(str, Font_6x8, White);
			  			 						free(str);
			  			 					}
			  			 	else{ //if the GPS doesn't have a fix, print a message
			  			 						char *str = (char*)malloc(sizeof(char)*20);
			  			 						ssd1306_SetCursor(32, 44);
			  			 						ssd1306_WriteString("Wait gps", Font_6x8, White);
			  			 						free(str);
			  			 	}
				  if(BTN_A>=1){
			  		state++;
			  		BTN_A=0;
			  		}
			  break;
		  case STATE_INFO:
			  ssd1306_Fill(Black);
			  nmea_parse(&myData, DataBuffer);
			  if(BTN_B==0){
			  if(myData.fix == 1){ //if the GPS has a fix, print the data
			 			 						char * str = (char*)malloc(sizeof(char)*20);
			 			 						snprintf(str,15, "hdop=%.1f",myData.hdop);//sert a	connaitre la qualitée du fix si proche de 1 voir inférieur alors le fix est tres bon
			 			 						ssd1306_SetCursor(32, 32);
			 			 						ssd1306_WriteString(str, Font_6x8, White);
			 			 						snprintf(str,15, "SatNb :%d",myData.satelliteCount);
			 			 						ssd1306_SetCursor(32, 42);
			 			 						ssd1306_WriteString(str, Font_6x8, White);
			 			 						free(str);
			 			 					}
			 			 	else{ //if the GPS doesn't have a fix, print a message
			 			 						char *str = (char*)malloc(sizeof(char)*20);
			 			 						ssd1306_SetCursor(32, 32);
			 			 						ssd1306_WriteString("balise", Font_6x8, White);
			 			 						ssd1306_SetCursor(32, 44);
			 			 						ssd1306_WriteString("Wait gps", Font_6x8, White);
			 			 						free(str);
			 			 					}
			  }
			  else{
			  		ssd1306_DrawBitmap(32, 32, speciale, 64, 64, White);
			  		ssd1306_UpdateScreen();//easter egg
			  		HAL_Delay(1500);
			  		BTN_B=0;
			  		}
			  if(BTN_A>=1){
			  		state++;
			  		BTN_A=0;
			  		}
			  break;
		  case STATE_CHOOSE:
		  		if(choose==0){//choose se met a jour à 1 dès que le choix de l'heure de fin a été fait ainsi on nous le redemande jamais avant que le compte a rebours soit finis

			  ssd1306_Fill(Black);
		  			ssd1306_SetCursor(32, 32);
		  			ssd1306_WriteString("choosehoure", Font_6x8, White);
		  		switch(statecpt){
		  			case STATE_HEURES:
		  				if(BTN_A>=1){
		  					(boumheure)=(boumheure)+1;// on choisi l'heure avec le bouton a et on passe aux minutes avec le bouton b
		  					BTN_A=0;
		  				}
		  				if(boumheure>=24){
		  					boumheure=0;
		  				}
		  				if(BTN_B>=1){
		  					statecpt++;
		  					BTN_B=0;
		  				}
		  				ssd1306_SetCursor(32, 42);
		  				snprintf(str,15, "hr:%d",boumheure);
		  				ssd1306_WriteString(str, Font_6x8, White);
		  				break;
		  			case STATE_MINUTE:
		  				if(BTN_A>=1){
		  					(boumminutes)=(boumminutes)+1;
		  					BTN_A=0;
		  				}
		  				if(boumminutes>=59){
		  					boumminutes=0;
		  				}
		  				if(BTN_B>=1){
		  					statecpt++;
		  					BTN_B=0;
		  				}
		  				ssd1306_SetCursor(32, 42);
		  				snprintf(str,15, "min:%d",boumminutes);
		  				ssd1306_WriteString(str, Font_6x8, White);
		  				break;
		  			case STATE_VALID:// on rappelle l'heure choisie puis on remet tout a zero sauf choose
		  				ssd1306_Fill(Black);
		  				char * str = (char*)malloc(sizeof(char)*20);
		  				ssd1306_WriteString("heure choisi:", Font_6x8, White);
		  				ssd1306_SetCursor(32, 42);
		  				snprintf(str,15, "%dH%dMin",boumheure,boumminutes);
		  				ssd1306_WriteString(str, Font_6x8, White);
		  				ssd1306_UpdateScreen();
		  				HAL_Delay(2000);
		  				choose=1;
		  				free(str);
		  				statecpt--;
		  				statecpt--;
		  				state--;
		  				state--;//state=state-4 ne marche pas pour une raison que nous ignorons
		  				state--;
		  				state--;
		  				break;
	}
		  		}
		  		else{
		  			ssd1306_Fill(Black);
		  			nmea_parse(&myData, DataBuffer);
		  			ssd1306_SetCursor(32, 32);
		  			ssd1306_WriteString("tps restant:", Font_6x8, White);// mode compte a rebours apres le choix
		  						  int heure=0;



		  						  if(myData.fix == 1){ //if the GPS has a fix, print the data
		  							heure=(myData.lastMeasure[0]&0x0f)*10+(myData.lastMeasure[1]&0x0f);
		  							if((heure+2)>=24){
		  									 heure=heure-22;
		  									  	}
		  								else{
		  									heure=heure+2;
		  									  	}

		  							  	char * str = (char*)malloc(sizeof(char)*20);
		  						  		ssd1306_SetCursor(34, 40);
		  						  		if((boumminutes-((myData.lastMeasure[2]&0x0F)*10+(myData.lastMeasure[3]&0x0F)))>=0){
		  						  			 					snprintf(str,15, "%d:%d",(boumheure-heure),(boumminutes-((myData.lastMeasure[2]&0x0F)*10+(myData.lastMeasure[3]&0x0F))));// on s'assure de bien afficher le temps restant

		  						  			 					}
		  						  		else{
		  						  			 					snprintf(str,15, "%d:%d",(boumheure-heure)-1,60+(boumminutes-((myData.lastMeasure[2]&0x0F)*10+(myData.lastMeasure[3]&0x0F))));
		  						  			 					}
		  						  			 				ssd1306_WriteString(str, Font_11x18, White);
		  						  			 				snprintf(str,15, "%d sec",60-(((myData.lastMeasure[4]&0x0F)*10)+(myData.lastMeasure[5]&0x0F)));
		  						  			 				ssd1306_SetCursor(34, 58);
		  						  			 				ssd1306_WriteString(str, Font_6x8, White);
		  						  			 				free(str);
		  						  		if((boumheure-heure)<=0&&(boumminutes-((myData.lastMeasure[2]&0x0F)*10+(myData.lastMeasure[3]&0x0F)))<=0){// si le compte a rebours tombe a zero on allume un gpio, on affiche un message puis on reset definitivement le compte a rebours
		  						  			 							HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,1);
		  						  			 						ssd1306_Fill(Black);

		  						  			 							ssd1306_SetCursor(32, 48);

		  						  			 							ssd1306_WriteString("explosion", Font_6x8, White);
		  						  			 						ssd1306_UpdateScreen();
		  						  			 						HAL_Delay(2000);
		  						  			 							HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,0);
		  						  			 							boumheure=0;
		  						  			 							boumminutes=0;
		  						  			 							choose=0;
		  						  			 							state--;
		  						  			 							state--;//state=state-4 ne marche pas pour une raison que nous ignorons
		  						  			 							state--;
		  						  			 							state--;
		  						  			 							BTN_A=0;
		  						  			 						}
		  						  			 					}
		  						  			 	else{ //if the GPS doesn't have a fix, print a message
		  						  			 						char *str = (char*)malloc(sizeof(char)*20);
		  						  			 						ssd1306_SetCursor(32, 44);
		  						  			 						ssd1306_WriteString("Wait gps", Font_6x8, White);
		  						  			 						free(str);
		  						  			 	}









		  						  if(BTN_A>=1){
		  							state--;
		  							state--;//state=state-4 ne marche pas pour une raison que nous ignorons
		  							state--;
		  							state--;
		  							BTN_A=0;
		  							}




		  		}
		  		 break;
	}

return ;
}




























