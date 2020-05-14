#ifndef DELAY_H
#define DELAY_H

#include "reg51.h"
#include <intrins.h>
#include "config_stc.h"
#include "global.h"

#ifndef uint8
#define uint8 unsigned char
#endif

#ifndef uint16
#define uint16 unsigned int
#endif

#ifndef uint32
#define uint32 unsigned long int
#endif

bit Delay800ms_Long_Press(void);
bit Delay300ms_Double_Click(void);
void Delay500us(void);
void Delay5ms(void);
void Delay50ms(void);
void Delay200ms(void);
void Delay1000ms(void);
void Delay3us(void);
void Delay19971us(void);
void Delay7971us(void);
void Delay3971us(void);
void Delay1971us(void);
void Delay771us(void);
void Delay371us(void);
void Delay171us(void);
void Delay51us(void);
void Delay18us(void);
void Delay6us(void);
//void Delay1us(void);

#endif