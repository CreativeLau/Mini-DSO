/*
    ssd1306.c

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

    Note:

    版本：V0.2
    作者：老刘爱捣鼓
    时间：2020-04-24
    老刘爱捣鼓（全网同名）期待您的关注！

    说明：
    为Mini DSO这个项目临时编写的SSD1306 128x64驱动，并不完善
    有写字符，字符串，数字，16x16中文字符，画水平线，垂直线，任意两点线段，绘制图片等功能
    主要参考了Arduino中Adafruit SSD1306的库函数
    采用全屏buf驱动，需占用128x8个uint8内存空间
    全屏缓存操控屏幕内容比较方便，刷新速度快，但对单片机的内存大小有要求
    F6x8字体来自中景园电子的DEMO

    2020-04-30
    1.  更新OLED_Draw_Byte函数，修正以覆盖方式跨页写入时会同时修改两页内容，上下页的其他字符被覆盖，现在可以在任意点写入字符，不会影响上下页的内容
    2.  优化算法/8改为>>3，%8改为&7
    3.  修正OLED初始化错误，行模式要设置0x21和0x22设置显示范围，并用0x40设置起始行，
        0xB0-0xB7设置起始页是页面模式专用，在行模式下使用会造成设置的起始页第一次到达结尾时无法自动换页
    4.  删除OLED_Set_Pos中多余的设置内存位置命令
    5.  画线或像素时，置位OLED_Reverse则进行擦除
*/
#include "ssd1306.h"
#include "ssd1306font.h"

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
#define _swap_char(a, b) \
    {                    \
        uint8 t = a;     \
        a = b;           \
        b = t;           \
    }
#endif

bit _OLED_Reverse = 0;     
bit _OLED_Overlap = 1;
uint8 _buf[WIDTH * PAGES]; //全屏缓存,横向WIDTH个像素，纵向PAGES页，页内每8个像素作为一个字节，共WIDTH * PAGES个字节

static char _x, _y;
static uint8 _Font_Width = 6;

/*  写命令
    Write Command   */
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

/*  写数据
    Write Data  */
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

/* 1：反显，0：正常
   1: Reverse, 0: Normal*/
void OLED_Reverse(bit i)
{
    _OLED_Reverse = i;
}

/* 绘图模式：1：叠加，0：覆盖
   Draw Mode: 1: overlap, 0: Redraw*/
void OLED_Overlap(bit i)
{
    _OLED_Overlap = i;
}

/* 设置ASCII字符宽度
   Set ASCII character width*/
void OLED_SetFontWidth(uint8 w)
{
    _Font_Width = w;
}

/* 设置内存位置x:0-127，y:0-63
   Set Ram address x:0-127, y:0-63*/
void OLED_Set_Pos(uint8 x, uint8 y)
{
    _x = x;
    _y = y;
}

/* 清除缓存内容
   Clear data in buffer*/
void OLED_Clear(void)
{
    memset(_buf, 0x00, WIDTH * PAGES);
}

/*  绘制字节至缓存
    offset: 字节偏移量
    reserve_hl: 保留高位或低位
    比如要写入的字节1111 1111跨了两页0001 1111 1110 0000，
    在覆盖模式下，为避免覆盖相邻的内容，在写入0001 1111时，指定offset=3，reserve_hl=1，写入1110 0000时指定指定offset=5，reserve_hl=0
    Draw byte to buffer
    offset: Offset of Byte
    reserve_hl: Reserve High or Low
    If the byte 1111 1111 we want to write cross two pages 0001 1111 1110 0000
    In redraw mode, to avoid affect adjacent content. Set offset=3, reserve_hl=1, when write 0001 1111. Set offset=5, reserve_hl=0 when write 1110 0000
    */
void OLED_Draw_Byte(uint8 *pBuf, uint8 mask, uint8 offset, bit reserve_hl)
{
    if (_OLED_Overlap)
    {
        if (_OLED_Reverse)
            *pBuf |= ~mask;
        else
            *pBuf |= mask;
    }
    else
    {
        if (_OLED_Reverse)
        {
            /* 保留高位
               Reserve upper */
            if (reserve_hl) 
            {
                *pBuf &= (~mask) | (0xFF << (8 - offset));
                *pBuf |= (~mask) & (0xFF >> offset);
            }
            /* 保留低位
               Reserve lower */
            else 
            {
                *pBuf &= (~mask) | (0xFF >> (8 - offset));
                *pBuf |= (~mask) & (0xFF << offset);
            }
        }
        else
        {
            /* 保留高位
               Reserve upper */
            if (reserve_hl) 
            {
                *pBuf &= mask | (0xFF << (8 - offset));
                *pBuf |= mask & (0xFF >> offset);
            }
            /* 保留低位
               Reserve lower */ 
            else 
            {
                *pBuf &= mask | (0xFF >> (8 - offset));
                *pBuf |= mask & (0xFF << offset);
            }
        }
    }
}

/*  绘制字符至缓存
    Draw char to buffer*/
void OLED_DrawChar(uint8 x, uint8 y, uint8 chr)
{
    uint8 c;
    uint8 i;
    uint8 mask;
    uint8 *pBuf;
    uint8 offset;
    offset = y & 7;
    c = chr - ' ';

    pBuf = &_buf[(y >> 3) * WIDTH + x];

    for (i = 0; i < _Font_Width; i++)
    {
        mask = F6x8[c][i] << offset;
        OLED_Draw_Byte(pBuf++, mask, offset, 0);
    }

    if (offset && y < HEIGHT - 8)
    {
        pBuf = &_buf[((y >> 3) + 1) * WIDTH + x];
        for (i = 0; i < _Font_Width; i++)
        {
            mask = F6x8[c][i] >> (8 - offset);
            OLED_Draw_Byte(pBuf++, mask, 8 - offset, 1);
        }
    }
}

/*  绘制字符串至缓存
    Draw string to buffer   */
void OLED_DrawString(uint8 *chr)
{
    while (*chr)
    {
        if (_x > WIDTH - _Font_Width)
        {
            _x = 0;
            _y += 8;
        }
        if (_y > HEIGHT - 8)
        {
            _y = 0;
        }

        OLED_DrawChar(_x, _y, *chr++);
        _x += _Font_Width;
    }
}

/* m^n */
uint32 oled_pow(uint8 m, uint8 n) //reentrant
{
    /* 常规算法 */
    uint32 result = 1;

    while (n--)
        result *= m;

    return result;

    /* 递归算法 需用reentrant修饰函数*/
    // if (n != 0)
    //     return (m * oled_pow(m, n - 1));
    // else
    //     return 1;
}

/*  绘制数字至缓存
    Draw digit to buffer
    len: length of digit
    digit:  0~4294967295   */
void OLED_DrawNum(uint32 digit, uint8 len)
{
    uint8 t, i, temp;
    uint8 enshow = 0;
    i = 0;
    for (t = 0; t < len; t++)
    {
        temp = (digit / oled_pow(10, len - t - 1)) % 10;

        if (enshow == 0 && t < (len - 1))
        {
            if (temp == 0)
            {
                i++;
                continue;
            }
            else
                enshow = 1;
        }

        if (_x > WIDTH - _Font_Width)
        {
            _x = 0;
            _y += 8;
        }
        if (_y > HEIGHT - 8)
        {
            _y = 0;
        }

        OLED_DrawChar(_x + (_Font_Width) * (t - i), _y, temp + '0');
    }
    _x += len;
}

/*  绘制16x16汉字至缓存
    Draw 16x16 Chinese to buffer    */
void OLED_DrwCHS_16x16(uint8 chs_no)
{
    if (_x > WIDTH - 16)
    {
        _x = 0;
        _y += 16;
    }
    if (_y > HEIGHT - 16)
    {
        _y = 0;
    }
    OLED_DrawBMP(_x, _y, _x + 15, _y + 15, CHS_16x16[chs_no], sizeof(CHS_16x16[chs_no]) / sizeof(CHS_16x16[chs_no][0]));
    _x += 16;
}

/*  绘制一个像素至缓存
    Draw one pixel to buffer    */
void OLED_DrawPixel(uint8 x, uint8 y)
{
    uint8 mask;
    uint8 *pBuf;

    if (_x > WIDTH - 1)
    {
        _x = 0;
        _y += 1;
    }
    if (_y > HEIGHT - 1)
    {
        _y = 0;
    }

    pBuf = &_buf[(y >> 3) * WIDTH + x];
    mask = 1 << (y & 7);
    if (_OLED_Reverse)
    {
        *pBuf++ &= ~mask;
    }
    else
    {
        *pBuf++ |= mask;
    }
}

/*  绘制两点线段至缓存
    Draw line between two points to buffer  */
void OLED_DrawLine(uint8 x0, uint8 y0, uint8 x1, uint8 y1)
{
    char dx, dy, ystep;
    int err;
    bit swapxy = 0;

    if (x0 > WIDTH - 1)
        x0 = WIDTH - 1;

    if (y0 > HEIGHT - 1)
        y0 = HEIGHT - 1;

    if (x1 > WIDTH - 1)
        x1 = WIDTH - 1;

    if (y1 > HEIGHT - 1)
        y1 = HEIGHT - 1;

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

/*  绘制垂直线至缓存
    Draw vertical line to buffer*/
void OLED_DrawVLine(uint8 x, uint8 y, uint8 w)
{
    uint8 mask;
    uint8 *pBuf;

    if (x > WIDTH - 1)
        x = WIDTH - 1;

    if (y + w > HEIGHT)
        w = HEIGHT - y;

    while (w--)
    {
        pBuf = &_buf[(y >> 3) * WIDTH + x];
        mask = 1 << (y & 7);
        if (_OLED_Reverse)
        {
            *pBuf++ &= ~mask;
        }
        else
        {
            *pBuf++ |= mask;
        }
        y++;
    }
}

/*  绘制水平线至缓存
    Draw horizontal line to buffer  */
void OLED_DrawHLine(uint8 x, uint8 y, uint8 w)
{
    uint8 *pBuf;
    uint8 mask;

    if (x + w > WIDTH)
        w = WIDTH - x;

    if (y > HEIGHT - 1)
        y = HEIGHT - 1;

    pBuf = &_buf[(y >> 3) * WIDTH + x];
    mask = 1 << (y & 7);

    while (w--)
    {
        if (_OLED_Reverse)
        {
            *pBuf++ &= ~mask;
        }
        else
        {
            *pBuf++ |= mask;
        }
    }
}

/*  绘制图片至缓存
    Draw pic to buffer  */
void OLED_DrawBMP(uint8 x0, uint8 y0, uint8 x1, uint8 y1, uint8 *BMP, uint16 len)
{
    uint16 j = 0;
    uint8 x, y;
    uint8 mask;
    uint8 *pBuf;
    uint8 offset;

    if (x0 > x1)
    {
        _swap_char(x0, x1);
    }

    if (y0 > y1)
    {
        _swap_char(y0, y1);
    }

    if (x0 > WIDTH - 1)
        x0 = WIDTH - 1;

    if (y0 > HEIGHT - 1)
        y0 = HEIGHT - 1;

    if (x1 > WIDTH - 1)
        x1 = WIDTH - 1;

    if (y1 > HEIGHT - 1)
        y1 = HEIGHT - 1;

    for (y = y0; y <= y1; y += 8)
    {
        offset = y & 7;

        for (x = x0; x <= x1; x++)
        {
            if (j > len - 1)
                break;

            pBuf = &_buf[(y >> 3) * WIDTH + x];
            mask = *(BMP + j++) << offset;
            OLED_Draw_Byte(pBuf++, mask, offset, 0);

            if (offset && y < HEIGHT - 8)
            {
                j--;
                pBuf = &_buf[((y >> 3) + 1) * WIDTH + x];
                mask = *(BMP + j++) >> (8 - offset);
                OLED_Draw_Byte(pBuf++, mask, 8 - offset, 1);
            }
        }
    }
}

/*  将缓存内容显示到屏幕上
    Send buffer to display  */
void OLED_Display(void)
{
    uint8 i, j;
    uint8 *pBuf;
    pBuf = _buf;

    for (j = 0; j < PAGES; j++)
    {
        for (i = 0; i < WIDTH; i++)
        {
            OLED_Write_Data(*pBuf++);
        }
    }
}

/*  初始化SSD1306
    Initialize SSD1306  */
void OLED_Init(void)
{
    OLED_RST_Set();
    Delay50ms();
    OLED_RST_Clr();
    Delay50ms();
    OLED_RST_Set();

    OLED_Write_Command(0xAE); //--Turn off oled panel

    /*  
        以下三条命令是给页面模式用的，行模式和列模式不要设置，会引起问题
        行模式或列模式，要用0x21和0x22设置范围
        Following three commands are for Page Addressing Mode. Do not set them in Horizontal addressing mode or Vertical addressing mode, will rise problem.
        For Horizontal addressing mode or Vertical addressing mode, should use 0x21 and 0x22 set column and page address
    */
    //	OLED_Write_Command(0x00); //-Set Lower Column Start Address for Page Addressing Mode
    //	OLED_Write_Command(0x10); //-Set Higher Column Start Address for Page Addressing Mode
    //	OLED_Write_Command(0xB0); //-Set the page start address of the target display location by command for Page Addressing Mode

    OLED_Write_Command(0x20); //-Set Page Addressing Mode (0x00/0x01/0x02)
    OLED_Write_Command(0x00); //--0x00: Horizontal addressing mode, 0x01: Vertical addressing mode, 0x02: Page addressing mode
    
    OLED_Write_Command(0x21); //-Set Column Address
    OLED_Write_Command(0x00); //--Start address
    OLED_Write_Command(0x7f); //--End address

    OLED_Write_Command(0x22); //-Set Page Address
    OLED_Write_Command(0x00); //---Start address
    OLED_Write_Command(0x07); //--End address

    OLED_Write_Command(0x40); //-Set Display Start Line (0x40h~0x7F)

    OLED_Write_Command(0x81);                 //-Set Contrast Control for BANK0
    OLED_Write_Command(OLED_Brightness * 10); //    -0x00 to 0xFF, The segment output current increases as the contrast step value increases

    OLED_Write_Command(0xA1); //-Set Segment Re-map. 0xA1: Normal, 0xA0: Re-map left and right
    OLED_Write_Command(0xC8); //-Set COM Output Scan Direction. 0xC8: Normal, 0xC0: Re-map up and down
    OLED_Write_Command(0xA6); //-Set Normal/Inverse Display, 0xA6:Normal, 0xA7: Inverse

    OLED_Write_Command(0xA8); //-Set Multiplex Ratio (16~63)
    OLED_Write_Command(0x3F); //--63 multiplex mode

    OLED_Write_Command(0xD3); //-Set Display Offset (0x00~0x3F)
    OLED_Write_Command(0x00); //--No offset

    OLED_Write_Command(0xD5); //-Set display clock divide ratio/oscillator frequency
    OLED_Write_Command(0x20); //--Set Clock as 60 Frames/Sec

    OLED_Write_Command(0xD9); //-Set pre-charge period
    OLED_Write_Command(0xF2); //--Set Pre-Charge as 15 Clocks & Discharge as 2 Clock

    OLED_Write_Command(0xDA); //-Set com pins hardware configuration
    OLED_Write_Command(0x12); //

    OLED_Write_Command(0xDB); //-Set VCOM Deselect Level
    OLED_Write_Command(0x30); //--0.83xVcc

    OLED_Write_Command(0x8D); //-Set Charge Pump enable/disable
    OLED_Write_Command(0x14); //--0x14: Enable, 0x10: Disable

    OLED_Write_Command(0xA4); //-Entire Display ON, 0xA4: Disable, 0xA5: Enable
    OLED_Write_Command(0xAF); //-Turn on oled panel
}
