/*
    Mini DSO with STC MCU

    GitHub: https://github.com/CreativeLau/Mini-DSO
    
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

     
    Version: V0.3
    MCU Type: STC8A8K64S4A12 @27MHz
    YouTube Tutorial(First Version): https://youtu.be/c6gBv6Jcz7w
    YouTube Tutorial(V0.3): https://youtu.be/-8PadlS7c4c
    Instructable(First Version): https://www.instructables.com/id/Make-Your-Own-OscilloscopeMini-DSO-With-STC-MCU-Ea/
    Instructable(V0.3): 
    Any question please contact CreativeLauLab@gmail.com
    YouTube Channel: https://www.youtube.com/c/CreativeLau

    Note:
    How to Operation:
        Main Interface - Parameter Mode:
            Single Click Encoder: Run/Stop sampling
            Double Click Encoder: Enter Wave Scroll Mode
            Long Press Encoder: Enter Settings Interface
            Rotate Encoder: Adjust parameters
            Rotate Encoder While Pressing: Switch between options
            
        Main Interface - Wave Scroll Mode: 
            Single Click Encoder: Run/Stop sampling
            Double Click Encoder: Enter Parameter Mode
            Long Press Encoder: Enter Settings Interface
            Rotate Encoder: Scroll waveform horizontally (only available when sampling stopped)
            Rotate Encoder While Pressing: Scroll waveform vertically (only available when sampling stopped)
            
        Settings Interface:
            Single Click Encoder: N/A
            Double Click Encoder: N/A
            Long Press Encoder: Return to Main Interface
            Rotate Encoder: Adjust parameters
            Rotate Encoder While Pressing: Switch between options

    Parameters in Main Interface:
        Seconds Per Division: "500ms", "200ms", "100ms", "50ms", "20ms", "10ms","5ms", "2ms", "1ms", "500us", "200us", "100us"(100us only available in Auto Trigger Mode)
        Voltage Range: Maximum voltage is 30V. Rotate Encoder clockwise continuous to enter auto range. Rotate Encoder anticlockwise to enter manual range. 
        Trigger Level: Set trigger voltage level. The trigger position will be put at the center of chart. 
                       You should set a correct Trigger Level in Single and Normal Trigger Mode in order to capture the waveform.
        Trigger Slope: Trigger on Rising or Falling Edge. 
                       You should set a correct Trigger Slop in Single and Normal Trigger Mode in order to capture the waveform.
        Trigger Mode:
            Auto:   Sweep continuous, indicator always on. Single click the encoder to stop or run sampling. 
                    If triggered, the waveform will be shown on the display and the trigger position will be put at the center of chart. 
                    Otherwise, the waveform will scroll irregular, and 'Fail' will be shown on the display.
            Normal: When complete pre-sampling, you can input signal. If triggered, waveform shown on the display and waiting for new trigger. 
                    If no new trigger, the waveform will be kept.
            Single: When complete pre-sampling, you can input signal. If triggered, waveform shown on display and stop sampling. 
                    User need to single click Encoder to start next sampling.
            For Normal Mode and Single Mode, be sure the trigger level has been adjusted correctly, otherwise no waveform will be shown on the display.

    Status in Main Interface:
        'Run': Sampling Running.
        'Stop': Sampling Stopped.
        'Fail': The Trigger Level beyond the waveform in Auto Trigger Mode.
        'Auto': Auto Voltage Range.
                    
    Parameters in Settings Interface:
        PMode(Plot Mode): Show waveform in Vector or Dots.
        LSB(Sampling Coefficient): 100 times of voltage dividing coefficient. e.g. the resistor for voltage dividing is 10k and 2k, calculate the voltage dividing coefficient (10+2)/2=6. Get the LSB=6x100=600.
        BRT(Brightness): Adjust OLED Brightness

    Saving Settings: When exit settings interface, all parameters in settings and main interface will be saved in EEPROM.

    Indicator:
    Generally, the indicator on means the sampling is running. 
    The more important use is in Single and Normal Trigger Mode, before get into the trigger stage, pre-sampling is required. 
    The indicator will not on during pre-sampling stage. We should not input signal until the indicator comes on. 
    The longer time scale selected, the longer waiting time of pre-sampling.

    2020-04-30 Update
    1. Add Single Sweep and Normal Sweep function. Set trigger value to catch the waveform mutation.
    2. According to datasheet, set ADC port to high-impedance.

    2020-05-3 Update
    1. Show frequency of waveform.
    2. Scroll waveform horizontally/vertically with EC11 Encoder.
    3. Zoom waveform vertically with EC11 Encoder.
    4. The trigger level could be customized.

    简易示波器
    版本：V0.3
    作者：老刘爱捣鼓
    单片机型：STC8A8K64S4A12 @27MHz
    任何问题请联系CreativeLauLab@gmail.com
    老刘爱捣鼓（全网同名）期待您的关注！
    B站教程（初版）：https://www.bilibili.com/video/BV1ai4y1t79R
    B站教程（V0.3）：https://www.bilibili.com/video/BV1ai4y1t79R
    油管频道: https://www.youtube.com/c/CreativeLau

    操作说明：
        主界面-选项设置模式：
            单击编码器：切换开始/暂停采样
            双击编码器：切换波形水平滚动模式/选项设置模式
            长按编码器：进入设置界面
            旋转编码器：修改当前参数
            按下同时旋转编码器：在参数间切换
            
        主界面-波形水平滚动模式：
            单击编码器：切换开始/暂停采样
            双击编码器：切换波形水平滚动模式/选项设置模式
            长按编码器：进入设置界面
            旋转编码器：水平滚动波形（在暂停采样状态下有效）
            按下同时旋转编码器：垂直滚动波形（仅在暂停采样状态下有效）
            
        设置界面：
            单击编码器：无效
            双击编码器：无效
            长按编码器：返回主界面
            旋转编码器：修改当前选项
            按下同时旋转编码器：切换选项

    主界面参数：
        横轴时间区间："500ms", "200ms", "100ms", "50ms", "20ms", "10ms","5ms", "2ms", "1ms", "500us", "200us", "100us"(100us仅在自动触发模式下可用)
        纵轴电压区间：设定电压量程上限，最大30V，顺时针旋转编码器将电压上限设置为0则进入自动量程，逆时针旋转进入手动量程
        触发值：设定触发电压值，触发位置被放在屏幕中心
        触发方向：上下箭头，分别代表上升沿触发和下降沿触发
        触发方式：
            Auto(自动触发)  ：连续采样，指示灯始终点亮，通过单击编码器控制启停；触发成功时，触发点位于屏幕中心，触发失败则波形无规律滚动，屏幕左侧显示Fail标志
            Normal(普通触发)：等待预采样，待指示灯亮起表示预采样结束，这时可以输入信号；
                              触发成功后，显示波形，并继续等待下次触发；
                              如果有新的触发成功，则自动显示新的波形，如没有新的触发成功则屏幕波形保持并一直等待下次触发；
                              需注意设置正确的触发值，否则一直处于等待触发状态，屏幕不会显示波形
            Single(单次触发)：等待预采样，待指示灯亮起表示预采样结束，这时可以输入信号；
                              触发成功后，显示波形，并停止采样，需用户单击编码器开始下次采样	
                              需注意设置正确的触发值，否则一直处于等待触发状态，屏幕不会显示波形
            
    主界面状态显示：	
        Run：正在采样
        Stop：停止采样
        Fail：自动模式下，触发值超出波形范围引起触发失败
        Auto：自动量程
            
    设置界面选项：
        PMode(绘图模式)：
            Vector：波形以矢量显示
            Dots  ：波形以点阵显示
        LSB(采样系数)：分压系数的100倍，比如用10k加2k的电阻进行分压，分压系数为(10+2/2)=6，LSB理论取值为6x100=600，根据实际电阻精度进行微调	
        BRT：OLED亮度

    保存设置：长按编码器退出设置界面时，设置界面和主界面的所有参数被保存到EEPROM

    2020-04-30更新
    1. 增加单次触发和普通触发功能，可自定义触发点用来捕捉波形变化
    2. 按照规格书要求将ADC采样端口设置为高阻输入
    2020-5-3更新
    1. 增加波形频率显示
    2. 使用编码器水平/垂直滚动波形
    3. 使用编码器垂直缩放波形
    4. 用户可设定触发电平
*/

#include <reg51.h>
#include "global.h"
#include "delay.h"
#include "adc_stc8.h"
#include "config_stc.h"
#include "settings.h"
#include "chart.h"
#include "ssd1306.h"

// #include "uart.h"
// #include "stdio.h"

#define VBAT_UPDATE_FREQ 40 //电池电压信息的更新频率 VBAT_UPDATE_FREQ*25ms
#define VBAT_LSB 600        //电池电压采样分压系数*100
#define INT_0 0             //外部中断0序号，编码器旋转
#define INT_1 2             //外部中断1序号，编码器按下
#define TIMER_0 1           //定时器0中断序号

void init()
{
    InSettings = 0;         //初始化开机界面
    OptionInSettings = 0;   //初始化设置界面的选项编号
    OptionInChart = 0;      //初始化主界面的选项标号
    WaveScroll = 0;         //清零波形滚动标志
    OptionChanged = 0;      //清零设置修改标志
    ADCRunning = 1;         //置位采样标志
    ADCInterrupt = 0;       //清零ADC中断标志
    ADCComplete = 0;        //清零ADC完成标志
    WaveUpdate = 0;         //清零波形更新标志
    ClearWave = 0;          //清零清除波形标志
    ClearDisplay = 1;       //置位清空屏幕标志
    EC11PressAndRotate = 0; //清零编码器旋转标志
    UpdateVbat = 0;         //清零采样电池电压标志
    ADCComplete = 0;        //清零ADC完成采样标志
    WaveFreq = 0;           //清零波形频率
    TriPos = 50;            //初始化触发点位置
    TriPosOffset = 0;       //初始化触发点偏移量
    TriFail = 0;            //清零触发失败标志
    VMax = 0;               //清零波形电压最大值
    VMin = 0;               //清零波形电压最小值
    WaveLengthSumNum = 0;   //清零波形长度求和次数
    WaveLengthSum = 0;      //清零波形长度求和

    /* 波形滚动模式下正在采样则禁用外部中断0（编码器旋转）
       Disable external interrupt 0(Encoder rotation) in waveform scroll mode when sampling*/
    if (ADCRunning && WaveScroll)
        EX0 = 0;

    /* 获取内部参考电压值
       Get internal reference voltage */
    BGV = GetBGV();
}

void runWhenInSettrings()
{
    P_Ready = 0;
    ClearDisplay = 1;
    while (InSettings)
    {
        /* 更新电池电压信息
           Update voltage infor of battery */
        if (UpdateVbat)
        {
            UpdateVbat = 0;
            VBat = GetVoltage(ADC_BAT, VBAT_LSB);
        }

        /* 更新屏幕亮度
           Update OLED Brightness */
        if (OLED_BrightnessChanged)
        {
            OLED_BrightnessChanged = 0;
            OLED_Write_Command(0x81);
            OLED_Write_Command(OLED_Brightness * 10);
        }

        /* 更新参数
           Update parameters */
        if (DisplayUpdate)
        {
            DisplayUpdate = 0;
            PlotSettings();
            OLED_Display();
        }
    }

    /* 保存设置
       Save Settings */
    EA = 0;
    if (OptionChanged)
    {
        OptionChanged = 0;
        ClearDisplay = 0;
        /* 保存设置并显示保存状态
           Save settings and display saving status*/
        PlotSaveStatus(Save_Options());
        OLED_Display();
        Delay1000ms();
    }

    /* 退出settings后刷新屏幕
       Redraw display after exit settings */
    ClearDisplay = 1;
    PlotChart();
    OLED_Display();
    IE0 = 0;
    IE1 = 0;
    EA = 1;
}

/*
    ADCRuning=1
    |	WaveUpdate=1
    |	|	ClearWave=1
    |	|	ClearWave=0
    |	WaveUpdate=0
    ADCRuning=0
        DisplayUpdate=1
            WaveUpdate=1
            |	ClearWave=1
            |	ClearWave=0
            WaveUpdate=0
*/
void runWhenADCInterrupt()
{
    ADCInterrupt = 0;
    P_Ready = 0;

    /* ADC采样运行
       ADC Sampling Running */
    if (ADCRunning)
    {
        EX0 = 0;

        if (WaveUpdate)
        {
            WaveUpdate = 0;
            ClearDisplay = 1;

            /* 清空波形缓存
               Clear waveform in buffer*/
            if (ClearWave)
            {
                ClearWave = 0;
                memset(ADCbuf, 0x00, SAMPLE_NUM * 2);
                WaveFreq = 0;
                TriFail = 0;
                VMax = 0;
                VMin = 0;
                PlotChart();
            }
            /* 显示缓存中的波形
               Display waveform in buffer*/
            else
            {
                AnalyseData();
                PlotChart();
                PlotWave();
            }
        }
        else
        {
            ClearDisplay = 0;
            PlotChart();
        }
        OLED_Display();

        /* ADCRuning和WaveScroll同时置位时，不开启外部中断0（编码器旋转）
           Not to enable external interrupt 0(Encoder Rotation) when both of ADCRunning and WaveScroll set. */
        if (!WaveScroll)
        {
            IE0 = 0;
            EX0 = 1;
        }
    }

    /* ADC采样停止
       ADC Sampling Stopped */
    else
    {
        while (!ADCRunning && !InSettings)
        {
            if (DisplayUpdate)
            {
                EX0 = 0;
                DisplayUpdate = 0;
                if (WaveUpdate)
                {
                    WaveUpdate = 0;
                    /* 分析缓存中完成采样的采样数据
                       Analyse completed sampling data in buffer */
                    AnalyseData();
                    ClearDisplay = 1;

                    /* 清空屏幕，绘制波形
                       Clear display and draw waveform */
                    if (ScaleH == ScaleH_tmp)
                    {
                        /* 采样点数有限，不支持横向缩放波形，只有时间区间与完成采样的时间区间相同才绘制波形
                           Since the sampling points limitation, scaling waveform along horizontal is not support.
                           Show waveform only when time scale is same as that when sampling completed */
                        PlotChart();
                        PlotWave();
                    }

                    /* 清空屏幕，不绘制波形
                       Clear display and no waveform */
                    else
                    {
                        WaveFreq = 0; //清零WaveFreq，绘制界面时，频率位置会显示****Hz
                        PlotChart();
                    }
                }
                /* 只更新屏幕上的参数，波形不变
                   Update parameters on display only, keep waveform */
                else
                {
                    ClearDisplay = 0;
                    PlotChart();
                }

                OLED_Display();
                IE0 = 0;
                EX0 = 1;
            }
        }
    }
}

void runWhenADCComplete()
{
    /*  ADC采样完成 - 单次触发
        ADC Sampling Complete - Single Trigger Mode */
    if (TriMode == 2)
    {
        EX0 = 0;
        ADCInterrupt = 1;
        DisplayUpdate = 0;
        WaveUpdate = 0;
        ADCRunning = 0; //清零ADC运行标志，停止采样
        AnalyseData();
        ClearDisplay = 1;
        PlotChart();
        PlotWave();
        OLED_Display();
        IE0 = 0;
        EX0 = 1;
    }

    /*  ADC采样完成 - 自动触发或普通触发
        ADC Sampling Complete - Auto or Normal Trigger Mode */
    else
    {
        //EX0 = 0;
        ClearDisplay = 1; //置位清空屏幕标志
        AnalyseData();    //分析采样数据
        PlotChart();      //绘制主界面
        PlotWave();       //绘制波形
        OLED_Display();
        //IE0 = 0;
        //EX0 = 1;
    }
}

/* 程序入口
   Program Entry */
void main()
{
    /* 将ADC端口P0.4，P0.6设置为高阻
       Set ADC port P0.4, P0.6 as high-impedance */
    P0M1 |= 0x50;
    P0M0 &= ~0x50;

    /* 设置指示灯端口P1.1为推挽输出
       Set indicator port P1.1 as push-pull output */
    P0M1 &= ~0x02;
    P0M0 |= 0x02;

    /* 设置编码器端口P3.2，P3.3，P3.4为准双向
       Set Encoder port P3.2, P3.3, P3.4 as quasi bidirectional */
    P3M1 &= ~0x1C;
    P3M0 &= ~0x1C;

    /* 设置OLED端口P2.3，P2.4，P2.5，P2.6，P2.7为准双向
       Set OLED port P2.3, P2.4, P2.5, P2.6 as quasi bidirectional */
    P2M1 &= ~0xF8;
    P2M0 &= ~0xF8;

    /* 编码器旋转外部中断 
       Interrupt for rotating of Encoder */
    IT0 = 0; //外部中断0触发方式，上升沿和下降沿
    PX0 = 1; //外部中断0高优先级，可打断按键中断，用于判断按下同时旋转
    EX0 = 1; //开启外部中断0

    /* 编码器按键外部中断 
       Interrupt for clicking of Encoder */
    IT1 = 1; //外部中断1触发方式，下降沿
    PX1 = 0; //外部中断1低优先级
    EX1 = 1; //开启外部中断1

    /* 定时器0，为更新电池电压信息计时
       Timer 0, for updating voltage of battery */
    AUXR &= 0x7F; //定时器时钟12T模式
    TMOD &= 0xF0; //设置定时器模式
    TL0 = 0x46;   //设置定时初值 //27MHz，25ms
    TH0 = 0x24;   //设置定时初值
    TF0 = 0;      //清除TF0标志
    ET0 = 1;      //允许定时器0中断

    /* 开总中断
       Enable global interrupt */
    EA = 1;

    //UartInit();

    Read_Options();
    init();
    OLED_Init();
    OLED_SetFontWidth(6);
    PlotChart();
    OLED_Display();

    while (1)
    {
        P_Ready = 0;

        /* 采样波形
           Sampling*/
        GetWaveData();

        /* 进入设置界面
           Enter Settings Interface */
        if (InSettings)
        {
            runWhenInSettrings();
        }

        /* ADC采样中断
           ADC Sampling Interrupt */
        else if (ADCInterrupt)
        {
            runWhenADCInterrupt();
        }

        /* ADC采样完成
           ADC Sampling Complete */
        else
        {
            runWhenADCComplete();
        }
    }
}

/* 分析编码器旋转
   Analyse Rotation of Encoder */
void Scan_EC11(void)
{
    /* 延时去抖动
       Delay to remove jitter */
    Delay500us();

    /* 正转
       Clockwise */
    if (EC11_A != EC11_B)
    {
        Change_Val(1);
    }

    /* 反转
       Anticlockwise */
    else if (EC11_A == EC11_B)
    {
        Change_Val(0);
    }
}

/* 编码器旋转中断
   Interrput for Encoder Rotated */
void INT0_interrupt(void) interrupt INT_0
{
    Delay500us();

    /* 编码器是否被按下
       Whether the Encoder is pressed */
    if (!EC11_KEY)
        EC11PressAndRotate = 1;
    else
        EC11PressAndRotate = 0;

    Scan_EC11();
    ADCInterrupt = 1;
    DisplayUpdate = 1;
    IE1 = 0; //清零外部中断1标志位
    IE0 = 0; //清零外部中断0标志位
}

/* 编码器点击中断
   Interrput for Encoder Pressed */
void INT1_interrupt(void) interrupt INT_1
{
    Delay50ms();
    if (!EC11_KEY)
    {
        EC11PressAndRotate = 0;

        /*  长按编码器按键 - 切换主界面和设置界面
            Long presse Encoder - Switch main interface and settings interface */
        if (Delay800ms_Long_Press())
        {
            InSettings = ~InSettings;

            /*  进入设置界面
                Enter Settings  */
            if (InSettings)
            {
                DisplayUpdate = 1;
                UpdateVbat = 1;
                TF0 = 0; //清零定时器0溢出标志
                TR0 = 1; //定时器0开始计时，开始电池电压信息更新计时
                IE0 = 0; //清零外部中断0中断标志
                EX0 = 1; //开启外部中断0（编码器旋转）
            }

            /*  回到主界面
                Retrurn to main interface   */
            else
            {
                TR0 = 0; //清零定时器0溢出标志
                TF0 = 0; //定时器0停止计时，停止电池电压信息更新计时
                WaveFreq = 0;
                TriFail = 0;
                VMax = 0;
                VMin = 0;
                DisplayUpdate = 1;
                WaveUpdate = 1;
                ClearWave = 0;
            }
            ADCInterrupt = 1;
        }

        /*  按住编码器同时旋转
            Rotate Encoder while pressing  */
        else if (EC11PressAndRotate)
        {
            /* 由编码器旋转中断执行操作
               Operations Performed by Interrupt of Encoder Rotation */
        }

        /* 双击编码器按键 - 在主界面，滚动波形和选项模式之间切换
           Double click Encoder - Switch between Waveform Scroll Mode and Parameter Mode in Main Interface */
        else if (Delay300ms_Double_Click())
        {
            /* 主界面
               Main Interface*/
            if (!InSettings)
            {
                WaveScroll = ~WaveScroll;
                OptionChanged = 1;
                ADCInterrupt = 1;
                DisplayUpdate = 1;
                ClearWave = 0;
            }
        }

        /*  单击编码器按键 - 在主界面，切换Stop/Run状态
            Single click Encoder - Switch Run/Stop in main interface    */
        else if (!InSettings)
        {
            EX0 = 0;
            ADCRunning = ~ADCRunning;
            if (ADCRunning)
            {
                WaveUpdate = 1;
                ClearWave = 1;
            }
            else
            {
                DisplayUpdate = 1;
                WaveUpdate = 1;
            }
            ADCInterrupt = 1;
            IE0 = 0;
            IE1 = 0;
        }
    }
    IE1 = 0;
}

/* 为更新Vbat计时
   Timer for updating Vbat */
void TIMER0_interrupt(void) interrupt TIMER_0
{
    static uint8 n;

    if (++n >= VBAT_UPDATE_FREQ)
    {
        n = 0;
        UpdateVbat = 1;
        DisplayUpdate = 1;
    }
}