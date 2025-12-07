/*
 * statemachine.h
 *
 *  Created on: May 27, 2024
 *      Author: mathi
 */

#ifndef INC_STATEMACHINE_H_
#define INC_STATEMACHINE_H_

#include "main.h"
#include "ssd1306.h"
#include "GNSS.h"
#include "rtc.h"




typedef enum{
	STATE_SPEED,
	STATE_BALISE,
	STATE_POS,
	STATE_CHRONOMETER,
	STATE_ACCEL

}STATE_TYPE;

typedef enum{
	STATE_HEURES,
	STATE_MINUTE,
	STATE_VALID

}COMPTEUR;

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
	STATE_RESET,
	STATE_RUN,
	STATE_PAUSE

}CHRONO;

typedef enum{
	STATE_MARCHE,
	STATE_REPOS

}KEYBOARD;

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

typedef enum{
	WAITFORGPS,
	WAITFORPUSH,
	WAITFORSTOP,
	INRUN,
	RESULT



}ACCELSTATE;

typedef struct
{
    /* Etats principaux et sous-états */
    uint8_t state;
    uint8_t spdstate;
    uint8_t balisestate;
    uint8_t ecranstate;
    uint8_t posstate;
    uint8_t chronostate;
    uint8_t accelstate;

    uint8_t  flag_usb_mounted;

    float    vitmax;

    /* Gestion heure/date */
    uint8_t  settimeen;
    uint8_t  SEC;
    uint8_t  MINUTE;
    uint8_t  HR;
    uint8_t JOURS;
    uint8_t MOIS;
    uint16_t ANNEE;

    /* Chronomètre */
    uint32_t calctime;
    uint32_t starttime;
    uint32_t timehandler;

    /* Accélération 0-50 / 0-100 */
    uint16_t timecounter;
    uint8_t  counterforstart;
    uint16_t indexcounterforstart;

    uint32_t accel_start_time;
    float    time50kmh;
    float    time100kmh;
    uint8_t  flag_50kmh;
    uint8_t  flag_100kmh;

    /* Position de départ de balise */
    float    oldlat;
    float    oldlong;

} AppStateMachineContext;


void StateMachine_Run(AppStateMachineContext *ctx,GNSS_StateHandle *gps,Buttons_t *buttons,AdcContext_t *gAdc);
void set_time (uint8_t hr, uint8_t min, uint8_t sec);
void set_date (uint8_t year, uint8_t month, uint8_t date, uint8_t day);
void get_time_date(AppStateMachineContext * context);

#endif /* INC_STATEMACHINE_H_ */
