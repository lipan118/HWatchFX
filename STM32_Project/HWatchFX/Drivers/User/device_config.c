#include "device_config.h"

//w25q16
//总大小：16M-bit = 2M = (2 * 1024^2)字节
//按扇区擦除，每个扇区16页，每页256字节，每个扇区总大小 256*16 = 4096
//程序使用前8K空间，共2个扇区32页，循环切换页索引进行配置存储，避免重复擦除同一扇区
//正向写入配置，反向查找配置索引

//全局系统配置
G_config g_config;
static u16 g_config_index;	//系统配置存储索引

union config_id{
	u8 id_u8[4];
	u32 id_u32;
};

union config_id config_board_id; //设备ID

//默认配置
static void DeviceConfig_Default(void);
static void DeviceConfig_Init(void);
static int read_write_check(u32 addr);
static void config_clear(void);

void HWatchDevice_Init()
{
	//初始化并读取配置
	DeviceConfig_Init();
	
	//屏幕默认上电点亮,无关配置存储
	g_config.hwatch_config.screen_state = STATE_ON;
}

/**
* 写入配置到Flash
*/
void HWatchDevice_ConfigSave()
{
	u32 addr;
	u8 is_first_page = 0; //是否为首页
	
	if(g_config_index >= 31)
	{
		is_first_page = 1; //达到64K设定页，重置为0
	}
	
	//正向写入,索引递增
	if(is_first_page == 1)
	{
		//擦除64K全部扇区，索引清零
		g_config_index = 0;
		config_clear();
	}
	else
		g_config_index++;
	
	addr = g_config_index * CONFIG_MAX_SIZE; //计算写入地址
	
	printf("HWatch config write begin, page, addr: %d, %d\n", g_config_index, addr);
	W25QXX_Write(g_config.config, addr, sizeof(HWatchConfig)); //一次性写入全部数据
	
	//检测是否写入成功
	if(read_write_check(addr) == 0)
	{
		printf("HWatch config write succeeded\n");
	}else{
		printf("HWatch config write failed\n");
	}
	
}

//读取Flash配置并初始化
static void DeviceConfig_Init()
{
	u16 page_index; //使用前64K，页索引范围(0 ~ 255)
	u32 addr;
	u32 secpos;
	u8 admin_flag = 0;
	
	if(sizeof(HWatchConfig) > CONFIG_MAX_SIZE)
	{
		printf("HWatch config size over!\n");
		while(1);
	}
	
	//反向查找
	for(page_index=0 ;page_index<32;page_index++)
	{
		u16 index = (31 - page_index);
		addr =  index * CONFIG_MAX_SIZE; //计算读取地址
		W25QXX_Read(config_board_id.id_u8, addr, sizeof(u32)); //读取前四个字节设备ID
		
		if(config_board_id.id_u32 == DEVICE_BOARD_CONFIG_ID)
		{
			//找到设备ID，记录当前配置存储页索引
			g_config_index = index;
			
#if CONFIG_CLEAR_ADMIN
			printf("HWatch clear config by admin\n");
			secpos = g_config_index / 4096;
			W25QXX_Erase_Sector(secpos);		//擦除这个扇区
			admin_flag = 1;
#endif
			
			break;
		}

	}
	
	//未找到存储配置，使用默认配置
	if(page_index > 31 || admin_flag)
	{
		g_config_index = 0;
		config_clear();
		
		printf("HWatch load default config\n");
		DeviceConfig_Default();
		addr = g_config_index * CONFIG_MAX_SIZE;
		
		//从地址0写入配置
		printf("HWatch config write begin, page, addr: 0, 0\n");
		W25QXX_Write(g_config.config, addr, sizeof(HWatchConfig));
		
		//验证写入
		if(read_write_check(addr) == 0)
		{
			printf("HWatch default config write succeeded\n");
		}else{
			printf("HWatch default config write failed\n");
		}
	}else
	{
		//找到配置,加载配置
		printf("HWatch config found at w25q16 page, addr: %d, %d\n", g_config_index, addr);
		printf("HWatch device id: %x\n", config_board_id.id_u32);
		
		//addr = g_config_index * CONFIG_MAX_SIZE;	
		W25QXX_Read(g_config.config, addr, sizeof(HWatchConfig));
		
		if(g_config.hwatch_config.bord_id == DEVICE_BOARD_CONFIG_ID)
		{
			printf("Hwatch config load succeeded\n");
		}else{
			printf("Hwatch config load failed, load default config\n");
			DeviceConfig_Default();
		}
	}
}

void HWatchDevice_Printf()
{
	printf("\n**********HWatchFX Config**********\n");
	printf("config size: %d\n", sizeof(g_config));
	printf("config_app_name: %s\n", g_config.hwatch_config.app_name);
	printf("config_app_ver: %s\n", g_config.hwatch_config.app_ver);
	printf("config_device_id: %x\n", g_config.hwatch_config.bord_id);
	printf("config_blight_level: %d\n", g_config.hwatch_config.blight_level);
	printf("config_sound_state: %d\n", g_config.hwatch_config.sound_state);
	printf("config_alarm_state: %d\n", g_config.hwatch_config.alarm_state);
	printf("config_screen_state: %d\n", g_config.hwatch_config.screen_state);
	printf("config_screen_locktime: %d\n", g_config.hwatch_config.home_screen_lock_t);

	for(int i=0;i<10;i++)
	{
		printf("config_alarm_clock %d: %06d\n", i, g_config.hwatch_config.alarm_clock[i]);
	}
	
	printf("**********HWatchFX Config**********\n\n");
}

//默认配置
static void DeviceConfig_Default()
{
	g_config.hwatch_config.bord_id = DEVICE_BOARD_CONFIG_ID;
	g_config.hwatch_config.blight_level = 6;
	g_config.hwatch_config.sound_state = STATE_OFF;
	g_config.hwatch_config.alarm_state = STATE_OFF;
	g_config.hwatch_config.home_screen_lock_t = 0;
	g_config.hwatch_config.screen_state = STATE_ON;
	g_config.hwatch_config.home_screen_lock_t = 60;
	
	sprintf(g_config.hwatch_config.app_name, "%s", APP_NAME);
	sprintf(g_config.hwatch_config.app_ver, "%s", APP_VER);
	
	for(int i=0;i<ALARM_CLOCK_CNT_MAX;i++)
	{
		g_config.hwatch_config.alarm_clock[i] = 0;
	}
}

static int read_write_check(u32 addr)
{
	W25QXX_Read(config_board_id.id_u8, addr, sizeof(u32));
	if(config_board_id.id_u32 == DEVICE_BOARD_CONFIG_ID)
	{
		return 0;
	}else
	{
		return -1;
	}
	
	return -1;
}

static void config_clear()
{
	u32 secpos;
	printf("HWatch erase sectors begin\n");
	for(secpos = 0; secpos < 2; secpos++)
	{
		W25QXX_Erase_Sector(secpos);
	}
	printf("HWatch erase sectors end\n");
}