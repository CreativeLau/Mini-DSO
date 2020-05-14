#ifndef EEPROM_H
#define EEPROM_H

#include "intrins.h"
#include "delay.h"

#ifndef uint8
#define uint8 unsigned char
#endif

#ifndef uint16
#define uint16 unsigned int
#endif	

#ifndef uint32
#define uint32 unsigned long int
#endif

//-----------------------------------------------
//IAP/ISP 特殊功能寄存器
sfr IAP_DATA    =   0xc2;
sfr IAP_ADDRH   =   0xc3;
sfr IAP_ADDRL   =   0xc4;
sfr IAP_CMD     =   0xc5;
sfr IAP_TRIG    =   0xc6;
sfr IAP_CONTR   =   0xc7;
sfr ISP_DATA    =   0xc2;
sfr ISP_ADDRH   =   0xc3;
sfr ISP_ADDRL   =   0xc4;
sfr ISP_CMD     =   0xc5;
sfr ISP_TRIG    =   0xc6;
sfr ISP_CONTR   =   0xc7;

#define IAP_IDL     0x00 //空操作
#define IAP_READ    0x01 //读EEPROM
#define IAP_WRITE   0x02 //写EEPROM
#define IAP_ERASE   0x03 //擦除EEPROM
#define IAPEN       0x80 //使能EEPROM
#define SWBS        0x40 //软件复位选择控制位 0：软件复位后从用户代码开始执行程序 1：软件复位后从系统 ISP 监控代码区开始执行程序
#define SWRST       0x20 //软件复位控制位 0：无动作 1：产生软件复位
#define CMD_FAIL    0x10 //EEPROM操作失败状态位，需要软件清零 0： EEPROM 操作正确 1： EEPROM 操作失败

//EEPROM操作的等待时间
//#define IAP_WT 0x00 //if SYSCLK<30MHz
#define IAP_WT 0x01 //if SYSCLK<24MHz
//#define IAP_WT 0x02 //if SYSCLK<20MHz
//#define IAP_WT 0x03 //if SYSCLK<12MHz
//#define IAP_WT 0x04 //if SYSCLK<6MHz
//#define IAP_WT 0x05 //if SYSCLK<3MHz
//#define IAP_WT 0x06 //if SYSCLK<2MHz
//#define IAP_WT 0x07 //if SYSCLK<1MHz

//扇区首地址
#define IAP_ADDRESS 0x0000

bit EEPROM_Save(uint8 *s,char para_num);
void EEPROM_Read(uint8 *s, char para_num);


#endif

