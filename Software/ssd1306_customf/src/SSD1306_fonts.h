#ifndef __FONTS_H__
#define __FONTS_H__

#include <stdint.h>
#include <stddef.h>

typedef struct {
    uint8_t FontWidth;      // Largeur du caractère
    uint8_t FontHeight;     // Hauteur du caractère
    const uint16_t *data;   // Pointeur vers les données de police
} FontDef;

// Déclarations des polices disponibles
extern FontDef Font_6x8;
extern FontDef Font_7x10;
extern FontDef Font_16x24;

#endif // __FONTS_H__