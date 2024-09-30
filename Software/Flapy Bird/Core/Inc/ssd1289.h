/*
 * ssd1289.h
 *
 *  Created on: Jan 5, 2019
 *      Author: Cuong
 */

#ifndef SSD1289_H_
#define SSD1289_H_

#include "stm32f4xx_hal.h"

#define  MAX_X         319
#define  MAX_Y         239

#define  YES					 1
#define	 NO						 0

#define  Blue          0x001F
#define  Red		   0xF800
#define  Yellow		   0xFFE0
#define  Green         0x07E0
#define  White         0xFFFF
#define  Black		   0x0000
#define  Pill_Color	   0x1E04
#define  Background	   0xC7FF
#define  Hscolor	   0xF01F

#define RS HAL_GPIO_WritePin(GPIOD, GPIO_PIN_6, GPIO_PIN_SET)			/*RS = 1 ghi data*/
#define RSN HAL_GPIO_WritePin(GPIOD, GPIO_PIN_6, GPIO_PIN_RESET)		/*RS = 0 ghi lenh*/

#define WR HAL_GPIO_WritePin(GPIOD, GPIO_PIN_5, GPIO_PIN_SET)	/*Ghi du lieu, suon len cua xung tren chan WR se ghi data len LCD*/
#define WRN HAL_GPIO_WritePin(GPIOD, GPIO_PIN_5, GPIO_PIN_RESET)

#define RD HAL_GPIO_WritePin(GPIOD, GPIO_PIN_4, GPIO_PIN_SET)	/*Doc du lieu, suon len cua xung tren chan RD doc trang thai LCD*/
#define RDN HAL_GPIO_WritePin(GPIOD, GPIO_PIN_4, GPIO_PIN_RESET)


#define CS HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10, GPIO_PIN_SET)	/*Chipselect - tich cuc muc thap*/
#define CSN HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10, GPIO_PIN_RESET)

#define RST HAL_GPIO_WritePin(GPIOD, GPIO_PIN_3, GPIO_PIN_SET)	/*Chan RESET LCD tich cuc muc thap*/
#define RSTN HAL_GPIO_WritePin(GPIOD, GPIO_PIN_3, GPIO_PIN_RESET)



/*For SSD1289*/
void SSD1289_Init();		/*Khoi tao man hinh cam ung*/
void SSD1289_Reset();

void swap(uint16_t *a, uint16_t *b);

void SSD1289_Write_Com(uint16_t  DH);	/*Gui dia chi thanh ghi*/
void SSD1289_Write_Data(uint16_t DH); /*Gan du lieu vao thanh ghi vua gui*/
void SSD1289_Write_Com_Data(uint16_t com1,uint16_t dat1);	/*Ghi du lieu vao thanh ghi co dia chi cho truoc*/

void SSD1289_Fill_Color(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);	/*To mau mot vung hinh chu nhat*/
void SSD1289_Address_Set(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2);
void SSD1289_Set_Cursor(uint16_t x_pos, uint16_t y_pos);

void SSD1289_Write_Pixel(uint16_t x, uint16_t y, uint16_t color);		/*Ghi mau vao pixel co toa do nhat dinh*/
void SSD1289_Write_Pic(const uint16_t *pic);	/*Hien thi anh*/
//void SSD1289_Write_Pic_Coor(const uint16_t *pic, uint16_t x, uint16_t y, uint16_t length, uint16_t width);	/*Hien thi anh theo toa do*/
void SSD1289_Clear_Screen();	/*Xoa man hinh*/
void SSD1289_Write_Back_Ground(uint16_t color);

void SSD1289_Print_Char(uint16_t x_pos, uint16_t y_pos, uint16_t font_size, uint16_t color, uint16_t back_color, char ch);
void SSD1289_Print_String(uint16_t x_pos, uint16_t y_pos, uint16_t dis_char, uint16_t dis_line, uint16_t font_size, uint16_t color, uint16_t back_color, char* string);

void SSD1289_Draw_H_Line(uint16_t x1, uint16_t x2, uint16_t y1, uint16_t color);
void SSD1289_Draw_V_Line(uint16_t x1, uint16_t y1, uint16_t y2, uint16_t color);
void SSD1289_Draw_Rectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t fill, uint16_t color);

void SSD1289_Write_Pic_Coor(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t *pic);
void SSD1289_Print(uint16_t x, uint16_t y, char *string);
#endif /* SSD1289_H_ */
