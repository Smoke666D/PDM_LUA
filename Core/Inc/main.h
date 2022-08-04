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
#define Din3_Pin GPIO_PIN_2
#define Din3_GPIO_Port GPIOE
#define Din4_Pin GPIO_PIN_3
#define Din4_GPIO_Port GPIOE
#define Din5_Pin GPIO_PIN_4
#define Din5_GPIO_Port GPIOE
#define Din6_Pin GPIO_PIN_5
#define Din6_GPIO_Port GPIOE
#define Din7_Pin GPIO_PIN_6
#define Din7_GPIO_Port GPIOE
#define CS20_2_Pin GPIO_PIN_3
#define CS20_2_GPIO_Port GPIOF
#define CS20_1_Pin GPIO_PIN_4
#define CS20_1_GPIO_Port GPIOF
#define CS8_14_Pin GPIO_PIN_5
#define CS8_14_GPIO_Port GPIOF
#define CS8_13_Pin GPIO_PIN_6
#define CS8_13_GPIO_Port GPIOF
#define CS8_18_Pin GPIO_PIN_7
#define CS8_18_GPIO_Port GPIOF
#define CS8_17_Pin GPIO_PIN_8
#define CS8_17_GPIO_Port GPIOF
#define CS20_3_Pin GPIO_PIN_9
#define CS20_3_GPIO_Port GPIOF
#define CS8_15_Pin GPIO_PIN_10
#define CS8_15_GPIO_Port GPIOF
#define CS8_16_Pin GPIO_PIN_0
#define CS8_16_GPIO_Port GPIOC
#define CS20_4_Pin GPIO_PIN_1
#define CS20_4_GPIO_Port GPIOC
#define CS8_10_Pin GPIO_PIN_2
#define CS8_10_GPIO_Port GPIOC
#define CS8_9_Pin GPIO_PIN_3
#define CS8_9_GPIO_Port GPIOC
#define CS20_5_Pin GPIO_PIN_0
#define CS20_5_GPIO_Port GPIOA
#define CS20_6_Pin GPIO_PIN_1
#define CS20_6_GPIO_Port GPIOA
#define CS8_12_Pin GPIO_PIN_2
#define CS8_12_GPIO_Port GPIOA
#define CS8_11_Pin GPIO_PIN_3
#define CS8_11_GPIO_Port GPIOA
#define CS20_7_Pin GPIO_PIN_4
#define CS20_7_GPIO_Port GPIOA
#define CS8_20_Pin GPIO_PIN_5
#define CS8_20_GPIO_Port GPIOA
#define CS8_19_Pin GPIO_PIN_6
#define CS8_19_GPIO_Port GPIOA
#define CS20_8_Pin GPIO_PIN_7
#define CS20_8_GPIO_Port GPIOA
#define ADC1_4_Pin GPIO_PIN_4
#define ADC1_4_GPIO_Port GPIOC
#define ADC1_5_Pin GPIO_PIN_5
#define ADC1_5_GPIO_Port GPIOC
#define ADC1_8_Pin GPIO_PIN_0
#define ADC1_8_GPIO_Port GPIOB
#define ADC1_9_Pin GPIO_PIN_1
#define ADC1_9_GPIO_Port GPIOB
#define Din10_Pin GPIO_PIN_13
#define Din10_GPIO_Port GPIOF
#define Din11_Pin GPIO_PIN_14
#define Din11_GPIO_Port GPIOF
#define Din12_Pin GPIO_PIN_15
#define Din12_GPIO_Port GPIOF
#define Cs_Dis20_5_Pin GPIO_PIN_1
#define Cs_Dis20_5_GPIO_Port GPIOG
#define Cs_Dis20_6_Pin GPIO_PIN_7
#define Cs_Dis20_6_GPIO_Port GPIOE
#define Cs_Dis8_9_10_Pin GPIO_PIN_8
#define Cs_Dis8_9_10_GPIO_Port GPIOE
#define InCH8_9_Pin GPIO_PIN_9
#define InCH8_9_GPIO_Port GPIOE
#define InCH8_10_Pin GPIO_PIN_11
#define InCH8_10_GPIO_Port GPIOE
#define InCH20_5_Pin GPIO_PIN_13
#define InCH20_5_GPIO_Port GPIOE
#define InCH20_6_Pin GPIO_PIN_14
#define InCH20_6_GPIO_Port GPIOE
#define InCH8_12_Pin GPIO_PIN_10
#define InCH8_12_GPIO_Port GPIOB
#define InCH8_11_Pin GPIO_PIN_11
#define InCH8_11_GPIO_Port GPIOB
#define InCH20_7_Pin GPIO_PIN_14
#define InCH20_7_GPIO_Port GPIOB
#define InCH8_19_Pin GPIO_PIN_15
#define InCH8_19_GPIO_Port GPIOB
#define Cs_Dis8_11_12_Pin GPIO_PIN_8
#define Cs_Dis8_11_12_GPIO_Port GPIOD
#define Cs_Dis20_7_Pin GPIO_PIN_9
#define Cs_Dis20_7_GPIO_Port GPIOD
#define Cs_Dis8_19_20_Pin GPIO_PIN_10
#define Cs_Dis8_19_20_GPIO_Port GPIOD
#define Cs_Dis20_8_Pin GPIO_PIN_11
#define Cs_Dis20_8_GPIO_Port GPIOD
#define InCH8_20_Pin GPIO_PIN_12
#define InCH8_20_GPIO_Port GPIOD
#define InCH20_8_Pin GPIO_PIN_13
#define InCH20_8_GPIO_Port GPIOD
#define InCH20_1_Pin GPIO_PIN_14
#define InCH20_1_GPIO_Port GPIOD
#define InCH20_2_Pin GPIO_PIN_15
#define InCH20_2_GPIO_Port GPIOD
#define Cs_Dis20_2_Pin GPIO_PIN_2
#define Cs_Dis20_2_GPIO_Port GPIOG
#define Cs_Dis20_1_Pin GPIO_PIN_3
#define Cs_Dis20_1_GPIO_Port GPIOG
#define Cs_Dis8_13_14_Pin GPIO_PIN_4
#define Cs_Dis8_13_14_GPIO_Port GPIOG
#define Cs_Dis8_17_18_Pin GPIO_PIN_5
#define Cs_Dis8_17_18_GPIO_Port GPIOG
#define USB_PULLUP_Pin GPIO_PIN_6
#define USB_PULLUP_GPIO_Port GPIOG
#define USB_VBAT_DET_Pin GPIO_PIN_7
#define USB_VBAT_DET_GPIO_Port GPIOG
#define InCH8_13_Pin GPIO_PIN_6
#define InCH8_13_GPIO_Port GPIOC
#define InCH8_14_Pin GPIO_PIN_7
#define InCH8_14_GPIO_Port GPIOC
#define InCH8_17_Pin GPIO_PIN_8
#define InCH8_17_GPIO_Port GPIOC
#define InCH8_18_Pin GPIO_PIN_9
#define InCH8_18_GPIO_Port GPIOC
#define Cs_Dis8_15_16_Pin GPIO_PIN_10
#define Cs_Dis8_15_16_GPIO_Port GPIOG
#define Cs_Dis20_3_Pin GPIO_PIN_11
#define Cs_Dis20_3_GPIO_Port GPIOG
#define Cs_Dis20_4_Pin GPIO_PIN_12
#define Cs_Dis20_4_GPIO_Port GPIOG
#define InCH8_16_Pin GPIO_PIN_3
#define InCH8_16_GPIO_Port GPIOB
#define InCH8_15_Pin GPIO_PIN_4
#define InCH8_15_GPIO_Port GPIOB
#define InCH20_4_Pin GPIO_PIN_5
#define InCH20_4_GPIO_Port GPIOB
#define Din9_Pin GPIO_PIN_8
#define Din9_GPIO_Port GPIOB
#define Din8_Pin GPIO_PIN_9
#define Din8_GPIO_Port GPIOB
#define Din1_Pin GPIO_PIN_0
#define Din1_GPIO_Port GPIOE
#define Din2_Pin GPIO_PIN_1
#define Din2_GPIO_Port GPIOE
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
