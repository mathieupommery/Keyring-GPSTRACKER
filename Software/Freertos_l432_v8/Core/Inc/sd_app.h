#ifndef INC_SD_APP_H_
#define INC_SD_APP_H_

#include "main.h"
#include "fatfs.h"
#include "GNSS.h"

typedef enum {
    SD_FORMAT_CSV = 0,
    SD_FORMAT_GPX = 1
} RecordFormat_t;

typedef enum {
    SD_FREQ_1HZ = 1000, // 1000 ms
    SD_FREQ_10HZ = 100  // 100 ms
} RecordFreq_t;

typedef struct {
    FATFS FatFs;      // FATFS handle
    FIL fil;          // File handle
    FRESULT fresult;

    // Paramètres de configuration
    volatile uint8_t is_recording;

    uint8_t sync_counter;
    RecordFormat_t format;
    RecordFreq_t frequency;

    uint8_t is_mounted;
    volatile uint8_t file_is_open;
    uint32_t last_record_tick;
    uint16_t tracknumber;

    volatile uint8_t isthereasdcard;
    volatile uint8_t mscconnected;
    volatile uint8_t is_usb_safe;

    char current_filename[20];
    char sdcardbuffer[256];
} SDCard_struct;

void Init_Sd(SDCard_struct *sd);
void SD_Manager(SDCard_struct *sd, GNSS_StateHandle *gps, AdcContext_t *gAdc);
void SD_Close_File(SDCard_struct *sd);

#endif /* INC_SD_APP_H_ */


