﻿#ifndef UTILS_H_INCLUDED
#define UTILS_H_INCLUDED

#include "utils.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

static long MathMap(long x, long in_min, long in_max, long out_min, long out_max)
{
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

static int CompareFloat(const void *a, const void *b)
{
    float da = *(float *)a;
    float db = *(float *)b;
    return (da > db) ? 1 : -1;
}

static int CompareInt(const void *a,const void *b)
{
    return (*(int *)a-*(int *)b);       //升序
    //return (*(int *b)-*(int *)a); //降序 
}

static void QSortFloat(float * arr, int len)
{
    qsort(arr, len, sizeof(arr[0]), CompareFloat);
}

static void QSortInt(uint32_t * arr, int len)
{
    qsort(arr, len, sizeof(arr[0]), CompareInt);
}

static char *itoa( int value, char *string, int radix )
{
    int     i, d;
    int     flag = 0;
    char    *ptr = string;
 
    /* This implementation only works for decimal numbers. */
    if (radix != 10)
    {
        *ptr = 0;
        return string;
    }
 
    if (!value)
    {
        *ptr++ = 0x30;
        *ptr = 0;
        return string;
    }
 
    /* if this is a negative value insert the minus sign. */
    if (value < 0)
    {
        *ptr++ = '-';
 
        /* Make the value positive. */
        value *= -1;
        
    }
 
    for (i = 10000; i > 0; i /= 10)
    {
        d = value / i;
 
        if (d || flag)
        {
            *ptr++ = (char)(d + 0x30);
            value -= (d * i);
            flag = 1;
        }
    }
 
    /* Null terminate the string. */
    *ptr = 0;
 
    return string;
 
} /* NCL_Itoa */

static void USART_printf ( USART_TypeDef * USARTx, char * Data, ... )
{
    const char *s;
    int d;   
    char buf[16];
 
    
    va_list ap;
    va_start(ap, Data);
 
    while ( * Data != 0 )     // 判断是否到达字符串结束符
    {                                          
        if ( * Data == 0x5c )  //'\'
        {                                      
            switch ( *++Data )
            {
                case 'r':                                      //回车符
                USART_SendData(USARTx, 0x0d);
                Data ++;
                break;
 
                case 'n':                                      //换行符
                USART_SendData(USARTx, 0x0a);    
                Data ++;
                break;
 
                default:
                Data ++;
                break;
            }             
        }
        
        else if ( * Data == '%')
        {                                      //
            switch ( *++Data )
            {                
                case 's':                                          //字符串
                s = va_arg(ap, const char *);
                
                for ( ; *s; s++) 
                {
                    USART_SendData(USARTx,*s);
                    while( USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET );
                }
                
                Data++;
                
                break;
 
                case 'd':            
                    //十进制
                d = va_arg(ap, int);
                
                itoa(d, buf, 10);
                
                for (s = buf; *s; s++) 
                {
                    USART_SendData(USARTx,*s);
                    while( USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET );
                }
                
                Data++;
                
                break;
                
                default:
                Data++;
                
                break;
                
            }         
        }
        
        else USART_SendData(USARTx, *Data++);
        
        while ( USART_GetFlagStatus ( USARTx, USART_FLAG_TXE ) == RESET );
        
    }
}

#endif // UTILS_H_INCLUDED
