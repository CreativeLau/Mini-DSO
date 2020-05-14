/*
    adc_stc8.h

    Sampling with ADC built in MCU
    
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

#ifndef ADC_STC8_H
#define ADC_STC8_H

#define STC8
#ifdef STC8

#include "config_stc.h"
#include "intrins.h"
#include "delay.h"
#include "chart.h"
#include <string.h>

#ifndef uint8
#define uint8 unsigned char
#endif

#ifndef uint16
#define uint16 unsigned int
#endif

#ifndef uint32
#define uint32 unsigned long int
#endif

//ADC 特殊功能寄存器
sfr ADC_CONTR = 0xbc;
sfr ADC_RES = 0xbd;
sfr ADC_RESL = 0xbe;
sfr ADCCFG = 0xde;

//Bandgap电压在RAM和ROM中的存放地址，单位mV，RAM中的数据有可能被冲掉，所以推荐使用ROM中的数据，RAM中的数据可以作为备选
//WORD idata Vbg_RAM _at_ 0xef; //对于只有256字节RAM的MCU存放地址为0EFH
//idata 的 EFH 地址存放高字节
//idata 的 F0H 地址存放低字节
//WORD idata Vbg_RAM _at_ 0x6f; //对于只有128字节RAM的MCU存放地址为06FH
//注意:需要在下载代码时选择"在ID号前添?重要测试参数"选项,才可在程序ROM中获取此参数
//WORD code Vbg_ROM _at_ 0x03f7; //1K程序空间的MCU
//WORD code Vbg_ROM _at_ 0x07f7; //2K程序空间的MCU
//WORD code Vbg_ROM _at_ 0x0bf7; //3K程序空间的MCU
//WORD code Vbg_ROM _at_ 0x0ff7; //4K程序空间的MCU
//WORD code Vbg_ROM _at_ 0x13f7; //5K程序空间的MCU
//WORD code Vbg_ROM _at_ 0x1ff7; //8K程序空间的MCU
//WORD code Vbg_ROM _at_ 0x27f7; //10K程序空间的MCU
//WORD code Vbg_ROM _at_ 0x2ff7; //12K程序空间的MCU
//WORD code Vbg_ROM _at_ 0x3ff7; //16K程序空间的MCU
//WORD code Vbg_ROM _at_ 0x4ff7; //20K程序空间的MCU
//WORD code Vbg_ROM _at_ 0x5ff7; //24K程序空间的MCU
//WORD code Vbg_ROM _at_ 0x6ff7; //28K程序空间的MCU
//WORD code Vbg_ROM _at_ 0x7ff7; //32K程序空间的MCU
//WORD code Vbg_ROM _at_ 0x9ff7; //40K程序空间的MCU
//WORD code Vbg_ROM _at_ 0xbff7; //48K程序空间的MCU
//WORD code Vbg_ROM _at_ 0xcff7; //52K程序空间的MCU
//WORD code Vbg_ROM _at_ 0xdff7; //56K程序空间的MCU
//WORD code Vbg_ROM _at_ 0xeff7; //60K程序空间的MCU
//WORD code Vbg_ROM _at_ 0xfdf7; //64K程序空间的MCU
#define BGV_ADR 0xfdf7
#define ADC_POWER 0x80     //ADC电源控制位
#define ADC_FLAG 0x20      //ADC完成标志
#define ADC_START 0x40     //ADC起始控制位
#define ADC_SPEED_512 0x0F //512个时钟
#define ADC_SPEED_480 0x0E //480个时钟
#define ADC_SPEED_448 0x0D //448个时钟
#define ADC_SPEED_416 0x0C //416个时钟
#define ADC_SPEED_384 0x0B //384个时钟
#define ADC_SPEED_352 0x0A //352个时钟
#define ADC_SPEED_320 0x09 //320个时钟
#define ADC_SPEED_288 0x08 //288个时钟
#define ADC_SPEED_256 0x07 //256个时钟
#define ADC_SPEED_224 0x06 //224个时钟
#define ADC_SPEED_192 0x05 //192个时钟
#define ADC_SPEED_160 0x04 //160个时钟
#define ADC_SPEED_128 0x03 //128个时钟
#define ADC_SPEED_96 0x02  //96个时钟
#define ADC_SPEED_64 0x01  //64个时钟
#define ADC_SPEED_32 0x00  //32个时钟
#define RESFMT 0x20        //转换结果控制位，左对齐0x00，右对齐0x20
#define ADC_CHS_10 0x00    //将P1.0的模拟值输入ADC模拟通道
#define ADC_CHS_11 0x01    //将P1.1的模拟值输入ADC模拟通道
#define ADC_CHS_12 0x02    //将P1.2的模拟值输入ADC模拟通道
#define ADC_CHS_13 0x03    //将P1.3的模拟值输入ADC模拟通道
#define ADC_CHS_14 0x04    //将P1.4的模拟值输入ADC模拟通道
#define ADC_CHS_15 0x05    //将P1.5的模拟值输入ADC模拟通道
#define ADC_CHS_16 0x06    //将P1.6的模拟值输入ADC模拟通道
#define ADC_CHS_17 0x07    //将P1.7的模拟值输入ADC模拟通道
#define ADC_CHS_00 0x08    //将P0.0的模拟值输入ADC模拟通道
#define ADC_CHS_01 0x09    //将P0.1的模拟值输入ADC模拟通道
#define ADC_CHS_02 0x0A    //将P0.2的模拟值输入ADC模拟通道
#define ADC_CHS_03 0x0B    //将P0.3的模拟值输入ADC模拟通道
#define ADC_CHS_04 0x0C    //将P0.4的模拟值输入ADC模拟通道
#define ADC_CHS_05 0x0D    //将P0.5的模拟值输入ADC模拟通道
#define ADC_CHS_06 0x0E    //将P0.6的模拟值输入ADC模拟通道
#define ADC_CHS_BG 0x0F    //将第16通道（内部BandGap参考电压）的模拟值输入ADC模拟通道

uint16 GetADC_CHX(uint8 chx);                 //获得chx通道ADC
uint16 GetVoltage(uint8 chx, uint16 lsb);     //获得chx通道电压值 mV
uint16 *GetBGV(void);                         //获得内部参考电压BGV mV
uint16 *GetWaveADC(uint8 chx, uint8 scale_h); //获得波形采样

#endif
#endif