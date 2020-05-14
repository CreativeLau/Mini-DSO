/*
    settings.h

    Settings Control and Parameter Adjusting

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

#ifndef SETTINGS_H
#define SETTINGS_H

#include "eeprom.h"
#include "chart.h"

#ifndef uint8
#define uint8 unsigned char
#endif

#ifndef uint16
#define uint16 unsigned int
#endif

#ifndef uint32
#define uint32 unsigned long int
#endif

#define SCALE_H_MAX 11 //时间区间最大序号
#define SCALE_H_MIN 0  //时间区间最小序号
#define MAX_V 30000	   //纵轴电压最大值 mV
#define MIN_V 0		   //纵轴电压最小值 mV
#define LSB_MAX 750	   //分压系数，分压比x100,5k和1k电阻分压系数为(5+1)/1*100,
					   //算法限制：由于STC8A8K储存的最大整数是2^32, LSB最大不能超过2^32/4095/1349=777
					   //12位ADC范围=2^12=4095,BGV=1349mV
#define LSB_MIN 50

void Change_Val(bit);
bit Save_Options();
void Read_Options();
void check_Options();
void Check_MinTimeInterval();

#endif