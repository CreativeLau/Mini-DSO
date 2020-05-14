#ifndef CONFIG_STC_H
#define CONFIG_STC_H

#include <reg51.h>
//#include <STC8.H>

#define ADC_DSO ADC_CHS_06
#define ADC_BAT ADC_CHS_04

sfr AUXR        =   0x8e;

sbit EC11_KEY = P3 ^ 3; //编码器按键连接外部中断1
sbit EC11_A = P3 ^ 2;   //编码器A脉冲连接外部中断0
sbit EC11_B = P3 ^ 4;   //编码器B脉冲连接普通IO口

#endif