#ifndef DELAY_H
#define DELAY_H

#include <intrins.h>
#include "reg51.h"
#include "config_stc.h"

#ifndef uint8
#define uint8 unsigned char
#endif

#ifndef uint16
#define uint16 unsigned int
#endif	

#ifndef uint32
#define uint32 unsigned long int
#endif

bit Delay500ms_long_click(void);
bit Delay200ms_double_click(void);
void Delay500us(void);
void Delay5ms(void);
void Delay50ms(void);
void Delay200ms(void);
void Delay1000ms(void);
void Delay19972us(void);
void Delay7972us(void);
void Delay3972us(void);
void Delay1972us(void);
void Delay772us(void);
void Delay372us(void);
void Delay172us(void);
void Delay52us(void);
void Delay20us(void);
void Delay8us(void);
void Delay4us(void);

#endif