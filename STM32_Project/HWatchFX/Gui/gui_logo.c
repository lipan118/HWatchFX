#include "gui_logo.h"

static void anim_w_cb(void *var, int32_t v)
{
    lv_obj_set_width(var, v);
}

static void anim_y_cb(void *var, int32_t v)
{
    lv_obj_set_y(var, v);
}

static void a2_ready_cb(lv_anim_t* a2)
{
	lv_anim_del(a2, NULL);
	gui_delay(250);
	page_call_handler((void*)PAGE_MAIN);
}

static void a1_ready_cb(lv_anim_t *a1)
{
	lv_obj_t* loading_processbar = lv_obj_create(page_self_root);
	lv_obj_set_size(loading_processbar, 5, 5);
	lv_obj_align_to(loading_processbar, a1->user_data, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 10);
	
	lv_anim_del(a1, NULL);

	lv_anim_t a2;
	lv_anim_init(&a2);
	lv_anim_set_var(&a2, loading_processbar);
	lv_anim_set_values(&a2, lv_obj_get_x(loading_processbar), 175);
	lv_anim_set_time(&a2, 300);
	lv_anim_set_delay(&a2, 100);
	lv_anim_set_ready_cb(&a2, a2_ready_cb);
	lv_anim_set_exec_cb(&a2, anim_w_cb);
	lv_anim_set_path_cb(&a2, lv_anim_path_linear);
	lv_anim_start(&a2);
}

static void logo_anim_start()
{
	lv_obj_t* logo_label = lv_label_create(page_self_root);
	lv_obj_add_style(logo_label, &font_32, 0);
	lv_label_set_text(logo_label, APP_NAME);
	lv_obj_align(logo_label, LV_ALIGN_TOP_MID, 0, 0);
	
	lv_anim_t a;
	lv_anim_init(&a);
	lv_anim_set_var(&a, logo_label);
	lv_anim_set_values(&a, 0, 90);
	lv_anim_set_time(&a, 300);
	lv_anim_set_delay(&a, 100);
	lv_anim_set_user_data(&a, (void*)logo_label);
	lv_anim_set_ready_cb(&a, a1_ready_cb);
	lv_anim_set_exec_cb(&a, anim_y_cb);
	lv_anim_set_path_cb(&a, lv_anim_path_overshoot);
	lv_anim_start(&a);
}

void gui_logo_init(lv_obj_t* root)
{
	page_self_root = root;
	lv_obj_clear_flag(page_self_root, LV_OBJ_FLAG_SCROLLABLE);

	init_font_style(&font_32, lv_palette_lighten(LV_PALETTE_RED, 1), &lv_font_montserrat_32);
}

void gui_logo_focus()
{
	gui_delay(200);
	logo_anim_start();
}