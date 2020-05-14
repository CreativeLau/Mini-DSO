/*
    settings.c

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

#include "settings.h"

    //单片机速度限制，单次和普通触发的中间计算造成单片机无法完成100us区间计算
    //MCU speed limitation result in the Single Sweep and Normal Sweep calculations could not complete in 100us time interval.
    void
    Check_MinTimeInterval()
{
    if (TriMode)
    {
        if (ScaleH > SCALE_H_MAX - 1)
            ScaleH = SCALE_H_MAX - 1;
    }
    else
    {
        if (ScaleH > SCALE_H_MAX)
            ScaleH = SCALE_H_MAX;
    }
}

void change_ScaleH(bit i)
{
    if (i)
    {
        ScaleH++;
        Check_MinTimeInterval();
    }
    else
    {
        ScaleH--;

        if (ScaleH < SCALE_H_MIN)
            ScaleH = SCALE_H_MIN;
    }
}

void change_TriggerValue(bit i)
{
    if (i)
    {
        if (TriLevel >= 5000)
            TriLevel += 500;
        else
            TriLevel += 100;

        if (TriLevel > MAX_V)
            TriLevel = MAX_V;
    }
    else
    {
        if (TriLevel <= 5000)
            TriLevel -= 100;
        else
            TriLevel -= 500;

        if (TriLevel < MIN_V)
            TriLevel = MIN_V;
    }
}

void change_LSB(bit i)
{
    if (i)
    {
        Lsb++;

        if (Lsb > LSB_MAX)
            Lsb = LSB_MAX;
    }
    else
    {
        Lsb--;

        if (Lsb < LSB_MIN)
            Lsb = LSB_MIN;
    }
}

void change_PlotMode()
{
    PlotMode = ~PlotMode;
}

void change_TriMode(bit i)
{
    if (i)
    {
        TriMode++;

        if (TriMode > 2)
            TriMode = 0;
    }
    else
    {
        TriMode--;

        if (TriMode < 0)
            TriMode = 2;
    }
}

void change_TriPosOffset(bit i)
{
    if (i)
    {
        TriPosOffset--;
        if (TriPosOffset + TriPos < ((CHART_H_MAX - CHART_H_MIN) >> 1))
            TriPosOffset = ((CHART_H_MAX - CHART_H_MIN) >> 1) - TriPos;
    }
    else
    {
        TriPosOffset++;
        if (TriPosOffset + TriPos > SAMPLE_NUM - ((CHART_H_MAX - CHART_H_MIN) >> 1) - 1)
            TriPosOffset = SAMPLE_NUM - ((CHART_H_MAX - CHART_H_MIN) >> 1) - 1 - TriPos;
    }
}

void change_OptionInChart(bit i)
{
    if (i)
    {
        OptionInChart++;

        if (OptionInChart > 4)
            OptionInChart = 0;
    }
    else
    {
        OptionInChart--;

        if (OptionInChart < 0)
            OptionInChart = 4;
    }
}

void change_OptionInSettings(bit i)
{
    if (i)
    {
        OptionInSettings++;
        if (OptionInSettings > 2)
            OptionInSettings = 0;
    }
    else
    {
        OptionInSettings--;
        if (OptionInSettings < 0)
            OptionInSettings = 2;
    }
}

void change_RulerVMax(bit i)
{
    if (i)
    {
        if (RulerVMax <= 1000)
        {
            RulerVMax -= 100;
            //RulerVMin += 100;
        }
        else
        {
            RulerVMax -= 500;
            //RulerVMin += 500;
        }

        if (RulerVMax <= RulerVMin)
            ScaleV_Auto = 1;
    }
    else
    {
        ScaleV_Auto = 0;
        if (RulerVMax >= 1000)
        {
            RulerVMax += 500;
            //RulerVMin -= 500;
        }
        else
        {
            RulerVMax += 100;
            //RulerVMin -= 100;
        }

        if (RulerVMax > MAX_V)
            RulerVMax = MAX_V;

        // if (RulerVMin < MIN_V)
        //     RulerVMin = MIN_V;
    }
}

void change_RulerV(bit i)
{
    ScaleV_Auto = 0;
    if (i)
    {
        RulerVMax -= 100;
        RulerVMin -= 100;
        if (RulerVMin < MIN_V + 100)
        {
            RulerVMax += 100;
            RulerVMin = MIN_V;
        }
    }
    else
    {
        RulerVMax += 100;
        RulerVMin += 100;
        if (RulerVMax > MAX_V)
        {
            RulerVMax = MAX_V;
            RulerVMin -= 100;
        }
    }
}

void change_OLED_Brightness(bit i)
{
    if (i)
    {
        OLED_Brightness++;
        if (OLED_Brightness > BRIGHTNESS_MAX)
            OLED_Brightness = BRIGHTNESS_MAX;
    }
    else
    {
        OLED_Brightness--;
        if (OLED_Brightness < BRIGHTNESS_MIN)
            OLED_Brightness = BRIGHTNESS_MIN;
    }
    OLED_BrightnessChanged = 1;
}

void Change_Val(bit i)
{
    /* 按下同时旋转编码器
       Rotating Encoder while pressing */
    if (EC11PressAndRotate)
    {
        //设置界面
        //In settings
        if (InSettings)
        {
            change_OptionInSettings(i);
        }

        //波形滚动模式
        //In waveform scroll mode
        else if (WaveScroll)
        {
            change_RulerV(i);
            WaveUpdate = 1; //置位波形更新标志
        }

        //非波形滚动模式
        //Not in waveform scroll mode
        else
        {
            change_OptionInChart(i);
            //切换选项时，不清空波形
            //Not to clear waveform when switch options
            ClearWave = 0;
        }
    }

    /* 在设置界面旋转编码器
       Rotating Encoder in settings */
    else if (InSettings)
    {
        /* 调节绘图模式
           Ajust Plot Mode */
        if (OptionInSettings == 0)
            change_PlotMode();

        /* 调节LSB
           Adjust LSB */
        else if (OptionInSettings == 1)
            change_LSB(i);

        /* 调节OLED_Brightness
           Adjust OLED_Brightness */
        else if (OptionInSettings == 2)
            change_OLED_Brightness(i);

        /* 置位设置保存标志
           Options need to be saved */
        OptionChanged = 1;
    }

    /* 滚动波形模式下旋转编码器
       Rotate Encoder in waveform horizontal scroll mode */
    else if (WaveScroll)
    {
        change_TriPosOffset(i);
        WaveUpdate = 1; 
    }

    /* 在主界面旋转编码器
       Rotate Encoder in Main interface */
    else
    {
        /* 调节时间区间
           Adjust time scale */
        if (OptionInChart == 0)
        {
            change_ScaleH(i);
            WaveUpdate = 1;
            ClearWave = 1;
            WaveLengthSumNum = 0; 
            WaveLengthSum = 0;   
        }

        /* 调节量程
           Adjust measuring range */
        else if (OptionInChart == 1)
        {
            change_RulerVMax(i);
            WaveUpdate = 1;
            ClearWave = 0;
        }

        /* 调节触发值
           Adjust Trigger level */
        else if (OptionInChart == 2)
        {
            change_TriggerValue(i);
            WaveUpdate = 0;
            ClearWave = 0;
        }

        /* 调节触发方向
           Adjust Trigger Slope */
        else if (OptionInChart == 3)
        {
            TriSlope = ~TriSlope;
            WaveUpdate = 0;
            ClearWave = 0;
        }

        /* 更改触发方式
           Switch Trigger Mode */
        if (OptionInChart == 4)
        {
            change_TriMode(i);
            Check_MinTimeInterval(); //检查时间区间的合法性
            WaveUpdate = 1;
            ClearWave = 1;
            WaveLengthSumNum = 0; 
            WaveLengthSum = 0;   
        }

        /* 置位设置保存标志
           Options need to be saved */
        OptionChanged = 1;
    }
}

void check_Options()
{
    if (Lsb > LSB_MAX || Lsb < LSB_MIN)
        Lsb = 600;

    if (ScaleH > SCALE_H_MAX || ScaleH < SCALE_H_MIN)
        ScaleH = 0;

    if (RulerVMax > MAX_V || RulerVMax < MIN_V)
        RulerVMax = 0;

    if (RulerVMin > MAX_V || RulerVMin < MIN_V)
        RulerVMin = 0;

    if (RulerVMax <= RulerVMin)
    {
        RulerVMax = RulerVMin;
        ScaleV_Auto = 1;
    }

    if (TriLevel > MAX_V || TriLevel < MIN_V)
        TriLevel = 2000;

    if (TriMode > 2 || TriMode < 0)
        TriMode = 0;

    if (OLED_Brightness > BRIGHTNESS_MAX || OLED_Brightness < BRIGHTNESS_MIN)
        OLED_Brightness = 10;

    Check_MinTimeInterval();
}

bit Save_Options()
{
    uint8 ops[15];
    uint8 *p;
    check_Options();
    p = ops;
    *p++ = Lsb >> 8;       //写入Lsb高8位 uint16
    *p++ = Lsb;            //写入Lsb低8位
    *p++ = PlotMode;       //写入绘图模式 bit
    *p++ = ScaleH;         //写入时间区间 char
    *p++ = ScaleV_Auto;    //读取自动量程标志 bit
    *p++ = RulerVMax >> 8; //写入纵轴电压高8位 int16
    *p++ = RulerVMax;      //写入纵轴电压低8位
    *p++ = RulerVMin >> 8; //写入纵轴电压高8位 int16
    *p++ = RulerVMin;      //写入纵轴电压低8位
    *p++ = TriLevel >> 8;  //写入触发值高8位 int16
    *p++ = TriLevel;       //写入触发值低8位
    *p++ = TriMode;        //写入触发方式 int8
    *p++ = TriSlope;       //写入触发方向 bit
    *p++ = WaveScroll;     //写入波形滚动标志 bit
    *p = OLED_Brightness;  //写入OLED亮度 uint8

    //	printf("Lsb=%hu\r\n",Lsb);
    //	printf("DrawMode=%X\r\n",PlotMode);
    //	printf("ScaleH=%bd\r\n",ScaleH);
    //	printf("ScaleV_Auto=%X\r\n",ScaleV_Auto);
    //	printf("RulerVMax=%hd\r\n",RulerVMax);
    //	printf("RulerVMin=%hd\r\n",RulerVMin);
    //	printf("TriLevel=%hd\r\n",TriLevel);
    //	printf("TriMode=%bd\r\n",TriMode);
    //	printf("TriSlope=%X\r\n",TriSlope);
    //	printf("WaveScroll=%X\r\n",WaveScroll);
    //	printf("OLED_Brightness=%bu\r\n",OLED_Brightness);
    //	printf("\r\n");
    return EEPROM_Save(ops, sizeof(ops) / sizeof(ops[0]));
}

void Read_Options()
{
    uint8 ops[15];
    uint8 *p;
    p = ops;
    EEPROM_Read(ops, sizeof(ops) / sizeof(ops[0]));
    Lsb = *p++;           //读取Lsb高8位 uint16
    Lsb <<= 8;            //将低8位移到高八位
    Lsb |= *p++;          //读取Lsb低8位
    PlotMode = *p++;      //读取绘图模式 bit
    ScaleH = *p++;        //读取时间区间 char
    ScaleV_Auto = *p++;   //读取自动量程标志 bit
    RulerVMax = *p++;     //读取纵轴电压最大值高8位 int16
    RulerVMax <<= 8;      //将低8位移到高八位
    RulerVMax |= *p++;    //读取纵轴电压最大值低8位
    RulerVMin = *p++;     //读取纵轴电压最小高8位 int16
    RulerVMin <<= 8;      //将低8位移到高八位
    RulerVMin |= *p++;    //读取纵轴电压最小低8位
    TriLevel = *p++;      //读取触发值高8位 int16
    TriLevel <<= 8;       //将低8位移到高八位
    TriLevel |= *p++;     //读取触发值低8位
    TriMode = *p++;       //读取触发方式 int8
    TriSlope = *p++;      //读取触发方向 bit
    WaveScroll = *p++;    //读取波形滚动标志 bit
    OLED_Brightness = *p; //读取OLED亮度 uint8

    check_Options(); //检查选项合法性
    //	printf("Lsb=%hu\r\n",Lsb);
    //	printf("DrawMode=%X\r\n",PlotMode);
    //	printf("ScaleH=%bd\r\n",ScaleH);
    //	printf("ScaleV_Auto=%X\r\n",ScaleV_Auto);
    //	printf("RulerVMax=%hd\r\n",RulerVMax);
    //	printf("RulerVMin=%hd\r\n",RulerVMin);
    //	printf("TriLevel=%hd\r\n",TriLevel);
    //	printf("TriMode=%bd\r\n",TriMode);
    //	printf("TriSlope=%X\r\n",TriSlope);
    //	printf("WaveScroll=%X\r\n",WaveScroll);
    //	printf("OLED_Brightness=%bu\r\n",OLED_Brightness);
    //	printf("\r\n");
}
