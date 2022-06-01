#include "buzzer.h"
#include "device_config.h"

buzzer_cfg_t buzzer_cfg;

void buzzer_on_direct()
{
	HAL_TIM_PWM_Start(buzzer_cfg.tim,buzzer_cfg.tim_channel); //4kHZ
}

void buzzer_on()
{
	if(!g_config.hwatch_config.sound_state) return;
	buzzer_on_direct();
}

void buzzer_off()
{
	HAL_TIM_PWM_Stop(buzzer_cfg.tim,buzzer_cfg.tim_channel);
}
