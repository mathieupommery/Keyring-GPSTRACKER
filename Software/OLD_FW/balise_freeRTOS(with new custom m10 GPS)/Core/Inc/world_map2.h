/*
 * world_map2.h
 *
 *  Created on: Jun 6, 2025
 *      Author: mathi
 */

#ifndef INC_WORLD_MAP2_H_
#define INC_WORLD_MAP2_H_


#define CAL_VILLE_1_LAT 48.857431f
#define CAL_VILLE_1_LONG 2.340603f
#define CAL_VILLE_1_X 108.0f
#define CAL_VILLE_1_Y 50.0f

#define CAL_VILLE_2_LAT 43.007032f
#define CAL_VILLE_2_LONG 9.402580f
#define CAL_VILLE_2_X 195.0f
#define CAL_VILLE_2_Y 166.0f

#define SCALE_X ((CAL_VILLE_2_X - CAL_VILLE_1_X) / (CAL_VILLE_2_LONG - CAL_VILLE_1_LONG))
#define SCALE_Y ((CAL_VILLE_2_Y - CAL_VILLE_1_Y) / (CAL_VILLE_2_LAT - CAL_VILLE_1_LAT))
#define MAP_WIDTH 200
#define MAP_HEIGHT 200

#endif /* INC_WORLD_MAP2_H_ */
