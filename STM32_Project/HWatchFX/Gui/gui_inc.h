#ifndef _GUI_INC_H
#define _GUI_INC_H

/**
 *@ HWatchFX
 *@ Aries.Hu
 */

#include <stdio.h>
#include "lvgl.h"
#include "lv_port_disp.h"
#include "lv_port_indev.h"
#include "hwatch_symbol_def.h"
#include "devices.h"
#include "comm.h"

#define GUI_DEBUG 1

#define PAGE_INIT_DEF(TAG)  gui_##TAG##_init
#define PAGE_FOCUS_DEF(TAG) gui_##TAG##_focus

#define GUI_WIDTH  LCD_1IN28_WIDTH
#define GUI_HEIGHT LCD_1IN28_HEIGHT

#define ANIM_TIME 250

#define gui_delay(t){\
	int _t = t;\
	while(_t--) \
	{ \
		lv_task_handler();\
		CPU_TS_Tmr_Delay_MS(1);\
	}\
}

enum GUI_PAGE
{
	PAGE_NULL = -1,
	PAGE_MAIN,
	PAGE_APPS,
	PAGE_APP_CANLENDAR,
	PAGE_APP_CALC,
	PAGE_APP_STOPWATCH,
	PAGE_APP_GAME,
	PAGE_APP_SETTING,
	PAGE_APP_ABOUT,
	PAGE_LOGO
};

typedef void(*page_call_handle)(void* arg);

typedef struct SysTimer
{
	void(*tick_call_handle)();
	bool(*tick_state)();
	void(*tick_set)(bool en);
	uint16_t tick_period;
}sys_timer_t;

LV_FONT_DECLARE(hwatch_icons_font_16);

extern page_call_handle page_call_handler;
extern lv_style_t style_transp;
extern sys_timer_t sys_timer;

static lv_obj_t* page_self_root;

static lv_style_t font_32;
static lv_style_t font_14;
static lv_style_t font_22;
static lv_style_t font_icon_16;
static lv_style_t font_icon_24;

//页面初始化函数
void gui_logo_init(lv_obj_t* root);
void gui_main_init(lv_obj_t* root);
void gui_apps_init(lv_obj_t* root);
void gui_calc_init(lv_obj_t* root);
void gui_game_init(lv_obj_t* root);
void gui_canlendar_init(lv_obj_t* root);
void gui_setting_init(lv_obj_t* root);
void gui_env_init(lv_obj_t* root);
void gui_about_init(lv_obj_t* root);
void gui_stopwatch_init(lv_obj_t* root);
//页面焦点函数
void gui_logo_focus(void);
void gui_main_focus(void);
void gui_apps_focus(void);
void gui_calc_focus(void);
void gui_game_focus(void);
void gui_canlendar_focus(void);
void gui_setting_focus(void);
void gui_env_focus(void);
void gui_about_focus(void);
void gui_stopwatch_focus(void);

static inline void init_font_style(lv_style_t *style, lv_color_t color, const lv_font_t *font)
{
	lv_style_reset(style);
	lv_style_init(style);

	lv_style_set_text_font(style, font);
	lv_style_set_text_color(style, color);
}

static inline void lv_read_mem(float *pmem)
{
	float _mem = 0.0f;
	lv_mem_monitor_t gui_mem_monitor; //内存使用
	lv_mem_monitor(&gui_mem_monitor);
	_mem = (gui_mem_monitor.total_size - gui_mem_monitor.free_size) * 1.0f / gui_mem_monitor.total_size * 100.0f;
	
	if(!pmem)
	{
#if GUI_DEBUG
		printf("lv_mem: %.1f %%\n", _mem);
#endif
	}
	else
		*pmem = _mem;
}

static inline void init_outline_shadow_style(lv_style_t *style, lv_opa_t opa, lv_palette_t color, uint8_t w)
{
	lv_style_reset(style);
	lv_style_init(style);

	// Set background & radius
	lv_style_set_radius(style, 5);
	lv_style_set_bg_opa(style, opa);
	lv_style_set_bg_color(style, lv_palette_lighten(LV_PALETTE_GREY, 1));

	// Add shadow
	lv_style_set_shadow_width(style, w);
	lv_style_set_shadow_color(style, lv_palette_main(color));
}

#endif





