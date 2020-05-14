/*
    chart.c

    Sampling Data Analyse and Plot Interface

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

    Note:
    2020-04-30更新
    1. 修正波形最大电压和最小电压值，之前显示的是所有采样点中的最大和最小值，修正后显示为屏幕范围内的最大值和最小值
    2. 触发位置从之前的固定波形中点改为用户自定义
    3. 增加计算波形频率函数
*/
#include "chart.h"
#include "bmp.h"
//#include "stdio.h"

uint16 *pADCSampling;       //指向ADC实时采样的指针
uint16 waveBuf[SAMPLE_NUM]; //经过计算整理后的波形数据
uint8 triPre;               //触发位置前方等值点
uint8 triAft;               //触发位置后方等值点
uint16 plotADCMax, plotADCMin;

void GetWaveData()
{
    pADCSampling = GetWaveADC(ADC_DSO, ScaleH);
}

/* 获得自动量程纵轴上下限
   Calculate voltage range for Auto Range*/
void getRulerV()
{
    //自动量程，根据采样点的最大最小值，按500mV扩大范围取整，作为垂直标尺范围mV
    if (ScaleV_Auto == 1)
    {
        if (VMax / 100 % 10 >= 5)
            RulerVMax = (VMax + 500) / 1000 * 1000;
        else
            RulerVMax = VMax / 1000 * 1000 + 500;

        if (VMin / 100 % 10 >= 5)
            RulerVMin = VMin / 1000 * 1000 + 500;
        else
            RulerVMin = VMin / 1000 * 1000;

        if (RulerVMax > MAX_V)
            RulerVMax = MAX_V;

        if (RulerVMin < MIN_V)
            RulerVMin = MIN_V;
    }
}

/* 计算波长
   Calculate wave length*/
bit getWaveLength(uint16 triLevel, bit right_or_left)
{
    int16 i;
    uint8 tri_pre = 255;
    uint8 tri_aft = 255;
    uint8 triPos_tmp;
    bit triSlope_tmp;
    bit triFail = 1;
    /* 查找距离屏幕中心最近的触发点
       Search the trigger poiont closest to the center of chart */
    if (right_or_left) //向右查找临时触发点 / Search right side
    {
        for (i = TriPos + TriPosOffset; i < SAMPLE_NUM - 2; i++)
        {
            if (GetTriggerPos(*(ADCbuf + i), *(ADCbuf + i + 1), triLevel, 1)) //按上升沿查找 / Search on rising edge
            {
                triPos_tmp = i;
                triSlope_tmp = 1;
                triFail = 0;
                break;
            }
            else if (GetTriggerPos(*(ADCbuf + i), *(ADCbuf + i + 1), triLevel, 0)) //按下降沿查找 / Search on falling edge
            {
                triPos_tmp = i;
                triSlope_tmp = 0;
                triFail = 0;
                break;
            }
        }
    }
    else //向左查找临时触发点 / Search left side
    {
        for (i = TriPos + TriPosOffset; i > 0; i--)
        {
            if (GetTriggerPos(*(ADCbuf + i), *(ADCbuf + i + 1), triLevel, 1)) //按上升沿查找 / Search on rising edge
            {
                triPos_tmp = i;
                triSlope_tmp = 1;
                triFail = 0;
                break;
            }
            else if (GetTriggerPos(*(ADCbuf + i), *(ADCbuf + i + 1), triLevel, 0)) //按下降沿查找 / Search on falling edge
            {
                triPos_tmp = i;
                triSlope_tmp = 0;
                triFail = 0;
                break;
            }
        }
    }

    if (!triFail)
    {
        for (i = triPos_tmp; i >= 0; i--) //查询触发位置左侧的等值点 / Search equal point left side
        {
            if (triSlope_tmp) //上升触发,查找下降点 / Trigger on rising edge, search on falling edge
            {
                if (*(ADCbuf + i) >= triLevel && *(ADCbuf + i + 1) <= triLevel)
                {
                    tri_pre = i;
                    break;
                }
            }
            else //下降触发，查找上升点 / Trigger on falling edge, search on rising edge
            {
                if (*(ADCbuf + i) <= triLevel && *(ADCbuf + i + 1) >= triLevel)
                {
                    tri_pre = i;
                    break;
                }
            }
        }

        for (i = triPos_tmp; i < SAMPLE_NUM - 1; i++) //查询触发位置右侧的等值点 / Search equal point right side
        {
            if (triSlope_tmp) //上升触发,查找下降点 / Trigger on rising edge, search on falling edge
            {
                if (*(ADCbuf + i) >= triLevel && *(ADCbuf + i + 1) <= triLevel)
                {
                    tri_aft = i;
                    break;
                }
            }
            else //下降触发，查找上升点 / Trigger on falling edge, search on rising edge
            {
                if (*(ADCbuf + i) <= triLevel && *(ADCbuf + i + 1) >= triLevel)
                {
                    tri_aft = i;
                    break;
                }
            }
        }
    }
    if (tri_pre == 255 || tri_aft == 255 || (tri_pre == tri_aft))
    {
        WaveLength = 0;
        return 0;
    }
    else
    {
        WaveLength = tri_aft - tri_pre;
        return 1;
    }
}

/* 计算波形频率
   Calculate the wave frequency*/
void getWaveFreq()
{
    uint16 m_num; //求和次数
    uint8 n_num;  //求平均值右移位数

    //ADC停止采样时，每次动作都会显示波形频率，因此不求平均值
    if (!ADCRunning)
    {
        WaveLengthSumNum = 0;
        WaveLengthSum = 0;
    }

    //自动模式下为了让频率不频繁变动，取平均值，大于等于100ms区间不取平均值，一个原因是100ms波形变化慢，所以频率跳动不频繁，
    //另一方面越长的时间区间取平均值的延迟越高，影响使用体验
    else if (TriMode == 0 && ScaleH > 2)
    {
        switch (ScaleH)
        {
        case 3: //50ms
            m_num = 2;
            n_num = 1;
            break;

        case 4: //20ms
            m_num = 4;
            n_num = 2;
            break;

        default: //<10ms
            m_num = 8;
            n_num = 3;
            break;
        }

        WaveLengthSum += WaveLength;

        //达到求和次数后
        if (++WaveLengthSumNum >= m_num)
        {
            WaveLength = WaveLengthSum >> n_num; //求波长平均值
            WaveLengthSumNum = 0;                //清零波长求和次数
            WaveLengthSum = 0;                   //清零波长总和
        }

        //没达到求和次数，则返回，WaveFreq保持不变
        else
            return;
    }

    switch (ScaleH)
    {
    case 0:                                      //500ms
        WaveFreq = (float)50 / WaveLength + 0.5; //WaveFreq=25000/(500*WaveLength);
        break;

    case 1:                          //200ms
        WaveFreq = 125 / WaveLength; //WaveFreq=25000/(200*WaveLength);
        break;

    case 2:                          //100ms
        WaveFreq = 250 / WaveLength; //WaveFreq=25000/(100*WaveLength);
        break;

    case 3:                          //50ms
        WaveFreq = 500 / WaveLength; //WaveFreq=25000/(50*WaveLength);
        break;

    case 4:                           //20ms
        WaveFreq = 1250 / WaveLength; //WaveFreq=25000/(20*WaveLength);
        break;

    case 5:                           //10ms
        WaveFreq = 2500 / WaveLength; //WaveFreq=25000/(10*WaveLength);
        break;

    case 6:                           //5ms
        WaveFreq = 5000 / WaveLength; //WaveFreq=25000/(5*WaveLength);
        break;

    case 7:                            //2ms
        WaveFreq = 12500 / WaveLength; //WaveFreq=25000/(2*WaveLength);
        break;

    case 8:                            //1ms
        WaveFreq = 25000 / WaveLength; //WaveFreq=25000/(1*WaveLength);
        break;

    case 9:                            //500us
        WaveFreq = 50000 / WaveLength; //WaveFreq=25000000/(500*WaveLength);
        break;

    case 10:                            //200us
        WaveFreq = 125000 / WaveLength; //WaveFreq=25000000/(200*WaveLength);
        break;

    case 11:                            //100us
        WaveFreq = 250000 / WaveLength; //WaveFreq=25000000/(100*WaveLength);
        break;
    }
}

/* 将mv转换为ADC
   Convert voltage in mV to ADC*/
uint16 Convert_mv_ADC(uint16 mv, uint16 *BGV, uint16 ADCbg, uint16 lsb)
{
    uint16 ADC;
    ADC = (uint32)mv * ADCbg * 100 / (*BGV) / lsb;
    return ADC;
}

/* 将mv转换为ADC
   Convert ADC to voltage in mV */
uint16 c_ADC_mv(uint16 ADC, uint16 *BGV, uint16 BGADC, uint16 lsb)
{
    uint16 mv;
    mv = (uint32)ADC * *BGV * lsb / BGADC / 100;
    return mv;
}

/* 将uint16格式的mV转化为字符型V
   Convert voltage in mV to string*/
uint8 *c_mv_V(uint16 v)
{
    static uint8 s[5];

    if (v < 10000)
    {
        s[0] = v / 1000 + '0';
        s[1] = '.';
        s[2] = v / 100 % 10 + '0';
        s[3] = v / 10 % 10 + '0';
        s[4] = '\0';
    }
    else
    {
        s[0] = v / 10000 + '0';
        s[1] = v / 1000 % 10 + '0';
        s[2] = '.';
        s[3] = v / 100 % 10 + '0';
        s[4] = '\0';
    }

    return s;
}

/* 转换波形频率为字符
   Convert frequency to string */
uint8 *c_WaveFreq_Str()
{
    static uint8 s[5];
    if (WaveFreq == 0)
    {
        s[0] = '*';
        s[1] = '*';
        s[2] = '*';
        s[3] = '*';
        s[4] = '\0';
    }
    else if (WaveFreq >= 10000000)
    {
        s[0] = WaveFreq / 10000000 + '0';
        s[2] = ((WaveFreq) / 1000000) % 10 + '0';
        s[3] = 'M';
        s[4] = '\0';
    }
    else if (WaveFreq >= 1000000)
    {
        s[0] = WaveFreq / 1000000 + '0';
        s[1] = '.';
        s[2] = ((WaveFreq) / 100000) % 10 + '0';
        s[3] = 'M';
        s[4] = '\0';
    }
    else if (WaveFreq >= 100000)
    {
        s[0] = WaveFreq / 100000 + '0';
        s[1] = (WaveFreq / 10000) % 10 + '0';
        s[2] = ((WaveFreq) / 1000) % 10 + '0';
        s[3] = 'k';
        s[4] = '\0';
    }
    else if (WaveFreq >= 10000)
    {
        s[0] = WaveFreq / 10000 + '0';
        s[1] = ((WaveFreq) / 1000) % 10 + '0';
        s[2] = 'k';
        s[3] = '\0';
    }
    else if (WaveFreq >= 1000)
    {
        s[0] = WaveFreq / 1000 + '0';
        s[1] = '.';
        if ((WaveFreq / 10) % 10 > 5)
            s[2] = ((WaveFreq) / 100) % 10 + '0';
        else
            s[2] = ((WaveFreq) / 100) % 10 + '0';
        s[3] = 'k';
        s[4] = '\0';
    }
    else if (WaveFreq >= 100)
    {
        s[0] = WaveFreq / 100 + '0';
        s[1] = (WaveFreq / 10) % 10 + '0';
        s[2] = (WaveFreq) % 10 + '0';
        s[3] = '\0';
    }
    else if (WaveFreq >= 10)
    {
        s[0] = WaveFreq / 10 + '0';
        s[1] = (WaveFreq) % 10 + '0';
        s[2] = '\0';
    }
    else if (WaveFreq >= 1)
    {
        s[0] = (WaveFreq) % 10 + '0';
        s[1] = '\0';
    }
    return s;
}

/* 将采样值的映射到屏幕的显示范围，并反转
   Remap sampling data to display range and inverse */
uint16 remap(uint16 val, uint16 rangeMax, uint16 rangeMin, uint16 rangeMaxNew, uint16 rangeMinNew)
{
    if (val > rangeMax)
        val = rangeMax;
    else if (val < rangeMin)
        val = rangeMin;

    val = rangeMinNew + (uint32)(rangeMax - val) * (rangeMaxNew - rangeMinNew) / (rangeMax - rangeMin);
    return val;
}

/* 获取触发点位置
   Get Trigger Position */
bit GetTriggerPos(uint16 d1, uint16 d2, uint16 dTrigger, bit triSlope)
{
    /*  上升沿触发
        Trigger on Rising Edge  */
    if (triSlope)
    {
        if (d1 <= dTrigger && d2 >= dTrigger)
        {
            return 1;
        }
    }

    /*  下降沿触发
        Trigger on Falling Edge */
    else
    {
        if (d1 >= dTrigger && d2 <= dTrigger)
        {
            return 1;
        }
    }

    return 0;
}

/* 分析采样数据
   Analyse sampling date */
void AnalyseData()
{
    int16 i;
    uint16 tmp;
    uint16 adcMax = 0;
    uint16 adcMin = 4095;
    uint16 adcMid = 0;
    uint16 plotADCMid = 0;
    if (ADCComplete)
    {
        ScaleH_tmp = ScaleH; //记录完成采样的时间区间，由于采样点的数量较少，因此不支持实时根据时间区间缩放波形，时间区间改变则清空波形
        //将采样点复制到另一个数组，避免采样中断造成数据混乱
        //若采样被中断，则使用缓存中旧采样点显示波形
        for (i = 0; i < SAMPLE_NUM; i++)
        {
            *(ADCbuf + i) = *(pADCSampling + i);
        }

        //计算触发点位置
        //ADC采样停止时，TriPos不变，所以不进行下列计算
        TriPos = SAMPLE_NUM / 2;
        TriFail = 1; //置位触发失败标志
        for (i = ((CHART_H_MAX - CHART_H_MIN) >> 1); i < SAMPLE_NUM - ((CHART_H_MAX - CHART_H_MIN) >> 1); i++)
        {
            if (GetTriggerPos(*(ADCbuf + i), *(ADCbuf + i + 1), TriggerADC, TriSlope))
            {
                TriPos = i;
                TriFail = 0; //清零触发失败标志
                break;
            }
        }
        TriPosOffset = 0;
    }

    /*  获取屏幕显示波形的最大和最小值
        Get the MAX and MIN value of waveform on display*/
    for (i = 0; i <= 100; i++)
    {
        tmp = *(ADCbuf + TriPos + TriPosOffset - 50 + i);
        if (tmp > adcMax)
            adcMax = tmp;
        else if (tmp < adcMin)
            adcMin = tmp;
    }

    //将采样点的最大最小采样值转换成电压值mV
    VMax = c_ADC_mv(adcMax, BGV, ADCbg, Lsb);
    VMin = c_ADC_mv(adcMin, BGV, ADCbg, Lsb);

    //获得垂直标尺的上下限
    getRulerV();

    //用垂直标尺mV范围反求出ADC值的范围作为图表的显示上下限
    plotADCMax = Convert_mv_ADC(RulerVMax, BGV, ADCbg, Lsb);
    plotADCMin = Convert_mv_ADC(RulerVMin, BGV, ADCbg, Lsb);

    //计算波形的频率
    //如果当前的时间区间和采样数据的时间间隔一致则进行频率计算
    //由于STC8A8K在高频采样时，ADC值经常莫名其妙地跳变到满量程4095，咨询STC工程师无果
    //为避免ADC采样出错时频率跳变厉害，计算波长时使用电压标尺的中点和波幅中点的较小值
    //如果遇到频率跳变无法读取，将自动量程切换至合适的手动量程即可
    adcMid = (adcMax + adcMin) >> 1;
    plotADCMid = (plotADCMax + plotADCMin) >> 1;
    if (getWaveLength(adcMid < plotADCMid ? adcMid : plotADCMid, 1) || getWaveLength(adcMid < plotADCMid ? adcMid : plotADCMid, 0))
    {
        getWaveFreq();
    }
    else
    {
        WaveFreq = 0;
    }

    //映射采样值至屏幕的显示范围
    for (i = 0; i < SAMPLE_NUM; i++)
    {
        waveBuf[i] = remap(*(ADCbuf + i), plotADCMax, plotADCMin, CHART_V_MAX, CHART_V_MIN);
    }
}

/* 绘制主界面
   Draw main interface */
void PlotChart(void)
{
    uint8 i;
    uint8 *s;

    if (ClearDisplay)
    {
        ClearDisplay = 0;
        OLED_Clear();

        /* 图表边框
           波形横向绘图区101格,26~126
           波形纵向绘图区45格,8~52
           Frame of chart 45x101*/
        OLED_DrawHLine(CHART_H_MIN, CHART_V_MIN, 4);
        OLED_DrawHLine(CHART_H_MIN, CHART_V_MAX, 4);
        OLED_DrawHLine(CHART_H_MAX - 3, CHART_V_MIN, 4);
        OLED_DrawHLine(CHART_H_MAX - 3, CHART_V_MAX, 4);
        OLED_DrawHLine(CHART_H_MIN + 25 - 2, CHART_V_MIN, 5);
        OLED_DrawHLine(CHART_H_MIN + 25 - 2, CHART_V_MAX, 5);
        OLED_DrawHLine(CHART_H_MIN + 50 - 2, CHART_V_MIN, 5);
        OLED_DrawHLine(CHART_H_MIN + 50 - 2, CHART_V_MAX, 5);
        OLED_DrawHLine(CHART_H_MIN + 75 - 2, CHART_V_MIN, 5);
        OLED_DrawHLine(CHART_H_MIN + 75 - 2, CHART_V_MAX, 5);
        OLED_DrawVLine(CHART_H_MIN - 1, CHART_V_MIN, CHART_V_MAX - CHART_V_MIN + 1);
        OLED_DrawVLine(CHART_H_MAX + 1, CHART_V_MIN, CHART_V_MAX - CHART_V_MIN + 1);

        /* 图表虚线网格
           Grid of chart */
        for (i = 0; i < 15; i++)
        {
            OLED_DrawHLine(CHART_H_MIN + 7 * i, CHART_V_MIN + ((CHART_V_MAX - CHART_V_MIN) >> 1), 3);
        }
        for (i = 0; i < 6; i++)
        {
            OLED_DrawVLine(CHART_H_MIN + 25, CHART_V_MIN + 1 + i * 8, 3);
            OLED_DrawVLine(CHART_H_MIN + 50, CHART_V_MIN + 1 + i * 8, 3);
            OLED_DrawVLine(CHART_H_MIN + 75, CHART_V_MIN + 1 + i * 8, 3);
        }

        /* 波形位置标尺
           Ruler for waveform position*/
        OLED_DrawHLine(0, 62, 25);
        OLED_DrawVLine(0, 60, 3);
        OLED_DrawVLine(24, 60, 3);
        OLED_DrawVLine((TriPos + TriPosOffset - 50) * 24 / 119, 58, 4);

        /* 波形电压范围
           Voltage range of waveform*/
        OLED_Set_Pos(26, 56);
        s = c_mv_V(VMin);
        OLED_DrawString(s);
        OLED_DrawString("-");
        s = c_mv_V(VMax);
        OLED_DrawString(s);
        OLED_DrawString("V");
    }

    OLED_Overlap(0); //设置绘图模式为覆盖

    /* 频率
       Frequency */
    OLED_Set_Pos(92, 0);
    OLED_DrawString("      ");
    OLED_Set_Pos(92, 0);
    s = c_WaveFreq_Str();
    OLED_DrawString(s);
    OLED_DrawString("Hz");

    /* 自动量程标志
       Flag for Auto Range*/
    if (ScaleV_Auto == 1)
    {
        OLED_Set_Pos(0, 0);
        OLED_DrawString("Auto");
    }
    else
    {
        OLED_Set_Pos(0, 0);
        OLED_DrawString("    ");
    }

    /* 触发值
       Trigger Level */
    OLED_Set_Pos(33, 0);
    if (OptionInChart == 2 && !WaveScroll)
    {
        OLED_DrawVLine(69, 0, 8);
        OLED_Reverse(1);
    }
    else
    {
        OLED_Reverse(1);
        OLED_DrawVLine(69, 0, 8);
        OLED_Reverse(0);
    }
    s = c_mv_V(TriLevel);
    OLED_DrawString("T");
    OLED_DrawString(s);
    OLED_DrawString("V");
    OLED_Reverse(0);

    /* 触发方向标志
       Trigger Slope */
    if (OptionInChart == 3 && !WaveScroll)
    {
        OLED_DrawVLine(71, 0, 8);
        OLED_DrawVLine(78, 0, 8);
        OLED_Reverse(1);
    }
    else
    {
        OLED_Reverse(1);
        OLED_DrawVLine(71, 0, 8);
        OLED_DrawVLine(78, 0, 8);
        OLED_Reverse(0);
    }
    if (TriSlope)
    {
        OLED_DrawChar(72, 0, 123); //123上箭头，上升沿触发
    }
    else
    {
        OLED_DrawChar(72, 0, 124); //124下箭头，下降沿触发
    }
    OLED_Reverse(0);

    /* 触发方式标志
       Trigger Mode */
    if (OptionInChart == 4 && !WaveScroll)
    {
        OLED_DrawVLine(86, 0, 8);
        OLED_Reverse(1);
    }
    else
    {
        OLED_Reverse(1);
        OLED_DrawVLine(86, 0, 8);
        OLED_Reverse(0);
    }
    OLED_Set_Pos(80, 0);
    if (TriMode == 0)
    {
        OLED_DrawString("A");
    }
    else if (TriMode == 1)
    {
        OLED_DrawString("N");
    }
    else if (TriMode == 2)
    {
        OLED_DrawString("S");
    }
    OLED_Reverse(0);

    /* 触发失败标志
       Flag for Trigger Fail*/
    if (TriFail)
    {
        OLED_Set_Pos(0, 24);
        OLED_DrawString("Fail");
    }

    //绘制运行/停止标志
    //    if (TriS && ADCRuning)
    //    {
    //        OLED_Set_Pos(0, 16);
    //        OLED_DrawString("Wait");
    //    }
    if (ADCRunning)
    {
        OLED_Set_Pos(0, 16);
        OLED_DrawString("Run ");
    }
    else
    {
        OLED_Set_Pos(0, 16);
        OLED_DrawString("Stop");
    }

    /* 横轴时间区间
       Seconds per division */
    OLED_Set_Pos(97, 56);
    OLED_DrawString("     ");
    if (OptionInChart == 0 && !WaveScroll)
    {
        OLED_Reverse(1);
    }
    OLED_Set_Pos(97, 56);
    OLED_DrawString(ScaleHTxt[ScaleH]);
    OLED_Reverse(0);

    /* 纵轴电压区间
       Ruler for Voltage */
    OLED_Set_Pos(0, 8);
    OLED_DrawString("    ");
    if (OptionInChart == 1 && !WaveScroll)
    {
        OLED_Reverse(1);
    }
    s = c_mv_V(RulerVMax);
    OLED_Set_Pos(0, 8);
    OLED_DrawString(s);
    OLED_Reverse(0);
    s = c_mv_V(RulerVMin);
    OLED_Set_Pos(0, 46);
    OLED_DrawString(s);

    OLED_Overlap(1); //恢复绘图模式为叠加
}

/* 绘制波形
   Draw waveform*/
void PlotWave(void)
{
    uint8 i;
    //Vector Mode
    if (PlotMode == 0)
    {
        for (i = 0; i < (CHART_H_MAX - CHART_H_MIN); i++)
        {
            OLED_DrawLine(
                i + CHART_H_MIN,
                waveBuf[TriPos + TriPosOffset - ((CHART_H_MAX - CHART_H_MIN) >> 1) + i],
                i + CHART_H_MIN + 1,
                waveBuf[TriPos + TriPosOffset - ((CHART_H_MAX - CHART_H_MIN) >> 1) + i + 1]);
        }
    }
    //Dots Mode
    if (PlotMode == 1)
    {
        for (i = 0; i <= (CHART_H_MAX - CHART_H_MIN); i++)
        {
            OLED_DrawPixel(i + CHART_H_MIN, waveBuf[TriPos + TriPosOffset - ((CHART_H_MAX - CHART_H_MIN) >> 1) + i]);
        }
    }
}

/* 绘制设置界面
   Draw settings */
void PlotSettings()
{
    if (ClearDisplay)
    {
        ClearDisplay = 0;
        OLED_Clear();

        //LOGO
        OLED_DrawBMP(18, 24, 18 + 82 - 1, 24 + 13 - 1, MINIDSO, sizeof(MINIDSO) / sizeof(MINIDSO[0]));

        //Version
        OLED_Set_Pos(102, 30);
        OLED_DrawString("V0.3");

        //CopyRight
        OLED_Set_Pos(30, 39);
        OLED_DrawString("By Creative Lau");
        OLED_Set_Pos(44, 48);
        OLED_DrwCHS_16x16(0);
        OLED_DrwCHS_16x16(1);
        OLED_DrwCHS_16x16(2);
        OLED_DrwCHS_16x16(3);
        OLED_DrwCHS_16x16(4);
    }

    OLED_Overlap(0);

    /* 选项
       Options */
    /* DrawMode */
    OLED_Set_Pos(0, 0);
    OLED_DrawString("PMode=");
    OLED_DrawString(PlotModeTxt[PlotMode]);

    /* LSB */
    OLED_Set_Pos(70, 0);
    OLED_DrawString("LSB=");
    OLED_DrawString("   ");
    OLED_Set_Pos(94, 0);
    OLED_DrawNum(Lsb, 3);

    /* OLED_Brightness */
    OLED_Set_Pos(70, 12);
    OLED_DrawString("BRT=");
    OLED_DrawString("   ");
    OLED_Set_Pos(94, 12);
    OLED_DrawNum(OLED_Brightness, 3);

    /* Vbat */
    OLED_Set_Pos(0, 12);
    OLED_DrawString("Vbat=");
    OLED_DrawString(c_mv_V(VBat));
    OLED_DrawString("V");

    /* 选项标志，125对应左箭头
       Option indicator, 125 is Left arrow */
    /* PlotMode */
    if (OptionInSettings == 0)
    {
        OLED_DrawChar(64, 0, 125);
        OLED_DrawChar(116, 0, ' ');
        OLED_DrawChar(116, 12, ' ');
    }
    /* LSB */
    else if (OptionInSettings == 1)
    {
        OLED_DrawChar(64, 0, ' ');
        OLED_DrawChar(116, 0, 125);
        OLED_DrawChar(116, 12, ' ');
    }
    /* OLED_Brightness */
    else if (OptionInSettings == 2)
    {
        OLED_DrawChar(64, 0, ' ');
        OLED_DrawChar(116, 0, ' ');
        OLED_DrawChar(116, 12, 125);
    }

    OLED_Overlap(1);
}

/* 绘制保存状态
   Draw saving status*/
void PlotSaveStatus(bit _saveStatus)
{
    OLED_Set_Pos(64, 12);
    OLED_Overlap(0); //设置绘图模式为覆盖
    if (_saveStatus)
    {
        OLED_DrawString("Save Succ.");
    }
    else
    {
        OLED_DrawString("Save Fail!");
    }
    OLED_Overlap(1); //设置绘图模式为覆盖
}