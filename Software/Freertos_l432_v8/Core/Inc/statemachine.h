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
#include "math.h"
#include "sd_app.h"
#include <string.h>
#include <math.h>




typedef enum{
	STATE_SPEED,
	STATE_BALISE,
	STATE_POS,
	STATE_CITY,
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
	ECRANBALISESTATE4,
	ECRANBALISESTATE5,
	ECRANBALISESTATE6,
	ECRANBALISESTATE7

}ECRANBALISESTATE;

typedef enum {
    SETTING_FREQ,
    SETTING_FORMAT
} SETTING_STATE;

#pragma pack(push, 1)
typedef struct {
    int32_t lat;
    int32_t lon;
    uint32_t pop;
    char country[2];
    char name[18];
} CityRecord;
#pragma pack(pop)

typedef enum{
	CITY,
	MOUNTAIN

}SEARCHSTATE;


#define PI_F 3.1415926535f
#define DEG_TO_RAD (PI_F / 180.0f)


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


    SEARCHSTATE searchstate;
    uint8_t search_initialised;
    uint8_t city_search_done;
    uint8_t mountain_search_done;
    char found_name[18];
    char found_country[3];
    float found_lat;
    float found_lon;
    uint32_t city_pop;
    uint32_t mountain_alt;
    float found_dist_km;
    uint8_t distance_calc_counter;

    /* Position de départ de balise */
    float    oldlat;
    float    oldlong;

} AppStateMachineContext;


void StateMachine_Run(AppStateMachineContext *ctx,GNSS_StateHandle*gps,Buttons_t *buttons,AdcContext_t *gAdc,SDCard_struct *sd);
void set_time (uint8_t hr, uint8_t min, uint8_t sec);
void set_date (uint8_t year, uint8_t month, uint8_t date, uint8_t day);
void get_time_date(AppStateMachineContext * context);
void FindClosest(GNSS_StateHandle *gps, AppStateMachineContext *ctx, SDCard_struct *sd);

#endif /* INC_STATEMACHINE_H_ */
