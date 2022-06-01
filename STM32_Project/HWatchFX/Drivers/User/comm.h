#ifndef _COMM_H
#define _COMM_H

#include "system.h"
#include "usart.h"
#include "devices.h"
#include <stdbool.h>

/**
 * 通信模块
 * 通信协议请参考Readme.txt文件
 */

enum CMD_MASTER_STATE
{
	MASTER_CONN = 0x0,
	MASTER_ALARM_GET,
	MASTER_TIME_SET,
	MASTER_ALARM_SET
};

enum CMD_SLAVE_STATE
{
	SLAVE_ENV_UPLOAD = 0x10,
	SLAVE_ALARM_UPLOAD
};

typedef struct TimeSetCmd
{
	uint8_t use[7];
	uint8_t remain[26];
}tim_set_t;

typedef struct AlarmSetCmd
{
	uint8_t use[4];
	uint8_t remain[29];
}alarm_set_t;

typedef struct EnvInfoCmd
{
	uint8_t use[3][4];
	uint8_t remain[21];
}env_info_t;

typedef struct AlarmInfoCmd
{
	uint8_t use[30];
	uint8_t remain[3];
}alarm_info_t;

typedef struct FrameData
{
	uint8_t head[2];
	uint8_t state;
	union data_t
	{
		tim_set_t timeset;
		alarm_set_t ararmset;
		env_info_t envinfo;
		alarm_info_t araminfo;
		uint8_t content[33];
	}data;
	uint8_t tail[2];
	
}frame_data_t;

void frame_recv(void);
void comm_env_set(float temp, float humi, float press);
void comm_upload(uint8_t state);
__weak void comm_handler(const char * msg);

#endif