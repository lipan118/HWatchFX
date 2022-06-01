#include "gui_calc.h"
#include "calculator.h"

lv_obj_t *lv_enter_label;
lv_obj_t *lv_result_label;
lv_obj_t *lv_btnm;

/**

KEY

0 	1 	2 	3
4 	5 	6 	7
8 	9 	10 	11
12  13 	14 	15
16  17 	18 	19

*/
static const char * calc_btn_map[] = {	
	"(", ")", "CE", "/", "\n",
	"7", "8", "9", "*", "\n",
	"4", "5", "6", "-", "\n",
	"1", "2", "3", "+", "\n",
	".", "0", "ANS", "=", ""
};

void calc_callback(char *enter_str, double result)
{
	char out[100];
//	printf("%s, %1f\n", enter_str, result);
	sprintf(out, "%1f", result);
	lv_label_set_text(lv_enter_label,enter_str);
	lv_label_set_text(lv_result_label, out);
}

static void event_key_handler(lv_event_t *e)
{	
	switch(e->code)
	{
		case LV_EVENT_KEY:
		{
			const uint32_t key = lv_indev_get_key(lv_indev_get_act());
			lv_obj_t * obj = lv_event_get_target(e);
			
			if(key == LV_KEY_ENTER)
			{
				uint32_t id = lv_btnmatrix_get_selected_btn(obj);
        const char * txt = lv_btnmatrix_get_btn_text(obj, id);
				
				//printf("Click key val: %s\n", txt);
				
				Calc_Run(id);
			}else if(key == LV_KEY_BACKSPACE)
			{
				lv_label_set_text(lv_enter_label,"");
				lv_label_set_text(lv_result_label, "0");
				
				Calc_Exit();
				page_call_handler((void*)PAGE_APPS);
			}
		}
	}
}

void gui_calc_init(lv_obj_t* root)
{
	page_self_root = root;
//	lv_group_add_obj(group, page_self_root);
//	lv_obj_add_event_cb(page_self_root, event_key_handler, LV_EVENT_KEY, NULL);
	
	init_font_style(&font_14, lv_palette_lighten(LV_PALETTE_RED, 1), &lv_font_montserrat_14);
	
	//输入显示标签
	lv_enter_label = lv_label_create(page_self_root);
	lv_label_set_long_mode(lv_enter_label, LV_LABEL_LONG_SCROLL_CIRCULAR);
	lv_obj_set_size(lv_enter_label, 168, 16);
	lv_label_set_text(lv_enter_label,"");
	lv_obj_set_style_text_align(lv_enter_label, LV_TEXT_ALIGN_CENTER, 0);
	lv_obj_add_style(lv_enter_label, &font_14, 0);
	lv_obj_align(lv_enter_label, LV_ALIGN_TOP_MID, 0, 20);
	
	//结果显示标签
	lv_result_label = lv_label_create(page_self_root);
	lv_label_set_long_mode(lv_result_label, LV_LABEL_LONG_SCROLL_CIRCULAR);
	lv_obj_set_size(lv_result_label, 168, 16);
	lv_label_set_text(lv_result_label,"0");
	lv_obj_set_style_text_align(lv_enter_label, LV_TEXT_ALIGN_CENTER, 0);
	lv_obj_add_style(lv_enter_label, &font_14, 0);
	lv_obj_align_to(lv_result_label, lv_enter_label, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
	
	//按钮矩阵
	lv_btnm = lv_btnmatrix_create(page_self_root);
	lv_obj_set_size(lv_btnm, 210, 155);
	lv_obj_add_style(lv_btnm, &style_transp, 0);
	lv_btnmatrix_set_map(lv_btnm, calc_btn_map);
	lv_obj_align_to(lv_btnm, lv_result_label, LV_ALIGN_OUT_BOTTOM_MID, 0, 2);
	
	lv_group_add_obj( lv_group_get_default(), lv_btnm );
	lv_obj_add_event_cb(lv_btnm, event_key_handler, LV_EVENT_ALL, NULL);
	
	//计算器相关初始化
	Init_Calc_Book(calc_callback);
}

void gui_calc_focus()
{
	lv_group_focus_obj(lv_btnm);

}