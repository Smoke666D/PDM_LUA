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
#include "stm32f4xx_hal.h"

#include "stm32f4xx_ll_dma.h"
#include "stm32f4xx_ll_rcc.h"
#include "stm32f4xx_ll_bus.h"
#include "stm32f4xx_ll_system.h"
#include "stm32f4xx_ll_exti.h"
#include "stm32f4xx_ll_cortex.h"
#include "stm32f4xx_ll_utils.h"
#include "stm32f4xx_ll_pwr.h"
#include "stm32f4xx_ll_gpio.h"


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

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define Din12_Pin GPIO_PIN_2
#define Din12_GPIO_Port GPIOE
#define Din13_Pin GPIO_PIN_3
#define Din13_GPIO_Port GPIOE
#define Din14_Pin GPIO_PIN_4
#define Din14_GPIO_Port GPIOE
#define Din15_Pin GPIO_PIN_5
#define Din15_GPIO_Port GPIOE
#define Din16_Pin GPIO_PIN_6
#define Din16_GPIO_Port GPIOE
#define CS1_Pin GPIO_PIN_6
#define CS1_GPIO_Port GPIOF
#define CS2_Pin GPIO_PIN_7
#define CS2_GPIO_Port GPIOF
#define CS3_Pin GPIO_PIN_8
#define CS3_GPIO_Port GPIOF
#define CS4_Pin GPIO_PIN_9
#define CS4_GPIO_Port GPIOF
#define AIN6_Pin GPIO_PIN_0
#define AIN6_GPIO_Port GPIOC
#define AIN5_Pin GPIO_PIN_1
#define AIN5_GPIO_Port GPIOC
#define AIN4_Pin GPIO_PIN_2
#define AIN4_GPIO_Port GPIOC
#define AIN3_Pin GPIO_PIN_0
#define AIN3_GPIO_Port GPIOA
#define AIN2_Pin GPIO_PIN_1
#define AIN2_GPIO_Port GPIOA
#define AIN1_Pin GPIO_PIN_2
#define AIN1_GPIO_Port GPIOA
#define AIN12_Pin GPIO_PIN_3
#define AIN12_GPIO_Port GPIOA
#define AIN13_Pin GPIO_PIN_4
#define AIN13_GPIO_Port GPIOA
#define AIN11_Pin GPIO_PIN_5
#define AIN11_GPIO_Port GPIOA
#define AIN10_Pin GPIO_PIN_6
#define AIN10_GPIO_Port GPIOA
#define AIN9_Pin GPIO_PIN_7
#define AIN9_GPIO_Port GPIOA
#define AIN8_Pin GPIO_PIN_0
#define AIN8_GPIO_Port GPIOB
#define AIN7_Pin GPIO_PIN_1
#define AIN7_GPIO_Port GPIOB
#define Din17_Pin GPIO_PIN_11
#define Din17_GPIO_Port GPIOF
#define Din18_Pin GPIO_PIN_12
#define Din18_GPIO_Port GPIOF
#define Din19_Pin GPIO_PIN_13
#define Din19_GPIO_Port GPIOF
#define Din20_Pin GPIO_PIN_14
#define Din20_GPIO_Port GPIOF
#define CS_DIS3_Pin GPIO_PIN_1
#define CS_DIS3_GPIO_Port GPIOG
#define DR9_Pin GPIO_PIN_9
#define DR9_GPIO_Port GPIOE
#define DR10_Pin GPIO_PIN_11
#define DR10_GPIO_Port GPIOE
#define DR11_Pin GPIO_PIN_13
#define DR11_GPIO_Port GPIOE
#define DR12_Pin GPIO_PIN_14
#define DR12_GPIO_Port GPIOE
#define CS_DIS4_Pin GPIO_PIN_15
#define CS_DIS4_GPIO_Port GPIOE
#define DR13_Pin GPIO_PIN_10
#define DR13_GPIO_Port GPIOB
#define DR14_Pin GPIO_PIN_11
#define DR14_GPIO_Port GPIOB
#define DR15_Pin GPIO_PIN_14
#define DR15_GPIO_Port GPIOB
#define DR16_Pin GPIO_PIN_15
#define DR16_GPIO_Port GPIOB
#define CS_DIS5_Pin GPIO_PIN_10
#define CS_DIS5_GPIO_Port GPIOD
#define DR17_Pin GPIO_PIN_12
#define DR17_GPIO_Port GPIOD
#define DR18_Pin GPIO_PIN_13
#define DR18_GPIO_Port GPIOD
#define DR19_Pin GPIO_PIN_14
#define DR19_GPIO_Port GPIOD
#define DR20_Pin GPIO_PIN_15
#define DR20_GPIO_Port GPIOD
#define CS_DIS1_Pin GPIO_PIN_4
#define CS_DIS1_GPIO_Port GPIOG
#define USB_PULLUP_Pin GPIO_PIN_6
#define USB_PULLUP_GPIO_Port GPIOG
#define USB_VBAT_DET_Pin GPIO_PIN_7
#define USB_VBAT_DET_GPIO_Port GPIOG
#define DR4_Pin GPIO_PIN_6
#define DR4_GPIO_Port GPIOC
#define DR3_Pin GPIO_PIN_7
#define DR3_GPIO_Port GPIOC
#define DR2_Pin GPIO_PIN_8
#define DR2_GPIO_Port GPIOC
#define DR1_Pin GPIO_PIN_9
#define DR1_GPIO_Port GPIOC
#define DR5_Pin GPIO_PIN_15
#define DR5_GPIO_Port GPIOA
#define Din1_Pin GPIO_PIN_7
#define Din1_GPIO_Port GPIOD
#define Din2_Pin GPIO_PIN_9
#define Din2_GPIO_Port GPIOG
#define Din3_Pin GPIO_PIN_10
#define Din3_GPIO_Port GPIOG
#define Din4_Pin GPIO_PIN_11
#define Din4_GPIO_Port GPIOG
#define Din5_Pin GPIO_PIN_12
#define Din5_GPIO_Port GPIOG
#define Din6_Pin GPIO_PIN_13
#define Din6_GPIO_Port GPIOG
#define Din7_Pin GPIO_PIN_14
#define Din7_GPIO_Port GPIOG
#define Din8_Pin GPIO_PIN_15
#define Din8_GPIO_Port GPIOG
#define DR6_Pin GPIO_PIN_3
#define DR6_GPIO_Port GPIOB
#define DR7_Pin GPIO_PIN_4
#define DR7_GPIO_Port GPIOB
#define DR8_Pin GPIO_PIN_5
#define DR8_GPIO_Port GPIOB
#define CS_DIS2_Pin GPIO_PIN_7
#define CS_DIS2_GPIO_Port GPIOB
#define SPEED_IN_Pin GPIO_PIN_8
#define SPEED_IN_GPIO_Port GPIOB
#define Din9_Pin GPIO_PIN_9
#define Din9_GPIO_Port GPIOB
#define Din10_Pin GPIO_PIN_0
#define Din10_GPIO_Port GPIOE
#define Din11_Pin GPIO_PIN_1
#define Din11_GPIO_Port GPIOE

/* USER CODE BEGIN Private defines */
void WDT_Reset();
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
