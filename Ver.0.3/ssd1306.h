/*
    ssd1306.h

    Driver for SSD1306 IC with SPI Interface

    Copyright (c) 2020 Creative Lau (creativelaulab@gmail.com)

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.
*/

#ifndef SSD1306_H
#define SSD1306_H

#include "REG51.h"
#include <string.h>
#include <stdlib.h>
#include "delay.h"

#ifndef uint8
#define uint8 unsigned char
#endif

#ifndef uint16
#define uint16 unsigned int
#endif

#ifndef uint32
#define uint32 unsigned long int
#endif

/*  OLED Command */
#define OLED_CS_Clr() OLED_CS = 0
#define OLED_CS_Set() OLED_CS = 1

#define OLED_RST_Clr() OLED_RST = 0
#define OLED_RST_Set() OLED_RST = 1

#define OLED_DC_Clr() OLED_DC = 0
#define OLED_DC_Set() OLED_DC = 1

#define OLED_SCLK_Clr() OLED_SCLK = 0
#define OLED_SCLK_Set() OLED_SCLK = 1

#define OLED_SDIN_Clr() OLED_SDIN = 0
#define OLED_SDIN_Set() OLED_SDIN = 1

/*  OLED Pixel */
#define WIDTH 128
#define HEIGHT 64
#define PAGES 8

/*  OLED Brightness */
#define BRIGHTNESS_MIN 1
#define BRIGHTNESS_MAX 25

/*  OLED Driver */
void OLED_Init(void);
void OLED_Write_Command(uint8 dat);
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
