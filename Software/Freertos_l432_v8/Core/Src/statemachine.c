/*
 * statemachine.c
 *
 *  Created on: May 27, 2024
 *      Author: mathi
 */

#include "statemachine.h"

extern uint8_t bufferscreen[50];
extern AdcContext_t gAdc;

static void utility_function(AppStateMachineContext *ctx, GNSS_StateHandle *gps) {

	if (gps->fixType >= 2 && ctx->rtc_synced == 0) {
		ctx->HR = gps->hour;
		ctx->MINUTE = gps->min;
		ctx->SEC = gps->sec;
		ctx->JOURS = gps->day;
		ctx->MOIS = gps->month;
		ctx->ANNEE = (uint16_t) gps->year;
		set_time(ctx->HR, ctx->MINUTE, ctx->SEC);
		ctx->rtc_synced = 1;
	}

}

void StateMachine_Run(AppStateMachineContext *ctx, GNSS_StateHandle *gps,
		Buttons_t *buttons, AdcContext_t *gAdc, SDCard_struct *sd) {
	utility_function(ctx, gps);

	switch (ctx->state) {
	/* --------------------------------------------------------------------- */
	/*                              STATE_SPEED                              */
	/* --------------------------------------------------------------------- */
	case STATE_SPEED: {
		ssd1306_Fill(Black);

		/* Calcul vitmax + pace */
		if (gps->fgSpeed >= ctx->vitmax) {
			ctx->vitmax = gps->fgSpeed;
		}

		float pace = 0.0f;
		float sec = 0.0f;

		if (gps->fgSpeed != 0.0f) {
			pace = 1000.0f / (60.0f * gps->fgSpeed);
			sec = (pace - floorf(pace)) * 60.0f;
		} else {
			pace = 99.0f;
			sec = 59.0f;
		}

		float speed_kmh = gps->fgSpeed * 3.6f;
		float vitmax_kmh = ctx->vitmax * 3.6f;

		switch (ctx->spdstate) {
		case STATE_GROS:
			ssd1306_SetCursor(32, 32);
#ifndef USE_FAST_PRINT
            snprintf((char *)bufferscreen, 15, "%0.1f", speed_kmh);
#else
			fast_printf((char*) bufferscreen, NULL, speed_kmh, TYPE_FLOAT, 1,
					POS_PREFIX);
#endif
			ssd1306_WriteString((char*) bufferscreen, Font_16x24, White);

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

#ifndef USE_FAST_PRINT
            snprintf((char *)bufferscreen, 15, "%0.1f", vitmax_kmh);
#else
			fast_printf((char*) bufferscreen, NULL, vitmax_kmh, TYPE_FLOAT, 1,
					POS_PREFIX);
#endif

			ssd1306_WriteString((char*) bufferscreen, Font_16x24, White);
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
#ifndef USE_FAST_PRINT
        	            snprintf((char *)bufferscreen, 15, "%0.0fmin%0.0fs", floorf(pace), floorf(sec));
        	#else
			uint32_t p = (uint32_t) pace;
			uint32_t s = (uint32_t) sec;
			uint8_t idx = 0;

			if (p >= 100)
				bufferscreen[idx++] = (p / 100) + '0';
			if (p >= 10)
				bufferscreen[idx++] = ((p / 10) % 10) + '0';
			bufferscreen[idx++] = (p % 10) + '0';

			bufferscreen[idx++] = 'm';
			bufferscreen[idx++] = 'i';
			bufferscreen[idx++] = 'n';

			bufferscreen[idx++] = (s / 10) + '0';
			bufferscreen[idx++] = (s % 10) + '0';

			bufferscreen[idx++] = 's';
			bufferscreen[idx++] = '\0';
#endif
			ssd1306_WriteString((char*) bufferscreen, Font_6x8, White);
			ssd1306_SetCursor(32, 42);
			ssd1306_WriteString("pace", Font_6x8, White);
			ssd1306_SetCursor(32, 56);
#ifndef USE_FAST_PRINT
            snprintf((char *)bufferscreen, 15, "%0.1f", speed_kmh);
#else
			fast_printf((char*) bufferscreen, "kmh", speed_kmh, TYPE_FLOAT, 1,
					POS_SUFFIX);
#endif

			ssd1306_WriteString((char*) bufferscreen, Font_6x8, White);

			batterygauge(gAdc->vbat, 79, 43, 1);

			if (buttons->BTN_B >= 1) {
				ctx->spdstate -= 2; /* retour au début du cycle */
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
			ctx->state += 4;
			buttons->BTN_A = 0;
			buttons->BTN_B = 0;
			buttons->BTN_A_LONG = 0;
		}

		if (buttons->BTN_B_LONG >= 1) {
			ctx->vitmax = 0.0f;
			buttons->BTN_B_LONG = 0;
		}
	}
		break;

		/* --------------------------------------------------------------------- */
		/*                             STATE_BALISE                              */
		/* --------------------------------------------------------------------- */
	case STATE_BALISE: {
		ssd1306_Fill(Black);

		switch (ctx->balisestate) {
		case BALISESTATE1:
			ssd1306_SetCursor(32, 32);
			if ((sd->isthereasdcard == 0) || (sd->mscconnected == 1)
					|| (gps->numSV < 3)) {
				if (sd->mscconnected == 1) {
					ssd1306_WriteString("usb", Font_7x10, White);
				}
				if (sd->isthereasdcard == 0) {
					ssd1306_SetCursor(32, 42);
					ssd1306_WriteString("no sd", Font_7x10, White);
				}
				if (gps->numSV < 3) {
					ssd1306_SetCursor(32, 52);
					ssd1306_WriteString("no GPS", Font_7x10, White);
				}
			}

			else {
				ssd1306_SetCursor(32, 32);
				ssd1306_WriteString("OK", Font_16x24, White);
				ssd1306_SetCursor(32, 56);

#ifndef USE_FAST_PRINT
            	snprintf((char *)bufferscreen, 15, "sat:%d",gps->numSV);
#else
				fast_printf((char*) bufferscreen, "sat:", gps->numSV, TYPE_INT,
						1, POS_PREFIX);
#endif

				ssd1306_WriteString((char*) bufferscreen, Font_6x8, White);

				if (buttons->BTN_B_LONG >= 1) {
					ctx->balisestate++;
					buttons->BTN_B_LONG = 0;
					buttons->BTN_A = 0;
					ctx->oldlat = gps->fLat;
					ctx->oldlong = gps->fLon;
					sd->is_recording = 1;
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
				buttons->BTN_B = 0;
			}
			break;

		case BALISESTATE2:
			ssd1306_SetCursor(32, 32);

			if (sd->is_recording == 0) {
				ctx->balisestate = BALISESTATE1;
				break;
			}

			uint8_t any_button_pressed = (buttons->BTN_A >= 1
					|| buttons->BTN_B >= 1 || buttons->BTN_A_LONG >= 1
					|| buttons->BTN_B_LONG >= 1);
			if (any_button_pressed) {
				ctx->last_action_tick = HAL_GetTick();
			}
			if ((HAL_GetTick() - ctx->last_action_tick) > 20000) {

				if ((HAL_GetTick() / 500) % 2) {
					ssd1306_SetCursor(40, 38);
					ssd1306_WriteString("REC", Font_16x24, White);
				}

				if (any_button_pressed) {
					buttons->BTN_A = 0;
					buttons->BTN_B = 0;
					buttons->BTN_A_LONG = 0;
					buttons->BTN_B_LONG = 0;
				}

			} else {
				switch (ctx->ecranstate) {
				case ECRANBALISESTATE1:

#ifndef USE_FAST_PRINT
            snprintf((char *)bufferscreen, 50, "%0.3f",(float) gps->distance_parcouru);
#else
					fast_printf((char*) bufferscreen, NULL,
							gps->distance_parcouru, TYPE_FLOAT, 3, POS_PREFIX);
#endif
					ssd1306_WriteString((char*) bufferscreen, Font_7x10, White);
					ssd1306_SetCursor(32, 42);
					ssd1306_WriteString("Dist(km)", Font_6x8, White);

					if (buttons->BTN_B >= 1) {
						ctx->ecranstate++;
						buttons->BTN_B = 0;
						buttons->BTN_A = 0;
					}
					break;

				case ECRANBALISESTATE2:
#ifndef USE_FAST_PRINT
            	snprintf((char *)bufferscreen, 50, "%0.1f", gps->fgSpeed * 3.6f);
#else
					fast_printf((char*) bufferscreen, NULL, gps->fgSpeed * 3.6f,
							TYPE_FLOAT, 1, POS_PREFIX);
#endif

					ssd1306_WriteString((char*) bufferscreen, Font_7x10, White);

					ssd1306_SetCursor(32, 42);
					ssd1306_WriteString("Vit(kmh)", Font_6x8, White);

					if (buttons->BTN_B >= 1) {
						ctx->ecranstate++;
						buttons->BTN_B = 0;
						buttons->BTN_A = 0;
					}
					break;

				case ECRANBALISESTATE3:
#ifndef USE_FAST_PRINT
                snprintf((char *)bufferscreen, 50, "%0.1f", gAdc->vbat);
#else
					fast_printf((char*) bufferscreen, NULL, gAdc->vbat,
							TYPE_FLOAT, 1, POS_PREFIX);
#endif
					ssd1306_WriteString((char*) bufferscreen, Font_7x10, White);

					ssd1306_SetCursor(32, 42);
					ssd1306_WriteString("Vbat(V)", Font_6x8, White);

					if (buttons->BTN_B >= 1) {
						ctx->ecranstate++;
						buttons->BTN_B = 0;
						buttons->BTN_A = 0;
					}
					break;

				case ECRANBALISESTATE4:
#ifndef USE_FAST_PRINT
            	snprintf((char *)bufferscreen, 50, "%0.1f", ctx->vitmax * 3.6f);
#else
					fast_printf((char*) bufferscreen, NULL, ctx->vitmax * 3.6f,
							TYPE_FLOAT, 1, POS_PREFIX);
#endif

					ssd1306_WriteString((char*) bufferscreen, Font_7x10, White);

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
#ifndef USE_FAST_PRINT
            snprintf((char *)bufferscreen, 50, "%0.2fV", gAdc->vbat);
#else
				fast_printf((char*) bufferscreen, "V", gAdc->vbat, TYPE_FLOAT,
						2, POS_SUFFIX);
#endif

				ssd1306_WriteString((char*) bufferscreen, Font_6x8, White);

				ssd1306_SetCursor(65, 52);
#ifndef USE_FAST_PRINT
            snprintf((char *)bufferscreen, 50, "sat=%d", gps->numSV);
#else
				fast_printf((char*) bufferscreen, "sat=", gps->numSV, TYPE_INT,
						1, POS_PREFIX);
#endif
				ssd1306_WriteString((char*) bufferscreen, Font_6x8, White);

				if (buttons->BTN_B_LONG >= 1) {
					ctx->balisestate--;
					buttons->BTN_B_LONG = 0;
					buttons->BTN_A = 0;
					sd->is_recording = 0;
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
	case STATE_POS: {
		ssd1306_Fill(Black);

		switch (ctx->posstate) {
		case STATE_SUMMARY1:
			ssd1306_SetCursor(32, 32);
			ssd1306_WriteString("Latitude:", Font_6x8, White);

#ifndef USE_FAST_PRINT
            snprintf((char *)bufferscreen, 15, "%0.7f", gps->fLat);
#else
			fast_printf((char*) bufferscreen, NULL, gps->fLat, TYPE_FLOAT, 7,
					POS_PREFIX);
#endif

			ssd1306_SetCursor(32, 40);
			ssd1306_WriteString((char*) bufferscreen, Font_6x8, White);

			ssd1306_SetCursor(32, 48);
			ssd1306_WriteString("Longitude:", Font_6x8, White);

#ifndef USE_FAST_PRINT
            snprintf((char *)bufferscreen, 15, "%0.7f", gps->fLon);
#else
			fast_printf((char*) bufferscreen, NULL, gps->fLon, TYPE_FLOAT, 7,
					POS_PREFIX);
#endif
			ssd1306_SetCursor(32, 56);
			ssd1306_WriteString((char*) bufferscreen, Font_6x8, White);

			if (buttons->BTN_B >= 1) {
				ctx->posstate++;
				buttons->BTN_B = 0;
			}
			break;

		case STATE_INFO:
			ssd1306_Fill(Black);

			ssd1306_SetCursor(32, 32);
#ifndef USE_FAST_PRINT
                        snprintf((char *)bufferscreen, 15, "hacc=%0.2fm", gps->fhACC);
            #else
			bufferscreen[0] = 'h';
			bufferscreen[1] = 'a';
			bufferscreen[2] = 'c';
			bufferscreen[3] = 'c';
			bufferscreen[4] = '=';
			fast_printf((char*) bufferscreen + 5, "m", (float) gps->fhACC,
					TYPE_FLOAT, 2, POS_SUFFIX);
#endif
			ssd1306_WriteString((char*) bufferscreen, Font_7x10, White);

			ssd1306_SetCursor(32, 42);
#ifndef USE_FAST_PRINT
                        snprintf((char *)bufferscreen, 20, "v=%0.2fV", gAdc->vbat);
            #else
			bufferscreen[0] = 'v';
			bufferscreen[1] = '=';
			fast_printf((char*) bufferscreen + 2, "V", (float) gAdc->vbat,
					TYPE_FLOAT, 2, POS_SUFFIX);
#endif
			ssd1306_WriteString((char*) bufferscreen, Font_7x10, White);

			ssd1306_SetCursor(32, 52);
#ifndef USE_FAST_PRINT
                        snprintf((char *)bufferscreen, 15, "T=%0.2fC", gAdc->temp);
            #else
			bufferscreen[0] = 'T';
			bufferscreen[1] = '=';
			fast_printf((char*) bufferscreen + 2, "C", (float) gAdc->temp,
					TYPE_FLOAT, 2, POS_SUFFIX);
#endif
			ssd1306_WriteString((char*) bufferscreen, Font_7x10, White);

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
			ssd1306_SetCursor(32, 32);
			ssd1306_WriteString("Satnum", Font_6x8, White);

#ifndef USE_FAST_PRINT
            snprintf((char *)bufferscreen, 20, "%d sat", gps->numSV);
#else
			fast_printf((char*) bufferscreen, " sat", gps->numSV, TYPE_INT, 1,
					POS_SUFFIX);
#endif

			ssd1306_SetCursor(32, 40);
			ssd1306_WriteString((char*) bufferscreen, Font_6x8, White);

			ssd1306_SetCursor(32, 48);
			ssd1306_WriteString("DateJ/M/Y", Font_6x8, White);

			ssd1306_SetCursor(32, 56);
#ifndef USE_FAST_PRINT
                        snprintf((char *)bufferscreen, 15, "%d/%d/%d", gps->day, gps->month, gps->year);
            #else

			// Jour
			bufferscreen[0] = (gps->day / 10) + '0';
			bufferscreen[1] = (gps->day % 10) + '0';
			bufferscreen[2] = '/';

			// Mois
			bufferscreen[3] = (gps->month / 10) + '0';
			bufferscreen[4] = (gps->month % 10) + '0';
			bufferscreen[5] = '/';

			// Année (Si gps->year est sur 4 chiffres, ex: 2026)
			bufferscreen[6] = (gps->year / 1000) + '0';
			bufferscreen[7] = ((gps->year / 100) % 10) + '0';
			bufferscreen[8] = ((gps->year / 10) % 10) + '0';
			bufferscreen[9] = (gps->year % 10) + '0';
			bufferscreen[10] = '\0'; // Fin de chaîne
#endif
			ssd1306_WriteString((char*) bufferscreen, Font_6x8, White);

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
			ssd1306_WriteString("altitude:", Font_6x8, White);

#ifndef USE_FAST_PRINT
            snprintf((char *)bufferscreen, 20, "%0.2f m", gps->fhMSL);
#else
			fast_printf((char*) bufferscreen, " m", gps->fhMSL, TYPE_FLOAT, 2,
					POS_SUFFIX);
#endif

			ssd1306_SetCursor(32, 40);
			ssd1306_WriteString((char*) bufferscreen, Font_6x8, White);

			ssd1306_SetCursor(32, 48);
			ssd1306_WriteString("Accuracy(m)", Font_6x8, White);

#ifndef USE_FAST_PRINT
            snprintf((char *)bufferscreen, 15, "%0.1f", gps->fvACC);
#else
			fast_printf((char*) bufferscreen, NULL, gps->fvACC, TYPE_FLOAT, 1,
					POS_SUFFIX);
#endif

			ssd1306_SetCursor(32, 56);
			ssd1306_WriteString((char*) bufferscreen, Font_6x8, White);

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
#ifndef USE_FAST_PRINT
                        snprintf((char *)bufferscreen, 15, "%02d:%02d", ctx->HR, ctx->MINUTE);
            #else
			// Méthode directe ultra-rapide (Gère les zéros automatiquement)
			bufferscreen[0] = (ctx->HR / 10) + '0';
			bufferscreen[1] = (ctx->HR % 10) + '0';
			bufferscreen[2] = ':';
			bufferscreen[3] = (ctx->MINUTE / 10) + '0';
			bufferscreen[4] = (ctx->MINUTE % 10) + '0';
			bufferscreen[5] = '\0'; // Fin de chaîne
#endif
			ssd1306_WriteString((char*) bufferscreen, Font_7x10, White);

			ssd1306_SetCursor(32, 52);
#ifndef USE_FAST_PRINT
                        snprintf((char *)bufferscreen, 15, "%02d sec", ctx->SEC);
            #else
			// Méthode directe pour garantir le "0" devant les chiffres < 10
			bufferscreen[0] = (ctx->SEC / 10) + '0';
			bufferscreen[1] = (ctx->SEC % 10) + '0';
			bufferscreen[2] = ' ';
			bufferscreen[3] = 's';
			bufferscreen[4] = 'e';
			bufferscreen[5] = 'c';
			bufferscreen[6] = '\0';
#endif
			ssd1306_WriteString((char*) bufferscreen, Font_7x10, White);

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
			buttons->BTN_A = 0;
			buttons->BTN_B = 0;
			buttons->BTN_A_LONG = 0;
		}
	}
		break;
		/* --------------------------------------------------------------------- */
		/* STATE_SETTINGS                                                        */
		/* --------------------------------------------------------------------- */
	case STATE_SETTINGS: {

		// --- AFFICHAGE ET CONFIGURATION ---
		switch (ctx->settingstate) {
		case SETTING_FREQ:
			ssd1306_SetCursor(32, 32);
			ssd1306_WriteString("Freq:", Font_7x10, White);

			ssd1306_SetCursor(62, 32);
			if (sd->frequency == SD_FREQ_1HZ)
				ssd1306_WriteString("1Hz ", Font_7x10, White);
			else if (sd->frequency == SD_FREQ_10HZ)
				ssd1306_WriteString("10Hz ", Font_7x10, White);
			else
				ssd1306_WriteString("0.1Hz", Font_7x10, White);

			if (buttons->BTN_B >= 1) {
				if (sd->frequency == SD_FREQ_1HZ)
					sd->frequency = SD_FREQ_1HZ;
				else if (sd->frequency == SD_FREQ_10HZ)
					sd->frequency = SD_FREQ_10HZ;
				else
					sd->frequency = SD_FREQ_01HZ;
				buttons->BTN_B = 0;
			}
			break;

		case SETTING_FORMAT:
			ssd1306_SetCursor(42, 32);
			ssd1306_WriteString("Type:", Font_7x10, White);

			ssd1306_SetCursor(42, 62);
			if (sd->format == SD_FORMAT_GPX)
				ssd1306_WriteString("GPX", Font_7x10, White);
			else
				ssd1306_WriteString("CSV", Font_7x10, White);

			// Clic B Court : Basculer le format
			if (buttons->BTN_B >= 1) {
				sd->format =(sd->format == SD_FORMAT_GPX) ?SD_FORMAT_CSV : SD_FORMAT_GPX;
				buttons->BTN_B = 0;
			}
			break;

		default:
			ctx->settingstate = SETTING_FREQ;
			break;
		}

		// --- BANDEAU D'AIDE (BAS DE L'ÉCRAN) ---
		ssd1306_SetCursor(32, 56);
		ssd1306_WriteString("Hold B : Next", Font_6x8, White);

		if (buttons->BTN_B_LONG >= 1) {
			ctx->settingstate =(ctx->settingstate == SETTING_FREQ) ?SETTING_FORMAT : SETTING_FREQ;
			buttons->BTN_B_LONG = 0;
			buttons->BTN_A = 0;
		}

		if (buttons->BTN_A >= 1) {
			ctx->state = STATE_SPEED;
			buttons->BTN_A = 0;
			buttons->BTN_B = 0;
		}
		if (buttons->BTN_A_LONG >= 1) {
			ctx->state--;
			buttons->BTN_A_LONG = 0;
			buttons->BTN_B = 0;
		}
	}
		break;

	case STATE_SCREENSAVER: {
		if (sd->is_recording) {
			if ((HAL_GetTick() / 500) % 2) {
				ssd1306_SetCursor(40, 20);
				ssd1306_WriteString("REC", Font_16x24, White);
			}
		} else {

			batterygauge_screensaver(gAdc->vbat, 32, 32);
		}
	}
		break;
	default:
		ctx->state = STATE_SPEED;
		break;
	}
}

void set_time(uint8_t hr, uint8_t min, uint8_t sec) {
	RTC_TimeTypeDef sTime = { 0 };
	sTime.Hours = hr;
	sTime.Minutes = min;
	sTime.Seconds = sec;
	sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
	sTime.StoreOperation = RTC_STOREOPERATION_RESET;
	if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN) != HAL_OK) {
		Error_Handler();
	}
}

void set_date(uint8_t year, uint8_t month, uint8_t date, uint8_t day) // monday = 1
{
	RTC_DateTypeDef sDate = { 0 };
	sDate.WeekDay = day;
	sDate.Month = month;
	sDate.Date = date;
	sDate.Year = year;
	if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN) != HAL_OK) {
		Error_Handler();
	}

	HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR1, 0x2345);  // backup register
}

void get_time_date(AppStateMachineContext *context) {
	RTC_DateTypeDef gDate;
	RTC_TimeTypeDef gTime;

	/* Get the RTC current Time */
	HAL_RTC_GetTime(&hrtc, &gTime, RTC_FORMAT_BIN);
	/* Get the RTC current Date */
	HAL_RTC_GetDate(&hrtc, &gDate, RTC_FORMAT_BIN);

	context->SEC = gTime.Seconds;
	context->HR = gTime.Hours;
	context->MINUTE = gTime.Minutes;
	context->JOURS = gDate.Date;
	context->MOIS = gDate.Month;
	context->ANNEE = gDate.Year + 2000;
}

