#ifndef DEVICE_CONFIG_H_INCLUDED
#define DEVICE_CONFIG_H_INCLUDED

#include "system.h"
#include "w25q16.h"

#define STATE_ON 	1
#define STATE_OFF 0

#define APP_NAME	"HWatchFX"
#define APP_VER		"V1.0.0"

#define CONFIG_CLEAR_ADMIN 0	//管理员重置配置
#define DEVICE_BOARD_CONFIG_ID 0xFFFFFFFE	//设备ID标记，最大值

#define ALARM_CLOCK_CNT_MAX 10 //最多10个闹钟

#define CONFIG_MAX_SIZE 256		//配置文件最大为一页大小
#define CONFIG_SECTOR_SIZE (16 * 256) //扇区大小
#define TOTAL_CONFIG_PAGE_SIZE (2 * 16 * 256) //使用W25Q16前8K空间，共2扇区，(2 * 16 * 256)字节


//全局配置结构体
typedef struct{
	u32 bord_id;	//32位ID
	u32 alarm_clock[ALARM_CLOCK_CNT_MAX]; //存储设定闹钟，包含星期，时，分//最多10个
	char app_name[16];	//软件名称
	char app_ver[16];	//软件版本
	
	u8 blight_level: 4;	//背光等级  (0 ~ 10)
	u8 sound_state: 1;		//音频状态	(0 | 1)
	u8 alarm_state: 1;		//闹钟状态	(0 | 1)
	u8 screen_state:1;		//屏幕亮灭状态	(0 | 1)
	u8 home_screen_lock_t:6; //主页面屏幕息屏等待时间(s)//10,20,30,40,50,60

}HWatchConfig;

typedef union{
	u8 config[sizeof(HWatchConfig)];
	HWatchConfig hwatch_config;
}G_config;

extern G_config g_config;

void HWatchDevice_Init(void);
void HWatchDevice_Printf(void);
void HWatchDevice_ConfigSave(void);

#endif // DEVICE_CONFIG_H_INCLUDED
