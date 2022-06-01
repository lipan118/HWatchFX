#ifndef _KEY_H
#define _KEY_H

#include "gpio.h"

#define KEY_UP_VAL !HAL_GPIO_ReadPin(KEY_UP_PIN_GPIO_Port, KEY_UP_PIN_Pin)
#define KEY_RIGHT_VAL !HAL_GPIO_ReadPin(KEY_RIGHT_PIN_GPIO_Port, KEY_RIGHT_PIN_Pin)
#define KEY_LEFT_VAL !HAL_GPIO_ReadPin(KEY_LEFT_PIN_GPIO_Port, KEY_LEFT_PIN_Pin)
#define KEY_DOWN_VAL !HAL_GPIO_ReadPin(KEY_DOWN_PIN_GPIO_Port, KEY_DOWN_PIN_Pin)
#define KEY_A_VAL HAL_GPIO_ReadPin(KEY_A_PIN_GPIO_Port, KEY_A_PIN_Pin)
#define KEY_B_VAL !HAL_GPIO_ReadPin(KEY_B_PIN_GPIO_Port, KEY_B_PIN_Pin)

#define KEY_UP_DEF 1
#define KEY_DOWN_DEF 2
#define KEY_LEFT_DEF 3
#define KEY_RIGHT_DEF 4
#define KEY_A_DEF 5 //ENTER
#define KEY_B_DEF 6 //BACK

uint8_t key_read(uint8_t mode);

#endif