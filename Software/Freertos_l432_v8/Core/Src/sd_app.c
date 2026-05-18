/*
 * sd_app.c
 *
 *  Created on: Jun 20, 2025
 *      Author: mathi
 */
#include "sd_app.h"

extern FATFS FatFs;   // FATFS handle
extern FILINFO fno;	  // Structure holds information
extern FATFS *getFreeFs; 	  // Read information
extern FIL fil;
extern DIR dir;			  // Directory object structure
extern DWORD free_clusters;  // Free Clusters
extern DWORD free_sectors;	  // Free Sectors
extern DWORD total_sectors;



FRESULT store_in_sd(FRESULT fres){

	uint16_t sizeligne=0;

	sizeligne=snprintf((char *)sdcardbuffer,256,"abc\n\r");




			  if (fres == FR_OK) {
			  fres = f_open(&fil, (uint8_t *) "SAT_01.csv",FA_WRITE | FA_OPEN_ALWAYS);

				  }
				  if (fres == FR_OK) {
					  fres=f_lseek(&fil, f_size(&fil));
					  UINT bytesWrote;
					  fres = f_write(&fil,(char*)sdcardbuffer, sizeligne, &bytesWrote);
					  f_close(&fil);
				  }


	return fres;

}
