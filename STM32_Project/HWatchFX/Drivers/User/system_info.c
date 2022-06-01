#include "system_info.h"

float sys_temp;
float sys_bat;

void calc_sys_temp(uint16_t adc)
{
	float tmp =(float)adc*(3.3f/4096);		//转换电压值
	sys_temp = (1.43f - tmp)/0.0043f+25.0f;		//转换温度值
}

//(10 + 1)K电阻分压检测
void calc_sys_bat(uint16_t vref_adc, uint16_t adc)
{
	sys_bat = (1.22f * adc / vref_adc) * 11.0f;
}

float get_sys_temp()
{
	return sys_temp;
}

float get_sys_bat()
{
	return sys_bat;
}