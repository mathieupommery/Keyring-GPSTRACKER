#include "tarvos.h"
#include "usart.h"
#include "main.h"
#include "adc.h"
#include "dma.h"
#include "i2c.h"
#include "usart.h"
#include "spi.h"
#include "tim.h"
#include "usb.h"
#include "gpio.h"
#include "nmea_parse.h"
#include <stdint.h>
// Définition des variables
#define CMD_PAYLOAD_MAX 224
typedef struct {
uint8_t Stx; //start of text
uint8_t Cmd;
uint8_t Length;
uint8_t Data[CMD_PAYLOAD_MAX+1]; /* +1 for CRC8 */
} CMD_Frame_t;
#define CMD_OFFSET_TO_DATAFIELD 3
#define CMD_OVERHEAD (CMD_OFFSET_TO_DATAFIELD+1)

extern UART_HandleTypeDef hlpuart1;
// Définition des fonctions qui vont être utiles
void INIT_TARVOS(UART_HandleTypeDef hlpuart1){
	SET_tcMODE("command");

	HAL_UART_Transmit(&hlpuart1,"yo",sizeof("yo"),100);
	HAL_Delay(1000);
}

void INIT_PERM_TARVOS(uint8_t Hadr, uint8_t Sadr) {
    // On passe le module en mode 1 byte (sécurité)
    uint8_t mode1[6] = {0x02, 0x09, 0x02, 0x04, 0x01,0x00};
    mode1[5] = Get_CRC8(mode1, 5); // Ajout du CRC
    HAL_UART_Transmit(&hlpuart1,(uint8_t *)mode1, sizeof(mode1), 500);

    // On change le canal (la fréquence du module)
    uint8_t frequence[6] = {0x02, 0x09, 0x02, 0x03, 0x82, 0x00}; // 869.5 MHz -> 130 = 0x82
    frequence[5] = Get_CRC8(frequence, 5); // Ajout du CRC
    HAL_UART_Transmit(&hlpuart1, frequence, sizeof(frequence), 500);

    // On modifie la puissance (ici à 500mW max)
    uint8_t puissance[6] = {0x02, 0x09, 0x02, 0x02, 0x1B, 0x00};
    puissance[5] = Get_CRC8(puissance, 5); // Ajout du CRC
    HAL_UART_Transmit(&hlpuart1, puissance, sizeof(puissance), 500);

    // On modifie l'adresse de la source
    uint8_t Sadrr[6] = {0x02, 0x09, 0x02, 0x0B, Hadr, 0x00};
    Sadrr[5] = Get_CRC8(Sadrr, 5); // Ajout du CRC
    HAL_UART_Transmit(&hlpuart1, Sadrr, sizeof(Sadrr), 500);

    // On modifie l'adresse avec qui on veut communiquer
	uint8_t send[6] = {0x02, 0x09, 0x02, 0x08, Sadr, 0x00};
	send[5] = Get_CRC8(send, 5); // Ajout du CRC
	HAL_UART_Transmit(&hlpuart1, send, sizeof(send), 500);

    // On redémarre le module pour mettre à jour les paramètres
    uint8_t restart[4] = {0x02, 0x05, 0x00, 0x07};
    HAL_UART_Transmit(&hlpuart1, restart, sizeof(restart), 500);

    HAL_Delay(1000);
}
//Changement de mode (transparent,command)
void SET_tcMODE(char mode){
	uint8_t transparent[] = {0x02, 0x04, 0x01, 0x00, 0x07};
	uint8_t command[] = {0x02, 0x04, 0x01, 0x10, 0x07};
	if (mode=="transparent"){
		HAL_UART_Transmit(&hlpuart1,transparent,sizeof(transparent),500);
	}
	if (mode=="command"){
		HAL_UART_Transmit(&hlpuart1,command,sizeof(command),500);
	}
}
//Calcule du CRC
uint8_t Get_CRC8(uint8_t * bufP, uint16_t len)
{
uint8_t crc = 0x00;
for (uint16_t i = 0; i < len; i++)
{
crc ^= bufP[i];
}
return crc;
}

void SEND_DATA_NETW(uint8_t *data, uint8_t channel, uint8_t dest_adress, int length) {
    // Longueur totale de la trame : en-tête (5 octets) + données + CRC
    uint8_t trame[5 + length + 1]; // +1 pour le CRC

    // Initialisation de l'en-tête
    trame[0] = 0x02;            // Start byte
    trame[1] = 0x01;            // Command identifier
    trame[2] = length + 2;      // Longueur totale (length + channel + dest_adress)
    trame[3] = channel;         // Canal
    trame[4] = dest_adress;     // Adresse de destination

    // Copie des données dans la trame
    for (uint8_t i = 0; i < length; i++) {
        trame[5 + i] = data[i];
    }

    // Calcul du CRC (sur tout sauf le CRC lui-même)
    trame[5 + length] = Get_CRC8(trame, 5 + length);

    // Transmission de la trame
    HAL_UART_Transmit(&hlpuart1, trame, sizeof(trame), 500);
}

void SET_Destadr(uint8_t Destadrr){
	//on change l'adresse de destination à Destadrr
	uint8_t buffer[5]={0x02,0x08,0x01,Destadrr,0x00};
	buffer[4]=Get_CRC8(buffer,4);
    HAL_UART_Transmit(&hlpuart1,buffer, sizeof(buffer), 500);
}

void REQ(uint8_t index){
	uint8_t buffer[5]={0x02,0x0A,0x01,index,0x00};
	buffer[4]=Get_CRC8(buffer,4);
    HAL_UART_Transmit(&hlpuart1,buffer, sizeof(buffer), 500);

}

void SEND_DATA(uint8_t *data,int length) {
    // Longueur totale de la trame : en-tête (3 octets) + données + CRC
    uint8_t trame[3 + length + 1]; // +1 pour le CRC

    // Initialisation de l'en-tête
    trame[0] = 0x02;            // Start byte
    trame[1] = 0x01;            // Command identifier
    trame[2] = length;      // Longueur totale (length + channel + dest_adress)

    // Copie des données dans la trame
    for (uint8_t i = 0; i < length; i++) {
        trame[3 + i] = data[i];
    }

    // Calcul du CRC (sur tout sauf le CRC lui-même)
    trame[3 + length] = Get_CRC8(trame, 3 + length);

    // Transmission de la trame
    HAL_UART_Transmit(&hlpuart1, trame, sizeof(trame), 500);
}


