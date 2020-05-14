/*
    adc_stc8.c

    Sampling with ADC built in MCU
    
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

#include "adc_stc8.h"

#ifdef STC8

/* scale_h: Time scale 500ms, 200ms, 100ms, 50ms, 20ms, 10ms, 5ms, 2ms, 1ms, 500us, 200us, 100us */
void ADCInit(uint8 scale_h)
{
    uint8 ADC_SPEED;

    switch (scale_h)
    {
    case 0: //500ms
        ADC_SPEED = ADC_SPEED_512;
        break;

    case 1: //200ms
        ADC_SPEED = ADC_SPEED_512;
        break;

    case 2: //100ms
        ADC_SPEED = ADC_SPEED_512;
        break;

    case 3: //50ms
        ADC_SPEED = ADC_SPEED_512;
        break;

    case 4: //20ms
        ADC_SPEED = ADC_SPEED_512;
        break;

    case 5: //10ms
        ADC_SPEED = ADC_SPEED_512;
        break;

    case 6: //5ms
        ADC_SPEED = ADC_SPEED_512;
        break;

    case 7: //2ms
        ADC_SPEED = ADC_SPEED_512;
        break;

    case 8: //1ms
        ADC_SPEED = ADC_SPEED_352;
        break;

    case 9: //500us
        ADC_SPEED = ADC_SPEED_192;
        break;

    case 10: //200us
        ADC_SPEED = ADC_SPEED_32;
        break;

    case 11: //100us
        ADC_SPEED = ADC_SPEED_32;
        break;
    }

    ADCCFG = RESFMT | ADC_SPEED; //结果右对齐，设置 ADC 时钟为系统时钟/2/16/16
    ADC_CONTR = ADC_POWER;       //使能 ADC 模块
    Delay5ms();                  //ADC上电并延时
}

uint16 ADCRead(uint8 chx)
{
    uint16 res;
    ADC_RES = 0;            //清除结果寄存器
    ADC_RESL = 0;           //清除结果寄存器
    ADC_CONTR &= 0xF0;      //选择ADC输入通道
    ADC_CONTR |= chx;       //选择ADC输入通道
    ADC_CONTR |= ADC_START; //启动 AD 转换
    _nop_();
    _nop_();

    while (!(ADC_CONTR & ADC_FLAG))
        ; //查询 ADC 完成标志

    ADC_CONTR &= ~ADC_FLAG;          //清完成标志
    res = (ADC_RES << 8) | ADC_RESL; //读取 ADC 结果
    return res;
}

uint16 *GetBGV(void)
{
    uint16 *BGV;
    BGV = (uint16 code *)BGV_ADR;
    return BGV;
}

uint16 GetADC_CHX(uint8 chx)
{
    uint16 ADCx;
    uint8 i;

    ADCInit(0); //ADC 初始化

    //通过ADC的chx通道读取Bandgap电压的ADC测量值
    ADCRead(chx); //前两个数据丢弃
    ADCRead(chx);
    ADCx = 0;

    for (i = 0; i < 16; i++)
    {
        ADCx += ADCRead(chx);
    }

    ADCx >>= 4; //取平均值
    return ADCx;
}

uint16 GetVoltage(uint8 chx, uint16 lsb)
{
    uint16 ADCbg;
    uint16 *BGV;
    uint16 ADCx;
    uint16 Vx;

    ADCInit(0); //ADC 初始化

    //通过ADC的第16通道读取Bandgap电压的ADC平均值
    ADCbg = GetADC_CHX(ADC_CHS_BG);

    //通过ADC的chx通道读取外部输入电压的ADC平均值
    ADCx = GetADC_CHX(chx);

    //通过公式计算外部输入的实际电压值
    BGV = GetBGV();
    Vx = (uint32)*BGV * ADCx * lsb / ADCbg / 100;
    return Vx;
}

//	ADCRead(chx) Timing:500ms, 200ms, 100ms, 50ms, 20ms, 10ms, 5ms, 2ms, 1ms, 500us, 200us, 100us
//	屏幕的横向共4格，每格代表一个时间范围，每格25个采样点
//  24MHz
//	ADC_SPEED_512 28us
//	ADC_SPEED_480 26.2us
//	ADC_SPEED_448 25us
//	ADC_SPEED_416 23.8us
//	ADC_SPEED_384 22us
//	ADC_SPEED_352 20us
//	ADC_SPEED_320 18us
//	ADC_SPEED_288 17us
//	ADC_SPEED_256 15us
//	ADC_SPEED_224 14us
//	ADC_SPEED_192 12us
//	ADC_SPEED_160 10.4us
//	ADC_SPEED_128 9us
//	ADC_SPEED_96 7us
//	ADC_SPEED_64 5.6us
//	ADC_SPEED_32 4us
//
//  27MHz
//	ADC_SPEED_512 26us
//	ADC_SPEED_352 19us
//	ADC_SPEED_192 11us
//	ADC_SPEED_32 4us
void switch_Dealy(uint8 scale_h)
{
    switch (scale_h)
    {
        //500ms ADC_SPEED_512
    case 0:
        Delay19971us();
        break;

        //200ms ADC_SPEED_512
    case 1:
        Delay7971us();
        break;

        //100ms	ADC_SPEED_512
    case 2:
        Delay3971us();
        break;

        //50ms ADC_SPEED_512
    case 3:
        Delay1971us();
        break;

        //20ms ADC_SPEED_512
    case 4:
        Delay771us();
        break;

        //10ms ADC_SPEED_512
    case 5:
        Delay371us();
        break;

        //5ms ADC_SPEED_512
    case 6:
        Delay171us();
        break;

        //2ms ADC_SPEED_512
    case 7:
        Delay51us();
        break;

        //1ms ADC_SPEED_352
    case 8:
        Delay18us();
        break;

        //500us	ADC_SPEED_192
    case 9:
        Delay6us();
        break;

        //200us ADC_SPEED_32
    case 10:
        /* 微调采样时间间隔
           fine tuning the delay */
        _nop_();
        //_nop_();
        break;

        //100us ADC_SPEED_32
    case 11:

        break;
    }
}

uint16 *GetWaveADC(uint8 chx, uint8 scale_h)
{
    uint8 i, j;
    static uint16 ADCSampling[SAMPLE_NUM];
    uint16 ADCPreSampling[PRE_BUF_NUM + 1]; //单次触发预缓存，有效采样点PRE_BUF_NUM个，多出的第一个位置用于复制最后一个位置的值，组成循环缓存

    ADCComplete = 0; //清零采样完成标志
    if (ADCInterrupt)
        return ADCSampling;
    memset(ADCSampling, 0x00, SAMPLE_NUM * 2);
    memset(ADCPreSampling, 0x00, (PRE_BUF_NUM + 1) * 2);
    //BGV = GetBGV(); //获取参考电压值
    ADCbg = GetADC_CHX(ADC_CHS_BG);                         //获取参考ADC值
    TriggerADC = Convert_mv_ADC(TriLevel, BGV, ADCbg, Lsb); //将用户设定的出发电压值转换为ADC值

    //初次打开内部A/D转换模拟电源，需适当延时，等内部模拟电源稳定后，再启动A/D转换
    ADCInit(scale_h);
    //通过ADC的第x通道读取外部输入电压的ADC测量值
    ADCRead(chx); //前两个数据丢弃
    ADCRead(chx);

    //时间间隔100us不支持单次触发，采样过程中不能有多余的代码否则达不到4us每次的速度，所以单独分立出来
    if (scale_h == 11) //100 us
    {
        P_Ready = 1; //开始采样，点亮指示灯
        for (i = 0; i < SAMPLE_NUM; i++)
        {
            if (ADCInterrupt)
                return ADCSampling;
            ADCSampling[i] = ADCRead(chx);
#ifdef DEBUG
            P15 = ~P15;
#endif
        }
        P_Ready = 0; //结束采样，熄灭指示灯
    }

    /* 单次触发或普通触发
       Single or Normal Trigger Mode */
    else if (TriMode)
    {
        P_Ready = 0;                       //预缓存，熄灭指示灯，完成预缓存前不要输入触发信号，会错过触发
        for (j = 1; j <= PRE_BUF_NUM; j++) //预缓存PRE_BUF_NUM个采样点，预缓存的第一个位置保留，用于存储和预缓存最后一个位置相同的值，组成环形缓存
        {
            if (ADCInterrupt)
                return ADCSampling;

            Delay3us();            //修正循环采样的时间间隔比连续采样慢3us
            switch_Dealy(scale_h); //采样延时
            ADCPreSampling[j] = ADCRead(chx);
#ifdef DEBUG
            P15 = ~P15;
#endif
        }
        P_Ready = 1; //预缓存结束，点亮指示灯，可输入触发信号

        //循环缓存触发前的PRE_BUF_NUM个采样点，并检测采样点是否满足触发条件
        while (1)
        {
            if (ADCInterrupt)
                return ADCSampling;

            //预缓存溢出则置1，并把最后一个采样值复制到第一个位置
            if (j > PRE_BUF_NUM)
            {
                j = 1;
                ADCPreSampling[0] = ADCPreSampling[PRE_BUF_NUM];
            }
            switch_Dealy(scale_h); //采样延时
            ADCPreSampling[j] = ADCRead(chx);
            if (GetTriggerPos(ADCPreSampling[j - 1], ADCPreSampling[j], TriggerADC, TriSlope)) //是否满足触发条件
            {
                P_Ready = 0; //触发成功，跳出while循环，熄灭指示灯
                break;
            }
            j++;
#ifdef DEBUG
            P15 = ~P15;
#endif
        }

        //继续采样AFT_BUF_NUM个采样点
        for (i = 0; i < AFT_BUF_NUM; i++)
        {
            if (ADCInterrupt)
                return ADCSampling;

            Delay3us();            //修正循环采样的时间间隔比连续采样慢3us
            switch_Dealy(scale_h); //采样延时
            ADCSampling[i + PRE_BUF_NUM] = ADCRead(chx);
#ifdef DEBUG
            P15 = ~P15;
#endif
        }

        //将前PRE_BUF_NUM个和后PRE_BUF_NUM个采样点组合成完整波形
        for (i = 0; i < PRE_BUF_NUM; i++) //预缓存中第一个和最后一个采样值相等，舍掉第一个值，将剩余PRE_BUF_NUM-1个采样点按采样顺序排序，作为ADCSampling的前PRE_BUF_NUM-1个采样点
        {
            if (ADCInterrupt)
                return ADCSampling;
            if (++j > PRE_BUF_NUM) //预缓存溢出，跳转到预缓存第二个位置
                j = 1;

            ADCSampling[i] = ADCPreSampling[j];
        }
    }

    /* 自动触发
       Auto Trigger Mode */
    else
    {
        P_Ready = 1; //开始采样，点亮指示灯
        for (i = 0; i < SAMPLE_NUM; i++)
        {
            if (ADCInterrupt)
                return ADCSampling;
            ADCSampling[i] = ADCRead(chx);
            Delay3us();            //循环采样的时间间隔比连续采样慢3us
            switch_Dealy(scale_h); //采样延时
#ifdef DEBUG
            P15 = ~P15;
#endif
        }
        P_Ready = 0; //采样结束，熄灭指示灯
    }
    ADCComplete = 1; //置位采样完成标志
    return ADCSampling;
}

#endif
