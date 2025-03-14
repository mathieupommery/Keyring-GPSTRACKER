/*
 * statemachine.h
 *
 *  Created on: May 27, 2024
 *      Author: mathi
 */

#ifndef INC_STATEMACHINE_H_
#define INC_STATEMACHINE_H_



#endif /* INC_STATEMACHINE_H_ */

#include "main.h"
#include "ssd1306.h"
#include "math.h"



typedef enum{
	STATE_SPEED,
	STATE_POS,
	STATE_HEURE,
	STATE_INFO,
	STATE_CHRONOMETER,
	STATE_BALISE,
	STATE_USB,
	STATE_MONITOR,
	STATE_TARVOS,
	STATE_BARO,
	STATE_TEST


}STATE_TYPE;

void statemachine(void);

typedef enum{
	STATE_HEURES,
	STATE_MINUTE,
	STATE_VALID

}COMPTEUR;

typedef enum{
	STATE_DIGIT,
	STATE_OLD

}HEURE;

typedef enum{
	STATE_SUMMARY,
	STATE_COMPTEUR,
	STATE_GRAPH

}SPEED;

typedef enum{
	STATE_SUMMARY1,
	STATE_LAT,
	STATE_LONG,
	STATE_ALT

}POS;

typedef enum{
	STATE_RESET,
	STATE_RUN,
	STATE_PAUSE

}CHRONO;

typedef enum{
	STATE_MARCHE,
	STATE_REPOS

}KEYBOARD;

typedef enum{
	GIF1,
	GIF2,
	GIF3,
	GIF4

}GIF;

typedef enum{
	USBSTATE1,
	USBSTATE2,
	USBSTATE3

}USBSTATE;

typedef enum{
	BALISESTATE1,
	BALISESTATE2,
	BALISESTATE3

}BALISESTATE;

typedef enum{
	TARVOS1,
	TARVOS2,
	TARVOS3

}TARVOSSTATE;

typedef enum{
	BARO1,
	BARO2

}BAROSTATE;


typedef struct
{
	uint8_t MODIFIER;
	uint8_t RESERVED;
	uint8_t KEYCODE1;
	uint8_t KEYCODE2;
	uint8_t KEYCODE3;
	uint8_t KEYCODE4;
	uint8_t KEYCODE5;
	uint8_t KEYCODE6;
} keyboardHID;

