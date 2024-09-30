/*
 * ssd1289.c
 *
 *  Created on: Jan 5, 2019
 *      Author: Cuong
 */

#include "ssd1289.h"
#include "font.h"


void swap(uint16_t *a, uint16_t *b)
{
    uint16_t temp = 0x0000;

    temp = *b;
    *b = *a;
    *a = temp;
}

/*Ghui dia chi thanh ghi*/
void SSD1289_Write_Com(uint16_t  DH)
{
  RSN;
	CSN;
	GPIOE -> ODR = DH;
	WRN;
	WR;
	CS;
}

/*Gui du lieu ghi vao thanh ghi*/
void SSD1289_Write_Data(uint16_t DH)
{
  RS;
	CSN;
	GPIOE -> ODR = DH;
	WRN;
	WR;
	CS;
}

/*Gui dong thoi dia chi thanh ghi va du lieu thanh ghi*/
void SSD1289_Write_Com_Data(uint16_t com1,uint16_t dat1)
{
   SSD1289_Write_Com(com1);
   SSD1289_Write_Data(dat1);
}

/*Reset man hinh*/
void SSD1289_Reset(){
	RSTN;
	HAL_Delay(10);
	RST;
}

/*Set dia chi cua so*/
void SSD1289_Address_Set(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2)	/*Thiet lap vung lam viec cua man hinh*/
{
	/*Doi toa do ve toa do mong muon*/
	swap(&x1, &y1);
	swap(&x2, &y2);

	SSD1289_Write_Com_Data(0x0044,(x2<<8)+x1);
	SSD1289_Write_Com_Data(0x0045, y1);
	SSD1289_Write_Com_Data(0x0046, y2);
	SSD1289_Write_Com_Data(0x004E, x1);
	SSD1289_Write_Com_Data(0x004F, y1);
	SSD1289_Write_Com(0x0022);
}

/*Ghi pixel*/
void SSD1289_Write_Pixel(uint16_t x, uint16_t y, uint16_t color)
{
  CSN;

	swap(&x, &y);
  SSD1289_Write_Com_Data(0x004E, x);
  SSD1289_Write_Com_Data(0x004F, y);
  SSD1289_Write_Com_Data(0x0022, color);

  CS;
}

/*Ve duong thang*/
void SSD1289_Draw_Line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color)
{
    uint16_t dx = 0x0000;
    uint16_t dy = 0x0000;
    uint16_t stepx = 0x0000;
    uint16_t stepy = 0x0000;
    uint16_t fraction = 0x0000;

    dy = (y2 - y1);
    dx = (x2 - x1);

    if(dy < 0)
    {
        dy = -dy;
        stepy = -1;
    }
    else
    {
        stepy = 1;
    }

    if(dx < 0)
    {
        dx = -dx;
        stepx = -1;
    }
    else
    {
        stepx = 1;
    }

    dx <<= 0x01;
    dy <<= 0x01;

    SSD1289_Write_Pixel(x1, y1, color);

    if(dx > dy)
    {
        fraction = (dy - (dx >> 1));
        while(x1 != x2)
        {
            if(fraction >= 0)
            {
                y1 += stepy;
                fraction -= dx;
            }
            x1 += stepx;
            fraction += dy;

            SSD1289_Write_Pixel(x1, y1, color);
        }
    }
    else
    {
        fraction = (dx - (dy >> 1));

        while(y1 != y2)
        {
            if (fraction >= 0)
            {
                x1 += stepx;
                fraction -= dy;
            }
            y1 += stepy;
            fraction += dx;
            SSD1289_Write_Pixel(x1, y1, color);
        }
    }
}

/*Set vi tri con tro*/
void SSD1289_Set_Cursor(uint16_t x_pos, uint16_t y_pos)		/*Thiet lap vi tri con tro ban dau khi ve*/
{
    swap(&x_pos, &y_pos);

    SSD1289_Write_Com_Data(0x004E, x_pos);
    SSD1289_Write_Com_Data(0x004F, y_pos);
    SSD1289_Write_Com(0x0022);
}

/*Ve mau*/
void SSD1289_Fill_Color(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color){	/*To mau trong khoang da duoc quy dinh*/
	uint16_t max_index = (x2 - x1 + 1)*(y2 - y1 + 1);
	SSD1289_Address_Set(x1, y1, x2, y2);

	while(max_index){
			SSD1289_Write_Data(color);
			max_index--;
	}
}


void SSD1289_Write_Pic_Coor(uint16_t x, uint16_t y, uint16_t length, uint16_t width, uint16_t *pic){
	uint16_t index_x = 0, index_y = 0, i = 0;
	for(index_y = 0; index_y < width; index_y++){
		for(index_x = 0; index_x < length; index_x++){
			SSD1289_Write_Pixel(x + index_x, y + index_y, pic[i]);
			i++;
		}
	}
}
void SSD1289_Print_Char(uint16_t x_pos, uint16_t y_pos, uint16_t font_size, uint16_t color, uint16_t back_color, char ch){
	int i = 0;
	int j = 0;
	uint16_t y = y_pos;
	uint16_t column = 0;
	if(font_size == 0)
		font_size = 1;
	uint16_t *value = font[(uint16_t)ch-32];
	for(i = 0; i < 5; i++){
		column = value[i];
		y_pos = y;
		for(j = 0; j < 7; j++){
			if((column >> j) & 0x0001)
				SSD1289_Fill_Color(x_pos, y_pos, (x_pos + font_size - 1), (y_pos +font_size - 1), color);
			else{
				SSD1289_Fill_Color(x_pos, y_pos, (x_pos + font_size - 1), (y_pos +font_size - 1), back_color);
			}
			y_pos += font_size;
		}
		x_pos+= font_size;
	}
}

void SSD1289_Print_String(uint16_t x_pos, uint16_t y_pos, uint16_t dis_char, uint16_t dis_line, uint16_t font_size, uint16_t color, uint16_t back_color, char *str){
	uint16_t x = x_pos;
	if(str == NULL)
		return;
	do{
		if(*str == '\n'){
			str++;
			y_pos = y_pos +  font_size * 7 + dis_line;
			x_pos = x;
		}
		SSD1289_Print_Char(x_pos, y_pos, font_size, color, back_color, *str++);
		x_pos += (font_size *5 + dis_char);
	}
	while(*str != '\0');
}
void SSD1289_Print(uint16_t x, uint16_t y, char *string){
	SSD1289_Write_Back_Ground(Black);
	SSD1289_Print_String(x, y, 1, 1, 1, White, Black, string);
}
void SSD1289_Draw_V_Line(uint16_t x1, uint16_t y1, uint16_t y2, uint16_t color)
{

    if(y1 > y2)
    {
       swap(&y1, &y2);
    }

    while(y2 > (y1-1))
    {
        SSD1289_Write_Pixel(x1, y2, color);
        y2--;
    }
}


void SSD1289_Draw_H_Line(uint16_t x1, uint16_t x2, uint16_t y1, uint16_t color)
{

    if(x1 > x2)
    {
       swap(&x1, &x2);
    }

    while(x2 > (x1 - 1))
    {
        SSD1289_Write_Pixel(x2, y1, color);
        x2--;
    }
}
void SSD1289_Draw_Rectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t fill, uint16_t color)
{
     uint16_t i = 0x0000;
     uint16_t xmin = 0x0000;
     uint16_t xmax = 0x0000;
     uint16_t ymin = 0x0000;
     uint16_t ymax = 0x0000;

     switch(fill)
     {
         case YES:
         {
             if(x1 < x2)
             {
                xmin = x1;
                xmax = x2;
             }
             else
             {
                xmin = x2;
                xmax = x1;
             }

             if(y1 < y2)
             {
                ymin = y1;
                ymax = y2;
             }
             else
             {
                ymin = y2;
                ymax = y1;
             }

             for(; xmin <= xmax; ++xmin)
             {
                 for(i = ymin; i <= ymax; ++i)
                 {
                     SSD1289_Write_Pixel(xmin, i, color);
                 }
             }

             break;
         }
         default:
         {
             SSD1289_Draw_V_Line(x1, y1, y2, color);
             SSD1289_Draw_V_Line(x2, y1, y2, color);
             SSD1289_Draw_H_Line(x1, x2, y1, color);
             SSD1289_Draw_H_Line(x1, x2, y2, color);
             break;
         }
     }
}

void SSD1289_Write_Pic(const uint16_t *pic)
{
  uint16_t i_x = 0;
  uint16_t i_y = 0;
  uint32_t i = 0;

  for(i_y = 0; i_y < 240; i_y++)
  {
    for(i_x = 0; i_x < 320; i_x++)
    {
      SSD1289_Write_Pixel(i_x, i_y, pic[i]);
      i++;
    }
  }
}

void SSD1289_Write_Back_Ground(uint16_t color)
{
	uint16_t x = 0, y = 0;
	for(y = 0; y < 240; y++){
		for(x = 0; x < 320;  x++){
			SSD1289_Write_Pixel(x, y, color);
		}
	}
}
void SSD1289_Clear_Screen(){
	SSD1289_Write_Back_Ground(White);
}
void SSD1289_Init(void)
{

	RST;
  	HAL_Delay(100);
	RSTN;;
	HAL_Delay(100);
	RST;
	CS;
	RD;
	WR;
	HAL_Delay(100);

    SSD1289_Write_Com_Data(0x0000,0x0001);    HAL_Delay(1);
    SSD1289_Write_Com_Data(0x0003,0xA8A4);    HAL_Delay(1);
    SSD1289_Write_Com_Data(0x000C,0x0000);    HAL_Delay(1);
    SSD1289_Write_Com_Data(0x000D,0x080C);    HAL_Delay(1);
    SSD1289_Write_Com_Data(0x000E,0x2B00);    HAL_Delay(1);
    SSD1289_Write_Com_Data(0x001E,0x00B0);    HAL_Delay(1);
    SSD1289_Write_Com_Data(0x0001,0x293F);    HAL_Delay(1);
    SSD1289_Write_Com_Data(0x0002,0x0600);    HAL_Delay(1);
    SSD1289_Write_Com_Data(0x0010,0x0000);    HAL_Delay(1);
    SSD1289_Write_Com_Data(0x0011,0x6070);    HAL_Delay(1);
    SSD1289_Write_Com_Data(0x0005,0x0000);    HAL_Delay(1);
    SSD1289_Write_Com_Data(0x0006,0x0000);    HAL_Delay(1);
    SSD1289_Write_Com_Data(0x0016,0xEF1C);    HAL_Delay(1);
    SSD1289_Write_Com_Data(0x0017,0x0003);    HAL_Delay(1);
    SSD1289_Write_Com_Data(0x0007,0x0233);    HAL_Delay(1);
    SSD1289_Write_Com_Data(0x000B,0x0000);    HAL_Delay(1);
    SSD1289_Write_Com_Data(0x000F,0x0000);    HAL_Delay(1);
    SSD1289_Write_Com_Data(0x0041,0x0000);    HAL_Delay(1);
    SSD1289_Write_Com_Data(0x0042,0x0000);    HAL_Delay(1);
    SSD1289_Write_Com_Data(0x0048,0x0000);    HAL_Delay(1);
    SSD1289_Write_Com_Data(0x0049,0x013F);    HAL_Delay(1);
    SSD1289_Write_Com_Data(0x004A,0x0000);    HAL_Delay(1);
    SSD1289_Write_Com_Data(0x004B,0x0000);    HAL_Delay(1);
    SSD1289_Write_Com_Data(0x0044,0xEF00);    HAL_Delay(1);
    SSD1289_Write_Com_Data(0x0045,0x0000);    HAL_Delay(1);
    SSD1289_Write_Com_Data(0x0046,0x013F);    HAL_Delay(1);
    SSD1289_Write_Com_Data(0x0030,0x0707);    HAL_Delay(1);
    SSD1289_Write_Com_Data(0x0031,0x0204);    HAL_Delay(1);
    SSD1289_Write_Com_Data(0x0032,0x0204);    HAL_Delay(1);
    SSD1289_Write_Com_Data(0x0033,0x0502);    HAL_Delay(1);
    SSD1289_Write_Com_Data(0x0034,0x0507);    HAL_Delay(1);
    SSD1289_Write_Com_Data(0x0035,0x0204);    HAL_Delay(1);
    SSD1289_Write_Com_Data(0x0036,0x0204);    HAL_Delay(1);
    SSD1289_Write_Com_Data(0x0037,0x0502);    HAL_Delay(1);
    SSD1289_Write_Com_Data(0x003A,0x0302);    HAL_Delay(1);
    SSD1289_Write_Com_Data(0x003B,0x0302);    HAL_Delay(1);
    SSD1289_Write_Com_Data(0x0023,0x0000);    HAL_Delay(1);
    SSD1289_Write_Com_Data(0x0024,0x0000);    HAL_Delay(1);
    SSD1289_Write_Com_Data(0x0025,0x8000);    HAL_Delay(1);
    SSD1289_Write_Com_Data(0x004f,0);
    SSD1289_Write_Com_Data(0x004e,0);
	SSD1289_Write_Com(0x0022);
}




