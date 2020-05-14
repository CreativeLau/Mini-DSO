#include "global.h"

uint16 Lsb;                                                //输入采样端分压系数x100 / Input voltage dividing coefficient x 100
uint16 VBat;                                               //电池电压 / Voltage of battery
uint8 WaveLength;                                          //波长 / Wave length
uint32 WaveFreq;                                           //波频率 / Wave frequency
int16 TriLevel;                                            //用户设定触发值，mV / Trigger Level in mV
uint8 TriPos;                                              //触发位置 / Trigger Position
int8 TriPosOffset;                                         //触发位置偏移量 / Trigger Position Offset
int16 RulerVMin, RulerVMax;                                //纵轴电压最大最小值 / Min and Max voltage on vertical ruler
uint16 VMax, VMin;                                         //屏幕波形的最大和最小电压mV，上限65535mV
uint16 *BGV;                                               //内部BG参考电压
uint16 ADCbg;                                              //内部BG参考ADC
uint16 TriggerADC;                                         //触发值ADC
uint16 WaveLengthSumNum;                                   //平均波形长度求和次数
uint32 WaveLengthSum;                                      //平局波形长度求和
int8 ScaleH_tmp;                                           //临时记录采样完成的时间区间
uint8 OLED_Brightness;                                     //OLED亮度1-254
bit OLED_BrightnessChanged;                                //OLED修改标志位
bit DisplayUpdate;                                         //更新屏幕信息标志位
bit EC11PressAndRotate;                                    //EC11编码器按下同时旋转标志位
bit DisplayUpdate;                                         //更新屏幕信息标志位
bit ClearDisplay;                                          //清空屏幕标志位
bit ClearWave;                                             //清空波形标志位
bit ADCRunning;                                            //ADC采样标志位
bit ADCInterrupt;                                          //ADC采样中断标志位
bit UpdateVbat;                                            //更新电池电压信息标志位
bit OptionChanged;                                         //设置被修改标志位
bit InSettings;                                            //进入设置界面标志位
bit PlotMode;                                              //绘图模式 0:Vect 1:Dots
bit TriSlope;                                              //触发方向，1：上升触发，0：下降触发
bit TriFail;                                               //触发失败标志位
bit WaveScroll;                                            //波形滚动标志位
bit WaveUpdate;                                            //更新波形标志位
bit ScaleV_Auto;                                           //自动量程标志位
bit ADCComplete;                                           //ADC完成采样，为单次采样服务
int8 ScaleH = 2;                                           //时间区间，0-11对应500ms-100us
int8 OptionInSettings;                                     //设置界面的选项序号, 0:TriS, 1:PlotMode, 2:LSB
int8 OptionInChart;                                        //主界面的选项序号, 0:ScaleH, 1:ScaleV, 2:TriLevel, 3:TriSlope, 4:TriMode
int8 TriMode;                                              //触发方式，2:单次 1：普通，0：自动
                                                           //自动模式:连续采样，用户手动停止
                                                           //普通：触发后停止采样，并自动等待下次触发
                                                           //单次：触发后停止采样，用户手动开始等待下次触发
uint16 ADCbuf[SAMPLE_NUM];                                 //缓存原始采样数据
uint8 code *PlotModeTxt[] = {"Vect", "Dots"};              //绘图模式文字
uint8 code *TriModeTxt[] = {"Auto  ", "Normal", "Single"}; //触发方式文字
uint8 code *ScaleHTxt[] =                                  //水平标尺文字
    {
        "500ms", //0
        "200ms", //1
        "100ms", //2
        "50ms",  //3
        "20ms",  //4
        "10ms",  //5
        "5ms",   //6
        "2ms",   //7
        "1ms",   //8
        "500us", //9
        "200us", //10
        "100us"  //11
};