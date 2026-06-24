#ifndef SCREEN_BITMAPS_H
#define SCREEN_BITMAPS_H
#include <stdint.h>

extern const uint8_t bmp_speed[256];
extern const uint8_t bmp_balise[256];
extern const uint8_t bmp_pos[256];
extern const uint8_t bmp_city[256];
extern const uint8_t bmp_settings[256];
extern const uint8_t startimg[256];
const uint8_t *ScreenAnim_GetBitmap(uint8_t state);

#endif
