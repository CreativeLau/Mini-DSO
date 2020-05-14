#include "chart.h"
#include "bmp.h"
//#include "stdio.h"
#include "config_stc.h"

uint16 *WaveData;
uint16 bgADC;
uint16 VBat;
uint16 dacMax, dacMin;
uint16 triP;       //触发位置
bit TriD = 1;      //触发方向，1：上升触发，0：下降触发
uint16 vMax, vMin; //曲线上的最大和最小电压mV，上限65535mV
uint16 plotADCMax, plotADCMin;
uint16 rulerVMax, rulerVMin;
bit HoldDisplay = 0;   //停止更新图表标志位
bit DisplayUpdate = 0; //更新屏幕信息标志位
bit OptionChanged = 0; //设置被修改标志位
bit ClearDisplay = 0;  //清空屏幕
bit WaitADC = 1;       //等待ADC采样标志位
bit UpdateVbat = 0;    //更新电池电压信息标志位
bit TriggerFail=0;     //触发失败标志
void GetWaveData()
{
    WaveData = GetADC(ADC_DSO, Scale_H);
    bgADC = GetBGADC();
}

uint8 *convertVol(uint16 v)
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

void PlotChart(void)
{
    uint8 i;
    uint8 *s;

    if (ClearDisplay)
    {
        OLED_Clear();
        ClearDisplay = 0;

        //绘制图表实线边框
        OLED_DrawHLine(26, 0, 4); //方框内纵向留52格,1~52
		//OLED_DrawHLine(23, 26, 3);
        OLED_DrawHLine(26, 53, 4);
        
        OLED_DrawHLine(123, 0, 4);
        OLED_DrawHLine(123, 53, 4);
        OLED_DrawVLine(25, 0, 54); //方框内横向留101格,26~126
        OLED_DrawVLine(127, 0, 54);

        //绘制图表虚线网格
        for (i = 0; i < 17; i++)
        {
            OLED_DrawHLine(28 + 6 * i, 26, 2);
        }
        for (i = 0; i < 9; i++)
        {
            OLED_DrawVLine(26 + 25, 2 + i * 6, 2);
            OLED_DrawVLine(26 + 50, 2 + i * 6, 2);
            OLED_DrawVLine(26 + 75, 2 + i * 6, 2);
        }

        //绘制触发方式标志
        if (TriD)
        {
            OLED_DrawChar(18, 56, 123); //123上箭头，上升沿触发
        }
        else
        {
            OLED_DrawChar(18, 56, 124); //124下箭头，下降沿触发
        }
		
        //绘制波形电压范围
        OLED_Set_Pos(26, 56);
        s = convertVol(vMin);
        OLED_DrawString(s);
        OLED_DrawString("-");
        s = convertVol(vMax);
        OLED_DrawString(s);
        OLED_DrawString("V");
		
		//绘制触发失败标志
		if (TriggerFail)
		{
			OLED_Set_Pos(0, 16);
			OLED_DrawString("Tri.");
			OLED_Set_Pos(0, 24);
			OLED_DrawString("Fail");
		}
    }

	//绘制自动量程标志
	if(HoldDisplay)
	{
		OLED_Overlap(1);
		OLED_Set_Pos(8, 56);
		OLED_DrawString(" ");
		OLED_Overlap(0);	
	}
	if(Scale_V==0)
	{
		OLED_Set_Pos(8, 56);
		OLED_DrawString("A");
	}

	
	//绘制横轴时间区间
	if(HoldDisplay)
	{
		OLED_Overlap(1);
		OLED_Set_Pos(97, 56);
		OLED_DrawString("     ");
		OLED_Overlap(0);	
	}
	if(Option_In_Chart==1)
	{
		OLED_Reverse(1);
	}
	OLED_Set_Pos(97, 56);
	OLED_DrawString(ScaleHTxt[Scale_H]);
	OLED_Reverse(0);
	
	//绘制纵轴电压区间
	if(HoldDisplay)
	{
		OLED_Overlap(1);
		OLED_Set_Pos(0, 0);
		OLED_DrawString("    ");
		OLED_Overlap(0);
	}		
	if(Option_In_Chart==0)
	{
		OLED_Reverse(1);
	}
	s = convertVol(rulerVMax);
	OLED_Set_Pos(0, 0);
	OLED_DrawString(s);
	OLED_Reverse(0);
	
	s = convertVol(rulerVMin);
	OLED_Set_Pos(0, 45);
	OLED_DrawString(s);
		
    //绘制等待标志
    if (WaitADC)
    {
        OLED_Set_Pos(0, 8);
        OLED_DrawString("Wait");
    }
    else
    {
        OLED_Overlap(0); //设置绘图模式为覆盖
        OLED_Set_Pos(0, 8);
        OLED_DrawString("    ");
        OLED_Overlap(1); //恢复绘图模式为叠加
    }
    //绘制Hold标志
    if (HoldDisplay)
    {
        OLED_Set_Pos(0, 56);
        OLED_DrawString("H");
    }
    else
    {
        OLED_Overlap(0); //设置绘图模式为覆盖
        OLED_Set_Pos(0, 56);
        OLED_DrawString(" ");
        OLED_Overlap(1); //恢复绘图模式为叠加
    }
}

void PlotWave(void)
{
    uint8 i;
    if (Draw_Mode == 0) //Line Mode
    {
        for (i = 0; i < 100; i++)
        {
            OLED_DrawLine(i + 26, *(WaveData + triP - 50 + i), i + 27, *(WaveData + triP - 50 + i + 1));
        }
    }
    if (Draw_Mode == 1) //Dot Mode
    {
        for (i = 0; i <= 100; i++)
        {
            OLED_DrawPixel(i + 26, *(WaveData + triP - 50 + i));
        }
    }
}

void PlotSettings()
{
    if (ClearDisplay)
    {
        OLED_Clear();
        ClearDisplay = 0;

        //LOGO
        OLED_DrawBMP(24, 24, 24 + 82 - 1, 24 + 13 - 1, MINIDSO, sizeof(MINIDSO) / sizeof(MINIDSO[0]));

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

	//Vbat
	OLED_Set_Pos(64, 0);
	OLED_DrawString("Vbat=");
	OLED_Overlap(0); //设置绘图模式为覆盖
	OLED_DrawString(convertVol(VBat));
	OLED_Overlap(1); //恢复绘图模式为叠加
	OLED_DrawString("V");	
	
    //LSB
    OLED_Set_Pos(0, 0);
    OLED_DrawString("LSB=");
    OLED_Overlap(0); //设置绘图模式为覆盖
    OLED_DrawNum(Lsb, 3);
    OLED_Overlap(1); //恢复绘图模式为叠加

    //DrawMode
    OLED_Set_Pos(0, 12);
    OLED_DrawString("Mode=");
    OLED_Overlap(0); //设置绘图模式为覆盖
    OLED_DrawString(DrawModeTxt[Draw_Mode]);
    OLED_Overlap(1); //恢复绘图模式为叠加

    //选项标志
    if (Option_In_Settings == 0)
    {
        OLED_Overlap(0);           //设置绘图模式为覆盖
        OLED_DrawChar(44, 0, 125); //125左箭头
        OLED_DrawChar(56, 12, ' ');
        OLED_Overlap(1); //恢复绘图模式为叠加
    }
    else if (Option_In_Settings == 1)
    {
        OLED_Overlap(0); //设置绘图模式为覆盖
        OLED_DrawChar(44, 0, ' ');
        OLED_DrawChar(56, 12, 125); //125左箭头
        OLED_Overlap(1);            //恢复绘图模式为叠加
    }
}

//将图表值映射到屏幕的显示范围 图表值转换至1-52，同时将最大最小值转置
uint16 remap(uint16 val, uint16 rangeMax, uint16 rangeMin, uint16 rangeMaxNew, uint16 rangeMinNew)
{
	if(val>rangeMax)
		val=rangeMax;
	else if(val<rangeMin)
		val=rangeMin;
    val = rangeMinNew + (uint32)(rangeMax - val) * (rangeMaxNew - rangeMinNew) / (rangeMax - rangeMin);
	//printf("%hd\r\n",val);
    return val;
}

void AnalyseData()
{
    uint16 i;
    uint16 tmp;
    uint16 dacMid;
    dacMax = 0;
    dacMin = 4095;
    for (i = 0; i < SAMPLE_NUM; i++)
    {
        tmp = *(WaveData + i);
        if (tmp > dacMax)
            dacMax = tmp;
        else if (tmp < dacMin)
            dacMin = tmp;
    }

	GetBGV(); //获取BGV电压值
	
    //将采样点的最大最小ADC值转换成电压值mV
    vMax = (uint32)dacMax * *BGV * Lsb / bgADC / 100;
    vMin = (uint32)dacMin * *BGV * Lsb / bgADC / 100;

	if(Scale_V==0)
	{
		//根据采样点的最大最小值，按500mV扩大范围取整，作为垂直标尺范围mV
		if (vMax / 100 % 10 >= 5)
			rulerVMax = (vMax + 500) / 1000 * 1000;
		else
			rulerVMax = vMax / 1000 * 1000 + 500;
		if (vMin / 100 % 10 >= 5)
			rulerVMin = vMin / 1000 * 1000 + 500;
		else
			rulerVMin = vMin / 1000 * 1000;

		if (rulerVMax > MAX_V)
			rulerVMax = MAX_V;
		if (rulerVMin < MIN_V)
			rulerVMin = MIN_V;
	}
	else
	{
		//根据手动量程计算垂直标尺最大值mV
		rulerVMin=0;
		switch(Scale_V)
		{
			case 1:
				rulerVMax=200;
				break;
			case 2:
				rulerVMax=500;
				break;
			case 3:
				rulerVMax=1000;
				break;
			case 4:
				rulerVMax=2000;
				break;
			case 5:
				rulerVMax=5000;
				break;
			case 6:
				rulerVMax=10000;
				break;
			case 7:
				rulerVMax=15000;
				break;
			case 8:
				rulerVMax=20000;
				break;
			case 9:
				rulerVMax=25000;
				break;
			case 10:
				rulerVMax=30000;
				break;			
		}
	}
    //用垂直标尺mV范围反求出ADC值的范围作为图表的显示上下限
    plotADCMax = (uint32)rulerVMax * bgADC * 100 / (*BGV) / Lsb;
    plotADCMin = (uint32)rulerVMin * bgADC * 100 / (*BGV) / Lsb;

    //查询触发位置
    dacMid = (dacMax + dacMin) >> 1;
    triP = SAMPLE_NUM / 2;
	TriggerFail=1;
    for (i = 50; i <= SAMPLE_NUM-50; i++)
    {
        if (TriD) //上升触发
        {
            if (*(WaveData + i) <= dacMid && *(WaveData + i + 1) >= dacMid)
            {
                triP = i;
				TriggerFail=0;
                break;
            }
        }
        else //下降触发
        {
            if (*(WaveData + i) >= dacMid && *(WaveData + i + 1) <= dacMid)
            {
                triP = i;
				TriggerFail=0;
                break;
            }
        }
    }

    //重新映射采样点ADC值至图表的垂直范围
    for (i = 0; i < SAMPLE_NUM; i++)
    {
		//printf("%hd\r\n", *(WaveData + i));
        *(WaveData + i) = remap(*(WaveData + i), plotADCMax, plotADCMin, 52, 1);
    }
}

void PlotSaveStatus(bit _saveStatus)
{
    OLED_Set_Pos(62, 12);
    if (_saveStatus)
    {
        OLED_DrawString("Saved");
    }
    else
    {
        OLED_DrawString("Failed Save");
    }
}