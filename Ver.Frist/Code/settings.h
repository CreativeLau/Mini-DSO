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

#define SCALE_H_MAX 11
#define SCALE_H_MIN 0
#define SCALE_V_MAX 10
#define SCALE_V_MIN 0
#define MAX_V 30000 //mV
#define MIN_V 0
#define LSB_MAX 750 //分压系数，分压比x100,5k和1k电阻分压系数为(5+1)/1*100, 
					//算法限制：由于STC8A8K储存的最大整数是2^32, LSB最大不能超过2^32/4095/1349=777
					//12位ADC范围=2^12=4095,BGV=1349mV
#define LSB_MIN 50

extern uint8 code *DrawModeTxt[]; //图表的显示形式
//extern uint8 code *ScaleVTxt[]; //图表垂直标尺信息
extern uint8 code *ScaleHTxt[]; //图表水平标尺信息
extern char Option_In_Settings;
extern char Option_In_Chart;  
extern char Scale_H;
extern char Scale_V;
extern bit Draw_Mode;
extern bit In_Settings;
extern uint16 Lsb;

void Change_Val(bit);
bit Save_Options();
void Read_Options();

#endif