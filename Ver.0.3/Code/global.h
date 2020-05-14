/* All global variables defined here*/
#ifndef GLOBAL_H
#define GLOBAL_H

#ifndef uint8
#define uint8 unsigned char
#endif

#ifndef uint16
#define uint16 unsigned int
#endif

#ifndef uint32
#define uint32 unsigned long int
#endif

#ifndef int8
#define int8 signed char
#endif

#ifndef int16
#define int16 signed int
#endif

#define SAMPLE_NUM 220      //采样缓存点数，显示波形需101个点，查找触发点从第51个点到第SAMPLE_NUM-50个点，
#define PRE_BUF_NUM 62      //单次触发和普通触发的预缓存点数大于等于52
#define AFT_BUF_NUM 158     //单次触发和普通触发的触发后缓存点数等于SAMPLE_NUM-PRE_BUF_NUM

extern uint16 VBat;
extern uint16 Lsb;
extern int16 TriLevel;
extern uint8 TriPos;
extern int8 TriPosOffset;
extern uint8 WaveLength;
extern uint32 WaveFreq;
extern int16 RulerVMin, RulerVMax;
extern uint16 VMax, VMin;
extern uint16 *BGV;
extern uint16 ADCbg;
extern uint16 TriggerADC;
extern uint16 WaveLengthSumNum;
extern uint32 WaveLengthSum;
extern int8 ScaleH_tmp;
extern uint8 OLED_Brightness;
extern bit OLED_BrightnessChanged;
extern bit EC11PressAndRotate;
extern bit DisplayUpdate;
extern bit ClearDisplay;
extern bit ClearWave;
extern bit ADCRunning;
extern bit ADCInterrupt;
extern bit UpdateVbat;
extern bit TriFail;
extern bit PlotMode;
extern bit OptionChanged;
extern bit TriSlope;
extern bit InSettings;
extern bit WaveScroll;
extern bit WaveUpdate;
extern bit ScaleV_Auto;
extern bit ADCComplete;
extern int8 OptionInSettings;
extern int8 OptionInChart;
extern int8 ScaleH;
extern int8 TriMode;
extern uint16 ADCbuf[SAMPLE_NUM];
extern uint8 code *PlotModeTxt[];
extern uint8 code *TriModeTxt[];
extern uint8 code *ScaleHTxt[];
#endif