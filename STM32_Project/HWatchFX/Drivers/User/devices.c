#include "devices.h"

BMP280_HandleTypedef bmp280;
bool bme280p = false;

bool timer_flag = false;

_RTC rtc = {
    .Year = 22, .Month = 05, .Date = 04,
    .DaysOfWeek = TUESDAY,
    .Hour = 8, .Min = 35, .Sec = 22
};

const uint8_t flash_buf[]="www.wiyixiao4.com/blog";

void device_init()
{
	//====================Init LCD====================
	LCD_1IN28_Init(0);
	HAL_TIM_PWM_Start(&htim4,TIM_CHANNEL_3);
	LCD_1IN28_Clear(BLACK);
	//====================Init Flash====================
	W25QXX_Init(&hspi2);
	printf("Flash type: %x\n", W25QXX_ReadID());
	//====================Init Config====================
	HWatchDevice_Init();
	HWatchDevice_Printf();
	lcd_backlight_set(g_config.hwatch_config.blight_level); //设置背光
	//====================Init Buzzer====================
	buzzer_cfg.tim = &htim2;
	buzzer_cfg.tim_channel = TIM_CHANNEL_1;
	//====================Init HDC1080====================
	HDC1080_Init(&hi2c1);
	printf("HDC1080 Info: \nManufacturerID: %x, DeviceID: %x\n", HDC1080_ReadManufacturerId(), HDC1080_ReadDeviceId());
	//====================Init BMP280====================
	bmp280_init_default_params(&bmp280.params);
	bmp280.addr = BMP280_I2C_ADDRESS_0;
	bmp280.i2c = &hi2c1;
	while (!bmp280_init(&bmp280, &bmp280.params)) {
		printf("BMP280 initialization failed\n");
		HAL_Delay(2000);
	}
	bme280p = bmp280.id == BME280_CHIP_ID;
	printf("BMP280: found %s\n", bme280p ? "BME280" : "BMP280");
	//====================Init DS3231====================
	DS3231_Init(&hi2c2);
//  DS3231_SetTime(&rtc);
	DS3231_ClearAlarm1();
//	DS3231_SetAlarm1(ALARM_MODE_SEC_MATCHED, 0, 0, 0, 30);
}

void bmp280_read(float *press, float *temp)
{
	float humi = 0.0f;
	while (!bmp280_read_float(&bmp280, temp, press, &humi)) {
		printf("Temperature/pressure reading failed\n");
		HAL_Delay(2000);
	}
//	printf("Pressure: %.2f Pa, Temperature: %.2f C", *press, *temp);
	if (bme280p) {
		printf(", Humidity: %.2f\n", humi);
	}
	else {
//		printf("\n");
	}
}

void ds3231_read()
{
	DS3231_GetTime(&rtc);
//	printf("%d-%d-%d %d:%d:%d %d\n", rtc.Year, rtc.Month, rtc.Date, rtc.Hour, rtc.Min, rtc.Sec, rtc.DaysOfWeek);
}

void ds3231_write()
{
	DS3231_SetTime(&rtc);
}

void low_bat_detect()
{
	static uint8_t flag = 0;
	ADC1_Start();
	float bat = get_sys_bat();
	if(bat <= 3.0f)
	{
		if(!flag)
			buzzer_on_direct();
		else
			buzzer_off();
		
		flag = !flag;
	}
	else
	{
		buzzer_off();
		flag = 0;
	}
}

void sys_timer_set(bool en)
{
	if(en)
	{
		HAL_TIM_Base_Start_IT(&htim7);
		timer_flag = true;
	}
	else
	{
		HAL_TIM_Base_Stop_IT(&htim7);
		timer_flag = false;
	}
}

bool sys_timer_state()
{
	return timer_flag;
}

void lcd_backlight_set(uint8_t level)
{
	__HAL_TIM_SetCompare(&htim4, TIM_CHANNEL_3, level*100); //10ms//100~1000
}









