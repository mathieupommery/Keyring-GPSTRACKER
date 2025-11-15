

#include "world_map.h"
#include "world_map2.h"
#include "math.h"
#include "ssd1306.h"

uint8_t buffer_affichage[256];
extern const uint8_t world_map[];
int xposition12=0;
int yposition12=0;



float lat_to_mercator_y(float lat){
    float rad = (float) lat*(M_PI/180.0);
    return logf(tanf((M_PI / 4.0) + (rad / 2.0)));
}

void mercator_to_pixel(float lat,float  lon){

    		    xposition12 =(int)floor( CAL_VILLE_1_X + (lon - CAL_VILLE_1_LONG) * SCALE_X);
    		    yposition12 =(int)floor( CAL_VILLE_1_Y + (lat - CAL_VILLE_1_LAT) * SCALE_Y);
}



void extract_view(const uint8_t *world_map,float lat, float lon) {
	    memset((uint8_t*)buffer_affichage, 0, 256);
	    mercator_to_pixel(lat,lon);

	    for (int y = 0; y < 32; y++) {
	        int map_y = yposition12 - 16 + y;
	        if (map_y < 0 || map_y >= MAP_HEIGHT){
	            continue;
	        }
	        for (int x = 0; x < 64; x++) {
	            int map_x = xposition12 - 32 + x;
	            if (map_x < 0 || map_x >= MAP_WIDTH){
	                continue;
	            }


	            int map_byte_index = (map_y * MAP_WIDTH + map_x) / 8;
	            int map_bit_index = 7 - ((map_x) % 8);
	            uint8_t map_bit = (world_map[map_byte_index] >> map_bit_index) & 1;

	            // Écrire dans le screen_buffer
	            int screen_byte_index = (y * 64 + x) / 8;
	            int screen_bit_index = 7 - (x % 8);
	            if (map_bit)
	                buffer_affichage[screen_byte_index] |= (1 << screen_bit_index);
	        }
	    }

	    ssd1306_Fill(Black);
	    ssd1306_DrawBitmap(32, 32, buffer_affichage, 64, 32, White);
	}

