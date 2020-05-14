#ifndef CHART_H
#define CHART_H

#include "ssd1306.h"
#include "adc_stc8.h"
#include "settings.h"
#include "config_stc.h"

#ifndef uint8
#define uint8 unsigned char
#endif

#ifndef uint16
#define uint16 unsigned int
#endif

#ifndef int16
#define int16 int
#endif

#ifndef uint32
#define uint32 unsigned long int
#endif

/* 屏幕上波形显示区域
   Region for waveform on display*/
#define CHART_H_MAX 126 
#define CHART_H_MIN 26
#define CHART_V_MAX 52
#define CHART_V_MIN 8

void PlotChart(void);
void PlotSettings(void);
void PlotSaveStatus(bit _saveStatus);
void GetWaveData(void);
bit GetTriggerPos(uint16 d1, uint16 d2, uint16 dTrigger, bit triSlope);
uint16 Convert_mv_ADC(uint16 mv, uint16 *BGV, uint16 BGADC, uint16 lsb);
void AnalyseData();
void PlotWave();

#endif