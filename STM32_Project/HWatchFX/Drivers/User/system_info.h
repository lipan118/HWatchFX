#ifndef _SYSTEM_UTIL_H
#define _SYSTEM_UTIL_H

#include "main.h"

void calc_sys_temp(uint16_t adc);
void calc_sys_bat(uint16_t vref_adc, uint16_t adc);
float get_sys_temp();
float get_sys_bat();

#endif