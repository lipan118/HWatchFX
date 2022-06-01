#include "gui_stopwatch.h"

#define SEC_CNT_MAX 15

lv_obj_t *sec_label;
lv_obj_t *sec_list;

static uint8_t list_index = 0;

typedef struct SecStruct
{
	uint8_t min;
	uint8_t sec;
	uint8_t ms;
}_sec_t;

_sec_t sec_t;

static void list_add(const char * txt);
static void list_clear(void);

static void sec_timer_task()
{
	sec_t.ms++;
	if(sec_t.ms == 100)
	{
		sec_t.ms = 0;
		sec_t.sec++;
		if(sec_t.sec == 60)
		{
			sec_t.sec = 0;
			sec_t.min++;
			if(sec_t.min == 60)
			{
				sys_timer.tick_set(false); //超出范围,暂停计时
			}
		}
	}
	
	char str[32];
	memset(str, 0x00, sizeof(str));
	sprintf(str, "%02d:%02d.%02d", sec_t.min, sec_t.sec, sec_t.ms);
	lv_label_set_text(sec_label, str);
}

static void list_add(const char * txt)
{
	if(list_index >= SEC_CNT_MAX) return;
	lv_obj_t *label = lv_obj_get_child(sec_list, list_index);
	lv_label_set_text(label, txt);
	lv_event_send(label, LV_EVENT_REFRESH, NULL);
	list_index++;
}

static void list_clear()
{
	for(int i=0;i<SEC_CNT_MAX;i++)
	{
		lv_obj_t *label = lv_obj_get_child(sec_list, i);
		lv_label_set_text(label, "--:--.--");
	}
	
	lv_label_set_text(sec_label, "00:00.00");
	
	list_index = 0;
	sec_t.min = 0;
	sec_t.sec = 0;
	sec_t.ms = 0;
}

static void event_key_handler(lv_event_t *e)
{	
	static uint8_t ym_flag = 0;
	lv_event_code_t code = lv_event_get_code(e);
	lv_obj_t * obj = lv_event_get_target(e);
	const uint32_t key = lv_indev_get_key(lv_indev_get_act());
	
	if(key == LV_KEY_BACKSPACE)
	{
		//暂停
		if(sys_timer.tick_state())
		{
			sys_timer.tick_set(false);
		}
		else
		{
			list_clear();
			page_call_handler((void*)PAGE_APPS);
		}
	}
	else if(key == LV_KEY_ENTER)
	{
		//开始
		if(!sys_timer.tick_state())
		{
			sys_timer.tick_set(true);
		}
		else
		{
			//添加记录
			char str[32];
			memset(str, 0x00, sizeof(str));
			sprintf(str, "%02d %02d:%02d.%02d", list_index, sec_t.min, sec_t.sec, sec_t.ms);
			list_add(str);
		}
	
	}
	else if(key == LV_KEY_LEFT)
	{
		if(sys_timer.tick_state()) return; //运行状态不清除记录
		//清空记录
		list_clear();
		buzzer_on();
		gui_delay(100);
		buzzer_off();
	}
}

void gui_stopwatch_init(lv_obj_t* root)
{
	page_self_root = root;
//	lv_group_add_obj(lv_group_get_default(), page_self_root);
//	lv_obj_add_event_cb(page_self_root, event_key_handler, LV_EVENT_KEY, NULL);
	
	init_font_style(&font_22, lv_palette_lighten(LV_PALETTE_RED, 1), &lv_font_montserrat_22);
	
	//秒标签
	sec_label = lv_label_create(page_self_root);
	lv_obj_set_size(sec_label, 180, LV_SIZE_CONTENT);
	lv_label_set_recolor(sec_label, true); 
	lv_obj_set_style_text_align(sec_label, LV_TEXT_ALIGN_CENTER, 0);
	lv_obj_align(sec_label, LV_ALIGN_TOP_MID, 0, 20);
	lv_obj_add_style(sec_label, &font_22, 0);
	lv_label_set_text(sec_label, "#ff0000 00:00.00#");
	
	//列表
	sec_list = lv_list_create(page_self_root);
	lv_obj_set_size(sec_list, 180, 160);
	lv_obj_align_to(sec_list, sec_label, LV_ALIGN_OUT_BOTTOM_MID, 0, 5);
	lv_group_add_obj(lv_group_get_default(), sec_list);
	lv_obj_add_event_cb(sec_list, event_key_handler, LV_EVENT_KEY, NULL);
	
	for(int i=0;i<SEC_CNT_MAX;i++)
	{
		lv_list_add_text(sec_list, "--:--.--");
	}

	sys_timer.tick_call_handle = sec_timer_task;
	sys_timer.tick_period = 10; 
	sys_timer.tick_set(false);
}

void gui_stopwatch_focus()
{
	lv_group_focus_obj(sec_list);
}