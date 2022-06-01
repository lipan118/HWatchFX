#include "gui_port.h"
#include "gui_main.h"
#include "gui_apps.h"
#include "gui_calc.h"
#include "gui_game.h"
#include "gui_canlendar.h"
#include "gui_setting.h"
#include "gui_env.h"
#include "gui_about.h"

page_list_t *plist;
lv_style_t style_transp;
page_call_handle page_call_handler;

//硬件定时器
sys_timer_t sys_timer = {
	.tick_state = sys_timer_state,
	.tick_set = sys_timer_set
}; 

static page_list_t *page_foreach();
static void page_add(page_init_handle init_handler, page_focus_handle focus_handler, uint8_t id);
static void page_remove(uint8_t id);
static void page_clear();
static page_list_t *page_find(uint8_t id);
static lv_obj_t* page_init(uint8_t id, page_list_t *page);

static page_list_t *page_foreach()
{
	page_list_t *tmp = plist;
	while(tmp->next_page)
	{
		tmp = tmp->next_page;
		printf("page_id: %d\n", tmp->id);
	}
	
	return tmp;
}

static void page_add(page_init_handle init_handler, page_focus_handle focus_handler, uint8_t id)
{
	page_list_t *tmp = plist;
	page_list_t *insert = NULL;
	
	//尾部插入
	while(tmp->next_page) tmp = tmp->next_page;
	
	insert = (page_list_t*)malloc(sizeof(page_list_t));
	if(insert == NULL)
		GUI_ASSERT
	
	insert->id = id;
	insert->init_handler = init_handler;
	insert->focus_handler = focus_handler;
	insert->next_page = tmp->next_page;
	tmp->next_page = insert;
}

static void page_remove(uint8_t id)
{
	page_list_t *tmp = plist;
	page_list_t *prev = tmp;
	page_list_t *del = NULL;
	
	while(tmp->next_page)
	{
		tmp = tmp->next_page;
		if(tmp->id == id)
		{			
			del = tmp;
			prev->next_page = tmp->next_page;
			break;
		}
		
		prev = tmp;
	}
}

static void page_clear()
{
	page_list_t *tmp = plist, *del = NULL;
	while(tmp->next_page)
	{
		del = tmp->next_page;
		tmp->next_page = del->next_page;
		free(del);
	}
}

static page_list_t *page_find(uint8_t id)
{
	page_list_t *tmp = plist;
	while(tmp->next_page)
	{
		tmp = tmp->next_page;
		if(tmp->id == id)
		{
			return tmp;
		}
	}
	
	return NULL;
}

static void lvgl_init()
{
	lv_init(); 							//lvgl 系统初始化
	lv_port_disp_init();		//lvgl 显示接口初始化
	lv_port_indev_init();		//lvgl 输入接口初始化，此处仅使用按键
	lv_group_set_default(group); //设置默认组, lv_port_indev.c
}

static lv_obj_t* page_init(uint8_t id, page_list_t *page)
{
	if(page)
	{
		page->root = lv_obj_create(NULL);
		lv_obj_set_size(page->root, GUI_WIDTH, GUI_HEIGHT);
		lv_obj_set_style_radius(page->root, LV_RADIUS_CIRCLE, 0);
		lv_obj_center(page->root);
		page->init_handler(page->root);
		
	}
	
	return page->root;
}

static void page_callback(void* arg)
{
	static uint8_t last_id = 0;
	uint8_t id = (uint8_t)arg;
	
	page_list_t *page = page_find(id);
	if(!page) return;
#if GUI_DEBUG
	printf("page handler: %d\n", page->id);
#endif
	
	lv_obj_t *root = page->root;
	
	if(id == PAGE_MAIN)
	{
		if(root) lv_scr_load_anim(root, LV_SCR_LOAD_ANIM_FADE_ON, ANIM_TIME, 50, true); //释放非主页内存
	}
	else
	{
		root = page_init(id, page);
		if(last_id == PAGE_MAIN)
		{
			if(root) lv_scr_load_anim(root, LV_SCR_LOAD_ANIM_MOVE_LEFT, ANIM_TIME, 50, false); //保留主页内存
		}
		else
		{
			uint8_t anim_type = id == PAGE_APPS ? LV_SCR_LOAD_ANIM_MOVE_BOTTOM : LV_SCR_LOAD_ANIM_MOVE_LEFT;
			if(root) lv_scr_load_anim(root, anim_type, ANIM_TIME, 50, true); 
		}
	}
	
	page->focus_handler();
	last_id = page->id;
}

void gui_port_init()
{
	lvgl_init();
	
	plist = (page_list_t*)malloc(sizeof(page_list_t));
	if(plist == NULL)
		GUI_ASSERT
	
	//添加页面
	page_add(PAGE_INIT_DEF(main), PAGE_FOCUS_DEF(main), PAGE_MAIN);
	page_add(PAGE_INIT_DEF(apps), PAGE_FOCUS_DEF(apps), PAGE_APPS);
	page_add(PAGE_INIT_DEF(canlendar), PAGE_FOCUS_DEF(canlendar), PAGE_APP_CANLENDAR);
	page_add(PAGE_INIT_DEF(calc), PAGE_FOCUS_DEF(calc), PAGE_APP_CALC);
	page_add(PAGE_INIT_DEF(stopwatch), PAGE_FOCUS_DEF(stopwatch), PAGE_APP_STOPWATCH);
	page_add(PAGE_INIT_DEF(game), PAGE_FOCUS_DEF(game), PAGE_APP_GAME);
	page_add(PAGE_INIT_DEF(setting), PAGE_FOCUS_DEF(setting), PAGE_APP_SETTING);
	page_add(PAGE_INIT_DEF(about), PAGE_FOCUS_DEF(about), PAGE_APP_ABOUT);
	page_add(PAGE_INIT_DEF(logo), PAGE_FOCUS_DEF(logo), PAGE_LOGO);

#if GUI_DEBUG
	page_foreach();
#endif
	
	//初始化透明背景样式
	lv_style_init(&style_transp);
	lv_style_set_bg_opa(&style_transp, LV_OPA_TRANSP);
	lv_style_set_border_opa(&style_transp, LV_OPA_TRANSP);
	
	//初始化页面回调
	page_call_handler = page_callback;
	
	//初始化首页
	page_init(PAGE_MAIN, page_find(PAGE_MAIN));
	page_list_t *page = page_find(PAGE_LOGO);
	if(page)
	{
		lv_scr_load(page_init(PAGE_LOGO, page));
		page->focus_handler();
	}
}

void gui_port_task()
{
	lv_task_handler();
}

void gui_port_tick()
{
	lv_tick_inc(1);
}

void gui_port_systick()
{
	static uint16_t tick_cnt = 0;
	tick_cnt++;
	if(tick_cnt % sys_timer.tick_period != 0) return;
	
	if(sys_timer.tick_state())
		sys_timer.tick_call_handle();

}

void gui_port_memlog()
{
	lv_read_mem(NULL);
}

uint8_t key_read(uint8_t mode)
{
	static u8 key=1;
	if(key==1&&(KEY_UP_VAL==1||KEY_DOWN_VAL==1||KEY_LEFT_VAL==1||KEY_RIGHT_VAL==1||KEY_A_VAL==1||KEY_B_VAL==1)) //任意一个按键按下
	{
		buzzer_on();
		gui_delay(20);
		buzzer_off();
		key=0;
		if(KEY_UP_VAL==1)
		{
			return KEY_UP_DEF; 
		}
		else if(KEY_DOWN_VAL==1)
		{
			return KEY_DOWN_DEF; 
		}
		else if(KEY_LEFT_VAL==1)
		{
			return KEY_LEFT_DEF;
		}
		else if(KEY_RIGHT_VAL==1)
		{
			return KEY_RIGHT_DEF;
		}else if(KEY_A_VAL==1)
		{
			return KEY_A_DEF;
		}else
		{
			return KEY_B_DEF;
		}
	}
	else if(KEY_UP_VAL==0&&KEY_DOWN_VAL==0&&KEY_LEFT_VAL==0&&KEY_RIGHT_VAL==0&&KEY_A_VAL==0&&KEY_B_VAL==0)    //无按键按下
	{
		key=1;
	}
	if(mode==1) //连续按键按下
	{
		key=1;
	}
	return 0;
}









