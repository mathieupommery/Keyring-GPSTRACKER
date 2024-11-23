/*
 * usb.c
 *
 *  Created on: Nov 23, 2024
 *      Author: mathi
 */

#include "usb.h"



extern int gputemp;
extern int gpupower;

extern uint8_t usbbuffer[64];







int usbindex=0;

uint8_t usbnumbuf1[10];
uint8_t usbnumbuf2[10];




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















