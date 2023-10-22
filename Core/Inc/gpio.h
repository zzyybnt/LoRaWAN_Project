/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    gpio.h
  * @brief   This file contains all the function prototypes for
  *          the gpio.c file
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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
#ifndef __GPIO_H__
#define __GPIO_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* USER CODE BEGIN Private defines */

//-------------Key1\Key2 Macro definition-----------------------------------------
#define Gpio_KEY1   GPIOC
#define Gpio_KEY2   GPIOE
#define Pin_KEY1    GPIO_PIN_13
#define Pin_KEY2    GPIO_PIN_6
#define GET_KEY1_LEVEL		    HAL_GPIO_ReadPin(Gpio_KEY1,Pin_KEY1)
#define GET_KEY2_LEVEL		    HAL_GPIO_ReadPin(Gpio_KEY2,Pin_KEY2)

//----------LED OF WAKE\MODE\BUSY\STAT\RST Macro definition-------------------------
#define LedGpio_D6		GPIOD
#define LedGpio_D7    	GPIOA
#define LedGpio_D8		GPIOB
#define LedGpio_D11		GPIOE
#define LedPin_D6  		GPIO_PIN_7	 
#define LedPin_D7  		GPIO_PIN_10	 
#define LedPin_D8 		GPIO_PIN_1
#define LedPin_D11 		GPIO_PIN_0

#define GET_LED_WAKE_LEVEL		HAL_GPIO_ReadPin(LedGpio_D6, LedPin_D6)
#define GET_LED_MODE_LEVEL		HAL_GPIO_ReadPin(LedGpio_D7,LedPin_D7)
#define GET_LED_BUSY_LEVEL		HAL_GPIO_ReadPin(LedGpio_D8,LedPin_D8)
#define GET_LED_STAT_LEVEL		HAL_GPIO_ReadPin(LedGpio_D11,LedPin_D11)

#define LEDWAKE_ON   HAL_GPIO_WritePin(LedGpio_D6, LedPin_D6, GPIO_PIN_RESET)
#define LEDWAKE_OFF  HAL_GPIO_WritePin(LedGpio_D6, LedPin_D6, GPIO_PIN_SET)

#define LEDMODE_ON   HAL_GPIO_WritePin(LedGpio_D7, LedPin_D7, GPIO_PIN_RESET)
#define LEDMODE_OFF  HAL_GPIO_WritePin(LedGpio_D7, LedPin_D7, GPIO_PIN_SET)

#define LEDBUSY_ON   HAL_GPIO_WritePin(LedGpio_D8, LedPin_D8, GPIO_PIN_RESET)
#define LEDBUSY_OFF  HAL_GPIO_WritePin(LedGpio_D8, LedPin_D8, GPIO_PIN_SET)

#define LEDSTAT_ON   HAL_GPIO_WritePin(LedGpio_D11, LedPin_D11, GPIO_PIN_RESET)
#define LEDSTAT_OFF  HAL_GPIO_WritePin(LedGpio_D11, LedPin_D11, GPIO_PIN_SET)

#define LEDALL_ON    

#define LEDALL_OFF   

//----------WAKE\MODE\BUSY\STAT\RST Macro definition-------------------------
#define Gpio_WAKE    GPIOC
#define Gpio_MODE    GPIOA
#define Gpio_BUSY    GPIOC
#define Gpio_STAT    GPIOC
#define Gpio_RST     GPIOA
#define Pin_WAKE     GPIO_PIN_3
#define Pin_MODE     GPIO_PIN_0
#define Pin_BUSY     GPIO_PIN_5
#define Pin_STAT     GPIO_PIN_4
#define Pin_RST      GPIO_PIN_1
		
/** 读取连接模块WAKE引脚的电平 */
#define GET_WAKE_LEVEL		HAL_GPIO_ReadPin(Gpio_WAKE, Pin_WAKE)

/** 读取连接模块MODE引脚的电平 */
#define GET_MODE_LEVEL		HAL_GPIO_ReadPin(Gpio_MODE, Pin_MODE)

/** 读取连接模块BUSY引脚的电平 */
#define GET_BUSY_LEVEL		HAL_GPIO_ReadPin(Gpio_BUSY, Pin_BUSY)

/** 读取连接模块STAT引脚的电平 */
#define GET_STAT_LEVEL		HAL_GPIO_ReadPin(Gpio_STAT, Pin_STAT)

/** 设置连接模块WAKE引脚的为高电平 */
#define SET_WAKE_HIGH		HAL_GPIO_WritePin(Gpio_WAKE, Pin_WAKE, GPIO_PIN_SET)

/** 设置连接模块WAKE引脚的为低电平 */
#define SET_WAKE_LOW		HAL_GPIO_WritePin(Gpio_WAKE, Pin_WAKE, GPIO_PIN_RESET)

/** 设置连接模块MODE引脚的为高电平 */
#define SET_MODE_HIGH		HAL_GPIO_WritePin(Gpio_MODE, Pin_MODE, GPIO_PIN_SET)

/** 设置连接模块MODE引脚的为低电平 */
#define SET_MODE_LOW		HAL_GPIO_WritePin(Gpio_MODE, Pin_MODE, GPIO_PIN_RESET)

/** 设置连接模块RESET引脚的为高电平 */
#define SET_RESET_HIGH		HAL_GPIO_WritePin(Gpio_RST, Pin_RST, GPIO_PIN_SET)

/** 设置连接模块RESET引脚的为低电平 */
#define SET_RESET_LOW		HAL_GPIO_WritePin(Gpio_RST, Pin_RST, GPIO_PIN_RESET)

void gpio_into_sleep(void);
void gpio_into_wakeup(void);

/* USER CODE END Private defines */

void MX_GPIO_Init(void);

/* USER CODE BEGIN Prototypes */

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif
#endif /*__ GPIO_H__ */

