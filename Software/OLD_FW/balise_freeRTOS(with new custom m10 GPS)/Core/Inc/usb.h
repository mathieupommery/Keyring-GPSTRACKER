/*
 * usb.h
 *
 *  Created on: Nov 23, 2024
 *      Author: mathi
 */

#ifndef INC_USB_H_
#define INC_USB_H_



#endif /* INC_USB_H_ */


#include "main.h"
#include "math.h"




void getgpu(uint8_t * usbbuffer);
void usbtransmit(void);

typedef enum{
	STATE_UNDER16PAGES1,
	STATE_UNDER16PAGES2,
	STATE_UNDER16PAGESFINISH,
	STATE_OVER16PAGES1,
	STATE_OVER16PAGES2,
	STATE_OVER16PAGES3,
	STATE_OVER16PAGESFINISH



}TRANSMITUSBSTATE;
