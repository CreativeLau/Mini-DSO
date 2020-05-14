#include "REG51.h"
#ifndef SSD1306_H
#define SSD1306_H

#ifndef uint8
#define uint8 unsigned char
#endif

#ifndef uint16
#define uint16 unsigned int
#endif

#ifndef uint32
#define uint32 unsigned long int
#endif


#define OLED_CS_Clr() OLED_CS = 0
#define OLED_CS_Set() OLED_CS = 1

#define OLED_RST_Clr() OLED_RST = 0
#define OLED_RST_Set() OLED_RST = 1

#define OLED_DC_Clr() OLED_DC = 0
#define OLED_DC_Set() OLED_DC = 1

#define OLED_SCLK_Clr() OLED_SCL = 0
#define OLED_SCLK_Set() OLED_SCL = 1

#define OLED_SDIN_Clr() OLED_SDIN = 0
#define OLED_SDIN_Set() OLED_SDIN = 1

#define WIDTH 128
#define HEIGHT 64

//-----------------OLED端口定义----------------
sbit OLED_SCL = P2 ^ 7;  //时钟 D0（SCLK）
sbit OLED_SDIN = P2 ^ 6; //数据D1（MOSI）
sbit OLED_RST = P2 ^ 5;  //复位
sbit OLED_DC = P2 ^ 4;   //数据/命令控制
sbit OLED_CS = P2 ^ 3;   //片选

//OLED控制用函数
void OLED_Init(void);
void OLED_Clear(void);
void OLED_Display(void);
void OLED_Set_Pos(uint8 x, uint8 y);
void OLED_WR_Byte(uint8 dat, uint8 cmd);
void OLED_DrawChar(uint8 x, uint8 y, uint8 chr);
void OLED_DrawString(uint8 *p);
void OLED_DrawNum(uint32 num, uint8 len);
void OLED_DrwCHS_16x16(uint8 chs_no);
void OLED_DrawPixel(uint8 x, uint8 y);
void OLED_DrawHLine(uint8 x, uint8 y, uint8 w);
void OLED_DrawVLine(uint8 x, uint8 y, uint8 w);
void OLED_DrawLine(uint8 x0, uint8 y0, uint8 x1, uint8 y1);
void OLED_DrawBMP(uint8 x0, uint8 y0, uint8 x1, uint8 y1, uint8 *BMP, uint16 len);
void OLED_SetFontWidth(uint8 w);
void OLED_Reverse(bit i);
void OLED_Overlap(bit i);
#endif
