/*
 * sd_app.c
 *
 *  Created on: Jun 20, 2025
 *      Author: mathi
 */

#include "sd_app.h"
#include "usbd_def.h"
#include "stdio.h"
#include "string.h"

extern USBD_HandleTypeDef hUsbDeviceFS;

static uint8_t Is_SD_Present(void) {
	return (HAL_GPIO_ReadPin(SD_DET_GPIO_Port, SD_DET_Pin) == GPIO_PIN_RESET);
}

/**
 * @brief Vérifie si l'appareil est connecté à un VRAI ordinateur (pas un chargeur)
 */
static uint8_t Is_Connected_To_PC(void) {
	if (HAL_GPIO_ReadPin(USB_DET_GPIO_Port, USB_DET_Pin) == GPIO_PIN_SET) {
		if (hUsbDeviceFS.dev_state == USBD_STATE_CONFIGURED) {
			return 1; // PC énuméré
		}
	}
	return 0;
}

/**
 * @brief Initialisation de base de la structure
 */
void Init_Sd(SDCard_struct *sd) {
	sd->is_recording = 0;
	sd->is_mounted = 0;
	sd->file_is_open = 0;
	sd->tracknumber = 1;
	sd->last_record_tick = 0;
	sd->isthereasdcard = 0;
	sd->mscconnected = 0;
	sd->is_usb_safe = 0;
	sd->sync_counter = 0;

	// Configuration par défaut
	sd->format = SD_FORMAT_GPX;
	sd->frequency = SD_FREQ_1HZ;
}

void SD_Close_File(SDCard_struct *sd) {
	if (sd->file_is_open) {
		if (sd->format == SD_FORMAT_GPX) {
			// Ajouter les balises de fin du GPX avant de fermer
			const char *gpx_footer = "    </trkseg>\n  </trk>\n</gpx>\n";
			UINT bytesWrote;
			f_write(&sd->fil, gpx_footer, strlen(gpx_footer), &bytesWrote);
		}
		f_close(&sd->fil);
		sd->file_is_open = 0;
	}
}

void SD_Manager(SDCard_struct *sd, GNSS_StateHandle *gps, AdcContext_t *gAdc) {

	if (!Is_SD_Present()) {
		if (sd->file_is_open)
			SD_Close_File(sd);
		if (sd->is_mounted) {
			f_mount(NULL, "", 0);
			sd->is_mounted = 0;
		}
		sd->isthereasdcard = 0;
		sd->is_usb_safe = 0;
		return;
	} else {
		sd->isthereasdcard = 1;
	}

	if (Is_Connected_To_PC()) {
		if (sd->file_is_open)
			SD_Close_File(sd);
		if (sd->is_mounted) {
			f_mount(NULL, "", 0);
			sd->is_mounted = 0;
		}
		sd->is_recording = 0;
		sd->mscconnected = 1;
		sd->is_usb_safe = 1;
		return;
	} else {
		sd->mscconnected = 0;
		sd->is_usb_safe = 0;
	}

	/* 3. VÉRIFICATION DU FLAG D'ENREGISTREMENT */
	if (sd->is_recording == 0) {
		if (sd->file_is_open) {
			SD_Close_File(sd);
			sd->tracknumber++;
		}
		return;
	}

	/* === À PARTIR D'ICI : ON DOIT ENREGISTRER === */

	/* 4. MONTAGE FATFS (Si ce n'est pas déjà fait) */

	int len = 0;
	UINT bytesWrote;
	if (!sd->is_mounted) {
		sd->fresult = f_mount(&sd->FatFs, "", 1);
		if (sd->fresult == FR_OK) {
			sd->is_mounted = 1;
		} else {
			return;
		}
	}

	/* 6. CRÉATION / OUVERTURE DU FICHIER ET EN-TÊTE */
	if (!sd->file_is_open) {

		const char *ext = (sd->format == SD_FORMAT_GPX) ? "GPX" : "CSV";
		FILINFO fno;

		while (1) {
			sprintf(sd->current_filename, "TRK_%03d.%s", sd->tracknumber, ext);

			// Si le fichier existe, on incrémente
			if (f_stat(sd->current_filename, &fno) == FR_OK) {
				sd->tracknumber++;
				if (sd->tracknumber > 999)
					break;
			} else {
				break;
			}
		}
		sd->fresult = f_open(&sd->fil, sd->current_filename,
				FA_WRITE | FA_OPEN_APPEND | FA_CREATE_ALWAYS);

		if (sd->fresult == FR_OK) {
			sd->file_is_open = 1;

			if (sd->format == SD_FORMAT_GPX) {
				const char gpx_header[] =
						"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
								"<gpx version=\"1.1\" creator=\"STM32\">\n  <trk>\n    <trkseg>\n";

				f_write(&sd->fil, gpx_header, sizeof(gpx_header) - 1,
						&bytesWrote);

			} else { // CSV
				const char csv_header[] = "Lat,Lon,Ele,Sat,Vbat\n";

				f_write(&sd->fil, csv_header, sizeof(csv_header) - 1,
						&bytesWrote);
			}
		} else {
			return; // Impossible d'ouvrir le fichier
		}
	}

	/* 7. FORMATAGE DES DONNÉES (Ultra Rapide - Zéro FPU Complexe) */
	float lat = gps->fLat;
	if (lat < 0)
		lat = -lat;
	float lon = gps->fLon;
	if (lon < 0)
		lon = -lon;
	float ele = gps->fheight;
	if (ele < 0)
		ele = -ele;

	char lat_sign = (gps->fLat < 0) ? '-' : ' ';
	char lon_sign = (gps->fLon < 0) ? '-' : ' ';
	char ele_sign = (gps->fheight < 0) ? '-' : ' ';

	uint32_t lat_int = (uint32_t) lat;
	uint32_t lat_frac = (uint32_t) ((lat - lat_int) * 1000000.0f);

	uint32_t lon_int = (uint32_t) lon;
	uint32_t lon_frac = (uint32_t) ((lon - lon_int) * 1000000.0f);

	uint32_t ele_int = (uint32_t) ele;
	uint32_t ele_frac = (uint32_t) ((ele - ele_int) * 10.0f);

	if (sd->format == SD_FORMAT_GPX) {
		len = snprintf(sd->sdcardbuffer, sizeof(sd->sdcardbuffer),
				"      <trkpt lat=\"%c%lu.%06lu\" lon=\"%c%lu.%06lu\">\n"
						"        <ele>%c%lu.%01lu</ele>\n"
						"        <time>%04d-%02d-%02dT%02d:%02d:%02dZ</time>\n"
						"      </trkpt>\n", (lat_sign == '-' ? '-' : ' '),
				lat_int, lat_frac, (lon_sign == '-' ? '-' : ' '), lon_int,
				lon_frac, (ele_sign == '-' ? '-' : ' '), ele_int, ele_frac,
				gps->year, gps->month, gps->day, gps->hour, gps->min, gps->sec);

	} else {
		uint32_t vbat_int = (uint32_t) gAdc->vbat;
		uint32_t vbat_frac = (uint32_t) ((gAdc->vbat - vbat_int) * 100.0f);

		len = snprintf(sd->sdcardbuffer, sizeof(sd->sdcardbuffer),
				"%c%lu.%06lu,%c%lu.%06lu,%c%lu.%01lu,%d,%lu.%02lu\n",
				(lat_sign == '-' ? '-' : ' '), lat_int, lat_frac,
				(lon_sign == '-' ? '-' : ' '), lon_int, lon_frac,
				(ele_sign == '-' ? '-' : ' '), ele_int, ele_frac, gps->satCount,
				vbat_int, vbat_frac);
	}

	/* 8. ÉCRITURE ET SMART SYNCHRONISATION */
	if (len > 0) {
		UINT bytesWrote;
		sd->fresult = f_write(&sd->fil, sd->sdcardbuffer, len, &bytesWrote);

		if (sd->fresult == FR_OK) {
			sd->sync_counter++;
			uint8_t sync_threshold = (sd->frequency == SD_FREQ_10HZ) ? 10 : 1;

			if (sd->sync_counter >= sync_threshold) {
				f_sync(&sd->fil);
				HAL_GPIO_TogglePin(LED_BLUE_GPIO_Port, LED_BLUE_Pin);

				sd->sync_counter = 0;
			}

		} else {
			sd->file_is_open = 0;
			sd->is_mounted = 0;
		}
	}
}
