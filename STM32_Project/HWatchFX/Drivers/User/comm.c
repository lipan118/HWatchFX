#include "comm.h"
#include <string.h>

frame_data_t frame_data;

uint8_t frame_len = sizeof(frame_data_t);
char msg_buf[64];

__weak void comm_handler(const char * msg)
{
	
}

void FloatToByte(float floatNum,unsigned char* byteArry)
{
	char* pchar=(char*)&floatNum;
	for(int i=0;i<4;i++)
	{
		*byteArry=*pchar;
		pchar++;
		byteArry++;
	}
}

static void time_set()
{
	rtc.Year = frame_data.data.timeset.use[0];
	rtc.Month = frame_data.data.timeset.use[1];
	rtc.Date = frame_data.data.timeset.use[2];
	rtc.Hour = frame_data.data.timeset.use[3];
	rtc.Min = frame_data.data.timeset.use[4];
	rtc.Sec = frame_data.data.timeset.use[5];
	rtc.DaysOfWeek = frame_data.data.timeset.use[6] + 1;
	DS3231_SetTime(&rtc);
}

static void send_alarmclock_list()
{
	
}

void frame_recv()
{
	uint16_t head;
	uint16_t tail;
	
	memcpy(&frame_data, uart_dma_recvbuf, DMA_BUFF_SIZE);
	head = (frame_data.head[0] << 8) | frame_data.head[1];
	tail = (frame_data.tail[0] << 8) | frame_data.tail[1];
	
	if(head != 0xffff || tail != 0x0d0a) return;
	memset(msg_buf, 0x00, sizeof(msg_buf));
//	UART_DMA_SEND(&frame_data, sizeof(frame_data_t));
	
	switch(frame_data.state)
	{
		case MASTER_CONN:
			sprintf(msg_buf, "Device connected");
			break;
		case MASTER_ALARM_GET:
			send_alarmclock_list();
			sprintf(msg_buf, "Read alarmclock list...");
			break;
		case MASTER_TIME_SET:
			time_set();
			sprintf(msg_buf, "Set time: \n%02d/%02d/%02d \n%02d:%02d:%02d \n%s", 
							rtc.Year, 
							rtc.Month,
							rtc.Date,
							rtc.Hour,
							rtc.Min,
							rtc.Sec,
							DS3231_weeks[rtc.DaysOfWeek-1]);
			break;
		case MASTER_ALARM_SET:
			//
			break;
		default:
			break;
	}
	
	comm_handler(msg_buf);
}

void comm_env_set(float temp, float humi, float press)
{
	
	FloatToByte(temp, frame_data.data.envinfo.use[0]);
	FloatToByte(humi, frame_data.data.envinfo.use[1]);
	FloatToByte(press, frame_data.data.envinfo.use[2]);
	
	comm_upload(SLAVE_ENV_UPLOAD);
}

void comm_upload(uint8_t state)
{
	frame_data.head[0] = 0xff;
	frame_data.head[1] = 0xff;
	frame_data.state = state;
	frame_data.tail[0] = 0x0d;
	frame_data.tail[1] = 0x0a;
	
	UART_DMA_SEND(&frame_data, sizeof(frame_data_t));
}

