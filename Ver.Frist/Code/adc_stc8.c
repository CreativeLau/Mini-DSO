#include "adc_stc8.h"

#ifdef STC8
uint16 *BGV;
bit Stop_Aquire = 0;
static uint16 ADCBuf[SAMPLE_NUM];

void ADCInit(uint8 scale_h)
{
    uint8 ADC_SPEED;
    switch (scale_h)
    {
    case 0:
        ADC_SPEED = ADC_SPEED_512;
        break;
    case 1:
        ADC_SPEED = ADC_SPEED_512;
        break;
    case 2:
        ADC_SPEED = ADC_SPEED_512;
        break;
    case 3:
        ADC_SPEED = ADC_SPEED_512;
        break;
    case 4:
        ADC_SPEED = ADC_SPEED_512;
        break;
    case 5:
        ADC_SPEED = ADC_SPEED_512;
        break;
    case 6:
        ADC_SPEED = ADC_SPEED_512;
        break;
    case 7:
        ADC_SPEED = ADC_SPEED_512;
        break;
    case 8:
        ADC_SPEED = ADC_SPEED_352;
        break;
    case 9:
        ADC_SPEED = ADC_SPEED_192;
        break;
    case 10:
        ADC_SPEED = ADC_SPEED_32;
        break;
    case 11:
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
        ;                            //查询 ADC 完成标志
    ADC_CONTR &= ~ADC_FLAG;          //清完成标志
    res = (ADC_RES << 8) | ADC_RESL; //读取 ADC 结果
    return res;
}

void GetBGV(void)
{
	BGV = (uint16 code *)BGV_ADR;
}

uint16 GetVoltage(uint8 chx, uint16 lsb)
{
    uint16 ADCbg;
    uint16 ADCx;
    uint16 Vx;
    uint8 i;
    
    ADCInit(0); //ADC 初始化

    //通过ADC的第16通道读取Bandgap电压的ADC测量值
    ADCRead(ADC_CHS_BG);
    ADCRead(ADC_CHS_BG); //前两个数据丢弃
    ADCbg = 0;
    for (i = 0; i < 16; i++)
    {
        ADCbg += ADCRead(ADC_CHS_BG); 
    }
    ADCbg >>= 4; //取平均值

    //通过ADC的chx通道读取外部输入电压的ADC测量值
    ADCRead(chx);
    ADCRead(chx); //前两个数据丢弃
    ADCx = 0;
    for (i = 0; i < 16; i++)
    {
        ADCx += ADCRead(chx); 
    }
    ADCx >>= 4; //取平均值

    //通过公式计算外部输入的实际电压值
	GetBGV();
    Vx = (uint32)*BGV * ADCx * lsb / ADCbg;
    return Vx;
}

uint16 GetBGADC()
{
    uint16 ADCbg;
    uint8 i;
    //BGV = (uint16 code *)BGV_ADR;
    ADCInit(0); //ADC 初始化

    //通过ADC的第16通道读取Bandgap电压的ADC测量值
    ADCRead(ADC_CHS_BG);
    ADCRead(ADC_CHS_BG); //前两个数据丢弃
    ADCbg = 0;
    for (i = 0; i < 16; i++)
    {
        ADCbg += ADCRead(ADC_CHS_BG); //读取 8 次数据
    }
    ADCbg >>= 4; //取平均值
    return ADCbg;
}

//	ADCRead(chx) Timing:500ms, 200ms, 100ms, 50ms, 20ms, 10ms, 5ms, 2ms, 1ms, 500us, 200us, 100us
//	屏幕的横向共4格，每格代表一个时间范围，每格25个采样点
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
uint16 *GetADC(uint8 chx, uint8 scale_h)
{
    uint8 i;
	
    //初次打开内部A/D转换模拟电源，需适当延时，等内部模拟电源稳定后，再启动A/D转换
    ADCInit(scale_h);
	
    //通过ADC的第x通道读取外部输入电压的ADC测量值
    ADCRead(chx);
    ADCRead(chx); //前两个数据丢弃

    switch (scale_h)
    {
    case 0: //500ms ADC_SPEED_512
        for (i = 0; i < SAMPLE_NUM; i++)
        {
            ADCBuf[i] = ADCRead(chx);
            Delay19972us();
            if (Stop_Aquire)
                break;
            //P15 = ~P15;
        }
        break;
    case 1: //200ms ADC_SPEED_512
        for (i = 0; i < SAMPLE_NUM; i++)
        {
            ADCBuf[i] = ADCRead(chx);
            Delay7972us();
            if (Stop_Aquire)
                break;
            //P15 = ~P15;
        }
        break;
    case 2: //100ms	ADC_SPEED_512
        for (i = 0; i < SAMPLE_NUM; i++)
        {
            ADCBuf[i] = ADCRead(chx);
            Delay3972us();
            if (Stop_Aquire)
                break;
            //P15 = ~P15;
        }
        break;
    case 3: //50ms ADC_SPEED_512
        for (i = 0; i < SAMPLE_NUM; i++)
        {
            ADCBuf[i] = ADCRead(chx);
            Delay1972us();
            if (Stop_Aquire)
                break;
            //P15 = ~P15;
        }
        break;
    case 4: //20ms ADC_SPEED_512
        for (i = 0; i < SAMPLE_NUM; i++)
        {
            ADCBuf[i] = ADCRead(chx);
            Delay772us();
            if (Stop_Aquire)
                break;
            //P15 = ~P15;
        }
        break;
    case 5: //10ms ADC_SPEED_512
        for (i = 0; i < SAMPLE_NUM; i++)
        {
            ADCBuf[i] = ADCRead(chx);
            Delay372us();
            //P15 = ~P15;
        }
        break;
    case 6: //5ms ADC_SPEED_512
        for (i = 0; i < SAMPLE_NUM; i++)
        {
            ADCBuf[i] = ADCRead(chx);
            Delay172us();
            //P15 = ~P15;
        }
        break;
    case 7: //2ms ADC_SPEED_512
        for (i = 0; i < SAMPLE_NUM; i++)
        {
            ADCBuf[i] = ADCRead(chx);
            Delay52us();
            //P15 = ~P15;
        }
        break;
    case 8: //1ms ADC_SPEED_352
        for (i = 0; i < SAMPLE_NUM; i++)
        {
            ADCBuf[i] = ADCRead(chx);
            Delay20us();
            //P15 = ~P15;
        }
        break;
    case 9: //500us	ADC_SPEED_192
        for (i = 0; i < SAMPLE_NUM; i++)
        {
            ADCBuf[i] = ADCRead(chx);
            Delay8us();
            //P15 = ~P15;
        }
        break;
    case 10: //200us ADC_SPEED_32
        for (i = 0; i < SAMPLE_NUM; i++)
        {
            ADCBuf[i] = ADCRead(chx);
            Delay4us();
            //P15 = ~P15;
        }
        break;
    case 11: //100us ADC_SPEED_32
        for (i = 0; i < SAMPLE_NUM; i++)
        {
            ADCBuf[i] = ADCRead(chx);
            //P15 = ~P15;
        }
        break;
    }
    return ADCBuf;
}

#endif
