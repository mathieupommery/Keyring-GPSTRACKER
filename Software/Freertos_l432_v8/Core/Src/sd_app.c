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
    sd->isthereasdcard=0;
    sd->mscconnected=0;
    sd->is_usb_safe=0;

    // Configuration par défaut
    sd->format = SD_FORMAT_GPX;
    sd->frequency = SD_FREQ_1HZ;
}

void SD_Close_File(SDCard_struct *sd) {
    if (sd->file_is_open) {
        if (sd->format == SD_FORMAT_GPX) {
            // Ajouter les balises de fin du GPX avant de fermer
            const char* gpx_footer = "    </trkseg>\n  </trk>\n</gpx>\n";
            UINT bytesWrote;
            f_write(&sd->fil, gpx_footer, strlen(gpx_footer), &bytesWrote);
        }
        f_close(&sd->fil);
        sd->file_is_open = 0;
    }
}

void SD_Manager(SDCard_struct *sd, GNSS_StateHandle *gps, AdcContext_t *gAdc) {

    if (!Is_SD_Present()) {
        if (sd->file_is_open) SD_Close_File(sd);
        if (sd->is_mounted) {
            f_mount(NULL, "", 0);
            sd->is_mounted = 0;
        }
        sd->isthereasdcard=0;
        sd->is_usb_safe = 0;
        return;
    }
    else{
    	sd->isthereasdcard=1;
    }


    if (Is_Connected_To_PC()) {
        if (sd->file_is_open) SD_Close_File(sd);
        if (sd->is_mounted) {
            f_mount(NULL, "", 0);
            sd->is_mounted = 0;
        }
        sd->is_recording = 0;
        sd->mscconnected=1;
        sd->is_usb_safe = 1;
        return;
    }
    else{
    	sd->mscconnected=0;
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
    	                if (sd->tracknumber > 999) break;
    	            } else {
    	                break;
    	            }
    	        }
        sd->fresult = f_open(&sd->fil, sd->current_filename, FA_WRITE | FA_OPEN_APPEND | FA_CREATE_ALWAYS);

        if (sd->fresult == FR_OK) {
            sd->file_is_open = 1;

            // Écriture de l'en-tête selon le format
            if (sd->format == SD_FORMAT_GPX) {

            	len = snprintf(sd->sdcardbuffer, sizeof(sd->sdcardbuffer),
                        "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                        "<gpx version=\"1.1\" creator=\"STM32\">\n  <trk>\n    <trkseg>\n");
            	f_write(&sd->fil, sd->sdcardbuffer, len, &bytesWrote);
            } else {
            	len = snprintf(sd->sdcardbuffer, sizeof(sd->sdcardbuffer),"Lat,Lon,Ele,Sat,Vbat\n");
            	f_write(&sd->fil, sd->sdcardbuffer, len, &bytesWrote);
            }
        } else {
            return; // Impossible d'ouvrir le fichier
        }
    }

    /* 7. FORMATAGE DES DONNÉES */
    if (sd->format == SD_FORMAT_GPX) {
            // Rappel : Utilisez snprintf1() au lieu de snprintf() pour des performances maximales !
            len = snprintf(sd->sdcardbuffer, sizeof(sd->sdcardbuffer),
                "      <trkpt lat=\"%.6f\" lon=\"%.6f\">\n"
                "        <ele>%.1f</ele>\n"
                "        <time>%04d-%02d-%02dT%02d:%02d:%02dZ</time>\n"
                "      </trkpt>\n",
                gps->fLat, gps->fLon, gps->fheight,
                gps->year, gps->month, gps->day,
                gps->hour, gps->min, gps->sec); // <-- Adaptez ces variables avec les vôtres !

        } else { // CSV
            // Si vous voulez aussi l'heure dans le CSV, vous pouvez l'ajouter ici
            len = snprintf(sd->sdcardbuffer, sizeof(sd->sdcardbuffer),
                "%.6f,%.6f,%.1f,%d,%.2f\n",
    			gps->fLat, gps->fLon, gps->fheight, gps->satCount, gAdc->vbat);
        }

    /* 8. ÉCRITURE ET SYNCHRONISATION (Au lieu de f_close) */
    if (len > 0) {
        UINT bytesWrote;
        sd->fresult = f_write(&sd->fil, sd->sdcardbuffer, len, &bytesWrote);

        if (sd->fresult == FR_OK) {
        	HAL_GPIO_TogglePin(LED_BLUE_GPIO_Port,LED_BLUE_Pin);
            f_sync(&sd->fil);
        } else {
            sd->file_is_open = 0;
            sd->is_mounted = 0;
        }
    }
}
