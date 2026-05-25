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
#include "math.h"
#include "sd_app.h"




typedef enum{
	STATE_SPEED,
	STATE_BALISE,
	STATE_POS,
	STATE_SETTINGS,
	STATE_SCREENSAVER

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
	ECRANBALISESTATE4

}ECRANBALISESTATE;

typedef enum {
    SETTING_FREQ,
    SETTING_FORMAT
} SETTING_STATE;

typedef struct
{
    /* Etats principaux et sous-états */
    uint8_t state;
    uint8_t previous_state;
    uint8_t spdstate;
    uint8_t balisestate;
    uint8_t ecranstate;
    uint8_t posstate;
    uint8_t settingstate;

    float    vitmax;

    uint32_t last_action_tick;

    /* Gestion heure/date */
    uint8_t  SEC;
    uint8_t  MINUTE;
    uint8_t  HR;
    uint8_t JOURS;
    uint8_t MOIS;
    uint16_t ANNEE;
    uint8_t rtc_synced;

    /* Position de départ de balise */
    float    oldlat;
    float    oldlong;

} AppStateMachineContext;


void StateMachine_Run(AppStateMachineContext *ctx,GNSS_StateHandle*gps,Buttons_t *buttons,AdcContext_t *gAdc,SDCard_struct *sd);
void set_time (uint8_t hr, uint8_t min, uint8_t sec);
void set_date (uint8_t year, uint8_t month, uint8_t date, uint8_t day);
void get_time_date(AppStateMachineContext * context);

#endif /* INC_STATEMACHINE_H_ */
