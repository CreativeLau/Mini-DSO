/*
版本：V0.1
作者：老刘爱捣鼓
时间：2020-04-24
老刘爱捣鼓（全网同名）期待您的关注！

备注：为Mini DSO这个项目临时编写的SSD1306 128x64驱动，尚不完善
有写字符，字符串，数字，16x16中文字符，画水平线，垂直线，任意两点线段，绘制图片等功能
主要参考了Arduino中Adafruit SSD1306的库函数
采用全屏buf驱动，需占用128x8个uint8内存空间
全屏缓存操控屏幕内容比较方便，刷新速度快，但对单片机的内存大小有要求
F6x8字体来自中景园电子的DEMO
*/
#include "ssd1306.h"
#include "ssd1306font.h"
#include <string.h>
#include <stdlib.h>

//OLED的显存
//存放格式如下.
//[0]0 1 2 3 ... 127
//[1]0 1 2 3 ... 127
//[2]0 1 2 3 ... 127
//[3]0 1 2 3 ... 127
//[4]0 1 2 3 ... 127
//[5]0 1 2 3 ... 127
//[6]0 1 2 3 ... 127
//[7]0 1 2 3 ... 127

#ifndef _swap_char
#define _swap_char(a, b)     \
    {                        \
        uint8 t = a; \
        a = b;               \
        b = t;               \
    }
#endif

bit _oled_reverse = 0; 	//绘图模式，1:白底黑字，0:黑底白字
bit _oled_overlap = 1; 	//绘图模式，1:叠加，0:覆盖；清屏后没有覆盖要求用叠加，覆盖时如果遇到跨行，则会覆盖掉两行内容，后续需改进
uint8 buffer[1024]; 	//128x64全屏缓存,横向128个像素，纵向64个像素分为8页，每页8个像素作为一个字节，共128x8个字节
static char _x, _y;
static uint8 _font_width = 6; //ASCII字符宽度

void delay_ms(unsigned int ms)
{
    unsigned int a;
    while (ms)
    {
        a = 1800;
        while (a--)
            ;
        ms--;
    }
    return;
}

//写命令
void OLED_Write_Command(uint8 dat)
{
    uint8 i;
    OLED_DC_Clr();
    OLED_CS_Clr();
    for (i = 0; i < 8; i++)
    {
        OLED_SCLK_Clr();
        if (dat & 0x80) //判断数据最高位是1还是0
        {
            OLED_SDIN_Set();
        }
        else
            OLED_SDIN_Clr();
        OLED_SCLK_Set();
        dat <<= 1;
    }
    OLED_CS_Set();
//    OLED_DC_Set();
}

//写数据
void OLED_Write_Data(uint8 dat)
{
    uint8 i;
    OLED_DC_Set();
    OLED_CS_Clr();
    for (i = 0; i < 8; i++)
    {
        OLED_SCLK_Clr();
        if (dat & 0x80) //判断数据最高位是1还是0
        {
            OLED_SDIN_Set();
        }
        else
            OLED_SDIN_Clr();
        OLED_SCLK_Set();
        dat <<= 1;
    }
    OLED_CS_Set();
//    OLED_DC_Set();
}

//1：白底黑字，0：黑底白字
void OLED_Reverse(bit i)
{
	_oled_reverse=i;
}

//绘制模式：1：叠加，0：覆盖
void OLED_Overlap(bit i)
{
    _oled_overlap = i;
}

//设置ASCII字符宽度
void OLED_SetFontWidth(uint8 w)
{
    _font_width = w;
}


//设置内存位置x:0-127,y:0-63
void OLED_Set_Pos(uint8 x, uint8 y)
{
    _x = x;
    _y = y;
    OLED_Write_Command(0xb0 + y / 8);
    OLED_Write_Command(((x & 0xf0) >> 4) | 0x10);
    OLED_Write_Command((x & 0x0f) | 0x00);
}

//清除缓存内容
void OLED_Clear(void)
{
    memset(buffer, 0x00, WIDTH * ((HEIGHT + 7) / 8));
}


//绘制字节至缓存
void OLED_Draw_Byte(uint8 *pBuf, uint8 mask)
{
	if (_oled_overlap)
	{
		if (_oled_reverse)
			*pBuf |= ~mask;
		else
			*pBuf |= mask;
	}
	else
	{
		if (_oled_reverse)
		{
			*pBuf &= ~mask;
			*pBuf |= ~mask;
		}
		else
		{
			*pBuf &= mask;
			*pBuf |= mask;
		}
	}
}
	
//绘制字符至缓存
void OLED_DrawChar(uint8 x, uint8 y, uint8 chr)
{
    uint8 c = 0, i = 0;
    uint8 mask;
	uint8 *pBuf;
    c = chr - ' '; //得到偏移后的值
    if (x > WIDTH - _font_width)
    {
        x = 0;
        y += 8;
    }
    pBuf = &buffer[(y / 8) * WIDTH + x];

    for (i = 0; i <_font_width; i++)
    {
        mask = F6x8[c][i] << (y & 7);
		OLED_Draw_Byte(pBuf++, mask);
    }
    if (y % 8 && y<HEIGHT-8)
    {
        pBuf = &buffer[(y / 8 + 1) * WIDTH + x];
        for (i = 0; i < _font_width; i++)
        {
            mask = F6x8[c][i] >> (8 - (y & 7));
            OLED_Draw_Byte(pBuf++, mask);
        }
    }
}

//绘制字符串至缓存
void OLED_DrawString(uint8 *chr)
{
    while (*chr)
    {
        OLED_DrawChar(_x, _y, *chr++);
        _x += _font_width;

        if (_x > WIDTH - _font_width)
        {
            _x = 0;
            _y += 8;
        }
    }
}

//m^n函数
uint32 oled_pow(uint8 m, uint8 n)
{
    uint32 result = 1;
    while (n--)
        result *= m;
    return result;
}

//绘制数字至缓存
//numlen :数字的位数
//num:数值(0~4294967295);
void OLED_DrawNum(uint32 num, uint8 numlen)
{
    uint8 t, temp;
    uint8 enshow = 0;
    for (t = 0; t < numlen; t++)
    {
        temp = (num / oled_pow(10, numlen - t - 1)) % 10;
        if (enshow == 0 && t < (numlen - 1))
        {
            if (temp == 0)
            {
                OLED_DrawChar(_x + (_font_width)*t, _y, ' ');
                continue;
            }
            else
                enshow = 1;
        }
        OLED_DrawChar(_x + (_font_width)*t, _y, temp + '0');
    }
}

//绘制图片至缓存
void OLED_DrawBMP(uint8 x0, uint8 y0, uint8 x1, uint8 y1, uint8 *BMP, uint16 len)
{
    uint16 j = 0;
    uint8 x, y;
    uint8 mask;
	uint8 *pBuf;
    if (x0 > x1)
    {
        _swap_char(x0, x1);
    }
    if (y0 > y1)
    {
        _swap_char(y0, y1);
    }

    if (x1 > 127)
        x1 = 127;
    if (y1 > 63)
        y1 = 63;

    for (y = y0; y <= y1; y += 8)
    {
        for (x = x0; x <= x1; x++)
        {
            if (j > len - 1)
                break;
            pBuf = &buffer[(y / 8) * WIDTH + x];
            mask = *(BMP + j++) << (y & 7);
            OLED_Draw_Byte(pBuf++, mask);

            if (y % 8 && y < HEIGHT - 8)
            {
                j--;
                pBuf = &buffer[(y / 8 + 1) * WIDTH + x];
                mask = *(BMP + j++) >> (8 - (y & 7));
                OLED_Draw_Byte(pBuf++, mask);
            }
        }
    }
}

//绘制汉字至缓存
void OLED_DrwCHS_16x16(uint8 chs_no)
{
    if (_x > WIDTH - 16)
    {
        _x = 0;
        _y += 16;
    }
    OLED_DrawBMP(_x, _y, _x + 15, _y + 15, CHS_16x16[chs_no], sizeof(CHS_16x16[chs_no]) / sizeof(CHS_16x16[chs_no][0]));
    _x += 16;
}

//绘制一个像素至缓存
void OLED_DrawPixel(uint8 x, uint8 y)
{
    uint8 mask;
	uint8 *pBuf;
    pBuf = &buffer[(y / 8) * WIDTH + x];
    mask = 1 << (y & 7);
    *pBuf++ |= mask;
}

//绘制两点线段至缓存
void OLED_DrawLine(uint8 x0, uint8 y0, uint8 x1, uint8 y1)
{
    char dx, dy, ystep;
    int err;
    bit swapxy = 0;

    if (x0 > 127)
        x0 = 127;
    if (y0 > 63)
        y0 = 63;
    if (x1 > 127)
        x1 = 127;
    if (y1 > 63)
        y1 = 63;

    dx = abs(x1 - x0);
    dy = abs(y1 - y0);

    if (dy > dx)
    {
        swapxy = 1;
        _swap_char(dx, dy);
        _swap_char(x0, y0);
        _swap_char(x1, y1);
    }

    if (x0 > x1)
    {
        _swap_char(x0, x1);
        _swap_char(y0, y1);
    }
    err = dx >> 1;

    if (y0 < y1)
    {
        ystep = 1;
    }
    else
    {
        ystep = -1;
    }

    for (; x0 <= x1; x0++)
    {
        if (swapxy == 0)
            OLED_DrawPixel(x0, y0);
        else
            OLED_DrawPixel(y0, x0);
        err -= dy;
        if (err < 0)
        {
            y0 += ystep;
            err += dx;
        }
    }
}

//绘制垂直线至缓存
void OLED_DrawVLine(uint8 x, uint8 y, uint8 w)
{
    uint8 mask;
	uint8 *pBuf;
    while (w--)
    {
        pBuf = &buffer[(y / 8) * WIDTH + x];
        mask = 1 << (y & 7);
        *pBuf++ |= mask;
        y++;
    }
}

//绘制水平线至缓存
void OLED_DrawHLine(uint8 x, uint8 y, uint8 w)
{
	uint8 *pBuf;
    uint8 mask;
    pBuf = &buffer[(y / 8) * WIDTH + x];
    mask = 1 << (y & 7);
    while (w--)
    {
        *pBuf++ |= mask;
    }
}

//将缓存内容显示到屏幕上
void OLED_Display(void)
{
    uint8 i, j;
	uint8 *pBuf;
    OLED_Write_Command(0xB0); //设置页地址（0~7）
    OLED_Write_Command(0x00); //设置显示位置—列低地址
    OLED_Write_Command(0x10); //设置显示位置—列高地址
    pBuf = buffer;
    for (j = 0; j < HEIGHT; j += 8)
    {
        OLED_Set_Pos(0, j);
        for (i = 0; i < WIDTH; i++)
        {
            OLED_Write_Data(*pBuf++);
        }
    }
}

//初始化SSD1306
void OLED_Init(void)
{
    OLED_RST_Set();
    delay_ms(100);
    OLED_RST_Clr();
    delay_ms(100);
    OLED_RST_Set();

    OLED_Write_Command(0xAE); //--turn off oled panel
    OLED_Write_Command(0x00); //---set low column address
    OLED_Write_Command(0x10); //---set high column address
    OLED_Write_Command(0x40); //--set start line address  Set Mapping RAM Display Start Line (0x00~0x3F)
    OLED_Write_Command(0xB0); //
    OLED_Write_Command(0x81); //--set contrast control register
    OLED_Write_Command(0xFE); // Set SEG Output Current Brightness0x66
    OLED_Write_Command(0xA1); //--Set SEG/Column Mapping     0xa0左右反置 0xa1正常
    OLED_Write_Command(0xA6); // Disable Inverse Display On (0xa6/a7)
    OLED_Write_Command(0xC8); //Set COM/Row Scan Direction   0xc0上下反置 0xc8正常
    OLED_Write_Command(0xA6); //--set normal display
    OLED_Write_Command(0xA8); //--set multiplex ratio(1 to 64)
    OLED_Write_Command(0x3f); //--1/64 duty
    OLED_Write_Command(0xD3); //-set display offset	Shift Mapping RAM Counter (0x00~0x3F)
    OLED_Write_Command(0x00); //-not offset
    OLED_Write_Command(0xd5); //--set display clock divide ratio/oscillator frequency
    OLED_Write_Command(0x80); //--set divide ratio, Set Clock as 100 Frames/Sec
    OLED_Write_Command(0xD9); //--set pre-charge period
    OLED_Write_Command(0xF1); //Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
    OLED_Write_Command(0xDA); //--set com pins hardware configuration
    OLED_Write_Command(0x12);
    OLED_Write_Command(0xDB); //--set vcomh
    OLED_Write_Command(0x30); //Set VCOM Deselect Level
    OLED_Write_Command(0x20); //-Set Page Addressing Mode (0x00/0x01/0x02)
    OLED_Write_Command(0x00); //
    OLED_Write_Command(0x8D); //--set Charge Pump enable/disable
    OLED_Write_Command(0x14); //--set(0x10) disable
    OLED_Write_Command(0xA4); // Disable Entire Display On (0xa4/0xa5)
    OLED_Write_Command(0xAF); //--turn on oled panel
}
