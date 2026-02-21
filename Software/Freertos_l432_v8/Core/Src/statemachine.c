/*
 * statemachine.c
 *
 *  Created on: May 27, 2024
 *      Author: mathi
 */

#include "statemachine.h"


extern uint8_t bufferscreen[50];
extern AdcContext_t gAdc;


void StateMachine_Run(AppStateMachineContext *ctx,GNSS_StateHandle*gps,Buttons_t *buttons,AdcContext_t *gAdc)
{
    ctx->flag_usb_mounted =(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_4) == GPIO_PIN_RESET) ? 0 : 1;

    if (ctx->settimeen == 0) {
        ctx->settimeen = 1;
        ctx->HR        = gps->hour;
        ctx->MINUTE    = gps->min;
        ctx->SEC       = gps->sec;
        ctx->JOURS     = gps->day;
        ctx->MOIS      = gps->month;
        ctx->ANNEE     =(uint16_t ) gps->year;
        set_time(ctx->HR, ctx->MINUTE, ctx->SEC);
    }

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

        ssd1306_SetCursor(32, 32);
        snprintf((char *)bufferscreen, 15, "usb: %d", ctx->flag_usb_mounted);
        ssd1306_WriteString((char *)bufferscreen, Font_7x10, White);

        switch (ctx->balisestate)
        {
        case BALISESTATE1:
            ssd1306_SetCursor(32, 32);

            if (buttons->BTN_B_LONG >= 1) {
                ctx->balisestate++;
                buttons->BTN_B_LONG = 0;
                buttons->BTN_A      = 0;
                ctx->oldlat         = gps->fLat;
                ctx->oldlong        = gps->fLon;
                // osThreadResume(BALISEHandle);
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

            switch (ctx->ecranstate)
            {
            case ECRANBALISESTATE1:
                snprintf((char *)bufferscreen, 50, "no data");
                ssd1306_WriteString((char *)bufferscreen, Font_7x10, White);

                ssd1306_SetCursor(32, 42);
                ssd1306_WriteString("Capacity", Font_6x8, White);

                if (buttons->BTN_B >= 1) {
                    ctx->ecranstate++;
                    buttons->BTN_B = 0;
                    buttons->BTN_A = 0;
                }
                break;

            case ECRANBALISESTATE2:
                snprintf((char *)bufferscreen, 50, "%0.3lf", gps->distance_parcouru);
                ssd1306_WriteString((char *)bufferscreen, Font_7x10, White);

                ssd1306_SetCursor(32, 42);
                ssd1306_WriteString("Dist(km)", Font_6x8, White);

                if (buttons->BTN_B >= 1) {
                    ctx->ecranstate++;
                    buttons->BTN_B = 0;
                    buttons->BTN_A = 0;
                }
                break;

            case ECRANBALISESTATE3:
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

            case ECRANBALISESTATE5:
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

            case ECRANBALISESTATE6:
                snprintf((char *)bufferscreen, 50, "%0.1f", ctx->vitmax * 3.6f);
                ssd1306_WriteString((char *)bufferscreen, Font_7x10, White);

                ssd1306_SetCursor(32, 42);
                ssd1306_WriteString("MaxV", Font_6x8, White);

                if (buttons->BTN_B >= 1) {
                    ctx->ecranstate -= 4;
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
                // osThreadSuspend(BALISEHandle);
                HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
            }
            break;

        case BALISESTATE3:
            ssd1306_SetCursor(32, 32);
            ssd1306_WriteString("fin de", Font_6x8, White);
            ssd1306_SetCursor(32, 42);
            ssd1306_WriteString("memoire", Font_6x8, White);

            if (buttons->BTN_A >= 1) {
                ctx->state++;
                buttons->BTN_A   = 0;
                buttons->BTN_B   = 0;
                ctx->settimeen   = 0;
            }
            if (buttons->BTN_A_LONG >= 1) {
                ctx->state--;
                buttons->BTN_A   = 0;
                buttons->BTN_B   = 0;
                buttons->BTN_A_LONG = 0;
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
    /*                             STATE_ACCEL                               */
    /* --------------------------------------------------------------------- */
    case STATE_ACCEL:
    {
        ssd1306_Fill(Black);
        ssd1306_SetCursor(32, 32);
        ssd1306_WriteString("0-100", Font_6x8, White);

        switch (ctx->accelstate)
        {
        case WAITFORGPS:
            ssd1306_SetCursor(32, 40);
            ssd1306_WriteString("GPS fix", Font_7x10, White);

            if (gps->fixType >= 2) {
                ctx->accelstate++;
            }

            if (buttons->BTN_A >= 1) {
                ctx->state -= 4;
                buttons->BTN_A = 0;
                buttons->BTN_B = 0;
            }
            if (buttons->BTN_A_LONG >= 1) {
                ctx->state--;
                buttons->BTN_A      = 0;
                buttons->BTN_B      = 0;
                buttons->BTN_A_LONG = 0;
            }
            break;

        case WAITFORPUSH:
            ctx->timecounter++;
            if (ctx->timecounter >= 10) {
                ctx->timecounter = 0;
            }

            if ((ctx->timecounter % 2) == 0) {
                ssd1306_SetCursor(32, 40);
                ssd1306_WriteString("Push B", Font_7x10, White);
            }

            if (buttons->BTN_B_LONG >= 1) {
                ctx->accelstate++;
                buttons->BTN_B_LONG = 0;
            } else {
                if (buttons->BTN_A >= 1) {
                    ctx->state -= 4;
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

        case WAITFORSTOP:
            if (gps->fgSpeed <= 1.0f) {
                ssd1306_SetCursor(56, 40);
                snprintf((char *)bufferscreen, 15, "%d", 3 - ctx->counterforstart);
                ssd1306_WriteString((char *)bufferscreen, Font_16x24, White);

                ctx->indexcounterforstart++;
                if ((ctx->indexcounterforstart % 10) == 0) {
                    ctx->counterforstart++;
                }

                if (ctx->counterforstart == 3) {
                    ctx->accelstate++;
                    ctx->counterforstart      = 0;
                    ctx->indexcounterforstart = 0;
                    ctx->accel_start_time=uwTick;
                }
            } else {
                ssd1306_SetCursor(32, 40);
                ssd1306_WriteString("Please stop", Font_6x8, White);
                ctx->counterforstart      = 0;
                ctx->indexcounterforstart = 0;
            }
            break;

        case INRUN:
        {
            float speed_kmh = gps->fgSpeed * 3.6f;

            if ((speed_kmh >= 50.0f) && (ctx->flag_50kmh == 0)) {
                ctx->time50kmh  = (float)(uwTick-ctx->accel_start_time)/1000.0f;
                ctx->flag_50kmh = 1;
            }

            if ((speed_kmh >= 100.0f) && (ctx->flag_100kmh == 0)) {
                ctx->time100kmh  = (float)(uwTick-ctx->accel_start_time)/1000.0f;
                ctx->flag_100kmh = 1;
                ctx->accelstate++;
            }

            ssd1306_SetCursor(32, 40);
            snprintf((char *)bufferscreen, 15, "%0.1f", speed_kmh);
            ssd1306_WriteString((char *)bufferscreen, Font_16x24, White);

            if ((buttons->BTN_B_LONG >= 1) || ((ctx->flag_100kmh == 1)&&(ctx->flag_50kmh == 1))) {
                ctx->accelstate++;
                buttons->BTN_B_LONG = 0;
            }
        }
            break;

        case RESULT:
            ssd1306_Fill(Black);

            ssd1306_SetCursor(32, 32);
            ssd1306_WriteString("0-50kmh", Font_6x8, White);
            ssd1306_SetCursor(32, 40);
            snprintf((char *)bufferscreen, 15, "%0.1fs", ctx->time50kmh);
            ssd1306_WriteString((char *)bufferscreen, Font_6x8, White);

            ssd1306_SetCursor(32, 48);
            ssd1306_WriteString("0-100kmh", Font_6x8, White);
            ssd1306_SetCursor(32, 56);
            snprintf((char *)bufferscreen, 15, "%0.1fs", ctx->time100kmh);
            ssd1306_WriteString((char *)bufferscreen, Font_6x8, White);

            if ((buttons->BTN_A >= 1) ||(buttons->BTN_B >= 1) ||(buttons->BTN_A_LONG >= 1)) {

                ctx->time50kmh   = 0.0f;
                ctx->time100kmh  = 0.0f;
                ctx->flag_50kmh  = 0;
                ctx->flag_100kmh = 0;
                ctx->accelstate  = WAITFORGPS;
            }

            if (buttons->BTN_A >= 1) {
                ctx->state -= 4;
                buttons->BTN_A = 0;
                buttons->BTN_B = 0;
            }
            if (buttons->BTN_A_LONG >= 1) {
                ctx->state--;
                buttons->BTN_A      = 0;
                buttons->BTN_B      = 0;
                buttons->BTN_A_LONG = 0;
            }
            if (buttons->BTN_B >= 1) {
                buttons->BTN_B = 0;
            }
            break;

        default:
            ctx->accelstate = WAITFORGPS;
            break;
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

