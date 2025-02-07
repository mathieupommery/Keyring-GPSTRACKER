/*
 * tarvos.h
 *
 *  Created on: Jan 19, 2025
 *      Author: louisvoz
 */


#include "main.h"

void SET_tcMODE(char mode);
void INIT_PERM_TARVOS(uint8_t Sadr,uint8_t Hadr);
void INIT_TARVOS(UART_HandleTypeDef hlpuart1);
uint8_t Get_CRC8(uint8_t * bufP, uint16_t len);
