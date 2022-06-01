#include "gui_setting.h"

#define SETTING_COUNT 4

#define SCREEN_RESET_TIME_OPTIONS ("\
10\n20\n30\n40\n50\n60\
")\

lv_obj_t* tab_title_label;
lv_obj_t* tab_conts[SETTING_COUNT];
lv_obj_t* lv_sound_sw;
lv_obj_t* lv_backlight_slider;
lv_obj_t* lv_backlight_label;
lv_obj_t* lv_backlight_dropdown;
lv_obj_t* lv_alarmstate_label;
lv_obj_t* lv_alarmclock_list;
lv_obj_t* lv_ble_textarea;

uint8_t x1 = 30;
uint8_t x2 = 210;
static short index = 0;
bool anim_runnning = false;

static char str[64];

static const char*titles[] = {
	"Sound","BackLight","AlarmClock","BlueTooth"
};

static void tab_index_set(void);

static void anim_x_cb(void * var, int32_t v)
{
    lv_obj_set_x(var, v);
}

static void anim_size_cb(void * var, int32_t v)
{
    lv_obj_set_size(var, v, v);
}

static void obj_init_task()
{
	char clock[8];
	
	//声音状态
	lv_obj_add_state(lv_sound_sw, g_config.hwatch_config.sound_state); 
	
	//背光状态
	lv_slider_set_value(lv_backlight_slider, g_config.hwatch_config.blight_level, LV_ANIM_ON); 
	char str[16];
	sprintf(str, "%d", (int)lv_slider_get_value(lv_backlight_slider));
	lv_label_set_text(lv_backlight_label, str);
	
	//息屏时间
	lv_dropdown_set_selected(lv_backlight_dropdown, (g_config.hwatch_config.home_screen_lock_t/10)-1);
	
	//闹钟状态
	lv_label_set_text(lv_alarmstate_label, g_config.hwatch_config.alarm_state ? "On" : "Off");
	
	//闹钟列表
	//6位
	//星期,时,分
	uint8_t clock_cnt = sizeof(g_config.hwatch_config.alarm_clock) / sizeof(uint32_t);
	for(int i=0;i< clock_cnt;i++)
	{
		sprintf(clock, "%06d", g_config.hwatch_config.alarm_clock[i]);
		lv_list_add_text(lv_alarmclock_list, clock);
	}
}

static void obj_focus_switch()
{
	switch(index)
	{
		case 0:
			lv_group_focus_obj(lv_sound_sw);
			break;
		case 1:
			lv_group_focus_obj(lv_backlight_slider);
			break;
		case 2:
			lv_group_focus_obj(lv_alarmclock_list);
			break;
		case 3:
			lv_group_focus_obj(lv_ble_textarea);
			break;
	}
}

static void anim_ready_cb(lv_anim_t *a)
{
	lv_anim_del(a, NULL);
	anim_runnning = false;
	
	uint8_t user_data = (uint8_t)(a->user_data);
//	obj_focus_switch();
}

static void tab_index_set()
{
	if(anim_runnning) return;
	
	memset(str, 0x00, sizeof(str));
	sprintf(str, "#%s <# #0000ff %s# #%s >#", index <= 0 ? "cccccc" : "666666", titles[index], index < (SETTING_COUNT - 1) ? "666666" : "cccccc");
	lv_label_set_text(tab_title_label, str);
	
	for(int i=0;i<SETTING_COUNT;i++)
	{
		lv_obj_add_flag(tab_conts[i], LV_OBJ_FLAG_HIDDEN);
		lv_obj_set_x(tab_conts[i], x2);
	}
	
	lv_obj_clear_flag(tab_conts[index], LV_OBJ_FLAG_HIDDEN);
	
	//设置动画切换
	lv_anim_t a;
	lv_anim_init(&a);
	lv_anim_set_var(&a, tab_conts[index]);
	lv_anim_set_values(&a, x2, x1);
	lv_anim_set_time(&a, 250);
	lv_anim_set_user_data(&a, (void*)index);
	lv_anim_set_path_cb(&a, lv_anim_path_ease_in_out);
	lv_anim_set_exec_cb(&a, anim_x_cb);
	lv_anim_set_ready_cb(&a, anim_ready_cb);
	lv_anim_start(&a);
	anim_runnning = true;
	
}

static void event_key_handler(lv_event_t *e)
{	
	lv_event_code_t code = lv_event_get_code(e);
	lv_obj_t * obj = lv_event_get_target(e);
	const uint32_t key = lv_indev_get_key(lv_indev_get_act());
	uint8_t obj_child_cnt = lv_obj_get_child_cnt(obj);
	
	if(obj == page_self_root)
	{
		if(key == LV_KEY_BACKSPACE)
		{
			//退出保存配置
			HWatchDevice_ConfigSave();
			gui_delay(100);
			page_call_handler((void*)PAGE_APPS);
		}
		else if(key == LV_KEY_ENTER)
		{
			obj_focus_switch();
		}
		else if(key == LV_KEY_LEFT)
		{
			index--;
			if(index <= 0) index = 0;
			tab_index_set();
		}
		else if(key == LV_KEY_RIGHT)
		{
			index++;
			if(index >= (SETTING_COUNT - 1)) index = (SETTING_COUNT - 1);
			tab_index_set();
		}
	}
	else if(obj == lv_sound_sw)
	{
		if(key == LV_KEY_BACKSPACE)
		{
			lv_group_focus_obj(page_self_root);
		}
		else if(key == LV_KEY_ENTER)
		{
			
		}
	}
	else if(obj == lv_backlight_slider)
	{
		if(key == LV_KEY_BACKSPACE)
		{
			lv_group_focus_obj(lv_backlight_dropdown);
		}
	}
	else if(obj == lv_backlight_dropdown)
	{
		if(key == LV_KEY_BACKSPACE)
		{
			lv_group_focus_obj(page_self_root);
		}
	}
	else if(obj == lv_alarmclock_list)
	{
		if(key == LV_KEY_ENTER)
		{
			//设置闹钟状态
			g_config.hwatch_config.alarm_state = !g_config.hwatch_config.alarm_state;
			lv_label_set_text(lv_alarmstate_label, g_config.hwatch_config.alarm_state ? "On" : "Off");
		}
		else if(key == LV_KEY_BACKSPACE)
		{
			lv_group_focus_obj(page_self_root);
		}
		else if(key == LV_KEY_LEFT)
		{
			for(int i=0;i<obj_child_cnt;i++)
			{
				lv_obj_t *label = lv_obj_get_child(obj, i);
				lv_label_set_text(label, "-- -- --");
				
				g_config.hwatch_config.alarm_clock[i] = 0; //清除所有闹钟
			}
			
			buzzer_on();
			gui_delay(200);
			buzzer_off();
		}
	}
	else if(obj == lv_ble_textarea)
	{
		if(key == LV_KEY_BACKSPACE)
		{
			lv_group_focus_obj(page_self_root);
		}
		else if(key == LV_KEY_ENTER)
		{
			
		}
	}

}

static void event_val_cb(lv_event_t *e)
{
	lv_obj_t * obj = lv_event_get_target(e);
	char buf[16];
	if(obj == lv_backlight_slider)
	{
		g_config.hwatch_config.blight_level = (int)lv_slider_get_value(obj);
    lv_snprintf(buf, sizeof(buf), "%d", (int)lv_slider_get_value(obj));
    lv_label_set_text(lv_backlight_label, buf);
		
		lcd_backlight_set(g_config.hwatch_config.blight_level); //设置背光等级
	}
	else if(obj == lv_backlight_dropdown)
	{
		lv_dropdown_get_selected_str(lv_backlight_dropdown, buf, sizeof(buf));
		g_config.hwatch_config.home_screen_lock_t = atoi(buf); //设置息屏时间
	}
	else if(obj == lv_sound_sw)
	{
		g_config.hwatch_config.sound_state = lv_obj_has_state(obj, LV_STATE_CHECKED);
		printf("%d\n", g_config.hwatch_config.sound_state);  //设置声音状态
	}
}

void gui_setting_init(lv_obj_t* root)
{
	page_self_root = root;
	lv_group_add_obj(lv_group_get_default(), page_self_root);
	lv_obj_add_event_cb(page_self_root, event_key_handler, LV_EVENT_KEY, NULL);
	
	//顶部标题
	tab_title_label = lv_label_create(page_self_root);
	lv_obj_set_size(tab_title_label, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
	lv_label_set_recolor(tab_title_label, true); 
	lv_obj_align(tab_title_label, LV_ALIGN_TOP_MID, 0, 15);
	lv_obj_add_style(tab_title_label, &font_14, 0);
	
	//容器
	for(int i=0;i<SETTING_COUNT;i++)
	{
		tab_conts[i] = lv_obj_create(page_self_root);
		lv_obj_set_size(tab_conts[i], 180, 160);
		lv_obj_add_style(tab_conts[i], &style_transp, 0);
		lv_group_add_obj(lv_group_get_default(), tab_conts[i]);
		lv_obj_add_event_cb(tab_conts[i], event_key_handler, LV_EVENT_KEY, NULL);
		lv_obj_add_flag(tab_conts[i], LV_OBJ_FLAG_HIDDEN);
		lv_obj_clear_flag(tab_conts[i], LV_OBJ_FLAG_SCROLLABLE);
		lv_obj_set_y(tab_conts[i], 40);
		lv_obj_set_x(tab_conts[i], x1);
	}
	
	//声音开关
	lv_sound_sw = lv_switch_create(tab_conts[0]);
	lv_obj_set_size(lv_sound_sw, 60, 25);
	lv_obj_add_event_cb(lv_sound_sw, event_key_handler, LV_EVENT_KEY, NULL);
	lv_obj_center(lv_sound_sw);
	lv_obj_add_event_cb(lv_sound_sw, event_val_cb, LV_EVENT_VALUE_CHANGED, NULL);
	
	//背光滑动条
	lv_backlight_slider = lv_slider_create(tab_conts[1]);
	lv_obj_set_size(lv_backlight_slider, 120, 16);
	lv_obj_align(lv_backlight_slider, LV_ALIGN_TOP_MID, 0, 20);
	lv_slider_set_range(lv_backlight_slider, 1, 10);
	lv_obj_add_event_cb(lv_backlight_slider, event_key_handler, LV_EVENT_KEY, NULL);
	lv_obj_add_event_cb(lv_backlight_slider, event_val_cb, LV_EVENT_VALUE_CHANGED, NULL);
	
	lv_backlight_label = lv_label_create(tab_conts[1]);
	lv_label_set_text(lv_backlight_label, "");
	lv_obj_align_to(lv_backlight_label, lv_backlight_slider, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);
	
	//息屏等待下拉列表
	lv_backlight_dropdown = lv_dropdown_create(tab_conts[1]);
	lv_dropdown_set_options(lv_backlight_dropdown, SCREEN_RESET_TIME_OPTIONS);
	lv_obj_align_to(lv_backlight_dropdown, lv_backlight_label, LV_ALIGN_OUT_BOTTOM_MID, 0, 20);
	lv_obj_add_event_cb(lv_backlight_dropdown, event_key_handler, LV_EVENT_KEY, NULL);
	lv_obj_add_event_cb(lv_backlight_dropdown, event_val_cb, LV_EVENT_VALUE_CHANGED, NULL);
	
	//闹钟列表
	lv_alarmclock_list = lv_list_create(tab_conts[2]);
	lv_obj_set_size(lv_alarmclock_list, 155, 130);
	lv_group_add_obj(lv_group_get_default(), lv_alarmclock_list);
	lv_obj_add_event_cb(lv_alarmclock_list, event_key_handler, LV_EVENT_KEY, NULL);
	lv_obj_center(lv_alarmclock_list);
	
	lv_alarmstate_label = lv_label_create(tab_conts[2]);
	lv_obj_align(lv_alarmstate_label, LV_ALIGN_TOP_MID, 0, -15);
	
	
	//蓝牙消息文本区域
	lv_ble_textarea = lv_textarea_create(tab_conts[3]);
	lv_obj_set_size(lv_ble_textarea, 160, 130);
	lv_obj_add_event_cb(lv_ble_textarea, event_key_handler, LV_EVENT_KEY, NULL);
	lv_textarea_set_max_length(lv_ble_textarea, 1024);
	lv_obj_center(lv_ble_textarea);
	
}

void gui_setting_focus()
{
	lv_group_focus_obj(page_self_root);
	tab_index_set();
	
	lv_timer_t *timer = lv_timer_create_basic();
	lv_timer_set_cb(timer, obj_init_task);
	lv_timer_set_repeat_count(timer, 1);
}

/**
 * 通信处理回调
 */
void comm_handler(const char* msg)
{
	if(lv_ble_textarea == NULL) return;
	lv_textarea_set_text(lv_ble_textarea, "");
	lv_textarea_add_text(lv_ble_textarea, msg);
}
