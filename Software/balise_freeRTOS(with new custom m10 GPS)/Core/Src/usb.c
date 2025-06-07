/*
 * usb.c
 *
 *  Created on: Nov 23, 2024
 *      Author: mathi
 */

#include "usb.h"

#include "ssd1306.h"
#include "math.h"
#include "statemachine.h"
#include <stdio.h>
#include "usbd_def.h"
#include "usbd_core.h"
#include "usbd_cdc_if.h"
#include "spif.h"
#include "spi.h"
#include "tim.h"
#include "cmsis_os.h"


extern int gputemp;
extern int gpupower;

extern uint8_t usbbuffer[64];

extern osThreadId USBTRANSMITHandle;
extern SPIF_HandleTypeDef hspif1;
extern uint8_t flashread[2048];
extern USBSTATE usbstate;

extern int BTN_A;
extern int BTN_B;
extern int BTN_B_LONG;
extern int BTN_A_LONG;

extern int pageoffset;
extern int pagenumber;

extern int erasetime;
extern int erasedisplay;
extern int usbtransmiten;

extern int enablewrite;




int usbindexpage=0;
int usbindexsecteur=0;

uint8_t usbnumbuf1[10];
uint8_t usbnumbuf2[10];

TRANSMITUSBSTATE usbtransmitstate=STATE_UNDER16PAGES1;


void getgpu(uint8_t * usbbuffer){
	usbbuffer[64]='$';
	memset(usbnumbuf1,'$',10);
	memset(usbnumbuf2,'$',10);


	int cnt=0;
	while(usbbuffer[cnt]!='$'){

		usbnumbuf1[cnt]=usbbuffer[cnt];
				  cnt++;
			  }
			  cnt++;
			  int cnt1=0;
			  while(usbbuffer[cnt]!='$'){

			  		  usbnumbuf2[cnt1]=usbbuffer[cnt];
			  		  cnt1++;
			  		  cnt++;
			  	  }

	gputemp=atoi((uint8_t *)usbnumbuf1);
	gpupower=atoi((uint8_t *)usbnumbuf2);


}

void usbtransmit(void){

	if(pagenumber>0){
		ssd1306_Fill(Black);
		ssd1306_SetCursor(32,32);
		ssd1306_WriteString("write",Font_6x8,White);
		int taillefin=0;
	switch(usbtransmitstate){

	case STATE_UNDER16PAGES1:
		SPIF_ReadPage(&hspif1,usbindexpage, (uint8_t  *)flashread, 256, 0);
		CDC_Transmit_FS((char  * )flashread,256);
		percentage((float) (usbindexpage*100)/pagenumber);
		usbindexpage++;
		if(usbindexpage==pagenumber){
			usbtransmitstate++;

		}

		break;
	case STATE_UNDER16PAGES2:
		SPIF_ReadPage(&hspif1,pagenumber, (uint8_t   *)flashread, pageoffset, 0);
		CDC_Transmit_FS((uint8_t   * )flashread,pageoffset);
		HAL_Delay(100);
		taillefin = snprintf((char  *)usbbuffer,64,"findetrame\n\r");
		CDC_Transmit_FS((char  *)usbbuffer,taillefin);
		usbtransmitstate++;

		break;
	case STATE_UNDER16PAGESFINISH:
		ssd1306_SetCursor(32,48);
		ssd1306_WriteString("finish",Font_7x10,White);
		usbstate=USBSTATEFINISH;
		usbtransmiten=0;
		usbindexsecteur=0;
		usbindexpage=0;
		osThreadSuspend(USBTRANSMITHandle);


		break;
	case STATE_OVER16PAGES1:
			SPIF_ReadSector(&hspif1,usbindexsecteur, (uint8_t  *)flashread, 2048, 0);
			CDC_Transmit_FS((char  * )flashread,2048);
			percentage((float) (usbindexsecteur*16*100)/pagenumber);
			usbindexsecteur++;
			if(usbindexsecteur==((int)(floor(pagenumber/16)))){
				usbtransmitstate++;

			}

		break;
	case STATE_OVER16PAGES2:
		SPIF_ReadPage(&hspif1,(usbindexsecteur*16+usbindexpage), (uint8_t  *)flashread, 256, 0);
		CDC_Transmit_FS((char  * )flashread,256);
		percentage((float) ((usbindexsecteur*16+usbindexpage)*100)/pagenumber);
		usbindexpage++;
		if(((usbindexsecteur)*16+usbindexpage)==(pagenumber)){
			usbtransmitstate++;
		}

		break;
	case STATE_OVER16PAGES3:
		SPIF_ReadPage(&hspif1,pagenumber, (uint8_t   *)flashread, pageoffset, 0);
		CDC_Transmit_FS((uint8_t   * )flashread,pageoffset);
		taillefin = snprintf((char  *)usbbuffer,64,"findetrame\n\r");
		CDC_Transmit_FS((char  *)usbbuffer,taillefin);
		usbtransmitstate++;
		break;
	case STATE_OVER16PAGESFINISH:
			ssd1306_SetCursor(32,48);
			ssd1306_WriteString("finish",Font_7x10,White);
			usbstate=USBSTATEFINISH;
			usbtransmiten=0;
			usbindexsecteur=0;
					usbindexpage=0;
			osThreadSuspend(USBTRANSMITHandle);


		break;
	}
	}


			else{
					ssd1306_Fill(Black);
					ssd1306_SetCursor(32,40);
					ssd1306_WriteString("write",Font_7x10,White);
					ssd1306_SetCursor(32,50);
					ssd1306_WriteString("nothing",Font_7x10,White);
					usbstate=USBSTATEFINISH;
					usbtransmiten=0;
					usbindexsecteur=0;
							usbindexpage=0;
					osThreadSuspend(USBTRANSMITHandle);


				}


}













