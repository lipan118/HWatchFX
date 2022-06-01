/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define KEY_UP_PIN_Pin GPIO_PIN_13
#define KEY_UP_PIN_GPIO_Port GPIOC
#define KEY_B_PIN_Pin GPIO_PIN_2
#define KEY_B_PIN_GPIO_Port GPIOC
#define KEY_A_PIN_Pin GPIO_PIN_0
#define KEY_A_PIN_GPIO_Port GPIOA
#define W25QX_CS_Pin GPIO_PIN_1
#define W25QX_CS_GPIO_Port GPIOA
#define LCD_CS_PIN_Pin GPIO_PIN_4
#define LCD_CS_PIN_GPIO_Port GPIOA
#define LCD_DC_PIN_Pin GPIO_PIN_6
#define LCD_DC_PIN_GPIO_Port GPIOC
#define LCD_RST_PIN_Pin GPIO_PIN_8
#define LCD_RST_PIN_GPIO_Port GPIOC
#define KEY_DOWN_PIN_Pin GPIO_PIN_10
#define KEY_DOWN_PIN_GPIO_Port GPIOC
#define KEY_LEFT_PIN_Pin GPIO_PIN_11
#define KEY_LEFT_PIN_GPIO_Port GPIOC
#define KEY_RIGHT_PIN_Pin GPIO_PIN_12
#define KEY_RIGHT_PIN_GPIO_Port GPIOC
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
