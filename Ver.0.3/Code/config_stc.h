/*Register of MCU and Pins defined here*/

#ifndef CONFIG_STC_H
#define CONFIG_STC_H

#include <reg51.h>
//#include <STC8.H>
//#define DEBUG

/* ADC采样通道设置
   ADC Channel Setup */
#define ADC_DSO ADC_CHS_06
#define ADC_BAT ADC_CHS_04

/* 特殊功能寄存器
    Special Function Register */
sfr AUXR = 0x8e;
sfr P0M0 = 0x94;
sfr P0M1 = 0x93;
sfr P1M0 = 0x92;
sfr P1M1 = 0x91;
sfr P2M0 = 0x96;
sfr P2M1 = 0x95;
sfr P3M0 = 0xb2;
sfr P3M1 = 0xb1;

/* EC11编码器管脚设置
   Setup EC11 Encoder Pin */
sbit EC11_KEY = P3 ^ 3; //编码器按键连接外部中断1 connect to External Interrupt 1
sbit EC11_A = P3 ^ 2;   //编码器A脉冲连接外部中断0 connect to External Interrupt 0
sbit EC11_B = P3 ^ 4;   //编码器B脉冲连接普通IO口 connect to GPIO

/* 采样运行指示灯
   Indicator for sampling running */
sbit P_Ready = P1 ^ 1;

/*  OLED Pin */
sbit OLED_SCLK = P2 ^ 7; //时钟（D0/SCLK）
sbit OLED_SDIN = P2 ^ 6; //数据（D1/MOSI）
sbit OLED_RST = P2 ^ 5;  //复位（RES）
sbit OLED_DC = P2 ^ 4;   //数据/命令控制（DC）
sbit OLED_CS = P2 ^ 3;   //片选（CS）

#ifdef DEBUG
sbit P15 = P1 ^ 5;
#endif

#endif