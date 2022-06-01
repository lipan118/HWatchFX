#ifndef _GUI_PORT_H
#define _GUI_PORT_H

#include "gui_inc.h"

#define GUI_ASSERT while(1){}

typedef void(*page_init_handle)(lv_obj_t* root);
typedef void(*page_focus_handle)();

typedef struct PageList
{
	uint8_t id;
	lv_obj_t* root;
	page_init_handle init_handler;
	page_focus_handle focus_handler;
	struct PageList *next_page;
}page_list_t;

void gui_port_init(void);
void gui_port_task(void);
void gui_port_tick(void);
void gui_port_systick(void);
void gui_port_memlog(void);

#endif
