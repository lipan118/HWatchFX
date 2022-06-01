#include "gui_main.h"

static lv_obj_t *main_root;
lv_obj_t* meter;
lv_obj_t* time_label;
lv_obj_t* date_label;
lv_obj_t* day_label;
lv_obj_t* arc_temp;
lv_obj_t* arc_humi;
lv_obj_t* arc_bat;
lv_obj_t* arc_temp1;
lv_obj_t* th_label;
lv_obj_t* sys_label;
lv_obj_t* press_label;
lv_obj_t* sound_label;
lv_obj_t* alarm_label;
lv_meter_indicator_t* indic;

lv_timer_t* lv_timer;

short screen_off_cnt = 0;

static void screen_off_detect()
{
	//息屏检测
	if(screen_off_cnt > 0) screen_off_cnt--;
	if(screen_off_cnt == 5)
	{
		//倒计时5秒,降低亮度
		lcd_backlight_set(1); 
	}
	else if(screen_off_cnt == 1)
	{
		//息屏
		lcd_backlight_set(0);
	}
}

void timer_task(lv_timer_t *timer)
{
	static uint8_t low_bat_flag = 0;
	char str[32];
	
	//设置表盘刻度, 日期
	ds3231_read();
	sprintf(str, "%02d : %02d : %02d", rtc.Hour, rtc.Min, rtc.Sec);
	lv_label_set_text(time_label, str);
	sprintf(str, "%04d/%02d/%02d", rtc.Year + 2000, rtc.Month, rtc.Date);
	lv_label_set_text(date_label, str);
	lv_label_set_text(day_label, DS3231_weeks[rtc.DaysOfWeek-1]);
	lv_meter_set_indicator_end_value(meter, indic, rtc.Sec);
	
	//息屏检测
	screen_off_detect();
	
	//环境传感器数据更新
	if(rtc.Sec % 2 == 0)
	{
		//设置温湿度
		float temp1, temp2, hum, press;
		HDC1080_ReadTempHumidity(&temp1, &hum);
		lv_arc_set_value(arc_humi, hum);
		lv_arc_set_value(arc_temp, 100-temp1);
		sprintf(str, "#0000ff %.1f%%# #ff0000 %.1f'C#", hum, temp1);
		lv_label_set_text(th_label, str);
		
		//设置电池电压, BMP280温度
		bmp280_read(&press, &temp2);
		lv_arc_set_value(arc_temp1, 100-temp2);
		
		float bat = get_sys_bat();
		lv_arc_set_value(arc_bat, bat * 10);
		sprintf(str, "#ff0000 %.1f'C# #0000ff %.2fV#", temp2, bat);
		lv_label_set_text(sys_label, str);
		
		//设置大气压
		sprintf(str, "#33ccff %.1f hPa#", press/100.0f);
		lv_label_set_text(press_label, str);
		
		//上传环境信息,取平均温度
		comm_env_set((temp1 + temp2) * 0.5f, hum, press);
	}
}

static void event_key_handler(lv_event_t *e)
{
	switch(e->code)
	{
		case LV_EVENT_KEY:
		{
			const uint32_t key = lv_indev_get_key(lv_indev_get_act());
			if(key == LV_KEY_ENTER)
			{
				if(page_call_handler && screen_off_cnt > 0) //非息屏状态
				{
					lv_timer_pause(lv_timer);
					page_call_handler((void*)PAGE_APPS);
				}
				
			}
			else if(key == LV_KEY_BACKSPACE)
			{
				//重置息屏检测变量
				screen_off_cnt = g_config.hwatch_config.home_screen_lock_t;
				lcd_backlight_set(g_config.hwatch_config.blight_level);
			}
		}
			break;
	}
}

void gui_main_init(lv_obj_t* root)
{
	main_root = root;
	init_font_style(&font_32, lv_palette_lighten(LV_PALETTE_RED, 1), &lv_font_montserrat_32);
	init_font_style(&font_22, lv_palette_lighten(LV_PALETTE_RED, 1), &lv_font_montserrat_22);
	init_font_style(&font_14, lv_palette_lighten(LV_PALETTE_RED, 1), &lv_font_montserrat_14); 
	init_font_style(&font_icon_16, lv_palette_lighten(LV_PALETTE_RED, 1), &hwatch_icons_font_16); 
	
	lv_group_add_obj(lv_group_get_default(), main_root);
	lv_obj_add_event_cb(main_root, event_key_handler, LV_EVENT_KEY, NULL);
	
	//创建表盘刻度
	meter = lv_meter_create(main_root);
	
	lv_obj_remove_style(meter, NULL, LV_PART_MAIN);
	lv_obj_remove_style(meter, NULL, LV_PART_INDICATOR);
	lv_obj_set_size(meter, GUI_WIDTH - 4, GUI_HEIGHT - 4);
	lv_obj_center(meter);

	lv_meter_scale_t* scale_sec = lv_meter_add_scale(meter);
	lv_meter_set_scale_ticks(meter, scale_sec, 61, 4, 6, lv_palette_main(LV_PALETTE_GREY));
	lv_meter_set_scale_range(meter, scale_sec, 0, 60, 360, 270);

	indic = lv_meter_add_scale_lines(meter, scale_sec, lv_palette_main(LV_PALETTE_PINK), lv_palette_main(LV_PALETTE_PINK), false, 0);
	lv_meter_set_indicator_start_value(meter, indic, 0);
	lv_meter_set_indicator_end_value(meter, indic, 0);
	
	//创建温湿度弧线
	static lv_style_t style_arc, style_arc1, style_arc_bg;
	lv_style_init(&style_arc);
	lv_style_init(&style_arc1);
	lv_style_init(&style_arc_bg);
	lv_style_set_arc_width(&style_arc, 6);
	lv_style_set_arc_width(&style_arc1, 6);
	lv_style_set_arc_width(&style_arc_bg, 6);
	lv_style_set_arc_color(&style_arc, lv_palette_main(LV_PALETTE_BLUE));
	lv_style_set_arc_color(&style_arc1, lv_palette_main(LV_PALETTE_RED));
	lv_style_set_arc_opa(&style_arc_bg, LV_OPA_0);
	lv_style_set_text_font(&style_arc_bg, &lv_font_montserrat_14);
	lv_style_set_text_color(&style_arc_bg, lv_palette_main(LV_PALETTE_CYAN));

	arc_humi = lv_arc_create(main_root);
	lv_obj_set_size(arc_humi, 220, 220);
	lv_obj_add_style(arc_humi, &style_arc_bg, LV_PART_MAIN);
	lv_obj_add_style(arc_humi, &style_arc, LV_PART_INDICATOR);
	lv_arc_set_rotation(arc_humi, 120);
	lv_arc_set_bg_angles(arc_humi, 0, 60);
	lv_obj_remove_style(arc_humi, NULL, LV_PART_KNOB);   /*Be sure the knob is not displayed*/
	lv_obj_clear_flag(arc_humi, LV_OBJ_FLAG_CLICKABLE);  /*To not allow adjusting by click*/
	lv_obj_center(arc_humi);
	lv_arc_set_value(arc_humi, 0);

	arc_temp = lv_arc_create(main_root);
	lv_obj_set_size(arc_temp, 220, 220);
	lv_obj_add_style(arc_temp, &style_arc_bg, LV_PART_MAIN);
	lv_obj_add_style(arc_temp, &style_arc1, LV_PART_INDICATOR);
	lv_arc_set_mode(arc_temp, LV_ARC_MODE_REVERSE);
	lv_arc_set_rotation(arc_temp, 360);
	lv_arc_set_bg_angles(arc_temp, 0, 60);
	lv_obj_remove_style(arc_temp, NULL, LV_PART_KNOB);   /*Be sure the knob is not displayed*/
	lv_obj_clear_flag(arc_temp, LV_OBJ_FLAG_CLICKABLE);  /*To not allow adjusting by click*/
	lv_obj_center(arc_temp);
	lv_arc_set_value(arc_temp, 100);
	
	arc_temp1 = lv_arc_create(main_root);
	lv_obj_set_size(arc_temp1, 220, 220);
	lv_obj_add_style(arc_temp1, &style_arc_bg, LV_PART_MAIN);
	lv_obj_add_style(arc_temp1, &style_arc1, LV_PART_INDICATOR);
	lv_arc_set_mode(arc_temp1, LV_ARC_MODE_REVERSE);
	lv_arc_set_rotation(arc_temp1, 180);
	lv_arc_set_bg_angles(arc_temp1, 0, 60);
	lv_obj_remove_style(arc_temp1, NULL, LV_PART_KNOB);   /*Be sure the knob is not displayed*/
	lv_obj_clear_flag(arc_temp1, LV_OBJ_FLAG_CLICKABLE);  /*To not allow adjusting by click*/
	lv_obj_center(arc_temp1);
	lv_arc_set_value(arc_temp1, 100);
	
	arc_bat = lv_arc_create(main_root);
	lv_obj_set_size(arc_bat, 220, 220);
	lv_obj_add_style(arc_bat, &style_arc_bg, LV_PART_MAIN);
	lv_obj_add_style(arc_bat, &style_arc, LV_PART_INDICATOR);
	lv_arc_set_range(arc_bat, 30, 43); //3.3 ~ 4.3
	lv_arc_set_rotation(arc_bat, 300);
	lv_arc_set_bg_angles(arc_bat, 0, 60);
	lv_obj_remove_style(arc_bat, NULL, LV_PART_KNOB);   /*Be sure the knob is not displayed*/
	lv_obj_clear_flag(arc_bat, LV_OBJ_FLAG_CLICKABLE);  /*To not allow adjusting by click*/
	lv_obj_center(arc_bat);
	lv_arc_set_value(arc_bat, 30);
	
	//创建时间日期标签
	time_label = lv_label_create(main_root);
	lv_obj_set_size(time_label, 180, LV_SIZE_CONTENT);
	lv_obj_align(time_label, LV_ALIGN_CENTER, 0, 0);
	lv_obj_set_style_text_align(time_label, LV_TEXT_ALIGN_CENTER, 0);
	lv_obj_add_style(time_label, &font_32, 0);
	lv_label_set_text(time_label, "00 : 00 : 00");
	
	date_label = lv_label_create(main_root);
	lv_obj_set_size(date_label, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
	lv_obj_align_to(date_label, time_label, LV_ALIGN_OUT_BOTTOM_LEFT, 5, 0);
	lv_obj_add_style(date_label, &font_14, 0);
	lv_label_set_text(date_label, "2022/04/30");
	
	day_label = lv_label_create(main_root);
	lv_obj_set_size(day_label, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
	lv_obj_align_to(day_label, time_label, LV_ALIGN_OUT_BOTTOM_RIGHT, -10, 0);
	lv_obj_add_style(day_label, &font_14, 0);
	lv_label_set_text(day_label, "SUN.");
	
	//电池电压, MCU内部温度标签
	sys_label = lv_label_create(main_root);
	lv_obj_set_size(sys_label, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
	lv_label_set_recolor(sys_label, true); 
	lv_obj_align(sys_label, LV_ALIGN_TOP_MID, 0, 15);
	lv_obj_add_style(sys_label, &font_14, 0);
	lv_label_set_text(sys_label, "#ff0000 0.0'C# #0000ff 0.00V#");
	
	//温湿度标签
	th_label = lv_label_create(main_root);
	lv_obj_set_size(th_label, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
	lv_label_set_recolor(th_label, true); 
	lv_obj_align(th_label, LV_ALIGN_BOTTOM_MID, 0, -15);
	lv_obj_add_style(th_label, &font_14, 0);
	lv_label_set_text(th_label, "#0000ff 0.0%# #ff0000 0.0'C#");
	
	//大气压标签
	press_label = lv_label_create(main_root);
	lv_obj_set_size(press_label, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
	lv_label_set_recolor(press_label, true); 
	lv_obj_align(press_label, LV_ALIGN_BOTTOM_MID, 0, -45);
	lv_obj_add_style(press_label, &font_22, 0);
	lv_label_set_text(press_label, "#33ccff 0.0 hPa#");
	
	//LOGO
	lv_obj_t* logo_label = lv_label_create(main_root);
	lv_obj_set_size(logo_label, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
	lv_label_set_recolor(logo_label, true); 
	lv_obj_align(logo_label, LV_ALIGN_TOP_MID, 0, 65);
	lv_obj_add_style(logo_label, &font_22, 0);
	lv_label_set_text(logo_label, "#660000 HWatchFX#");
	
	//声音, 闹钟标签
	sound_label = lv_label_create(main_root);
	lv_obj_set_size(sound_label, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
	lv_obj_align_to(sound_label, logo_label, LV_ALIGN_OUT_TOP_LEFT, 5, -5);
	lv_obj_add_style(sound_label, &font_icon_16, 0);
	lv_label_set_text(sound_label, HWATCH_SYMBOL_SOUND_OFF);
	
	alarm_label = lv_label_create(main_root);
	lv_obj_set_size(alarm_label, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
	lv_obj_align_to(alarm_label, logo_label, LV_ALIGN_OUT_TOP_RIGHT, 10, -5);
	lv_obj_add_style(alarm_label, &font_icon_16, 0);
	lv_label_set_text(alarm_label, HWATCH_SYMBOL_ALARM_OFF);
	
	lv_timer = lv_timer_create(timer_task, 1000, NULL); //1s任务
}

void gui_main_focus()
{
	lv_group_focus_obj(main_root);
	
	screen_off_cnt = g_config.hwatch_config.home_screen_lock_t;
	
	//设置声音,闹钟图标状态
	if(g_config.hwatch_config.sound_state)
		lv_label_set_text(sound_label, HWATCH_SYMBOL_SOUND_ON);
	else
		lv_label_set_text(sound_label, HWATCH_SYMBOL_SOUND_OFF);
	
	if(g_config.hwatch_config.alarm_state)
		lv_label_set_text(alarm_label, HWATCH_SYMBOL_ALARM_ON);
	else
		lv_label_set_text(alarm_label, HWATCH_SYMBOL_ALARM_OFF);
	
	gui_delay(100); //延时开启任务
	lv_timer_reset(lv_timer);
	lv_timer_resume(lv_timer);
	
}

