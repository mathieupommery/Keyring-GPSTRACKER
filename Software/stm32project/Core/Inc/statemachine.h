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
#include "QMC5883.h"
#include "ssd1306.h"
#include "math.h"



typedef enum{
	STATE_SPEED,
	STATE_POS,
	STATE_HEURE,
	STATE_INFO,
	STATE_CHRONOMETER

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
