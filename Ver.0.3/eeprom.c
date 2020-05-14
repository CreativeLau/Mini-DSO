/* EEPROM Functions. 
   Reference STC8 Series Datasheet */
#include "eeprom.h"

/*----------------------------
关闭IAP
----------------------------*/
void IapIdle()
{
    IAP_CONTR = 0;     //关闭IAP功能
    IAP_CMD = IAP_IDL; //清除命令寄存器
    IAP_TRIG = 0;      //清除触发寄存器
    IAP_ADDRH = 0x80;  //将地址设置到非IAP区域
    IAP_ADDRL = 0;
}
/*----------------------------
从ISP/IAP/EEPROM区域读取一字节
----------------------------*/
uint8 IapReadByte(uint16 addr)
{
    uint8 dat;                  //数据缓冲区
    IAP_CONTR = IAPEN | IAP_WT; //使能IAP
    IAP_CMD = IAP_READ;         //设置IAP命令
    IAP_ADDRL = addr;           //设置IAP低地址
    IAP_ADDRH = addr >> 8;      //设置IAP高地址
    IAP_TRIG = 0x5a;            //写触发命令(0x5a)
    IAP_TRIG = 0xa5;            //写触发命令(0xa5)
    _nop_();                    //等待ISP/IAP/EEPROM操作完成
    dat = IAP_DATA;             //读ISP/IAP/EEPROM数据
    IapIdle();                  //关闭IAP功能
    return dat;                 //返回
}

/*----------------------------
写一字节数据到ISP/IAP/EEPROM区域
----------------------------*/
void IapProgramByte(uint16 addr, uint8 dat)
{
    IAP_CONTR = IAPEN | IAP_WT; //使能IAP
    IAP_CMD = IAP_WRITE;        //设置IAP命令
    IAP_ADDRL = addr;           //设置IAP低地址
    IAP_ADDRH = addr >> 8;      //设置IAP高地址
    IAP_DATA = dat;             //写ISP/IAP/EEPROM数据
    IAP_TRIG = 0x5a;            //写触发命令(0x5a)
    IAP_TRIG = 0xa5;            //写触发命令(0xa5)
    _nop_();                    //等待ISP/IAP/EEPROM操作完成
    IapIdle();
}
/*----------------------------
扇区擦除
----------------------------*/
void IapEraseSector(uint16 addr)
{
    IAP_CONTR = IAPEN | IAP_WT; //使能IAP
    IAP_CMD = IAP_ERASE;        //设置IAP命令
    IAP_ADDRL = addr;           //设置IAP低地址
    IAP_ADDRH = addr >> 8;      //设置IAP高地址
    IAP_TRIG = 0x5a;            //写触发命令(0x5a)
    IAP_TRIG = 0xa5;            //写触发命令(0xa5)
    _nop_();                    //等待ISP/IAP/EEPROM操作完成
    IapIdle();
}

/* Write EEPROM
   s: Pointer to parameters
   para_num: Length of parameters */
bit EEPROM_Save(uint8 *s, uint8 para_num)
{
    int i; //不要改成uint8，误改了好几次！！
	//扇区擦除
    IapEraseSector(IAP_ADDRESS); 
	//检测是否擦除成功(全FF检测)
    for (i = 0; i < 512; i++)    
    {
        if (IapReadByte(IAP_ADDRESS + i) != 0xff)
            return 0; 
    }
	//编程EEPROM
    for (i = 0; i < para_num; i++) 
    {
        IapProgramByte(IAP_ADDRESS + i, *(s + i));
    }
	//校验EEPROM
    for (i = 0; i < para_num; i++) 
    {
        if (IapReadByte(IAP_ADDRESS + i) != *(s + i))
            return 0; 
    }

    return 1;
}

/* Read EEPROM
   s: Pointer to parameters
   para_num: Length of parameters */
void EEPROM_Read(uint8 *s, uint8 para_num)
{
    uint8 i;

    for (i = 0; i < para_num; i++) 
    {
        *s++ = IapReadByte(IAP_ADDRESS + i);
    }
}
