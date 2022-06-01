#ifndef _DEVICES_H
#define _DEVICES_H

#include "delay.h"
#include "system.h"
#include "system_info.h"
#include "lcd_1inch28.h"
#include "hdc1080.h"
#include "bmp280.h"
#include "stm32_ds3231.h"
#include "buzzer.h"
#include "w25q16.h"
#include "key.h"
#include "adc.h"
#include "GUI_Paint.h"
#include "i2c.h"
#include "tim.h"
#include "device_config.h"
#include "usart.h"

#define DEVICE_VER "1.0.0"

extern _RTC rtc;

void device_init(void);
void bmp280_read(float *press, float *temp);
void ds3231_read(void);
void ds3231_write(void);
void low_bat_detect(void);
void sys_timer_set(bool en);
bool sys_timer_state(void);
void lcd_backlight_set(uint8_t level);

#endif