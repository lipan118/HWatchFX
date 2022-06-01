#include "gui_about.h"

lv_obj_t* lv_about;

static void event_key_handler(lv_event_t *e)
{	
	static uint8_t ym_flag = 0;
	lv_event_code_t code = lv_event_get_code(e);
	lv_obj_t * obj = lv_event_get_target(e);
	const uint32_t key = lv_indev_get_key(lv_indev_get_act());
	
	if(key == LV_KEY_BACKSPACE)
	{
		page_call_handler((void*)PAGE_APPS);
	}
	
}

void gui_about_init(lv_obj_t* root)
{
	page_self_root = root;
	lv_group_add_obj(lv_group_get_default(), page_self_root);
	lv_obj_add_event_cb(page_self_root, event_key_handler, LV_EVENT_KEY, NULL);

	lv_obj_t* lv_version = lv_label_create(page_self_root);
	lv_obj_set_size(lv_version, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
	lv_obj_set_style_text_align(lv_version, LV_TEXT_ALIGN_CENTER, 0);
	lv_obj_align(lv_version, LV_ALIGN_CENTER, 0, 0);
	char str[64];
	sprintf(str, "%s\n%s\n\nPowered by LVGL\n%d.%d.%d", APP_NAME, APP_VER, lv_version_major(), lv_version_minor(), lv_version_patch());
	printf("%s\n", str);
	lv_label_set_text(lv_version, str);
}

void gui_about_focus()
{
	lv_group_focus_obj(page_self_root);
}