#include "delay.h"

void Delay500us() //@24.000MHz
{
    unsigned char i, j;

    i = 16;
    j = 147;
    do
    {
        while (--j)
            ;
    } while (--i);
}

void Delay5ms() //@24.000MHz
{
    unsigned char i, j;

    _nop_();
    _nop_();
    i = 156;
    j = 213;
    do
    {
        while (--j)
            ;
    } while (--i);
}

void Delay50ms() //@24.000MHz
{
    unsigned char i, j, k;

    _nop_();
    i = 7;
    j = 23;
    k = 105;
    do
    {
        do
        {
            while (--k)
                ;
        } while (--j);
    } while (--i);
}

bit Delay500ms_long_click() //@24.000MHz
{
    unsigned char i, j, k;
    _nop_();
    _nop_();
    i = 61;
    j = 225;
    k = 62;
    do
    {
        do
        {
            if (EC11_KEY)
                return 0;
            while (--k)
                ;
        } while (--j);
    } while (--i);
    return 1;
}

bit Delay200ms_double_click() //@24.000MHz
{
    unsigned char i, j, k;
    Delay50ms();
    _nop_();
    _nop_();
    i = 25;
    j = 90;
    k = 176;
    do
    {
        do
        {
            if (!EC11_KEY)
                return 1;
            while (--k)
                ;
        } while (--j);
    } while (--i);
	return 0;
}


void Delay1000ms() //@24.000MHz
{
    unsigned char i, j, k;

    _nop_();
    _nop_();
    i = 122;
    j = 193;
    k = 128;
    do
    {
        do
        {
            while (--k)
                ;
        } while (--j);
    } while (--i);
}

void Delay19972us() //@24.000MHz for scaleH 500ms with ADC_SPEED_512 20ms-28us
{
    unsigned char i, j, k;

    i = 3;
    j = 111;
    k = 124;
    do
    {
        do
        {
            while (--k)
                ;
        } while (--j);
    } while (--i);
}

void Delay7972us() //@24.000MHz for scaleH 200ms with ADC_SPEED_512 8ms-28us
{
    unsigned char i, j;

    _nop_();
    _nop_();
    i = 249;
    j = 119;
    do
    {
        while (--j)
            ;
    } while (--i);
}

void Delay3972us() //@24.000MHz for scaleH 100ms with ADC_SPEED_512 4ms-28us
{
    unsigned char i, j;

    i = 124;
    j = 203;
    do
    {
        while (--j)
            ;
    } while (--i);
}

void Delay1972us() //@24.000MHz for scaleH 50ms with ADC_SPEED_512 2ms-28us
{
    unsigned char i, j;

    _nop_();
    i = 62;
    j = 116;
    do
    {
        while (--j)
            ;
    } while (--i);
}

void Delay772us() //@24.000MHz for scaleH 20ms with ADC_SPEED_512 800us-28us
{
    unsigned char i, j;

    i = 25;
    j = 13;
    do
    {
        while (--j)
            ;
    } while (--i);
}

void Delay372us() //@24.000MHz for scaleH 10ms with ADC_SPEED_512 400us-28us
{
    unsigned char i, j;

    _nop_();
    _nop_();
    i = 12;
    j = 149;
    do
    {
        while (--j)
            ;
    } while (--i);
}

void Delay172us() //@24.000MHz for scaleH 5ms with ADC_SPEED_512 200us-28us
{
    unsigned char i, j;

    _nop_();
    _nop_();
    i = 6;
    j = 89;
    do
    {
        while (--j)
            ;
    } while (--i);
}

void Delay52us() //@24.000MHz for scaleH 2ms with ADC_SPEED_512 80us-28us
{
    unsigned char i, j;

    _nop_();
    i = 2;
    j = 156;
    do
    {
        while (--j)
            ;
    } while (--i);
}

void Delay20us() //@24.000MHz for scaleH 1ms with ADC_SPEED_352 40us-20us
{
    unsigned char i;

    i = 158;
    while (--i)
        ;
}

void Delay8us() //@24.000MHz for scaleH 500us with ADC_SPEED_192 20us-12us
{
    unsigned char i;

    i = 56; //微调62至56
    while (--i)
        ;
}

void Delay4us() //@24.000MHz for scaleH 200us with ADC_SPEED_32 8us-4us
{
    unsigned char i;

    i = 28; //微调30至28
    while (--i)
        ;
}
