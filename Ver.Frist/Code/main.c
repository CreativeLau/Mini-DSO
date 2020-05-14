/*
简易示波器
日期：2020-04-22
作者：老刘爱捣鼓
参考：STC8系列规格书
说明：请勿用于商业用途，由于本代码造成的任何损失与作者无关。
任何问题请联系CreativeLauLab@gmail.com
老刘爱捣鼓（全网同名）期待您的关注！
B站视频教程：https://www.bilibili.com/video/BV1ai4y1t79R
头条视频教程：https://www.ixigua.com/i6818625118101570060

Mini DSO
Date: 2020-04-22
Author: Creative Lau
Reference: STC8 Series Datasheet
Description: Do not put this code in commercial use. Author is not responsible for any loss caused by this code.
Any question please contact CreativeLauLab@gmail.com
Youtube Channel: https://www.youtube.com/channel/UCdmdxlUnprbFKZszKpb48Vg
*/
#include <reg51.h>
#include "adc_stc8.h"
#include "config_stc.h"
#include "settings.h"
#include "chart.h"
#define VBAT_UPDATE_FREQ 40 //电池电压信息的更新频率 VBAT_UPDATE_FREQ*25ms
#define VBAT_LSB 6			//电池电压采样分压系数
#define INT_0 0 //外部中断0序号，编码器旋转
#define INT_1 2 //外部中断1序号，编码器按下
#define TIMER_0 1 //定时器0中断序号

bit _saveSettings = 0;
uint8 n; //为定时器0中断计数
void main()
{
    //******编码器旋转外部中断*******//
    IT0 = 0; //外部中断0触发方式，上升沿和下降沿
    PX0 = 1; //外部中断0高优先级
    EX0 = 1; //开启外部中断0

    //******编码器按键外部中断*******//
    IT1 = 1; //外部中断1触发方式，下降沿
    PX1 = 1; //外部中断1高优先级
    EX1 = 1; //开启外部中断1

	//******定时器0，为更新电池电压信息计时*******//
	AUXR &= 0x7F;	//定时器时钟12T模式
	TMOD &= 0xF0;	//设置定时器模式
	TL0 = 0xB0;		//设置定时初值 //24MHz，25ms
	TH0 = 0x3C;		//设置定时初值
	TF0 = 0;		//清除TF0标志
	ET0=1;			//允许定时器0中断
	
    EA = 1; //开总中断

    OLED_Init();          //初始化OLED
    OLED_SetFontWidth(6); //设置ASCII字体宽度
                          //OLED_Clear();
    Read_Options();       //从EEPROM读取设置参数

    ClearDisplay = 1; //清空屏幕
    PlotChart();
    OLED_Display();
    while (1)
    {
        ClearDisplay = 1;   //清空屏幕
        while (In_Settings) //在settings界面
        {
            if (UpdateVbat)
            {
                VBat = GetVoltage(ADC_BAT,VBAT_LSB);
                UpdateVbat = 0;
            }
            if (DisplayUpdate)
            {
                PlotSettings(); //绘制设置界面
                OLED_Display();
                DisplayUpdate = 0;
            }
        }

        if (_saveSettings) //保存设置
        {
			if(OptionChanged)
			{
				_saveSettings = 0;
				OptionChanged = 0;
				PlotSaveStatus(Save_Options());
				OLED_Display();
				Delay1000ms();

				In_Settings = 0; //防止在保存设置后延时1s过程中按键触发回到设置界面或者处于Hold状态
				HoldDisplay = 0;
			}
			_saveSettings=0; 
        }

        GetWaveData();   //采样ADC
		AnalyseData();		//分析采样数据
        WaitADC = 0;     //采样结束后，清零WaitADC标志
        if (Stop_Aquire) //中断采样
        {
            Stop_Aquire = 0;
            while (HoldDisplay)
            {
                //进入Hold状态
                ClearDisplay = 0; 	//不清空屏幕
                PlotChart();		//显示Hold图标
                OLED_Display();
            }
			WaitADC = 1; 		//显示Wait标志
			TriggerFail=0;      //置零TriggerFail标志
            ClearDisplay = 1;	//清空屏幕
            PlotChart();		//绘制主界面
            OLED_Display();
        }
        else
        {
            PlotChart();   	//绘制主界面
            PlotWave();		//绘制波形
            OLED_Display(); 
            Delay50ms();
        }
    }
}

void Scan_EC11(void) //编码器旋转响应函数
{
    //    EX0 = 0;                                                    //关闭外部中断0
    Delay500us();                                                     //延时去信号抖动
    if ((EC11_A == 0 && EC11_B == 1) || (EC11_A == 1 && EC11_B == 0)) //正转增加数值
    {
        Change_Val(1);
    }
    else if ((EC11_A == 0 && EC11_B == 0) || (EC11_A == 1 && EC11_B == 1)) //反转减小数值
    {
        Change_Val(0);
    }
    IE0 = 0; //清除由于信号抖动触发的外部中断1标志
    //    EX0 = 1; //重新开启外部中断0
}

/*
旋转编码器旋转中断，
在主界面（非Hold状态）调节时间横轴和电压纵轴数值
在主界面（Hold状态）在时间横轴和电压纵轴之间切换
在设置界面，调节LSB和DrawMode选项
*/
void INT0_interrupt(void) interrupt INT_0 
{
    Scan_EC11();       //分析EC11转动方向
    Stop_Aquire = 1;   //通知ADC停止采样
    DisplayUpdate = 1; //更新屏幕信息
    TriggerFail = 0;
    if (!HoldDisplay) //不在Hold状态下，置位设置修改标志位
    {
        OptionChanged = 1;
    }
}

//旋转编码器点击中断
void INT1_interrupt(void) interrupt INT_1 
{
    bit k = 0;
    Delay50ms();
    if (!EC11_KEY)
    {
		//长按编码器按键
		//切换主界面和设置界面
        if (Delay500ms_long_click()) 
        {
            In_Settings = ~In_Settings; //切换主界面和设置界面
            if (HoldDisplay)	   //在Hold模式中
                HoldDisplay = 0;   //退出Hold状态
            if (In_Settings)       //进入设置界面
			{
				TF0 = 0;			//清除TF0标志
				TR0 = 1;			//定时器0开始计时
                UpdateVbat = 1;    	//更新电池电压信息
			}
            if (!In_Settings)      //退出设置界面
			{
				TR0 = 0;		//定时器0停止计时
				TF0 = 0;		//清除TF0标志
                _saveSettings = 1; //置位保存标志
			}
            Stop_Aquire = 1;       //通知ADC停止采样
            DisplayUpdate = 1;     //通知屏幕需更新
            ClearDisplay = 1;      //清空屏幕
        }
		//双击编码器按键
		//切换触发方式上升或下降
        else if (Delay200ms_double_click()) 
        {
            if (!HoldDisplay && !In_Settings) //不在hold状态和设置界面
            {
                k = 1;           //给中断函数结束处的判断按键是否抬起增加附加条件，让长按绕过按键抬起判断，可以及时刷新LCD，让双击动作需要判断按键抬起，避免双击后面触发一个短按
                TriD = ~TriD;    //改变触发条件
                Stop_Aquire = 1; //通知ADC停止采样
            }
        }
		//单击编码器按键
		//在主界面切换Hold状态
		//在设置界面切换设置选项
        else 
        {
            if (In_Settings) //在设置界面
            {
                Option_In_Settings++; //切换设置选项
                if (Option_In_Settings > 1)
                    Option_In_Settings = 0;
                DisplayUpdate = 1; //通知屏幕需更新
            }
            else //在主界面
            {
                HoldDisplay = ~HoldDisplay; //切换Hold状态
                //ClearDisplay = 1;
                Stop_Aquire = 1; //通知ADC停止采样
            }
        }
    }
    //   Delay50ms();
    while (!EC11_KEY && k) //等待双击后编码器按键抬起
        ;
}

void TIMER0_interrupt(void) interrupt TIMER_0 
{
	if(++n>=VBAT_UPDATE_FREQ)
	{
		n=0;
		UpdateVbat=1;
		DisplayUpdate=1;
	}	
}