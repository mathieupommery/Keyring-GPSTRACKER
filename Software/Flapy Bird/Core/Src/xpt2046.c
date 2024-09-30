#include "xpt2046.h"


extern SPI_HandleTypeDef hspi3;
volatile uint8_t receive_data = 0;
volatile uint8_t send_cmd;

uint8_t Read_IRQ(){
	return HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_1);
}

uint16_t XPT2046_Read(uint8_t cmd){
	uint16_t coor;

	T_CSN;
	send_cmd = cmd;
	HAL_SPI_Transmit(&hspi3, (uint8_t*) &send_cmd, 1, 1000);
	send_cmd = 0x00;
	HAL_SPI_TransmitReceive(&hspi3, (uint8_t*) &send_cmd, (uint8_t*) &receive_data, 1, 1000);
	coor = (uint16_t) receive_data;
	coor = coor << 8;
	send_cmd = 0x00;
	HAL_SPI_TransmitReceive(&hspi3, (uint8_t*) &send_cmd, (uint8_t*) &receive_data, 1, 1000);
	coor = coor | (uint16_t) receive_data;
	coor = coor >> 3;
	T_CS;

	return coor;
}

/*sel = 0: Tinh toa do x
  sel != 0: Tinh toa do y
	coor: toa do nhan duoc*/
uint16_t  XPT2046_Calibrate(uint16_t coor, uint16_t r){
	uint16_t temp = 0;

	if(r == 1){
		temp = (uint16_t) (LCD_WID * ((float)(coor - X_T_MIN)/T_WIDTH));
	}
	else{
		temp = (uint16_t) (LCD_HEI * ((float)(coor - Y_T_MIN)/T_HEIGH));
	}

	return temp;
}
