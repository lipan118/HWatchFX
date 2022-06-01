#include "gui_apps.h"

#define APPS_COUNT 6

lv_obj_t *lv_objs[APPS_COUNT];
static lv_obj_t* title_label;
static short btn_index = 0;

static const lv_coord_t obj_width = 56;
static const lv_coord_t obj_height = 52;
static uint8_t _show = 0;

static lv_style_t obj_style;

static const char *apps_icon[] = 
{
	HWATCH_SYMBOL_CALENDAR,
	HWATCH_SYMBOL_CALC,
	HWATCH_SYMBOL_HOURGLASS,
	HWATCH_SYMBOL_GAME,
	HWATCH_SYMBOL_SETTING,
	HWATCH_SYMBOL_ABOUT
};

static const char*apps_title[] = 
{
	"Calendar", "Calculator", "StopWatch", "Game", "Setting", "About"
};

static void event_key_handler(lv_event_t *e)
{	
	switch(e->code)
	{
		case LV_EVENT_KEY:
		{
			const uint32_t key = lv_indev_get_key(lv_indev_get_act());
//			lv_obj_t *target = lv_event_get_target(e);
//			int id = (int)lv_obj_get_user_data(target);
			
			if(key == LV_KEY_LEFT || key == LV_KEY_UP)
			{
				btn_index--;
				btn_index = btn_index <= 0 ? 0 : btn_index;

				if (btn_index >= 0)
						lv_obj_remove_style(lv_objs[btn_index + 1], &obj_style, 0);
				lv_obj_add_style(lv_objs[btn_index], &obj_style, 0);
			}
			else if(key == LV_KEY_RIGHT || key == LV_KEY_DOWN)
			{
				btn_index++;
				btn_index = btn_index >= APPS_COUNT ? (APPS_COUNT - 1) : btn_index;

				if (btn_index > 0)
						lv_obj_remove_style(lv_objs[btn_index - 1], &obj_style, 0);
				lv_obj_add_style(lv_objs[btn_index], &obj_style, 0);
			}
			else if(key == LV_KEY_ENTER)
			{
				if(page_call_handler) page_call_handler((void*)(btn_index+2));
			}
			else if(key == LV_KEY_BACKSPACE)
			{
				if(page_call_handler)
				{
					btn_index = 0;
					page_call_handler((void*)PAGE_MAIN);
				}
				
			}
			
			lv_label_set_text(title_label, apps_title[btn_index]);
		}
			break;
	}
}

static void add_btn(lv_obj_t *root, uint8_t index)
{
	lv_objs[index] = lv_obj_create(root);
	lv_obj_set_user_data(lv_objs[index], (void*)index);   
	lv_obj_set_size(lv_objs[index], obj_width, obj_height);

	lv_obj_t *label = lv_label_create(lv_objs[index]);
	lv_obj_set_size(label, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
	lv_obj_add_style(label, &font_icon_16, 0);     //设置字体样式
	lv_label_set_text(label, apps_icon[index]); //设置字体
	lv_obj_center(label);
}

void gui_apps_init(lv_obj_t* root)
{
	page_self_root = root;
	lv_group_add_obj(lv_group_get_default(), page_self_root);
	lv_obj_add_event_cb(page_self_root, event_key_handler, LV_EVENT_KEY, NULL);
	
	init_font_style(&font_icon_16, lv_palette_lighten(LV_PALETTE_PINK, 1), &hwatch_icons_font_16);
	init_font_style(&font_14, lv_palette_lighten(LV_PALETTE_BROWN, 1), &lv_font_montserrat_14);
	
	//顶部标题
	title_label = lv_label_create(page_self_root);
	lv_obj_set_size(title_label, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
	lv_obj_align(title_label, LV_ALIGN_TOP_MID, 0, 10);
	lv_obj_add_style(title_label, &font_14, 0);
	lv_label_set_text(title_label, apps_title[0]);
	
	//创建按钮布局
	lv_obj_t *lv_btn_cont = lv_obj_create(page_self_root);
	lv_obj_set_size(lv_btn_cont, GUI_WIDTH, GUI_HEIGHT);
	lv_obj_add_style(lv_btn_cont, &style_transp, 0);
	lv_obj_center(lv_btn_cont);

	lv_obj_set_flex_flow(lv_btn_cont, LV_FLEX_FLOW_ROW_WRAP);
	lv_obj_set_flex_align(lv_btn_cont, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
	
	//添加按钮
	for (int i = 0; i < APPS_COUNT; i++)
	{
		add_btn(lv_btn_cont, i);
	}
	
	//初始化按钮边框样式
	init_outline_shadow_style(&obj_style, LV_OPA_0, LV_PALETTE_BLUE, obj_width - 20);
	lv_obj_add_style(lv_objs[btn_index], &obj_style, 0);
}

void gui_apps_focus()
{
	lv_group_focus_obj(page_self_root);
}
