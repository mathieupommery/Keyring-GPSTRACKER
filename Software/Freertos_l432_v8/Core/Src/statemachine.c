/*
 * statemachine.c
 *
 *  Created on: May 27, 2024
 *      Author: mathi
 */

#include "statemachine.h"



extern uint8_t bufferscreen[50];
extern AdcContext_t gAdc;

static void utility_function(AppStateMachineContext * ctx,GNSS_StateHandle*gps){

	if (gps->fixType >= 2 && ctx->rtc_synced == 0) {
	        ctx->HR        = gps->hour;
	        ctx->MINUTE    = gps->min;
	        ctx->SEC       = gps->sec;
	        ctx->JOURS     = gps->day;
	        ctx->MOIS      = gps->month;
	        ctx->ANNEE     =(uint16_t ) gps->year;
	        set_time(ctx->HR, ctx->MINUTE, ctx->SEC);
	        ctx->rtc_synced = 1 ;
	    }

}



void StateMachine_Run(AppStateMachineContext *ctx,GNSS_StateHandle*gps,Buttons_t *buttons,AdcContext_t *gAdc,SDCard_struct *sd)
{
	utility_function(ctx,gps);

    switch (ctx->state)
    {
    /* --------------------------------------------------------------------- */
    /*                              STATE_SPEED                              */
    /* --------------------------------------------------------------------- */
    case STATE_SPEED:
    {
        ssd1306_Fill(Black);

        /* Calcul vitmax + pace */
        if (gps->fgSpeed >= ctx->vitmax) {
            ctx->vitmax = gps->fgSpeed;
        }

        float pace = 0.0f;
        float sec  = 0.0f;

        if (gps->fgSpeed != 0.0f) {
            pace = 1000.0f / (60.0f * gps->fgSpeed);
            sec  = (pace - floorf(pace)) * 60.0f;
        } else {
            pace = 99.0f;
            sec  = 59.0f;
        }

        float speed_kmh  = gps->fgSpeed * 3.6f;
        float vitmax_kmh = ctx->vitmax * 3.6f;

        switch (ctx->spdstate)
        {
        case STATE_GROS:
            ssd1306_SetCursor(32, 32);
            snprintf((char *)bufferscreen, 15, "%0.1f", speed_kmh);
            ssd1306_WriteString((char *)bufferscreen, Font_16x24, White);

            ssd1306_SetCursor(32, 56);
            ssd1306_WriteString("Vit(kmh)", Font_6x8, White);

            batterygauge(gAdc->vbat, 79, 57, 1);

            if (buttons->BTN_B >= 1) {
                ctx->spdstate++;
                buttons->BTN_B = 0;
            }
            break;

        case STATE_GROS1:
            ssd1306_SetCursor(32, 32);
            snprintf((char *)bufferscreen, 15, "%0.1f", vitmax_kmh);
            ssd1306_WriteString((char *)bufferscreen, Font_16x24, White);

            ssd1306_SetCursor(32, 56);
            ssd1306_WriteString("maxV", Font_6x8, White);

            batterygauge(gAdc->vbat, 79, 57, 1);

            if (buttons->BTN_B >= 1) {
                ctx->spdstate++;
                buttons->BTN_B = 0;
            }
            break;

        case STATE_SUMMARY:
            ssd1306_SetCursor(32, 32);
            snprintf((char *)bufferscreen, 15, "%0.0fmin%0.0fs",floorf(pace), floorf(sec));
            ssd1306_WriteString((char *)bufferscreen, Font_7x10, White);
            ssd1306_SetCursor(32, 42);
            ssd1306_WriteString("pace", Font_6x8, White);
            ssd1306_SetCursor(32, 56);
            snprintf((char *)bufferscreen, 15, "V=%0.1fkmh", vitmax_kmh);
            ssd1306_WriteString((char *)bufferscreen, Font_6x8, White);

            batterygauge(gAdc->vbat, 79, 43, 1);

            if (buttons->BTN_B >= 1) {
                ctx->spdstate -= 2;   /* retour au début du cycle */
                buttons->BTN_B = 0;
            }
            break;

        default:
            ctx->spdstate = STATE_GROS;
            break;
        }

        if (buttons->BTN_A >= 1) {
            ctx->state++;
            buttons->BTN_A = 0;
            buttons->BTN_B = 0;
        }

        if (buttons->BTN_A_LONG >= 1) {
            ctx->state+=4;
            buttons->BTN_A      = 0;
            buttons->BTN_B      = 0;
            buttons->BTN_A_LONG = 0;
        }

        if (buttons->BTN_B_LONG >= 1) {
            ctx->vitmax        = 0.0f;
            buttons->BTN_B_LONG = 0;
        }
    }
        break;

    /* --------------------------------------------------------------------- */
    /*                             STATE_BALISE                              */
    /* --------------------------------------------------------------------- */
    case STATE_BALISE:
    {
        ssd1306_Fill(Black);



        switch (ctx->balisestate)
        {
        case BALISESTATE1:
            ssd1306_SetCursor(32, 32);
            if((sd->isthereasdcard == 0) || (sd->mscconnected == 1) || (gps->numSV < 3 )){
            if(sd->mscconnected == 1){
            	snprintf((char *)bufferscreen, 15, "usb");
            	ssd1306_WriteString((char *)bufferscreen, Font_7x10, White);
                    }
            if(sd->isthereasdcard == 0){
            	ssd1306_SetCursor(32, 42);
                snprintf((char *)bufferscreen, 15, "no sd");
                ssd1306_WriteString((char *)bufferscreen, Font_7x10, White);
                 }
            if(gps->numSV < 3 ){
                        	ssd1306_SetCursor(32, 52);
                            snprintf((char *)bufferscreen, 15, "no gps");
                            ssd1306_WriteString((char *)bufferscreen, Font_7x10, White);
                             }
            }

            else{
            	ssd1306_SetCursor(32, 32);
            	snprintf((char *)bufferscreen, 15, "OK");
            	ssd1306_WriteString((char *)bufferscreen, Font_16x24, White);
            	ssd1306_SetCursor(32, 56);
            	snprintf((char *)bufferscreen, 15, "sat:%d",gps->numSV);
            	ssd1306_WriteString((char *)bufferscreen, Font_6x8, White);


            if (buttons->BTN_B_LONG >= 1) {
                ctx->balisestate++;
                buttons->BTN_B_LONG = 0;
                buttons->BTN_A      = 0;
                ctx->oldlat         = gps->fLat;
                ctx->oldlong        = gps->fLon;
                sd->is_recording=1;
            }
            }

            if (buttons->BTN_A >= 1) {
                ctx->state++;
                buttons->BTN_A = 0;
                buttons->BTN_B = 0;
            }

            if (buttons->BTN_A_LONG >= 1) {
                ctx->state--;
                buttons->BTN_A_LONG = 0;
                buttons->BTN_B      = 0;
            }
            break;

        case BALISESTATE2:
            ssd1306_SetCursor(32, 32);

            if (sd->is_recording == 0) {
                ctx->balisestate = BALISESTATE1;
                break;
             }

            uint8_t any_button_pressed = (buttons->BTN_A >= 1 || buttons->BTN_B >= 1 || buttons->BTN_A_LONG >= 1 || buttons->BTN_B_LONG >= 1);
            if (any_button_pressed) {
                 ctx->last_action_tick = HAL_GetTick();
                 }
            if ((HAL_GetTick() - ctx->last_action_tick) > 20000)
                        {

                            if ((HAL_GetTick() / 500) % 2) {
                                ssd1306_SetCursor(40, 38);
                                ssd1306_WriteString("REC", Font_16x24, White);
                            }

                            if (any_button_pressed) {
                                                buttons->BTN_A = 0; buttons->BTN_B = 0;
                                                buttons->BTN_A_LONG = 0; buttons->BTN_B_LONG = 0;
                                            }


                        }
                        else
                        {
            switch (ctx->ecranstate)
            {
            case ECRANBALISESTATE1:
                snprintf((char *)bufferscreen, 50, "%0.3f",(float) gps->distance_parcouru);
                ssd1306_WriteString((char *)bufferscreen, Font_7x10, White);

                ssd1306_SetCursor(32, 42);
                ssd1306_WriteString("Dist(km)", Font_6x8, White);

                if (buttons->BTN_B >= 1) {
                    ctx->ecranstate++;
                    buttons->BTN_B = 0;
                    buttons->BTN_A = 0;
                }
                break;

            case ECRANBALISESTATE2:
                snprintf((char *)bufferscreen, 50, "%0.1f", gps->fgSpeed * 3.6f);
                ssd1306_WriteString((char *)bufferscreen, Font_7x10, White);

                ssd1306_SetCursor(32, 42);
                ssd1306_WriteString("Vit(kmh)", Font_6x8, White);

                if (buttons->BTN_B >= 1) {
                    ctx->ecranstate++;
                    buttons->BTN_B = 0;
                    buttons->BTN_A = 0;
                }
                break;

            case ECRANBALISESTATE3:
                snprintf((char *)bufferscreen, 50, "%0.1f", gAdc->vbat);
                ssd1306_WriteString((char *)bufferscreen, Font_7x10, White);

                ssd1306_SetCursor(32, 42);
                ssd1306_WriteString("Vbat(V)", Font_6x8, White);

                if (buttons->BTN_B >= 1) {
                    ctx->ecranstate++;
                    buttons->BTN_B = 0;
                    buttons->BTN_A = 0;
                }
                break;

            case ECRANBALISESTATE4:
                snprintf((char *)bufferscreen, 50, "%0.1f", ctx->vitmax * 3.6f);
                ssd1306_WriteString((char *)bufferscreen, Font_7x10, White);

                ssd1306_SetCursor(32, 42);
                ssd1306_WriteString("MaxV", Font_6x8, White);

                if (buttons->BTN_B >= 1) {
                    ctx->ecranstate -= 3;
                    buttons->BTN_B = 0;
                    buttons->BTN_A = 0;
                }
                break;

            default:
                ctx->ecranstate = ECRANBALISESTATE1;
                break;
            }

            batterygauge(gAdc->vbat, 79, 42, 1);

            ssd1306_SetCursor(32, 52);
            snprintf((char *)bufferscreen, 50, "%0.2fV", gAdc->vbat);
            ssd1306_WriteString((char *)bufferscreen, Font_6x8, White);

            ssd1306_SetCursor(65, 52);
            snprintf((char *)bufferscreen, 50, "sat=%d", gps->numSV);
            ssd1306_WriteString((char *)bufferscreen, Font_6x8, White);

            if (buttons->BTN_B_LONG >= 1) {
                ctx->balisestate--;
                buttons->BTN_B_LONG = 0;
                buttons->BTN_A      = 0;
                sd->is_recording=0;
            }
          }
            break;
        default:
            ctx->balisestate = BALISESTATE1;
            break;
        }
    }
        break;

    /* --------------------------------------------------------------------- */
    /*                              STATE_POS                                */
    /* --------------------------------------------------------------------- */
    case STATE_POS:
    {
        ssd1306_Fill(Black);

        switch (ctx->posstate)
        {
        case STATE_SUMMARY1:
            ssd1306_SetCursor(32, 32);
            snprintf((char *)bufferscreen, 15, "Latitude:");
            ssd1306_WriteString((char *)bufferscreen, Font_6x8, White);

            snprintf((char *)bufferscreen, 15, "%0.7f", gps->fLat);
            ssd1306_SetCursor(32, 40);
            ssd1306_WriteString((char *)bufferscreen, Font_6x8, White);

            snprintf((char *)bufferscreen, 15, "Longitude:");
            ssd1306_SetCursor(32, 48);
            ssd1306_WriteString((char *)bufferscreen, Font_6x8, White);

            snprintf((char *)bufferscreen, 15, "%0.7f", gps->fLon);
            ssd1306_SetCursor(32, 56);
            ssd1306_WriteString((char *)bufferscreen, Font_6x8, White);

            if (buttons->BTN_B >= 1) {
                ctx->posstate++;
                buttons->BTN_B = 0;
            }
            break;

        case STATE_INFO:
            ssd1306_Fill(Black);

            snprintf((char *)bufferscreen, 15, "hacc=%0.2fm", gps->fhACC);
            ssd1306_SetCursor(32, 32);
            ssd1306_WriteString((char *)bufferscreen, Font_7x10, White);

            snprintf((char *)bufferscreen, 20, "v=%0.2fV", gAdc->vbat);
            ssd1306_SetCursor(32, 42);
            ssd1306_WriteString((char *)bufferscreen, Font_7x10, White);

            ssd1306_SetCursor(32, 52);
            snprintf((char *)bufferscreen, 15, "T=%0.2fC", gAdc->temp);
            ssd1306_WriteString((char *)bufferscreen, Font_7x10, White);

            if (buttons->BTN_B >= 1) {
                ctx->posstate++;
                buttons->BTN_B = 0;
            }
            if (buttons->BTN_B_LONG >= 1) {
                ctx->posstate--;
                buttons->BTN_B_LONG = 0;
            }
            break;

        case STATE_INFO2:
            ssd1306_Fill(Black);

            snprintf((char *)bufferscreen, 15, "Satnum");
            ssd1306_SetCursor(32, 32);
            ssd1306_WriteString((char *)bufferscreen, Font_6x8, White);

            snprintf((char *)bufferscreen, 20, "%d sat", gps->numSV);
            ssd1306_SetCursor(32, 40);
            ssd1306_WriteString((char *)bufferscreen, Font_6x8, White);

            ssd1306_SetCursor(32, 48);
            snprintf((char *)bufferscreen, 15, "DateJ/M/Y");
            ssd1306_WriteString((char *)bufferscreen, Font_6x8, White);

            ssd1306_SetCursor(32, 56);
            snprintf((char *)bufferscreen, 15, "%d/%d/%d",
                     gps->day, gps->month, gps->year);
            ssd1306_WriteString((char *)bufferscreen, Font_6x8, White);

            if (buttons->BTN_B >= 1) {
                ctx->posstate++;
                buttons->BTN_B = 0;
            }
            if (buttons->BTN_B_LONG >= 1) {
                ctx->posstate--;
                buttons->BTN_B_LONG = 0;
            }
            break;

        case STATE_ALT:
            ssd1306_SetCursor(32, 32);
            snprintf((char *)bufferscreen, 15, "altitude:");
            ssd1306_WriteString((char *)bufferscreen, Font_6x8, White);

            snprintf((char *)bufferscreen, 15, "%0.2f m", gps->fhMSL);
            ssd1306_SetCursor(32, 40);
            ssd1306_WriteString((char *)bufferscreen, Font_6x8, White);

            snprintf((char *)bufferscreen, 15, "Accuracy(m)");
            ssd1306_SetCursor(32, 48);
            ssd1306_WriteString((char *)bufferscreen, Font_6x8, White);

            snprintf((char *)bufferscreen, 15, "%0.1f", gps->fvACC);
            ssd1306_SetCursor(32, 56);
            ssd1306_WriteString((char *)bufferscreen, Font_6x8, White);

            if (buttons->BTN_B >= 1) {
                ctx->posstate++;
                buttons->BTN_B = 0;
            }
            if (buttons->BTN_B_LONG >= 1) {
                ctx->posstate--;
                buttons->BTN_B_LONG = 0;
            }
            break;

        case STATE_HEURE:
            ssd1306_Fill(Black);
            get_time_date(ctx);

            ssd1306_SetCursor(32, 32);
            ssd1306_WriteString("hr GMT:", Font_6x8, White);

            ssd1306_SetCursor(32, 42);
            snprintf((char *)bufferscreen, 15, "%02d:%02d", ctx->HR, ctx->MINUTE);
            ssd1306_WriteString((char *)bufferscreen, Font_7x10, White);

            ssd1306_SetCursor(32, 52);
            snprintf((char *)bufferscreen, 15, "%02d sec", ctx->SEC);
            ssd1306_WriteString((char *)bufferscreen, Font_7x10, White);

            if (buttons->BTN_B >= 1) {
                ctx->posstate -= 4;
                buttons->BTN_B = 0;
            }
            if (buttons->BTN_B_LONG >= 1) {
                ctx->posstate--;
                buttons->BTN_B_LONG = 0;
            }
            break;

        default:
            ctx->posstate = STATE_SUMMARY1;
            break;
        }

        if (buttons->BTN_A >= 1) {
            ctx->state++;
            buttons->BTN_A = 0;
            buttons->BTN_B = 0;
        }
        if (buttons->BTN_A_LONG >= 1) {
            ctx->state--;
            buttons->BTN_A      = 0;
            buttons->BTN_B      = 0;
            buttons->BTN_A_LONG = 0;
        }
    }
        break;

    /* --------------------------------------------------------------------- */
    /*                         STATE_CHRONOMETER                             */
    /* --------------------------------------------------------------------- */
    case STATE_CHRONOMETER:
    {
        ssd1306_Fill(Black);
        ssd1306_SetCursor(32, 32);
        ssd1306_WriteString("chrono", Font_6x8, White);
        ssd1306_SetCursor(32, 40);

        switch (ctx->chronostate)
        {
        case STATE_RESET:
            ctx->calctime    = 0;
            ctx->timehandler = 0;

            if (buttons->BTN_B >= 1) {
                ctx->chronostate++;
                buttons->BTN_B  = 0;
                ctx->starttime  = uwTick;
            }
            break;

        case STATE_RUN:
            ctx->calctime = uwTick - ctx->starttime + ctx->timehandler;

            if (buttons->BTN_B >= 1) {
                ctx->chronostate++;
                buttons->BTN_B = 0;
            }
            break;

        case STATE_PAUSE:
            ctx->timehandler = ctx->calctime;

            if (buttons->BTN_B >= 1) {
                ctx->chronostate--;
                buttons->BTN_B  = 0;
                ctx->starttime  = uwTick;
            }
            if (buttons->BTN_B_LONG >= 1) {
                ctx->chronostate -= 2;
                buttons->BTN_B_LONG = 0;
                ctx->timehandler = 0;
            }
            break;

        default:
            ctx->chronostate = STATE_RESET;
            break;
        }

        float min     = floorf((float)ctx->calctime / 60000.0f);
        float seconde = (float)(ctx->calctime - (uint32_t)(min * 60000.0f)) / 1000.0f;

        snprintf((char *)bufferscreen, 15, "%0.0fmin", min);
        ssd1306_WriteString((char *)bufferscreen, Font_7x10, White);

        ssd1306_SetCursor(32, 50);
        snprintf((char *)bufferscreen, 15, "%0.3fs", seconde);
        ssd1306_WriteString((char *)bufferscreen, Font_7x10, White);

        if (buttons->BTN_A >= 1) {
            ctx->state--;
            ctx->state--;
            ctx->state--;
            buttons->BTN_A = 0;
            buttons->BTN_B = 0;
        }
        if (buttons->BTN_A_LONG >= 1) {
            ctx->state--;
            buttons->BTN_A      = 0;
            buttons->BTN_B      = 0;
            buttons->BTN_A_LONG = 0;
        }
    }
        break;
    default:
        ctx->state = STATE_SPEED;
        break;
    }
}

void set_time (uint8_t hr, uint8_t min, uint8_t sec)
{
	RTC_TimeTypeDef sTime = {0};
	sTime.Hours = hr;
	sTime.Minutes = min;
	sTime.Seconds = sec;
	sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
	sTime.StoreOperation = RTC_STOREOPERATION_RESET;
	if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN) != HAL_OK)
	{
		Error_Handler();
	}
}

void set_date (uint8_t year, uint8_t month, uint8_t date, uint8_t day)  // monday = 1
{
	RTC_DateTypeDef sDate = {0};
	sDate.WeekDay = day;
	sDate.Month = month;
	sDate.Date = date;
	sDate.Year = year;
	if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN) != HAL_OK)
	{
		Error_Handler();
	}

	HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR1, 0x2345);  // backup register
}


void get_time_date(AppStateMachineContext * context)
{
  RTC_DateTypeDef gDate;
  RTC_TimeTypeDef gTime;

  /* Get the RTC current Time */
  HAL_RTC_GetTime(&hrtc, &gTime, RTC_FORMAT_BIN);
  /* Get the RTC current Date */
  HAL_RTC_GetDate(&hrtc, &gDate, RTC_FORMAT_BIN);

  context->SEC=gTime.Seconds;
  context->HR=gTime.Hours;
  context->MINUTE=gTime.Minutes;
  context->JOURS=gDate.Date;
  context->MOIS=gDate.Month;
  context->ANNEE=gDate.Year+2000;
}



