/*
 * statemachine.c
 *
 * Created on: May 27, 2024
 * Author: mathi
 */

#include "statemachine.h"
#include <string.h>
#include <math.h>

extern uint8_t bufferscreen[50];
extern AdcContext_t gAdc;
extern uint32_t lat_index[182];

extern uint8_t Is_SD_Present(void);
extern uint8_t Is_Connected_To_PC(void);

/* ===================================================================== */
/* FONCTIONS UTILITAIRES                         */
/* ===================================================================== */

static void FindClosestCity(float my_lat, float my_lon, AppStateMachineContext *ctx, SDCard_struct *sd) {
	// SÉCURITÉ 1 : Ne rien faire si le PC a la main sur la SD ou si pas de SD
	if (!Is_SD_Present() || Is_Connected_To_PC()) {
		return;
	}

	// Montage (au cas où la tâche SD_Manager l'aurait démonté)
	if (!sd->is_mounted) {
		if (f_mount(&sd->FatFs, "", 1) == FR_OK) {
			sd->is_mounted = 1;
		} else {
			return;
		}
	}

	FIL f_villes;
	UINT bytesRead;
	CityRecord current_city;
	float min_dist_sq = 999999999.0f;
	float cos_lat = cosf(my_lat * DEG_TO_RAD);

	if (f_open(&f_villes, "VILLES.BIN", FA_READ) == FR_OK) {
		int lat_deg = (int) floorf(my_lat);
		uint32_t offset_start = lat_index[lat_deg + 90];
		uint32_t offset_end = lat_index[lat_deg + 91];

		f_lseek(&f_villes, offset_start);
		uint32_t current_offset = offset_start;

		while (current_offset < offset_end) {
			f_read(&f_villes, &current_city, sizeof(CityRecord), &bytesRead);
			if (bytesRead != sizeof(CityRecord))
				break;

			float c_lat = (float) current_city.lat / 10000000.0f;
			float c_lon = (float) current_city.lon / 10000000.0f;

			float dx = (c_lon - my_lon) * cos_lat;
			float dy = (c_lat - my_lat);
			float dist_sq = (dx * dx) + (dy * dy);

			if (dist_sq < min_dist_sq) {
				min_dist_sq = dist_sq;
				strncpy(ctx->city_name, current_city.name, 17);
				ctx->city_name[17] = '\0';
				ctx->city_country[0] = current_city.country[0];
				ctx->city_country[1] = current_city.country[1];
				ctx->city_country[2] = '\0';
				ctx->city_pop = current_city.pop;
				ctx->city_dist_km = sqrtf(dist_sq) * 111.32f;
			}
			current_offset += sizeof(CityRecord);
		}
		f_close(&f_villes);
	}
}


/* ===================================================================== */
/* GESTION GLOBALE DE LA NAVIGATION (BTN_A)                              */
/* ===================================================================== */
static void Handle_Global_Navigation(AppStateMachineContext *ctx, Buttons_t *buttons, SDCard_struct *sd) {

    if (ctx->state == STATE_SCREENSAVER) return;

    if (ctx->state == STATE_BALISE && sd->is_recording == 1) {
        buttons->BTN_A = 0;
        buttons->BTN_A_LONG = 0;
        return;
    }
    if (buttons->BTN_A >= 1) {
        if (ctx->state == STATE_SETTINGS) {
            ctx->state = STATE_SPEED;
        } else {
            ctx->state++;
        }

        ctx->city_search_done = 0;
        buttons->BTN_A = 0;
        buttons->BTN_B = 0;
    }
    if (buttons->BTN_A_LONG >= 1) {
        if (ctx->state == STATE_SPEED) {
            ctx->state = STATE_SETTINGS;
        } else {
            ctx->state--;
        }

        ctx->city_search_done = 0;
        buttons->BTN_A_LONG = 0;
        buttons->BTN_A = 0;
        buttons->BTN_B = 0;
    }
}

/* ===================================================================== */
/* HANDLERS DES ÉTATS (STATIQUES)                      */
/* ===================================================================== */

static void Handle_State_Speed(AppStateMachineContext *ctx, GNSS_StateHandle *gps, Buttons_t *buttons, AdcContext_t *gAdc, SDCard_struct *sd) {
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
		snprintf((char*) bufferscreen, 15, "%0.1f", speed_kmh);
#else
		fast_printf((char*) bufferscreen, NULL, speed_kmh, TYPE_FLOAT, 1, POS_PREFIX);
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
		snprintf((char*) bufferscreen, 15, "%0.1f", vitmax_kmh);
#else
		fast_printf((char*) bufferscreen, NULL, vitmax_kmh, TYPE_FLOAT, 1, POS_PREFIX);
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
		snprintf((char*) bufferscreen, 15, "%0.0fmin%0.0fs", floorf(pace), floorf(sec));
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
		snprintf((char*) bufferscreen, 15, "%0.1f", speed_kmh);
#else
		fast_printf((char*) bufferscreen, "kmh", speed_kmh, TYPE_FLOAT, 1, POS_SUFFIX);
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

	if (buttons->BTN_B_LONG >= 1) {
		ctx->vitmax = 0.0f;
		buttons->BTN_B_LONG = 0;
	}
}

static void Handle_State_Balise(AppStateMachineContext *ctx, GNSS_StateHandle *gps, Buttons_t *buttons, AdcContext_t *gAdc, SDCard_struct *sd) {
	ssd1306_Fill(Black);

	switch (ctx->balisestate) {
	case BALISESTATE1:
		ssd1306_SetCursor(32, 32);
		if ((sd->isthereasdcard == 0) || (sd->mscconnected == 1) || (gps->numSV < 3)) {
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
		} else {
			ssd1306_SetCursor(32, 32);
			ssd1306_WriteString("OK", Font_16x24, White);
			ssd1306_SetCursor(32, 56);

#ifndef USE_FAST_PRINT
			snprintf((char*) bufferscreen, 15, "sat:%d", gps->numSV);
#else
			fast_printf((char*) bufferscreen, "sat:", gps->numSV, TYPE_INT, 1, POS_PREFIX);
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
		break;

	case BALISESTATE2:
		ssd1306_SetCursor(32, 32);

		if (sd->is_recording == 0) {
			ctx->balisestate = BALISESTATE1;
			break;
		}
		switch (ctx->ecranstate) {
		case ECRANBALISESTATE1:
#ifndef USE_FAST_PRINT
			snprintf((char*) bufferscreen, 50, "%0.3f", (float) gps->distance_parcouru);
#else
			fast_printf((char*) bufferscreen, NULL, gps->distance_parcouru, TYPE_FLOAT, 3, POS_PREFIX);
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
			snprintf((char*) bufferscreen, 50, "%0.1f", gps->fgSpeed * 3.6f);
#else
			fast_printf((char*) bufferscreen, NULL, gps->fgSpeed * 3.6f, TYPE_FLOAT, 1, POS_PREFIX);
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
			snprintf((char*) bufferscreen, 50, "%0.1f", gAdc->vbat);
#else
			fast_printf((char*) bufferscreen, NULL, gAdc->vbat, TYPE_FLOAT, 1, POS_PREFIX);
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
			snprintf((char*) bufferscreen, 50, "%0.1f", ctx->vitmax * 3.6f);
#else
			fast_printf((char*) bufferscreen, NULL, ctx->vitmax * 3.6f, TYPE_FLOAT, 1, POS_PREFIX);
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
		snprintf((char*) bufferscreen, 50, "%0.2fV", gAdc->vbat);
#else
		fast_printf((char*) bufferscreen, "V", gAdc->vbat, TYPE_FLOAT, 2, POS_SUFFIX);
#endif

		ssd1306_WriteString((char*) bufferscreen, Font_6x8, White);

		ssd1306_SetCursor(65, 52);
#ifndef USE_FAST_PRINT
		snprintf((char*) bufferscreen, 50, "sat=%d", gps->numSV);
#else
		fast_printf((char*) bufferscreen, "sat=", gps->numSV, TYPE_INT, 1, POS_PREFIX);
#endif
		ssd1306_WriteString((char*) bufferscreen, Font_6x8, White);

		if (buttons->BTN_B_LONG >= 1) {
			ctx->balisestate--;
			buttons->BTN_B_LONG = 0;
			buttons->BTN_A = 0;
			sd->is_recording = 0;
		}
		break;
	default:
		ctx->balisestate = BALISESTATE1;
		break;
	}
}

static void Handle_State_Pos(AppStateMachineContext *ctx, GNSS_StateHandle *gps, Buttons_t *buttons, AdcContext_t *gAdc, SDCard_struct *sd) {

	// 1. Nettoyage de l'écran (commun à tous les sous-états)
	ssd1306_Fill(Black);

	// 2. AFFICHAGE SPÉCIFIQUE
	switch (ctx->posstate) {
	case STATE_SUMMARY1:
		ssd1306_SetCursor(32, 32);
		ssd1306_WriteString("Latitude:", Font_6x8, White);

#ifndef USE_FAST_PRINT
		snprintf((char*) bufferscreen, 15, "%0.7f", gps->fLat);
#else
		fast_printf((char*) bufferscreen, NULL, gps->fLat, TYPE_FLOAT, 7, POS_PREFIX);
#endif
		ssd1306_SetCursor(32, 40);
		ssd1306_WriteString((char*) bufferscreen, Font_6x8, White);

		ssd1306_SetCursor(32, 48);
		ssd1306_WriteString("Longitude:", Font_6x8, White);

#ifndef USE_FAST_PRINT
		snprintf((char*) bufferscreen, 15, "%0.7f", gps->fLon);
#else
		fast_printf((char*) bufferscreen, NULL, gps->fLon, TYPE_FLOAT, 7, POS_PREFIX);
#endif
		ssd1306_SetCursor(32, 56);
		ssd1306_WriteString((char*) bufferscreen, Font_6x8, White);
		break;

	case STATE_INFO:
		ssd1306_SetCursor(32, 32);
#ifndef USE_FAST_PRINT
		snprintf((char*) bufferscreen, 15, "hacc=%0.2fm", gps->fhACC);
#else
		bufferscreen[0] = 'h';
		bufferscreen[1] = 'a';
		bufferscreen[2] = 'c';
		bufferscreen[3] = 'c';
		bufferscreen[4] = '=';
		fast_printf((char*) bufferscreen + 5, "m", (float) gps->fhACC, TYPE_FLOAT, 2, POS_SUFFIX);
#endif
		ssd1306_WriteString((char*) bufferscreen, Font_7x10, White);

		ssd1306_SetCursor(32, 42);
#ifndef USE_FAST_PRINT
		snprintf((char*) bufferscreen, 20, "v=%0.2fV", gAdc->vbat);
#else
		bufferscreen[0] = 'v';
		bufferscreen[1] = '=';
		fast_printf((char*) bufferscreen + 2, "V", (float) gAdc->vbat, TYPE_FLOAT, 2, POS_SUFFIX);
#endif
		ssd1306_WriteString((char*) bufferscreen, Font_7x10, White);

		ssd1306_SetCursor(32, 52);
#ifndef USE_FAST_PRINT
		snprintf((char*) bufferscreen, 15, "T=%0.2fC", gAdc->temp);
#else
		bufferscreen[0] = 'T';
		bufferscreen[1] = '=';
		fast_printf((char*) bufferscreen + 2, "C", (float) gAdc->temp, TYPE_FLOAT, 2, POS_SUFFIX);
#endif
		ssd1306_WriteString((char*) bufferscreen, Font_7x10, White);
		break;

	case STATE_INFO2:
		ssd1306_SetCursor(32, 32);
		ssd1306_WriteString("Satnum", Font_6x8, White);

#ifndef USE_FAST_PRINT
		snprintf((char*) bufferscreen, 20, "%d sat", gps->numSV);
#else
		fast_printf((char*) bufferscreen, " sat", gps->numSV, TYPE_INT, 1, POS_SUFFIX);
#endif
		ssd1306_SetCursor(32, 40);
		ssd1306_WriteString((char*) bufferscreen, Font_6x8, White);

		ssd1306_SetCursor(32, 48);
		ssd1306_WriteString("DateJ/M/Y", Font_6x8, White);

		ssd1306_SetCursor(32, 56);
#ifndef USE_FAST_PRINT
		snprintf((char*) bufferscreen, 15, "%d/%d/%d", gps->day, gps->month, gps->year);
#else
		bufferscreen[0] = (gps->day / 10) + '0';
		bufferscreen[1] = (gps->day % 10) + '0';
		bufferscreen[2] = '/';
		bufferscreen[3] = (gps->month / 10) + '0';
		bufferscreen[4] = (gps->month % 10) + '0';
		bufferscreen[5] = '/';
		bufferscreen[6] = (gps->year / 1000) + '0';
		bufferscreen[7] = ((gps->year / 100) % 10) + '0';
		bufferscreen[8] = ((gps->year / 10) % 10) + '0';
		bufferscreen[9] = (gps->year % 10) + '0';
		bufferscreen[10] = '\0';
#endif
		ssd1306_WriteString((char*) bufferscreen, Font_6x8, White);
		break;

	case STATE_ALT:
		ssd1306_SetCursor(32, 32);
		ssd1306_WriteString("altitude:", Font_6x8, White);

#ifndef USE_FAST_PRINT
		snprintf((char*) bufferscreen, 20, "%0.2f m", gps->fhMSL);
#else
		fast_printf((char*) bufferscreen, " m", gps->fhMSL, TYPE_FLOAT, 2, POS_SUFFIX);
#endif
		ssd1306_SetCursor(32, 40);
		ssd1306_WriteString((char*) bufferscreen, Font_6x8, White);

		ssd1306_SetCursor(32, 48);
		ssd1306_WriteString("Accuracy(m)", Font_6x8, White);

#ifndef USE_FAST_PRINT
		snprintf((char*) bufferscreen, 15, "%0.1f", gps->fvACC);
#else
		fast_printf((char*) bufferscreen, NULL, gps->fvACC, TYPE_FLOAT, 1, POS_SUFFIX);
#endif
		ssd1306_SetCursor(32, 56);
		ssd1306_WriteString((char*) bufferscreen, Font_6x8, White);
		break;

	case STATE_HEURE:
		ssd1306_SetCursor(32, 32);
		ssd1306_WriteString("hr GMT:", Font_6x8, White);

		ssd1306_SetCursor(32, 42);
#ifndef USE_FAST_PRINT
		snprintf((char*) bufferscreen, 15, "%02d:%02d", ctx->HR, ctx->MINUTE);
#else
		bufferscreen[0] = (gps->hour / 10) + '0';
		bufferscreen[1] = (gps->hour % 10) + '0';
		bufferscreen[2] = ':';
		bufferscreen[3] = (gps->min / 10) + '0';
		bufferscreen[4] = (gps->min % 10) + '0';
		bufferscreen[5] = '\0';
#endif
		ssd1306_WriteString((char*) bufferscreen, Font_7x10, White);

		ssd1306_SetCursor(32, 52);
#ifndef USE_FAST_PRINT
		snprintf((char*) bufferscreen, 15, "%02d sec", ctx->SEC);
#else
		bufferscreen[0] = (gps->sec / 10) + '0';
		bufferscreen[1] = (gps->sec % 10) + '0';
		bufferscreen[2] = ' ';
		bufferscreen[3] = 's';
		bufferscreen[4] = 'e';
		bufferscreen[5] = 'c';
		bufferscreen[6] = '\0';
#endif
		ssd1306_WriteString((char*) bufferscreen, Font_7x10, White);
		break;

	default:
		ctx->posstate = STATE_SUMMARY1;
		break;
	}

	// 3. GESTION DE LA SOUS-NAVIGATION (Bouton B commun)
	if (buttons->BTN_B >= 1) {
		if (ctx->posstate == STATE_HEURE) {
			ctx->posstate -= 4; // Retour au début (bouclage)
		} else {
			ctx->posstate++;
		}
		buttons->BTN_B = 0;
	}
	else if (buttons->BTN_B_LONG >= 1) {
		if (ctx->posstate != STATE_SUMMARY1) {
			ctx->posstate--; // On recule, sauf si on est déjà au début
		}
		buttons->BTN_B_LONG = 0;
	}
}

static void Handle_State_City(AppStateMachineContext *ctx, GNSS_StateHandle *gps, Buttons_t *buttons, AdcContext_t *gAdc, SDCard_struct *sd) {
	ssd1306_Fill(Black);

	// Variables statiques pour gérer le rafraîchissement temporel
	static uint32_t last_search_time = 0;
	uint32_t current_time = HAL_GetTick();

	// SÉCURITÉ USB & SD (Priorité absolue à l'affichage)
	if (!Is_SD_Present() || Is_Connected_To_PC()) {
		ssd1306_SetCursor(32, 32);
		if (!Is_SD_Present())
			ssd1306_WriteString("No SD", Font_7x10, White);
		else
			ssd1306_WriteString("USB Mode", Font_7x10, White);

		ctx->city_search_done = 0;
	}
	// Pas de signal GPS
	else if (gps->numSV < 3 || gps->fixType < 2) {
		ssd1306_SetCursor(32, 32);
		ssd1306_WriteString("No GPS", Font_7x10, White);
		ctx->city_search_done = 0;
	} else {
		// TRIGGER DE RECHERCHE : Première fois OU Bouton B OU toutes les 30 secondes
		uint8_t force_search = 0;

		if (buttons->BTN_B >= 1) {
			force_search = 1;
			buttons->BTN_B = 0;
		}
		// Si 30000 ms se sont écoulées (30 sec)
		else if (current_time - last_search_time >= 30000) {
			force_search = 1;
		}

		// Exécution de la recherche
		if (ctx->city_search_done == 0 || force_search) {
			ssd1306_SetCursor(32, 32);
			ssd1306_WriteString("Recherche...", Font_7x10, White);
			ssd1306_UpdateScreen(); // Force l'affichage immédiat du texte d'attente

			FindClosestCity(gps->fLat, gps->fLon, ctx, sd);

			ctx->city_search_done = 1;
			last_search_time = HAL_GetTick(); // Reset du chrono
		}
		// Affichage des données
		else {
			ssd1306_SetCursor(32, 32);
			ssd1306_WriteString(ctx->city_name, Font_6x8, White);
			ssd1306_SetCursor(32, 42);
#ifndef USE_FAST_PRINT
			snprintf((char*) bufferscreen, 30, "%s | %.1fkm", ctx->city_country, ctx->city_dist_km);
#else
			bufferscreen[0] = ctx->city_country[0];
			bufferscreen[1] = ctx->city_country[1];
			bufferscreen[2] = ' ';
			bufferscreen[3] = '|';
			bufferscreen[4] = ' ';
			fast_printf((char*) bufferscreen + 5, "km", ctx->city_dist_km, TYPE_FLOAT, 1, POS_SUFFIX);
#endif
			ssd1306_WriteString((char*) bufferscreen, Font_6x8, White);

			// Population (Ex: Pop:250000)
			ssd1306_SetCursor(32, 52);
#ifndef USE_FAST_PRINT
			snprintf((char*) bufferscreen, 30, "Pop:%lu", ctx->city_pop);
#else
			fast_printf((char*) bufferscreen, "Pop:", (uint32_t) ctx->city_pop, TYPE_INT, 1, POS_PREFIX);
#endif
			ssd1306_WriteString((char*) bufferscreen, Font_6x8, White);
		}
	}

}

static void Handle_State_Settings(AppStateMachineContext *ctx, GNSS_StateHandle *gps, Buttons_t *buttons, AdcContext_t *gAdc, SDCard_struct *sd) {
	ssd1306_Fill(Black);
	ssd1306_SetCursor(32, 32);
	ssd1306_WriteString("Frq:", Font_7x10, White);
	ssd1306_SetCursor(32, 42);
	ssd1306_WriteString("Type:", Font_7x10, White);

	ssd1306_SetCursor(62, 32);
	if (sd->frequency == SD_FREQ_1HZ)
		ssd1306_WriteString("1Hz ", Font_7x10, White);
	else if (sd->frequency == SD_FREQ_10HZ)
		ssd1306_WriteString("10Hz ", Font_7x10, White);
	else
		ssd1306_WriteString("0.1Hz", Font_7x10, White);

	ssd1306_SetCursor(62, 42);
	if (sd->format == SD_FORMAT_GPX)
		ssd1306_WriteString("GPX", Font_7x10, White);
	else
		ssd1306_WriteString("CSV", Font_7x10, White);

	switch (ctx->settingstate) {
	case SETTING_FREQ:
		if (buttons->BTN_B >= 1) {
			if (sd->frequency == SD_FREQ_1HZ) {
				sd->frequency = SD_FREQ_10HZ;
			} else if (sd->frequency == SD_FREQ_10HZ) {
				sd->frequency = SD_FREQ_1HZ;
			}
			buttons->BTN_B = 0;
		}
		break;

	case SETTING_FORMAT:
		if (buttons->BTN_B >= 1) {
			if (sd->format == SD_FORMAT_GPX) {
				sd->format = SD_FORMAT_CSV;
			} else if (sd->format == SD_FORMAT_CSV) {
				sd->format = SD_FORMAT_GPX;
			}
			buttons->BTN_B = 0;
		}
		break;

	default:
		ctx->settingstate = SETTING_FREQ;
		break;
	}

	ssd1306_SetCursor(32, 56);
	ssd1306_WriteString("Hold B:Next", Font_6x8, White);

	if (buttons->BTN_B_LONG >= 1) {
		ctx->settingstate = (ctx->settingstate == SETTING_FREQ) ? SETTING_FORMAT : SETTING_FREQ;
		buttons->BTN_B_LONG = 0;
		buttons->BTN_A = 0;
	}
}

static void Handle_State_Screensaver(AppStateMachineContext *ctx, GNSS_StateHandle *gps, Buttons_t *buttons, AdcContext_t *gAdc, SDCard_struct *sd) {
	ssd1306_Fill(Black);
	if (sd->is_recording) {
		if ((HAL_GetTick() / 500) % 2) {
			ssd1306_SetCursor(42, 36);
			ssd1306_WriteString("REC", Font_16x24, White);
		}
	} else {
		batterygauge_screensaver(gAdc->vbat, 32, 32);
	}
}

/* ===================================================================== */
/* FONCTION PRINCIPALE (DISPATCHER)                 */
/* ===================================================================== */

void StateMachine_Run(AppStateMachineContext *ctx, GNSS_StateHandle *gps, Buttons_t *buttons, AdcContext_t *gAdc, SDCard_struct *sd) {
	if (buttons->BTN_A >= 1 || buttons->BTN_B >= 1 || buttons->BTN_A_LONG >= 1 || buttons->BTN_B_LONG >= 1) {
		ctx->last_action_tick = xTaskGetTickCount();
		if (ctx->state == STATE_SCREENSAVER) {
			ctx->state = ctx->previous_state;
			buttons->BTN_A = 0;
			buttons->BTN_B = 0;
			buttons->BTN_A_LONG = 0;
			buttons->BTN_B_LONG = 0;
		}
	}
	if ((xTaskGetTickCount() - ctx->last_action_tick ) >= pdMS_TO_TICKS(10000)) {

		if (ctx->state != STATE_SCREENSAVER) {
			if (sd->is_recording || Is_Connected_To_PC()) {
				ctx->previous_state = ctx->state;
				ctx->state = STATE_SCREENSAVER;
			} else {
				ctx->last_action_tick  = xTaskGetTickCount();
			}
		}
	}
	switch (ctx->state) {
	case STATE_SPEED:
		Handle_State_Speed(ctx, gps, buttons, gAdc, sd);
		break;

	case STATE_BALISE:
		Handle_State_Balise(ctx, gps, buttons, gAdc, sd);
		break;

	case STATE_POS:
		Handle_State_Pos(ctx, gps, buttons, gAdc, sd);
		break;

//	case STATE_CITY:
//		Handle_State_City(ctx, gps, buttons, gAdc, sd);
//		break;

	case STATE_SETTINGS:
		Handle_State_Settings(ctx, gps, buttons, gAdc, sd);
		break;

	case STATE_SCREENSAVER:
		Handle_State_Screensaver(ctx, gps, buttons, gAdc, sd);
		break;

	default:
		ctx->state = STATE_SPEED;
		break;
	}
	Handle_Global_Navigation(ctx, buttons, sd);
}
