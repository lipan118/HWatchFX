/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "dma.h"
#include "i2c.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
#include "devices.h"
#include "gui_port.h"

#include "arduboy.h"
#include "SnakePort.h"
#include "FlappyBall.h"
#include "ArduboyButtons.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define ARDUBOY_GAME 0  //修改Stack_Size==>0x3000
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_NVIC_Init(void);
/* USER CODE BEGIN PFP */
static void HWatchFX_Logo(void);
static void arduboy_update(const unsigned char *image);
static void arduboy_delay(int ms);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
	
//  __HAL_RCC_PWR_CLK_ENABLE();//使能PWR时钟
//  if (__HAL_PWR_GET_FLAG(PWR_FLAG_SB) != RESET)
//  {
//    /* Clear Standby flag */
//    __HAL_PWR_CLEAR_FLAG(PWR_FLAG_SB);
//  }
//	HAL_Delay(1000);
//	HAL_PWR_DisableWakeUpPin(PWR_WAKEUP_PIN1);
//	__HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);
//	HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN1);
//	HAL_PWR_EnterSTANDBYMode();//进入待机模式

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_USART1_UART_Init();
  MX_ADC1_Init();
  MX_SPI1_Init();
  MX_SPI2_Init();
  MX_I2C1_Init();
  MX_I2C2_Init();
  MX_TIM2_Init();
  MX_TIM4_Init();
  MX_TIM6_Init();
  MX_TIM7_Init();

  /* Initialize interrupts */
  MX_NVIC_Init();
  /* USER CODE BEGIN 2 */
	//====================Init Device====================
	HAL_UART_Receive_DMA(&huart1, uart_dma_recvbuf, DMA_BUFF_SIZE);
//	while(1){}
	device_init();
	//====================Init Arduboy====================
#if ARDUBOY_GAME
	arm_arduboy.delayHandler = arduboy_delay;
	arm_arduboy.paintHandler = arduboy_update;
	arm_arduboy.begin();
	arm_arduboy.setFrameRate(15);
	arm_arduboy.bootLogo();
	
	flappyball_init();
//	arduboy_btn_init();
//	snake_init();
#else
	//====================Init LVGL GUI====================
	gui_port_init();
	HAL_TIM_Base_Start_IT(&htim6);
#endif

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	
	int count = 0;
	printf("System start!\n");
	UART_DMA_SEND("dma send", 9);
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
		//Watch GUI
#if ARDUBOY_GAME
		flappyball_loop();
//		arduboy_btn_loop();
//		snake_loop();
#else
		gui_port_task();
#endif
		
		CPU_TS_Tmr_Delay_MS(1);
		
		if(count == 1000)
		{
			low_bat_detect();
			gui_port_memlog();
			count = 0;
		}
		count++;
		
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }

  /** Enables the Clock Security System
  */
  HAL_RCC_EnableCSS();
}

/**
  * @brief NVIC Configuration.
  * @retval None
  */
static void MX_NVIC_Init(void)
{
  /* TIM6_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(TIM6_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(TIM6_IRQn);
}

/* USER CODE BEGIN 4 */

void timer6_handler(void)
{
	gui_port_tick();
}

void timer7_handler(void)
{
	gui_port_systick();  //1MS
}

#if ARDUBOY_GAME
uint16_t img[128*64];
static void arduboy_update(const unsigned char *image)
{
	uint16_t len = arm_arduboy.width() * arm_arduboy.height() / 8;
	uint8_t row_index = 0;
	memset(img, 0x00, sizeof(uint16_t));
	
	for(int i=0;i<len;i+=arm_arduboy.width())
	{
		for(int col=0;col<arm_arduboy.width();col++)
		{
			uint16_t index = (i + col);
			uint8_t tmp = *(image + index);
			uint8_t bit = 0;
			for(int row=0;row<8;row++)
			{
				bit = (tmp & (1 << row)) >> row;
				index = (row_index + row) * arm_arduboy.width() + col + row + row_index;
				img[index] = bit == 0 ? FLIPW(BLACK) : FLIPW(GREEN);
			}
			
		}
		row_index+=8;
	}
	LCD_1IN28_DisplayWindows(56,64,184,128,img);
}

static void arduboy_delay(int ms)
{
	CPU_TS_Tmr_Delay_MS(ms);
}

#endif

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
