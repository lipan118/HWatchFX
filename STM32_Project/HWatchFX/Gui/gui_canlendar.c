#include "gui_canlendar.h"

lv_obj_t *lv_calendar;
lv_obj_t *lv_year_list;
lv_obj_t *lv_month_list;
lv_obj_t *lv_month_matrix;
lv_obj_t *lv_date_label;

static void date_display(void);
static void date_init_task(lv_timer_t *timer);

static void date_init_task(lv_timer_t *timer)
{
	lv_calendar_date_t lv_today;
	
	ds3231_read();
	lv_today.year = rtc.Year + 2000;
	lv_today.month = rtc.Month;
	lv_today.day = rtc.Date;
	
	lv_calendar_set_today_date(lv_calendar, lv_today.year, lv_today.month, lv_today.day);
	lv_calendar_set_showed_date(lv_calendar, lv_today.year, lv_today.month);
	
	buzzer_on();
	gui_delay(200);
	buzzer_off();
	
	date_display();
}

static void date_init()
{
	gui_delay(200);
	lv_timer_t * timer = lv_timer_create_basic();
	lv_timer_set_cb(timer, date_init_task);
	lv_timer_set_repeat_count(timer, 1);
}

static void date_display()
{
	lv_calendar_date_t date;
	if(lv_calendar_get_pressed_date(lv_calendar, &date)) {
		char str[32];
		sprintf(str, "%04d/%02d/%02d", date.year, date.month, date.day);
		lv_label_set_text(lv_date_label, str);
		gui_delay(10);
	}
}

static void event_key_handler(lv_event_t *e)
{	
	static uint8_t ym_flag = 0;
	lv_event_code_t code = lv_event_get_code(e);
	lv_obj_t * obj = lv_event_get_target(e);
	const uint32_t key = lv_indev_get_key(lv_indev_get_act());
	
	if(obj == lv_calendar)
	{
		if(key == LV_KEY_ENTER)
		{
			date_init();
		}
		else if(key == LV_KEY_BACKSPACE)
		{
			page_call_handler((void*)PAGE_APPS);
		}
		else if(key == LV_KEY_UP)
		{

		}
		else if(key == LV_KEY_DOWN)
		{

		}
		else if(key == LV_KEY_LEFT)
		{
			lv_dropdown_open(lv_year_list);
			lv_group_focus_obj(lv_year_list);
		}
		else if(key == LV_KEY_RIGHT)
		{
			lv_dropdown_open(lv_month_list);
			lv_group_focus_obj(lv_month_list);
		}
	}
	else if(obj == lv_year_list || obj == lv_month_list)
	{
		if(key == LV_KEY_BACKSPACE)
		{
			lv_group_focus_obj(lv_month_matrix);
			date_display();
		}
	}
	else if(obj == lv_month_matrix)
	{
		if(key == LV_KEY_ENTER)
		{

		}
		else if(key == LV_KEY_BACKSPACE)
		{
			lv_group_focus_obj(lv_calendar);
		}
		else if(key == LV_KEY_LEFT || key == LV_KEY_RIGHT || key == LV_KEY_UP || key == LV_KEY_DOWN)
		{
			date_display();
		}
	}
	
}

void gui_canlendar_init(lv_obj_t* root)
{
	page_self_root = root;
	init_font_style(&font_14, lv_palette_lighten(LV_PALETTE_BROWN, 1), &lv_font_montserrat_14);
	
	//日历控件
	lv_calendar = lv_calendar_create(page_self_root);
	lv_obj_set_size(lv_calendar, 200, 180);
	lv_obj_add_style(lv_calendar, &style_transp, 0);
	lv_obj_align(lv_calendar, LV_ALIGN_CENTER, 0, 0);

#if LV_USE_CALENDAR_HEADER_DROPDOWN
    lv_calendar_header_dropdown_create(lv_calendar);
#elif LV_USE_CALENDAR_HEADER_ARROW
    lv_calendar_header_arrow_create(lv_canlendar);
#endif
	
	lv_obj_t *year_month_list = lv_obj_get_child(lv_calendar, 0);
	lv_month_matrix = lv_obj_get_child(lv_calendar, 1);
	lv_year_list = lv_obj_get_child(year_month_list, 0);
	lv_month_list = lv_obj_get_child(year_month_list, 1);
	
	lv_group_add_obj(lv_group_get_default(), lv_calendar);
	lv_obj_add_event_cb(lv_calendar, event_key_handler, LV_EVENT_KEY, NULL);
	lv_group_add_obj(lv_group_get_default(), lv_month_matrix);
	lv_obj_add_event_cb(lv_month_matrix, event_key_handler, LV_EVENT_KEY, NULL);
	lv_group_add_obj(lv_group_get_default(), lv_year_list);
	lv_obj_add_event_cb(lv_year_list, event_key_handler, LV_EVENT_KEY, NULL);
	lv_group_add_obj(lv_group_get_default(), lv_month_list);
	lv_obj_add_event_cb(lv_month_list, event_key_handler, LV_EVENT_KEY, NULL);
	
	lv_calendar_set_today_date(lv_calendar, 2021, 12, 12);
	lv_calendar_set_showed_date(lv_calendar, 2021, 12);
	
	//日期显示
	lv_date_label = lv_label_create(page_self_root);
	lv_obj_set_size(lv_date_label, LV_SIZE_CONTENT, LV_SIZE_CONTENT); 
	lv_obj_align(lv_date_label, LV_ALIGN_TOP_MID, 0, 15);
	lv_obj_add_style(lv_date_label, &font_14, 0);
	lv_label_set_text(lv_date_label, "");
}

void gui_canlendar_focus()
{
	lv_group_focus_obj(lv_month_matrix);
	
	date_init();
}