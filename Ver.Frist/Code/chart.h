#ifndef CHART_H
#define CHART_H

#include "ssd1306.h"
#include "adc_stc8.h"
#include "settings.h"


#ifndef uint8
#define uint8 unsigned char
#endif

#ifndef uint16
#define uint16 unsigned int
#endif

#ifndef uint32
#define uint32 unsigned long int
#endif

extern uint16 *WaveData;
extern uint16 VBat;
extern bit TriD;
extern bit HoldDisplay; 	
extern bit DisplayUpdate; 
extern bit OptionChanged;  	
extern bit ClearDisplay;  
extern bit WaitADC; 	
extern bit UpdateVbat; 
extern bit TriggerFail;  
void PlotChart(void);
void PlotSettings(void);
void PlotSaveStatus(bit _saveStatus);
void GetWaveData(void);
void AnalyseData();
void PlotWave();

#endif